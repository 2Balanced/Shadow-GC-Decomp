/* Dolphin SDK EXI: the channel driver. Each of the three channels has five registers (status and
 * chip select, DMA address, DMA length, control, immediate data) and three interrupt sources:
 * EXI (raised by the selected device), TC (transfer complete) and EXT (device removed).
 *
 * A channel is shared by up to three devices, so callers take turns with EXILock; one that is
 * refused may leave a callback that runs when the channel is released. EXIGetID reads the 32-bit
 * device ID of a device and remembers it for the two card slots until the device is removed. */
#include <dolphin/exi.h>
#include <dolphin/os_interrupt.h>

/* The whole file was built without instruction scheduling: every function keeps its
 * instructions in source order (prologue saves first, then the argument copies), unlike the rest
 * of the library. */
#pragma scheduling off

#define EXI_MAX_CHAN 3
#define EXI_MAX_DEV 3

/* Per-channel registers. */
#define EXI_REG_CPR 0     /* status, interrupt masks, chip select and clock */
#define EXI_REG_MAR 1     /* DMA address */
#define EXI_REG_LENGTH 2  /* DMA length */
#define EXI_REG_CR 3      /* transfer control */
#define EXI_REG_DATA 4    /* immediate data */
#define EXI_REG_COUNT 5

/* EXI_REG_CPR */
#define CPR_EXI_MASK 0x0001
#define CPR_EXI_INT 0x0002
#define CPR_TC_MASK 0x0004
#define CPR_TC_INT 0x0008
#define CPR_CLOCK 0x0070
#define CPR_CS0 0x0080
#define CPR_EXT_MASK 0x0400
#define CPR_EXT_INT 0x0800
#define CPR_EXT_PRESENT 0x1000
#define CPR_ROM_DISABLE 0x2000
/* Everything except the three "interrupt pending" bits, which are cleared by writing 1. */
#define CPR_KEEP 0x07F5
/* What stays when the chip selects and the clock are dropped. */
#define CPR_MASKS (CPR_EXI_MASK | CPR_TC_MASK | CPR_EXT_MASK)

/* EXI_REG_CR */
#define CR_START 0x01
#define CR_DMA 0x02

/* Interrupt mask bits (interrupt n is bit 0x80000000 >> n; a channel's three follow each other). */
#define IRQ_EXI_0_EXI 9
#define IRQ_EXI_0_TC 10
#define IRQ_EXI_0_EXT 11
#define IRQ_EXI_1_EXI 12
#define IRQ_EXI_1_TC 13
#define IRQ_EXI_1_EXT 14
#define IRQ_EXI_2_EXI 15
#define IRQ_EXI_2_TC 16
#define IRQ_PI_DEBUG 25
#define IRQ_BIT(n) (0x80000000u >> (n))
#define IRQS_PER_CHAN 3

#define DI_CONFIG_NONE 0xFF
#define CONSOLE_TYPE_MASK 0xF0000000
#define CONSOLE_TYPE_TDEV 0x20000000
#define DEVICE_CODE_HDD 0x8200

/* A device has to stay in its slot this long (in 100 ms units) before it is believed. */
#define INSERT_SETTLE_TIME 3

typedef struct EXIControl {
    EXICallback exiCallback;
    EXICallback tcCallback;
    EXICallback extCallback;
    volatile u32 state;
    s32 immLen;   /* bytes still to be fetched from the data register */
    u8 *immBuf;
    u32 dev;      /* device the channel is locked for */
    u32 id;       /* cached device ID of device 0 */
    s32 idTime;   /* insertion time the cached ID belongs to; 0: none */
    s32 items;
    struct {
        u32 dev;
        EXICallback callback;
    } queue[EXI_MAX_DEV];
} EXIControl;

volatile u32 __EXIRegs[EXI_MAX_CHAN][EXI_REG_COUNT] : 0xCC006800;
#define REG(chan, idx) (__EXIRegs[chan][idx])

u32 __OSBusClock : 0x800000F8;
u16 __OSDeviceCode : 0x800030E6;
/* When a device was first seen in each slot (in 100 ms units, never 0); 0 while the slot is
 * empty. Kept in low memory so that it survives into the next program. */
s32 __EXIInsertTime[2] : 0x800030C0;

#define OS_TIMER_CLOCK (__OSBusClock / 4)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000))

extern BOOL __OSInIPL;
u32 OSGetConsoleType(void);
u32 __OSGetDIConfig(void);
void *memmove(void *dst, const void *src, unsigned long n);

const char *__EXIVersion = "<< Dolphin SDK - EXI\trelease build: Apr  5 2004 04:14:14 (0x2301) >>";

static EXIControl Ecb[EXI_MAX_CHAN];
static u32 IDSerialPort1;

static BOOL __EXIProbe(s32 chan);

/* The EXI interrupt of a channel is open while somebody wants it and the channel is free. Channel
 * 0 also serves channel 2's callback, and channel 2 itself reports through the debug interrupt. */
static void SetExiInterruptMask(s32 chan, EXIControl *exi) {
    EXIControl *exi2 = &Ecb[2];

    switch (chan) {
    case 0:
        if ((exi->exiCallback == NULL && exi2->exiCallback == NULL) || (exi->state & EXI_STATE_LOCKED)) {
            __OSMaskInterrupts(IRQ_BIT(IRQ_EXI_0_EXI) | IRQ_BIT(IRQ_EXI_2_EXI));
        } else {
            __OSUnmaskInterrupts(IRQ_BIT(IRQ_EXI_0_EXI) | IRQ_BIT(IRQ_EXI_2_EXI));
        }
        break;
    case 1:
        if (exi->exiCallback == NULL || (exi->state & EXI_STATE_LOCKED)) {
            __OSMaskInterrupts(IRQ_BIT(IRQ_EXI_1_EXI));
        } else {
            __OSUnmaskInterrupts(IRQ_BIT(IRQ_EXI_1_EXI));
        }
        break;
    case 2:
        if (__OSGetInterruptHandler(IRQ_PI_DEBUG) == NULL || (exi->state & EXI_STATE_LOCKED)) {
            __OSMaskInterrupts(IRQ_BIT(IRQ_PI_DEBUG));
        } else {
            __OSUnmaskInterrupts(IRQ_BIT(IRQ_PI_DEBUG));
        }
        break;
    }
}

/* Ends an immediate or DMA transfer: an immediate read is unpacked from the data register, most
 * significant byte first. */
static inline void CompleteTransfer(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    u8 *buf;
    u32 data;
    s32 i;
    s32 len;

    if (exi->state & EXI_STATE_BUSY) {
        if ((exi->state & EXI_STATE_IMM) && (len = exi->immLen) != 0) {
            buf = exi->immBuf;
            data = REG(chan, EXI_REG_DATA);
            for (i = 0; i < len; i++) {
                *buf++ = (u8)(data >> ((3 - i) * 8));
            }
        }
        exi->state &= ~EXI_STATE_BUSY;
    }
}

BOOL EXIImm(s32 chan, void *buf, s32 len, u32 type, EXICallback callback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if ((exi->state & EXI_STATE_BUSY) || !(exi->state & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->tcCallback = callback;
    if (exi->tcCallback) {
        EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
        __OSUnmaskInterrupts(IRQ_BIT(IRQ_EXI_0_TC) >> (IRQS_PER_CHAN * chan));
    }
    exi->state |= EXI_STATE_IMM;
    if (type != EXI_READ) {
        u32 data = 0;
        int i;

        for (i = 0; i < len; i++) {
            data |= ((u8 *)buf)[i] << ((3 - i) * 8);
        }
        REG(chan, EXI_REG_DATA) = data;
    }
    exi->immBuf = buf;
    exi->immLen = (type != EXI_WRITE) ? len : 0;
    REG(chan, EXI_REG_CR) = CR_START | (type << 2) | ((len - 1) << 4);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* A transfer of any length, four bytes at a time, each one waited for. */
BOOL EXIImmEx(s32 chan, void *buf, s32 len, u32 mode) {
    s32 chunk;

    while (len) {
        chunk = (len < 4) ? len : 4;
        if (!EXIImm(chan, buf, chunk, mode, NULL)) {
            return FALSE;
        }
        if (!EXISync(chan)) {
            return FALSE;
        }
        buf = (u8 *)buf + chunk;
        len -= chunk;
    }
    return TRUE;
}

BOOL EXIDma(s32 chan, void *buf, s32 len, u32 type, EXICallback callback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if ((exi->state & EXI_STATE_BUSY) || !(exi->state & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->tcCallback = callback;
    if (exi->tcCallback) {
        EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
        __OSUnmaskInterrupts(IRQ_BIT(IRQ_EXI_0_TC) >> (IRQS_PER_CHAN * chan));
    }
    exi->state |= EXI_STATE_DMA;
    REG(chan, EXI_REG_MAR) = (u32)buf & 0x03FFFFE0;
    REG(chan, EXI_REG_LENGTH) = (u32)len;
    REG(chan, EXI_REG_CR) = CR_START | CR_DMA | (type << 2);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* Waits for the transfer in progress. The last test refuses a four-byte transfer whose data
 * register reads back as one of three debugging-adapter IDs on a retail console without a
 * hard-disk device code. */
BOOL EXISync(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL rc = FALSE;
    BOOL enabled;

    while (exi->state & EXI_STATE_SELECTED) {
        if (!(REG(chan, EXI_REG_CR) & CR_START)) {
            enabled = OSDisableInterrupts();
            if (exi->state & EXI_STATE_SELECTED) {
                CompleteTransfer(chan);
                if (__OSGetDIConfig() != DI_CONFIG_NONE ||
                    (OSGetConsoleType() & CONSOLE_TYPE_MASK) == CONSOLE_TYPE_TDEV || exi->immLen != 4 ||
                    (REG(chan, EXI_REG_CPR) & CPR_CLOCK) != 0 ||
                    (REG(chan, EXI_REG_DATA) != EXI_ID_USB_ADAPTER && REG(chan, EXI_REG_DATA) != EXI_ID_IS_VIEWER &&
                     REG(chan, EXI_REG_DATA) != EXI_ID_ARTIST_ETHER) ||
                    __OSDeviceCode == DEVICE_CODE_HDD) {
                    rc = TRUE;
                }
            }
            OSRestoreInterrupts(enabled);
            break;
        }
    }
    return rc;
}

/* Acknowledges the chosen interrupts of a channel; returns the status register as it was. */
u32 EXIClearInterrupts(s32 chan, BOOL exi, BOOL tc, BOOL ext) {
    u32 cpr;
    u32 prev;

    prev = cpr = REG(chan, EXI_REG_CPR);
    cpr &= CPR_KEEP;
    if (exi) {
        cpr |= CPR_EXI_INT;
    }
    if (tc) {
        cpr |= CPR_TC_INT;
    }
    if (ext) {
        cpr |= CPR_EXT_INT;
    }
    REG(chan, EXI_REG_CPR) = cpr;
    return prev;
}

EXICallback EXISetExiCallback(s32 chan, EXICallback exiCallback) {
    EXIControl *exi = &Ecb[chan];
    EXICallback prev;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    prev = exi->exiCallback;
    exi->exiCallback = exiCallback;
    if (chan != 2) {
        SetExiInterruptMask(chan, exi);
    } else {
        SetExiInterruptMask(0, &Ecb[0]);
    }
    OSRestoreInterrupts(enabled);
    return prev;
}

/* Is there a device in the slot, and has it been there long enough? */
static BOOL __EXIProbe(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;
    BOOL rc;
    u32 cpr;
    s32 t;

    if (chan == 2) {
        return TRUE;
    }
    rc = TRUE;
    enabled = OSDisableInterrupts();
    cpr = REG(chan, EXI_REG_CPR);
    if (!(exi->state & EXI_STATE_ATTACHED)) {
        if (cpr & CPR_EXT_INT) {
            EXIClearInterrupts(chan, FALSE, FALSE, TRUE);
            __EXIInsertTime[chan] = exi->idTime = 0;
        }
        if (cpr & CPR_EXT_PRESENT) {
            t = (s32)(OSTicksToMilliseconds(OSGetTime()) / 100) + 1;
            if (__EXIInsertTime[chan] == 0) {
                __EXIInsertTime[chan] = t;
            }
            if (t - __EXIInsertTime[chan] < INSERT_SETTLE_TIME) {
                rc = FALSE;
            }
        } else {
            __EXIInsertTime[chan] = exi->idTime = 0;
            rc = FALSE;
        }
    } else if (!(cpr & CPR_EXT_PRESENT) || (cpr & CPR_EXT_INT)) {
        __EXIInsertTime[chan] = exi->idTime = 0;
        rc = FALSE;
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

/* TRUE when a settled device is present whose ID could be read. */
BOOL EXIProbe(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL rc;
    u32 id;

    rc = __EXIProbe(chan);
    if (rc && exi->idTime == 0) {
        rc = EXIGetID(chan, 0, &id) ? TRUE : FALSE;
    }
    return rc;
}

/* 1: device present, 0: a device is still settling, -1: the slot is empty. */
s32 EXIProbeEx(s32 chan) {
    if (EXIProbe(chan)) {
        return 1;
    } else if (__EXIInsertTime[chan] != 0) {
        return 0;
    } else {
        return -1;
    }
}

static inline BOOL __EXIAttach(s32 chan, EXICallback extCallback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if ((exi->state & EXI_STATE_ATTACHED) || __EXIProbe(chan) == FALSE) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    EXIClearInterrupts(chan, TRUE, FALSE, FALSE);
    exi->extCallback = extCallback;
    __OSUnmaskInterrupts(IRQ_BIT(IRQ_EXI_0_EXT) >> (IRQS_PER_CHAN * chan));
    exi->state |= EXI_STATE_ATTACHED;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* Claims the device in a slot; extCallback runs when it is pulled out. */
BOOL EXIAttach(s32 chan, EXICallback extCallback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;
    BOOL rc;

    EXIProbe(chan);
    enabled = OSDisableInterrupts();
    if (exi->idTime == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    rc = __EXIAttach(chan, extCallback);
    OSRestoreInterrupts(enabled);
    return rc;
}

BOOL EXIDetach(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (!(exi->state & EXI_STATE_ATTACHED)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }
    if ((exi->state & EXI_STATE_LOCKED) && exi->dev == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state &= ~EXI_STATE_ATTACHED;
    __OSMaskInterrupts((IRQ_BIT(IRQ_EXI_0_EXI) | IRQ_BIT(IRQ_EXI_0_EXT)) >> (IRQS_PER_CHAN * chan));
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* Raises the chip select of a device at one of the clock rates. The slot's "removed" interrupt
 * is held off while its device is selected. */
BOOL EXISelect(s32 chan, u32 dev, u32 freq) {
    EXIControl *exi = &Ecb[chan];
    u32 cpr;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if ((exi->state & EXI_STATE_SELECTED) ||
        (chan != 2 && ((dev == 0 && !(exi->state & EXI_STATE_ATTACHED) && !__EXIProbe(chan)) ||
                       !(exi->state & EXI_STATE_LOCKED) || exi->dev != dev))) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state |= EXI_STATE_SELECTED;
    cpr = REG(chan, EXI_REG_CPR);
    cpr &= CPR_MASKS;
    cpr |= ((1 << dev) << 7) | (freq << 4);
    REG(chan, EXI_REG_CPR) = cpr;
    if (exi->state & EXI_STATE_ATTACHED) {
        switch (chan) {
        case 0:
            __OSMaskInterrupts(IRQ_BIT(IRQ_EXI_0_EXT));
            break;
        case 1:
            __OSMaskInterrupts(IRQ_BIT(IRQ_EXI_1_EXT));
            break;
        }
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIDeselect(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    u32 cpr;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (!(exi->state & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state &= ~EXI_STATE_SELECTED;
    cpr = REG(chan, EXI_REG_CPR);
    REG(chan, EXI_REG_CPR) = cpr & CPR_MASKS;
    if (exi->state & EXI_STATE_ATTACHED) {
        switch (chan) {
        case 0:
            __OSUnmaskInterrupts(IRQ_BIT(IRQ_EXI_0_EXT));
            break;
        case 1:
            __OSUnmaskInterrupts(IRQ_BIT(IRQ_EXI_1_EXT));
            break;
        }
    }
    OSRestoreInterrupts(enabled);
    if (chan != 2 && (cpr & CPR_CS0)) {
        return __EXIProbe(chan) ? TRUE : FALSE;
    }
    return TRUE;
}

static void EXIIntrruptHandler(__OSInterrupt interrupt, OSContext *context) {
    s32 chan;
    EXIControl *exi;
    EXICallback callback;
    OSContext exceptionContext;

    chan = (interrupt - IRQ_EXI_0_EXI) / IRQS_PER_CHAN;
    EXIClearInterrupts(chan, TRUE, FALSE, FALSE);
    exi = &Ecb[chan];
    callback = exi->exiCallback;
    if (callback) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

static void TCIntrruptHandler(__OSInterrupt interrupt, OSContext *context) {
    s32 chan;
    EXIControl *exi;
    EXICallback callback;
    OSContext exceptionContext;

    chan = (interrupt - IRQ_EXI_0_TC) / IRQS_PER_CHAN;
    exi = &Ecb[chan];
    __OSMaskInterrupts(IRQ_BIT(interrupt));
    EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
    callback = exi->tcCallback;
    if (callback) {
        exi->tcCallback = NULL;
        CompleteTransfer(chan);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

static void EXTIntrruptHandler(__OSInterrupt interrupt, OSContext *context) {
    s32 chan;
    EXIControl *exi;
    EXICallback callback;
    OSContext exceptionContext;

    chan = (interrupt - IRQ_EXI_0_EXT) / IRQS_PER_CHAN;
    __OSMaskInterrupts((IRQ_BIT(IRQ_EXI_0_EXI) | IRQ_BIT(IRQ_EXI_0_EXT)) >> (IRQS_PER_CHAN * chan));
    exi = &Ecb[chan];
    callback = exi->extCallback;
    exi->state &= ~EXI_STATE_ATTACHED;
    if (callback) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        exi->extCallback = NULL;
        callback(chan, context);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

void EXIInit(void) {
    u32 id;

    while ((REG(0, EXI_REG_CR) & CR_START) == CR_START || (REG(1, EXI_REG_CR) & CR_START) == CR_START ||
           (REG(2, EXI_REG_CR) & CR_START) == CR_START) {
    }
    __OSMaskInterrupts(IRQ_BIT(IRQ_EXI_0_EXI) | IRQ_BIT(IRQ_EXI_0_TC) | IRQ_BIT(IRQ_EXI_0_EXT) |
                       IRQ_BIT(IRQ_EXI_1_EXI) | IRQ_BIT(IRQ_EXI_1_TC) | IRQ_BIT(IRQ_EXI_1_EXT) |
                       IRQ_BIT(IRQ_EXI_2_EXI) | IRQ_BIT(IRQ_EXI_2_TC));
    REG(0, EXI_REG_CPR) = 0;
    REG(1, EXI_REG_CPR) = 0;
    REG(2, EXI_REG_CPR) = 0;
    REG(0, EXI_REG_CPR) = CPR_ROM_DISABLE;
    __OSSetInterruptHandler(IRQ_EXI_0_EXI, EXIIntrruptHandler);
    __OSSetInterruptHandler(IRQ_EXI_0_TC, TCIntrruptHandler);
    __OSSetInterruptHandler(IRQ_EXI_0_EXT, EXTIntrruptHandler);
    __OSSetInterruptHandler(IRQ_EXI_1_EXI, EXIIntrruptHandler);
    __OSSetInterruptHandler(IRQ_EXI_1_TC, TCIntrruptHandler);
    __OSSetInterruptHandler(IRQ_EXI_1_EXT, EXTIntrruptHandler);
    __OSSetInterruptHandler(IRQ_EXI_2_EXI, EXIIntrruptHandler);
    __OSSetInterruptHandler(IRQ_EXI_2_TC, TCIntrruptHandler);
    EXIGetID(0, 2, &IDSerialPort1);
    if (__OSInIPL) {
        Ecb[0].idTime = Ecb[1].idTime = __EXIInsertTime[0] = __EXIInsertTime[1] = 0;
        __EXIProbe(0);
        __EXIProbe(1);
    } else if (EXIGetID(0, 0, &id) && id == EXI_ID_DEBUG_ADAPTER) {
        __OSEnableBarnacle(1, 0);
    } else if (EXIGetID(1, 0, &id) && id == EXI_ID_DEBUG_ADAPTER) {
        __OSEnableBarnacle(0, 2);
    }
    OSRegisterVersion(__EXIVersion);
}

/* Takes the channel for a device. When it is taken already, unlockedCallback (if any) is queued
 * to be told when the channel is free; a device can wait only once. */
BOOL EXILock(s32 chan, u32 dev, EXICallback unlockedCallback) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;
    s32 i;

    enabled = OSDisableInterrupts();
    if (exi->state & EXI_STATE_LOCKED) {
        if (unlockedCallback) {
            for (i = 0; i < exi->items; i++) {
                if (exi->queue[i].dev == dev) {
                    OSRestoreInterrupts(enabled);
                    return FALSE;
                }
            }
            exi->queue[exi->items].callback = unlockedCallback;
            exi->queue[exi->items].dev = dev;
            exi->items++;
        }
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state |= EXI_STATE_LOCKED;
    exi->dev = dev;
    SetExiInterruptMask(chan, exi);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIUnlock(s32 chan) {
    EXIControl *exi = &Ecb[chan];
    BOOL enabled;
    EXICallback unlockedCallback;

    enabled = OSDisableInterrupts();
    if (!(exi->state & EXI_STATE_LOCKED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    exi->state &= ~EXI_STATE_LOCKED;
    SetExiInterruptMask(chan, exi);
    if (exi->items > 0) {
        unlockedCallback = exi->queue[0].callback;
        if (--exi->items > 0) {
            memmove(&exi->queue[0], &exi->queue[1], sizeof(exi->queue[0]) * exi->items);
        }
        unlockedCallback(chan, NULL);
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 EXIGetState(s32 chan) {
    EXIControl *exi = &Ecb[chan];

    return exi->state;
}

/* The channel became free while EXIGetID was waiting for it: read the ID now. */
static void UnlockedHandler(s32 chan, OSContext *context) {
    u32 id;

    EXIGetID(chan, 0, &id);
}

/* Reads a device's ID. For a slot device the result is the insertion time the ID belongs to (0
 * on failure), and the ID is kept until the device is removed. */
s32 EXIGetID(s32 chan, u32 dev, u32 *id) {
    EXIControl *exi = &Ecb[chan];
    BOOL err;
    u32 cmd;
    s32 startTime;
    BOOL enabled;

    if (chan == 0 && dev == 2 && IDSerialPort1 != 0) {
        *id = IDSerialPort1;
        return 1;
    }
    if (chan < 2 && dev == 0) {
        if (!__EXIProbe(chan)) {
            return 0;
        }
        if (exi->idTime == __EXIInsertTime[chan]) {
            *id = exi->id;
            return exi->idTime;
        }
        if (!__EXIAttach(chan, NULL)) {
            return 0;
        }
        startTime = __EXIInsertTime[chan];
    }
    enabled = OSDisableInterrupts();
    err = !EXILock(chan, dev, (chan < 2 && dev == 0) ? UnlockedHandler : NULL);
    if (!err) {
        err = !EXISelect(chan, dev, 0);
        if (!err) {
            cmd = 0;
            err |= !EXIImm(chan, &cmd, 2, EXI_WRITE, NULL);
            err |= !EXISync(chan);
            err |= !EXIImm(chan, id, 4, EXI_READ, NULL);
            err |= !EXISync(chan);
            err |= !EXIDeselect(chan);
        }
        EXIUnlock(chan);
    }
    OSRestoreInterrupts(enabled);
    if (chan < 2 && dev == 0) {
        EXIDetach(chan);
        enabled = OSDisableInterrupts();
        err |= (startTime != __EXIInsertTime[chan]);
        if (!err) {
            exi->id = *id;
            exi->idTime = startTime;
        }
        OSRestoreInterrupts(enabled);
        return err ? 0 : exi->idTime;
    }
    return err ? 0 : 1;
}

/* Dead-stripped from the game, but its device names stayed in the library's data, and they have
 * to come out of this file in the same order. Nothing in the binary says which ID each name
 * belongs to: the pairing below follows the IDs the rest of the library tests and the well-known
 * memory-card sizes, and is otherwise a guess. */
char *EXIGetTypeString(u32 type) {
    switch (type) {
    case 0x00000004:
        return "Memory Card 59";
    case 0x00000008:
        return "Memory Card 123";
    case 0x00000010:
        return "Memory Card 251";
    case 0x00000020:
        return "Memory Card 507";
    case 0x00000040:
        return "Memory Card 1019";
    case 0x00000080:
        return "Memory Card 2043";
    case EXI_ID_USB_ADAPTER:
        return "USB Adapter";
    case 0x80000004:
    case 0x80000008:
    case 0x80000010:
    case 0x80000020:
        return "Net Card";
    case EXI_ID_ARTIST_ETHER:
        return "Artist Ether";
    case 0x04020200:
        return "Broadband Adapter";
    case 0x04130000:
        return "Stream Hanger";
    case EXI_ID_IS_VIEWER:
        return "IS-DOL-VIEWER";
    }
    return NULL;
}

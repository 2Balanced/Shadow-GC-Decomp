/* Dolphin SDK SI: the serial interface that the four controller ports hang off.
 *
 * One command/response transfer runs at a time (Si says which channel owns the bus). A transfer
 * asked for while the bus is busy, or before its channel's inter-command delay has passed, waits
 * in that channel's Packet slot; the transfer-complete interrupt and a per-channel alarm start
 * the waiting ones. Besides transfers the hardware polls the enabled channels by itself every
 * few lines; the "read status" interrupt hands the polled words to the registered handlers.
 *
 * The library also keeps each channel's device type, asked for again whenever it goes stale, and
 * settles the ID of a wireless (WaveBird) receiver against the one stored in SRAM. */
#include <dolphin/si.h>

#define SI_COMCSR_IDX 13
#define SI_STATUS_IDX 14
#define SI_POLL_IDX 12
#define SI_IOBUF_IDX 32

#define SI_COMCSR_TCINT 0x80000000
#define SI_COMCSR_TCINTMSK 0x40000000
#define SI_COMCSR_COMERR 0x20000000
#define SI_COMCSR_RDSTINT 0x10000000
#define SI_COMCSR_RDSTINTMSK 0x08000000
#define SI_COMCSR_TSTART 0x00000001

/* The type and status command, and the commands that fix a wireless receiver's ID. */
#define SI_CMD_TYPE_AND_STATUS 0x00
#define SI_CMD_FIX_DEVICE 0x4E000000

#define OS_TIMER_CLOCK (__OSBusClock / 4)
#define OSMicrosecondsToTicks(usec) (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))

typedef struct SIPacket {
    s32 chan; /* -1 while the slot is free */
    void *output;
    u32 outputBytes;
    void *input;
    u32 inputBytes;
    SICallback callback;
    OSTime fire; /* when the transfer may start */
} SIPacket;

typedef struct SIControl {
    s32 chan; /* the channel a transfer is running on, or -1 */
    u32 poll; /* what the poll register holds */
    u32 inputBytes;
    void *input;
    SICallback callback;
} SIControl;

typedef union SIComm {
    u32 val;
    struct {
        u32 tcint : 1;
        u32 tcintmsk : 1;
        u32 comerr : 1;
        u32 rdstint : 1;
        u32 rdstintmsk : 1;
        u32 pad0 : 4;
        u32 outlngth : 7;
        u32 pad1 : 1;
        u32 inlngth : 7;
        u32 pad2 : 5;
        u32 channel : 2;
        u32 tstart : 1;
    } f;
} SIComm;

volatile u32 __SIRegs[] : 0xCC006400;
u32 __OSBusClock : 0x800000F8;

u32 fn_8037F258(void);
#define VIGetCurrentLine fn_8037F258
u16 OSGetWirelessID(s32 chan);
void OSSetWirelessID(s32 chan, u16 id);

const char *__SIVersion = "<< Dolphin SDK - SI\trelease build: Apr  5 2004 04:14:16 (0x2301) >>";

static SIControl Si = {-1};
static u32 Type[SI_MAX_CHAN] = {
    SI_ERROR_NO_RESPONSE,
    SI_ERROR_NO_RESPONSE,
    SI_ERROR_NO_RESPONSE,
    SI_ERROR_NO_RESPONSE,
};

static SIPacket Packet[SI_MAX_CHAN];
static OSAlarm Alarm[SI_MAX_CHAN];
static OSTime TypeTime[SI_MAX_CHAN];
static OSTime XferTime[SI_MAX_CHAN];
static SITypeAndStatusCallback TypeCallback[SI_MAX_CHAN][4];
static __OSInterruptHandler RDSTHandler[4];
static BOOL InputBufferValid[SI_MAX_CHAN];
static u32 InputBuffer[SI_MAX_CHAN][2];
static volatile u32 InputBufferVcount[SI_MAX_CHAN];

/* Channels whose wireless receiver ID is to be fixed; set by PAD. */
u32 __PADFixBits;

static BOOL __SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes,
                         SICallback callback);
static BOOL SIGetResponseRaw(s32 chan);
static void GetTypeCallback(s32 chan, u32 error, OSContext *context);

BOOL SIBusy(void) {
    return Si.chan != -1 ? TRUE : FALSE;
}

BOOL SIIsChanBusy(s32 chan) {
    return Packet[chan].chan != -1 || Si.chan == chan;
}

static void SIClearTCInterrupt(void) {
    u32 reg;

    reg = __SIRegs[SI_COMCSR_IDX];
    reg |= SI_COMCSR_TCINT;
    reg &= ~SI_COMCSR_TSTART;
    __SIRegs[SI_COMCSR_IDX] = reg;
}

/* Finishes the running transfer: copies the answer out of the I/O buffer and returns the
 * transfer's error bits. */
static u32 CompleteTransfer(void) {
    u32 sr;
    u32 i;
    u32 rLen;
    u8 *input;

    sr = __SIRegs[SI_STATUS_IDX];
    SIClearTCInterrupt();

    if (Si.chan != -1) {
        XferTime[Si.chan] = __OSGetSystemTime();

        input = Si.input;
        rLen = Si.inputBytes / 4;
        for (i = 0; i < rLen; i++) {
            *(u32 *)input = __SIRegs[SI_IOBUF_IDX + i];
            input += 4;
        }
        rLen = Si.inputBytes & 3;
        if (rLen != 0) {
            u32 word = __SIRegs[SI_IOBUF_IDX + i];

            for (i = 0; i < rLen; i++) {
                *input++ = (u8)(word >> (8 * (3 - i)));
            }
        }

        if (__SIRegs[SI_COMCSR_IDX] & SI_COMCSR_COMERR) {
            sr >>= 8 * (3 - Si.chan);
            sr &= 0xF;
            if ((sr & SI_ERROR_NO_RESPONSE) && !(Type[Si.chan] & SI_ERROR_BUSY)) {
                Type[Si.chan] = SI_ERROR_NO_RESPONSE;
            }
            if (sr == 0) {
                sr = SI_ERROR_COLLISION;
            }
        } else {
            TypeTime[Si.chan] = __OSGetSystemTime();
            sr = 0;
        }
        Si.chan = -1;
    }
    return sr;
}

/* Starts the next waiting transfer that is due, looking from the channel after `chan`. */
static void SITransferNext(s32 chan) {
    int i;
    SIPacket *packet;

    for (i = 0; i < SI_MAX_CHAN; ++i) {
        ++chan;
        chan %= SI_MAX_CHAN;
        packet = &Packet[chan];
        if (packet->chan != -1 && packet->fire <= __OSGetSystemTime()) {
            if (__SITransfer(packet->chan, packet->output, packet->outputBytes, packet->input,
                             packet->inputBytes, packet->callback)) {
                OSCancelAlarm(&Alarm[chan]);
                packet->chan = -1;
            }
            break;
        }
    }
}

static void SIInterruptHandler(__OSInterrupt interrupt, OSContext *context) {
    u32 reg;

    reg = __SIRegs[SI_COMCSR_IDX];

    if ((reg & (SI_COMCSR_TCINT | SI_COMCSR_TCINTMSK)) ==
        (SI_COMCSR_TCINT | SI_COMCSR_TCINTMSK)) {
        s32 chan;
        u32 sr;
        SICallback callback;

        chan = Si.chan;
        sr = CompleteTransfer();
        callback = Si.callback;
        Si.callback = NULL;

        SITransferNext(chan);

        if (callback != NULL) {
            callback(chan, sr, context);
        }

        sr = __SIRegs[SI_STATUS_IDX];
        sr &= 0x0F000000 >> (8 * chan);
        __SIRegs[SI_STATUS_IDX] = sr;

        /* The transfer was the type query's turn, taken by somebody else: ask again. */
        if (Type[chan] == SI_ERROR_BUSY && !SIIsChanBusy(chan)) {
            static u32 cmdTypeAndStatus = SI_CMD_TYPE_AND_STATUS << 24;

            SITransfer(chan, &cmdTypeAndStatus, 1, &Type[chan], 3, GetTypeCallback,
                       OSMicrosecondsToTicks(65));
        }
    }

    if ((reg & (SI_COMCSR_RDSTINT | SI_COMCSR_RDSTINTMSK)) ==
        (SI_COMCSR_RDSTINT | SI_COMCSR_RDSTINTMSK)) {
        int i;
        u32 vcount;
        u32 x;

        vcount = VIGetCurrentLine() + 1;
        x = (Si.poll & 0x03FF0000) >> 16;

        for (i = 0; i < SI_MAX_CHAN; ++i) {
            if (SIGetResponseRaw(i)) {
                InputBufferVcount[i] = vcount;
            }
        }

        /* Wait until every polled channel has answered within the last half period. */
        for (i = 0; i < SI_MAX_CHAN; ++i) {
            if (!(Si.poll & (SI_CHAN0_BIT >> (31 - 7 + i)))) {
                continue;
            }
            if (InputBufferVcount[i] == 0 || InputBufferVcount[i] + (x / 2) < vcount) {
                return;
            }
        }

        for (i = 0; i < SI_MAX_CHAN; ++i) {
            InputBufferVcount[i] = 0;
        }

        for (i = 0; i < 4; ++i) {
            if (RDSTHandler[i] != NULL) {
                RDSTHandler[i](interrupt, context);
            }
        }
    }
}

static BOOL SIEnablePollingInterrupt(BOOL enable) {
    BOOL enabled;
    BOOL rc;
    u32 reg;
    int i;

    enabled = OSDisableInterrupts();
    reg = __SIRegs[SI_COMCSR_IDX];
    rc = (reg & SI_COMCSR_RDSTINTMSK) ? TRUE : FALSE;
    if (enable) {
        reg |= SI_COMCSR_RDSTINTMSK;
        for (i = 0; i < SI_MAX_CHAN; ++i) {
            InputBufferVcount[i] = 0;
        }
    } else {
        reg &= ~SI_COMCSR_RDSTINTMSK;
    }
    reg &= ~(SI_COMCSR_TCINT | SI_COMCSR_TSTART);
    __SIRegs[SI_COMCSR_IDX] = reg;
    OSRestoreInterrupts(enabled);
    return rc;
}

BOOL SIRegisterPollingHandler(__OSInterruptHandler handler) {
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();
    for (i = 0; i < 4; ++i) {
        if (RDSTHandler[i] == handler) {
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    for (i = 0; i < 4; ++i) {
        if (RDSTHandler[i] == NULL) {
            RDSTHandler[i] = handler;
            SIEnablePollingInterrupt(TRUE);
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}

BOOL SIUnregisterPollingHandler(__OSInterruptHandler handler) {
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();
    for (i = 0; i < 4; ++i) {
        if (RDSTHandler[i] == handler) {
            RDSTHandler[i] = NULL;
            for (i = 0; i < 4; ++i) {
                if (RDSTHandler[i] != NULL) {
                    break;
                }
            }
            if (i == 4) {
                SIEnablePollingInterrupt(FALSE);
            }
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}

void SIInit(void) {
    OSRegisterVersion(__SIVersion);

    Packet[0].chan = Packet[1].chan = Packet[2].chan = Packet[3].chan = -1;

    Si.poll = 0;
    SISetSamplingRate(0);

    while (__SIRegs[SI_COMCSR_IDX] & SI_COMCSR_TSTART) {
    }
    __SIRegs[SI_COMCSR_IDX] = SI_COMCSR_TCINT;

    __OSSetInterruptHandler(20, SIInterruptHandler);
    __OSUnmaskInterrupts(0x00000800);

    SIGetType(0);
    SIGetType(1);
    SIGetType(2);
    SIGetType(3);
}

static BOOL __SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes,
                         SICallback callback) {
    BOOL enabled;
    u32 rLen;
    u32 i;
    u32 sr;
    SIComm comcsr;

    enabled = OSDisableInterrupts();
    if (Si.chan != -1) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    sr = __SIRegs[SI_STATUS_IDX];
    sr &= 0x0F000000 >> (8 * chan);
    __SIRegs[SI_STATUS_IDX] = sr;

    Si.chan = chan;
    Si.callback = callback;
    Si.inputBytes = inputBytes;
    Si.input = input;

    rLen = (outputBytes + 3) / 4;
    for (i = 0; i < rLen; i++) {
        __SIRegs[SI_IOBUF_IDX + i] = ((u32 *)output)[i];
    }

    comcsr.val = __SIRegs[SI_COMCSR_IDX];
    comcsr.f.tcint = 1;
    comcsr.f.tcintmsk = callback ? 1 : 0;
    comcsr.f.outlngth = (outputBytes == SI_MAX_COMCSR_OUTLNGTH) ? 0 : outputBytes;
    comcsr.f.inlngth = (inputBytes == SI_MAX_COMCSR_INLNGTH) ? 0 : inputBytes;
    comcsr.f.channel = chan;
    comcsr.f.tstart = 1;
    __SIRegs[SI_COMCSR_IDX] = comcsr.val;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 SIGetStatus(s32 chan) {
    BOOL enabled;
    u32 sr;
    int chanShift;

    enabled = OSDisableInterrupts();
    sr = __SIRegs[SI_STATUS_IDX];
    chanShift = 8 * (SI_MAX_CHAN - 1 - chan);
    sr >>= chanShift;
    if (sr & SI_ERROR_NO_RESPONSE) {
        if (!(Type[chan] & SI_ERROR_BUSY)) {
            Type[chan] = SI_ERROR_NO_RESPONSE;
        }
    }
    OSRestoreInterrupts(enabled);
    return sr;
}

void SISetCommand(s32 chan, u32 command) {
    __SIRegs[3 * chan] = command;
}

void SITransferCommands(void) {
    __SIRegs[SI_STATUS_IDX] = 0x80000000;
}

u32 SISetXY(u32 x, u32 y) {
    u32 poll;
    BOOL enabled;

    poll = x << 16;
    poll |= y << 8;

    enabled = OSDisableInterrupts();
    Si.poll &= ~(0x03FF0000 | 0x0000FF00);
    Si.poll |= poll;
    poll = Si.poll;
    __SIRegs[SI_POLL_IDX] = poll;
    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIEnablePolling(u32 poll) {
    BOOL enabled;
    u32 en;

    if (poll == 0) {
        return Si.poll;
    }

    enabled = OSDisableInterrupts();

    poll >>= 31 - 7;
    en = poll & 0xF0;

    poll &= (en >> 4) | 0x03FFFFF0;
    poll &= ~(0x03FF0000 | 0x0000FF00);

    Si.poll &= ~(en >> 4);
    Si.poll |= poll;
    poll = Si.poll;

    SITransferCommands();
    __SIRegs[SI_POLL_IDX] = poll;

    OSRestoreInterrupts(enabled);
    return poll;
}

u32 SIDisablePolling(u32 poll) {
    BOOL enabled;

    if (poll == 0) {
        return Si.poll;
    }

    enabled = OSDisableInterrupts();

    poll >>= 31 - 7;
    poll &= 0xF0;

    poll = Si.poll & ~poll;
    __SIRegs[SI_POLL_IDX] = poll;
    Si.poll = poll;

    OSRestoreInterrupts(enabled);
    return poll;
}

static BOOL SIGetResponseRaw(s32 chan) {
    u32 sr;

    sr = SIGetStatus(chan);
    if (sr & SI_ERROR_RDST) {
        InputBuffer[chan][0] = __SIRegs[3 * chan + 1];
        InputBuffer[chan][1] = __SIRegs[3 * chan + 2];
        InputBufferValid[chan] = TRUE;
        return TRUE;
    }
    return FALSE;
}

BOOL SIGetResponse(s32 chan, void *data) {
    BOOL rc;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SIGetResponseRaw(chan);
    rc = InputBufferValid[chan];
    InputBufferValid[chan] = FALSE;
    if (rc) {
        ((u32 *)data)[0] = InputBuffer[chan][0];
        ((u32 *)data)[1] = InputBuffer[chan][1];
    }
    OSRestoreInterrupts(enabled);
    return rc;
}

static void AlarmHandler(OSAlarm *alarm, OSContext *context) {
    s32 chan;
    SIPacket *packet;

    chan = alarm - Alarm;
    packet = &Packet[chan];
    if (packet->chan != -1) {
        if (__SITransfer(packet->chan, packet->output, packet->outputBytes, packet->input,
                         packet->inputBytes, packet->callback)) {
            packet->chan = -1;
        }
    }
}

BOOL SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes,
                SICallback callback, OSTime delay) {
    BOOL enabled;
    SIPacket *packet = &Packet[chan];
    OSTime now;
    OSTime fire;

    enabled = OSDisableInterrupts();
    if (packet->chan != -1 || Si.chan == chan) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    now = __OSGetSystemTime();
    if (delay == 0) {
        fire = now;
    } else {
        fire = XferTime[chan] + delay;
    }
    if (now < fire) {
        delay = fire - now;
        OSSetAlarm(&Alarm[chan], delay, AlarmHandler);
    } else if (__SITransfer(chan, output, outputBytes, input, inputBytes, callback)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }

    packet->chan = chan;
    packet->output = output;
    packet->outputBytes = outputBytes;
    packet->input = input;
    packet->inputBytes = inputBytes;
    packet->callback = callback;
    packet->fire = fire;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

static inline void CallTypeAndStatusCallback(s32 chan, u32 type) {
    SITypeAndStatusCallback callback;
    int i;

    for (i = 0; i < 4; ++i) {
        callback = TypeCallback[chan][i];
        if (callback != NULL) {
            TypeCallback[chan][i] = NULL;
            callback(chan, type);
        }
    }
}

static void GetTypeCallback(s32 chan, u32 error, OSContext *context) {
    static u32 cmdFixDevice[SI_MAX_CHAN];
    u32 type;
    u32 chanBit;
    BOOL fix;
    u32 id;

    Type[chan] &= ~SI_ERROR_BUSY;
    Type[chan] |= error;
    TypeTime[chan] = __OSGetSystemTime();

    type = Type[chan];

    chanBit = SI_CHAN0_BIT >> chan;
    fix = (BOOL)(__PADFixBits & chanBit);
    __PADFixBits &= ~chanBit;

    if ((error & 0xF) || (type & SI_TYPE_MASK) != SI_TYPE_GC || !(type & SI_GC_WIRELESS) ||
        (type & SI_WIRELESS_IR)) {
        OSSetWirelessID(chan, 0);
        CallTypeAndStatusCallback(chan, Type[chan]);
        return;
    }

    id = (u32)(OSGetWirelessID(chan) << 8);

    if (fix && (id & SI_WIRELESS_FIX_ID)) {
        cmdFixDevice[chan] = SI_CMD_FIX_DEVICE | SI_WIRELESS_FIX_ID | (id & SI_WIRELESS_TYPE_ID);
        Type[chan] = SI_ERROR_BUSY;
        SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
        return;
    }

    if (type & SI_WIRELESS_FIX_ID) {
        if ((id & SI_WIRELESS_TYPE_ID) != (type & SI_WIRELESS_TYPE_ID)) {
            if (!(id & SI_WIRELESS_FIX_ID)) {
                id = SI_WIRELESS_FIX_ID | (type & SI_WIRELESS_TYPE_ID);
                OSSetWirelessID(chan, (u16)((id >> 8) & 0xFFFF));
            }
            cmdFixDevice[chan] = SI_CMD_FIX_DEVICE | id;
            Type[chan] = SI_ERROR_BUSY;
            SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
            return;
        }
    } else if (type & SI_WIRELESS_RECEIVED) {
        id = type & SI_WIRELESS_TYPE_ID;
        id |= SI_WIRELESS_FIX_ID;
        OSSetWirelessID(chan, (u16)((id >> 8) & 0xFFFF));
        cmdFixDevice[chan] = SI_CMD_FIX_DEVICE | id;
        Type[chan] = SI_ERROR_BUSY;
        SITransfer(chan, &cmdFixDevice[chan], 3, &Type[chan], 3, GetTypeCallback, 0);
        return;
    } else {
        OSSetWirelessID(chan, 0);
    }

    CallTypeAndStatusCallback(chan, Type[chan]);
}

u32 SIGetType(s32 chan) {
    static u32 cmdTypeAndStatus;
    BOOL enabled;
    u32 type;
    OSTime diff;

    enabled = OSDisableInterrupts();

    type = Type[chan];
    diff = __OSGetSystemTime() - TypeTime[chan];
    if (Si.poll & (0x80 >> chan)) {
        if (type != SI_ERROR_NO_RESPONSE) {
            TypeTime[chan] = __OSGetSystemTime();
            OSRestoreInterrupts(enabled);
            return type;
        } else {
            type = Type[chan] = SI_ERROR_BUSY;
        }
    } else if (diff <= OSMillisecondsToTicks(50) && type != SI_ERROR_NO_RESPONSE) {
        OSRestoreInterrupts(enabled);
        return type;
    } else if (diff <= OSMillisecondsToTicks(75)) {
        Type[chan] = SI_ERROR_BUSY;
    } else {
        type = Type[chan] = SI_ERROR_BUSY;
    }
    TypeTime[chan] = __OSGetSystemTime();

    SITransfer(chan, &cmdTypeAndStatus, 1, &Type[chan], 3, GetTypeCallback,
               OSMicrosecondsToTicks(65));

    OSRestoreInterrupts(enabled);
    return type;
}

u32 SIGetTypeAsync(s32 chan, SITypeAndStatusCallback callback) {
    BOOL enabled;
    u32 type;

    enabled = OSDisableInterrupts();
    type = SIGetType(chan);
    if (Type[chan] & SI_ERROR_BUSY) {
        int i;

        for (i = 0; i < 4; ++i) {
            if (TypeCallback[chan][i] == callback) {
                break;
            }
            if (TypeCallback[chan][i] == NULL) {
                TypeCallback[chan][i] = callback;
                break;
            }
        }
    } else {
        callback(chan, type);
    }
    OSRestoreInterrupts(enabled);
    return type;
}

/* Dead-stripped; kept for its names, which stay in the data. */
char *SIGetTypeString(u32 type) {
    if (type == SI_ERROR_NO_RESPONSE) {
        return "No response";
    }
    if (type == SI_N64_CONTROLLER) {
        return "N64 controller";
    }
    if (type == SI_N64_MIC) {
        return "N64 microphone";
    }
    if (type == SI_N64_KEYBOARD) {
        return "N64 keyboard";
    }
    if (type == SI_N64_MOUSE) {
        return "N64 mouse";
    }
    if (type == SI_GBA) {
        return "GameBoy Advance";
    }
    if (type == SI_GC_CONTROLLER) {
        return "Standard controller";
    }
    if (type == SI_GC_RECEIVER) {
        return "Wireless receiver";
    }
    if (type == SI_GC_WAVEBIRD) {
        return "WaveBird controller";
    }
    if (type == SI_GC_KEYBOARD) {
        return "Keyboard";
    }
    if (type == SI_GC_STEERING) {
        return "Steering";
    }
    return NULL;
}

/* Dolphin SDK odemuexi2: the debugger link through the development hardware on EXI channel 2
 * (device 0). The hardware has a status word, a mailbox and a shared buffer; a message is put
 * in the buffer and announced through the mailbox, and an incoming mailbox word raises the
 * debug interrupt. The driver talks to the channel's registers directly instead of going
 * through the EXI library, so that it works while the library is being debugged.
 *
 * Nothing in the binary names these functions; the macros give them readable names.
 *
 * Built with GC/1.2.5 (the rest of the SDK here is 1.2.5n): the epilogues restore the link
 * register before the last saved register. Whether a helper is inlined is the compiler's own
 * choice (-inline auto), and it follows the helper's statement count: DBGRead and DBGWrite stay
 * out of line only with the pointer step written as its own statement. */
#include <dolphin/os_interrupt.h>

#define DBGEXIImm fn_803B1FE4
#define DBGReadMailbox fn_803B227C
#define DBGRead fn_803B2328
#define DBGWrite fn_803B2404
#define DBGReadStatus fn_803B24E0
#define MWCallback fn_803B258C
#define DBGHandler fn_803B25C8
#define DBInitComm fn_803B2608
#define DBInitInterrupts fn_803B2680
#define DBQueryData fn_803B26D4
#define DBRead fn_803B2770
#define DBWrite fn_803B27FC
#define DBOpen fn_803B2A5C
#define DBClose fn_803B2A60

typedef void (*MTRCallbackType)(int);
typedef void (*DBGCallbackType)(u32, OSContext *);

volatile u32 __PIRegs[] : 0xCC003000;
volatile u32 __EXIRegs[] : 0xCC006800;

/* Channel 2's registers. */
#define EXI2_CPR 10
#define EXI2_CR 13
#define EXI2_DATA 14

#define CPR_MASKS 0x405
#define CPR_SELECT_DEV0_8MHZ 0xC0
#define CR_START 1

#define PI_INTSR 0
#define PI_INT_DEBUG 0x1000

#define IRQ_EXI_2_EXI_BIT 0x10000
#define IRQ_EXI_2_TC_BIT 0x8000
#define IRQ_PI_DEBUG 25
#define IRQ_PI_DEBUG_BIT 0x40

/* Commands: the top three bits of the first word. */
#define CMD_READ_BUFFER 0x20000000
#define CMD_READ_STATUS 0x40000000
#define CMD_READ_MAILBOX 0x60000000
#define CMD_WRITE_BUFFER 0xA0000000
#define CMD_WRITE_MAILBOX 0xC0000000
#define BUFFER_ADDR(addr) (((addr) << 8) & 0x01FFFC00)

#define STATUS_RECV_PENDING 0x01 /* a mailbox word from the host is waiting */
#define STATUS_SEND_PENDING 0x02 /* the host has not yet taken our mailbox word */

/* Mailbox word: 0x1F in bits 24-28, a toggling sequence bit, the message length. */
#define MAIL_MASK 0x1FFFFFFF
#define MAIL_MAGIC 0x1F000000
#define MAIL_PAGE 0x00010000
#define MAIL_LENGTH 0x00007FFF

/* The shared buffer: two pages for each direction, chosen by the sequence bit. */
#define SEND_BUFFER 0x1C000
#define RECV_BUFFER 0x1E000
#define BUFFER_PAGE 0x1000

static MTRCallbackType MTRCallback;
static DBGCallbackType DBGCallback;
static u32 SendMailData;
static s32 RecvDataLeng;
static u8 *pEXIInputFlag;
static u8 EXIInputFlag;
static u8 SendCount = 0x80;

static inline BOOL DBGEXISelect(u32 v) {
    u32 regs = __EXIRegs[EXI2_CPR];

    regs &= CPR_MASKS;
    regs |= v;
    __EXIRegs[EXI2_CPR] = regs;
    return TRUE;
}

static inline BOOL DBGEXIDeselect(void) {
    __EXIRegs[EXI2_CPR] &= CPR_MASKS;
    return TRUE;
}

static inline BOOL DBGEXISync(void) {
    while (__EXIRegs[EXI2_CR] & CR_START) {
    }
    return TRUE;
}

/* One immediate transfer of up to four bytes, waited for. */
static BOOL DBGEXIImm(void *buffer, s32 bytes, u32 write) {
    u32 data;
    int i;

    if (write) {
        data = 0;
        for (i = 0; i < bytes; i++) {
            data |= ((u8 *)buffer)[i] << ((3 - i) * 8);
        }
        __EXIRegs[EXI2_DATA] = data;
    }
    __EXIRegs[EXI2_CR] = CR_START | (write << 2) | ((bytes - 1) << 4);
    while (__EXIRegs[EXI2_CR] & CR_START) {
    }
    if (!write) {
        u8 *p = buffer;

        data = __EXIRegs[EXI2_DATA];
        for (i = 0; i < bytes; i++) {
            *p++ = (u8)(data >> ((3 - i) * 8));
        }
    }
    return TRUE;
}

static BOOL DBGReadMailbox(u32 *mail) {
    BOOL err;
    u32 cmd;

    DBGEXISelect(CPR_SELECT_DEV0_8MHZ);
    cmd = CMD_READ_MAILBOX;
    err = !DBGEXIImm(&cmd, 2, TRUE);
    err |= !DBGEXISync();
    err |= !DBGEXIImm(mail, 4, FALSE);
    err |= !DBGEXISync();
    err |= !DBGEXIDeselect();
    return !err;
}

static BOOL DBGRead(u32 addr, u32 *buffer, s32 bytes) {
    BOOL err;
    u32 cmd;
    u32 data;
    u32 *p = buffer;

    DBGEXISelect(CPR_SELECT_DEV0_8MHZ);
    cmd = BUFFER_ADDR(addr) | CMD_READ_BUFFER;
    err = !DBGEXIImm(&cmd, 4, TRUE);
    err |= !DBGEXISync();
    while (bytes) {
        err |= !DBGEXIImm(&data, 4, FALSE);
        err |= !DBGEXISync();
        *p = data;
        p++; /* a statement of its own: see the note on inlining at the top */
        bytes -= 4;
        if (bytes < 0) {
            bytes = 0;
        }
    }
    err |= !DBGEXIDeselect();
    return !err;
}

static BOOL DBGWrite(u32 addr, u32 *buffer, s32 bytes) {
    BOOL err;
    u32 cmd;
    u32 data;
    u32 *p = buffer;

    DBGEXISelect(CPR_SELECT_DEV0_8MHZ);
    cmd = BUFFER_ADDR(addr) | CMD_WRITE_BUFFER;
    err = !DBGEXIImm(&cmd, 4, TRUE);
    err |= !DBGEXISync();
    while (bytes) {
        data = *p;
        p++; /* a statement of its own: see the note on inlining at the top */
        err |= !DBGEXIImm(&data, 4, TRUE);
        err |= !DBGEXISync();
        bytes -= 4;
        if (bytes < 0) {
            bytes = 0;
        }
    }
    err |= !DBGEXIDeselect();
    return !err;
}

static BOOL DBGReadStatus(u32 *status) {
    BOOL err;
    u32 cmd;

    DBGEXISelect(CPR_SELECT_DEV0_8MHZ);
    cmd = CMD_READ_STATUS;
    err = !DBGEXIImm(&cmd, 2, TRUE);
    err |= !DBGEXISync();
    err |= !DBGEXIImm(status, 4, FALSE);
    err |= !DBGEXISync();
    err |= !DBGEXIDeselect();
    return !err;
}

/* The two extra locals each take a word of DBWrite's frame, as retail's does. */
static inline BOOL DBGWriteMailbox(u32 mail) {
    BOOL err;
    u32 value;
    u32 cmd;
    u32 code;

    DBGEXISelect(CPR_SELECT_DEV0_8MHZ);
    value = mail & MAIL_MASK;
    code = CMD_WRITE_MAILBOX;
    cmd = value | code;
    err = !DBGEXIImm(&cmd, 4, TRUE);
    err |= !DBGEXISync();
    err |= !DBGEXIDeselect();
    return !err;
}

/* The debug interrupt as the debugger's monitor sees it: input is waiting. */
static void MWCallback(u32 interrupt, OSContext *context) {
    EXIInputFlag = TRUE;
    if (MTRCallback) {
        MTRCallback(0);
    }
}

static void DBGHandler(__OSInterrupt interrupt, OSContext *context) {
    __PIRegs[PI_INTSR] = PI_INT_DEBUG;
    if (DBGCallback) {
        DBGCallback(interrupt, context);
    }
}

void DBInitComm(u8 **inputFlagPtr, MTRCallbackType monitorCallback) {
    BOOL enabled = OSDisableInterrupts();

    pEXIInputFlag = &EXIInputFlag;
    *inputFlagPtr = pEXIInputFlag;
    MTRCallback = monitorCallback;
    __OSMaskInterrupts(IRQ_EXI_2_EXI_BIT | IRQ_EXI_2_TC_BIT);
    __EXIRegs[EXI2_CPR] = 0;
    OSRestoreInterrupts(enabled);
}

void DBInitInterrupts(void) {
    __OSMaskInterrupts(IRQ_EXI_2_EXI_BIT | IRQ_EXI_2_TC_BIT);
    __OSMaskInterrupts(IRQ_PI_DEBUG_BIT);
    DBGCallback = MWCallback;
    __OSSetInterruptHandler(IRQ_PI_DEBUG, DBGHandler);
    __OSUnmaskInterrupts(IRQ_PI_DEBUG_BIT);
}

/* The length of the message that is waiting, 0 if there is none. Retail calls the two status
 * helpers here although DBWrite has DBGReadStatus inlined. If nothing is waiting to be asked
 * for, `enabled` is restored without having been set. */
#pragma dont_inline on
u32 DBQueryData(void) {
    BOOL enabled;
    u32 status;

    EXIInputFlag = FALSE;
    if (RecvDataLeng == 0) {
        enabled = OSDisableInterrupts();
        DBGReadStatus(&status);
        if (status & STATUS_RECV_PENDING) {
            DBGReadMailbox(&status);
            status &= MAIL_MASK;
            if ((status & MAIL_MAGIC) == MAIL_MAGIC) {
                SendMailData = status;
                RecvDataLeng = status & MAIL_LENGTH;
                EXIInputFlag = TRUE;
            }
        }
    }
    OSRestoreInterrupts(enabled);
    return RecvDataLeng;
}

#pragma dont_inline reset

u32 DBRead(u32 *buffer, s32 bytes) {
    BOOL enabled = OSDisableInterrupts();

    DBGRead(((SendMailData & MAIL_PAGE) ? BUFFER_PAGE : 0) + RECV_BUFFER, buffer, (bytes + 3) & ~3);
    RecvDataLeng = 0;
    EXIInputFlag = FALSE;
    OSRestoreInterrupts(enabled);
    return 0;
}

u32 DBWrite(const void *buffer, u32 bytes) {
    BOOL enabled;
    u32 busy;
    u32 offset;
    u32 mail;

    enabled = OSDisableInterrupts();
    do {
        DBGReadStatus(&busy);
    } while (busy & STATUS_SEND_PENDING);
    SendCount++;
    offset = (SendCount & 1) ? BUFFER_PAGE : 0;
    while (!DBGWrite(offset | SEND_BUFFER, (u32 *)buffer, (bytes + 3) & ~3)) {
    }
    do {
        DBGReadStatus(&busy);
    } while (busy & STATUS_SEND_PENDING);
    mail = MAIL_MAGIC | (SendCount << 16) | bytes;
    while (!DBGWriteMailbox(mail)) {
    }
    do {
        while (!DBGReadStatus(&busy)) {
        }
    } while (busy & STATUS_SEND_PENDING);
    OSRestoreInterrupts(enabled);
    return 0;
}

void DBOpen(void) {
}

void DBClose(void) {
}

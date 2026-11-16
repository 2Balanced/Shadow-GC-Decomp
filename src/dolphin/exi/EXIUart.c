/* Dolphin SDK EXI: the debug console's serial output. Text goes to the UART of a development
 * console (channel 0, device 1) or to a Barnacle adapter that EXIInit found in a memory-card slot
 * or on serial port 1. The C library's console writes come through InitializeUART and
 * WriteUARTN. */
#include <dolphin/exi.h>

#define UART_ENABLED 0xA5FF005A

#define UART_FREQ 3 /* 8 MHz */

/* Commands, sent as four bytes. */
#define UART_CMD_REVISION 0x20011300
#define UART_CMD_QUEUE_LENGTH 0x20010000
#define UART_CMD_WRITE 0xA0010000

#define UART_QUEUE_SIZE 16
#define UART_BURST 12 /* wait until the adapter has room for this much (or all that is left) */

/* C library console results. */
#define UART_NO_ERROR 0
#define UART_NOT_INITIALIZED 2
#define UART_QUEUE_ERROR 3

#define CONSOLE_DEVELOPMENT 0x10000000

u32 OSGetConsoleType(void);

static s32 Chan;
static u32 Dev;
static u32 Enabled;
static u32 BarnacleEnabled;

/* Asks the device for its revision; an empty bus reads back as all ones. */
static BOOL ProbeBarnacle(s32 chan, u32 dev, u32 *revision) {
    BOOL err;
    u32 cmd;

    if (chan != 2 && dev == 0) {
        if (!EXIAttach(chan, NULL)) {
            return FALSE;
        }
    }
    err = !EXILock(chan, dev, NULL);
    if (!err) {
        err = !EXISelect(chan, dev, 0);
        if (!err) {
            cmd = UART_CMD_REVISION;
            err = !EXIImm(chan, &cmd, 4, EXI_WRITE, NULL);
            err |= !EXISync(chan);
            err |= !EXIImm(chan, revision, 4, EXI_READ, NULL);
            err |= !EXISync(chan);
            err |= !EXIDeselect(chan);
        }
        EXIUnlock(chan);
    }
    if (chan != 2 && dev == 0) {
        EXIDetach(chan);
    }
    if (err) {
        return FALSE;
    }
    return (*revision != 0xFFFFFFFF) ? TRUE : FALSE;
}

/* Uses the device at (chan, dev) for console output if it is none of the known devices and
 * answers like a Barnacle. */
void __OSEnableBarnacle(s32 chan, u32 dev) {
    u32 id;

    if (EXIGetID(chan, dev, &id)) {
        switch (id) {
        /* Nothing there, or one of the devices known when the library was built. */
        case 0xFFFFFFFF:
        case 0x00000004:
        case 0x00000008:
        case 0x00000010:
        case 0x00000020:
        case 0x80000004:
        case 0x80000008:
        case 0x80000010:
        case 0x80000020:
        case EXI_ID_USB_ADAPTER:
        case 0x01020000:
        case 0x02020000:
        case 0x03010000:
        case 0x04020100:
        case 0x04020200:
        case 0x04020300:
        case 0x04040404:
        case 0x04060000:
        case 0x04120000:
        case 0x04130000:
        case 0x04220000:
            break;
        default:
            if (ProbeBarnacle(chan, dev, &id)) {
                Chan = chan;
                Dev = dev;
                Enabled = BarnacleEnabled = UART_ENABLED;
            }
            break;
        }
    }
}

u32 InitializeUART(u32 baudRate) {
    if (BarnacleEnabled == UART_ENABLED) {
        return UART_NO_ERROR;
    }
    if (!(OSGetConsoleType() & CONSOLE_DEVELOPMENT)) {
        Enabled = 0;
        return UART_NOT_INITIALIZED;
    } else {
        Chan = 0;
        Dev = 1;
        Enabled = UART_ENABLED;
        return UART_NO_ERROR;
    }
}

/* How many bytes the adapter's transmit queue can still take; -1 if it cannot be asked. */
static inline int QueueLength(void) {
    u32 cmd;

    if (!EXISelect(Chan, Dev, UART_FREQ)) {
        return -1;
    }
    cmd = UART_CMD_QUEUE_LENGTH;
    EXIImm(Chan, &cmd, 4, EXI_WRITE, NULL);
    EXISync(Chan);
    EXIImm(Chan, &cmd, 1, EXI_READ, NULL);
    EXISync(Chan);
    EXIDeselect(Chan);
    return UART_QUEUE_SIZE - (int)((cmd >> 24) & 0xFF);
}

/* Sends len bytes, line feeds turned into carriage returns (in the caller's buffer). */
u32 WriteUARTN(const void *buf, unsigned long len) {
    u32 cmd;
    int qLen;
    long xLen;
    char *ptr;
    BOOL locked;
    u32 error;

    if (Enabled != UART_ENABLED) {
        return UART_NOT_INITIALIZED;
    }
    locked = EXILock(Chan, Dev, NULL);
    if (!locked) {
        return UART_NO_ERROR;
    }
    for (ptr = (char *)buf; ptr - (char *)buf < len; ptr++) {
        if (*ptr == '\n') {
            *ptr = '\r';
        }
    }
    error = UART_NO_ERROR;
    cmd = UART_CMD_WRITE;
    while (len) {
        qLen = QueueLength();
        if (qLen < 0) {
            error = UART_QUEUE_ERROR;
            break;
        }
        if (qLen >= UART_BURST || qLen >= len) {
            if (!EXISelect(Chan, Dev, UART_FREQ)) {
                error = UART_QUEUE_ERROR;
                break;
            }
            EXIImm(Chan, &cmd, 4, EXI_WRITE, NULL);
            EXISync(Chan);
            while (qLen && len) {
                if (qLen < 4 && qLen < len) {
                    break;
                }
                xLen = (len < 4) ? (long)len : 4;
                EXIImm(Chan, (void *)buf, xLen, EXI_WRITE, NULL);
                buf = (u8 *)buf + xLen;
                len -= xLen;
                qLen -= xLen;
                EXISync(Chan);
            }
            EXIDeselect(Chan);
        }
    }
    EXIUnlock(Chan);
    return error;
}

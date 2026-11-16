#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>
#include <TRK_MINNOW_DOLPHIN/CircleBuffer.h>

/* The connection through the GDEV's debugger port (the Dolphin SDK's OdemuExi2 library).
 * Received bytes are collected in a ring buffer until a whole message can be handed to the nub. */

/* The port's library (OdemuExi2). */
#define DBInitComm fn_803B2608
#define DBInitInterrupts fn_803B2680
#define DBQueryData fn_803B26D4
#define DBRead fn_803B2770
#define DBWrite fn_803B27FC
#define DBOpen fn_803B2A5C
#define DBClose fn_803B2A60

extern void DBInitComm(void *inputPendingPtr, DBCommCallback callback);
extern void DBInitInterrupts(void);
extern int DBQueryData(void);
extern int DBRead(void *data, u32 length);
extern int DBWrite(const void *data, u32 length);
extern void DBOpen(void);
extern void DBClose(void);

#define RECEIVE_BUFFER_SIZE 0x500

#define CC_NOT_INITIALIZED -10001
#define CC_ALREADY_OPEN -10005
#define CC_READ_ERROR -10009

static CircleBuffer gRecvCB;
static u8 gRecvBuf[RECEIVE_BUFFER_SIZE];
static BOOL gIsInitialized;

int gdev_cc_initialize(void *inputPendingPtr, DBCommCallback callback) {
    MWTRACE(1, "CALLING EXI2_Init\n");
    DBInitComm(inputPendingPtr, callback);
    MWTRACE(1, "DONE CALLING EXI2_Init\n");
    CircleBufferInitialize(&gRecvCB, gRecvBuf, RECEIVE_BUFFER_SIZE);
    return 0;
}

int gdev_cc_shutdown(void) {
    return 0;
}

int gdev_cc_open(void) {
    if (gIsInitialized) {
        return CC_ALREADY_OPEN;
    }
    gIsInitialized = TRUE;
    return 0;
}

int gdev_cc_close(void) {
    return 0;
}

int gdev_cc_read(u8 *data, int length) {
    u8 received[RECEIVE_BUFFER_SIZE];
    u32 result = 0;
    int pending;
    int wanted;
    int expected;

    if (!gIsInitialized) {
        return CC_NOT_INITIALIZED;
    }
    MWTRACE(1, "Expected packet size : 0x%08x (%ld)\n", length, length);
    expected = length;
    wanted = length;
    while (CBGetBytesAvailableForRead(&gRecvCB) < wanted) {
        result = 0;
        pending = DBQueryData();
        if (pending != 0) {
            result = DBRead(received, wanted);
            if (result == 0) {
                CircleBufferWriteBytes(&gRecvCB, received, pending);
            }
        }
    }
    if (result == 0) {
        CircleBufferReadBytes(&gRecvCB, data, expected);
    } else {
        MWTRACE(8, "cc_read : error reading bytes from EXI2 %ld\n", result);
    }
    return result;
}

int gdev_cc_write(const u8 *data, int length) {
    int left = length;
    const u8 *next = data;
    int written;

    if (!gIsInitialized) {
        MWTRACE(8, "cc not initialized\n");
        return CC_NOT_INITIALIZED;
    }
    MWTRACE(8, "cc_write : Output data 0x%08x %ld bytes\n", data, length);
    while (left > 0) {
        MWTRACE(1, "cc_write sending %ld bytes\n", left);
        written = DBWrite(next, left);
        if (written == 0) {
            break;
        }
        next += written;
        left -= written;
    }
    return 0;
}

int gdev_cc_pre_continue(void) {
    DBClose();
    return 0;
}

int gdev_cc_post_stop(void) {
    DBOpen();
    return 0;
}

int gdev_cc_peek(void) {
    u8 received[RECEIVE_BUFFER_SIZE];
    int pending = DBQueryData();

    if (pending <= 0) {
        return 0;
    }
    if (DBRead(received, pending) == 0) {
        CircleBufferWriteBytes(&gRecvCB, received, pending);
    } else {
        return CC_READ_ERROR;
    }
    return pending;
}

int gdev_cc_initinterrupts(void) {
    DBInitInterrupts();
    return 0;
}

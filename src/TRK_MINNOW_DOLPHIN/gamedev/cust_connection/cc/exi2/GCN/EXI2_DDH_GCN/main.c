#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>
#include <TRK_MINNOW_DOLPHIN/CircleBuffer.h>

/* The connection through the AMC DDH adapter (EXI2 of the development hardware). Received bytes
 * are collected in a ring buffer until a whole message can be handed to the nub. */

/* The adapter's library (AmcExi2); these are stubs in the game. */
#define EXI2_Init fn_803B1FAC
#define EXI2_EnableInterrupts fn_803B1FB0
#define EXI2_Poll fn_803B1FB4
#define EXI2_ReadN fn_803B1FBC
#define EXI2_WriteN fn_803B1FC4
#define EXI2_Reserve fn_803B1FCC
#define EXI2_Unreserve fn_803B1FD0

extern void EXI2_Init(void *inputPendingPtr, DBCommCallback callback);
extern void EXI2_EnableInterrupts(void);
extern int EXI2_Poll(void);
extern int EXI2_ReadN(void *data, u32 length);
extern int EXI2_WriteN(const void *data, u32 length);
extern void EXI2_Reserve(void);
extern void EXI2_Unreserve(void);

#define RECEIVE_BUFFER_SIZE 0x800

#define CC_NOT_INITIALIZED -10001
#define CC_ALREADY_OPEN -10005
#define CC_READ_ERROR -10009

static CircleBuffer gRecvCB;
static u8 gRecvBuf[RECEIVE_BUFFER_SIZE];
static BOOL gIsInitialized;

int ddh_cc_initialize(void *inputPendingPtr, DBCommCallback callback) {
    MWTRACE(1, "CALLING EXI2_Init\n");
    EXI2_Init(inputPendingPtr, callback);
    MWTRACE(1, "DONE CALLING EXI2_Init\n");
    CircleBufferInitialize(&gRecvCB, gRecvBuf, RECEIVE_BUFFER_SIZE);
    return 0;
}

int ddh_cc_shutdown(void) {
    return 0;
}

int ddh_cc_open(void) {
    if (gIsInitialized) {
        return CC_ALREADY_OPEN;
    }
    gIsInitialized = TRUE;
    return 0;
}

int ddh_cc_close(void) {
    return 0;
}

int ddh_cc_read(u8 *data, int length) {
    u8 received[RECEIVE_BUFFER_SIZE];
    u32 result = 0;
    int pending;

    if (!gIsInitialized) {
        return CC_NOT_INITIALIZED;
    }
    MWTRACE(1, "Expected packet size : 0x%08x (%ld)\n", length, length);
    while (CBGetBytesAvailableForRead(&gRecvCB) < length) {
        result = 0;
        pending = EXI2_Poll();
        if (pending != 0) {
            result = EXI2_ReadN(received, pending);
            if (result == 0) {
                CircleBufferWriteBytes(&gRecvCB, received, pending);
            }
        }
    }
    if (result == 0) {
        CircleBufferReadBytes(&gRecvCB, data, length);
    } else {
        MWTRACE(8, "cc_read : error reading bytes from EXI2 %ld\n", result);
    }
    return result;
}

int ddh_cc_write(const u8 *data, int length) {
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
        written = EXI2_WriteN(next, left);
        if (written == 0) {
            break;
        }
        next += written;
        left -= written;
    }
    return 0;
}

int ddh_cc_pre_continue(void) {
    EXI2_Unreserve();
    return 0;
}

int ddh_cc_post_stop(void) {
    EXI2_Reserve();
    return 0;
}

int ddh_cc_peek(void) {
    u8 received[RECEIVE_BUFFER_SIZE];
    int pending = EXI2_Poll();

    if (pending <= 0) {
        return 0;
    }
    if (EXI2_ReadN(received, pending) == 0) {
        CircleBufferWriteBytes(&gRecvCB, received, pending);
    } else {
        return CC_READ_ERROR;
    }
    return pending;
}

int ddh_cc_initinterrupts(void) {
    EXI2_EnableInterrupts();
    return 0;
}

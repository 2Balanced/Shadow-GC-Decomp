#include <TRK_MINNOW_DOLPHIN/trk.h>

BOOL gTRKBigEndian;

static inline BOOL TRKInitializeEndian(void);

TRKResult TRKInitializeNub(void) {
    TRKResult result;
    TRKResult uartResult;

    result = TRKInitializeEndian();
    MWTRACE(1, "Initialize NUB\n");
    if (result == kTRKOk) {
        usr_put_initialize();
    }
    if (result == kTRKOk) {
        result = TRKInitializeEventQueue();
    }
    if (result == kTRKOk) {
        result = TRKInitializeMessageBuffers();
    }
    if (result == kTRKOk) {
        result = TRKInitializeDispatcher();
    }
    InitializeProgramEndTrap();
    if (result == kTRKOk) {
        result = TRKInitializeSerialHandler();
    }
    if (result == kTRKOk) {
        result = TRKInitializeTarget();
    }
    if (result == kTRKOk) {
        uartResult = TRKInitializeIntDrivenUART(57600, 1, 0, &gTRKInputPendingPtr);
        TRKTargetSetInputPendingPtr(gTRKInputPendingPtr);
        if (uartResult != kTRKOk) {
            result = uartResult;
        }
    }
    return result;
}

TRKResult TRKTerminateNub(void) {
    TRKTerminateSerialHandler();
    return kTRKOk;
}

void TRKNubWelcome(void) {
    TRK_board_display("MetroTRK for GAMECUBE v2.6");
}

/* Finds the byte order of the processor the nub runs on; TRUE when it is neither. */
static inline BOOL TRKInitializeEndian(void) {
    u8 bytes[4];
    BOOL unknown = FALSE;

    gTRKBigEndian = TRUE;
    bytes[0] = 0x12;
    bytes[1] = 0x34;
    bytes[2] = 0x56;
    bytes[3] = 0x78;
    if (*(u32 *)bytes == 0x12345678) {
        gTRKBigEndian = TRUE;
    } else if (*(u32 *)bytes == 0x78563412) {
        gTRKBigEndian = FALSE;
    } else {
        unknown = TRUE;
    }
    return unknown;
}

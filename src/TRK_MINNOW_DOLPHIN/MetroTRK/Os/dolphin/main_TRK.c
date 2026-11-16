#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>

static TRKResult TRK_mainError;

void TRK_main(void) {
    MWTRACE(1, "TRK_Main \n");
    TRK_mainError = TRKInitializeNub();
    if (TRK_mainError == kTRKOk) {
        TRKNubWelcome();
        TRKNubMainLoop();
    }
    TRK_mainError = TRKTerminateNub();
}

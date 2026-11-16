#include <TRK_MINNOW_DOLPHIN/trk.h>

/* The nub's idle loop: handle queued events, poll the host connection when there is none, and let
 * the target run again once the connection is quiet. */
void TRKNubMainLoop(void) {
    TRKEvent event;
    BOOL shutdown = FALSE;
    BOOL polled = FALSE;

    while (!shutdown) {
        if (TRKGetNextEvent(&event)) {
            polled = FALSE;
            switch (event.type) {
            case kTRKEventNull:
                break;
            case kTRKEventRequest:
                TRKDispatchMessage(TRKGetBuffer(event.bufferId));
                break;
            case kTRKEventShutdown:
                shutdown = TRUE;
                break;
            case kTRKEventBreakpoint:
            case kTRKEventException:
                TRKTargetInterrupt(&event);
                break;
            case kTRKEventSupport:
                TRKTargetSupportRequest();
                break;
            }
            TRKDestructEvent(&event);
        } else if (!polled || *(u8 *)gTRKInputPendingPtr != 0) {
            polled = TRUE;
            TRKGetInput();
        } else {
            if (!TRKTargetStopped()) {
                TRKTargetContinue();
            }
            polled = FALSE;
        }
    }
}

#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>

extern void TRKSwapAndGo(void);

/* Lets the target run; returns when it stops again. */
TRKResult TRKTargetContinue(void) {
    TRKTargetSetStopped(FALSE);
    UnreserveEXI2Port();
    TRKSwapAndGo();
    ReserveEXI2Port();
    return kTRKOk;
}

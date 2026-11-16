#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>

/* Whether the target's console output goes to the host. */
static u8 bUseSerialIO;

void SetUseSerialIO(u8 enable) {
    bUseSerialIO = enable;
}

u8 GetUseSerialIO(void) {
    return bUseSerialIO;
}

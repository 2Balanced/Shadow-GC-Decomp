#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>

/* The target's requests to the nub (assembly in the original). Each is a trap that always fires:
 * the nub's program-exception handler sees it, finds the request in r3 and its arguments in
 * r4-r6, serves it (TRKTargetSupportRequest) and resumes after the trap with the result in r3.
 *
 * The game calls only two of them; force_active keeps the others linked, as the original
 * assembly file's single section did. */

#pragma force_active on

asm u32 TRKAccessFile(u32 command, u32 handle, TRKSize *count, u8 *data) {
    nofralloc
    twi 31, r0, 0
    blr
}

asm u32 TRKOpenFile(u32 command, const char *path, u32 mode, u32 *handle) {
    nofralloc
    twi 31, r0, 0
    blr
}

asm u32 TRKCloseFile(u32 command, u32 handle) {
    nofralloc
    twi 31, r0, 0
    blr
}

asm u32 TRKPositionFile(u32 command, u32 handle, u32 *position, u32 mode) {
    nofralloc
    twi 31, r0, 0
    blr
}

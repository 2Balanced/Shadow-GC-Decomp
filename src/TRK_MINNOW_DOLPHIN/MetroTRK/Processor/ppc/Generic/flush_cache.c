#include <TRK_MINNOW_DOLPHIN/trk.h>

/* Writes back and invalidates the cache lines of a range the debugger changed (a breakpoint, or
 * memory written by the host), so that the processor executes the new contents. */
asm void TRK_flush_cache(register u32 address, register u32 length) {
    nofralloc
    lis r5, 0xFFFF
    ori r5, r5, 0xFFF1
    and r5, r5, address
    subf address, r5, address
    add length, length, address
next:
    dcbst 0, r5
    dcbf 0, r5
    sync
    icbi 0, r5
    addic r5, r5, 8
    subic. length, length, 8
    bge next
    isync
    blr
}

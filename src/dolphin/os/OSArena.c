/* Dolphin SDK OS: the arena, the free memory between the end of the loaded program and the top
 * of main RAM. OSInit sets its bounds; allocations carve from either end and never return.
 * (OSAllocFromArenaHi is unused by the game and was dead-stripped.) */
#include <dolphin/types.h>

#define ROUND(n, a) (((u32)(n) + (a) - 1) & ~((a) - 1))

static void *__OSArenaHi;
static void *__OSArenaLo = (void *)-1; /* -1 until OSInit runs */

void *OSGetArenaHi(void) {
    return __OSArenaHi;
}

void *OSGetArenaLo(void) {
    return __OSArenaLo;
}

void OSSetArenaHi(void *newHi) {
    __OSArenaHi = newHi;
}

void OSSetArenaLo(void *newLo) {
    __OSArenaLo = newLo;
}

/* Takes `size` bytes from the bottom of the arena, aligned to `align` (a power of two); the new
 * bottom is aligned too. */
void *OSAllocFromArenaLo(u32 size, u32 align) {
    void *ptr;
    u8 *arenaLo;

    ptr = OSGetArenaLo();
    arenaLo = ptr = (void *)ROUND(ptr, align);
    arenaLo += size;
    arenaLo = (u8 *)ROUND(arenaLo, align);
    OSSetArenaLo(arenaLo);
    return ptr;
}

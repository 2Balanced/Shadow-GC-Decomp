/* Dolphin SDK OS: the arena is the span of main RAM between the end of the loaded program and
 * the top of memory. OSInit fixes both edges; allocating moves an edge inward for good, nothing is
 * ever handed back. Only the low edge is allocated from here (OSAllocFromArenaHi was stripped). */
#include <dolphin/types.h>

/* Bump `addr` up to the next multiple of `align`, which must be a power of two. */
#define ARENA_ALIGN_UP(addr, align) (((u32)(addr) + (align) - 1) & ~((align) - 1))

/* Upper edge lives in .sbss; the lower edge starts out as an all-ones "not set yet" marker. */
static void *__OSArenaHi;
static void *__OSArenaLo = (void *)-1;

void *OSGetArenaHi(void) { return __OSArenaHi; }
void *OSGetArenaLo(void) { return __OSArenaLo; }
void OSSetArenaHi(void *top) { __OSArenaHi = top; }
void OSSetArenaLo(void *bottom) { __OSArenaLo = bottom; }

/* Hands out `size` bytes at the aligned low edge, then re-aligns the edge past the block so the
 * next caller starts on a boundary as well. */
void *OSAllocFromArenaLo(u32 size, u32 align) {
    void *block;
    u8 *end;

    block = OSGetArenaLo();
    end = block = (void *)ARENA_ALIGN_UP(block, align);
    end += size;
    end = (u8 *)ARENA_ALIGN_UP(end, align);
    OSSetArenaLo(end);
    return block;
}

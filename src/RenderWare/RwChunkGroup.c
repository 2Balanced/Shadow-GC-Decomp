/* RenderWare core: the chunk group module (0x80482BC4-0x80482CA4).
 * Only the module open and close are left in retail (the RwChunkGroup API is dead-stripped): they
 * create and destroy the free list of 0x21-byte chunk groups (memory hint 0x40412). The file owns
 * the module globals (.sbss), the free list parameters (.sdata) and the free list (.bss).
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        RwFreeList *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);

static RwModuleInfo chunkGroupModule;
static s32 chunkGroupFreeListBlockSize = 0x10;
static s32 chunkGroupFreeListPreallocBlocks = 1;
static RwFreeList chunkGroupFreeList;

#define RWCHUNKGROUPGLOBAL (RWGLOBALAT(RwFreeList *, chunkGroupModule.globalsOffset))

/* Chunk group module open. */
void *fn_80482BC4(void *instance, s32 offset) {
    chunkGroupModule.globalsOffset = offset;
    RWCHUNKGROUPGLOBAL = fn_8047FC04(0x21, chunkGroupFreeListBlockSize, 4, chunkGroupFreeListPreallocBlocks, &chunkGroupFreeList, 0x40412);
    if (!RWCHUNKGROUPGLOBAL) {
        return NULL;
    }
    chunkGroupModule.numInstances++;
    return instance;
}

/* Chunk group module close. */
void *fn_80482C50(void *instance) {
    if (RWCHUNKGROUPGLOBAL) {
        fn_8047FCDC(RWCHUNKGROUPGLOBAL);
    }
    chunkGroupModule.numInstances--;
    return instance;
}

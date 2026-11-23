/* RenderWare core: the heap of the resource arena (0x80482984-0x80482BC4).
 * Initialise, a stub, free and first-fit allocate over 32-byte block headers; the resource arena
 * (the file before the stream module) is its only user. No data.
 * Boundary: between the vector module (its own constant pool, ends at 0x80482984) and the chunk
 * group module (its own globals from 0x80482BC4). C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

/* Heap initialise: the first block header starts on the 32-byte boundary after the heap's two
 * words; fails when less than one 32-byte block would remain. */
RwBool fn_80482984(RwHeap *heap, u32 size) {
    RwHeapBlock *block = (RwHeapBlock *)(((u32)heap + 0x27) & ~0x1F);
    s32 space = (((u32)heap + size) & ~0x1F) - (u32)block - sizeof(RwHeapBlock);

    if (space < 0x20) {
        return FALSE;
    }
    block->heap = heap;
    block->next = NULL;
    block->prev = NULL;
    block->used = 0;
    block->size = space;
    heap->firstBlock = block;
    heap->firstFree = block;
    return TRUE;
}

RwBool fn_804829D4(void) {
    return TRUE;
}

/* Heap free: the block is merged with a free neighbour on either side. */
void fn_804829DC(void *mem) {
    RwHeapBlock *block = (RwHeapBlock *)((u8 *)mem - sizeof(RwHeapBlock));
    RwHeap *heap;
    RwHeapBlock *prev;
    RwHeapBlock *next;

    block->used = 0;
    heap = block->heap;
    prev = block->prev;
    next = block->next;
    if (!heap->firstFree || block < heap->firstFree) {
        heap->firstFree = block;
    }
    if (prev && (~prev->used & 1)) {
        prev->next = next;
        if (next) {
            next->prev = prev;
        }
        prev->size += block->size + sizeof(RwHeapBlock);
        block = prev;
    }
    if (next && (~next->used & 1)) {
        block->next = next->next;
        if (next->next) {
            next->next->prev = block;
        }
        block->size += next->size + sizeof(RwHeapBlock);
    }
}

/* Heap allocate: first fit from the first free block, splitting a block that is more than a
 * header and 32 bytes too large. */
void *fn_80482A94(RwHeap *heap, u32 size) {
    RwHeapBlock *block = heap->firstFree;
    RwHeapBlock *found = NULL;

    size = (size + 0x1F) & ~0x1F;
    while (block && !found) {
        if ((~block->used & 1) && block->size >= size) {
            found = block;
        }
        block = block->next;
    }
    if (!found) {
        return NULL;
    }
    if (found->size > size + 0x40) {
        RwHeapBlock *next = found->next;
        RwHeapBlock *split;
        split = (RwHeapBlock *)((u32)found + size + sizeof(RwHeapBlock));
        if (next && (~next->used & 1)) {
            split->next = next->next;
            split->size = found->next->size + (found->size - size);
        } else {
            split->next = next;
            split->size = found->size - size - sizeof(RwHeapBlock);
        }
        found->next = split;
        split->used = 0;
        split->prev = found;
        if (split->next) {
            split->next->prev = split;
        }
        found->size = size;
        split->heap = found->heap;
    }
    if (found == heap->firstFree) {
        do {
            heap->firstFree = heap->firstFree->next;
        } while (heap->firstFree && (heap->firstFree->used & 1));
    }
    found->used = 1;
    return (u8 *)found + sizeof(RwHeapBlock);
}

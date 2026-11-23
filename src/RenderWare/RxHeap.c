/* RenderWare PowerPipe: the pipelines' heap (RxHeap), 0x8049EA2C..0x8049F2A4.
 * The game keeps RxHeapFree, _rxHeapReset, RxHeapDestroy and RxHeapCreate; the linker dropped the
 * rest of the file.  The file has no data of its own (it reaches the engine's memory functions
 * through RwEngineInstance).  Functions keep their fn_ symbols; comments give the public API name.
 * Built with cflags_rw_driver (C, -O4,p, no lmw/stmw, inlining of functions declared inline only,
 * in definition order): the static helpers below are expanded where they are used, two levels
 * deep in _rxHeapReset, while _rxHeapReset stays a call in RxHeapCreate.
 * Status: fn_8049ECFC, fn_8049F0CC and fn_8049F168 match; fn_8049EA2C (RxHeapFree) has retail's
 * instructions but its saved registers permuted (retail: free entry r31, header r30, heap r29). */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

/* Takes an entry of the free block table, growing the table by 32 entries when it is full. */
static inline RxHeapFreeEntry *HeapFreeEntryAlloc(RxHeap *heap) {
    u32 used = heap->entriesUsed;
    RxHeapFreeEntry *entries = heap->freeBlocks;

    if (heap->entriesAlloced <= used) {
        u32 alloced = heap->entriesAlloced + 32;
        heap->entriesAlloced = alloced;
        entries = (RxHeapFreeEntry *)RwEngineInstance->memRealloc(
            entries, alloced * sizeof(RxHeapFreeEntry), 0x1030409);
        if (entries == NULL) {
            RWERROR(1, (0x80000013, heap->entriesAlloced * sizeof(RxHeapFreeEntry)));
            heap->entriesAlloced -= 32;
        } else {
            if (entries != heap->freeBlocks && used != 0) {
                RxHeapFreeEntry *entry = entries;
                do {
                    entry->ptr->freeEntry = entry;
                    entry++;
                } while (--used != 0);
            }
            heap->freeBlocks = entries;
        }
    }
    if (entries != NULL) {
        entries += heap->entriesUsed++;
    }
    return entries;
}

/* RxHeapFree: returns a block, merging it with free neighbours. */
void fn_8049EA2C(RxHeap *heap, void *block) {
    RxHeapBlockHeader *hdr = (RxHeapBlockHeader *)block - 1;
    s32 prevFree = (hdr->prev != NULL && hdr->prev->freeEntry != NULL);
    s32 nextFree = (hdr->next != NULL && hdr->next->freeEntry != NULL);

    if (prevFree) {
        if (nextFree) {
            RxHeapFreeEntry *entry = hdr->next->freeEntry;
            if (&heap->freeBlocks[heap->entriesUsed - 1] != entry) {
                *entry = heap->freeBlocks[heap->entriesUsed - 1];
                entry->ptr->freeEntry = entry;
            }
            heap->entriesUsed--;
            hdr->prev->size += hdr->size + hdr->next->size + 2 * sizeof(RxHeapBlockHeader);
            hdr->prev->freeEntry->size = hdr->prev->size;
            hdr->prev->next = hdr->next->next;
            if (hdr->next->next != NULL) hdr->next->next->prev = hdr->prev;
        } else {
            hdr->prev->size += hdr->size + sizeof(RxHeapBlockHeader);
            hdr->prev->freeEntry->size = hdr->prev->size;
            hdr->prev->next = hdr->next;
            if (hdr->next != NULL) hdr->next->prev = hdr->prev;
        }
    } else if (nextFree) {
        hdr->size += hdr->next->size + sizeof(RxHeapBlockHeader);
        hdr->freeEntry = hdr->next->freeEntry;
        hdr->next->freeEntry->ptr = hdr;
        hdr->next->freeEntry->size = hdr->size;
        hdr->next = hdr->next->next;
        if (hdr->next != NULL) hdr->next->prev = hdr;
    } else {
        RxHeapFreeEntry *entry = HeapFreeEntryAlloc(heap);
        if (entry != NULL) {
            entry->ptr = hdr;
            entry->size = hdr->size;
            hdr->freeEntry = entry;
        }
    }
}

/* Lays a super block out as one free block between two empty end markers and links it after
 * the previous super block's end marker. */
static inline s32 HeapSuperBlockInit(RxHeap *heap, RxHeapSuperBlock *sb, RxHeapSuperBlock *prev) {
    RxHeapFreeEntry *entry = HeapFreeEntryAlloc(heap);

    if (entry != NULL) {
        RxHeapBlockHeader *first = (RxHeapBlockHeader *)sb->start;
        RxHeapBlockHeader *block = first + 1;
        RxHeapBlockHeader *last = (RxHeapBlockHeader *)((u8 *)first + sb->size) - 1;

        first->prev = NULL;
        first->next = NULL;
        first->size = 0;
        first->freeEntry = NULL;
        *last = *first;
        first->next = block;
        block->prev = first;
        block->next = last;
        last->prev = block;
        block->size = (u8 *)last - (u8 *)(block + 1);
        block->freeEntry = entry;
        entry->ptr = block;
        entry->size = block->size;
        if (prev != NULL) {
            RxHeapBlockHeader *prevLast = (RxHeapBlockHeader *)((u8 *)prev->start + prev->size) - 1;
            prevLast->next = first;
            first->prev = prevLast;
        }
        return TRUE;
    }
    return FALSE;
}

/* _rxHeapReset */
s32 fn_8049ECFC(RxHeap *heap) {
    RxHeapSuperBlock *prev;
    RxHeapSuperBlock *sb;

    heap->entriesUsed = 0;
    prev = NULL;
    sb = heap->head->next;
    while (sb != NULL) {
        if (!HeapSuperBlockInit(heap, sb, prev)) return FALSE;
        if (prev == NULL) heap->headBlock = (RxHeapBlockHeader *)sb->start;
        prev = sb;
        sb = sb->next;
    }
    sb = heap->head;
    if (!HeapSuperBlockInit(heap, sb, prev)) return FALSE;
    if (prev == NULL) heap->headBlock = (RxHeapBlockHeader *)sb->start;
    heap->dirty = FALSE;
    return TRUE;
}

static inline void HeapSuperBlockDestroy(RxHeapSuperBlock *sb) {
    if (sb != NULL) {
        RwEngineInstance->memFree(sb);
    }
}

/* RxHeapDestroy */
void fn_8049F0CC(RxHeap *heap) {
    if (heap != NULL) {
        RxHeapSuperBlock *sb;
        if (heap->freeBlocks != NULL) {
            RwEngineInstance->memFree(heap->freeBlocks);
            heap->freeBlocks = NULL;
        }
        sb = heap->head;
        while (sb != NULL) {
            RxHeapSuperBlock *next = sb->next;
            HeapSuperBlockDestroy(sb);
            sb = next;
        }
        RwEngineInstance->memFree(heap);
    }
}

/* A super block: one allocation holding the descriptor and 128-byte aligned block space. */
static inline RxHeapSuperBlock *HeapSuperBlockCreate(u32 size) {
    RxHeapSuperBlock *sb = (RxHeapSuperBlock *)RwEngineInstance->memAlloc(
        size + sizeof(RxHeapSuperBlock) + 127, 0x1040409);
    if (sb != NULL) {
        sb->start = (void *)(((u32)(sb + 1) + 127) & ~127);
        sb->size = size;
        sb->next = NULL;
    }
    return sb;
}

/* RxHeapCreate */
RxHeap *fn_8049F168(u32 size) {
    RxHeap *heap;
    RxHeapSuperBlock *sb;

    if (size < 1024) size = 1024;
    heap = (RxHeap *)RwEngineInstance->memAlloc(sizeof(RxHeap), 0x40409);
    if (heap != NULL) {
        size = (size + 31) & ~31;
        if (size < 128) size = 128;
        sb = HeapSuperBlockCreate(size);
        if (sb != NULL) {
            heap->superBlockSize = size;
            heap->head = sb;
            heap->freeBlocks = NULL;
            heap->entriesAlloced = 0;
            heap->entriesUsed = 0;
            heap->dirty = TRUE;
            if (!heap->dirty ? TRUE : fn_8049ECFC(heap)) return heap;
            HeapSuperBlockDestroy(sb);
        }
        RwEngineInstance->memFree(heap);
    }
    return NULL;
}

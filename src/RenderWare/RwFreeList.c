/* RenderWare free lists (RwFreeList*) and the memory functions, 0x8047FA28..0x8048053C: create,
 * preallocate, destroy, allocate and free entries, the iteration over used entries, the default
 * memory functions, the memory module's open and close (the free list of free lists and the list of
 * all free lists) and RwOsGetMemoryInterface.
 *
 * One source file: its .sdata is the switch that lets free lists preallocate (0x805EDEA0), its
 * .sbss the free list module's state (0x805F1568..0x805F1578: whether it is open, the free list of
 * free lists and the list of all of them, which the create function and the memory open both use)
 * and its .bss the free list of free lists itself (0x805E2A80). Retail inlines the block release
 * (fn_8047FC24) into RwFreeListDestroy and that into the memory close.
 *
 * A block is an R5Link, a bitmap of heapSize bytes (bit 7 of byte 0 is entry 0) and the entries,
 * aligned to the free list's alignment.
 *
 * NonMatching: fn_8047FC24, and fn_8047FCDC and fn_8048038C, which inline it: retail recomputes
 * the block list's address in the release loop where GC/2.7 keeps it in a saved register (every
 * loop shape and compiler version tried, probes/RenderWare_RwFreeList*.py); fn_8047FDB0,
 * fn_8047FF50 and fn_804800DC: same instructions, other register allocation (retail keeps the
 * block search's variables in saved registers that the new block reuses).
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * C.
 * Flags: GC/2.7 -O4,p, no stmw/lmw, automatic inlining, -lang=c, writable pooled strings
 * (cflags_rw_r5). Measured per file against the other RenderWare flag sets: GC 2.0-2.7 give the
 * same code, GC 1.3.2, -O4,s, stmw/lmw and -inline noauto match fewer functions, and the
 * read-only string sets (cflags_rw_r3/r6/c) would move strings out of .data/.sdata. */
#include "Game/rw/r5.h"

void *memset(void *dst, int c, u32 n);
void *memcpy(void *dst, const void *src, u32 n);
void *fn_803A2DC4(u32 size);                /* malloc */
void fn_803A2D54(void *mem);                /* free */
void *fn_803A2CC8(void *mem, u32 size);     /* realloc */
void *fn_803A2C40(u32 num, u32 size);       /* calloc */

/* Whether free lists may preallocate blocks. */
RwBool lbl_805EDEA0 = 1;

/* The free list module's state. */
s32 lbl_805F1568;            /* open */
R5FreeList *lbl_805F156C;    /* the free list the free lists come from */
R5Link lbl_805F1570;         /* the list of all free lists */

/* The free list of free lists. */
R5FreeList lbl_805E2A80;

/* The first entry of a block. */
#define BLOCKENTRIES(block, heapSize, alignment) \
    (((u32)(block) + (heapSize) + sizeof(R5Link) + (alignment) - 1) & ~((alignment) - 1))

/* Lets free lists preallocate their blocks, or not. */
void fn_8047FA28(RwBool enable) {
    lbl_805EDEA0 = enable;
}

void fn_8047FC24(R5FreeList *freeList);

/* Sets a free list up (in `inPlace` when given, else in a new one) and preallocates its blocks. */
R5FreeList *fn_8047FA30(u32 entrySize, u32 entriesPerBlock, u32 alignment, u32 preallocBlocks,
                        R5FreeList *inPlace, u32 hint) {

    R5FreeList *freeList;
    u32 heapSize;
    u32 blockSize;

    if (!lbl_805EDEA0) {
        preallocBlocks = 0;
    }
    if (alignment == 0) {
        alignment = 32;
    }
    if (inPlace == 0) {
        if (lbl_805F156C != 0) {
            inPlace = (R5FreeList *)RWSRCGLOBAL(memoryAlloc)(lbl_805F156C, hint & 0xFF0000);
        } else {
            inPlace = (R5FreeList *)RWSRCGLOBAL(rwmalloc)(sizeof(R5FreeList), hint & 0xFF0000);
        }
        if (inPlace == 0) {
            return 0;
        }
        inPlace->flags = 2;
    } else {
        inPlace->flags = 3;
    }
    freeList = inPlace;
    {
        u32 size = (entrySize + (alignment - 1)) & ~(alignment - 1);

        heapSize = (entriesPerBlock + 7) >> 3;
        freeList->entrySize = size;
        freeList->entriesPerBlock = entriesPerBlock;
        freeList->alignment = alignment;
        freeList->heapSize = heapSize;
        freeList->blockList.next = &freeList->blockList;
        freeList->blockList.prev = &freeList->blockList;
        blockSize = sizeof(R5Link) + heapSize + entriesPerBlock * size + alignment - 1;
    }
    while (preallocBlocks != 0) {
        R5Link *block = (R5Link *)RWSRCGLOBAL(rwmalloc)(blockSize, hint);

        if (block == 0) {
            fn_8047FC24(freeList);
            return 0;
        }
        block->prev = 0;
        block->next = 0;
        block->next = freeList->blockList.next;
        block->prev = &freeList->blockList;
        freeList->blockList.next->prev = block;
        freeList->blockList.next = block;
        memset(block + 1, 0, heapSize);
        preallocBlocks--;
    }
    freeList->link.next = lbl_805F1570.next;
    freeList->link.prev = &lbl_805F1570;
    lbl_805F1570.next->prev = &freeList->link;
    lbl_805F1570.next = &freeList->link;
    return freeList;
}


/* RwFreeListCreate. */
R5FreeList *fn_8047FBD8(u32 entrySize, u32 entriesPerBlock, u32 alignment, u32 hint) {
    return fn_8047FA30(entrySize, entriesPerBlock, alignment, 1, 0, hint);
}

/* RwFreeListCreateAndPreallocateSpace. */
R5FreeList *fn_8047FC04(u32 entrySize, u32 entriesPerBlock, u32 alignment, u32 preallocBlocks,
                        R5FreeList *inPlace, u32 hint) {
    return fn_8047FA30(entrySize, entriesPerBlock, alignment, preallocBlocks, inPlace, hint);
}

/* Frees a free list's blocks and the list itself. */
void fn_8047FC24(R5FreeList *freeList) {
    R5Link *block;

    for (block = freeList->blockList.next; block != &freeList->blockList; block = freeList->blockList.next) {
        block->prev->next = block->next;
        block->next->prev = block->prev;
        RWSRCGLOBAL(rwfree)(block);
    }
    if (!(freeList->flags & 1)) {
        if (lbl_805F156C == freeList || lbl_805F156C == 0) {
            RWSRCGLOBAL(rwfree)(freeList);
        } else {
            RWSRCGLOBAL(memoryFree)(lbl_805F156C, freeList);
        }
    }
}

/* RwFreeListDestroy. */
RwBool fn_8047FCDC(R5FreeList *freeList) {
    freeList->link.prev->next = freeList->link.next;
    freeList->link.next->prev = freeList->link.prev;
    fn_8047FC24(freeList);
    return 1;
}

/* RwFreeListAlloc: the first free entry of the blocks, else the first of a new block. */
void *fn_8047FDB0(R5FreeList *freeList, u32 hint) {
    u32 heapSize = freeList->heapSize;
    R5Link *block = freeList->blockList.next;
    void *entry = 0;

    while (block != &freeList->blockList && entry == 0) {
        u8 *heap = (u8 *)(block + 1);
        u32 left = freeList->entriesPerBlock;
        u32 index = 0;
        u32 i;

        for (i = 0; i < heapSize; i++, heap++, index += 8) {
            u8 bits = *heap;

            if (bits != 0xFF) {
                u32 bit;

                for (bit = 0; bit < 8 && left != 0; bit++, left--) {
                    u8 mask = 0x80 >> bit;

                    if (!(bits & mask)) {
                        *heap = mask | bits;
                        entry = (void *)(BLOCKENTRIES(block, heapSize, freeList->alignment) +
                                         freeList->entrySize * (index + bit));
                        break;
                    }
                }
            } else {
                left -= 8;
            }
            if (entry != 0) {
                break;
            }
        }
        block = block->next;
    }
    if (entry == 0) {
        u8 *heap;

        block = (R5Link *)RWSRCGLOBAL(rwmalloc)(
            heapSize + freeList->entriesPerBlock * freeList->entrySize + freeList->alignment + sizeof(R5Link) - 1,
            hint);
        if (block == 0) {
            return 0;
        }
        heap = (u8 *)(block + 1);
        memset(heap, 0, heapSize);
        block->next = freeList->blockList.next;
        block->prev = &freeList->blockList;
        freeList->blockList.next->prev = block;
        freeList->blockList.next = block;
        *heap = 0x80;
        entry = (void *)BLOCKENTRIES(block, heapSize, freeList->alignment);
    }
    return entry;
}

/* RwFreeListFree: clears the entry's bit; a block left empty is freed when the list says so. */
R5FreeList *fn_8047FF50(R5FreeList *freeList, void *entry) {
    u32 heapSize = freeList->heapSize;
    R5Link *block;

    for (block = freeList->blockList.next; block != &freeList->blockList; block = block->next) {
        u8 *first = (u8 *)block + heapSize + sizeof(R5Link);

        if ((u8 *)entry >= first && (u8 *)entry <= first + freeList->entriesPerBlock * freeList->entrySize) {
            u32 index = ((u8 *)entry - first) / freeList->entrySize;
            u8 *heap = (u8 *)(block + 1);

            heap[index >> 3] &= ~(u8)(0x80 >> (index & 7));
            if (freeList->flags & 2) {
                u32 used = 0;
                u32 i;

                for (i = 0; i < heapSize; i++) {
                    used += heap[i];
                }
                if (used == 0) {
                    block->prev->next = block->next;
                    block->next->prev = block->prev;
                    RWSRCGLOBAL(rwfree)(block);
                }
            }
            return freeList;
        }
    }
    return 0;
}

/* RwFreeListForAllUsed: calls `callBack` for every used entry, from a copy of each block's bitmap
 * (the callback may free entries). */
R5FreeList *fn_804800DC(R5FreeList *freeList, R5FreeListCallBack callBack, void *data) {
    u32 heapSize = freeList->heapSize;
    R5Link *block = freeList->blockList.next;

    while (block != &freeList->blockList) {
        u8 *heap = (u8 *)(block + 1);
        u8 *copy = (u8 *)RWSRCGLOBAL(rwmalloc)(heapSize, 0x10000);
        R5Link *next;
        u32 index;
        u32 i;

        if (copy == 0) {
            return 0;
        }
        memcpy(copy, heap, heapSize);
        next = block->next;
        heap = copy;
        index = 0;
        for (i = 0; i < heapSize; i++, heap++, index += 8) {
            u8 bits = *heap;

            if (bits != 0) {
                u32 n = index;
                u32 bit;

                for (bit = 0; bit < 8; bit++, n++) {
                    if (bits & (u8)(0x80 >> bit)) {
                        callBack((void *)(BLOCKENTRIES(block, heapSize, freeList->alignment) + freeList->entrySize * n),
                                 data);
                    }
                }
            }
        }
        RWSRCGLOBAL(rwfree)(copy);
        block = next;
    }
    return freeList;
}

/* The default memory functions (the hint is not used). */
void *fn_80480214(u32 size, u32 hint) {
    return fn_803A2DC4(size);
}

void *fn_80480234(void *mem, u32 size, u32 hint) {
    return fn_803A2CC8(mem, size);
}

void *fn_80480254(u32 num, u32 size, u32 hint) {
    return fn_803A2C40(num, size);
}

/* Opens the free list module: the list of all free lists and the free list of free lists. */
static RwBool FreeListOpen(void) {
    lbl_805F1570.next = &lbl_805F1570;
    lbl_805F1570.prev = &lbl_805F1570;
    lbl_805F1568 = 1;
    lbl_805F156C = fn_8047FA30(sizeof(R5FreeList), 16, 32, 0, &lbl_805E2A80, 0x40000);
    if (lbl_805F156C == 0) {
        lbl_805F1568 = 0;
        return 0;
    }
    lbl_805F156C->link.prev->next = lbl_805F156C->link.next;
    lbl_805F156C->link.next->prev = lbl_805F156C->link.prev;
    return 1;
}

/* Opens the memory module: the free lists, then the memory functions (the caller's, else the
 * defaults). */
RwBool fn_80480274(const R5MemoryFunctions *functions) {
    if (!FreeListOpen()) {
        return 0;
    }
    if (functions != 0) {
        *(R5MemoryFunctions *)&RWSRCGLOBAL(rwmalloc) = *functions;
    } else {
        RWSRCGLOBAL(rwmalloc) = fn_80480214;
        RWSRCGLOBAL(rwfree) = fn_803A2D54;
        RWSRCGLOBAL(rwrealloc) = fn_80480234;
        RWSRCGLOBAL(rwcalloc) = fn_80480254;
    }
    return 1;
}

/* Closes the memory module: destroys every free list, then the free list of free lists. */
void fn_8048038C(void) {
    while (lbl_805F1570.next != &lbl_805F1570) {
        fn_8047FCDC((R5FreeList *)((u8 *)lbl_805F1570.next - 0x1C));
    }
    fn_8047FCDC(lbl_805F156C);
    lbl_805F156C = 0;
    lbl_805F1568 = 0;
}

/* RwOsGetMemoryInterface: the memory functions (RwEngineInstance + 0x108). */
R5MemoryFunctions *fn_80480530(void) {
    return (R5MemoryFunctions *)&RWSRCGLOBAL(rwmalloc);
}

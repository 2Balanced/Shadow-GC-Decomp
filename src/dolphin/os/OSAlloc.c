/* Dolphin SDK OS: the heap allocator. OSInitAlloc puts an array of heap descriptors at the start
 * of a range; each heap keeps a free list (address-ordered, coalesced on free) and an allocated
 * list of cells, each with a 32-byte header. OSAllocFixed, OSDestroyHeap, OSAddToHeap and the
 * debug walkers are unused by the game and were dead-stripped. */
#include <dolphin/os.h>

#define HEADERSIZE 32u
#define MINOBJSIZE 64u

typedef struct Cell Cell;
typedef struct HeapDesc HeapDesc;

struct Cell {
    Cell *prev;
    Cell *next;
    s32 size; /* including the header */
};

struct HeapDesc {
    s32 size; /* negative while the descriptor is unused */
    Cell *free;
    Cell *allocated;
};

volatile int __OSCurrHeap = -1;

static HeapDesc *HeapArray;
static int NumHeaps;
static void *ArenaStart;
static void *ArenaEnd;

static Cell *DLAddFront(Cell *list, Cell *cell) {
    cell->next = list;
    cell->prev = 0;
    if (list) {
        list->prev = cell;
    }
    return cell;
}

static Cell *DLExtract(Cell *list, Cell *cell) {
    if (cell->next) {
        cell->next->prev = cell->prev;
    }
    if (cell->prev == NULL) {
        return cell->next;
    }
    cell->prev->next = cell->next;
    return list;
}

/* Inserts `cell` in address order, merging it with an adjacent neighbour on either side. */
static Cell *DLInsert(Cell *list, Cell *cell) {
    Cell *prev;
    Cell *next;

    for (next = list, prev = NULL; next != 0; prev = next, next = next->next) {
        if (cell <= next) {
            break;
        }
    }

    cell->next = next;
    cell->prev = prev;
    if (next) {
        next->prev = cell;
        if ((u8 *)cell + cell->size == (u8 *)next) {
            cell->size += next->size;
            next = next->next;
            cell->next = next;
            if (next) {
                next->prev = cell;
            }
        }
    }
    if (prev) {
        prev->next = cell;
        if ((u8 *)prev + prev->size == (u8 *)cell) {
            prev->size += cell->size;
            prev->next = next;
            if (next) {
                next->prev = prev;
            }
        }
        return list;
    }
    return cell;
}

/* First fit; the remainder of the chosen cell is split off unless it would be too small. */
void *OSAllocFromHeap(int heap, u32 size) {
    HeapDesc *hd;
    Cell *cell;
    Cell *newCell;
    s32 leftoverSize;

    hd = &HeapArray[heap];
    size += HEADERSIZE;
    size = (size + 0x1F) & 0xFFFFFFE0;

    for (cell = hd->free; cell != NULL; cell = cell->next) {
        if ((signed)size <= (signed)cell->size) {
            break;
        }
    }

    if (cell == NULL) {
        return NULL;
    }

    leftoverSize = cell->size - size;
    if (leftoverSize < MINOBJSIZE) {
        hd->free = DLExtract(hd->free, cell);
    } else {
        cell->size = size;
        newCell = (void *)((u8 *)cell + size);
        newCell->size = leftoverSize;
        newCell->prev = cell->prev;
        newCell->next = cell->next;
        if (newCell->next != NULL) {
            newCell->next->prev = newCell;
        }
        if (newCell->prev != NULL) {
            newCell->prev->next = newCell;
        } else {
            hd->free = newCell;
        }
    }

    hd->allocated = DLAddFront(hd->allocated, cell);
    return (u8 *)cell + HEADERSIZE;
}

void OSFreeToHeap(int heap, void *ptr) {
    HeapDesc *hd;
    Cell *cell;

    cell = (void *)((u32)ptr - HEADERSIZE);
    hd = &HeapArray[heap];
    hd->allocated = DLExtract(hd->allocated, cell);
    hd->free = DLInsert(hd->free, cell);
}

int OSSetCurrentHeap(int heap) {
    int prev;

    prev = __OSCurrHeap;
    __OSCurrHeap = heap;
    return prev;
}

void *OSInitAlloc(void *arenaStart, void *arenaEnd, int maxHeaps) {
    u32 arraySize;
    int i;
    HeapDesc *hd;

    arraySize = maxHeaps * sizeof(HeapDesc);
    HeapArray = arenaStart;
    NumHeaps = maxHeaps;

    for (i = 0; i < NumHeaps; i++) {
        hd = &HeapArray[i];
        hd->size = -1;
        hd->free = hd->allocated = 0;
    }
    __OSCurrHeap = -1;
    arenaStart = (void *)((u32)((char *)HeapArray + arraySize));
    arenaStart = (void *)(((u32)arenaStart + 0x1F) & 0xFFFFFFE0);
    ArenaStart = arenaStart;
    ArenaEnd = (void *)((u32)arenaEnd & 0xFFFFFFE0);
    return arenaStart;
}

/* Takes the first unused descriptor for [start, end), rounded inwards to 32 bytes. */
int OSCreateHeap(void *start, void *end) {
    int heap;
    HeapDesc *hd;
    Cell *cell;

    start = (void *)(((u32)start + 0x1FU) & ~((32) - 1));
    end = (void *)(((u32)end) & ~((32) - 1));

    for (heap = 0; heap < NumHeaps; heap++) {
        hd = &HeapArray[heap];
        if (hd->size < 0) {
            hd->size = (u32)end - (u32)start;
            cell = start;
            cell->prev = 0;
            cell->next = 0;
            cell->size = hd->size;
            hd->free = cell;
            hd->allocated = 0;
            return heap;
        }
    }
    return -1;
}

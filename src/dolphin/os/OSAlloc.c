/* Dolphin SDK OS: the multi-heap allocator.
 *
 * OSInitAlloc carves a table of heap descriptors off the front of the arena it is given; each
 * OSCreateHeap claims one descriptor for a 32-byte aligned range. A heap tracks two lists of
 * blocks: free blocks in address order (neighbours are merged when a block is returned) and
 * allocated blocks in no particular order. Every block starts with a 32-byte header. OSAllocFixed,
 * OSDestroyHeap, OSAddToHeap and the debug walkers are not referenced by the game and were
 * dead-stripped. */
#include <dolphin/os.h>

#define BLOCK_HEADER 32u   /* header in front of every block, keeps payloads 32-byte aligned */
#define BLOCK_MIN 64u      /* smallest leftover worth splitting off as its own free block */
#define ALIGN32_MASK 0xFFFFFFE0
#define ROUND_UP_32(x) (((x) + 0x1F) & ALIGN32_MASK)

typedef struct Block {
    struct Block *back;
    struct Block *fwd;
    s32 bytes; /* whole block, header included */
} Block;

typedef struct Heap {
    s32 bytes; /* < 0 marks a descriptor nobody has claimed */
    Block *freeList;
    Block *usedList;
} Heap;

volatile int __OSCurrHeap = -1;

static Heap *HeapArray;
static int NumHeaps;
static void *ArenaStart;
static void *ArenaEnd;

/* Pushes `b` on the front of `head`; returns the new head. */
static Block *DLAddFront(Block *head, Block *b) {
    b->fwd = head;
    b->back = NULL;
    if (head != NULL) {
        head->back = b;
    }
    return b;
}

/* Unlinks `b` from `head`; returns the (possibly new) head. */
static Block *DLExtract(Block *head, Block *b) {
    if (b->fwd != NULL) {
        b->fwd->back = b->back;
    }
    if (b->back == NULL) {
        /* `b` was the head. */
        return b->fwd;
    }
    b->back->fwd = b->fwd;
    return head;
}

/* Links `b` into the address-ordered list `head`, absorbing whichever neighbours touch it. */
static Block *DLInsert(Block *head, Block *b) {
    Block *lo;
    Block *hi;

    lo = NULL;
    for (hi = head; hi != NULL; hi = hi->fwd) {
        if (b <= hi) {
            break;
        }
        lo = hi;
    }

    b->fwd = hi;
    b->back = lo;

    if (hi != NULL) {
        hi->back = b;
        /* Touches the following block: swallow it. */
        if ((u8 *)b + b->bytes == (u8 *)hi) {
            b->bytes += hi->bytes;
            hi = hi->fwd;
            b->fwd = hi;
            if (hi != NULL) {
                hi->back = b;
            }
        }
    }

    if (lo != NULL) {
        lo->fwd = b;
        /* Touches the preceding block: let it swallow us. */
        if ((u8 *)lo + lo->bytes == (u8 *)b) {
            lo->bytes += b->bytes;
            lo->fwd = hi;
            if (hi != NULL) {
                hi->back = lo;
            }
        }
        return head;
    }
    return b; /* new lowest block */
}

/* First fit over the free list. When enough is left over, the tail of the block becomes a new
 * free block in its place; otherwise the whole block is handed out. */
void *OSAllocFromHeap(int heap, u32 size) {
    Heap *h = &HeapArray[heap];
    Block *b;
    Block *tail;
    s32 spare;

    size = ROUND_UP_32(size + BLOCK_HEADER);

    for (b = h->freeList; b != NULL; b = b->fwd) {
        if ((s32)size <= b->bytes) {
            break;
        }
    }
    if (b == NULL) {
        return NULL;
    }

    spare = b->bytes - size;
    if (spare < BLOCK_MIN) {
        h->freeList = DLExtract(h->freeList, b);
    } else {
        b->bytes = size;
        tail = (Block *)((u8 *)b + size);
        tail->bytes = spare;
        tail->back = b->back;
        tail->fwd = b->fwd;
        if (tail->fwd != NULL) {
            tail->fwd->back = tail;
        }
        if (tail->back != NULL) {
            tail->back->fwd = tail;
        } else {
            h->freeList = tail;
        }
    }

    h->usedList = DLAddFront(h->usedList, b);
    return (u8 *)b + BLOCK_HEADER;
}

void OSFreeToHeap(int heap, void *p) {
    Block *b = (Block *)((u32)p - BLOCK_HEADER);
    Heap *h = &HeapArray[heap];

    h->usedList = DLExtract(h->usedList, b);
    h->freeList = DLInsert(h->freeList, b);
}

int OSSetCurrentHeap(int heap) {
    int old = __OSCurrHeap;

    __OSCurrHeap = heap;
    return old;
}

/* Places `maxHeaps` empty descriptors at `lo`; returns where the usable arena now begins. */
void *OSInitAlloc(void *lo, void *hi, int maxHeaps) {
    u32 tableBytes = sizeof(Heap) * maxHeaps;
    int n;
    Heap *h;

    HeapArray = lo;
    NumHeaps = maxHeaps;
    for (n = 0; n < NumHeaps; n++) {
        h = HeapArray + n;
        h->bytes = -1;
        h->freeList = h->usedList = NULL;
    }
    __OSCurrHeap = -1;

    lo = (u8 *)HeapArray + tableBytes;
    lo = (void *)ROUND_UP_32((u32)lo);
    ArenaStart = lo;
    ArenaEnd = (void *)((u32)hi & ALIGN32_MASK);
    return lo;
}

/* Claims the first unused descriptor for [lo, hi) shrunk inwards to 32-byte boundaries; the
 * whole range starts out as a single free block. Returns -1 when the table is full. */
int OSCreateHeap(void *lo, void *hi) {
    Heap *h;
    Block *b;
    int n;

    lo = (void *)ROUND_UP_32((u32)lo);
    hi = (void *)((u32)hi & ALIGN32_MASK);

    for (n = 0; n < NumHeaps; n++) {
        h = &HeapArray[n];
        if (h->bytes >= 0) {
            continue;
        }
        h->bytes = (u32)hi - (u32)lo;
        b = lo;
        b->back = NULL;
        b->fwd = NULL;
        b->bytes = h->bytes;
        h->freeList = b;
        h->usedList = NULL;
        return n;
    }
    return -1;
}

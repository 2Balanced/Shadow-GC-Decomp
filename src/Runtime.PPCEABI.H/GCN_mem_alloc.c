/* The system allocator underneath the MSL heap (alloc.c): blocks come from the Dolphin OS's
 * current heap. If the program never created one, a default heap covering the whole arena is
 * made on first use. */

typedef unsigned long u32;

extern volatile int __OSCurrHeap;

void OSReport(const char *msg, ...);
void *OSGetArenaLo(void);
void *OSGetArenaHi(void);
void OSSetArenaLo(void *addr);
void *OSInitAlloc(void *arenaStart, void *arenaEnd, int maxHeaps);
int OSCreateHeap(void *start, void *end);
int OSSetCurrentHeap(int heap);
void *OSAllocFromHeap(int heap, u32 size);
void OSFreeToHeap(int heap, void *ptr);

static inline void InitDefaultHeap(void) {
    void *lo;
    void *hi;

    OSReport("GCN_Mem_Alloc.c : InitDefaultHeap. No Heap Available\n");
    OSReport("Metrowerks CW runtime library initializing default heap\n");

    lo = OSGetArenaLo();
    hi = OSGetArenaHi();

    /* The allocator's heap table goes at the bottom of the arena. */
    lo = OSInitAlloc(lo, hi, 1);
    OSSetArenaLo(lo);

    /* Heaps are 32-byte aligned. */
    lo = (void *)(((u32)lo + 0x1F) & ~0x1F);
    hi = (void *)((u32)hi & ~0x1F);

    OSSetCurrentHeap(OSCreateHeap(lo, hi));
    OSSetArenaLo(lo = hi);
}

void __sys_free(void *ptr) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }
    OSFreeToHeap(__OSCurrHeap, ptr);
}

void *__sys_alloc(u32 size) {
    if (__OSCurrHeap == -1) {
        InitDefaultHeap();
    }
    return OSAllocFromHeap(__OSCurrHeap, size);
}

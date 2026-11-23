/* AX SDLIB, the game's GameCube AX sound library (see include/Game/rw/r1.h), 0x8043AA7C..0x8043B25C:
 * ARAM transfer requests, the library's first-fit heap and loading sound banks into memory and ARAM.
 *
 * The file name is retail's: the panic messages pass "gcaxSndMemory.c" and lines 174 and 194. The
 * library is C (that name), built with the game's flags, so with deferred inlining its functions
 * come out in reverse definition order; with line 174 inside the third function from the start,
 * the functions after it fit into the first 170 lines only if the file ends here, where the
 * library's float constants (in the next file's .sdata2) start being used.
 * Function and member names are inferred from behavior. */
#include <dolphin/types.h>
#include "size_assert.h"
#include "Game/rw/r1.h"

void fn_803836F4(void *, u32, int, int, void *, int, u32, void (*)(void *));
void fn_8043AC4C(GcaxArqRequest *entry);
void fn_8043ACE8(void *param);
void *fn_8043AD94(u32 size);
u32 fn_8043AA7C(void *object, int index, u32 unused, void *a, void *b, int wait);

static inline u32 FindFreeEntry(void) {
    u32 i;
    for (i = 0; i < 64; i++) {
        if (lbl_805F1404->m_requestBusy[i] == 0) {
            return i;
        }
    }
    OSReport("AX SDLIB: Panic: no more at %s:%s\n", "gcaxSndMemory.c", 194);
    while (1) {
    }
}

static inline GcaxHeapNode *FindPrevious(GcaxHeapNode *node) {
    GcaxHeapNode *current = lbl_805F1404->m_heap;
    GcaxHeapNode *previous = NULL;
    while (current->m_next != NULL) {
        if (current == node) {
            return previous;
        }
        previous = current;
        current = current->m_next;
    }
    return NULL;
}

static inline int GetFreeSize(void) {
    int remaining = 0;
    u32 interrupt = OSDisableInterrupts();
    if (lbl_805F1404 != NULL && lbl_805F1404->m_heap != NULL) {
        GcaxHeapNode *block = lbl_805F1404->m_heap;
        while (block->m_next != NULL) {
            if (block->m_magic == GCAX_HEAP_FREE) {
                remaining += (u8 *)block->m_next - (u8 *)block - 0x20;
            }
            block = block->m_next;
        }
    }
    OSRestoreInterrupts(interrupt);
    return remaining;
}

static inline void InitHeapBlocks(GcaxHeapNode *first, u32 size) {
    GcaxHeapNode *last = (GcaxHeapNode *)((u8 *)first + size) - 1;
    first->m_magic = GCAX_HEAP_FREE;
    first->m_flags = 0;
    first->m_next = last;
    last->m_magic = GCAX_HEAP_BUSY;
    last->m_flags = 0;
    last->m_next = NULL;
}

int fn_8043B1AC(int bank, int size) {
    int result = -1;
    size = (size + 0x1F) & 0xFFFFFFE0;
    if (bank >= 0 && bank < 16 && lbl_805F1404->m_aramFree >= size) {
        result = 0;
        lbl_805F1404->m_aramBanks[bank].m_address = lbl_805F1404->m_aramNext;
        lbl_805F1404->m_aramBanks[bank].m_size = size;
        lbl_805F1404->m_aramNext += size;
        lbl_805F1404->m_aramFree -= size;
    }
    if (result != 0) {
        OSReport("AX SDLIB: gcaxNewAramBank(%d,%d) failed (should not happen\n", bank, size);
    }
    return result;
}

int fn_8043B05C(int bank, void *file, GcaxCallback callback, void *data, int wait) {
    int result;
    GcaxChunk *program;
    GcaxChunk *wave = (GcaxChunk *)((u8 *)file + 0x10);
    if (wave->m_magic != 0x67636178 || wave->m_type != 0x4D504257) {
        return -1;
    }
    if (lbl_805F1404->m_aramBanks[bank].m_size < wave->m_size) {
        return -2;
    }
    if (callback == (GcaxCallback)-1) {
        wait = 0;
        callback = NULL;
    }
    result = fn_8043AA7C(wave->m_data, lbl_805F1404->m_aramBanks[bank].m_address, wave->m_size,
                         callback, data, wait);
    program = (GcaxChunk *)&wave->m_data[wave->m_size];
    if (program->m_magic != 0x67636178 || program->m_type != 0x4D504250) {
        return -3;
    }
    if (lbl_805F1404->m_programBanks[bank].m_data != NULL) {
        fn_8043ACE8(lbl_805F1404->m_programBanks[bank].m_data);
    }
    lbl_805F1404->m_programBanks[bank].m_data = fn_8043AD94(program->m_size);
    if (lbl_805F1404->m_programBanks[bank].m_data == NULL) {
        return -4;
    }
    lbl_805F1404->m_programBanks[bank].m_size = program->m_size;
    memcpy(lbl_805F1404->m_programBanks[bank].m_data, program->m_data, program->m_size);
    return result;
}

int fn_8043AF64(int bank, GcaxChunk *chunk, GcaxCallback callback, void *data, int wait) {
    if (chunk->m_magic != 0x67636178 || chunk->m_type != 0x4D534220) {
        return -1;
    }
    if (callback == (GcaxCallback)-1) {
        wait = 0;
        callback = NULL;
    }
    if (lbl_805F1404->m_sequenceBanks[bank].m_data != NULL) {
        fn_8043ACE8(lbl_805F1404->m_sequenceBanks[bank].m_data);
    }
    lbl_805F1404->m_sequenceBanks[bank].m_data = fn_8043AD94(chunk->m_size);
    if (lbl_805F1404->m_sequenceBanks[bank].m_data == NULL) {
        return -2;
    }
    lbl_805F1404->m_sequenceBanks[bank].m_size = chunk->m_size;
    memcpy(lbl_805F1404->m_sequenceBanks[bank].m_data, chunk->m_data, chunk->m_size);
    if (wait != 0) {
        callback(data);
    }
    return 0;
}

void fn_8043AEBC(u32 base, int size) {
    u32 interrupt;
    u32 inner;
    interrupt = OSDisableInterrupts();
    lbl_805F1404->m_heap = (GcaxHeapNode *)((base + 0x1F) & 0xFFFFFFE0);
    size -= (0x20 - (base & 0x1F)) & 0x1F;
    lbl_805F1404->m_heapSize = size & 0xFFFFFFFC;
    inner = OSDisableInterrupts();
    InitHeapBlocks(lbl_805F1404->m_heap, lbl_805F1404->m_heapSize);
    OSRestoreInterrupts(inner);
    OSRestoreInterrupts(interrupt);
}

void *fn_8043AD94(u32 size) {
    GcaxHeapNode *current = lbl_805F1404->m_heap;
    GcaxHeapNode *next;
    u32 interrupt = OSDisableInterrupts();
    u32 aligned = (size + 0x1F) & 0xFFFFFFE0;
    u32 available;
    while ((next = current->m_next) != NULL) {
        if (current->m_magic == GCAX_HEAP_FREE) {
            available = (u8 *)next - (u8 *)current - 0x20;
            if (available >= aligned) {
                goto found;
            }
        }
        current = next;
    }
    OSReport("AX SDLIB: gacxSndMalloc(%d) failed (should not happen, rest=%d\n", size,
             GetFreeSize());
    OSRestoreInterrupts(interrupt);
    return NULL;
found:
    if (available > aligned + 0x20) {
        GcaxHeapNode *split = (GcaxHeapNode *)((u8 *)(current + 1) + aligned);
        split->m_magic = GCAX_HEAP_FREE;
        split->m_flags = 0;
        split->m_next = next;
        current->m_next = split;
    }
    current->m_magic = GCAX_HEAP_BUSY;
    OSRestoreInterrupts(interrupt);
    return current + 1;
}

void fn_8043ACE8(void *param) {
    u32 interrupt;
    GcaxHeapNode *node;
    GcaxHeapNode *next;
    GcaxHeapNode *previous;
    interrupt = OSDisableInterrupts();
    node = (GcaxHeapNode *)((u8 *)param - 0x20);
    node->m_magic = GCAX_HEAP_FREE;
    next = node->m_next;
    if (next != NULL && next->m_magic == GCAX_HEAP_FREE) {
        node->m_next = next->m_next;
    }
    previous = FindPrevious(node);
    if (previous != NULL && previous->m_magic == GCAX_HEAP_FREE) {
        previous->m_next = node->m_next;
    }
    OSRestoreInterrupts(interrupt);
}

void fn_8043AC4C(GcaxArqRequest *entry) {
    u32 i;
    for (i = 0; i < 64; i++) {
        if (&lbl_805F1404->m_requests[i] == entry) {
            goto found;
        }
    }
    OSReport("AX SDLIB: Panic: arq not found at %s:%s\n", "gcaxSndMemory.c", 174);
    while (1) {
    }
found:
    lbl_805F1404->m_requestBusy[i] = 0;
    if (lbl_805F1404->m_requestCallbacks[i] != NULL) {
        lbl_805F1404->m_requestCallbacks[i](lbl_805F1404->m_requestData[i]);
    }
}

void fn_8043AB6C(void *object, int index, u32 unused, void *a, void *b) {
    u32 i;
    for (i = 0; i < 64; i++) {
        if (lbl_805F1404->m_requestBusy[i] == 0) {
            goto found;
        }
    }
    OSReport("AX SDLIB: Panic: no more at %s:%s\n", "gcaxSndMemory.c", 194);
    while (1) {
    }
found:
    lbl_805F1404->m_requestBusy[i] = 1;
    lbl_805F1404->m_requestCallbacks[i] = (void (*)(void *))a;
    lbl_805F1404->m_requestData[i] = b;
    fn_803836F4(&lbl_805F1404->m_requests[i], lbl_805F1404->m_arqOwner, 0, 1, object, index, unused,
                (void (*)(void *))fn_8043AC4C);
    while (lbl_805F1404->m_requestBusy[i] != 0) {
    }
}

u32 fn_8043AA7C(void *object, int index, u32 unused, void *a, void *b, int wait) {
    u32 i = FindFreeEntry();
    volatile u8 *used;
    lbl_805F1404->m_requestBusy[i] = 1;
    lbl_805F1404->m_requestCallbacks[i] = (void (*)(void *))a;
    lbl_805F1404->m_requestData[i] = b;
    fn_803836F4(&lbl_805F1404->m_requests[i], lbl_805F1404->m_arqOwner, 0, 1, object, index, unused,
                (void (*)(void *))fn_8043AC4C);
    used = &lbl_805F1404->m_requestBusy[i];
    if (wait == 0) {
        while (*used != 0) {
        }
    }
    return i;
}

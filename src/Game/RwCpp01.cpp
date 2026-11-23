/* AX SDLIB, the game's GameCube AX sound library (see include/Game/rw/r1.h), 0x8043AA7C..0x8043B810:
 * ARAM transfer requests, the library's first-fit heap, sound bank loading and the singly linked
 * list helpers. Function and member names are inferred from behavior. Definitions are in reverse
 * address order because deferred inlining emits the last one first. All 19 functions match. */
#include <string.h>
#include <dolphin/types.h>
#include "size_assert.h"
#include "Game/rw/r1.h"

extern GcaxGlobals *lbl_805F1404;
extern const char lbl_8051BB28[];
extern const char lbl_8051BB4C[];
extern const char lbl_8051BB5C[];
extern const char lbl_8051BB88[]; // "AX SDLIB: gacxSndMalloc(%d) failed (should not happen, rest=%d" + newline
extern const char lbl_8051BBC8[]; // "AX SDLIB: gcaxNewAramBank(%d,%d) failed (should not happen" + newline
extern const float lbl_805FAE48;
extern const float lbl_805FAE4C;
extern const float lbl_805FAE50;
extern const float lbl_805FAE54;
extern const float lbl_805FAE58;
extern const float lbl_805FAE5C;
extern const float lbl_805FAE60;
extern const float lbl_805FAE64;
extern const float lbl_805FAE68;
extern float lbl_8056B090[];

extern "C" {
void OSReport(const char *, ...);
u32 OSDisableInterrupts(void);
void OSRestoreInterrupts(u32);
void fn_803836F4(void *, u32, int, int, void *, int, u32, void (*)(void *));
void fn_8043BB54(void *);
void fn_8043BD80(void *);
void fn_8043BE04(void *);
void fn_8043AC4C(GcaxArqRequest *entry);
void fn_8043ACE8(void *param);
void *fn_8043AD94(u32 size);
u32 fn_8043AA7C(void *object, int index, u32 unused, void *a, void *b, int wait);
GcaxListNode *fn_8043B3DC(GcaxListNode **list, GcaxListNode *node);

static inline u32 FindFreeEntry() {
    u32 i;
    for (i = 0; i < 64; i++) {
        if (lbl_805F1404->m_requestBusy[i] == 0) {
            return i;
        }
    }
    OSReport(lbl_8051BB28, lbl_8051BB4C, 194);
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

static inline int GetFreeSize() {
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

void fn_8043B794(GcaxListNode **list) {
    GcaxListNode *outer = NULL;
    while ((outer = fn_8043B3DC(list, outer)) != NULL) {
        GcaxGroup *group = (GcaxGroup *)outer->m_item;
        if (group != NULL) {
            GcaxListNode *inner = NULL;
            while ((inner = fn_8043B3DC(&group->m_voices, inner)) != NULL) {
                if (inner->m_item != NULL) {
                    fn_8043BD80(inner->m_item);
                }
            }
        }
    }
}

void fn_8043B6D0(GcaxListNode **list) {
    GcaxListNode *outer = NULL;
    while ((outer = fn_8043B3DC(list, outer)) != NULL) {
        GcaxGroup *group = (GcaxGroup *)outer->m_item;
        if (group != NULL) {
            GcaxListNode *inner = NULL;
            while ((inner = fn_8043B3DC(&group->m_voices, inner)) != NULL) {
                GcaxVoice *voice = (GcaxVoice *)inner->m_item;
                if (voice != NULL) {
                    if (voice->m_state >= 4) {
                        voice->m_level = lbl_805FAE64;
                        voice->m_decibels = lbl_805FAE68;
                        voice->m_state = 5;
                    } else {
                        fn_8043BE04(voice);
                    }
                }
            }
        }
    }
}

void fn_8043B5FC(GcaxListNode **list) {
    int count;
    GcaxListNode *outer;
    GcaxGroup *group;
restart:
    outer = NULL;
    while ((outer = fn_8043B3DC(list, outer)) != NULL) {
        group = (GcaxGroup *)outer->m_item;
        count = 0;
        if (group != NULL) {
            GcaxListNode *inner = NULL;
            while ((inner = fn_8043B3DC(&group->m_voices, inner)) != NULL) {
                GcaxVoice *voice = (GcaxVoice *)inner->m_item;
                if (voice != NULL) {
                    count++;
                    if ((voice->m_flags & 0x80000003) == 0x80000002) {
                        if (voice->field_0x34->field_0x146 == 0 ||
                            (voice->m_state == 5 && voice->m_level < lbl_805FAE60)) {
                            count--;
                        }
                    }
                }
            }
        }
        if (count == 0) {
            fn_8043BB54(group);
            goto restart;
        }
    }
}

void *fn_8043B560(u32 size, GcaxListNode **list, void (*callback)(void *)) {
    GcaxListNode *node = *list;
    GcaxListNode *memory = (GcaxListNode *)fn_8043AD94(size);
    if (memory == NULL) {
        return NULL;
    }
    memset(memory, 0, size);
    if (*list == NULL) {
        *list = memory;
    } else {
        while (node->m_next != NULL) {
            node = node->m_next;
        }
        node->m_next = memory;
    }
    if (callback != NULL) {
        callback(memory);
    }
    return memory;
}

void fn_8043B4B0(void *target, void **list, void (*callback)(void *)) {
    void *next;
    void *current = *list;
    if (current == target) {
        next = *(void **)current;
        if (callback != NULL) {
            callback(current);
        }
        fn_8043ACE8(current);
        *list = next;
        return;
    }
    goto previous_check;
    for (;;) {
        next = *(void **)current;
        if (next == NULL) {
            return;
        }
        if (next == target) {
            *(void **)current = *(void **)next;
            if (callback != NULL) {
                callback(next);
            }
            fn_8043ACE8(next);
            return;
        }
        current = next;
previous_check:
        if (current == NULL) {
            return;
        }
    }
}

void fn_8043B44C(GcaxListNode **list, void (*callback)(GcaxListNode *)) {
    GcaxListNode *next;
    GcaxListNode *node;
    while ((node = *list) != NULL) {
        next = node->m_next;
        if (callback != NULL) {
            callback(node);
        }
        fn_8043ACE8(node);
        *list = next;
    }
}

/* Singly linked list helpers: a list is a pointer to its first node, a node starts with its
 * link. */
GcaxListNode *fn_8043B3F4(GcaxListNode **list, int (*predicate)(GcaxListNode *)) {
    GcaxListNode *node = *list;
    while (node != NULL) {
        if (predicate(node) == 0) {
            break;
        }
        node = node->m_next;
    }
    return node;
}

GcaxListNode *fn_8043B3DC(GcaxListNode **list, GcaxListNode *node) {
    if (node == NULL) {
        return *list;
    }
    return node->m_next;
}

float fn_8043B3A0(float value) {
    for (int i = 0; i < 1025; i++) {
        if (lbl_8056B090[i * 2 + 1] > value) {
            return lbl_8056B090[i * 2];
        }
    }
    return lbl_805FAE48;
}

void fn_8043B25C(void) {
    lbl_805F1404->m_config[0] = 0xE1000;
    lbl_805F1404->m_config[1] = 0x7D000;
    lbl_805F1404->m_config[2] = 0x145000;

    lbl_805F1404->m_config[3] = 0x4B000;
    lbl_805F1404->m_config[4] = 0x57800;
    lbl_805F1404->m_config[5] = 0x23000;
    lbl_805F1404->m_config[6] = 0xC8000;
    lbl_805F1404->m_config[7] = 0xFA000;
    lbl_805F1404->m_config[8] = 0x200;
    lbl_805F1404->m_config[9] = 0x200;
    lbl_805F1404->m_config[10] = 0x200;
    lbl_805F1404->m_config[11] = 0x80;
    lbl_805F1404->m_config[12] = 0x80;
    lbl_805F1404->m_config[13] = 0x80;
    lbl_805F1404->m_config[14] = 0x80;
    lbl_805F1404->m_config[15] = 0x80;
    lbl_805F1404->m_reverbHi.m_time = lbl_805FAE48;
    lbl_805F1404->m_reverbHi.m_preDelay = lbl_805FAE4C;
    lbl_805F1404->m_reverbHi.m_damping = lbl_805FAE50;
    lbl_805F1404->m_reverbHi.m_coloration = lbl_805FAE54;
    lbl_805F1404->m_reverbHi.m_crosstalk = lbl_805FAE58;
    lbl_805F1404->m_reverbHi.m_mix = lbl_805FAE48;
    lbl_805F1404->field_0x13A44 = lbl_805FAE5C;
    lbl_805F1404->field_0x13A38 = 0;
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
        OSReport(lbl_8051BBC8, bank, size);
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
    u32 interrupt = OSDisableInterrupts();
    lbl_805F1404->m_heap = (GcaxHeapNode *)((base + 0x1F) & 0xFFFFFFE0);
    size -= (0x20 - (base & 0x1F)) & 0x1F;
    lbl_805F1404->m_heapSize = size & 0xFFFFFFFC;
    u32 inner = OSDisableInterrupts();
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
    OSReport(lbl_8051BB88, size, GetFreeSize());
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
    u32 interrupt = OSDisableInterrupts();
    GcaxHeapNode *node = (GcaxHeapNode *)((u8 *)param - 0x20);
    node->m_magic = GCAX_HEAP_FREE;
    GcaxHeapNode *next = node->m_next;
    if (next != NULL && next->m_magic == GCAX_HEAP_FREE) {
        node->m_next = next->m_next;
    }
    GcaxHeapNode *previous = FindPrevious(node);
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
    OSReport(lbl_8051BB5C, lbl_8051BB4C, 174);
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
    OSReport(lbl_8051BB28, lbl_8051BB4C, 194);
    while (1) {
    }
found:
    lbl_805F1404->m_requestBusy[i] = 1;
    lbl_805F1404->m_requestCallbacks[i] = (void (*)(void *))a;
    lbl_805F1404->m_requestData[i] = b;
    fn_803836F4(&lbl_805F1404->m_requests[i], lbl_805F1404->m_arqOwner, 0, 1, object, index, unused, (void (*)(void *))fn_8043AC4C);
    while (lbl_805F1404->m_requestBusy[i] != 0) {
    }
}

u32 fn_8043AA7C(void *object, int index, u32 unused, void *a, void *b, int wait) {
    u32 i = FindFreeEntry();
    lbl_805F1404->m_requestBusy[i] = 1;
    lbl_805F1404->m_requestCallbacks[i] = (void (*)(void *))a;
    lbl_805F1404->m_requestData[i] = b;
    fn_803836F4(&lbl_805F1404->m_requests[i], lbl_805F1404->m_arqOwner, 0, 1, object, index, unused, (void (*)(void *))fn_8043AC4C);
    volatile u8 *used = &lbl_805F1404->m_requestBusy[i];
    if (wait == 0) {
        while (*used != 0) {
        }
    }
    return i;
}
}

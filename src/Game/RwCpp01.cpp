/* RenderWare's 64-entry object pool. Function and member names are inferred from behavior.
 * Definitions are in reverse address order because deferred inlining emits the last one first. */
#include <string.h>
#include <dolphin/types.h>
#include "size_assert.h"

struct RwCpp01Entry {
    /* 0x00 */ u8 data[0x20];
};

struct RwCpp01Node {
    /* 0x0 */ u16 m_magic;
    /* 0x2 */ u16 m_flags;
    /* 0x4 */ RwCpp01Node *m_next;
};

struct RwCpp01Pool {
    /* 0x0000 */ u8 field_0x0[0x4854];
    /* 0x4854 */ void *m_list;
    /* 0x4858 */ u32 field_0x4858;
    /* 0x485C */ RwCpp01Entry m_entries[64];
    /* 0x505C */ u8 m_used[64];
    /* 0x509C */ u32 field_0x509C;
    /* 0x50A0 */ void (*m_callbacks[64])(void *);
    /* 0x51A0 */ void *m_data[64];
};
SIZE_ASSERT(RwCpp01Pool, 0x52A0);

extern RwCpp01Pool *lbl_805F1404;
extern const char lbl_8051BB28[];
extern const char lbl_8051BB4C[];
extern const char lbl_8051BB5C[];
extern float lbl_805FAE48;
extern float lbl_805FAE4C;
extern float lbl_805FAE50;
extern float lbl_805FAE54;
extern float lbl_805FAE58;
extern float lbl_805FAE5C;
extern float lbl_805FAE60;
extern float lbl_805FAE64;
extern float lbl_805FAE68;
extern float lbl_8056B090[];

extern "C" {
void OSReport(const char *, ...);
u32 OSDisableInterrupts(void);
void OSRestoreInterrupts(u32);
void fn_803836F4(void *, u32, int, int, void *, int, u32, void (*)(void *));
void fn_8043BB54(void *);
void fn_8043BD80(void *);
void fn_8043BE04(void *);

void fn_8043AC4C(RwCpp01Entry *entry) {
    u32 i;
    for (i = 0; i < 64; i++) {
        if (&lbl_805F1404->m_entries[i] == entry) {
            goto found;
        }
    }
    OSReport(lbl_8051BB5C, lbl_8051BB4C, 174);
    while (1) {
    }
found:
    lbl_805F1404->m_used[i] = 0;
    if (lbl_805F1404->m_callbacks[i] != NULL) {
        lbl_805F1404->m_callbacks[i](lbl_805F1404->m_data[i]);
    }
}

void fn_8043AB6C(void *object, int index, u32 unused, void *a, void *b) {
    u32 i;
    for (i = 0; i < 64; i++) {
        if (lbl_805F1404->m_used[i] == 0) {
            goto found;
        }
    }
    OSReport(lbl_8051BB28, lbl_8051BB4C, 194);
    while (1) {
    }
found:
    lbl_805F1404->m_used[i] = 1;
    lbl_805F1404->m_callbacks[i] = (void (*)(void *))a;
    lbl_805F1404->m_data[i] = b;
    fn_803836F4(&lbl_805F1404->m_entries[i], lbl_805F1404->field_0x509C, 0, 1, object, index, unused, (void (*)(void *))fn_8043AC4C);
    u8 *used = &lbl_805F1404->m_used[i];
    while (*used != 0) {
    }
}

u32 fn_8043AA7C(void *object, int index, u32 unused, void *a, void *b, int wait) {
    u32 i;
    for (i = 0; i < 64; i++) {
        if (lbl_805F1404->m_used[i] == 0) {
            goto found;
        }
    }
    OSReport(lbl_8051BB28, lbl_8051BB4C, 194);
    while (1) {
    }
found:
    lbl_805F1404->m_used[i] = 1;
    lbl_805F1404->m_callbacks[i] = (void (*)(void *))a;
    lbl_805F1404->m_data[i] = b;
    fn_803836F4(&lbl_805F1404->m_entries[i], lbl_805F1404->field_0x509C, 0, 1, object, index, unused, (void (*)(void *))fn_8043AC4C);
    u8 *used = &lbl_805F1404->m_used[i];
    if (wait == 0) {
        while (*used != 0) {
        }
    }
    return i;
}

void fn_8043ACE8(void *param) {
    RwCpp01Node *node = (RwCpp01Node *)((u8 *)param - 0x20);
    OSDisableInterrupts();
    node->m_magic = 0x4D46;
    RwCpp01Node *next = node->m_next;
    if (next != NULL && next->m_magic == 0x4D46) {
        node->m_next = next->m_next;
    }
    RwCpp01Node *current = (RwCpp01Node *)lbl_805F1404->m_list;
    RwCpp01Node *previous = NULL;
    do {
        previous = current;
        current = current->m_next;
        if (current == NULL) {
            previous = NULL;
            break;
        }
    } while (current != node);
    if (previous != NULL && previous->m_magic == 0x4D46) {
        previous->m_next = node->m_next;
    }
    OSRestoreInterrupts(0);
}

void *fn_8043AD94(u32 size) {
    RwCpp01Node *current = (RwCpp01Node *)lbl_805F1404->m_list;
    u32 interrupt = OSDisableInterrupts();
    u32 aligned = (size + 0x1F) & 0xFFFFFFE0;
    while (current != NULL) {
        RwCpp01Node *next = current->m_next;
        if (next == NULL) {
            int remaining = 0;
            OSDisableInterrupts();
            if (lbl_805F1404 != NULL && lbl_805F1404->m_list != NULL) {
                RwCpp01Node *block = (RwCpp01Node *)lbl_805F1404->m_list;
                while (block->m_next != NULL) {
                    if (block->m_magic == 0x4D46) {
                        remaining += (u8 *)block->m_next - (u8 *)block - 0x20;
                    }
                    block = block->m_next;
                }
            }
            OSRestoreInterrupts(0);
            OSReport("AX SDLIB: gacxSndMalloc(%d) failed (should not happen, rest=%d\n", size, remaining);
            OSRestoreInterrupts(interrupt);
            return NULL;
        }
        u32 available = (u8 *)next - (u8 *)current - 0x20;
        if (current->m_magic == 0x4D46 && available >= aligned) {
            break;
        }
        current = next;
    }
    RwCpp01Node *next = current->m_next;
    u32 available = (u8 *)next - (u8 *)current - 0x20;
    if (available > aligned + 0x20) {
        RwCpp01Node *split = (RwCpp01Node *)((u8 *)current + aligned + 0x20);
        split->m_magic = 0x4D46;
        split->m_flags = 0;
        split->m_next = next;
        current->m_next = split;
    }
    current->m_magic = 0x4D42;
    OSRestoreInterrupts(interrupt);
    return (u8 *)current + 0x20;
}

void fn_8043AEBC(u32 base, int size) {
    u32 interrupt = OSDisableInterrupts();
    lbl_805F1404->m_list = (void *)((base + 0x1F) & 0xFFFFFFE0);
    lbl_805F1404->field_0x4858 = (size - (0x20 - (base & 0x1F) & 0x1F)) & 0xFFFFFFFC;
    OSDisableInterrupts();
    int blockSize = lbl_805F1404->field_0x4858;
    RwCpp01Node *first = (RwCpp01Node *)lbl_805F1404->m_list;
    first->m_magic = 0x4D46;
    RwCpp01Node *last = (RwCpp01Node *)((u8 *)first + blockSize - 0x20);
    first->m_flags = 0;
    first->m_next = last;
    last->m_magic = 0x4D42;
    last->m_flags = 0;
    last->m_next = NULL;
    OSRestoreInterrupts(0);
    OSRestoreInterrupts(interrupt);
}

int fn_8043AF64(int bank, int *header, void (*callback)(void *), void *data, int wait) {
    if (header[0] != 0x67636178 || header[1] != 0x4D534220) {
        return -1;
    }
    if (callback == (void (*)(void *))-1) {
        wait = 0;
        callback = NULL;
    }
    bank *= 8;
    if (*(u32 *)((u8 *)lbl_805F1404 + bank + 0x53F8) != 0) {
        fn_8043ACE8(*(void **)((u8 *)lbl_805F1404 + bank + 0x53F8));
    }
    void *memory = fn_8043AD94(header[3]);
    *(void **)((u8 *)lbl_805F1404 + bank + 0x53F8) = memory;
    if (memory == NULL) {
        return -2;
    }
    *(int *)((u8 *)lbl_805F1404 + bank + 0x53FC) = header[3];
    memcpy(memory, header + 4, header[3]);
    if (wait != 0) {
        callback(data);
    }
    return 0;
}

int fn_8043B05C(int bank, int *header, void (*callback)(void *), void *data, int wait) {
    if (*(u32 *)((u8 *)header + 0x10) != 0x67636178 ||
        *(u32 *)((u8 *)header + 0x14) != 0x4D504257) {
        return -1;
    }
    bank *= 8;
    if (*(u32 *)((u8 *)lbl_805F1404 + bank + 0x52FC) < *(u32 *)((u8 *)header + 0x1C)) {
        return -2;
    }
    if (callback == (void (*)(void *))-1) {
        wait = 0;
        callback = NULL;
    }
    int result = fn_8043AA7C(header + 8, *(u32 *)((u8 *)lbl_805F1404 + bank + 0x52F8),
                             *(u32 *)((u8 *)header + 0x1C), callback, data, wait);
    int *wave = (int *)((u8 *)header + *(u32 *)((u8 *)header + 0x1C) + 0x20);
    if (wave[0] != 0x67636178 || wave[1] != 0x4D504250) {
        return -3;
    }
    if (*(u32 *)((u8 *)lbl_805F1404 + bank + 0x5378) != 0) {
        fn_8043ACE8(*(void **)((u8 *)lbl_805F1404 + bank + 0x5378));
    }
    void *memory = fn_8043AD94(wave[3]);
    *(void **)((u8 *)lbl_805F1404 + bank + 0x5378) = memory;
    if (memory == NULL) {
        return -4;
    }
    *(int *)((u8 *)lbl_805F1404 + bank + 0x537C) = wave[3];
    memcpy(memory, wave + 4, wave[3]);
    return result;
}

int fn_8043B1AC(int bank, u32 size) {
    u32 aligned = (size + 0x1F) & 0xFFFFFFE0;
    int result = -1;
    if (bank >= 0 && bank < 0x10 && aligned <= *(u32 *)((u8 *)lbl_805F1404 + 0x52A8)) {
        result = 0;
        *(u32 *)((u8 *)lbl_805F1404 + bank * 8 + 0x52F8) = *(u32 *)((u8 *)lbl_805F1404 + 0x52AC);
        *(u32 *)((u8 *)lbl_805F1404 + bank * 8 + 0x52FC) = aligned;
        *(u32 *)((u8 *)lbl_805F1404 + 0x52AC) += aligned;
        *(u32 *)((u8 *)lbl_805F1404 + 0x52A8) -= aligned;
    }
    if (result != 0) {
        OSReport("AX SDLIB: gcaxNewAramBank(%d,%d) failed (should not happen\n", bank);
    }
    return result;
}

void fn_8043B25C(void) {
    *(u32 *)((u8 *)lbl_805F1404 + 0x52B8) = 0xE1000;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52BC) = 0x7D000;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52C0) = 0x145000;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52C4) = 0x4B000;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52C8) = 0x57800;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52CC) = 0x23000;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52D0) = 0xC8000;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52D4) = 0xFA000;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52D8) = 0x200;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52DC) = 0x200;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52E0) = 0x200;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52E4) = 0x80;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52E8) = 0x80;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52EC) = 0x80;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52F0) = 0x80;
    *(u32 *)((u8 *)lbl_805F1404 + 0x52F4) = 0x80;
    *(float *)((u8 *)lbl_805F1404 + 0x10128) = lbl_805FAE48;
    *(float *)((u8 *)lbl_805F1404 + 0x10130) = lbl_805FAE4C;
    *(float *)((u8 *)lbl_805F1404 + 0x1012C) = lbl_805FAE50;
    *(float *)((u8 *)lbl_805F1404 + 0x10120) = lbl_805FAE54;
    *(float *)((u8 *)lbl_805F1404 + 0x10134) = lbl_805FAE58;
    *(float *)((u8 *)lbl_805F1404 + 0x10124) = lbl_805FAE48;
    *(float *)((u8 *)lbl_805F1404 + 0x13A44) = lbl_805FAE5C;
    *(u8 *)((u8 *)lbl_805F1404 + 0x13A38) = 0;
}

float fn_8043B3A0(float value) {
    for (int i = 0; i < 1025; i++) {
        if (lbl_8056B090[i * 2 + 1] > value) {
            return lbl_8056B090[i * 2];
        }
    }
    return lbl_805FAE48;
}

void *fn_8043B3DC(void *first, void *second) {
    if (second == NULL) {
        return *(void **)first;
    }
    return *(void **)second;
}

void **fn_8043B3F4(void **list, int (*predicate)(void *)) {
    list = (void **)*list;
    goto check;
    for (;;) {
        if (predicate(list) == 0) {
            break;
        }
        list = (void **)*list;
check:
        if (list == NULL) {
            break;
        }
    }
    return list;
}

void fn_8043B44C(void **list, void (*callback)(void *)) {
    void *next;
    while ((next = *list) != NULL) {
        void *current = next;
        if (callback != NULL) {
            callback(current);
        }
        fn_8043ACE8(current);
        *list = next;
    }
}

void fn_8043B4B0(void *target, void **list, void (*callback)(void *)) {
    void *next;
    void *current = *list;
    if (current == target) {
        next = current;
        if (callback != NULL) {
            callback(current);
        }
        fn_8043ACE8(current);
        *list = next;
        return;
    }
    void *previous;
    do {
        previous = current;
        if (previous == NULL) {
            return;
        }
        current = *(void **)previous;
        if (current == NULL) {
            return;
        }
    } while (current != target);
    *(void **)previous = *(void **)current;
    if (callback != NULL) {
        callback(current);
    }
    fn_8043ACE8(current);
}

void *fn_8043B560(u32 size, void **list, void (*callback)(void *)) {
    void *current = *list;
    void *memory = fn_8043AD94(size);
    if (memory == NULL) {
        return NULL;
    }
    memset(memory, 0, size);
    if (*list == NULL) {
        *list = memory;
    } else {
        void *next;
        do {
            next = current;
            current = *(void **)next;
        } while (current != NULL);
        *(void **)next = memory;
    }
    if (callback != NULL) {
        callback(memory);
    }
    return memory;
}

void fn_8043B5FC(void *param) {
    void *outer = NULL;
    goto outer_check;
    for (;;) {
        void *owner = *(void **)((u8 *)outer + 4);
        int count = 0;
        if (owner != NULL) {
            void *inner = NULL;
            int currentCount = 0;
            goto inner_check;
            for (;;) {
                count = currentCount;
                inner = fn_8043B3DC((u8 *)owner + 0x10, inner);
                if (inner != NULL) {
                    void **object = *(void ***)((u8 *)inner + 4);
                    currentCount = count;
                    if (object != NULL) {
                        currentCount = count + 1;
                        if (((u32)object[0] & 0x80000003) - 0x80000000 == 2) {
                            if (*(u16 *)((u8 *)object[0xD] + 0x146) != 0) {
                                if (*(u32 *)((u8 *)object + 0xD8) != 5) {
                                    continue;
                                }
                                if (*(float *)((u8 *)object + 0xAC) >= lbl_805FAE60) {
                                    continue;
                                }
                            }
                            currentCount = count;
                        }
                    }
                }
inner_check:
                if (inner == NULL) {
                    break;
                }
            }
        }
        if (count == 0) {
            fn_8043BB54(owner);
        }
        outer = fn_8043B3DC(param, outer);
outer_check:
        if (outer == NULL) {
            break;
        }
    }
}

void fn_8043B6D0(void *param) {
    void *outer = NULL;
    while ((outer = fn_8043B3DC(param, outer)) != NULL) {
        void *owner = *(void **)((u8 *)outer + 4);
        if (owner != NULL) {
            float first = lbl_805FAE64;
            float second = lbl_805FAE68;
            void *inner = NULL;
            while ((inner = fn_8043B3DC((u8 *)owner + 0x10, inner)) != NULL) {
                void *object = *(void **)((u8 *)inner + 4);
                if (object != NULL) {
                    if (*(int *)((u8 *)object + 0xD8) >= 4) {
                        *(float *)((u8 *)object + 0xAC) = first;
                        *(float *)((u8 *)object + 0xB0) = second;
                        *(u32 *)((u8 *)object + 0xD8) = 5;
                    } else {
                        fn_8043BE04(object);
                    }
                }
            }
        }
    }
}

void fn_8043B794(void *param) {
    void *outer = NULL;
    goto outer_check;
    for (;;) {
        void *owner = *(void **)((u8 *)outer + 4);
        if (owner != NULL) {
            void *inner = NULL;
            goto inner_check;
            for (;;) {
                if (*(u32 *)((u8 *)inner + 4) != 0) {
                    fn_8043BD80(*(void **)((u8 *)inner + 4));
                }
                inner = fn_8043B3DC((u8 *)owner + 0x10, inner);
inner_check:
                if (inner == NULL) {
                    break;
                }
            }
        }
        outer = fn_8043B3DC(param, outer);
outer_check:
        if (outer == NULL) {
            break;
        }
    }
}
}

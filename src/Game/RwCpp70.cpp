/* RenderWare container and resource helpers. Function and member names are inferred from behavior.
 * Definitions are in reverse address order because deferred inlining emits the last one first.
 * Remaining: register allocation in fn_804A8B08, fn_804A9274, and fn_804A9914.
 * Compiler profile: GC 2.7 -O4,p with use_lmw_stmw off. */
#include <string.h>
#include <dolphin/types.h>

struct RwCpp70Container {
    /* 0x00 */ u8 field_0x0[8];
    /* 0x08 */ void *m_items[10];
    /* 0x30 */ void *m_field_0x30;
};

struct RwCpp70Item {
    /* 0x00 */ u8 field_0x0[0x54];
    /* 0x54 */ int m_references;
};

struct RwCpp70Bytes {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ u8 m_data[];
};

struct RwCpp70IndexEntry {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 m_hasData;
    /* 0x8 */ u32 m_offset;
    /* 0xC */ u32 field_0xC;
};

struct RwCpp70InitEntry {
    /* 0x00 */ int m_index;
    /* 0x04 */ void *m_field_0x4;
    /* 0x08 */ void *m_field_0x8;
    /* 0x0C */ void *m_field_0xC;
    /* 0x10 */ void *m_field_0x10;
};

struct RwCpp70Callbacks {
    /* 0x00 */ u8 field_0x0[0xCC];
    /* 0xCC */ void (*m_cc)(void *, void *);
    /* 0xD0 */ void (*m_callback)(void *, void *, int, RwCpp70Callbacks *);
    /* 0xD4 */ u8 field_0xD4[4];
    /* 0xD8 */ void (*m_d8)(void *, void *, int, RwCpp70Callbacks *);
    /* 0xDC */ u8 field_0xDC[0xC];
    /* 0xE8 */ int (*m_e8)(void *, void *);
    /* 0xEC */ u8 field_0xEC[0x1C];
    /* 0x108 */ void *(*m_allocate)(u32, u32, RwCpp70Callbacks *);
    /* 0x10C */ void (*m_free)(void *, RwCpp70Callbacks *);
};

struct RwCpp70Node {
    /* 0x00 */ void *m_field_0x0;
    /* 0x04 */ u32 m_field_0x4;
    /* 0x08 */ void *m_field_0x8;
    /* 0x0C */ void *m_field_0xC;
    /* 0x10 */ u8 field_0x10[0x18];
    /* 0x28 */ RwCpp70Node *m_next;
    /* 0x2C */ RwCpp70Node *m_previous;
};

struct RwCpp70Counter {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ int m_count;
};

extern RwCpp70IndexEntry lbl_805E32D8[];
extern RwCpp70InitEntry lbl_805E3378[];
extern RwCpp70Callbacks *lbl_805F15B4;
extern int lbl_805EDF68;
extern void *lbl_805F16D0;

extern "C" {
void fn_8048CD8C(void *);
void *fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void fn_804A9674(RwCpp70Node *);
int fn_8045B2CC(int, void *, void (*)(void), void (*)(void), void (*)(void));
int fn_8045B30C(void *, void *, void *, void *);
int fn_80485D78(int, int, void (*)(void), void (*)(void));
void fn_804A871C(void);
void fn_804A8728(void);
void fn_804A87D8(void);
void *fn_804A8B08(void *, void *, void *, void *);
void fn_804A89C4(void);
void fn_804A8924(void);
void fn_804A869C(void);
void fn_804A86DC(void);
void *fn_80481368(int, int, void *);
int fn_8048124C(void *, int);
int fn_8047CF50(void *, int, void *, void *);
u32 fn_80480D4C(void *, void *, int);
void fn_8047D254(void *, int);
u32 fn_80483060(void *, void *);
u32 fn_8047D868(void *, void *, int);
u32 fn_804A0CDC(void *, void *);
void *fn_804A8FCC(RwCpp70Container *, void *);

void fn_804A9BE8(void *object) {
    ((RwCpp70Counter *)object)->m_count++;
}

void *fn_804A9BA4(void *object, void *arg) {
    lbl_805F15B4->m_callback((u8 *)object + 8, arg, 0x1F, lbl_805F15B4);
    return object;
}

void *fn_804A9914(void *param) {
    u32 local10;
    u32 localC;
    u32 index;
    struct {
        u8 data[0x18];
    } local14;
    u8 *base = (u8 *)lbl_805F15B4 + lbl_805EDF68;
    void *found = NULL;
    void *node;
    void *head = *(void **)(base + 8);
    if (head != NULL) {
        void *search = *(void **)head;
        while (search != head) {
            node = (u8 *)search - 0x28;
            if (lbl_805F15B4->m_e8((u8 *)node + 8, param) == 0) {
                goto firstFound;
            }
            search = *(void **)search;
        }
        node = NULL;
firstFound:
        found = node;
    } else {
        void *outer = *(void **)base;
        while (outer != base) {
            void *end = (u8 *)outer - 8;
            void *search = *(void **)end;
            while (search != end) {
                node = (u8 *)search - 0x28;
                if (lbl_805F15B4->m_e8((u8 *)node + 8, param) == 0) {
                    goto nestedFound;
                }
                search = *(void **)search;
            }
            node = NULL;
nestedFound:
            found = node;
            if (found != NULL) {
                break;
            }
            outer = *(void **)outer;
        }
    }
    if (found != NULL) {
        fn_804A9BE8(found);
        return found;
    }

    base = (u8 *)*(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 0x14);
    lbl_805F15B4->m_cc(base, *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 0x10));
    lbl_805F15B4->m_d8(base, param, 0x1F, lbl_805F15B4);
    base = (u8 *)fn_80481368(2, 1, base);
    if (base == NULL) {
        return NULL;
    }
    if (fn_8047CF50(base, 0x20, NULL, NULL) == 0) {
        fn_8048124C(base, 0);
        return NULL;
    }
    if (fn_8047CF50(base, 1, NULL, NULL) == 0 ||
        fn_80480D4C(base, &index, 4) == 0) {
        param = NULL;
        goto close;
    }
    fn_8047D254(&index, 4);
    RwCpp70InitEntry *entry = &lbl_805E3378[index];
    if (entry->m_field_0x8 == NULL) {
        param = NULL;
        goto close;
    }
    if (fn_80483060(&local14, base) == 0) {
        param = NULL;
        goto close;
    }
    if (fn_8047CF50(base, 3, &local10, &localC) == 0) {
        param = NULL;
        goto close;
    }
    param = ((void *(*)(void *, u32, u32, u32))entry->m_field_0x8)(
        base, index, localC, local10);
    if (param == NULL) {
        param = NULL;
        goto close;
    }
    fn_804A9BA4(param, &local14);
close:
    fn_8048124C(base, 0);
    return param;
}

RwCpp70Node *fn_804A9584(RwCpp70Node *node);

#define RWCPP70_DESTROY(node)                                                                  \
    do {                                                                                       \
        if ((int)(node)->m_field_0x0 != 0) {                                                      \
            RwCpp70InitEntry *entry = &lbl_805E3378[(u32)(node)->m_field_0x0];                  \
            if (entry->m_field_0x4 != NULL) {                                                   \
                ((void (*)(void *))entry->m_field_0x4)(node);                                   \
                break;                                                                          \
            }                                                                                  \
        }                                                                                      \
        lbl_805F15B4->m_free(node, lbl_805F15B4);                                                \
    } while (0)

void fn_804A9674(RwCpp70Node *node) {
    node->m_field_0x4--;
    if (node->m_field_0x4 != 0) {
        return;
    }
    if (node->m_next != NULL) {
        node->m_previous->m_field_0x0 = node->m_next;
        node->m_next->m_field_0x4 = (int)node->m_previous;
        node->m_field_0x4--;
        if (node->m_field_0x4 == 0) {
            if (node->m_next != NULL) {
                node->m_previous->m_field_0x0 = node->m_next;
                node->m_next->m_field_0x4 = (int)node->m_previous;
                node->m_field_0x4--;
                if (node->m_field_0x4 == 0) {
                    if (node->m_next != NULL) {
                        node->m_previous->m_field_0x0 = node->m_next;
                        node->m_next->m_field_0x4 = (int)node->m_previous;
                        node->m_field_0x4--;
                        if (node->m_field_0x4 == 0) {
                            if (node->m_next != NULL) {
                                node->m_previous->m_field_0x0 = node->m_next;
                                node->m_next->m_field_0x4 = (int)node->m_previous;
                                node->m_field_0x4--;
                                if (node->m_field_0x4 == 0) {
                                    fn_804A9584(node);
                                    RWCPP70_DESTROY(node);
                                }
                            }
                            RWCPP70_DESTROY(node);
                        }
                    }
                    RWCPP70_DESTROY(node);
                }
            }
            RWCPP70_DESTROY(node);
        }
    }
    RWCPP70_DESTROY(node);
}

RwCpp70Node *fn_804A95D4(void) {
    RwCpp70Node *node = (RwCpp70Node *)lbl_805F15B4->m_allocate(0x30, 0x3012C, lbl_805F15B4);
    if (node == NULL) {
        struct {
            u32 field_0x0;
            void *field_0x4;
        } failure;
        failure.field_0x0 = 0x120;
        failure.field_0x4 = fn_8047DC28(0x80000013, 0x30);
        fn_8047DB84(&failure);
        return NULL;
    }
    memset(node, 0, 0x30);
    node->m_field_0x0 = NULL;
    node->m_field_0x4 = 1;
    node->m_previous = NULL;
    node->m_next = NULL;
    return node;
}

RwCpp70Node *fn_804A9584(RwCpp70Node *node) {
    if (node->m_next != NULL) {
        node->m_previous->m_field_0x0 = node->m_next;
        node->m_next->m_field_0x4 = (int)node->m_previous;
        fn_804A9674(node);
    }
    return node;
}

void *fn_804A9500(void *list, RwCpp70Node *node) {
    if (node->m_next != NULL) {
        node->m_previous->m_field_0x0 = node->m_next;
        node->m_next->m_field_0x4 = (int)node->m_previous;
        fn_804A9674(node);
    }
    RwCpp70Node *head = (RwCpp70Node *)list;
    node->m_next = (RwCpp70Node *)head->m_field_0x0;
    node->m_previous = head;
    ((RwCpp70Node *)head->m_field_0x0)->m_field_0x4 = (int)(u8 *)node + 0x28;
    head->m_field_0x0 = (u8 *)node + 0x28;
    fn_804A9BE8(node);
    return list;
}

void fn_804A9468(void *object) {
    if (object == *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 8)) {
        *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 8) = NULL;
    }
    void *current = *(void **)object;
    while (current != object) {
        void *node = (u8 *)current - 0x28;
        current = *(void **)current;
        fn_804A9584((RwCpp70Node *)node);
    }
    void *field8 = *(void **)((u8 *)object + 8);
    void **fieldC = *(void ***)((u8 *)object + 0xC);
    *fieldC = field8;
    fieldC = *(void ***)((u8 *)object + 0xC);
    field8 = *(void **)((u8 *)object + 8);
    *(void **)((u8 *)field8 + 4) = fieldC;
    lbl_805F15B4->m_free(object, lbl_805F15B4);
}

void *fn_804A93B0(void) {
    void *object = lbl_805F15B4->m_allocate(0x10, 0x3012C, lbl_805F15B4);
    if (object == NULL) {
        struct {
            u32 field_0x0;
            void *field_0x4;
        } failure;
        failure.field_0x0 = 0x120;
        failure.field_0x4 = fn_8047DC28(0x80000013, 0x10);
        fn_8047DB84(&failure);
        return NULL;
    }
    *(void **)object = object;
    void *link = (u8 *)object + 8;
    *(void **)((u8 *)object + 4) = object;
    *(void **)((u8 *)object + 8) = *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68);
    *(void **)((u8 *)object + 0xC) = (u8 *)lbl_805F15B4 + lbl_805EDF68;
    void *head = *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68);
    *(void **)((u8 *)head + 4) = link;
    *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68) = link;
    return object;
}

void *fn_804A932C(void *object, int arg) {
    memset(object, 0, 0x30);
    *(void **)object = (void *)arg;
    *(void **)((u8 *)object + 4) = (void *)1;
    *(void **)((u8 *)object + 0x2C) = NULL;
    *(void **)((u8 *)object + 0x28) = NULL;
    if (arg != NULL) {
        void *list = *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 8);
        if (list != NULL) {
            fn_804A9500(list, (RwCpp70Node *)object);
        }
    }
    return object;
}

int fn_804A9274(void) {
    void *entry = *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 0x10);
    if (entry != NULL) {
        lbl_805F15B4->m_free(entry, lbl_805F15B4);
        *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 0x10) = NULL;
        *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 0x14) = NULL;
        *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 0xC) = NULL;
    }
    void *list = (u8 *)lbl_805F15B4 + lbl_805EDF68;
    void *target = lbl_805F16D0;
    void *current = *(void **)list;
    while (current != list) {
        if ((u8 *)current - 8 == target) {
            fn_804A9468(target);
            lbl_805F16D0 = NULL;
            break;
        }
        current = *(void **)current;
    }
    return 1;
}

int fn_804A9158(void) {
    *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68) = (u8 *)lbl_805F15B4 + lbl_805EDF68;
    *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 4) = (u8 *)lbl_805F15B4 + lbl_805EDF68;
    void *object = fn_804A93B0();
    lbl_805F16D0 = object;
    if (object == NULL) {
        return 0;
    }
    *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 8) = object;
    void *memory = lbl_805F15B4->m_allocate(0x220, 0x4012C, lbl_805F15B4);
    if (memory == NULL) {
        struct {
            u32 field_0x0;
            void *field_0x4;
        } failure;
        fn_804A9468(*(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 8));
        failure.field_0x0 = 0x120;
        failure.field_0x4 = fn_8047DC28(0x80000013, 0x220);
        fn_8047DB84(&failure);
        return 0;
    }
    memset(memory, 0, 0x220);
    *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 0x10) = memory;
    *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 0x14) = (u8 *)memory + 0x100;
    *(void **)((u8 *)lbl_805F15B4 + lbl_805EDF68 + 0xC) = (void *)0x100;
    return 1;
}

int fn_804A912C(int index, void *arg4, void *arg5, void *arg6, void *arg7) {
    RwCpp70InitEntry *entry = &lbl_805E3378[index];
    entry->m_index = index;
    entry->m_field_0x8 = arg4;
    entry->m_field_0xC = arg5;
    entry->m_field_0x10 = arg6;
    entry->m_field_0x4 = arg7;
    return 1;
}

int fn_804A90F8(void) {
    memset(lbl_805E3378, 0, 0xC8);
    return 1;
}

void *fn_804A90C8(void *object, int index) {
    RwCpp70IndexEntry *entry = &lbl_805E32D8[index];
    if (entry->m_hasData != 0) {
        return *(void **)((u8 *)object + entry->m_offset);
    }
    return NULL;
}

u8 fn_804A90BC(u8 *object, int index) {
    return object[index + 0x28];
}

u8 *fn_804A90B0(u8 *object, int index, u8 value) {
    object[index + 0x28] = value;
    return object;
}

void *fn_804A90A0(RwCpp70Container *container, int index) {
    return container->m_items[index];
}

RwCpp70Container *fn_804A9030(RwCpp70Container *container, int index, RwCpp70Item *item) {
    if (container->m_items[index] != NULL) {
        fn_8048CD8C(container->m_items[index]);
    }
    container->m_items[index] = item;
    if (item != NULL) {
        item->m_references++;
    }
    return container;
}

void *fn_804A9028(RwCpp70Container *container) {
    return container->m_field_0x30;
}

void *fn_804A8FCC(RwCpp70Container *container, void *item) {
    if (container->m_field_0x30 != NULL) {
        fn_804A9674((RwCpp70Node *)container->m_field_0x30);
    }
    container->m_field_0x30 = item;
    if (item != NULL) {
        fn_804A9BE8(container->m_field_0x30);
    }
    return container;
}

int fn_804A8EFC(int index, void *arg4, void *arg5) {
    int result = fn_8045B2CC(4, arg4, fn_804A871C, fn_804A8728, fn_804A87D8);
    if (result < 0) {
        return 0;
    }
    RwCpp70IndexEntry *entry = &lbl_805E32D8[index];
    entry->m_offset = result;
    result = fn_8045B30C(arg4, (void *)fn_804A8B08, (void *)fn_804A89C4, (void *)fn_804A8924);
    if (result < 0) {
        return 0;
    }
    entry->field_0x0 = index;
    entry->m_hasData = (u32)arg4;
    entry->field_0xC = (u32)arg5;
    return 1;
}

int fn_804A8E90(void) {
    if (fn_804A90F8() == 0) {
        return 0;
    }
    memset(lbl_805E32D8, 0, 0xA0);
    int result = fn_80485D78(0x18, 0x12C, fn_804A869C, fn_804A86DC);
    int success = (result < 0) ? 0 : 1;
    lbl_805EDF68 = result;
    return success;
}

#define RWCPP70_CONSTRUCT_FAIL(result)                                                          \
    do {                                                                                       \
        i = 0;                                                                                 \
        slot = result;                                                                         \
        for (; i < (result)[1]; slot++, i++) {                                                \
            if (slot[2] != 0) {                                                                \
                fn_8048CD8C((void *)slot[2]);                                                  \
                slot[2] = 0;                                                                   \
            }                                                                                  \
        }                                                                                      \
        if ((result)[0xC] != 0) {                                                              \
            fn_804A9674((RwCpp70Node *)(result)[0xC]);                                         \
            (result)[0xC] = 0;                                                                 \
        }                                                                                      \
        lbl_805F15B4->m_free(result, lbl_805F15B4);                                              \
        return NULL;                                                                           \
    } while (0)

void *fn_804A8B08(void *stream, void *source, void *destination, void *context) {
    struct {
        u8 data[0x28];
    } local18;
    u32 localC;
    if (fn_8047D868(stream, &localC, 4) == 0) {
        return NULL;
    }
    u8 local8[4];
    if (fn_80480D4C(stream, local8, 4) == 0) {
        return NULL;
    }
    u8 count = local8[1];
    size_t size;
    u32 i;
    u32 *slot;
    u32 *entry = (u32 *)lbl_805E32D8 + local8[0] * 4;
    size = entry[3] + 0x38;
    u32 *result = (u32 *)((void *(*)(u32, u32))lbl_805F15B4->m_allocate)(size, 0x103012C);
    if (result == NULL) {
        struct {
            u32 field_0x0;
            void *field_0x4;
        } failure;
        failure.field_0x0 = 0x120;
        failure.field_0x4 = fn_8047DC28(0x80000013, size);
        fn_8047DB84(&failure);
        result = NULL;
        goto allocated;
    }
    memset(result, 0, size);
    result[0] = (u32)entry;
    result[1] = count;
    if (entry[3] != 0) {
        result[0xD] = (u32)(result + 0xE);
    }
allocated:
    if (result == NULL) {
        return NULL;
    }
    if (*(volatile u8 *)(local8 + 1) != 0) {
        if (fn_80480D4C(stream, result + 0xA, result[1]) == 0) {
            RWCPP70_CONSTRUCT_FAIL(result);
        }
        for (u32 i = 0, *slot = result; i < result[1]; slot++, i++) {
            if (fn_804A0CDC(stream, slot + 2) == 0) {
                RWCPP70_CONSTRUCT_FAIL(result);
            }
        }
    }
    if ((local8[2] & 1) != 0) {
        if (fn_80483060(&local18, stream) == 0) {
            RWCPP70_CONSTRUCT_FAIL(result);
        }
        void *item = fn_804A9914(&local18);
        if (item == NULL) {
            item = fn_804A95D4();
            if (item == NULL) {
                RWCPP70_CONSTRUCT_FAIL(result);
            }
            fn_804A9BA4(item, &local18);
        }
        fn_804A8FCC((RwCpp70Container *)result, item);
        fn_804A9674((RwCpp70Node *)item);
    }
    *(void **)((u8 *)destination + (u32)context) = result;
    return stream;
}
}

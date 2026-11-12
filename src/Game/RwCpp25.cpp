/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F150C;
extern u32 lbl_805F1510;
extern u32 lbl_805F1514;
extern u32 lbl_805F1508;
extern u32 lbl_805F1518;
extern u8 *lbl_805F15B4;

extern "C" {
void fn_80461164(void) {
}

void fn_804612A4(void) {
}

u32 fn_80462884(void) {
    return 1;
}

u32 fn_804622F0(u8 *object) {
    return *(u32 *)(object + lbl_805F150C);
}

u32 fn_8046256C(u8 *object) {
    return *(u32 *)(object + lbl_805F1510);
}

u32 fn_804626EC(u8 *object) {
    return *(u32 *)(object + lbl_805F1514);
}

u32 fn_804621B0(u8 *object) {
    return *(u32 *)(object + lbl_805F1508 + 0x0C);
}

void fn_8046510C(void);
void fn_804651AC(void);

void fn_80461C80(void) {
    fn_8046510C();
}

void fn_80461D08(void) {
    fn_804651AC();
}

void fn_80465138(u32, u32);
void fn_80465200(u32, u32);

void fn_80461C18(u32 first, u32 second, u32 third) {
    fn_80465138(first, third);
}

void fn_80461CA0(u32 first, u32 second, u32 third) {
    fn_80465200(first, third);
}

void fn_804612A8(void);
u32 fn_8045C9C8(u32, u32);
void fn_80463244(void);
void fn_80463308(void);
void fn_804631D4(void);

u8 *fn_80461238(u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F1510);
    target[0] = 0;
    target[1] = *(u32 *)(lbl_805F15B4 + lbl_805F1518 + 8);
    return object;
}

void fn_80461548(u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F1514);
    target[0] = 0;
    target[1] = ((u32 *)object)[4];
    ((u32 *)object)[4] = (u32)fn_804612A8;
}

void fn_80461BF0(const u32 *object) {
    fn_8045C9C8(object[0x15], (u32)object);
}

void fn_80462770(void) {
    fn_80463244();
    fn_80463308();
    fn_804631D4();
}

u32 fn_80462184(u32 unused, u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F1508);
    if (target[3] != 0) {
        target[3] = 0;
        target[2] = 0;
    }
    return 0;
}

void fn_8045C56C(u32, u32, u32, u32);

void fn_80461B60(u32 first, u32 second, u32 *third) {
    fn_8045C56C(third[0x15], (u32)third, first, second + 0x20);
}

void *fn_80461030(u8 *);
void fn_804621C0(u32, u32);
void fn_80462578(u32, u32);
void fn_80462130(u32, u32);
u32 fn_8045FEA0(u32 *);

void fn_8046112C(u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F150C);
    target[0] = 0;
    ((u16 *)object)[0x30] = ((u16 *)lbl_805F15B4)[4] - 1;
    target[1] = ((u32 *)object)[4];
    ((u32 *)object)[4] = (u32)fn_80461030;
}

u32 fn_804619C8(u32 first, u32 second) {
    fn_804621C0(second, first);
    return first;
}

u32 fn_80461A50(u32 first, u32 second) {
    fn_80462578(second, first);
    return first;
}

u32 fn_80461AD8(u32 first, u32 second) {
    fn_80462130(second, first);
    return first;
}

u32 fn_80461DE8(u32 *object) {
    return fn_8045C9C8(object[0x1E], fn_8045FEA0(object));
}

extern u32 lbl_805F1690;

void fn_804626F8(u32 value) {
    if (value == 0) {
        value = *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x58);
    }
    if (value == 0) {
        value = 0;
    }
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x40) = value;
}

void fn_80462734(u32 value) {
    if (value == 0) {
        value = *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x54);
    }
    if (value == 0) {
        value = 0;
    }
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x3C) = value;
}

void fn_80460E74(u32 *, u8 *);

/* Validate object, release its linked nodes, then call fn_80460E74.
 * NonMatching: the unlink loop and calls match; retail uses four individual register saves and
 * assigns r29/r28/r29/r30 to callbacks, result, link and sentinel instead of this build's r30/r30/
 * r28/r29. */
void *fn_80461030(u8 *object) {
    u32 *callbacks = (u32 *)(object + lbl_805F150C);
    u32 (*validate)(u8 *) = (u32 (*)(u8 *))callbacks[1];
    if (validate(object) != 0) {
        u8 *result = (u8 *)callbacks[0];
        if (result != NULL) {
            u32 *sentinel = (u32 *)(object + 0x64);
            u32 *link = ((u32 **)object)[0x19];
            while (link != sentinel) {
                u32 *node = link - 3;
                link = (u32 *)*link;
                if (node[2] == 0 || node[5] == 0) {
                    break;
                }
                u32 value = node[3];
                *(u32 *)node[4] = value;
                value = node[4];
                *(u32 *)(node[3] + 4) = value;
                value = node[0];
                *(u32 *)node[1] = value;
                value = node[1];
                *(u32 *)(node[0] + 4) = value;
                ((void (*)(u32))*(u32 *)(lbl_805F15B4 + 0x11C))(
                    *(u32 *)(lbl_805F15B4 + lbl_805F1518));
            }
            fn_80460E74((u32 *)result, object);
            return object;
        }
    }
    return NULL;
}

/* Release object's linked nodes and install callback[1].
 * NonMatching: the entire loop and final store match; only the four individual saves at entry and
 * exit differ from this build's stmw/lmw pair. */
void *fn_80461168(u8 *object) {
    u32 *sentinel = (u32 *)(object + 0x64);
    u32 *link = ((u32 **)object)[0x19];
    u32 *callback = (u32 *)(object + lbl_805F150C);
    while (link != sentinel) {
        u32 *node = link - 3;
        link = (u32 *)*link;
        if (node[2] == 0 || node[5] == 0) {
            break;
        }
        u32 value = node[3];
        *(u32 *)node[4] = value;
        value = node[4];
        *(u32 *)(node[3] + 4) = value;
        value = node[0];
        *(u32 *)node[1] = value;
        value = node[1];
        *(u32 *)(node[0] + 4) = value;
        ((void (*)(u32))*(u32 *)(lbl_805F15B4 + 0x11C))(
            *(u32 *)(lbl_805F15B4 + lbl_805F1518));
    }
    ((u32 *)object)[4] = callback[1];
    return object;
}

void fn_804622FC(void *, void *);

void *fn_80461260(void *object, void *entry) {
    void *resource = *(void **)((u8 *)entry + lbl_805F1510);
    if (resource != NULL) {
        fn_804622FC(resource, object);
    }
    return object;
}

void *fn_80461570(void *object, void *entry) {
    void *resource = *(void **)((u8 *)entry + lbl_805F1514);
    if (resource != NULL) {
        fn_80462578((u32)resource, (u32)object);
    }
    return object;
}

void *fn_804615B4(u8 *object) {
    u32 *sentinel = (u32 *)(object + 0x2C);
    u32 *link = ((u32 **)object)[0x0B];
    while (link != sentinel) {
        u32 *node = link - 3;
        link = (u32 *)*link;
        u32 value = node[3];
        *(u32 *)node[4] = value;
        value = node[4];
        *(u32 *)(node[3] + 4) = value;
        value = node[0];
        *(u32 *)node[1] = value;
        value = node[1];
        *(u32 *)(node[0] + 4) = value;
        ((void (*)(u32))*(u32 *)(lbl_805F15B4 + 0x11C))(
            *(u32 *)(lbl_805F15B4 + lbl_805F1518 + 4));
    }
    return object;
}

u32 *fn_804566D0(u32);

void fn_80460E74(u32 *tree, u8 *object) {
    struct InsertLocals {
        float upper[3];
        float lower[3];
        u32 *parents[64];
    } locals;
    float *matrix = (float *)fn_804566D0((u32)object);
    locals.lower[0] = matrix[0] - matrix[3];
    locals.lower[1] = matrix[1] - matrix[3];
    locals.lower[2] = matrix[2] - matrix[3];
    locals.upper[0] = matrix[0] + matrix[3];
    locals.upper[1] = matrix[1] + matrix[3];
    locals.upper[2] = matrix[2] + matrix[3];
    u32 *current = (u32 *)tree[7];
    s32 depth = 0;
    while (depth >= 0) {
        u32 kind = current[0];
        if ((s32)kind < 0) {
            u32 *created = ((u32 *(*)(void *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
                *(void **)(lbl_805F15B4 + lbl_805F1518), 0x507);
            created[5] = (u32)current;
            created[2] = (u32)object;
            u32 *link = current + 14;
            current = locals.parents[depth];
            created[0] = link[0];
            created[1] = (u32)link;
            ((u32 *)link[0])[1] = (u32)created;
            link[0] = (u32)created;
            created[3] = ((u32 *)object)[0x19];
            created[4] = (u32)(object + 0x64);
            ((u32 *)created[3])[1] = (u32)(created + 3);
            ((u32 *)object)[0x19] = (u32)(created + 3);
        } else if (((float *)current)[4] > locals.lower[kind]) {
            if (((float *)current)[5] < locals.upper[kind]) {
                depth++;
                locals.parents[depth] = (u32 *)current[3];
            } else {
                current = (u32 *)current[2];
            }
        } else if (((float *)current)[5] >= locals.upper[kind]) {
            current = locals.parents[depth];
            depth--;
        } else {
            current = (u32 *)current[3];
        }
    }
}

void fn_8046221C(void *, void *);

void *fn_80461A00(u8 *object, void *entry) {
    void *candidate = object + lbl_805F150C;
    if (candidate != NULL && entry == *(void **)candidate) {
        fn_8046221C(entry, object);
    }
    return object;
}

void fn_80462618(void *, void *);

void *fn_80461A88(u8 *object, void *entry) {
    void *candidate = object + lbl_805F1514;
    if (candidate != NULL && entry == *(void **)candidate) {
        fn_80462618(entry, object);
    }
    return object;
}

void *fn_80461B10(u8 *object, void *entry) {
    void *candidate = object + lbl_805F1508;
    if (candidate != NULL && entry == *(void **)((u8 *)candidate + 0x0C)) {
        fn_80462184((u32)entry, object);
    }
    return object;
}

u32 *fn_8045C770(u8 *, u8 *, u32 *);

#pragma use_lmw_stmw off
void *fn_80461B94(void *stream, void *entry, u32 *object) {
    object[0x15] = (u32)fn_8045C770((u8 *)stream, (u8 *)object, object + 8);
    if (object[0x15] != 0) {
        return stream;
    }
    return NULL;
}

u32 fn_80465180(void *, void *, void *);

void *fn_80461C3C(void *stream, void *entry, void *object) {
    if (fn_80465180(stream, object, object) != 0) {
        return stream;
    }
    return NULL;
}

u32 fn_80465260(void *, void *, void *);

void *fn_80461CC4(void *stream, void *entry, void *object) {
    if (fn_80465260(stream, object, object) != 0) {
        return stream;
    }
    return NULL;
}

void fn_80461D28(void *stream, void *entry, u32 *object) {
    u32 value = fn_8045FEA0(object);
    fn_8045C56C(object[0x1E], value, (u32)stream, value + 0x10);
}

void *fn_80461D7C(void *stream, void *entry, u32 *object) {
    u32 value = fn_8045FEA0(object);
    object[0x1E] = (u32)fn_8045C770((u8 *)stream, (u8 *)value, (u32 *)(value + 0x10));
    if (object[0x1E] != 0) {
        return stream;
    }
    return NULL;
}
#pragma use_lmw_stmw reset
}

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

void *fn_804612A8(u8 *);
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
void *fn_804621C0(u32, u32);
void *fn_80462578(u32, u32);
void *fn_80462130(u32, u32);
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

void *fn_804622FC(void *, void *);

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

void *RwFrameGetLTM(void *);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);

/* Collect the tree nodes whose box the object's own box overlaps, appending every accepted
 * leaf to the growth pool of the stream entry block: entry[0] = pool, entry[1] = capacity in
 * nodes, entry[2] = collected count, entry[3] = tree. argument[3] == 2 selects the opposite
 * child of a node whose box contains the query box. A leaf tests six planes; each transforms
 * the node's 3x3 matrix at +0x48 with the plane record's three byte row selectors.
 * NonMatching: MWCC runs the plane loop through CTR/BDNZ, spills the two box differences to
 * the stack for their integer sign tests and selects the child with MFCTR/RLWINM on the CR0
 * flag of the split compare; the retail frame is 0x170 bytes. */
void *fn_80461658(void *argument, u8 *object) {
    struct InsertLocals {
        float bounds[9];
        u32 *parents[64];
    } locals;
    u32 *entry = (u32 *)(object + lbl_805F1508);
    float *matrix = (float *)RwFrameGetLTM(*(void **)(object + 4));
    locals.bounds[0] = *(float *)(object + 0x10C);
    locals.bounds[1] = *(float *)(object + 0x110);
    locals.bounds[2] = *(float *)(object + 0x114);
    locals.bounds[3] = *(float *)(object + 0x118);
    locals.bounds[4] = *(float *)(object + 0x120 - 4);
    locals.bounds[5] = *(float *)(object + 0x120);
    locals.bounds[6] = matrix[0x30 / 4];
    locals.bounds[7] = matrix[0x34 / 4];
    locals.bounds[8] = matrix[0x38 / 4];
    u32 inverted = ((s32)((u32 *)argument)[3] == 2);
    u32 *current = (u32 *)((u32 *)entry[3])[7];
    s32 depth = 0;
    s32 count = 0;
    u32 slot = 0;
    for (;;) {
        s32 axis = (s32)current[0];
        if (axis < 0) {
            const float *rows = (const float *)current + 0x48 / 4;
            u32 failed;
            for (u32 section = 0; section < 3; section++) {
                const u8 *record = object + 0x94 + section * 0x28;
                const float *plane = (const float *)record;
                failed = (rows[record[0x11] * 3 + 1] * plane[1] +
                          rows[record[0x10] * 3] * plane[0] +
                          rows[record[0x12] * 3 + 2] * plane[2] - plane[3]) < 0.0f;
                if (failed != 0) {
                    break;
                }
                failed = (rows[record[0x25] * 3 + 1] * plane[6] +
                          rows[record[0x24] * 3] * plane[5] +
                          rows[record[0x26] * 3 + 2] * plane[7] - plane[8]) < 0.0f;
                if (failed != 0) {
                    break;
                }
            }
            if (failed == 0) {
                if (count >= (s32)entry[1]) {
                    u32 size = (entry[1] + 50) * 4;
                    u32 *grown;
                    if (entry[0] == 0) {
                        grown = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(
                            size, 0x01030507);
                    } else {
                        grown = ((u32 *(*)(void *, u32, u32))*(u32 *)(lbl_805F15B4 + 0x110))(
                            (void *)entry[0], size, 0x01030507);
                    }
                    if (grown == NULL) {
                        u32 args[2];
                        args[0] = 2;
                        args[1] = fn_8047DC28(0x80000013, size);
                        fn_8047DB84(args);
                        entry[2] = (u32)count;
                        return argument;
                    }
                    entry[0] = (u32)grown;
                    entry[1] = entry[1] + 50;
                }
                count++;
                *(u32 *)((u8 *)entry[0] + slot) = (u32)current;
                slot += 4;
            }
            current = locals.parents[depth];
            depth--;
        } else {
            float upper = *(float *)((u8 *)locals.bounds + axis + 0x0C) - ((float *)current)[4];
            if (upper < 0.0f &&
                ((float *)current)[5] - *(float *)((u8 *)locals.bounds + axis) < 0.0f) {
                if (inverted == (((float *)current)[1] <
                                 *(float *)((u8 *)locals.bounds + axis + 0x18))) {
                    depth++;
                    locals.parents[depth] = (u32 *)current[3];
                    current = (u32 *)current[3 - 1];
                } else {
                    depth++;
                    locals.parents[depth] = (u32 *)current[2];
                    current = (u32 *)current[3];
                }
            } else if (upper < 0.0f) {
                current = (u32 *)current[2];
            } else {
                current = (u32 *)current[3];
            }
        }
        if (depth < 0) {
            entry[2] = (u32)count;
            return argument;
        }
    }
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

void *fn_8046221C(void *, void *);

void *fn_80461A00(u8 *object, void *entry) {
    void *candidate = object + lbl_805F150C;
    if (candidate != NULL && entry == *(void **)candidate) {
        fn_8046221C(entry, object);
    }
    return object;
}

void *fn_80462618(void *, void *);

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

void *RwFrameGetLTM(void *);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);

void *fn_804612A8(u8 *object) {
    struct InsertLocals {
        float upper[3];
        float lower[3];
        u32 *parents[64];
    } locals;
    u32 *callbacks = (u32 *)(object + lbl_805F1514);
    u32 (*validate)(u8 *) = (u32 (*)(u8 *))callbacks[1];
    if (validate(object) == 0) {
        u32 args[2];
        args[0] = 2;
        args[1] = fn_8047DC28(0x80000016);
        fn_8047DB84(args);
        return object;
    }
    if ((s32)object[1] < 0x80) {
        return object;
    }
    u32 *tree = (u32 *)callbacks[0];
    u32 *frame = (u32 *)((u32 *)object)[1];
    if (tree == NULL || frame == NULL) {
        return object;
    }
    float offset = ((float *)object)[5];
    u32 *link = ((u32 **)object)[0x0B];
    u32 *sentinel = (u32 *)(object + 0x2C);
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
    float *matrix = (float *)RwFrameGetLTM(frame);
    locals.lower[0] = matrix[0] - offset;
    locals.lower[1] = matrix[1] - offset;
    locals.lower[2] = matrix[2] - offset;
    locals.upper[0] = matrix[0] + offset;
    locals.upper[1] = matrix[1] + offset;
    locals.upper[2] = matrix[2] + offset;
    u32 *current = (u32 *)tree[7];
    s32 depth = 0;
    while (depth >= 0) {
        u32 kind = current[0];
        if ((s32)kind < 0) {
            u32 *created = ((u32 *(*)(void *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
                *(void **)(lbl_805F15B4 + lbl_805F1518), 0x507);
            created[5] = (u32)current;
            created[2] = (u32)object;
            u32 *link = current + 16;
            current = locals.parents[depth];
            created[0] = link[0];
            created[1] = (u32)link;
            ((u32 *)link[0])[1] = (u32)created;
            link[0] = (u32)created;
            created[3] = ((u32 *)object)[0x0B];
            created[4] = (u32)(object + 0x2C);
            ((u32 *)created[3])[1] = (u32)(created + 3);
            ((u32 *)object)[0x0B] = (u32)(created + 3);
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
    return object;
}

void fn_8048722C(u32);

#pragma use_lmw_stmw off
void *fn_80462130(u32 stream, u32 object) {
    u32 value = *(u32 *)(object + 4);
    u32 *target = (u32 *)(object + lbl_805F1508);
    if (value != 0) {
        fn_8048722C(value);
        target[3] = (u32)stream;
    }
    return (void *)stream;
}

#pragma use_lmw_stmw on
void *fn_804621C0(u32 stream, u32 object) {
    u32 value = *(u32 *)(object + 4);
    u32 *target = (u32 *)(object + lbl_805F150C);
    if (value != 0) {
        fn_8048722C(value);
        *(u32 *)target = stream;
    }
    return (void *)stream;
}

void fn_8048071C(u32);

#pragma use_lmw_stmw off
void *fn_8046221C(void *stream, void *object_) {
    u32 *object = (u32 *)object_;
    u32 *target = (u32 *)((u8 *)object + lbl_805F150C);
    u32 value = object[5];
    if (value != 0) {
        fn_8048071C(value);
    }
    u32 *link = ((u32 **)object)[0x19];
    u32 *sentinel = (u32 *)((u8 *)object + 0x64);
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
    *target = 0;
    return stream;
}
#pragma use_lmw_stmw reset

void fn_80456C54(void *, void *, void *);
void fn_80456D54(void *, void *, void *);
void fn_80456CD0(void *, void *, void *);
void fn_8047E990(u32, u32);

void *fn_804622FC(void *stream, void *object_) {
    u32 *object = (u32 *)object_;
    u32 *target = (u32 *)((u8 *)object + lbl_805F1510);
    u32 value = object[1];
    u32 *link = (u32 *)((u8 *)stream + 0x2C);
    object[8] = ((u32 *)stream)[0x0B];
    object[9] = (u32)link;
    link[1] = (u32)(object + 8);
    ((u32 *)stream)[0x0B] = (u32)(object + 8);
    ((u32 *)stream)[9] = ((u32 *)stream)[9] + 1;
    target[0] = (u32)stream;
    fn_80456C54(object, (void *)fn_804619C8, stream);
    fn_80456D54(object, (void *)fn_80461A50, stream);
    fn_80456CD0(object, (void *)fn_80461AD8, stream);
    if (value != 0) {
        fn_8047E990(value + 0x10, 0);
        fn_8048722C(value);
    }
    target[1] = *(u32 *)(lbl_805F15B4 + lbl_805F1518 + 8);
    return stream;
}

void *fn_804623EC(void *stream, void *object_) {
    u32 *object = (u32 *)object_;
    u32 *target = (u32 *)((u8 *)object + lbl_805F1510);
    u32 *entry = *(u32 **)target;
    if (entry != NULL) {
        entry[9] = entry[9] - 1;
        entry = *(u32 **)target;
        if ((u32)(object + 8) == entry[0x0A]) {
            entry[0x0A] = object[8];
        }
        u32 *link = (u32 *)object[9];
        link[0] = object[8];
        link = (u32 *)object[8];
        link[1] = object[9];
        fn_80456C54(object, (void *)fn_80461A00, stream);
        fn_80456D54(object, (void *)fn_80461A88, stream);
        fn_80456CD0(object, (void *)fn_80461B10, stream);
        *target = 0;
        return stream;
    } else {
        u32 args[2];
        args[0] = 2;
        args[1] = fn_8047DC28(4);
        fn_8047DB84(args);
        return NULL;
    }
}

#pragma use_lmw_stmw off
void *fn_804624E8(void *stream, void *callback, void *data) {
    u32 *entry = (u32 *)((u8 *)stream + lbl_805F1508);
    u32 *current = (u32 *)entry[0];
    s32 count = entry[2];
    while (count != 0) {
        if (((u32 (*)(u32, void *))callback)(current[0], data) != 0) {
            return stream;
        }
        current++;
        count--;
    }
    return stream;
}

void *fn_80462578(u32 stream, u32 object) {
    u32 *target = (u32 *)(object + lbl_805F1514);
    *target = stream;
    if (*(u8 *)(object + 1) < 0x80) {
        u32 value = *(u32 *)(stream + 0x3C);
        u32 *link = (u32 *)(stream + 0x3C);
        ((u32 *)object)[0x0D] = value;
        ((u32 *)object)[0x0E] = (u32)link;
        link[1] = object + 0x34;
        *(u32 *)(stream + 0x3C) = object + 0x34;
    } else {
        u32 value = *(u32 *)(object + 4);
        if (value != 0) {
            fn_8048722C(value);
        }
        value = *(u32 *)(stream + 0x34);
        u32 *link = (u32 *)(stream + 0x34);
        ((u32 *)object)[0x0D] = value;
        ((u32 *)object)[0x0E] = (u32)link;
        link[1] = object + 0x34;
        *(u32 *)(stream + 0x34) = object + 0x34;
    }
    return (void *)stream;
}

#pragma use_lmw_stmw on
void *fn_80462618(void *stream, void *object_) {
    u32 *object = (u32 *)object_;
    u32 *target = (u32 *)((u8 *)object + lbl_805F1514);
    *target = 0;
    u32 *sentinel = (u32 *)((u8 *)object + 0x2C);
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
    u32 value = object[0x0D];
    *(u32 *)object[0x0E] = value;
    value = object[0x0E];
    *(u32 *)(object[0x0D] + 4) = value;
    return stream;
}
#pragma use_lmw_stmw reset

u32 fn_804631CC(void);
u32 fn_80463298(void);
u32 fn_804631D8(void);
void fn_80463244(void);
void fn_80463308(void);
void fn_804631D4(void);

u32 fn_80462798(void) {
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x3C) = 0;
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x40) = 0;
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x44) = 0;
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x48) = 0;
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x4C) = 0;
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x50) = 0;
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x54) = 0;
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x58) = 0;
    *(u32 *)(lbl_805F15B4 + lbl_805F1690 + 0x5C) = 0;
    fn_804631CC();
    fn_80463298();
    if (fn_804631D8() == 0) {
        return 1;
    }
    fn_80463244();
    fn_80463308();
    fn_804631D4();
    return 0;
}

void fn_8039561C(void);
s32 fn_8049A78C(u32);
extern u16 lbl_805EDF40;

u32 fn_8046288C(u32 *object) {
    u32 value = ((u16 *)object)[0x0C];
    if (value == lbl_805EDF40) {
        fn_8039561C();
        s32 next = (s32)lbl_805EDF40 + 1;
        lbl_805EDF40 = next % 0x2000;
        fn_8049A78C(value);
    }
    return value;
}
}

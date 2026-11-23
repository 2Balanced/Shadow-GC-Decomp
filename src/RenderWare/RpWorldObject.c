/* RenderWare 3.7 world plugin: world objects, 0x80460974-0x804626F8 (wave 41 re-cut of the size-cut
 * Game/RwCpp24 tail and Game/RwCpp25). Atomics, clumps and lights in a world and the ties between
 * them and the world sectors: the world-object module (plugin 0x509) with its two free lists of
 * ties (.bss 0x805E1640, 0x805E1664) and their creation parameters (.sdata 0x805EDDC8-0x805EDDD8),
 * the plugin extensions it registers on world sectors, atomics, clumps and lights and its module
 * record (.sbss 0x805F1508-0x805F1520), the stream callbacks of the binary mesh (0x50E) and native
 * data (0x510) plugins, and the API that adds objects to worlds and removes them.
 *
 * Names are inferred from behaviour and the public RenderWare API:
 *   fn_80460974 / fn_80460A88  world-object module open / close
 *   fn_80461E20  registers the module, the extensions, the stream plugins and the rights callbacks
 *   fn_804622FC / fn_804623EC  RpWorldAddClump / RpWorldRemoveClump
 *   fn_80462578 / fn_80462618  RpWorldAddLight / RpWorldRemoveLight
 * Most functions are still untyped drafts. Built with cflags_rw_r3 (configure.py).
 * Status: 47 of 55 functions match; fn_80460E74, fn_804612A8, fn_80461658 (drafts), fn_80461B60
 * (argument order), fn_8046221C (two loads), fn_804622FC, fn_804623EC and fn_80462578 are left.
 */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

void *memset(void *, int, u32);
void *memcpy(void *, const void *, u32);

/* The untyped functions below still index the engine block by offset. */
#define RWENGINEBYTES ((u8 *)lbl_805F15B4)

/* The tie free lists and their creation parameters. */
static u32 lbl_805EDDC8 = 256; /* atomic ties: block size */
static u32 lbl_805EDDCC = 1;   /* atomic ties: blocks to preallocate */
static u32 lbl_805EDDD0 = 32;  /* light ties: block size */
static u32 lbl_805EDDD4 = 1;   /* light ties: blocks to preallocate */
static RwFreeList lbl_805E1640;
static RwFreeList lbl_805E1664;

/* The module record and the offsets of the extensions (uninitialized statics come out in reverse
 * order). */
static RwModuleInfo lbl_805F1518;
static u32 lbl_805F1514; /* lights */
static u32 lbl_805F1510; /* clumps */
static u32 lbl_805F150C; /* atomics */
static u32 lbl_805F1508; /* world sectors */

/* Functions of this file. */
u32 *fn_80460974(u32 *object, u32 offset);
u32 *fn_80460A88(u32 *object);
void fn_80460B1C(u32 *object);
void fn_80460B68(u8 *object);
u32 *fn_80460BA4(u32 *object);
void fn_80460C48(u32 *object);
u32 *fn_80460CA4(u32 *object, u32 *engine);
u32 *fn_80460D00(u32 *object);
u32 fn_80460D80(u32 *object);
u32 fn_80460DF0(u32 *object);
void fn_80460E74(u32 *tree, u8 *object);
void *fn_80461030(u8 *object);
void fn_8046112C(u8 *object);
void fn_80461164(void);
void *fn_80461168(u8 *object);
u8 *fn_80461238(u8 *object);
void *fn_80461260(void *object, void *entry);
void fn_804612A4(void);
void *fn_804612A8(u8 *object);
void fn_80461548(u8 *object);
void *fn_80461570(void *object, void *entry);
void *fn_804615B4(u8 *object);
void *fn_80461658(void *argument, u8 *object);
u32 fn_804619C8(u32 first, u32 second);
void *fn_80461A00(u8 *object, void *entry);
u32 fn_80461A50(u32 first, u32 second);
void *fn_80461A88(u8 *object, void *entry);
u32 fn_80461AD8(u32 first, u32 second);
void *fn_80461B10(u8 *object, void *entry);
void fn_80461B60(u32 first, u32 second, u32 *third);
void *fn_80461B94(void *stream, void *entry, u32 *object);
void fn_80461BF0(const u32 *object);
void fn_80461C18(u32 first, u32 second, u32 third);
void *fn_80461C3C(void *stream, void *entry, void *object);
void fn_80461C80(void);
void fn_80461CA0(u32 first, u32 second, u32 third);
void *fn_80461CC4(void *stream, void *entry, void *object);
void fn_80461D08(void);
void fn_80461D28(void *stream, void *entry, u32 *object);
void *fn_80461D7C(void *stream, void *entry, u32 *object);
u32 fn_80461DE8(u32 *object);
u32 fn_80461E20(void);
void *fn_80462130(u32 stream, u32 object);
u32 fn_80462184(u32 unused, u8 *object);
u32 fn_804621B0(u8 *object);
void *fn_804621C0(u32 stream, u32 object);
void *fn_8046221C(void *stream, void *object_);
u32 fn_804622F0(u8 *object);
void *fn_804622FC(void *stream, void *object_);
void *fn_804623EC(void *stream, void *object_);
void *fn_804624E8(void *stream, void *callback, void *data);
u32 fn_8046256C(u8 *object);
void *fn_80462578(u32 stream, u32 object);
void *fn_80462618(void *stream, void *object_);
u32 fn_804626EC(u8 *object);

void *fn_8047FC04(u32, u32, u32, u32, u32, u32);
u32 *fn_804566D0(u32);
u32 fn_8045FEA0(u32 *);
u32 fn_8045C9C8(u32, u32);
void fn_80463244(void);
void fn_80463308(void);
void fn_804631D4(void);
void *RwFrameGetLTM(void *);
void fn_8045C56C(u32, u32, u32, u32);
u32 *fn_8045C770(u8 *, u8 *, u32 *);
void fn_80465138(u32, u32);
void fn_80465200(u32, u32);
u32 fn_80465180(void *, void *, void *);
void fn_8046510C(void);
void fn_804651AC(void);
u32 fn_80465260(void *, void *, void *);
u32 fn_80485D78(u32, u32, void *, void *);
u32 fn_804859B8(u32, u32, void *, void *, void *);
u32 fn_80458B6C(u32, u32, void *, void *, void *);
u32 fn_80458BAC(u32, u32, void *, void *, void *);
u32 fn_8045AAA0(u32, u32, void *, void *, void *);
u32 fn_8045994C(u32, u32, void *, void *, void *);
u32 fn_8045F2F8(u32, u32, void *, void *, void *);
u32 fn_8045998C(u32, void *, void *, void *);
u32 fn_8045F338(u32, void *, void *, void *);
u32 fn_80458BEC(u32, void *, void *, void *);
u32 fn_80460790(u32, u32, void *, void *, void *);
u32 fn_804607D0(u32, void *, void *, void *);
u32 fn_8045B2CC(u32, u32, void *, void *, void *);
u32 fn_8045B30C(u32, void *, void *, void *);
void fn_80455D4C(void);
void fn_80455DC8(void);
void fn_80455E44(void);
void fn_80454AA4(void);
void fn_80454B20(void);
void fn_80454B9C(void);
void fn_80454BC4(void);
void fn_80454C40(void);
void fn_80454CBC(void);
void fn_8045AF28(void);
void fn_8045AFA4(void);
void fn_8045B020(void);
void fn_8048722C(u32);
void fn_8048071C(u32);
void fn_80456C54(void *, void *, void *);
void fn_80456D54(void *, void *, void *);
void fn_80456CD0(void *, void *, void *);
void fn_8047E990(u32, u32);

u32 *fn_80460974(u32 *object, u32 offset) {
    lbl_805F1518.globalsOffset = offset;
    *(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset) =
        (u32 *)fn_8047FC04(24, lbl_805EDDC8, 4, lbl_805EDDCC, (u32)&lbl_805E1640, 0x40507);
    if (*(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset) == NULL) {
        return NULL;
    }
    *(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 4) =
        (u32 *)fn_8047FC04(24, lbl_805EDDD0, 4, lbl_805EDDD4, (u32)&lbl_805E1664, 0x40507);
    if (*(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 4) == 0) {
        fn_8047FCDC(*(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset));
        *(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset) = NULL;
        return NULL;
    }
    ((u16 *)RWENGINEBYTES)[4] = 1;
    *(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 8) = 0;
    lbl_805F1518.numInstances++;
    return object;
}

u32 *fn_80460A88(u32 *object) {
    u32 *first = *(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 4);
    if (first != NULL) {
        fn_8047FCDC(first);
        *(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 4) = NULL;
    }
    {
    u32 *second = *(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset);
    if (second != NULL) {
        fn_8047FCDC(second);
        *(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset) = NULL;
    }
    lbl_805F1518.numInstances--;
    return object;
}}

void fn_80460B1C(u32 *object) {
    u8 *engine = RWENGINEBYTES;
    u32 offset = lbl_805F1508;
    u32 *table = (u32 *)((u8 *)object + offset);
    ((u32 *)engine)[1] = table[3];
    engine = RWENGINEBYTES;
    {
    u32 count = ((u16 *)engine)[4];
    ((u16 *)engine)[4] = count + 1;
    ((void (*)(u32 *, u32))table[4])(object, count);
}}

void fn_80460B68(u8 *object) {
    u32 offset = lbl_805F1508;
    u32 *target = (u32 *)(object + offset);
    ((u32 *)RWENGINEBYTES)[1] = 0;
    {
    void (*callback)(u8 *) = (void (*)(u8 *))target[5];
    callback(object);
}}

u32 *fn_80460BA4(u32 *object) {
    u32 *table = (u32 *)((u8 *)object + lbl_805F1508);
    if (((u32 (*)(u32 *))table[6])(object) != 0) {
        u32 *child = (u32 *)table[3];
        if (child != NULL) {
            void (*notify)(u32, u32, u32) = (void (*)(u32, u32, u32))*(u32 *)(RWENGINEBYTES + 0xA0);
            fn_80461658(child, (u8 *)object);
            notify(0, 0, child[3] == 1 ? 1 : 0);
        }
        return object;
    }
    return NULL;
}

void fn_80460C48(u32 *object) {
    u32 *table = (u32 *)((u8 *)object + lbl_805F1508);
    table[0] = 0;
    table[1] = 0;
    table[2] = 0;
    table[4] = object[6];
    table[5] = object[7];
    table[6] = object[4];
    object[4] = (u32)fn_80460BA4;
    object[6] = (u32)fn_80460B1C;
    object[7] = (u32)fn_80460B68;
    table[3] = 0;
}

u32 *fn_80460CA4(u32 *object, u32 *engine) {
    u32 offset = lbl_805F1508;
    u32 *table = (u32 *)((u8 *)object + offset);
    table[0] = 0;
    {
    u32 condition = *(u32 *)((u8 *)engine + offset + 12);
    table[1] = 0;
    table[2] = 0;
    if (condition != 0) {
        fn_80462130(condition, (u32)object);
    }
    return object;
}}

u32 *fn_80460D00(u32 *object) {
    u32 *table = (u32 *)((u8 *)object + lbl_805F1508);
    if (table[0] != 0) {
        ((void (*)(u32 *))*(u32 *)(RWENGINEBYTES + 0x10C))((u32 *)table[0]);
    }
    table[0] = 0;
    table[1] = 0;
    table[2] = 0;
    object[6] = table[4];
    object[7] = table[5];
    object[4] = table[6];
    return object;
}

u32 fn_80460D80(u32 *object) {
    ((u32 *)object[4])[0] = object[3];
    ((u32 *)object[3])[1] = object[4];
    ((u32 *)object[1])[0] = object[0];
    ((u32 *)object[0])[1] = object[1];
    ((void (*)(u32 *, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
        *(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 4), object);
    return 1;
}

u32 fn_80460DF0(u32 *object) {
    if (object[2] != 0 && object[5] != 0) {
        ((u32 *)object[4])[0] = object[3];
        ((u32 *)object[3])[1] = object[4];
        ((u32 *)object[1])[0] = object[0];
        ((u32 *)object[0])[1] = object[1];
        ((void (*)(u32 *, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
            *(u32 **)(RWENGINEBYTES + lbl_805F1518.globalsOffset), object);
    }
    return 1;
}

void fn_80460E74(u32 *tree, u8 *object) {
    struct InsertLocals {
        float upper[3];
        float lower[3];
        u32 *parents[64];
    } locals;
    {
    float *matrix = (float *)fn_804566D0((u32)object);
    locals.lower[0] = matrix[0] - matrix[3];
    locals.lower[1] = matrix[1] - matrix[3];
    locals.lower[2] = matrix[2] - matrix[3];
    locals.upper[0] = matrix[0] + matrix[3];
    locals.upper[1] = matrix[1] + matrix[3];
    locals.upper[2] = matrix[2] + matrix[3];
    {
    u32 *current = (u32 *)tree[7];
    s32 depth = 0;
    while (depth >= 0) {
        u32 kind = current[0];
        if ((s32)kind < 0) {
            u32 *created = ((u32 *(*)(void *, u32))*(u32 *)(RWENGINEBYTES + 0x118))(
                *(void **)(RWENGINEBYTES + lbl_805F1518.globalsOffset), 0x507);
            created[5] = (u32)current;
            created[2] = (u32)object;
            {
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
        }} else if (((float *)current)[4] > locals.lower[kind]) {
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
}}}

/* Validate object, release its linked nodes, then call fn_80460E74.
 * NonMatching: the unlink loop and calls match; retail uses four individual register saves and
 * assigns r29/r28/r29/r30 to callbacks, result, link and sentinel instead of this build's r30/r30/
 * r28/r29. */
void *fn_80461030(u8 *object) {
    u32 *sentinel;
    u32 *link;
    u32 *callbacks;
    u8 *result;
    callbacks = (u32 *)(object + lbl_805F150C);
    if (((u32 (*)(u8 *))callbacks[1])(object) != 0) {
        result = (u8 *)callbacks[0];
        if (result != NULL) {
            link = ((u32 **)object)[0x19];
            sentinel = (u32 *)(object + 0x64);
            while (link != sentinel) {
                u32 *node = link - 3;
                link = (u32 *)*link;
                if (node[2] == 0 || node[5] == 0) {
                    continue;
                }
                {
                u32 value = node[3];
                *(u32 *)node[4] = value;
                value = node[4];
                *(u32 *)(node[3] + 4) = value;
                value = node[0];
                *(u32 *)node[1] = value;
                value = node[1];
                *(u32 *)(node[0] + 4) = value;
                ((void (*)(u32, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
                    *(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset), node);
            }}
            fn_80460E74((u32 *)result, object);
        }
        return object;
    }
    return NULL;
}

void fn_8046112C(u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F150C);
    target[0] = 0;
    ((u16 *)object)[0x30] = ((u16 *)RWENGINEBYTES)[4] - 1;
    target[1] = ((u32 *)object)[4];
    ((u32 *)object)[4] = (u32)fn_80461030;
}
void fn_80461164(void) {
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
            continue;
        }
        {
        u32 value = node[3];
        *(u32 *)node[4] = value;
        value = node[4];
        *(u32 *)(node[3] + 4) = value;
        value = node[0];
        *(u32 *)node[1] = value;
        value = node[1];
        *(u32 *)(node[0] + 4) = value;
        ((void (*)(u32, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
            *(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset), node);
    }}
    ((u32 *)object)[4] = callback[1];
    return object;
}

u8 *fn_80461238(u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F1510);
    target[0] = 0;
    target[1] = *(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 8);
    return object;
}

void *fn_80461260(void *object, void *entry) {
    void *resource = *(void **)((u8 *)entry + lbl_805F1510);
    if (resource != NULL) {
        fn_804622FC(resource, object);
    }
    return object;
}

void fn_804612A4(void) {
}

void *fn_804612A8(u8 *object) {
    struct InsertLocals {
        float upper[3];
        float lower[3];
        u32 *parents[64];
    } locals;
    {
    u32 *callbacks = (u32 *)(object + lbl_805F1514);
    u32 (*validate)(u8 *) = (u32 (*)(u8 *))callbacks[1];
    if (validate(object) == 0) {
        u32 args[2];
        args[0] = 2;
        args[1] = fn_8047DC28(0x80000016);
        fn_8047DB84((RwError *)args);
        return object;
    }
    if ((s32)object[1] < 0x80) {
        return object;
    }
    {
    u32 *tree = (u32 *)callbacks[0];
    u32 *frame = (u32 *)((u32 *)object)[1];
    if (tree == NULL || frame == NULL) {
        return object;
    }
    {
    float offset = ((float *)object)[5];
    u32 *link = ((u32 **)object)[0x0B];
    u32 *sentinel = (u32 *)(object + 0x2C);
    while (link != sentinel) {
        u32 *node = link - 3;
        link = (u32 *)*link;
        {
        u32 value = node[3];
        *(u32 *)node[4] = value;
        value = node[4];
        *(u32 *)(node[3] + 4) = value;
        value = node[0];
        *(u32 *)node[1] = value;
        value = node[1];
        *(u32 *)(node[0] + 4) = value;
        ((void (*)(u32, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
            *(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 4), node);
    }}
    {
    float *matrix = (float *)RwFrameGetLTM(frame);
    locals.lower[0] = matrix[0] - offset;
    locals.lower[1] = matrix[1] - offset;
    locals.lower[2] = matrix[2] - offset;
    locals.upper[0] = matrix[0] + offset;
    locals.upper[1] = matrix[1] + offset;
    locals.upper[2] = matrix[2] + offset;
    {
    u32 *current = (u32 *)tree[7];
    s32 depth = 0;
    while (depth >= 0) {
        u32 kind = current[0];
        if ((s32)kind < 0) {
            u32 *created = ((u32 *(*)(void *, u32))*(u32 *)(RWENGINEBYTES + 0x118))(
                *(void **)(RWENGINEBYTES + lbl_805F1518.globalsOffset), 0x507);
            created[5] = (u32)current;
            created[2] = (u32)object;
            {
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
        }} else if (((float *)current)[4] > locals.lower[kind]) {
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
}}}}}}

void fn_80461548(u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F1514);
    target[0] = 0;
    target[1] = ((u32 *)object)[4];
    ((u32 *)object)[4] = (u32)fn_804612A8;
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
        {
        u32 value = node[3];
        *(u32 *)node[4] = value;
        value = node[4];
        *(u32 *)(node[3] + 4) = value;
        value = node[0];
        *(u32 *)node[1] = value;
        value = node[1];
        *(u32 *)(node[0] + 4) = value;
        ((void (*)(u32, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
            *(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 4), node);
    }}
    return object;
}

/* Collect the tree nodes whose box the object's own box overlaps, appending every accepted
 * leaf to the growth pool of the stream entry block: entry[0] = pool, entry[1] = capacity in
 * nodes, entry[2] = collected count, entry[3] = tree. argument[3] == 2 selects the opposite
 * child of a node whose box contains the query box. A leaf tests six planes; each transforms
 * the node's 3x3 matrix at +0x48 with the plane record's three byte row selectors.
 * NonMatching: MWCC runs the plane loop through CTR/BDNZ, spills the two box differences and
 * every plane value to the stack for their integer sign tests, selects the child with
 * MFCTR/RLWINM on the CR0 field of the split compare, materialises the allocator result in a
 * register for the shared failure exit, and puts the nine bounds floats at r1+28 with the
 * parent stack at r1+64 in a 0x170 byte frame that saves no f31. */
void *fn_80461658(void *argument, u8 *object) {
    struct InsertLocals {
        float bounds[9];
        u32 *parents[64];
    } locals;
    {
    u32 *entry = (u32 *)(object + lbl_805F1508);
    float *matrix = (float *)RwFrameGetLTM(*(void **)(object + 4));
    locals.bounds[0] = *(float *)(object + 0x10C);
    locals.bounds[1] = *(float *)(object + 0x110);
    locals.bounds[2] = *(float *)(object + 0x114);
    locals.bounds[3] = *(float *)(object + 0x118);
    locals.bounds[4] = *(float *)(object + 0x11C);
    locals.bounds[5] = *(float *)(object + 0x120);
    locals.bounds[6] = matrix[0x30 / 4];
    locals.bounds[7] = matrix[0x34 / 4];
    locals.bounds[8] = matrix[0x38 / 4];
    {
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
            { u32 section; for (section = 0; section < 3; section++) {
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
            }}
            if (failed == 0) {
                if (count >= (s32)entry[1]) {
                    u32 size = (entry[1] + 50) * 4;
                    u32 *grown;
                    if (entry[0] == 0) {
                        grown = ((u32 *(*)(u32, u32))*(u32 *)(RWENGINEBYTES + 0x108))(
                            size, 0x01030507);
                    } else {
                        grown = ((u32 *(*)(void *, u32, u32))*(u32 *)(RWENGINEBYTES + 0x110))(
                            (void *)entry[0], size, 0x01030507);
                    }
                    if (grown == NULL) {
                        u32 args[2];
                        args[0] = 2;
                        args[1] = fn_8047DC28(0x80000013, size);
                        fn_8047DB84((RwError *)args);
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
                    current = (u32 *)current[2];
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
}}}

u32 fn_804619C8(u32 first, u32 second) {
    fn_804621C0(second, first);
    return first;
}

void *fn_80461A00(u8 *object, void *entry) {
    void *candidate = object + lbl_805F150C;
    if (candidate != NULL && entry == *(void **)candidate) {
        fn_8046221C(entry, object);
    }
    return object;
}

u32 fn_80461A50(u32 first, u32 second) {
    fn_80462578(second, first);
    return first;
}

void *fn_80461A88(u8 *object, void *entry) {
    void *candidate = object + lbl_805F1514;
    if (candidate != NULL && entry == *(void **)candidate) {
        fn_80462618(entry, object);
    }
    return object;
}

u32 fn_80461AD8(u32 first, u32 second) {
    fn_80462130(second, first);
    return first;
}

void *fn_80461B10(u8 *object, void *entry) {
    void *candidate = object + lbl_805F1508;
    if (candidate != NULL && entry == *(void **)((u8 *)candidate + 0x0C)) {
        fn_80462184((u32)entry, object);
    }
    return object;
}

void fn_80461B60(u32 first, u32 second, u32 *third) {
    fn_8045C56C(third[0x15], (u32)third, first, (u32)third + 0x20);
}

void *fn_80461B94(void *stream, void *entry, u32 *object) {
    object[0x15] = (u32)fn_8045C770((u8 *)stream, (u8 *)object, object + 8);
    if (object[0x15] != 0) {
        return stream;
    }
    return NULL;
}

void fn_80461BF0(const u32 *object) {
    fn_8045C9C8(object[0x15], (u32)object);
}

void fn_80461C18(u32 first, u32 second, u32 third) {
    fn_80465138(first, third);
}

void *fn_80461C3C(void *stream, void *entry, void *object) {
    if (fn_80465180(stream, object, object) != 0) {
        return stream;
    }
    return NULL;
}

void fn_80461C80(void) {
    fn_8046510C();
}

void fn_80461CA0(u32 first, u32 second, u32 third) {
    fn_80465200(first, third);
}

void *fn_80461CC4(void *stream, void *entry, void *object) {
    if (fn_80465260(stream, object, object) != 0) {
        return stream;
    }
    return NULL;
}

void fn_80461D08(void) {
    fn_804651AC();
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

u32 fn_80461DE8(u32 *object) {
    return fn_8045C9C8(object[0x1E], fn_8045FEA0(object));
}

/* Register the stream types and plugin records of this file: the boxed geometry record
 * (28 bytes) and the tree node, plane list and node pool records (8 bytes each) are registered
 * with their three stream callbacks under id 1289, the collection and attribute plugins under
 * 1294 and 1296, and four further plugin records under stream id 31. Each registration result
 * is stored in the global slot it owns and OR-ed into the running status, which is kept in one
 * register across the whole chain.
 * NonMatching: retail stores each result through the sdata21 base register (stw r3,0(r2)) and
 * ORs the return value straight into the accumulator; reloading every global for the OR would
 * add a load per registration. */
u32 fn_80461E20(void) {
    u32 status = fn_80485D78(12, 1289, (void *)fn_80460974, (void *)fn_80460A88);
    status |= (lbl_805F1508 =
                   fn_804859B8(28, 1289, (void *)fn_80460C48, (void *)fn_80460D00,
                               (void *)fn_80460CA4));
    status |= (lbl_805F150C = fn_80458B6C(8, 1289, (void *)fn_8046112C, (void *)fn_80461168,
                                          (void *)fn_80461164));
    status |= (lbl_805F1510 = fn_80458BAC(8, 1289, (void *)fn_80461238, (void *)fn_804612A4,
                                          (void *)fn_80461260));
    status |= (lbl_805F1514 = fn_8045AAA0(8, 1289, (void *)fn_80461548, (void *)fn_804615B4,
                                          (void *)fn_80461570));
    status |= fn_8045994C(0, 1294, NULL, NULL, NULL);
    status |= fn_8045F2F8(0, 1294, NULL, NULL, NULL);
    status |= fn_8045998C(1294, (void *)fn_80461B94, (void *)fn_80461B60, (void *)fn_80461BF0);
    status |= fn_8045F338(1294, (void *)fn_80461D7C, (void *)fn_80461D28, (void *)fn_80461DE8);
    status |= fn_8045994C(0, 1296, NULL, NULL, NULL);
    status |= fn_8045F2F8(0, 1296, NULL, NULL, NULL);
    status |= fn_8045998C(1296, (void *)fn_80461C3C, (void *)fn_80461C18, (void *)fn_80461C80);
    status |= fn_8045F338(1296, (void *)fn_80461CC4, (void *)fn_80461CA0, (void *)fn_80461D08);
    status |= fn_80458B6C(0, 31, NULL, NULL, NULL);
    status |= fn_80458BEC(31, (void *)fn_80455D4C, (void *)fn_80455DC8, (void *)fn_80455E44);
    status |= fn_80460790(0, 31, NULL, NULL, NULL);
    status |= fn_804607D0(31, (void *)fn_80454AA4, (void *)fn_80454B20, (void *)fn_80454B9C);
    status |= fn_8045F2F8(0, 31, NULL, NULL, NULL);
    status |= fn_8045F338(31, (void *)fn_80454BC4, (void *)fn_80454C40, (void *)fn_80454CBC);
    status |= fn_8045B2CC(0, 31, NULL, NULL, NULL);
    status |= fn_8045B30C(31, (void *)fn_8045AF28, (void *)fn_8045AFA4, (void *)fn_8045B020);
    if ((s32)status < 0) {
        return 0;
    }
    return fn_80462884() != 0;
}

void *fn_80462130(u32 stream, u32 object) {
    u32 value = *(u32 *)(object + 4);
    u32 *target = (u32 *)(object + lbl_805F1508);
    if (value != 0) {
        fn_8048722C(value);
    }
    target[3] = (u32)stream;
    return (void *)stream;
}

u32 fn_80462184(u32 world, u8 *object) {
    u32 *target = (u32 *)(object + lbl_805F1508);
    if (target[3] != 0) {
        target[3] = 0;
        target[2] = 0;
        return world;
    }
    return 0;
}

u32 fn_804621B0(u8 *object) {
    return *(u32 *)(object + lbl_805F1508 + 0x0C);
}

void *fn_804621C0(u32 stream, u32 object) {
    u32 value = *(u32 *)(object + 4);
    u32 *target = (u32 *)(object + lbl_805F150C);
    if (value != 0) {
        fn_8048722C(value);
    }
    *(u32 *)target = stream;
    return (void *)stream;
}

void *fn_8046221C(void *stream, void *object_) {
    u32 *object = (u32 *)object_;
    u32 *target = (u32 *)((u8 *)object + lbl_805F150C);
    u32 value = object[5];
    if (value != 0) {
        fn_8048071C(value);
    }
    {
    u32 *link = ((u32 **)object)[0x19];
    u32 *sentinel = (u32 *)((u8 *)object + 0x64);
    while (link != sentinel) {
        u32 *node = link - 3;
        link = (u32 *)*link;
        if (node[2] == 0 || node[5] == 0) {
            continue;
        }
        {
        u32 value = node[3];
        *(u32 *)node[4] = value;
        value = node[4];
        *(u32 *)(node[3] + 4) = value;
        value = node[0];
        *(u32 *)node[1] = value;
        value = node[1];
        *(u32 *)(node[0] + 4) = value;
        ((void (*)(u32, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
            *(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset), node);
    }}
    *target = 0;
    return stream;
}}

u32 fn_804622F0(u8 *object) {
    return *(u32 *)(object + lbl_805F150C);
}

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
    target[1] = *(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 8);
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
        {
        u32 *link = (u32 *)object[9];
        link[0] = object[8];
        link = (u32 *)object[8];
        link[1] = object[9];
        fn_80456C54(object, (void *)fn_80461A00, stream);
        fn_80456D54(object, (void *)fn_80461A88, stream);
        fn_80456CD0(object, (void *)fn_80461B10, stream);
        *target = 0;
        return stream;
    }} else {
        u32 args[2];
        args[0] = 2;
        args[1] = fn_8047DC28(4);
        fn_8047DB84((RwError *)args);
        return NULL;
    }
}

void *fn_804624E8(void *stream, void *callback, void *data) {
    u32 *entry = (u32 *)((u8 *)stream + lbl_805F1508);
    u32 *current = (u32 *)entry[0];
    s32 count = entry[2];
    while (count != 0) {
        if (((u32 (*)(u32, void *))callback)(current[0], data) == 0) {
            return stream;
        }
        current++;
        count--;
    }
    return stream;
}

u32 fn_8046256C(u8 *object) {
    return *(u32 *)(object + lbl_805F1510);
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
        {
        u32 *link = (u32 *)(stream + 0x34);
        ((u32 *)object)[0x0D] = value;
        ((u32 *)object)[0x0E] = (u32)link;
        link[1] = object + 0x34;
        *(u32 *)(stream + 0x34) = object + 0x34;
    }}
    return (void *)stream;
}

void *fn_80462618(void *stream, void *object_) {
    u32 *object = (u32 *)object_;
    u32 *target = (u32 *)((u8 *)object + lbl_805F1514);
    *target = 0;
    {
    u32 *sentinel = (u32 *)((u8 *)object + 0x2C);
    u32 *link = ((u32 **)object)[0x0B];
    while (link != sentinel) {
        u32 *node = link - 3;
        link = (u32 *)*link;
        {
        u32 value = node[3];
        *(u32 *)node[4] = value;
        value = node[4];
        *(u32 *)(node[3] + 4) = value;
        value = node[0];
        *(u32 *)node[1] = value;
        value = node[1];
        *(u32 *)(node[0] + 4) = value;
        ((void (*)(u32, u32 *))*(u32 *)(RWENGINEBYTES + 0x11C))(
            *(u32 *)(RWENGINEBYTES + lbl_805F1518.globalsOffset + 4), node);
    }}
    {
    u32 value = object[0x0D];
    *(u32 *)object[0x0E] = value;
    value = object[0x0E];
    *(u32 *)(object[0x0D] + 4) = value;
    return stream;
}}}

u32 fn_804626EC(u8 *object) {
    return *(u32 *)(object + lbl_805F1514);
}

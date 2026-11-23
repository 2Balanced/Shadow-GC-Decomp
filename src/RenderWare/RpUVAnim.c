/* RenderWare: RpUVAnim (plugin 0x135), the plugin itself: the engine open and close callbacks
 * (free list, the UV animation dictionary schema and the two keyframe schemes), RpUVAnimPluginAttach
 * (fn_80453A78), the material extension's constructor, destructor, copy and stream callbacks,
 * the UV animation objects (create, destroy, add-ref, name, keyframe lists), and the material API
 * (apply update, set current time, add time, exists).
 *
 * 0x80453980-0x80454AA4 (.rodata 0x8051BD40-0x8051BD68, .data 0x8056D748-0x8056D7E0,
 * .sdata 0x805EDD88-0x805EDDA0, .sbss 0x805F1498-0x805F14A0, .sdata2 0x805FB0F8-0x805FB108,
 * .sbss2 0x805FB558-0x805FB560). RpUVAnimParam.c comes before it (see RpUVAnimKeyFrame.c for the
 * literal pools that separate the three UV animation files); RpWorldStream.c follows. The .data
 * holds the dictionary schema and both keyframe schemes, so the keyframe files reference them here;
 * fn_8045462C reaches this file's .rodata through its pool base, which is how its start at
 * 0x8051BD40 shows. The schema's name is a writable string in .sdata, as in RenderWare's other
 * files with strings (-str noreadonly).
 *
 * Built as C with cflags_rw_c plus -str noreadonly (configure.py).
 * fn_80453FB0 (the material stream read) matches with its name lookup and the stand-in animation
 * as inline helpers (the stand-in is inlined twice; its `{0}` node map is the .sbss2 initializer)
 * and the identity matrix written as chained assignments.
 * Floors: fn_80454294 and fn_804549DC test the eight interpolators in an unrolled loop whose copies
 * step the pointer (lwz 12(p); addi p,p,4); GC 1.3.2-2.7 fold every step into the offset, unrolled
 * by hand or by `#pragma opt_unroll_loops` (probes/RenderWare_RpUVAnim_W41b.py). fn_80453C04,
 * fn_80453E24 and fn_8045462C are wave-40 drafts.
 */
#include <dolphin/types.h>
#include "Game/rw/r2.h"

/* Keyframe callbacks of RpUVAnimKeyFrame.c and RpUVAnimParam.c. */
void fn_804530CC(void);
void fn_804531B0(void);
void fn_804532A4(void);
void fn_804532E4(void);
void fn_80453350(void);
void fn_804533E0(void);
void fn_8045345C(void);
void fn_80453500(void);
void fn_804535C8(void);
void fn_804536B4(void);
void fn_80453770(void);
void fn_80453818(void);
void fn_8045381C(void);
void fn_80453018(u32 *, u32 *, u32 *, u32 *, f32);

/* RtDict's own stream callbacks for UV animations. */
void fn_80477978(void);
void fn_804775CC(void);
void fn_80477860(void);

u32 *fn_80454618(u32 *object);
u32 fn_8045459C(u32 *object);
u32 fn_804548E4(u32 *object);

static R2ModuleInfo uvAnimModule = {0, 0};
static s32 uvAnimFreeListBlockSize = 128;
static s32 uvAnimFreeListPreallocBlocks = 1;

static u32 uvAnimMaterialOffset;
static void *uvAnimFreeList;

/* The UV animation dictionary schema (the game's resource loaders use it too). */
R2DictSchema lbl_8056D748 = {
    "UVAnim", 0x2B, 0x1B, 0x36000, NULL, NULL,
    (void *)fn_80454618, (void *)fn_8045459C, (void *)fn_804548E4,
    (void *)fn_80477978, (void *)fn_804775CC, NULL, (void *)fn_80477860,
};

/* The linear (matrix) keyframe scheme, 0x1C0. */
R2AnimInterpInfo lbl_8056D77C = {
    0x1C0, 0x20, 0x20,
    (void *)fn_804532E4, (void *)fn_804533E0, (void *)fn_80453350, (void *)fn_80453500,
    (void *)fn_8045345C, (void *)fn_804530CC, (void *)fn_804531B0, (void *)fn_804532A4, 0x44,
};

/* The parameterised keyframe scheme, 0x1C1. */
R2AnimInterpInfo lbl_8056D7AC = {
    0x1C1, 0x20, 0x20,
    (void *)fn_804535C8, (void *)fn_80453770, (void *)fn_804536B4, (void *)fn_8045381C,
    (void *)fn_80453818, (void *)fn_804530CC, (void *)fn_804531B0, (void *)fn_804532A4, 0x44,
};

/* The half-texel offsets the parameterised scheme rotates about (RpUVAnimParam.c uses them too). */
const float lbl_8051BD40[3] = {-0.5f, -0.5f, 0.0f};
const float lbl_8051BD4C[3] = {0.5f, 0.5f, 0.0f};

void *fn_8047FC04(u32, s32, u32, s32, void *, u32);
void fn_80479040(void *);
void fn_80477344(void *);
u32 fn_8047909C(u32 *);
void fn_8047FCDC(void *);
s32 fn_80485D78(u32, u32, void *, void *);
s32 fn_8045B2CC(u32, u32, void *, void *, void *);
s32 fn_8045B30C(u32, void *, void *, void *);
void fn_8047F958(void *);
void *fn_8047F994(void);
u32 fn_80477A8C(u32);
u32 fn_804779A8(u32 *);
u32 fn_804779DC(u32, u32);
void fn_80477BF0(u32 *, u32 *);
s32 fn_80477AB8(u32 *, u32 *);
void *fn_8047CEF0(void *, u32, u32, u32, u32);
s32 fn_8047CF50(void *, u32, u32 *, u32 *);
u32 fn_8047D868(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_80480D4C(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);
u32 fn_804790C4(u32 *);
u32 *fn_804790D4(u32, u32 *);
void fn_8047759C(u32 *);
u32 *fn_80477468(u32, u32, u32, f32);
void fn_803ACABC(u32 *, u32 *, u32);
void fn_8047EBA0(u32 *);
void fn_8047F59C(u32 *, u32 *, u32);
void fn_8047EFC8(u32 *, u32 *, u32, f32);
void fn_8047F72C(u32 *, u32 *, u32);
u32 *fn_804A2BE0(u32 *, u32 *, u32 *);
void fn_804782E8(u32 *, f32);
void fn_80477ED4(u32 *, f32);

void *fn_80453FB0(void *stream, s32 length, void *object);
void *fn_80453E24(void *stream, u32 length, u8 *object);
u32 fn_80454294(u32 *object);
u32 *fn_80453B64(u32 *object);
u32 *fn_80453C04(u32 *dest, u32 *source);
void *fn_80453B24(u8 *object);
u32 *fn_80453D08(u32 *object);
u32 *fn_80453980(u32 *object);
u32 *fn_80453A14(u32 *object);
u32 *fn_80454374(void *stream);
u32 *fn_8045449C(u32 *source, u32 count, u32 config, f32 duration, u32 *dest, s32 mode);
s32 fn_804549DC(u32 *object);



/* Chunk 0x135 constructor: build the class record, then the static sub-objects. */
u32 *fn_80453980(u32 *object)
{
    uvAnimFreeList = fn_8047FC04(0x44, uvAnimFreeListBlockSize, 4, uvAnimFreeListPreallocBlocks,
                                 uvAnimFreeList, 0x40135);
    if (uvAnimFreeList == NULL) {
        object = NULL;
    }
    fn_80479040(&lbl_8056D748);
    fn_80477344(&lbl_8056D77C);
    fn_80477344(&lbl_8056D7AC);
    uvAnimModule.numInstances++;
    return object;
}

/* Chunk 0x135 destructor. */
u32 *fn_80453A14(u32 *object)
{
    uvAnimModule.numInstances--;
    fn_8047909C((u32 *)&lbl_8056D748);
    if (uvAnimFreeList != NULL) {
        fn_8047FCDC(uvAnimFreeList);
        uvAnimFreeList = NULL;
    }
    return object;
}

/* Install the chunk 0x135 plugin: an 0x28 byte payload with four callbacks, plus
   the stream trio whose read and size halves live in the next unit. */
u32 fn_80453A78(void)
{
    if (fn_80485D78(0, 0x135, fn_80453980, fn_80453A14) < 0) {
        return 0;
    }
    uvAnimMaterialOffset = fn_8045B2CC(0x28, 0x135, fn_80453B24, fn_80453B64, fn_80453C04);
    { s32 streamResult = fn_8045B30C(0x135, fn_80453FB0, fn_80453E24, fn_80454294);
    s32 result = 0;
    if (streamResult >= 0) {
        if ((s32)uvAnimMaterialOffset >= 0) {
            result = 1;
        }
    }
    return result;
 }}


void *fn_80453B24(u8 *object) {
    memset(object + uvAnimMaterialOffset, 0, 0x28);
    return object;
}

/* Chunk 0x135 destructor callback: the two frames, then release the nodes. Retail
   never clears the eight node slots here. */
u32 *fn_80453B64(u32 *object)
{
    u32 *data = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    { u32 i; for (i = 0; i < 2; i++) {
        if (data[i] != 0) {
            fn_8047F958((void *)data[i]);
        }
        data[i] = 0;
    } }
    { u32 node;
    { u32 j; for (j = 0; j < 8; j++) {
        node = data[2 + j];
        if (node != 0) {
            fn_8045459C((u32 *)*(u32 *)node);
            fn_80477A8C(node);
        }
    } }
    return object;
 }}

/* Chunk 0x135 copy: every node of the source gets a fresh list holding the same
   element count; on failure the copies made so far are released again. */
u32 *fn_80453C04(u32 *dest, u32 *source)
{
    u32 *destData = (u32 *)((u8 *)dest + uvAnimMaterialOffset);
    u32 *sourceData = (u32 *)((u8 *)source + uvAnimMaterialOffset);
    u32 slot = 0;
    for (; slot < 8; slot++) {
        u32 node = sourceData[2 + slot];
        if (node == 0) {
            destData[2 + slot] = 0;
            continue;
        }
        { u32 list = *(u32 *)node;
        u32 copy = fn_804779DC(fn_804779A8((u32 *)list), *(u32 *)(*(u32 *)list + 4));
        destData[2 + slot] = copy;
        if (copy == 0) {
            break;
        }
        fn_80477BF0((u32 *)copy, (u32 *)node);
        /* Not reached when the copy above failed, which is what retail does. */
        fn_80454618((u32 *)list);
     }}
    if (slot == 8) {
        return dest;
    }
    { u32 i; for (i = 0; i < slot; i++) {
        u32 copy = destData[2 + i];
        if (copy != 0) {
            fn_8045459C((u32 *)*(u32 *)copy);
            fn_80477A8C(copy);
        }
    } }
    return 0;
}

/* Create or drop the frame for each of the two slots by looking for a node whose
   element list names that slot. The argument is the payload block itself. */
u32 *fn_80453D08(u32 *object)
{
    { u32 slot; for (slot = 0; slot < 2; slot++) {
        u32 index = 0;
        u32 element = 0;
        s32 found = 0;
        while (index < 8) {
            u32 node = object[2 + index];
            if (node != 0) {
                u32 list = *(u32 *)node;
                u32 *elements = *(u32 **)((u8 *)list + 0x14);
                if (slot == *(u32 *)((u8 *)elements + 0x20 + 4 * element)) {
                    found = 1;
                    break;
                }
                element++;
                if (element >= *(u32 *)((u8 *)node + 0x2C)) {
                    element = 0;
                    index++;
                }
            } else {
                index++;
            }
        }
        if (found && object[slot] == 0) {
            object[slot] = (u32)fn_8047F994();
            if (object[slot] == 0) {
                return 0;
            }
        } else if (!found && object[slot] != 0) {
            fn_8047F958((void *)object[slot]);
            object[slot] = 0;
        }
    } }
    return object;
}

/* Chunk 0x135 stream write: a presence mask for the eight node slots, then every
   node's 0x20 byte chunk. The length handed to the header call is not used. */
void *fn_80453E24(void *stream, u32 length, u8 *object)
{
    u32 *slots = (u32 *)(object + uvAnimMaterialOffset + 0x08);
    u32 mask = 0;
    u32 size = 4;
    { u32 pass; for (pass = 0; pass < 2; pass++) {
        { u32 bit; for (bit = pass * 4; bit < pass * 4 + 4; bit++) {
            if (slots[bit] != 0) {
                mask |= 1u << bit;
                size += 0x20;
            }
        } }
    } }
    if (fn_8047CEF0(stream, 1, size, 0x37002, 0x37) == 0) {
        return 0;
    }
    if (fn_8047D458(stream, &mask, 4) == 0) {
        return 0;
    }
    { u32 i; for (i = 0; i < 8; i++) {
        if (slots[i] != 0) {
            /* Retail never checks the result of the node write. */
            fn_80480ED0(stream, (void *)fn_804548E4((u32 *)*(u32 *)slots[i]), 0x20);
        }
    } }
    return stream;
}


/* A stand-in for an animation the dictionary does not hold: one node, two identity keyframes a
 * second apart (inlined twice below). */
static inline u32 *UVAnimCreateDefault(u32 *name) {
    u32 nodeToUVChannelMap[1] = {0};
    u32 *anim;
    R2Matrix matrix;
    u8 *first;
    u8 *second;

    anim = fn_8045449C(name, 1, 2, 1.0f, nodeToUVChannelMap, 0);
    if (anim == NULL) {
        return NULL;
    }
    first = (u8 *)anim[4];
    second = first + ((u32 *)anim[0])[2];
    matrix.right.x = matrix.up.y = matrix.at.z = 1.0f;
    matrix.right.y = matrix.right.z = matrix.up.x = 0.0f;
    matrix.up.z = matrix.at.x = matrix.at.y = 0.0f;
    matrix.pos.x = matrix.pos.y = matrix.pos.z = 0.0f;
    matrix.flags |= 0x00020003;
    fn_80453018(anim, (u32 *)first, NULL, (u32 *)&matrix, 0.0f);
    fn_80453018(anim, (u32 *)second, (u32 *)first, (u32 *)&matrix, 1.0f);
    return anim;
}

/* Reads an animation name and finds the animation in the current dictionary. */
static inline void *UVAnimStreamReadName(void *stream, u32 **animOut) {
    char name[32];
    u32 dict;
    u32 *anim;

    if (fn_80480D4C(stream, name, 32) == 0) {
        return NULL;
    }
    dict = fn_804790C4((u32 *)&lbl_8056D748);
    if (dict != 0) {
        *animOut = fn_804790D4(dict, (u32 *)name);
        if (*animOut != NULL) {
            fn_80454618(*animOut);
        } else {
            *animOut = UVAnimCreateDefault((u32 *)name);
        }
    } else {
        *animOut = UVAnimCreateDefault((u32 *)name);
    }
    if (*animOut == NULL) {
        return NULL;
    }
    return stream;
}

/* Material stream read callback: the mask of the eight animation slots, then each animation's
 * name; every named animation gets an interpolator. */
void *fn_80453FB0(void *stream, s32 length, void *object) {
    u32 *slot;
    u32 *slots;
    u32 i;
    u32 mask;

    slots = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    if (fn_8047CF50(stream, 1, NULL, NULL) == 0 || fn_8047D868(stream, &mask, 4) == 0) {
        return NULL;
    }
    for (i = 0, slot = slots; i < 8; i++, slot++) {
        if (mask & (1 << i)) {
            u32 *anim;

            if (UVAnimStreamReadName(stream, &anim) == NULL) {
                return NULL;
            }
            if (anim != NULL) {
                if ((slot[2] = fn_804779DC(fn_804779A8(anim), ((u32 *)anim[0])[1])) == 0 ||
                    fn_80477AB8((u32 *)slot[2], anim) == 0) {
                    fn_8045459C(anim);
                    return NULL;
                }
            }
        }
    }
    if (fn_80453D08(slots) != NULL) {
        return stream;
    }
    return NULL;
}


u32 fn_80454294(u32 *object) {
    if (fn_804549DC(object) == 0) {
        goto failure;
    }
    { u32 *entry = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    u32 size = 16;
    if (entry[2] != 0) {
        size = 48;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    goto success;
failure:
    return 0;
success:
    return size;
 }}

u32 *fn_80454374(void *stream) {
    u32 *object = ((u32 *(*)(u32 *, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x118))(
        (u32 *)uvAnimFreeList, 0x00030135);
    if (object == NULL) {
        return NULL;
    }
    if (fn_80480D4C(stream, object, 32) == 0) {
        return NULL;
    }
    if (fn_8047D868(stream, object + 8, 32) == 0) {
        return NULL;
    }
    object[16] = 1;
    return object;
}

u32 *fn_8045441C(u32 *object, void *stream) {
    if (fn_80480ED0(stream, object, 32) == 0) {
        return NULL;
    }
    if (fn_8047D458(stream, object + 8, 32) != 0) {
        return object;
    }
    return NULL;
}

u32 fn_80454494(void) {
    return 0x40;
}


u32 *fn_8045449C(u32 *source, u32 count, u32 config, f32 duration, u32 *dest, s32 mode) {
    u32 *owner = NULL;
    switch (mode) {
    case 0:
        owner = fn_80477468(lbl_8056D77C.typeID, config, 0, duration);
        break;
    case 1:
        owner = fn_80477468(lbl_8056D7AC.typeID, config, 0, duration);
        break;
    default:
        break;
    }
    if (owner == NULL) {
        goto failure;
    }
    { u32 *object = ((u32 *(*)(u32 *, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x118))(
        (u32 *)uvAnimFreeList, 0x00030135);
    if (object == NULL) {
        return NULL;
    }
    fn_803ACABC(object, source, 32);
    ((u8 *)object)[31] = 0;
    memcpy(object + 8, dest, count * 4);
    object[16] = 1;
    { u32 *result = owner;
    owner[5] = (u32)object;
    goto success;
failure:
    result = NULL;
success:
    return result;
 } }}

u32 fn_8045459C(u32 *object) {
    u32 *nested = (u32 *)object[5];
    nested[16] = nested[16] - 1;
    { u32 released;
    if (nested[16] != 0) {
        released = 0;
    } else {
        ((void (*)(u32 *, u32 *))*(u32 *)((u8 *)lbl_805F15B4 + 0x11C))(
            (u32 *)uvAnimFreeList, (u32 *)object[5]);
        released = 1;
    }
    if ((s32)released != 0) {
        fn_8047759C(object);
    }
    return 1;
 }}

u32 *fn_80454618(u32 *object) {
    ((u32 *)object[5])[0x10]++;
    return object;
}



u32 *fn_8045462C(u32 *object) {
    u32 *base = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    f32 first = 1.0f;
    f32 second = 0.0f;
    u32 *record = (u32 *)base[0];
    if (record != NULL) {
        ((f32 *)record)[10] = first;
        ((f32 *)record)[5] = first;
        ((f32 *)record)[0] = first;
        ((f32 *)record)[4] = second;
        ((f32 *)record)[2] = second;
        ((f32 *)record)[1] = second;
        ((f32 *)record)[9] = second;
        ((f32 *)record)[8] = second;
        ((f32 *)record)[6] = second;
        ((f32 *)record)[14] = second;
        ((f32 *)record)[13] = second;
        ((f32 *)record)[12] = second;
        record[3] |= 0x00020003;
    }
    record = (u32 *)base[1];
    if (record != NULL) {
        ((f32 *)record)[10] = first;
        ((f32 *)record)[5] = first;
        ((f32 *)record)[0] = first;
        ((f32 *)record)[4] = second;
        ((f32 *)record)[2] = second;
        ((f32 *)record)[1] = second;
        ((f32 *)record)[9] = second;
        ((f32 *)record)[8] = second;
        ((f32 *)record)[6] = second;
        ((f32 *)record)[14] = second;
        ((f32 *)record)[13] = second;
        ((f32 *)record)[12] = second;
        record[3] |= 0x00020003;
    }
    { u32 *current = base;
    { s32 i; for (i = 0; i < 8; i++) {
        u32 *entry = (u32 *)current[2];
        if (entry != NULL) {
            u32 *config = (u32 *)entry[0];
            u32 stride = entry[9];
            u32 callback = entry[15];
            s32 count = (s32)entry[11];
            u32 *data = entry + 19;
            u32 *types = (u32 *)(config[5] + 32);
            { s32 j; for (j = 0; j < count; j++) {
                u32 type = types[0];
                types++;
                if (type < 2) {
                    u32 *target = (u32 *)base[type];
                    if (target != NULL) {
                        u32 buffer[15];
                        if (callback == (u32)fn_804532E4) {
                            ((f32 *)buffer)[0] = ((f32 *)data)[2];
                            ((f32 *)buffer)[1] = ((f32 *)data)[3];
                            ((f32 *)buffer)[2] = 0.0f;
                            ((f32 *)buffer)[4] = ((f32 *)data)[4];
                            ((f32 *)buffer)[5] = ((f32 *)data)[5];
                            ((f32 *)buffer)[6] = 0.0f;
                            ((f32 *)buffer)[8] = 0.0f;
                            ((f32 *)buffer)[9] = 0.0f;
                            ((f32 *)buffer)[10] = 0.0f;
                            ((f32 *)buffer)[12] = ((f32 *)data)[6];
                            ((f32 *)buffer)[13] = ((f32 *)data)[7];
                            ((f32 *)buffer)[14] = 0.0f;
                            fn_8047EBA0(buffer);
                        } else if (callback == (u32)fn_804535C8) {
                            R2V3d extra = {0.0f, 0.0f, 1.0f};
                            ((f32 *)buffer)[0] = ((f32 *)data)[3];
                            ((f32 *)buffer)[1] = ((f32 *)data)[5];
                            ((f32 *)buffer)[2] = 0.0f;
                            ((f32 *)buffer)[4] = 0.0f;
                            ((f32 *)buffer)[5] = ((f32 *)data)[4];
                            ((f32 *)buffer)[6] = 0.0f;
                            ((f32 *)buffer)[8] = 0.0f;
                            ((f32 *)buffer)[9] = 0.0f;
                            ((f32 *)buffer)[10] = 0.0f;
                            ((f32 *)buffer)[12] = ((f32 *)data)[6];
                            ((f32 *)buffer)[13] = ((f32 *)data)[7];
                            ((f32 *)buffer)[14] = 0.0f;
                            fn_8047EBA0(buffer);
                            fn_8047F59C(buffer, (u32 *)lbl_8051BD40, 2);
                            { f32 value = 180.0f * (((f32 *)data)[2] / 3.1415927f);
                            fn_8047EFC8(buffer, (u32 *)&extra, 2, value);
                            fn_8047F59C(buffer, (u32 *)lbl_8051BD4C, 2);
                         }} else {
                            ((void (*)(u32 *, u32 *))callback)(buffer, data);
                        }
                        fn_8047F72C(target, buffer, 1);
                    }
                }
                data = (u32 *)((u8 *)data + stride);
            } }
        }
        current++;
    } }
    return fn_804A2BE0(object, (u32 *)base[0], (u32 *)base[1]);
 }}

u32 fn_804548E4(u32 *object) {
    return object[5];
}

u32 *fn_804548EC(u32 *object, f32 value) {
    u32 *entry = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    { u32 i; for (i = 0; i < 8; i++) {
        u32 *target = (u32 *)entry[2];
        if (target != NULL) {
            fn_804782E8(target, value);
        }
        entry++;
    } }
    return object;
}

u32 *fn_80454964(u32 *object, f32 value) {
    u32 *entry = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    { u32 i; for (i = 0; i < 8; i++) {
        u32 *target = (u32 *)entry[2];
        if (target != NULL) {
            fn_80477ED4(target, value);
        }
        entry++;
    } }
    return object;
}


s32 fn_804549DC(u32 *object) {
    u32 *entry = (u32 *)((u8 *)object + uvAnimMaterialOffset);
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    return 0;
}

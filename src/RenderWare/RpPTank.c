/* RenderWare: RpPTank (plugin 0x12F, rwID_PTANKPLUGIN), 0x80447598..0x804485BC: the particle tank's
 * platform-independent part: the atomic extension's callbacks and the plugin's attach, laying out
 * the tank's particle arrays, creating a tank atomic, the render callback hook and the array
 * locks. The GameCube back end (pipeline, vertex writers) starts at 0x804485BC in its own file: its
 * .sbss starts at 0x805F1450 and its .sdata2 pool (0x805FB068) has 1.0f again. This file's data:
 * .rodata 0x8051BCA8..0x8051BCF0 (the array strides, and the defaults a new tank copies, padded
 * to 8 bytes), .data 0x8056D6F8 (the default platform callbacks), .sbss 0x805F1448..0x805F1450,
 * .sdata2 0x805FB050..0x805FB068 and .sbss2 0x805FB550..0x805FB558 (the defaults' small parts).
 * Built as C without lmw/stmw at -O4,p, inlining only what is declared inline, in definition order
 * (cflags_rw_c; cflags_rw_r3, _r5 and _r6 give the same code here, 9 of 12 functions). Names are inferred from behavior. NonMatching: fn_804476B8, fn_80447AF8 and
 * fn_804480C8 (drafts: retail builds the new tank's defaults from initialized locals, which is
 * where its .sdata2 and .sbss2 objects come from). */
#include <dolphin/types.h>
#include "Game/rw/r1.h"

extern u8 *lbl_805F15B4;

/* C lays out tentative definitions in reverse order. */
s32 lbl_805F144C; /* the engine plugin offset */
s32 lbl_805F1448; /* the atomic extension offset */

typedef void (*R1Callback)(void);

void fn_804485BC(void);
void fn_80448610(void);
void fn_80448850(void);
void fn_80448A1C(void);
void fn_80448A24(void);
void fn_8045B270(u32, u32);
s32 fn_80458B6C(u32, u32, R1Callback, R1Callback, void *);
s32 fn_80485D78(u32, u32, R1Callback, R1Callback);
s32 fn_80456DD8(void);
void fn_80456FD8(void);
void fn_80455EB4(void);

/* 0x8051BCA8: the stride of each of the nine particle arrays. */
static const u32 s_strides[9] = {0xC, 0x40, 0xC, 8, 4, 0x10, 4, 0x10, 0x20};
/* 0x8051BCCC */
static const u32 s_defaults[8] = {0, 0, 0, 0x3F800000, 0x3F800000, 0x3F800000, 0x3F800000, 0};

/* 0x8056D6F8: the platform's callbacks a tank is created with. */
static u32 s_defaultCallbacks[4] = {0, (u32)fn_80448850, (u32)fn_80448A1C, (u32)fn_80448A24};

void fn_80447598(u8 *base) {
    *(u32 *)(base + lbl_805F1448) = 0;
}

/* Plugin 0x12F's destructor: frees the object's plugin data. */
void *fn_804475A8(void *object, s32 offset, s32 size) {
    if (*(u32 **)((u8 *)object + lbl_805F1448) != NULL) {
        fn_8045B270(**(u32 **)(*(u8 **)((u8 *)object + 0x18) + 0x20), 0);
        if ((*(u32 **)((u8 *)object + lbl_805F1448))[3] != 0) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(
                (void *)(*(u32 **)((u8 *)object + lbl_805F1448))[3]);
        }
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(*(u32 **)((u8 *)object + lbl_805F1448));
        *(u32 **)((u8 *)object + lbl_805F1448) = NULL;
    }
    return object;
}

u32 fn_80447640(void) {
    s32 result = fn_80485D78(4, 0x12F, (R1Callback)fn_804485BC, (R1Callback)fn_80448610);
    lbl_805F144C = result;
    if (result >= 0) {
        lbl_805F1448 = fn_80458B6C(4, 0x12F, (R1Callback)fn_80447598, (R1Callback)fn_804475A8, NULL);
        return ((u32)lbl_805F1448 >> 31) ^ 1;
    }
    return 0;
}

static inline u32 RwCpp09Process(u32 *object, u32 flags, int multiply) {
    static const u16 indices[9] = {0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F, 0x21, 0x23};
    static const u16 sizes[9] = {0xC, 0x40, 0xC, 8, 4, 0x10, 4, 0x10, 0x20};
    static const u16 bits[9] = {1, 8, 0x10, 4, 2, 0x40, 0x20, 0x80, 0x100};
    const u32 *table = s_strides;
    u32 result = 0;
    u32 i;
    u32 *raw;
    u32 base;
    object[0x28] &= ~0x1FF;
    object[0x26] = 0;
    object[0x27] = 0;
    if (multiply) object[0xF] = 0;

    for (i = 0; i < 9; i++) {
        if ((flags & bits[i]) == 0) continue;
        object[0x26]++;
        object[0x28] |= bits[i];
        if (i == 0) {
            if (multiply) result = object[0] * table[0];
        } else {
            result += multiply ? object[0] * table[i] : table[i];
        }
    }

    if (!multiply) {
        object[0x27] = (result + 3) & ~3;
        result = object[0] * object[0x27];
    }
    raw = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(result + 0x10, 0x3012F);
    if (raw == NULL) return 0;
    base = ((u32)raw + 0xF) & ~0xF;
    for (i = 0; i < 9; i++) {
        if ((flags & bits[i]) == 0) continue;
        object[indices[i]] = base + result;
        object[indices[i] + 1] = sizes[i];
        if (i != 8) result += multiply ? object[0] * table[i] : table[i];
    }
    object[0x3] = (u32)raw;
    return 1;
}

/* Lays the arrays out one after the other. NonMatching draft. */
u32 fn_804476B8(u32 *object, u32 flags, void *argument) {
    return RwCpp09Process(object, flags, 1);
}

/* Lays the arrays out interleaved. NonMatching draft. */
u32 fn_80447AF8(u32 *object, u32 flags, void *argument) {
    return RwCpp09Process(object, flags, 0);
}

u32 fn_80447EB4(u32 flags) {
    int bit0 = flags & 1;
    int bit2 = (flags >> 2) & 1;
    int bit3 = (flags >> 3) & 1;
    int bit5 = (flags >> 5) & 1;
    int bit6 = (flags >> 6) & 1;
    int bit7 = (flags >> 7) & 1;
    int bit8 = (flags >> 8) & 1;
    int bit15 = (flags >> 15) & 1;
    int bit17 = (flags >> 17) & 1;
    int bit18 = (flags >> 18) & 1;
    int bit19 = (flags >> 19) & 1;
    int bit20 = (flags >> 20) & 1;

    if (bit3 == 1 && bit15 == 1) {
        flags &= 0x8000;
    }
    if (bit5 == 1 && bit17 == 1) {
        flags &= 0x20000;
    }
    if (bit6 == 1 && bit18 == 1) {
        flags &= 0x40000;
    }
    if (bit7 == 1 && bit19 == 1) {
        flags &= 0x80000;
    }
    if (bit8 == 1 && bit20 == 1) {
        flags &= 0x100000;
    }
    if (bit3 == 1 && bit0 == 1) {
        flags &= ~1;
    }
    if (bit3 == 1 && bit2 == 1) {
        flags &= ~4;
    }
    if (bit3 == 1 && bit5 == 1) {
        flags &= ~0x20;
    }
    if (bit3 == 0 && bit0 == 0) {
        flags |= 1;
    }
    if ((flags & 0x30000000) == 0) {
        flags |= 0x20000000;
    }
    return flags;
}

/* Runs plugin 0x12F's callbacks for the object: the first (then clears its argument), the
 * second, and the third when the second returned 1 (or there is no second). */
void *fn_80447FC0(void *object) {
    int result = 1;
    u32 *data = *(u32 **)((u8 *)object + lbl_805F1448);
    if ((s32)data[1] > 0) {
        if (data[7] != 0) {
            ((void (*)(void *, void *, u32, u32))data[7])(object, data + 0x12, data[1], data[0x10]);
            data[0x10] = 0;
        }
        if (data[8] != 0) {
            result = ((int (*)(void *, void *, u32))data[8])(object, data + 0x12, data[1]);
        }
        if (result == 1) {
            ((void (*)(void *))data[4])(object);
        }
    }
    return object;
}

void *fn_804480C8(void *parent, u32 flags, void *argument, void *config);

void *fn_80448074(u32 *object, u32 value, void *argument) {
    return fn_804480C8(object, fn_80447EB4(value), argument, s_defaultCallbacks);
}

/* Creates the tank's data for an atomic. NonMatching draft. */
void *fn_804480C8(void *parent, u32 flags, void *argument, void *config) {
    u32 *object;
    u32 *owner;
    u32 result;
    const u32 *table;
    u32 *ownerData;
    if (fn_80456DD8() == 0) {
        fn_80456FD8();
        return NULL;
    }
    object = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(0x138, 0x3012F);
    if (object == NULL) {
        fn_80456FD8();
        return NULL;
    }

    owner = (u32 *)parent;
    object[4] = owner[0x12];
    owner[0x12] = (u32)fn_80447FC0;
    if (owner[0x12] == 0) owner[0x12] = (u32)fn_80455EB4;
    object[0] = (u32)parent;
    object[1] = 0;
    object[0x28] = flags;
    object[0x11] = (u32)argument;
    object[0x10] = 0xFFFFFFFF;
    object[0xF] = 0;
    object[3] = 0;
    *(u32 **)((u8 *)owner + lbl_805F1448) = object;
    memcpy(object + 5, config, 0x10);

    result = 0;
    if (object[5] != 0) {
        object[3] = ((u32 (*)(void *, u32, u32, void *))object[5])(object + 0x12, object[0], flags, argument);
    } else if ((flags & 0x20000000) != 0) {
        result = fn_80447AF8(object, flags, argument);
    } else {
        result = fn_804476B8(object, flags, argument);
    }
    if (result != 1 && object[3] == 0) {
        fn_80456FD8();
        return NULL;
    }
    if (object[6] != 0 &&
        ((u32 (*)(void *, void *, u32, u32, void *))object[6])(owner, object + 0x12, object[0], flags,
                                                               argument) != 1) {
        fn_80456FD8();
        return NULL;
    }

    table = s_defaults;
    ownerData = (u32 *)owner[6];
    fn_8045B270(*(u32 *)(ownerData[8] + 0), 0);
    object[0x2E] = (u32)config;
    object[0x2F] = (u32)argument;
    object[0x2C] = (u32)parent;
    object[0x2D] = result;
    *(float *)(object + 0x30) = 0.0f;
    *(float *)(object + 0x48) = 1.0f;
    *(float *)(object + 0x43) = 1.0f;
    *(float *)(object + 0x3E) = 1.0f;
    *(float *)(object + 0x42) = 0.0f;
    *(float *)(object + 0x40) = 0.0f;
    *(float *)(object + 0x3F) = 0.0f;
    *(float *)(object + 0x47) = 0.0f;
    *(float *)(object + 0x46) = 0.0f;
    *(float *)(object + 0x44) = 0.0f;
    *(float *)(object + 0x4C) = 0.0f;
    *(float *)(object + 0x4B) = 0.0f;
    *(float *)(object + 0x4A) = 0.0f;
    object[0x41] = flags | 0x20003;
    object[0x31] = flags;
    object[0x32] = flags;
    object[0x33] = flags;
    object[0x34] = flags;
    object[0x35] = flags;
    object[0x36] = table[0];
    object[0x37] = table[1];
    object[0x38] = table[2];
    object[0x39] = table[3];
    object[0x3A] = table[4];
    object[0x3B] = table[5];
    object[0x3C] = table[6];
    object[0x3D] = table[7];
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(0xA, object + 0x29);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(0xB, object + 0x2A);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(0xC, object + 0x2B);
    return owner;
}

void fn_804483E8(void) {
    fn_80456FD8();
}

/* A lock of one of the object's arrays: data and stride. */
typedef struct R1Pair {
    u32 m_first;
    u32 m_second;
} R1Pair;

/* Copies the object's (plugin 0x12F) pair for `type` (one bit) to `out`; with bit 30 of `flags`
 * set, a present pair also marks the type in the object's flags. 0 when the type is not set. */
int fn_80448408(void *object, R1Pair *out, u32 type, u32 flags) {
    u32 *data = *(u32 **)((u8 *)object + lbl_805F1448);
    if ((data[0x28] & type) == 0) {
        out->m_first = 0;
        out->m_second = 0;
        return 0;
    }
    switch (type) {
    case 1:
        *out = *(R1Pair *)(data + 0x13);
        break;
    case 2:
        *out = *(R1Pair *)(data + 0x1B);
        break;
    case 4:
        *out = *(R1Pair *)(data + 0x19);
        break;
    case 8:
        *out = *(R1Pair *)(data + 0x15);
        break;
    case 0x10:
        *out = *(R1Pair *)(data + 0x17);
        break;
    case 0x20:
        *out = *(R1Pair *)(data + 0x1F);
        break;
    case 0x40:
        *out = *(R1Pair *)(data + 0x1D);
        break;
    case 0x80:
        *out = *(R1Pair *)(data + 0x21);
        break;
    case 0x100:
        *out = *(R1Pair *)(data + 0x23);
        break;
    }
    if ((s32)data[0x27] != 0) {
        out->m_second = data[0x27];
    }
    if (out->m_first != 0 && (flags & 0x40000000) == 0x40000000) {
        (*(u32 **)((u8 *)object + lbl_805F1448))[0xF] |= flags | type;
    }
    return 1;
}

void *fn_80448598(void *object) {
    u32 *data = *(u32 **)((u8 *)object + lbl_805F1448);
    data[0x10] |= data[0x0F];
    data[0x0F] = 0;
    return object;
}

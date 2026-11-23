/* RenderWare core: the vector module (0x8048222C-0x80482984).
 * RwV3dNormalize, RwV3dLength, _rwSqrt and _rwInvSqrt (table lookups), RwV2dLength, then
 * RwV3dTransformPoint (0x80482458), RwV3dTransformPoints (0x804824A4) and RwV3dTransformVector
 * (0x80482504), which retail has as hand-written paired-single assembly (psq_l/ps_madds0/ps_madds1/
 * psq_st, no compiler prologue): they stay unwritten here, so this unit cannot link. Then the module
 * close and open, which build the two mantissa tables of 0x800 entries for odd and even exponents
 * with the C library's inline sqrtf. The file owns the module globals (.sbss) and its constant pool
 * (.sdata2 0x805FB2B0-0x805FB2D0). The two table builders are static functions defined first: the
 * module open inlines them, and their own copies (dead-stripped in retail) put 1.0f at the head of
 * the pool, as in retail.
 * Boundary: the plugin registry ends at 0x8048222C; the resource heap after 0x80482984 has no data
 * and is its own file. C, cflags_rw_core (configure.py).
 * NonMatching: the three assembly routines above, and fn_804825EC (_rwVectorOpen) is one
 * instruction longer than retail around the inlined sqrtf. */
#include "Game/rw/r6.h"

extern s32 lbl_805EDCC8[]; /* __float_nan */

f32 fn_80482340(f32 value);
f32 fn_80482394(f32 value);

static RwModuleInfo vectorModule;

#define RWVECTORGLOBAL(var) (RWGLOBALAT(RwSqrtTables, vectorModule.globalsOffset).var)

/* The C library's inline sqrtf: two Newton steps on the reciprocal square root estimate. */
static inline f32 Sqrt(f32 x) {
    if (x > 0.0f) {
        f64 guess = __frsqrte((f64)x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        guess = 0.5 * guess * (3.0 - guess * guess * x);
        return (f32)(x * guess);
    } else if (x < 0.0) {
        return *(f32 *)lbl_805EDCC8;
    }
    return x;
}

/* Both tables hold 0x800 mantissas for odd exponents after 0x800 for even ones. */
static RwBool SqrtTableCreate(void) {
    u32 *table = (u32 *)RwEngineInstance->memoryFuncs.rwmalloc(0x4000, 0x40401);

    if (!table) {
        RWERROR((0x80000013, 0x4000));
        return FALSE;
    } else {
        volatile RwSplitBits spin;
        volatile RwSplitBits root;
        u32 *entry;
        s32 i;

        spin.nReal = 1.0f;
        entry = table + 0x800;
        for (i = 0; i < 0x800; i++) {
            volatile u32 bits;
            root.nReal = Sqrt(spin.nReal);
            bits = root.nUInt - 0x1FC00000;
            *entry++ = bits;
            spin.nUInt += 0x1000;
        }
        entry = table;
        for (i = 0; i < 0x800; i++) {
            volatile RwSplitBits root2;
            volatile u32 bits;
            root2.nReal = Sqrt(spin.nReal);
            bits = root2.nUInt - 0x20000000;
            *entry++ = bits;
            spin.nUInt += 0x1000;
        }
        RWVECTORGLOBAL(sqrtTab) = table;
    }
    return TRUE;
}

static RwBool InvSqrtTableCreate(void) {
    u32 *table = (u32 *)RwEngineInstance->memoryFuncs.rwmalloc(0x4000, 0x40401);

    if (!table) {
        RWERROR((0x80000013, 0x4000));
        return FALSE;
    } else {
        volatile RwSplitBits spin;
        volatile RwSplitBits root;
        u32 *entry;
        s32 i;

        spin.nReal = 1.0f;
        entry = table + 0x800;
        for (i = 0; i < 0x800; i++) {
            volatile u32 bits;
            root.nReal = 1.0f / Sqrt(spin.nReal);
            bits = root.nUInt - 0x20000000;
            *entry++ = bits;
            spin.nUInt += 0x1000;
        }
        entry = table;
        for (i = 0; i < 0x800; i++) {
            volatile RwSplitBits root2;
            volatile u32 bits;
            root2.nReal = 1.0f / Sqrt(spin.nReal);
            bits = root2.nUInt - 0x1FC00000;
            *entry++ = bits;
            spin.nUInt += 0x1000;
        }
        RWVECTORGLOBAL(invSqrtTab) = table;
    }
    return TRUE;
}

/* RwV3dNormalize. */
f32 fn_8048222C(RwV3d *out, const RwV3d *in) {
    f32 length2 = in->x * in->x + in->y * in->y + in->z * in->z;
    f32 length = fn_80482340(length2);
    f32 recip = fn_80482394(length2);

    out->x = in->x * recip;
    out->y = in->y * recip;
    out->z = in->z * recip;
    if (length <= 0.0f) {
        RWERROR((0x19));
    }
    return length;
}

/* RwV3dLength. */
f32 fn_80482300(const RwV3d *in) {
    return fn_80482340(in->x * in->x + in->y * in->y + in->z * in->z);
}

/* _rwSqrt: mantissa table lookup. */
f32 fn_80482340(f32 value) {
    volatile RwSplitBits bits;

    bits.nReal = value;
    if (bits.nUInt != 0) {
        bits.nUInt += 0x800;
        bits.nUInt = ((bits.nUInt >> 1) & 0x3FC00000) +
                     RWVECTORGLOBAL(sqrtTab)[(bits.nUInt >> 12) & 0xFFF];
    }
    return bits.nReal;
}

/* _rwInvSqrt. */
f32 fn_80482394(f32 value) {
    volatile RwSplitBits bits;

    bits.nReal = value;
    if (bits.nUInt != 0) {
        bits.nUInt += 0x800;
        bits.nUInt = ((~bits.nUInt >> 1) & 0x3FC00000) +
                     RWVECTORGLOBAL(invSqrtTab)[(bits.nUInt >> 12) & 0xFFF];
    }
    return bits.nReal;
}

/* RwV2dLength. */
f32 fn_804823F0(const RwV2d *in) {
    volatile RwSplitBits bits;

    bits.nReal = in->x * in->x + in->y * in->y;
    if (bits.nUInt != 0) {
        bits.nUInt += 0x800;
        bits.nUInt = ((bits.nUInt >> 1) & 0x3FC00000) +
                     RWVECTORGLOBAL(sqrtTab)[(bits.nUInt >> 12) & 0xFFF];
    }
    return bits.nReal;
}

/* _rwVectorClose. */
void *fn_80482548(void *instance) {
    if (RWVECTORGLOBAL(invSqrtTab)) {
        RwEngineInstance->memoryFuncs.rwfree(RWVECTORGLOBAL(invSqrtTab));
        RWVECTORGLOBAL(invSqrtTab) = NULL;
    }
    if (RWVECTORGLOBAL(sqrtTab)) {
        RwEngineInstance->memoryFuncs.rwfree(RWVECTORGLOBAL(sqrtTab));
        RWVECTORGLOBAL(sqrtTab) = NULL;
    }
    vectorModule.numInstances--;
    return instance;
}

/* _rwVectorOpen. */
void *fn_804825EC(void *instance, s32 offset) {
    vectorModule.globalsOffset = offset;
    if (!SqrtTableCreate()) {
        return NULL;
    }
    if (!InvSqrtTableCreate()) {
        return NULL;
    }
    vectorModule.numInstances++;
    return instance;
}

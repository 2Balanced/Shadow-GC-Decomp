/* Dolphin SDK GX: lighting.
 *
 * Light objects hold a colour, the angle (spot) and distance attenuation coefficients, a position
 * and a direction; the initialisers derive the coefficients from friendlier parameters and
 * GXLoadLightObjImm sends a whole object to one of the eight light slots of the transform unit.
 * Also programs the two colour channels (ambient/material colours and their lighting control). */
#include <dolphin/gx/GXPrivate.h>

f32 cosf(f32 x);

typedef struct GXLightObjPriv {
    u32 reserved[3]; /* 0x00 */
    GXColor Color;   /* 0x0C */
    f32 a[3];        /* 0x10: angle attenuation */
    f32 k[3];        /* 0x1C: distance attenuation */
    f32 lpos[3];     /* 0x28 */
    f32 ldir[3];     /* 0x34 */
} GXLightObjPriv;

void GXInitLightAttn(GXLightObj *lt_obj, f32 a0, f32 a1, f32 a2, f32 k0, f32 k1, f32 k2) {
    GXLightObjPriv *obj = (GXLightObjPriv *)lt_obj;

    obj->a[0] = a0;
    obj->a[1] = a1;
    obj->a[2] = a2;
    obj->k[0] = k0;
    obj->k[1] = k1;
    obj->k[2] = k2;
}

void GXInitLightAttnA(GXLightObj *lt_obj, f32 a0, f32 a1, f32 a2) {
    GXLightObjPriv *obj = (GXLightObjPriv *)lt_obj;

    obj->a[0] = a0;
    obj->a[1] = a1;
    obj->a[2] = a2;
}

void GXInitLightSpot(GXLightObj *lt_obj, f32 cutoff, GXSpotFn spot_func) {
    f32 a0;
    f32 a1;
    f32 a2;
    f32 r;
    f32 d;
    f32 cr;
    GXLightObjPriv *obj = (GXLightObjPriv *)lt_obj;

    if (cutoff <= 0.0f || cutoff > 90.0f) {
        spot_func = 0; /* GX_SP_OFF */
    }

    r = (3.1415927f * cutoff) / 180.0f;
    cr = cosf(r);

    switch (spot_func) {
    case 1: /* GX_SP_FLAT */
        a0 = -1000.0f * cr;
        a1 = 1000.0f;
        a2 = 0.0f;
        break;
    case 2: /* GX_SP_COS */
        a1 = 1.0f / (1.0f - cr);
        a0 = -cr * a1;
        a2 = 0.0f;
        break;
    case 3: /* GX_SP_COS2 */
        a2 = 1.0f / (1.0f - cr);
        a0 = 0.0f;
        a1 = -cr * a2;
        break;
    case 4: /* GX_SP_SHARP */
        d = 1.0f / ((1.0f - cr) * (1.0f - cr));
        a0 = (cr * (cr - 2.0f)) * d;
        a1 = 2.0f * d;
        a2 = -d;
        break;
    case 5: /* GX_SP_RING1 */
        d = 1.0f / ((1.0f - cr) * (1.0f - cr));
        a2 = -4.0f * d;
        a0 = a2 * cr;
        a1 = (4.0f * (1.0f + cr)) * d;
        break;
    case 6: /* GX_SP_RING2 */
        d = 1.0f / ((1.0f - cr) * (1.0f - cr));
        a0 = 1.0f - 2.0f * cr * cr * d;
        a1 = 4.0f * cr * d;
        a2 = -2.0f * d;
        break;
    case 0: /* GX_SP_OFF */
    default:
        a0 = 1.0f;
        a1 = 0.0f;
        a2 = 0.0f;
        break;
    }

    obj->a[0] = a0;
    obj->a[1] = a1;
    obj->a[2] = a2;
}

void GXInitLightDistAttn(GXLightObj *lt_obj, f32 ref_dist, f32 ref_br, GXDistAttnFn dist_func) {
    f32 k0;
    f32 k1;
    f32 k2;
    GXLightObjPriv *obj = (GXLightObjPriv *)lt_obj;

    if (ref_dist < 0.0f) {
        dist_func = 0; /* GX_DA_OFF */
    }
    if (ref_br <= 0.0f || ref_br >= 1.0f) {
        dist_func = 0;
    }

    switch (dist_func) {
    case 1: /* GX_DA_GENTLE */
        k0 = 1.0f;
        k1 = (1.0f - ref_br) / (ref_br * ref_dist);
        k2 = 0.0f;
        break;
    case 2: /* GX_DA_MEDIUM */
        k0 = 1.0f;
        k1 = 0.5f * (1.0f - ref_br) / (ref_br * ref_dist);
        k2 = 0.5f * (1.0f - ref_br) / (ref_br * ref_dist * ref_dist);
        break;
    case 3: /* GX_DA_STEEP */
        k0 = 1.0f;
        k1 = 0.0f;
        k2 = (1.0f - ref_br) / (ref_br * ref_dist * ref_dist);
        break;
    case 0: /* GX_DA_OFF */
    default:
        k0 = 1.0f;
        k1 = 0.0f;
        k2 = 0.0f;
        break;
    }

    obj->k[0] = k0;
    obj->k[1] = k1;
    obj->k[2] = k2;
}

void GXInitLightPos(GXLightObj *lt_obj, f32 x, f32 y, f32 z) {
    GXLightObjPriv *obj = (GXLightObjPriv *)lt_obj;

    obj->lpos[0] = x;
    obj->lpos[1] = y;
    obj->lpos[2] = z;
}

void GXInitLightDir(GXLightObj *lt_obj, f32 nx, f32 ny, f32 nz) {
    GXLightObjPriv *obj = (GXLightObjPriv *)lt_obj;

    obj->ldir[0] = -nx;
    obj->ldir[1] = -ny;
    obj->ldir[2] = -nz;
}

void GXInitLightColor(GXLightObj *lt_obj, GXColor color) {
    GXLightObjPriv *obj = (GXLightObjPriv *)lt_obj;

    *(u32 *)&obj->Color = *(u32 *)&color;
}

/* Sends the 16 words of a light object: three reserved zero words, the colour, then the twelve
 * floats, read and written two at a time with paired-single moves. */
static inline void WriteLightObj(register volatile void *dst, register const GXLightObjPriv *src) {
    register u32 zero;
    register u32 color;
    register f32 a0_a1;
    register f32 a2_k0;
    register f32 k1_k2;
    register f32 px_py;
    register f32 pz_dx;
    register f32 dy_dz;

    asm {
        lwz color, 12(src)
        xor zero, zero, zero
        psq_l a0_a1, 16(src), 0, 0
        psq_l a2_k0, 24(src), 0, 0
        psq_l k1_k2, 32(src), 0, 0
        psq_l px_py, 40(src), 0, 0
        psq_l pz_dx, 48(src), 0, 0
        psq_l dy_dz, 56(src), 0, 0
        stw zero, 0(dst)
        stw zero, 0(dst)
        stw zero, 0(dst)
        stw color, 0(dst)
        psq_st a0_a1, 0(dst), 0, 0
        psq_st a2_k0, 0(dst), 0, 0
        psq_st k1_k2, 0(dst), 0, 0
        psq_st px_py, 0(dst), 0, 0
        psq_st pz_dx, 0(dst), 0, 0
        psq_st dy_dz, 0(dst), 0, 0
    }
}

void GXLoadLightObjImm(GXLightObj *lt_obj, u32 light) {
    u32 addr;
    u32 idx;
    GXLightObjPriv *obj = (GXLightObjPriv *)lt_obj;

    idx = 31 - __cntlzw(light);
    idx &= 7;
    addr = idx * 0x10 + 0x600;
    GX_WRITE_U8(0x10);
    GX_WRITE_U32(addr | 0xF0000);
    WriteLightObj(&GXWGFifo, obj);
    gx->bpSentNot = GX_TRUE;
}

void GXSetChanAmbColor(GXChannelID chan, GXColor amb_color) {
    u32 reg;
    u32 colIdx;

    switch (chan) {
    case 0: /* GX_COLOR0 */
        reg = gx->ambColor[0];
        SET_REG_FIELD(reg, 24, 8, *(u32 *)&amb_color >> 8);
        colIdx = 0;
        break;
    case 1: /* GX_COLOR1 */
        reg = gx->ambColor[1];
        SET_REG_FIELD(reg, 24, 8, *(u32 *)&amb_color >> 8);
        colIdx = 1;
        break;
    case 2: /* GX_ALPHA0 */
        reg = gx->ambColor[0];
        SET_REG_FIELD(reg, 8, 0, amb_color.a);
        colIdx = 0;
        break;
    case 3: /* GX_ALPHA1 */
        reg = gx->ambColor[1];
        SET_REG_FIELD(reg, 8, 0, amb_color.a);
        colIdx = 1;
        break;
    case 4: /* GX_COLOR0A0 */
        reg = *(u32 *)&amb_color;
        colIdx = 0;
        break;
    case 5: /* GX_COLOR1A1 */
        reg = *(u32 *)&amb_color;
        colIdx = 1;
        break;
    default:
        return;
    }

    GX_WRITE_XF_REG(0xA + colIdx, reg);
    gx->bpSentNot = GX_TRUE;
    gx->ambColor[colIdx] = reg;
}

void GXSetChanMatColor(GXChannelID chan, GXColor mat_color) {
    u32 reg;
    u32 colIdx;

    switch (chan) {
    case 0: /* GX_COLOR0 */
        reg = gx->matColor[0];
        SET_REG_FIELD(reg, 24, 8, *(u32 *)&mat_color >> 8);
        colIdx = 0;
        break;
    case 1: /* GX_COLOR1 */
        reg = gx->matColor[1];
        SET_REG_FIELD(reg, 24, 8, *(u32 *)&mat_color >> 8);
        colIdx = 1;
        break;
    case 2: /* GX_ALPHA0 */
        reg = gx->matColor[0];
        SET_REG_FIELD(reg, 8, 0, mat_color.a);
        colIdx = 0;
        break;
    case 3: /* GX_ALPHA1 */
        reg = gx->matColor[1];
        SET_REG_FIELD(reg, 8, 0, mat_color.a);
        colIdx = 1;
        break;
    case 4: /* GX_COLOR0A0 */
        reg = *(u32 *)&mat_color;
        colIdx = 0;
        break;
    case 5: /* GX_COLOR1A1 */
        reg = *(u32 *)&mat_color;
        colIdx = 1;
        break;
    default:
        return;
    }

    GX_WRITE_XF_REG(0xC + colIdx, reg);
    gx->bpSentNot = GX_TRUE;
    gx->matColor[colIdx] = reg;
}

void GXSetNumChans(u8 nChans) {
    SET_REG_FIELD(gx->genMode, 3, 4, nChans);
    GX_WRITE_XF_REG(9, nChans);
    gx->dirtyState |= GX_DIRTY_GEN_MODE;
}

void GXSetChanCtrl(GXChannelID chan, GXBool enable, GXColorSrc amb_src, GXColorSrc mat_src,
                   u32 light_mask, GXDiffuseFn diff_fn, GXAttnFn attn_fn) {
    u32 reg;
    u32 idx;

    idx = chan & 3;
    reg = 0;
    SET_REG_FIELD(reg, 1, 1, enable);
    SET_REG_FIELD(reg, 1, 0, mat_src);
    SET_REG_FIELD(reg, 1, 6, amb_src);
    if (attn_fn == 0) { /* GX_AF_SPEC */
        diff_fn = 0;    /* GX_DF_NONE */
    }
    SET_REG_FIELD(reg, 2, 7, diff_fn);
    SET_REG_FIELD(reg, 1, 9, (attn_fn != 2));  /* not GX_AF_NONE */
    SET_REG_FIELD(reg, 1, 10, (attn_fn != 0)); /* not GX_AF_SPEC */
    SET_REG_FIELD(reg, 4, 2, light_mask & 0xF);
    SET_REG_FIELD(reg, 4, 11, (light_mask >> 4) & 0xF);
    GX_WRITE_XF_REG(0xE + idx, reg);

    if (chan == 4) { /* GX_COLOR0A0 */
        GX_WRITE_XF_REG(0x10, reg);
    } else if (chan == 5) { /* GX_COLOR1A1 */
        GX_WRITE_XF_REG(0x11, reg);
    }
    gx->bpSentNot = GX_TRUE;
}

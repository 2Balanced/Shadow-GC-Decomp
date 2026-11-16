/* Dolphin SDK GX: vertex attributes.
 *
 * Keeps the shadow copies of the vertex descriptor (which attributes a vertex carries, and
 * whether each is direct or indexed) and of the eight vertex attribute format tables. Setters
 * only mark the state dirty; GXBegin sends it (__GXSetVCD, __GXSetVAT) and recomputes the
 * vertex size (__GXCalculateVLim). Also programs array bases/strides and texture coordinate
 * generation. */
#include <dolphin/gx/GXPrivate.h>

void GXSetVtxDesc(GXAttr attr, GXAttrType type) {
    switch (attr) {
    case 0: /* GX_VA_PNMTXIDX */
        SET_REG_FIELD(gx->vcdLo, 1, 0, type);
        break;
    case 1: /* GX_VA_TEX0MTXIDX ... */
        SET_REG_FIELD(gx->vcdLo, 1, 1, type);
        break;
    case 2:
        SET_REG_FIELD(gx->vcdLo, 1, 2, type);
        break;
    case 3:
        SET_REG_FIELD(gx->vcdLo, 1, 3, type);
        break;
    case 4:
        SET_REG_FIELD(gx->vcdLo, 1, 4, type);
        break;
    case 5:
        SET_REG_FIELD(gx->vcdLo, 1, 5, type);
        break;
    case 6:
        SET_REG_FIELD(gx->vcdLo, 1, 6, type);
        break;
    case 7:
        SET_REG_FIELD(gx->vcdLo, 1, 7, type);
        break;
    case 8: /* ... GX_VA_TEX7MTXIDX */
        SET_REG_FIELD(gx->vcdLo, 1, 8, type);
        break;
    case 9: /* GX_VA_POS */
        SET_REG_FIELD(gx->vcdLo, 2, 9, type);
        break;
    case 10: /* GX_VA_NRM */
        if (type != 0) {
            gx->hasNrms = GX_TRUE;
            gx->hasBiNrms = GX_FALSE;
            gx->nrmType = type;
        } else {
            gx->hasNrms = GX_FALSE;
        }
        break;
    case 25: /* GX_VA_NBT */
        if (type != 0) {
            gx->hasBiNrms = GX_TRUE;
            gx->hasNrms = GX_FALSE;
            gx->nrmType = type;
        } else {
            gx->hasBiNrms = GX_FALSE;
        }
        break;
    case 11: /* GX_VA_CLR0 */
        SET_REG_FIELD(gx->vcdLo, 2, 13, type);
        break;
    case 12: /* GX_VA_CLR1 */
        SET_REG_FIELD(gx->vcdLo, 2, 15, type);
        break;
    case 13: /* GX_VA_TEX0 ... */
        SET_REG_FIELD(gx->vcdHi, 2, 0, type);
        break;
    case 14:
        SET_REG_FIELD(gx->vcdHi, 2, 2, type);
        break;
    case 15:
        SET_REG_FIELD(gx->vcdHi, 2, 4, type);
        break;
    case 16:
        SET_REG_FIELD(gx->vcdHi, 2, 6, type);
        break;
    case 17:
        SET_REG_FIELD(gx->vcdHi, 2, 8, type);
        break;
    case 18:
        SET_REG_FIELD(gx->vcdHi, 2, 10, type);
        break;
    case 19:
        SET_REG_FIELD(gx->vcdHi, 2, 12, type);
        break;
    case 20: /* ... GX_VA_TEX7 */
        SET_REG_FIELD(gx->vcdHi, 2, 14, type);
        break;
    default:
        break;
    }

    if (gx->hasNrms || gx->hasBiNrms) {
        SET_REG_FIELD(gx->vcdLo, 2, 11, gx->nrmType);
    } else {
        SET_REG_FIELD(gx->vcdLo, 2, 11, 0);
    }
    gx->dirtyState |= GX_DIRTY_VCD;
}

void __GXSetVCD(void) {
    u32 nCols;
    u32 nNrm;
    u32 nTex;
    u32 reg;

    GX_WRITE_CP_REG(0x50, gx->vcdLo);
    GX_WRITE_CP_REG(0x60, gx->vcdHi);

    if (gx->hasBiNrms) {
        nNrm = 2;
    } else if (gx->hasNrms) {
        nNrm = 1;
    } else {
        nNrm = 0;
    }

    /* A two-bit field per attribute: count the attributes present from the highest one. */
    nCols = 33 - __cntlzw((gx->vcdLo & 0x1E000) >> 13);
    nCols /= 2;
    nTex = 33 - __cntlzw((gx->vcdHi & 0xFFFF) >> 0);
    nTex /= 2;
    reg = nCols | (nNrm << 2) | (nTex << 4);
    GX_WRITE_XF_REG(8, reg);
    gx->bpSentNot = GX_TRUE;
}

void __GXCalculateVLim(void) {
    static u8 tbl1[] = {0, 4, 1, 2};
    static u8 tbl2[] = {0, 8, 1, 2};
    static u8 tbl3[] = {0, 12, 1, 2};
    GXCompCnt nc;
    u32 vlm;
    u32 b;
    u32 vl;
    u32 vh;
    u32 va;

    if (gx->vNum == 0) {
        return;
    }

    vl = gx->vcdLo;
    vh = gx->vcdHi;
    va = gx->vatA[0];

    vlm = GET_REG_FIELD(vl, 1, 0);
    vlm += GET_REG_FIELD(vl, 1, 1);
    vlm += GET_REG_FIELD(vl, 1, 2);
    vlm += GET_REG_FIELD(vl, 1, 3);
    vlm += GET_REG_FIELD(vl, 1, 4);
    vlm += GET_REG_FIELD(vl, 1, 5);
    vlm += GET_REG_FIELD(vl, 1, 6);
    vlm += GET_REG_FIELD(vl, 1, 7);
    vlm += GET_REG_FIELD(vl, 1, 8);
    vlm += tbl3[GET_REG_FIELD(vl, 2, 9)];
    nc = GET_REG_FIELD(va, 1, 9);
    b = (nc == 1) ? 3 : 1;
    vlm += tbl3[GET_REG_FIELD(vl, 2, 11)] * b;
    vlm += tbl1[GET_REG_FIELD(vl, 2, 13)];
    vlm += tbl1[GET_REG_FIELD(vl, 2, 15)];
    vlm += tbl2[GET_REG_FIELD(vh, 2, 0)];
    vlm += tbl2[GET_REG_FIELD(vh, 2, 2)];
    vlm += tbl2[GET_REG_FIELD(vh, 2, 4)];
    vlm += tbl2[GET_REG_FIELD(vh, 2, 6)];
    vlm += tbl2[GET_REG_FIELD(vh, 2, 8)];
    vlm += tbl2[GET_REG_FIELD(vh, 2, 10)];
    vlm += tbl2[GET_REG_FIELD(vh, 2, 12)];
    vlm += tbl2[GET_REG_FIELD(vh, 2, 14)];
    gx->vLim = vlm;
}

void GXClearVtxDesc(void) {
    gx->vcdLo = 0;
    SET_REG_FIELD(gx->vcdLo, 2, 9, 1);
    gx->vcdHi = 0;
    gx->hasNrms = GX_FALSE;
    gx->hasBiNrms = GX_FALSE;
    gx->dirtyState |= GX_DIRTY_VCD;
}

/* Stores one attribute's component count, type and fraction bits into the format's three
 * attribute-table words. */
static inline void SETVAT(u32 *va, u32 *vb, u32 *vc, GXAttr attr, GXCompCnt cnt, GXCompType type,
                          u8 frac) {
    switch (attr) {                                                                                \
    case 9: /* GX_VA_POS */                                                                        \
        SET_REG_FIELD(*va, 1, 0, cnt);                                                             \
        SET_REG_FIELD(*va, 3, 1, type);                                                            \
        SET_REG_FIELD(*va, 5, 4, frac);                                                            \
        break;                                                                                     \
    case 10: /* GX_VA_NRM */                                                                       \
    case 25: /* GX_VA_NBT */                                                                       \
        SET_REG_FIELD(*va, 3, 10, type);                                                           \
        if (cnt == 2) {                                                                            \
            SET_REG_FIELD(*va, 1, 9, 1);                                                           \
            SET_REG_FIELD(*va, 1, 31, 1);                                                          \
        } else {                                                                                   \
            SET_REG_FIELD(*va, 1, 9, cnt);                                                         \
            SET_REG_FIELD(*va, 1, 31, 0);                                                          \
        }                                                                                          \
        break;                                                                                     \
    case 11: /* GX_VA_CLR0 */                                                                      \
        SET_REG_FIELD(*va, 1, 13, cnt);                                                            \
        SET_REG_FIELD(*va, 3, 14, type);                                                           \
        break;                                                                                     \
    case 12: /* GX_VA_CLR1 */                                                                      \
        SET_REG_FIELD(*va, 1, 17, cnt);                                                            \
        SET_REG_FIELD(*va, 3, 18, type);                                                           \
        break;                                                                                     \
    case 13: /* GX_VA_TEX0 */                                                                      \
        SET_REG_FIELD(*va, 1, 21, cnt);                                                            \
        SET_REG_FIELD(*va, 3, 22, type);                                                           \
        SET_REG_FIELD(*va, 5, 25, frac);                                                           \
        break;                                                                                     \
    case 14:                                                                                       \
        SET_REG_FIELD(*vb, 1, 0, cnt);                                                             \
        SET_REG_FIELD(*vb, 3, 1, type);                                                            \
        SET_REG_FIELD(*vb, 5, 4, frac);                                                            \
        break;                                                                                     \
    case 15:                                                                                       \
        SET_REG_FIELD(*vb, 1, 9, cnt);                                                             \
        SET_REG_FIELD(*vb, 3, 10, type);                                                           \
        SET_REG_FIELD(*vb, 5, 13, frac);                                                           \
        break;                                                                                     \
    case 16:                                                                                       \
        SET_REG_FIELD(*vb, 1, 18, cnt);                                                            \
        SET_REG_FIELD(*vb, 3, 19, type);                                                           \
        SET_REG_FIELD(*vb, 5, 22, frac);                                                           \
        break;                                                                                     \
    case 17:                                                                                       \
        SET_REG_FIELD(*vb, 1, 27, cnt);                                                            \
        SET_REG_FIELD(*vb, 3, 28, type);                                                           \
        SET_REG_FIELD(*vc, 5, 0, frac);                                                            \
        break;                                                                                     \
    case 18:                                                                                       \
        SET_REG_FIELD(*vc, 1, 5, cnt);                                                             \
        SET_REG_FIELD(*vc, 3, 6, type);                                                            \
        SET_REG_FIELD(*vc, 5, 9, frac);                                                            \
        break;                                                                                     \
    case 19:                                                                                       \
        SET_REG_FIELD(*vc, 1, 14, cnt);                                                            \
        SET_REG_FIELD(*vc, 3, 15, type);                                                           \
        SET_REG_FIELD(*vc, 5, 18, frac);                                                           \
        break;                                                                                     \
    case 20: /* GX_VA_TEX7 */                                                                      \
        SET_REG_FIELD(*vc, 1, 23, cnt);                                                            \
        SET_REG_FIELD(*vc, 3, 24, type);                                                           \
        SET_REG_FIELD(*vc, 5, 27, frac);                                                           \
        break;                                                                                     \
    default:                                                                                       \
        break;                                                                                     \
    }
}

void GXSetVtxAttrFmt(GXVtxFmt vtxfmt, GXAttr attr, GXCompCnt cnt, GXCompType type, u8 frac) {
    u32 *va = &gx->vatA[vtxfmt];
    u32 *vb = &gx->vatB[vtxfmt];
    u32 *vc = &gx->vatC[vtxfmt];

    SETVAT(va, vb, vc, attr, cnt, type, frac);
    gx->dirtyState |= GX_DIRTY_VAT;
    gx->dirtyVAT |= (u8)(1 << (u8)vtxfmt);
}

void GXSetVtxAttrFmtv(GXVtxFmt vtxfmt, const GXVtxAttrFmtList *list) {
    u32 *va = &gx->vatA[vtxfmt];
    u32 *vb = &gx->vatB[vtxfmt];
    u32 *vc = &gx->vatC[vtxfmt];

    for (; list->attr != 0xFF; list++) {
        SETVAT(va, vb, vc, list->attr, list->cnt, list->compType, list->frac);
    }
    gx->dirtyState |= GX_DIRTY_VAT;
    gx->dirtyVAT |= (u8)(1 << (u8)vtxfmt);
}

void __GXSetVAT(void) {
    u32 i = 0;
    u32 dirty = gx->dirtyVAT;

    do {
        if (dirty & 1) {
            GX_WRITE_CP_REG(i | 0x70, gx->vatA[i]);
            GX_WRITE_CP_REG(i | 0x80, gx->vatB[i]);
            GX_WRITE_CP_REG(i | 0x90, gx->vatC[i]);
        }
        dirty >>= 1;
        i++;
    } while (dirty != 0);
    gx->dirtyVAT = 0;
}

void GXSetArray(GXAttr attr, const void *base_ptr, u8 stride) {
    if (attr == 25) { /* GX_VA_NBT shares the normal array */
        attr = 10;
    }
    attr -= 9;
    GX_WRITE_CP_REG(0xA0 | attr, (u32)base_ptr & 0x3FFFFFFF);
    GX_WRITE_CP_REG(0xB0 | attr, stride);
}

void GXInvalidateVtxCache(void) {
    GX_WRITE_U8(0x48);
}

void GXSetTexCoordGen2(GXTexCoordID dst_coord, GXTexGenType func, GXTexGenSrc src_param, u32 mtx,
                       GXBool normalize, u32 pt_texmtx) {
    u32 reg = 0;
    u32 row;
    u32 bumprow;
    u32 form;
    GXAttr mtxIdAttr;

    form = 0;
    row = 5;
    switch (src_param) {
    case 0: /* GX_TG_POS */
        row = 0;
        form = 1;
        break;
    case 1: /* GX_TG_NRM */
        row = 1;
        form = 1;
        break;
    case 2: /* GX_TG_BINRM */
        row = 3;
        form = 1;
        break;
    case 3: /* GX_TG_TANGENT */
        row = 4;
        form = 1;
        break;
    case 19: /* GX_TG_COLOR0 */
        row = 2;
        break;
    case 20: /* GX_TG_COLOR1 */
        row = 2;
        break;
    case 4: /* GX_TG_TEX0 */
        row = 5;
        break;
    case 5:
        row = 6;
        break;
    case 6:
        row = 7;
        break;
    case 7:
        row = 8;
        break;
    case 8:
        row = 9;
        break;
    case 9:
        row = 10;
        break;
    case 10:
        row = 11;
        break;
    case 11: /* GX_TG_TEX7 */
        row = 12;
        break;
    case 12: /* GX_TG_TEXCOORD0 ... */
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18: /* ... GX_TG_TEXCOORD6 */
        break;
    default:
        break;
    }

    switch (func) {
    case 1: /* GX_TG_MTX2x4 */
        SET_REG_FIELD(reg, 1, 1, 0);
        SET_REG_FIELD(reg, 1, 2, form);
        SET_REG_FIELD(reg, 3, 4, 0);
        SET_REG_FIELD(reg, 5, 7, row);
        break;
    case 0: /* GX_TG_MTX3x4 */
        SET_REG_FIELD(reg, 1, 1, 1);
        SET_REG_FIELD(reg, 1, 2, form);
        SET_REG_FIELD(reg, 3, 4, 0);
        SET_REG_FIELD(reg, 5, 7, row);
        break;
    case 2: /* GX_TG_BUMP0 ... */
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9: /* ... GX_TG_BUMP7 */
        bumprow = src_param - 12;
        SET_REG_FIELD(reg, 1, 1, 0);
        SET_REG_FIELD(reg, 1, 2, form);
        SET_REG_FIELD(reg, 3, 4, 1);
        SET_REG_FIELD(reg, 5, 7, row);
        SET_REG_FIELD(reg, 3, 12, bumprow);
        SET_REG_FIELD(reg, 3, 15, func - 2);
        break;
    case 10: /* GX_TG_SRTG */
        SET_REG_FIELD(reg, 1, 1, 0);
        SET_REG_FIELD(reg, 1, 2, form);
        if (src_param == 19) {
            SET_REG_FIELD(reg, 3, 4, 2);
        } else {
            SET_REG_FIELD(reg, 3, 4, 3);
        }
        SET_REG_FIELD(reg, 5, 7, 2);
        break;
    default:
        break;
    }

    GX_WRITE_XF_REG(0x40 + dst_coord, reg);
    reg = 0;
    SET_REG_FIELD(reg, 6, 0, pt_texmtx - 64);
    SET_REG_FIELD(reg, 1, 8, normalize);
    GX_WRITE_XF_REG(0x50 + dst_coord, reg);

    switch (dst_coord) {
    case 0:
        SET_REG_FIELD(gx->matIdxA, 6, 6, mtx);
        break;
    case 1:
        SET_REG_FIELD(gx->matIdxA, 6, 12, mtx);
        break;
    case 2:
        SET_REG_FIELD(gx->matIdxA, 6, 18, mtx);
        break;
    case 3:
        SET_REG_FIELD(gx->matIdxA, 6, 24, mtx);
        break;
    case 4:
        SET_REG_FIELD(gx->matIdxB, 6, 0, mtx);
        break;
    case 5:
        SET_REG_FIELD(gx->matIdxB, 6, 6, mtx);
        break;
    case 6:
        SET_REG_FIELD(gx->matIdxB, 6, 12, mtx);
        break;
    default:
        SET_REG_FIELD(gx->matIdxB, 6, 18, mtx);
        break;
    }

    mtxIdAttr = dst_coord + 1;
    __GXSetMatrixIndex(mtxIdAttr);
}

void GXSetNumTexGens(u8 nTexGens) {
    SET_REG_FIELD(gx->genMode, 4, 0, nTexGens);
    GX_WRITE_XF_REG(0x3F, nTexGens);
    gx->dirtyState |= GX_DIRTY_GEN_MODE;
}

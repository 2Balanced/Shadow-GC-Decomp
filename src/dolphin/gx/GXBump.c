/* Dolphin SDK GX: indirect (bump) texturing.
 *
 * Programs the indirect texture stages: which coordinate and map each one reads, the coordinate
 * scales, the three indirect offset matrices, and how each TEV stage applies an indirect lookup
 * to its regular texture coordinate. */
#include <dolphin/gx/GXPrivate.h>

void GXSetTevIndirect(GXTevStageID tev_stage, GXIndTexStageID ind_stage, GXIndTexFormat format,
                      GXIndTexBiasSel bias_sel, GXIndTexMtxID matrix_sel, GXIndTexWrap wrap_s,
                      GXIndTexWrap wrap_t, GXBool add_prev, GXBool utc_lod,
                      GXIndTexAlphaSel alpha_sel) {
    u32 reg;

    reg = 0;
    SET_REG_FIELD(reg, 2, 0, ind_stage);
    SET_REG_FIELD(reg, 2, 2, format);
    SET_REG_FIELD(reg, 3, 4, bias_sel);
    SET_REG_FIELD(reg, 2, 7, alpha_sel);
    SET_REG_FIELD(reg, 4, 9, matrix_sel);
    SET_REG_FIELD(reg, 3, 13, wrap_s);
    SET_REG_FIELD(reg, 3, 16, wrap_t);
    SET_REG_FIELD(reg, 1, 19, utc_lod);
    SET_REG_FIELD(reg, 1, 20, add_prev);
    SET_REG_FIELD(reg, 8, 24, tev_stage + 0x10);
    GX_WRITE_RAS_REG(reg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetIndTexMtx(GXIndTexMtxID mtx_id, const f32 offset[2][3], s8 scale_exp) {
    s32 mtx[6];
    u32 reg;
    u32 id;

    switch (mtx_id) {
    case 1: /* GX_ITM_0 ... GX_ITM_2 */
    case 2:
    case 3:
        id = mtx_id - 1;
        break;
    case 5: /* GX_ITM_S0 ... GX_ITM_S2 */
    case 6:
    case 7:
        id = mtx_id - 5;
        break;
    case 9: /* GX_ITM_T0 ... GX_ITM_T2 */
    case 10:
    case 11:
        id = mtx_id - 9;
        break;
    default:
        id = 0;
        break;
    }

    /* Elements are sent as 11-bit fixed point; the exponent's bits are spread over the three
     * register writes. */
    mtx[0] = (s32)(1024.0f * offset[0][0]) & 0x7FF;
    mtx[1] = (s32)(1024.0f * offset[1][0]) & 0x7FF;
    scale_exp += 17;
    reg = 0;
    SET_REG_FIELD(reg, 11, 0, mtx[0]);
    SET_REG_FIELD(reg, 11, 11, mtx[1]);
    SET_REG_FIELD(reg, 2, 22, scale_exp & 3);
    SET_REG_FIELD(reg, 8, 24, id * 3 + 6);
    GX_WRITE_RAS_REG(reg);

    mtx[2] = (s32)(1024.0f * offset[0][1]) & 0x7FF;
    mtx[3] = (s32)(1024.0f * offset[1][1]) & 0x7FF;
    reg = 0;
    SET_REG_FIELD(reg, 11, 0, mtx[2]);
    SET_REG_FIELD(reg, 11, 11, mtx[3]);
    SET_REG_FIELD(reg, 2, 22, (scale_exp >> 2) & 3);
    SET_REG_FIELD(reg, 8, 24, id * 3 + 7);
    GX_WRITE_RAS_REG(reg);

    mtx[4] = (s32)(1024.0f * offset[0][2]) & 0x7FF;
    mtx[5] = (s32)(1024.0f * offset[1][2]) & 0x7FF;
    reg = 0;
    SET_REG_FIELD(reg, 11, 0, mtx[4]);
    SET_REG_FIELD(reg, 11, 11, mtx[5]);
    SET_REG_FIELD(reg, 2, 22, (scale_exp >> 4) & 3);
    SET_REG_FIELD(reg, 8, 24, id * 3 + 8);
    GX_WRITE_RAS_REG(reg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetIndTexCoordScale(GXIndTexStageID ind_state, GXIndTexScale scale_s, GXIndTexScale scale_t) {
    switch (ind_state) {
    case 0:
        SET_REG_FIELD(gx->IndTexScale0, 4, 0, scale_s);
        SET_REG_FIELD(gx->IndTexScale0, 4, 4, scale_t);
        SET_REG_FIELD(gx->IndTexScale0, 8, 24, 0x25);
        GX_WRITE_RAS_REG(gx->IndTexScale0);
        break;
    case 1:
        SET_REG_FIELD(gx->IndTexScale0, 4, 8, scale_s);
        SET_REG_FIELD(gx->IndTexScale0, 4, 12, scale_t);
        SET_REG_FIELD(gx->IndTexScale0, 8, 24, 0x25);
        GX_WRITE_RAS_REG(gx->IndTexScale0);
        break;
    case 2:
        SET_REG_FIELD(gx->IndTexScale1, 4, 0, scale_s);
        SET_REG_FIELD(gx->IndTexScale1, 4, 4, scale_t);
        SET_REG_FIELD(gx->IndTexScale1, 8, 24, 0x26);
        GX_WRITE_RAS_REG(gx->IndTexScale1);
        break;
    case 3:
        SET_REG_FIELD(gx->IndTexScale1, 4, 8, scale_s);
        SET_REG_FIELD(gx->IndTexScale1, 4, 12, scale_t);
        SET_REG_FIELD(gx->IndTexScale1, 8, 24, 0x26);
        GX_WRITE_RAS_REG(gx->IndTexScale1);
        break;
    default:
        break;
    }
    gx->bpSentNot = GX_FALSE;
}

void GXSetIndTexOrder(GXIndTexStageID ind_stage, GXTexCoordID tex_coord, GXTexMapID tex_map) {
    if (tex_map == 0xFF) {
        tex_map = 0;
    }
    if (tex_coord == 0xFF) {
        tex_coord = 0;
    }

    switch (ind_stage) {
    case 0:
        SET_REG_FIELD(gx->iref, 3, 0, tex_map);
        SET_REG_FIELD(gx->iref, 3, 3, tex_coord);
        break;
    case 1:
        SET_REG_FIELD(gx->iref, 3, 6, tex_map);
        SET_REG_FIELD(gx->iref, 3, 9, tex_coord);
        break;
    case 2:
        SET_REG_FIELD(gx->iref, 3, 12, tex_map);
        SET_REG_FIELD(gx->iref, 3, 15, tex_coord);
        break;
    case 3:
        SET_REG_FIELD(gx->iref, 3, 18, tex_map);
        SET_REG_FIELD(gx->iref, 3, 21, tex_coord);
        break;
    default:
        break;
    }
    GX_WRITE_RAS_REG(gx->iref);
    gx->dirtyState |= GX_DIRTY_SU_TEX | GX_DIRTY_BP_MASK;
    gx->bpSentNot = GX_FALSE;
}

void GXSetNumIndStages(u8 nIndStages) {
    SET_REG_FIELD(gx->genMode, 3, 16, nIndStages);
    gx->dirtyState |= GX_DIRTY_BP_MASK | GX_DIRTY_GEN_MODE;
}

/* Retail calls GXSetTevIndirect here; the compiler would otherwise inline it. */
#pragma dont_inline on
void GXSetTevDirect(GXTevStageID tev_stage) {
    GXSetTevIndirect(tev_stage, 0, 0, 0, 0, 0, 0, GX_FALSE, GX_FALSE, 0);
}
#pragma dont_inline reset

void __GXUpdateBPMask(void) {
}

void __GXSetIndirectMask(u32 mask) {
    SET_REG_FIELD(gx->bpMask, 8, 0, mask);
    GX_WRITE_RAS_REG(gx->bpMask);
    gx->bpSentNot = GX_FALSE;
}

void __GXFlushTextureState(void) {
    GX_WRITE_RAS_REG(gx->bpMask);
    gx->bpSentNot = GX_FALSE;
}

/* Dolphin SDK GX: primitives and geometry state.
 *
 * GXBegin first sends whatever state the setters left dirty, then starts a primitive. When the
 * transform unit needs it (see GXSetMisc's XF flush), a dummy primitive is sent first to flush
 * the vertex pipeline. Also holds line/point size, culling and the co-planar mode. */
#include <dolphin/gx/GXPrivate.h>

void __GXSetDirtyState(void) {
    u32 dState = gx->dirtyState;

    if (dState & GX_DIRTY_SU_TEX) {
        __GXSetSUTexRegs();
    }
    if (dState & GX_DIRTY_BP_MASK) {
        __GXUpdateBPMask();
    }
    if (dState & GX_DIRTY_GEN_MODE) {
        __GXSetGenMode();
    }
    if (dState & GX_DIRTY_VCD) {
        __GXSetVCD();
    }
    if (dState & GX_DIRTY_VAT) {
        __GXSetVAT();
    }
    if (dState & (GX_DIRTY_VCD | GX_DIRTY_VAT)) {
        __GXCalculateVLim();
    }
    gx->dirtyState = 0;
}

void GXBegin(GXPrimitive type, GXVtxFmt vtxfmt, u16 nverts) {
    if (gx->dirtyState) {
        __GXSetDirtyState();
    }
    /* vNumNot and bpSentNot both clear: the vertex pipeline needs flushing first. */
    if (*(u32 *)&gx->vNumNot == 0) {
        __GXSendFlushPrim();
    }
    GX_WRITE_U8(vtxfmt | type);
    GX_WRITE_U16(nverts);
}

void __GXSendFlushPrim(void) {
    u32 i;
    u32 numD = gx->vNum * gx->vLim;

    GX_WRITE_U8(0x98);
    GX_WRITE_U16(gx->vNum);
    for (i = 0; i < numD; i += 4) {
        GX_WRITE_U32(0);
    }
    gx->bpSentNot = GX_TRUE;
}

void GXSetLineWidth(u8 width, int texOffsets) {
    SET_REG_FIELD(gx->lpSize, 8, 0, width);
    SET_REG_FIELD(gx->lpSize, 3, 16, texOffsets);
    GX_WRITE_RAS_REG(gx->lpSize);
    gx->bpSentNot = GX_FALSE;
}

void GXSetPointSize(u8 pointSize, int texOffsets) {
    SET_REG_FIELD(gx->lpSize, 8, 8, pointSize);
    SET_REG_FIELD(gx->lpSize, 3, 19, texOffsets);
    GX_WRITE_RAS_REG(gx->lpSize);
    gx->bpSentNot = GX_FALSE;
}

void GXEnableTexOffsets(GXTexCoordID coord, GXBool line_enable, GXBool point_enable) {
    SET_REG_FIELD(gx->suTs0[coord], 1, 18, line_enable);
    SET_REG_FIELD(gx->suTs0[coord], 1, 19, point_enable);
    GX_WRITE_RAS_REG(gx->suTs0[coord]);
    gx->bpSentNot = GX_FALSE;
}

void GXSetCullMode(GXCullMode mode) {
    GXCullMode hwMode;

    /* The hardware numbers front and back the other way round. */
    hwMode = GET_REG_FIELD(mode, 1, 1);
    SET_REG_FIELD(hwMode, 1, 1, mode);
    SET_REG_FIELD(gx->genMode, 2, 14, hwMode);
    gx->dirtyState |= GX_DIRTY_GEN_MODE;
}

void GXSetCoPlanar(GXBool enable) {
    SET_REG_FIELD(gx->genMode, 1, 19, enable);
    GX_WRITE_RAS_REG(0xFE080000);
    GX_WRITE_RAS_REG(gx->genMode);
}

void __GXSetGenMode(void) {
    GX_WRITE_RAS_REG(gx->genMode);
    gx->bpSentNot = GX_FALSE;
}

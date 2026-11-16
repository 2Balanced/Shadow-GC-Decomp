/* Dolphin SDK GX: texture environment (TEV) stages.
 *
 * Each of the sixteen stages has a colour and an alpha combiner register; GXSetTevOp loads
 * preset combinations, the other setters edit single fields. Also holds the constant-colour
 * selection, the swap tables, the alpha compare, the Z texture and the per-stage order (which
 * texture coordinate, texture map and rasterised colour a stage reads). */
#include <dolphin/gx/GXPrivate.h>

/* Preset combiner settings for GX_MODULATE, GX_DECAL, GX_BLEND, GX_REPLACE and GX_PASSCLR: for
 * stage 0 (which reads the rasterised colour) and for later stages (which read the previous
 * stage). */
static u32 TEVCOpTableST0[5] = {0xC008F8AF, 0xC008A89F, 0xC008AC8F, 0xC008FFF8, 0xC008FFFA};
static u32 TEVCOpTableST1[5] = {0xC008F80F, 0xC008089F, 0xC0080C8F, 0xC008FFF8, 0xC008FFF0};
static u32 TEVAOpTableST0[5] = {0xC108F2F0, 0xC108FFD0, 0xC108F2F0, 0xC108FFC0, 0xC108FFD0};
static u32 TEVAOpTableST1[5] = {0xC108F070, 0xC108FF80, 0xC108F070, 0xC108FFC0, 0xC108FF80};

void GXSetTevOp(GXTevStageID id, GXTevMode mode) {
    u32 *ctmp;
    u32 *atmp;
    u32 tevReg;

    if (id == 0) {
        ctmp = &TEVCOpTableST0[mode];
        atmp = &TEVAOpTableST0[mode];
    } else {
        ctmp = &TEVCOpTableST1[mode];
        atmp = &TEVAOpTableST1[mode];
    }

    tevReg = gx->tevc[id];
    tevReg = (*ctmp & ~0xFF000000) | (tevReg & 0xFF000000);
    GX_WRITE_RAS_REG(tevReg);
    gx->tevc[id] = tevReg;

    tevReg = gx->teva[id];
    tevReg = (*atmp & ~0xFF00000F) | (tevReg & 0xFF00000F);
    GX_WRITE_RAS_REG(tevReg);
    gx->teva[id] = tevReg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevColorIn(GXTevStageID stage, GXTevColorArg a, GXTevColorArg b, GXTevColorArg c,
                     GXTevColorArg d) {
    u32 tevReg;

    tevReg = gx->tevc[stage];
    SET_REG_FIELD(tevReg, 4, 12, a);
    SET_REG_FIELD(tevReg, 4, 8, b);
    SET_REG_FIELD(tevReg, 4, 4, c);
    SET_REG_FIELD(tevReg, 4, 0, d);
    GX_WRITE_RAS_REG(tevReg);
    gx->tevc[stage] = tevReg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevAlphaIn(GXTevStageID stage, GXTevAlphaArg a, GXTevAlphaArg b, GXTevAlphaArg c,
                     GXTevAlphaArg d) {
    u32 tevReg;

    tevReg = gx->teva[stage];
    SET_REG_FIELD(tevReg, 3, 13, a);
    SET_REG_FIELD(tevReg, 3, 10, b);
    SET_REG_FIELD(tevReg, 3, 7, c);
    SET_REG_FIELD(tevReg, 3, 4, d);
    GX_WRITE_RAS_REG(tevReg);
    gx->teva[stage] = tevReg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevColorOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale,
                     GXBool clamp, GXTevRegID out_reg) {
    u32 tevReg;

    tevReg = gx->tevc[stage];
    SET_REG_FIELD(tevReg, 1, 18, op & 1);
    if (op <= 1) {
        SET_REG_FIELD(tevReg, 2, 20, scale);
        SET_REG_FIELD(tevReg, 2, 16, bias);
    } else {
        /* Compare operations: the bias field selects compare mode. */
        SET_REG_FIELD(tevReg, 2, 20, (op >> 1) & 3);
        SET_REG_FIELD(tevReg, 2, 16, 3);
    }
    SET_REG_FIELD(tevReg, 1, 19, clamp);
    SET_REG_FIELD(tevReg, 2, 22, out_reg);
    GX_WRITE_RAS_REG(tevReg);
    gx->tevc[stage] = tevReg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevAlphaOp(GXTevStageID stage, GXTevOp op, GXTevBias bias, GXTevScale scale,
                     GXBool clamp, GXTevRegID out_reg) {
    u32 tevReg;

    tevReg = gx->teva[stage];
    SET_REG_FIELD(tevReg, 1, 18, op & 1);
    if (op <= 1) {
        SET_REG_FIELD(tevReg, 2, 20, scale);
        SET_REG_FIELD(tevReg, 2, 16, bias);
    } else {
        SET_REG_FIELD(tevReg, 2, 20, (op >> 1) & 3);
        SET_REG_FIELD(tevReg, 2, 16, 3);
    }
    SET_REG_FIELD(tevReg, 1, 19, clamp);
    SET_REG_FIELD(tevReg, 2, 22, out_reg);
    GX_WRITE_RAS_REG(tevReg);
    gx->teva[stage] = tevReg;
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevColor(GXTevRegID id, GXColor color) {
    u32 rgba;
    u32 regRA;
    u32 regBG;

    rgba = *(u32 *)&color;
    regRA = (0xE0 + id * 2) << 24;
    SET_REG_FIELD(regRA, 8, 0, rgba >> 24);
    SET_REG_FIELD(regRA, 8, 12, rgba);
    GX_WRITE_RAS_REG(regRA);
    regBG = (0xE1 + id * 2) << 24;
    SET_REG_FIELD(regBG, 8, 0, rgba >> 8);
    SET_REG_FIELD(regBG, 8, 12, rgba >> 16);
    /* The blue/green word has to be sent three times for the register to take it. */
    GX_WRITE_RAS_REG(regBG);
    GX_WRITE_RAS_REG(regBG);
    GX_WRITE_RAS_REG(regBG);
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevColorS10(GXTevRegID id, GXColorS10 color) {
    u32 sRG;
    u32 sBA;
    u32 regRA;
    u32 regBG;

    sRG = *(u32 *)&color;
    sBA = *((u32 *)&color + 1);
    regRA = (0xE0 + id * 2) << 24;
    SET_REG_FIELD(regRA, 11, 0, sRG >> 16);
    SET_REG_FIELD(regRA, 11, 12, sBA);
    GX_WRITE_RAS_REG(regRA);
    regBG = (0xE1 + id * 2) << 24;
    SET_REG_FIELD(regBG, 11, 0, sBA >> 16);
    SET_REG_FIELD(regBG, 11, 12, sRG);
    GX_WRITE_RAS_REG(regBG);
    GX_WRITE_RAS_REG(regBG);
    GX_WRITE_RAS_REG(regBG);
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevKColor(GXTevKColorID id, GXColor color) {
    u32 rgba;
    u32 regRA;
    u32 regBG;

    rgba = *(u32 *)&color;
    regRA = (0xE0 + id * 2) << 24;
    SET_REG_FIELD(regRA, 8, 0, rgba >> 24);
    SET_REG_FIELD(regRA, 8, 12, rgba);
    SET_REG_FIELD(regRA, 4, 20, 8);
    regBG = (0xE1 + id * 2) << 24;
    SET_REG_FIELD(regBG, 8, 0, rgba >> 8);
    SET_REG_FIELD(regBG, 8, 12, rgba >> 16);
    SET_REG_FIELD(regBG, 4, 20, 8);
    GX_WRITE_RAS_REG(regRA);
    GX_WRITE_RAS_REG(regBG);
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevKColorSel(GXTevStageID stage, GXTevKColorSel sel) {
    u32 *Kreg;

    Kreg = &gx->tevKsel[stage >> 1];
    if (stage & 1) {
        SET_REG_FIELD(*Kreg, 5, 14, sel);
    } else {
        SET_REG_FIELD(*Kreg, 5, 4, sel);
    }
    GX_WRITE_RAS_REG(*Kreg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevKAlphaSel(GXTevStageID stage, GXTevKAlphaSel sel) {
    u32 *Kreg;

    Kreg = &gx->tevKsel[stage >> 1];
    if (stage & 1) {
        SET_REG_FIELD(*Kreg, 5, 19, sel);
    } else {
        SET_REG_FIELD(*Kreg, 5, 9, sel);
    }
    GX_WRITE_RAS_REG(*Kreg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevSwapMode(GXTevStageID stage, GXTevSwapSel ras_sel, GXTevSwapSel tex_sel) {
    u32 *pTevReg;

    pTevReg = &gx->teva[stage];
    SET_REG_FIELD(*pTevReg, 2, 0, ras_sel);
    SET_REG_FIELD(*pTevReg, 2, 2, tex_sel);
    GX_WRITE_RAS_REG(*pTevReg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevSwapModeTable(GXTevSwapSel table, GXTevColorChan red, GXTevColorChan green,
                           GXTevColorChan blue, GXTevColorChan alpha) {
    u32 *Kreg;
    int index = table * 2;

    Kreg = &gx->tevKsel[index];
    SET_REG_FIELD(*Kreg, 2, 0, red);
    SET_REG_FIELD(*Kreg, 2, 2, green);
    GX_WRITE_RAS_REG(*Kreg);

    Kreg = &gx->tevKsel[index + 1];
    SET_REG_FIELD(*Kreg, 2, 0, blue);
    SET_REG_FIELD(*Kreg, 2, 2, alpha);
    GX_WRITE_RAS_REG(*Kreg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetAlphaCompare(GXCompare comp0, u8 ref0, GXAlphaOp op, GXCompare comp1, u8 ref1) {
    u32 reg;

    reg = 0xF3000000;
    SET_REG_FIELD(reg, 8, 0, ref0);
    SET_REG_FIELD(reg, 8, 8, ref1);
    SET_REG_FIELD(reg, 3, 16, comp0);
    SET_REG_FIELD(reg, 3, 19, comp1);
    SET_REG_FIELD(reg, 2, 22, op);
    GX_WRITE_RAS_REG(reg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetZTexture(int op, GXTexFmt fmt, u32 bias) {
    u32 zenv0;
    u32 zenv1;
    u32 type;

    zenv0 = 0;
    SET_REG_FIELD(zenv0, 24, 0, bias);
    SET_REG_FIELD(zenv0, 8, 24, 0xF4);

    zenv1 = 0;
    switch (fmt) {
    case 0x11: /* GX_TF_Z8 */
        type = 0;
        break;
    case 0x13: /* GX_TF_Z16 */
        type = 1;
        break;
    case 0x16: /* GX_TF_Z24X8 */
        type = 2;
        break;
    default:
        type = 2;
        break;
    }
    SET_REG_FIELD(zenv1, 2, 0, type);
    SET_REG_FIELD(zenv1, 2, 2, op);
    SET_REG_FIELD(zenv1, 8, 24, 0xF5);
    GX_WRITE_RAS_REG(zenv0);
    GX_WRITE_RAS_REG(zenv1);
    gx->bpSentNot = GX_FALSE;
}

void GXSetTevOrder(GXTevStageID stage, GXTexCoordID coord, GXTexMapID map, GXChannelID color) {
    /* Rasterised colour selection for each GXChannelID. */
    static int c2r[] = {0, 1, 0, 1, 0, 1, 7, 5, 6, 0};
    u32 *ptref;
    u32 tmap;
    u32 tcoord;

    ptref = &gx->tref[stage / 2];
    gx->texmapId[stage] = map;

    tmap = map & ~0x100;
    tmap = (tmap >= 8) ? 0 : tmap;

    if (coord >= 8) {
        gx->tevTcEnab = gx->tevTcEnab & ~(1 << stage);
        tcoord = 0;
    } else {
        tcoord = coord;
        gx->tevTcEnab = gx->tevTcEnab | (1 << stage);
    }

    if (stage & 1) {
        SET_REG_FIELD(*ptref, 3, 12, tmap);
        SET_REG_FIELD(*ptref, 3, 15, tcoord);
        SET_REG_FIELD(*ptref, 3, 19, (color == 0xFF) ? 7 : c2r[color]);
        SET_REG_FIELD(*ptref, 1, 18, (map != 0xFF && !(map & 0x100)));
    } else {
        SET_REG_FIELD(*ptref, 3, 0, tmap);
        SET_REG_FIELD(*ptref, 3, 3, tcoord);
        SET_REG_FIELD(*ptref, 3, 7, (color == 0xFF) ? 7 : c2r[color]);
        SET_REG_FIELD(*ptref, 1, 6, (map != 0xFF && !(map & 0x100)));
    }

    GX_WRITE_RAS_REG(*ptref);
    gx->bpSentNot = GX_FALSE;
    gx->dirtyState |= GX_DIRTY_SU_TEX;
}

void GXSetNumTevStages(u8 nStages) {
    SET_REG_FIELD(gx->genMode, 4, 10, nStages - 1);
    gx->dirtyState |= GX_DIRTY_GEN_MODE;
}

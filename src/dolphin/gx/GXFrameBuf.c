/* Dolphin SDK GX: embedded frame buffer copies.
 *
 * Sets up and starts the copies out of the embedded frame buffer: display copies into an
 * external frame buffer (with vertical scaling, gamma and the anti-aliasing/deflicker filter)
 * and texture copies. Also defines the standard render modes. */
#include <dolphin/gx/GXPrivate.h>

/* NTSC 640x480 interlaced, with the deflicker filter. */
GXRenderModeObj GXNtsc480IntDf = {
    0, 640, 480, 480, 40, 0, 640, 480, 1, GX_FALSE, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

/* NTSC 640x480 progressive. */
GXRenderModeObj GXNtsc480Prog = {
    2, 640, 480, 480, 40, 0, 640, 480, 0, GX_FALSE, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {0, 0, 21, 22, 21, 0, 0},
};

/* MPAL 640x480 interlaced, with the deflicker filter. */
GXRenderModeObj GXMpal480IntDf = {
    8, 640, 480, 480, 40, 0, 640, 480, 1, GX_FALSE, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

/* PAL 640x528 interlaced, with the deflicker filter. */
GXRenderModeObj GXPal528IntDf = {
    4, 640, 528, 528, 40, 23, 640, 528, 1, GX_FALSE, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

/* EURGB60 640x480 interlaced, with the deflicker filter. */
GXRenderModeObj GXEurgb60Hz480IntDf = {
    20, 640, 480, 480, 40, 0, 640, 480, 1, GX_FALSE, GX_FALSE,
    {{6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}, {6, 6}},
    {8, 8, 10, 12, 10, 8, 8},
};

void GXSetDispCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    gx->cpDispSrc = 0;
    SET_REG_FIELD(gx->cpDispSrc, 10, 0, left);
    SET_REG_FIELD(gx->cpDispSrc, 10, 10, top);
    SET_REG_FIELD(gx->cpDispSrc, 8, 24, 0x49);
    gx->cpDispSize = 0;
    SET_REG_FIELD(gx->cpDispSize, 10, 0, wd - 1);
    SET_REG_FIELD(gx->cpDispSize, 10, 10, ht - 1);
    SET_REG_FIELD(gx->cpDispSize, 8, 24, 0x4A);
}

void GXSetTexCopySrc(u16 left, u16 top, u16 wd, u16 ht) {
    gx->cpTexSrc = 0;
    SET_REG_FIELD(gx->cpTexSrc, 10, 0, left);
    SET_REG_FIELD(gx->cpTexSrc, 10, 10, top);
    SET_REG_FIELD(gx->cpTexSrc, 8, 24, 0x49);
    gx->cpTexSize = 0;
    SET_REG_FIELD(gx->cpTexSize, 10, 0, wd - 1);
    SET_REG_FIELD(gx->cpTexSize, 10, 10, ht - 1);
    SET_REG_FIELD(gx->cpTexSize, 8, 24, 0x4A);
}

void GXSetDispCopyDst(u16 wd, u16 ht) {
    u16 stride;

    stride = (u16)(wd * 2);
    gx->cpDispStride = 0;
    SET_REG_FIELD(gx->cpDispStride, 10, 0, stride >> 5);
    SET_REG_FIELD(gx->cpDispStride, 8, 24, 0x4D);
}

void GXSetTexCopyDst(u16 wd, u16 ht, GXTexFmt fmt, GXBool mipmap) {
    u32 rowTiles;
    u32 colTiles;
    u32 cmpTiles;
    u32 peTexFmt;
    u32 peTexFmtH;

    gx->cpTexZ = GX_FALSE;
    peTexFmt = fmt & 0xF;
    if (fmt == 0x13) { /* GX_TF_Z16 */
        peTexFmt = 0xB;
    }
    switch (fmt) {
    case 0: /* GX_TF_I4 */
    case 1: /* GX_TF_I8 */
    case 2: /* GX_TF_IA4 */
    case 3: /* GX_TF_IA8 */
    case 0x26: /* GX_CTF_YUVA8 */
        SET_REG_FIELD(gx->cpTex, 2, 15, 3);
        break;
    default:
        SET_REG_FIELD(gx->cpTex, 2, 15, 2);
        break;
    }
    gx->cpTexZ = (fmt & 0x10) == 0x10;
    peTexFmtH = (peTexFmt & 8) >> 3;
    SET_REG_FIELD(gx->cpTex, 1, 3, peTexFmtH);
    peTexFmt = peTexFmt & 7;
    __GetImageTileCount(fmt, wd, ht, &rowTiles, &colTiles, &cmpTiles);
    gx->cpTexStride = 0;
    SET_REG_FIELD(gx->cpTexStride, 10, 0, rowTiles * cmpTiles);
    SET_REG_FIELD(gx->cpTexStride, 8, 24, 0x4D);
    SET_REG_FIELD(gx->cpTex, 1, 9, mipmap);
    SET_REG_FIELD(gx->cpTex, 3, 4, peTexFmt);
}

void GXSetDispCopyFrame2Field(int mode) {
    SET_REG_FIELD(gx->cpDisp, 2, 12, mode);
    SET_REG_FIELD(gx->cpTex, 2, 12, 0);
}

void GXSetCopyClamp(GXFBClamp clamp) {
    u8 clmpB;
    u8 clmpT;

    clmpT = (clamp & 1) == 1;
    clmpB = (clamp & 2) == 2;
    SET_REG_FIELD(gx->cpDisp, 1, 0, clmpT);
    SET_REG_FIELD(gx->cpDisp, 1, 1, clmpB);
    SET_REG_FIELD(gx->cpTex, 1, 0, clmpT);
    SET_REG_FIELD(gx->cpTex, 1, 1, clmpB);
}

/* Number of external frame buffer lines a copy of `efbHt` lines scaled by 256/iScale writes. */
static inline u32 __GXGetNumXfbLines(u32 efbHt, u32 iScale) {
    u32 count;
    u32 realHt;
    u32 iScaleD;

    count = (efbHt - 1) * 0x100;
    realHt = (count / iScale) + 1;
    iScaleD = iScale;
    if (iScaleD > 0x80 && iScaleD < 0x100) {
        while (iScaleD % 2 == 0) {
            iScaleD /= 2;
        }
        if (efbHt % iScaleD == 0) {
            realHt++;
        }
    }
    if (realHt > 0x400) {
        realHt = 0x400;
    }
    return realHt;
}

u32 GXSetDispCopyYScale(f32 vscale) {
    u8 enable;
    u32 iScale;
    u32 ht;
    u32 reg;

    iScale = (u32)(256.0f / vscale) & 0x1FF;
    enable = (iScale != 256);
    reg = 0;
    SET_REG_FIELD(reg, 9, 0, iScale);
    SET_REG_FIELD(reg, 8, 24, 0x4E);
    GX_WRITE_RAS_REG(reg);
    gx->bpSentNot = GX_FALSE;
    SET_REG_FIELD(gx->cpDisp, 1, 10, enable);
    ht = GET_REG_FIELD(gx->cpDispSize, 10, 10) + 1;
    return __GXGetNumXfbLines(ht, iScale);
}

void GXSetCopyClear(GXColor clear_clr, u32 clear_z) {
    u32 reg;

    reg = 0;
    SET_REG_FIELD(reg, 8, 0, clear_clr.r);
    SET_REG_FIELD(reg, 8, 8, clear_clr.a);
    SET_REG_FIELD(reg, 8, 24, 0x4F);
    GX_WRITE_RAS_REG(reg);
    reg = 0;
    SET_REG_FIELD(reg, 8, 0, clear_clr.b);
    SET_REG_FIELD(reg, 8, 8, clear_clr.g);
    SET_REG_FIELD(reg, 8, 24, 0x50);
    GX_WRITE_RAS_REG(reg);
    reg = 0;
    SET_REG_FIELD(reg, 24, 0, clear_z);
    SET_REG_FIELD(reg, 8, 24, 0x51);
    GX_WRITE_RAS_REG(reg);
    gx->bpSentNot = GX_FALSE;
}

void GXSetCopyFilter(GXBool aa, const u8 sample_pattern[12][2], GXBool vf, const u8 vfilter[7]) {
    u32 msLoc[4];
    u32 coeff0;
    u32 coeff1;

    if (aa) {
        msLoc[0] = 0;
        SET_REG_FIELD(msLoc[0], 4, 0, sample_pattern[0][0]);
        SET_REG_FIELD(msLoc[0], 4, 4, sample_pattern[0][1]);
        SET_REG_FIELD(msLoc[0], 4, 8, sample_pattern[1][0]);
        SET_REG_FIELD(msLoc[0], 4, 12, sample_pattern[1][1]);
        SET_REG_FIELD(msLoc[0], 4, 16, sample_pattern[2][0]);
        SET_REG_FIELD(msLoc[0], 4, 20, sample_pattern[2][1]);
        SET_REG_FIELD(msLoc[0], 8, 24, 1);
        msLoc[1] = 0;
        SET_REG_FIELD(msLoc[1], 4, 0, sample_pattern[3][0]);
        SET_REG_FIELD(msLoc[1], 4, 4, sample_pattern[3][1]);
        SET_REG_FIELD(msLoc[1], 4, 8, sample_pattern[4][0]);
        SET_REG_FIELD(msLoc[1], 4, 12, sample_pattern[4][1]);
        SET_REG_FIELD(msLoc[1], 4, 16, sample_pattern[5][0]);
        SET_REG_FIELD(msLoc[1], 4, 20, sample_pattern[5][1]);
        SET_REG_FIELD(msLoc[1], 8, 24, 2);
        msLoc[2] = 0;
        SET_REG_FIELD(msLoc[2], 4, 0, sample_pattern[6][0]);
        SET_REG_FIELD(msLoc[2], 4, 4, sample_pattern[6][1]);
        SET_REG_FIELD(msLoc[2], 4, 8, sample_pattern[7][0]);
        SET_REG_FIELD(msLoc[2], 4, 12, sample_pattern[7][1]);
        SET_REG_FIELD(msLoc[2], 4, 16, sample_pattern[8][0]);
        SET_REG_FIELD(msLoc[2], 4, 20, sample_pattern[8][1]);
        SET_REG_FIELD(msLoc[2], 8, 24, 3);
        msLoc[3] = 0;
        SET_REG_FIELD(msLoc[3], 4, 0, sample_pattern[9][0]);
        SET_REG_FIELD(msLoc[3], 4, 4, sample_pattern[9][1]);
        SET_REG_FIELD(msLoc[3], 4, 8, sample_pattern[10][0]);
        SET_REG_FIELD(msLoc[3], 4, 12, sample_pattern[10][1]);
        SET_REG_FIELD(msLoc[3], 4, 16, sample_pattern[11][0]);
        SET_REG_FIELD(msLoc[3], 4, 20, sample_pattern[11][1]);
        SET_REG_FIELD(msLoc[3], 8, 24, 4);
    } else {
        /* Every sample at the pixel centre. */
        msLoc[0] = 0x01666666;
        msLoc[1] = 0x02666666;
        msLoc[2] = 0x03666666;
        msLoc[3] = 0x04666666;
    }
    GX_WRITE_RAS_REG(msLoc[0]);
    GX_WRITE_RAS_REG(msLoc[1]);
    GX_WRITE_RAS_REG(msLoc[2]);
    GX_WRITE_RAS_REG(msLoc[3]);

    coeff0 = 0;
    SET_REG_FIELD(coeff0, 8, 24, 0x53);
    coeff1 = 0;
    SET_REG_FIELD(coeff1, 8, 24, 0x54);
    if (vf) {
        SET_REG_FIELD(coeff0, 6, 0, vfilter[0]);
        SET_REG_FIELD(coeff0, 6, 6, vfilter[1]);
        SET_REG_FIELD(coeff0, 6, 12, vfilter[2]);
        SET_REG_FIELD(coeff0, 6, 18, vfilter[3]);
        SET_REG_FIELD(coeff1, 6, 0, vfilter[4]);
        SET_REG_FIELD(coeff1, 6, 6, vfilter[5]);
        SET_REG_FIELD(coeff1, 6, 12, vfilter[6]);
    } else {
        /* Pass the middle line through unfiltered. */
        SET_REG_FIELD(coeff0, 6, 0, 0);
        SET_REG_FIELD(coeff0, 6, 6, 0);
        SET_REG_FIELD(coeff0, 6, 12, 21);
        SET_REG_FIELD(coeff0, 6, 18, 22);
        SET_REG_FIELD(coeff1, 6, 0, 21);
        SET_REG_FIELD(coeff1, 6, 6, 0);
        SET_REG_FIELD(coeff1, 6, 12, 0);
    }
    GX_WRITE_RAS_REG(coeff0);
    GX_WRITE_RAS_REG(coeff1);
    gx->bpSentNot = GX_FALSE;
}

void GXSetDispCopyGamma(GXGamma gamma) {
    SET_REG_FIELD(gx->cpDisp, 2, 7, gamma);
}

void GXCopyDisp(void *dest, GXBool clear) {
    u32 reg;
    u32 tempPeCtrl;
    u32 phyAddr;
    GXBool changePeCtrl;

    if (clear) {
        reg = gx->zmode;
        SET_REG_FIELD(reg, 1, 0, 1);
        SET_REG_FIELD(reg, 3, 1, 7);
        GX_WRITE_RAS_REG(reg);
        reg = gx->cmode0;
        SET_REG_FIELD(reg, 1, 0, 0);
        SET_REG_FIELD(reg, 1, 1, 0);
        GX_WRITE_RAS_REG(reg);
    }

    changePeCtrl = GX_FALSE;
    if ((clear || GET_REG_FIELD(gx->peCtrl, 3, 0) == 3) && GET_REG_FIELD(gx->peCtrl, 1, 6) == 1) {
        changePeCtrl = GX_TRUE;
        tempPeCtrl = gx->peCtrl;
        SET_REG_FIELD(tempPeCtrl, 1, 6, 0);
        GX_WRITE_RAS_REG(tempPeCtrl);
    }

    GX_WRITE_RAS_REG(gx->cpDispSrc);
    GX_WRITE_RAS_REG(gx->cpDispSize);
    GX_WRITE_RAS_REG(gx->cpDispStride);

    phyAddr = (u32)dest & 0x3FFFFFFF;
    reg = 0;
    SET_REG_FIELD(reg, 21, 0, phyAddr >> 5);
    SET_REG_FIELD(reg, 8, 24, 0x4B);
    GX_WRITE_RAS_REG(reg);

    SET_REG_FIELD(gx->cpDisp, 1, 11, clear);
    SET_REG_FIELD(gx->cpDisp, 1, 14, 1);
    SET_REG_FIELD(gx->cpDisp, 8, 24, 0x52);
    GX_WRITE_RAS_REG(gx->cpDisp);

    if (clear) {
        GX_WRITE_RAS_REG(gx->zmode);
        GX_WRITE_RAS_REG(gx->cmode0);
    }
    if (changePeCtrl) {
        GX_WRITE_RAS_REG(gx->peCtrl);
    }
    gx->bpSentNot = GX_FALSE;
}

void GXCopyTex(void *dest, GXBool clear) {
    u32 reg;
    u32 tempPeCtrl;
    u32 phyAddr;
    GXBool changePeCtrl;

    if (clear) {
        reg = gx->zmode;
        SET_REG_FIELD(reg, 1, 0, 1);
        SET_REG_FIELD(reg, 3, 1, 7);
        GX_WRITE_RAS_REG(reg);
        reg = gx->cmode0;
        SET_REG_FIELD(reg, 1, 0, 0);
        SET_REG_FIELD(reg, 1, 1, 0);
        GX_WRITE_RAS_REG(reg);
    }

    changePeCtrl = GX_FALSE;
    tempPeCtrl = gx->peCtrl;
    if (gx->cpTexZ && GET_REG_FIELD(tempPeCtrl, 3, 0) != 3) {
        changePeCtrl = GX_TRUE;
        SET_REG_FIELD(tempPeCtrl, 3, 0, 3);
    }
    if ((clear || GET_REG_FIELD(tempPeCtrl, 3, 0) == 3) && GET_REG_FIELD(tempPeCtrl, 1, 6) == 1) {
        changePeCtrl = GX_TRUE;
        SET_REG_FIELD(tempPeCtrl, 1, 6, 0);
    }
    if (changePeCtrl) {
        GX_WRITE_RAS_REG(tempPeCtrl);
    }

    GX_WRITE_RAS_REG(gx->cpTexSrc);
    GX_WRITE_RAS_REG(gx->cpTexSize);
    GX_WRITE_RAS_REG(gx->cpTexStride);

    phyAddr = (u32)dest & 0x3FFFFFFF;
    reg = 0;
    SET_REG_FIELD(reg, 21, 0, phyAddr >> 5);
    SET_REG_FIELD(reg, 8, 24, 0x4B);
    GX_WRITE_RAS_REG(reg);

    SET_REG_FIELD(gx->cpTex, 1, 11, clear);
    SET_REG_FIELD(gx->cpTex, 1, 14, 0);
    SET_REG_FIELD(gx->cpTex, 8, 24, 0x52);
    GX_WRITE_RAS_REG(gx->cpTex);

    if (clear) {
        GX_WRITE_RAS_REG(gx->zmode);
        GX_WRITE_RAS_REG(gx->cmode0);
    }
    if (changePeCtrl) {
        GX_WRITE_RAS_REG(gx->peCtrl);
    }
    gx->bpSentNot = GX_FALSE;
}

void GXClearBoundingBox(void) {
    GX_WRITE_RAS_REG(0x550003FF);
    GX_WRITE_RAS_REG(0x560003FF);
    gx->bpSentNot = GX_FALSE;
}

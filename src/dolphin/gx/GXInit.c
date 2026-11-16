/* Dolphin SDK GX: library start-up.
 *
 * GXInit maps the register blocks, binds one FIFO as both the CPU and the graphics FIFO, resets
 * the shadow registers in __GXData to their power-on values and then programs the whole default
 * pipeline state through the public setters (__GXInitGX). It also registers a reset function
 * that, at reset time, waits for the pixel engine to go idle and then aborts the FIFO. */
#include <dolphin/gx/GXPrivate.h>

/* This unit was built with the peephole pass off: retail keeps branches to the next block, the
 * separate compare after a mask and `mr` copies that the pass would fold. */
#pragma peephole off

char *__GXVersion = "<< Dolphin SDK - GX\trelease build: Nov 10 2004 06:27:12 (0x2301) >>";

/* Texture shown when nothing is loaded: one 4x4 RGBA8 tile of opaque white. */
static u8 GXDefaultTexData[32] __attribute__((aligned(32))) = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static GXVtxAttrFmtList GXDefaultVATList[] = {
    {9, 1, 4, 0},   /* position: xyz, f32 */
    {10, 0, 4, 0},  /* normal: xyz, f32 */
    {11, 1, 5, 0},  /* color 0: rgba8 */
    {12, 1, 5, 0},  /* color 1: rgba8 */
    {13, 1, 4, 0},  /* texcoords 0-7: st, f32 */
    {14, 1, 4, 0},
    {15, 1, 4, 0},
    {16, 1, 4, 0},
    {17, 1, 4, 0},
    {18, 1, 4, 0},
    {19, 1, 4, 0},
    {20, 1, 4, 0},
    {0xFF, 0, 0, 0},
};

/* Orthographic projection that maps the unit cube to itself. */
static f32 GXDefaultProjData[7] = {1.0f, 0.0f, 1.0f, 0.0f, -1.0f, -2.0f, 0.0f};

static GXFifoObj FifoObj;
static GXData gxData;
GXData *const __GXData = &gxData;

vu32 *__piReg = NULL;
vu16 *__cpReg = NULL;
vu16 *__peReg = NULL;
vu16 *__memReg = NULL;

static GXTexRegion *__GXDefaultTexRegionCallback(const GXTexObj *t_obj, GXTexMapID id) {
    GXTexFmt fmt = GXGetTexObjFmt(t_obj);
    GXBool mipmap = GXGetTexObjMipMap(t_obj);

    id = (GXTexMapID)(id % 8);
    switch (fmt) {
    case 6: /* RGBA8 */
        if (mipmap) {
            return &gx->TexRegions2[id];
        }
        return &gx->TexRegions1[id];
    case 8:  /* C4 */
    case 9:  /* C8 */
    case 10: /* C14X2 */
        return &gx->TexRegions0[id];
    default:
        if (mipmap) {
            return &gx->TexRegions1[id];
        }
        return &gx->TexRegions0[id];
    }
}

static GXTlutRegion *__GXDefaultTlutRegionCallback(u32 idx) {
    if (idx >= 20) {
        return NULL;
    }
    return &gx->TlutRegions[idx];
}

static BOOL __GXShutdown(BOOL final) {
    static u32 peCount;
    static OSTime time;
    static u32 calledOnce;
    u32 reg;
    OSTime now;

    if (!final) {
        if (!calledOnce) {
            peCount = __GXReadMEMCounterU32(MEM_PE_REQ_LO, MEM_PE_REQ_HI);
            time = OSGetTime();
            calledOnce = 1;
            return FALSE;
        }

        now = OSGetTime();
        reg = __GXReadMEMCounterU32(MEM_PE_REQ_LO, MEM_PE_REQ_HI);

        if (now - time < 10) {
            return FALSE;
        }
        if (reg != peCount) {
            peCount = reg;
            time = now;
            return FALSE;
        }
    } else {
        GXSetBreakPtCallback(NULL);
        GXSetDrawSyncCallback(NULL);
        GXSetDrawDoneCallback(NULL);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        GX_WRITE_U32(0);
        PPCSync();
        __cpReg[1] = 0;
        __cpReg[2] = 3;
        gx->abtWaitPECopy = GX_TRUE;
        __GXAbort();
    }
    return TRUE;
}

void __GXInitRevisionBits(void) {
    u32 i;
    u32 reg;
    u32 reg2;
    u32 reg3;

    for (i = 0; i < 8; i++) {
        SET_REG_FIELD(gx->vatA[i], 1, 30, 1);
        SET_REG_FIELD(gx->vatB[i], 1, 31, 1);
        GX_WRITE_U8(0x08);
        GX_WRITE_U8(i | 0x80);
        GX_WRITE_U32(gx->vatB[i]);
    }

    reg = 0;
    SET_REG_FIELD(reg, 1, 0, 1);
    SET_REG_FIELD(reg, 1, 1, 1);
    SET_REG_FIELD(reg, 1, 2, 1);
    SET_REG_FIELD(reg, 1, 3, 1);
    SET_REG_FIELD(reg, 1, 4, 1);
    SET_REG_FIELD(reg, 1, 5, 1);
    GX_WRITE_XF_REG(0, reg);

    reg3 = 0;
    SET_REG_FIELD(reg3, 1, 0, 1);
    GX_WRITE_XF_REG(0x12, reg3);

    reg2 = 0;
    SET_REG_FIELD(reg2, 1, 0, 1);
    SET_REG_FIELD(reg2, 1, 1, 1);
    SET_REG_FIELD(reg2, 1, 2, 1);
    SET_REG_FIELD(reg2, 1, 3, 1);
    SET_REG_FIELD(reg2, 8, 24, 0x58);
    GX_WRITE_RAS_REG(reg2);
}

static u32 GXTexRegionAddrTable[48] = {
    0x00000, 0x10000, 0x20000, 0x30000, 0x40000, 0x50000, 0x60000, 0x70000,
    0x08000, 0x18000, 0x28000, 0x38000, 0x48000, 0x58000, 0x68000, 0x78000,
    0x00000, 0x90000, 0x20000, 0xB0000, 0x40000, 0x98000, 0x60000, 0xB8000,
    0x80000, 0x10000, 0xA0000, 0x30000, 0x88000, 0x50000, 0xA8000, 0x70000,
    0x00000, 0x90000, 0x20000, 0xB0000, 0x40000, 0x90000, 0x60000, 0xB0000,
    0x80000, 0x10000, 0xA0000, 0x30000, 0x80000, 0x50000, 0xA0000, 0x70000,
};

static OSResetFunctionInfo GXResetFuncInfo = {__GXShutdown, 127};

GXFifoObj *GXInit(void *base, u32 size) {
    static u32 resetFuncRegistered = 0;
    u32 i;
    u32 reg;
    u32 freqBase;
    u32 hid2;

    OSRegisterVersion(__GXVersion);
    gx->inDispList = GX_FALSE;
    gx->dlSaveContext = GX_TRUE;
    gx->abtWaitPECopy = GX_TRUE;
    gx->tcsManEnab = 0;
    gx->tevTcEnab = 0;
    GXSetMisc(1, 0);

    __piReg = (vu32 *)0xCC003000;
    __cpReg = (vu16 *)0xCC000000;
    __peReg = (vu16 *)0xCC001000;
    __memReg = (vu16 *)0xCC004000;
    __GXFifoInit();
    GXInitFifoBase(&FifoObj, base, size);
    GXSetCPUFifo(&FifoObj);
    GXSetGPFifo(&FifoObj);

    if (!resetFuncRegistered) {
        OSRegisterResetFunction(&GXResetFuncInfo);
        resetFuncRegistered = 1;
    }

    __GXPEInit();

    hid2 = PPCMfhid2();
    PPCMtwpar(OSCachedToPhysical(0x8C008000));
    PPCMthid2(hid2 | 0x40000000);

    gx->genMode = 0;
    SET_REG_FIELD(gx->genMode, 8, 24, 0x00);
    gx->bpMask = 0xFF;
    SET_REG_FIELD(gx->bpMask, 8, 24, 0x0F);
    gx->lpSize = 0;
    SET_REG_FIELD(gx->lpSize, 8, 24, 0x22);

    for (i = 0; i < 16; i++) {
        gx->tevc[i] = 0;
        gx->teva[i] = 0;
        gx->tref[i / 2] = 0;
        gx->texmapId[i] = 0xFF;
        SET_REG_FIELD(gx->tevc[i], 8, 24, 0xC0 + i * 2);
        SET_REG_FIELD(gx->teva[i], 8, 24, 0xC1 + i * 2);
        SET_REG_FIELD(gx->tevKsel[i / 2], 8, 24, 0xF6 + i / 2);
        SET_REG_FIELD(gx->tref[i / 2], 8, 24, 0x28 + i / 2);
    }

    gx->iref = 0;
    SET_REG_FIELD(gx->iref, 8, 24, 0x27);

    for (i = 0; i < 8; i++) {
        gx->suTs0[i] = 0;
        gx->suTs1[i] = 0;
        SET_REG_FIELD(gx->suTs0[i], 8, 24, 0x30 + i * 2);
        SET_REG_FIELD(gx->suTs1[i], 8, 24, 0x31 + i * 2);
    }

    SET_REG_FIELD(gx->suScis0, 8, 24, 0x20);
    SET_REG_FIELD(gx->suScis1, 8, 24, 0x21);
    SET_REG_FIELD(gx->cmode0, 8, 24, 0x41);
    SET_REG_FIELD(gx->cmode1, 8, 24, 0x42);
    SET_REG_FIELD(gx->zmode, 8, 24, 0x40);
    SET_REG_FIELD(gx->peCtrl, 8, 24, 0x43);
    SET_REG_FIELD(gx->cpTex, 2, 7, 0);

    gx->zScale = 16777216.0f;
    gx->zOffset = 0.0f;
    gx->dirtyState = 0;
    gx->dirtyVAT = 0;

    freqBase = __OSBusClock / 500;
    __GXFlushTextureState();
    reg = (freqBase >> 11) | 0x400 | 0x69000000;
    GX_WRITE_RAS_REG(reg);

    __GXFlushTextureState();
    reg = (freqBase / 4224) | 0x200 | 0x46000000;
    GX_WRITE_RAS_REG(reg);

    __GXInitRevisionBits();

    for (i = 0; i < 8; i++) {
        GXInitTexCacheRegion(&gx->TexRegions0[i], GX_FALSE, GXTexRegionAddrTable[i], 0,
                             GXTexRegionAddrTable[i + 8], 0);
        GXInitTexCacheRegion(&gx->TexRegions1[i], GX_FALSE, GXTexRegionAddrTable[i + 16], 0,
                             GXTexRegionAddrTable[i + 24], 0);
        GXInitTexCacheRegion(&gx->TexRegions2[i], GX_TRUE, GXTexRegionAddrTable[i + 32], 0,
                             GXTexRegionAddrTable[i + 40], 0);
    }

    for (i = 0; i < 16; i++) {
        GXInitTlutRegion(&gx->TlutRegions[i], 0xC0000 + 0x2000 * i, 16);
    }
    for (i = 0; i < 4; i++) {
        GXInitTlutRegion(&gx->TlutRegions[i + 16], 0xE0000 + 0x8000 * i, 64);
    }

    __cpReg[3] = 0;
    SET_REG_FIELD(gx->perfSel, 4, 4, 0);
    GX_WRITE_CP_REG(0x20, gx->perfSel);
    GX_WRITE_XF_REG(6, 0);
    GX_WRITE_RAS_REG(0x23000000);
    GX_WRITE_RAS_REG(0x24000000);
    GX_WRITE_RAS_REG(0x67000000);
    __GXSetIndirectMask(0);
    __GXSetTmemConfig(2);
    __GXInitGX();

    return &FifoObj;
}

void __GXInitGX(void) {
    GXTexObj tex_obj;
    Mtx identity_mtx;
    GXColor clearColor = {64, 64, 64, 255};
    GXColor black = {0, 0, 0, 0};
    GXColor white = {255, 255, 255, 255};
    GXRenderModeObj *rmode;
    u32 i;

    switch (VIGetTvFormat()) {
    case 0:
        rmode = &GXNtsc480IntDf;
        break;
    case 1:
        rmode = &GXPal528IntDf;
        break;
    case 5:
        rmode = &GXEurgb60Hz480IntDf;
        break;
    case 2:
        rmode = &GXMpal480IntDf;
        break;
    default:
        rmode = &GXNtsc480IntDf;
        break;
    }

    GXSetCopyClear(clearColor, 0xFFFFFF);

    GXSetTexCoordGen2(0, 1, 4, 60, GX_FALSE, 125);
    GXSetTexCoordGen2(1, 1, 5, 60, GX_FALSE, 125);
    GXSetTexCoordGen2(2, 1, 6, 60, GX_FALSE, 125);
    GXSetTexCoordGen2(3, 1, 7, 60, GX_FALSE, 125);
    GXSetTexCoordGen2(4, 1, 8, 60, GX_FALSE, 125);
    GXSetTexCoordGen2(5, 1, 9, 60, GX_FALSE, 125);
    GXSetTexCoordGen2(6, 1, 10, 60, GX_FALSE, 125);
    GXSetTexCoordGen2(7, 1, 11, 60, GX_FALSE, 125);
    GXSetNumTexGens(1);
    GXClearVtxDesc();
    GXInvalidateVtxCache();

    for (i = 9; i <= 24; i++) {
        GXSetArray(i, gx, 0);
    }
    for (i = 0; i < 8; i++) {
        GXSetVtxAttrFmtv(i, GXDefaultVATList);
    }

    GXSetLineWidth(6, 0);
    GXSetPointSize(6, 0);
    GXEnableTexOffsets(0, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(1, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(2, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(3, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(4, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(5, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(6, GX_FALSE, GX_FALSE);
    GXEnableTexOffsets(7, GX_FALSE, GX_FALSE);

    identity_mtx[0][0] = 1.0f;
    identity_mtx[0][1] = 0.0f;
    identity_mtx[0][2] = 0.0f;
    identity_mtx[0][3] = 0.0f;
    identity_mtx[1][0] = 0.0f;
    identity_mtx[1][1] = 1.0f;
    identity_mtx[1][2] = 0.0f;
    identity_mtx[1][3] = 0.0f;
    identity_mtx[2][0] = 0.0f;
    identity_mtx[2][1] = 0.0f;
    identity_mtx[2][2] = 1.0f;
    identity_mtx[2][3] = 0.0f;
    GXLoadPosMtxImm(identity_mtx, 0);
    GXLoadNrmMtxImm(identity_mtx, 0);
    GXSetCurrentMtx(0);
    GXLoadTexMtxImm(identity_mtx, 60, 0);
    GXLoadTexMtxImm(identity_mtx, 125, 0);
    GXSetViewport(0.0f, 0.0f, rmode->fbWidth, rmode->xfbHeight, 0.0f, 1.0f);
    GXSetProjectionv(GXDefaultProjData);
    GXSetCoPlanar(GX_FALSE);
    GXSetCullMode(2);
    GXSetClipMode(0);
    GXSetScissor(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetScissorBoxOffset(0, 0);

    GXSetNumChans(0);
    GXSetChanCtrl(4, GX_FALSE, 0, 1, 0, 0, 2);
    GXSetChanAmbColor(4, black);
    GXSetChanMatColor(4, white);
    GXSetChanCtrl(5, GX_FALSE, 0, 1, 0, 0, 2);
    GXSetChanAmbColor(5, black);
    GXSetChanMatColor(5, white);

    GXInvalidateTexAll();
    GXSetTexRegionCallback(__GXDefaultTexRegionCallback);
    GXSetTlutRegionCallback(__GXDefaultTlutRegionCallback);

    GXInitTexObj(&tex_obj, GXDefaultTexData, 4, 4, 3, 0, 0, GX_FALSE);
    GXLoadTexObj(&tex_obj, 0);
    GXLoadTexObj(&tex_obj, 1);
    GXLoadTexObj(&tex_obj, 2);
    GXLoadTexObj(&tex_obj, 3);
    GXLoadTexObj(&tex_obj, 4);
    GXLoadTexObj(&tex_obj, 5);
    GXLoadTexObj(&tex_obj, 6);
    GXLoadTexObj(&tex_obj, 7);

    GXSetTevOrder(0, 0, 0, 4);
    GXSetTevOrder(1, 1, 1, 4);
    GXSetTevOrder(2, 2, 2, 4);
    GXSetTevOrder(3, 3, 3, 4);
    GXSetTevOrder(4, 4, 4, 4);
    GXSetTevOrder(5, 5, 5, 4);
    GXSetTevOrder(6, 6, 6, 4);
    GXSetTevOrder(7, 7, 7, 4);
    GXSetTevOrder(8, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(9, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(10, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(11, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(12, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(13, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(14, 0xFF, 0xFF, 0xFF);
    GXSetTevOrder(15, 0xFF, 0xFF, 0xFF);
    GXSetNumTevStages(1);
    GXSetTevOp(0, 3);
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetZTexture(0, 0x11, 0);
    for (i = 0; i < 16; i++) {
        GXSetTevKColorSel(i, 6);
        GXSetTevKAlphaSel(i, 0);
        GXSetTevSwapMode(i, 0, 0);
    }
    GXSetTevSwapModeTable(0, 0, 1, 2, 3);
    GXSetTevSwapModeTable(1, 0, 0, 0, 3);
    GXSetTevSwapModeTable(2, 1, 1, 1, 3);
    GXSetTevSwapModeTable(3, 2, 2, 2, 3);
    for (i = 0; i < 16; i++) {
        GXSetTevDirect(i);
    }
    GXSetNumIndStages(0);
    GXSetIndTexCoordScale(0, 0, 0);
    GXSetIndTexCoordScale(1, 0, 0);
    GXSetIndTexCoordScale(2, 0, 0);
    GXSetIndTexCoordScale(3, 0, 0);

    GXSetFog(0, 0.0f, 1.0f, 0.1f, 1.0f, black);
    GXSetFogRangeAdj(GX_FALSE, 0, NULL);
    GXSetBlendMode(0, 4, 5, 0);
    GXSetColorUpdate(GX_TRUE);
    GXSetAlphaUpdate(GX_TRUE);
    GXSetZMode(GX_TRUE, 3, GX_TRUE);
    GXSetZCompLoc(GX_TRUE);
    GXSetDither(GX_TRUE);
    GXSetDstAlpha(GX_FALSE, 0);
    GXSetPixelFmt(0, 0);
    GXSetFieldMask(GX_TRUE, GX_TRUE);
    GXSetFieldMode(rmode->field_rendering,
                   rmode->viHeight == 2 * rmode->xfbHeight ? GX_TRUE : GX_FALSE);

    GXSetDispCopySrc(0, 0, rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyDst(rmode->fbWidth, rmode->efbHeight);
    GXSetDispCopyYScale((f32)rmode->xfbHeight / (f32)rmode->efbHeight);
    GXSetCopyClamp(3);
    GXSetCopyFilter(rmode->aa, rmode->sample_pattern, GX_TRUE, rmode->vfilter);
    GXSetDispCopyGamma(0);
    GXSetDispCopyFrame2Field(0);
    GXClearBoundingBox();

    GXPokeColorUpdate(GX_TRUE);
    GXPokeAlphaUpdate(GX_TRUE);
    GXPokeDither(GX_FALSE);
    GXPokeBlendMode(0, 0, 1, 15);
    GXPokeAlphaMode(7, 0);
    GXPokeAlphaRead(1);
    GXPokeDstAlpha(GX_FALSE, 0);
    GXPokeZMode(GX_TRUE, 7, GX_TRUE);
    GXSetGPMetric(35, 22);
    GXClearGPMetric();
}

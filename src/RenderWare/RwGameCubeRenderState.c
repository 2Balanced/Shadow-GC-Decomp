/* RenderWare 3.7 GameCube driver: render states (0x80497198..0x80497FB3). The device's render state
 * open and close (GX's default state, the default texture and the 4x4 white raster), RwRenderStateGet
 * and RwRenderStateSet on the driver side, and the fog, alpha test, texture and blending helpers
 * they share with the raster renderer. The file owns the fog and blend tables and its jump tables
 * (.data 0x8056E958..0x8056EA8F), the render state cache (.bss lbl_805E3210, which game code reads
 * too), the default raster and texture (.sbss 0x805F1660) and its constants (.sdata2
 * 0x805FB3D0..0x805FB3DF: the white colour and the default fog planes). Built as C with the
 * driver's flags (-O4,p, no automatic inlining, writable strings); retail inlines the fog switch
 * and the blend helper into RwRenderStateSet and keeps them out of line as well, which is what
 * #pragma auto_inline on around their definitions gives.
 *
 * Names are inferred from behaviour; definitions are in retail address order. Matching: every
 * RwRenderStateSet case is an inline helper returning its result, as retail's code shows. */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include "Game/rw/r7.h"

void *memset(void *dst, int c, unsigned long n);

extern RwDlGlobals *lbl_805F15B4;  /* RwEngineInstance */
extern RwInt32 lbl_805F1658;       /* offset of the driver's data in a raster (RwGameCubeRaster.c) */

void fn_80499BC8(void);                               /* the texture cache reset */
void fn_80499C38(RwDlTexture *texture, RwInt32 map);  /* loads a texture into a GX texture map */
RwBool fn_8049714C(void *texture, void *raster, RwInt32 num); /* TextureSetRaster */
RwDlTexture *fn_8048CCDC(void *raster);               /* RwTextureCreate */
RwBool fn_8048CA00(RwDlTexture *texture, void *raster); /* RwTextureSetRaster */
RwBool fn_8048CD8C(RwDlTexture *texture);             /* RwTextureDestroy */
RwDlRaster *fn_80489E74(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags); /* RwRasterCreate */
void *fn_80489F70(void *raster, RwUInt32 level, RwInt32 mode);                         /* RwRasterLock */
void fn_80489B40(void *raster);                       /* RwRasterUnlock */
RwBool fn_80489BDC(void *raster);                     /* RwRasterDestroy */

/* .sbss (defined in reverse: MWCC lays them out from the last definition). */
RwDlTexture *lbl_805F1664;   /* the texture set for drawing */
RwDlRaster *lbl_805F1660;    /* a 4x4 white raster */

/* .bss: the render state cache. */
RwDlRenderState lbl_805E3210;

/* GX fog type of each RwFogType. */
static GXFogType sDlFogTypes[4] = {0, 2, 4, 5};

/* GX blend factor of each RwBlendFunction. */
static GXBlendFactor sDlBlendFactors[12] = {0, 0, 1, 2, 3, 4, 5, 6, 7, 2, 3, 0};

/* Render state open: GX's default state for RenderWare, the default texture (bilinear, wrapped)
 * and a white raster for untextured drawing. */
void fn_80497198(void) {
    GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    lbl_805E3210.fogEnabled = 0;
    lbl_805E3210.fogType = 1;
    lbl_805E3210.fogColorPacked = 0;
    lbl_805E3210.fogColor.rgba.red = 0;
    lbl_805E3210.fogColor.rgba.green = 0;
    lbl_805E3210.fogColor.rgba.blue = 0;
    lbl_805E3210.fogColor.rgba.alpha = 0;
    lbl_805E3210.fogStart = 5.0f;
    lbl_805E3210.fogEnd = 10.0f;
    lbl_805E3210.fogNear = 0.05f;
    lbl_805E3210.fogFar = 10.0f;
    lbl_805E3210.fogEndSet = 0;
    lbl_805E3210.zWrite = 1;
    lbl_805E3210.zTest = 1;
    lbl_805E3210.alphaTestOff = 1;
    lbl_805E3210.srcBlend = 5;
    lbl_805E3210.destBlend = 6;
    lbl_805E3210.cullMode = 2;
    lbl_805E3210.zFunc = 3;
    GXSetZMode(1, 3, 1);
    GXSetZCompLoc(1);
    lbl_805E3210.alphaComp0 = 4;
    lbl_805E3210.alphaRef0 = 0;
    lbl_805E3210.alphaOp = 0;
    lbl_805E3210.alphaComp1 = 7;
    lbl_805E3210.alphaRef1 = 0;
    lbl_805E3210.alphaSet = 0;
    GXSetAlphaCompare(7, 0, 0, 7, 0);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetCullMode(1);
    GXSetBlendMode(1, 4, 5, 0);
    GXSetChanCtrl(2, 0, 0, 0, 0, 0, 2);
    GXSetChanCtrl(3, 0, 0, 0, 0, 0, 2);
    GXSetChanCtrl(0, 0, 0, 0, 0, 0, 2);
    GXSetChanCtrl(1, 0, 0, 0, 0, 0, 2);
    GXSetChanMatColor(2, white);
    GXSetChanMatColor(3, white);
    GXSetChanMatColor(0, white);
    GXSetChanMatColor(1, white);
    GXSetColorUpdate(1);
    GXSetAlphaUpdate(1);
    GXSetCopyClear(white, 0xFFFFFF);
    GXSetCurrentMtx(0);
    fn_80499BC8();
    lbl_805F1664 = fn_8048CCDC(NULL);
    lbl_805F1664->filterAddressing = (lbl_805F1664->filterAddressing & 0xFFFFFF00) | 2;
    lbl_805F1664->filterAddressing = (lbl_805F1664->filterAddressing & 0xFFFF00FF) | 0x1100;
    lbl_805F1660 = fn_80489E74(4, 4, 0x10, 0x204);
    memset(fn_80489F70(lbl_805F1660, 0, 9), 0xFF, 0x20);
    fn_80489B40(lbl_805F1660);
}

/* Render state close: frees the white raster and the default texture. */
void fn_8049742C(void) {
    fn_80489BDC(lbl_805F1660);
    lbl_805F1660 = NULL;
    fn_8048CA00(lbl_805F1664, NULL);
    fn_8048CD8C(lbl_805F1664);
    lbl_805F1664 = NULL;
}

/* Driver side of RwRenderStateGet. */
RwBool fn_80497474(RwInt32 state, void *param) {
    RwInt32 *value = (RwInt32 *)param;
    switch (state) {
    case 14: /* rwRENDERSTATEFOGENABLE */
        *value = lbl_805E3210.fogEnabled;
        return 1;
    case 16: /* rwRENDERSTATEFOGTYPE */
        *value = lbl_805E3210.fogType;
        return 1;
    case 15: /* rwRENDERSTATEFOGCOLOR */
        *value = lbl_805E3210.fogColorPacked;
        return 1;
    case 17: /* rwRENDERSTATEFOGDENSITY */
        return 0;
    case 2: { /* rwRENDERSTATETEXTUREADDRESS */
        RwInt32 u = (lbl_805F1664->filterAddressing >> 8) & 0xF;
        RwInt32 v = (lbl_805F1664->filterAddressing >> 12) & 0xF;
        if (u == v) {
            *value = u;
            return 1;
        }
        return 0;
    }
    case 3: /* rwRENDERSTATETEXTUREADDRESSU */
        *value = (lbl_805F1664->filterAddressing >> 8) & 0xF;
        return 1;
    case 4: /* rwRENDERSTATETEXTUREADDRESSV */
        *value = (lbl_805F1664->filterAddressing >> 12) & 0xF;
        return 1;
    case 9: /* rwRENDERSTATETEXTUREFILTER */
        *value = lbl_805F1664->filterAddressing & 0xFF;
        return 1;
    case 1: /* rwRENDERSTATETEXTURERASTER */
        *value = (RwInt32)lbl_805F1664->raster;
        return 1;
    case 8: /* rwRENDERSTATEZWRITEENABLE */
        *value = lbl_805E3210.zWrite;
        return 1;
    case 6: /* rwRENDERSTATEZTESTENABLE */
        *value = lbl_805E3210.zTest;
        return 1;
    case 10: /* rwRENDERSTATESRCBLEND */
        *value = lbl_805E3210.srcBlend;
        return 1;
    case 11: /* rwRENDERSTATEDESTBLEND */
        *value = lbl_805E3210.destBlend;
        return 1;
    case 7: /* rwRENDERSTATESHADEMODE: always rwSHADEMODEGOURAUD */
        *value = 2;
        return 1;
    case 13: /* rwRENDERSTATEBORDERCOLOR */
        return 0;
    case 5: /* rwRENDERSTATETEXTUREPERSPECTIVE */
        *value = 1;
        return 1;
    case 20: /* rwRENDERSTATECULLMODE */
        *value = lbl_805E3210.cullMode;
        return 1;
    case 29: /* rwRENDERSTATEALPHATESTFUNCTION: only a single test against ref0 has one */
        if (lbl_805E3210.alphaOp == 0 && lbl_805E3210.alphaComp1 == 7) {
            *value = lbl_805E3210.alphaComp0 + 1;
        } else {
            *value = 0;
        }
        return 1;
    case 30: /* rwRENDERSTATEALPHATESTFUNCTIONREF */
        *value = lbl_805E3210.alphaRef0;
        return 1;
    default:
        return 0;
    }
}

#pragma auto_inline on
/* Fog on (with the current camera's planes, the fog end only if the application has not set
 * it) or off. */
RwBool fn_80497668(RwBool enable) {
    if (enable) {
        if (lbl_805E3210.fogEnabled == 0) {
            RwDlCamera *camera = lbl_805F15B4->curCamera;
            if (camera != NULL) {
                if (lbl_805E3210.fogEndSet == 0) lbl_805E3210.fogEnd = camera->farPlane;
                lbl_805E3210.fogStart = camera->fogPlane;
                lbl_805E3210.fogNear = camera->nearPlane;
                lbl_805E3210.fogFar = camera->farPlane;
            }
            GXSetFog(sDlFogTypes[lbl_805E3210.fogType], lbl_805E3210.fogStart, lbl_805E3210.fogEnd,
                     lbl_805E3210.fogNear, lbl_805E3210.fogFar, *(GXColor *)&lbl_805E3210.fogColor);
            lbl_805E3210.fogEnabled = 1;
        }
    } else if (lbl_805E3210.fogEnabled != 0) {
        GXSetFog(0, 5.0f, 10.0f, 0.05f, 10.0f, *(GXColor *)&lbl_805E3210.fogColor);
        lbl_805E3210.fogEnabled = 0;
    }
    return 1;
}
#pragma auto_inline reset

/* Switches the alpha test: off (Z compared before texturing) or the test kept in the cache. */
static inline void RwDlSetAlphaTestOff(RwInt32 off) {
    if (lbl_805E3210.alphaTestOff != off) {
        if (off == 1) {
            GXSetAlphaCompare(7, 0, 0, 7, 0);
        } else {
            GXSetAlphaCompare(lbl_805E3210.alphaComp0, lbl_805E3210.alphaRef0, lbl_805E3210.alphaOp,
                              lbl_805E3210.alphaComp1, lbl_805E3210.alphaRef1);
        }
        GXSetZCompLoc(off);
        lbl_805E3210.alphaTestOff = off;
    }
}

void fn_80497778(RwInt32 off) {
    RwDlSetAlphaTestOff(off);
}

/* Sets the texture of the current raster on texture map 0 and switches the alpha test off for a
 * raster without alpha. */
void fn_80497804(void) {
    if (lbl_805F1664->raster != NULL) {
        fn_80499C38(lbl_805F1664, 0);
        RwDlSetAlphaTestOff(
            (*(RwUInt32 *)((u8 *)lbl_805F1664->raster + lbl_805F1658 + 0x14) & 1) ^ 1);
    }
}

#pragma auto_inline on
/* Blending: source and destination RwBlendFunction. */
void fn_804978C0(RwInt32 src, RwInt32 dest) {
    GXSetBlendMode(1, sDlBlendFactors[src], sDlBlendFactors[dest], 0);
}
#pragma auto_inline reset

/* RwRenderStateSet's helpers: each returns whether the state could be set. */
static inline RwBool RwDlSetFogColor(RwInt32 color) {
    if (color != lbl_805E3210.fogColorPacked) {
        RwDlCamera *camera = lbl_805F15B4->curCamera;
        lbl_805E3210.fogColor.rgba.alpha = (RwUInt32)color >> 24;
        lbl_805E3210.fogColor.rgba.red = (RwUInt32)color >> 16;
        lbl_805E3210.fogColor.rgba.green = (RwUInt32)color >> 8;
        lbl_805E3210.fogColor.rgba.blue = color;
        GXSetFog(sDlFogTypes[lbl_805E3210.fogType], camera->fogPlane, camera->farPlane, camera->nearPlane,
                 camera->farPlane, *(GXColor *)&lbl_805E3210.fogColor);
        lbl_805E3210.fogColorPacked = color;
    }
    return 1;
}

static inline RwBool RwDlSetFogType(RwInt32 type) {
    if (type != lbl_805E3210.fogType) {
        RwDlCamera *camera;
        if (type != 1) return 0;
        camera = lbl_805F15B4->curCamera;
        GXSetFog(sDlFogTypes[type], camera->fogPlane, camera->farPlane, camera->nearPlane, camera->farPlane,
                 *(GXColor *)&lbl_805E3210.fogColor);
        lbl_805E3210.fogType = type;
    }
    return 1;
}

static inline RwBool RwDlSetTextureAddress(RwInt32 mode) {
    if (mode == 4) return 0;
    lbl_805F1664->filterAddressing =
        (lbl_805F1664->filterAddressing & 0xFFFF00FF) | (((mode << 8) & 0xF00) | ((mode << 12) & 0xF000));
    return 1;
}

static inline RwBool RwDlSetTextureAddressU(RwInt32 mode) {
    if (mode == 4) return 0;
    lbl_805F1664->filterAddressing = (lbl_805F1664->filterAddressing & ~0xF00) | ((mode << 8) & 0xF00);
    return 1;
}

static inline RwBool RwDlSetTextureAddressV(RwInt32 mode) {
    if (mode == 4) return 0;
    lbl_805F1664->filterAddressing = (lbl_805F1664->filterAddressing & ~0xF000) | ((mode << 12) & 0xF000);
    return 1;
}

static inline RwBool RwDlSetTextureFilter(RwInt32 mode) {
    lbl_805F1664->filterAddressing = (lbl_805F1664->filterAddressing & ~0xFF) | (mode & 0xFF);
    return 1;
}

static inline RwBool RwDlSetTextureRaster(void *raster) {
    if (raster != lbl_805F1664->raster) fn_8049714C(lbl_805F1664, raster, 0);
    return 1;
}

static inline RwBool RwDlSetZWrite(RwBool enable) {
    if (enable) {
        if (lbl_805E3210.zWrite == 0) {
            GXSetZMode(1, lbl_805E3210.zFunc, 1);
            lbl_805E3210.zWrite = 1;
        }
    } else if (lbl_805E3210.zWrite != 0) {
        GXSetZMode(1, lbl_805E3210.zFunc, 0);
        lbl_805E3210.zWrite = 0;
    }
    return 1;
}

static inline RwBool RwDlSetZTest(RwBool enable) {
    if (enable) {
        if (lbl_805E3210.zTest == 0) {
            GXSetZMode(1, 3, lbl_805E3210.zWrite);
            lbl_805E3210.zFunc = 3;
            lbl_805E3210.zTest = 1;
        }
    } else if (lbl_805E3210.zTest != 0) {
        GXSetZMode(1, 7, lbl_805E3210.zWrite);
        lbl_805E3210.zFunc = 7;
        lbl_805E3210.zTest = 0;
    }
    return 1;
}

static inline RwBool RwDlSetSrcBlend(RwInt32 blend) {
    if (blend != lbl_805E3210.srcBlend) {
        switch (blend) {
        case 1:
        case 2:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            fn_804978C0(blend, lbl_805E3210.destBlend);
            lbl_805E3210.srcBlend = blend;
            break;
        case 3:
        case 4:
        case 11:
        default:
            return 0;
        }
    }
    return 1;
}

static inline RwBool RwDlSetDestBlend(RwInt32 blend) {
    if (blend != lbl_805E3210.destBlend) {
        switch (blend) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            fn_804978C0(lbl_805E3210.srcBlend, blend);
            lbl_805E3210.destBlend = blend;
            break;
        case 9:
        case 10:
        case 11:
        default:
            return 0;
        }
    }
    return 1;
}

static inline RwBool RwDlSetCullMode(RwInt32 mode) {
    if (mode != lbl_805E3210.cullMode) {
        GXSetCullMode(mode - 1);
        lbl_805E3210.cullMode = mode;
    }
    return 1;
}

static inline RwBool RwDlSetAlphaTestRef(RwInt32 ref) {
    if (ref != lbl_805E3210.alphaRef0) {
        if (lbl_805E3210.alphaTestOff != 1) {
            GXSetAlphaCompare(lbl_805E3210.alphaComp0, ref, lbl_805E3210.alphaOp, lbl_805E3210.alphaComp1,
                              lbl_805E3210.alphaRef1);
        }
        lbl_805E3210.alphaRef0 = ref;
    }
    return 1;
}

/* Driver side of RwRenderStateSet. */
RwBool fn_80497904(RwInt32 state, RwInt32 value) {
    RwBool result = 0;
    switch (state) {
    case 14: /* rwRENDERSTATEFOGENABLE */
        result = fn_80497668(value);
        break;
    case 15: /* rwRENDERSTATEFOGCOLOR */
        result = RwDlSetFogColor(value);
        break;
    case 16: /* rwRENDERSTATEFOGTYPE */
        result = RwDlSetFogType(value);
        break;
    case 17: /* rwRENDERSTATEFOGDENSITY */
        result = 0;
        break;
    case 2: /* rwRENDERSTATETEXTUREADDRESS */
        result = RwDlSetTextureAddress(value);
        break;
    case 3: /* rwRENDERSTATETEXTUREADDRESSU */
        result = RwDlSetTextureAddressU(value);
        break;
    case 4: /* rwRENDERSTATETEXTUREADDRESSV */
        result = RwDlSetTextureAddressV(value);
        break;
    case 9: /* rwRENDERSTATETEXTUREFILTER */
        result = RwDlSetTextureFilter(value);
        break;
    case 1: /* rwRENDERSTATETEXTURERASTER */
        result = RwDlSetTextureRaster((void *)value);
        break;
    case 8: /* rwRENDERSTATEZWRITEENABLE */
        result = RwDlSetZWrite(value);
        break;
    case 6: /* rwRENDERSTATEZTESTENABLE */
        result = RwDlSetZTest(value);
        break;
    case 10: /* rwRENDERSTATESRCBLEND */
        result = RwDlSetSrcBlend(value);
        break;
    case 11: /* rwRENDERSTATEDESTBLEND */
        result = RwDlSetDestBlend(value);
        break;
    case 7: /* rwRENDERSTATESHADEMODE: only Gouraud */
        result = value == 2;
        break;
    case 13: /* rwRENDERSTATEBORDERCOLOR */
        result = 0;
        break;
    case 5: /* rwRENDERSTATETEXTUREPERSPECTIVE */
        result = value;
        break;
    case 20: /* rwRENDERSTATECULLMODE */
        result = RwDlSetCullMode(value);
        break;
    case 29: /* rwRENDERSTATEALPHATESTFUNCTION */
        if (lbl_805E3210.alphaSet) {
            if (lbl_805E3210.alphaTestOff != 1) {
                GXSetAlphaCompare(value - 1, lbl_805E3210.alphaRef0, 0, 7, 0);
            }
            lbl_805E3210.alphaComp0 = value - 1;
            lbl_805E3210.alphaOp = 0;
            lbl_805E3210.alphaComp1 = 7;
            lbl_805E3210.alphaRef1 = 0;
        } else {
            if (--value != lbl_805E3210.alphaComp0) {
                if (lbl_805E3210.alphaTestOff != 1) {
                    GXSetAlphaCompare(value, lbl_805E3210.alphaRef0, lbl_805E3210.alphaOp, lbl_805E3210.alphaComp1,
                                      lbl_805E3210.alphaRef1);
                }
                lbl_805E3210.alphaComp0 = value;
            }
        }
        result = 1;
        break;
    case 30: /* rwRENDERSTATEALPHATESTFUNCTIONREF */
        result = RwDlSetAlphaTestRef(value);
        break;
    }
    return result;
}

/* Keeps an alpha test for textures with alpha; puts the previous one in GX unless the test is
 * off. */
void fn_80497F24(RwInt32 comp0, u8 ref0, RwInt32 op, RwInt32 comp1, u8 ref1) {
    if (lbl_805E3210.alphaTestOff != 1) {
        GXSetAlphaCompare(lbl_805E3210.alphaComp0, lbl_805E3210.alphaRef0, lbl_805E3210.alphaOp,
                          lbl_805E3210.alphaComp1, lbl_805E3210.alphaRef1);
    }
    lbl_805E3210.alphaComp0 = comp0;
    lbl_805E3210.alphaRef0 = ref0;
    lbl_805E3210.alphaOp = op;
    lbl_805E3210.alphaComp1 = comp1;
    lbl_805E3210.alphaRef1 = ref1;
    lbl_805E3210.alphaSet = 1;
}

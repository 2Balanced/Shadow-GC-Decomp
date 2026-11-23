/* RenderWare GameCube driver, 0x80497474..0x80498E07: the render state file (RwRenderStateGet
 * and its helpers for fog, the alpha test and blending; the cache at lbl_805E3210, its tables at
 * 0x8056E958) and raster rendering (RwRasterRender and its variants). Names are inferred from
 * behaviour; definitions are in retail address order.
 *
 * Not written yet: fn_80497904 (RwRenderStateSet; it inlines the helpers above), fn_80497FB4,
 * fn_80498344 (the raster blit) and fn_80498AC4. Every function written matches. */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include "Game/rw/r7.h"

extern "C" {
extern RwDlGlobals *lbl_805F15B4;  /* RwEngineInstance */
extern RwDlRenderState lbl_805E3210;
extern GXFogType lbl_8056E958[4];  /* RwFogType -> GXFogType */
extern s32 lbl_8056E968[12];       /* RwBlendFunction -> GXBlendFactor */
extern RwDlTexture *lbl_805F1664;  /* the texture set for drawing */
extern RwInt32 lbl_805F1658;       /* offset of the driver's data in a raster */

void fn_80499C38(RwDlTexture *texture, RwInt32 map);
RwBool fn_80498344(RwDlRaster *raster, RwDlRect *rect);
extern RwDlRaster *lbl_805F1668; /* the current raster (the render target) */
/* This file's messages are reached as symbols of the size-cut .data, not as literals pooled in
 * this unit (which would not be at retail's addresses). */
extern char lbl_8056EADC[]; /* "SRC, DST Raster render combination not supported" */

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
            GXSetFog(lbl_8056E958[lbl_805E3210.fogType], lbl_805E3210.fogStart, lbl_805E3210.fogEnd,
                     lbl_805E3210.fogNear, lbl_805E3210.fogFar, *(GXColor *)&lbl_805E3210.fogColor);
            lbl_805E3210.fogEnabled = 1;
        }
    } else if (lbl_805E3210.fogEnabled != 0) {
        GXSetFog(0, 5.0f, 10.0f, 0.05f, 10.0f, *(GXColor *)&lbl_805E3210.fogColor);
        lbl_805E3210.fogEnabled = 0;
    }
    return 1;
}

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

/* Blending: source and destination RwBlendFunction. */
void fn_804978C0(RwInt32 src, RwInt32 dest) {
    GXSetBlendMode(1, lbl_8056E968[src], lbl_8056E968[dest], 0);
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

/* Draws a raster into the current one (RwRasterRender and its variants): only into a camera
 * raster, from a normal, texture or camera texture raster; the rectangle's size is the
 * raster's own unless the render is scaled. */
RwBool fn_804988D4(RwDlRaster *raster, RwDlRect *rect, RwBool scaled, RwBool blend) {
    RwDlRect r;
    r.x = rect->x;
    r.y = rect->y;
    if (scaled) {
        r.w = rect->w;
        r.h = rect->h;
    } else {
        r.w = raster->width;
        r.h = raster->height;
    }
    switch (lbl_805F1668->cType) {
    case 0:
    case 4:
    case 5:
        switch (raster->cType) {
        case 0:
        case 1:
        case 2:
        case 4:
        case 5:
            RW_ERROR_STRING(2, lbl_8056EADC);
            return 0;
        }
        break;
    case 2:
        switch (raster->cType) {
        case 0:
        case 4:
        case 5:
            fn_80498344(raster, &r);
            break;
        case 1:
        case 2:
            RW_ERROR_STRING(2, lbl_8056EADC);
            return 0;
        }
        break;
    default:
        RW_ERROR_STRING(2, lbl_8056EADC);
        return 0;
    }
    return 1;
}

/* Driver standard functions: RwRasterRender, RwRasterRenderFast and RwRasterRenderScaled. */
RwBool fn_80498A4C(void *out, void *inOut, RwInt32 num) {
    return fn_804988D4((RwDlRaster *)out, (RwDlRect *)inOut, 0, 1);
}

RwBool fn_80498A74(void *out, void *inOut, RwInt32 num) {
    return fn_804988D4((RwDlRaster *)out, (RwDlRect *)inOut, 0, 0);
}

RwBool fn_80498A9C(void *out, void *inOut, RwInt32 num) {
    return fn_804988D4((RwDlRaster *)out, (RwDlRect *)inOut, 1, 1);
}
}

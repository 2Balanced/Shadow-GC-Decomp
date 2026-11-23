/* RenderWare 3.7 GameCube driver: raster rendering and clearing (0x80497FB4..0x8049952B). The GX
 * set-up for drawing a raster as a textured quad, the raster blit, the driver's RasterRender,
 * RasterRenderFast and RasterRenderScaled, the clear quad and the RasterClear, RasterClearRect and
 * SetRasterContext standard functions. The file owns two pixel-space projections and position
 * matrices and its message and jump table (.data 0x8056EA90..0x8056EBCF), the raster context
 * (.sbss 0x805F1668) and its constants (.sdata2 0x805FB3E0..0x805FB417, which repeat the Im2D
 * file's 0.0, 1.0, 2.0 and -2.0: a pool of its own). Built as C with the driver's flags (-O4,p,
 * no automatic inlining, writable strings).
 *
 * Names are inferred from behaviour; definitions are in retail address order.
 * Not written yet: fn_80498344 (the blit), fn_80498AC4 (the clear quad), fn_80498E08 (clears a
 * rectangle) and fn_804990D4 (RasterClearRect's worker). */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include "Game/rw/r7.h"

extern RwDlRenderMode *lbl_805F164C;  /* render mode in use (RwGameCubeDevice.c) */
extern RwInt32 lbl_805F15F0;          /* nonzero: the frame is drawn in two halves */
extern RwInt32 lbl_805EDF28;          /* nonzero while the top half is drawn */
extern RwInt32 lbl_805F1648;          /* the line where the two halves meet */

u32 fn_8037F1BC(void); /* VIGetNextField */

/* .sbss: the raster the clear and render functions work on (SetRasterContext). */
RwDlRaster *lbl_805F1668;

/* The pixel-space orthographic projection and the position matrix of the raster blit. */
static f32 sDlBlitProjection[7] = {1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f};
static Mtx sDlBlitPosMtx = {
    {1.0f, 0.0f, 0.0f, 0.5f},
    {0.0f, 1.0f, 0.0f, 0.5f},
    {0.0f, 0.0f, -1.0f, 0.0f},
};

/* Sets GX up for drawing a raster as a textured quad into a rectangle of the current raster:
 * 16-bit positions and float texture coordinates in vertex format 0, the texture replacing the
 * colour, a viewport over the frame buffer, a scissor box over the rectangle (clipped to the half
 * being drawn in two-halves mode), and the pixel-space projection and position matrix. */
void fn_80497FB4(RwDlRect *rect) {
    RwDlRenderMode *mode;
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxAttrFmt(0, 9, 0, 3, 0);
    GXSetVtxDesc(0xD, 1);
    GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
    GXSetNumTevStages(1);
    GXSetTevOrder(0, 0, 0, 0xFF);
    GXSetTevOp(0, 3);
    GXSetNumChans(0);
    GXSetChanCtrl(4, 0, 0, 0, 0, 0, 2);
    GXSetChanCtrl(5, 0, 0, 0, 0, 0, 2);
    mode = lbl_805F164C;
    if (mode->gx.field_rendering != 0) {
        GXSetViewportJitter(0.0f, 0.0f, mode->gx.fbWidth, mode->gx.xfbHeight, 0.0f, 1.0f, fn_8037F1BC() ^ 1);
    } else {
        GXSetViewport(0.0f, 0.0f, mode->gx.fbWidth, mode->gx.xfbHeight, 0.0f, 1.0f);
    }
    if (lbl_805F15F0 == 0) {
        GXSetScissor(lbl_805F1668->nOffsetX + rect->x, lbl_805F1668->nOffsetY + rect->y, rect->w, rect->h);
    } else if (lbl_805EDF28 != 0) {
        RwInt32 limit = lbl_805F1648 + 2;
        RwInt32 y = lbl_805F1668->nOffsetY * 2 + rect->y;
        if (rect->h + y <= limit) {
            GXSetScissor(lbl_805F1668->nOffsetX + rect->x, y, rect->w, rect->h);
        } else if (y > limit) {
            GXSetScissor(0, 0, lbl_805F164C->gx.fbWidth, limit);
        } else {
            GXSetScissor(lbl_805F1668->nOffsetX + rect->x, y, rect->w, limit - y);
        }
        GXSetScissorBoxOffset(0, 0);
    } else {
        RwInt32 limit = lbl_805F1648 - 2;
        RwInt32 y = lbl_805F1668->nOffsetY * 2 + rect->y;
        if (y >= limit) {
            GXSetScissor(lbl_805F1668->nOffsetX + rect->x, y, rect->w, rect->h);
        } else {
            RwInt32 bottom = rect->h + y;
            if (bottom < limit) {
                GXSetScissor(0, limit, lbl_805F164C->gx.fbWidth, lbl_805F1648 + 2);
            } else {
                GXSetScissor(lbl_805F1668->nOffsetX + rect->x, limit, rect->w, bottom - limit);
            }
        }
        GXSetScissorBoxOffset(0, lbl_805F1648 - 2);
    }
    sDlBlitProjection[1] = 2.0f / lbl_805F164C->gx.fbWidth;
    sDlBlitProjection[3] = -2.0f / lbl_805F164C->gx.xfbHeight;
    GXSetProjectionv(sDlBlitProjection);
    GXLoadPosMtxImm(sDlBlitPosMtx, 0);
    GXSetCurrentMtx(0);
}

RwBool fn_80498344(RwDlRaster *raster, RwDlRect *rect);

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
            RW_ERROR_STRING(2, "SRC, DST Raster render combination not supported");
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
            RW_ERROR_STRING(2, "SRC, DST Raster render combination not supported");
            return 0;
        }
        break;
    default:
        RW_ERROR_STRING(2, "SRC, DST Raster render combination not supported");
        return 0;
    }
    return 1;
}

/* Driver standard functions RasterRender, RasterRenderFast and RasterRenderScaled. */
RwBool fn_80498A4C(void *out, void *inOut, RwInt32 num) {
    return fn_804988D4((RwDlRaster *)out, (RwDlRect *)inOut, 0, 1);
}

RwBool fn_80498A74(void *out, void *inOut, RwInt32 num) {
    return fn_804988D4((RwDlRaster *)out, (RwDlRect *)inOut, 0, 0);
}

RwBool fn_80498A9C(void *out, void *inOut, RwInt32 num) {
    return fn_804988D4((RwDlRaster *)out, (RwDlRect *)inOut, 1, 1);
}

/* The pixel-space orthographic projection and the position matrix of the clear quad. */
static f32 sDlClearProjection[7] = {1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f};
static Mtx sDlClearPosMtx = {
    {1.0f, 0.0f, 0.0f, 0.5f},
    {0.0f, 1.0f, 0.0f, 0.5f},
    {0.0f, 0.0f, -1.0f, 0.0f},
};

RwBool fn_804990D4(RwDlRaster *raster, void *rect, RwInt32 num);

/* Driver standard function RasterClearRect: clears a rectangle of the current raster. */
RwBool fn_804994B8(void *out, void *inOut, RwInt32 num) {
    return fn_804990D4(lbl_805F1668, inOut, num);
}

/* Driver standard function RasterClear: clears the whole current raster. */
RwBool fn_804994DC(void *out, void *inOut, RwInt32 num) {
    RwDlRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = lbl_805F1668->width;
    rect.h = lbl_805F1668->height;
    return fn_804990D4(lbl_805F1668, &rect, num);
}

/* Driver standard function SetRasterContext: sets the current raster. */
RwBool fn_80499520(void *out, void *inOut, RwInt32 num) {
    lbl_805F1668 = (RwDlRaster *)inOut;
    return 1;
}

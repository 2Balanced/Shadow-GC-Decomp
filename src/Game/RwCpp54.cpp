/* RenderWare 0x8049043C..0x80490E2F: the start of the GameCube driver's immediate-mode 2D file (GX
 * set-up and restore, RwIm2DRenderTriangle and RwIm2DRenderLine). Names are inferred from
 * behaviour; definitions are in retail address order. The palette quantiser's tail that this size
 * cut used to open with (0x8048F5D0..0x8049043B) moved to RenderWare/RwPaletteQuantize.c in wave 41
 * (range r6: the quantiser file starts at 0x8048D85C). */
#include <string.h>
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/gx/GXPrivate.h>
#include "Game/rw/r7.h"

extern "C" {
extern u8 *lbl_805F15B0;
extern s32 lbl_805EDF20;
extern u32 lbl_805E2D58[];
extern u32 lbl_805FB560;
extern f32 lbl_805FB368;

void *fn_8047FBD8(u32 size, u32 entries, u32 align, u32 hint); /* RwFreeListCreate */
void fn_8047FCDC(void *freeList); /* RwFreeListDestroy */
void fn_8048EB24(f32 *, const u8 *, s32);
u32 fn_8048EEFC(u32 *, s32);
u32 fn_8048F1E4(f32 *, u8 *, u32, s32);
void fn_8048E300(u32 *, u32 *, u32, u32 *, u32);
void fn_8048E47C(f32 *, u32 *, u32, u32 *, f32 *);
f32 fn_8048E748(f32 *, u32 *, u32, u32 *, f32 *);
u32 fn_8048E988(f32 *, f32 *, u32 *, u32 *);
u32 fn_8048F4B4(u32 *, u32, s32);
extern RwDlGlobals *lbl_805F15B4;     /* RwEngineInstance */

#define RW_API(type, offset) ((type)*(u32 *)((u8 *)lbl_805F15B4 + (offset)))

/* ---- 0x8049043C: start of the driver's immediate-mode 2D file (its .sdata2 constants start at
 * 0x805FB380 and its .data, the GX primitive table, at 0x8056E060). ---- */
extern RwDlRenderMode *lbl_805F164C;  /* render mode in use */
extern u32 *lbl_805F1664;             /* -> nonzero while Im2D draws with a texture */
extern s32 lbl_805F15F0;              /* nonzero: the frame is drawn in two halves */
extern s32 lbl_805EDF28;              /* nonzero while the top half is drawn */
extern s32 lbl_805F1648;              /* the line where the two halves meet */
extern f32 lbl_8056E07C[7];           /* GXSetProjectionv array: pixel-space orthographic */
extern Mtx lbl_8056E098;        /* position matrix: the camera raster's offset */
extern f32 lbl_805E3158[7];           /* the 3D projection, saved while Im2D draws */

u32 fn_8037F1BC(void); /* VIGetNextField */
void fn_80497804(void);

/* Sets GX up for 2D drawing: position and colour (and texture coordinates while a texture is
 * set) in vertex format 0, the vertex colour as the only channel, a viewport over the whole frame
 * buffer, a pixel-space orthographic projection (the 3D one is saved) and the camera raster's
 * offset as the position matrix. */
void fn_8049043C(void) {
    RwDlRenderMode *mode;
    RwDlRaster *raster;
    GXClearVtxDesc();
    GXSetVtxDesc(9, 1);
    GXSetVtxAttrFmt(0, 9, 1, 4, 0);
    GXSetVtxDesc(0xB, 1);
    GXSetVtxAttrFmt(0, 0xB, 1, 5, 0);
    GXSetNumTevStages(1);
    GXSetNumChans(1);
    GXSetChanCtrl(4, 0, 0, 1, 0, 0, 2);
    GXSetChanCtrl(5, 0, 0, 0, 0, 0, 2);
    if (*lbl_805F1664 != 0) {
        GXSetVtxDesc(0xD, 1);
        GXSetVtxAttrFmt(0, 0xD, 1, 4, 0);
        GXSetTevOp(0, 0);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 0x3C, 0, 0x7D);
        GXSetTevOrder(0, 0, 0, 4);
        fn_80497804();
    } else {
        GXSetNumTexGens(0);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
        GXSetTevOp(0, 4);
    }
    mode = lbl_805F164C;
    if (mode->gx.field_rendering != 0) {
        GXSetViewportJitter(0.0f, 0.0f, mode->gx.fbWidth, mode->gx.xfbHeight, 0.0f, 1.0f, fn_8037F1BC() ^ 1);
    } else {
        GXSetViewport(0.0f, 0.0f, mode->gx.fbWidth, mode->gx.xfbHeight, 0.0f, 1.0f);
    }
    raster = lbl_805F15B4->curCamera->frameBuffer;
    lbl_8056E07C[1] = 2.0f / lbl_805F164C->gx.fbWidth;
    lbl_8056E07C[3] = -2.0f / lbl_805F164C->gx.xfbHeight;
    GXGetProjectionv(lbl_805E3158);
    GXSetProjectionv(lbl_8056E07C);
    lbl_8056E098[0][3] = 0.5f + raster->nOffsetX;
    lbl_8056E098[1][3] = 0.5f + raster->nOffsetY;
    GXLoadPosMtxImm(lbl_8056E098, 0);
    GXSetCurrentMtx(0);
}

/* Puts the camera's viewport, scissor box and 3D projection back after 2D drawing. */
void fn_80490724(void) {
    RwDlRaster *raster = lbl_805F15B4->curCamera->frameBuffer;
    if (raster != raster->parent) {
        if (lbl_805F15F0 == 0) {
            if (lbl_805F164C->gx.field_rendering != 0) {
                GXSetViewportJitter(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height, 0.0f,
                            1.0f, fn_8037F1BC() ^ 1);
            } else {
                GXSetViewport(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height, 0.0f,
                            1.0f);
            }
            GXSetScissor(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height);
        } else {
            if (lbl_805F164C->gx.field_rendering != 0) {
                GXSetViewportJitter(raster->nOffsetX, raster->nOffsetY, raster->width, raster->height, 0.0f,
                            1.0f, fn_8037F1BC() ^ 1);
            } else {
                GXSetViewport(raster->nOffsetX, raster->nOffsetY * 2, raster->width,
                            raster->height * 2, 0.0f, 1.0f);
            }
            if (lbl_805EDF28 != 0) {
                if ((raster->nOffsetY + raster->height) * 2 <= lbl_805F1648 + 2) {
                    GXSetScissor(raster->nOffsetX, raster->nOffsetY * 2, lbl_805F164C->gx.fbWidth,
                                raster->height * 2);
                } else if (raster->nOffsetY * 2 > lbl_805F1648 + 2) {
                    GXSetScissor(0, 0, lbl_805F164C->gx.fbWidth, lbl_805F1648 + 2);
                } else {
                    GXSetScissor(raster->nOffsetX, raster->nOffsetY * 2, lbl_805F164C->gx.fbWidth,
                                lbl_805F1648 + 2);
                }
                GXSetScissorBoxOffset(0, 0);
            } else {
                if (raster->nOffsetY * 2 >= lbl_805F1648 - 2) {
                    GXSetScissor(raster->nOffsetX, raster->nOffsetY * 2, lbl_805F164C->gx.fbWidth,
                                raster->height * 2);
                } else if ((raster->nOffsetY + raster->height) * 2 < lbl_805F1648 + 2) {
                    GXSetScissor(0, lbl_805F1648 - 2, lbl_805F164C->gx.fbWidth, lbl_805F1648 + 2);
                } else {
                    GXSetScissor(raster->nOffsetX, lbl_805F1648 - 2, lbl_805F164C->gx.fbWidth,
                                raster->height * 2);
                }
                GXSetScissorBoxOffset(0, lbl_805F1648 - 2);
            }
        }
    }
    GXSetProjectionv(lbl_805E3158);
}

/* RwIm2DRenderTriangle: three vertices of the array as one GX triangle. */
s32 fn_80490AB8(RwDlIm2DVertex *vertices, s32 numVertices, s32 vert1, s32 vert2, s32 vert3) {
    RwDlIm2DVertex *a = &vertices[vert1];
    RwDlIm2DVertex *b = &vertices[vert2];
    RwDlIm2DVertex *c = &vertices[vert3];
    fn_8049043C();
    GXBegin(0x90, 0, 3);
    if (*lbl_805F1664 != 0) {
        GXPosition3f32(a->x, a->y, a->z);
        GXColor4u8(a->color.red, a->color.green, a->color.blue, a->color.alpha);
        GXTexCoord2f32(a->u, a->v);
        GXPosition3f32(b->x, b->y, b->z);
        GXColor4u8(b->color.red, b->color.green, b->color.blue, b->color.alpha);
        GXTexCoord2f32(b->u, b->v);
        GXPosition3f32(c->x, c->y, c->z);
        GXColor4u8(c->color.red, c->color.green, c->color.blue, c->color.alpha);
        GXTexCoord2f32(c->u, c->v);
    } else {
        GXPosition3f32(a->x, a->y, a->z);
        GXColor4u8(a->color.red, a->color.green, a->color.blue, a->color.alpha);
        GXPosition3f32(b->x, b->y, b->z);
        GXColor4u8(b->color.red, b->color.green, b->color.blue, b->color.alpha);
        GXPosition3f32(c->x, c->y, c->z);
        GXColor4u8(c->color.red, c->color.green, c->color.blue, c->color.alpha);
    }
    fn_80490724();
    return 1;
}

/* RwIm2DRenderLine: two vertices of the array as one GX line. */
s32 fn_80490CBC(RwDlIm2DVertex *vertices, s32 numVertices, s32 vert1, s32 vert2) {
    RwDlIm2DVertex *a = &vertices[vert1];
    RwDlIm2DVertex *b = &vertices[vert2];
    fn_8049043C();
    GXBegin(0xA8, 0, 2);
    if (*lbl_805F1664 != 0) {
        GXPosition3f32(a->x, a->y, a->z);
        GXColor4u8(a->color.red, a->color.green, a->color.blue, a->color.alpha);
        GXTexCoord2f32(a->u, a->v);
        GXPosition3f32(b->x, b->y, b->z);
        GXColor4u8(b->color.red, b->color.green, b->color.blue, b->color.alpha);
        GXTexCoord2f32(b->u, b->v);
    } else {
        GXPosition3f32(a->x, a->y, a->z);
        GXColor4u8(a->color.red, a->color.green, a->color.blue, a->color.alpha);
        GXPosition3f32(b->x, b->y, b->z);
        GXColor4u8(b->color.red, b->color.green, b->color.blue, b->color.alpha);
    }
    fn_80490724();
    return 1;
}
}

/* RenderWare 3.7 GameCube driver: immediate-mode 2D (0x8049043C..0x80491AB3). The device's Im2D
 * entry points (RwIm2DRenderLine, RwIm2DRenderTriangle, RwIm2DRenderPrimitive and
 * RwIm2DRenderIndexedPrimitive, listed in the RwDevice of RwGameCubeDevice.c) and the GX set-up and
 * restore they share. The file owns the GX primitive table, the pixel-space projection and position
 * matrix (.data 0x8056E060), the saved 3D projection (.bss 0x805E3158) and its constants (.sdata2
 * 0x805FB380..0x805FB3A7): no other function addresses them, and the next file's constants start a
 * new pool. Built as C with the driver's flags (-O4,p, no automatic inlining, writable strings).
 *
 * Names are inferred from behaviour; definitions are in retail address order.
 * NonMatching: fn_80490E30 and fn_8049140C are drafts (retail writes every loop twice, once per
 * state of the texture flag). */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/gx/GXPrivate.h>
#include "Game/rw/r7.h"

extern RwDlRenderMode *lbl_805F164C;  /* render mode in use (RwGameCubeDevice.c) */
extern u32 *lbl_805F1664;             /* -> nonzero while Im2D draws with a texture */
extern RwInt32 lbl_805F15F0;          /* nonzero: the frame is drawn in two halves */
extern RwInt32 lbl_805EDF28;          /* nonzero while the top half is drawn */
extern RwInt32 lbl_805F1648;          /* the line where the two halves meet */
extern RwDlGlobals *lbl_805F15B4;     /* RwEngineInstance */

u32 fn_8037F1BC(void); /* VIGetNextField */
void fn_80497804(void);

/* GX primitive of each RwPrimitiveType (none, line list, polyline, triangle list, strip, fan,
 * point list). */
static u32 sIm2DPrimitives[7] = {0, 0xA8, 0xB0, 0x90, 0x98, 0xA0, 0xB8};

/* GXSetProjectionv array of the pixel-space orthographic projection: the x and y scales are set
 * from the frame buffer's size before each use. */
static f32 sIm2DProjection[7] = {1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f};

/* The position matrix: the camera raster's offset is written into its translation. */
static Mtx sIm2DPosMtx = {
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, -1.0f, 0.0f},
};

/* The 3D projection, saved while Im2D draws. */
static f32 sIm2DSavedProjection[7];

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
    sIm2DProjection[1] = 2.0f / lbl_805F164C->gx.fbWidth;
    sIm2DProjection[3] = -2.0f / lbl_805F164C->gx.xfbHeight;
    GXGetProjectionv(sIm2DSavedProjection);
    GXSetProjectionv(sIm2DProjection);
    sIm2DPosMtx[0][3] = 0.5f + raster->nOffsetX;
    sIm2DPosMtx[1][3] = 0.5f + raster->nOffsetY;
    GXLoadPosMtxImm(sIm2DPosMtx, 0);
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
    GXSetProjectionv(sIm2DSavedProjection);
}

/* RwIm2DRenderTriangle: three vertices of the array as one GX triangle. */
RwInt32 fn_80490AB8(RwDlIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2, RwInt32 vert3) {
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
RwInt32 fn_80490CBC(RwDlIm2DVertex *vertices, RwInt32 numVertices, RwInt32 vert1, RwInt32 vert2) {
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

/* One vertex of the Im2D array: three position floats, four colour bytes and - while a texture
 * is set - two texture-coordinate floats, each pushed on its own into the GX FIFO. */
static inline void RwDlIm2DWriteVertex(const u8 *entry) {
    *(volatile f32 *)0xCC008000 = *(const f32 *)(entry + 0x00);
    *(volatile f32 *)0xCC008000 = *(const f32 *)(entry + 0x04);
    *(volatile f32 *)0xCC008000 = *(const f32 *)(entry + 0x08);
    *(volatile u8 *)0xCC008000 = entry[0x0C];
    *(volatile u8 *)0xCC008000 = entry[0x0D];
    *(volatile u8 *)0xCC008000 = entry[0x0E];
    *(volatile u8 *)0xCC008000 = entry[0x0F];
    if (*lbl_805F1664 != 0) {
        *(volatile f32 *)0xCC008000 = *(const f32 *)(entry + 0x10);
        *(volatile f32 *)0xCC008000 = *(const f32 *)(entry + 0x14);
    }
}

/* RwIm2DRenderPrimitive: `count` vertices of the array as one primitive of `type`.
 * Draft: retail holds each loop twice (with and without the texture coordinates). */
RwInt32 fn_80490E30(RwUInt32 type, u8 *data, RwUInt32 count) {
    fn_8049043C();
    GXBegin(sIm2DPrimitives[type], 0, count & 0xFFFF);
    switch (type) {
    case 3: {
        RwUInt32 primitives = count / 3;
        while (primitives-- != 0) {
            RwDlIm2DWriteVertex(data);
            RwDlIm2DWriteVertex(data + 0x18);
            RwDlIm2DWriteVertex(data + 0x30);
            data += 0x48;
        }
        break;
    }
    case 1: {
        /* Retail shifts the count arithmetically (srawi), so the counter is signed. */
        RwInt32 primitives = (RwInt32)count >> 1;
        while (primitives-- != 0) {
            RwDlIm2DWriteVertex(data);
            RwDlIm2DWriteVertex(data + 0x18);
            data += 0x30;
        }
        break;
    }
    case 2:
    case 4:
    case 5: {
        RwUInt32 primitives = count >> 1;
        RwUInt32 odd;
        while (primitives-- != 0) {
            RwDlIm2DWriteVertex(data);
            RwDlIm2DWriteVertex(data + 0x18);
            data += 0x30;
        }
        odd = count & 1;
        while (odd-- != 0) {
            RwDlIm2DWriteVertex(data);
            data += 0x18;
        }
        break;
    }
    default:
        RW_ERROR(0x25);
        break;
    }
    fn_80490724();
    return 1;
}

/* RwIm2DRenderIndexedPrimitive: as fn_80490E30 with the vertices named by index.
 * Draft: as fn_80490E30. */
RwInt32 fn_8049140C(RwUInt32 type, u8 *records, RwInt32 numVertices, u16 *indices, RwUInt32 count) {
    fn_8049043C();
    GXBegin(sIm2DPrimitives[type], 0, count & 0xFFFF);
    switch (type) {
    case 3: {
        RwUInt32 primitives = count / 3;
        while (primitives-- != 0) {
            RwDlIm2DWriteVertex(records + indices[0] * 0x18);
            RwDlIm2DWriteVertex(records + indices[1] * 0x18);
            RwDlIm2DWriteVertex(records + indices[2] * 0x18);
            indices += 3;
        }
        break;
    }
    case 1: {
        RwInt32 primitives = (RwInt32)count >> 1;
        while (primitives-- != 0) {
            RwDlIm2DWriteVertex(records + indices[0] * 0x18);
            RwDlIm2DWriteVertex(records + indices[1] * 0x18);
            indices += 2;
        }
        break;
    }
    case 2:
    case 4:
    case 5: {
        RwUInt32 primitives = count >> 1;
        RwUInt32 odd;
        while (primitives-- != 0) {
            RwDlIm2DWriteVertex(records + indices[0] * 0x18);
            RwDlIm2DWriteVertex(records + indices[1] * 0x18);
            indices += 2;
        }
        odd = count & 1;
        while (odd-- != 0) {
            RwDlIm2DWriteVertex(records + indices[0] * 0x18);
            indices += 1;
        }
        break;
    }
    default:
        RW_ERROR(0x25);
        break;
    }
    fn_80490724();
    return 1;
}

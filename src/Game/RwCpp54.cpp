/* RenderWare 0x8048F5D0..0x80490E2F: the end of the palette quantiser (an octree over the image's
 * colours: fn_8048F5D0 resolves the palette, fn_8048FAB0 maps an image's pixels to it,
 * fn_8048FED8/fn_804903F8 set the tree up and free it) and, from 0x8049043C, the start of the
 * GameCube driver's immediate-mode 2D file (GX set-up and restore, RwIm2DRenderTriangle and
 * RwIm2DRenderLine). Names are inferred from behaviour; definitions are in retail address order.
 *
 * Still first drafts (far from retail): fn_8048F5D0 and fn_8048FAB0. The rest match; the
 * quantiser's recursive node free is inlined into itself eight levels deep because it is
 * declared inline (the driver units are built with -inline noauto). */
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

u32 fn_8048F5D0(u8 *dest, u32 count, u8 *work) {
    fn_8048EB24(*(f32 **)(work + 0x4000), (const u8 *)&lbl_805FB560, lbl_805EDF20);
    memset(dest, 0, count);
    u32 leaves = fn_8048EEFC(*(u32 **)(work + 0x4000), lbl_805EDF20);
    if (leaves <= count) return fn_8048F1E4(*(f32 **)(work + 0x4000), dest, 0, lbl_805EDF20);

    u32 point[4] = {0, 0, 0, 0};
    u32 bounds[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    for (u32 i = 0; i < 4; i++) bounds[i + 4] = 1 << lbl_805EDF20;
    f32 candidates[256];
    for (u32 i = 0; i < count; i++) candidates[i] = lbl_805FB368;
    u32 selected = 0;
    for (u32 i = 1; i <= count; i++) {
        s32 best = -1;
        f32 error = lbl_805FB368;
        for (u32 j = 0; j < i; j++) {
            if (candidates[j] > error) {
                error = candidates[j];
                best = (s32)j;
            }
        }
        if (best < 0) break;
        u32 bestBounds[8];
        if (fn_8048E988(*(f32 **)(work + 0x4000), candidates + best, bounds, bestBounds) != 0) {
            fn_8048E47C(*(f32 **)(work + 0x4000), point, lbl_805EDF20, bounds, candidates + best);
            selected++;
        } else {
            candidates[best] = lbl_805FB368;
        }
    }
    for (u32 i = 0; i < selected; i++) {
        fn_8048E300(*(u32 **)(work + 0x4000), point, lbl_805EDF20, bounds, i);
        fn_8048F1E4(*(f32 **)(work + 0x4000), dest, i, lbl_805EDF20);
    }
    return selected;
}

void fn_8048FAB0(u8 *dest, u32 destStride, u32 kind, u32 pack, u8 *work, u32 *source) {
    u32 srcStride = source[4];
    u8 *pixels = (u8 *)source[5];
    for (u32 row = 0; row < source[2]; row++) {
        u8 *input = pixels;
        u8 *output = dest;
        for (u32 x = 0; x < source[1]; x++) {
            u8 rgba[4];
            if (source[3] == 0x20) {
                rgba[0] = input[0];
                rgba[1] = input[1];
                rgba[2] = input[2];
                rgba[3] = input[3];
                input += 4;
            } else {
                u8 *color = (u8 *)source[6] + *input * 4;
                rgba[0] = color[0];
                rgba[1] = color[1];
                rgba[2] = color[2];
                rgba[3] = color[3];
                input++;
            }
            u32 path = ((u32)rgba[0] << 24) | ((u32)rgba[1] << 16) |
                       ((u32)rgba[2] << 8) | rgba[3];
            u8 index = (u8)fn_8048F4B4(*(u32 **)(work + 0x4000), path, lbl_805EDF20);
            if (kind == 4 && pack == 1) {
                if ((x & 1) == 0) {
                    *output = (*output & 0xF0) | (index & 0xF);
                } else {
                    *output = (*output & 0xF) | (index << 4);
                    output++;
                }
            } else {
                *output++ = index;
            }
        }
        pixels += srcStride;
        dest += destStride;
    }
}

/* Sets the quantiser up: the table that spreads the bits of a colour channel one per nibble
 * (lbl_805E2D58, used to build tree paths), the node free list and an empty root. */
s32 fn_8048FED8(RwDlPalQuant *pq) {
    u32 i;
    u32 bit;
    u32 count;
    u32 spread;
    RwDlPalNode *root;
    count = 1 << lbl_805EDF20;
    for (i = 0; i < count; i++) {
        spread = 0;
        for (bit = 0; bit < lbl_805EDF20; bit++) {
            spread |= (i & (1 << bit)) ? (1 << ((lbl_805EDF20 - 1 - bit) * 4)) : 0;
        }
        lbl_805E2D58[i] = spread;
    }
    pq->nodeFreeList = fn_8047FBD8(sizeof(RwDlPalNode), 0x400, 4, 0x30411);
    pq->root = (RwDlPalNode *)lbl_805F15B4->freeListAlloc(pq->nodeFreeList, 0x30411);
    root = pq->root;
    for (i = 0; i < 16; i++) root->child[i] = NULL;
    return 1;
}

/* Frees a subtree of the colour tree. */
static inline void fn_804900D4(RwDlPalQuant *pq, RwDlPalNode *node, RwInt32 level) {
    if (node != NULL) {
        if (level > 0) {
            RwInt32 i;
            for (i = 0; i < 16; i++) fn_804900D4(pq, node->child[i], level - 1);
        }
        lbl_805F15B4->freeListFree(pq->nodeFreeList, node);
    }
}

/* Frees the colour tree and its free list. */
void fn_804903F8(RwDlPalQuant *pq) {
    fn_804900D4(pq, pq->root, lbl_805EDF20);
    pq->root = NULL;
    fn_8047FCDC(pq->nodeFreeList);
}

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

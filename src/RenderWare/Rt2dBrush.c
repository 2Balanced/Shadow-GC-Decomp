/* RenderWare 2D toolkit (Rt2d): brushes, 0x80470474..0x80470AC4. A brush paints with four
 * corner colours and texture coordinates and an optional texture; brushes come from a free list
 * of 0x78-byte entries that the module's open and close create and destroy. The functions are
 * the toolkit's brush API, kept under their fn_ names: fn_8047054C Rt2dBrushCreate, fn_80470620
 * Rt2dBrushDestroy, fn_80470694 Rt2dBrushSetRGBA, fn_80470998 Rt2dBrushSetUV, fn_80470A4C
 * Rt2dBrushSetTexture; fn_804704D4 and fn_80470474 are the module's open and close (Rt2dOpen
 * and Rt2dClose call them). The game creates brushes, sets their colours and destroys them.
 *
 * Source file: its own .sdata2 pool (0x805FB220..0x805FB230: 0.0f, 0.5f and the unsigned
 * int-to-float double), .sdata (0x805EDDF0..0x805EDDF8: the free list's block size and number of
 * preallocated blocks) and .bss (0x805E18E0..0x805E1908: the free list, padded to 8 bytes). The
 * font file after it starts a new pool at 0x805FB230 that repeats 0.0f. Old unit Game/RwCpp35
 * (middle part).
 *
 * Built as C with CodeWarrior GC 1.3.2, -O4,p, no stmw/lmw (cflags_rw_132). Every function and
 * section matches; the unit is Matching. */
#include <dolphin/types.h>
#include "Game/rw/r4.h"

/* The parameters of the brushes' free list, and its storage. */
static RwInt32 brushFreeListBlockSize = 8;
static RwInt32 brushFreeListPreallocBlocks = 1;
static RwFreeList brushFreeList;

/* Closes the brushes: their free list and the vertex buffer. */
void fn_80470474(void) {
    fn_8047FCDC(lbl_805E1950.brushFreeList);
    lbl_805E1950.brushFreeList = NULL;
    RwFree(lbl_805E1950.brushBuffer);
    lbl_805E1950.brushBuffer = NULL;
}

/* Opens the brushes: a 0x900-byte vertex buffer and a free list of 0x78-byte brushes. */
s32 fn_804704D4(void) {
    lbl_805E1950.brushBuffer = RwMalloc(0x900, 0x40190);
    lbl_805E1950.brushFreeList = fn_8047FC04(sizeof(Rt2dBrush), brushFreeListBlockSize, 16,
                                             brushFreeListPreallocBlocks, &brushFreeList, 0x401A2);
    return 1;
}

Rt2dBrush *fn_80470694(Rt2dBrush *brush, const RwRGBA *c0, const RwRGBA *c1,
                            const RwRGBA *c2, const RwRGBA *c3);
Rt2dBrush *fn_80470998(Rt2dBrush *brush, const RwGameCubeV2d *uv0, const RwGameCubeV2d *uv1,
                            const RwGameCubeV2d *uv2, const RwGameCubeV2d *uv3);
Rt2dBrush *fn_80470A4C(Rt2dBrush *brush, RwTexture *texture);

/* Creates a brush: white, untextured, with every texture coordinate zero. */
Rt2dBrush *fn_8047054C(void) {
    Rt2dBrush *brush = (Rt2dBrush *)RwFreeListAlloc(lbl_805E1950.brushFreeList, 0x301A2);

    if (brush != NULL) {
        RwGameCubeV2d uv;
        RwRGBA color;

        memset(brush, 0, sizeof(Rt2dBrush));
        color.red = 0xFF;
        color.green = 0xFF;
        color.blue = 0xFF;
        color.alpha = 0xFF;
        fn_80470694(brush, &color, &color, &color, &color);
        uv.v[0] = 0.0f;
        uv.v[1] = 0.0f;
        fn_80470998(brush, &uv, &uv, &uv, &uv);
        fn_80470A4C(brush, NULL);
        brush->halfWidth = 0.5f;
        brush->refCount = 1;
        return brush;
    }
    return NULL;
}

/* Releases a brush: the last reference returns it, and its texture, to the free list. */
s32 fn_80470620(Rt2dBrush *brush) {
    if (--brush->refCount <= 0) {
        if (brush->texture != NULL) {
            fn_8048CD8C(brush->texture);
        }
        RwFreeListFree(lbl_805E1950.brushFreeList, brush);
        return 1;
    }
    return 0;
}

/* Sets a brush's corner colours. */
Rt2dBrush *fn_80470694(Rt2dBrush *brush, const RwRGBA *c0, const RwRGBA *c1,
                            const RwRGBA *c2, const RwRGBA *c3) {
    brush->v[0].color.red = c3->red;
    brush->v[0].color.green = c3->green;
    brush->v[0].color.blue = c3->blue;
    brush->v[0].color.alpha = c3->alpha;
    brush->v[1].color.red = c2->red;
    brush->v[1].color.green = c2->green;
    brush->v[1].color.blue = c2->blue;
    brush->v[1].color.alpha = c2->alpha;
    brush->v[1].color.red = brush->v[1].color.red - brush->v[0].color.red;
    brush->v[1].color.green = brush->v[1].color.green - brush->v[0].color.green;
    brush->v[1].color.blue = brush->v[1].color.blue - brush->v[0].color.blue;
    brush->v[1].color.alpha = brush->v[1].color.alpha - brush->v[0].color.alpha;
    brush->v[2].color.red = c0->red;
    brush->v[2].color.green = c0->green;
    brush->v[2].color.blue = c0->blue;
    brush->v[2].color.alpha = c0->alpha;
    brush->v[3].color.red = c1->red;
    brush->v[3].color.green = c1->green;
    brush->v[3].color.blue = c1->blue;
    brush->v[3].color.alpha = c1->alpha;
    brush->v[3].color.red = brush->v[3].color.red - brush->v[2].color.red;
    brush->v[3].color.green = brush->v[3].color.green - brush->v[2].color.green;
    brush->v[3].color.blue = brush->v[3].color.blue - brush->v[2].color.blue;
    brush->v[3].color.alpha = brush->v[3].color.alpha - brush->v[2].color.alpha;
    memcpy(&brush->color, c3, sizeof(RwRGBA));
    if (brush->v[1].color.red == 0.0f && brush->v[3].color.red == 0.0f &&
        brush->v[0].color.red == brush->v[2].color.red && brush->v[1].color.green == 0.0f &&
        brush->v[3].color.green == 0.0f && brush->v[0].color.green == brush->v[2].color.green &&
        brush->v[1].color.blue == 0.0f && brush->v[3].color.blue == 0.0f &&
        brush->v[0].color.blue == brush->v[2].color.blue && brush->v[1].color.alpha == 0.0f &&
        brush->v[3].color.alpha == 0.0f && brush->v[0].color.alpha == brush->v[2].color.alpha) {
        brush->flags &= ~1;
    } else {
        brush->flags |= 1;
    }
    return brush;
}


/* Sets a brush's corner texture coordinates. */
Rt2dBrush *fn_80470998(Rt2dBrush *brush, const RwGameCubeV2d *uv0, const RwGameCubeV2d *uv1,
                            const RwGameCubeV2d *uv2, const RwGameCubeV2d *uv3) {
    brush->v[0].uv = *uv3;
    brush->v[1].uv.v[0] = uv2->v[0] - uv3->v[0];
    brush->v[1].uv.v[1] = uv2->v[1] - uv3->v[1];
    brush->v[2].uv = *uv0;
    brush->v[3].uv.v[0] = uv1->v[0] - uv0->v[0];
    brush->v[3].uv.v[1] = uv1->v[1] - uv0->v[1];
    if (brush->v[1].uv.v[0] == 0.0f && brush->v[1].uv.v[1] == 0.0f && brush->v[3].uv.v[0] == 0.0f &&
        brush->v[3].uv.v[1] == 0.0f) {
        brush->flags &= ~2;
    } else {
        brush->flags |= 2;
    }
    return brush;
}

/* Sets a brush's texture, taking a reference to it. */
Rt2dBrush *fn_80470A4C(Rt2dBrush *brush, RwTexture *texture) {
    if (texture != NULL) {
        brush->flags |= 2;
        texture->refCount++;
    } else {
        brush->flags &= ~2;
    }
    if (brush->texture != NULL) {
        fn_8048CD8C(brush->texture);
    }
    brush->texture = texture;
    return brush;
}

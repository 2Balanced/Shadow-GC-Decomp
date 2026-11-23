/* RenderWare GameCube driver, 0x80498E08..0x8049A95B: raster clearing, native texture sizes,
 * the per-raster GX texture state, the draw-sync token test, and the start of the immediate-mode
 * 3D module (RwIm3DTransform, RwIm3DEnd). The unit is cut by size; see the notes at the functions
 * for where the source files change. Names are inferred from behaviour; definitions are in retail
 * address order.
 *
 * Not written yet: fn_80498E08, fn_804990D4 (clears a rectangle of the current raster),
 * fn_804995B4, fn_8049988C, fn_80499C38 and fn_8049A518. */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <string.h>
#include "Game/rw/r7.h"

extern "C" {
extern RwDlRaster *lbl_805F1668; /* the raster the clear functions work on */
extern s32 lbl_805F1670;         /* offset of RwDlTexExt in a raster */
extern void *lbl_8056EC1C[8];    /* rasters bound to the eight GX texture maps */
extern volatile u16 lbl_805F1678; /* last draw-sync token read back */
extern volatile u16 lbl_805EDF40; /* last draw-sync token sent */
extern u8 *lbl_805F15B4;         /* RwEngineInstance */
extern s32 lbl_805F1684;         /* offset of RwDlIm3DGlobals in the engine instance */
extern f32 lbl_805FB420;

s32 fn_804990D4(void *, void *, s32);
s32 fn_80495558(RwDlRaster *);
s32 fn_8048D280(s32, u32, void *, void *, void *); /* RwRasterRegisterPlugin */
void *fn_8049B674(void *, void *, s32);            /* _rxPipelineExecute */
f32 GXGetTexObjLODBias(void *);
u8 GXGetTexObjBiasClamp(void *);
u8 GXGetTexObjEdgeLOD(void *);
s32 GXGetTexObjMaxAniso(void *);

#define RW_IM3D_GLOBAL(member) (((RwDlIm3DGlobals *)(lbl_805F15B4 + lbl_805F1684))->member)

/* Driver standard function: clears a rectangle of the current raster. */
s32 fn_804994B8(void *out, void *inOut, s32 num) {
    return fn_804990D4(lbl_805F1668, inOut, num);
}

/* Driver standard function: clears the whole current raster. */
s32 fn_804994DC(void *out, void *inOut, s32 num) {
    RwDlRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = lbl_805F1668->width;
    rect.h = lbl_805F1668->height;
    return fn_804990D4(lbl_805F1668, &rect, num);
}

/* Driver standard function: sets the current raster. */
s32 fn_80499520(void *out, void *inOut, s32 num) {
    lbl_805F1668 = (RwDlRaster *)inOut;
    return 1;
}

/* Driver standard function: the size of a texture's native chunk (0x74 bytes of header, the
 * palette of a palettised raster, a length word and the pixels). */
s32 fn_8049952C(void *out, void *inOut, s32 num) {
    s32 *size = (s32 *)out;
    RwDlRaster *raster = *(RwDlRaster **)inOut;
    s32 total;
    if (raster == NULL) {
        *size = 100;
        return 1;
    }
    total = 0x74;
    if ((RW_RASTER_FORMAT(raster) & 0x6000) != 0) {
        total = (1 << raster->depth) * 2 + 0x74;
    }
    total += 4;
    total += fn_80495558(raster);
    *size = total;
    return 1;
}

/* Raster plugin constructor: no GX texture object yet. */
void *fn_80499B24(void *object) {
    ((RwDlTexExt *)((u8 *)object + lbl_805F1670))->flags = 0x1000000;
    return object;
}

/* Raster plugin destructor: forgets the raster wherever it is bound. */
void *fn_80499B38(void *object) {
    s32 i;
    for (i = 0; i < 8; i++) {
        if (object == lbl_8056EC1C[i]) {
            lbl_8056EC1C[i] = NULL;
        }
    }
    return object;
}

/* Unbinds the eight texture maps. */
void fn_80499BC8(void) {
    u32 i = 8;
    while (i-- != 0) {
        lbl_8056EC1C[i] = NULL;
    }
}

/* Registers the raster plugin above. */
void fn_80499BF8(void) {
    lbl_805F1670 = fn_8048D280(0x24, 0x40C, fn_80499B24, fn_80499B38, NULL);
}

/* Reads a raster's texture LOD settings back, or the defaults when it has no texture object. */
void fn_8049A6E4(RwDlRaster *raster, f32 *lodBias, s32 *biasClamp, s32 *edgeLOD, s32 *maxAniso) {
    RwDlTexExt *ext = (RwDlTexExt *)((u8 *)raster + lbl_805F1670);
    if ((ext->flags & 0x1000000) != 0) {
        *lodBias = lbl_805FB420;
        *biasClamp = 1;
        *edgeLOD = 1;
        *maxAniso = 0;
    } else {
        *lodBias = GXGetTexObjLODBias(ext);
        *biasClamp = GXGetTexObjBiasClamp(ext);
        *edgeLOD = GXGetTexObjEdgeLOD(ext);
        *maxAniso = GXGetTexObjMaxAniso(ext);
    }
}

/* Has the graphics processor passed the draw-sync token? Tokens count up to 0xE000 and wrap, so
 * the test is whether the token lies outside the range still to be drawn. */
s32 fn_8049A78C(u16 token) {
    lbl_805F1678 = GXReadDrawSync();
    if (lbl_805F1678 >= 0xE000) {
        return 0;
    }
    if (lbl_805EDF40 >= lbl_805F1678) {
        s32 passed = 0;
        if (token <= lbl_805F1678 || token > lbl_805EDF40) {
            passed = 1;
        }
        return passed;
    } else {
        s32 passed = 0;
        if (token > lbl_805EDF40 && token <= lbl_805F1678) {
            passed = 1;
        }
        return passed;
    }
}

/* RwIm3DTransform(vertices, numVerts, ltm, flags). The source file changes here: from this
 * function on the code is the generic immediate-mode module (globals at lbl_805F1684). */
void *fn_8049A82C(void *verts, u32 numVerts, void *ltm, u32 flags) {
    if (numVerts > 0x10000) {
        RW_ERROR(0x32);
    } else {
        RW_IM3D_GLOBAL(stash.numVerts) = numVerts;
        RW_IM3D_GLOBAL(stash.verts) = verts;
        RW_IM3D_GLOBAL(stash.stride) = 0x24;
        RW_IM3D_GLOBAL(stash.render.ltm) = ltm;
        RW_IM3D_GLOBAL(stash.render.flags) = flags | 0x18;
        if (fn_8049B674(RW_IM3D_GLOBAL(transformPipeline), &RW_IM3D_GLOBAL(stash), 1) != NULL) {
            return verts;
        }
    }
    return NULL;
}

/* RwIm3DEnd. */
s32 fn_8049A90C(void) {
    if (RW_IM3D_GLOBAL(stash.verts) == NULL) {
        return 0;
    }
    memset(&RW_IM3D_GLOBAL(stash), 0, sizeof(RwDlIm3DStash));
    return 1;
}
}

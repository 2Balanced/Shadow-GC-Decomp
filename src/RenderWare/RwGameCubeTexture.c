/* RenderWare 3.7 GameCube driver: GX texture objects (0x80499B24..0x8049A78B). The raster plugin
 * (ID 0x40C, 0x24 bytes at lbl_805F1670) that keeps a GX texture object in every raster, the
 * rasters bound to the eight GX texture maps, and the loading of a texture into a map with its
 * filter, addressing and LOD state. The file owns the filter and address mode tables and the bound
 * rasters (.data 0x8056EBD0..0x8056EC3F), the plugin offset (.sbss 0x805F1670) and its constants
 * (.sdata2 0x805FB420..0x805FB42F). Built as C with the driver's flags (-O4,p, no automatic
 * inlining, writable strings).
 *
 * Names are inferred from behaviour; definitions are in retail address order.
 * Not written yet: fn_80499C38 (loads a texture into a map).
 * NonMatching: fn_8049A518 (13 rows: the scheduling of GXInitTexObj's arguments in the
 * unpalettised branch; locals, getters, casts and groupings of the wrap modes do not move it). */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include "Game/rw/r7.h"

RwInt32 fn_8048D280(RwInt32 size, RwUInt32 id, void *ctor, void *dtor, void *copy); /* RwRasterRegisterPlugin */
f32 GXGetTexObjLODBias(const GXTexObj *obj);
GXBool GXGetTexObjBiasClamp(const GXTexObj *obj);
GXBool GXGetTexObjEdgeLOD(const GXTexObj *obj);
RwInt32 GXGetTexObjMaxAniso(const GXTexObj *obj);
u32 GXGetTexObjTlut(const GXTexObj *obj);
void GXInitTexObjCI(GXTexObj *obj, void *image_ptr, u16 width, u16 height, RwInt32 format, GXTexWrapMode wrap_s,
                    GXTexWrapMode wrap_t, GXBool mipmap, u32 tlut_name);
void GXInitTexObjLOD(GXTexObj *obj, GXTexFilter min_filt, GXTexFilter mag_filt, f32 min_lod, f32 max_lod,
                     f32 lod_bias, GXBool bias_clamp, GXBool do_edge_lod, GXAnisotropy max_aniso);

extern RwInt32 lbl_805F1658; /* offset of the driver's data in a raster (RwGameCubeRaster.c) */

/* .sbss: the offset of the GX texture object in a raster. */
RwInt32 lbl_805F1670;

/* GX min and mag filters of each RwTextureFilterMode. */
static RwInt32 sDlFilterModes[7][2] = {{0, 0}, {0, 0}, {1, 1}, {2, 0}, {3, 1}, {4, 0}, {5, 1}};

/* GX wrap mode of each RwTextureAddressMode. */
static RwInt32 sDlAddressModes[5] = {0, 1, 2, 0, 0};

/* The rasters bound to the eight GX texture maps. */
static void *sDlBoundRasters[8] = {0};

/* Raster plugin constructor: no GX texture object yet. */
void *fn_80499B24(void *object) {
    ((RwDlTexExt *)((u8 *)object + lbl_805F1670))->flags = 0x1000000;
    return object;
}

/* Raster plugin destructor: forgets the raster wherever it is bound. */
void *fn_80499B38(void *object) {
    RwInt32 i;
    for (i = 0; i < 8; i++) {
        if (object == sDlBoundRasters[i]) {
            sDlBoundRasters[i] = NULL;
        }
    }
    return object;
}

/* Unbinds the eight texture maps. */
void fn_80499BC8(void) {
    RwUInt32 i = 8;
    while (i-- != 0) {
        sDlBoundRasters[i] = NULL;
    }
}

/* Registers the raster plugin above. */
void fn_80499BF8(void) {
    lbl_805F1670 = fn_8048D280(0x24, 0x40C, fn_80499B24, fn_80499B38, NULL);
}

/* Sets a texture's GX texture object up from its raster (image, size, format, palette) and its
 * filter and addressing modes, with the given LOD settings. */
void fn_8049A518(RwDlTexture *texture, f32 lodBias, RwBool biasClamp, RwBool edgeLOD, RwInt32 maxAniso) {
    RwDlRasterExt *ext;
    GXTexObj *texObj;
    RwDlRaster *raster;
    RwInt32 format;
    RwInt32 minFilter;
    RwInt32 magFilter;
    raster = texture->raster;
    texObj = (GXTexObj *)((u8 *)texture + lbl_805F1670);
    ext = (RwDlRasterExt *)((u8 *)raster->parent + lbl_805F1658);
    format = RW_RASTER_FORMAT(raster);
    if (format & 0x6000) {
        u32 tlut;
        RwInt32 filter;
        if (((RwDlTexExt *)texObj)->flags & 0x2000000) {
            tlut = GXGetTexObjTlut(texObj);
        } else {
            tlut = 0;
        }
        GXInitTexObjCI(texObj, ext->pixels, raster->width, raster->height, ext->texFormat,
                       sDlAddressModes[(texture->filterAddressing >> 8) & 0xF],
                       sDlAddressModes[(texture->filterAddressing >> 12) & 0xF], (format >> 15) & 1,
                       tlut);
        filter = texture->filterAddressing & 0xFF;
        if (filter == 6 || filter == 5) {
            minFilter = sDlFilterModes[4][0];
            magFilter = sDlFilterModes[4][1];
        } else {
            minFilter = sDlFilterModes[filter][0];
            magFilter = sDlFilterModes[filter][1];
        }
    } else {
        GXInitTexObj(texObj, ext->pixels, raster->width, raster->height, ext->texFormat,
                     sDlAddressModes[(texture->filterAddressing >> 8) & 0xF],
                     sDlAddressModes[(texture->filterAddressing >> 12) & 0xF], (format >> 15) & 1);
        minFilter = sDlFilterModes[texture->filterAddressing & 0xFF][0];
        magFilter = sDlFilterModes[texture->filterAddressing & 0xFF][1];
    }
    GXInitTexObjLOD(texObj, minFilter, magFilter, 0.0f, ext->mipLevels, lodBias, biasClamp, edgeLOD, maxAniso);
    ((RwDlTexExt *)texObj)->flags =
        ((((RwDlTexExt *)texObj)->flags & 0xFFFF0000) | (texture->filterAddressing & 0xFFFF)) & ~0x1000000;
}

/* Reads a raster's texture LOD settings back, or the defaults when it has no texture object. */
void fn_8049A6E4(RwDlRaster *raster, f32 *lodBias, RwInt32 *biasClamp, RwInt32 *edgeLOD, RwInt32 *maxAniso) {
    RwDlTexExt *ext = (RwDlTexExt *)((u8 *)raster + lbl_805F1670);
    if ((ext->flags & 0x1000000) != 0) {
        *lodBias = 0.0f;
        *biasClamp = 1;
        *edgeLOD = 1;
        *maxAniso = 0;
    } else {
        *lodBias = GXGetTexObjLODBias((GXTexObj *)ext);
        *biasClamp = GXGetTexObjBiasClamp((GXTexObj *)ext);
        *edgeLOD = GXGetTexObjEdgeLOD((GXTexObj *)ext);
        *maxAniso = GXGetTexObjMaxAniso((GXTexObj *)ext);
    }
}

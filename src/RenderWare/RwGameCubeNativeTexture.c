/* RenderWare 3.7 GameCube driver: native textures in streams (0x8049952C..0x80499B23). The driver's
 * NativeTextureGetSize, NativeTextureWrite and NativeTextureRead standard functions: a struct
 * chunk with the texture's header (platform 6, filtering and addressing, the GX LOD settings and
 * the names), the raster's description, its palette and its image in GX's tiled layout. The file
 * owns one constant (.sdata2 0x805FB418, a 0.0 that the texture file's pool repeats: a pool of its
 * own). Built as C with the driver's flags (-O4,p, no automatic inlining, writable strings); retail
 * inlines NativeTextureGetSize into NativeTextureWrite.
 *
 * Names are inferred from behaviour; definitions are in retail address order.
 * NonMatching: fn_8049988C (5 rows: where the scheduler puts the address of the texture's name
 * for the call after the three filter and addressing stores). */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include "Game/rw/r7.h"

void *memcpy(void *dst, const void *src, unsigned long n);

extern RwInt32 lbl_805F1658; /* offset of the driver's data in a raster (RwGameCubeRaster.c) */
extern RwInt32 lbl_805F1670; /* offset of a texture's GX texture object (RwGameCubeTexture.c) */

RwInt32 fn_80495558(RwDlRaster *raster); /* bytes of all mip levels (RwGameCubeRaster.c) */
RwBool fn_80496A20(RwDlRaster *raster, RwUInt8 levels); /* allocates a texture raster's image */
void fn_8049A518(RwDlTexture *texture, f32 lodBias, RwBool biasClamp, RwBool edgeLOD, RwInt32 maxAniso);
void *fn_8047CEF0(void *stream, RwInt32 type, RwInt32 size, RwUInt32 version, RwUInt32 build); /* _rwStreamWriteVersionedChunkHeader */
RwBool fn_8047CF50(void *stream, RwInt32 type, RwUInt32 *length, RwUInt32 *version); /* RwStreamFindChunk */
void *fn_80480ED0(void *stream, const void *buffer, RwUInt32 length); /* RwStreamWrite */
RwUInt32 fn_80480D4C(void *stream, void *buffer, RwUInt32 length);    /* RwStreamRead */
RwInt32 fn_80489CE4(RwDlRaster *raster);                              /* RwRasterGetNumLevels */
RwDlRaster *fn_80489E74(RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 flags); /* RwRasterCreate */
RwBool fn_80489BDC(RwDlRaster *raster);                               /* RwRasterDestroy */
RwDlTexture *fn_8048CCDC(RwDlRaster *raster);                         /* RwTextureCreate */
RwDlTexture *fn_8048CE54(RwDlTexture *texture, const char *name);     /* RwTextureSetName */
RwDlTexture *fn_8048CF00(RwDlTexture *texture, const char *mask);     /* RwTextureSetMaskName */
RwInt32 GXGetTexObjMaxAniso(const GXTexObj *obj);
GXBool GXGetTexObjBiasClamp(const GXTexObj *obj);
GXBool GXGetTexObjEdgeLOD(const GXTexObj *obj);
f32 GXGetTexObjLODBias(const GXTexObj *obj);

/* The texture part of a native texture chunk (0x58 bytes). */
typedef struct RwDlNativeTexture {
    RwInt32 platform;          /* 0x00: 6, the GameCube */
    RwUInt32 filterAddressing; /* 0x04 */
    RwInt32 maxAniso;          /* 0x08 */
    RwBool biasClamp;          /* 0x0C */
    RwBool edgeLOD;            /* 0x10 */
    f32 lodBias;               /* 0x14 */
    char name[32];             /* 0x18 */
    char mask[32];             /* 0x38 */
} RwDlNativeTexture;

/* The raster part (0x10 bytes). */
typedef struct RwDlNativeRaster {
    RwInt32 format;   /* 0x00: type and raster format */
    u16 width;        /* 0x04 */
    u16 height;       /* 0x06 */
    u8 depth;         /* 0x08 */
    u8 numLevels;     /* 0x09 */
    u8 texFormat;     /* 0x0A */
    u8 tlutFormat;    /* 0x0B */
    RwUInt32 alpha;   /* 0x0C */
} RwDlNativeRaster;

#pragma auto_inline on
/* Driver standard function NativeTextureGetSize: the size of a texture's native chunk (0x74 bytes
 * of header, the palette of a palettised raster, a length word and the pixels). */
RwBool fn_8049952C(void *out, void *inOut, RwInt32 num) {
    RwInt32 *size = (RwInt32 *)out;
    RwDlRaster *raster = *(RwDlRaster **)inOut;
    RwInt32 total;
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
#pragma auto_inline reset

/* Driver standard function NativeTextureWrite. */
RwBool fn_804995B4(void *streamIn, void *textureIn, RwInt32 num) {
    RwDlNativeTexture header;
    RwDlNativeRaster info;
    RwUInt32 imageSize;
    RwInt32 bytesLeft;
    void *stream = streamIn;
    RwDlTexture *texture = (RwDlTexture *)textureIn;
    u8 *ext;
    RwDlRaster *raster;
    RwDlRasterExt *rasterExt;
    fn_8049952C(&bytesLeft, texture, 0);
    bytesLeft -= 12;
    if (!fn_8047CEF0(stream, 1, bytesLeft, 0x37002, 0x37)) return 0;
    header.platform = 6;
    ext = (u8 *)texture + lbl_805F1670;
    header.filterAddressing = (((texture->filterAddressing >> 8) & 0xF) << 8) | (texture->filterAddressing & 0xFF) | (((texture->filterAddressing >> 12) & 0xF) << 12);
    if (((RwDlTexExt *)ext)->flags & 0x1000000) {
        header.maxAniso = 0;
        header.biasClamp = 1;
        header.edgeLOD = 1;
        header.lodBias = 0.0f;
    } else {
        header.maxAniso = GXGetTexObjMaxAniso((GXTexObj *)ext);
        header.biasClamp = GXGetTexObjBiasClamp((GXTexObj *)ext);
        header.edgeLOD = GXGetTexObjEdgeLOD((GXTexObj *)ext);
        header.lodBias = GXGetTexObjLODBias((GXTexObj *)ext);
    }
    memcpy(header.name, texture->name, 32);
    memcpy(header.mask, texture->mask, 32);
    if (!fn_80480ED0(stream, &header, sizeof(header))) return 0;
    raster = texture->raster;
    bytesLeft -= sizeof(header);
    rasterExt = (RwDlRasterExt *)((u8 *)raster->parent + lbl_805F1658);
    info.format = RW_RASTER_FORMAT(raster) | raster->cType;
    info.width = raster->width;
    info.height = raster->height;
    info.depth = raster->depth;
    info.numLevels = fn_80489CE4(raster);
    info.texFormat = rasterExt->texFormat;
    info.tlutFormat = rasterExt->tlutFormat;
    info.alpha = rasterExt->flags & 1;
    if (!fn_80480ED0(stream, &info, sizeof(info))) return 0;
    bytesLeft -= sizeof(info);
    if (RW_RASTER_FORMAT(raster) & 0x6000) {
        RwUInt32 paletteSize = (1 << raster->depth) * 2;
        if (!fn_80480ED0(stream, rasterExt->palette, paletteSize)) return 0;
        bytesLeft -= paletteSize;
    }
    imageSize = fn_80495558(raster);
    if (!fn_80480ED0(stream, &imageSize, sizeof(imageSize))) return 0;
    bytesLeft -= sizeof(imageSize);
    if (!fn_80480ED0(stream, rasterExt->pixels, imageSize)) return 0;
    bytesLeft -= imageSize;
    return 1;
}

/* Driver standard function NativeTextureRead. */
RwBool fn_8049988C(void *streamIn, void *textureOut, RwInt32 num) {
    RwDlNativeTexture header;
    RwDlNativeRaster info;
    RwUInt32 length;
    RwUInt32 version;
    RwUInt32 imageSize;
    void *stream = streamIn;
    RwDlRaster *raster;
    RwDlRasterExt *ext;
    if (!fn_8047CF50(stream, 1, &length, &version)) return 0;
    if (version < 0x35000 || version > 0x37002) return 0;
    if (fn_80480D4C(stream, &header, sizeof(header)) != sizeof(header)) return 0;
    if (header.platform != 6) return 0;
    if (fn_80480D4C(stream, &info, sizeof(info)) != sizeof(info)) return 0;
    raster = fn_80489E74(info.width, info.height, info.depth, info.format | 0x80);
    if (raster == NULL) return 0;
    ext = (RwDlRasterExt *)((u8 *)raster + lbl_805F1658);
    ext->texFormat = info.texFormat;
    ext->tlutFormat = info.tlutFormat;
    ext->flags = info.alpha != 0;
    if (!fn_80496A20(raster, info.numLevels)) {
        fn_80489BDC(raster);
        return 0;
    }
    raster->cFlags &= 0x7F;
    if (RW_RASTER_FORMAT(raster) & 0x6000) {
        RwUInt32 paletteSize = (1 << raster->depth) * 2;
        if (fn_80480D4C(stream, ext->palette, paletteSize) != paletteSize) return 0;
        DCFlushRange(ext->palette, (1 << raster->depth) * 2);
    }
    {
    RwUInt32 autoMipmap;
    RwDlTexture *texture;
    autoMipmap = raster->cFormat & 0x10;
    raster->cFormat &= ~autoMipmap;
    if (fn_80480D4C(stream, &imageSize, sizeof(imageSize)) != sizeof(imageSize)) return 0;
    if (fn_80480D4C(stream, ext->pixels, imageSize) != imageSize) return 0;
    DCFlushRange(ext->pixels, imageSize);
    GXInvalidateTexAll();
    raster->cFormat |= autoMipmap;
    texture = fn_8048CCDC(raster);
    if (texture == NULL) {
        fn_80489BDC(raster);
        return 0;
    }
    texture->filterAddressing = (texture->filterAddressing & ~0xFF) | (header.filterAddressing & 0xFF);
    texture->filterAddressing = (texture->filterAddressing & ~0xF00) | (header.filterAddressing & 0xF00);
    texture->filterAddressing = (texture->filterAddressing & ~0xF000) | (header.filterAddressing & 0xF000);
    fn_8048CE54(texture, header.name);
    fn_8048CF00(texture, header.mask);
    fn_8049A518(texture, header.lodBias, header.biasClamp, header.edgeLOD, header.maxAniso);
    *(RwDlTexture **)textureOut = texture;
    return 1;
    }
}

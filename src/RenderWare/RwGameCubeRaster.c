/* RenderWare 3.7 GameCube driver: rasters (0x804953F0..0x80497197). The raster plugin that holds
 * the driver's data in every raster (its offset is lbl_805F1658), texture sizes in GX's tiled
 * layout, mip levels, the tiling copies between linear images and GX textures, and the driver's
 * raster standard functions (lock and unlock, palette lock and unlock, create and destroy, texture
 * set raster, sub-raster). The file owns its jump tables and messages (.data 0x8056E4F8..
 * 0x8056E957: fn_80496408 reaches its messages from the jump table at the start of the file's data,
 * one pool base) and the plugin offset (.sbss 0x805F1658). Built as C with the driver's flags
 * (-O4,p, no automatic inlining, writable strings).
 *
 * Names are inferred from behaviour; definitions are in retail address order.
 * NonMatching: fn_8049572C and fn_804959C0 (saved registers of the four tiling loops),
 * fn_80495C54 (1 row: retail adds the image base as pixels + offset; written that way the offset
 * takes another register), fn_80496408 (code identical: our object reaches the .data pool through
 * the section symbol where retail's split names the first object, @32). fn_80495688,
 * fn_804955CC and fn_80496A20 are inlined into their later callers (#pragma auto_inline on). */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include "Game/rw/r7.h"

void *memcpy(void *dst, const void *src, unsigned long n);
void *memset(void *dst, int c, unsigned long n);

extern RwDlGlobals *lbl_805F15B4;      /* RwEngineInstance */
extern RwDlRenderMode *lbl_805F164C;   /* render mode in use (RwGameCubeDevice.c) */
extern RwInt32 lbl_805F1670;           /* offset of the texture's GX state (RwGameCubeTexture.c) */
extern RwDlTexture *lbl_805F1664;      /* the texture set for drawing (RwGameCubeRenderState.c) */
extern volatile u16 lbl_805EDF40;               /* the next draw-sync token (RwGameCubeDrawSync.c) */
extern u16 lbl_805F1678;               /* the last draw-sync token passed */

RwInt32 fn_80491AB4(RwInt32 value);    /* log2 (RwGameCubeRasterConvert.c) */
RwBool fn_8049A78C(u16 token);         /* the draw-sync token has been passed */
RwInt32 fn_80489C54(RwInt32 size, RwInt32 id, void *ctor, void *dtor, void *copy); /* RwRasterRegisterPlugin */
void fn_8039744C(void *tlut, void *lut, RwInt32 fmt, RwUInt32 entries); /* GXInitTlutObj */
void fn_8048D2C0(void *raster, RwInt32 flags);

#define RW_ERROR_SIZED(value, size)                                                                \
    do {                                                                                           \
        RwDlError error;                                                                           \
        error.pluginID = 1;                                                                        \
        error.errorCode = fn_8047DC28((value), (size));                                            \
        fn_8047DB84(&error);                                                                       \
    } while (0)

/* .sbss: the offset of the driver's data in a raster. */
RwInt32 lbl_805F1658;

/* Attaches the raster plugin (0x34 bytes, ID 0x40C). */
void fn_804953F0(void) {
    lbl_805F1658 = fn_80489C54(0x34, 0x40C, 0, 0, 0);
}

/* Bytes of one mip level of a texture raster in GX's tiled layout (tiles of 8x8 texels at 4 bits,
 * 8x4 at 8 bits, 4x4 at 16 and 32 bits), rounded up to 32 bytes. A locked raster is measured
 * at its full size. */
RwInt32 fn_80495428(RwDlRaster *raster, RwUInt8 level) {
    RwDlRaster *parent = raster->parent;
    RwUInt32 width;
    RwUInt32 height;
    RwUInt32 size;
    if (raster->privateFlags & 6) {
        width = parent->width;
        height = parent->height;
    } else {
        width = parent->width >> level;
        height = parent->height >> level;
        width = width ? width : 1;
        height = height ? height : 1;
    }
    switch (raster->depth) {
    case 4:
        size = (((width + 7) & ~7) * ((height + 7) & ~7)) >> 1;
        break;
    case 8:
        size = ((width + 7) & ~7) * ((height + 3) & ~3);
        break;
    case 16:
        size = (((width + 3) & ~3) * ((height + 3) & ~3)) << 1;
        break;
    case 32:
        size = (((width + 3) & ~3) * ((height + 3) & ~3)) << 2;
        break;
    case 24:
    default:
        RW_ERROR(0x8000000C);
        return 0;
    }
    return (size + 31) & ~31;
}

RwBool fn_80495688(void *out, void *inOut, RwInt32 num);

/* Bytes of all the mip levels of a texture raster. */
RwInt32 fn_80495558(RwDlRaster *raster) {
    RwInt32 numLevels;
    RwInt32 size = 0;
    fn_80495688(&numLevels, raster, 0);
    while (numLevels--) {
        size += fn_80495428(raster, numLevels);
    }
    return size;
}

#pragma auto_inline on
/* Bytes of one row of tiles of a mip level of a texture raster. */
RwInt32 fn_804955CC(RwDlRaster *raster, RwUInt8 level) {
    RwInt32 w = raster->parent->width >> level;
    RwInt32 width = w ? w : 1;
    switch (raster->depth) {
    case 4:
        return ((width + 7) & ~7) >> 1;
    case 8:
        return (width + 7) & ~7;
    case 16:
        return ((width + 3) & ~3) << 1;
    case 32:
        return ((width + 3) & ~3) << 2;
    case 24:
    default:
        RW_ERROR(0x8000000C);
        return 0;
    }
}
#pragma auto_inline reset

#pragma auto_inline on
/* Driver standard function RasterGetMipLevels: the number of mip levels of a raster: the count
 * made for it, else down to 1x1 for a mip-mapped format, else 1. */
RwBool fn_80495688(void *out, void *inOut, RwInt32 num) {
    RwInt32 *numLevels = (RwInt32 *)out;
    RwDlRaster *raster = (RwDlRaster *)inOut;
    RwDlRasterExt *ext = (RwDlRasterExt *)((u8 *)raster->parent + lbl_805F1658);
    if (ext->mipLevels != 0xFF) {
        *numLevels = ext->mipLevels + 1;
    } else {
        RwUInt8 levels;
        if (RW_RASTER_FORMAT(raster) & 0x8000) {
            if (raster->width > raster->height) {
                levels = (RwUInt8)fn_80491AB4(raster->width) + 1;
            } else {
                levels = (RwUInt8)fn_80491AB4(raster->height) + 1;
            }
        } else {
            levels = 1;
        }
        *numLevels = levels;
    }
    return 1;
}
#pragma auto_inline reset

/* Converts a linear image into GX's tiled texture layout: 8x8 tiles at 4 bits, 8x4 at 8 bits,
 * 4x4 at 16 bits, and at 32 bits 4x4 tiles whose alpha/red and green/blue halves are 32 bytes
 * apart (the source pixels are already in that order).
 * NonMatching: the code is retail's, the saved registers of the four cases are not. */
void fn_8049572C(u8 *dst, u8 *src, RwInt32 width, RwInt32 height, RwInt32 depth, RwInt32 stride) {
    switch (depth) {
    case 4: {
        RwInt32 offset;
        RwInt32 tile;
        RwInt32 srcRow;
        RwInt32 tilesPerRow;
        RwInt32 y;
        u8 *line;
        RwInt32 rowBase;
        RwInt32 lineOffset;
        RwInt32 x;
        tilesPerRow = ((width + 7) & ~7) >> 3;
        srcRow = 0;
        for (y = 0; y < height; y++) {
            rowBase = tilesPerRow * (y >> 3);
            line = src + srcRow;
            offset = 0;
            tile = 0;
            lineOffset = (y & 7) << 3;
            for (x = 0; x < width; x += 8) {
                memcpy(dst + ((RwUInt32)(((rowBase + tile) << 6) + lineOffset) >> 1), line + offset, 4);
                offset += 4;
                tile++;
            }
            srcRow += stride;
        }
        break;
    }
    case 8: {
        RwInt32 tilesPerRow;
        RwInt32 y;
        u8 *line;
        RwInt32 rowBase;
        RwInt32 lineOffset;
        RwInt32 x;
        RwInt32 tile;
        RwInt32 srcRow;
        tilesPerRow = ((width + 7) & ~7) >> 3;
        srcRow = 0;
        for (y = 0; y < height; y++) {
            line = src + srcRow;
            rowBase = tilesPerRow * (y >> 2);
            lineOffset = (y & 3) << 3;
            x = 0;
            tile = 0;
            for (; x < width; x += 8) {
                memcpy(dst + ((rowBase + tile) << 5) + lineOffset, line + x, 8);
                tile++;
            }
            srcRow += stride;
        }
        break;
    }
    case 16: {
        RwInt32 tilesPerRow;
        RwInt32 y;
        RwInt32 rowBase;
        RwInt32 lineOffset;
        RwInt32 x;
        u8 *line;
        RwInt32 tile;
        RwInt32 srcRow;
        tilesPerRow = ((width + 3) & ~3) >> 2;
        srcRow = 0;
        for (y = 0; y < height; y++) {
            rowBase = tilesPerRow * (y >> 2);
            lineOffset = (y & 3) << 2;
            line = src + srcRow;
            x = 0;
            tile = 0;
            for (; x < width; x += 4) {
                memcpy(dst + ((((rowBase + tile) << 4) + lineOffset) << 1), line, 8);
                line += 8;
                tile++;
            }
            srcRow += stride;
        }
        break;
    }
    case 32: {
        RwInt32 offset;
        RwInt32 srcRow;
        RwInt32 tilesPerRow;
        RwInt32 y;
        u8 *line;
        RwInt32 rowBase;
        RwInt32 lineOffset;
        RwInt32 x;
        RwInt32 tileOffset;
        tilesPerRow = ((width + 3) & ~3) >> 2;
        srcRow = 0;
        for (y = 0; y < height; y++) {
            rowBase = tilesPerRow * (y >> 2);
            line = src + srcRow;
            lineOffset = (y & 3) << 2;
            tileOffset = 0;
            offset = 0;
            for (x = 0; x < width; x++) {
                RwInt32 at;
                if ((x & 3) == 0) tileOffset = (rowBase + (x >> 2)) << 4;
                at = (tileOffset << 2) + ((lineOffset + (x & 3)) << 1);
                *(u16 *)(dst + at) = *(u16 *)(line + offset);
                *(u16 *)(dst + at + 0x20) = *(u16 *)(line + offset + 2);
                offset += 4;
            }
            srcRow += stride;
        }
        break;
    }
    case 24:
    default:
        RW_ERROR(0x8000000C);
        break;
    }
}

/* Converts a texture in GX's tiled layout back into a linear image (the reverse of
 * fn_8049572C). NonMatching: register allocation. */
void fn_804959C0(void *dest, void *source, RwInt32 width, RwInt32 height, RwUInt32 format, RwInt32 stride) {
    switch (format - 4) {
    case 0: {
        RwUInt32 destOffset = 0;
        RwInt32 blocks = ((width + 7) & ~7) >> 3;
        RwInt32 row;
        for (row = 0; row < height; row++) {
            RwUInt32 blockRow = blocks * (row >> 3);
            u8 *out = (u8 *)dest + destOffset;
            RwUInt32 sourceRow = (row & 7) * 8;
            RwUInt32 block = 0;
            RwInt32 outOffset;
            RwInt32 x;
            outOffset = block;
            for (x = 0; x < width; x += 8) {
                RwUInt32 sourceOffset = (((blockRow + block) * 0x40) + sourceRow) >> 1;
                memcpy(out + outOffset, (u8 *)source + sourceOffset, 4);
                block++;
                outOffset += 4;
            }
            destOffset += stride;
        }
        break;
    }
    case 1:
        goto invalid;
    case 4: {
        RwUInt32 destOffset = 0;
        RwInt32 blocks = ((width + 7) & ~7) >> 3;
        RwInt32 row;
        for (row = 0; row < height; row++) {
            RwUInt32 blockRow = blocks * (row >> 2);
            u8 *out = (u8 *)dest + destOffset;
            RwUInt32 sourceRow = (row & 3) * 8;
            RwUInt32 block = 0;
            RwInt32 outOffset;
            for (outOffset = 0; outOffset < width; outOffset += 8) {
                memcpy(out + outOffset,
                       (u8 *)source + ((blockRow + block) * 0x20) + sourceRow, 8);
                block++;
            }
            destOffset += stride;
        }
        break;
    }
    case 0xC: {
        RwUInt32 destOffset = 0;
        RwInt32 blocks = ((width + 3) & ~3) >> 2;
        RwInt32 row;
        for (row = 0; row < height; row++) {
            RwUInt32 blockRow = blocks * (row >> 2);
            RwUInt32 sourceRow = (row & 3) * 4;
            u8 *out = (u8 *)dest + destOffset;
            RwInt32 x = 0;
            RwUInt32 block = 0;
            while (x < width) {
                RwUInt32 sourceOffset = (((blockRow + block) * 0x10) + sourceRow) * 2;
                memcpy(out, (u8 *)source + sourceOffset, 8);
                block++;
                out += 8;
                x += 4;
            }
            destOffset += stride;
        }
        break;
    }
    case 0x1C: {
        RwUInt32 destOffset = 0;
        RwInt32 blocks = ((width + 3) & ~3) >> 2;
        RwInt32 row;
        for (row = 0; row < height; row++) {
            RwUInt32 blockRow = blocks * (row >> 2);
            u8 *out = (u8 *)dest + destOffset;
            RwUInt32 sourceRow = (row & 3) * 4;
            RwUInt32 block = 0;
            RwInt32 outOffset = 0;
            RwInt32 x;
            for (x = 0; x < width; x++) {
                RwUInt32 pixel = x & 3;
                RwUInt32 sourceOffset;
                if (pixel == 0) block = (blockRow + (x >> 2)) * 0x10;
                sourceOffset = block * 4 + (sourceRow + pixel) * 2;
                *(u16 *)(out + outOffset) = *(u16 *)((u8 *)source + sourceOffset);
                *(u16 *)(out + outOffset + 2) = *(u16 *)((u8 *)source + sourceOffset + 0x20);
                outOffset += 4;
            }
            destOffset += stride;
        }
        break;
    }
    default:
invalid:
        RW_ERROR(0x8000000C);
        break;
    }
}

/* Waits until GX has drawn everything that used a raster: sends the current draw-sync token first if
 * the raster was drawn since the last one went out. */
static inline void RwDlRasterSync(RwDlRasterExt *ext) {
    if (ext->token == lbl_805EDF40) {
        GXSetDrawSync(lbl_805EDF40);
        lbl_805EDF40 = (lbl_805EDF40 + 1) % 0xE000;
    }
    while (!fn_8049A78C(ext->token)) {
    }
}

/* Driver standard function RasterLock: the pixels of a mip level, copied out of GX's tiled layout
 * unless the lock is raw (flag 8). */
RwBool fn_80495C54(void *pixelsOut, void *inOut, RwInt32 flags) {
    RwInt32 pitch;
    RwUInt8 i;
    RwInt32 raw;
    RwDlRaster *raster;
    RwDlRaster *parent;
    RwDlRasterExt *ext;
    RwUInt32 offset;
    RwUInt8 level;
    raster = (RwDlRaster *)inOut;
    level = (flags >> 8) & 0xFF;
    parent = raster->parent;
    ext = (RwDlRasterExt *)((u8 *)parent + lbl_805F1658);
    switch (raster->cType & 7) {
    case 0:
    case 4:
    case 5: {
        offset = 0;
        i = level;
        while (i--) {
            offset += fn_80495428(raster, i);
        }
        offset += (RwUInt32)ext->pixels;
        pitch = fn_804955CC(raster, level);
        raw = flags & 8;
        if (!raw) {
            RwInt32 h = parent->height >> level;
            ext->lockedPixels = lbl_805F15B4->malloc(pitch * (h ? h : 1), 0x30411);
            if (ext->lockedPixels == NULL) return 0;
        }
        if (parent == raster) {
            RwInt32 w = raster->width >> level;
            RwInt32 h = raster->height >> level;
            RwInt32 width = w ? w : 1;
            RwInt32 height = h ? h : 1;
            raster->originalWidth = raster->width;
            raster->originalHeight = raster->height;
            raster->width = width;
            raster->height = height;
            if (!raw) {
                raster->cpPixels = ext->lockedPixels;
            } else {
                raster->cpPixels = (u8 *)offset;
            }
        } else if (!raw) {
            switch (raster->depth) {
            case 4:
                raster->cpPixels = ext->lockedPixels + pitch * raster->nOffsetY + (raster->nOffsetX >> 1);
                break;
            case 8:
                raster->cpPixels = ext->lockedPixels + pitch * raster->nOffsetY + raster->nOffsetX;
                break;
            case 16:
                raster->cpPixels = ext->lockedPixels + pitch * raster->nOffsetY + raster->nOffsetX * 2;
                break;
            case 32:
                raster->cpPixels = ext->lockedPixels + pitch * raster->nOffsetY + raster->nOffsetX * 4;
                break;
            case 24:
            default:
                RW_ERROR(0x8000000C);
                return 0;
            }
        } else {
            RW_ERROR(0x8000000E);
            return 0;
        }
        ext->levelPixels = (u8 *)offset;
        raster->stride = pitch;
        ext->sharedPalette = level;
        if (flags & 2) {
            if ((raster->cType & 7) == 5) {
                DCFlushRange(ext->levelPixels, fn_80495428(raster, level));
            }
            raster->privateFlags |= 2;
            parent->privateFlags |= 2;
            if (!raw) {
                fn_804959C0(ext->lockedPixels, ext->levelPixels, parent->width, parent->height, raster->depth,
                            raster->stride);
            }
        }
        if (flags & 1) {
            raster->privateFlags |= 4;
            parent->privateFlags |= 4;
            RwDlRasterSync(ext);
        }
        if (raw) {
            raster->privateFlags |= 0x20;
            parent->privateFlags |= 0x20;
        }
        break;
    }
    case 3:
    default:
        RW_ERROR(0x8000000E);
        return 0;
    }
    *(u8 **)pixelsOut = raster->cpPixels;
    return 1;
}

/* Driver standard function RasterUnlock. */
RwBool fn_804960B4(void *param1, void *inOut, RwInt32 num) {
    RwUInt32 *object = (RwUInt32 *)inOut;
    RwInt32 kind = ((u8 *)object)[0x20] & 7;
    RwUInt32 *base = (RwUInt32 *)object[0];
    RwUInt32 *viewport = (RwUInt32 *)((u8 *)base + lbl_805F1658);
    switch (kind) {
    case 0:
    case 4:
    case 5: {
        if ((((u8 *)object)[0x22] & 4) != 0) {
            if ((((u8 *)object)[0x22] & 0x20) == 0) {
                fn_8049572C((u8 *)viewport[9], (u8 *)viewport[10], base[3], base[4], object[5], object[6]);
            }
            DCFlushRange((void *)viewport[9], fn_80495428((RwDlRaster *)object, ((u8 *)viewport)[0x33]));
            GXInvalidateTexAll();
        }
        if (base == object) {
            object[3] = object[10];
            object[4] = object[11];
        }
        if ((((u8 *)object)[0x22] & 0x20) == 0) {
            lbl_805F15B4->free((void *)viewport[10]);
            viewport[10] = 0;
        }
        object[6] = 0;
        object[1] = 0;
        if ((((u8 *)object)[0x22] & 4) != 0 && (((u8 *)object)[0x23] & 0x10) != 0 &&
            ((u8 *)viewport)[0x33] == 0) {
            ((u8 *)viewport)[0x33] = 0xFF;
            ((u8 *)object)[0x22] &= -0x27;
            ((u8 *)base)[0x22] &= -0x27;
            fn_8048D2C0(object, 0);
        } else {
            ((u8 *)viewport)[0x33] = 0xFF;
            ((u8 *)object)[0x22] &= -0x27;
            ((u8 *)base)[0x22] &= -0x27;
        }
        break;
    }
    case 3:
    default:
        RW_ERROR(0x80000011);
        return 0;
    }
    return 1;
}

/* Driver standard function RasterLockPalette. */
RwBool fn_80496268(void *outp, void *inOut, RwInt32 mode) {
    RwUInt32 *out = (RwUInt32 *)outp;
    RwUInt32 *object = (RwUInt32 *)inOut;
    RwInt32 kind = ((u8 *)object)[0x20] & 7;
    switch (kind) {
    case 0:
    case 4:
        if (object == (RwUInt32 *)object[0] && object[2] == 0) {
            RwUInt32 *palette = (RwUInt32 *)((u8 *)object + lbl_805F1658);
            if ((mode & 2) != 0) ((u8 *)object)[0x22] |= 8;
            if ((mode & 1) != 0) ((u8 *)object)[0x22] |= 0x10;
            object[2] = palette[8];
            out[0] = object[2];
        }
        return 1;
    case 3:
    case 5:
    default:
        RW_ERROR(0x8000000E);
        return 0;
    }
}

/* Driver standard function RasterUnlockPalette. */
RwBool fn_80496338(void *param1, void *inOut, RwInt32 num) {
    RwUInt32 *object = (RwUInt32 *)inOut;
    RwInt32 kind = ((u8 *)object)[0x20] & 7;
    switch (kind) {
    case 0:
    case 4:
        if (object == (RwUInt32 *)object[0]) {
            RwUInt32 *palette = (RwUInt32 *)((u8 *)object + lbl_805F1658);
            if ((((u8 *)object)[0x22] & 0x10) != 0) {
                DCFlushRange((void *)palette[8], (1 << object[5]) * 2);
            }
            ((u8 *)object)[0x22] &= ~0x18;
            object[2] = 0;
        }
        return 1;
    case 3:
    case 5:
    default:
        RW_ERROR(0x80000011);
        return 0;
    }
}

/* Sets a new raster's type, flags, GX texture format, palette format and depth from the raster
 * format asked for. */
RwBool fn_80496408(RwUInt32 *object, RwUInt32 format) {
    RwUInt32 *viewport = (RwUInt32 *)(object[0] + lbl_805F1658);
    RwUInt32 other = format & 0xFF00;
    RwInt32 sub;
    u8 kind;
    ((u8 *)object)[0x20] = format & 7;
    ((u8 *)object)[0x21] = format & 0xF8;
    kind = ((u8 *)object)[0x20];

    switch (kind) {
    case 0:
    case 4:
        goto common;
    case 5:
        if ((other & 0x6000) != 0) {
            RW_ERROR_STRING(2, "rwRASTERTYPECAMERATEXTURE can not be palletized");
            return 0;
        }
        goto common;
    case 1:
    case 2:
        goto low_kind;
    case 3:
    default:
        goto kind_error;
    }

common:
    sub = other & 0xF00;
    if (sub != 0) goto dispatch;

    {
        switch (object[5]) {
        case 0:
            if (kind == 5) {
                viewport[3] = 4;
                other |= 0x200;
                viewport[5] = 0;
                object[5] = 0x10;
            } else {
                other |= 0x100;
                viewport[3] = 5;
                viewport[5] = 1;
                if ((other & 0x4000) != 0) object[5] = 4;
                else if ((other & 0x2000) != 0) object[5] = 8;
                else object[5] = 0x10;
            }
            break;
        case 1:
            goto invalid_format;
        case 4:
            if ((other & 0x4000) != 0) {
                viewport[3] = 8;
                other |= 0x100;
                viewport[4] = 2;
                viewport[5] = 1;
            } else {
                viewport[3] = 0;
                viewport[5] = 0;
            }
            break;
        case 8:
            if ((other & 0x2000) != 0) {
                viewport[3] = 9;
                other |= 0x100;
                viewport[4] = 2;
                viewport[5] = 1;
            } else {
                viewport[3] = 1;
                viewport[5] = 0;
            }
            break;
        case 0x10:
            viewport[3] = 5;
            other |= 0x100;
            viewport[5] = 1;
            break;
        case 0x20:
            viewport[3] = 6;
            other |= 0x500;
            viewport[5] = 1;
            break;
        default:
invalid_format:
            RW_ERROR(0x8000000C);
            return 0;
        }
        goto done;
    }
dispatch:
    if (sub == 0x600) goto case600;
    if (sub < 0x600) {
        if (sub == 0x300) goto case300;
        if (sub < 0x300) {
            if (sub == 0x200) goto case200;
            if (sub < 0x200) {
                if (sub == 0x100) goto case100;
            }
            goto invalid_sub;
        }
        if (sub == 0x500) goto case500;
        if (sub < 0x500) {
            if (sub == 0x400) goto case400;
        }
        goto invalid_sub;
    }
    if (sub == 0x900) goto case900;
    if (sub < 0x900) {
        if (sub == 0x800) goto case800;
        if (sub < 0x800) {
            if (sub == 0x700) goto case700;
        }
        goto invalid_sub;
    }
    if (sub == 0xA00) goto caseA00;
    if (sub != 0xA00) goto invalid_sub;

caseA00: {
        if ((other & 0x4000) != 0) {
            viewport[3] = 8;
            viewport[4] = 2;
            object[5] = 4;
        } else if ((other & 0x2000) != 0) {
            viewport[3] = 9;
            viewport[4] = 2;
            object[5] = 8;
        } else {
            viewport[3] = 5;
            object[5] = 0x10;
        }
        viewport[5] = 0;
        goto done;
    }
case200: {
        if ((other & 0x4000) != 0) {
            viewport[3] = 8;
            viewport[4] = 1;
            object[5] = 4;
        } else if ((other & 0x2000) != 0) {
            viewport[3] = 9;
            viewport[4] = 1;
            object[5] = 8;
        } else {
            viewport[3] = 4;
            object[5] = 0x10;
        }
        viewport[5] = 0;
        goto done;
    }
case100:
case300: {
        if ((other & 0x4000) != 0) {
            viewport[3] = 8;
            viewport[4] = 2;
            object[5] = 4;
        } else if ((other & 0x2000) != 0) {
            viewport[3] = 9;
            viewport[4] = 2;
            object[5] = 8;
        } else {
            viewport[3] = 5;
            object[5] = 0x10;
        }
        viewport[5] = 1;
        goto done;
    }
case600: {
        if ((other & 0x6000) != 0) {
            RW_ERROR_STRING(2, "rwRASTERFORMAT888 invalid format");
            return 0;
        }
        viewport[3] = 6;
        viewport[5] = 0;
        object[5] = 0x20;
        goto done;
    }
case500: {
        if ((other & 0x6000) != 0) {
            RW_ERROR_STRING(2, "rwRASTERFORMAT8888 invalid format");
            return 0;
        }
        viewport[3] = 6;
        viewport[5] = 1;
        object[5] = 0x20;
        goto done;
    }
case400: {
        RW_ERROR_STRING(2, "rwRASTERFORMATLUM8 invalid format");
        return 0;
    }
case700: {
        RW_ERROR_STRING(2, "rwRASTERFORMAT16 invalid format");
        return 0;
    }
case800: {
        RW_ERROR_STRING(2, "rwRASTERFORMAT24 invalid format");
        return 0;
    }
case900: {
        RW_ERROR_STRING(2, "rwRASTERFORMAT32 invalid format");
        return 0;
    }
invalid_sub:
    RW_ERROR(0x8000000D);
    return 0;

low_kind:
    if ((other & 0xF00) == 0) {
        if (lbl_805F164C->gx.aa != 0) {
            viewport[3] = 4;
            other |= 0x200;
            viewport[5] = 0;
            object[5] = 0x10;
        } else {
            viewport[3] = 6;
            other |= 0x600;
            viewport[5] = 0;
            object[5] = 0x20;
        }
        goto done;
    }
    if (other == 0x700 && lbl_805F164C->gx.aa != 0) {
        if (other != 0x800) goto done;
        if (lbl_805F164C->gx.aa != 0) goto done;
    }
    RW_ERROR(0x8000000D);
    return 0;

kind_error:
    RW_ERROR(0x8000000D);
    return 0;

done:
    ((u8 *)object)[0x23] = (other >> 8) & 0xFF;
    return 1;
}

#pragma auto_inline on
/* Allocates a texture raster's image (all its mip levels, 32-byte aligned) and its palette. */
RwBool fn_80496A20(RwDlRaster *raster, RwUInt8 levels) {
    RwUInt32 total = 0;
    RwUInt32 *ext = (RwUInt32 *)((u8 *)raster + lbl_805F1658);
    RwInt32 numLevels;
    ((u8 *)ext)[0x32] = levels - 1;
    fn_80495688(&numLevels, raster, 0);
    while (numLevels--) {
        total += fn_80495428(raster, numLevels);
    }
    if (RW_RASTER_FORMAT(raster) & 0x6000) {
        RwUInt32 bytes = total + (1 << raster->depth) * 2 + 0x1F;
        ext[6] = (RwUInt32)lbl_805F15B4->malloc(bytes, 0x30411);
        if (ext[6] == 0) {
            RW_ERROR_SIZED(0x80000013, bytes);
            return 0;
        }
        ext[7] = (ext[6] + 0x1F) & ~0x1F;
        ext[8] = ext[7] + total;
        fn_8039744C(ext, (void *)ext[8], ext[4], (1 << raster->depth) & 0xFFFF);
    } else {
        ext[6] = (RwUInt32)lbl_805F15B4->malloc(total + 0x1F, 0x30411);
        if (ext[6] == 0) {
            RW_ERROR_SIZED(0x80000013, total + 0x1F);
            return 0;
        }
        ext[7] = (ext[6] + 0x1F) & ~0x1F;
        if (raster->cType == 5) DCInvalidateRange((void *)ext[7], total);
    }
    return 1;
}
#pragma auto_inline reset

/* Driver standard function RasterCreate. */
RwBool fn_80496C58(void *out, void *inOut, RwInt32 flags) {
    RwDlRaster *raster = (RwDlRaster *)inOut;
    RwUInt32 *ext = (RwUInt32 *)((u8 *)raster + lbl_805F1658);
    RwInt32 width;
    RwInt32 height;
    raster->stride = 0;
    ext[3] = 0xFF;
    ext[4] = 0xFF;
    ext[5] = 0;
    ext[6] = 0;
    ext[7] = 0;
    ext[8] = 0;
    ext[9] = 0;
    ext[10] = 0;
    ext[11] = 0;
    *(u16 *)((u8 *)ext + 0x30) = lbl_805F1678;
    ((u8 *)ext)[0x32] = 0xFF;
    ((u8 *)ext)[0x33] = 0xFF;
    if (!fn_80496408((RwUInt32 *)raster, flags)) return 0;
    if ((width = raster->width) != 0 && (height = raster->height) != 0) {
        switch (raster->cType) {
        case 0:
        case 4:
        case 5:
            if (!(raster->cFlags & 0x80)) {
                RwUInt8 levels;
                if (RW_RASTER_FORMAT(raster) & 0x8000) {
                    if (width > height) {
                        levels = (RwUInt8)fn_80491AB4(width) + 1;
                    } else {
                        levels = (RwUInt8)fn_80491AB4(height) + 1;
                    }
                } else {
                    levels = 1;
                }
                if (!fn_80496A20(raster, levels)) {
                    RW_ERROR_STRING(2, "Failed to create surface for texture");
                    return 0;
                }
            }
            break;
        case 1:
        case 2:
            raster->cFlags = 0x80;
            break;
        case 3:
        default:
            RW_ERROR(0x8000000D);
            return 0;
        }
    } else {
        raster->cFlags = 0x80;
        return 1;
    }
    return 1;
}

RwBool fn_8049714C(void *out, void *inOut, RwInt32 num);

/* Driver standard function RasterDestroy. */
RwBool fn_80497008(void *param1, void *inOut, RwInt32 num) {
    RwUInt32 *object = (RwUInt32 *)inOut;
    if ((RwUInt32 *)object[0] == object && (((u8 *)object)[0x21] & 0x80) == 0) {
        u8 kind = ((u8 *)object)[0x20];
        switch (kind) {
        case 1:
        case 2:
            break;
        case 0:
        case 4:
        case 5: {
            RwUInt32 *viewport = (RwUInt32 *)((u8 *)object + lbl_805F1658);
            if (*(u16 *)((u8 *)viewport + 0x30) == lbl_805EDF40) {
                GXSetDrawSync(*(volatile u16 *)&lbl_805EDF40);
                lbl_805EDF40 = (lbl_805EDF40 + 1) % 0xE000;
            }
            while (fn_8049A78C(*(u16 *)((u8 *)viewport + 0x30)) == 0) {}
            if (lbl_805F1664 != NULL && (RwDlRaster *)object == lbl_805F1664->raster) {
                fn_8049714C(lbl_805F1664, 0, 0);
            }
            lbl_805F15B4->free(*(void **)((u8 *)viewport + 0x18));
            break;
        }
        case 3:
        default:
            RW_ERROR(0x8000000D);
            return 0;
        }
    }
    return 1;
}

/* Driver standard function TextureSetRaster: the texture's GX object has to be set up again. */
RwBool fn_8049714C(void *out, void *inOut, RwInt32 num) {
    RwUInt32 *object = (RwUInt32 *)out;
    object[0] = (RwUInt32)inOut;
    *(RwUInt32 *)((u8 *)object + lbl_805F1670 + 0x20) = 0x1000000;
    return 1;
}

/* Driver standard function RasterSubRaster: a sub-raster shares its parent's format. */
RwBool fn_80497168(void *out, void *inOut, RwInt32 num) {
    u8 *dest = (u8 *)out;
    u8 *source = (u8 *)inOut;
    *(RwUInt32 *)(dest + 0x18) = *(RwUInt32 *)(source + 0x18);
    *(RwUInt32 *)(dest + 0x14) = *(RwUInt32 *)(source + 0x14);
    dest[0x20] = source[0x20];
    dest[0x23] = source[0x23];
    *(RwUInt32 *)(dest + 4) = 0;
    return 1;
}

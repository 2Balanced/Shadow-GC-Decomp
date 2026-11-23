/* RenderWare 3.7 GameCube driver: conversion between images and rasters (0x80491AB4..0x804939DB).
 * The driver's standard functions RGBToPixel and PixelToRGB, ImageGetRaster (RwImageSetFromRaster),
 * RasterSetImage (RwRasterSetFromImage) and ImageFindRasterFormat, the pixel packers and unpackers
 * of the GX texture formats, the per-depth converters, and the colour reduction that turns a 32-bit
 * image into a palettised one through the palette quantiser. The file owns its messages and jump
 * tables (.data 0x8056E0C8..0x8056E347: the first message is shared by six functions, so it is one
 * file's pooled string) and one constant (.sdata2 0x805FB3A8). fn_80491AB4 (the log2 of a size,
 * used by the raster file) has no data of its own and is placed at the head of this file.
 * Built as C with the driver's flags (-O4,p, no automatic inlining, writable strings).
 *
 * Names are inferred from behaviour; definitions are in retail address order.
 * NonMatching: fn_80491AF4 and fn_80491B14 (3 and 6 rows: the register of the last byte load),
 * fn_80491BAC and fn_80491EC0 (retail inlines most of the packers and keeps the pixel in a stack
 * slot; marking them #pragma auto_inline on inlines all of them and is further off), fn_804920C8,
 * fn_8049231C and fn_80492610 (drafts: retail's depth switches are jump tables and their error
 * paths are cloned). */
#include <dolphin/types.h>
#include "Game/rw/r7.h"

extern RwInt32 lbl_805F1658; /* offset of the driver's data in a raster (RwGameCubeRaster.c) */

void *fn_80489F70(void *raster, RwUInt32 level, RwInt32 mode); /* RwRasterLock */
void *fn_80489C94(void *raster, RwInt32 mode);                 /* RwRasterLockPalette */
void fn_80489B88(void *raster);                                /* RwRasterUnlockPalette */
void fn_80489B40(void *raster);                                /* RwRasterUnlock */
RwDlImage *fn_80487D08(RwInt32 width, RwInt32 height, RwInt32 depth); /* RwImageCreate */
void fn_80487E18(void);                                        /* RwImageAllocatePixels */
void fn_80487DB0(void *image);                                 /* RwImageDestroy */
int fn_8048FED8(void *quant);                                  /* the palette quantiser: set up */
void fn_8048DEB0(void *quant, f32 weight, void *image);         /* add an image's colours */
void fn_8048F5D0(RwDlRGBA *palette, RwInt32 size, void *quant);  /* resolve the palette */
void fn_8048FAB0(RwUInt32 pixels, RwUInt32 stride, RwUInt32 depth, RwUInt32 pack, void *quant,
                 void *image);                                 /* map the pixels */
void fn_804903F8(void *quant);                                 /* free */

/* The log2 of a size (the position of its highest set bit), -1 for 0. */
RwInt32 fn_80491AB4(RwInt32 value) {
    RwInt32 result = -1;
    for (; value != 0; value >>= 1) result++;
    return result;
}

/* RwRGBA to a GX pixel: RGB5A3 with its top bit set (opaque 555). */
RwUInt32 fn_80491AD4(const u8 *color) {
    RwUInt32 red = color[0];
    RwUInt32 green = color[1];
    RwUInt32 blue = color[2];
    RwUInt32 packed = ((red & 0xF8) << 7) | 0x8000;
    packed = __rlwimi(packed, green, 2, 22, 26);
    return __rlwimi(packed, blue, 29, 27, 31);
}

/* RwRGBA to RGB565. */
RwUInt32 fn_80491AF4(const u8 *color) {
    RwUInt32 packed = *(volatile u8 *)(color + 1);
    RwUInt32 red = *(volatile u8 *)color;
    RwUInt32 blue;
    RwUInt32 result;
    packed = (packed & 0xFC) << 3;
    blue = color[2];
    packed = __rlwimi(packed, red, 8, 16, 20);
    result = __rlwimi(packed, blue, 29, 27, 31);
    return result;
}

/* RwRGBA to RGB5A3: 4443 while it has alpha, else opaque 555. */
RwUInt32 fn_80491B14(const u8 *color) {
    RwUInt32 green;
    RwUInt32 packed;
    RwUInt32 red;
    RwUInt32 blue;
    RwUInt32 alpha = color[3];
    if (alpha != 0xFF) {
        RwUInt32 result;
        packed = *(volatile u8 *)color;
        green = *(volatile u8 *)(color + 1);
        packed = (packed & 0xF0) << 4;
        blue = color[2];
        packed = __rlwimi(packed, alpha, 7, 17, 19);
        packed = __rlwimi(packed, green, 0, 24, 27);
        result = __rlwimi(packed, blue, 28, 28, 31);
        return result;
    }
    red = color[0];
    green = color[1];
    blue = color[2];
    packed = ((red & 0xF8) << 7) | 0x8000;
    packed = __rlwimi(packed, green, 2, 22, 26);
    return __rlwimi(packed, blue, 29, 27, 31);
}

/* RwRGBA to an opaque 32-bit pixel. */
RwUInt32 fn_80491B64(const u8 *color) {
    RwUInt32 red = color[0];
    RwUInt32 green = color[1];
    RwUInt32 blue = color[2];
    return blue | ((red << 16) | 0xFF000000 | (green << 8));
}

/* RwRGBA to a 32-bit pixel with alpha. */
RwUInt32 fn_80491B88(const u8 *color) {
    RwUInt32 packed = (RwUInt32)color[0] << 16;
    RwUInt32 alpha = color[3];
    RwUInt32 green = color[1];
    RwUInt32 blue;
    packed = __rlwimi(packed, alpha, 24, 0, 7);
    blue = color[2];
    packed = __rlwimi(packed, green, 8, 16, 23);
    return blue | packed;
}

/* Driver standard function RGBToPixel: an RwRGBA as a pixel of the raster format. */
RwBool fn_80491BAC(RwUInt32 *out, const u8 *color, RwInt32 format) {
    switch (format & 0xF00) {
    case 0x000:
    case 0x200: *out = fn_80491AF4(color); break;
    case 0x100:
    case 0x300: *out = fn_80491B14(color); break;
    case 0x400: RW_ERROR_STRING(2, "rwRASTERFORMATLUM8 not yet supported"); break;
    case 0x500: *out = fn_80491B88(color); break;
    case 0x600: *out = fn_80491B64(color); break;
    case 0xA00: *out = fn_80491AD4(color); break;
    default: RW_ERROR(0x8000000D); break;
    }
    return 1;
}

/* Opaque 555 to RwRGBA. */
void fn_80491D98(u8 *out, RwUInt32 packed) {
    out[0] = (packed >> 7) & 0xF8;
    out[1] = (packed >> 2) & 0xF8;
    out[2] = packed << 3;
    out[3] = 0xFF;
}

/* RGB565 to RwRGBA. */
void fn_80491DBC(u8 *out, RwUInt32 packed) {
    out[0] = (packed >> 8) & 0xF8;
    out[1] = (packed >> 3) & 0xFC;
    out[2] = packed << 3;
    out[3] = 0xFF;
}

/* RGB5A3 to RwRGBA, alpha only on or off. */
void fn_80491DE0(u8 *out, RwUInt32 packed) {
    if ((packed & 0x8000) != 0) {
        out[0] = (packed >> 7) & 0xF8;
        out[1] = (packed >> 2) & 0xF8;
        out[2] = packed << 3;
        out[3] = 0xFF;
    } else {
        out[0] = (packed >> 4) & 0xF0;
        out[1] = packed & 0xF0;
        out[2] = packed << 4;
        out[3] = 0;
    }
}

/* RGB5A3 to RwRGBA. */
void fn_80491E30(u8 *out, RwUInt32 packed) {
    if ((packed & 0x8000) != 0) {
        out[0] = (packed >> 7) & 0xF8;
        out[1] = (packed >> 2) & 0xF8;
        out[2] = packed << 3;
        out[3] = 0xFF;
    } else {
        out[0] = (packed >> 4) & 0xF0;
        out[1] = packed & 0xF0;
        out[2] = packed << 4;
        out[3] = (packed >> 7) & 0xE0;
    }
}

/* An opaque 32-bit pixel to RwRGBA. */
void fn_80491E80(u8 *out, RwUInt32 packed) {
    out[3] = 0xFF;
    out[0] = (packed >> 16) & 0xFF;
    out[1] = (packed >> 8) & 0xFF;
    out[2] = packed & 0xFF;
}

/* A 32-bit pixel to RwRGBA. */
void fn_80491EA0(u8 *out, RwUInt32 packed) {
    out[3] = (packed >> 24) & 0xFF;
    out[0] = (packed >> 16) & 0xFF;
    out[1] = (packed >> 8) & 0xFF;
    out[2] = packed & 0xFF;
}

/* Driver standard function PixelToRGB: a pixel of the raster format as an RwRGBA. */
RwBool fn_80491EC0(u8 *out, const RwUInt32 *color, RwInt32 format) {
    RwUInt32 packed = *color;
    switch (format & 0xF00) {
    case 0x000: fn_80491DBC(out, packed); break;
    case 0x100: fn_80491DE0(out, packed); break;
    case 0x200: fn_80491DBC(out, packed); break;
    case 0x300: fn_80491E30(out, packed); break;
    case 0x400: RW_ERROR_STRING(2, "rwRASTERFORMATLUM8 not yet supported"); break;
    case 0x500: fn_80491EA0(out, packed); break;
    case 0x600: fn_80491E80(out, packed); break;
    case 0xA00: fn_80491D98(out, packed); break;
    default: RW_ERROR(0x8000000D); break;
    }
    return 1;
}

typedef void (*RwDlUnpacker)(u8 *out, RwUInt32 packed);

/* Raster to 4-bit image: the palette through the unpacker of the raster's format, then two
 * indices per byte. Draft. */
void fn_804920C8(u8 *dest, u8 *source) {
    RwDlUnpacker reader = NULL;
    switch (((RwUInt32)source[0x23] << 8) & 0xF00) {
    case 0x100: reader = fn_80491DE0; break;
    case 0x200: reader = fn_80491DBC; break;
    case 0x300: reader = fn_80491E30; break;
    case 0x400: RW_ERROR_STRING(2, "rwRASTERFORMATLUM8 not yet supported"); break;
    case 0x500: reader = fn_80491EA0; break;
    case 0x600: reader = fn_80491E80; break;
    case 0xA00: reader = fn_80491D98; break;
    default: RW_ERROR(0x8000000D); break;
    }
    switch (*(RwUInt32 *)(source + 0x14)) {
    case 4: {
        RwUInt32 i;
        RwUInt32 row;
        for (i = 0; i < 0x10; i++)
            reader((u8 *)(*(RwUInt32 *)(dest + 0x18) + i * 4), *(u16 *)(*(RwUInt32 *)(source + 0x08) + i * 2));
        for (row = 0; row < *(RwUInt32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x04) + *(RwUInt32 *)(source + 0x18) * row;
            u8 *output = *(u8 **)(dest + 0x14) + *(RwUInt32 *)(dest + 0x10) * row;
            RwUInt32 x;
            for (x = 0; x < *(RwUInt32 *)(source + 0x0C); x += 2) {
                output[0] = input[0] >> 4;
                output[1] = input[0] & 0xF;
                input++;
                output += 2;
            }
        }
        break;
    }
    case 8:
    case 16:
    case 32:
        RW_ERROR_STRING(2, "Conversion from 8/16/32bit rasters to 4bit images is not supported");
        break;
    default:
        RW_ERROR(0x8000000C);
        break;
    }
}

/* Raster to 8-bit image. Draft. */
void fn_8049231C(u8 *dest, u8 *source) {
    RwDlUnpacker reader = NULL;
    switch (((RwUInt32)source[0x23] << 8) & 0xF00) {
    case 0x100: reader = fn_80491DE0; break;
    case 0x200: reader = fn_80491DBC; break;
    case 0x300: reader = fn_80491E30; break;
    case 0x400: RW_ERROR_STRING(2, "rwRASTERFORMATLUM8 not yet supported"); break;
    case 0x500: reader = fn_80491EA0; break;
    case 0x600: reader = fn_80491E80; break;
    case 0xA00: reader = fn_80491D98; break;
    default: RW_ERROR(0x8000000D); break;
    }
    switch (*(RwUInt32 *)(source + 0x14)) {
    case 4: {
        RwUInt32 i;
        RwUInt32 row;
        for (i = 0; i < 0x10; i++)
            reader((u8 *)(*(RwUInt32 *)(dest + 0x18) + i * 4), *(u16 *)(*(RwUInt32 *)(source + 0x08) + i * 2));
        for (row = 0; row < *(RwUInt32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x04) + *(RwUInt32 *)(source + 0x18) * row;
            u8 *output = *(u8 **)(dest + 0x14) + *(RwUInt32 *)(dest + 0x10) * row;
            RwUInt32 x;
            for (x = 0; x < *(RwUInt32 *)(source + 0x0C); x += 2) {
                output[0] = input[0] >> 4;
                output[1] = input[0] & 0xF;
                input++;
                output += 2;
            }
        }
        break;
    }
    case 8: {
        RwUInt32 i;
        RwUInt32 row;
        for (i = 0; i < 0x100; i++)
            reader((u8 *)(*(RwUInt32 *)(dest + 0x18) + i * 4), *(u16 *)(*(RwUInt32 *)(source + 0x08) + i * 2));
        for (row = 0; row < *(RwUInt32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x04) + *(RwUInt32 *)(source + 0x18) * row;
            u8 *output = *(u8 **)(dest + 0x14) + *(RwUInt32 *)(dest + 0x10) * row;
            RwUInt32 x;
            for (x = 0; x < *(RwUInt32 *)(source + 0x0C); x++) *output++ = *input++;
        }
        break;
    }
    case 16:
    case 32:
        RW_ERROR_STRING(2, "Conversion from 16/32bit rasters to 8bit images is not supported");
        break;
    default:
        RW_ERROR(0x8000000C);
        break;
    }
}

/* Raster to 32-bit image. Draft. */
void fn_80492610(u8 *dest, u8 *source) {
    RwDlUnpacker reader = NULL;
    switch (((RwUInt32)source[0x23] << 8) & 0xF00) {
    case 0x100: reader = fn_80491DE0; break;
    case 0x200: reader = fn_80491DBC; break;
    case 0x300: reader = fn_80491E30; break;
    case 0x400: RW_ERROR_STRING(2, "rwRASTERFORMATLUM8 not yet supported"); break;
    case 0x500: reader = fn_80491EA0; break;
    case 0x600: reader = fn_80491E80; break;
    case 0xA00: reader = fn_80491D98; break;
    default: RW_ERROR(0x8000000D); break;
    }
    switch (*(RwUInt32 *)(source + 0x14)) {
    case 4: {
        RwUInt32 palette[0x10];
        RwUInt32 i;
        RwUInt32 row;
        for (i = 0; i < 0x10; i++)
            reader((u8 *)&palette[i], *(u16 *)(*(RwUInt32 *)(source + 0x08) + i * 2));
        for (row = 0; row < *(RwUInt32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x04) + *(RwUInt32 *)(source + 0x18) * row;
            RwUInt32 *output = (RwUInt32 *)(*(u8 **)(dest + 0x14) + *(RwUInt32 *)(dest + 0x10) * row);
            RwUInt32 x;
            for (x = 0; x < *(RwUInt32 *)(source + 0x0C); x += 2) {
                output[0] = palette[input[0] >> 4];
                output[1] = palette[input[0] & 0xF];
                input++;
                output += 2;
            }
        }
        break;
    }
    case 8: {
        RwUInt32 palette[0x100];
        RwUInt32 i;
        RwUInt32 row;
        for (i = 0; i < 0x100; i++)
            reader((u8 *)&palette[i], *(u16 *)(*(RwUInt32 *)(source + 0x08) + i * 2));
        for (row = 0; row < *(RwUInt32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x04) + *(RwUInt32 *)(source + 0x18) * row;
            RwUInt32 *output = (RwUInt32 *)(*(u8 **)(dest + 0x14) + *(RwUInt32 *)(dest + 0x10) * row);
            RwUInt32 x;
            for (x = 0; x < *(RwUInt32 *)(source + 0x0C); x++) *output++ = palette[*input++];
        }
        break;
    }
    case 16: {
        RwUInt32 row;
        for (row = 0; row < *(RwUInt32 *)(source + 0x10); row++) {
            u16 *input = (u16 *)(*(u8 **)(source + 0x04) + *(RwUInt32 *)(source + 0x18) * row);
            u8 *output = *(u8 **)(dest + 0x14) + *(RwUInt32 *)(dest + 0x10) * row;
            RwUInt32 x;
            for (x = 0; x < *(RwUInt32 *)(source + 0x0C); x++) {
                reader(output, *input);
                output += 4;
                input++;
            }
        }
        break;
    }
    case 32: {
        RwUInt32 row;
        for (row = 0; row < *(RwUInt32 *)(source + 0x10); row++) {
            RwUInt32 *input = (RwUInt32 *)(*(u8 **)(source + 0x04) + *(RwUInt32 *)(source + 0x18) * row);
            u8 *output = *(u8 **)(dest + 0x14) + *(RwUInt32 *)(dest + 0x10) * row;
            RwUInt32 x;
            for (x = 0; x < *(RwUInt32 *)(source + 0x0C); x++) {
                reader(output, *input);
                output += 4;
                input++;
            }
        }
        break;
    }
    default:
        RW_ERROR(0x8000000C);
        break;
    }
}

/* Driver standard function ImageGetRaster (RwImageSetFromRaster): locks the raster (and its
 * palette) for reading unless the caller already has, and converts by the image's depth. */
RwBool fn_804929F0(void *out, void *inOut, RwInt32 num) {
    RwDlImage *image = (RwDlImage *)out;
    RwDlRaster *raster = (RwDlRaster *)inOut;
    RwInt32 lockedPixels = 0;
    RwInt32 lockedPalette = 0;
    if ((raster->privateFlags & 2) == 0) {
        fn_80489F70(raster, 0, 2);
        lockedPixels = 1;
    }
    if (((raster->cFormat << 8) & 0x6000) != 0 && (raster->privateFlags & 8) == 0) {
        fn_80489C94(raster, 2);
        lockedPalette = 1;
    }
    switch (image->depth) {
    case 4:
        fn_804920C8((u8 *)image, (u8 *)raster);
        break;
    case 8:
        fn_8049231C((u8 *)image, (u8 *)raster);
        break;
    case 32:
        fn_80492610((u8 *)image, (u8 *)raster);
        break;
    default:
        RW_ERROR(0x80000008);
        break;
    }
    if (lockedPalette == 1) fn_80489B88(raster);
    if (lockedPixels == 1) fn_80489B40(raster);
    return 1;
}

/* Makes a copy of an image with the given palettised depth (colour reduction through the
 * palette quantiser). */
RwDlImage *fn_80492B24(RwDlImage *image, RwInt32 depth) {
    RwDlImage *result = fn_80487D08(image->width, image->height, depth);
    u8 buffer[0x4000];
    if (result == NULL) return NULL;
    fn_80487E18();
    if (fn_8048FED8(buffer) == 0) return NULL;
    fn_8048DEB0(buffer, 1.0f, image);
    fn_8048F5D0(result->palette, 1 << depth, buffer);
    fn_8048FAB0((RwUInt32)result->cpPixels, result->stride, result->depth, 0, buffer, image);
    fn_804903F8(buffer);
    return result;
}

typedef RwUInt32 (*RwDlPacker)(const RwDlRGBA *rgba);

/* Picks the function that packs an RwRGBA into a pixel of the raster's colour format
 * (rwRASTERFORMAT1555 .. rwRASTERFORMAT555, the format's 0x0F00 field). An inline function in
 * retail too: its two error records sit below the caller's on the stack. */
static inline RwDlPacker RwDlSelectPacker(RwInt32 format) {
    RwDlPacker packer = NULL;
    switch (format & 0x0F00) {
    case 0x400:
        RW_ERROR_STRING(2, "rwRASTERFORMATLUM8 not yet supported");
        break;
    case 0xA00:
        packer = (RwDlPacker)fn_80491AD4;
        break;
    case 0x200:
        packer = (RwDlPacker)fn_80491AF4;
        break;
    case 0x100:
    case 0x300:
        packer = (RwDlPacker)fn_80491B14;
        break;
    case 0x600:
        packer = (RwDlPacker)fn_80491B64;
        break;
    case 0x500:
        packer = (RwDlPacker)fn_80491B88;
        break;
    default:
        RW_ERROR(0x8000000D);
        break;
    }
    return packer;
}

/* Image to 4-bit palettised raster: packs two indices per byte and converts the 16 palette
 * entries; an 8-bit or 32-bit image goes through a 4-bit copy first. */
void fn_80492BE8(RwDlRaster *raster, RwDlImage *image) {
    RwDlRasterExt *ext = (RwDlRasterExt *)((u8 *)raster->parent + lbl_805F1658);
    RwDlPacker packer = RwDlSelectPacker(RW_RASTER_FORMAT(raster));
    switch (image->depth) {
    case 4: {
        RwInt32 y;
        for (y = 0; y < raster->height; y++) {
            u8 *src = image->cpPixels + image->stride * y;
            u8 *dst = raster->cpPixels + raster->stride * y;
            RwInt32 x;
            for (x = 0; x < raster->width; x += 2) {
                *dst++ = ((src[0] & 0xF) << 4) | (src[1] & 0xF);
                src += 2;
            }
        }
        if (!ext->sharedPalette) {
            u16 *entry = (u16 *)raster->palette;
            RwInt32 i;
            for (i = 0; i < 16; i++) {
                entry[i] = packer(&image->palette[i]);
            }
        }
        break;
    }
    case 8:
    case 32: {
        RwDlImage *reduced = fn_80492B24(image, raster->depth);
        if (reduced != NULL) {
            fn_80492BE8(raster, reduced);
            fn_80487DB0(reduced);
        }
        break;
    }
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

/* Image to 8-bit palettised raster: copies the indices and converts the palette; a 32-bit image
 * goes through an 8-bit copy first. */
void fn_80492E40(RwDlRaster *raster, RwDlImage *image) {
    RwDlPacker packer = RwDlSelectPacker(RW_RASTER_FORMAT(raster));
    switch (image->depth) {
    case 4:
    case 8: {
        RwInt32 y = 0;
        RwDlRasterExt *ext = (RwDlRasterExt *)((u8 *)raster->parent + lbl_805F1658);
        for (; y < raster->height; y++) {
            u8 *src = image->cpPixels + image->stride * y;
            u8 *dst = raster->cpPixels + raster->stride * y;
            RwInt32 x;
            for (x = 0; x < raster->width; x++) {
                *dst = *src;
                src++;
                dst++;
            }
        }
        if (!ext->sharedPalette) {
            u16 *entry = (u16 *)raster->palette;
            RwInt32 i;
            for (i = 0; i < (1 << image->depth); i++) {
                entry[i] = packer(&image->palette[i]);
            }
        }
        break;
    }
    case 32: {
        RwDlImage *reduced = fn_80492B24(image, raster->depth);
        if (reduced != NULL) {
            fn_80492E40(raster, reduced);
            fn_80487DB0(reduced);
        }
        break;
    }
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

/* Image to 16-bit raster: palettised images through a converted palette, 32-bit images pixel by
 * pixel. */
void fn_8049309C(RwDlRaster *raster, RwDlImage *image) {
    RwDlPacker packer = RwDlSelectPacker(RW_RASTER_FORMAT(raster));
    switch (image->depth) {
    case 4:
    case 8: {
        u16 palette[256];
        RwInt32 i;
        RwInt32 y;
        for (i = 0; i < (1 << image->depth); i++) {
            palette[i] = packer(&image->palette[i]);
        }
        for (y = 0; y < raster->height; y++) {
            u8 *src = image->cpPixels + image->stride * y;
            u16 *dst = (u16 *)(raster->cpPixels + raster->stride * y);
            RwInt32 x;
            for (x = 0; x < raster->width; x++) {
                *dst = palette[*src];
                src++;
                dst++;
            }
        }
        break;
    }
    case 32: {
        RwInt32 y;
        RwInt32 x;
        u8 *src;
        u16 *dst;
        for (y = 0; y < raster->height; y++) {
            src = image->cpPixels + image->stride * y;
            dst = (u16 *)(raster->cpPixels + raster->stride * y);
            for (x = 0; x < raster->width; x++) {
                *dst = packer((RwDlRGBA *)src);
                dst++;
                src += 4;
            }
        }
        break;
    }
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

/* Image to 32-bit raster; as fn_8049309C with 32-bit pixels. */
void fn_80493330(RwDlRaster *raster, RwDlImage *image) {
    RwDlPacker packer = RwDlSelectPacker(RW_RASTER_FORMAT(raster));
    switch (image->depth) {
    case 4:
    case 8: {
        RwUInt32 palette[256];
        RwInt32 i;
        RwInt32 y;
        for (i = 0; i < (1 << image->depth); i++) {
            palette[i] = packer(&image->palette[i]);
        }
        for (y = 0; y < raster->height; y++) {
            u8 *src = image->cpPixels + image->stride * y;
            RwUInt32 *dst = (RwUInt32 *)(raster->cpPixels + raster->stride * y);
            RwInt32 x;
            for (x = 0; x < raster->width; x++) {
                *dst = palette[*src];
                src++;
                dst++;
            }
        }
        break;
    }
    case 32: {
        RwInt32 y;
        RwInt32 x;
        u8 *src;
        RwUInt32 *dst;
        for (y = 0; y < raster->height; y++) {
            src = image->cpPixels + image->stride * y;
            dst = (RwUInt32 *)(raster->cpPixels + raster->stride * y);
            for (x = 0; x < raster->width; x++) {
                *dst = packer((RwDlRGBA *)src);
                dst++;
                src += 4;
            }
        }
        break;
    }
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

/* Driver standard function RasterSetImage (RwRasterSetFromImage): locks the raster (and its
 * palette) for writing unless the caller already has, and converts by the raster's depth. */
RwBool fn_804935C4(void *out, void *inOut, RwInt32 num) {
    RwDlRaster *raster = (RwDlRaster *)out;
    RwInt32 pixelsLocked = 0;
    RwInt32 paletteLocked = 0;
    RwInt32 format = RW_RASTER_FORMAT(raster);
    RwDlImage *image = (RwDlImage *)inOut;
    if ((raster->privateFlags & 4) != 0) pixelsLocked = 1;
    if (!pixelsLocked) {
        if (fn_80489F70(raster, 0, 5) == 0) return 0;
    }
    format &= 0x6000;
    if (format != 0) {
        if ((raster->privateFlags & 0x10) != 0) paletteLocked = 1;
        if (!paletteLocked) {
            if (fn_80489C94(raster, 5) == 0) return 0;
        }
    }
    switch (raster->depth) {
    case 4:
        fn_80492BE8(raster, image);
        break;
    case 8:
        fn_80492E40(raster, image);
        break;
    case 16:
        fn_8049309C(raster, image);
        break;
    case 24:
        RW_ERROR(0x8000000C);
        break;
    case 32:
        fn_80493330(raster, image);
        break;
    default:
        RW_ERROR(0x8000000C);
        break;
    }
    if (format != 0) {
        if (!paletteLocked) fn_80489B88(raster);
    }
    if (!pixelsLocked) fn_80489B40(raster);
    return 1;
}

/* Adds the palette bit of a 4-bit or 8-bit image to a raster format. Retail ORs at run time,
 * which only an inline function whose parameter is modified gives. */
static inline RwInt32 RwDlPalettised(RwInt32 format, RwInt32 depth) {
    if (depth == 4) {
        format |= 0x4000;
    } else {
        format |= 0x2000;
    }
    return format;
}

/* Picks the raster format that suits an image: 0x100 (1555) when its alpha is only on or off,
 * 0x200 (565) when it is opaque, 0x300 (4444) when it has partial alpha, with the palette bits
 * (0x2000 for 8-bit, 0x4000 for 4-bit) of a palettised image. */
RwInt32 fn_8049375C(RwDlImage *image) {
    RwInt32 depth = image->depth;
    RwInt32 hasAlpha = 0;
    RwInt32 format;
    if (depth == 4 || depth == 8) {
        RwInt32 width = image->width;
        RwInt32 height = image->height;
        RwInt32 y = 0;
        u8 *row = image->cpPixels;
        RwDlRGBA *palette = image->palette;
        for (; y < height; y++) {
            u8 *pixel = row;
            RwInt32 x;
            for (x = 0; x < width; x++) {
                u8 alpha = palette[*pixel].alpha;
                if (alpha != 0xFF) {
                    hasAlpha = 1;
                    if (alpha > 0xF) {
                        return RwDlPalettised(0x300, depth);
                    }
                }
                pixel++;
            }
            row += image->stride;
        }
    } else {
        RwInt32 width = image->width;
        RwInt32 height = image->height;
        RwInt32 y = 0;
        u8 *row = image->cpPixels;
        for (; y < height; y++) {
            u8 *pixel = row;
            RwInt32 x;
            for (x = 0; x < width; x++) {
                u8 alpha = pixel[3];
                if (alpha != 0xFF) {
                    hasAlpha = 1;
                    if (alpha > 0xF) {
                        return 0x300;
                    }
                }
                pixel += 4;
            }
            row += image->stride;
        }
    }
    format = 0x200;
    if (hasAlpha) format = 0x100;
    if (depth == 4) {
        format |= 0x4000;
    } else if (depth == 8) {
        format |= 0x2000;
    }
    return format;
}

/* Driver standard function ImageFindRasterFormat: fills in the type, size and format of a raster
 * that would hold the image. Textures are at most 1024 texels a side and, with mip maps (0x8000),
 * rounded down to powers of two; camera and Z rasters keep the image's size. */
RwBool fn_80493888(void *out, void *inOut, RwInt32 flags) {
    RwDlRaster *raster = (RwDlRaster *)out;
    RwDlImage *image = (RwDlImage *)inOut;
    RwInt32 type = flags & 7;
    raster->cType = type;
    raster->depth = 0;
    switch (type) {
    case 0:
    case 4:
    case 5:
    {

        RwInt32 w, h;
        w = image->width;
        raster->width = (w > 1024) ? 1024 : w;
        h = image->height;
        raster->height = (h > 1024) ? 1024 : h;
        if ((flags & 0x8000) != 0) {
            RwInt32 size;
            RwInt32 logWidth = -1;
            RwInt32 logHeight;
            for (size = raster->width; size != 0; size >>= 1) logWidth++;
            raster->width = 1 << logWidth;
            logHeight = -1;
            for (size = raster->height; size != 0; size >>= 1) logHeight++;
            raster->height = 1 << logHeight;
        }
        {
            RwInt32 f;
            f = fn_8049375C(image);
            f |= flags & 0x9000;
            raster->cFormat = f >> 8;
        }
        return 1;
    }
    case 1:
    case 2:
        raster->cFormat = 0;
        raster->width = image->width;
        raster->height = image->height;
        return 1;
    default:
        RW_ERROR(0x8000000D);
        return 0;
    }
}

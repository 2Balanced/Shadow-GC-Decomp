/* RenderWare core, 0x8048915C..0x8048A96B (a size cut, not a source file):
 *   0x8048915C  the rest of the image module: depth conversion, copy, gamma correction, gamma
 *               tables, image from raster and raster from image, raster format search
 *   0x80489B2C  the raster module: offset, unlock, destroy, plugin registration, levels, show,
 *               sub-raster, create, lock, module close/open
 *   0x8048A178  image resampling: one row span, one block of rows, RwImageResample
 * Functions keep their fn_ names because game code calls them by those; each comment gives the
 * public RenderWare function the behaviour corresponds to. Compiled as C with cflags_rw_r6
 * (configure.py), see RwCpp48.cpp.
 * NonMatching (probes/Game_RwCpp50.py): fn_8048915C, fn_804893B8 and fn_80489830 are register
 * numbering only; fn_804894B8: retail unrolls the gamma loop by four, GC/2.7 by two for every
 * loop spelling tried; fn_8048A178 and fn_8048A790: scheduling, and GC/2.7 reassociates
 * srcY + (yStep - 1). */
#pragma cplusplus off
#include <dolphin/types.h>
#include "Game/rw/r6.h"

extern RwModuleInfo lbl_805F15C0;     /* image module */
extern RwModuleInfo lbl_805F15C8;     /* raster module */
extern RwPluginRegistry lbl_8056E018; /* raster plugins */
extern s32 lbl_805EDEF8;
extern s32 lbl_805EDEFC;
extern u8 lbl_805E2CE8[];
extern const f32 lbl_805FB310; /* 1.0f */
extern const f32 lbl_805FB314; /* 255.0f */
extern const f32 lbl_805FB318; /* 0.5f */
extern const f32 lbl_805FB328; /* 1.0f / 65536.0f */
extern const f32 lbl_805FB32C; /* 1.0f / 255.0f */
extern const f32 lbl_805FB330; /* 1.0f */
extern const f32 lbl_805FB348; /* 65536.0f */
extern const f32 lbl_805FB34C; /* 0.5f */
extern const f32 lbl_805FB350; /* 255.0f */

double pow(double x, double y);
void fn_804807CC(void);
RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        void *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
s32 fn_80481DC8(RwPluginRegistry *reg, s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
const RwPluginRegistry *fn_8048209C(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_80482144(const RwPluginRegistry *reg, void *object);

#define IMAGEGLOBAL(field) (RWGLOBALAT(rwImageGlobals, lbl_805F15C0.globalsOffset).field)
#define RASTERGLOBAL(field) (RWGLOBALAT(rwRasterGlobals, lbl_805F15C8.globalsOffset).field)

/* Converts the pixels of an image of another depth: 4-bit indices to 8-bit ones, palettised
 * pixels to 32-bit through the palette. */
RwBool fn_8048915C(RwImage *destImage, const RwImage *sourceImage) {
    RwBool result = FALSE;
    s32 width = destImage->width;
    s32 height = destImage->height;
    u8 *dstPixels = destImage->cpPixels;
    const u8 *srcPixels = sourceImage->cpPixels;
    const RwRGBA *palette = sourceImage->palette;

    switch ((sourceImage->depth << 8) | destImage->depth) {
    case 0x404:
    case 0x808:
    case 0x2020:
        result = TRUE;
        break;
    case 0x408: {
        s32 srcStride = sourceImage->stride;
        s32 y;

        for (y = 0; y < height; y++) {
            memcpy(dstPixels, srcPixels, width);
            srcPixels += srcStride;
            dstPixels += destImage->stride;
        }
        result = TRUE;
        break;
    }
    case 0x420:
    case 0x820: {
        s32 srcStride = sourceImage->stride;
        s32 y;

        for (y = 0; y < height; y++) {
            s32 x;

            for (x = 0; x < width; x++) {
                ((RwRGBA *)dstPixels)[x] = palette[srcPixels[x]];
            }
            srcPixels += srcStride;
            dstPixels += destImage->stride;
        }
        result = TRUE;
        break;
    }
    case 0x804:
    case 0x2004:
    case 0x2008:
    default:
        RWERROR((0x80000009));
        break;
    }
    return result;
}

/* RwImageCopy: the palette and the rows when the depths agree, a conversion otherwise; the
 * gamma corrected flag follows the source. */
RwImage *fn_804893B8(RwImage *destImage, const RwImage *sourceImage) {
    if (destImage->depth == sourceImage->depth) {
        u8 *dst;
        const u8 *src;
        s32 y;
        s32 srcStride;
        s32 rowBytes;

        if (destImage->palette && sourceImage->palette && sourceImage->depth <= 8) {
            memcpy(destImage->palette, sourceImage->palette,
                   (1 << sourceImage->depth) * sizeof(RwRGBA));
        }
        rowBytes = ((destImage->depth + 7) >> 3) * destImage->width;
        src = sourceImage->cpPixels;
        dst = destImage->cpPixels;
        srcStride = sourceImage->stride;
        for (y = 0; y < destImage->height; y++) {
            memcpy(dst, src, rowBytes);
            src += srcStride;
            dst += destImage->stride;
        }
    } else if (!fn_8048915C(destImage, sourceImage)) {
        destImage = NULL;
    }
    destImage->flags &= ~2;
    destImage->flags |= sourceImage->flags & 2;
    return destImage;
}

/* Puts the red, green and blue of count colours through the gamma table. */
static inline void ImageGammaCorrectColors(RwRGBA *dst, const RwRGBA *src, s32 count) {
    const u8 *gamma = IMAGEGLOBAL(gammaTable);

    while (count--) {
        dst->red = gamma[src->red];
        dst->green = gamma[src->green];
        dst->blue = gamma[src->blue];
        dst->alpha = src->alpha;
        dst++;
        src++;
    }
}

/* RwImageGammaCorrect. */
RwImage *fn_804894B8(RwImage *image) {
    switch (image->depth) {
    case 4:
    case 8: {
        RwRGBA *palette = image->palette;
        s32 count = 1 << image->depth;

        if (!palette) {
            RWERROR((0x80000016));
            return NULL;
        }
        ImageGammaCorrectColors(palette, palette, count);
        break;
    }
    case 32: {
        u8 *row = image->cpPixels;
        s32 width = image->width;
        s32 height = image->height;
        s32 y;

        if (!row) {
            RWERROR((0x80000016));
            return NULL;
        }
        for (y = 0; y < height; y++) {
            ImageGammaCorrectColors((RwRGBA *)row, (RwRGBA *)row, width);
            row += image->stride;
        }
        break;
    }
    default:
        RWERROR((0x80000008));
        return NULL;
    }
    image->flags |= 2;
    return image;
}

/* RwImageSetGamma: the gamma table and its inverse. */
RwBool fn_80489830(f32 gammaValue) {
    f32 invGamma = lbl_805FB310 / gammaValue;
    s32 i;

    IMAGEGLOBAL(gammaVal) = gammaValue;
    IMAGEGLOBAL(gammaTable)[0] = 0;
    IMAGEGLOBAL(invGammaTable)[0] = 0;
    for (i = 1; i < 256; i++) {
        f32 value = (f32)i / lbl_805FB314;

        f32 scaled;

        scaled = lbl_805FB314 * (f32)pow(value, invGamma);
        IMAGEGLOBAL(gammaTable)[i] = lbl_805FB318 + scaled;
        scaled = lbl_805FB314 * (f32)pow(value, gammaValue);
        IMAGEGLOBAL(invGammaTable)[i] = lbl_805FB318 + scaled;
    }
    return TRUE;
}

/* RwImageSetFromRaster. */
RwImage *fn_804899A8(RwImage *image, RwRaster *raster) {
    if (RwEngineInstance->stdFunc[6](image, raster, 0)) {
        if (raster->privateFlags & 1) {
            image->flags |= 2;
        }
        return image;
    }
    return NULL;
}

/* RwRasterSetFromImage. */
RwRaster *fn_80489A1C(RwRaster *raster, RwImage *image) {
    if (RwEngineInstance->stdFunc[7](raster, image, 0)) {
        if (image->flags & 2) {
            raster->privateFlags |= 1;
        }
        return raster;
    }
    return NULL;
}

/* RwImageFindRasterFormat. */
RwImage *fn_80489A90(RwImage *image, s32 rasterType, s32 *width, s32 *height, s32 *depth,
                     s32 *format) {
    RwRaster raster;

    if (!RwEngineInstance->stdFunc[9](&raster, image, rasterType)) {
        return NULL;
    }
    *format = (raster.cFormat << 8) | raster.cType;
    *width = raster.width;
    *height = raster.height;
    *depth = raster.depth;
    return image;
}

/* RwRasterGetOffset. */
RwRaster *fn_80489B2C(RwRaster *raster, s16 *xOffset, s16 *yOffset) {
    *xOffset = raster->nOffsetX;
    *yOffset = raster->nOffsetY;
    return raster;
}

/* A raster request through the device's standard function 16. */
RwRaster *fn_80489B40(RwRaster *raster) {
    RwEngineInstance->stdFunc[16](NULL, raster, 0);
    return raster;
}

/* RwRasterUnlock: the device's standard function 24, then the lock bits go. */
RwRaster *fn_80489B88(RwRaster *raster) {
    RwEngineInstance->stdFunc[24](NULL, raster, 0);
    raster->privateFlags &= ~0x18;
    return raster;
}

/* RwRasterDestroy. */
RwBool fn_80489BDC(RwRaster *raster) {
    fn_80482144(&lbl_8056E018, raster);
    RwEngineInstance->stdFunc[5](NULL, raster, 0);
    RwEngineInstance->memoryFree(RASTERGLOBAL(rasterFreeList), raster);
    return TRUE;
}

/* RwRasterRegisterPlugin. */
s32 fn_80489C54(s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return fn_80481DC8(&lbl_8056E018, size, pluginID, constructCB, destructCB, copyCB);
}

/* A raster query through the device's standard function 23. */
s32 fn_80489C94(RwRaster *raster, s32 value) {
    s32 result;

    if (RwEngineInstance->stdFunc[23](&result, raster, value)) {
        return result;
    }
    return 0;
}

/* RwRasterGetNumLevels. */
s32 fn_80489CE4(RwRaster *raster) {
    s32 numLevels;

    if (!((raster->cFormat << 8) & 0x8000)) {
        return 1;
    }
    if (RwEngineInstance->stdFunc[28](&numLevels, raster, 0)) {
        return numLevels;
    }
    return -1;
}

/* RwRasterShowRaster. */
RwRaster *fn_80489D44(RwRaster *raster, void *device, u32 flags) {
    RwStandardFunc showRaster = RwEngineInstance->stdFunc[20];

    fn_804807CC();
    if (showRaster(raster, device, flags)) {
        return raster;
    }
    return NULL;
}

/* RwRasterSubRaster. */
RwRaster *fn_80489DC4(RwRaster *raster, RwRaster *parent, RwRect *rect) {
    if (!(raster->cFlags & 0x80)) {
        return NULL;
    }
    raster->width = rect->w;
    raster->height = rect->h;
    raster->nOffsetX = parent->nOffsetX + (s16)rect->x;
    raster->nOffsetY = parent->nOffsetY + (s16)rect->y;
    if (RwEngineInstance->stdFunc[12](raster, parent, 0)) {
        raster->parent = parent->parent;
        return raster;
    }
    return NULL;
}

/* RwRasterCreate. */
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags) {
    RwRaster *raster =
        (RwRaster *)RwEngineInstance->memoryAlloc(RASTERGLOBAL(rasterFreeList), 0x30407);

    if (raster) {
        RwStandardFunc createRaster = RwEngineInstance->stdFunc[4];

        raster->cFlags = raster->privateFlags = 0;
        raster->width = width;
        raster->height = height;
        raster->nOffsetX = 0;
        raster->nOffsetY = 0;
        raster->depth = depth;
        raster->parent = raster;
        raster->cpPixels = NULL;
        raster->palette = NULL;
        if (!createRaster(NULL, raster, flags)) {
            RwEngineInstance->memoryFree(RASTERGLOBAL(rasterFreeList), raster);
            return NULL;
        }
        fn_8048209C(&lbl_8056E018, raster);
        return raster;
    }
    return NULL;
}

/* RwRasterLock. */
u8 *fn_80489F70(RwRaster *raster, u8 level, s32 lockMode) {
    u8 *pixels;

    if (RwEngineInstance->stdFunc[15](&pixels, raster, lockMode + (level << 8))) {
        return pixels;
    }
    return NULL;
}

/* _rwRasterClose. */
void *fn_80489FC0(void *instance, s32 offset, s32 size) {
    if (RASTERGLOBAL(rasterFreeList)) {
        fn_8047FCDC(RASTERGLOBAL(rasterFreeList));
        RASTERGLOBAL(rasterFreeList) = NULL;
    }
    lbl_805F15C8.numInstances--;
    return instance;
}

/* _rwRasterOpen: the context stack starts with an empty raster. */
void *fn_8048A02C(void *instance, s32 offset, s32 size) {
    lbl_805F15C8.globalsOffset = offset;
    memset(&RASTERGLOBAL(dummyRaster), 0, sizeof(RwRaster));
    RASTERGLOBAL(dummyRaster).width = 0;
    RASTERGLOBAL(dummyRaster).height = 0;
    RASTERGLOBAL(dummyRaster).depth = 0;
    RASTERGLOBAL(dummyRaster).cFlags = 0x80;
    RASTERGLOBAL(dummyRaster).cpPixels = NULL;
    RASTERGLOBAL(dummyRaster).palette = NULL;
    RASTERGLOBAL(dummyRaster).cType = 0;
    RASTERGLOBAL(rasterSP) = 0;
    RASTERGLOBAL(rasterStack)[0] = &RASTERGLOBAL(dummyRaster);
    RASTERGLOBAL(rasterFreeList) = fn_8047FC04(lbl_8056E018.sizeOfStruct, lbl_805EDEF8, 4,
                                               lbl_805EDEFC, lbl_805E2CE8, 0x40407);
    if (!RASTERGLOBAL(rasterFreeList)) {
        return NULL;
    }
    lbl_805F15C8.numInstances++;
    return instance;
}

#define RGBAREALFROMPIXEL(o, p)                                                                    \
    do {                                                                                           \
        (o).red = lbl_805FB32C * (f32)(p)[0];                                                      \
        (o).green = lbl_805FB32C * (f32)(p)[1];                                                    \
        (o).blue = lbl_805FB32C * (f32)(p)[2];                                                     \
        (o).alpha = lbl_805FB32C * (f32)(p)[3];                                                    \
    } while (0)

#define RGBAREALSCALE(o, s)                                                                        \
    do {                                                                                           \
        (o).red *= (s);                                                                            \
        (o).green *= (s);                                                                          \
        (o).blue *= (s);                                                                           \
        (o).alpha *= (s);                                                                          \
    } while (0)

#define RGBAREALADD(o, a)                                                                          \
    do {                                                                                           \
        (o).red = (o).red + (a).red;                                                               \
        (o).green = (o).green + (a).green;                                                         \
        (o).blue = (o).blue + (a).blue;                                                            \
        (o).alpha = (o).alpha + (a).alpha;                                                         \
    } while (0)

/* The mean colour of one row of a 32-bit image between two 16.16 x positions, end pixels
 * weighted by how much of them the span covers. */
void fn_8048A178(const RwImage *image, s32 x0, s32 x1, s32 y, RwRGBAReal *out) {
    f32 width = (f32)(x1 - x0) * lbl_805FB328;
    s32 first = x0 >> 16;
    s32 last = x1 >> 16;
    const u8 *pixel = image->cpPixels + (y >> 16) * image->stride + first * 4;

    if (first == last) {
        RGBAREALFROMPIXEL(*out, pixel);
        RGBAREALSCALE(*out, width);
    } else {
        s32 pos = (first + 1) << 16;
        f32 weight = (f32)(pos - x0) * lbl_805FB328;
        RwRGBAReal color;
        s32 i;

        RGBAREALFROMPIXEL(*out, pixel);
        RGBAREALSCALE(*out, weight);
        pixel += 4;
        for (i = pos >> 16; i != last; i++) {
            RGBAREALFROMPIXEL(color, pixel);
            RGBAREALADD(*out, color);
            pixel += 4;
            pos += 0x10000;
        }
        weight = (f32)(x1 - pos) * lbl_805FB328;
        RGBAREALFROMPIXEL(color, pixel);
        RGBAREALSCALE(color, weight);
        RGBAREALADD(*out, color);
    }
    width = lbl_805FB330 / width;
    RGBAREALSCALE(*out, width);
}

/* The mean colour of a block of rows between two 16.16 y positions. */
void fn_8048A50C(const RwImage *image, s32 x0, s32 x1, s32 y0, s32 y1, RwRGBAReal *out) {
    f32 height = (f32)(y1 - y0) * lbl_805FB328;
    s32 first = y0 >> 16;
    s32 last = y1 >> 16;

    if (first == last) {
        fn_8048A178(image, x0, x1, y0, out);
        RGBAREALSCALE(*out, height);
    } else {
        s32 pos = (first + 1) << 16;
        RwRGBAReal row;
        f32 weight;

        fn_8048A178(image, x0, x1, y0, out);
        weight = (f32)(pos - y0) * lbl_805FB328;
        RGBAREALSCALE(*out, weight);
        for (first = pos >> 16; first != last; first++) {
            fn_8048A178(image, x0, x1, pos, &row);
            pos += 0x10000;
            out->red = row.red + out->red;
            out->green = row.green + out->green;
            out->blue = row.blue + out->blue;
            out->alpha = row.alpha + out->alpha;
        }
        fn_8048A178(image, x0, x1, pos, &row);
        weight = (f32)(y1 - pos) * lbl_805FB328;
        RGBAREALSCALE(row, weight);
        RGBAREALADD(*out, row);
    }
    height = lbl_805FB330 / height;
    RGBAREALSCALE(*out, height);
}

/* RwImageResample: each destination pixel is the mean of the source area it covers. */
RwImage *fn_8048A790(RwImage *destImage, const RwImage *sourceImage) {
    s32 destWidth = destImage->width;
    s32 destHeight = destImage->height;
    s32 srcY = 0;
    s32 xStep;
    s32 yStep;
    s32 y;
    RwRGBAReal color;

    destImage->flags |= sourceImage->flags & 2;
    xStep = lbl_805FB348 * ((f32)sourceImage->width / (f32)destWidth);
    yStep = lbl_805FB348 * ((f32)sourceImage->height / (f32)destHeight);
    for (y = 0; y < destHeight; y++) {
        s32 srcYEnd = srcY + (yStep - 1);
        u8 *pixel = destImage->cpPixels + destImage->stride * y;
        s32 srcX = 0;
        s32 x;

        for (x = 0; x < destWidth; x++) {
            fn_8048A50C(sourceImage, srcX, srcX + (xStep - 1), srcY, srcYEnd, &color);
            srcX += xStep;
            pixel[0] = lbl_805FB350 * color.red + lbl_805FB34C;
            pixel[1] = lbl_805FB350 * color.green + lbl_805FB34C;
            pixel[2] = lbl_805FB350 * color.blue + lbl_805FB34C;
            pixel[3] = lbl_805FB350 * color.alpha + lbl_805FB34C;
            pixel += 4;
        }
        srcY += yStep;
    }
    return destImage;
}

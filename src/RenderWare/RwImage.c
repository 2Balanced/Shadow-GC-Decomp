/* RenderWare core: the image module (0x80487914-0x80489B2C).
 * Module open and close (the image and image format free lists, the image path and the scratch
 * buffer), RwImageCreate, RwImageDestroy, RwImageAllocatePixels, RwImageFreePixels, RwImageMakeMask,
 * RwImageApplyMask, the scratch buffer, RwImageSetPath, the path search and its two callbacks,
 * RwImageFindFileType, the image read, RwImageReadMaskedImage, RwRGBASetFromPixel, the depth
 * conversion, RwImageCopy, RwImageGammaCorrect, RwImageSetGamma, then the image/raster
 * conversions RwImageSetFromRaster, RwRasterSetFromImage and RwImageFindRasterFormat. The file owns
 * the module globals (.sbss), the free list parameters (.sdata), the two free lists (.bss), the
 * image plugin registry (.data) and its constant pool (.sdata2 0x805FB310-0x805FB328: the image
 * open's 1.0f and the gamma tables' constants share it, so the gamma code is this file's).
 * Boundary: the raster module from 0x80489B2C has its own globals; the image/raster conversions
 * between them use no data and stay with the images they convert.
 * NonMatching, every opcode right but registers (probes/Game_RwCpp49.py, Game_RwCpp50.py):
 * fn_80487FA8 (volatile register numbering of the two loops), fn_80488088 (the temporary image
 * and the width trade r30/r31; retail also shifts the mask's palette index with slwi where GC/2.7
 * masks the byte with rlwinm), fn_804886CC (the directory length and the buffer size trade
 * r27/r28), fn_80488B94 (the image and the mask name trade r30/r31), fn_8048915C and fn_804893B8
 * (register numbering) and fn_804894B8 (retail unrolls the gamma loop by four, GC/2.7 by two).
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

double pow(double x, double y);
RwMatrix *fn_8047EFC8(RwMatrix *matrix, const RwV3d *axis, f32 angle, s32 combine);
RwBool fn_8047A67C(const char *name);
RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        RwFreeList *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
const RwPluginRegistry *fn_8048209C(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_80482144(const RwPluginRegistry *reg, void *object);
RwBool fn_80483444(const char *path);
RwBool fn_80489830(f32 gamma);
RwImage *fn_804893B8(RwImage *dst, const RwImage *src);

static RwModuleInfo imageModule;
static RwPluginRegistry imageTKList = {sizeof(RwImage), sizeof(RwImage), 0, 0, NULL, NULL};
static s32 imageFreeListBlockSize = 0x80;
static s32 imageFreeListPreallocBlocks = 1;
static s32 imageFormatFreeListPreallocBlocks = 1;
static RwFreeList imageFreeList;
static RwFreeList imageFormatFreeList;

#define IMAGEGLOBAL(field) (RWGLOBALAT(rwImageGlobals, imageModule.globalsOffset).field)

/* _rwImageOpen: the image and image format free lists, the image path and the scratch buffer. */
void *fn_80487914(void *instance, s32 offset, s32 size) {
    imageModule.globalsOffset = offset;
    IMAGEGLOBAL(imageFreeList) = fn_8047FC04(imageTKList.sizeOfStruct, imageFreeListBlockSize, 4,
                                             imageFreeListPreallocBlocks, &imageFreeList, 0x40018);
    if (!IMAGEGLOBAL(imageFreeList)) {
        return NULL;
    }
    IMAGEGLOBAL(imageFormatFreeList) = fn_8047FC04(sizeof(rwImageFormat), imageFreeListBlockSize, 4,
                                                   imageFormatFreeListPreallocBlocks, &imageFormatFreeList, 0x40406);
    if (!IMAGEGLOBAL(imageFormatFreeList)) {
        fn_8047FCDC(IMAGEGLOBAL(imageFreeList));
        IMAGEGLOBAL(imageFreeList) = NULL;
        return NULL;
    }
    IMAGEGLOBAL(imagePathSize) = 256;
    IMAGEGLOBAL(imagePath) =
        (char *)RwEngineInstance->memoryFuncs.rwmalloc(IMAGEGLOBAL(imagePathSize), 0x1040406);
    if (!IMAGEGLOBAL(imagePath)) {
        fn_8047FCDC(IMAGEGLOBAL(imageFormatFreeList));
        IMAGEGLOBAL(imageFormatFreeList) = NULL;
        fn_8047FCDC(IMAGEGLOBAL(imageFreeList));
        IMAGEGLOBAL(imageFreeList) = NULL;
        return NULL;
    }
    IMAGEGLOBAL(imagePath)[0] = '\0';
    imageModule.numInstances++;
    fn_80489830(1.0f);
    IMAGEGLOBAL(imageFormats) = NULL;
    IMAGEGLOBAL(scratchMemSize) = 256;
    IMAGEGLOBAL(scratchMem) =
        (char *)RwEngineInstance->memoryFuncs.rwmalloc(IMAGEGLOBAL(scratchMemSize), 0x1040018);
    if (!IMAGEGLOBAL(scratchMem)) {
        RwEngineInstance->memoryFuncs.rwfree(IMAGEGLOBAL(imagePath));
        IMAGEGLOBAL(imagePath) = NULL;
        IMAGEGLOBAL(imagePathSize) = 0;
        fn_8047FCDC(IMAGEGLOBAL(imageFormatFreeList));
        IMAGEGLOBAL(imageFormatFreeList) = NULL;
        fn_8047FCDC(IMAGEGLOBAL(imageFreeList));
        IMAGEGLOBAL(imageFreeList) = NULL;
        return NULL;
    }
    return instance;
}

/* _rwImageClose. */
void *fn_80487BB0(void *instance, s32 offset, s32 size) {
    if (IMAGEGLOBAL(scratchMem)) {
        RwEngineInstance->memoryFuncs.rwfree(IMAGEGLOBAL(scratchMem));
        IMAGEGLOBAL(scratchMem) = NULL;
        IMAGEGLOBAL(scratchMemSize) = 0;
    }
    if (IMAGEGLOBAL(imagePath)) {
        RwEngineInstance->memoryFuncs.rwfree(IMAGEGLOBAL(imagePath));
        IMAGEGLOBAL(imagePath) = NULL;
        IMAGEGLOBAL(imagePathSize) = 0;
    }
    while (IMAGEGLOBAL(imageFormats)) {
        rwImageFormat *format = IMAGEGLOBAL(imageFormats);

        IMAGEGLOBAL(imageFormats) = format->next;
        RwEngineInstance->memoryFree(IMAGEGLOBAL(imageFormatFreeList), format);
    }
    if (IMAGEGLOBAL(imageFormatFreeList)) {
        fn_8047FCDC(IMAGEGLOBAL(imageFormatFreeList));
        IMAGEGLOBAL(imageFormatFreeList) = NULL;
    }
    if (IMAGEGLOBAL(imageFreeList)) {
        fn_8047FCDC(IMAGEGLOBAL(imageFreeList));
        IMAGEGLOBAL(imageFreeList) = NULL;
    }
    imageModule.numInstances--;
    return instance;
}

/* RwImageCreate: no pixels yet. */
RwImage *fn_80487D08(s32 width, s32 height, s32 depth) {
    RwImage *image = (RwImage *)RwEngineInstance->memoryAlloc(IMAGEGLOBAL(imageFreeList), 0x30018);

    if (!image) {
        return NULL;
    }
    image->width = width;
    image->height = height;
    image->depth = depth;
    image->cpPixels = NULL;
    image->palette = NULL;
    image->flags = 0;
    fn_8048209C(&imageTKList, image);
    return image;
}

RwImage *fn_80487F50(RwImage *image);

/* RwImageDestroy. */
RwBool fn_80487DB0(RwImage *image) {
    if (image->flags & 1) {
        fn_80487F50(image);
    }
    fn_80482144(&imageTKList, image);
    RwEngineInstance->memoryFree(IMAGEGLOBAL(imageFreeList), image);
    return TRUE;
}

/* RwImageAllocatePixels: rows padded to four bytes, the palette after the pixels. */
RwImage *fn_80487E18(RwImage *image) {
    RwBool palettised = FALSE;
    s32 paletteSize;
    s32 pixelSize;
    s32 size;

    if ((u32)image->depth == 4 || (u32)image->depth == 8) {
        palettised = TRUE;
    }
    paletteSize = palettised ? (1 << image->depth) * (s32)sizeof(RwRGBA) : 0;
    image->stride = (image->depth + 7) >> 3;
    image->stride = image->stride * image->width;
    image->stride = (image->stride + 3) & ~3;
    pixelSize = image->stride * image->height;
    size = pixelSize + paletteSize;
    image->cpPixels = (u8 *)RwEngineInstance->memoryFuncs.rwmalloc(size, 0x30018);
    if (!image->cpPixels) {
        RWERROR((0x80000013, size));
        return NULL;
    }
    image->palette = palettised ? (RwRGBA *)(image->cpPixels + pixelSize) : NULL;
    image->flags |= 1;
    return image;
}

/* RwImageFreePixels. */
RwImage *fn_80487F50(RwImage *image) {
    RwEngineInstance->memoryFuncs.rwfree(image->cpPixels);
    image->cpPixels = NULL;
    image->palette = NULL;
    image->flags &= ~1;
    return image;
}

/* RwImageMakeMask: each colour's alpha becomes the largest of its red, green and blue. */
RwImage *fn_80487FA8(RwImage *image) {
    switch (image->depth) {
    case 4:
    case 8: {
        RwRGBA *color = image->palette;
        s32 count = 1 << image->depth;
        s32 i;

        for (i = 0; i < count; i++) {
            s32 max = color->red;

            if (color->green > max) {
                max = color->green;
            }
            if (color->blue > max) {
                max = color->blue;
            }
            color->alpha = max;
            color++;
        }
        break;
    }
    case 32: {
        u8 *row = image->cpPixels;
        s32 y;

        for (y = 0; y < image->height; y++) {
            u8 *pixel = row;
            s32 x;

            for (x = 0; x < image->width; x++) {
                s32 max = pixel[0];

                if (pixel[1] > max) {
                    max = pixel[1];
                }
                if (pixel[2] > max) {
                    max = pixel[2];
                }
                pixel[3] = max;
                pixel += 4;
            }
            row += image->stride;
        }
        break;
    }
    }
    return image;
}

/* RwImageApplyMask: a palettised image becomes 32-bit first, then takes the mask's alpha. */
RwImage *fn_80488088(RwImage *image, const RwImage *mask) {
    if (image->width != mask->width || image->height != mask->height) {
        RWERROR((0x8000000A));
        return NULL;
    }
    switch (image->depth) {
    case 4:
    case 8: {
        RwImage *temp = fn_80487D08(image->width, image->height, image->depth);

        if (!temp) {
            return NULL;
        }
        if (!fn_80487E18(temp)) {
            fn_80487DB0(temp);
            return NULL;
        }
        fn_804893B8(temp, image);
        if (image->flags & 1) {
            fn_80487F50(image);
        }
        image->depth = 32;
        fn_80487E18(image);
        fn_804893B8(image, temp);
        fn_80487F50(temp);
        fn_80487DB0(temp);
    }
    case 32: {
        const u8 *maskRow = mask->cpPixels;
        const RwRGBA *maskPalette = mask->palette;
        u8 *imageRow = image->cpPixels;
        s32 maskDepth = mask->depth;
        s32 maskStride = mask->stride;
        s32 y;

        for (y = 0; y < image->height; y++) {
            u8 *dst = imageRow;

            switch (maskDepth) {
            case 4:
            case 8: {
                const u8 *src = maskRow;
                s32 x;

                for (x = 0; x < image->width; x++) {
                    dst[3] = maskPalette[*src++].alpha;
                    dst += 4;
                }
                break;
            }
            case 32: {
                const u8 *src = maskRow;
                s32 x;

                for (x = 0; x < image->width; x++) {
                    dst[3] = src[3];
                    src += 4;
                    dst += 4;
                }
                break;
            }
            }
            maskRow += maskStride;
            imageRow += image->stride;
        }
        break;
    }
    default:
        RWERROR((0x80000009));
        return NULL;
    }
    return image;
}

/* The scratch buffer of the image module, grown to at least size bytes. */
static inline char *ImageScratch(s32 size) {
    if (size > IMAGEGLOBAL(scratchMemSize)) {
        char *mem;

        if (IMAGEGLOBAL(scratchMem)) {
            mem = (char *)RwEngineInstance->memoryFuncs.rwrealloc(IMAGEGLOBAL(scratchMem), size,
                                                                  0x1040018);
        } else {
            mem = (char *)RwEngineInstance->memoryFuncs.rwmalloc(size, 0x1040018);
        }
        if (!mem) {
            RWERROR((0x80000013, size));
            return NULL;
        }
        IMAGEGLOBAL(scratchMem) = mem;
        IMAGEGLOBAL(scratchMemSize) = size;
    }
    return IMAGEGLOBAL(scratchMem);
}

/* RwImageSetPath. */
const char *fn_804885DC(const char *path) {
    s32 size = RWSTRLEN(path) + 1;

    if (size > IMAGEGLOBAL(imagePathSize)) {
        char *newPath = (char *)RwEngineInstance->memoryFuncs.rwrealloc(IMAGEGLOBAL(imagePath),
                                                                         size, 0x1040406);

        if (!newPath) {
            RWERROR((0x80000013, size));
            return NULL;
        }
        IMAGEGLOBAL(imagePath) = newPath;
        IMAGEGLOBAL(imagePathSize) = size;
    }
    memcpy(IMAGEGLOBAL(imagePath), path, size);
    return path;
}

/* Retail calls the path search out of line everywhere: it is kept out of auto inlining. */
#pragma auto_inline off
/* Calls the callback with the name prefixed by each directory of the image path in turn (or
 * once with the name alone when it is absolute or there is no path) until it returns NULL. */
const char *fn_804886CC(const char *name, s32 extraSize, const char *(*callback)(char *, void *),
                        void *data) {
    char *path = IMAGEGLOBAL(imagePath);

    if (fn_80483444(name) || !path || !*path) {
        char *buffer = ImageScratch(RWSTRLEN(name) + extraSize);

        if (!buffer) {
            return NULL;
        }
        RWSTRCPY(buffer, name);
        callback(buffer, data);
    } else {
        while (path && *path) {
            char *next;
            s32 length;
            char *buffer;

            RWSTRCHR(path, ';');
            next = RWSTRCHR(path, ';');
            if (next) {
                length = next - path;
                next++;
            } else {
                length = RWSTRLEN(path);
            }
            buffer = ImageScratch(length + (RWSTRLEN(name) + extraSize));
            if (!buffer) {
                return NULL;
            }
            memcpy(buffer, path, length);
            RWSTRCPY(buffer + length, name);
            if (!callback(buffer, data)) {
                return name;
            }
            path = next;
        }
    }
    return name;
}

#pragma auto_inline on

typedef struct rwImageReadData {
    RwImage *(*imageRead)(const char *name);
    RwImage *image;
} rwImageReadData;

/* The path search callback of an image read: reads the first file that exists. */
const char *fn_80488A04(const char *path, rwImageReadData *data) {
    if (fn_8047A67C(path)) {
        data->image = data->imageRead(path);
        if (data->image) {
            return NULL;
        }
    }
    return path;
}

/* The path search callback that tries each registered extension, lower then upper case. */
const char *fn_80488A70(char *path, const char **extension) {
    rwImageFormat *format;
    char *end = path + RWSTRLEN(path);

    for (format = IMAGEGLOBAL(imageFormats); format; format = format->next) {
        RWSTRCPY(end, format->lowerExtension);
        if (fn_8047A67C(path)) {
            *extension = format->lowerExtension;
            return NULL;
        }
        RWSTRCPY(end, format->upperExtension);
        if (fn_8047A67C(path)) {
            *extension = format->upperExtension;
            return NULL;
        }
    }
    return path;
}

/* RwImageFindFileType: the extension of the first file found for the name. */
const char *fn_80488B58(const char *name) {
    const char *extension = NULL;

    fn_804886CC(name, 20, (const char *(*)(char *, void *))fn_80488A70, &extension);
    return extension;
}

/* RwImageRead without the plugin step: the format from the name's extension, the file from the
 * image path. */
static inline RwImage *ImageReadFile(const char *name) {
    const char *start;
    const char *found;
    const char *extension;
    rwImageFormat *format;

    found = RWSTRRCHR(name, ':');
    if (found) {
        start = found;
    } else {
        start = name;
    }
    found = RWSTRRCHR(start, '/');
    if (found) {
        start = found;
    }
    found = RWSTRRCHR(start, '\\');
    extension = RWSTRRCHR(found ? found : start, '.');
    if (extension) {
        for (format = IMAGEGLOBAL(imageFormats); format; format = format->next) {
            if (!RWSTRCMP(format->lowerExtension, extension) ||
                !RWSTRCMP(format->upperExtension, extension)) {
                if (format->imageRead) {
                    rwImageReadData data;

                    data.imageRead = format->imageRead;
                    data.image = NULL;
                    fn_804886CC(name, 5, (const char *(*)(char *, void *))fn_80488A04, &data);
                    return data.image;
                }
                return NULL;
            }
        }
        return NULL;
    }
    return NULL;
}

/* RwImageReadMaskedImage. */
RwImage *fn_80488B94(const char *imageName, const char *maskName) {
    RwImage *image = ImageReadFile(imageName);

    if (image) {
        if (maskName && *maskName) {
            RwImage *mask = ImageReadFile(maskName);

            if (!mask) {
                fn_80487DB0(image);
                return NULL;
            }
            if (!fn_80487FA8(mask)) {
                fn_80487DB0(image);
                fn_80487DB0(mask);
                return NULL;
            }
            if (!fn_80488088(image, mask)) {
                fn_80487DB0(image);
                fn_80487DB0(mask);
                return NULL;
            }
            fn_80487DB0(mask);
        }
        return image;
    }
    return NULL;
}

/* A raster call through the device's standard function 3. */
RwRaster *fn_80489118(RwRaster *raster, s32 value, s32 flags) {
    RwEngineInstance->stdFunc[3](raster, &value, flags);
    return raster;
}

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
    f32 invGamma = 1.0f / gammaValue;
    s32 i;

    IMAGEGLOBAL(gammaVal) = gammaValue;
    IMAGEGLOBAL(gammaTable)[0] = 0;
    IMAGEGLOBAL(invGammaTable)[0] = 0;
    for (i = 1; i < 256; i++) {
        f32 value = (f32)i / 255.0f;

        f32 scaled;

        scaled = 255.0f * (f32)pow(value, invGamma);
        IMAGEGLOBAL(gammaTable)[i] = 0.5f + scaled;
        scaled = 255.0f * (f32)pow(value, gammaValue);
        IMAGEGLOBAL(invGammaTable)[i] = 0.5f + scaled;
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

/* RenderWare core: the texture module (0x8048B6AC-0x8048D7C0).
 * The default mipmap name callback, palettising the images of a texture (one image, or a set of
 * mipmap levels with one palette), reading a texture's image resampled to its raster, the default
 * texture read callback (with and without mipmap levels), the default mipmap builder and find
 * callback, the mipmapping switches, RwTextureSetRaster, texture dictionaries (create, destroy,
 * walks, add, find, current), RwTextureCreate/Destroy, the names, RwTextureGenerateMipmapName,
 * RwTextureRead, RwTextureRegisterPlugin, RwTextureRasterGenerateMipmaps, module close and open.
 * The file owns the default dictionary and the module globals (.sbss), the empty mask name, the
 * missing mask name and the free list parameters (.sdata), the two free lists (.bss), the texture
 * and dictionary plugin registries (.data), the hex digits of the mipmap names (.rodata) and the
 * palettising weight (.sdata2 0x805FB358).
 * Functions come out in definition order and are inlined into the functions after them:
 * RwTexDictionaryDestroy calls RwTexDictionaryForAllTextures (defined after it) out of line, while
 * the module close inlines both, so the destroy callback is called directly there.
 * NonMatching: fn_8048CD8C (RwTextureDestroy), one instruction: retail loads the TRUE it returns
 * into r3 before the reference count and again after the free; eight spellings of the return
 * tried.
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        RwFreeList *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
s32 fn_80481DC8(RwPluginRegistry *reg, s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB);
const RwPluginRegistry *fn_8048209C(const RwPluginRegistry *reg, void *object);
const RwPluginRegistry *fn_80482144(const RwPluginRegistry *reg, void *object);
RwImage *fn_80487D08(s32 width, s32 height, s32 depth);
RwBool fn_80487DB0(RwImage *image);
RwImage *fn_80487E18(RwImage *image);
const char *fn_80488B58(const char *name);
RwImage *fn_80488B94(const char *imageName, const char *maskName);
RwImage *fn_804893B8(RwImage *dst, const RwImage *src);
RwImage *fn_804894B8(RwImage *image);
RwImage *fn_804899A8(RwImage *image, RwRaster *raster);
RwRaster *fn_80489A1C(RwRaster *raster, RwImage *image);
RwImage *fn_80489A90(RwImage *image, s32 rasterType, s32 *width, s32 *height, s32 *depth,
                     s32 *format);
RwRaster *fn_80489B40(RwRaster *raster);
RwBool fn_80489BDC(RwRaster *raster);
s32 fn_80489CE4(RwRaster *raster);
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);
u8 *fn_80489F70(RwRaster *raster, u8 level, s32 lockMode);
RwImage *fn_8048A790(RwImage *destImage, const RwImage *sourceImage);
RwImage *fn_8048A96C(const RwImage *srcImage, s32 width, s32 height);
void fn_8048DEB0(rwPalQuant *pq, RwImage *image, f32 weight);
s32 fn_8048F5D0(RwRGBA *palette, s32 numColors, rwPalQuant *pq);
void fn_8048FAB0(u8 *pixels, s32 stride, s32 depth, RwBool pack, rwPalQuant *pq,
                 const RwImage *image);
RwBool fn_8048FED8(rwPalQuant *pq);
void fn_804903F8(rwPalQuant *pq);

RwTexture *fn_8048CCDC(RwRaster *raster);
RwBool fn_8048CD8C(RwTexture *texture);
RwTexture *fn_8048CE54(RwTexture *texture, const char *name);
RwTexture *fn_8048CF00(RwTexture *texture, const char *maskName);
RwTexture *fn_8048CFFC(RwTexDictionary *dict, const char *name);
RwBool fn_8048D0EC(char *name, char *maskName, u8 mipLevel, s32 format);

static RwModuleInfo textureModule;
static RwTexDictionary *defaultTexDict;
RwPluginRegistry textureTKList = {sizeof(RwTexture), sizeof(RwTexture), 0, 0, NULL, NULL};
RwPluginRegistry texDictTKList = {sizeof(RwTexDictionary), sizeof(RwTexDictionary), 0, 0, NULL,
                                  NULL};

static const char hexDigits[] = "0123456789abcdef";

#define TEXTUREGLOBAL(field) (RWGLOBALAT(rwTextureGlobals, textureModule.globalsOffset).field)

/* A copy of a name into a buffer of size bytes, cut short with an error when it does not fit. */
#define TEXTURENAMECOPY(dst, src, size)                                                            \
    do {                                                                                           \
        ((char *(*)(char *, const char *, u32))RwEngineInstance->stringFuncs.vecStrncpy)(          \
            (dst), (src), (size));                                                                 \
        if (RWSTRLEN(src) >= (size)) {                                                             \
            RWERROR((0x8000001E, (src), (size), (size) - 1, (src)[(size) - 1]));                    \
            (dst)[(size) - 1] = '\0';                                                              \
        }                                                                                          \
    } while (0)

#define RWSTRCAT(d, s) (((char *(*)(char *, const char *))RwEngineInstance->stringFuncs.vecStrcat)((d), (s)))

/* The default mipmap name callback: level n of a name gets "m" and the level's hex digit. */
RwBool fn_8048B6AC(char *name, char *maskName, u8 mipLevel, s32 format) {
    char suffix[3];
    RwBool valid;

    suffix[0] = 'm';
    valid = FALSE;
    if (mipLevel && mipLevel < 16) {
        valid = TRUE;
    }
    suffix[1] = valid ? hexDigits[mipLevel] : 0;
    suffix[2] = '\0';
    if (suffix[1]) {
        RWSTRCAT(name, suffix);
        if (maskName && maskName[0]) {
            RWSTRCAT(maskName, suffix);
        }
    }
    return TRUE;
}

/* Converts a 32-bit image to a palettised one of the given depth with its own palette. */
RwBool fn_8048B774(RwImage **image, s32 depth) {
    RwRGBA palette[256];
    rwPalQuant palQuant;
    s32 numColors;
    RwImage *palImage;

    if (!fn_8048FED8(&palQuant)) {
        return FALSE;
    }
    fn_8048DEB0(&palQuant, *image, 1.0f);
    numColors = 1 << depth;
    fn_8048F5D0(palette, numColors, &palQuant);
    palImage = fn_80487D08((*image)->width, (*image)->height, depth);
    if (palImage) {
        fn_80487E18(palImage);
        fn_8048FAB0(palImage->cpPixels, palImage->stride, palImage->depth, FALSE, &palQuant,
                    *image);
        memcpy(palImage->palette, palette, numColors * sizeof(RwRGBA));
        fn_80487DB0(*image);
        *image = palImage;
    } else {
        return FALSE;
    }
    fn_804903F8(&palQuant);
    return TRUE;
}

/* Gives a set of images (mipmap levels) one shared palette of the given depth: the first image's
 * palette when every image already has the same one, a quantised one otherwise. The images are
 * replaced; the one passed as exclude is not destroyed. */
RwBool fn_8048B860(RwRGBA *palette, RwImage *exclude, RwImage **images, s32 numImages, s32 depth) {
    rwPalQuant palQuant;
    RwRGBA *firstPalette = images[0]->palette;
    s32 numColors;
    s32 i;

    if (firstPalette) {
        numColors = 1 << depth;

        for (i = 1; i < numImages; i++) {
            RwRGBA *otherPalette = images[i]->palette;
            s32 j;

            if (!firstPalette || !otherPalette) {
                i = 64;
                break;
            }
            for (j = 0; j < numColors; j++) {
                if (*(u32 *)&firstPalette[j] != *(u32 *)&otherPalette[j]) {
                    i = 64;
                    break;
                }
            }
        }
        if (i == numImages) {
            memcpy(palette, firstPalette, (1 << images[0]->depth) * sizeof(RwRGBA));
            return TRUE;
        }
    }
    if (!fn_8048FED8(&palQuant)) {
        return FALSE;
    }
    for (i = 0; i < numImages; i++) {
        fn_8048DEB0(&palQuant, images[i], 1.0f);
    }
    fn_8048F5D0(palette, 1 << depth, &palQuant);
    for (i = 0; i < numImages; i++) {
        RwImage *image = images[i];
        RwImage *palImage = fn_80487D08(image->width, image->height, depth);

        if (palImage) {
            fn_80487E18(palImage);
            fn_8048FAB0(palImage->cpPixels, palImage->stride, palImage->depth, FALSE, &palQuant,
                        image);
            palImage->palette = palette;
            images[i] = palImage;
            if (image != exclude) {
                fn_80487DB0(image);
            }
        } else {
            return FALSE;
        }
    }
    fn_804903F8(&palQuant);
    return TRUE;
}

/* Reads the image (and mask) of a texture and brings it to the raster's size: the raster format
 * is looked up first when the size is not known yet, and a palettised image is palettised again
 * after resampling. */
RwImage *fn_8048BA30(const char *name, const char *maskName, s32 rasterFlags, s32 *width,
                     s32 *height, s32 *depth, s32 *format) {
    char imageName[256];
    char maskImageName[256];
    RwImage *image;
    const char *extension;

    TEXTURENAMECOPY(imageName, name, 256);
    extension = fn_80488B58(name);
    if (extension) {
        RWSTRCAT(imageName, extension);
    }
    maskImageName[0] = '\0';
    if (maskName && maskName[0]) {
        TEXTURENAMECOPY(maskImageName, maskName, 256);
        extension = fn_80488B58(maskName);
        if (extension) {
            RWSTRCAT(maskImageName, extension);
        }
    }
    image = fn_80488B94(imageName, maskImageName);
    if (!image) {
        return NULL;
    }
    if (!*width || !*height) {
        if (!fn_80489A90(image, rasterFlags, width, height, depth, format)) {
            fn_80487DB0(image);
            RWERROR((0x80000009));
            return NULL;
        }
    }
    if (image->width != *width || image->height != *height) {
        s32 imageDepth = image->depth;
        RwImage *resampled;

        if (imageDepth != 32) {
            RwImage *sourceImage = image;

            image = fn_80487D08(sourceImage->width, sourceImage->height, 32);
            if (!image) {
                fn_80487DB0(sourceImage);
                return NULL;
            }
            if (!fn_80487E18(image)) {
                fn_80487DB0(image);
                fn_80487DB0(sourceImage);
                return NULL;
            }
            fn_804893B8(image, sourceImage);
            fn_80487DB0(sourceImage);
        }
        resampled = fn_80487D08(*width, *height, 32);
        if (!resampled) {
            fn_80487DB0(image);
            return NULL;
        }
        if (!fn_80487E18(resampled)) {
            fn_80487DB0(resampled);
            fn_80487DB0(image);
            return NULL;
        }
        fn_8048A790(resampled, image);
        fn_80487DB0(image);
        image = resampled;
        if (imageDepth == 4) {
            fn_8048B774(&image, imageDepth);
        } else if (imageDepth == 8) {
            fn_8048B774(&image, imageDepth);
        }
    }
    return image;
}

/* The default texture read without mipmap levels. */
RwTexture *fn_8048BD84(const char *name, const char *maskName) {
    char imageName[256];
    char maskImageName[256];
    RwRGBA palette[256];
    RwImage *image;
    s32 width, height, depth, format;
    RwTexture *texture;
    RwRaster *raster;

    TEXTURENAMECOPY(imageName, name, 256);
    maskImageName[0] = '\0';
    if (maskName && maskName[0]) {
        TEXTURENAMECOPY(maskImageName, maskName, 256);
    }
    fn_8048D0EC(imageName, maskImageName, 0, 4);
    width = 0;
    height = 0;
    image = fn_8048BA30(imageName, maskImageName, 4, &width, &height, &depth, &format);
    if (!image) {
        return NULL;
    }
    raster = fn_80489E74(width, height, depth, format);
    if (!raster) {
        fn_80487DB0(image);
        return NULL;
    }
    if (((raster->cFormat & 0xFF) << 8) & 0x6000) {
        if (((raster->cFormat & 0xFF) << 8) & 0x4000) {
            fn_8048B860(palette, NULL, &image, 1, 4);
        } else {
            fn_8048B860(palette, NULL, &image, 1, 8);
        }
        image->palette = palette;
    }
    fn_804894B8(image);
    if (!fn_80489A1C(raster, image)) {
        fn_80489BDC(raster);
        fn_80487DB0(image);
        return NULL;
    }
    fn_80487DB0(image);
    texture = fn_8048CCDC(raster);
    if (!texture) {
        fn_80489BDC(raster);
        return NULL;
    }
    fn_8048CE54(texture, name);
    if (maskName) {
        fn_8048CF00(texture, maskName);
    } else {
        fn_8048CF00(texture, "");
    }
    return texture;
}

/* The default texture read with mipmap levels: each level's image from its own name, or levels
 * the device builds itself. */
RwTexture *fn_8048C038(const char *name, const char *maskName) {
    char imageName[256];
    char maskImageName[256];
    RwRGBA palette[256];
    RwImage *images[16];
    s32 width, height, depth, format;
    RwRaster *raster;
    s32 rasterFlags;
    s32 i;
    s32 numLevels;
    RwTexture *texture;

    TEXTURENAMECOPY(imageName, name, 256);
    maskImageName[0] = '\0';
    if (maskName && maskName[0]) {
        TEXTURENAMECOPY(maskImageName, maskName, 256);
    }
    rasterFlags = 4;
    if (TEXTUREGLOBAL(haveTexMipmaps)) {
        rasterFlags |= 0x8000;
        if (TEXTUREGLOBAL(generateTexMipmaps)) {
            rasterFlags |= 0x1000;
        }
    }
    fn_8048D0EC(imageName, maskImageName, 0, rasterFlags);
    width = 0;
    height = 0;
    images[0] = fn_8048BA30(imageName, maskImageName, rasterFlags, &width, &height, &depth, &format);
    if (!images[0]) {
        return NULL;
    }
    raster = fn_80489E74(width, height, depth, format);
    if (!raster) {
        fn_80487DB0(images[0]);
        return NULL;
    }
    if (format & 0x8000) {
        if (format & 0x1000) {
            if (!fn_80489A1C(raster, images[0])) {
                fn_80489BDC(raster);
                fn_80487DB0(images[0]);
                return NULL;
            }
            fn_80487DB0(images[0]);
        } else {
            numLevels = fn_80489CE4(raster);
            for (i = 1; i < numLevels; i++) {
                TEXTURENAMECOPY(imageName, name, 256);
                maskImageName[0] = '\0';
                if (maskName && maskName[0]) {
                    TEXTURENAMECOPY(maskImageName, maskName, 256);
                }
                fn_8048D0EC(imageName, maskImageName, (u8)i, rasterFlags);
                fn_80489F70(raster, (u8)i, 5);
                width = raster->width;
                height = raster->height;
                depth = raster->depth;
                format = (raster->cFormat << 8) | raster->cType;
                fn_80489B40(raster);
                images[i] = fn_8048BA30(imageName, maskImageName, rasterFlags, &width, &height,
                                        &depth, &format);
                if (!images[i]) {
                    while (--i >= 0) {
                        fn_80487DB0(images[i]);
                    }
                    fn_80489BDC(raster);
                    return NULL;
                }
            }
            if (((raster->cFormat & 0xFF) << 8) & 0x6000) {
                if (((raster->cFormat & 0xFF) << 8) & 0x4000) {
                    fn_8048B860(palette, NULL, images, numLevels, 4);
                } else {
                    fn_8048B860(palette, NULL, images, numLevels, 8);
                }
                fn_804894B8(images[0]);
            } else {
                for (i = 0; i < numLevels; i++) {
                    fn_804894B8(images[i]);
                }
            }
            for (i = 0; i < numLevels; i++) {
                if (fn_80489F70(raster, (u8)i, 5)) {
                    if (!fn_80489A1C(raster, images[i])) {
                        for (; i < numLevels; i++) {
                            fn_80487DB0(images[i]);
                        }
                        fn_80489BDC(raster);
                        return NULL;
                    }
                    fn_80489B40(raster);
                }
                fn_80487DB0(images[i]);
            }
        }
    } else {
        fn_804894B8(images[0]);
        if (!fn_80489A1C(raster, images[0])) {
            fn_80489BDC(raster);
            fn_80487DB0(images[0]);
            return NULL;
        }
        fn_80487DB0(images[0]);
    }
    texture = fn_8048CCDC(raster);
    if (!texture) {
        fn_80489BDC(raster);
        return NULL;
    }
    fn_8048CE54(texture, name);
    if (maskName) {
        fn_8048CF00(texture, maskName);
    } else {
        fn_8048CF00(texture, "");
    }
    return texture;
}

/* The default texture read callback. */
RwTexture *fn_8048C5F4(const char *name, const char *maskName) {
    if (TEXTUREGLOBAL(haveTexMipmaps)) {
        return fn_8048C038(name, maskName);
    }
    return fn_8048BD84(name, maskName);
}

/* The default mipmap builder: each level resampled from the one above it. */
RwRaster *fn_8048C634(RwRaster *raster, RwImage *image) {
    RwImage *images[16];
    RwRGBA palette[256];
    u8 autoMipmap;
    s32 i;
    s32 numLevels;
    s32 width = raster->width;
    s32 height = raster->height;

    if (!image) {
        images[0] = fn_80487D08(width, height, 32);
        if (images[0]) {
            if (!fn_80487E18(images[0])) {
                return NULL;
            }
            fn_804899A8(images[0], raster);
        }
    } else if (image->depth != 32) {
        images[0] = fn_80487D08(width, height, 32);
        if (images[0]) {
            if (!fn_80487E18(images[0])) {
                return NULL;
            }
            fn_804893B8(images[0], image);
        }
    } else {
        images[0] = image;
    }
    if (!images[0]) {
        return NULL;
    }
    autoMipmap = raster->cFormat & 0x10;
    raster->cFormat &= ~autoMipmap;
    numLevels = fn_80489CE4(raster);
    for (i = 1; i < numLevels; i++) {
        images[i] = NULL;
        if (fn_80489F70(raster, (u8)i, 2)) {
            images[i] = fn_8048A96C(images[i - 1], raster->width, raster->height);
            fn_80489B40(raster);
        }
        if (!images[i]) {
            while (--i >= 0) {
                if (images[i] != image) {
                    fn_80487DB0(images[i]);
                }
            }
            raster->cFormat |= autoMipmap;
            return NULL;
        }
    }
    if (((raster->cFormat & 0xFF) << 8) & 0x6000) {
        if (((raster->cFormat & 0xFF) << 8) & 0x4000) {
            if (!fn_8048B860(palette, image, images, numLevels, 4)) {
                for (i = 0; i < numLevels; i++) {
                    if (images[i] != image) {
                        fn_80487DB0(images[i]);
                    }
                    raster->cFormat |= autoMipmap;
                    return NULL;
                }
            }
        } else {
            if (!fn_8048B860(palette, image, images, numLevels, 8)) {
                for (i = 0; i < numLevels; i++) {
                    if (images[i] != image) {
                        fn_80487DB0(images[i]);
                    }
                    raster->cFormat |= autoMipmap;
                    return NULL;
                }
            }
        }
        fn_804894B8(images[0]);
    } else {
        for (i = 0; i < numLevels; i++) {
            fn_804894B8(images[i]);
        }
    }
    for (i = 0; i < numLevels; i++) {
        if (fn_80489F70(raster, (u8)i, 5)) {
            fn_80489A1C(raster, images[i]);
            fn_80489B40(raster);
        }
        if (images[i] != image) {
            fn_80487DB0(images[i]);
        }
    }
    raster->cFormat |= autoMipmap;
    return raster;
}

/* The default texture find callback: the current dictionary, or every dictionary. */
RwTexture *fn_8048C91C(const char *name) {
    rwTextureGlobals *globals = &RWGLOBALAT(rwTextureGlobals, textureModule.globalsOffset);
    RwLLLink *cur;

    if (globals->currentTexDict) {
        return fn_8048CFFC(globals->currentTexDict, name);
    }
    cur = globals->texDictList.link.next;
    while (cur != &globals->texDictList.link) {
        RwTexture *texture = fn_8048CFFC((RwTexDictionary *)((u8 *)cur - 0x10), name);

        if (texture) {
            return texture;
        }
        cur = cur->next;
    }
    return NULL;
}

/* RwTextureSetMipmapping. */
RwBool fn_8048C9A8(RwBool enable) {
    TEXTUREGLOBAL(haveTexMipmaps) = enable;
    return TRUE;
}

/* RwTextureGetMipmapping. */
RwBool fn_8048C9C0(void) {
    return TEXTUREGLOBAL(haveTexMipmaps);
}

/* RwTextureSetAutoMipmapping. */
RwBool fn_8048C9D4(RwBool enable) {
    TEXTUREGLOBAL(generateTexMipmaps) = enable;
    return TRUE;
}

/* RwTextureGetAutoMipmapping. */
RwBool fn_8048C9EC(void) {
    return TEXTUREGLOBAL(generateTexMipmaps);
}

/* RwTextureSetRaster: the device's texture-set-raster function, or no raster. */
RwTexture *fn_8048CA00(RwTexture *texture, RwRaster *raster) {
    if (raster) {
        if (RwEngineInstance->stdFunc[8](texture, raster, 0)) {
            return texture;
        }
        return NULL;
    }
    texture->raster = NULL;
    return texture;
}

/* RwTexDictionaryCreate. */
RwTexDictionary *fn_8048CA64(void) {
    RwTexDictionary *dict =
        (RwTexDictionary *)RwEngineInstance->memoryAlloc(TEXTUREGLOBAL(texDictFreeList), 0x30016);

    if (!dict) {
        return NULL;
    }
    dict->object.type = 6;
    dict->object.subType = 0;
    dict->object.flags = 0;
    dict->object.privateFlags = 0;
    dict->object.parent = NULL;
    dict->lInInstance.next = TEXTUREGLOBAL(texDictList).link.next;
    dict->lInInstance.prev = &TEXTUREGLOBAL(texDictList).link;
    TEXTUREGLOBAL(texDictList).link.next->prev = &dict->lInInstance;
    TEXTUREGLOBAL(texDictList).link.next = &dict->lInInstance;
    dict->texturesInDict.link.next = &dict->texturesInDict.link;
    dict->texturesInDict.link.prev = &dict->texturesInDict.link;
    fn_8048209C(&texDictTKList, dict);
    return dict;
}

const RwTexDictionary *fn_8048CBE0(const RwTexDictionary *dict,
                                   RwTexture *(*callBack)(RwTexture *texture, void *data),
                                   void *data);

/* RwTexDictionaryDestroy: its textures are destroyed with it. */
RwBool fn_8048CB38(RwTexDictionary *dict) {
    if (TEXTUREGLOBAL(currentTexDict) == dict) {
        TEXTUREGLOBAL(currentTexDict) = NULL;
    }
    fn_8048CBE0(dict, (RwTexture *(*)(RwTexture *, void *))fn_8048CD8C, NULL);
    fn_80482144(&texDictTKList, dict);
    dict->lInInstance.prev->next = dict->lInInstance.next;
    dict->lInInstance.next->prev = dict->lInInstance.prev;
    RwEngineInstance->memoryFree(TEXTUREGLOBAL(texDictFreeList), dict);
    return TRUE;
}

/* RwTexDictionaryForAllTextures. */
const RwTexDictionary *fn_8048CBE0(const RwTexDictionary *dict,
                                   RwTexture *(*callBack)(RwTexture *texture, void *data),
                                   void *data) {
    RwLLLink *next;
    RwLLLink *cur = dict->texturesInDict.link.next;
    const RwLLLink *end = &dict->texturesInDict.link;

    while (cur != end) {
        next = cur->next;

        if (!callBack((RwTexture *)((u8 *)cur - 8), data)) {
            break;
        }
        cur = next;
    }
    return dict;
}

/* RwTexDictionaryForAllTexDictionaries. */
RwBool fn_8048CC54(RwTexDictionary *(*callBack)(RwTexDictionary *dict, void *data), void *data) {
    rwTextureGlobals *globals = &RWGLOBALAT(rwTextureGlobals, textureModule.globalsOffset);
    RwLLLink *cur = globals->texDictList.link.next;

    while (cur != &globals->texDictList.link) {
        RwLLLink *next = cur->next;

        if (!callBack((RwTexDictionary *)((u8 *)cur - 0x10), data)) {
            break;
        }
        cur = next;
    }
    return TRUE;
}

/* RwTextureCreate: wrapping addressing, nearest filtering. */
RwTexture *fn_8048CCDC(RwRaster *raster) {
    RwTexture *texture =
        (RwTexture *)RwEngineInstance->memoryAlloc(TEXTUREGLOBAL(textureFreeList), 0x30006);

    if (texture) {
        texture->dict = NULL;
        texture->name[0] = '\0';
        texture->mask[0] = '\0';
        texture->raster = raster;
        texture->refCount = 1;
        texture->filterAddressing = 0;
        texture->filterAddressing = (texture->filterAddressing & ~0xFF00) | 0x1100;
        texture->filterAddressing = (texture->filterAddressing & ~0xFF) | 1;
        fn_8048209C(&textureTKList, texture);
    }
    return texture;
}

/* RwTextureDestroy: only the last reference destroys it. */
RwBool fn_8048CD8C(RwTexture *texture) {
    texture->refCount--;
    if (texture->refCount <= 0) {
        texture->refCount++;
        fn_80482144(&textureTKList, texture);
        if (texture->dict) {
            texture->lInDictionary.prev->next = texture->lInDictionary.next;
            texture->lInDictionary.next->prev = texture->lInDictionary.prev;
        }
        if (texture->raster) {
            fn_80489BDC(texture->raster);
            texture->raster = NULL;
        }
        texture->refCount--;
        RwEngineInstance->memoryFree(TEXTUREGLOBAL(textureFreeList), texture);
    }
    return TRUE;
}

/* RwTextureSetName. */
RwTexture *fn_8048CE54(RwTexture *texture, const char *name) {
    TEXTURENAMECOPY(texture->name, name, 32);
    return texture;
}

/* RwTextureSetMaskName. */
RwTexture *fn_8048CF00(RwTexture *texture, const char *maskName) {
    TEXTURENAMECOPY(texture->mask, maskName, 32);
    return texture;
}

/* RwTexDictionaryAddTexture. */
RwTexture *fn_8048CFAC(RwTexDictionary *dict, RwTexture *texture) {
    if (texture->dict) {
        texture->lInDictionary.prev->next = texture->lInDictionary.next;
        texture->lInDictionary.next->prev = texture->lInDictionary.prev;
    }
    texture->dict = dict;
    texture->lInDictionary.next = dict->texturesInDict.link.next;
    texture->lInDictionary.prev = &dict->texturesInDict.link;
    dict->texturesInDict.link.next->prev = &texture->lInDictionary;
    dict->texturesInDict.link.next = &texture->lInDictionary;
    return texture;
}

/* Texture names compare without regard to case (the caller checks the name first). */
static RwBool TextureNameMatch(const char *a, const char *b) {
    while (*a && *b) {
        char ca = *a;
        char cb = *b;

        if (ca >= 'a' && ca <= 'z') {
            ca -= 'a' - 'A';
        }
        if (cb >= 'a' && cb <= 'z') {
            cb -= 'a' - 'A';
        }
        if (ca != cb) {
            return FALSE;
        }
        a++;
        b++;
    }
    if (*a == *b) {
        return TRUE;
    }
    return FALSE;
}

/* RwTexDictionaryFindNamedTexture. */
RwTexture *fn_8048CFFC(RwTexDictionary *dict, const char *name) {
    RwLLLink *cur = dict->texturesInDict.link.next;
    RwLLLink *end = &dict->texturesInDict.link;

    while (cur != end) {
        RwTexture *texture = (RwTexture *)((u8 *)cur - 8);

        if (texture->name && TextureNameMatch(texture->name, name)) {
            return texture;
        }
        cur = cur->next;
    }
    return NULL;
}

/* RwTexDictionarySetCurrent. */
RwTexDictionary *fn_8048D0C4(RwTexDictionary *dict) {
    TEXTUREGLOBAL(currentTexDict) = dict;
    return dict;
}

/* RwTexDictionaryGetCurrent. */
RwTexDictionary *fn_8048D0D8(void) {
    return TEXTUREGLOBAL(currentTexDict);
}

/* RwTextureGenerateMipmapName. */
RwBool fn_8048D0EC(char *name, char *maskName, u8 mipLevel, s32 format) {
    if (TEXTUREGLOBAL(mipmapNameCallBack)) {
        return TEXTUREGLOBAL(mipmapNameCallBack)(name, maskName, mipLevel, format);
    }
    return FALSE;
}

/* RwTextureRead: from the dictionaries first, then through the read callback into the current
 * dictionary. */
RwTexture *fn_8048D130(const char *name, const char *maskName) {
    RwTexture *texture = TEXTUREGLOBAL(textureFindCallBack)(name);

    if (texture) {
        texture->refCount++;
        return texture;
    }
    texture = TEXTUREGLOBAL(textureReadCallBack)(name, maskName);
    if (!texture) {
        if (maskName) {
            RWERROR((0x16, name, maskName));
        } else {
            RWERROR((0x16, name, "(null)"));
        }
        return NULL;
    }
    if (TEXTUREGLOBAL(currentTexDict)) {
        fn_8048CFAC(TEXTUREGLOBAL(currentTexDict), texture);
    }
    return texture;
}

/* RwTextureRegisterPlugin. */
s32 fn_8048D280(s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    return fn_80481DC8(&textureTKList, size, pluginID, constructCB, destructCB, copyCB);
}

/* RwTextureRasterGenerateMipmaps. */
RwBool fn_8048D2C0(RwRaster *raster, RwImage *image) {
    return TEXTUREGLOBAL(mipmapGenerationCallBack)(raster, image) != NULL;
}

static s32 textureFreeListBlockSize = 0x80;
static s32 textureFreeListPreallocBlocks = 1;
static s32 texDictFreeListBlockSize = 5;
static s32 texDictFreeListPreallocBlocks = 1;
static RwFreeList textureFreeList;
static RwFreeList texDictFreeList;

/* _rwTextureClose: the default dictionary goes with the module. */
void *fn_8048D300(void *instance, s32 offset, s32 size) {
    rwTextureGlobals *globals;

    if (TEXTUREGLOBAL(scratchMem)) {
        RwEngineInstance->memoryFuncs.rwfree(TEXTUREGLOBAL(scratchMem));
        TEXTUREGLOBAL(scratchMem) = NULL;
        TEXTUREGLOBAL(scratchMemSize) = 0;
    }
    globals = &RWGLOBALAT(rwTextureGlobals, textureModule.globalsOffset);
    if (globals->textureFreeList && globals->texDictFreeList) {
        RwLLLink *cur = globals->texDictList.link.next;

        while (cur != &globals->texDictList.link) {
            RwTexDictionary *curDict = (RwTexDictionary *)((u8 *)cur - 0x10);

            cur = cur->next;
            if (curDict == defaultTexDict) {
                fn_8048CB38(defaultTexDict);
                defaultTexDict = NULL;
                break;
            }
        }
    }
    if (TEXTUREGLOBAL(textureFreeList)) {
        fn_8047FCDC(TEXTUREGLOBAL(textureFreeList));
        TEXTUREGLOBAL(textureFreeList) = NULL;
    }
    if (TEXTUREGLOBAL(texDictFreeList)) {
        fn_8047FCDC(TEXTUREGLOBAL(texDictFreeList));
        TEXTUREGLOBAL(texDictFreeList) = NULL;
    }
    textureModule.numInstances--;
    return instance;
}

/* _rwTextureOpen: the free lists, the default dictionary and the default callbacks. */
void *fn_8048D4DC(void *instance, s32 offset, s32 size) {
    textureModule.globalsOffset = offset;
    TEXTUREGLOBAL(textureFreeList) =
        fn_8047FC04(textureTKList.sizeOfStruct, textureFreeListBlockSize, 4,
                    textureFreeListPreallocBlocks, &textureFreeList, 0x40006);
    if (!TEXTUREGLOBAL(textureFreeList)) {
        return NULL;
    }
    TEXTUREGLOBAL(texDictFreeList) =
        fn_8047FC04(texDictTKList.sizeOfStruct, texDictFreeListBlockSize, 4,
                    texDictFreeListPreallocBlocks, &texDictFreeList, 0x40408);
    if (!TEXTUREGLOBAL(texDictFreeList)) {
        fn_8047FCDC(TEXTUREGLOBAL(textureFreeList));
        TEXTUREGLOBAL(textureFreeList) = NULL;
        return NULL;
    }
    TEXTUREGLOBAL(texDictList).link.next = &TEXTUREGLOBAL(texDictList).link;
    TEXTUREGLOBAL(texDictList).link.prev = &TEXTUREGLOBAL(texDictList).link;
    textureModule.numInstances++;
    TEXTUREGLOBAL(currentTexDict) = defaultTexDict = fn_8048CA64();
    if (!TEXTUREGLOBAL(currentTexDict)) {
        fn_8047FCDC(TEXTUREGLOBAL(texDictFreeList));
        TEXTUREGLOBAL(texDictFreeList) = NULL;
        fn_8047FCDC(TEXTUREGLOBAL(textureFreeList));
        TEXTUREGLOBAL(textureFreeList) = NULL;
        return NULL;
    }
    TEXTUREGLOBAL(haveTexMipmaps) = FALSE;
    TEXTUREGLOBAL(generateTexMipmaps) = FALSE;
    TEXTUREGLOBAL(textureFindCallBack) = fn_8048C91C;
    TEXTUREGLOBAL(textureReadCallBack) = fn_8048C5F4;
    TEXTUREGLOBAL(mipmapGenerationCallBack) = fn_8048C634;
    TEXTUREGLOBAL(mipmapNameCallBack) = fn_8048B6AC;
    TEXTUREGLOBAL(scratchMem) = NULL;
    TEXTUREGLOBAL(scratchMemSize) = 0;
    return instance;
}

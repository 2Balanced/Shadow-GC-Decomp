/* RenderWare material effects plugin (RpMatFX, plugin id 0x120), platform-independent part,
 * 0x804A076C..0x804A2C90: engine open/close, the material, atomic and world sector plugin
 * callbacks, RpMatFXPluginAttach, the bump map texture builder and the RpMatFXMaterial accessors.
 * Data: .rodata 0x8051C3C0..0x8051C400 (the empty initialisers of two texture names), .data
 * 0x8056ED00..0x8056ED28 (the module record, then the jump table of RpMatFXMaterialSetEffects),
 * .bss 0x805E32A0 (in-place space of the material data free list), .sdata 0x805EDF60 (the free
 * list's block size and preallocation, which RpMatFXSetFreeListCreateParams would set), .sbss
 * 0x805F16A8..0x805F16B8 (the plugin offsets in RpMaterial, RpAtomic and RpWorldSector) and
 * .sdata2 0x805FB438..0x805FB450.
 * Built with cflags_rw_r5 (C, -O4,p, no lmw/stmw, automatic inlining in definition order): the
 * texture stream helpers fn_804A0C50, fn_804A0CDC and fn_804A0DB8 are called from the multi-texture
 * plugin and also expanded in the stream callbacks defined after them.
 * Functions keep their fn_ symbols; each comment gives the public API name the behaviour matches.
 * The bump texture name is built by a static function that the compiler expands in fn_804A1A08 and
 * fn_804A2304 (its loop leaves retail's two dead branches at the end of fn_804A2304); its own copy
 * is never called, and the linker drops it.
 * Open: fn_804A09D0 (retail keeps the effect offset in r0 and the switch value in r3; 7 rows) and
 * fn_804A1A08 (saved registers permuted). */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

/* RwFreeList, as RwFreeListCreateAndPreallocateSpace keeps it in the space it is given. */
typedef struct MatFXFreeList {
    u32 entrySize;
    u32 entriesPerBlock;
    u32 heapSize;
    u32 alignment;
    void *blockList[2];
    u32 flags;
    void *link[2];
} MatFXFreeList;

/* The plugin's module record: +8 is the free list of MatFXMaterialData. */
static MatFXInfo MatFXModule = {0, 0, NULL};

/* RpMatFXSetFreeListCreateParams' values. */
static s32 MatFXFreeListBlockSize = 128;
static s32 MatFXFreeListPreallocBlocks = 1;
static MatFXFreeList MatFXFreeListSpace;

/* The plugin data offsets (small uninitialised data comes out in reverse order of definition). */
s32 MatFXWorldSectorDataOffset;
s32 MatFXAtomicDataOffset;
s32 MatFXMaterialDataOffset;

static inline MatFXEffectData *MatFXFindEffect(MatFXMaterialData *data, s32 type) {
    u8 i;
    for (i = 0; i < 2; i++) {
        if (data->data[i].flag == type) return &data->data[i];
    }
    return NULL;
}

static inline MatFXMaterialData *MatFXMaterialDataCreate(RpMaterial *material) {
    MatFXMaterialData *data = MATFXMATERIALGETDATA(material);
    if (data == NULL) {
        data = (MatFXMaterialData *)RwEngineInstance->freeListAlloc(MatFXModule.materialDataFreeList,
                                                                    0x30120);
        if (data == NULL) return NULL;
        memset(data, 0, sizeof(MatFXMaterialData));
        MATFXMATERIALGETDATA(material) = data;
    }
    return data;
}
RxNodeDefinition *fn_804A0760(void);
RpMaterial *fn_804A20D0(RpMaterial *material, s32 flags);
RpMaterial *fn_804A25E0(RpMaterial *material, RwFrame *frame);
RpMaterial *fn_804A2628(RpMaterial *material, f32 coef);
RwFrame *fn_804A26BC(const RpMaterial *material);
f32 fn_804A2704(const RpMaterial *material);
RpMaterial *fn_804A2750(RpMaterial *material, RwTexture *texture);
RpMaterial *fn_804A27F8(RpMaterial *material, RwFrame *frame);
RpMaterial *fn_804A2840(RpMaterial *material, s32 useFrameBufferAlpha);
RpMaterial *fn_804A2888(RpMaterial *material, f32 coef);
RwTexture *fn_804A28D0(const RpMaterial *material);
RwFrame *fn_804A2918(const RpMaterial *material);
s32 fn_804A2960(const RpMaterial *material);
f32 fn_804A29A8(const RpMaterial *material);
RpMaterial *fn_804A29F0(RpMaterial *material, RwTexture *texture);
RpMaterial *fn_804A2AB0(RpMaterial *material, s32 srcBlendMode, s32 dstBlendMode);
RwTexture *fn_804A2B44(const RpMaterial *material);
RpMaterial *fn_804A2B8C(RpMaterial *material, s32 *srcBlendMode, s32 *dstBlendMode);
RpMaterial *fn_804A2BE0(RpMaterial *material, RwMatrix *baseTransform, RwMatrix *dualTransform);
RpMaterial *fn_804A2C2C(RpMaterial *material, RwMatrix **baseTransform, RwMatrix **dualTransform);
s32 fn_804A5494(void);
void fn_804A5524(void);
void *fn_804A076C(void *, s32, s32);
void *fn_804A07DC(void *, s32, s32);
void *fn_804A0884(void *, s32, s32);
void *fn_804A0894(void *, s32, s32);
void *fn_804A09D0(void *, const void *, s32, s32);
RpMaterial *fn_804A20D0(RpMaterial *material, s32 flags);
RpMaterial *fn_804A2304(RpMaterial *material, RwTexture *texture);
RpMaterial *fn_804A2628(RpMaterial *material, f32 coef);
RpMaterial *fn_804A2750(RpMaterial *material, RwTexture *texture);
RpMaterial *fn_804A2840(RpMaterial *material, s32 useFrameBufferAlpha);
RpMaterial *fn_804A2888(RpMaterial *material, f32 coef);
RpMaterial *fn_804A29F0(RpMaterial *material, RwTexture *texture);
RpMaterial *fn_804A2AB0(RpMaterial *material, s32 srcBlendMode, s32 dstBlendMode);
void *fn_804A2004(void *atomic);
void *fn_804A2070(void *sector);
void *fn_804A558C(void *atomic);
void *fn_804A5598(void *sector);
RwTexture *fn_804A55AC(RwTexture *baseTexture, RwTexture *heightTexture);
s32 fn_804A6C74(void);


/* Engine close callback. */
void *fn_804A076C(void *instance, s32 offset, s32 size) {
    if (--MatFXModule.numInstances == 0) {
        fn_804A5524();
    }
    if (MatFXModule.materialDataFreeList != NULL) {
        fn_8047FCDC(MatFXModule.materialDataFreeList);
        MatFXModule.materialDataFreeList = NULL;
    }
    return instance;
}

/* Engine open callback. */
void *fn_804A07DC(void *instance, s32 offset, s32 size) {
    if (MatFXModule.numInstances == 0) {
        MatFXModule.materialDataFreeList = fn_8047FC04(
            sizeof(MatFXMaterialData), MatFXFreeListBlockSize, 4, MatFXFreeListPreallocBlocks, &MatFXFreeListSpace, 0x40120);
        if (MatFXModule.materialDataFreeList == NULL) return NULL;
        if (!fn_804A5494()) return NULL;
    }
    MatFXModule.numInstances++;
    return instance;
}

/* Material constructor. */
void *fn_804A0884(void *object, s32 offset, s32 size) {
    MATFXMATERIALGETDATA(object) = NULL;
    return object;
}

/* Material destructor. */
void *fn_804A0894(void *object, s32 offset, s32 size) {
    u8 i;
    MatFXEffectData *effect;
    MatFXMaterialData *data;

    data = MATFXMATERIALGETDATA(object);
    if (data != NULL) {
        for (i = 0; i < 2; i++) {
            effect = &data->data[i];
            switch (effect->flag) {
            case rpMATFXEFFECTBUMPMAP:
                if (effect->u.bump.bumpTexture != NULL) fn_8048CD8C(effect->u.bump.bumpTexture);
                if (effect->u.bump.texture != NULL) fn_8048CD8C(effect->u.bump.texture);
                break;
            case rpMATFXEFFECTENVMAP:
                if (effect->u.env.texture != NULL) fn_8048CD8C(effect->u.env.texture);
                break;
            case rpMATFXEFFECTDUAL:
                if (effect->u.dual.texture != NULL) fn_8048CD8C(effect->u.dual.texture);
                break;
            case rpMATFXEFFECTUVTRANSFORM:
                effect->u.uv.baseTransform = NULL;
                effect->u.uv.dualTransform = NULL;
                break;
            case rpMATFXEFFECTBUMPENVMAP:
                break;
            }
        }
        memset(data, 0, sizeof(MatFXMaterialData));
        RwEngineInstance->freeListFree(MatFXModule.materialDataFreeList, data);
        MATFXMATERIALGETDATA(object) = NULL;
    }
    return object;
}

/* Material copy. */
void *fn_804A09D0(void *dstObject, const void *srcObject, s32 offset, s32 size) {
    RpMaterial *src = (RpMaterial *)srcObject;
    const MatFXMaterialData *srcData = MATFXMATERIALGETDATA(src);
    RpMaterial *dst = (RpMaterial *)dstObject;
    MatFXMaterialData *dstData;
    u8 i;

    if (srcData == NULL) return NULL;
    dstData = MatFXMaterialDataCreate(dst);
    if (dstData == NULL) return NULL;
    fn_804A20D0(dst, srcData->flags);
    for (i = 0; i < 2; i++) {
        const MatFXEffectData *srcEffect = &srcData->data[i];
        switch (srcEffect->flag) {
        case rpMATFXEFFECTBUMPMAP: {
            MatFXEffectData *dstEffect = &dstData->data[i];
            RwFrame *frame = fn_804A26BC(src);
            f32 coef = fn_804A2704(src);
            fn_804A25E0(dst, frame);
            fn_804A2628(dst, coef);
            dstEffect->u.bump.bumpTexture = srcEffect->u.bump.bumpTexture;
            dstEffect->u.bump.texture = srcEffect->u.bump.texture;
            if (dstEffect->u.bump.bumpTexture != NULL) {
                dstEffect->u.bump.bumpTexture->refCount++;
            }
            if (dstEffect->u.bump.texture != NULL) {
                dstEffect->u.bump.texture->refCount++;
            }
            break;
        }
        case rpMATFXEFFECTENVMAP: {
            RwTexture *texture = fn_804A28D0(src);
            RwFrame *frame = fn_804A2918(src);
            f32 coef = fn_804A29A8(src);
            s32 useFrameBufferAlpha = fn_804A2960(src);
            if (texture != NULL) {
                fn_804A2750(dst, texture);
            }
            fn_804A27F8(dst, frame);
            fn_804A2840(dst, useFrameBufferAlpha);
            fn_804A2888(dst, coef);
            break;
        }
        case rpMATFXEFFECTDUAL: {
            RwTexture *texture = fn_804A2B44(src);
            s32 srcBlendMode;
            s32 dstBlendMode;
            fn_804A2B8C(src, &srcBlendMode, &dstBlendMode);
            if (texture != NULL) {
                fn_804A29F0(dst, texture);
            }
            fn_804A2AB0(dst, srcBlendMode, dstBlendMode);
            break;
        }
        case rpMATFXEFFECTUVTRANSFORM: {
            RwMatrix *baseTransform;
            RwMatrix *dualTransform;
            src = fn_804A2C2C(src, &baseTransform, &dualTransform);
            dst = fn_804A2BE0(dst, baseTransform, dualTransform);
            break;
        }
        case rpMATFXEFFECTBUMPENVMAP:
            break;
        }
    }
    return dstObject;
}

/* Writes "texture present" and the texture chunk. */
RwStream *fn_804A0C50(RwStream *stream, const RwTexture *texture) {
    s32 present = (texture != NULL);
    if (!fn_8047D458(stream, &present, sizeof(present))) return NULL;
    if (present) {
        if (!fn_80483BA0(texture, stream)) return NULL;
    }
    return stream;
}

/* Reads what fn_804A0C50 wrote; a texture that cannot be found is not an error. */
RwStream *fn_804A0CDC(RwStream *stream, RwTexture **texture) {
    s32 present;
    if (!fn_8047D868(stream, &present, sizeof(present))) return NULL;
    if (present) {
        RwError error;
        if (!fn_8047CF50(stream, 6, NULL, NULL)) return NULL;
        fn_8047DBE4(&error);
        *texture = fn_80483D28(stream);
        if (*texture == NULL) {
            fn_8047DBE4(&error);
            if (error.errorCode != (s32)0x80000000 && error.errorCode != 0x16) {
                fn_8047DB84(&error);
                return NULL;
            }
        }
    } else {
        *texture = NULL;
    }
    return stream;
}

/* The stream size of what fn_804A0C50 writes. */
s32 fn_804A0DB8(const RwTexture *texture) {
    s32 size = sizeof(s32);
    if (texture != NULL) {
        size = fn_80483B38(texture) + 0x10;
    }
    return size;
}

/* Material stream write callback. */
RwStream *fn_804A0DEC(RwStream *stream, s32 length, const void *object, s32 offset, s32 size) {
    const MatFXMaterialData *data = MATFXMATERIALGETDATA(object);
    s32 value;
    u8 i;

    value = data->flags;
    if (!fn_8047D458(stream, &value, sizeof(value))) return NULL;
    for (i = 0; i < 2; i++) {
        const MatFXEffectData *effect = &data->data[i];
        s32 type = effect->flag;
        if (!fn_8047D458(stream, &type, sizeof(type))) return NULL;
        switch (type) {
        case rpMATFXEFFECTBUMPMAP: {
            f32 coef = -effect->u.bump.coef;
            if (!fn_8047D350(stream, &coef, sizeof(coef))) return NULL;
            if (!fn_804A0C50(stream, effect->u.bump.bumpTexture)) return NULL;
            if (!fn_804A0C50(stream, effect->u.bump.texture)) return NULL;
            break;
        }
        case rpMATFXEFFECTENVMAP:
            if (!fn_8047D350(stream, &effect->u.env.coef, sizeof(f32))) return NULL;
            value = effect->u.env.useFrameBufferAlpha;
            if (!fn_8047D458(stream, &value, sizeof(value))) return NULL;
            if (!fn_804A0C50(stream, effect->u.env.texture)) return NULL;
            break;
        case rpMATFXEFFECTDUAL:
            value = effect->u.dual.srcBlendMode;
            if (!fn_8047D458(stream, &value, sizeof(value))) return NULL;
            value = effect->u.dual.dstBlendMode;
            if (!fn_8047D458(stream, &value, sizeof(value))) return NULL;
            if (!fn_804A0C50(stream, effect->u.dual.texture)) return NULL;
            break;
        case rpMATFXEFFECTBUMPENVMAP:
        case rpMATFXEFFECTUVTRANSFORM:
            break;
        }
    }
    return stream;
}

/* Material stream read callback. */
RwStream *fn_804A114C(RwStream *stream, s32 length, void *object, s32 offset, s32 size) {
    RpMaterial *material = (RpMaterial *)object;
    MatFXMaterialData *data = MatFXMaterialDataCreate(material);
    s32 flags;
    u8 i;

    if (data == NULL) return NULL;
    if (!fn_8047D868(stream, &flags, sizeof(flags))) return NULL;
    fn_804A20D0(material, flags);
    for (i = 0; i < 2; i++) {
        s32 type;
        if (!fn_8047D868(stream, &type, sizeof(type))) return NULL;
        switch (type) {
        case rpMATFXEFFECTBUMPMAP: {
            RwTexture *bumpTexture = NULL;
            RwTexture *texture = NULL;
            MatFXEffectData *effect = &data->data[i];
            f32 coef;
            if (!fn_8047D784(stream, &coef, sizeof(coef))) return NULL;
            if (!fn_804A0CDC(stream, &bumpTexture)) return NULL;
            if (!fn_804A0CDC(stream, &texture)) {
                if (bumpTexture != NULL) fn_8048CD8C(bumpTexture);
                return NULL;
            }
            if (bumpTexture != NULL) {
                effect->u.bump.bumpTexture = bumpTexture;
                effect->u.bump.texture = texture;
                effect->u.bump.invBumpWidth = 1.0f / (f32)effect->u.bump.bumpTexture->raster->width;
            } else if (texture != NULL) {
                fn_804A2304(material, texture);
                fn_8048CD8C(texture);
            } else {
                effect->u.bump.bumpTexture = NULL;
                effect->u.bump.texture = NULL;
            }
            fn_804A2628(material, coef);
            break;
        }
        case rpMATFXEFFECTENVMAP: {
            RwTexture *texture = NULL;
            f32 coef;
            s32 useFrameBufferAlpha;
            if (!fn_8047D784(stream, &coef, sizeof(coef))) return NULL;
            if (!fn_8047D868(stream, &useFrameBufferAlpha, sizeof(useFrameBufferAlpha))) return NULL;
            if (!fn_804A0CDC(stream, &texture)) return NULL;
            if (texture != NULL) {
                fn_804A2750(material, texture);
                fn_8048CD8C(texture);
            }
            fn_804A2888(material, coef);
            fn_804A2840(material, useFrameBufferAlpha);
            break;
        }
        case rpMATFXEFFECTDUAL: {
            RwTexture *texture = NULL;
            s32 blendModes[2];
            if (!fn_8047D868(stream, blendModes, sizeof(blendModes))) return NULL;
            if (!fn_804A0CDC(stream, &texture)) return NULL;
            if (texture != NULL) {
                fn_804A29F0(material, texture);
                fn_8048CD8C(texture);
            }
            fn_804A2AB0(material, blendModes[0], blendModes[1]);
            break;
        }
        case rpMATFXEFFECTBUMPENVMAP:
        case rpMATFXEFFECTUVTRANSFORM:
            break;
        }
    }
    return stream;
}

/* Material stream size callback. */
s32 fn_804A16D4(const void *object, s32 offset, s32 sizeInObject) {
    const MatFXEffectData *effect;
    const MatFXMaterialData *data = MATFXMATERIALGETDATA(object);
    s32 size;
    u8 i;

    if (data == NULL || data->flags == rpMATFXEFFECTNULL) return 0;
    size = sizeof(s32);
    for (i = 0; i < 2; i++) {
        effect = &data->data[i];
        size += sizeof(s32);
        switch (effect->flag) {
        case rpMATFXEFFECTBUMPMAP:
            size += sizeof(f32);
            size += fn_804A0DB8(effect->u.bump.bumpTexture);
            size += fn_804A0DB8(effect->u.bump.texture);
            break;
        case rpMATFXEFFECTENVMAP:
            size += sizeof(f32) + sizeof(s32);
            size += fn_804A0DB8(effect->u.env.texture);
            break;
        case rpMATFXEFFECTDUAL:
            size += 2 * sizeof(s32);
            size += fn_804A0DB8(effect->u.dual.texture);
            break;
        case rpMATFXEFFECTBUMPENVMAP:
        case rpMATFXEFFECTUVTRANSFORM:
            break;
        }
    }
    return size;
}

/* Atomic plugin data: one flag, "effects enabled". */
void *fn_804A1818(void *object, s32 offset, s32 size) {
    *(s32 *)((u8 *)object + MatFXAtomicDataOffset) = 0;
    return object;
}

void *fn_804A1828(void *object, s32 offset, s32 size) {
    *(s32 *)((u8 *)object + MatFXAtomicDataOffset) = 0;
    return object;
}

void *fn_804A1838(void *dst, const void *src, s32 offset, s32 size) {
    if (*(s32 *)((u8 *)src + MatFXAtomicDataOffset) != 0) {
        *(s32 *)((u8 *)dst + MatFXAtomicDataOffset) = 1;
    }
    return dst;
}

RwStream *fn_804A1854(RwStream *stream, s32 length, const void *object, s32 offset, s32 size) {
    s32 enabled = *(s32 *)((u8 *)object + MatFXAtomicDataOffset);
    return fn_8047D458(stream, &enabled, sizeof(enabled));
}

RwStream *fn_804A1888(RwStream *stream, s32 length, void *object, s32 offset, s32 size) {
    s32 enabled;
    if (!fn_8047D868(stream, &enabled, sizeof(enabled))) return NULL;
    if (enabled) {
        fn_804A2004(object);
    }
    return stream;
}

s32 fn_804A18F0(const void *object, s32 offset, s32 size) {
    return *(s32 *)((u8 *)object + MatFXAtomicDataOffset) == 0 ? 0 : 4;
}

/* World sector plugin data: the same flag. */
void *fn_804A1910(void *object, s32 offset, s32 size) {
    *(s32 *)((u8 *)object + MatFXWorldSectorDataOffset) = 0;
    return object;
}

void *fn_804A1920(void *object, s32 offset, s32 size) {
    *(s32 *)((u8 *)object + MatFXWorldSectorDataOffset) = 0;
    return object;
}

void *fn_804A1930(void *dst, const void *src, s32 offset, s32 size) {
    if (*(s32 *)((u8 *)src + MatFXWorldSectorDataOffset) != 0) {
        *(s32 *)((u8 *)dst + MatFXWorldSectorDataOffset) = 1;
    }
    return dst;
}

RwStream *fn_804A194C(RwStream *stream, s32 length, const void *object, s32 offset, s32 size) {
    s32 enabled = *(s32 *)((u8 *)object + MatFXWorldSectorDataOffset);
    return fn_8047D458(stream, &enabled, sizeof(enabled));
}

RwStream *fn_804A1980(RwStream *stream, s32 length, void *object, s32 offset, s32 size) {
    s32 enabled;
    if (!fn_8047D868(stream, &enabled, sizeof(enabled))) return NULL;
    if (enabled) {
        fn_804A2070(object);
    }
    return stream;
}

s32 fn_804A19E8(const void *object, s32 offset, s32 size) {
    return *(s32 *)((u8 *)object + MatFXWorldSectorDataOffset) == 0 ? 0 : 4;
}

/* The name of a generated bump texture: the characters of the two source names interleaved. */
static void MatFXBumpTextureName(char *name, RwTexture *baseTexture, RwTexture *heightTexture) {
    const char *source[2];
    s32 count = 0;
    char *out = name;
    s32 which;

    if (baseTexture != NULL) {
        source[0] = baseTexture->name;
        source[1] = heightTexture->name;
    } else {
        source[0] = source[1] = heightTexture->name;
    }
    while (count < 30 && (*source[0] != 0 || *source[1] != 0)) {
        for (which = 0; which < 2; which++) {
            if (*source[which] != 0) {
                *out++ = *source[which]++;
                count++;
            }
        }
    }
    *out = 0;
}

/* Builds the bump texture of a base texture and a height map: the height map becomes the alpha
 * channel of a copy of the base texture (white when there is none). */
RwTexture *fn_804A1A08(RwTexture *baseTexture, RwTexture *heightTexture) {
    RwRaster *heightRaster = heightTexture->raster;
    RwRaster *baseRaster = NULL;
    s32 heightWidth = heightRaster->width;
    s32 heightHeight = heightRaster->height;
    s32 width;
    s32 height;
    RwImage *heightImage;
    RwImage *image;
    RwRaster *raster;
    RwTexture *texture;
    s32 rasterWidth;
    s32 rasterHeight;
    s32 rasterDepth;
    s32 rasterFormat;
    s32 format;
    s32 addressing;
    s32 filter;

    heightImage = fn_80487D08(heightWidth, heightHeight, 32);
    fn_80487E18(heightImage);
    fn_804899A8(heightImage, heightRaster);
    if (baseTexture != NULL) {
        baseRaster = baseTexture->raster;
        width = baseRaster->width;
        height = baseRaster->height;
        image = fn_80487D08(width, height, 32);
        fn_80487E18(image);
        fn_804899A8(image, baseRaster);
    } else {
        s32 x;
        s32 y;
        width = heightRaster->width;
        height = heightRaster->height;
        image = fn_80487D08(width, height, 32);
        fn_80487E18(image);
        for (y = 0; y < height; y++) {
            for (x = 0; x < width; x++) {
                *(u32 *)(image->cpPixels + image->stride * y + x * 4) = 0xFFFFFFFF;
            }
        }
    }
    if (width != heightWidth || height != heightHeight) {
        RwImage *resampled = fn_80487D08(width, height, 32);
        fn_80487E18(resampled);
        fn_8048A790(resampled, heightImage);
        fn_80487DB0(heightImage);
        heightImage = resampled;
    }
    fn_80487FA8(heightImage);
    fn_80488088(image, heightImage);
    fn_80489A90(image, 4, &rasterWidth, &rasterHeight, &rasterDepth, &rasterFormat);
    if (baseTexture != NULL) {
        format = (baseRaster->cFormat & 0xFF) << 8;
    } else {
        format = (heightRaster->cFormat & 0xFF) << 8;
    }
    if (format & 0x8000) {
        rasterFormat |= 0x9000;
    }
    raster = fn_80489E74(rasterWidth, rasterHeight, rasterDepth, rasterFormat);
    fn_80489A1C(raster, image);
    texture = fn_8048CCDC(raster);
    if (baseTexture != NULL) {
        u32 u = (baseTexture->filterAddressing >> 8) & 0xF;
        u32 v = (baseTexture->filterAddressing >> 12) & 0xF;
        addressing = (u == v) ? v : 0;
        filter = baseTexture->filterAddressing & 0xFF;
    } else {
        u32 u = (heightTexture->filterAddressing >> 8) & 0xF;
        u32 v = (heightTexture->filterAddressing >> 12) & 0xF;
        addressing = (u == v) ? v : 0;
        filter = heightTexture->filterAddressing & 0xFF;
    }
    texture->filterAddressing = (texture->filterAddressing & 0xFFFF00FF) | ((addressing << 8) & 0xF00) |
                                ((addressing << 12) & 0xF000);
    texture->filterAddressing = (texture->filterAddressing & 0xFFFFFF00) | (filter & 0xFF);
    fn_80487DB0(image);
    fn_80487DB0(heightImage);
    {
        char name[32] = "";
        MatFXBumpTextureName(name, baseTexture, heightTexture);
        fn_8048CE54(texture, name);
    }
    return texture;
}

/* RpMatFXPluginAttach */
s32 fn_804A1E74(void) {
    if (fn_80485D78(0, 0x120, fn_804A07DC, fn_804A076C) < 0) return 0;
    MatFXMaterialDataOffset = fn_8045B2CC(sizeof(MatFXMaterialData *), 0x120, fn_804A0884, fn_804A0894,
                               fn_804A09D0);
    if (MatFXMaterialDataOffset < 0) return 0;
    if (fn_8045B30C(0x120, fn_804A114C, fn_804A0DEC, fn_804A16D4) < 0) return 0;
    MatFXAtomicDataOffset = fn_80458B6C(sizeof(s32), 0x120, fn_804A1818, fn_804A1828, fn_804A1838);
    if (MatFXAtomicDataOffset < 0) return 0;
    if (fn_80458BEC(0x120, fn_804A1888, fn_804A1854, fn_804A18F0) < 0) return 0;
    MatFXWorldSectorDataOffset = fn_8045F2F8(sizeof(s32), 0x120, fn_804A1910, fn_804A1920, fn_804A1930);
    if (MatFXWorldSectorDataOffset < 0) return 0;
    if (fn_8045F338(0x120, fn_804A1980, fn_804A194C, fn_804A19E8) < 0) return 0;
    return fn_804A6C74() != 0;
}

/* RpMatFXAtomicEnableEffects */
void *fn_804A2004(void *atomic) {
    s32 offset = MatFXAtomicDataOffset;
    if (*(s32 *)((u8 *)atomic + offset) == 0) {
        if (fn_804A558C(atomic) == NULL) return NULL;
        *(s32 *)((u8 *)atomic + offset) = 1;
    }
    return atomic;
}

/* RpMatFXAtomicQueryEffects */
s32 fn_804A2064(void *atomic) {
    return *(s32 *)((u8 *)atomic + MatFXAtomicDataOffset);
}

/* RpMatFXWorldSectorEnableEffects */
void *fn_804A2070(void *sector) {
    s32 offset = MatFXWorldSectorDataOffset;
    if (*(s32 *)((u8 *)sector + offset) == 0) {
        if (fn_804A5598(sector) == NULL) return NULL;
        *(s32 *)((u8 *)sector + offset) = 1;
    }
    return sector;
}

/* RpMatFXMaterialSetEffects */
RpMaterial *fn_804A20D0(RpMaterial *material, s32 flags) {
    MatFXMaterialData *data = MatFXMaterialDataCreate(material);
    u8 i;

    if (data == NULL) return NULL;
    if (flags == rpMATFXEFFECTNULL || (data->flags != rpMATFXEFFECTNULL && data->flags != flags)) {
        for (i = 0; i < 2; i++) {
            MatFXEffectData *effect = &data->data[i];
            switch (effect->flag) {
            case rpMATFXEFFECTBUMPMAP:
                if (effect->u.bump.bumpTexture != NULL) fn_8048CD8C(effect->u.bump.bumpTexture);
                if (effect->u.bump.texture != NULL) fn_8048CD8C(effect->u.bump.texture);
                break;
            case rpMATFXEFFECTENVMAP:
                if (effect->u.env.texture != NULL) fn_8048CD8C(effect->u.env.texture);
                break;
            case rpMATFXEFFECTDUAL:
                if (effect->u.dual.texture != NULL) fn_8048CD8C(effect->u.dual.texture);
                break;
            case rpMATFXEFFECTUVTRANSFORM:
                effect->u.uv.baseTransform = NULL;
                effect->u.uv.dualTransform = NULL;
                break;
            case rpMATFXEFFECTBUMPENVMAP:
                break;
            }
        }
        memset(data, 0, sizeof(MatFXMaterialData));
    }
    data->flags = flags;
    switch (data->flags) {
    case rpMATFXEFFECTNULL:
        break;
    case rpMATFXEFFECTBUMPMAP:
        data->data[0].flag = rpMATFXEFFECTBUMPMAP;
        break;
    case rpMATFXEFFECTENVMAP:
        data->data[0].flag = rpMATFXEFFECTENVMAP;
        break;
    case rpMATFXEFFECTBUMPENVMAP:
        data->data[0].flag = rpMATFXEFFECTBUMPMAP;
        data->data[1].flag = rpMATFXEFFECTENVMAP;
        break;
    case rpMATFXEFFECTDUAL:
        data->data[0].flag = rpMATFXEFFECTDUAL;
        fn_804A2AB0(material, 5, 6);
        break;
    case rpMATFXEFFECTUVTRANSFORM:
        data->data[0].flag = rpMATFXEFFECTUVTRANSFORM;
        break;
    case rpMATFXEFFECTDUALUVTRANSFORM:
        data->data[0].flag = rpMATFXEFFECTUVTRANSFORM;
        data->data[1].flag = rpMATFXEFFECTDUAL;
        fn_804A2AB0(material, 5, 6);
        break;
    }
    return material;
}

/* RpMatFXMaterialGetEffects */
s32 fn_804A22E4(const RpMaterial *material) {
    const MatFXMaterialData *data = MATFXMATERIALGETDATA(material);
    if (data == NULL) {
        return rpMATFXEFFECTNULL;
    }
    return data->flags;
}

/* RpMatFXMaterialSetBumpMapTexture */
RpMaterial *fn_804A2304(RpMaterial *material, RwTexture *texture) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTBUMPMAP);

    if (effect->u.bump.texture != NULL) {
        fn_8048CD8C(effect->u.bump.texture);
        effect->u.bump.texture = NULL;
    }
    if (effect->u.bump.bumpTexture != NULL) {
        fn_8048CD8C(effect->u.bump.bumpTexture);
        effect->u.bump.bumpTexture = NULL;
        effect->u.bump.invBumpWidth = 0.0f;
    }
    if (texture != NULL) {
        RwTexture *baseTexture;
        s32 empty;
        effect->u.bump.texture = texture;
        effect->u.bump.texture->refCount++;
        baseTexture = material->texture;
        empty = (texture->raster->width == 0);
        if (!empty && baseTexture != NULL) {
            empty = (baseTexture->raster->width == 0);
        }
        if (!empty) {
            char name[32] = "";
            RwTexDictionary *dict;
            MatFXBumpTextureName(name, baseTexture, texture);
            dict = fn_8048D0D8();
            effect->u.bump.bumpTexture = NULL;
            if (dict != NULL) {
                effect->u.bump.bumpTexture = fn_8048CFFC(dict, name);
            }
            if (effect->u.bump.bumpTexture == NULL) {
                effect->u.bump.bumpTexture = fn_804A55AC(baseTexture, texture);
                if (effect->u.bump.bumpTexture == NULL) return NULL;
                if (dict != NULL) {
                    fn_8048CFAC(dict, effect->u.bump.bumpTexture);
                }
            } else {
                effect->u.bump.bumpTexture->refCount++;
            }
            effect->u.bump.invBumpWidth = 1.0f / (f32)effect->u.bump.bumpTexture->raster->width;
        }
    } else {
        effect->u.bump.invBumpWidth = 1.0f / (f32)material->texture->raster->width;
    }
    return material;
}

/* RpMatFXMaterialSetBumpMapFrame */
RpMaterial *fn_804A25E0(RpMaterial *material, RwFrame *frame) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTBUMPMAP);
    effect->u.bump.frame = frame;
    return material;
}

/* RpMatFXMaterialSetBumpMapCoefficient */
RpMaterial *fn_804A2628(RpMaterial *material, f32 coef) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTBUMPMAP);
    effect->u.bump.coef = -coef;
    return material;
}

/* RpMatFXMaterialGetBumpMapTexture */
RwTexture *fn_804A2674(const RpMaterial *material) {
    return MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTBUMPMAP)->u.bump.texture;
}

/* RpMatFXMaterialGetBumpMapFrame */
RwFrame *fn_804A26BC(const RpMaterial *material) {
    return MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTBUMPMAP)->u.bump.frame;
}

/* RpMatFXMaterialGetBumpMapCoefficient */
f32 fn_804A2704(const RpMaterial *material) {
    return -MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTBUMPMAP)->u.bump.coef;
}

/* RpMatFXMaterialSetEnvMapTexture */
RpMaterial *fn_804A2750(RpMaterial *material, RwTexture *texture) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTENVMAP);
    texture->refCount++;
    if (effect->u.env.texture != NULL) {
        fn_8048CD8C(effect->u.env.texture);
        effect->u.env.texture = NULL;
    }
    effect->u.env.texture = texture;
    return material;
}

/* RpMatFXMaterialSetEnvMapFrame */
RpMaterial *fn_804A27F8(RpMaterial *material, RwFrame *frame) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTENVMAP);
    effect->u.env.frame = frame;
    return material;
}

/* RpMatFXMaterialSetEnvMapFrameBufferAlpha */
RpMaterial *fn_804A2840(RpMaterial *material, s32 useFrameBufferAlpha) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTENVMAP);
    effect->u.env.useFrameBufferAlpha = useFrameBufferAlpha;
    return material;
}

/* RpMatFXMaterialSetEnvMapCoefficient */
RpMaterial *fn_804A2888(RpMaterial *material, f32 coef) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTENVMAP);
    effect->u.env.coef = coef;
    return material;
}

/* RpMatFXMaterialGetEnvMapTexture */
RwTexture *fn_804A28D0(const RpMaterial *material) {
    return MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTENVMAP)->u.env.texture;
}

/* RpMatFXMaterialGetEnvMapFrame */
RwFrame *fn_804A2918(const RpMaterial *material) {
    return MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTENVMAP)->u.env.frame;
}

/* RpMatFXMaterialGetEnvMapFrameBufferAlpha */
s32 fn_804A2960(const RpMaterial *material) {
    return MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTENVMAP)
        ->u.env.useFrameBufferAlpha;
}

/* RpMatFXMaterialGetEnvMapCoefficient */
f32 fn_804A29A8(const RpMaterial *material) {
    return MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTENVMAP)->u.env.coef;
}

/* RpMatFXMaterialSetDualTexture */
RpMaterial *fn_804A29F0(RpMaterial *material, RwTexture *texture) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTDUAL);
    texture->refCount++;
    if (effect->u.dual.texture != NULL) {
        fn_8048CD8C(effect->u.dual.texture);
        effect->u.dual.texture = NULL;
    }
    effect->u.dual.texture = texture;
    fn_804A55A4(effect, 2);
    fn_804A55A4(effect, 1);
    return material;
}

/* RpMatFXMaterialSetDualBlendModes */
RpMaterial *fn_804A2AB0(RpMaterial *material, s32 srcBlendMode, s32 dstBlendMode) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTDUAL);
    effect->u.dual.srcBlendMode = srcBlendMode;
    effect->u.dual.dstBlendMode = dstBlendMode;
    fn_804A55A4(effect, 10);
    fn_804A55A4(effect, 11);
    return material;
}

/* RpMatFXMaterialGetDualTexture */
RwTexture *fn_804A2B44(const RpMaterial *material) {
    return MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTDUAL)->u.dual.texture;
}

/* RpMatFXMaterialGetDualBlendModes */
RpMaterial *fn_804A2B8C(RpMaterial *material, s32 *srcBlendMode, s32 *dstBlendMode) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTDUAL);
    *srcBlendMode = effect->u.dual.srcBlendMode;
    *dstBlendMode = effect->u.dual.dstBlendMode;
    return material;
}

/* RpMatFXMaterialSetUVTransformMatrices */
RpMaterial *fn_804A2BE0(RpMaterial *material, RwMatrix *baseTransform, RwMatrix *dualTransform) {
    MatFXEffectData *effect =
        MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTUVTRANSFORM);
    effect->u.uv.baseTransform = baseTransform;
    effect->u.uv.dualTransform = dualTransform;
    return material;
}

/* RpMatFXMaterialGetUVTransformMatrices */
RpMaterial *fn_804A2C2C(RpMaterial *material, RwMatrix **baseTransform, RwMatrix **dualTransform) {
    MatFXEffectData *effect =
        MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTUVTRANSFORM);
    if (baseTransform != NULL) *baseTransform = effect->u.uv.baseTransform;
    if (dualTransform != NULL) *dualTransform = effect->u.uv.dualTransform;
    return material;
}

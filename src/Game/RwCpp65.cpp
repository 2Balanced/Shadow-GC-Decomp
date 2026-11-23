/* RenderWare material effects plugin (rpmatfx, plugin id 0x120), 0x804A0DEC..0x804A29A8: the
 * material/atomic/world sector stream callbacks, RpMatFXPluginAttach, the enable/query calls and the
 * bump map and environment map accessors.  The plugin's file starts in RwCpp64 (engine open/close,
 * material constructor/destructor/copy) and continues in RwCpp66 (dual pass and UV transform
 * accessors).  Functions keep their fn_ symbols; each comment gives the public API name the
 * behaviour matches.  fn_804A114C and fn_804A20D0 are instruction-identical; they only reference
 * constants retail keeps outside this cut (the int to float double, the switch's jump table).
 * Open: fn_804A1A08 (saved registers permuted), fn_804A2304 (retail has two dead branches after
 * the bump texture is set up). */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

#ifdef __cplusplus
extern "C" {
#endif
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

static inline RwStream *MatFXTextureWrite(RwStream *stream, const RwTexture *texture) {
    s32 present = (texture != NULL);
    if (!fn_8047D458(stream, &present, sizeof(present))) return NULL;
    if (present) {
        if (!fn_80483BA0(texture, stream)) return NULL;
    }
    return stream;
}

static inline RwStream *MatFXTextureRead(RwStream *stream, RwTexture **texture) {
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

static inline s32 MatFXTextureSize(const RwTexture *texture) {
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
            if (!MatFXTextureWrite(stream, effect->u.bump.bumpTexture)) return NULL;
            if (!MatFXTextureWrite(stream, effect->u.bump.texture)) return NULL;
            break;
        }
        case rpMATFXEFFECTENVMAP:
            if (!fn_8047D350(stream, &effect->u.env.coef, sizeof(f32))) return NULL;
            value = effect->u.env.useFrameBufferAlpha;
            if (!fn_8047D458(stream, &value, sizeof(value))) return NULL;
            if (!MatFXTextureWrite(stream, effect->u.env.texture)) return NULL;
            break;
        case rpMATFXEFFECTDUAL:
            value = effect->u.dual.srcBlendMode;
            if (!fn_8047D458(stream, &value, sizeof(value))) return NULL;
            value = effect->u.dual.dstBlendMode;
            if (!fn_8047D458(stream, &value, sizeof(value))) return NULL;
            if (!MatFXTextureWrite(stream, effect->u.dual.texture)) return NULL;
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
            if (!MatFXTextureRead(stream, &bumpTexture)) return NULL;
            if (!MatFXTextureRead(stream, &texture)) {
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
            if (!MatFXTextureRead(stream, &texture)) return NULL;
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
            if (!MatFXTextureRead(stream, &texture)) return NULL;
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
            size += MatFXTextureSize(effect->u.bump.bumpTexture);
            size += MatFXTextureSize(effect->u.bump.texture);
            break;
        case rpMATFXEFFECTENVMAP:
            size += sizeof(f32) + sizeof(s32);
            size += MatFXTextureSize(effect->u.env.texture);
            break;
        case rpMATFXEFFECTDUAL:
            size += 2 * sizeof(s32);
            size += MatFXTextureSize(effect->u.dual.texture);
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
    *(s32 *)((u8 *)object + lbl_805F16AC) = 0;
    return object;
}

void *fn_804A1828(void *object, s32 offset, s32 size) {
    *(s32 *)((u8 *)object + lbl_805F16AC) = 0;
    return object;
}

void *fn_804A1838(void *dst, const void *src, s32 offset, s32 size) {
    if (*(s32 *)((u8 *)src + lbl_805F16AC) != 0) {
        *(s32 *)((u8 *)dst + lbl_805F16AC) = 1;
    }
    return dst;
}

RwStream *fn_804A1854(RwStream *stream, s32 length, const void *object, s32 offset, s32 size) {
    s32 enabled = *(s32 *)((u8 *)object + lbl_805F16AC);
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
    return *(s32 *)((u8 *)object + lbl_805F16AC) == 0 ? 0 : 4;
}

/* World sector plugin data: the same flag. */
void *fn_804A1910(void *object, s32 offset, s32 size) {
    *(s32 *)((u8 *)object + lbl_805F16B0) = 0;
    return object;
}

void *fn_804A1920(void *object, s32 offset, s32 size) {
    *(s32 *)((u8 *)object + lbl_805F16B0) = 0;
    return object;
}

void *fn_804A1930(void *dst, const void *src, s32 offset, s32 size) {
    if (*(s32 *)((u8 *)src + lbl_805F16B0) != 0) {
        *(s32 *)((u8 *)dst + lbl_805F16B0) = 1;
    }
    return dst;
}

RwStream *fn_804A194C(RwStream *stream, s32 length, const void *object, s32 offset, s32 size) {
    s32 enabled = *(s32 *)((u8 *)object + lbl_805F16B0);
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
    return *(s32 *)((u8 *)object + lbl_805F16B0) == 0 ? 0 : 4;
}

/* The name of a generated bump texture: the characters of the two source names interleaved. */
#define MATFX_BUMP_NAME(name, baseTexture, heightTexture) \
    do { \
        const char *source[2]; \
        char *out = (name); \
        s32 count = 0; \
        s32 which; \
        if ((baseTexture) != NULL) { \
            source[0] = (baseTexture)->name; \
            source[1] = (heightTexture)->name; \
        } else { \
            source[0] = source[1] = (heightTexture)->name; \
        } \
        while (count < 30 && (*source[0] != 0 || *source[1] != 0)) { \
            for (which = 0; which < 2; which++) { \
                if (*source[which] != 0) { \
                    *out++ = *source[which]++; \
                    count++; \
                } \
            } \
        } \
        *out = 0; \
    } while (0)

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
        MATFX_BUMP_NAME(name, baseTexture, heightTexture);
        fn_8048CE54(texture, name);
    }
    return texture;
}

/* RpMatFXPluginAttach */
s32 fn_804A1E74(void) {
    if (fn_80485D78(0, 0x120, fn_804A07DC, fn_804A076C) < 0) return 0;
    lbl_805F16A8 = fn_8045B2CC(sizeof(MatFXMaterialData *), 0x120, fn_804A0884, fn_804A0894,
                               fn_804A09D0);
    if (lbl_805F16A8 < 0) return 0;
    if (fn_8045B30C(0x120, fn_804A114C, fn_804A0DEC, fn_804A16D4) < 0) return 0;
    lbl_805F16AC = fn_80458B6C(sizeof(s32), 0x120, fn_804A1818, fn_804A1828, fn_804A1838);
    if (lbl_805F16AC < 0) return 0;
    if (fn_80458BEC(0x120, fn_804A1888, fn_804A1854, fn_804A18F0) < 0) return 0;
    lbl_805F16B0 = fn_8045F2F8(sizeof(s32), 0x120, fn_804A1910, fn_804A1920, fn_804A1930);
    if (lbl_805F16B0 < 0) return 0;
    if (fn_8045F338(0x120, fn_804A1980, fn_804A194C, fn_804A19E8) < 0) return 0;
    return fn_804A6C74() != 0;
}

/* RpMatFXAtomicEnableEffects */
void *fn_804A2004(void *atomic) {
    s32 offset = lbl_805F16AC;
    if (*(s32 *)((u8 *)atomic + offset) == 0) {
        if (fn_804A558C(atomic) == NULL) return NULL;
        *(s32 *)((u8 *)atomic + offset) = 1;
    }
    return atomic;
}

/* RpMatFXAtomicQueryEffects */
s32 fn_804A2064(void *atomic) {
    return *(s32 *)((u8 *)atomic + lbl_805F16AC);
}

/* RpMatFXWorldSectorEnableEffects */
void *fn_804A2070(void *sector) {
    s32 offset = lbl_805F16B0;
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
            MATFX_BUMP_NAME(name, baseTexture, texture);
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
#ifdef __cplusplus
}
#endif

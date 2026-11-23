/* RenderWare core: textures and texture dictionaries in streams (0x80483AFC-0x804842C8):
 * RwTextureRegisterPluginStream, RwTextureStreamGetSize, RwTextureStreamWrite, RwTextureStreamRead,
 * the destroy callback of a failed dictionary read and RwTexDictionaryStreamRead. No data; the
 * texture and dictionary plugin registries belong to the texture module.
 * Boundary: the camera module starts at 0x804842C8 (its own constant pool from 0x805FB2E0).
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwStream *fn_8047CEF0(RwStream *stream, u32 type, s32 size, u32 version, u32 build);
RwBool fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
void *fn_8047D158(void *mem, u32 size);
void *fn_8047D254(void *mem, u32 size);
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);
RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length);
s32 fn_804815F0(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB);
const RwPluginRegistry *fn_804816AC(const RwPluginRegistry *reg, RwStream *stream, void *object);
s32 fn_804818FC(const RwPluginRegistry *reg, const void *object);
const RwPluginRegistry *fn_80481980(const RwPluginRegistry *reg, RwStream *stream,
                                    const void *object);
const RwPluginRegistry *fn_80481AD4(const RwPluginRegistry *reg, RwStream *stream);
u32 fn_80482F64(const char *string);
const char *fn_80482FA4(const char *string, RwStream *stream);
char *fn_80483060(char *string, RwStream *stream);
RwBool fn_80485C3C(RwDevice *device, s32 request, void *out, void *inOut, s32 number);
RwBool fn_8048C9A8(RwBool enable);
RwBool fn_8048C9C0(void);
RwBool fn_8048C9D4(RwBool enable);
RwBool fn_8048C9EC(void);
RwTexDictionary *fn_8048CA64(void);
RwBool fn_8048CB38(RwTexDictionary *dict);
const RwTexDictionary *fn_8048CBE0(const RwTexDictionary *dict,
                                   RwTexture *(*callback)(RwTexture *texture, void *data),
                                   void *data);
RwBool fn_8048CD8C(RwTexture *texture);
RwTexture *fn_8048CFAC(RwTexDictionary *dict, RwTexture *texture);
RwTexture *fn_8048D130(const char *name, const char *mask);

extern RwPluginRegistry textureTKList;
extern RwPluginRegistry texDictTKList;

/* RwTextureRegisterPluginStream. */
s32 fn_80483AFC(u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return fn_804815F0(&textureTKList, pluginID, readCB, writeCB, getSizeCB);
}

/* RwTextureStreamGetSize. */
u32 fn_80483B38(const RwTexture *texture) {
    u32 size;

    size = fn_80482F64(texture->name) + 0x1C;
    size += fn_80482F64(texture->mask) + 12;
    size += fn_804818FC(&textureTKList, texture) + 12;
    return size;
}

/* RwTextureStreamWrite: a texture chunk holding the filter word, both names and the plugins. */
const RwTexture *fn_80483BA0(const RwTexture *texture, RwStream *stream) {
    u32 flags;
    RwBool userMipmaps;

    if (!fn_8047CEF0(stream, 6, fn_80483B38(texture), 0x37002, 0x37)) {
        return NULL;
    }
    if (!fn_8047CEF0(stream, 1, sizeof(flags), 0x37002, 0x37)) {
        return NULL;
    }
    if (texture->raster && !(texture->raster->cFormat & 0x10)) {
        userMipmaps = TRUE;
    } else {
        userMipmaps = FALSE;
    }
    flags = (texture->filterAddressing & 0xFFFF) | ((userMipmaps & 0xFF) << 16);
    fn_8047D158(&flags, sizeof(flags));
    if (!fn_80480ED0(stream, &flags, sizeof(flags))) {
        return NULL;
    }
    if (!fn_80482FA4(texture->name, stream)) {
        return NULL;
    }
    if (!fn_80482FA4(texture->mask, stream)) {
        return NULL;
    }
    if (!fn_80481980(&textureTKList, stream, texture)) {
        return NULL;
    }
    return texture;
}

/* RwTextureStreamRead: the texture is found by name; mipmapping follows the stored filter while
 * it is read and the plugin data is only read for a texture nobody else holds. */
RwTexture *fn_80483D28(RwStream *stream) {
    u32 size;
    u32 version;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        u32 flags;
        char name[128];
        char mask[128];
        RwTexture *texture;
        s32 filtering;
        RwBool oldMipmapping;
        RwBool oldAutoMipmapping;
        u32 mipFlags;
        u32 addressingU;
        u32 addressingV;

        memset(&flags, 0, sizeof(flags));
        if (fn_80480D4C(stream, &flags, size) != size) {
            return NULL;
        }
        fn_8047D254(&flags, sizeof(flags));
        filtering = flags & 0xFF;
        addressingV = (flags >> 12) & 0xF;
        addressingU = (flags >> 8) & 0xF;
        if (addressingV == 0) {
            flags |= (addressingU & 0xF) << 12;
        }
        mipFlags = (flags >> 16) & 0xFF;
        oldMipmapping = fn_8048C9C0();
        oldAutoMipmapping = fn_8048C9EC();
        if (filtering == 3 || filtering == 4 || filtering == 5 || filtering == 6) {
            fn_8048C9A8(TRUE);
            if (mipFlags & 1) {
                fn_8048C9D4(FALSE);
            } else {
                fn_8048C9D4(TRUE);
            }
        } else {
            fn_8048C9A8(FALSE);
            fn_8048C9D4(FALSE);
        }
        if (!fn_80483060(name, stream)) {
            fn_8048C9A8(oldMipmapping);
            fn_8048C9D4(oldAutoMipmapping);
            return NULL;
        }
        if (!fn_80483060(mask, stream)) {
            fn_8048C9A8(oldMipmapping);
            fn_8048C9D4(oldAutoMipmapping);
            return NULL;
        }
        texture = fn_8048D130(name, mask);
        if (!texture) {
            fn_80481AD4(&textureTKList, stream);
            fn_8048C9A8(oldMipmapping);
            fn_8048C9D4(oldAutoMipmapping);
            return NULL;
        }
        fn_8048C9A8(oldMipmapping);
        fn_8048C9D4(oldAutoMipmapping);
        if (texture->refCount == 1) {
            texture->filterAddressing = flags & 0xFFFF;
            if (!fn_804816AC(&textureTKList, stream, texture)) {
                return NULL;
            }
        } else {
            if (!fn_80481AD4(&textureTKList, stream)) {
                return NULL;
            }
        }
        return texture;
    }
    RWERROR((0x80000004));
    return NULL;
}

/* The texture callback that destroys each texture of a dictionary whose stream read failed. */
RwTexture *fn_80483F8C(RwTexture *texture, void *data) {
    fn_8048CD8C(texture);
    return texture;
}

static inline void TexDictionaryDestroyAll(RwTexDictionary *texDict) {
    fn_8048CBE0(texDict, fn_80483F8C, NULL);
    fn_8048CB38(texDict);
}

/* RwTexDictionaryStreamRead: the texture count and device id, then each native texture with its
 * plugin data, then the dictionary's plugin data. */
RwTexDictionary *fn_80483FBC(RwStream *stream) {
    u32 size;
    u32 version;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        RwDevice *device = &RwEngineInstance->dOpenDevice;
        rwStreamTexDictionary binTexDict;
        RwTexture *texture;
        RwTexDictionary *texDict;
        u16 deviceId;

        memset(&binTexDict, 0, sizeof(binTexDict));
        if (fn_80480D4C(stream, &binTexDict, size) != size) {
            return NULL;
        }
        fn_8047D254(&binTexDict, sizeof(binTexDict));
        deviceId = 0;
        fn_80485C3C(device, 0x16, &deviceId, NULL, 0);
        deviceId &= 0xFF;
        if (deviceId && binTexDict.deviceId && deviceId != binTexDict.deviceId) {
            return NULL;
        }
        texDict = fn_8048CA64();
        if (!texDict) {
            return NULL;
        }
        while (binTexDict.numTextures--) {
            if (!fn_8047CF50(stream, 0x15, &size, &version)) {
                TexDictionaryDestroyAll(texDict);
                return NULL;
            }
            if (version < 0x35000 || version > 0x37002) {
                TexDictionaryDestroyAll(texDict);
                RWERROR((0x80000004));
                return NULL;
            }
            if (!RwEngineInstance->stdFunc[26](stream, &texture, size)) {
                TexDictionaryDestroyAll(texDict);
                return NULL;
            }
            if (!texture) {
                TexDictionaryDestroyAll(texDict);
                return NULL;
            }
            if (!fn_804816AC(&textureTKList, stream, texture)) {
                TexDictionaryDestroyAll(texDict);
                return NULL;
            }
            fn_8048CFAC(texDict, texture);
        }
        if (!fn_804816AC(&texDictTKList, stream, texDict)) {
            TexDictionaryDestroyAll(texDict);
            return NULL;
        }
        return texDict;
    }
    RWERROR((0x80000004));
    return NULL;
}

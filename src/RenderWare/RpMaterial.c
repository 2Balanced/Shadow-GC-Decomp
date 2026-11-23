/* RenderWare 3.7 world plugin: RpMaterial, 0x8045AF28-0x8045B6F0 (wave 41 re-cut of the size-cut
 * Game/RwCpp21). The file owns its plugin registry and default surface properties (.data
 * 0x8056D840-0x8056D864, padded to 0x8056D868), its free list (.bss 0x805E15F0) and the free list's
 * creation parameters (.sdata 0x805EDDB8), and the rights-chunk state and module record (.sbss
 * 0x805F14E0-0x805F14F0).
 *
 * Names are inferred from behaviour and the public RenderWare API:
 *   fn_8045AF28 / fn_8045AFA4 / fn_8045B020  material "rights" chunk read / write / size
 *   fn_8045B048 / fn_8045B0D8  material module open / close (plugin 0x501)
 *   fn_8045B13C  RpMaterialCreate         fn_8045B1F4  RpMaterialDestroy
 *   fn_8045B270  RpMaterialSetTexture     fn_8045B2CC  RpMaterialRegisterPlugin
 *   fn_8045B30C  RpMaterialRegisterPluginStream  fn_8045B348  RpMaterialStreamRead
 * RpMaterialStreamRead expands RpMaterialDestroy, which the material-list file behind it calls:
 * that is where the file ends.
 * Built with cflags_rw_r3 (configure.py): C, functions in definition order.
 * Matching: RpMaterialStreamRead expands RpMaterialDestroy (and RpMaterialSetTexture in it); the
 * uninitialized statics are defined in reverse of their .sbss order.
 */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

void *memset(void *, int, u32);

s32 fn_804815F0(RwPluginRegistry *, u32, void *, void *, void *);
s32 fn_80481DC8(RwPluginRegistry *, s32, u32, void *, void *, void *);
void fn_8048209C(RwPluginRegistry *, void *);
void fn_80482144(RwPluginRegistry *, void *);
void *fn_804816AC(RwPluginRegistry *, RwStream *, void *);
void fn_80481870(RwPluginRegistry *, u32, void *, u32); /* invoke the rights callback */

RwFreeList *fn_8047FC04(s32, s32, s32, s32, void *, u32); /* RwFreeListCreateAndPreallocateSpace */
s32 fn_8047FCDC(RwFreeList *);                            /* RwFreeListDestroy */

s32 fn_8047CF50(RwStream *, u32, u32 *, u32 *);
u32 fn_80480D4C(RwStream *, void *, u32);
void *fn_8047D254(void *, u32);
RwStream *fn_8047D868(RwStream *, void *, u32); /* RwStreamReadInt32 */
RwStream *fn_8047D458(RwStream *, const void *, u32); /* RwStreamWriteInt32 */
RwTexture *fn_80483D28(RwStream *);             /* RwTextureStreamRead */
s32 fn_8048CD8C(RwTexture *);                   /* RwTextureDestroy */

RpMaterial *fn_8045B270(RpMaterial *material, RwTexture *texture);

/* The plugin registry of materials (0x1C bytes each) and the default surface properties. */
RwPluginRegistry materialTKList = { sizeof(RpMaterial), sizeof(RpMaterial), 0, 0, NULL, NULL };
static RwSurfaceProperties defaultSurfaceProperties = { 1.0f, 1.0f, 1.0f };

/* The material free list and its creation parameters. */
static s32 materialFreeListBlockSize = 256;
static s32 materialFreeListPreallocBlocks = 1;
static RwFreeList materialFreeList;

/* The module record (plugin 0x501), then the rights chunk last read: its plugin and extra data
 * (applied after the plugin chunks). Uninitialized statics come out in reverse order. */
static RwModuleInfo materialModule;
static u32 rightsPluginID;
static u32 rightsExtraData;

RwStream *fn_8045AF28(RwStream *stream, s32 length) {
    if (fn_8047D868(stream, &rightsPluginID, sizeof(u32)) == NULL) {
        return NULL;
    }
    if (length == 8) {
        if (fn_8047D868(stream, &rightsExtraData, sizeof(u32)) == NULL) {
            return NULL;
        }
    }
    return stream;
}

RwStream *fn_8045AFA4(RwStream *stream, s32 length, const RpMaterial *material) {
    if (fn_8047D458(stream, &material->pipeline->pluginId, sizeof(u32)) == NULL) {
        return NULL;
    }
    if (fn_8047D458(stream, &material->pipeline->pluginData, sizeof(u32)) != NULL) {
        return stream;
    }
    return NULL;
}

s32 fn_8045B020(const RpMaterial *material) {
    if (material->pipeline != NULL) {
        if (material->pipeline->pluginId != 0) {
            return 8;
        }
    }
    return 0;
}

void *fn_8045B048(void *instance, s32 offset) {
    materialModule.globalsOffset = offset;
    RWPLUGINGLOBAL(RwFreeList *, materialModule) = fn_8047FC04(
        materialTKList.sizeOfStruct, materialFreeListBlockSize, 4, materialFreeListPreallocBlocks, &materialFreeList, 0x40007);
    if (RWPLUGINGLOBAL(RwFreeList *, materialModule) == NULL) {
        return NULL;
    }
    materialModule.numInstances++;
    return instance;
}

void *fn_8045B0D8(void *instance) {
    if (RWPLUGINGLOBAL(RwFreeList *, materialModule) != NULL) {
        fn_8047FCDC(RWPLUGINGLOBAL(RwFreeList *, materialModule));
        RWPLUGINGLOBAL(RwFreeList *, materialModule) = NULL;
    }
    materialModule.numInstances--;
    return instance;
}

RpMaterial *fn_8045B13C(void) {
    RwRGBA color;
    RpMaterial *material =
        (RpMaterial *)RwFreeListAlloc(RWPLUGINGLOBAL(RwFreeList *, materialModule), 0x30007);
    if (material == NULL) {
        return NULL;
    }
    material->refCount = 1;
    color.red = 0xFF;
    color.green = 0xFF;
    color.blue = 0xFF;
    color.alpha = 0xFF;
    material->color = color;
    material->texture = NULL;
    material->pipeline = NULL;
    material->surfaceProps = defaultSurfaceProperties;
    fn_8048209C(&materialTKList, material);
    return material;
}

s32 fn_8045B1F4(RpMaterial *material) {
    if (material->refCount == 1) {
        fn_80482144(&materialTKList, material);
        fn_8045B270(material, NULL);
        RwFreeListFree(RWPLUGINGLOBAL(RwFreeList *, materialModule), material);
    } else {
        material->refCount--;
    }
    return 1;
}

RpMaterial *fn_8045B270(RpMaterial *material, RwTexture *texture) {
    if (texture != NULL) {
        texture->refCount++;
    }
    if (material->texture != NULL) {
        fn_8048CD8C(material->texture);
    }
    material->texture = texture;
    return material;
}

s32 fn_8045B2CC(s32 size, u32 pluginID, void *constructCB, void *destructCB, void *copyCB) {
    return fn_80481DC8(&materialTKList, size, pluginID, constructCB, destructCB, copyCB);
}

s32 fn_8045B30C(u32 pluginID, void *readCB, void *writeCB, void *getSizeCB) {
    return fn_804815F0(&materialTKList, pluginID, readCB, writeCB, getSizeCB);
}

typedef struct RpMaterialChunkInfo {
    s32 flags;
    RwRGBA color;
    s32 unused;
    s32 textured;
    RwSurfaceProperties surfaceProps;
} RpMaterialChunkInfo;

RpMaterial *fn_8045B348(RwStream *stream) {
    u32 size;
    u32 version;
    RpMaterialChunkInfo info;
    RpMaterial *material;
    RwRGBA color;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        memset(&info, 0, sizeof(info));
        if (fn_80480D4C(stream, &info, size) != size) {
            return NULL;
        }
        /* The colour is bytes: it does not take part in the word swap. */
        color = info.color;
        fn_8047D254(&info, sizeof(info));
        info.color = color;

        material = fn_8045B13C();
        if (material == NULL) {
            return NULL;
        }
        material->color = info.color;
        material->surfaceProps = info.surfaceProps;
        material->texture = NULL;
        if (info.textured) {
            if (!fn_8047CF50(stream, 6, NULL, &version)) {
                fn_8045B1F4(material);
                return NULL;
            }
            if (version >= 0x35000 && version <= 0x37002) {
                material->texture = fn_80483D28(stream);
            } else {
                fn_8045B1F4(material);
                RWERROR((E_RW_BADVERSION));
                return NULL;
            }
        }
        rightsPluginID = 0;
        rightsExtraData = 0;
        if (fn_804816AC(&materialTKList, stream, material) == NULL) {
            fn_8045B1F4(material);
            return NULL;
        }
        if (rightsPluginID != 0) {
            fn_80481870(&materialTKList, rightsPluginID, material, rightsExtraData);
        }
        return material;
    }
    RWERROR((E_RW_BADVERSION));
    return NULL;
}

/* RenderWare world plugin, four modules cut by size:
 *   RpLight (balight)        0x8045A760-0x8045AF28 (its first three functions are in RwCpp20)
 *   RpMaterial (bamateri)    0x8045AF28-0x8045B6F0
 *   material lists (bamatlst) 0x8045B6F0-0x8045BF30
 *   meshes (bamesh)          0x8045BF30- (continues in RwCpp22)
 *
 * Names are inferred from behaviour and the public RenderWare API:
 *   fn_8045A7C4  RpLightSetColor          fn_8045A818  RpLightGetConeAngle
 *   fn_8045AA44  RpLightSetConeAngle      fn_8045AAA0  RpLightRegisterPlugin
 *   fn_8045AAE0  RpLightRegisterPluginStream  fn_8045AB1C  RpLightStreamRead
 *   fn_8045ACD4  RpLightDestroy           fn_8045AD34  RpLightCreate
 *   fn_8045AE20 / fn_8045AE98  light module close / open (plugin 0x505)
 *   fn_8045AF28 / fn_8045AFA4 / fn_8045B020  material "rights" chunk read / write / size
 *   fn_8045B048 / fn_8045B0D8  material module open / close (plugin 0x501)
 *   fn_8045B13C  RpMaterialCreate         fn_8045B1F4  RpMaterialDestroy
 *   fn_8045B270  RpMaterialSetTexture     fn_8045B2CC  RpMaterialRegisterPlugin
 *   fn_8045B30C  RpMaterialRegisterPluginStream  fn_8045B348  RpMaterialStreamRead
 *   fn_8045B6F0  _rpMaterialListDeinitialize  fn_8045B788  _rpMaterialListInitialize
 *   fn_8045B79C  _rpMaterialListGetMaterial   fn_8045B7AC  _rpMaterialListAppendMaterial
 *   fn_8045B8EC  _rpMaterialListFindMaterialIndex  fn_8045B924  _rpMaterialListStreamRead
 *   fn_8045BF30  mesh allocation helper   fn_8045BF64 / fn_8045BFBC  mesh module close / open
 *   fn_8045C144  _rpBuildMeshCreate       fn_8045C26C  _rpBuildMeshDestroy
 *   fn_8045C2D4  _rpMeshDestroy
 * The float constants are the unit's .sdata2 literals; they are named by address until the unit
 * owns that section. Built with cflags_rw_r3 (configure.py): functions are in address order.
 * Status: every function matches except fn_8045B8EC (the material array loaded into r5 and the
 * scaled count into r0, retail the other way round; 3 rows) and fn_8045B924, where retail gives the
 * two parameters r31/r30 and the inlined list helpers' variables the registers below them, ours the
 * reverse (a whole-function register permutation). Both are recorded in probes/Game_RwCpp21.py. */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

extern RwModuleInfo lbl_805F14D8;     /* light module */
extern u32 lbl_805F14E0;              /* extra data of the last rights chunk read */
extern u32 lbl_805F14E4;              /* plugin of the last rights chunk read */
extern RwModuleInfo lbl_805F14E8;     /* material module */
extern RwModuleInfo lbl_805F14F0;     /* mesh module */
extern RwPluginRegistry lbl_8056D828; /* lightTKList */
extern RwPluginRegistry lbl_8056D840; /* materialTKList */
extern RwSurfaceProperties lbl_8056D858; /* default surface properties */
extern u8 lbl_805E15C8[0x28];         /* static light free list */
extern u8 lbl_805E15F0[0x28];         /* static material free list */
extern s32 lbl_805EDDB0;              /* light free list block size */
extern s32 lbl_805EDDB4;              /* light free list preallocation */
extern s32 lbl_805EDDB8;              /* material free list block size */
extern s32 lbl_805EDDBC;              /* material free list preallocation */
extern RwFreeList *lbl_805EDDC0;      /* build mesh free list */

extern const f32 lbl_805FB120; /* 0.0f */
extern const f32 lbl_805FB124; /* pi */
extern const f32 lbl_805FB128; /* pi / 2 */
extern const f32 lbl_805FB12C; /* pi / 2, high part */
extern const f32 lbl_805FB130; /* pi / 2, low part */
extern const f32 lbl_805FB134; /* pS0 */
extern const f32 lbl_805FB138; /* pS1 */
extern const f32 lbl_805FB13C; /* pS2 */
extern const f32 lbl_805FB140; /* pS3 */
extern const f32 lbl_805FB144; /* pS4 */
extern const f32 lbl_805FB148; /* pS5 */
extern const f32 lbl_805FB14C; /* 1.0f */
extern const f32 lbl_805FB150; /* qS1 */
extern const f32 lbl_805FB154; /* qS2 */
extern const f32 lbl_805FB158; /* qS3 */
extern const f32 lbl_805FB15C; /* qS4 */
extern const f32 lbl_805FB160; /* 0.5f */
extern const f32 lbl_805FB164; /* pi, single precision sum */
extern const f32 lbl_805FB168; /* 2.0f */

void *memset(void *, int, u32);
double cos(double);
f32 fn_80482340(f32); /* _rwSqrt */

s32 fn_804815F0(RwPluginRegistry *, u32, void *, void *, void *);
s32 fn_80481DC8(RwPluginRegistry *, s32, u32, void *, void *, void *);
void fn_8048209C(RwPluginRegistry *, void *);
void fn_80482144(RwPluginRegistry *, void *);
void *fn_804816AC(RwPluginRegistry *, RwStream *, void *);
void fn_80481870(RwPluginRegistry *, u32, void *, u32); /* invoke the rights callback */
void fn_8048D834(void *);                               /* release the object's frame */

RwFreeList *fn_8047FC04(s32, s32, s32, s32, void *, u32); /* RwFreeListCreateAndPreallocateSpace */
RwFreeList *fn_8047FBD8(s32, s32, s32, u32);              /* RwFreeListCreate */
s32 fn_8047FCDC(RwFreeList *);                            /* RwFreeListDestroy */
RwFreeList *fn_804800DC(RwFreeList *, void *, void *);    /* RwFreeListForAllUsed */

s32 fn_8047CF50(RwStream *, u32, u32 *, u32 *);
u32 fn_80480D4C(RwStream *, void *, u32);
void *fn_8047D254(void *, u32);
RwStream *fn_8047D868(RwStream *, void *, u32); /* RwStreamReadInt32 */
RwStream *fn_8047D458(RwStream *, const void *, u32); /* RwStreamWriteInt32 */
RwTexture *fn_80483D28(RwStream *);             /* RwTextureStreamRead */
s32 fn_8048CD8C(RwTexture *);                   /* RwTextureDestroy */

void fn_8045A760(void);
void fn_8045A780(void);
RpLight *fn_8045AD34(s32 type);
RpMaterial *fn_8045B270(RpMaterial *material, RwTexture *texture);

/* --- RpLight --- */

RpLight *fn_8045A7C4(RpLight *light, const RwRGBAReal *color) {
    light->color = *color;
    if (light->color.red == light->color.green && light->color.red == light->color.blue) {
        light->object.object.privateFlags = 1;
    } else {
        light->object.object.privateFlags = 0;
    }
    return light;
}

/* Access to the bits of a float, as fdlibm does it. */
typedef union RwFloatBits {
    f32 value;
    s32 word;
} RwFloatBits;

#define GET_FLOAT_WORD(i, d)     \
    do {                         \
        RwFloatBits getBits;     \
        getBits.value = (d);     \
        (i) = getBits.word;      \
    } while (0)

#define SET_FLOAT_WORD(d, i)     \
    do {                         \
        RwFloatBits setBits;     \
        setBits.word = (i);      \
        (d) = setBits.value;     \
    } while (0)

/* The angle is the arc cosine of the stored value, in single precision (the fdlibm algorithm). */
f32 fn_8045A818(const RpLight *light) {
    f32 x = -light->minusCosAngle;
    f32 z, p, q, r, w, s, c, df;
    s32 hx, ix;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7FFFFFFF;
    if (ix >= 0x3F800000) {
        if (hx > 0) {
            return lbl_805FB120;
        }
        return lbl_805FB124;
    }
    if (ix < 0x3F000000) {
        if (ix <= 0x23000000) {
            return lbl_805FB128;
        }
        z = x * x;
        p = z * (lbl_805FB134 +
                 z * (lbl_805FB138 +
                      z * (lbl_805FB13C + z * (lbl_805FB140 + z * (lbl_805FB144 + lbl_805FB148 * z)))));
        q = lbl_805FB14C +
            z * (lbl_805FB150 + z * (lbl_805FB154 + z * (lbl_805FB158 + lbl_805FB15C * z)));
        r = p / q;
        return lbl_805FB12C - (x - (lbl_805FB130 - x * r));
    } else if (hx < 0) {
        z = (lbl_805FB14C + x) * lbl_805FB160;
        s = fn_80482340(z);
        p = z * (lbl_805FB134 +
                 z * (lbl_805FB138 +
                      z * (lbl_805FB13C + z * (lbl_805FB140 + z * (lbl_805FB144 + lbl_805FB148 * z)))));
        q = lbl_805FB14C +
            z * (lbl_805FB150 + z * (lbl_805FB154 + z * (lbl_805FB158 + lbl_805FB15C * z)));
        r = p / q;
        w = r * s - lbl_805FB130;
        return lbl_805FB164 - lbl_805FB168 * (s + w);
    } else {
        s32 idf;
        z = (lbl_805FB14C - x) * lbl_805FB160;
        s = fn_80482340(z);
        df = s;
        GET_FLOAT_WORD(idf, df);
        SET_FLOAT_WORD(df, idf & 0xFFFFF000);
        c = (z - df * df) / (s + df);
        p = z * (lbl_805FB134 +
                 z * (lbl_805FB138 +
                      z * (lbl_805FB13C + z * (lbl_805FB140 + z * (lbl_805FB144 + lbl_805FB148 * z)))));
        q = lbl_805FB14C +
            z * (lbl_805FB150 + z * (lbl_805FB154 + z * (lbl_805FB158 + lbl_805FB15C * z)));
        r = p / q;
        w = r * s + c;
        return lbl_805FB168 * (df + w);
    }
}

RpLight *fn_8045AA44(RpLight *light, f32 angle) {
    if (angle < lbl_805FB120 || angle > lbl_805FB128) {
        return NULL;
    }
    light->minusCosAngle = -(f32)cos(angle);
    return light;
}

s32 fn_8045AAA0(s32 size, u32 pluginID, void *constructCB, void *destructCB, void *copyCB) {
    return fn_80481DC8(&lbl_8056D828, size, pluginID, constructCB, destructCB, copyCB);
}

s32 fn_8045AAE0(u32 pluginID, void *readCB, void *writeCB, void *getSizeCB) {
    return fn_804815F0(&lbl_8056D828, pluginID, readCB, writeCB, getSizeCB);
}

typedef struct RpLightChunkInfo {
    f32 radius;
    f32 red;
    f32 green;
    f32 blue;
    f32 minusCosAngle;
    u32 typeAndFlags;
} RpLightChunkInfo;

RpLight *fn_8045AB1C(RwStream *stream) {
    RpLight *light;
    RpLightChunkInfo info;
    u32 size;
    u32 version;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        memset(&info, 0, sizeof(info));
        if (fn_80480D4C(stream, &info, size) != size) {
            return NULL;
        }
        fn_8047D254(&info, sizeof(info));
        light = fn_8045AD34((info.typeAndFlags >> 16) & 0xFF);
        if (light == NULL) {
            return NULL;
        }
        light->radius = info.radius;
        light->color.red = info.red;
        light->color.green = info.green;
        light->color.blue = info.blue;
        if (version < 0x30300) {
            /* Old streams hold the tangent of the cone angle. */
            light->minusCosAngle =
                -fn_80482340(lbl_805FB14C / (lbl_805FB14C + info.minusCosAngle * info.minusCosAngle));
        } else {
            light->minusCosAngle = info.minusCosAngle;
        }
        if (light->color.red == light->color.green && light->color.red == light->color.blue) {
            light->object.object.privateFlags = 1;
        } else {
            light->object.object.privateFlags = 0;
        }
        light->object.object.flags = info.typeAndFlags;
        if (fn_804816AC(&lbl_8056D828, stream, light) == NULL) {
            return NULL;
        }
        return light;
    }
    RWERROR((E_RW_BADVERSION));
    return NULL;
}

s32 fn_8045ACD4(RpLight *light) {
    fn_80482144(&lbl_8056D828, light);
    fn_8048D834(light);
    RwFreeListFree(RWPLUGINGLOBAL(RwFreeList *, lbl_805F14D8), light);
    return 1;
}

RpLight *fn_8045AD34(s32 type) {
    RpLight *light = (RpLight *)RwFreeListAlloc(RWPLUGINGLOBAL(RwFreeList *, lbl_805F14D8), 0x30012);
    if (light == NULL) {
        return NULL;
    }
    light->object.object.type = 3;
    light->object.object.subType = type;
    light->object.object.flags = 0;
    light->object.object.privateFlags = 0;
    light->object.object.parent = NULL;
    light->object.sync = (RwObjectHasFrame *(*)(RwObjectHasFrame *))fn_8045A780;
    light->radius = lbl_805FB120;
    light->minusCosAngle = lbl_805FB120;
    light->color.red = lbl_805FB14C;
    light->color.green = lbl_805FB14C;
    light->color.blue = lbl_805FB14C;
    light->color.alpha = lbl_805FB14C;
    light->object.object.privateFlags = 1;
    light->WorldSectorsInLight.link.next = &light->WorldSectorsInLight.link;
    light->WorldSectorsInLight.link.prev = &light->WorldSectorsInLight.link;
    light->inWorld.prev = NULL;
    light->inWorld.next = NULL;
    light->lightFrame = RWSRCGLOBAL(lightFrame) - 1;
    light->object.object.flags = 3;
    fn_8048209C(&lbl_8056D828, light);
    return light;
}

void *fn_8045AE20(void *instance) {
    fn_804800DC(RWPLUGINGLOBAL(RwFreeList *, lbl_805F14D8), (void *)fn_8045A760, NULL);
    fn_8047FCDC(RWPLUGINGLOBAL(RwFreeList *, lbl_805F14D8));
    RWPLUGINGLOBAL(RwFreeList *, lbl_805F14D8) = NULL;
    lbl_805F14D8.numInstances--;
    return instance;
}

void *fn_8045AE98(void *instance, s32 offset) {
    lbl_805F14D8.globalsOffset = offset;
    RWPLUGINGLOBAL(RwFreeList *, lbl_805F14D8) = fn_8047FC04(
        lbl_8056D828.sizeOfStruct, lbl_805EDDB0, 4, lbl_805EDDB4, lbl_805E15C8, 0x40012);
    if (RWPLUGINGLOBAL(RwFreeList *, lbl_805F14D8) != NULL) {
        lbl_805F14D8.numInstances++;
        return instance;
    }
    return NULL;
}

/* --- RpMaterial --- */

RwStream *fn_8045AF28(RwStream *stream, s32 length) {
    if (fn_8047D868(stream, &lbl_805F14E4, sizeof(u32)) == NULL) {
        return NULL;
    }
    if (length == 8) {
        if (fn_8047D868(stream, &lbl_805F14E0, sizeof(u32)) == NULL) {
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
    lbl_805F14E8.globalsOffset = offset;
    RWPLUGINGLOBAL(RwFreeList *, lbl_805F14E8) = fn_8047FC04(
        lbl_8056D840.sizeOfStruct, lbl_805EDDB8, 4, lbl_805EDDBC, lbl_805E15F0, 0x40007);
    if (RWPLUGINGLOBAL(RwFreeList *, lbl_805F14E8) == NULL) {
        return NULL;
    }
    lbl_805F14E8.numInstances++;
    return instance;
}

void *fn_8045B0D8(void *instance) {
    if (RWPLUGINGLOBAL(RwFreeList *, lbl_805F14E8) != NULL) {
        fn_8047FCDC(RWPLUGINGLOBAL(RwFreeList *, lbl_805F14E8));
        RWPLUGINGLOBAL(RwFreeList *, lbl_805F14E8) = NULL;
    }
    lbl_805F14E8.numInstances--;
    return instance;
}

RpMaterial *fn_8045B13C(void) {
    RwRGBA color;
    RpMaterial *material =
        (RpMaterial *)RwFreeListAlloc(RWPLUGINGLOBAL(RwFreeList *, lbl_805F14E8), 0x30007);
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
    material->surfaceProps = lbl_8056D858;
    fn_8048209C(&lbl_8056D840, material);
    return material;
}

/* The material file expands RpMaterialDestroy in RpMaterialStreamRead; the material-list file
 * behind it calls it. While both are one unit, the function is kept out of line and the stream
 * reader uses the two inline copies below (remove them when the unit is cut at 0x8045B6F0). */
#pragma dont_inline on
s32 fn_8045B1F4(RpMaterial *material) {
    if (material->refCount == 1) {
        fn_80482144(&lbl_8056D840, material);
        fn_8045B270(material, NULL);
        RwFreeListFree(RWPLUGINGLOBAL(RwFreeList *, lbl_805F14E8), material);
    } else {
        material->refCount--;
    }
    return 1;
}
#pragma dont_inline reset

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

static inline RpMaterial *RpMaterialSetTextureInline(RpMaterial *material, RwTexture *texture) {
    if (texture != NULL) {
        texture->refCount++;
    }
    if (material->texture != NULL) {
        fn_8048CD8C(material->texture);
    }
    material->texture = texture;
    return material;
}

static inline s32 RpMaterialDestroyInline(RpMaterial *material) {
    if (material->refCount == 1) {
        fn_80482144(&lbl_8056D840, material);
        RpMaterialSetTextureInline(material, NULL);
        RwFreeListFree(RWPLUGINGLOBAL(RwFreeList *, lbl_805F14E8), material);
    } else {
        material->refCount--;
    }
    return 1;
}

s32 fn_8045B2CC(s32 size, u32 pluginID, void *constructCB, void *destructCB, void *copyCB) {
    return fn_80481DC8(&lbl_8056D840, size, pluginID, constructCB, destructCB, copyCB);
}

s32 fn_8045B30C(u32 pluginID, void *readCB, void *writeCB, void *getSizeCB) {
    return fn_804815F0(&lbl_8056D840, pluginID, readCB, writeCB, getSizeCB);
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
                RpMaterialDestroyInline(material);
                return NULL;
            }
            if (version >= 0x35000 && version <= 0x37002) {
                material->texture = fn_80483D28(stream);
            } else {
                RpMaterialDestroyInline(material);
                RWERROR((E_RW_BADVERSION));
                return NULL;
            }
        }
        lbl_805F14E4 = 0;
        lbl_805F14E0 = 0;
        if (fn_804816AC(&lbl_8056D840, stream, material) == NULL) {
            RpMaterialDestroyInline(material);
            return NULL;
        }
        if (lbl_805F14E4 != 0) {
            fn_80481870(&lbl_8056D840, lbl_805F14E4, material, lbl_805F14E0);
        }
        return material;
    }
    RWERROR((E_RW_BADVERSION));
    return NULL;
}

/* --- material lists --- */

RpMaterialList *fn_8045B6F0(RpMaterialList *matList) {
    RpMaterial **materials = matList->materials;
    if (materials != NULL) {
        s32 count = matList->numMaterials;
        s32 i;
        for (i = 0; i < count; i++) {
            fn_8045B1F4(materials[i]);
            materials[i] = NULL;
        }
        RwFree(materials);
        matList->materials = NULL;
    }
    matList->numMaterials = 0;
    matList->space = 0;
    return matList;
}

RpMaterialList *fn_8045B788(RpMaterialList *matList) {
    matList->space = 0;
    matList->materials = NULL;
    matList->numMaterials = 0;
    return matList;
}

RpMaterial *fn_8045B79C(const RpMaterialList *matList, s32 index) {
    return matList->materials[index];
}

static RpMaterialList *RpMaterialListSetSize(RpMaterialList *matList, s32 space) {
    if (matList->space < space) {
        RpMaterial **materials;
        u32 size = space * sizeof(RpMaterial *);
        if (matList->materials != NULL) {
            materials = (RpMaterial **)RwRealloc(matList->materials, size, 0x1030008);
        } else {
            materials = (RpMaterial **)RwMalloc(size, 0x1030008);
        }
        if (materials == NULL) {
            RWERROR((E_RW_NOMEM, size));
            return NULL;
        }
        matList->materials = materials;
        matList->space = space;
    }
    return matList;
}

s32 fn_8045B7AC(RpMaterialList *matList, RpMaterial *material) {
    RpMaterial **materials;
    s32 space;
    u32 size;

    if (matList->space > matList->numMaterials) {
        matList->materials[matList->numMaterials] = material;
        material->refCount++;
        matList->numMaterials++;
        return matList->numMaterials - 1;
    }
    space = matList->space + 20;
    size = space * sizeof(RpMaterial *);
    if (matList->materials != NULL) {
        materials = (RpMaterial **)RwRealloc(matList->materials, space * sizeof(RpMaterial *),
                                             0x1030008);
    } else {
        materials = (RpMaterial **)RwMalloc(space * sizeof(RpMaterial *), 0x1030008);
    }
    if (materials == NULL) {
        RWERROR((E_RW_NOMEM, size));
        return -1;
    }
    matList->materials = materials;
    matList->space += 20;
    materials[matList->numMaterials] = material;
    material->refCount++;
    matList->numMaterials++;
    return matList->numMaterials - 1;
}

s32 fn_8045B8EC(const RpMaterialList *matList, const RpMaterial *material) {
    s32 i = matList->numMaterials;
    RpMaterial **entry = matList->materials + i;
    while (entry--, i-- > 0) {
        if (*entry == material) {
            return i;
        }
    }
    return i;
}

RpMaterialList *fn_8045B924(RwStream *stream, RpMaterialList *matList) {
    s32 count;
    u32 size;
    u32 version;
    s32 *indices;
    s32 i;
    s32 status;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        status = (fn_8047D868(stream, &count, sizeof(count)) != NULL);
        if (!status) {
            return NULL;
        }
        fn_8045B788(matList);
        if (count == 0) {
            return matList;
        }
        if (RpMaterialListSetSize(matList, count) == NULL) {
            fn_8045B6F0(matList);
            return NULL;
        }
        indices = (s32 *)RwMalloc(count * sizeof(s32), 0x10501);
        status = (fn_8047D868(stream, indices, count * sizeof(s32)) != NULL);
        if (!status) {
            RwFree(indices);
            fn_8045B6F0(matList);
            return NULL;
        }
        for (i = 0; i < count; i++) {
            RpMaterial *material;
            if (indices[i] < 0) {
                if (!fn_8047CF50(stream, 7, NULL, &version)) {
                    RwFree(indices);
                    fn_8045B6F0(matList);
                    return NULL;
                }
                if (version >= 0x35000 && version <= 0x37002) {
                    material = fn_8045B348(stream);
                    if (material == NULL) {
                        RwFree(indices);
                        fn_8045B6F0(matList);
                        return NULL;
                    }
                } else {
                    RWERROR((E_RW_BADVERSION));
                    RwFree(indices);
                    fn_8045B6F0(matList);
                    return NULL;
                }
            } else {
                material = fn_8045B79C(matList, indices[i]);
                material->refCount++;
            }
            fn_8045B7AC(matList, material);
            fn_8045B1F4(material);
        }
        RwFree(indices);
        return matList;
    }
    RWERROR((E_RW_BADVERSION));
    return NULL;
}

/* --- meshes --- */

void *fn_8045BF30(u32 size) {
    return RwMalloc(size, 0x30502);
}

void *fn_8045BF64(void *instance) {
    lbl_805F14F0.numInstances--;
    if (lbl_805F14F0.numInstances == 0) {
        if (lbl_805EDDC0 != NULL) {
            fn_8047FCDC(lbl_805EDDC0);
            lbl_805EDDC0 = NULL;
        }
    }
    return instance;
}

#define RPMESHGLOBAL(member) (RWPLUGINGLOBAL(rpMeshGlobals, lbl_805F14F0).member)

void *fn_8045BFBC(void *instance, s32 offset) {
    lbl_805F14F0.globalsOffset = offset;
    if (lbl_805F14F0.numInstances == 0) {
        lbl_805EDDC0 = fn_8047FBD8(sizeof(RpBuildMesh), 50, 4, 0x40502);
        if (lbl_805EDDC0 == NULL) {
            if (lbl_805EDDC0 != NULL) {
                fn_8047FCDC(lbl_805EDDC0);
                lbl_805EDDC0 = NULL;
            }
            return NULL;
        }
    }
    RPMESHGLOBAL(nextSerialNum) = 1;
    lbl_805F14F0.numInstances++;
    RPMESHGLOBAL(meshFlagsToPrimType)[0] = 3;
    RPMESHGLOBAL(meshFlagsToPrimType)[1] = 4;
    RPMESHGLOBAL(meshFlagsToPrimType)[2] = 5;
    RPMESHGLOBAL(meshFlagsToPrimType)[4] = 1;
    RPMESHGLOBAL(meshFlagsToPrimType)[8] = 2;
    RPMESHGLOBAL(meshFlagsToPrimType)[16] = 6;
    RPMESHGLOBAL(primTypeToMeshFlags)[1] = 4;
    RPMESHGLOBAL(primTypeToMeshFlags)[2] = 8;
    RPMESHGLOBAL(primTypeToMeshFlags)[3] = 0;
    RPMESHGLOBAL(primTypeToMeshFlags)[4] = 1;
    RPMESHGLOBAL(primTypeToMeshFlags)[5] = 2;
    RPMESHGLOBAL(primTypeToMeshFlags)[6] = 16;
    return instance;
}

RpBuildMesh *fn_8045C144(u32 bufferSize) {
    RpBuildMesh *mesh = (RpBuildMesh *)RwFreeListAlloc(lbl_805EDDC0, 0x30502);
    if (mesh != NULL) {
        mesh->numTriangles = 0;
        if (bufferSize != 0) {
            u32 size = bufferSize * sizeof(RpBuildMeshTriangle);
            mesh->meshTriangles = (RpBuildMeshTriangle *)RwMalloc(size, 0x1030502);
            if (mesh->meshTriangles == NULL) {
                RwFreeListFree(lbl_805EDDC0, mesh);
                RWERROR((E_RW_NOMEM, size));
                return NULL;
            }
            mesh->triangleBufferSize = bufferSize;
        } else {
            mesh->meshTriangles = NULL;
            mesh->triangleBufferSize = 0;
        }
        return mesh;
    }
    RWERROR((E_RW_NOMEM, sizeof(RpBuildMesh)));
    return NULL;
}

s32 fn_8045C26C(RpBuildMesh *mesh) {
    if (mesh->meshTriangles != NULL) {
        RwFree(mesh->meshTriangles);
        mesh->meshTriangles = NULL;
    }
    RwFreeListFree(lbl_805EDDC0, mesh);
    return 1;
}

s32 fn_8045C2D4(RpMeshHeader *header) {
    if (header->flags != 0 || header->numMeshes != 0 || header->serialNum != 0 ||
        header->totalIndicesInMesh != 0 || header->firstMeshOffset != 0) {
        RwFree(header);
    }
    return 1;
}

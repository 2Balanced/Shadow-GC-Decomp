/* RenderWare 3.7 world plugin: RpLight, 0x8045A760-0x8045AF28 (wave 41 re-cut of the size-cut
 * Game/RwCpp20 tail and Game/RwCpp21 head). The file owns its plugin registry (.data 0x8056D828),
 * its free list (.bss 0x805E15C8) and the free list's creation parameters (.sdata 0x805EDDB0), its
 * module record (.sbss 0x805F14D8) and its literal pool (.sdata2 0x805FB120-0x805FB170: the
 * constants of the single-precision arc cosine that RpLightGetConeAngle expands, then 2.0f).
 *
 * Names are inferred from behaviour and the public RenderWare API:
 *   fn_8045A760  free-list callback that destroys a light (module close)
 *   fn_8045A780  the light's frame sync callback (does nothing)
 *   fn_8045A784  RpLightSetRadius         fn_8045A7C4  RpLightSetColor
 *   fn_8045A818  RpLightGetConeAngle      fn_8045AA44  RpLightSetConeAngle
 *   fn_8045AAA0  RpLightRegisterPlugin    fn_8045AAE0  RpLightRegisterPluginStream
 *   fn_8045AB1C  RpLightStreamRead        fn_8045ACD4  RpLightDestroy
 *   fn_8045AD34  RpLightCreate            fn_8045AE20 / fn_8045AE98  light module close / open
 * Built with cflags_rw_r3 (configure.py): C, functions in definition order.
 * Matching: the arc cosine's constants come out in retail's pool order as written.
 */
#include <dolphin/types.h>
#include "Game/rw/r3.h"

void *memset(void *, int, u32);
double cos(double);
f32 fn_80482340(f32); /* _rwSqrt */

s32 fn_804815F0(RwPluginRegistry *, u32, void *, void *, void *);
s32 fn_80481DC8(RwPluginRegistry *, s32, u32, void *, void *, void *);
void fn_8048209C(RwPluginRegistry *, void *);
void fn_80482144(RwPluginRegistry *, void *);
void *fn_804816AC(RwPluginRegistry *, RwStream *, void *);
void fn_8048D834(void *);                               /* release the object's frame */
void fn_8048722C(void *);                               /* RwFrameUpdateObjects */

RwFreeList *fn_8047FC04(s32, s32, s32, s32, void *, u32); /* RwFreeListCreateAndPreallocateSpace */
s32 fn_8047FCDC(RwFreeList *);                            /* RwFreeListDestroy */
RwFreeList *fn_804800DC(RwFreeList *, void *, void *);    /* RwFreeListForAllUsed */

s32 fn_8047CF50(RwStream *, u32, u32 *, u32 *);
u32 fn_80480D4C(RwStream *, void *, u32);
void *fn_8047D254(void *, u32);

s32 fn_8045ACD4(RpLight *light);
RpLight *fn_8045AD34(s32 type);

/* The plugin registry of lights (0x40 bytes each). */
RwPluginRegistry lightTKList = { sizeof(RpLight), sizeof(RpLight), 0, 0, NULL, NULL };

/* The light free list and its creation parameters. */
static s32 lightFreeListBlockSize = 32;
static s32 lightFreeListPreallocBlocks = 1;
static RwFreeList lightFreeList;

/* The module record (plugin 0x505). */
static RwModuleInfo lightModule;

/* Destroys each light still allocated when the module closes. */
void fn_8045A760(void *light, void *data) {
    fn_8045ACD4((RpLight *)light);
}

/* A light's frame sync callback: nothing to do. */
void fn_8045A780(void) {
}

RpLight *fn_8045A784(RpLight *light, f32 radius) {
    void *frame = light->object.object.parent;
    light->radius = radius;
    if (frame != NULL) {
        fn_8048722C(frame);
    }
    return light;
}

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
            return 0.0f;
        }
        return 3.14159274101e+00f;
    }
    if (ix < 0x3F000000) {
        if (ix <= 0x23000000) {
            return 1.57079637051e+00f;
        }
        z = x * x;
        p = z * (1.6666667163e-01f +
                 z * (-3.2556581497e-01f +
                      z * (2.0121252537e-01f + z * (-4.0055535734e-02f + z * (7.9153501429e-04f + 3.4793309169e-05f * z)))));
        q = 1.0f +
            z * (-2.4033949375e+00f + z * (2.0209457874e+00f + z * (-6.8828397989e-01f + 7.7038154006e-02f * z)));
        r = p / q;
        return 1.5707962513e+00f - (x - (7.5497894159e-08f - x * r));
    } else if (hx < 0) {
        z = (1.0f + x) * 0.5f;
        s = fn_80482340(z);
        p = z * (1.6666667163e-01f +
                 z * (-3.2556581497e-01f +
                      z * (2.0121252537e-01f + z * (-4.0055535734e-02f + z * (7.9153501429e-04f + 3.4793309169e-05f * z)))));
        q = 1.0f +
            z * (-2.4033949375e+00f + z * (2.0209457874e+00f + z * (-6.8828397989e-01f + 7.7038154006e-02f * z)));
        r = p / q;
        w = r * s - 7.5497894159e-08f;
        return 3.1415925026e+00f - 2.0f * (s + w);
    } else {
        s32 idf;
        z = (1.0f - x) * 0.5f;
        s = fn_80482340(z);
        df = s;
        GET_FLOAT_WORD(idf, df);
        SET_FLOAT_WORD(df, idf & 0xFFFFF000);
        c = (z - df * df) / (s + df);
        p = z * (1.6666667163e-01f +
                 z * (-3.2556581497e-01f +
                      z * (2.0121252537e-01f + z * (-4.0055535734e-02f + z * (7.9153501429e-04f + 3.4793309169e-05f * z)))));
        q = 1.0f +
            z * (-2.4033949375e+00f + z * (2.0209457874e+00f + z * (-6.8828397989e-01f + 7.7038154006e-02f * z)));
        r = p / q;
        w = r * s + c;
        return 2.0f * (df + w);
    }
}

RpLight *fn_8045AA44(RpLight *light, f32 angle) {
    if (angle < 0.0f || angle > 1.57079637051e+00f) {
        return NULL;
    }
    light->minusCosAngle = -(f32)cos(angle);
    return light;
}

s32 fn_8045AAA0(s32 size, u32 pluginID, void *constructCB, void *destructCB, void *copyCB) {
    return fn_80481DC8(&lightTKList, size, pluginID, constructCB, destructCB, copyCB);
}

s32 fn_8045AAE0(u32 pluginID, void *readCB, void *writeCB, void *getSizeCB) {
    return fn_804815F0(&lightTKList, pluginID, readCB, writeCB, getSizeCB);
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
                -fn_80482340(1.0f / (1.0f + info.minusCosAngle * info.minusCosAngle));
        } else {
            light->minusCosAngle = info.minusCosAngle;
        }
        if (light->color.red == light->color.green && light->color.red == light->color.blue) {
            light->object.object.privateFlags = 1;
        } else {
            light->object.object.privateFlags = 0;
        }
        light->object.object.flags = info.typeAndFlags;
        if (fn_804816AC(&lightTKList, stream, light) == NULL) {
            return NULL;
        }
        return light;
    }
    RWERROR((E_RW_BADVERSION));
    return NULL;
}

s32 fn_8045ACD4(RpLight *light) {
    fn_80482144(&lightTKList, light);
    fn_8048D834(light);
    RwFreeListFree(RWPLUGINGLOBAL(RwFreeList *, lightModule), light);
    return 1;
}

RpLight *fn_8045AD34(s32 type) {
    RpLight *light = (RpLight *)RwFreeListAlloc(RWPLUGINGLOBAL(RwFreeList *, lightModule), 0x30012);
    if (light == NULL) {
        return NULL;
    }
    light->object.object.type = 3;
    light->object.object.subType = type;
    light->object.object.flags = 0;
    light->object.object.privateFlags = 0;
    light->object.object.parent = NULL;
    light->object.sync = (RwObjectHasFrame *(*)(RwObjectHasFrame *))fn_8045A780;
    light->radius = 0.0f;
    light->minusCosAngle = 0.0f;
    light->color.red = 1.0f;
    light->color.green = 1.0f;
    light->color.blue = 1.0f;
    light->color.alpha = 1.0f;
    light->object.object.privateFlags = 1;
    light->WorldSectorsInLight.link.next = &light->WorldSectorsInLight.link;
    light->WorldSectorsInLight.link.prev = &light->WorldSectorsInLight.link;
    light->inWorld.prev = NULL;
    light->inWorld.next = NULL;
    light->lightFrame = RWSRCGLOBAL(lightFrame) - 1;
    light->object.object.flags = 3;
    fn_8048209C(&lightTKList, light);
    return light;
}

void *fn_8045AE20(void *instance) {
    fn_804800DC(RWPLUGINGLOBAL(RwFreeList *, lightModule), (void *)fn_8045A760, NULL);
    fn_8047FCDC(RWPLUGINGLOBAL(RwFreeList *, lightModule));
    RWPLUGINGLOBAL(RwFreeList *, lightModule) = NULL;
    lightModule.numInstances--;
    return instance;
}

void *fn_8045AE98(void *instance, s32 offset) {
    lightModule.globalsOffset = offset;
    RWPLUGINGLOBAL(RwFreeList *, lightModule) = fn_8047FC04(
        lightTKList.sizeOfStruct, lightFreeListBlockSize, 4, lightFreeListPreallocBlocks, &lightFreeList, 0x40012);
    if (RWPLUGINGLOBAL(RwFreeList *, lightModule) != NULL) {
        lightModule.numInstances++;
        return instance;
    }
    return NULL;
}

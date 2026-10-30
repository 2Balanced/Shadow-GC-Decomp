/* RenderWare material and frame helpers (file and names inferred; no class): the fade a clump's
 * atomics are rendered with (fn_80038198, the material colour passes and the light scale), the
 * fog/light mode pass, texture filter set-up, frame and hierarchy searches, atomic visibility and a
 * texture dictionary loader. Other units call these by their fn_ names; the rest are RenderWare
 * callbacks. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/DebugDispShape.h"
#include "Game/TBGRead.h"
#include <dolphin/types.h>
#include "size_assert.h"


struct RwStream;
struct RwTexDictionary;
struct RpWorld;
struct RpHAnimHierarchy;

struct RwRaster {
    /* 0x00 */ u8 field_0x0[0x23];
    /* 0x23 */ u8 cFormat;
};

struct RwTexture {
    /* 0x00 */ RwRaster *raster;
    /* 0x04 */ u8 field_0x4[0x50 - 0x4];
    /* 0x50 */ u32 filterAddressing;
};

struct RpMaterial {
    /* 0x00 */ RwTexture *texture;
    /* 0x04 */ RwRGBA color;
};

struct RpGeometry {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ u32 flags;
    /* 0x0C */ u8 field_0xC[0x14 - 0xC];
    /* 0x14 */ s32 numVertices;
};

struct RpAtomic;
typedef RpAtomic *(*RpAtomicCallBackRender)(RpAtomic *atomic);

struct RpAtomic {
    /* 0x00 */ u8 type;
    /* 0x01 */ u8 subType;
    /* 0x02 */ u8 flags;
    /* 0x03 */ u8 privateFlags;
    /* 0x04 */ RwFrame *frame;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ RpGeometry *geometry;
    /* 0x1C */ u8 field_0x1C[0x48 - 0x1C];
    /* 0x48 */ RpAtomicCallBackRender renderCallBack;
};

/* A node of an animation hierarchy (rphanim.h's RpHAnimNodeInfo). */
struct RpHAnimNodeInfo {
    /* 0x0 */ s32 nodeID;
    /* 0x4 */ s32 nodeIndex;
    /* 0x8 */ s32 flags;
    /* 0xC */ RwFrame *pFrame;
};

struct RpHAnimHierarchy {
    /* 0x00 */ s32 flags;
    /* 0x04 */ s32 numNodes;
    /* 0x08 */ u8 field_0x8[0x10 - 0x8];
    /* 0x10 */ RpHAnimNodeInfo *pNodeInfo;
};

struct RwIm2DVertex {
    /* 0x00 */ float x;
    /* 0x04 */ float y;
    /* 0x08 */ float z;
    /* 0x0C */ RwRGBA emissiveColor;
    /* 0x10 */ float u;
    /* 0x14 */ float v;
};

struct RwIm3DVertex {
    /* 0x00 */ RwV3d objVertex;
    /* 0x0C */ RwV3d objNormal;
    /* 0x18 */ RwRGBA color;
    /* 0x1C */ float u;
    /* 0x20 */ float v;
};

struct RwGlobals {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ float nearScreenZ;
    /* 0x1C */ u8 field_0x1C[0x20 - 0x1C];
    /* 0x20 */ int (*fpRenderStateSet)(int state, void *value);
    /* 0x24 */ int (*fpRenderStateGet)(int state, void *value);
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ int (*fpIm2DRenderPrimitive)(int primType, RwIm2DVertex *vertices, int numVertices);
};

extern RwGlobals *lbl_805F15B4;

/* A rectangle in pixels (rwcore.h's RwRect). */
struct RwRect {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
};

/* The renderer (Game/UnkRenderer_800289B0.cpp; the part used here): its fog/light mode byte. */
struct UnkRenderer_800289B0 {
    /* 0x000 */ u8 field_0x0[0x456];
    /* 0x456 */ u8 field_0x456;
};

/* A fade the effects hand to fn_80038198 (layout from its use; names inferred): colour factors
 * (each applied when not negative), an alpha per screen and scale factors. */
struct UnkFade_80038198 {
    /* 0x00 */ float m_color[4];
    /* 0x10 */ float m_alpha[2];
    /* 0x18 */ float m_scaleRed;
    /* 0x1C */ float m_scaleGreen;
    /* 0x20 */ float m_scaleBlue;
    /* 0x24 */ u8 m_fogMode;
    /* 0x28 */ float m_lightScale;
};

/* The render callback chain: what the plugin's render callback receives and the callback it
 * hands the atomic on to. */
struct UnkRenderLink_80037D04 {
    /* 0x0 */ void *m_data;
    /* 0x4 */ RpAtomicCallBackRender m_render;
};

/* The plugin's data in each atomic. */
struct UnkAtomicExt_80037E0C {
    /* 0x00 */ RwRGBA m_color;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC[3];
    /* 0x18 */ UnkRenderLink_80037D04 m_link;
};
SIZE_ASSERT(UnkAtomicExt_80037E0C, 0x20);

/* A search for the frame of a hierarchy node id. */
struct UnkNodeSearch_80038D24 {
    /* 0x0 */ s32 m_id;
    /* 0x4 */ RwFrame *m_frame;
};

/* A search for a frame by a key the frame reports (fn_8044758C). */
struct UnkFrameSearch_80038E84 {
    /* 0x0 */ RwFrame *m_frame;
    /* 0x4 */ s32 m_key;
};

typedef RpAtomic *(*RpAtomicCallBack)(RpAtomic *atomic, void *data);
typedef RpMaterial *(*RpMaterialCallBack)(RpMaterial *material, void *data);
typedef RwFrame *(*RwFrameCallBack)(RwFrame *frame, void *data);
typedef RwTexture *(*RwTextureCallBack)(RwTexture *texture, void *data);
extern "C" {
/* RenderWare. */
RpClump *fn_80456C54(RpClump *clump, RpAtomicCallBack callback, void *data);          /* RpClumpForAllAtomics */
RpGeometry *fn_804590E8(RpGeometry *geometry, RpMaterialCallBack callback, void *data); /* RpGeometryForAllMaterials */
RpWorld *fn_8046066C(RpWorld *world, RpMaterialCallBack callback, void *data);       /* RpWorldForAllMaterials */
RwFrame *fn_8048752C(RwFrame *frame, RwFrameCallBack callback, void *data);           /* RwFrameForAllChildren */
RwFrame *fn_804872D8(RwFrame *frame);                                                 /* RwFrameGetRoot */
RwTexDictionary *fn_8048CBE0(RwTexDictionary *dict, RwTextureCallBack callback, void *data);
s32 fn_80458B6C(s32 size, u32 pluginID, void *constructor, void *destructor, void *copy);
s32 fn_80458BEC(u32 pluginID, void *readCB, void *writeCB, void *getSizeCB);
RpAtomic *fn_80455EB4(RpAtomic *atomic); /* AtomicDefaultRenderCallBack */
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);
RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length);
void *fn_8047D158(void *mem, u32 size);
void *fn_8047D254(void *mem, u32 size);
s32 fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
RwTexDictionary *fn_80483FBC(RwStream *stream);
s32 fn_8048124C(RwStream *stream, void *data);
s32 fn_80489CE4(RwRaster *raster);
void fn_8049A6E4(RwTexture *texture, float *lodBias, s32 *a, s32 *b, s32 *c);
void fn_8049A518(RwTexture *texture, float lodBias, s32 a, s32 b, s32 c);
void *fn_8049A82C(RwIm3DVertex *vertices, s32 numVertices, RwMatrix *ltm, u32 flags);
s32 fn_8049AAF8(s32 primType);
s32 fn_8049A90C(void);
s32 fn_80443A54(RpGeometry *geometry);
float *fn_804442E4(RpAtomic *atomic);
void fn_80444630(RpAtomic *atomic, float value);
void fn_804443A0(RpAtomic *atomic, void *data);
RpHAnimHierarchy *fn_8044757C(RwFrame *frame);
s32 fn_8044758C(RwFrame *frame);
void fn_804474D8(RpHAnimHierarchy *hierarchy);
void *fn_8044CCAC(RpGeometry *geometry);
RwRGBA *fn_8044CDBC(void *data);
s32 fn_804549DC(RpMaterial *material);
void fn_804548EC(RpMaterial *material, float value);
void fn_8045462C(RpMaterial *material);
s32 fn_804A2064(RpAtomic *atomic);
s32 fn_804A22E4(RpMaterial *material);
float fn_804A29A8(RpMaterial *material);
void fn_804A2888(RpMaterial *material, float value);

/* The renderer and its fog/light modes (Game/UnkRenderer_800289B0.cpp). */
UnkRenderer_800289B0 *fn_800289B0(void);
void fn_80048390(UnkRenderer_800289B0 *renderer, int mode);
void fn_800482E0(UnkRenderer_800289B0 *renderer, int mode);

RwStream *fn_80044FEC(UnkFileSystem_800371D8 *fileSystem, const char *name);
void fn_802E0658(RwTexDictionary *dict);

}

/* The current screen (Game/CameraScreenSet.cpp). */
extern s32 lbl_805F0170;

/* The fade that changes nothing (every factor negative; read by Game/EffectObjectUSA.cpp). */
extern const float lbl_804AB220[4] = {-1.0f, -1.0f, -1.0f, -1.0f};

/* Atomic flags (read by Game/GadgetParticle.cpp; meaning unknown). */
extern const u32 lbl_805F1A88 = 8;

/* The material colours a pass saves and restores, the next slot, and the light scale. */
static RwRGBA lbl_805712B8[16];
static u32 lbl_805EE0E0;
static float lbl_805EE0DC;
static float lbl_805EE0E4;
static RwFrame *lbl_805EE0D0;
static RwFrame *lbl_805EE0D4;
static u8 lbl_805EE0D8;
static s32 lbl_805EE0E8;

extern "C" {
void fn_80039920(const UnkDebugLine_80020A20 *line, const UnkColor_8040D2D8 *color);
void fn_800396D0(const RwRect *rect, const RwRGBA *color, RwRaster *raster);
RwTexDictionary *fn_80039650(void *unused, const char *name);
void fn_80039618(void *unused, RwTexDictionary *dict, s32 mode);
RwTexture *fn_80039528(RwTexture *texture, void *data);
RpMaterial *fn_80039438(RpMaterial *material, void *data);
RpAtomic *fn_800393E4(RpAtomic *atomic, void *data);
RpAtomic *fn_800393D0(RpAtomic *atomic, void *data);
RwFrame *fn_80039398(RwFrame *frame, void *data);
RpAtomic *fn_80039380(RpAtomic *atomic, void *data);
RpAtomic *fn_80039288(RpAtomic *atomic, RwFrame *root);
RpAtomic *fn_80039270(RpAtomic *atomic, void *data);
RpAtomic *fn_80039178(RpAtomic *atomic, RwFrame *root);
RwFrame *fn_80039140(RwFrame *frame, void *data);
void fn_80039114(RpClump *clump, void *data);
void fn_800390E8(RpClump *clump);
RpMaterial *fn_80039078(RpMaterial *material, void *data);
RpAtomic *fn_80039030(RpAtomic *atomic, void *data);
RwFrame *fn_80038FEC(RpClump *clump, RwFrame *frame);
void fn_80038FC0(RpClump *clump);
void fn_80038F94(RpClump *clump);
RwFrame *fn_80038EC4(RwFrame *frame, UnkFrameSearch_80038E84 *search);
RwFrame *fn_80038E84(RwFrame *frame, s32 key);
RwFrame *fn_80038DD8(RwFrame *frame, UnkNodeSearch_80038D24 *search);
RwFrame *fn_80038D24(RpClump *clump, s32 id);
RwFrame *fn_80038CC8(RwFrame *frame, void *data);
void fn_80038C84(RpClump *clump);
s32 fn_80038B58(RwTexDictionary *dict, s32 mode);
RpAtomic *fn_80038B44(RpAtomic *atomic, void *data);
void fn_80038B18(RpClump *clump);
RpAtomic *fn_80038AB8(RpAtomic *atomic);
RpAtomic *fn_80038A24(RpAtomic *atomic, void *data);
void fn_800389F8(RpClump *clump, void *mode);
RpMaterial *fn_800389A4(RpMaterial *material, void *data);
RpMaterial *fn_80038950(RpMaterial *material, void *data);
RpMaterial *fn_80038780(RpMaterial *material, void *data);
RpAtomic *fn_800386C4(RpAtomic *atomic);
RpMaterial *fn_80038620(RpMaterial *material, void *data);
RpMaterial *fn_800385B8(RpMaterial *material, void *data);
RpAtomic *fn_800384F4(RpAtomic *atomic);
RpMaterial *fn_80038484(RpMaterial *material, void *data);
RpAtomic *fn_800383B4(RpAtomic *atomic);
RpAtomic *fn_800382DC(RpAtomic *atomic);
RpAtomic *fn_800381C4(RpAtomic *atomic, void *data);
void fn_80038198(RpClump *clump, UnkFade_80038198 *fade);
void fn_8003815C(RpClump *clump);
void fn_80038120(RpWorld *world);
void *fn_8003809C(void *object, s32 offset, s32 size);
void *fn_80038098(void *object, s32 offset, s32 size);
void *fn_80038020(void *dst, const void *src, s32 offset, s32 size);
RwStream *fn_80037F4C(RwStream *stream, s32 length, void *object, s32 offset, s32 size);
RwStream *fn_80037E98(RwStream *stream, s32 length, const void *object, s32 offset, s32 size);
s32 fn_80037E90(const void *object, s32 offset, s32 size);
bool fn_80037E0C(void);
s32 fn_80037DCC(RpAtomic *atomic, const RwRGBA *color);
s32 fn_80037D80(RpAtomic *atomic, const u32 *words, s32 size);
u32 *fn_80037D5C(RpAtomic *atomic);
s32 fn_80037D28(RpAtomic *atomic, UnkRenderLink_80037D04 *link);
UnkRenderLink_80037D04 *fn_80037D04(RpAtomic *atomic);
RpMaterial *fn_80037CE0(RpMaterial *material, void *data);
RpAtomic *fn_80037C4C(RpAtomic *atomic);
RpAtomic *fn_80037BB0(RpAtomic *atomic, void *data);
void fn_80037B84(RpClump *clump, const RwRGBA &color);
RpAtomic *fn_80037B54(RpAtomic *atomic, void *data);
RpAtomic *fn_80037B20(RpAtomic *atomic, void *data);
RpAtomic *fn_80037AB4(RpAtomic *atomic, void *data);
void fn_80037A80(RpClump *clump, void *data);
void fn_80037A48(RpClump *clump, float value);
void fn_800379E8(RpClump *clump, float value);
void fn_800379E4(void);
void fn_800379E0(void);
void fn_800379DC(void);
}

/* rwcore.h's RwTextureSetFilterMode. */
#define SET_FILTER_MODE(texture, mode) ((texture)->filterAddressing = ((texture)->filterAddressing & ~0xFF) | ((u32)(mode) & 0xFF))

inline RwTexDictionary *ReadTexDictionary(RwStream *stream) {
    RwTexDictionary *dict = 0;
    if (stream != 0 && fn_8047CF50(stream, 0x16, 0, 0)) {
        dict = fn_80483FBC(stream);
        fn_802E0658(dict);
    }
    return dict;
}

/* Loads the texture dictionary in file name. */
extern "C" RwTexDictionary *fn_80039650(void *unused, const char *name) {
    RwTexDictionary *dict = 0;
    RwStream *stream = fn_80044FEC(fn_800371D8(), name);
    if (stream != 0) {
        dict = ReadTexDictionary(stream);
        fn_8048124C(stream, 0);
    }
    return dict;
}

/* Sets the filter mode of every texture of dict (mode 6: trilinear). */
extern "C" void fn_80039618(void *unused, RwTexDictionary *dict, s32 mode) {
    lbl_805EE0E8 = mode;
    fn_8048CBE0(dict, fn_80039528, 0);
}

extern "C" RwTexture *fn_80039528(RwTexture *texture, void *data) {
    float lodBias;
    s32 a;
    s32 b;
    s32 c;
    fn_8049A6E4(texture, &lodBias, &a, &b, &c);
    RwRaster *raster = texture->raster;
    if (fn_80489CE4(raster) > 1) {
        SET_FILTER_MODE(texture, lbl_805EE0E8);
        u32 format = (raster->cFormat & 0xFF) << 8;
        if (format & 0x6000) {
            fn_8049A518(texture, lodBias, a, 1, 0);
        } else if ((format & 0x9000) && lbl_805EE0E8 == 6) {
            fn_8049A518(texture, lodBias, a, 1, 2);
        }
    } else if (lbl_805EE0E8 == 6) {
        SET_FILTER_MODE(texture, 2);
    } else {
        SET_FILTER_MODE(texture, 1);
    }
    return texture;
}

/* Sets a material's colour from four factors (each when not negative). */
extern "C" RpMaterial *fn_80039438(RpMaterial *material, void *data) {
    const float *factors = (const float *)data;
    if (factors == 0) {
        return material;
    }
    RwRGBA color = material->color;
    if (factors[3] >= 0.0f) {
        color.alpha = 255.0f * factors[3];
    }
    if (factors[0] >= 0.0f) {
        color.red = 255.0f * factors[0];
    }
    if (factors[1] >= 0.0f) {
        color.green = 255.0f * factors[1];
    }
    if (factors[2] >= 0.0f) {
        color.blue = 255.0f * factors[2];
    }
    material->color = color;
    return material;
}

extern "C" RpAtomic *fn_800393E4(RpAtomic *atomic, void *data) {
    RpGeometry *geometry = atomic->geometry;
    fn_804590E8(geometry, fn_80039438, data);
    geometry->flags |= 0x40;
    return atomic;
}

extern "C" RpAtomic *fn_800393D0(RpAtomic *atomic, void *data) {
    atomic->geometry->flags |= 0x40;
    return atomic;
}

/* Finds the first frame below a root that comes after lbl_805EE0D0 (the search clears it when it
 * meets it). */
extern "C" RwFrame *fn_80039398(RwFrame *frame, void *data) {
    if (*(RwFrame **)data == 0) {
        if (lbl_805EE0D0 == frame) {
            lbl_805EE0D0 = 0;
        } else if (lbl_805EE0D0 == 0) {
            *(RwFrame **)data = frame;
            lbl_805EE0D0 = frame;
        }
    }
    return frame;
}

extern "C" RpAtomic *fn_80039380(RpAtomic *atomic, void *data) {
    if (!(atomic->flags & 8)) {
        atomic->flags &= ~4;
    }
    return atomic;
}

extern "C" RwFrame *fn_80039140(RwFrame *frame, void *data);

/* Whether frame is below root (the search starts after lbl_805EE0D4). */
inline RwFrame *FindBelow(RwFrame *root, RwFrame *frame) {
    RwFrame *found = 0;
    lbl_805EE0D4 = frame;
    fn_8048752C(root, fn_80039140, &found);
    return found;
}

/* NonMatching (as fn_80039178): retail gives root r30 and the geometry r28 (swapped here). */
extern "C" RpAtomic *fn_80039288(RpAtomic *atomic, RwFrame *root) {
    RpGeometry *geometry = atomic->geometry;
    RwFrame *frame = atomic->frame;
    void *colors = fn_8044CCAC(geometry);
    if (colors != 0) {
        RwRGBA *color = fn_8044CDBC(colors);
        s32 count = geometry->numVertices;
        if (color != 0) {
            for (int i = 0; i <= count; i++) {
                if (color->red == lbl_805EE0D8 || color->green == lbl_805EE0D8 || color->blue == lbl_805EE0D8 ||
                    (color++)->alpha == lbl_805EE0D8) {
                    atomic->flags &= ~4;
                    break;
                }
            }
        }
    } else if (root == frame || FindBelow(root, frame) != 0) {
        atomic->flags &= ~4;
    }
    return atomic;
}

extern "C" RpAtomic *fn_80039270(RpAtomic *atomic, void *data) {
    if (!(atomic->flags & 8)) {
        atomic->flags |= 4;
    }
    return atomic;
}

/* NonMatching: retail gives root r30 and the geometry r28 (swapped here); ~6 shapes tried. */
extern "C" RpAtomic *fn_80039178(RpAtomic *atomic, RwFrame *root) {
    RpGeometry *geometry = atomic->geometry;
    RwFrame *frame = atomic->frame;
    void *colors = fn_8044CCAC(geometry);
    if (colors != 0) {
        RwRGBA *color = fn_8044CDBC(colors);
        s32 count = geometry->numVertices;
        if (color != 0) {
            for (int i = 0; i <= count; i++) {
                if (color->red == lbl_805EE0D8 || color->green == lbl_805EE0D8 || color->blue == lbl_805EE0D8 ||
                    (color++)->alpha == lbl_805EE0D8) {
                    atomic->flags |= 4;
                    break;
                }
            }
        }
    } else if (root == frame || FindBelow(root, frame) != 0) {
        atomic->flags |= 4;
    }
    return atomic;
}

extern "C" RwFrame *fn_80039140(RwFrame *frame, void *data) {
    if (*(RwFrame **)data == 0) {
        if (lbl_805EE0D4 == frame) {
            lbl_805EE0D4 = 0;
        } else if (lbl_805EE0D4 == 0) {
            *(RwFrame **)data = frame;
            lbl_805EE0D4 = frame;
        }
    }
    return frame;
}

extern "C" void fn_80039114(RpClump *clump, void *data) {
    fn_80456C54(clump, fn_800393E4, data);
}

extern "C" void fn_800390E8(RpClump *clump) {
    fn_80456C54(clump, fn_800393D0, 0);
}

extern "C" RpMaterial *fn_80039078(RpMaterial *material, void *data) {
    if (fn_804A22E4(material) == 2) {
        float value = lbl_805EE0E4 * fn_804A29A8(material);
        if (value < 0.0f) {
            value = 0.0f;
        }
        if (value > 1.0f) {
            value = 1.0f;
        }
        fn_804A2888(material, value);
    }
    return material;
}

extern "C" RpAtomic *fn_80039030(RpAtomic *atomic, void *data) {
    if (atomic->geometry != 0) {
        fn_804590E8(atomic->geometry, fn_80039078, 0);
    }
    return atomic;
}

extern "C" RwFrame *fn_80038FEC(RpClump *clump, RwFrame *frame) {
    RwFrame *found = 0;
    lbl_805EE0D0 = frame;
    if (clump != 0) {
        fn_80456C54(clump, (RpAtomicCallBack)fn_80039398, &found);
    }
    return found;
}

extern "C" void fn_80038FC0(RpClump *clump) {
    fn_80456C54(clump, fn_80039380, 0);
}

extern "C" void fn_80038F94(RpClump *clump) {
    fn_80456C54(clump, fn_80039270, 0);
}

extern "C" void fn_80038F60(RpClump *clump, RwFrame *frame, s32 value) {
    lbl_805EE0D8 = value;
    fn_80456C54(clump, (RpAtomicCallBack)fn_80039288, frame);
}

extern "C" void fn_80038F2C(RpClump *clump, RwFrame *frame, s32 value) {
    lbl_805EE0D8 = value;
    fn_80456C54(clump, (RpAtomicCallBack)fn_80039178, frame);
}

extern "C" RwFrame *fn_80038EC4(RwFrame *frame, UnkFrameSearch_80038E84 *search) {
    if (search->m_frame == 0) {
        if (search->m_key == fn_8044758C(frame)) {
            search->m_frame = frame;
        } else {
            fn_8048752C(frame, (RwFrameCallBack)fn_80038EC4, search);
        }
    }
    return frame;
}

extern "C" RwFrame *fn_80038E84(RwFrame *frame, s32 key) {
    UnkFrameSearch_80038E84 search;
    search.m_frame = 0;
    search.m_key = key;
    fn_8048752C(frame, (RwFrameCallBack)fn_80038EC4, &search);
    return search.m_frame;
}

inline RwFrame *FindNodeFrame(RwFrame *frame, UnkNodeSearch_80038D24 *search) {
    if (search->m_frame != 0) {
        return frame;
    }
    RpHAnimHierarchy *hierarchy = fn_8044757C(frame);
    if (hierarchy != 0) {
        for (int i = 0; i < hierarchy->numNodes; i++) {
            if (hierarchy->pNodeInfo[i].pFrame != 0 && search->m_id == hierarchy->pNodeInfo[i].nodeID) {
                search->m_frame = hierarchy->pNodeInfo[i].pFrame;
                return frame;
            }
        }
    }
    fn_8048752C(frame, (RwFrameCallBack)fn_80038DD8, search);
    return frame;
}

/* NonMatching (with fn_80038D24, which inlines it): the loop's volatile registers are allocated in
 * another order in retail; ~8 shapes tried. */
extern "C" RwFrame *fn_80038DD8(RwFrame *frame, UnkNodeSearch_80038D24 *search) {
    return FindNodeFrame(frame, search);
}

/* The frame of node id of clump's hierarchy. */
extern "C" RwFrame *fn_80038D24(RpClump *clump, s32 id) {
    UnkNodeSearch_80038D24 search;
    RwFrame *root = fn_804872D8(clump->frame);
    search.m_id = id;
    search.m_frame = 0;
    FindNodeFrame(root, &search);
    return search.m_frame;
}

extern "C" RwFrame *fn_80038CC8(RwFrame *frame, void *data) {
    RpHAnimHierarchy *hierarchy = fn_8044757C(frame);
    if (hierarchy == 0) {
        fn_8048752C(frame, fn_80038CC8, data);
        return frame;
    }
    *(RpHAnimHierarchy **)data = hierarchy;
    return frame;
}

extern "C" void fn_80038C84(RpClump *clump) {
    RpHAnimHierarchy *hierarchy = 0;
    fn_8048752C(fn_804872D8(clump->frame), fn_80038CC8, &hierarchy);
    fn_804474D8(hierarchy);
}

/* NonMatching: retail gives mode r31 and the raster r30 (swapped here); ~6 shapes tried. */
extern "C" RwTexture *fn_80038B88(RwTexture *texture, s32 mode) {
    float lodBias;
    s32 a;
    s32 b;
    s32 c;
    fn_8049A6E4(texture, &lodBias, &a, &b, &c);
    RwRaster *raster = texture->raster;
    if (fn_80489CE4(raster) > 1) {
        SET_FILTER_MODE(texture, mode);
        u32 format = (raster->cFormat & 0xFF) << 8;
        if (format & 0x6000) {
            fn_8049A518(texture, lodBias, a, 1, 0);
        } else if (format & 0x9000) {
            fn_8049A518(texture, lodBias, 0, 0, 0);
        }
    } else {
        if (mode == 6) {
            SET_FILTER_MODE(texture, 2);
        } else {
            SET_FILTER_MODE(texture, 1);
        }
        fn_8049A518(texture, lodBias, a, 1, 0);
    }
    return texture;
}

extern "C" s32 fn_80038B58(RwTexDictionary *dict, s32 mode) {
    fn_8048CBE0(dict, (RwTextureCallBack)fn_80038B88, (void *)mode);
    return 1;
}

extern "C" RpAtomic *fn_80038B44(RpAtomic *atomic, void *data) {
    atomic->geometry->flags &= ~0x20;
    return atomic;
}

extern "C" void fn_80038B18(RpClump *clump) {
    fn_80456C54(clump, fn_80038B44, 0);
}

extern "C" RpAtomic *fn_80038AB8(RpAtomic *atomic) {
    UnkRenderLink_80037D04 *link = fn_80037D04(atomic);
    fn_80048390(fn_800289B0(), (s8)(s32)link->m_data);
    UnkRenderer_800289B0 *renderer = fn_800289B0();
    fn_800482E0(renderer, renderer->field_0x456);
    link->m_render(atomic);
    return atomic;
}

extern "C" RpAtomic *fn_80038A24(RpAtomic *atomic, void *data) {
    UnkRenderLink_80037D04 link;
    if (atomic->renderCallBack == fn_80038AB8) {
        link = *fn_80037D04(atomic);
    } else {
        link.m_render = atomic->renderCallBack;
    }
    link.m_data = data;
    fn_80037D28(atomic, &link);
    atomic->renderCallBack = fn_80038AB8;
    if (atomic->renderCallBack == 0) {
        atomic->renderCallBack = fn_80455EB4;
    }
    return atomic;
}

/* Renders clump's atomics with fog/light mode (s8)mode. */
extern "C" void fn_800389F8(RpClump *clump, void *mode) {
    fn_80456C54(clump, fn_80038A24, mode);
}

/* Saves a material's colour. */
extern "C" RpMaterial *fn_800389A4(RpMaterial *material, void *data) {
    if (lbl_805EE0E0 >= 16) {
        lbl_805EE0E0 = 15;
    }
    lbl_805712B8[lbl_805EE0E0++] = material->color;
    return material;
}

/* Restores a material's colour (in the order they were saved). */
extern "C" RpMaterial *fn_80038950(RpMaterial *material, void *data) {
    if (lbl_805EE0E0 >= 16) {
        lbl_805EE0E0 = 15;
    }
    material->color = lbl_805712B8[lbl_805EE0E0++];
    return material;
}

/* Applies a fade's colour to a material. */
/* NonMatching (49 instructions): retail addresses the alpha factors through a base it computes
 * first (fade + 0x10) and schedules the second half differently; ~8 shapes tried. */
extern "C" RpMaterial *fn_80038780(RpMaterial *material, void *data) {
    UnkFade_80038198 *fade = (UnkFade_80038198 *)data;
    if (fade == 0) {
        return material;
    }
    fn_800389A4(material, 0);
    RwRGBA color = material->color;
    if (fade->m_color[0] >= 0.0f) {
        color.red = 255.0f * fade->m_color[0];
    }
    if (fade->m_color[1] >= 0.0f) {
        color.green = 255.0f * fade->m_color[1];
    }
    if (fade->m_color[2] >= 0.0f) {
        color.blue = 255.0f * fade->m_color[2];
    }
    if (fade->m_color[3] >= 0.0f) {
        color.alpha = 255.0f * fade->m_color[3];
    }
    color.red = color.red * fade->m_scaleRed + 0.5f;
    color.green = color.green * fade->m_scaleGreen + 0.5f;
    color.blue = color.blue * fade->m_scaleBlue + 0.5f;
    color.alpha = color.alpha * fade->m_alpha[lbl_805F0170] + 0.5f;
    material->color = color;
    return material;
}

extern "C" RpAtomic *fn_800386C4(RpAtomic *atomic) {
    UnkRenderLink_80037D04 *link = fn_80037D04(atomic);
    UnkFade_80038198 *fade = (UnkFade_80038198 *)link->m_data;
    RpGeometry *geometry = 0;
    if (fade != 0) {
        fn_80048390(fn_800289B0(), (s8)fade->m_fogMode);
        UnkRenderer_800289B0 *renderer = fn_800289B0();
        fn_800482E0(renderer, renderer->field_0x456);
        geometry = atomic->geometry;
        if (geometry != 0) {
            lbl_805EE0E0 = 0;
            fn_804590E8(geometry, fn_80038780, fade);
        }
    }
    link->m_render(atomic);
    if (geometry != 0) {
        lbl_805EE0E0 = 0;
        fn_804590E8(geometry, fn_80038950, 0);
    }
    return atomic;
}

extern "C" RpMaterial *fn_80038620(RpMaterial *material, void *data) {
    UnkFade_80038198 *fade = (UnkFade_80038198 *)data;
    if (fade == 0) {
        return material;
    }
    fn_80038780(material, fade);
    RwRGBA color = material->color;
    if (fn_804A22E4(material) == 2) {
        lbl_805EE0DC = fn_804A29A8(material);
        float value = lbl_805EE0DC * ((1.0f / 255.0f) * color.alpha);
        if (value >= 0.0f) {
            fn_804A2888(material, value);
        }
    }
    return material;
}

extern "C" RpMaterial *fn_800385B8(RpMaterial *material, void *data) {
    fn_80038950(material, 0);
    if (lbl_805EE0DC < 0.0f) {
        return material;
    }
    if (fn_804A22E4(material) == 2) {
        fn_804A2888(material, lbl_805EE0DC);
    }
    return material;
}

extern "C" RpAtomic *fn_800384F4(RpAtomic *atomic) {
    UnkRenderLink_80037D04 *link = fn_80037D04(atomic);
    UnkFade_80038198 *fade = (UnkFade_80038198 *)link->m_data;
    RpGeometry *geometry = 0;
    lbl_805EE0DC = -1.0f;
    if (fade != 0) {
        fn_80048390(fn_800289B0(), (s8)fade->m_fogMode);
        UnkRenderer_800289B0 *renderer = fn_800289B0();
        fn_800482E0(renderer, renderer->field_0x456);
        geometry = atomic->geometry;
        if (geometry != 0) {
            lbl_805EE0E0 = 0;
            fn_804590E8(geometry, fn_80038620, fade);
        }
    }
    link->m_render(atomic);
    if (geometry != 0) {
        lbl_805EE0E0 = 0;
        fn_804590E8(geometry, fn_800385B8, 0);
    }
    return atomic;
}

extern "C" RpMaterial *fn_80038484(RpMaterial *material, void *data) {
    UnkFade_80038198 *fade = (UnkFade_80038198 *)data;
    if (fade == 0) {
        return material;
    }
    if (fn_804549DC(material)) {
        fn_804548EC(material, fade->m_lightScale);
        s32 type = fn_804A22E4(material);
        if (type == 5 || type == 6) {
            fn_8045462C(material);
        }
    }
    return material;
}

extern "C" RpAtomic *fn_800383B4(RpAtomic *atomic) {
    UnkRenderLink_80037D04 *link = fn_80037D04(atomic);
    UnkFade_80038198 *fade = (UnkFade_80038198 *)link->m_data;
    RpGeometry *geometry = 0;
    if (fade != 0) {
        fn_80048390(fn_800289B0(), (s8)fade->m_fogMode);
        UnkRenderer_800289B0 *renderer = fn_800289B0();
        fn_800482E0(renderer, renderer->field_0x456);
        geometry = atomic->geometry;
        if (geometry != 0) {
            lbl_805EE0E0 = 0;
            fn_804590E8(geometry, fn_80038780, fade);
            fn_804590E8(geometry, fn_80038484, fade);
        }
    }
    link->m_render(atomic);
    if (geometry != 0) {
        lbl_805EE0E0 = 0;
        fn_804590E8(geometry, fn_80038950, 0);
    }
    return atomic;
}

extern "C" RpAtomic *fn_800382DC(RpAtomic *atomic) {
    UnkRenderLink_80037D04 *link = fn_80037D04(atomic);
    UnkFade_80038198 *fade = (UnkFade_80038198 *)link->m_data;
    RpGeometry *geometry = 0;
    lbl_805EE0DC = -1.0f;
    if (fade != 0) {
        fn_80048390(fn_800289B0(), (s8)fade->m_fogMode);
        UnkRenderer_800289B0 *renderer = fn_800289B0();
        fn_800482E0(renderer, renderer->field_0x456);
        geometry = atomic->geometry;
        if (geometry != 0) {
            lbl_805EE0E0 = 0;
            fn_804590E8(geometry, fn_80038620, fade);
        }
        fn_804590E8(geometry, fn_80038484, fade);
    }
    link->m_render(atomic);
    if (geometry != 0) {
        lbl_805EE0E0 = 0;
        fn_804590E8(geometry, fn_800385B8, 0);
    }
    return atomic;
}

extern "C" RpAtomic *fn_800381C4(RpAtomic *atomic, void *data) {
    UnkFade_80038198 *fade = (UnkFade_80038198 *)data;
    RpAtomicCallBackRender previous = atomic->renderCallBack;
    RpAtomicCallBackRender render;
    if (fn_804A2064(atomic)) {
        if (fade != 0 && fade->m_lightScale >= 0.0f) {
            render = fn_800382DC;
        } else {
            render = fn_800384F4;
        }
    } else if (fade != 0 && fade->m_lightScale >= 0.0f) {
        render = fn_800383B4;
    } else {
        render = fn_800386C4;
    }
    UnkRenderLink_80037D04 link;
    if (previous == render) {
        link = *fn_80037D04(atomic);
    } else {
        link.m_render = previous;
    }
    link.m_data = fade;
    fn_80037D28(atomic, &link);
    atomic->renderCallBack = render;
    if (atomic->renderCallBack == 0) {
        atomic->renderCallBack = fn_80455EB4;
    }
    atomic->geometry->flags |= 0x40;
    return atomic;
}

/* Renders clump's atomics with fade. */
extern "C" void fn_80038198(RpClump *clump, UnkFade_80038198 *fade) {
    fn_80456C54(clump, fn_800381C4, fade);
}

extern "C" void fn_8003815C(RpClump *clump) {
    lbl_805EE0E4 = 0.45f;
    if (clump != 0) {
        fn_80456C54(clump, fn_80039030, 0);
    }
}

extern "C" void fn_80038120(RpWorld *world) {
    lbl_805EE0E4 = 0.45f;
    if (world != 0) {
        fn_8046066C(world, fn_80039078, 0);
    }
}

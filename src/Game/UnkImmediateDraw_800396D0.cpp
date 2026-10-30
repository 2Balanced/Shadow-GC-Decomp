/* Immediate-mode draws (file and names inferred; no class): a 2D rectangle and the debug view's 3D
 * lines (Game/DebugDispLine.cpp). Definitions are in reverse
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

/* Opaque white (unused here). */
UnkColor_8040D2D8 lbl_805EE0F0(0xFF, 0xFF, 0xFF, 0xFF);

static RwIm2DVertex lbl_80571418[4];
static RwIm3DVertex lbl_805712F8[8];

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


inline void SetPosition(RwIm3DVertex *vertex, float x, float y, float z) {
    vertex->objVertex.x = x;
    vertex->objVertex.y = y;
    vertex->objVertex.z = z;
}

/* NonMatching (4 instructions): retail loads the line's y before its z for both points (the
 * registers match); ~6 shapes tried. */
/* Draws a line in 3D (the debug view's lines; Game/DebugDispLine.cpp). */
extern "C" void fn_80039920(const UnkDebugLine_80020A20 *line, const UnkColor_8040D2D8 *color) {
    void *zWrite;
    void *zTest;
    void *cull;
    lbl_805F15B4->fpRenderStateGet(8, &zWrite);
    lbl_805F15B4->fpRenderStateGet(14, &zTest);
    lbl_805F15B4->fpRenderStateGet(20, &cull);
    lbl_805F15B4->fpRenderStateSet(20, (void *)1);
    lbl_805F15B4->fpRenderStateSet(8, (void *)0);
    lbl_805F15B4->fpRenderStateSet(14, (void *)0);
    lbl_805F15B4->fpRenderStateSet(10, (void *)5);
    lbl_805F15B4->fpRenderStateSet(11, (void *)2);
    lbl_805F15B4->fpRenderStateSet(12, (void *)1);
    lbl_805F15B4->fpRenderStateSet(1, (void *)0);
    RwRGBA rgba = ToRwRGBA(*color);
    SetPosition(&lbl_805712F8[0], line->m_start.x, line->m_start.y, line->m_start.z);
    SetPosition(&lbl_805712F8[1], line->m_end.x, line->m_end.y, line->m_end.z);
    lbl_805712F8[0].u = 0.0f;
    lbl_805712F8[0].v = 0.0f;
    lbl_805712F8[1].u = 0.0f;
    lbl_805712F8[1].v = 0.0f;
    lbl_805712F8[0].color = rgba;
    lbl_805712F8[1].color = rgba;
    if (fn_8049A82C(lbl_805712F8, 2, 0, rgba.alpha == 0xFF ? 2 : 0) != 0) {
        fn_8049AAF8(1);
        fn_8049A90C();
    }
    lbl_805F15B4->fpRenderStateSet(20, cull);
    lbl_805F15B4->fpRenderStateSet(8, zWrite);
    lbl_805F15B4->fpRenderStateSet(14, zTest);
}

/* Draws a rectangle in 2D in color, textured with raster when it is set. */
extern "C" void fn_800396D0(const RwRect *rect, const RwRGBA *color, RwRaster *raster) {
    void *vertexAlpha;
    lbl_805F15B4->fpRenderStateGet(12, &vertexAlpha);
    lbl_805F15B4->fpRenderStateSet(1, raster);
    lbl_805F15B4->fpRenderStateSet(12, (void *)1);
    float z = lbl_805F15B4->nearScreenZ;
    lbl_80571418[0].x = rect->x;
    lbl_80571418[0].y = rect->y;
    lbl_80571418[0].z = z;
    lbl_80571418[1].x = rect->x;
    lbl_80571418[1].y = (float)rect->y + (float)rect->h;
    lbl_80571418[1].z = z;
    lbl_80571418[2].x = (float)rect->x + (float)rect->w;
    lbl_80571418[2].y = rect->y;
    lbl_80571418[2].z = z;
    lbl_80571418[3].x = (float)rect->x + (float)rect->w;
    lbl_80571418[3].y = (float)rect->y + (float)rect->h;
    lbl_80571418[3].z = z;
    if (raster != 0) {
        lbl_80571418[0].u = 0.0f;
        lbl_80571418[0].v = 0.0f;
        lbl_80571418[1].u = 0.0f;
        lbl_80571418[1].v = 1.0f;
        lbl_80571418[2].u = 1.0f;
        lbl_80571418[2].v = 0.0f;
        lbl_80571418[3].u = 1.0f;
        lbl_80571418[3].v = 1.0f;
    }
    for (int i = 0; i < 4; i++) {
        lbl_80571418[i].emissiveColor.red = color->red;
        lbl_80571418[i].emissiveColor.green = color->green;
        lbl_80571418[i].emissiveColor.blue = color->blue;
        lbl_80571418[i].emissiveColor.alpha = color->alpha;
    }
    lbl_805F15B4->fpIm2DRenderPrimitive(4, lbl_80571418, 4);
    lbl_805F15B4->fpRenderStateSet(12, vertexAlpha);
}

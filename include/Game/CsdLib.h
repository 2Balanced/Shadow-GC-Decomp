#ifndef GAME_CSDLIB_H
#define GAME_CSDLIB_H

/* The internals of the Chao CSD 2D library shared by its units (Game/csdLoader.cpp,
 * Game/csdMotionPalette.cpp, ...): a loaded project's scenes, layers and casts, their run-time
 * state, and the library functions they call. The library has no RTTI, so every name here is
 * inferred, except CTexList (from the RTTI of RCPtr<CTexList>::RCObject<CTexList> in game code). */

#include <dolphin/types.h>

#include "Game/CsdRCPtr.h"

extern "C" {
/* MSL bsearch (the symbol is still a placeholder). */
void *fn_803A7B40(const void *key, const void *base, u32 count, u32 size,
                  int (*compare)(const void *, const void *));
/* Prints a debug message (behaviour inferred). */
void fn_80361DC4(const char *format, ...);
/* The library allocator (Game/csdBase.cpp). */
void fn_80360528(void *p);
void *fn_80360564(u32 size, const char *file, int line);
}

namespace Chao {
namespace CSD {

/* Does nothing (an inline call that keeps its caller out of line). */
inline void __csd_nothing() {}

/* An entry of a sorted name table (see Game/csdAccess.cpp). */
struct SNameIndex {
    /* 0x0 */ const char *m_name;
    /* 0x4 */ s32 m_index;
};

/* A cast attribute value (see Game/csdAccess.cpp). */
union CValue {
    s32 m_int;
    f32 m_float;
};

/* A key of an animation track. */
struct SKey {
    /* 0x00 */ s32 m_frame;
    /* 0x04 */ CValue m_value;
    /* 0x08 */ s32 m_interp;
    /* 0x0C */ f32 m_tangentIn;
    /* 0x10 */ f32 m_tangentOut;
    /* 0x14 */ s32 field_0x14;
};

/* The animation track of one cast attribute. */
struct STrack {
    /* 0x0 */ s32 m_loop;
    /* 0x4 */ s32 m_keyCount;
    /* 0x8 */ SKey *m_keys;
};

struct SVec2 {
    f32 m_x;
    f32 m_y;
};

/* A vertex of an untextured cast. */
struct SVertex {
    /* 0x0 */ SVec2 m_pos;
    /* 0x8 */ u32 m_color;
};

/* A vertex of a textured cast. */
struct SVertexT {
    /* 0x00 */ SVec2 m_pos;
    /* 0x08 */ u32 m_color;
    /* 0x0C */ SVec2 m_uv;
};

/* A texture pattern: the texture and its rectangle. */
struct SPattern {
    /* 0x0 */ s32 m_texture;
    /* 0x4 */ SVec2 m_uv[2];
};

struct SLayerInfo;

/* A scene of a loaded project (the part read here). */
struct SSceneInfo {
    /* 0x00 */ s32 m_version;
    /* 0x04 */ u8 field_0x4[0x18 - 0x4];
    /* 0x18 */ SVec2 *m_textureSizes;
    /* 0x1C */ u8 field_0x1C[4];
    /* 0x20 */ SPattern *m_patterns;
    /* 0x24 */ s32 m_layerCount;
    /* 0x28 */ SLayerInfo *m_layers;
    /* 0x2C */ u8 field_0x2C[0x40 - 0x2C];
    /* 0x40 */ f32 m_aspect;
};

/* A cast of a layer as stored in the project. */
struct SCastInfo {
    /* 0x00 */ s32 m_version;
    /* 0x04 */ s32 m_type;
    /* 0x08 */ void *field_0x8;
    /* 0x0C */ SVec2 m_corners[4];
    /* 0x2C */ u32 m_attributes;
    /* 0x30 */ CValue *m_values;
    /* 0x34 */ u32 m_mode;
    /* 0x38 */ u32 m_flags;
    /* 0x3C */ s32 m_patternCount;
    /* 0x40 */ s32 *m_patternTable;
    /* 0x44 */ const char *m_text;
    /* 0x48 */ const char *m_font;
    /* 0x4C */ f32 m_textWidth;
};

/* A 2x3 matrix with two optional companion matrices: the matrix without the translation, and the
 * rotation alone (Game/csdMatrix.cpp). */
struct SMatrix {
    /* 0x00 */ f32 m[6];
    /* 0x18 */ f32 *m_scale;
    /* 0x1C */ f32 *m_base;
};

struct SLayer;

/* A cast at run time (0x144 bytes; see InitCast in Game/csdAccess.cpp). Its attribute values
 * (0x28..0x60) are the CValue list csdAccess.cpp's GetAttribute fills. */
struct SCast {
    /* 0x00 */ SMatrix m_matrix;
    /* 0x20 */ s32 m_hide;
    /* 0x24 */ u32 m_color;
    /* 0x28 */ s32 m_attrHide;
    /* 0x2C */ f32 m_x;
    /* 0x30 */ f32 m_y;
    /* 0x34 */ f32 m_rotation;
    /* 0x38 */ f32 m_scaleX;
    /* 0x3C */ f32 m_scaleY;
    /* 0x40 */ f32 m_pattern;
    /* 0x44 */ u32 m_attrColor;
    /* 0x48 */ u32 m_cornerColors[4];
    /* 0x58 */ s32 m_params[3];
    /* 0x64 */ const char *m_text;
    /* 0x68 */ const char *m_font;
    /* 0x6C */ f32 field_0x6C;
    /* 0x70 */ f32 m_textWidth;
    /* 0x74 */ u32 m_overrides;
    /* 0x78 */ CValue *m_overrideValues;
    /* 0x7C */ s32 m_changed;
    /* 0x80 */ SCast *m_parent;
    /* 0x84 */ SLayer *m_layer;
    /* 0x88 */ s32 m_quadCount;
    /* 0x8C */ void *m_vertices;
    /* 0x90 */ void *m_transformed;
    /* 0x94 */ s32 *m_textures;
    /* 0x98 */ u8 m_vertexBuffer[0x50];
    /* 0xE8 */ u8 m_transformedBuffer[0x50];
    /* 0x138 */ s32 m_textureBuffer;
    /* 0x13C */ s32 m_allocated;
    /* 0x140 */ s32 m_updated;
};

/* The animation of a cast: the attributes it animates and their tracks. */
struct SCastMotion {
    /* 0x0 */ u32 m_attributes;
    /* 0x4 */ STrack *m_tracks;
};

/* A node of a layer's cast tree. */
struct SCastLink {
    /* 0x0 */ s32 m_child;
    /* 0x4 */ s32 m_sibling;
};

/* A layer of a scene as stored in the project. */
struct SLayerInfo {
    /* 0x0 */ s32 m_castCount;
    /* 0x4 */ SCastInfo **m_casts;
    /* 0x8 */ s32 m_root;
    /* 0xC */ SCastLink *m_tree;
};

/* The animation of a layer. */
struct SLayerMotion {
    /* 0x0 */ u8 field_0x0[4];
    /* 0x4 */ SCastMotion *m_casts;
};

/* The animation of a scene. */
struct SMotion {
    /* 0x0 */ u8 field_0x0[4];
    /* 0x4 */ SLayerMotion *m_layers;
};

struct SSceneState;

/* A layer of a scene at run time. */
struct SLayer {
    /* 0x0 */ s32 m_castCount;
    /* 0x4 */ SCast *m_casts;
    /* 0x8 */ s32 m_updated;
    /* 0xC */ SSceneState *m_scene;
};

/* A scene at run time. */
struct SSceneState {
    /* 0x00 */ s32 m_layerCount;
    /* 0x04 */ SLayer *m_layers;
    /* 0x08 */ s32 m_updated;
    /* 0x0C */ f32 m_frame;
    /* 0x10 */ f32 m_speed;
    /* 0x14 */ SCast *m_parent;
    /* 0x18 */ f32 m_screenSize[2];
    /* 0x20 */ f32 m_screenBase[2];
    /* 0x28 */ f32 m_screenOffset[2];
    /* 0x30 */ s32 m_refresh;
    /* 0x34 */ const struct SMotion *m_motion;
    /* 0x38 */ void *m_texList;
    /* 0x3C */ void *m_matrices;
};

/* CMemoryAlloc (class name from the RTTI of game code, which derives TCsdSystem from it): the
 * library's memory allocator, of which the game installs one (Game/csdRW.cpp; the game's own code
 * declares it as UnkCsdAllocator_803605AC in Game/CsdRCPtr.h). A derived class supplies Allocate
 * and Deallocate; the rest are inline here, and the game links one copy of each (0x8003DF84...).
 * Names other than the class name are inferred. */
class CMemoryAlloc {
  public:
    CMemoryAlloc() {}
    virtual ~CMemoryAlloc() {}
    virtual void *Allocate(u32 size) = 0;
    virtual void Deallocate(void *p) = 0;
    virtual void *Alloc(u32 size, const char *file, int line) {
        return Allocate(size);
    }
    virtual void Free(void *p) {
        Deallocate(p);
    }
    virtual void *AllocDebug(u32 size, const char *file, int line) {
        return Allocate(size);
    }
    virtual void FreeDebug(void *p) {
        Deallocate(p);
    }
};

/* The texture list of a project (Game/csdTexList.cpp; virtual functions inferred). It is abstract:
 * every slot of its own virtual table is zero, the destructor's included. A texture list of each
 * type is registered as the prototype that makes the lists of its type. */
class CTexList {
  public:
    /* RCPtr's constructor is inline in the library's header and calls another inline function, so
     * this constructor is not inlined; the empty inline call does the same here. */
    CTexList() {
        __csd_nothing();
    }
    virtual ~CTexList() = 0;
    /* The type the list is registered by, and a new list of that type. */
    virtual u32 GetType() = 0;
    virtual RCPtr<CTexList> Create() = 0;
    /* Reads the textures named by a texture list chunk of size bytes. */
    virtual s32 Load(const void *data, u32 size) = 0;
    virtual void SetTexture(s32 index, const void *state, s32 count) = 0;
    virtual void SetFilter(s32 index, s32 filter, const void *state, s32 count) = 0;
    virtual void SetWrap(s32 s, s32 t, const void *state, s32 count) = 0;

    static void *operator new(u32 size) {
        return fn_803605AC()->Alloc(size, "csdBase.h", 40);
    }
    static void operator delete(void *p) {
        fn_803605AC()->Free(p);
    }

    /* 0x4 */ RCPtr<unsigned char> m_data;
};

/* The platform layer (Game/csdPlatform.cpp; CPlatformRW, its RenderWare implementation, is named
 * by the library's assert messages; virtual functions inferred). */
class CPlatform {
  public:
    /* (The empty inline call stands in for the inline base constructor the library's classes call,
     * which keeps their constructors out of line; see CTexList.) */
    CPlatform() : m_beginCount(0) {
        __csd_nothing();
    }
    virtual ~CPlatform() {}
    virtual void Print(const char *message) = 0;
    virtual s32 IsSwapped() = 0;
    virtual void SetBlend(s32 blend, const void *state, s32 count) = 0;
    virtual void SetAlphaTest(s32 alpha, const void *state, s32 count) = 0;
    virtual void DrawTextured(const void *vertices, s32 count, const void *state, s32 count2) = 0;
    virtual void BeginTextured(const void *state, s32 count) = 0;
    virtual void SetAlphaTestOff(s32 alpha, const void *state, s32 count) = 0;
    virtual void Draw(const void *vertices, s32 count, const void *state, s32 count2) = 0;
    virtual void BeginColored(const void *state, s32 count) = 0;
    virtual void Begin() = 0;
    virtual void End() = 0;

    static void operator delete(void *p) {
        fn_803605AC()->Free(p);
    }

    /* 0x4 */ s32 m_beginCount;
};

/* Game/csdAccess.cpp */
int CompareName(const void *a, const void *b);
void GetAttribute(CValue *out, s32 attr, u32 mask, const CValue *present);
u32 GetAllAttributes();

/* Game/csdLoader.cpp */
void SwapEndian(void *p, s32 size);

/* Game/csdMatrix.cpp */
SMatrix *GetMatrix();
void ScaleMatrix(float x, float y);
void RotateMatrix(float angle);
void TranslateMatrix1(float x, float y);
void TranslateMatrix2(float x, float y);
void TranslateMatrix0(float x, float y);
void TranslateMatrix3(float x, float y);
void LoadMatrix();
void PopMatrix();
void PushMatrix(SMatrix *m);
void ShutdownMatrix();
void InitMatrix();

/* Game/csdMotionPalette.cpp */
void InitCastFromInfo(SCast *cast, const SCastInfo *info, const SSceneInfo *scene, void *texList);
s32 GetWrapT(const SCastInfo *info);
s32 GetWrapS(const SCastInfo *info);
s32 IsFlipY(const SCastInfo *info);
s32 IsFlipX(const SCastInfo *info);
s32 GetFilter(const SCastInfo *info);
s32 GetAlphaMode(const SCastInfo *info);
s32 GetBlendMode(const SCastInfo *info);

/* The screen parameters (the unit after Game/csdMotionPalette.cpp). */
float GetScreenAspect();
void ScaleToScreen(float *x, float *y);
void GetScreenOffset(float *x, float *y);
void GetScreenBase(float *x, float *y);
void GetScreenSize(float *x, float *y);

/* Game/csdLoader.cpp */
void Shutdown();
void Initialize();

/* Game/csdPlatform.cpp */
extern u32 g_chunkInfo;
extern u32 g_chunkTexList;
extern u32 g_chunkObject;
extern u32 g_chunkMotion;
extern u32 g_chunkMM;
extern u32 g_chunkMC;
extern u32 g_chunkML;
extern u32 g_chunkMA;
extern u32 g_chunkLI;
extern u32 g_chunkCA;
extern u32 g_chunkMT;
extern u32 g_chunkNN;
extern u32 g_chunkOffsets;
extern u32 g_chunkEnd;
CPlatform *GetPlatform();
void EndPlatform(CPlatform *platform);
void BeginPlatform(CPlatform *platform);

/* The texture lists. */
void ShutdownTexList();
void InitTexList();

} // namespace CSD
} // namespace Chao

#endif

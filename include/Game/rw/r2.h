/* Private RenderWare type guesses of the units RwCpp10-RwCpp17 (wave 40, agent r2).
 * Offsets and sizes are read from the retail code; names follow the public RenderWare API where
 * the behaviour identifies the object. A later pass unifies these with the other agents' headers.
 */
#ifndef GAME_RW_R2_H
#define GAME_RW_R2_H

#include <dolphin/types.h>

#ifndef __cplusplus
typedef struct R2V3d R2V3d;
typedef struct R2Matrix R2Matrix;
typedef struct R2Object R2Object;
typedef struct R2Globals R2Globals;
typedef struct R2VertexDesc R2VertexDesc;
typedef struct R2SurfaceProperties R2SurfaceProperties;
typedef struct R2Texture R2Texture;
typedef struct R2Material R2Material;
typedef struct R2Mesh R2Mesh;
typedef struct R2MeshHeader R2MeshHeader;
typedef struct R2MeshGlobals R2MeshGlobals;
typedef struct R2Geometry R2Geometry;
typedef struct R2Atomic R2Atomic;
typedef struct R2PTank R2PTank;
typedef struct R2SkinSplit R2SkinSplit;
typedef struct R2Skin R2Skin;
typedef struct R2MatrixWeights R2MatrixWeights;
typedef struct R2SkinGlobals R2SkinGlobals;
typedef struct R2SkinAtomic R2SkinAtomic;
typedef struct R2LLLink R2LLLink;
typedef struct R2ResEntry R2ResEntry;
typedef struct R2SkinInstance R2SkinInstance;
typedef struct R2SkinResEntry R2SkinResEntry;
typedef struct R2ResGlobals R2ResGlobals;
typedef struct R2HAnimNodeInfo R2HAnimNodeInfo;
typedef struct R2HAnimHierarchy R2HAnimHierarchy;
typedef struct R2SkinBlendJob R2SkinBlendJob;
typedef struct R2Raster R2Raster;
typedef struct R2RasterExt R2RasterExt;
typedef struct R2Error R2Error;
typedef struct R2SkinRLE R2SkinRLE;
typedef struct R2DisplayList R2DisplayList;
typedef struct R2GxNodeData R2GxNodeData;
typedef struct R2AnimInterpInfo R2AnimInterpInfo;
typedef struct R2UVAnimation R2UVAnimation;
typedef struct R2UVKeyFrame R2UVKeyFrame;
#endif

/* These units are compiled as C (RenderWare is a C library), where the C++-only MSL stand-in
 * headers cannot be included. */
#ifdef __cplusplus
extern "C" {
#endif
void *memset(void *dst, int value, unsigned long size);
void *memcpy(void *dst, const void *src, unsigned long size);
double sin(double x);
double cos(double x);
double sqrt(double x);
#ifdef __cplusplus
}
#endif

struct R2V3d {
    f32 x;
    f32 y;
    f32 z;
};

/* RwMatrix: four rows of three floats, each followed by a word (flags after the first row). */
struct R2Matrix {
    R2V3d right;
    u32 flags;
    R2V3d up;
    u32 pad1C;
    R2V3d at;
    u32 pad2C;
    R2V3d pos;
    u32 pad3C;
};

struct R2Object {
    u8 type;
    u8 subType;
    u8 flags;
    u8 privateFlags;
    void *parent;
};

/* The engine's global block (RwEngineInstance). Only the members these units use are named. */
struct R2Globals {
    R2Object *curCamera;                             // 0x000
    void *curWorld;                                  // 0x004
    u16 renderFrame;                                 // 0x008
    u16 lightFrame;                                  // 0x00A
    u8 pad00C[0x14];
    u32 (*renderStateSet)(s32 state, void *value);   // 0x020
    u32 (*renderStateGet)(s32 state, void *value);   // 0x024
    u8 pad028[0xE0];
    void *(*memAlloc)(u32 size, u32 hint);           // 0x108
    void (*memFree)(void *memory);                   // 0x10C
    void *(*memRealloc)(void *memory, u32 size, u32 hint);   // 0x110
    void *(*memCalloc)(u32 count, u32 size, u32 hint);       // 0x114
    void *(*freeListAlloc)(void *list, u32 hint);    // 0x118
    void *(*freeListFree)(void *list, void *memory); // 0x11C
};

extern R2Globals *lbl_805F15B4;

#define R2_PLUGIN(type, object, offset) (*(type **)((u8 *)(object) + (offset)))

/* GameCube vertex descriptor used by the geometry (fn_8046301C creates it; reference counted). */
struct R2VertexDesc {
    u8 type[0xC];     // 0x00: per attribute, [0] position, [1] normal, [2..9] texture sets, [0xA] colour
    u8 frac[0xA];     // 0x0C
    u16 refCount;     // 0x16
};

/* RwRaster (members used here only). */
struct R2Raster {
    struct R2Raster *parent;   // 0x00
};

/* The GameCube driver's raster extension (at lbl_805F1658 in RwRaster). */
struct R2RasterExt {
    u8 pad00[0x14];
    u32 flags;                 // 0x14: bit 0 = no alpha (?)
};

/* RwTexture (members used here only). */
struct R2Texture {
    struct R2Raster *raster;   // 0x00
};

/* RwError */
struct R2Error {
    s32 pluginID;
    s32 errorCode;
};

struct R2SurfaceProperties {
    f32 ambient;
    f32 specular;
    f32 diffuse;
};

/* RpMaterial */
struct R2Material {
    R2Texture *texture;                 // 0x00
    u32 color;                          // 0x04
    void *pipeline;                     // 0x08
    R2SurfaceProperties surfaceProps;   // 0x0C
    s16 refCount;                       // 0x18
    s16 pad1A;
};

/* RpMesh / RpMeshHeader */
struct R2Mesh {
    u16 *indices;
    u32 numIndices;
    R2Material *material;
};

struct R2MeshHeader {
    u32 flags;               // 0x00
    u16 numMeshes;           // 0x04
    u16 serialNum;           // 0x06
    u32 totalIndicesInMesh;  // 0x08
    u32 firstMeshOffset;     // 0x0C
};

/* The mesh module's block inside the engine globals (at lbl_805F14F0). */
struct R2MeshGlobals {
    s16 nextSerialNum;       // 0x00
    u8 pad02[0x2B];
    u8 meshFlagsDefault;     // 0x2D
};

/* RpGeometry (members used here only). */
struct R2Geometry {
    R2Object object;         // 0x00
    u32 flags;               // 0x08
    u16 lockedSinceLastInst; // 0x0C
    s16 refCount;            // 0x0E
    s32 numTriangles;        // 0x10
    s32 numVertices;         // 0x14
    s32 numMorphTargets;     // 0x18
    u8 pad1C[0x38];
    R2MeshHeader *mesh;      // 0x54
    struct R2ResEntry *repEntry; // 0x58: the instance data when there is one morph target
    void *morphTarget;       // 0x5C
};

/* RpAtomic (members used here only). */
struct R2Atomic {
    R2Object object;         // 0x00
    u8 pad08[0xC];
    struct R2ResEntry *repEntry; // 0x14: the instance data of a morphing atomic
    R2Geometry *geometry;    // 0x18
    u8 pad1C[0x50];
    void *pipeline;          // 0x6C
};

/* RpPTank's per-atomic data on GameCube (the atomic extension holds a pointer to it). */
struct R2PTank {
    u32 unk00;
    u32 counts;                  // 0x04: low half = active particles
    u8 pad08[8];
    u32 (*render)(R2Atomic *);   // 0x10
    u8 pad14[0x10];
    void *unk24;                 // 0x24
    void *submit;                // 0x28
    void *unk2C;                 // 0x2C
    void *unk30;                 // 0x30
    void *unk34;                 // 0x34
    void *unk38;                 // 0x38
    u8 pad3C[8];
    u32 submitMask;              // 0x44
    u8 locks[0x4C];              // 0x48
    R2V3d *axes;                 // 0x94: right and up used to build the quads
    u8 pad98[8];
    u32 dataFlags;               // 0xA0
    u32 srcBlend;                // 0xA4
    u32 dstBlend;                // 0xA8
    s32 vertexAlphaBlend;        // 0xAC
    u8 padB0[0x10];
    f32 cRotate;                 // 0xC0
    u32 cColor;                  // 0xC4
    u8 padC8[0x30];
    R2Matrix cMatrix;            // 0xF8
};

/* One pair of RpSkin's run-length tables: per mesh (start run, number of runs), per run (first
 * bone, number of bones). */
struct R2SkinRLE {
    u8 start;
    u8 length;
};

/* RpSkin's split data (bone limit and the per-mesh bone remapping tables). */
struct R2SkinSplit {
    u32 boneLimit;       // 0x00
    u32 numMeshes;       // 0x04
    u32 numRLE;          // 0x08
    u8 *remapIndices;    // 0x0C: numBones bytes, then the two tables below
    u16 *meshRLECount;   // 0x10: numMeshes entries of two bytes
    u16 *meshRLE;        // 0x14: numRLE entries of two bytes
};

/* One vertex's bone weights. The code here only tests the words against zero. */
struct R2MatrixWeights {
    u32 w[4];
};

/* RpSkin (0x4C bytes, from the skin free list). */
struct R2Skin {
    u32 numBones;              // 0x00
    u32 numUsedBones;          // 0x04
    u8 *usedBones;             // 0x08
    R2Matrix *matrices;        // 0x0C: skin-to-bone matrices, 16-byte aligned
    u32 maxWeights;            // 0x10
    u32 *indices;              // 0x14: four bone indices per vertex
    R2MatrixWeights *weights;  // 0x18
    void *positions;           // 0x1C: vertex data kept for the geometry (see fn_8044CF68)
    void *normals;             // 0x20
    u8 *packedWeights;         // 0x24: maxWeights bytes per vertex, in a block of their own
    u8 *packedIndices;         // 0x28: maxWeights bytes per vertex, behind the weights
    u32 unk2C;                 // 0x2C
    R2SkinSplit split;         // 0x30
    void *memory;              // 0x48: the block the arrays above live in
};

/* RpSkin's module globals (lbl_8056D708). */
struct R2SkinGlobals {
    s32 engineOffset;          // 0x00
    s32 atomicOffset;          // 0x04
    s32 geometryOffset;        // 0x08
    void *matrixCache;         // 0x0C: matrixCacheMemory rounded up to 16
    void *matrixCacheMemory;   // 0x10
    u32 unk14;
    void *freeList;            // 0x18
    u32 unk1C;
    s32 numInstances;          // 0x20
};

/* RpSkin's per-atomic data (at atomicOffset). */
struct R2SkinAtomic {
    void *hierarchy;           // 0x00
    void *positions;           // 0x04
    void *normals;             // 0x08
};

struct R2LLLink {
    struct R2LLLink *next;
    struct R2LLLink *prev;
};

/* RwResEntry */
struct R2ResEntry {
    struct R2LLLink link;                 // 0x00
    s32 size;                             // 0x08
    void *owner;                          // 0x0C
    struct R2ResEntry **ownerRef;         // 0x10
    void (*destroyNotify)(struct R2ResEntry *entry);   // 0x14
};

/* The skin's instance data, right behind a geometry's resource entry: a header, the vertex
 * arrays (8 bytes each, positions first) and the display lists of the meshes. */
struct R2SkinInstance {
    u16 token;                 // 0x00: draw-sync token of the last frame that used the arrays
    u16 pad02;
    u32 flags;                 // 0x04
    u32 numArrays;             // 0x08
    void *positions;           // 0x0C
    u8 unk10;
    u8 normalStride;           // 0x11
    u8 pad12[2];
    void *normals;             // 0x14
};

struct R2SkinResEntry {
    struct R2ResEntry entry;        // 0x00
    struct R2SkinInstance instance; // 0x18
};

/* A mesh's display list in the instance data. */
struct R2DisplayList {
    void *data;
    u32 size;
};

/* What the GameCube pipeline hands a render callback. */
struct R2GxNodeData {
    R2SkinResEntry *resEntry;  // 0x00
    R2MeshHeader *meshHeader;  // 0x04
    u32 flags;                 // 0x08: 0x10 normals, 0x84 textured
    u8 lighting[0x10];         // 0x0C: handed to the material lighting callback
    u32 unk1C;                 // 0x1C
    u32 unk20;                 // 0x20
};

/* The per-mesh material lighting callback fn_804655D4 picks. */
typedef void (*R2GxLightCallback)(void *lighting, u32 *color, f32 ambient);

/* The resource arena's block in the engine globals (at lbl_805F1578). */
struct R2ResGlobals {
    u8 pad00[0x24];
    struct R2LLLink *usedEntries;  // 0x24
};

/* RpHAnimHierarchy (members used here only). */
struct R2HAnimNodeInfo {
    s32 nodeID;
    s32 nodeIndex;
    s32 flags;
    void *frame;
};

struct R2HAnimHierarchy {
    s32 flags;                     // 0x00
    s32 numNodes;                  // 0x04
    R2Matrix *matrixArray;         // 0x08
    void *matrixArrayUnaligned;    // 0x0C
    struct R2HAnimNodeInfo *nodeInfo;  // 0x10
};

/* What the software skinning loops (fn_80450F68, fn_80451254, fn_80451618) are handed. */
struct R2SkinBlendJob {
    void *dst;                 // 0x00
    u32 unk04;
    const void *src;           // 0x08
    u32 unk0C;
    u32 componentSize;         // 0x10: bytes per written component
    u32 unk14;
    u32 unk18;
    s32 numVertices;           // 0x1C
    u32 unk20;
};

/* RtAnimInterpolatorInfo: the keyframe scheme of an animation. */
struct R2AnimInterpInfo {
    s32 typeID;
    s32 interpKeyFrameSize;
    s32 animKeyFrameSize;
    void *keyFrameApplyCB;
    void *keyFrameBlendCB;
    void *keyFrameInterpolateCB;
    void *keyFrameAddCB;
    void *keyFrameMulRecipCB;
    void *keyFrameStreamReadCB;
    void *keyFrameStreamWriteCB;
    void *keyFrameStreamGetSizeCB;
    s32 customDataSize;
};
/* RtAnimAnimation (members used here only). */
struct R2UVAnimation {
    struct R2AnimInterpInfo *interpInfo;
};
/* RpUVAnim's keyframe: the previous frame, the time and six floats of payload. */
struct R2UVKeyFrame {
    void *prev;
    float time;
    float uv[6];
};

#endif

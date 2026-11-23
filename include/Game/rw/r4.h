/* Private RenderWare types of the units RwCpp29..RwCpp36 (0x80466B58..0x80473584), inferred
 * from the retail code. A later pass unifies them with the neighbouring units' headers. */
#ifndef GAME_RW_R4_H
#define GAME_RW_R4_H

#include <dolphin/types.h>

struct RwV3d {
    f32 x;
    f32 y;
    f32 z;
};

struct RwV2d {
    f32 x;
    f32 y;
};

struct RwTexCoords {
    f32 u;
    f32 v;
};

struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

/* A geometry's packed normal: 1.7 fixed point. */
struct RpVertexNormal {
    s8 x;
    s8 y;
    s8 z;
    u8 pad;
};

/* Three floats as GX reads them (positions, normals): the array member makes a copy a block
 * move, which is how retail copies them. */
struct RwGameCubeV3d {
    f32 v[3];
};

struct RwGameCubeV2d {
    f32 v[2];
};

/* RwEngineInstance (lbl_805F15B4): only the members these units use. */
struct RwMemoryFunctions {
    void *(*rwmalloc)(u32 size, u32 hint);
    void (*rwfree)(void *mem);
    void *(*rwrealloc)(void *mem, u32 size, u32 hint);
    void *(*rwcalloc)(u32 num, u32 size, u32 hint);
};

struct RwFreeList {
    u8 unk0[0x24];
};

/* RwStringFunctions: RenderWare's string helpers. */
struct RwStringFunctions {
    void *vecSprintf;
    void *vecVsprintf;
    char *(*rwstrcpy)(char *dst, const char *src);
    void *rwstrncpy;
    void *rwstrcat;
    void *rwstrncat;
    void *rwstrrchr;
    void *rwstrchr;
    void *rwstrstr;
    void *rwstrcmp;
    void *rwstrncmp;
    void *rwstricmp;
    u32 (*rwstrlen)(const char *str);
    void *rwstrupr;
    void *rwstrlwr;
    void *rwstrtok;
    void *rwsscanf;
};

struct RwGlobals {
    u8 unk0[0xC4];
    /* 0x0C4 */ RwStringFunctions stringFuncs;
    /* 0x108 */ RwMemoryFunctions memoryFuncs;
    /* 0x118 */ void *(*memoryAlloc)(RwFreeList *freeList, u32 hint);
    /* 0x11C */ RwFreeList *(*memoryFree)(RwFreeList *freeList, void *data);
};

struct RwSphere {
    RwV3d center;
    f32 radius;
};

struct RpMaterial;
struct RpGeometry;
struct RwResEntry;

struct RpMesh {
    /* 0x00 */ u16 *indices;
    /* 0x04 */ u32 numIndices;
    /* 0x08 */ RpMaterial *material;
};

struct RpMeshHeader {
    /* 0x00 */ u32 flags;
    /* 0x04 */ u16 numMeshes;
    /* 0x06 */ u16 serialNum;
    /* 0x08 */ u32 totalIndicesInMesh;
    /* 0x0C */ u32 firstMeshOffset;
};

/* The meshes follow their header. */
#define RPMESHES(header) ((RpMesh *)((header) + 1))

struct RpMorphTarget {
    /* 0x00 */ RpGeometry *parentGeom;
    /* 0x04 */ RwSphere boundingSphere;
    /* 0x14 */ RwV3d *verts;
    /* 0x18 */ RwV3d *normals;
};

struct RpGeometry {
    /* 0x00 */ u8 object[8];
    /* 0x08 */ u32 flags;
    /* 0x0C */ u16 lockedSinceLastInst;
    /* 0x0E */ s16 refCount;
    /* 0x10 */ s32 numTriangles;
    /* 0x14 */ s32 numVertices;
    /* 0x18 */ s32 numMorphTargets;
    /* 0x1C */ s32 numTexCoordSets;
    /* 0x20 */ u8 matList[0xC];
    /* 0x2C */ void *triangles;
    /* 0x30 */ RwRGBA *preLitLum;
    /* 0x34 */ RwTexCoords *texCoords[8];
    /* 0x54 */ RpMeshHeader *mesh;
    /* 0x58 */ void *repEntry;
    /* 0x5C */ RpMorphTarget *morphTarget;
};

struct RpWorldSector {
    /* 0x00 */ s32 type;
    /* 0x04 */ void *polygons;
    /* 0x08 */ RwV3d *vertices;
    /* 0x0C */ RpVertexNormal *normals;
    /* 0x10 */ RwTexCoords *texCoords[8];
    /* 0x30 */ RwRGBA *preLitLum;
    /* 0x34 */ RwResEntry *repEntry;
    /* 0x38 */ u8 unk38[0x6C - 0x38];
    /* 0x6C */ u8 unk6C[0x78 - 0x6C];
    /* 0x78 */ RpMeshHeader *mesh;
    /* 0x7C */ u8 unk7C[0x82 - 0x7C];
    /* 0x82 */ u16 numVertices;
};

struct RpWorld {
    /* 0x00 */ u8 object[8];
    /* 0x08 */ u32 flags;
    /* 0x0C */ s32 renderOrder;
    /* 0x10 */ u8 matList[0xC];
    /* 0x1C */ RpWorldSector *rootSector;
    /* 0x20 */ s32 numTexCoordSets;
};

/* GX (Dolphin SDK) enumerations the driver passes around; the project has no GX header yet. */
enum GXAttr {
    GX_VA_PNMTXIDX = 0,
    GX_VA_TEX0MTXIDX = 1,
    GX_VA_POS = 9,
    GX_VA_NRM = 10,
    GX_VA_CLR0 = 11,
    GX_VA_CLR1 = 12,
    GX_VA_TEX0 = 13,
    GX_VA_TEX1 = 14,
    GX_VA_TEX2 = 15,
    GX_VA_TEX3 = 16,
    GX_VA_TEX4 = 17,
    GX_VA_TEX5 = 18,
    GX_VA_TEX6 = 19,
    GX_VA_TEX7 = 20,
    GX_VA_NBT = 25
};

enum GXAttrType {
    GX_NONE = 0,
    GX_DIRECT = 1,
    GX_INDEX8 = 2,
    GX_INDEX16 = 3
};

/* The vertex descriptor of a GameCube display list: the values of GX's vertex attribute table
 * (VAT) and vertex descriptor (VCD) registers, kept by the driver (RwCpp34.cpp sets them, as
 * GXSetVtxAttrFmt and GXSetVtxDesc would). */
struct RwGameCubeVtxDesc {
    /* 0x00 */ u32 unk0;
    /* 0x04 */ u32 vatA; /* bit 9: normal, binormal and tangent; bit 31: indexed one by one */
    /* 0x08 */ u32 vatB;
    /* 0x0C */ u32 vatC;
    /* 0x10 */ u32 vcdLo;
    /* 0x14 */ u32 vcdHi;
    /* 0x18 */ u32 unk18;
    /* 0x1C */ u8 numArrays;
};

/* The attribute arrays a display list indexes, by GX attribute (GX_VA_MAX_ATTR entries). */
struct RwGameCubeArrays {
    /* 0x00 */ u32 num[26];
    /* 0x68 */ void *data[26];
};

#define RWGAMECUBE_POS_TYPE(desc) (((desc)->vcdLo >> 9) & 3)
#define RWGAMECUBE_NRM_TYPE(desc) (((desc)->vcdLo >> 11) & 3)
#define RWGAMECUBE_CLR_TYPE(desc, n) (((desc)->vcdLo >> (2 * (n) + 13)) & 3)
#define RWGAMECUBE_TEX_TYPE(desc, n) (((desc)->vcdHi >> (2 * (n))) & 3)

extern "C" {
/* RwCpp28.cpp: the size in bytes of one position, normal, colour and texture coordinate. */
u32 fn_8046693C(RwGameCubeVtxDesc *desc);
u32 fn_804669A0(RwGameCubeVtxDesc *desc);
u32 fn_804669EC(RwGameCubeVtxDesc *desc, u8 index);
u32 fn_80466A54(RwGameCubeVtxDesc *desc, u8 index);
}

/* One vertex attribute array handed to the index optimiser (fn_80466194..fn_80465CDC). */
struct RwGameCubeVtxStream {
    /* 0x00 */ void *data;
    /* 0x04 */ s32 type; /* 8 RwV3d, 4 RwRGBA, 7 RwTexCoords */
    /* 0x08 */ s8 link;
    /* 0x09 */ u8 unk9[0xF];
};

/* A geometry's GameCube native-data format (plugin data at lbl_805F1520): the GX component
 * type and fixed-point shift of each attribute array. */
struct RwGameCubeGeomFormat {
    /* 0x00 */ u8 posType;
    /* 0x01 */ u8 nrmType;
    /* 0x02 */ u8 texType[8];
    /* 0x0A */ u8 clrType;
    /* 0x0B */ u8 unkB;
    /* 0x0C */ u8 posFrac;
    /* 0x0D */ u8 nbt;
    /* 0x0E */ u8 texFrac[8];
};

/* One merged attribute array of the index optimiser's result (fn_80465ED4). */
struct RwGameCubeVtxArray {
    /* 0x00 */ void *data;
    /* 0x04 */ u32 num;
};

extern "C" {
extern RwGlobals *lbl_805F15B4;
/* The offset of the GameCube native-data plugin's data in an RpGeometry. */
extern s32 lbl_805F1520;
RwGameCubeGeomFormat *fn_80462E1C(void); /* the default format */
}
#define RWGAMECUBEGEOMFORMAT(geom) (*(RwGameCubeGeomFormat **)((u8 *)(geom) + lbl_805F1520))
extern "C" {
/* The same plugin's data in an RpWorld. */
extern s32 lbl_805F1524;
}
#define RWGAMECUBEWORLDFORMAT(world) (*(RwGameCubeGeomFormat **)((u8 *)(world) + lbl_805F1524))
#define RwMalloc(size, hint) (lbl_805F15B4->memoryFuncs.rwmalloc((size), (hint)))
#define RwFree(mem) (lbl_805F15B4->memoryFuncs.rwfree(mem))
#define RwRealloc(mem, size, hint) (lbl_805F15B4->memoryFuncs.rwrealloc((mem), (size), (hint)))
#define RwFreeListAlloc(fl, hint) (lbl_805F15B4->memoryAlloc((fl), (hint)))
#define RwFreeListFree(fl, data) (lbl_805F15B4->memoryFree((fl), (data)))

/* RenderWare's public resource-arena entry (RwResEntry); its data follows it. */
struct RwLLLink {
    RwLLLink *next;
    RwLLLink *prev;
};

struct RwResEntry;
typedef void (*RwResEntryDestroyNotify)(RwResEntry *resEntry);

struct RwResEntry {
    /* 0x00 */ RwLLLink link;
    /* 0x08 */ s32 size;
    /* 0x0C */ void *owner;
    /* 0x10 */ RwResEntry **ownerRef;
    /* 0x14 */ RwResEntryDestroyNotify destroyNotify;
};

/* The data of a GameCube display-list instance, behind its RwResEntry: this header (with
 * one record per attribute array after the first, fn_8046EE8C), one 8-byte list record per
 * mesh (fn_8046B0E0), the attribute arrays and the lists, each 32-byte aligned. */
struct RwGameCubeInstHeader {
    /* 0x00 */ u16 serial;     /* lbl_805F1678 at instancing */
    /* 0x02 */ u16 meshSerial; /* the mesh header's serialNum */
    /* 0x04 */ u32 flags;      /* bit 0: the prelight colours are not all opaque */
};

/* A display list of an instance (fn_8046B0E0 fills it). */
struct RwGameCubeDisplayList {
    /* 0x00 */ void *data;
    /* 0x04 */ u32 size;
};

extern "C" {
extern u16 lbl_805F1678;

/* RwResourcesAllocateResEntry (RwCpp07.cpp and others call it by this name). */
RwResEntry *fn_80480878(void *owner, RwResEntry **ownerRef, s32 size,
                        RwResEntryDestroyNotify destroyNotify);
void fn_8046288C(RwResEntry *resEntry); /* the instances' destroy notification */
void DCFlushRange(void *addr, u32 nBytes);
void fn_80394F2C(void);

/* RwCpp31.cpp */
u32 fn_8046ACBC(RwGameCubeVtxDesc *desc);
u32 fn_8046B094(RwGameCubeVtxDesc *desc, u32 numLists, u32 numIndices);
void fn_8046B0E0(void **block, u32 unused, u32 size, void *mem);
s32 fn_8046B110(void *object, void **vertexData, void *meshData);
/* RwCpp32.cpp and RwCpp33.cpp */
RwGameCubeVtxDesc *fn_8046B554(RpGeometry *geom, RwGameCubeVtxArray *arrays);
RwGameCubeVtxDesc *fn_8046BEC4(RpGeometry *geom);
s32 fn_8046C4D0(RpWorld *world, void *object, void **vertexData, void *meshData);
RwGameCubeVtxDesc *fn_8046C94C(RpWorld *world, RwGameCubeVtxArray *arrays);
RwGameCubeVtxDesc *fn_8046D2B8(RpWorld *world, RpWorldSector *sector);
void fn_8046D8C8(u16 *indices, u32 numIndices, s32 *numStrips, s32 *numVertices, s32 swap);
/* RwCpp34.cpp */
void fn_8046E594(RwGameCubeVtxDesc *desc, void *block, u16 **indices, u32 numIndices, s32 strip,
                 u32 vertexSize, s32 arg6, u32 primitive, void *arg8);
u32 fn_8046EE8C(RwGameCubeVtxDesc *desc);
u32 fn_8046EEA0(RwGameCubeVtxDesc *desc, RwGameCubeArrays *arrays);
void fn_8046F208(RwGameCubeVtxDesc *desc, RwGameCubeInstHeader *header, RwGameCubeArrays *arrays,
                 void *data);
void fn_8046F6CC(RwGameCubeVtxDesc *desc, RwGameCubeInstHeader *header, RwGameCubeArrays *arrays,
                 s32 arg3, void *arg4);
}

/* A real-valued colour (RwRGBAReal). */
struct RwRGBAReal {
    f32 red;
    f32 green;
    f32 blue;
    f32 alpha;
};

struct RwTexture {
    /* 0x00 */ void *raster;
    /* 0x04 */ u8 unk4[0x54 - 0x04];
    /* 0x54 */ s32 refCount;
};

/* A corner of a GameCube driver quad: its colour and texture coordinate. */
struct RwGameCubeQuadVertex {
    /* 0x00 */ RwRGBAReal color;
    /* 0x10 */ RwGameCubeV2d uv;
};

/* The GameCube driver's quad (RwCpp35.cpp, from a free list of 0x78-byte entries): the
 * corners are kept as two bases and two edge deltas (v[1] = corner - v[0], v[3] = corner -
 * v[2]), so a flat quad is the deltas being zero. */
struct RwGameCubeQuad {
    /* 0x00 */ RwGameCubeQuadVertex v[4];
    /* 0x60 */ RwRGBA color;
    /* 0x64 */ u32 flags; /* bit 0: the colours vary, bit 1: textured or the UVs vary */
    /* 0x68 */ RwTexture *texture;
    /* 0x6C */ u32 unk6C;
    /* 0x70 */ f32 unk70;
    /* 0x74 */ s32 refCount;
};

/* A loaded charset of the GameCube driver's list. */
struct RwGameCubeCharsetNode {
    /* 0x0 */ void *name;
    /* 0x4 */ void *charset;
    /* 0x8 */ RwGameCubeCharsetNode *next;
};

/* A glyph of a GameCube driver charset (32 bytes). */
struct RwGameCubeGlyph {
    /* 0x00 */ f32 advance;
    /* 0x04 */ RwTexture *texture; /* from the charset's textures; none for image glyphs */
    /* 0x08 */ f32 rect[4];
    /* 0x18 */ void *image;
    /* 0x1C */ s8 textureIndex;
    /* 0x1D */ u8 unk1D[0x20 - 0x1D];
};

/* A GameCube driver charset (bitmap font): glyph indices by character code, below 0x80
 * from a table and above it from a second table built by fn_80471300. */
struct RwGameCubeCharset {
    /* 0x000 */ u8 unk0[0x8];
    /* 0x008 */ f32 height;
    /* 0x00C */ f32 spacing;
    /* 0x010 */ u32 flags; /* bit 1: two-byte characters */
    /* 0x014 */ RwTexture *textures[4];
    /* 0x024 */ s16 asciiGlyphs[0x80];
    /* 0x124 */ s32 firstExtCode;
    /* 0x128 */ s32 numExtCodes;
    /* 0x12C */ s16 *extGlyphs;
    /* 0x130 */ s32 numGlyphs;
    /* 0x134 */ void *glyphData;
    /* 0x138 */ void *draw;
    /* 0x13C */ u32 unk13C;
    /* 0x140 */ u32 unk140;
};

/* The GameCube driver's globals (lbl_805E1950, 0xD58 bytes): the members these units use. */
struct RwGameCubeCharsetNode;

struct RwGameCubeDriver {
    /* 0x000 */ u8 unk0[0x80];
    /* 0x080 */ u16 quadIndices[16][6]; /* two triangles per quad of a text run */
    /* 0x140 */ RwFreeList *quadFreeList;
    /* 0x144 */ void *quadBuffer;
    /* 0x148 */ RwFreeList *charsetFreeList; /* 0x144-byte charsets */
    /* 0x14C */ char *path;                 /* the charset directory, with room for its size */
    /* 0x150 */ u32 pathSize;
    /* 0x154 */ RwFreeList *nodeFreeList;   /* 0xC-byte RwGameCubeCharsetNode */
    /* 0x158 */ RwGameCubeCharsetNode *charsets;
    /* 0x15C */ void *texDictionary;
    /* 0x160 */ void *currentTexDictionary;
    /* 0x164 */ u8 unk164[0x840 - 0x164];
    /* 0x840 */ void *readCallback;
    /* 0x844 */ u8 unk844[0xD4C - 0x844];
    /* 0xD4C */ s32 *glyphCodes; /* the character code of each glyph of the charset being read */
    /* 0xD50 */ u32 numGlyphCodes;
    /* 0xD54 */ u8 unkD54[0xD58 - 0xD54];
};

#endif

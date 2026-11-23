/* Private RenderWare types of range r4 (0x80466B58..0x80473E9C): the GameCube display-list code
 * of the world plugin (src/RenderWare/RpWorldGameCube*.c, RpGeometryGameCubeInstance.c,
 * RpWorldSectorGameCubeInstance.c) and the 2D toolkit's brushes and fonts (Rt2dBrush.c,
 * Rt2dFont.c). Inferred from the retail code; only the members these files use are named.
 * The sources are C; the header also compiles as C++. */
#ifndef GAME_RW_R4_H
#define GAME_RW_R4_H

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The C library functions the files call (MSL). */
void *memset(void *dst, int value, unsigned long n);
void *memcpy(void *dst, const void *src, unsigned long n);

/* RenderWare's scalar types: the 32-bit ones are int, not long. */
typedef int RwInt32;
typedef unsigned int RwUInt32;
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef signed char RwInt8;
typedef unsigned char RwUInt8;
typedef float RwReal;
typedef int RwBool;
typedef char RwChar;

typedef struct RwV2d {
    f32 x;
    f32 y;
} RwV2d;

typedef struct RwV3d {
    f32 x;
    f32 y;
    f32 z;
} RwV3d;

typedef struct RwTexCoords {
    f32 u;
    f32 v;
} RwTexCoords;

typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

typedef struct RwRGBAReal {
    f32 red;
    f32 green;
    f32 blue;
    f32 alpha;
} RwRGBAReal;

/* A geometry's packed normal: 1.7 fixed point. */
typedef struct RpVertexNormal {
    s8 x;
    s8 y;
    s8 z;
    u8 pad;
} RpVertexNormal;

/* Three floats as GX reads them (positions, normals): the array member makes a copy a block
 * move, which is how retail copies them. */
typedef struct RwGameCubeV3d {
    f32 v[3];
} RwGameCubeV3d;

typedef struct RwGameCubeV2d {
    f32 v[2];
} RwGameCubeV2d;

typedef struct RwSphere {
    RwV3d center;
    f32 radius;
} RwSphere;

typedef struct RwLLLink RwLLLink;
struct RwLLLink {
    RwLLLink *next;
    RwLLLink *prev;
};

/* RenderWare's free list (RwFreeList, 0x24 bytes): the toolkits keep theirs in static storage. */
typedef struct RwFreeList {
    u8 unk0[0x24];
} RwFreeList;

/* RenderWare's string helpers (RwStringFunctions, in RwGlobals at 0xC4). */
typedef struct RwStringFunctions {
    int (*vecSprintf)(RwChar *buffer, const RwChar *format, ...);
    void *vecVsprintf;
    RwChar *(*rwstrcpy)(RwChar *dst, const RwChar *src);
    RwChar *(*rwstrncpy)(RwChar *dst, const RwChar *src, u32 n);
    RwChar *(*rwstrcat)(RwChar *dst, const RwChar *src);
    RwChar *(*rwstrncat)(RwChar *dst, const RwChar *src, u32 n);
    RwChar *(*rwstrrchr)(const RwChar *s, int c);
    RwChar *(*rwstrchr)(const RwChar *s, int c);
    RwChar *(*rwstrstr)(const RwChar *s, const RwChar *t);
    int (*rwstrcmp)(const RwChar *a, const RwChar *b);
    int (*rwstrncmp)(const RwChar *a, const RwChar *b, u32 n);
    int (*rwstricmp)(const RwChar *a, const RwChar *b);
    u32 (*rwstrlen)(const RwChar *str);
    RwChar *(*rwstrupr)(RwChar *s);
    RwChar *(*rwstrlwr)(RwChar *s);
    RwChar *(*rwstrtok)(RwChar *s, const RwChar *delim);
    int (*rwsscanf)(const RwChar *buffer, const RwChar *format, ...);
} RwStringFunctions;

/* RenderWare's memory vectors (RwMemoryFunctions, in RwGlobals at 0x108). */
typedef struct RwMemoryFunctions {
    void *(*rwmalloc)(u32 size, u32 hint);
    void (*rwfree)(void *mem);
    void *(*rwrealloc)(void *mem, u32 size, u32 hint);
    void *(*rwcalloc)(u32 num, u32 size, u32 hint);
} RwMemoryFunctions;

/* RwEngineInstance (lbl_805F15B4): only the members these units use. */
typedef struct RwGlobals {
    u8 unk0[0xC4];
    /* 0x0C4 */ RwStringFunctions stringFuncs;
    /* 0x108 */ RwMemoryFunctions memoryFuncs;
    /* 0x118 */ void *(*memoryAlloc)(RwFreeList *freeList, u32 hint);
    /* 0x11C */ RwFreeList *(*memoryFree)(RwFreeList *freeList, void *data);
} RwGlobals;

extern RwGlobals *lbl_805F15B4;

#define RwMalloc(size, hint) (lbl_805F15B4->memoryFuncs.rwmalloc((size), (hint)))
#define RwFree(mem) (lbl_805F15B4->memoryFuncs.rwfree(mem))
#define RwRealloc(mem, size, hint) (lbl_805F15B4->memoryFuncs.rwrealloc((mem), (size), (hint)))
#define RwFreeListAlloc(fl, hint) (lbl_805F15B4->memoryAlloc((fl), (hint)))
#define RwFreeListFree(fl, data) (lbl_805F15B4->memoryFree((fl), (data)))
#define rwstrlen(s) (lbl_805F15B4->stringFuncs.rwstrlen(s))
#define rwstrcpy(d, s) (lbl_805F15B4->stringFuncs.rwstrcpy((d), (s)))

/* ---- The world plugin's GameCube display lists ------------------------------------------- */

typedef struct RpMaterial RpMaterial;
typedef struct RwResEntry RwResEntry;

typedef struct RpMesh {
    /* 0x00 */ u16 *indices;
    /* 0x04 */ u32 numIndices;
    /* 0x08 */ RpMaterial *material;
} RpMesh;

typedef struct RpMeshHeader {
    /* 0x00 */ u32 flags;
    /* 0x04 */ u16 numMeshes;
    /* 0x06 */ u16 serialNum;
    /* 0x08 */ u32 totalIndicesInMesh;
    /* 0x0C */ u32 firstMeshOffset;
} RpMeshHeader;

/* The meshes follow their header. */
#define RPMESHES(header) ((RpMesh *)((header) + 1))

typedef struct RpGeometry RpGeometry;

typedef struct RpMorphTarget {
    /* 0x00 */ RpGeometry *parentGeom;
    /* 0x04 */ RwSphere boundingSphere;
    /* 0x14 */ RwV3d *verts;
    /* 0x18 */ RwV3d *normals;
} RpMorphTarget;

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

typedef struct RpWorldSector {
    /* 0x00 */ s32 type;
    /* 0x04 */ void *polygons;
    /* 0x08 */ RwV3d *vertices;
    /* 0x0C */ RpVertexNormal *normals;
    /* 0x10 */ RwTexCoords *texCoords[8];
    /* 0x30 */ RwRGBA *preLitLum;
    /* 0x34 */ RwResEntry *repEntry;
    /* 0x38 */ u8 unk38[0x6C - 0x38];
    /* 0x6C */ RwV3d unk6C; /* the offset its positions are quantised from */
    /* 0x78 */ RpMeshHeader *mesh;
    /* 0x7C */ u8 unk7C[0x82 - 0x7C];
    /* 0x82 */ u16 numVertices;
} RpWorldSector;

typedef struct RpWorld {
    /* 0x00 */ u8 object[8];
    /* 0x08 */ u32 flags;
    /* 0x0C */ s32 renderOrder;
    /* 0x10 */ u8 matList[0xC];
    /* 0x1C */ RpWorldSector *rootSector;
    /* 0x20 */ s32 numTexCoordSets;
} RpWorld;

/* GX (Dolphin SDK) enumerations the driver passes around, by their SDK names. */
typedef enum GXAttr {
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
} GXAttr;

typedef enum GXAttrType {
    GX_NONE = 0,
    GX_DIRECT = 1,
    GX_INDEX8 = 2,
    GX_INDEX16 = 3
} GXAttrType;

/* The vertex descriptor of a GameCube display list: the values of GX's vertex attribute table
 * (VAT) and vertex descriptor (VCD) registers, kept by the driver (RpWorldGameCubeVertexFormat.c
 * sets them, as GXSetVtxAttrFmt and GXSetVtxDesc would). */
typedef struct RwGameCubeVtxDesc {
    /* 0x00 */ u32 unk0;
    /* 0x04 */ u32 vatA; /* bit 9: normal, binormal and tangent; bit 31: indexed one by one */
    /* 0x08 */ u32 vatB;
    /* 0x0C */ u32 vatC;
    /* 0x10 */ u32 vcdLo;
    /* 0x14 */ u32 vcdHi;
    /* 0x18 */ u32 unk18;
    /* 0x1C */ u8 numArrays;
} RwGameCubeVtxDesc;

/* The attribute arrays a display list indexes, by GX attribute (GX_VA_MAX_ATTR entries). */
typedef struct RwGameCubeArrays {
    /* 0x00 */ u32 num[26];
    /* 0x68 */ void *data[26];
} RwGameCubeArrays;

#define RWGAMECUBE_POS_TYPE(desc) (((desc)->vcdLo >> 9) & 3)
#define RWGAMECUBE_NRM_TYPE(desc) (((desc)->vcdLo >> 11) & 3)
#define RWGAMECUBE_CLR_TYPE(desc, n) (((desc)->vcdLo >> (2 * (n) + 13)) & 3)
#define RWGAMECUBE_TEX_TYPE(desc, n) (((desc)->vcdHi >> (2 * (n))) & 3)

/* The world plugin's vertex-format helpers (0x8046693C..0x80466B58): the size in bytes of one
 * position, normal, colour and texture coordinate. */
u32 fn_8046693C(RwGameCubeVtxDesc *desc);
u32 fn_804669A0(RwGameCubeVtxDesc *desc);
u32 fn_804669EC(RwGameCubeVtxDesc *desc, u8 index);
u32 fn_80466A54(RwGameCubeVtxDesc *desc, u8 index);

/* One vertex attribute array handed to the index optimiser (fn_80466194..fn_80465CDC). */
typedef struct RwGameCubeVtxStream {
    /* 0x00 */ void *data;
    /* 0x04 */ s32 type; /* 8 RwV3d, 4 RwRGBA, 7 RwTexCoords */
    /* 0x08 */ s8 link;
    /* 0x09 */ u8 unk9[0xF];
} RwGameCubeVtxStream;

/* A geometry's GameCube native-data format (plugin data at lbl_805F1520): the GX component
 * type and fixed-point shift of each attribute array. */
typedef struct RwGameCubeGeomFormat {
    /* 0x00 */ u8 posType;
    /* 0x01 */ u8 nrmType;
    /* 0x02 */ u8 texType[8];
    /* 0x0A */ u8 clrType;
    /* 0x0B */ u8 unkB;
    /* 0x0C */ u8 posFrac;
    /* 0x0D */ u8 nbt;
    /* 0x0E */ u8 texFrac[8];
} RwGameCubeGeomFormat;

/* One merged attribute array of the index optimiser's result (fn_80465ED4). */
typedef struct RwGameCubeVtxArray {
    /* 0x00 */ void *data;
    /* 0x04 */ u32 num;
} RwGameCubeVtxArray;

/* The offsets of the GameCube native-data plugin's data in an RpGeometry and an RpWorld. */
extern s32 lbl_805F1520;
extern s32 lbl_805F1524;
RwGameCubeGeomFormat *fn_80462E1C(void); /* the default format */
#define RWGAMECUBEGEOMFORMAT(geom) (*(RwGameCubeGeomFormat **)((u8 *)(geom) + lbl_805F1520))
#define RWGAMECUBEWORLDFORMAT(world) (*(RwGameCubeGeomFormat **)((u8 *)(world) + lbl_805F1524))

/* RenderWare's resource-arena entry (RwResEntry); its data follows it. */
typedef void (*RwResEntryDestroyNotify)(RwResEntry *resEntry);

struct RwResEntry {
    /* 0x00 */ RwLLLink link;
    /* 0x08 */ s32 size;
    /* 0x0C */ void *owner;
    /* 0x10 */ RwResEntry **ownerRef;
    /* 0x14 */ RwResEntryDestroyNotify destroyNotify;
};

/* One attribute array an instance carries (fn_8046F208 lays them out, fn_8046F6CC fills them). */
typedef struct RwGameCubeArrayRecord {
    /* 0x0 */ void *data;
    /* 0x4 */ u8 attr;   /* the GX attribute */
    /* 0x5 */ u8 stride; /* the size of one element */
    /* 0x6 */ u8 type;   /* GX_INDEX8 or GX_INDEX16 */
    /* 0x7 */ u8 pad;
} RwGameCubeArrayRecord;

/* The data of a GameCube display-list instance, behind its RwResEntry: this header (with
 * one record per attribute array, fn_8046EE8C), one 8-byte list record per mesh
 * (fn_8046B0E0), the attribute arrays and the lists, each 32-byte aligned. */
typedef struct RwGameCubeInstHeader {
    /* 0x00 */ u16 serial;     /* lbl_805F1678 at instancing */
    /* 0x02 */ u16 meshSerial; /* the mesh header's serialNum */
    /* 0x04 */ u32 flags;      /* bit 0: the prelight colours are not all opaque */
    /* 0x08 */ u32 numArrays;
    /* 0x0C */ RwGameCubeArrayRecord arrays[1];
} RwGameCubeInstHeader;

/* A display list of an instance (fn_8046B0E0 fills it). */
typedef struct RwGameCubeDisplayList {
    /* 0x00 */ void *data;
    /* 0x04 */ u32 size;
} RwGameCubeDisplayList;

extern u16 lbl_805F1678;

/* RwResourcesAllocateResEntry (RwCpp07.cpp and others call it by this name). */
RwResEntry *fn_80480878(void *owner, RwResEntry **ownerRef, s32 size,
                        RwResEntryDestroyNotify destroyNotify);
void fn_8046288C(RwResEntry *resEntry); /* the instances' destroy notification */
void DCFlushRange(void *addr, u32 nBytes);
void fn_80394F2C(void);

/* The index optimiser (0x80465CDC..0x8046693C). */
void *fn_80466194(RwGameCubeVtxStream *streams, s32 numStreams, s32 numVertices);
void *fn_80465ED4(void *table, RwGameCubeVtxStream *streams, s32 numStreams, s32 numVertices);
void *fn_80465CDC(void *table, u16 **indices, s32 numStreams, u32 numIndices);

/* RpWorldGameCubeVertexConvert.c */
s32 fn_80466B58(void *dest, const RwV3d *src, s32 type, s32 num, u32 stride, const RwV3d *offset,
                f32 scale);
s32 fn_80467BC8(void *dest, const RwV3d *src, s32 type, s32 num, u32 stride);
s32 fn_804683AC(void *dest, const RpVertexNormal *src, s32 type, s32 num, u32 stride);
s32 fn_80468ADC(void *dest, const RwV3d *src, s32 type, s32 num, u32 stride);
s32 fn_804692C0(void *dest, const RpVertexNormal *src, s32 type, s32 num, u32 stride);
s32 fn_804699F0(void *dest, const RwRGBA *src, s32 type, s32 num, u32 stride);
s32 fn_8046A270(void *dest, const RwTexCoords *src, s32 type, s32 num, u32 stride, f32 scale);

/* RpWorldGameCubeListSize.c */
u32 fn_8046ACBC(RwGameCubeVtxDesc *desc);
u32 fn_8046B094(RwGameCubeVtxDesc *desc, u32 numLists, u32 numIndices);
void fn_8046B0E0(void **block, u32 unused, u32 size, void *mem);

/* RpGeometryGameCubeInstance.c */
s32 fn_8046B110(RpGeometry *geom, void **vertexData, void ***meshData);
RwGameCubeVtxDesc *fn_8046B554(RpGeometry *geom, RwGameCubeVtxArray *arrays);
RwGameCubeVtxDesc *fn_8046BEC4(RpGeometry *geom);

/* RpWorldSectorGameCubeInstance.c */
s32 fn_8046C4D0(RpWorld *world, RpWorldSector *sector, void **vertexData, void ***meshData);
RwGameCubeVtxDesc *fn_8046C94C(RpWorld *world, RwGameCubeVtxArray *arrays);
RwGameCubeVtxDesc *fn_8046D2B8(RpWorld *world, RpWorldSector *sector);

/* RpWorldGameCubeDisplayList.c */
void fn_8046D8C8(u16 *indices, u32 numIndices, s32 *numStrips, s32 *numVertices, s32 swap);
void fn_8046E594(RwGameCubeVtxDesc *desc, RwGameCubeDisplayList *list, u16 **indices, u32 numIndices,
                 s32 split, u32 vertexSize, s32 swap, u32 command, const RwV3d *offset);

/* RpWorldGameCubeVertexFormat.c */
u32 fn_8046EE8C(RwGameCubeVtxDesc *desc);
u32 fn_8046EEA0(RwGameCubeVtxDesc *desc, RwGameCubeArrays *arrays);
void fn_8046F208(RwGameCubeVtxDesc *desc, RwGameCubeInstHeader *header, RwGameCubeArrays *arrays,
                 u8 *data);
void fn_8046F6CC(RwGameCubeVtxDesc *desc, RwGameCubeInstHeader *header, RwGameCubeArrays *arrays,
                 s32 packedNormals, const RwV3d *offset);
void fn_8046FC1C(RwGameCubeVtxDesc *desc);
void fn_8046FC48(RwGameCubeVtxDesc *desc, u32 value);
void fn_8046FC50(RwGameCubeVtxDesc *desc, s32 attr, s32 count, s32 type, u8 frac);
void fn_8046FF34(RwGameCubeVtxDesc *desc, s32 attr, GXAttrType type);
void fn_8047046C(RwGameCubeVtxDesc *desc, u8 numArrays);

/* ---- The 2D toolkit (Rt2d): brushes and fonts -------------------------------------------- */

typedef struct RwRaster {
    /* 0x00 */ u8 unk0[0xC];
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
} RwRaster;
typedef struct RwTexDictionary RwTexDictionary;
typedef struct RwStream RwStream;
typedef struct RwSList RwSList;
typedef struct Rt2dPath Rt2dPath;

typedef struct RwTexture {
    /* 0x00 */ RwRaster *raster;
    /* 0x04 */ u8 unk4[0x54 - 0x04];
    /* 0x54 */ s32 refCount;
} RwTexture;

/* One corner of a brush (two bases and two edge deltas: corner[1] = corner - corner[0],
 * corner[3] = corner - corner[2], so a flat brush is the deltas being zero). */
typedef struct Rt2dBrushCorner {
    /* 0x00 */ RwRGBAReal color;
    /* 0x10 */ RwGameCubeV2d uv;
} Rt2dBrushCorner;

/* A 2D toolkit brush (Rt2dBrush, from a free list of 0x78-byte entries). */
typedef struct Rt2dBrush {
    /* 0x00 */ Rt2dBrushCorner v[4];
    /* 0x60 */ RwRGBA color;
    /* 0x64 */ u32 flags; /* bit 0: the colours vary, bit 1: textured or the UVs vary */
    /* 0x68 */ RwTexture *texture;
    /* 0x6C */ u32 unk6C;
    /* 0x70 */ f32 halfWidth;
    /* 0x74 */ s32 refCount;
} Rt2dBrush;

/* A glyph of a font (32 bytes). */
typedef struct Rt2dFontGlyph {
    /* 0x00 */ f32 width;
    /* 0x04 */ RwTexture *texture; /* from the font's textures; none for outline glyphs */
    /* 0x08 */ f32 rect[4];
    /* 0x18 */ Rt2dPath *path;     /* an outline font's glyph */
    /* 0x1C */ s8 textureIndex;
    /* 0x1D */ u8 unk1D[0x20 - 0x1D];
} Rt2dFontGlyph;

typedef struct Rt2dFont Rt2dFont;
typedef Rt2dFont *(*Rt2dFontShowFunc)(Rt2dFont *font, const RwChar *string, f32 height,
                                      RwV2d *anchor, Rt2dBrush *brush);

/* A 2D toolkit font (Rt2dFont, 0x144 bytes): glyph indices by character code, below 0x80
 * from a table and above it from a second table built by fn_80471300. */
struct Rt2dFont {
    /* 0x000 */ s32 isOutline;
    /* 0x004 */ f32 height;
    /* 0x008 */ f32 baseline;
    /* 0x00C */ f32 intergap;
    /* 0x010 */ u32 flag; /* bit 1: two-byte strings */
    /* 0x014 */ RwTexture *texture[4];
    /* 0x024 */ s16 map[0x80];
    /* 0x124 */ s32 wideBase;
    /* 0x128 */ s32 wideCount;
    /* 0x12C */ s16 *wideMap;
    /* 0x130 */ s32 glyphCount;
    /* 0x134 */ RwSList *glyphs;
    /* 0x138 */ Rt2dFontShowFunc show;
    /* 0x13C */ u32 unk13C;
    /* 0x140 */ u32 unk140;
};

/* An entry of the list of fonts read so far. */
typedef struct Rt2dFontDictNode Rt2dFontDictNode;
struct Rt2dFontDictNode {
    /* 0x0 */ RwChar *name;
    /* 0x4 */ Rt2dFont *font;
    /* 0x8 */ Rt2dFontDictNode *next;
};

/* The 2D toolkit's globals (lbl_805E1950, 0xD58 bytes, defined by the toolkit's device file):
 * the members the brush and font files use. */
typedef struct Rt2dGlobals {
    /* 0x000 */ u8 unk0[0x80];
    /* 0x080 */ u16 quadIndices[16][6]; /* two triangles per glyph quad */
    /* 0x140 */ RwFreeList *brushFreeList;
    /* 0x144 */ void *brushBuffer;
    /* 0x148 */ RwFreeList *fontFreeList;   /* 0x144-byte fonts */
    /* 0x14C */ RwChar *fontPath;           /* the font directory, with room for its size */
    /* 0x150 */ u32 fontPathSize;
    /* 0x154 */ RwFreeList *dictFreeList;   /* 0xC-byte Rt2dFontDictNode */
    /* 0x158 */ Rt2dFontDictNode *fontDict;
    /* 0x15C */ RwTexDictionary *texDict;
    /* 0x160 */ RwTexDictionary *currentTexDict;
    /* 0x164 */ u8 unk164[0x840 - 0x164];
    /* 0x840 */ Rt2dFont *(*fontRead)(const RwChar *name);
    /* 0x844 */ u8 unk844[0x94C - 0x844];
    /* 0x94C */ RwChar fontName[0xD4C - 0x94C]; /* a file name being built */
    /* 0xD4C */ s32 *glyphCodes; /* the character code of each glyph of the font being read */
    /* 0xD50 */ u32 numGlyphCodes;
    /* 0xD54 */ u8 unkD54[0xD58 - 0xD54];
} Rt2dGlobals;

extern Rt2dGlobals lbl_805E1950;

/* Free lists (RwCpp44 and neighbours). */
RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 numBlocks,
                        RwFreeList *freeList, u32 hint); /* RwFreeListCreateAndPreallocateSpace */
void fn_8047FCDC(RwFreeList *freeList);                  /* RwFreeListDestroy */
s32 fn_8048CD8C(RwTexture *texture);                     /* RwTextureDestroy */

#ifdef __cplusplus
}
#endif

#endif

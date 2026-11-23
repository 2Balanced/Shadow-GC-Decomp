/* Private types of the RenderWare GameCube driver units RwCpp54 and RwCpp56..RwCpp62, inferred
 * from the disassembly (offsets and sizes only where the code shows them). A later pass unifies
 * these with the neighbouring agents' headers. */
#ifndef GAME_RW_R7_H
#define GAME_RW_R7_H

#include <dolphin/types.h>

/* RenderWare's basic types. Its 32-bit integers are int, not long as in dolphin/types.h: MWCC
 * hoists and spills loop invariants of a long differently (fn_804900D4 matches only with int). */
typedef int RwInt32;
typedef unsigned int RwUInt32;
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef float RwReal;
typedef int RwBool;

/* RwVideoMode as the driver stores it: 0x18 bytes. */
typedef struct RwDlVideoMode {
    s32 width;
    s32 height;
    s32 depth;
    s32 flags;
    s32 refRate;
    s32 format;
} RwDlVideoMode;

/* .data 0x8056E348, 0x8C bytes: the three TV standards' modes, the mode in use and two messages.
 * Addressed from one base register, so it is one file's initialised data. */
typedef struct RwDlVideoData {
    RwDlVideoMode standard[3]; /* 0x00: NTSC, PAL/EURGB60, MPAL */
    RwDlVideoMode current;     /* 0x48 */
    char invalidPixelFormat[0x18]; /* 0x60 "Invalid pixel format" */
    char invalidTvFormat[0x14];    /* 0x78 "Invalid TV format" */
} RwDlVideoData;

/* One entry of the display queue at 0x805E3178 (three entries): the external frame buffer a
 * finished frame was copied to and the FIFO write pointer at which its copy command sits. */
typedef struct RwDlQueuedFrame {
    void *xfb;
    void *breakPt;
} RwDlQueuedFrame;

/* RwError as RwErrorSet takes it. */
typedef struct RwDlError {
    s32 pluginID;
    s32 errorCode;
} RwDlError;

/* RwRaster (public layout of RenderWare 3.7). */
typedef struct RwDlRaster {
    struct RwDlRaster *parent; /* 0x00 */
    u8 *cpPixels;              /* 0x04 */
    u8 *palette;               /* 0x08 */
    s32 width;                 /* 0x0C */
    s32 height;                /* 0x10 */
    s32 depth;                 /* 0x14 */
    s32 stride;                /* 0x18 */
    s16 nOffsetX;              /* 0x1C */
    s16 nOffsetY;              /* 0x1E */
    u8 cType;                  /* 0x20 */
    u8 cFlags;                 /* 0x21 */
    u8 privateFlags;           /* 0x22: 2/4 pixels locked for reading/writing, 8/0x10 palette */
    u8 cFormat;                /* 0x23: the raster format >> 8 */
    u8 *originalPixels;        /* 0x24 */
    s32 originalWidth;         /* 0x28 */
    s32 originalHeight;        /* 0x2C */
    s32 originalStride;        /* 0x30 */
} RwDlRaster;

/* RwRGBA */
typedef struct RwDlRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwDlRGBA;

/* An RwRGBA copied as C copies it (one word); C++ would copy the members one by one. */
typedef union RwDlColor {
    RwDlRGBA rgba;
    u32 word;
} RwDlColor;

/* RwImage (public layout). */
typedef struct RwDlImage {
    s32 flags;      /* 0x00 */
    s32 width;      /* 0x04 */
    s32 height;     /* 0x08 */
    s32 depth;      /* 0x0C */
    s32 stride;     /* 0x10 */
    u8 *cpPixels;   /* 0x14 */
    struct RwDlRGBA *palette; /* 0x18: [1 << depth] */
} RwDlImage;

/* The driver's data in a raster, at the offset in lbl_805F1658 (registered as a raster
 * extension). Only the members these units read are named. */
typedef struct RwDlRasterExt {
    u8 unk00[0xC];
    s32 texFormat;    /* 0x0C: GXTexFmt */
    u8 unk10[0xC];
    u8 *pixels;       /* 0x1C: the texture image, in GX's tiled layout */
    u8 unk20[0xC];
    void *region;     /* 0x2C: the GXTexRegion of a preloaded texture, else NULL */
    u8 unk30[2];
    u8 mipLevels;     /* 0x32: the mip levels made for it less one; 0xFF when not set */
    u8 sharedPalette; /* 0x33: nonzero when the palette is not this raster's to fill */
} RwDlRasterExt;

/* RenderWare's error reporting, as the RWERROR macro expands. An error record is a block-scoped
 * local of each use. */
#ifdef __cplusplus
extern "C" {
#endif
u32 fn_8047DC28(u32 code, ...);   /* _rwerror */
void fn_8047DB84(void *error);    /* RwErrorSet */
#ifdef __cplusplus
}
#endif

#define RW_ERROR(value)                                                                              do {                                                                                                 RwDlError error;                                                                                 error.pluginID = 1;                                                                              error.errorCode = fn_8047DC28(value);                                                            fn_8047DB84(&error);                                                                         } while (0)

#define RW_ERROR_STRING(value, text)                                                                 do {                                                                                                 RwDlError error;                                                                                 error.pluginID = 1;                                                                              error.errorCode = fn_8047DC28((value), (text));                                                  fn_8047DB84(&error);                                                                         } while (0)

/* RwRasterGetFormat */
#define RW_RASTER_FORMAT(raster) (((raster)->cFormat & 0xFF) << 8)

/* RwRect */
typedef struct RwDlRect {
    s32 x;
    s32 y;
    s32 w;
    s32 h;
} RwDlRect;

/* The driver's data in a raster, at the offset in lbl_805F1670 (a raster plugin, ID 0x40C):
 * the GX texture object and its state. */
typedef struct RwDlTexExt {
    u32 texObj[8]; /* 0x00: GXTexObj */
    u32 flags;     /* 0x20: 0x01000000 = the texture object has not been set up */
} RwDlTexExt;

/* The data the immediate-mode pipelines get (0x30 bytes). */
typedef struct RwDlIm3DRender {
    u32 flags;           /* 0x00 */
    void *ltm;           /* 0x04 */
    u32 unk08[6];        /* 0x08 */
    void *pipeline;      /* 0x20 */
    RwInt32 primType;    /* 0x24 */
    void *indices;       /* 0x28 */
    RwInt32 numIndices;  /* 0x2C */
} RwDlIm3DRender;

/* What RwIm3DTransform leaves for the immediate-mode pipelines (0x3C bytes, cleared by
 * RwIm3DEnd): the vertex pool and the render data. */
typedef struct RwDlIm3DStash {
    u16 numVerts;          /* 0x00 */
    u16 pad02;
    void *verts;           /* 0x04 */
    u32 stride;            /* 0x08: 0x24, an RwIm3DVertex */
    RwDlIm3DRender render; /* 0x0C */
} RwDlIm3DStash;

/* The immediate-mode module's globals (0x74 bytes), at lbl_805F1684 in the engine instance. */
typedef struct RwDlIm3DGlobals {
    void *transformPipeline;          /* 0x00: the one in use */
    void *renderPipelines[6];         /* 0x04: per primitive type */
    void *platformTransformPipeline;  /* 0x1C: the driver's own */
    void *platformRenderPipelines[6]; /* 0x20 */
    RwDlIm3DStash stash;              /* 0x38 */
} RwDlIm3DGlobals;

/* RwModuleInfo: where a module's globals are in the engine instance, and its users. */
typedef struct RwDlModuleInfo {
    RwInt32 globalsOffset; /* 0x00 */
    RwInt32 numInstances;  /* 0x04 */
} RwDlModuleInfo;

/* PowerPipe (Rx) structures (public layouts). */
typedef struct RwDlClusterRef {
    void *clusterDef;      /* 0x00: RxClusterDefinition */
    RwInt32 forcePresent;  /* 0x04 */
    RwUInt32 reserved;     /* 0x08 */
} RwDlClusterRef;

typedef struct RwDlNodeDefinition {
    char *name;                         /* 0x00 */
    void *nodeMethods[7];               /* 0x04 */
    RwUInt32 numClustersOfInterest;     /* 0x20: io */
    RwDlClusterRef *clustersOfInterest; /* 0x24 */
    void *inputRequirements;            /* 0x28 */
    RwUInt32 numOutputs;                /* 0x2C */
    void *outputs;                      /* 0x30 */
} RwDlNodeDefinition;

/* A node's sort data (0x0C bytes). */
typedef struct RwDlTopSortData {
    u32 word[3];
} RwDlTopSortData;

typedef struct RwDlPipelineNode {
    RwDlNodeDefinition *nodeDef;      /* 0x00 */
    RwUInt32 numOutputs;              /* 0x04 */
    RwUInt32 *outputs;                /* 0x08 */
    void *slotClusterRefs;            /* 0x0C */
    void *slotsContinue;              /* 0x10 */
    void *privateData;                /* 0x14 */
    void *inputToClusterSlot;         /* 0x18 */
    RwDlTopSortData *topSortData;     /* 0x1C */
    void *initializationData;         /* 0x20 */
    RwUInt32 initializationDataSize;  /* 0x24 */
} RwDlPipelineNode;

typedef struct RwDlPipeline {
    RwBool locked;                  /* 0x00 */
    RwUInt32 numNodes;              /* 0x04 */
    RwDlPipelineNode *nodes;        /* 0x08 */
    RwUInt32 packetNumClusterSlots; /* 0x0C */
    RwInt32 embeddedPacketState;    /* 0x10 */
    struct RwDlPacket *embeddedPacket; /* 0x14 */
    RwUInt32 numInputRequirements;  /* 0x18 */
    void *inputRequirements;        /* 0x1C */
    void *superBlock;               /* 0x20 */
    RwUInt32 superBlockSize;        /* 0x24 */
    RwUInt32 entryPoint;            /* 0x28 */
    RwUInt32 pluginId;              /* 0x2C */
    RwUInt32 pluginData;            /* 0x30 */
} RwDlPipeline;

typedef struct RwDlCluster {
    RwUInt16 flags;       /* 0x00 */
    RwUInt16 stride;      /* 0x02 */
    void *data;           /* 0x04 */
    void *currentData;    /* 0x08 */
    RwUInt32 numAlloced;  /* 0x0C */
    RwUInt32 numUsed;     /* 0x10 */
    void *clusterRef;     /* 0x14 */
    RwUInt32 attributes;  /* 0x18 */
} RwDlCluster;

typedef struct RwDlPacket {
    RwUInt16 flags;           /* 0x00 */
    RwUInt16 numClusters;     /* 0x02 */
    RwDlPipeline *pipeline;   /* 0x04 */
    void *inputToClusterSlot; /* 0x08 */
    void *slotsContinue;      /* 0x0C */
    void *slotClusterRefs;    /* 0x10 */
    RwDlCluster clusters[1];  /* 0x14 */
} RwDlPacket;

/* The driver's render mode objects. RenderWare is C and copies a GXRenderModeObj a word at a
 * time; C++ copies that structure member by member, but a union bitwise, as C does. */
#ifdef DOLPHIN_GX_H
typedef union RwDlRenderMode {
    GXRenderModeObj gx;
    u32 word[15];
} RwDlRenderMode;
#endif

/* The palette quantiser's colour tree: each level splits on one bit of each of R, G, B and A, so
 * a node has 16 children (0x5C bytes, from a free list). */
typedef struct RwDlPalNode {
    u32 unk00[7];                  /* 0x00 */
    struct RwDlPalNode *child[16]; /* 0x1C */
} RwDlPalNode;

/* The palette quantiser's state (callers keep it on the stack). */
typedef struct RwDlPalQuant {
    u8 unk0000[0x4000];   /* 0x0000 */
    RwDlPalNode *root;    /* 0x4000 */
    void *nodeFreeList;   /* 0x4004 */
} RwDlPalQuant;

/* RwV3d, RwMatrix (public layouts). */
typedef struct RwDlV3d {
    f32 x;
    f32 y;
    f32 z;
} RwDlV3d;

typedef struct RwDlMatrix {
    RwDlV3d right; /* 0x00 */
    u32 flags;     /* 0x0C */
    RwDlV3d up;    /* 0x10 */
    u32 pad1;
    RwDlV3d at;    /* 0x20 */
    u32 pad2;
    RwDlV3d pos;   /* 0x30 */
    u32 pad3;
} RwDlMatrix;

/* RwCamera: only the members these units read. */
typedef struct RwDlCamera {
    u8 unk00[0x60];
    RwDlRaster *frameBuffer; /* 0x60 */
    RwDlRaster *zBuffer;     /* 0x64 */
    u8 unk68[0x18];
    f32 nearPlane;           /* 0x80 */
    f32 farPlane;            /* 0x84 */
    f32 fogPlane;            /* 0x88 */
} RwDlCamera;

/* RwTexture (public layout). */
typedef struct RwDlTexture {
    RwDlRaster *raster;        /* 0x00 */
    void *dict;                /* 0x04 */
    void *lInDictionary[2];    /* 0x08 */
    char name[32];             /* 0x10 */
    char mask[32];             /* 0x30 */
    RwUInt32 filterAddressing; /* 0x50: filter mode, U and V addressing in nibbles 2 and 3 */
    RwInt32 refCount;          /* 0x54 */
} RwDlTexture;

/* The driver's render state cache (lbl_805E3210, 0x50 bytes). */
typedef struct RwDlRenderState {
    RwInt32 zWrite;       /* 0x00 */
    RwInt32 zTest;        /* 0x04 */
    RwInt32 zFunc;        /* 0x08: GXCompare */
    RwInt32 cullMode;     /* 0x0C: RwCullMode */
    RwInt32 fogEnabled;   /* 0x10 */
    RwInt32 fogType;      /* 0x14: RwFogType */
    RwUInt32 fogColorPacked; /* 0x18: the fog colour as the application gave it */
    RwDlColor fogColor;   /* 0x1C */
    RwInt32 fogEndSet;    /* 0x20: nonzero once the application has set the fog end */
    f32 fogStart;         /* 0x24 */
    f32 fogEnd;           /* 0x28 */
    f32 fogNear;          /* 0x2C */
    f32 fogFar;           /* 0x30 */
    RwInt32 srcBlend;     /* 0x34: RwBlendFunction */
    RwInt32 destBlend;    /* 0x38 */
    RwInt32 alphaTestOff; /* 0x3C: 1 while the alpha test is off (Z compared before texturing) */
    RwInt32 alphaComp0;   /* 0x40: the alpha test kept for textures with alpha */
    RwInt32 alphaComp1;   /* 0x44 */
    RwInt32 alphaOp;      /* 0x48 */
    u8 alphaRef0;         /* 0x4C */
    u8 alphaRef1;         /* 0x4D */
    u8 alphaSet;          /* 0x4E */
} RwDlRenderState;

/* RwGlobals, which RwEngineInstance points to. */
typedef struct RwDlGlobals {
    RwDlCamera *curCamera;                            /* 0x000 */
    u8 unk004[0xE4];
    RwInt32 (*strcmp)(const char *a, const char *b);  /* 0x0E8: rwstrcmp */
    u8 unk0EC[0x20];
    void (*free)(void *block);                        /* 0x10C: RwFree */
    u8 unk110[0x8];
    void *(*freeListAlloc)(void *freeList, u32 hint);  /* 0x118: RwFreeListAlloc */
    void (*freeListFree)(void *freeList, void *block); /* 0x11C: RwFreeListFree */
} RwDlGlobals;

/* RwIm2DVertex of the GameCube driver (0x18 bytes). */
typedef struct RwDlIm2DVertex {
    f32 x;          /* 0x00 */
    f32 y;          /* 0x04 */
    f32 z;          /* 0x08 */
    RwDlRGBA color; /* 0x0C */
    f32 u;          /* 0x10 */
    f32 v;          /* 0x14 */
} RwDlIm2DVertex;

/* SDK functions dolphin/gx.h does not declare yet (GXLinkNames.h maps them). */
#ifdef DOLPHIN_GX_H
#ifdef __cplusplus
extern "C" {
#endif
void GXGetProjectionv(f32 *ptr);
void GXSetViewportJitter(f32 left, f32 top, f32 wd, f32 ht, f32 nearz, f32 farz, u32 field);
#ifdef __cplusplus
}
#endif
#endif

/* The SDK's vertex writers (GXVert.h): every value goes through the write-gather pipe. */
#ifdef DOLPHIN_GX_GXPRIVATE_H
static inline void GXPosition3f32(const f32 x, const f32 y, const f32 z) {
    GXWGFifo.f32 = x;
    GXWGFifo.f32 = y;
    GXWGFifo.f32 = z;
}

static inline void GXColor4u8(const u8 r, const u8 g, const u8 b, const u8 a) {
    GXWGFifo.u8 = r;
    GXWGFifo.u8 = g;
    GXWGFifo.u8 = b;
    GXWGFifo.u8 = a;
}

static inline void GXTexCoord2f32(const f32 s, const f32 t) {
    GXWGFifo.f32 = s;
    GXWGFifo.f32 = t;
}
#endif

#endif

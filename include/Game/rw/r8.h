#ifndef GAME_RW_R8_H
#define GAME_RW_R8_H

/* Private types of the RenderWare units RwCpp63..RwCpp70 (0x8049D934..0x804A9BF8), inferred from
 * the disassembly and the public RenderWare 3.7 object model.  A later pass unifies these with the
 * neighbouring agents' headers; nothing outside those units should include this file. */
#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *memset(void *dst, int value, unsigned long size);
void *memcpy(void *dst, const void *src, unsigned long size);

typedef struct RwFrame RwFrame;
typedef struct RwStream RwStream;
typedef struct RwTexDictionary RwTexDictionary;
typedef struct RwV3d {
    f32 x;
    f32 y;
    f32 z;
} RwV3d;

typedef struct RwMatrixTag {
    RwV3d right; /* 0x00 */
    u32 flags;   /* 0x0C */
    RwV3d up;    /* 0x10 */
    u32 pad1;
    RwV3d at;    /* 0x20 */
    u32 pad2;
    RwV3d pos;   /* 0x30 */
    u32 pad3;
} RwMatrix;

typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

typedef struct RwError {
    s32 pluginID;
    s32 errorCode;
} RwError;

typedef struct RwRaster {
    struct RwRaster *parent; /* 0x00 */
    u8 *cpPixels;            /* 0x04 */
    u8 *palette;             /* 0x08 */
    s32 width;               /* 0x0C */
    s32 height;              /* 0x10 */
    s32 depth;               /* 0x14 */
    s32 stride;              /* 0x18 */
    s16 nOffsetX;            /* 0x1C */
    s16 nOffsetY;            /* 0x1E */
    u8 cType;                /* 0x20 */
    u8 cFlags;               /* 0x21 */
    u8 privateFlags;         /* 0x22 */
    u8 cFormat;              /* 0x23 */
} RwRaster;

typedef struct RwTexture {
    RwRaster *raster;      /* 0x00 */
    RwTexDictionary *dict; /* 0x04 */
    void *linkNext;        /* 0x08 */
    void *linkPrev;        /* 0x0C */
    char name[32];         /* 0x10 */
    char mask[32];         /* 0x30 */
    u32 filterAddressing;  /* 0x50 */
    s32 refCount;          /* 0x54 */
} RwTexture;

typedef struct RwImage {
    s32 flags;       /* 0x00 */
    s32 width;       /* 0x04 */
    s32 height;      /* 0x08 */
    s32 depth;       /* 0x0C */
    s32 stride;      /* 0x10 */
    u8 *cpPixels;    /* 0x14 */
    RwRGBA *palette; /* 0x18 */
} RwImage;

typedef struct RwSurfaceProperties {
    f32 ambient;
    f32 specular;
    f32 diffuse;
} RwSurfaceProperties;

typedef struct RpMaterial {
    RwTexture *texture;               /* 0x00 */
    RwRGBA color;                     /* 0x04 */
    void *pipeline;                   /* 0x08 */
    RwSurfaceProperties surfaceProps; /* 0x0C */
    s16 refCount;                     /* 0x18 */
    s16 pad;
} RpMaterial;

/* The engine globals (RwEngineInstance, lbl_805F15B4): only the slots these units use. */
typedef struct RwGlobals {
    u32 unk0;
    void *curWorld; /* 0x04 */
    u8 pad08[0x18];
    s32 (*renderStateSet)(s32 state, void *value); /* 0x20 */
    s32 (*renderStateGet)(s32 state, void *value); /* 0x24 */
    u8 pad28[0x108 - 0x28];
    void *(*memAlloc)(u32 size, u32 hint);              /* 0x108: RwMalloc */
    void (*memFree)(void *block);                       /* 0x10C: RwFree */
    void *(*memRealloc)(void *block, u32 size, u32 hint); /* 0x110: RwRealloc */
    void *(*memCalloc)(u32 count, u32 size, u32 hint);  /* 0x114: RwCalloc */
    void *(*freeListAlloc)(void *freeList, u32 hint); /* 0x118 */
    void (*freeListFree)(void *freeList, void *block); /* 0x11C */
    u32 unk120;
    s32 engineStatus; /* 0x124: 3 once the engine is started */
} RwGlobals;

extern RwGlobals *lbl_805F15B4;
#define RwEngineInstance lbl_805F15B4

s32 fn_8047DC28(s32 code, ...); /* _rwerror: formats an error code, returns the code to store */

/* RenderWare's error report: the plugin id, then the code the formatter returns. */
#define RWERROR(plugin, args)                                                                      \
    do {                                                                                           \
        RwError rwErr;                                                                             \
        rwErr.pluginID = (plugin);                                                                 \
        rwErr.errorCode = fn_8047DC28 args;                                                        \
        fn_8047DB84(&rwErr);                                                                       \
    } while (0)

/* ---- material effects plugin (rpmatfx, plugin id 0x120) ---- */

enum {
    rpMATFXEFFECTNULL = 0,
    rpMATFXEFFECTBUMPMAP = 1,
    rpMATFXEFFECTENVMAP = 2,
    rpMATFXEFFECTBUMPENVMAP = 3,
    rpMATFXEFFECTDUAL = 4,
    rpMATFXEFFECTUVTRANSFORM = 5,
    rpMATFXEFFECTDUALUVTRANSFORM = 6
};

typedef struct MatFXBumpMapData {
    RwFrame *frame;         /* 0x00 */
    RwTexture *bumpTexture; /* 0x04 */
    RwTexture *texture;     /* 0x08 */
    f32 coef;               /* 0x0C, stored negated */
    f32 invBumpWidth;       /* 0x10 */
} MatFXBumpMapData;

typedef struct MatFXEnvMapData {
    RwFrame *frame;          /* 0x00 */
    RwTexture *texture;      /* 0x04 */
    f32 coef;                /* 0x08 */
    s32 useFrameBufferAlpha; /* 0x0C */
} MatFXEnvMapData;

typedef struct MatFXDualData {
    RwTexture *texture; /* 0x00 */
    s32 srcBlendMode;   /* 0x04 */
    s32 dstBlendMode;   /* 0x08 */
} MatFXDualData;

typedef struct MatFXUVTransformData {
    RwMatrix *baseTransform; /* 0x00 */
    RwMatrix *dualTransform; /* 0x04 */
} MatFXUVTransformData;

typedef struct MatFXEffectData {
    union {
        MatFXBumpMapData bump;
        MatFXEnvMapData env;
        MatFXDualData dual;
        MatFXUVTransformData uv;
    } u;
    s32 flag; /* 0x14: one of rpMATFXEFFECT* (never the two combined ones) */
} MatFXEffectData;

typedef struct MatFXMaterialData {
    MatFXEffectData data[2]; /* 0x00 */
    s32 flags;               /* 0x30: the material's rpMATFXEFFECT* value */
} MatFXMaterialData;

/* lbl_8056ED00: the plugin's module record; +8 is the free list of MatFXMaterialData. */
typedef struct MatFXInfo {
    s32 globalsOffset;
    s32 numInstances;
    void *materialDataFreeList;
} MatFXInfo;

extern MatFXInfo lbl_8056ED00;
extern s32 lbl_805F16A8; /* plugin data offset in RpMaterial */
extern s32 lbl_805F16AC; /* plugin data offset in RpAtomic */
extern s32 lbl_805F16B0; /* plugin data offset in RpWorldSector */

#define MATFXMATERIALGETDATA(material) (*(MatFXMaterialData **)((u8 *)(material) + lbl_805F16A8))

/* ---- GameCube driver ---- */
#ifndef DOLPHIN_GX_H
typedef struct GXColor {
    u8 r, g, b, a;
} GXColor;
#endif

typedef struct RwRGBAReal {
    f32 red;
    f32 green;
    f32 blue;
    f32 alpha;
} RwRGBAReal;

/* What the instancing code hands the per-mesh render callbacks about the lights in effect. */
typedef struct RwGameCubeLightState {
    u32 unk0;
    u32 unk4;
    u32 flags;          /* 0x08: 8 = prelit, 0x40 = modulate with the material colour,
                           4 / 0x80 = textured (one / two texture coordinate sets) */
    RwRGBAReal ambient; /* 0x0C */
    s32 ambientLit;     /* 0x1C */
    u32 lightMask;      /* 0x20 */
    s32 numLights;      /* 0x24 */
} RwGameCubeLightState;

void fn_80396A6C(s32 channel, GXColor color); /* GXSetChanAmbColor */
void fn_80396B54(s32 channel, GXColor color); /* GXSetChanMatColor */
void fn_80396C3C(u8 count);                   /* GXSetNumChans */
void fn_80396C78(s32 channel, u8 enable, s32 ambSrc, s32 matSrc, u32 lightMask, s32 diffuseFn,
                 s32 attnFn);                 /* GXSetChanCtrl */
void fn_803951BC(u8 count);                   /* GXSetNumTexGens */
void fn_80394F3C(s32 coord, s32 func, s32 src, u32 mtx, u8 normalize,
                 u32 postMtx);                /* GXSetTexCoordGen2 */
void fn_803989A0(u8 count);                   /* GXSetNumTevStages */
void fn_80398804(s32 stage, s32 coord, s32 map, s32 color); /* GXSetTevOrder */
void fn_803982A8(s32 stage, s32 mode);        /* GXSetTevOp */
void fn_80399000(void *list, u32 size);       /* GXCallDisplayList */
void fn_803992BC(f32 mtx[][4], u32 id, s32 type); /* GXLoadTexMtxImm */

/* One mesh as the instancing code hands it to a render callback (+8 is all these units read). */
typedef struct RwGameCubeMesh {
    u32 unk0;
    u32 unk4;
    struct RpMaterial *material; /* 0x08 */
} RwGameCubeMesh;

typedef struct RwGameCubeDisplayList {
    void *data;
    u32 size;
} RwGameCubeDisplayList;

/* The driver's raster extension (offset lbl_805F1658 in RwRaster). */
typedef struct RwGameCubeRasterExt {
    u8 pad00[0x14];
    u32 flags; /* 0x14: bit 0 = the texture has alpha */
} RwGameCubeRasterExt;

extern s32 lbl_805F1658;
#define RWGAMECUBERASTEREXT(raster) ((RwGameCubeRasterExt *)((u8 *)(raster) + lbl_805F1658))

/* What the material effects render callbacks have set for the atomic being drawn. */
typedef struct MatFXRenderState {
    s32 lightingSet;
    struct RpMaterial *material;
    s32 tevSet;
} MatFXRenderState;

extern MatFXRenderState lbl_805E32C8;
extern RwMatrix lbl_805E31CC; /* identity */

void fn_80499C38(struct RwTexture *texture, s32 stage); /* driver: select the texture */
void fn_80497778(s32 opaque);                           /* driver: alpha test/blend switch */
RwMatrix *RwFrameGetLTM(RwFrame *frame);
RwMatrix *fn_8047F0B0(RwMatrix *dst, const RwMatrix *src); /* RwMatrixInvert */
RwMatrix *fn_8047ECB4(RwMatrix *dst, const RwMatrix *src); /* RwMatrixOrthoNormalize */
RwMatrix *fn_8047EBB8(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b); /* RwMatrixMultiply */

/* ---- world objects, as far as the GameCube pipelines read them ---- */
typedef struct RpGeometry {
    u8 pad00[8];
    u32 flags;               /* 0x08: 0x10 = rpGEOMETRYNORMALS */
    u16 lockedSinceLastInst; /* 0x0C */
    s16 refCount;            /* 0x0E */
    s32 numTriangles;        /* 0x10 */
    s32 numVertices;         /* 0x14 */
    s32 numMorphTargets;     /* 0x18 */
} RpGeometry;

typedef struct RpAtomic {
    u8 type;              /* 0x00: rpATOMIC = 1 */
    u8 pad01[3];
    RwFrame *frame;       /* 0x04 */
    u8 pad08[0x10];
    RpGeometry *geometry; /* 0x18 */
    u8 pad1C[0x30];
    u32 interpolatorFlags; /* 0x4C */
    u8 pad50[0x1C];
    struct RxPipeline *pipeline; /* 0x6C */
} RpAtomic;

typedef struct RpWorldSector {
    u8 type; /* 0x00 */
    u8 pad01[0x6B];
    RwV3d origin; /* 0x6C: what the quantised vertex positions are relative to */
    u8 pad78[4];
    struct RxPipeline *pipeline; /* 0x7C */
    u16 pad80;
    u16 numVertices; /* 0x82 */
} RpWorldSector;

/* The driver's vertex format description (plugin data of RpGeometry at lbl_805F1520 and of
 * RpWorld at lbl_805F1524). */
typedef struct RwGameCubeVertexFormat {
    u8 positionType;  /* 0x00: 4 = float positions */
    u8 normalType;    /* 0x01: 1 = u8, 3 = s16, otherwise float normals (with binormal and tangent) */
    u8 pad02[0x0A];
    u8 positionShift; /* 0x0C: fraction bits of quantised positions */
    u8 skinned;       /* 0x0D: tested before fn_804A8120 */
} RwGameCubeVertexFormat;

extern s32 lbl_805F1520;
extern s32 lbl_805F1524;

/* A display list entry of the instanced data (8 bytes each). */
typedef struct RwGameCubeMeshList RwGameCubeMeshList;

typedef struct RwGameCubeInstance {
    u16 serial; /* 0x00 */
    u16 pad02;
    u32 unk4;
    u32 numStreams; /* 0x08 */
    struct {
        void *data;
        u32 info;
    } streams[1]; /* 0x0C: numStreams of them; the meshes' display lists follow the last */
} RwGameCubeInstance;

typedef struct RpMeshHeader {
    u32 flags;
    u16 numMeshes; /* 0x04 */
    u16 serialNum;
    u32 totalIndicesInMesh;
    u32 firstMeshOffset;
    /* RpMesh[numMeshes], 12 bytes each, follow at 0x10 */
} RpMeshHeader;

typedef struct RpMesh {
    void *indices;               /* 0x00 */
    u32 numIndices;              /* 0x04 */
    struct RpMaterial *material; /* 0x08 */
} RpMesh;

/* The repEntry-like node the driver hands the instance and render callbacks. */
typedef struct RwGameCubeRenderNode {
    u8 *resEntry;             /* 0x00: the instance starts 0x18 bytes in */
    RpMeshHeader *meshHeader; /* 0x04 */
    u32 flags;                /* 0x08: 0x10 = has normals */
} RwGameCubeRenderNode;

/* ---- pipelines (rxcore) ---- */
typedef struct RxClusterDefinition {
    char *name;               /* 0x00 */
    u32 defaultStride;        /* 0x04 */
    u32 defaultAttributes;    /* 0x08 */
    const char *attributeSet; /* 0x0C */
} RxClusterDefinition;

typedef struct RxClusterRef {
    RxClusterDefinition *clusterDef; /* 0x00 */
    s32 forcePresent;                /* 0x04 */
    u32 reserved;                    /* 0x08 */
} RxClusterRef;

typedef struct RxOutputSpec {
    char *name;           /* 0x00 */
    s32 *outputClusters;  /* 0x04: one RxClusterValid per cluster of interest */
    s32 allOtherClusters; /* 0x08 */
} RxOutputSpec;

typedef struct RxIoSpec {
    u32 numClustersOfInterest;        /* 0x00 */
    RxClusterRef *clustersOfInterest; /* 0x04 */
    s32 *inputRequirements;           /* 0x08: one rxCLREQ_* per cluster of interest */
    u32 numOutputs;                   /* 0x0C */
    RxOutputSpec *outputs;            /* 0x10 */
} RxIoSpec;

typedef struct RxNodeDefinition {
    char *name;                      /* 0x00 */
    void *nodeMethods[7];            /* 0x04 */
    RxIoSpec io;                     /* 0x20 */
    u32 pipelineNodePrivateDataSize; /* 0x34 */
    s32 editable;                    /* 0x38 */
    s32 inputPipesCnt;               /* 0x3C */
} RxNodeDefinition;

/* One cluster of a pipeline: its definition and the attributes it is created with. */
typedef struct RxPipelineCluster {
    RxClusterDefinition *clusterRef; /* 0x00 */
    u32 creationAttributes;          /* 0x04 */
} RxPipelineCluster;

struct RxDepReq;

typedef struct RxPipelineNodeTopSortData {
    u32 numIns;            /* 0x00 */
    u32 numInsVisited;     /* 0x04 */
    struct RxDepReq *req;  /* 0x08 */
} RxPipelineNodeTopSortData;

typedef struct RxPipelineNode {
    RxNodeDefinition *nodeDef;              /* 0x00 */
    u32 numOutputs;                         /* 0x04 */
    u32 *outputs;                           /* 0x08: node index per output, ~0 = none */
    RxPipelineCluster **slotClusterRefs;    /* 0x0C */
    u32 *slotsContinue;                     /* 0x10 */
    void *privateData;                      /* 0x14 */
    u32 *inputToClusterSlot;                /* 0x18 */
    RxPipelineNodeTopSortData *topSortData; /* 0x1C */
    void *initializationData;               /* 0x20 */
    u32 initializationDataSize;             /* 0x24 */
} RxPipelineNode;

typedef struct RxPipeline {
    s32 locked;                /* 0x00 */
    u32 numNodes;              /* 0x04 */
    RxPipelineNode *nodes;     /* 0x08 */
    u32 packetNumClusterSlots; /* 0x0C */
    s32 embeddedPacketState;   /* 0x10 */
    struct RxPacket *embeddedPacket; /* 0x14 */
    u32 numInputRequirements;  /* 0x18 */
    void *inputRequirements;   /* 0x1C */
    void *superBlock;          /* 0x20 */
    u32 superBlockSize;        /* 0x24 */
    u32 entryPoint;            /* 0x28 */
    u32 pluginId;              /* 0x2C */
    u32 pluginData;            /* 0x30 */
} RxPipeline;

/* The dependency chase's view of one node: which clusters it touches (RwCpp63). */
typedef struct RxDepEntry {
    RxClusterDefinition *clusterDef; /* 0x00 */
    s32 required;                    /* 0x04: the node's rxCLREQ_* for the cluster */
    u32 unk08;
    struct RxDepGroup *group;        /* 0x0C: the set of entries that share one packet slot */
    struct RxDepEntry *next;         /* 0x10: next entry of that set */
    struct RxDepReq *req;            /* 0x14: owner */
    u32 outputMask;                  /* 0x18: outputs the cluster leaves by */
    u32 slot;                        /* 0x1C: packet slot */
    u32 unk20;
} RxDepEntry;

typedef struct RxDepReq {
    u32 numEntries;        /* 0x00 */
    u32 maxEntries;        /* 0x04 */
    u32 slotsInUse;        /* 0x08: bit per packet slot taken by a cluster live here */
    RxDepEntry *entries;   /* 0x0C */
    RxPipelineNode *node;  /* 0x10 */
} RxDepReq;

/* A set of entries that must share a slot; merged sets hang off the root's chain. */
typedef struct RxDepGroup {
    RxDepEntry *entries;       /* 0x00 */
    struct RxDepGroup *chain;  /* 0x04 */
    struct RxDepGroup *next;   /* 0x08: next root in the pipeline's list */
    struct RxDepGroup *parent; /* 0x0C: the root this set was merged into */
} RxDepGroup;

RxPipeline *fn_8049B76C(void);                 /* RxPipelineCreate */
void fn_8049B800(RxPipeline *pipeline);        /* _rxPipelineDestroy */
RxPipeline *fn_8049CCB8(RxPipeline *pipeline); /* RxPipelineLock */
RxPipeline *fn_8049C1E0(RxPipeline *pipeline); /* RxLockedPipeUnlock */
RxPipeline *fn_8049D0D0(RxPipeline *pipeline, u32 *firstIndex, RxNodeDefinition *nodeDef,
                        ...);                  /* RxLockedPipeAddFragment */
RxPipeline *fn_8049AC90(RxPipeline *pipeline); /* RwIm3DSetTransformPipeline */
RxPipeline *fn_8049ACE0(RxPipeline *pipeline, s32 primType); /* RwIm3DSetRenderPipeline */

void *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                  void *inPlaceSpace, u32 hint); /* RwFreeListCreateAndPreallocateSpace */
s32 fn_8047FCDC(void *freeList);                 /* RwFreeListDestroy */

/* ---- functions of other RenderWare units, by what they do ---- */
RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length); /* RwStreamWrite */
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);            /* RwStreamRead */
RwStream *fn_8047D350(RwStream *stream, const f32 *reals, u32 bytes);  /* RwStreamWriteReal32 */
RwStream *fn_8047D458(RwStream *stream, const s32 *ints, u32 bytes);   /* RwStreamWriteInt32 */
RwStream *fn_8047D784(RwStream *stream, f32 *reals, u32 bytes);        /* RwStreamReadReal32 */
RwStream *fn_8047D868(RwStream *stream, s32 *ints, u32 bytes);         /* RwStreamReadInt32 */
s32 fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version); /* RwStreamFindChunk */
RwError *fn_8047DB84(RwError *error);                                  /* RwErrorSet */
RwError *fn_8047DBE4(RwError *error);                                  /* RwErrorGet */
u32 fn_80483B38(const RwTexture *texture);                             /* RwTextureStreamGetSize */
const RwTexture *fn_80483BA0(const RwTexture *texture, RwStream *stream); /* RwTextureStreamWrite */
RwTexture *fn_80483D28(RwStream *stream);                              /* RwTextureStreamRead */
RwTexture *fn_8048CCDC(RwRaster *raster);                              /* RwTextureCreate */
s32 fn_8048CD8C(RwTexture *texture);                                   /* RwTextureDestroy */
RwTexture *fn_8048CE54(RwTexture *texture, const char *name);          /* RwTextureSetName */
RwTexture *fn_8048CFAC(RwTexDictionary *dict, RwTexture *texture);     /* RwTexDictionaryAddTexture */
RwTexture *fn_8048CFFC(RwTexDictionary *dict, const char *name); /* RwTexDictionaryFindNamedTexture */
RwTexDictionary *fn_8048D0D8(void);                                    /* RwTexDictionaryGetCurrent */
RwImage *fn_80487D08(s32 width, s32 height, s32 depth);                /* RwImageCreate */
s32 fn_80487DB0(RwImage *image);                                       /* RwImageDestroy */
RwImage *fn_80487E18(RwImage *image);                                  /* RwImageAllocatePixels */
RwImage *fn_80487FA8(RwImage *image);                                  /* RwImageMakeMask */
RwImage *fn_80488088(RwImage *image, const RwImage *mask);             /* RwImageApplyMask */
RwImage *fn_804899A8(RwImage *image, RwRaster *raster);                /* RwImageSetFromRaster */
RwRaster *fn_80489A1C(RwRaster *raster, RwImage *image);               /* RwRasterSetFromImage */
RwImage *fn_80489A90(RwImage *image, s32 type, s32 *width, s32 *height, s32 *depth,
                     s32 *format);                                     /* RwImageFindRasterFormat */
RwRaster *fn_80489E74(s32 width, s32 height, s32 depth, s32 flags);    /* RwRasterCreate */
RwImage *fn_8048A790(RwImage *dst, const RwImage *src);                /* RwImageResample */

typedef void *(*RwPluginObjectConstructor)(void *object, s32 offset, s32 size);
typedef void *(*RwPluginObjectDestructor)(void *object, s32 offset, s32 size);
typedef void *(*RwPluginObjectCopy)(void *dst, const void *src, s32 offset, s32 size);
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, s32 length, void *object,
                                                   s32 offset, s32 size);
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, s32 length,
                                                    const void *object, s32 offset, s32 size);
typedef s32 (*RwPluginDataChunkGetSizeCallBack)(const void *object, s32 offset, s32 size);

s32 fn_80485D78(s32 size, u32 id, RwPluginObjectConstructor open,
                RwPluginObjectDestructor close); /* RwEngineRegisterPlugin */
s32 fn_8045B2CC(s32 size, u32 id, RwPluginObjectConstructor ctor, RwPluginObjectDestructor dtor,
                RwPluginObjectCopy copy); /* RpMaterialRegisterPlugin */
s32 fn_8045B30C(u32 id, RwPluginDataChunkReadCallBack read, RwPluginDataChunkWriteCallBack write,
                RwPluginDataChunkGetSizeCallBack getSize); /* RpMaterialRegisterPluginStream */
s32 fn_80458B6C(s32 size, u32 id, RwPluginObjectConstructor ctor, RwPluginObjectDestructor dtor,
                RwPluginObjectCopy copy); /* RpAtomicRegisterPlugin */
s32 fn_80458BEC(u32 id, RwPluginDataChunkReadCallBack read, RwPluginDataChunkWriteCallBack write,
                RwPluginDataChunkGetSizeCallBack getSize); /* RpAtomicRegisterPluginStream */
s32 fn_8045F2F8(s32 size, u32 id, RwPluginObjectConstructor ctor, RwPluginObjectDestructor dtor,
                RwPluginObjectCopy copy); /* RpWorldSectorRegisterPlugin */
s32 fn_8045F338(u32 id, RwPluginDataChunkReadCallBack read, RwPluginDataChunkWriteCallBack write,
                RwPluginDataChunkGetSizeCallBack getSize); /* RpWorldSectorRegisterPluginStream */

/* ---- the pipelines' heap (RxHeap) ---- */
typedef struct RxHeapFreeEntry RxHeapFreeEntry;

/* Every block of a heap starts with this header; free memory follows it. */
typedef struct RxHeapBlockHeader {
    struct RxHeapBlockHeader *prev; /* 0x00 */
    struct RxHeapBlockHeader *next; /* 0x04 */
    u32 size;                       /* 0x08: bytes after the header */
    RxHeapFreeEntry *freeEntry;     /* 0x0C: NULL while the block is in use */
    u32 pad[4];
} RxHeapBlockHeader;

/* The heap's table of free blocks. */
struct RxHeapFreeEntry {
    u32 size;               /* 0x00 */
    RxHeapBlockHeader *ptr; /* 0x04 */
};

/* One allocation the heap carves its blocks from. */
typedef struct RxHeapSuperBlock {
    void *start;                   /* 0x00: 128-byte aligned, after this descriptor */
    u32 size;                      /* 0x04 */
    struct RxHeapSuperBlock *next; /* 0x08 */
} RxHeapSuperBlock;

typedef struct RxHeap {
    u32 superBlockSize;          /* 0x00 */
    RxHeapSuperBlock *head;      /* 0x04 */
    RxHeapBlockHeader *headBlock; /* 0x08 */
    RxHeapFreeEntry *freeBlocks; /* 0x0C */
    u32 entriesAlloced;          /* 0x10 */
    u32 entriesUsed;             /* 0x14 */
    s32 dirty;                   /* 0x18 */
} RxHeap;

/* ---- the immediate-mode render state (RxRenderStateVector, 0x2C bytes) ---- */
typedef struct RxRenderStateVector {
    u32 flags;          /* 0x00: 1 perspective texturing, 2 z test, 4 z write */
    s32 shadeMode;      /* 0x04 */
    s32 srcBlend;       /* 0x08 */
    s32 destBlend;      /* 0x0C */
    RwRaster *textureRaster; /* 0x10 */
    s32 addressModeU;   /* 0x14 */
    s32 addressModeV;   /* 0x18 */
    s32 filterMode;     /* 0x1C */
    RwRGBA borderColor; /* 0x20 */
    s32 fogType;        /* 0x24 */
    RwRGBA fogColor;    /* 0x28 */
} RxRenderStateVector;

/* ---- the GameCube material instance data (RwCpp68/69): a resource made by fn_804A5F48 ---- */
typedef struct GcnMatTexGen {
    u32 flags;  /* 0x00: 4 = the coordinates are scaled by hand */
    s32 type;   /* 0x04: GXTexGenType; 2..9 are the bump types */
    s32 src;    /* 0x08: GXTexGenSrc; 2 and 3 are binormal and tangent, 4..11 textures */
    u32 unk0C;
    u32 unk10;
    u16 unk14;
    u16 unk16;
} GcnMatTexGen;

typedef struct GcnMatTevStage {
    u32 unk00[11];
    u8 unk2C;
    u32 unk30[4];
    u8 unk40;
    u32 unk44[7];  /* [4] (0x54) gets 0x100 when [6] (0x5C) has 0x80 in the stream */
    u32 indirect;  /* 0x60 */
} GcnMatTevStage;

typedef struct GcnMatElementC {
    u32 unk00[3];
    f32 matrix[12]; /* 0x0C */
} GcnMatElementC;

typedef struct GcnMatElementE {
    u32 unk00[3];
    f32 values[6]; /* 0x0C */
    s32 unk24;
} GcnMatElementE;

typedef struct GcnMatIndStage {
    u32 unk00[5];
} GcnMatIndStage;

/* What fn_804A60D0 hands out: the part behind the 0x30-byte resource header. */
typedef struct GcnMatInstanceBody {
    u32 unk00;
    u8 countTexGens;           /* 0x04: as allocated */
    u8 countTevStages;         /* 0x05 */
    u8 countC;                 /* 0x06 */
    u8 countIndStages;         /* 0x07 */
    u8 countE;                 /* 0x08 */
    u8 numTexGens;             /* 0x09: in use */
    u8 numTevStages;           /* 0x0A */
    u8 numC;                   /* 0x0B */
    u8 numIndStages;           /* 0x0C */
    u8 numE;                   /* 0x0D */
    u8 bytes0E[0x20];          /* 0x0E: stored as 32-bit words in the stream */
    u8 bytes2E[0x10];          /* 0x2E */
    u8 pad3E[2];
    GcnMatElementC *arrayC;    /* 0x40 */
    GcnMatTexGen *texGens;     /* 0x44 */
    GcnMatElementE *arrayE;    /* 0x48 */
    GcnMatIndStage *indStages; /* 0x4C */
    GcnMatTevStage *tevStages; /* 0x50 */
    u32 unk54;                 /* 0x54: high nibble of the stream's fourth byte */
    u32 unk58;                 /* 0x58: its low nibble */
} GcnMatInstanceBody;

typedef struct GcnMatInstance {
    u8 header[0x30];         /* 0x00: fn_804A932C (resource type 6) */
    GcnMatInstanceBody body; /* 0x30 */
} GcnMatInstance;

u8 *fn_804A60D0(u8 *object);

/* Two helpers of the plugin's source file that the compiler expanded in most of its functions.
 * The file is cut over RwCpp64..RwCpp66, so they are shared from here. */
static inline MatFXEffectData *MatFXFindEffect(MatFXMaterialData *data, s32 type) {
    u8 i;
    for (i = 0; i < 2; i++) {
        if (data->data[i].flag == type) return &data->data[i];
    }
    return NULL;
}

static inline MatFXMaterialData *MatFXMaterialDataCreate(RpMaterial *material) {
    MatFXMaterialData *data = MATFXMATERIALGETDATA(material);
    if (data == NULL) {
        data = (MatFXMaterialData *)RwEngineInstance->freeListAlloc(
            lbl_8056ED00.materialDataFreeList, 0x30120);
        if (data == NULL) return NULL;
        memset(data, 0, sizeof(MatFXMaterialData));
        MATFXMATERIALGETDATA(material) = data;
    }
    return data;
}

#ifdef __cplusplus
}
#endif

#endif

/* RenderWare structures inferred by agent r3 for RwCpp20-24 and RwCpp26-28 (wave 40).
 * Private to those units until a later pass unifies the RenderWare types. Layouts come from the
 * retail code's offsets; names follow the public RenderWare 3.7 API where the object is public. */
#ifndef GAME_RW_R3_H
#define GAME_RW_R3_H

#include <dolphin/types.h>

/* The units are C (RenderWare is a C library): every structure has a typedef. */
typedef struct RpAtomic RpAtomic;
typedef struct RpBuildMesh RpBuildMesh;
typedef struct RpBuildMeshTriangle RpBuildMeshTriangle;
typedef struct RpGeometry RpGeometry;
typedef struct RpLight RpLight;
typedef struct RpMaterial RpMaterial;
typedef struct RpMaterialList RpMaterialList;
typedef struct RpMesh RpMesh;
typedef struct RpMeshHeader RpMeshHeader;
typedef struct RpMorphTarget RpMorphTarget;
typedef struct RpTriangle RpTriangle;
typedef struct RwError RwError;
typedef struct RwFreeList RwFreeList;
typedef struct RwGlobals RwGlobals;
typedef struct RwLLLink RwLLLink;
typedef struct RwLinkList RwLinkList;
typedef struct RwModuleInfo RwModuleInfo;
typedef struct RwObject RwObject;
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwRGBA RwRGBA;
typedef struct RwRGBAReal RwRGBAReal;
typedef struct RwRaster RwRaster;
struct RwRaster {
    RwRaster *parent; /* 0x00 */
};
typedef struct RwResEntry RwResEntry;
typedef struct RwBBox RwBBox;
typedef struct RpWorld RpWorld;
typedef struct RpSector RpSector;
typedef struct RpWorldSector RpWorldSector;
typedef struct RwSphere RwSphere;
typedef struct RwStream RwStream;
typedef struct RwSurfaceProperties RwSurfaceProperties;
typedef struct RwTexCoords RwTexCoords;
typedef struct RwTexture RwTexture;
typedef struct RwV3d RwV3d;
typedef struct RxGCVertexDesc RxGCVertexDesc;
typedef struct RxGCNodeCallbacks RxGCNodeCallbacks;
typedef struct RxPipelineNode RxPipelineNode;
typedef struct RxPipeline RxPipeline;
typedef struct rpMeshGlobals rpMeshGlobals;


/* Per-plugin module record: where the plugin's globals sit in the engine block, and how many
 * engine instances opened it. */
struct RwModuleInfo {
    s32 globalsOffset;
    s32 numInstances;
};

struct RwError {
    s32 pluginID;
    s32 errorCode;
};

/* The engine block behind RwEngineInstance (lbl_805F15B4); only the memory functions are typed. */

struct RwGlobals {
    void *curCamera;                                      /* 0x000 */
    RpWorld *curWorld;                                    /* 0x004 */
    u16 renderFrame;                                      /* 0x008 */
    u16 lightFrame;                                       /* 0x00A */
    u8 unkC[0xFC];
    void *(*rwmalloc)(u32 size, u32 hint);                /* 0x108 */
    void (*rwfree)(void *block);                          /* 0x10C */
    void *(*rwrealloc)(void *block, u32 size, u32 hint);  /* 0x110 */
    void *(*rwcalloc)(u32 count, u32 size, u32 hint);     /* 0x114 */
    void *(*freeListAlloc)(RwFreeList *list, u32 hint);   /* 0x118 */
    RwFreeList *(*freeListFree)(RwFreeList *list, void *block); /* 0x11C */
};

/* A plugin registry (toolkit list); its first word is the size of the extended object. */
struct RwPluginRegistry {
    s32 sizeOfStruct;
    s32 origSizeOfStruct;
    s32 maxSizeOfStruct;
    s32 staticAlloc;
    void *firstRegEntry;
    void *lastRegEntry;
};

struct RwObject {
    u8 type;
    u8 subType;
    u8 flags;
    u8 privateFlags;
    void *parent;
};

struct RwRGBA {
    u8 red, green, blue, alpha;
};

struct RwV3d {
    f32 x, y, z;
};

struct RwSphere {
    RwV3d center;
    f32 radius;
};

struct RwTexCoords {
    f32 u, v;
};

struct RwRGBAReal {
    f32 red, green, blue, alpha;
};

struct RwSurfaceProperties {
    f32 ambient, specular, diffuse;
};

struct RwLLLink {
    RwLLLink *next;
    RwLLLink *prev;
};

struct RwLinkList {
    RwLLLink link;
};

/* A free list (0x24 bytes); the module files keep a static one for their objects. */
struct RwFreeList {
    u32 entrySize;       /* 0x00 */
    u32 entriesPerBlock; /* 0x04 */
    u32 heapSize;        /* 0x08 */
    u32 alignment;       /* 0x0C */
    RwLinkList blockList; /* 0x10 */
    u32 flags;           /* 0x18 */
    RwLLLink link;       /* 0x1C */
};

/* Only the fields these units read. */
struct RwTexture {
    RwRaster *raster; /* 0x00 */
    u8 unk4[0x50];
    s32 refCount;     /* 0x54 */
};

struct RxPipeline {
    u8 unk0[0x2C];
    u32 pluginId;   /* 0x2C */
    u32 pluginData; /* 0x30 */
};

struct RpMaterial {
    RwTexture *texture;               /* 0x00 */
    RwRGBA color;                     /* 0x04 */
    RxPipeline *pipeline;             /* 0x08 */
    RwSurfaceProperties surfaceProps; /* 0x0C */
    s16 refCount;                     /* 0x18 */
    s16 pad;                          /* 0x1A */
};

struct RwObjectHasFrame {
    RwObject object;                             /* 0x00 */
    RwLLLink lFrame;                             /* 0x08 */
    RwObjectHasFrame *(*sync)(RwObjectHasFrame *); /* 0x10 */
};

struct RpLight {
    RwObjectHasFrame object;        /* 0x00 */
    f32 radius;                     /* 0x14 */
    RwRGBAReal color;               /* 0x18 */
    f32 minusCosAngle;              /* 0x28 */
    RwLinkList WorldSectorsInLight; /* 0x2C */
    RwLLLink inWorld;               /* 0x34 */
    u16 lightFrame;                 /* 0x3C */
    u16 pad;                        /* 0x3E */
};

/* Mesh module globals in the engine block (plugin 0x502, 0x30 bytes). */
struct rpMeshGlobals {
    s16 nextSerialNum;           /* 0x00 */
    u8 pad2[6];
    u8 meshFlagsToPrimType[31];  /* 0x08 */
    u8 primTypeToMeshFlags[7];   /* 0x27 */
    u8 pad2E[2];
};

struct RpBuildMeshTriangle {
    u16 vertIndex[3];     /* 0x00 */
    RpMaterial *material; /* 0x08 */
    u16 matIndex;         /* 0x0C */
    u16 textureIndex;     /* 0x0E */
    u16 rasterIndex;      /* 0x10 */
    u16 pipelineIndex;    /* 0x12 */
};

struct RpBuildMesh {
    u32 triangleBufferSize;             /* 0x00 */
    u32 numTriangles;                   /* 0x04 */
    RpBuildMeshTriangle *meshTriangles; /* 0x08 */
};

struct RpMesh {
    u16 *indices;         /* 0x00 */
    u32 numIndices;       /* 0x04 */
    RpMaterial *material; /* 0x08 */
};

/* Followed by the RpMesh records (at firstMeshOffset) and, when not native, the indices. */
struct RpMeshHeader {
    u32 flags;              /* 0x00 */
    u16 numMeshes;          /* 0x04 */
    u16 serialNum;          /* 0x06 */
    u32 totalIndicesInMesh; /* 0x08 */
    u32 firstMeshOffset;    /* 0x0C */
};

struct RpMaterialList {
    RpMaterial **materials;
    s32 numMaterials;
    s32 space;
};

struct RpTriangle {
    u16 vertIndex[3];
    u16 matIndex;
};

struct RpMorphTarget {
    RpGeometry *parentGeom;  /* 0x00 */
    RwSphere boundingSphere; /* 0x04 */
    RwV3d *verts;            /* 0x14 */
    RwV3d *normals;          /* 0x18 */
};

enum {
    rpGEOMETRYLOCKVERTICES = 0x02,
    rpGEOMETRYLOCKNORMALS = 0x04,
    rpGEOMETRYLOCKPRELIGHT = 0x08,
    rpGEOMETRYLOCKTEXCOORDS1 = 0x10
};

enum {
    rpGEOMETRYTRISTRIP = 0x01,
    rpGEOMETRYPOSITIONS = 0x02,
    rpGEOMETRYTEXTURED = 0x04,
    rpGEOMETRYPRELIT = 0x08,
    rpGEOMETRYNORMALS = 0x10,
    rpGEOMETRYTEXTURED2 = 0x80,
    rpGEOMETRYNATIVE = 0x01000000,
    rpGEOMETRYNATIVEINSTANCE = 0x02000000,
    rpGEOMETRYLOCKALL = 0xFFF
};

struct RpGeometry {
    RwObject object;            /* 0x00 */
    u32 flags;                  /* 0x08 */
    u16 lockedSinceLastInst;    /* 0x0C */
    s16 refCount;               /* 0x0E */
    s32 numTriangles;           /* 0x10 */
    s32 numVertices;            /* 0x14 */
    s32 numMorphTargets;        /* 0x18 */
    s32 numTexCoordSets;        /* 0x1C */
    RpMaterialList matList;     /* 0x20 */
    RpTriangle *triangles;      /* 0x2C */
    RwRGBA *preLitLum;          /* 0x30 */
    RwTexCoords *texCoords[8];  /* 0x34 */
    RpMeshHeader *mesh;         /* 0x54 */
    RwResEntry *repEntry;       /* 0x58 */
    RpMorphTarget *morphTarget; /* 0x5C */
};

/* A resource-arena entry (instanced data); the payload follows the header. */
struct RwResEntry {
    RwLLLink link;                          /* 0x00 */
    s32 size;                               /* 0x08 */
    void *owner;                            /* 0x0C */
    RwResEntry **ownerRef;                  /* 0x10 */
    void (*destroyNotify)(RwResEntry *);    /* 0x14 */
};

struct RwBBox {
    RwV3d sup;
    RwV3d inf;
};

/* Only the leading fields of a world (its flags share the geometry's offset). */
struct RpWorld {
    RwObject object;                /* 0x00 */
    u32 flags;                      /* 0x08 */
    u32 renderOrder;                /* 0x0C */
    RpMaterialList matList;         /* 0x10 */
    RpSector *rootSector;           /* 0x1C */
    u8 unk20[0x14];                 /* 0x20: texture coordinate sets, clumps */
    RwLinkList lightList;           /* 0x34: lights with a position */
    RwLinkList directionalLightList; /* 0x3C: ambient and directional lights */
    RwV3d worldOrigin;              /* 0x44 */
    RwBBox boundingBox;             /* 0x50 */
};

/* A node of a world's sector tree: a world sector (type < 0) or a splitting plane whose type is
 * the byte offset of its axis in a vector (0, 4 or 8). */
struct RpSector {
    s32 type;
};

typedef struct RpPlaneSector {
    s32 type;              /* 0x00 */
    f32 value;             /* 0x04 */
    RpSector *leftSubTree;  /* 0x08 */
    RpSector *rightSubTree; /* 0x0C */
    f32 leftValue;         /* 0x10 */
    f32 rightValue;        /* 0x14 */
} RpPlaneSector;

struct RpWorldSector {
    s32 type;                            /* 0x00 */
    void *polygons;                      /* 0x04 */
    RwV3d *vertices;                     /* 0x08 */
    void *normals;                       /* 0x0C */
    RwTexCoords *texCoords[8];           /* 0x10 */
    RwRGBA *preLitLum;                   /* 0x30 */
    RwResEntry *repEntry;                /* 0x34 */
    RwLinkList collAtomicsInWorldSector; /* 0x38 */
    RwLinkList lightsInWorldSector;      /* 0x40 */
    RwBBox boundingBox;                  /* 0x48 */
    RwBBox tightBoundingBox;             /* 0x60 */
    RpMeshHeader *mesh;                  /* 0x78 */
    RxPipeline *pipeline;                /* 0x7C */
    u16 matListWindowBase;               /* 0x80 */
    u16 numVertices;                     /* 0x82 */
    u16 numPolygons;                     /* 0x84 */
    u16 pad;                             /* 0x86 */
};

typedef struct RwFrame RwFrame;

typedef struct RwMatrix {
    RwV3d right; /* 0x00 */
    u32 flags;   /* 0x0C */
    RwV3d up;    /* 0x10 */
    u32 pad1;    /* 0x1C */
    RwV3d at;    /* 0x20 */
    u32 pad2;    /* 0x2C */
    RwV3d pos;   /* 0x30 */
    u32 pad3;    /* 0x3C */
} RwMatrix;

enum {
    rpLIGHTDIRECTIONAL = 1,
    rpLIGHTAMBIENT = 2,
    rpLIGHTPOINT = 0x80,
    rpLIGHTSPOT = 0x81,
    rpLIGHTSPOTSOFT = 0x82
};

/* The GameCube light extension (plugin 0x505 of lights, 28 bytes): when set, the light uses its
 * own GX attenuation coefficients instead of the radius-based distance attenuation. */
typedef struct RpGCLightExt {
    u32 useAttn;       /* 0x00 */
    f32 a0, a1, a2;    /* 0x04 */
    f32 k0, k1, k2;    /* 0x10 */
} RpGCLightExt;

/* GameCube instance data (RwCpp27): the payload of a native geometry's or world sector's resource
 * entry. The header lists the vertex arrays, then one record per mesh; the arrays and display
 * lists follow, 32-byte aligned. In a stream their pointers are offsets from that data base. */
typedef struct RxGCInstanceArray {
    void *data; /* 0x00 */
    u8 unk4;    /* 0x04 */
    u8 stride;   /* 0x05: bytes per vertex */
    u8 attrType; /* 0x06: GX attribute type (direct or indexed) */
    u8 pad7;     /* 0x07 */
} RxGCInstanceArray;

typedef struct RxGCInstanceMesh {
    void *displayList; /* 0x00 */
    u32 size;          /* 0x04 */
} RxGCInstanceMesh;

typedef struct RxGCInstanceHeader {
    u16 serialNum;                /* 0x00 */
    u16 meshSerialNum;            /* 0x02: serial number of the mesh it was built from */
    u32 unk4;                     /* 0x04 */
    u32 numArrays;                /* 0x08 */
    RxGCInstanceArray arrays[1];  /* 0x0C, numArrays of them, then the meshes */
} RxGCInstanceHeader;

typedef struct RpInterpolator {
    s32 flags;            /* 0x00 */
    s16 startMorphTarget; /* 0x04 */
    s16 endMorphTarget;   /* 0x06 */
    f32 time;             /* 0x08 */
    f32 recipTime;        /* 0x0C */
    f32 position;         /* 0x10 */
} RpInterpolator;

enum {
    rpINTERPOLATORDIRTYINSTANCE = 0x01
};

/* Only the fields these units use. */
struct RpAtomic {
    RwObjectHasFrame object; /* 0x00 */
    RwResEntry *repEntry;    /* 0x14 */
    RpGeometry *geometry;    /* 0x18 */
    RwSphere boundingSphere; /* 0x1C */
    RwSphere worldBoundingSphere; /* 0x2C */
    void *clump;             /* 0x3C */
    RwLLLink inClumpLink;    /* 0x40 */
    RpAtomic *(*renderCallBack)(RpAtomic *atomic); /* 0x48 */
    RpInterpolator interpolator; /* 0x4C */
    u16 renderFrame;         /* 0x60 */
    u16 pad;                 /* 0x62 */
    RwLinkList llWorldSectorsInAtomic; /* 0x64 */
    RxPipeline *pipeline;    /* 0x6C */
};

/* GX enumeration values these units use (dolphin/gx.h lists none of them); the names are the
 * SDK's with an R3_ prefix so they cannot clash with a later gx.h. */
enum {
    R3_GX_VTXFMT0 = 0,
    R3_GX_VA_POS = 9,
    R3_GX_VA_NRM = 10,
    R3_GX_VA_CLR0 = 11,
    R3_GX_VA_TEX0 = 13,
    R3_GX_VA_NBT = 25,
    R3_GX_POS_XYZ = 1,
    R3_GX_NRM_XYZ = 0,
    R3_GX_NRM_NBT = 1,
    R3_GX_TEX_ST = 1
};

/* GameCube world pipelines (RwCpp26). */

/* Vertex descriptor of the GameCube instancing code (plugin 0x511 of geometries and world
 * sectors): one attribute type per slot, one component size per slot, and a reference count. */
struct RxGCVertexDesc {
    u8 type[12];  /* 0x00 */
    u8 size[10];  /* 0x0C */
    u16 refCount; /* 0x16 */
};

/* The four callbacks a GameCube instance node keeps in its private data. */
typedef struct RxGCInstanceData RxGCInstanceData;

/* Each callback gets the object (atomic or world sector) and returns it, or NULL on failure. */
typedef void *(*RxGCNodeCallBack)(void *object, RxGCInstanceData *data);

struct RxGCNodeCallbacks {
    RxGCNodeCallBack instance;   /* 0x00 */
    RxGCNodeCallBack reinstance; /* 0x04 */
    RxGCNodeCallBack lighting;   /* 0x08 */
    RxGCNodeCallBack render;     /* 0x0C */
};

/* What a GameCube instance node hands its callbacks: the object's instance data, mesh and flags,
 * the lights collected for it, and the node's callbacks. */
struct RxGCInstanceData {
    RwResEntry *repEntry;          /* 0x00 */
    RpMeshHeader *mesh;            /* 0x04 */
    u32 flags;                     /* 0x08 */
    RwRGBAReal ambient;            /* 0x0C */
    s32 ambientSet;                /* 0x1C */
    u32 lightMask;                 /* 0x20 */
    s32 numLights;                 /* 0x24 */
    RxGCNodeCallbacks *callbacks;  /* 0x28 */
};

/* What a pipeline node body gets: the object being rendered. */
typedef struct RxPipelineNodeParam {
    void *dataParam;
    void *heap;
} RxPipelineNodeParam;

/* Links a light to a world sector it lights. */
typedef struct RpLightTie {
    RwLLLink lightInWorldSector; /* 0x00 */
    RpLight *light;              /* 0x08 */
    RwLLLink WorldSectorInLight; /* 0x0C */
    RpWorldSector *sect;         /* 0x14 */
} RpLightTie;

enum {
    rpLIGHTLIGHTATOMICS = 0x01,
    rpLIGHTLIGHTWORLD = 0x02
};

enum {
    rpWORLDLIGHT = 0x20,
    rpGEOMETRYLIGHT = 0x20
};

/* Links an atomic to a world sector it lies in. */
typedef struct RpTie {
    RwLLLink lAtomicInWorldSector; /* 0x00 */
    RpAtomic *apAtom;              /* 0x08 */
    RwLLLink lWorldSectorInAtomic; /* 0x0C */
    RpWorldSector *worldSector;    /* 0x14 */
} RpTie;

/* A pipeline node definition (0x40 bytes): its name, methods, I/O specification and the size of
 * a pipeline node's private data. */
typedef struct RxNodeMethods {
    void *nodeBody;           /* 0x00 */
    void *nodeInit;           /* 0x04 */
    void *nodeTerm;           /* 0x08 */
    void *pipelineNodeInit;   /* 0x0C */
    void *pipelineNodeTerm;   /* 0x10 */
    void *pipelineNodeConfig; /* 0x14 */
    void *configMsgHandler;   /* 0x18 */
} RxNodeMethods;

typedef struct RxIoSpec {
    u32 numClustersOfInterest; /* 0x00 */
    void *clustersOfInterest;  /* 0x04 */
    void *inputRequirements;   /* 0x08 */
    u32 numOutputs;            /* 0x0C */
    void *outputs;             /* 0x10 */
} RxIoSpec;

typedef struct RxNodeDefinition {
    char *name;                      /* 0x00 */
    RxNodeMethods nodeMethods;       /* 0x04 */
    RxIoSpec io;                     /* 0x20 */
    u32 pipelineNodePrivateDataSize; /* 0x34 */
    s32 editable;                    /* 0x38 */
    s32 InputPipesCnt;               /* 0x3C */
} RxNodeDefinition;

struct RxPipelineNode {
    u8 unk0[0x14];
    RxGCNodeCallbacks *privateData; /* 0x14 */
};

#ifdef __cplusplus
extern "C" {
#endif
extern RwGlobals *lbl_805F15B4; /* RwEngineInstance */
u32 fn_8047DC28(u32 code, ...); /* _rwerror: builds the error code/message */
RwError *fn_8047DB84(RwError *error); /* RwErrorSet */
#ifdef __cplusplus
}
#endif

#define RWSRCGLOBAL(member) (lbl_805F15B4->member)
#define RwMalloc(size, hint) (RWSRCGLOBAL(rwmalloc)((size), (hint)))
#define RwFree(block) (RWSRCGLOBAL(rwfree)((block)))
#define RwRealloc(block, size, hint) (RWSRCGLOBAL(rwrealloc)((block), (size), (hint)))
#define RwFreeListAlloc(list, hint) (RWSRCGLOBAL(freeListAlloc)((list), (hint)))
#define RwFreeListFree(list, block) (RWSRCGLOBAL(freeListFree)((list), (block)))
/* A plugin's globals: a typed view of the engine block at the module's offset. */
#define RWPLUGINGLOBAL(type, module) (*(type *)((u8 *)lbl_805F15B4 + (module).globalsOffset))

/* Error codes seen in these units. */
#define E_RW_BADVERSION 0x80000004
#define E_RW_NOMEM 0x80000013
#define E_RP_GEOMETRY_TOOMANYVERTS 6

#define RWERROR(args)                    \
    do {                                 \
        RwError _rwErr;                  \
        _rwErr.pluginID = 2;             \
        _rwErr.errorCode = fn_8047DC28 args; \
        fn_8047DB84(&_rwErr);            \
    } while (0)

#endif

/* RenderWare 3.7 structures inferred by agent r6 for the RenderWare core files of range r6
 * (src/RenderWare/RwStreamIo.c .. RwPaletteQuantize.c, 0x80480C6C-0x8049043C; before wave 41 the
 * size-cut src/Game/RwCpp45..53). Those files are C, the language RenderWare is written in.
 * Private to those files: a later pass unifies the RenderWare types of all agents. Layouts come
 * from the retail code's offsets; names follow the public RenderWare API where it documents them. */
#ifndef GAME_RW_R6_H
#define GAME_RW_R6_H

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The files that include this are C: every tag gets its typedef. */
typedef struct RwError RwError;
typedef struct RwLLLink RwLLLink;
typedef struct RwLinkList RwLinkList;
typedef struct RwModuleInfo RwModuleInfo;
typedef struct RwV3d RwV3d;
typedef struct RwSqrtTables RwSqrtTables;
typedef union RwSplitBits RwSplitBits;
typedef struct RwV2d RwV2d;
typedef struct RwMatrix RwMatrix;
typedef struct RwBBox RwBBox;
typedef struct RwHeapBlock RwHeapBlock;
typedef struct RwHeap RwHeap;
typedef struct RwStringFunctions RwStringFunctions;
typedef struct RwRaster RwRaster;
typedef struct RwTexture RwTexture;
typedef struct RwTexDictionary RwTexDictionary;
typedef struct rwStreamTexDictionary rwStreamTexDictionary;
typedef struct RwObject RwObject;
typedef struct RwFrame RwFrame;
typedef struct rwFrameList rwFrameList;
typedef struct rwStreamFrame rwStreamFrame;
typedef struct RwPlane RwPlane;
typedef struct RwFrustumPlane RwFrustumPlane;
typedef struct RwSphere RwSphere;
typedef struct RwObjectHasFrame RwObjectHasFrame;
typedef struct RwDevice RwDevice;
typedef struct RwCamera RwCamera;
typedef struct rwStreamCamera rwStreamCamera;
typedef struct RwFreeList RwFreeList;
typedef struct RwMemoryFunctions RwMemoryFunctions;
typedef struct RwGlobals RwGlobals;
typedef struct RwResEntry RwResEntry;
typedef struct RwResourcesGlobals RwResourcesGlobals;
typedef struct RwMemory RwMemory;
typedef struct RwStreamMemory RwStreamMemory;
typedef struct RwStreamCustom RwStreamCustom;
typedef union RwStreamUnion RwStreamUnion;
typedef struct RwStream RwStream;
typedef struct RwPluginRegistry RwPluginRegistry;
typedef struct RwPluginRegistryEntry RwPluginRegistryEntry;
typedef struct RwRGBA RwRGBA;
typedef struct RwImage RwImage;
typedef struct rwImageFormat rwImageFormat;
typedef struct rwImageGlobals rwImageGlobals;

typedef s32 RwBool;

/* The C library, declared here because the stand-in MSL headers are C++ only. */
void *memcpy(void *dst, const void *src, unsigned long size);
void *memset(void *dst, int value, unsigned long size);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
char *strncat(char *dst, const char *src, unsigned long size);
int strcmp(const char *a, const char *b);
unsigned long strlen(const char *string);
char *strtok(char *string, const char *delimiters);
int sprintf(char *buffer, const char *format, ...);

struct RwError {
    s32 pluginID;
    s32 errorCode;
};

struct RwLLLink {
    RwLLLink *next;
    RwLLLink *prev;
};

struct RwLinkList {
    RwLLLink link;
};

struct RwModuleInfo {
    s32 globalsOffset;
    s32 numInstances;
};

struct RwV3d {
    f32 x, y, z;
};

/* Vector module globals (0x8048222C..): mantissa tables of the fast sqrt and inverse sqrt. */
struct RwSqrtTables {
    u32 *sqrtTab;
    u32 *invSqrtTab;
};

union RwSplitBits {
    f32 nReal;
    u32 nUInt;
};

struct RwV2d {
    f32 x, y;
};

struct RwMatrix {
    RwV3d right; /* 0x00 */
    u32 flags;   /* 0x0C */
    RwV3d up;    /* 0x10 */
    u32 pad1;
    RwV3d at;    /* 0x20 */
    u32 pad2;
    RwV3d pos;   /* 0x30 */
    u32 pad3;
};

struct RwBBox {
    RwV3d sup;
    RwV3d inf;
};

/* Arena heap of the resource module (0x80482984..): blocks carry a 32-byte header. */
struct RwHeap {
    RwHeapBlock *firstBlock; /* 0x00 */
    RwHeapBlock *firstFree;  /* 0x04 */
};

struct RwHeapBlock {
    RwHeap *heap;      /* 0x00 */
    RwHeapBlock *next; /* 0x04 */
    RwHeapBlock *prev; /* 0x08 */
    u32 size;          /* 0x0C */
    u32 used;          /* 0x10 */
    u32 pad[3];
};

/* The string function table of the engine globals, 0xC4..0x107. */
struct RwStringFunctions {
    void *vecSprintf;  /* 0xC4 */
    void *vecVsprintf; /* 0xC8 */
    char *(*vecStrcpy)(char *dst, const char *src); /* 0xCC */
    void *vecStrncpy;  /* 0xD0 */
    void *vecStrcat;   /* 0xD4 */
    void *vecStrncat;  /* 0xD8 */
    void *vecStrrchr;  /* 0xDC */
    void *vecStrchr;   /* 0xE0 */
    void *vecStrstr;   /* 0xE4 */
    void *vecStrcmp;   /* 0xE8 */
    void *vecStrncmp;  /* 0xEC */
    void *vecStricmp;  /* 0xF0 */
    u32 (*vecStrlen)(const char *string); /* 0xF4 */
    void *vecStrupr;   /* 0xF8 */
    void *vecStrlwr;   /* 0xFC */
    void *vecStrtok;   /* 0x100 */
    void *vecSscanf;   /* 0x104 */
};

struct RwRaster {
    RwRaster *parent;    /* 0x00 */
    u8 *cpPixels;        /* 0x04 */
    u8 *palette;         /* 0x08 */
    s32 width;           /* 0x0C */
    s32 height;          /* 0x10 */
    s32 depth;           /* 0x14 */
    s32 stride;          /* 0x18 */
    s16 nOffsetX;        /* 0x1C */
    s16 nOffsetY;        /* 0x1E */
    u8 cType;            /* 0x20 */
    u8 cFlags;           /* 0x21 */
    u8 privateFlags;     /* 0x22 */
    u8 cFormat;          /* 0x23 */
    u8 *originalPixels;  /* 0x24 */
    s32 originalWidth;   /* 0x28 */
    s32 originalHeight;  /* 0x2C */
    s32 originalStride;  /* 0x30 */
};

/* The raster module's globals (registered size 0x64). */
typedef struct rwRasterGlobals {
    RwRaster *rasterStack[10];  /* 0x00 */
    s32 rasterSP;               /* 0x28 */
    RwRaster dummyRaster;       /* 0x2C */
    RwFreeList *rasterFreeList; /* 0x60 */
} rwRasterGlobals;

typedef struct RwRect {
    s32 x, y, w, h;
} RwRect;

typedef struct RwRGBAReal {
    f32 red, green, blue, alpha;
} RwRGBAReal;

struct RwTexture {
    RwRaster *raster;       /* 0x00 */
    void *dict;             /* 0x04 */
    RwLLLink lInDictionary; /* 0x08 */
    char name[32];          /* 0x10 */
    char mask[32];          /* 0x30 */
    u32 filterAddressing;   /* 0x50 */
    s32 refCount;           /* 0x54 */
};

/* The struct chunk of a texture dictionary, as read after RwMemNative32 on big-endian. */
struct rwStreamTexDictionary {
    u16 deviceId;
    u16 numTextures;
};

struct RwObject {
    u8 type;         /* 0x00 */
    u8 subType;      /* 0x01 */
    u8 flags;        /* 0x02 */
    u8 privateFlags; /* 0x03 */
    void *parent;    /* 0x04 */
};

struct RwTexDictionary {
    RwObject object;           /* 0x00 */
    RwLinkList texturesInDict; /* 0x08 */
    RwLLLink lInInstance;      /* 0x10 */
};

/* The texture module's globals (registered size 0x34). */
typedef struct rwTextureGlobals {
    RwLinkList texDictList;                                        /* 0x00 */
    RwFreeList *textureFreeList;                                   /* 0x08 */
    RwFreeList *texDictFreeList;                                   /* 0x0C */
    RwTexDictionary *currentTexDict;                               /* 0x10 */
    RwTexture *(*textureReadCallBack)(const char *name, const char *maskName); /* 0x14 */
    RwTexture *(*textureFindCallBack)(const char *name);           /* 0x18 */
    RwBool haveTexMipmaps;                                         /* 0x1C */
    RwBool generateTexMipmaps;                                     /* 0x20 */
    void *scratchMem;                                              /* 0x24 */
    u16 scratchMemSize;                                            /* 0x28 */
    RwRaster *(*mipmapGenerationCallBack)(RwRaster *raster, RwImage *image); /* 0x2C */
    RwBool (*mipmapNameCallBack)(char *name, char *maskName, u8 mipLevel, s32 format); /* 0x30 */
} rwTextureGlobals;

/* The palette quantiser: an octree over the colours of the images added to it (four bits of the
 * colour address per level, one from each channel), then Wu's box cutting over the tree. Callers
 * keep the quantiser on the stack; its nodes come from a free list of 0x5C-byte entries. */
typedef struct rwPalQuantLeaf {
    f32 weight;  /* 0x00 */
    f32 r;       /* 0x04: red, scaled by its weight 0.509 */
    f32 g;       /* 0x08 */
    f32 b;       /* 0x0C: blue, scaled by its weight 0.194 */
    f32 a;       /* 0x10 */
    f32 var;     /* 0x14 */
    u8 index;    /* 0x18 */
} rwPalQuantLeaf;

typedef struct rwPalQuantNode {
    rwPalQuantLeaf leaf;                /* 0x00 */
    struct rwPalQuantNode *child[16];   /* 0x1C */
} rwPalQuantNode;

typedef struct rwPalQuantBox {
    s32 col0[4]; /* 0x00: first corner (inclusive) */
    s32 col1[4]; /* 0x10: second corner (exclusive) */
} rwPalQuantBox;

typedef struct rwPalQuant {
    rwPalQuantBox Mbox[256];   /* 0x0000 */
    f32 Mvv[256];              /* 0x2000 */
    rwPalQuantLeaf Mvol[256];  /* 0x2400 */
    rwPalQuantNode *root;      /* 0x4000 */
    RwFreeList *cubeFreeList;  /* 0x4004 */
} rwPalQuant;

struct RwFrame {
    RwObject object;      /* 0x00 */
    RwLLLink inDirtyList; /* 0x08 */
    RwMatrix modelling;   /* 0x10 */
    RwMatrix ltm;         /* 0x50 */
    RwLinkList objectList; /* 0x90 */
    RwFrame *child;       /* 0x98 */
    RwFrame *next;        /* 0x9C */
    RwFrame *root;        /* 0xA0 */
};

/* A frame hierarchy as the clump stream code holds it. */
struct rwFrameList {
    RwFrame **frames;
    s32 numFrames;
};

struct rwStreamFrame {
    RwV3d right, up, at, pos;
    s32 parentIndex;
    u32 data;
};

struct RwPlane {
    RwV3d normal;
    f32 distance;
};

struct RwFrustumPlane {
    RwPlane plane;
    u8 closestX, closestY, closestZ, pad;
};

struct RwSphere {
    RwV3d center;
    f32 radius;
};

struct RwObjectHasFrame {
    RwObject object;                                 /* 0x00 */
    RwLLLink lFrame;                                 /* 0x08 */
    RwObjectHasFrame *(*sync)(RwObjectHasFrame *o);  /* 0x10 */
};

struct RwCamera {
    RwObjectHasFrame object;                 /* 0x00 */
    s32 projectionType;                      /* 0x14 */
    RwCamera *(*beginUpdate)(RwCamera *c);   /* 0x18 */
    RwCamera *(*endUpdate)(RwCamera *c);     /* 0x1C */
    RwMatrix viewMatrix;                     /* 0x20 */
    RwRaster *frameBuffer;                   /* 0x60 */
    RwRaster *zBuffer;                       /* 0x64 */
    RwV2d viewWindow;                        /* 0x68 */
    RwV2d recipViewWindow;                   /* 0x70 */
    RwV2d viewOffset;                        /* 0x78 */
    f32 nearPlane;                           /* 0x80 */
    f32 farPlane;                            /* 0x84 */
    f32 fogPlane;                            /* 0x88 */
    f32 zScale;                              /* 0x8C */
    f32 zShift;                              /* 0x90 */
    RwFrustumPlane frustumPlanes[6];         /* 0x94 */
    RwBBox frustumBoundBox;                  /* 0x10C */
    RwV3d frustumCorners[8];                 /* 0x124 */
};

struct rwStreamCamera {
    RwV2d viewWindow;
    RwV2d viewOffset;
    f32 nearPlane, farPlane;
    f32 fogPlane;
    u32 projection;
};

struct RwMemoryFunctions {
    void *(*rwmalloc)(u32 size, u32 hint);
    void (*rwfree)(void *mem);
    void *(*rwrealloc)(void *mem, u32 size, u32 hint);
    void *(*rwcalloc)(u32 count, u32 size, u32 hint);
};

typedef RwBool (*RwStandardFunc)(void *out, void *inOut, s32 number);

struct RwDevice {
    f32 gammaCorrection; /* 0x00 */
    RwBool (*fpSystem)(s32 request, void *out, void *inOut, s32 number); /* 0x04 */
    f32 zBufferNear;     /* 0x08 */
    f32 zBufferFar;      /* 0x0C */
    void *fpRender[10];  /* 0x10: render state set/get, immediate mode 2D and 3D */
};

/* The engine's global block. Plugin and module globals live behind it at registered offsets. */
struct RwGlobals {
    void *curCamera;                                       /* 0x000 */
    void *curWorld;                                        /* 0x004 */
    u16 renderFrame;                                       /* 0x008 */
    u16 lightFrame;                                        /* 0x00A */
    u16 pad[2];
    RwDevice dOpenDevice;                                  /* 0x010 */
    RwStandardFunc stdFunc[29];                            /* 0x048 */
    RwLinkList dirtyFrameList;                             /* 0x0BC */
    RwStringFunctions stringFuncs;                         /* 0x0C4 */
    RwMemoryFunctions memoryFuncs;                         /* 0x108 */
    void *(*memoryAlloc)(RwFreeList *list, u32 hint);      /* 0x118 */
    RwFreeList *(*memoryFree)(RwFreeList *list, void *p);  /* 0x11C */
    void *metrics;                                         /* 0x120 */
    s32 engineStatus;                                      /* 0x124 */
    u32 resArenaInitSize;                                  /* 0x128 */
};

/* A free list. The modules keep theirs in .bss and hand it to RwFreeListCreate to fill in. */
struct RwFreeList {
    u32 entrySize;        /* 0x00 */
    u32 entriesPerBlock;  /* 0x04 */
    u32 heapSize;         /* 0x08 */
    u32 alignment;        /* 0x0C */
    RwLinkList blockList; /* 0x10 */
    u32 flags;            /* 0x18 */
    RwLLLink link;        /* 0x1C */
};

extern RwGlobals *lbl_805F15B4;
#define RwEngineInstance lbl_805F15B4
#define RWGLOBALAT(type, offset) (*(type *)((u8 *)RwEngineInstance + (offset)))

/* RwErrorSet(_rwerror(code, ...)) as the library's RWERROR macro spells it. */
s32 fn_8047DC28(s32 code, ...);
RwError *fn_8047DB84(RwError *error);
#define RWERROR(args)                                                                              \
    do {                                                                                           \
        RwError err_;                                                                              \
        err_.pluginID = 1;                                                                         \
        err_.errorCode = fn_8047DC28 args;                                                         \
        fn_8047DB84(&err_);                                                                        \
    } while (0)

/* Resource arena (0x80480878..). */
typedef void (*RwResEntryDestroyNotify)(RwResEntry *entry);

struct RwResEntry {
    RwLLLink link;                         /* 0x00 */
    s32 size;                              /* 0x08 */
    void *owner;                           /* 0x0C */
    RwResEntry **ownerRef;                 /* 0x10 */
    RwResEntryDestroyNotify destroyNotify; /* 0x14 */
};

struct RwResourcesGlobals {
    s32 maxSize;             /* 0x00 */
    s32 currentSize;         /* 0x04 */
    s32 reusageSize;         /* 0x08 */
    void *memHeap;           /* 0x0C */
    RwLinkList entriesA;     /* 0x10 */
    RwLinkList entriesB;     /* 0x18 */
    RwLinkList *freeEntries; /* 0x20 */
    RwLinkList *usedEntries; /* 0x24 */
};

/* Streams (0x80480C6C..). */
struct RwMemory {
    u8 *start;
    u32 length;
};

struct RwStreamMemory {
    u32 position;
    u32 nSize;
    u8 *memBlock;
};

struct RwStreamCustom {
    RwBool (*sfnclose)(void *data);
    u32 (*sfnread)(void *data, void *buffer, u32 length);
    RwBool (*sfnwrite)(void *data, const void *buffer, u32 length);
    RwBool (*sfnskip)(void *data, u32 offset);
    void *data;
};

union RwStreamUnion {
    RwStreamMemory memory;
    void *fpFile;
    RwStreamCustom custom;
};

struct RwStream {
    s32 type;           /* 0x00 */
    s32 accessType;     /* 0x04 */
    s32 position;       /* 0x08 */
    RwStreamUnion Type; /* 0x0C */
    RwBool rwOwned;     /* 0x20 */
};

/* Plugin registry (0x804815F0..). */
typedef RwStream *(*RwPluginDataChunkReadCallBack)(RwStream *stream, s32 length, void *object,
                                                   s32 offset, s32 size);
typedef RwStream *(*RwPluginDataChunkWriteCallBack)(RwStream *stream, s32 length,
                                                    const void *object, s32 offset, s32 size);
typedef s32 (*RwPluginDataChunkGetSizeCallBack)(const void *object, s32 offset, s32 size);
typedef RwBool (*RwPluginDataChunkAlwaysCallBack)(void *object, s32 offset, s32 size);
typedef RwBool (*RwPluginDataChunkRightsCallBack)(void *object, s32 offset, s32 size, u32 extra);
typedef void *(*RwPluginObjectConstructor)(void *object, s32 offset, s32 size);
typedef void *(*RwPluginObjectDestructor)(void *object, s32 offset, s32 size);
typedef void *(*RwPluginObjectCopy)(void *dst, const void *src, s32 offset, s32 size);

struct RwPluginRegistryEntry {
    s32 offset;                               /* 0x00 */
    s32 size;                                 /* 0x04 */
    u32 pluginID;                             /* 0x08 */
    RwPluginDataChunkReadCallBack readCB;     /* 0x0C */
    RwPluginDataChunkWriteCallBack writeCB;   /* 0x10 */
    RwPluginDataChunkGetSizeCallBack getSizeCB; /* 0x14 */
    RwPluginDataChunkAlwaysCallBack alwaysCB; /* 0x18 */
    RwPluginDataChunkRightsCallBack rightsCB; /* 0x1C */
    RwPluginObjectConstructor constructCB;    /* 0x20 */
    RwPluginObjectDestructor destructCB;      /* 0x24 */
    RwPluginObjectCopy copyCB;                /* 0x28 */
    void *errStrCB;                           /* 0x2C */
    RwPluginRegistryEntry *nextRegEntry;      /* 0x30 */
    RwPluginRegistryEntry *prevRegEntry;      /* 0x34 */
    RwPluginRegistry *parentRegistry;         /* 0x38 */
};

struct RwPluginRegistry {
    s32 sizeOfStruct;                    /* 0x00 */
    s32 origSizeOfStruct;                /* 0x04 */
    s32 maxSizeOfStruct;                 /* 0x08 */
    s32 staticAlloc;                     /* 0x0C */
    RwPluginRegistryEntry *firstRegEntry; /* 0x10 */
    RwPluginRegistryEntry *lastRegEntry;  /* 0x14 */
};

/* Images (0x80487914..). */
struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
};

struct RwImage {
    s32 flags;       /* 0x00: bit 0 = pixels allocated by the image */
    s32 width;       /* 0x04 */
    s32 height;      /* 0x08 */
    s32 depth;       /* 0x0C */
    s32 stride;      /* 0x10 */
    u8 *cpPixels;    /* 0x14 */
    RwRGBA *palette; /* 0x18 */
};

/* A registered image file format (0x34 bytes, from its own free list). */
struct rwImageFormat {
    char lowerExtension[20];                           /* 0x00 */
    char upperExtension[20];                           /* 0x14 */
    RwImage *(*imageRead)(const char *name);           /* 0x28 */
    RwImage *(*imageWrite)(RwImage *image, const char *name); /* 0x2C */
    rwImageFormat *next;                               /* 0x30 */
};

/* The image module's globals (registered size 0x220). */
struct rwImageGlobals {
    RwFreeList *imageFreeList;       /* 0x000 */
    char *imagePath;                 /* 0x004 */
    s32 imagePathSize;               /* 0x008 */
    u8 gammaTable[256];              /* 0x00C */
    u8 invGammaTable[256];           /* 0x10C */
    f32 gammaVal;                    /* 0x20C */
    char *scratchMem;                /* 0x210 */
    s32 scratchMemSize;              /* 0x214 */
    RwFreeList *imageFormatFreeList; /* 0x218 */
    rwImageFormat *imageFormats;     /* 0x21C */
};

/* The engine's string functions, typed (the table itself keeps untyped slots). */
#define RWSTRLEN(s) (RwEngineInstance->stringFuncs.vecStrlen(s))
#define RWSTRCPY(d, s) (RwEngineInstance->stringFuncs.vecStrcpy((d), (s)))
#define RWSTRCHR(s, c)                                                                             \
    (((char *(*)(const char *, int))RwEngineInstance->stringFuncs.vecStrchr)((s), (c)))
#define RWSTRRCHR(s, c)                                                                            \
    (((char *(*)(const char *, int))RwEngineInstance->stringFuncs.vecStrrchr)((s), (c)))
#define RWSTRCMP(a, b)                                                                             \
    (((int (*)(const char *, const char *))RwEngineInstance->stringFuncs.vecStrcmp)((a), (b)))

#ifdef __cplusplus
}
#endif

#endif

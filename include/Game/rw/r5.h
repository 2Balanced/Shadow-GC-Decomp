/* Private types of the RenderWare files of range r5 (src/RenderWare/Rt2dDevice.c .. RwResources.c,
 * 0x80473E9C..0x80480C6C; wave 40's size-cut units RwCpp37..RwCpp44, re-cut by source file in
 * wave 41). Inferred from the retail code only; a later pass unifies them with the neighbours'
 * headers. */
#ifndef GAME_RW_R5_H
#define GAME_RW_R5_H

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef s32 RwBool;

/* RwGlobals as far as these units reach into it: the string and memory vectors. */
typedef struct R5RwGlobals {
    /* 0x000 */ struct RwCamera *curCamera;
    /* 0x004 */ u8 pad004[0x14];
    /* 0x018 */ f32 zBufferNear; /* the device's */
    /* 0x01C */ f32 zBufferFar;
    /* 0x020 */ RwBool (*renderStateSet)(s32 state, void *value); /* the device's RwRenderStateSet */
    /* 0x024 */ RwBool (*renderStateGet)(s32 state, void *value);
    /* 0x028 */ u8 pad028[0xC];
    /* 0x034 */ RwBool (*im2DRenderIndexedPrimitive)(s32 type, void *vertices, s32 numVertices, u16 *indices,
                                                     s32 numIndices);
    /* 0x038 */ u8 pad038[0x8C];
    /* 0x0C4 */ s32 (*vecSprintf)(char *buffer, const char *format, ...);
    /* 0x0C8 */ void *vecVsprintf;
    /* 0x0CC */ char *(*vecStrcpy)(char *dst, const char *src);
    /* 0x0D0 */ char *(*vecStrncpy)(char *dst, const char *src, u32 n);
    /* 0x0D4 */ char *(*vecStrcat)(char *dst, const char *src);
    /* 0x0D8 */ char *(*vecStrncat)(char *dst, const char *src, u32 n);
    /* 0x0DC */ char *(*vecStrrchr)(const char *s, int c);
    /* 0x0E0 */ char *(*vecStrchr)(const char *s, int c);
    /* 0x0E4 */ char *(*vecStrstr)(const char *s, const char *t);
    /* 0x0E8 */ int (*vecStrcmp)(const char *a, const char *b);
    /* 0x0EC */ int (*vecStrncmp)(const char *a, const char *b, u32 n);
    /* 0x0F0 */ int (*vecStricmp)(const char *a, const char *b);
    /* 0x0F4 */ u32 (*vecStrlen)(const char *s);
    /* 0x0F8 */ char *(*vecStrupr)(char *s);
    /* 0x0FC */ char *(*vecStrlwr)(char *s);
    /* 0x100 */ char *(*vecStrtok)(char *s, const char *delim);
    /* 0x104 */ int (*vecSscanf)(const char *buffer, const char *format, ...);
    /* 0x108 */ void *(*rwmalloc)(u32 size, u32 hint);
    /* 0x10C */ void (*rwfree)(void *mem);
    /* 0x110 */ void *(*rwrealloc)(void *mem, u32 size, u32 hint);
    /* 0x114 */ void *(*rwcalloc)(u32 num, u32 size, u32 hint);
    /* 0x118 */ void *(*memoryAlloc)(void *freeList, u32 hint);
    /* 0x11C */ void *(*memoryFree)(void *freeList, void *mem);
    /* 0x120 */ void *metrics;
    /* 0x124 */ s32 engineStatus;
    /* 0x128 */ u32 resArenaInitSize; /* the resource arena's size, from RwEngineInit */
} R5RwGlobals;

extern R5RwGlobals *lbl_805F15B4; /* RwEngineInstance */
#define RWSRCGLOBAL(f) (lbl_805F15B4->f)

/* rwSList (RwCpp43): a growable array. */
typedef struct RwSList RwSList;
RwSList *fn_8047DC90(s32 entrySize, u32 hint); /* create */
void *fn_8047DFAC(RwSList *list);              /* the entries */
RwBool fn_8047DFC8(RwSList *list);             /* destroy */
void *fn_8047DDBC(RwSList *list, u32 hint);    /* a new entry */
s32 fn_8047E03C(RwSList *list);                /* the number of entries */
void *fn_8047DEA0(RwSList *list, s32 count, u32 hint); /* `count` new entries */
void *fn_8047E044(RwSList *list);              /* frees the list and returns its entries as an array */
void *fn_8047E0CC(RwSList *list, s32 index);   /* one entry */
RwSList *fn_8047DDB0(RwSList *list);           /* empties the list */

typedef struct RwTexture RwTexture;
typedef struct Rt2dPath Rt2dPath;
RwBool fn_80475E2C(Rt2dPath *path);    /* Rt2dPathDestroy */
RwBool fn_8048CD8C(RwTexture *texture); /* RwTextureDestroy */

/* One glyph of a 2D toolkit font, 0x20 bytes. */
typedef struct Rt2dGlyph {
    /* 0x00 */ f32 width;
    /* 0x04 */ u8 pad04[0x14];
    /* 0x18 */ Rt2dPath *path;
    /* 0x1C */ u32 pad1C;
} Rt2dGlyph;

/* A 2D toolkit font (Rt2dFont), 0x144 bytes at least. */
typedef struct Rt2dFont {
    /* 0x000 */ s32 isOutline;
    /* 0x004 */ f32 height;
    /* 0x008 */ f32 baseline;
    /* 0x00C */ f32 intergap;
    /* 0x010 */ u32 flag;        /* bit 1: two-byte strings */
    /* 0x014 */ RwTexture *texture[4];
    /* 0x024 */ s16 map[128];    /* glyph index of each 7-bit character, -1 = none */
    /* 0x124 */ s32 wideBase;    /* first character of wideMap */
    /* 0x128 */ s32 wideCount;
    /* 0x12C */ s16 *wideMap;
    /* 0x130 */ u32 glyphCount;
    /* 0x134 */ RwSList *glyphs;
    /* 0x138 */ void *show;
    /* 0x13C */ s32 field_0x13C;
    /* 0x140 */ s32 field_0x140;
} Rt2dFont;

typedef struct RwV2d {
    f32 x, y;
} RwV2d;

typedef struct RwV3d {
    f32 x, y, z;
} RwV3d;

typedef struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
} RwMatrix;

typedef struct RwRaster {
    /* 0x00 */ u8 pad00[0xC];
    /* 0x0C */ s32 width;
    /* 0x10 */ s32 height;
} RwRaster;

typedef struct RwRGBA {
    u8 red, green, blue, alpha;
} RwRGBA;

typedef struct RwRGBAReal {
    f32 red, green, blue, alpha;
} RwRGBAReal;

struct RwTexture {
    /* 0x00 */ RwRaster *raster;
    /* 0x04 */ u8 pad04[0x4C];
    /* 0x50 */ u32 filterAddressing; /* filter in bits 0-7, U addressing 8-11, V addressing 12-15 */
};

/* An immediate-mode 3D vertex of this driver, 0x24 bytes. */
typedef struct RwIm3DVertex {
    /* 0x00 */ RwV3d objVertex;
    /* 0x0C */ RwV3d objNormal;
    /* 0x18 */ RwRGBA color;
    /* 0x1C */ f32 u;
    /* 0x20 */ f32 v;
} RwIm3DVertex;

void *fn_8049A82C(RwIm3DVertex *vertices, s32 count, RwMatrix *ltm, u32 flags); /* RwIm3DTransform */
RwBool fn_8049A95C(s32 type, u16 *indices, s32 count); /* RwIm3DRenderIndexedPrimitive */
RwBool fn_8049A90C(void);                              /* RwIm3DEnd */

/* The colour and texture coordinates at one edge of a 2D brush, 0x18 bytes. */
typedef struct Rt2dShade {
    /* 0x00 */ RwRGBAReal col;
    /* 0x10 */ RwV2d uv;
} Rt2dShade;

/* A 2D toolkit brush (Rt2dBrush). */
typedef struct Rt2dBrush {
    /* 0x00 */ Rt2dShade top;     /* inner edge of a stroke */
    /* 0x18 */ Rt2dShade dtop;    /* its change along the path */
    /* 0x30 */ Rt2dShade bottom;  /* outer edge */
    /* 0x48 */ Rt2dShade dbottom;
    /* 0x60 */ RwRGBA colorCache; /* the flat colour */
    /* 0x64 */ s32 flag;          /* bit 0: shaded colours, bit 1: texture coordinates */
    /* 0x68 */ RwTexture *texture;
    /* 0x6C */ u32 pad6C;
    /* 0x70 */ f32 halfwidth;
} Rt2dBrush;

typedef struct RwFrame RwFrame;

typedef struct RwCamera {
    /* 0x00 */ u32 type;
    /* 0x04 */ RwFrame *frame;
    /* 0x08 */ u8 pad08[0xC];
    /* 0x14 */ s32 projection; /* 1 perspective, 2 parallel */
    /* 0x18 */ void *beginUpdate; /* guessed */
    /* 0x1C */ void *(*endUpdate)(void *camera); /* the hook the 2D toolkit replaces */
    /* 0x20 */ u8 pad20[0x40];
    /* 0x60 */ RwRaster *frameBuffer;
    /* 0x64 */ RwRaster *zBuffer;
    /* 0x68 */ RwV2d viewWindow;
} RwCamera;

/* One node of a flattened 2D path, 0x18 bytes. */
typedef struct Rt2dPathNode {
    /* 0x00 */ s32 type; /* 0 move, 1 line */
    /* 0x04 */ RwV2d pos;
    /* 0x0C */ RwV2d normal;
    /* 0x14 */ f32 dist;
} Rt2dPathNode;

/* A 2D path: a list of nodes and the sub-path that follows. */
struct Rt2dPath {
    /* 0x00 */ RwSList *segments; /* a list while locked, else a plain array of nodes */
    /* 0x04 */ s32 numSegment;    /* -1 while the path is locked for editing */
    /* 0x08 */ s32 closed;
    /* 0x0C */ s32 flat;          /* no curve segments */
    /* 0x10 */ f32 inset;
    /* 0x14 */ Rt2dPath *next;
    /* 0x18 */ Rt2dPath *curr;    /* the sub-path being built */
};

/* The 2D toolkit's static data (lbl_805E1950 is one .bss block of 0xD58 bytes). */
typedef struct Rt2dStatics {
    /* 0x000 */ u16 index[64];
    /* 0x080 */ u8 pad080[0xC4];
    /* 0x144 */ RwIm3DVertex *vertexBuffer;
    /* 0x148 */ void *fontFreeList;
    /* 0x14C */ char *fontPath;
    /* 0x150 */ u8 pad150[0x10];
    /* 0x160 */ u32 field_0x160;
    /* 0x164 */ void *pathFreeList;
    /* 0x168 */ Rt2dPath *scratchPath;
    /* 0x16C */ void *brushFreeList;
    /* 0x170 */ u32 pad170;
    /* 0x174 */ void *freeList174; /* 8-byte entries */
    /* 0x178 */ void *freeList178; /* 0x14-byte entries */
    /* 0x17C */ u8 pad17C[8];
    /* 0x184 */ RwMatrix viewMatrix;
    /* 0x1C4 */ RwMatrix *ctm[32];
    /* 0x244 */ s32 ctmTop;
    /* 0x248 */ RwV2d viewSize;
    /* 0x250 */ RwV2d rasterSize;
    /* 0x258 */ RwMatrix toDevice;     /* CTM x view, scaled to the view window */
    /* 0x298 */ RwMatrix deviceMatrix; /* its inverse */
    /* 0x2D8 */ RwMatrix toWorld;      /* toDevice x the camera frame's LTM */
    /* 0x318 */ s32 deviceValid;
    /* 0x31C */ u32 field_0x31C; /* RwIm3DTransform flags of the strokes */
    /* 0x320 */ f32 layerDepth;
    /* 0x324 */ f32 flatness;
    /* 0x328 */ f32 flatDepth;
    /* 0x32C */ s32 flatValid;
    /* 0x330 */ s32 field_0x330;
    /* 0x334 */ u32 pad334;
    /* 0x338 */ f32 field_0x338;
    /* 0x33C */ f32 field_0x33C;
    /* 0x340 */ f32 field_0x340;
    /* 0x344 */ f32 field_0x344;
    /* 0x348 */ u32 pad348;
    /* 0x34C */ RwCamera *camera;
    /* 0x350 */ u8 pad350[0x48];
    /* 0x398 */ void *(*cameraBeginUpdate)(void *camera);
    /* 0x39C */ u8 pad39C[0x4A4];
    /* 0x840 */ Rt2dFont *(*fontRead)(const char *name);
    /* 0x844 */ u8 pad844[0x100];
    /* 0x944 */ void *field_0x944;
    /* 0x948 */ s32 field_0x948;
    /* 0x94C */ u8 pad94C[0x400];
    /* 0xD4C */ s32 field_0xD4C; /* the font module's (read by the font file in range r4) */
    /* 0xD50 */ s32 field_0xD50;
} Rt2dStatics; /* 0xD54 bytes: the device file's whole .bss (padded to 0xD58 by the link) */

extern Rt2dStatics lbl_805E1950;

/* RwStream and friends (RwCpp42/43 and beyond). */
typedef struct RwStream RwStream;
typedef struct RwError {
    s32 pluginID;
    s32 errorCode;
} RwError;
s32 fn_8047DC28(s32 code, ...);           /* _rwerror */
RwError *fn_8047DB84(RwError *error);     /* RwErrorSet */
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length); /* RwStreamRead */
RwBool fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version); /* RwStreamFindChunk */
RwStream *fn_8047CEF0(RwStream *stream, s32 type, s32 size, u32 version, u32 build); /* writes a chunk header */
void *fn_8047D158(void *mem, u32 size);   /* converts stream floats in place */
void *fn_8047D254(void *mem, u32 size);   /* converts stream 32-bit words in place */
RwStream *fn_8047D868(RwStream *stream, s32 *ints, u32 size);     /* RwStreamReadInt32 */
RwStream *fn_8047D784(RwStream *stream, f32 *reals, u32 size);    /* RwStreamReadReal */
RwStream *fn_8047D458(RwStream *stream, const s32 *ints, u32 size); /* RwStreamWriteInt32 */
RwStream *fn_8047D350(RwStream *stream, const f32 *reals, u32 size); /* RwStreamWriteReal */

#define R5ERROR(id, code)                       \
    do {                                        \
        RwError err;                            \
        err.pluginID = (id);                    \
        err.errorCode = fn_8047DC28 code;       \
        fn_8047DB84(&err);                      \
    } while (0)

/* RtAnim, the animation toolkit (RwCpp39/40). */
typedef struct RtAnimAnimation RtAnimAnimation;
typedef struct RtAnimInterpolator RtAnimInterpolator;
typedef RtAnimInterpolator *(*RtAnimCallBack)(RtAnimInterpolator *interp, void *data);

typedef struct RtAnimInterpolatorInfo {
    /* 0x00 */ s32 typeID;
    /* 0x04 */ s32 interpKeyFrameSize;
    /* 0x08 */ s32 animKeyFrameSize;
    /* 0x0C */ void (*keyFrameApplyCB)(void *result, void *frame);
    /* 0x10 */ void (*keyFrameBlendCB)(void *out, void *in1, void *in2, f32 alpha);
    /* 0x14 */ void (*keyFrameInterpolateCB)(void *out, void *in1, void *in2, f32 time, void *custom);
    /* 0x18 */ void (*keyFrameAddCB)(void *out, void *in1, void *in2);
    /* 0x1C */ void (*keyFrameMulRecipCB)(void *frame, void *start);
    /* 0x20 */ RtAnimAnimation *(*keyFrameStreamReadCB)(RwStream *stream, RtAnimAnimation *anim);
    /* 0x24 */ RwBool (*keyFrameStreamWriteCB)(const RtAnimAnimation *anim, RwStream *stream);
    /* 0x28 */ s32 (*keyFrameStreamGetSizeCB)(const RtAnimAnimation *anim);
    /* 0x2C */ s32 customDataSize;
} RtAnimInterpolatorInfo;

struct RtAnimAnimation {
    /* 0x00 */ RtAnimInterpolatorInfo *interpInfo;
    /* 0x04 */ s32 numFrames;
    /* 0x08 */ s32 flags;
    /* 0x0C */ f32 duration;
    /* 0x10 */ void *pFrames;
    /* 0x14 */ void *customData;
};

/* The header every key frame starts with. */
typedef struct RtAnimKeyFrameHeader {
    /* 0x00 */ struct RtAnimKeyFrameHeader *prevFrame;
    /* 0x04 */ f32 time;
} RtAnimKeyFrameHeader;

/* The header every interpolated frame starts with. */
typedef struct RtAnimInterpFrameHeader {
    /* 0x00 */ RtAnimKeyFrameHeader *keyFrame1;
    /* 0x04 */ RtAnimKeyFrameHeader *keyFrame2;
} RtAnimInterpFrameHeader;

struct RtAnimInterpolator {
    /* 0x00 */ RtAnimAnimation *pCurrentAnim;
    /* 0x04 */ f32 currentTime;
    /* 0x08 */ void *pNextFrame;
    /* 0x0C */ RtAnimCallBack pAnimCallBack;
    /* 0x10 */ void *pAnimCallBackData;
    /* 0x14 */ f32 animCallBackTime;
    /* 0x18 */ RtAnimCallBack pAnimLoopCallBack;
    /* 0x1C */ void *pAnimLoopCallBackData;
    /* 0x20 */ s32 maxInterpKeyFrameSize;
    /* 0x24 */ s32 currentInterpKeyFrameSize;
    /* 0x28 */ s32 currentAnimKeyFrameSize;
    /* 0x2C */ s32 numNodes;
    /* 0x30 */ RwBool isSubInterpolator;
    /* 0x34 */ s32 offsetInParent;
    /* 0x38 */ RtAnimInterpolator *parentAnimation;
    /* 0x3C */ void (*keyFrameApplyCB)(void *result, void *frame);
    /* 0x40 */ void (*keyFrameBlendCB)(void *out, void *in1, void *in2, f32 alpha);
    /* 0x44 */ void (*keyFrameInterpolateCB)(void *out, void *in1, void *in2, f32 time, void *custom);
    /* 0x48 */ void (*keyFrameAddCB)(void *out, void *in1, void *in2);
    /* 0x4C: the interpolated frames, numNodes of currentInterpKeyFrameSize bytes */
};

#define R5ANIMFRAME(interp, n) \
    ((RtAnimInterpFrameHeader *)((u8 *)((interp) + 1) + (n) * (interp)->currentInterpKeyFrameSize))

/* RwImage, as far as RtCharset uses it. */
typedef struct R5Image {
    /* 0x00 */ s32 flags;
    /* 0x04 */ s32 width;
    /* 0x08 */ s32 height;
    /* 0x0C */ s32 depth;
    /* 0x10 */ s32 stride;
    /* 0x14 */ u8 *cpPixels;
    /* 0x18 */ RwRGBA *palette;
} R5Image;

/* An immediate-mode 2D vertex of this driver, 0x18 bytes. */
typedef struct RwIm2DVertex {
    /* 0x00 */ f32 x;
    /* 0x04 */ f32 y;
    /* 0x08 */ f32 z;
    /* 0x0C */ RwRGBA emissiveColor;
    /* 0x10 */ f32 u;
    /* 0x14 */ f32 v;
} RwIm2DVertex;

/* RtCharset, the debug character set (RwCpp40): a raster holding the font. */
typedef RwRaster RtCharset;

typedef struct RtCharsetDesc {
    /* 0x00 */ s32 width;
    /* 0x04 */ s32 height;
    /* 0x08 */ s32 width_internal;
    /* 0x0C */ s32 height_internal;
    /* 0x10 */ s32 count;
    /* 0x14 */ s32 tile_width;
    /* 0x18 */ s32 tile_height;
} RtCharsetDesc;

/* The buffers of RtCharsetPrintBuffered (lbl_8056DD68). */
typedef struct RtCharsetBuffer {
    /* 0x00 */ RwBool open;
    /* 0x04 */ s32 numChars;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ RwIm2DVertex *vertices;
    /* 0x10 */ u16 *indices;
    /* 0x14 */ RtCharset *charSet;
} RtCharsetBuffer;

/* RtDict (RwCpp40). */
typedef struct RtDict RtDict;
typedef struct RtDictSchema {
    /* 0x00 */ const char *name;
    /* 0x04 */ u32 dictChunkType;
    /* 0x08 */ u32 elementChunkType;
    /* 0x0C */ u32 compatibilityVersion;
    /* 0x10 */ RwSList *dictionaries;
    /* 0x14 */ RtDict *current;
    /* 0x18 */ void *(*addRefCB)(void *entry);
    /* 0x1C */ RwBool (*destroyCB)(void *entry);
    /* 0x20 */ const char *(*getNameCB)(void *entry);
    /* 0x24 */ u32 (*streamGetSizeCB)(const void *entry);
    /* 0x28 */ void *(*streamReadCB)(RwStream *stream);
    /* 0x2C */ RtDict *(*streamReadCompatibilityCB)(RwStream *stream);
} RtDictSchema;

struct RtDict {
    /* 0x00 */ RtDictSchema *schema;
    /* 0x04 */ RwSList *entries;
};

/* RtFSyst, the file system toolkit (RwCpp40/41). */
typedef struct RtFile RtFile;
typedef struct RtFileSystem RtFileSystem;

/* The file system manager (lbl_805E2A40). */
typedef struct RtFSManager {
    /* 0x00 */ RtFileSystem *fsList;
    /* 0x04 */ s32 maxNbFS;
    /* 0x08 */ s32 numFS;
    /* 0x0C */ RwBool isOpen;
    /* 0x10 */ u32 pad10;
    /* 0x14 */ void (*errorCallback)(s32 error);
} RtFSManager;

/* A file system; the type's own data follows. */
struct RtFileSystem {
    /* 0x00 */ RtFileSystem *next;
    /* 0x04 */ s32 maxNbOpenFiles;
    /* 0x08 */ s32 status;
    /* 0x0C */ char *deviceName;
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ RtFile *(*getFileObject)(RtFileSystem *fs, u32 index);
    /* 0x18 */ void (*close)(RtFileSystem *fs);
    /* 0x1C */ const char *(*getRootPath)(RtFileSystem *fs);
    /* 0x20 */ void (*registerCB)(char *name);
    /* 0x24 */ void (*unregisterCB)(char *name);
    /* 0x28 */ s32 (*fileOpen)(RtFileSystem *fs, RtFile *file, const char *name, u32 flags);
    /* 0x2C */ void (*fileClose)(RtFile *file);
    /* 0x30 */ u32 (*fileRead)(RtFile *file, void *buffer, u32 size);
    /* 0x34 */ u32 (*fileWrite)(RtFile *file, const void *buffer, u32 size);
    /* 0x38 */ s64 (*fileSetPosition)(RtFile *file, s32 offset, s32 whence);
    /* 0x3C */ s32 (*fileSync)(RtFile *file);
    /* 0x40 */ void (*fileAbort)(RtFile *file);
    /* 0x44 */ RwBool (*fileIsEOF)(RtFile *file);
    /* 0x48 */ s32 (*fileGetStatus)(RtFile *file);
    /* 0x4C */ RwBool (*fileExists)(RtFileSystem *fs, const char *name);
    /* 0x50 */ char name[8];
    /* 0x58: the GameCube DVD file system's own data */
    /* 0x58 */ RtFile *files;
    /* 0x5C */ void (*errorCallBack)(s32 error, void *data);
    /* 0x60 */ void *errorCallBackData;
};

/* The DVD's file information (the Dolphin SDK's DVDFileInfo, 0x3C bytes). */
typedef struct R5DVDFileInfo {
    /* 0x00 */ u8 commandBlock[0x2C];
    /* 0x2C */ s32 userData; /* the command block's: here the result of the read */
    /* 0x30 */ u32 startAddr;
    /* 0x34 */ u32 length;
    /* 0x38 */ void *callback;
} R5DVDFileInfo;

/* A file of a file system (0x90 bytes in the DVD file system). */
struct RtFile {
    /* 0x00 */ s64 length;
    /* 0x08 */ s64 position;
    /* 0x10 */ s64 startPosition; /* of the read in progress */
    /* 0x18 */ s32 field_0x18;    /* asynchronous */
    /* 0x1C */ s32 field_0x1C;
    /* 0x20 */ s32 status;        /* 1 = free */
    /* 0x24 */ s32 field_0x24;
    /* 0x28 */ s32 asyncStatus;   /* 1 = idle */
    /* 0x2C */ void *asyncData;
    /* 0x30 */ void (*asyncCallBack)(RtFile *file, void *data, s32 fileStatus, s32 status, void *callBackData);
    /* 0x34 */ void *asyncCallBackData;
    /* 0x38 */ RtFileSystem *fs;
    /* 0x3C */ u32 pad3C;
    /* 0x40 */ R5DVDFileInfo dvdInfo;
    /* 0x7C */ u8 *buffer;
    /* 0x80 */ s32 bufferValid;
    /* 0x84 */ s32 bufferPos;
    /* 0x88 */ s32 bufferSize;
    /* 0x8C */ u32 pad8C;
};

/* The stream module (RwCpp42/43). */
/* A chunk header as it is in a stream. */
typedef struct R5ChunkHeader {
    /* 0x00 */ u32 type;
    /* 0x04 */ u32 length;
    /* 0x08 */ u32 libraryID;
} R5ChunkHeader;

/* RwChunkHeaderInfo. */
typedef struct R5ChunkHeaderInfo {
    /* 0x00 */ u32 type;
    /* 0x04 */ u32 length;
    /* 0x08 */ u32 version;
    /* 0x0C */ u32 buildNum;
    /* 0x10 */ RwBool isComplex;
} R5ChunkHeaderInfo;

/* The engine modules (RwCpp43/44). */
/* A module's bookkeeping (RwModuleInfo): its globals' offset in RwEngineInstance and its
 * number of instances. */
typedef struct R5ModuleInfo {
    s32 globalsOffset;
    s32 numInstances;
} R5ModuleInfo;

/* The matrix module's globals in RwEngineInstance. */
typedef struct R5MatrixGlobals {
    /* 0x00 */ void *freeList;
    /* 0x04 */ u32 optimizations;
    /* 0x08 */ RwMatrix *(*multiply)(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b);
    /* 0x0C */ RwV3d tolerance; /* normal, orthogonal, identity */
} R5MatrixGlobals;

/* The free list module (RwCpp44). */
typedef struct R5Link {
    struct R5Link *next;
    struct R5Link *prev;
} R5Link;

/* RwFreeList. */
typedef struct R5FreeList {
    /* 0x00 */ u32 entrySize;       /* rounded up to the alignment */
    /* 0x04 */ u32 entriesPerBlock;
    /* 0x08 */ u32 heapSize;        /* bytes of the used-entry bitmap at the start of each block */
    /* 0x0C */ u32 alignment;
    /* 0x10 */ R5Link blockList;    /* the blocks: an R5Link, the bitmap, then the entries */
    /* 0x18 */ u32 flags; /* bit 0: the list lives in caller's memory; bit 1: free empty blocks */
    /* 0x1C */ R5Link link;         /* in the list of all free lists */
} R5FreeList;

typedef void (*R5FreeListCallBack)(void *entry, void *data);

/* RwMemoryFunctions: the memory vectors at RwEngineInstance + 0x108. */
typedef struct R5MemoryFunctions {
    void *(*rwmalloc)(u32 size, u32 hint);
    void (*rwfree)(void *mem);
    void *(*rwrealloc)(void *mem, u32 size, u32 hint);
    void *(*rwcalloc)(u32 num, u32 size, u32 hint);
} R5MemoryFunctions;

/* The resource arena (RenderWare/RwResources.c). */
typedef struct R5LinkList {
    R5Link link;
} R5LinkList;

typedef struct R5ResEntry R5ResEntry;
typedef void (*R5ResEntryDestroyNotify)(R5ResEntry *entry);

/* RwResEntry: a block of the arena, the user's data follows it. */
struct R5ResEntry {
    /* 0x00 */ R5Link link;
    /* 0x08 */ s32 size;
    /* 0x0C */ void *owner;
    /* 0x10 */ R5ResEntry **ownerRef;
    /* 0x14 */ R5ResEntryDestroyNotify destroyNotify;
};

/* The resources module's globals in RwEngineInstance. */
typedef struct R5ResourcesGlobals {
    /* 0x00 */ s32 maxSize;
    /* 0x04 */ s32 currentSize;
    /* 0x08 */ s32 reusageSize;
    /* 0x0C */ void *memHeap;
    /* 0x10 */ R5LinkList entriesA;
    /* 0x18 */ R5LinkList entriesB;
    /* 0x20 */ R5LinkList *freeEntries;
    /* 0x24 */ R5LinkList *usedEntries;
} R5ResourcesGlobals;

/* RwCpp39. */
/* The header of a path in a stream, 0x10 bytes. */
typedef struct Rt2dPathStreamHeader {
    /* 0x00 */ s32 type;       /* 1 */
    /* 0x04 */ s32 numSegment;
    /* 0x08 */ u32 flag;       /* bit 0 closed, bit 1 flat, bit 2 a sub-path follows */
    /* 0x0C */ f32 inset;
} Rt2dPathStreamHeader;

/* A variable argument list (the PowerPC EABI's va_list). */
typedef struct R5VaList {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} R5VaList[1];

#ifdef __cplusplus
}
#endif

#endif

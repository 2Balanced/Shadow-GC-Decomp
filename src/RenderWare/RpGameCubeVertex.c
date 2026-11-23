/* RenderWare 3.7 world plugin, GameCube: vertex helpers of the instancing code,
 * 0x80465CDC-0x80466B58 (wave 41 re-cut of the size-cut Game/RwCpp27 tail and Game/RwCpp28): index
 * renumbering after a vertex weld, the vertex stream descriptors and format helpers that the
 * geometry and world-sector instancing code calls. The file owns the jump tables of its switches
 * (.data 0x8056D978-0x8056DAF8). The vertex-array converters behind it (old Game/RwCpp29, RwCpp30,
 * built with GC/1.3.2) may continue this file; they are left as they are.
 * Most functions are still untyped drafts. */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include "Game/rw/r3.h"

extern s32 lbl_805F1690;            /* offset of the world plugin's globals in the engine block */

/* The world plugin keeps its default pipelines in its engine globals (an array of pointers). */
#define RPWORLDGLOBALPIPE(offset) \
    (((void **)((u8 *)lbl_805F15B4 + lbl_805F1690))[(offset) / sizeof(void *)])

s32 fn_80485D78(s32, u32, void *, void *);                 /* RwEngineRegisterPlugin */
s32 fn_8045994C(s32, u32, void *, void *, void *);         /* RpGeometryRegisterPlugin */
s32 fn_80460790(s32, u32, void *, void *, void *);         /* RpWorldSectorRegisterPlugin */
s32 fn_804647C8(void);
void fn_804626F8(void *pipeline);
void fn_80462734(void *pipeline);
void fn_8049B800(void *pipeline);                          /* RxPipelineDestroy */

void *fn_80462914(void *object, RxGCInstanceData *data);
void *fn_80463428(void *object, RxGCInstanceData *data);
void *fn_80463840(void *object, RxGCInstanceData *data);
void *fn_804639AC(void *object, RxGCInstanceData *data);
void *fn_80463DF4(void *object, RxGCInstanceData *data);
void *fn_80463F10(void *object, RxGCInstanceData *data);
RxPipeline *fn_8046335C(u32, u32, RxGCNodeCallBack, RxGCNodeCallBack, RxGCNodeCallBack,
                        RxGCNodeCallBack);
RxPipeline *fn_80463D28(u32, u32, RxGCNodeCallBack, RxGCNodeCallBack, RxGCNodeCallBack,
                        RxGCNodeCallBack);
RxPipeline *fn_8049B76C(void);                                            /* RxPipelineCreate */
RxPipeline *fn_8049CCB8(RxPipeline *pipeline);                            /* RxPipelineLock */
RxPipeline *fn_8049D0D0(RxPipeline *pipeline, u32 *firstIndex, RxNodeDefinition *nodeDef, ...);
RxPipeline *fn_8049C1E0(RxPipeline *pipeline);                            /* RxLockedPipeUnlock */
RxPipelineNode *fn_8049CFCC(RxPipeline *pipeline, const char *name, RxPipelineNode *start,
                            s32 *nodeIndex);                              /* RxPipelineFindNodeByName */
RxNodeDefinition *fn_80463D1C(void);
RxNodeDefinition *fn_804641C0(void);
void fn_80462BF0(RxPipelineNode *node, RxGCNodeCallBack callback);
void fn_80462BFC(RxPipelineNode *node, RxGCNodeCallBack callback);
void fn_80462C08(RxPipelineNode *node, RxGCNodeCallBack callback);
void fn_80462C14(RxPipelineNode *node, RxGCNodeCallBack callback);

void fn_80462FAC(RxGCVertexDesc *desc);
void fn_804630C0(RxGCVertexDesc *desc);

void fn_804641CC(u8 lightFlags, RxGCInstanceData *data);
void fn_804643E8(RpLight *light, RxGCInstanceData *data);
RwMatrix *RwFrameGetLTM(RwFrame *frame);
RwSphere *fn_804566D0(RpAtomic *atomic); /* RpAtomicGetWorldBoundingSphere */
extern s32 lbl_805F1658; /* offset of the GameCube raster extension in a raster */
void fn_80462C30(RxGCVertexDesc *desc, RxGCInstanceData *data);
void fn_804950AC(RwMatrix *matrix, s32 normals); /* load the object's matrix */
void fn_80499C38(RwTexture *texture, s32 stage); /* set a texture on GX */
void fn_80497778(s32 enable);
double fmod(double x, double y);

/* Sets a material's colour registers: the ambient colour, the material colour and the material's
 * ambient coefficient (the colour setters of RenderWare/RpMaterialGameCube.c). */
typedef void (*RpGCColourCallBack)(RwRGBAReal *ambient, RwRGBA *color, f32 scale);
RpGCColourCallBack fn_804655D4(u32 flags, u32 lightMask, s32 ambientSet, u32 alpha);
extern RwMatrix lbl_805E31CC;      /* the camera's view matrix */
s32 fn_8045AAA0(s32 size, u32 pluginID, void *constructCB, void *destructCB, void *copyCB);
RpWorld *fn_8045FEA0(const RpWorldSector *sector); /* RpWorldSectorGetWorld */
void fn_8046288C(RwResEntry *repEntry);           /* instance data destroy notify */
s32 fn_8047CF50(RwStream *stream, u32 type, u32 *lengthOut, u32 *versionOut); /* RwStreamFindChunk */
RwStream *fn_8047CEF0(RwStream *stream, u32 type, u32 size, u32 version, u32 buildNum);
RwStream *fn_8047D458(RwStream *stream, const void *ints, u32 numBytes); /* RwStreamWriteInt32 */
RwStream *fn_8047D868(RwStream *stream, void *ints, u32 numBytes);       /* RwStreamReadInt32 */
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);             /* RwStreamRead */
RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length); /* RwStreamWrite */
RwV3d *fn_80482458(RwV3d *pointOut, const RwV3d *pointIn, const RwMatrix *matrix);  /* RwV3dTransformPoint */
RwV3d *fn_80482504(RwV3d *vectorOut, const RwV3d *vectorIn, const RwMatrix *matrix); /* RwV3dTransformVector */
f32 fn_8045A818(RpLight *light); /* RpLightGetConeAngle */
/* Loads a light into the next free GX light (up to eight) in view space. */

/* For each of count index lists, a copy of its indices renumbered after a vertex weld: an index
 * whose remap entry is set is redirected there, then lowered by the number of welded (remapped)
 * vertices below it. One allocation holds the row pointers and the rows. */
typedef struct RpGCIndexRemap {
    s32 *remap;
    u32 unk4;
} RpGCIndexRemap;

void *memset(void *dst, int value, u32 size);

void *memset(void *, int, u32);

static inline u32 rwElementSize(u32 type) {
    switch (type) {
    case 1: return 1;
    case 2: return 2;
    case 3: return 3;
    case 4: return 4;
    case 6: return 4;
    case 7: return 8;
    case 8: return 12;
    case 9: return 16;
    case 10: return 4;
    default: return 0;
    }
}

static inline s32 rwElementEqual(u32 type, const u8 *a, const u8 *b) {
    switch (type) {
    case 1: return *a == *b;
    case 2: return *(const u16 *)a == *(const u16 *)b;
    case 3: return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
    case 4: return *(const u32 *)a == *(const u32 *)b;
    case 6: return *(const u32 *)a == *(const u32 *)b;
    case 7:
        return *(const u32 *)a == *(const u32 *)b &&
               *(const u32 *)(a + 4) == *(const u32 *)(b + 4);
    case 8:
        return *(const u32 *)a == *(const u32 *)b &&
               *(const u32 *)(a + 4) == *(const u32 *)(b + 4) &&
               *(const u32 *)(a + 8) == *(const u32 *)(b + 8);
    case 9:
        return *(const u32 *)a == *(const u32 *)b &&
               *(const u32 *)(a + 4) == *(const u32 *)(b + 4) &&
               *(const u32 *)(a + 8) == *(const u32 *)(b + 8) &&
               *(const u32 *)(a + 12) == *(const u32 *)(b + 12);
    case 10: return *(const u32 *)a == *(const u32 *)b;
    default: return 0;
    }
}

static inline u32 *rwAlloc(u32 size) {
    return (u32 *)((void *(*)(u32, u32))*(u32 *)((u8 *)lbl_805F15B4 + 0x108))(size, 0x3050D);
}

u16 **fn_80465CDC(RpGCIndexRemap *remaps, u16 **indexLists, u32 count, u32 numIndices) {
    u32 rowSize;
    u16 **rows;
    u32 offset;
    u32 i;

    rowSize = numIndices * sizeof(u16);
    rows = (u16 **)RwMalloc(count * sizeof(u16 *) + count * rowSize, 0x3050D);
    if (rows == NULL) {
        return NULL;
    }
    memset(rows, 0, count * sizeof(u16 *));
    offset = count * sizeof(u16 *);
    for (i = 0; i < count; i++) {
        rows[i] = (u16 *)((u8 *)rows + offset);
        offset += rowSize;
    }
    for (i = 0; i < count; i++) {
        s32 *remap = remaps[i].remap;
        u16 *src = indexLists[i];
        u16 *dst = rows[i];
        u32 j;
        for (j = 0; j < numIndices; j++) {
            u16 index = *src;
            u32 below = 0;
            u16 k;
            if (remap[index] != -1) {
                index = remap[index];
            }
            for (k = 0; k < index; k++) {
                if (remap[k] != -1) {
                    below++;
                }
            }
            src++;
            *dst = index - below;
            dst++;
        }
    }
    return rows;
}

u32 *fn_80465ED4(u32 *descriptors, u8 *inputs, u32 streams, u32 vertices) {
    u32 bytes = (streams & 0x1FFFFFFF) * 8;
    { u32 i; for (i = 0; i < streams; i++) {
        bytes += descriptors[i * 2 + 1] * rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
    }}
    {
    u32 *result = rwAlloc(bytes);
    if (result == NULL) return NULL;
    {
    u32 headerBytes = (streams & 0x1FFFFFFF) * 8;
    memset(result, 0, headerBytes);
    { u32 i, offset; for (i = 0, offset = headerBytes; i < streams; i++) {
        result[i * 2] = (u32)result + offset;
        offset += descriptors[i * 2 + 1] * rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
    }}
    { u32 i; for (i = 0; i < streams; i++) {
        u32 size = rwElementSize(*(u32 *)(inputs + i * 0x18 + 4));
        u8 *dest = (u8 *)result[i * 2];
        u8 *source = *(u8 **)(inputs + i * 0x18);
        { u32 j; for (j = 0; j < vertices; j++) {
            if (((s32 *)descriptors[i * 2])[j] == -1) {
                memcpy(dest, source, size);
                dest += size;
            }
            source += size;
        }}
        result[i * 2 + 1] = descriptors[i * 2 + 1];
    }}
    return result;
}}}

/* descriptors is an array of 0x18-byte stream descriptors: +0x0 element data
 * pointer, +0x4 element type, +0x8 onward signed stream indices whose elements
 * must match as well, terminated by the first negative index.  The returned
 * block is streams {pointer, count} pairs followed by streams * vertices
 * index words; every vertex starts unassigned (0xFFFFFFFF) and a vertex that
 * duplicates an earlier one stores that earlier vertex index and drops the
 * stream count. */
u32 *fn_80466194(u32 *descriptors, u32 streams, u32 vertices) {
    u32 total = streams * vertices;
    u32 headerBytes = streams * 8;
    u32 *result = rwAlloc(headerBytes + total * 4);
    if (result == NULL) return NULL;
    memset(result, 0, headerBytes);
    {
    u32 *indices = (u32 *)((u8 *)result + headerBytes);
    { u32 i; for (i = 0; i < total; i++) indices[i] = 0xFFFFFFFF; }
    {
    u32 *header = result;
    u32 offset = 0;
    { u32 i; for (i = 0; i < streams; i++) {
        header[i * 2] = (u32)((u8 *)indices + offset);
        header[i * 2 + 1] = vertices;
        offset += vertices * 4;
    }}
    {
    u8 *descriptor = (u8 *)descriptors;
    { u32 stream; for (stream = 0; stream < streams; stream++) {
        u32 size = rwElementSize(*(u32 *)(descriptor + 4));
        { u32 i; for (i = 0; i < vertices; i++) {
            if (((s32 *)header[0])[i] != -1) continue;
            { u32 j; for (j = i; j < vertices; j++) {
                if (i == j) continue;
                if (!rwElementEqual(*(u32 *)(descriptor + 4),
                                    *(u8 **)descriptor + i * size,
                                    *(u8 **)descriptor + j * size))
                    continue;
                {
                s32 equal = 1;
                const s8 *link = (const s8 *)(descriptor + 8);
                u32 linkSize =
                    rwElementSize(*(u32 *)((u8 *)descriptors + (s32)*link * 0x18 + 4));
                while ((s32)*link >= 0) {
                    u8 *other = (u8 *)descriptors + (s32)*link * 0x18;
                    u8 *otherData = *(u8 **)other;
                    if (!rwElementEqual(*(u32 *)(other + 4), otherData + i * linkSize,
                                        otherData + j * linkSize)) {
                        equal = 0;
                        break;
                    }
                    link++;
                    linkSize =
                        rwElementSize(*(u32 *)((u8 *)descriptors + (s32)*link * 0x18 + 4));
                }
                if (equal) {
                    ((u32 *)header[0])[j] = i;
                    header[1]--;
                }
            }}}
        }}
        descriptor += 0x18;
        header += 2;
    }}
    return result;
}}}}

u32 fn_8046693C(u32 *format) {
    u32 base = format[1] & 1;
    if (base == 1) {
        base = 3;
    } else if (base == 0) {
        base = 2;
    }
    {
    int kind = (format[1] >> 1) & 7;
    switch (kind) {
    case 0:
    case 1: return base;
    case 2:
    case 3: return base << 1;
    case 4: return base << 2;
    default: return 0;
    }
}}

u32 fn_804669A0(u32 *format) {
    int kind = (format[1] >> 10) & 7;
    switch (kind) {
    case 1: return 3;
    case 3: return 6;
    case 4: return 12;
    default: return 0;
    }
}

u32 fn_804669EC(u32 *format, u8 channel) {
    int kind = (format[1] >> (channel * 4 + 0xE)) & 7;
    switch (kind) {
    case 0:
    case 3: return 2;
    case 1:
    case 4: return 3;
    case 2:
    case 5: return 4;
    default: return 0;
    }
}

u32 fn_80466A54(u32 *format, u8 channel) {
    u32 present;
    u32 kind;
    switch (channel) {
    case 0:
        present = (format[1] >> 0x15) & 1;
        kind = (format[1] >> 0x16) & 7;
        break;
    case 1:
        present = format[2] & 1;
        kind = (format[2] >> 1) & 7;
        break;
    case 2:
        present = (format[2] >> 9) & 1;
        kind = (format[2] >> 10) & 7;
        break;
    case 3:
        present = (format[2] >> 0x12) & 1;
        kind = (format[2] >> 0x13) & 7;
        break;
    case 4:
        present = (format[2] >> 0x1B) & 1;
        kind = (format[2] >> 0x1C) & 7;
        break;
    case 5:
        present = (format[3] >> 5) & 1;
        kind = (format[3] >> 6) & 7;
        break;
    case 6:
        present = (format[3] >> 0xE) & 1;
        kind = (format[3] >> 0xF) & 7;
        break;
    case 7:
        present = (format[3] >> 0x17) & 1;
        kind = (format[3] >> 0x18) & 7;
        break;
    default:
        return 0;
    }
    {
    u32 base = present;
    if (base == 1) {
        base = 2;
    } else if (base == 0) {
        base = 1;
    }
    {
    int signedKind = kind;
    switch (signedKind) {
    case 0:
    case 1: return base;
    case 2:
    case 3: return base << 1;
    case 4: return base << 2;
    default: return 0;
    }
}}}

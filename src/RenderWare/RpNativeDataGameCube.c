/* RenderWare 3.7 world plugin, GameCube: native (pre-instanced) data streaming,
 * 0x80464A94-0x8046528C (wave 41 re-cut of the size-cut Game/RwCpp27). The platform half of the
 * native data plugin (0x510): read and write a native data chunk (instance data and offsets), and
 * the size, write and read of a geometry's and a world sector's native data. No data of its own.
 *   fn_80464A94 / fn_80464DAC  read / write a native data chunk
 *   fn_8046510C / fn_80465138 / fn_80465180  native size / write / read of a geometry
 *   fn_804651AC / fn_80465200 / fn_80465260  native size / write / read of a world sector
 * Built with cflags_rw_r3 (configure.py).
 * Status: fn_80464A94 (the array rebasing loop's walker and counter trade registers; a separate
 * array pointer gets them right but starts the walk 12 bytes on) and fn_80464DAC (the writer) are
 * left.
 */
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

extern volatile u16 lbl_805EDF40; /* draw-sync token of the instance data built last */
extern s32 lbl_805F1520; /* offset of the descriptor pointer in a geometry (render file) */
extern s32 lbl_805F1524; /* offset of the descriptor pointer in a world sector (render file) */

/* Reads a native data chunk into a new resource entry of the owner and turns the stored offsets
 * back into pointers. */
void *fn_80464A94(RwStream *stream, void *owner, RwResEntry **repEntryRef, u32 numMeshes) {
    u32 version;
    u32 size;
    u32 headerSize;
    u32 dataSize;
    s32 platform;
    RxGCInstanceHeader *header;
    RxGCInstanceMesh *meshes;
    u8 *base;
    u32 i;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version < 0x35000 || version > 0x37002) {
        RWERROR((E_RW_BADVERSION));
        return NULL;
    }
    if (fn_8047D868(stream, &platform, sizeof(platform)) == NULL) {
        return NULL;
    }
    if (platform != 6) {
        return NULL;
    }
    if (fn_8047D868(stream, &headerSize, sizeof(headerSize)) == NULL) {
        return NULL;
    }
    if (fn_8047D868(stream, &dataSize, sizeof(dataSize)) == NULL) {
        return NULL;
    }
    *repEntryRef = (RwResEntry *)RwMalloc(sizeof(RwResEntry) + headerSize + dataSize + 31, 0x3050D);
    header = (RxGCInstanceHeader *)(*repEntryRef + 1);
    if (fn_80480D4C(stream, header, headerSize) != headerSize) {
        return NULL;
    }
    if (fn_80480D4C(stream, (void *)(((u32)header + headerSize + 31) & ~31), dataSize) != dataSize) {
        return NULL;
    }
    meshes = (RxGCInstanceMesh *)((u8 *)(header + 1) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));
    base = (u8 *)(((u32)meshes + numMeshes * sizeof(RxGCInstanceMesh) + 31) & ~31);
    for (i = 0; i < header->numArrays; i++) {
        header->arrays[i].data = base + (u32)header->arrays[i].data;
    }
    for (i = 0; i < numMeshes; i++) {
        meshes[i].displayList = base + (u32)meshes[i].displayList;
    }
    (*repEntryRef)->link.next = NULL;
    (*repEntryRef)->link.prev = NULL;
    (*repEntryRef)->owner = owner;
    (*repEntryRef)->size = size;
    (*repEntryRef)->ownerRef = repEntryRef;
    (*repEntryRef)->destroyNotify = fn_8046288C;
    header->serialNum = lbl_805EDF40;
    DCFlushRange(*repEntryRef + 1, (*repEntryRef)->size);
    GXInvalidateVtxCache();
    return owner;
}

/* Writes a resource entry's instance data as a native data chunk: the platform, the sizes of the
 * header and of the data, then both, with the pointers stored as offsets from the data base. */
RwStream *fn_80464DAC(RwStream *stream, RwResEntry *repEntry, u32 numMeshes) {
    s32 platform;
    u32 headerSize;
    u32 dataSize;
    u8 *base;
    RxGCInstanceHeader *header;
    RxGCInstanceMesh *meshes;
    u32 i;

    platform = 6;
    header = (RxGCInstanceHeader *)(repEntry + 1);
    meshes = (RxGCInstanceMesh *)((u8 *)(header + 1) +
                                  (header->numArrays - 1) * sizeof(RxGCInstanceArray));
    if (fn_8047CEF0(stream, 1, repEntry->size - 19, 0x37002, 0x37) == NULL) {
        return NULL;
    }
    if (fn_8047D458(stream, &platform, sizeof(platform)) == NULL) {
        return NULL;
    }
    headerSize = sizeof(RxGCInstanceHeader) + (header->numArrays - 1) * sizeof(RxGCInstanceArray) +
                 numMeshes * sizeof(RxGCInstanceMesh);
    if (fn_8047D458(stream, &headerSize, sizeof(headerSize)) == NULL) {
        return NULL;
    }
    dataSize = repEntry->size - headerSize - 31;
    if (fn_8047D458(stream, &dataSize, sizeof(dataSize)) == NULL) {
        return NULL;
    }
    base = (u8 *)(((u32)meshes + (numMeshes * sizeof(RxGCInstanceMesh) + 31)) & ~31);
    for (i = 0; i < header->numArrays; i++) {
        header->arrays[i].data = (void *)((u8 *)header->arrays[i].data - base);
    }
    for (i = 0; i < numMeshes; i++) {
        meshes[i].displayList = (void *)((u8 *)meshes[i].displayList - base);
    }
    if (fn_80480ED0(stream, header, headerSize) == NULL) {
        return NULL;
    }
    for (i = 0; i < header->numArrays; i++) {
        header->arrays[i].data = base + (u32)header->arrays[i].data;
    }
    for (i = 0; i < numMeshes; i++) {
        meshes[i].displayList = base + (u32)meshes[i].displayList;
    }
    if (fn_80480ED0(stream, meshes->displayList, dataSize) == NULL) {
        return NULL;
    }
    DCFlushRange(repEntry + 1, repEntry->size);
    GXInvalidateVtxCache();
    return stream;
}

/* Size of a geometry's native data chunk: the size of its resource entry less 7, or nothing when
 * the geometry is not native or has no resource entry. */
s32 fn_8046510C(const RpGeometry *geometry) {
    if (geometry->flags & rpGEOMETRYNATIVE) {
        if (geometry->repEntry != NULL) {
            return geometry->repEntry->size - 7;
        }
    }
    return 0;
}

RwStream *fn_80465138(RwStream *stream, const RpGeometry *geometry) {
    if (geometry->flags & rpGEOMETRYNATIVE) {
        fn_80464DAC(stream, geometry->repEntry, geometry->mesh->numMeshes);
    }
    return stream;
}

void *fn_80465180(RwStream *stream, RpGeometry *geometry) {
    return fn_80464A94(stream, geometry, &geometry->repEntry, geometry->mesh->numMeshes);
}

s32 fn_804651AC(const RpWorldSector *sector) {
    if (fn_8045FEA0(sector)->flags & rpGEOMETRYNATIVE) {
        if (sector->repEntry != NULL) {
            return sector->repEntry->size - 7;
        }
    }
    return 0;
}

RwStream *fn_80465200(RwStream *stream, const RpWorldSector *sector) {
    if (fn_8045FEA0(sector)->flags & rpGEOMETRYNATIVE) {
        fn_80464DAC(stream, sector->repEntry, sector->mesh->numMeshes);
    }
    return stream;
}

void *fn_80465260(RwStream *stream, RpWorldSector *sector) {
    return fn_80464A94(stream, sector, &sector->repEntry, sector->mesh->numMeshes);
}

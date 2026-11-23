/* RenderWare 3.7 world plugin, GameCube: the instance render callback and the vertex descriptors,
 * 0x80462914-0x8046318C (wave 41 re-cut of the size-cut Game/RwCpp26). The render callback both
 * instance nodes share, the setters of a node's four callbacks, a driver word, the description of
 * the instance data's vertex arrays to GX, and the vertex-descriptor plugin (0x511) of geometries
 * and world sectors. The file owns the default descriptor (.bss 0x805E1688), the driver word (.sdata
 * 0x805EDDD8), the plugin offsets and the module record (.sbss 0x805F1520-0x805F1530; the offsets
 * are read by the instancing code of other files, so they keep their address names) and its literal
 * pool (.sdata2 0x805FB180-0x805FB190: 1.0f, 0.0f and the int-to-float constant).
 *
 * Names are inferred from behaviour:
 *   fn_80462914  render callback (matrix, vertex descriptor, colours, display lists)
 *   fn_80462BF0..fn_80462C14  set one of a node's four callbacks
 *   fn_80462C20 / fn_80462C28  set / get the driver word   fn_80462C30  describe the vertex arrays
 *   fn_80462E1C  the default vertex descriptor  fn_80462E28 / fn_80462E34  plugin constructor / destructor
 *   fn_80462E70 / fn_80462EC0  module open / close    fn_80462ED4  plugin attach
 *   fn_80462F7C..fn_80462FA4  descriptor slot setters  fn_80462FAC  descriptor initialise
 *   fn_8046301C / fn_804630C0  descriptor create / destroy  fn_80463104  give a geometry a descriptor
 * Built with cflags_rw_r3 (configure.py): C, functions in definition order.
 * Matching.
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
/* The driver word (read by the instance callbacks of the node files). */
s32 lbl_805EDDD8 = 1;

/* The default vertex descriptor. */
static RxGCVertexDesc defaultVertexDesc;

/* The module record and the descriptor offsets in geometries and world sectors (uninitialized
 * data comes out in reverse order). */
static RwModuleInfo vertexDescModule;
s32 lbl_805F1524;
s32 lbl_805F1520;

/* Render callback of both instance nodes: loads the object's matrix (for a world sector built with
 * fixed-point positions, a translation that undoes their quantisation), describes the vertex
 * arrays, then calls each mesh's display list with its texture and material colours. */
void *fn_80462914(void *object, RxGCInstanceData *data) {
    RxGCInstanceHeader *header;
    RxGCInstanceMesh *displayLists;
    RwMatrix *matrix;
    RxGCVertexDesc *desc;
    RwMatrix local;
    RpGCColourCallBack colourCB;
    RpMesh *mesh;
    u32 numMeshes;

    header = (RxGCInstanceHeader *)(data->repEntry + 1);
    displayLists = (RxGCInstanceMesh *)((u8 *)header->arrays + header->numArrays * sizeof(RxGCInstanceArray));
    header->serialNum = lbl_805EDF40;
    if (((RwObject *)object)->type == 1) {
        RpAtomic *atomic = (RpAtomic *)object;
        matrix = RwFrameGetLTM((RwFrame *)atomic->object.object.parent);
        desc = *(RxGCVertexDesc **)((u8 *)atomic->geometry + lbl_805F1520);
    } else {
        RpWorldSector *sector = (RpWorldSector *)object;
        desc = *(RxGCVertexDesc **)((u8 *)RWSRCGLOBAL(curWorld) + lbl_805F1524);
        if (desc != NULL && desc->type[0] != 4) {
            f32 quantum;
            local.right.x = local.up.y = local.at.z = 1.0f;
            local.right.y = local.right.z = local.up.x = 0.0f;
            local.up.z = local.at.x = local.at.y = 0.0f;
            local.pos.x = local.pos.y = local.pos.z = 0.0f;
            local.flags |= 0x20003;
            quantum = 1.0f / (f32)(1 << desc->size[0]);
            local.pos.x = sector->tightBoundingBox.inf.x - (f32)fmod(sector->tightBoundingBox.inf.x, quantum);
            local.pos.y = sector->tightBoundingBox.inf.y - (f32)fmod(sector->tightBoundingBox.inf.y, quantum);
            local.pos.z = sector->tightBoundingBox.inf.z - (f32)fmod(sector->tightBoundingBox.inf.z, quantum);
            matrix = &local;
        } else {
            matrix = NULL;
        }
    }
    fn_80462C30(desc, data);
    if (data->flags & rpGEOMETRYNORMALS) {
        fn_804950AC(matrix, 1);
    } else {
        fn_804950AC(matrix, 0);
    }
    colourCB = fn_804655D4(data->flags, data->lightMask, data->ambientSet, header->unk4 & 1);
    numMeshes = data->mesh->numMeshes;
    mesh = (RpMesh *)(data->mesh + 1);
    if (data->flags & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2)) {
        while (numMeshes--) {
            RwTexture *texture = mesh->material->texture;
            fn_80499C38(texture, 0);
            if (texture != NULL && texture->raster != NULL) {
                fn_80497778((((u32 *)((u8 *)texture->raster->parent + lbl_805F1658))[5] & 1) ^ 1);
            } else {
                fn_80497778(1);
            }
            if (colourCB != NULL) {
                colourCB(&data->ambient, &mesh->material->color, mesh->material->surfaceProps.ambient);
            }
            GXCallDisplayList(displayLists->displayList, displayLists->size);
            displayLists++;
            mesh++;
        }
    } else {
        fn_80497778(1);
        while (numMeshes--) {
            if (colourCB != NULL) {
                colourCB(&data->ambient, &mesh->material->color, mesh->material->surfaceProps.ambient);
            }
            GXCallDisplayList(displayLists->displayList, displayLists->size);
            displayLists++;
            mesh++;
        }
    }
    return object;
}

void fn_80462BF0(RxPipelineNode *node, RxGCNodeCallBack callback) {
    node->privateData->instance = callback;
}

void fn_80462BFC(RxPipelineNode *node, RxGCNodeCallBack callback) {
    node->privateData->reinstance = callback;
}

void fn_80462C08(RxPipelineNode *node, RxGCNodeCallBack callback) {
    node->privateData->lighting = callback;
}

void fn_80462C14(RxPipelineNode *node, RxGCNodeCallBack callback) {
    node->privateData->render = callback;
}

void fn_80462C20(s32 value) {
    lbl_805EDDD8 = value;
}

s32 fn_80462C28(void) {
    return lbl_805EDDD8;
}

/* Describes the instance data's vertex arrays to GX (format 0): positions, normals (or normals
 * with binormals and tangents), the prelight colour and the texture coordinate sets. */
void fn_80462C30(RxGCVertexDesc *desc, RxGCInstanceData *data) {
    u32 arrayIndex;
    RxGCInstanceHeader *header;

    if (desc == NULL) {
        desc = &defaultVertexDesc;
    }
    header = (RxGCInstanceHeader *)(data->repEntry + 1);
    GXClearVtxDesc();
    arrayIndex = 0;
    GXSetVtxDesc(R3_GX_VA_POS, header->arrays[arrayIndex].attrType);
    GXSetVtxAttrFmt(R3_GX_VTXFMT0, R3_GX_VA_POS, R3_GX_POS_XYZ, desc->type[0], desc->size[0]);
    GXSetArray(R3_GX_VA_POS, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
    arrayIndex++;
    if (data->flags & rpGEOMETRYNORMALS) {
        if (desc->size[1] != 0) {
            GXSetVtxDesc(R3_GX_VA_NBT, header->arrays[arrayIndex].attrType);
            GXSetVtxAttrFmt(R3_GX_VTXFMT0, R3_GX_VA_NBT, R3_GX_NRM_NBT, desc->type[1], 0);
            GXSetArray(R3_GX_VA_NBT, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
            arrayIndex++;
        } else {
            GXSetVtxDesc(R3_GX_VA_NRM, header->arrays[arrayIndex].attrType);
            GXSetVtxAttrFmt(R3_GX_VTXFMT0, R3_GX_VA_NRM, R3_GX_NRM_XYZ, desc->type[1], 0);
            GXSetArray(R3_GX_VA_NRM, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
            arrayIndex++;
        }
    }
    if (data->flags & rpGEOMETRYPRELIT) {
        GXSetVtxDesc(R3_GX_VA_CLR0, header->arrays[arrayIndex].attrType);
        GXSetVtxAttrFmt(R3_GX_VTXFMT0, R3_GX_VA_CLR0, desc->type[10] > 2, desc->type[10], 0);
        GXSetArray(R3_GX_VA_CLR0, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
        arrayIndex++;
    }
    if (data->flags & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2)) {
        s32 attr = R3_GX_VA_TEX0;
        while (arrayIndex < header->numArrays) {
            GXSetVtxDesc(attr, header->arrays[arrayIndex].attrType);
            GXSetVtxAttrFmt(R3_GX_VTXFMT0, attr, R3_GX_TEX_ST, desc->type[attr - R3_GX_VA_TEX0 + 2],
                            desc->size[attr - R3_GX_VA_TEX0 + 2]);
            GXSetArray(attr, header->arrays[arrayIndex].data, header->arrays[arrayIndex].stride);
            arrayIndex++;
            attr++;
        }
    }
}

RxGCVertexDesc *fn_80462E1C(void) {
    return &defaultVertexDesc;
}

void *fn_80462E28(void *object, s32 offset) {
    *(RxGCVertexDesc **)((u8 *)object + offset) = NULL;
    return object;
}

void *fn_80462E34(void *object, s32 offset) {
    RxGCVertexDesc *desc = *(RxGCVertexDesc **)((u8 *)object + offset);
    if (desc != NULL) {
        fn_804630C0(desc);
    }
    return object;
}

void *fn_80462E70(void *instance) {
    vertexDescModule.numInstances++;
    if (vertexDescModule.numInstances == 1) {
        fn_80462FAC(&defaultVertexDesc);
    }
    return instance;
}

void *fn_80462EC0(void *instance) {
    vertexDescModule.numInstances--;
    return instance;
}

s32 fn_80462ED4(void) {
    if (fn_80485D78(0, 0x511, fn_80462E70, fn_80462EC0) < 0) {
        return 0;
    }
    lbl_805F1520 = fn_8045994C(sizeof(RxGCVertexDesc *), 0x511, fn_80462E28, fn_80462E34, NULL);
    if (lbl_805F1520 < 0) {
        return 0;
    }
    lbl_805F1524 = fn_80460790(sizeof(RxGCVertexDesc *), 0x511, fn_80462E28, fn_80462E34, NULL);
    if (lbl_805F1524 < 0) {
        return 0;
    }
    return 1;
}

void fn_80462F7C(RxGCVertexDesc *desc, u8 type, u8 size) {
    desc->type[0] = type;
    desc->size[0] = size;
}

void fn_80462F88(RxGCVertexDesc *desc, u8 type, u8 size) {
    desc->type[1] = type;
    desc->size[1] = size;
}

void fn_80462F94(RxGCVertexDesc *desc, s32 index, u8 type, u8 size) {
    desc->type[index + 1] = type;
    desc->size[index + 1] = size;
}

void fn_80462FA4(RxGCVertexDesc *desc, u8 type) {
    desc->type[10] = type;
}

void fn_80462FAC(RxGCVertexDesc *desc) {
    desc->type[0] = 4;
    desc->type[1] = 4;
    desc->type[2] = 4;
    desc->type[3] = 4;
    desc->type[4] = 4;
    desc->type[5] = 4;
    desc->type[6] = 4;
    desc->type[7] = 4;
    desc->type[8] = 4;
    desc->type[9] = 4;
    desc->type[10] = 5;
    desc->type[11] = 1;
    desc->size[0] = 0;
    desc->size[2] = 0;
    desc->size[3] = 0;
    desc->size[4] = 0;
    desc->size[5] = 0;
    desc->size[6] = 0;
    desc->size[7] = 0;
    desc->size[8] = 0;
    desc->size[9] = 0;
    desc->size[1] = 0;
    desc->refCount = 1;
}

RxGCVertexDesc *fn_8046301C(void) {
    RxGCVertexDesc *desc = (RxGCVertexDesc *)RwMalloc(sizeof(RxGCVertexDesc), 0x3050D);
    fn_80462FAC(desc);
    return desc;
}

void fn_804630C0(RxGCVertexDesc *desc) {
    if (desc->refCount == 1) {
        RwFree(desc);
    } else {
        desc->refCount--;
    }
}

void fn_80463104(RpGeometry *geometry, RxGCVertexDesc *desc) {
    s32 offset = lbl_805F1520;
    if (*(RxGCVertexDesc **)((u8 *)geometry + offset) != NULL) {
        fn_804630C0(*(RxGCVertexDesc **)((u8 *)geometry + offset));
    }
    *(RxGCVertexDesc **)((u8 *)geometry + offset) = desc;
    (*(RxGCVertexDesc **)((u8 *)geometry + offset))->refCount++;
}

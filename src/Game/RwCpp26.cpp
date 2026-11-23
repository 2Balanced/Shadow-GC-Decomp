/* RenderWare GameCube driver, 0x80462914-0x804643E8: the render callback and vertex descriptor
 * setup shared by the instance nodes, the vertex descriptor plugin (0x511) of geometries and world
 * sectors, the creation of the default pipelines, the atomic and world-sector "all in one" instance
 * nodes (instance, reinstance, lighting callbacks and node bodies), and the head of the lighting
 * code that RwCpp27 continues.
 *
 * Source files: retail calls (instead of inlining) fn_80462ED4 from fn_8046318C, fn_80462E1C from
 * fn_804635D4, the node callback setters from the pipeline builders, and each builder's node
 * definition getter, which it reaches before that getter's definition; and the literal pools of
 * .sdata2 repeat 1.0f and 0.0f four times (0x805FB180, 0x805FB190, 0x805FB1A0, 0x805FB1A8). So this
 * unit holds about five files: 0x80462914 (render callback, setters, vertex descriptors),
 * 0x8046318C (plugin attach, default pipelines), 0x8046335C (atomic node), 0x80463D28 (world-sector
 * node) and 0x804641CC (lighting, continued in RwCpp27). Until it is re-cut, #pragma dont_inline
 * stands in for the file boundaries.
 *
 * Status: every function has retail's instructions except fn_80463428 (5 rows: the two list
 * terminators swap saved registers). fn_80462914, fn_804635D4 and fn_80463DF4 need float literals
 * (and the int-to-float magic constant) that the compiler pools under its own @ names, so objdiff
 * stays below 100% for them until the unit owns its .sdata2 (0x805FB180-0x805FB1B8).
 *
 * Names are inferred from behaviour:
 *   fn_80462914  render callback (matrix, vertex descriptor, colours, display lists)
 *   fn_80462BF0..fn_80462C14  set one of a node's four callbacks
 *   fn_80462C30  describe the instance data's vertex arrays to GX
 *   fn_80462C20 / fn_80462C28  set / get a driver word (lbl_805EDDD8)
 *   fn_80462E1C  the default vertex descriptor
 *   fn_80462E28 / fn_80462E34  plugin constructor / destructor of the descriptor pointer
 *   fn_80462E70 / fn_80462EC0  module open / close    fn_80462ED4  plugin attach
 *   fn_80462F7C..fn_80462FA4  descriptor slot setters  fn_80462FAC  descriptor initialise
 *   fn_8046301C  descriptor create   fn_804630C0  descriptor destroy
 *   fn_80463104  give a geometry a descriptor
 *   fn_804631D8 / fn_80463244  create / destroy the default world-sector pipeline
 *   fn_80463298 / fn_80463308  create / destroy the default atomic pipeline
 *   fn_8046318C  attach the GameCube plugins (vertex descriptors, light extension)
 *   fn_8046335C / fn_80463D28  build an atomic / world-sector instance pipeline
 *   fn_80463428 / fn_80463DF4  lighting callback of the atomic / world-sector node
 *   fn_804635D4  repack a geometry's locked vertex arrays   fn_80463840  atomic reinstance
 *   fn_804639AC / fn_80463F10  instance callback of the atomic / world-sector node
 *   fn_80463A5C / fn_80463FA4  body of the atomic / world-sector node
 *   fn_80463CE0 / fn_80464188  node initialisation of the atomic / world-sector instance node
 *   fn_80463D1C / fn_804641C0  the two node definitions
 *   fn_804641CC  load the world's directional lights and sum its ambient lights
 * Built with cflags_rw_r3 (configure.py): C, functions in address order. */
#include <dolphin/types.h>
#include <dolphin/gx.h>
#include <dolphin/os.h>
#include "Game/rw/r3.h"

extern s32 lbl_805EDDD8;
extern RxGCVertexDesc lbl_805E1688; /* default vertex descriptor */
extern RxNodeDefinition lbl_8056D8CC; /* atomic instance node definition */
extern RxNodeDefinition lbl_8056D934; /* world-sector instance node definition */
extern s32 lbl_805F1520;            /* offset of the descriptor pointer in a geometry */
extern s32 lbl_805F1524;            /* offset of the descriptor pointer in a world sector */
extern RwModuleInfo lbl_805F1528;   /* module record */
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
extern volatile u16 lbl_805EDF40; /* draw-sync token of the instance data built last */
void fn_80462C30(RxGCVertexDesc *desc, RxGCInstanceData *data);
void fn_804950AC(RwMatrix *matrix, s32 normals); /* load the object's matrix */
void fn_80499C38(RwTexture *texture, s32 stage); /* set a texture on GX */
void fn_80497778(s32 enable);
double fmod(double x, double y);

/* Sets a material's colour registers: the ambient colour, the material colour and the material's
 * ambient coefficient (the colour setters of RwCpp27). */
typedef void (*RwCpp26ColourCallBack)(RwRGBAReal *ambient, RwRGBA *color, f32 scale);
RwCpp26ColourCallBack fn_804655D4(u32 flags, u32 lightMask, s32 ambientSet, u32 alpha);

/* Render callback of both instance nodes: loads the object's matrix (for a world sector built with
 * fixed-point positions, a translation that undoes their quantisation), describes the vertex
 * arrays, then calls each mesh's display list with its texture and material colours. */
void *fn_80462914(void *object, RxGCInstanceData *data) {
    RxGCInstanceHeader *header;
    RxGCInstanceMesh *displayLists;
    RwMatrix *matrix;
    RxGCVertexDesc *desc;
    RwMatrix local;
    RwCpp26ColourCallBack colourCB;
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
        desc = &lbl_805E1688;
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
    return &lbl_805E1688;
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
    lbl_805F1528.numInstances++;
    if (lbl_805F1528.numInstances == 1) {
        fn_80462FAC(&lbl_805E1688);
    }
    return instance;
}

void *fn_80462EC0(void *instance) {
    lbl_805F1528.numInstances--;
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

/* Retail calls fn_80462ED4 here instead of inlining it: this function probably starts another
 * source file (see the unit's notes), so inlining is switched off for it. */
#pragma dont_inline on
s32 fn_8046318C(void) {
    if (fn_80462ED4() == 0) {
        return 0;
    }
    return fn_804647C8() != 0;
}
#pragma dont_inline reset

s32 fn_804631CC(void) {
    return 1;
}

void fn_804631D4(void) {
}

s32 fn_804631D8(void) {
    void *pipeline = fn_80463D28(2, 0, fn_80463F10, NULL, fn_80463DF4, fn_80462914);
    if (pipeline != NULL) {
        RPWORLDGLOBALPIPE(0x58) = pipeline;
        fn_804626F8(pipeline);
        return 1;
    }
    return 0;
}

void fn_80463244(void) {
    fn_804626F8(NULL);
    if (RPWORLDGLOBALPIPE(0x58) != NULL) {
        fn_8049B800(RPWORLDGLOBALPIPE(0x58));
        RPWORLDGLOBALPIPE(0x58) = NULL;
    }
}

s32 fn_80463298(void) {
    void *pipeline = fn_8046335C(2, 0, fn_804639AC, fn_80463840, fn_80463428, fn_80462914);
    if (pipeline != NULL) {
        RPWORLDGLOBALPIPE(0x54) = pipeline;
        fn_80462734(pipeline);
        return 1;
    }
    return 0;
}


void fn_80463308(void) {
    fn_80462734(NULL);
    if (RPWORLDGLOBALPIPE(0x54) != NULL) {
        fn_8049B800(RPWORLDGLOBALPIPE(0x54));
        RPWORLDGLOBALPIPE(0x54) = NULL;
    }
}

/* Builds a pipeline of the atomic instance node with the given callbacks. */
#pragma dont_inline on
RxPipeline *fn_8046335C(u32 pluginId, u32 pluginData, RxGCNodeCallBack instance,
                        RxGCNodeCallBack reinstance, RxGCNodeCallBack lighting,
                        RxGCNodeCallBack render) {
    RxPipeline *pipeline;
    RxPipeline *lpipe;
    RxNodeDefinition *nodeDef;
    RxPipelineNode *node;

    pipeline = fn_8049B76C();
    pipeline->pluginId = pluginId;
    pipeline->pluginData = pluginData;
    lpipe = fn_8049CCB8(pipeline);
    nodeDef = fn_80463D1C();
    fn_8049C1E0(fn_8049D0D0(lpipe, NULL, nodeDef, NULL));
    node = fn_8049CFCC(pipeline, nodeDef->name, NULL, NULL);
    fn_80462BF0(node, instance);
    fn_80462BFC(node, reinstance);
    fn_80462C08(node, lighting);
    fn_80462C14(node, render);
    return pipeline;
}
#pragma dont_inline reset



/* Lighting callback of the atomic node: the world's ambient and directional lights, then every
 * light of the world sectors the atomic is in whose sphere reaches the atomic's (each light once
 * per frame); the ambient colour is clamped to 1 when one was set. */
void *fn_80463428(void *object, RxGCInstanceData *data) {

    RpAtomic *atomic = (RpAtomic *)object;
    u32 flags = atomic->geometry->flags;

    data->lightMask = 0;
    data->ambient.red = 0.0f;
    data->ambient.green = 0.0f;
    data->ambient.blue = 0.0f;
    data->ambient.alpha = 1.0f;
    data->ambientSet = 0;
    data->numLights = 0;
    if ((flags & rpGEOMETRYLIGHT) && RWSRCGLOBAL(curWorld) != NULL) {
        RwLLLink *end;
        RwLLLink *cur;

        fn_804641CC(1, data);
        RWSRCGLOBAL(lightFrame)++;
        cur = atomic->llWorldSectorsInAtomic.link.next;
        end = &atomic->llWorldSectorsInAtomic.link;
        while (cur != end) {
            RpWorldSector *sector;
            RwLLLink *end2;
            RwLLLink *cur2;

            sector = ((RpTie *)((u8 *)cur - (u32)&((RpTie *)0)->lWorldSectorInAtomic))->worldSector;
            cur2 = sector->lightsInWorldSector.link.next;
            end2 = &sector->lightsInWorldSector.link;
            while (cur2 != end2) {
                RpLight *light;

                light = ((RpLightTie *)cur2)->light;
                if (light != NULL && light->lightFrame != RWSRCGLOBAL(lightFrame) &&
                    (light->object.object.flags & rpLIGHTLIGHTATOMICS)) {
                    RwMatrix *ltm;
                    RwSphere *sphere;
                    RwV3d d;
                    f32 r;

                    light->lightFrame = RWSRCGLOBAL(lightFrame);
                    ltm = RwFrameGetLTM((RwFrame *)light->object.object.parent);
                    sphere = fn_804566D0(atomic);
                    d.x = sphere->center.x - ltm->pos.x;
                    d.y = sphere->center.y - ltm->pos.y;
                    d.z = sphere->center.z - ltm->pos.z;
                    r = sphere->radius + light->radius;
                    if (d.x * d.x + d.y * d.y + d.z * d.z < r * r) {
                        fn_804643E8(((RpLightTie *)cur2)->light, data);
                    }
                }
                cur2 = cur2->next;
            }
            cur = cur->next;
        }
    }
    if (data->ambientSet != 0) {
        if (data->ambient.red > 1.0f) {
            data->ambient.red = 1.0f;
        }
        if (data->ambient.green > 1.0f) {
            data->ambient.green = 1.0f;
        }
        if (data->ambient.blue > 1.0f) {
            data->ambient.blue = 1.0f;
        }
    }
    return object;
}


RwResEntry *fn_8046B79C(RpGeometry *geometry, void *owner, RwResEntry **repEntryRef);
RwResEntry *fn_8046C0D4(RpGeometry *geometry, void *owner, RwResEntry **repEntryRef);


s32 fn_8049A78C(u16 token); /* has the GPU passed this draw-sync token? */
void fn_804635D4(RpGeometry *geometry, RxGCInstanceHeader *header, RxGCNodeCallbacks *callbacks);
void fn_80464814(RpGeometry *geometry, RxGCInstanceHeader *header, RpInterpolator *interpolator);

/* Before instance data is rewritten the GPU must be done with it: a buffer still carrying the
 * current token gets a draw sync of its own, and the token moves on. */
static void RwCpp26WaitForGPU(RxGCInstanceHeader *header) {
    if (header->serialNum == lbl_805EDF40) {
        GXSetDrawSync(lbl_805EDF40);
        lbl_805EDF40 = (lbl_805EDF40 + 1) % 0xE000;
    }
    while (!fn_8049A78C(header->serialNum)) {
    }
}

u32 fn_80466B58(void *dst, const RwV3d *src, u8 type, s32 numVertices, u8 stride, f32 scale,
                s32 unk); /* pack positions */
u32 fn_80467BC8(void *dst, const RwV3d *src, u8 type, s32 numVertices, u8 stride);
u32 fn_804699F0(void *dst, const RwRGBA *src, u8 type, s32 numVertices, u8 stride);
u32 fn_8046A270(void *dst, const RwTexCoords *src, u8 type, s32 numVertices, u8 stride,
                f32 scale);

/* Repacks the vertex arrays of a geometry that were locked since it was last instanced, in the
 * formats of its vertex descriptor (fixed-point formats are scaled by 2^fraction bits). Retail
 * calls fn_80462E1C, which lives in another source file: no inlining here. */
#pragma dont_inline on
void fn_804635D4(RpGeometry *geometry, RxGCInstanceHeader *header, RxGCNodeCallbacks *callbacks) {
    u32 numVertices;
    u32 flags;
    s32 arrayIndex;
    u32 locked;
    RxGCVertexDesc *desc;
    u32 size;

    arrayIndex = 0;
    locked = geometry->lockedSinceLastInst;
    flags = geometry->flags | rpGEOMETRYPOSITIONS;
    desc = *(RxGCVertexDesc **)((u8 *)geometry + lbl_805F1520);
    numVertices = geometry->numVertices;
    if (desc == NULL) {
        desc = fn_80462E1C();
    }
    if (flags & rpGEOMETRYPOSITIONS) {
        if (locked & rpGEOMETRYLOCKVERTICES) {
            size = fn_80466B58(header->arrays[0].data, geometry->morphTarget->verts, desc->type[0],
                               geometry->numVertices, header->arrays[0].stride,
                               (f32)(1 << desc->size[0]), 0);
            DCFlushRange(header->arrays[0].data, size);
        }
        arrayIndex = 1;
    }
    if (flags & rpGEOMETRYNORMALS) {
        if (locked & rpGEOMETRYLOCKNORMALS) {
            size = fn_80467BC8(header->arrays[arrayIndex].data, geometry->morphTarget->normals,
                               desc->type[1], geometry->numVertices,
                               header->arrays[arrayIndex].stride);
            DCFlushRange(header->arrays[arrayIndex].data, size);
        }
        arrayIndex++;
    }
    if (flags & rpGEOMETRYPRELIT) {
        if (locked & rpGEOMETRYLOCKPRELIGHT) {
            if (*(RxGCVertexDesc **)((u8 *)geometry + lbl_805F1520) == NULL) {
                u32 i;
                header->unk4 &= ~1;
                for (i = 0; i < numVertices; i++) {
                    if (geometry->preLitLum[i].alpha < 0xFF) {
                        header->unk4 |= 1;
                        break;
                    }
                }
            } else if (desc->type[10] > 2) {
                header->unk4 |= 1;
            } else {
                header->unk4 &= ~1;
            }
            size = fn_804699F0(header->arrays[arrayIndex].data, geometry->preLitLum, desc->type[10],
                               geometry->numVertices, header->arrays[arrayIndex].stride);
            DCFlushRange(header->arrays[arrayIndex].data, size);
        }
        arrayIndex++;
    }
    if (flags & (rpGEOMETRYTEXTURED | rpGEOMETRYTEXTURED2)) {
        s32 i;
        for (i = 0; i < geometry->numTexCoordSets; i++) {
            if ((rpGEOMETRYLOCKTEXCOORDS1 << i) & locked) {
                size = fn_8046A270(header->arrays[arrayIndex].data, geometry->texCoords[i],
                                   desc->type[2 + i], geometry->numVertices,
                                   header->arrays[arrayIndex].stride, (f32)(1 << desc->size[2 + i]));
                DCFlushRange(header->arrays[arrayIndex].data, size);
            }
            arrayIndex++;
        }
    }
    GXInvalidateVtxCache();
}
#pragma dont_inline reset

/* Reinstance callback of the atomic node: rebuilds the vertex data of a locked geometry and
 * interpolates the morph targets of an atomic whose interpolator moved. */
void *fn_80463840(void *object, RxGCInstanceData *data) {
    RpAtomic *atomic = (RpAtomic *)object;
    RpGeometry *geometry = atomic->geometry;
    RxGCInstanceHeader *header = (RxGCInstanceHeader *)(data->repEntry + 1);

    if (geometry->lockedSinceLastInst) {
        RwCpp26WaitForGPU(header);
        fn_804635D4(geometry, header, data->callbacks);
        geometry->lockedSinceLastInst = 0;
    }
    if (geometry->numMorphTargets != 1 && (atomic->interpolator.flags & rpINTERPOLATORDIRTYINSTANCE)) {
        RwCpp26WaitForGPU(header);
        fn_80464814(geometry, header, &atomic->interpolator);
        atomic->interpolator.flags &= ~rpINTERPOLATORDIRTYINSTANCE;
    }
    return object;
}

/* Instance callback of the atomic node: a geometry with one morph target keeps the instance data
 * itself, otherwise the atomic does. */
void *fn_804639AC(void *object, RxGCInstanceData *data) {
    RpAtomic *atomic = (RpAtomic *)object;
    RpGeometry *geometry = atomic->geometry;
    void *owner;
    RwResEntry **repEntryRef;

    if (geometry->numMorphTargets != 1) {
        owner = atomic;
        repEntryRef = &atomic->repEntry;
    } else {
        owner = geometry;
        repEntryRef = &geometry->repEntry;
    }
    if (geometry->flags & rpGEOMETRYNATIVEINSTANCE) {
        if (lbl_805EDDD8 == 1) {
            data->repEntry = fn_8046B79C(geometry, owner, repEntryRef);
        } else {
            data->repEntry = fn_8046C0D4(geometry, owner, repEntryRef);
        }
    } else {
        data->repEntry = fn_8046C0D4(geometry, owner, repEntryRef);
    }
    geometry->lockedSinceLastInst = 0;
    return object;
}

extern s32 lbl_805F1578; /* offset of the resources module's globals in the engine block */
void fn_8048071C(RwResEntry *entry); /* RwResourcesFreeResEntry */

/* The resources module keeps its list of used entries behind a pointer in its globals. */
#define RWRESOURCESUSEDLIST() \
    (((RwLinkList **)((u8 *)lbl_805F15B4 + lbl_805F1578))[0x24 / sizeof(RwLinkList *)])

/* Body of the atomic instance node: (re)instances the atomic's geometry unless it is native,
 * then lights and renders it through the node's callbacks. */
s32 fn_80463A5C(RxPipelineNode *self, const RxPipelineNodeParam *params) {
    RpAtomic *atomic;
    RxGCNodeCallbacks *callbacks;
    RpGeometry *geometry;
    RxGCInstanceData data;

    atomic = (RpAtomic *)params->dataParam;
    callbacks = self->privateData;
    geometry = atomic->geometry;
    data.callbacks = callbacks;
    if (!(geometry->flags & rpGEOMETRYNATIVE)) {
        RpMeshHeader *mesh;

        /* The vertex count is compared unsigned here. */
        if ((u32)geometry->numVertices == 0) {
            return 1;
        }
        mesh = geometry->mesh;
        if (mesh->numMeshes == 0) {
            return 1;
        }
        data.repEntry = (geometry->numMorphTargets != 1) ? atomic->repEntry : geometry->repEntry;
        data.mesh = geometry->mesh;
        data.flags = geometry->flags;
        if (data.repEntry != NULL &&
            ((RxGCInstanceHeader *)(data.repEntry + 1))->meshSerialNum != mesh->serialNum) {
            fn_8048071C(data.repEntry);
            data.repEntry = NULL;
        }
        if (data.repEntry != NULL) {
            if (callbacks->reinstance != NULL && callbacks->reinstance(atomic, &data) != atomic) {
                return 0;
            }
            if (data.repEntry->link.next != NULL) {
                data.repEntry->link.prev->next = data.repEntry->link.next;
                data.repEntry->link.next->prev = data.repEntry->link.prev;
                data.repEntry->link.next = RWRESOURCESUSEDLIST()->link.next;
                data.repEntry->link.prev = &RWRESOURCESUSEDLIST()->link;
                RWRESOURCESUSEDLIST()->link.next->prev = &data.repEntry->link;
                RWRESOURCESUSEDLIST()->link.next = &data.repEntry->link;
            }
        } else {
            if (callbacks->instance != NULL && callbacks->instance(atomic, &data) != atomic) {
                return 0;
            }
            if (callbacks->reinstance != NULL && callbacks->reinstance(atomic, &data) != atomic) {
                return 0;
            }
        }
    } else {
        data.repEntry = geometry->repEntry;
        data.mesh = geometry->mesh;
        data.flags = geometry->flags;
        if (callbacks->reinstance != NULL && callbacks->reinstance(atomic, &data) != atomic) {
            return 0;
        }
    }
    if (callbacks->lighting != NULL && callbacks->lighting(atomic, &data) != atomic) {
        return 0;
    }
    if (callbacks->render != NULL && callbacks->render(atomic, &data) != atomic) {
        return 0;
    }
    return 1;
}

s32 fn_80463CE0(RxPipelineNode *node) {
    RxGCNodeCallbacks *callbacks = node->privateData;
    callbacks->instance = fn_804639AC;
    callbacks->reinstance = fn_80463840;
    callbacks->lighting = fn_80463428;
    callbacks->render = fn_80462914;
    return 1;
}

RxNodeDefinition *fn_80463D1C(void) {
    return &lbl_8056D8CC;
}

/* Builds a pipeline of the world-sector instance node with the given callbacks. */
#pragma dont_inline on
RxPipeline *fn_80463D28(u32 pluginId, u32 pluginData, RxGCNodeCallBack instance,
                        RxGCNodeCallBack reinstance, RxGCNodeCallBack lighting,
                        RxGCNodeCallBack render) {
    RxPipeline *pipeline;
    RxPipeline *lpipe;
    RxNodeDefinition *nodeDef;
    RxPipelineNode *node;

    pipeline = fn_8049B76C();
    pipeline->pluginId = pluginId;
    pipeline->pluginData = pluginData;
    lpipe = fn_8049CCB8(pipeline);
    nodeDef = fn_804641C0();
    fn_8049C1E0(fn_8049D0D0(lpipe, NULL, nodeDef, NULL));
    node = fn_8049CFCC(pipeline, nodeDef->name, NULL, NULL);
    fn_80462BF0(node, instance);
    fn_80462BFC(node, reinstance);
    fn_80462C08(node, lighting);
    fn_80462C14(node, render);
    return pipeline;
}
#pragma dont_inline reset

RwResEntry *fn_8046CB94(RpWorld *world, RpWorldSector *sector, void *owner, RwResEntry **repEntryRef);
RwResEntry *fn_8046D4C8(RpWorld *world, RpWorldSector *sector, void *owner, RwResEntry **repEntryRef);


/* Lighting callback of the world-sector node: the world's ambient lights, then the lights tied to
 * the sector; the ambient colour is clamped to 1 when one was set. */
void *fn_80463DF4(void *object, RxGCInstanceData *data) {
    RpWorldSector *sector = (RpWorldSector *)object;
    RwLLLink *link;
    RwLLLink *end;

    data->lightMask = 0;
    data->ambient.red = 0.0f;
    data->ambient.green = 0.0f;
    data->ambient.blue = 0.0f;
    data->ambient.alpha = 1.0f;
    data->ambientSet = 0;
    data->numLights = 0;
    if (RWSRCGLOBAL(curWorld)->flags & rpWORLDLIGHT) {
        fn_804641CC(2, data);
        RWSRCGLOBAL(lightFrame)++;
        end = &sector->lightsInWorldSector.link;
        for (link = sector->lightsInWorldSector.link.next; link != end; link = link->next) {
            RpLight *light = ((RpLightTie *)link)->light;
            if (light != NULL && (light->object.object.flags & rpLIGHTLIGHTWORLD)) {
                fn_804643E8(light, data);
            }
        }
    }
    if (data->ambientSet != 0) {
        if (data->ambient.red > 1.0f) {
            data->ambient.red = 1.0f;
        }
        if (data->ambient.green > 1.0f) {
            data->ambient.green = 1.0f;
        }
        if (data->ambient.blue > 1.0f) {
            data->ambient.blue = 1.0f;
        }
    }
    return object;
}

/* Instance callback of the world-sector node. */
void *fn_80463F10(void *object, RxGCInstanceData *data) {
    RpWorldSector *sector = (RpWorldSector *)object;
    RpWorld *world = RWSRCGLOBAL(curWorld);

    if (world->flags & rpGEOMETRYNATIVEINSTANCE) {
        if (lbl_805EDDD8 == 1) {
            data->repEntry = fn_8046CB94(world, sector, sector, &sector->repEntry);
        } else {
            data->repEntry = fn_8046D4C8(world, sector, sector, &sector->repEntry);
        }
    } else {
        data->repEntry = fn_8046D4C8(world, sector, sector, &sector->repEntry);
    }
    return object;
}

/* Body of the world-sector instance node: instances the sector unless the world is native, then
 * lights and renders it through the node's callbacks. */
s32 fn_80463FA4(RxPipelineNode *self, const RxPipelineNodeParam *params) {
    RpWorldSector *sector;
    RxGCNodeCallbacks *callbacks;
    u32 flags;
    RxGCInstanceData data;

    sector = (RpWorldSector *)params->dataParam;
    callbacks = self->privateData;
    data.callbacks = callbacks;
    flags = RWSRCGLOBAL(curWorld)->flags;
    if (!(flags & rpGEOMETRYNATIVE)) {
        RpMeshHeader *mesh;

        if (sector->numVertices == 0) {
            return 1;
        }
        mesh = sector->mesh;
        if (mesh->numMeshes == 0) {
            return 1;
        }
        data.repEntry = sector->repEntry;
        data.mesh = mesh;
        data.flags = flags;
        if (data.repEntry != NULL &&
            ((RxGCInstanceHeader *)(data.repEntry + 1))->meshSerialNum != mesh->serialNum) {
            fn_8048071C(data.repEntry);
            data.repEntry = NULL;
        }
        if (data.repEntry != NULL) {
            if (data.repEntry->link.next != NULL) {
                data.repEntry->link.prev->next = data.repEntry->link.next;
                data.repEntry->link.next->prev = data.repEntry->link.prev;
                data.repEntry->link.next = RWRESOURCESUSEDLIST()->link.next;
                data.repEntry->link.prev = &RWRESOURCESUSEDLIST()->link;
                RWRESOURCESUSEDLIST()->link.next->prev = &data.repEntry->link;
                RWRESOURCESUSEDLIST()->link.next = &data.repEntry->link;
            }
        } else {
            if (callbacks->instance != NULL && callbacks->instance(sector, &data) != sector) {
                return 0;
            }
        }
    } else {
        data.repEntry = sector->repEntry;
        data.mesh = sector->mesh;
        data.flags = flags;
    }
    if (callbacks->lighting != NULL && callbacks->lighting(sector, &data) != sector) {
        return 0;
    }
    if (callbacks->render != NULL && callbacks->render(sector, &data) != sector) {
        return 0;
    }
    return 1;
}

s32 fn_80464188(RxPipelineNode *node) {
    RxGCNodeCallbacks *callbacks = node->privateData;
    callbacks->instance = fn_80463F10;
    callbacks->reinstance = NULL;
    callbacks->lighting = fn_80463DF4;
    callbacks->render = fn_80462914;
    return 1;
}

RxNodeDefinition *fn_804641C0(void) {
    return &lbl_8056D934;
}

/* --- probably the head of the GameCube lighting file, which RwCpp27 continues --- */

extern s32 lbl_805F1530;           /* offset of the light extension in a light */
extern GXLightObj lbl_805E16A0[8]; /* the hardware lights being set up */
extern RwMatrix lbl_805E31CC;      /* the camera's view matrix */
RwV3d *fn_80482504(RwV3d *vectorOut, const RwV3d *vectorIn, const RwMatrix *matrix); /* RwV3dTransformVector */

extern const f32 lbl_805FB1B0;
extern const f32 lbl_805FB1B4;
extern const f32 lbl_805FB1A8;
extern const f32 lbl_805FB1AC;
static GXColor RwCpp26GXColor(const RwRGBAReal *c) {
    GXColor color;
    color.r = (u8)(lbl_805FB1B4 * c->red);
    color.g = (u8)(lbl_805FB1B4 * c->green);
    color.b = (u8)(lbl_805FB1B4 * c->blue);
    color.a = 0;
    return color;
}

/* Loads the world's ambient and directional lights that have one of the given flags: directional
 * lights go into the next free GX lights, placed far away against their direction; ambient
 * colours are summed. */
void fn_804641CC(u8 lightFlags, RxGCInstanceData *data) {
    s32 n;
    RwLLLink *cur;
    RwLLLink *end;
    RpWorld *world = RWSRCGLOBAL(curWorld);

    cur = world->directionalLightList.link.next;
    end = &world->directionalLightList.link;
    while (cur != end) {
        RpLight *light = (RpLight *)((u8 *)cur - (u32)&((RpLight *)0)->inWorld);
        if (light != NULL && (light->object.object.flags & lightFlags)) {
            if ((s32)light->object.object.subType == rpLIGHTDIRECTIONAL) {
                RwMatrix *ltm;
                RwV3d dir;
                RpGCLightExt *ext;
                GXLightObj *lightObj;

                n = data->numLights;
                ltm = RwFrameGetLTM((RwFrame *)light->object.object.parent);
                fn_80482504(&dir, &ltm->at, &lbl_805E31CC);
                ext = (RpGCLightExt *)((u8 *)light + lbl_805F1530);
                if (ext->useAttn == 0) {
                    GXInitLightAttn(&lbl_805E16A0[n], lbl_805FB1A8, lbl_805FB1AC, lbl_805FB1AC, lbl_805FB1A8, lbl_805FB1AC, lbl_805FB1AC);
                } else {
                    GXInitLightAttn(&lbl_805E16A0[n], ext->a0, ext->a1, ext->a2, ext->k0, ext->k1,
                                    ext->k2);
                }
                lightObj = &lbl_805E16A0[n];
                GXInitLightPos(lightObj, lbl_805FB1B0 * -dir.x, lbl_805FB1B0 * dir.y,
                               lbl_805FB1B0 * -dir.z);
                GXInitLightColor(lightObj, RwCpp26GXColor(&light->color));
                GXLoadLightObjImm(lightObj, 1 << n);
                data->lightMask |= 1 << data->numLights;
                data->numLights++;
            } else {
                data->ambient.red += light->color.red;
                data->ambient.green += light->color.green;
                data->ambient.blue += light->color.blue;
                data->ambientSet = 1;
            }
        }
        cur = cur->next;
    }
}

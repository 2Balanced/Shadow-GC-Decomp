/* RenderWare 3.7 world plugin, GameCube: the atomic "all in one" instance node, 0x8046335C-0x80463D28
 * (wave 41 re-cut of the size-cut Game/RwCpp26). The pipeline builder, the lighting, reinstance and
 * instance callbacks, the node body and initialisation, and the node definition getter. The file
 * owns the node definition and its name (.data 0x8056D8B0-0x8056D90C) and its literal pool (.sdata2
 * 0x805FB190-0x805FB1A0: 0.0f, 1.0f and the int-to-float constant).
 *   fn_8046335C  build an atomic instance pipeline   fn_80463428  lighting callback
 *   fn_804635D4  repack a geometry's locked vertex arrays   fn_80463840  reinstance callback
 *   fn_804639AC  instance callback   fn_80463A5C  node body   fn_80463CE0  node initialisation
 *   fn_80463D1C  the node definition
 * Built with cflags_rw_r3 (configure.py).
 * Matching: the lighting callback reaches each list's first link through its terminator (end,
 * then end->next), with the inner terminator declared first.
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
extern s32 lbl_805EDDD8; /* driver word (render file) */
/* The node definition and its name. */
static char atomicAllInOneName[] = "GameCubeAtomicAllInOne.csl";

/* Defined in other files. */
RxGCVertexDesc *fn_80462E1C(void);

/* Builds a pipeline of the atomic instance node with the given callbacks. */
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
        RwLLLink *end2;
        RwLLLink *cur;
        RwLLLink *end;
        RwLLLink *cur2;

        fn_804641CC(1, data);
        RWSRCGLOBAL(lightFrame)++;
        end = &atomic->llWorldSectorsInAtomic.link;
        cur = end->next;
        while (cur != end) {
            RpWorldSector *sector;

            sector = ((RpTie *)((u8 *)cur - (u32)&((RpTie *)0)->lWorldSectorInAtomic))->worldSector;
            end2 = &sector->lightsInWorldSector.link;
            cur2 = end2->next;
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
static inline void GCInstanceWaitForGPU(RxGCInstanceHeader *header) {
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

/* Reinstance callback of the atomic node: rebuilds the vertex data of a locked geometry and
 * interpolates the morph targets of an atomic whose interpolator moved. */
void *fn_80463840(void *object, RxGCInstanceData *data) {
    RpAtomic *atomic = (RpAtomic *)object;
    RpGeometry *geometry = atomic->geometry;
    RxGCInstanceHeader *header = (RxGCInstanceHeader *)(data->repEntry + 1);

    if (geometry->lockedSinceLastInst) {
        GCInstanceWaitForGPU(header);
        fn_804635D4(geometry, header, data->callbacks);
        geometry->lockedSinceLastInst = 0;
    }
    if (geometry->numMorphTargets != 1 && (atomic->interpolator.flags & rpINTERPOLATORDIRTYINSTANCE)) {
        GCInstanceWaitForGPU(header);
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

/* The node definition (0x10 bytes of private data per pipeline node: its callbacks). */
static RxNodeDefinition atomicAllInOneNode = {
    atomicAllInOneName,
    { (void *)fn_80463A5C, NULL, NULL, (void *)fn_80463CE0, NULL, NULL, NULL },
    { 0, NULL, NULL, 0, NULL },
    sizeof(RxGCNodeCallbacks),
    0,
    0
};

RxNodeDefinition *fn_80463D1C(void) {
    return &atomicAllInOneNode;
}

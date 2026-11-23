/* RenderWare 3.7 world plugin, GameCube: the world-sector "all in one" instance node,
 * 0x80463D28-0x804641CC (wave 41 re-cut of the size-cut Game/RwCpp26): the twin of the atomic node
 * file. The file owns the node definition and its name (.data 0x8056D910-0x8056D974; retail spells
 * it "GamerCube") and its literal pool (.sdata2 0x805FB1A0-0x805FB1A8: 0.0f, 1.0f).
 *   fn_80463D28  build a world-sector instance pipeline   fn_80463DF4  lighting callback
 *   fn_80463F10  instance callback   fn_80463FA4  node body   fn_80464188  node initialisation
 *   fn_804641C0  the node definition
 * Built with cflags_rw_r3 (configure.py).
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
extern s32 lbl_805F1524; /* offset of the descriptor pointer in a world sector (render file) */
extern s32 lbl_805EDDD8; /* driver word (render file) */
/* The node definition and its name. */
static char sectorAllInOneName[] = "GamerCubeWorldSectorAllInOne.csl";

extern s32 lbl_805F1578; /* offset of the resources module's globals in the engine block */
void fn_8048071C(RwResEntry *entry); /* RwResourcesFreeResEntry */

/* The resources module keeps its list of used entries behind a pointer in its globals. */
#define RWRESOURCESUSEDLIST()     (((RwLinkList **)((u8 *)lbl_805F15B4 + lbl_805F1578))[0x24 / sizeof(RwLinkList *)])

/* Builds a pipeline of the world-sector instance node with the given callbacks. */
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

/* The node definition (0x10 bytes of private data per pipeline node: its callbacks). */
static RxNodeDefinition sectorAllInOneNode = {
    sectorAllInOneName,
    { (void *)fn_80463FA4, NULL, NULL, (void *)fn_80464188, NULL, NULL, NULL },
    { 0, NULL, NULL, 0, NULL },
    sizeof(RxGCNodeCallbacks),
    0,
    0
};

RxNodeDefinition *fn_804641C0(void) {
    return &sectorAllInOneNode;
}

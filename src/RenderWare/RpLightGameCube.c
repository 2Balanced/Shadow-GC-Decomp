/* RenderWare 3.7 world plugin, GameCube: lights, 0x804641CC-0x80464814 (wave 41 re-cut of the
 * size-cut Game/RwCpp26 tail and Game/RwCpp27 head). Collects the lights of an object into the
 * eight GX lights (ambient, directional, point and spot lights in view space) and keeps the light
 * extension (plugin 0x505 of lights: GX attenuation coefficients). The file owns the GX light
 * objects (.bss 0x805E16A0), the extension offset (.sbss 0x805F1530) and its literal pool (.sdata2
 * 0x805FB1A8-0x805FB1C0).
 *   fn_804641CC  load the lights of an object   fn_804643E8  load one light into the next GX light
 *   fn_804647B0 / fn_804647C0 / fn_804647C4  light extension constructor / destructor / copy
 *   fn_804647C8  attach the light extension (28 bytes)
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

/* The GX lights being set up, and the offset of the light extension in a light. */
static GXLightObj lightObjs[8];
static s32 lightExtOffset;

extern s32 lightExtOffset;           /* offset of the light extension in a light */
extern GXLightObj lightObjs[8]; /* the hardware lights being set up */
extern RwMatrix lbl_805E31CC;      /* the camera's view matrix */
RwV3d *fn_80482504(RwV3d *vectorOut, const RwV3d *vectorIn, const RwMatrix *matrix); /* RwV3dTransformVector */

static inline GXColor GCLightColor(const RwRGBAReal *c) {
    GXColor color;
    color.r = (u8)(255.0f * c->red);
    color.g = (u8)(255.0f * c->green);
    color.b = (u8)(255.0f * c->blue);
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
                ext = (RpGCLightExt *)((u8 *)light + lightExtOffset);
                if (ext->useAttn == 0) {
                    GXInitLightAttn(&lightObjs[n], 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
                } else {
                    GXInitLightAttn(&lightObjs[n], ext->a0, ext->a1, ext->a2, ext->k0, ext->k1,
                                    ext->k2);
                }
                lightObj = &lightObjs[n];
                GXInitLightPos(lightObj, -1048576.0f * -dir.x, -1048576.0f * dir.y,
                               -1048576.0f * -dir.z);
                GXInitLightColor(lightObj, GCLightColor(&light->color));
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

/* Loads a light into the next free GX light (up to eight) in view space. */
void fn_804643E8(RpLight *light, RxGCInstanceData *state) {
    GXColor color;
    RwV3d dir;
    RwV3d pos;
    RwMatrix *ltm;
    RpGCLightExt *ext;

    if (state->numLights < 8) {
        color.r = (u8)(255.0f * light->color.red);
        color.g = (u8)(255.0f * light->color.green);
        color.b = (u8)(255.0f * light->color.blue);
        color.a = 0;
        GXInitLightColor(&lightObjs[state->numLights], color);
        ltm = RwFrameGetLTM((RwFrame *)light->object.object.parent);
        fn_80482458(&pos, &ltm->pos, &lbl_805E31CC);
        GXInitLightPos(&lightObjs[state->numLights], -pos.x, pos.y, -pos.z);
        ext = (RpGCLightExt *)((u8 *)light + lightExtOffset);
        switch (light->object.object.subType) {
        case rpLIGHTDIRECTIONAL:
        case rpLIGHTAMBIENT:
            break;
        case rpLIGHTPOINT:
            if (ext->useAttn == 0) {
                GXInitLightAttnA(&lightObjs[state->numLights], 1.0f, 0.0f,
                                 0.0f);
                GXInitLightDistAttn(&lightObjs[state->numLights], 0.5f * light->radius,
                                    0.5f, 2);
            } else {
                GXInitLightAttn(&lightObjs[state->numLights], ext->a0, ext->a1, ext->a2,
                                ext->k0, ext->k1, ext->k2);
            }
            break;
        case rpLIGHTSPOT:
            fn_80482504(&dir, &ltm->at, &lbl_805E31CC);
            GXInitLightDir(&lightObjs[state->numLights], -dir.x, dir.y, -dir.z);
            if (ext->useAttn == 0) {
                GXInitLightSpot(&lightObjs[state->numLights], 57.29578f * fn_8045A818(light), 1);
                GXInitLightDistAttn(&lightObjs[state->numLights], 0.5f * light->radius,
                                    0.5f, 2);
            } else {
                GXInitLightAttn(&lightObjs[state->numLights], ext->a0, ext->a1, ext->a2,
                                ext->k0, ext->k1, ext->k2);
            }
            break;
        case rpLIGHTSPOTSOFT:
            fn_80482504(&dir, &ltm->at, &lbl_805E31CC);
            GXInitLightDir(&lightObjs[state->numLights], -dir.x, dir.y, -dir.z);
            if (ext->useAttn == 0) {
                GXInitLightSpot(&lightObjs[state->numLights], 57.29578f * fn_8045A818(light), 2);
                GXInitLightDistAttn(&lightObjs[state->numLights], 0.5f * light->radius,
                                    0.5f, 2);
            } else {
                GXInitLightAttn(&lightObjs[state->numLights], ext->a0, ext->a1, ext->a2,
                                ext->k0, ext->k1, ext->k2);
            }
            break;
        }
        GXLoadLightObjImm(&lightObjs[state->numLights], 1 << state->numLights);
        state->lightMask |= 1 << state->numLights;
        state->numLights++;
    }
}

void *fn_804647B0(void *object) {
    *(u32 *)((u8 *)object + lightExtOffset) = 0;
    return object;
}

void *fn_804647C0(void *object) {
    return object;
}

void *fn_804647C4(void *dstObject) {
    return dstObject;
}

s32 fn_804647C8(void) {
    lightExtOffset = fn_8045AAA0(0x1C, 0x505, fn_804647B0, fn_804647C0, fn_804647C4);
    return lightExtOffset >= 0;
}

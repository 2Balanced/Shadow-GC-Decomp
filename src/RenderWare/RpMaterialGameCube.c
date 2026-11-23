/* RenderWare 3.7 world plugin, GameCube: material colours, 0x8046528C-0x80465CDC (wave 41 re-cut of
 * the size-cut Game/RwCpp27). The colour setters that scale an ambient colour and load it (with the
 * material colour) into a TEV register or the channel colour registers, and the channel/TEV setup
 * that picks one for a mesh. The file owns its literal pool (.sdata2 0x805FB1D8-0x805FB1F0: two
 * colours, the int-to-float constant and 255.0f).
 *   fn_8046528C .. fn_8046554C  colour setters   fn_804655D4  set up the channels, return a setter
 * Built with cflags_rw_r3 (configure.py).
 * Matching: the two colours are file-scope constants, so they open the literal pool.
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

/* The colours the channel setup loads: they open the literal pool. */
static const GXColor opaqueWhite = { 255, 255, 255, 255 };
static const GXColor opaqueBlack = { 0, 0, 0, 255 };

void fn_8046528C(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    amb.r = (u8)(color->red * (ambient->red * scale));
    amb.g = (u8)(color->green * (ambient->green * scale));
    amb.b = (u8)(color->blue * (ambient->blue * scale));
    amb.a = 0;
    GXSetTevColor(1, *(GXColor *)color);
    GXSetTevColor(2, amb);
}

void fn_80465370(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    f32 s = 255.0f * scale;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
    amb.a = 0;
    GXSetTevColor(2, amb);
}

void fn_804653F8(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    f32 s = 255.0f * scale;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
    amb.a = 0;
    GXSetChanAmbColor(0, amb);
}

void fn_80465480(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    f32 s = 255.0f * scale;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
    amb.a = 0;
    GXSetChanMatColor(4, *(GXColor *)color);
    GXSetChanAmbColor(0, amb);
}

void fn_8046551C(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXSetChanMatColor(4, *(GXColor *)color);
}

void fn_8046554C(RwRGBAReal *ambient, f32 scale, RwRGBA *color) {
    GXColor amb;
    f32 s = 255.0f * scale;
    amb.r = (u8)(ambient->red * s);
    amb.g = (u8)(ambient->green * s);
    amb.b = (u8)(ambient->blue * s);
    amb.a = 0;
    GXSetChanMatColor(0, amb);
}

typedef void (*RpGCColourCallBack)(RwRGBAReal *ambient, f32 scale, RwRGBA *color);

/* Sets up the colour channels and the TEV stages for a mesh with the given geometry flags, lights
 * and prelight alpha, and returns the callback that loads each material's colours (or NULL).
 * Flag 0x40 is rpGEOMETRYMODULATEMATERIALCOLOR, 0x08 prelit, 0x84 textured. */
RpGCColourCallBack fn_804655D4(u32 flags, u32 lightMask, s32 ambientSet, s32 alpha) {
    u32 textured;
    RpGCColourCallBack callback;
    GXBool colorEnable;
    GXBool alphaEnable;
    GXColorSrc colorMatSrc;
    GXColorSrc alphaMatSrc;
    GXColorSrc colorAmbSrc;
    GXColorSrc alphaAmbSrc;
    u8 numStages;
    u32 prelit;

    textured = flags & 0x84;
    callback = NULL;
    if (textured) {
        prelit = flags & 0x08;
        if (prelit && ambientSet == 1) {
            if (lightMask) {
                if (flags & 0x40) {
                    callback = fn_8046528C;
                } else {
                    GXSetTevColor(1, opaqueWhite);
                    callback = fn_80465370;
                }
                colorMatSrc = 0;
                alphaMatSrc = 0;
                GXSetChanMatColor(4, opaqueWhite);
                colorEnable = 1;
                colorAmbSrc = 1;
                if (alpha == 1) {
                    alphaAmbSrc = 1;
                    alphaEnable = 1;
                } else {
                    alphaAmbSrc = 0;
                    GXSetChanAmbColor(2, opaqueBlack);
                    alphaEnable = 0;
                }
            } else {
                if (flags & 0x40) {
                    callback = fn_8046528C;
                } else {
                    GXSetTevColor(1, opaqueWhite);
                    callback = fn_80465370;
                }
                colorAmbSrc = 0;
                alphaAmbSrc = 0;
                colorMatSrc = 1;
                if (alpha == 1) {
                    alphaMatSrc = 1;
                } else {
                    alphaMatSrc = 0;
                    GXSetChanMatColor(2, opaqueBlack);
                }
                colorEnable = 0;
                alphaEnable = 0;
            }
            numStages = 2;
            GXSetTevColorIn(0, 15, 10, 2, 4);
            GXSetTevAlphaIn(0, 7, 5, 1, 2);
        } else {
            if (lightMask) {
                if (flags & 0x40) {
                    colorMatSrc = 0;
                    alphaMatSrc = 0;
                    colorEnable = 1;
                    if (prelit) {
                        colorAmbSrc = 1;
                        if (alpha == 1) {
                            alphaAmbSrc = 1;
                            alphaEnable = 1;
                        } else {
                            alphaAmbSrc = 0;
                            GXSetChanAmbColor(2, opaqueBlack);
                            alphaEnable = 0;
                        }
                        callback = fn_8046551C;
                    } else {
                        colorAmbSrc = 0;
                        alphaAmbSrc = 0;
                        alphaEnable = 0;
                        GXSetChanAmbColor(2, opaqueBlack);
                        callback = fn_80465480;
                    }
                } else {
                    colorMatSrc = 0;
                    alphaMatSrc = 0;
                    GXSetChanMatColor(4, opaqueWhite);
                    colorEnable = 1;
                    if (prelit) {
                        colorAmbSrc = 1;
                        if (alpha == 1) {
                            alphaAmbSrc = 1;
                            alphaEnable = 1;
                        } else {
                            alphaAmbSrc = 0;
                            alphaEnable = 0;
                        }
                    } else {
                        callback = fn_804653F8;
                        colorAmbSrc = 0;
                        alphaAmbSrc = 0;
                        alphaEnable = 0;
                    }
                }
            } else {
                if (flags & 0x40) {
                    colorMatSrc = 0;
                    alphaMatSrc = 0;
                    colorEnable = 1;
                    if (prelit) {
                        colorAmbSrc = 1;
                        if (alpha == 1) {
                            alphaAmbSrc = 1;
                            alphaEnable = 1;
                        } else {
                            alphaAmbSrc = 0;
                            GXSetChanAmbColor(2, opaqueBlack);
                            alphaEnable = 0;
                        }
                        callback = fn_8046551C;
                    } else {
                        colorAmbSrc = 0;
                        alphaAmbSrc = 0;
                        alphaEnable = 0;
                        GXSetChanAmbColor(2, opaqueBlack);
                        callback = fn_80465480;
                    }
                } else {
                    if (prelit) {
                        colorMatSrc = 1;
                        if (alpha == 1) {
                            alphaMatSrc = 1;
                        } else {
                            alphaMatSrc = 0;
                            GXSetChanMatColor(2, opaqueBlack);
                        }
                    } else {
                        colorMatSrc = 0;
                        alphaMatSrc = 0;
                        GXSetChanMatColor(2, opaqueBlack);
                        callback = fn_8046554C;
                    }
                    colorEnable = 0;
                    alphaEnable = 0;
                    colorAmbSrc = 0;
                    alphaAmbSrc = 0;
                }
            }
            numStages = 1;
            GXSetTevColorIn(0, 15, 10, 8, 15);
            GXSetTevAlphaIn(0, 7, 5, 4, 7);
        }
    } else {
        if (lightMask) {
            colorMatSrc = 0;
            alphaMatSrc = 0;
            GXSetChanMatColor(4, opaqueWhite);
            colorEnable = 1;
            if (flags & 0x08) {
                colorAmbSrc = 1;
                if (alpha == 1) {
                    alphaAmbSrc = 1;
                    alphaEnable = 1;
                } else {
                    alphaAmbSrc = 0;
                    alphaEnable = 0;
                }
            } else {
                colorAmbSrc = 0;
                alphaAmbSrc = 0;
                alphaEnable = 0;
                GXSetChanAmbColor(4, opaqueBlack);
            }
        } else {
            if (flags & 0x08) {
                colorMatSrc = 1;
                if (alpha == 1) {
                    alphaMatSrc = 1;
                } else {
                    alphaMatSrc = 0;
                    GXSetChanMatColor(2, opaqueBlack);
                }
            } else {
                colorMatSrc = 0;
                alphaMatSrc = 0;
                GXSetChanMatColor(4, opaqueBlack);
            }
            colorAmbSrc = 0;
            alphaAmbSrc = 0;
            colorEnable = 0;
            alphaEnable = 0;
        }
        numStages = 1;
        if (flags & 0x40) {
            callback = fn_8046528C;
        } else {
            GXSetTevColor(1, opaqueWhite);
            callback = fn_80465370;
        }
        GXSetTevColorIn(0, 15, 10, 2, 4);
        GXSetTevAlphaIn(0, 7, 5, 1, 2);
    }
    GXSetNumTevStages(numStages);
    GXSetTevColorOp(0, 0, 0, 0, 1, 0);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    if (numStages > 1) {
        GXSetTevColorIn(1, 15, 0, 8, 15);
        GXSetTevColorOp(1, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(1, 7, 0, 4, 7);
        GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
        GXSetTevOrder(1, 0, 0, 0xFF);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    } else if (textured) {
        GXSetTevOrder(0, 0, 0, 4);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(0, 1, 4, 60, 0, 125);
    } else {
        GXSetNumTexGens(0);
        GXSetTevOrder(0, 0xFF, 0xFF, 4);
    }
    GXSetNumChans(1);
    GXSetChanCtrl(0, colorEnable, colorAmbSrc, colorMatSrc, lightMask, 2, 1);
    GXSetChanCtrl(2, alphaEnable, alphaAmbSrc, alphaMatSrc, 0, 0, 2);
    GXSetChanCtrl(1, 0, 0, 0, 0, 0, 2);
    GXSetChanCtrl(3, 0, 0, 0, 0, 0, 2);
    return callback;
}

/* RenderWare material effects plugin (RpMatFX), GameCube part, 0x804A2C90..0x804A55CC: the
 * channel colour set-up shared by the per-mesh render callbacks, the dual pass, dual pass with UV
 * transform, environment map and bump map callbacks, the per-mesh dispatcher, the atomic and world
 * sector instance and render callbacks, the creation of the plugin's two pipelines and the small
 * hooks the generic part (RpMatFX.c) calls.  RpSkin's GameCube code calls fn_804A2F08 and reads
 * the default vertex format lbl_805F16B8, which therefore keeps its address name.
 * Data: .bss 0x805E32C8 (what the callbacks have already set for the atomic being drawn), .sbss
 * 0x805F16B8..0x805F16C8 (the default vertex format and the two pipelines), .sdata2
 * 0x805FB450..0x805FB488 (white and black, then the literals of the functions).
 * Built with cflags_rw_r5 like RpMatFX.c.  The render helpers are plain static functions that the
 * compiler expands: their float literals are pooled where the helpers are defined, which is what
 * puts -0.5f/0.5f (environment map matrix) ahead of 0.0f (UV transform) in .sdata2, and the small
 * uninitialised data comes out in reverse order of definition.  Functions keep their fn_ symbols.
 * Status: every written function matches except fn_804A3DB0 (bump map, 6 rows: retail keeps the
 * two reloads of the material data in r29 and r26, GC/2.7 in r24; the matrix buffers belong to the
 * callers, which is what gives retail's stack layout).  Not written yet: fn_804A457C (0xADC bytes,
 * the per-mesh dispatcher). */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

/* The plugin's two pipelines and its default vertex format (RpSkin's GameCube code reads that
 * too); small uninitialised data comes out in reverse order of definition. */
static RxPipeline *MatFXWorldSectorPipeline;
static RxPipeline *MatFXAtomicPipeline;
RwGameCubeVertexFormat *lbl_805F16B8;

/* What the render callbacks have set for the atomic being drawn. */
static MatFXRenderState MatFXGameCubeState;

static const GXColor MatFXWhite = {255, 255, 255, 255};
static const GXColor MatFXBlack = {0, 0, 0, 255};

extern u16 lbl_805EDF40;                     /* the driver's render serial number */

double fmod(double x, double y);

typedef void *(*RwGameCubeNodeCallBack)(void *object, RwGameCubeRenderNode *node);

/* GameCube world pipeline (other units) */
RwGameCubeVertexFormat *fn_8046301C(void);                               /* vertex format create */
void fn_80462F88(RwGameCubeVertexFormat *format, s32 a, s32 b);
void fn_804630C0(RwGameCubeVertexFormat *format);                        /* vertex format destroy */
void fn_80463104(RpGeometry *geometry, RwGameCubeVertexFormat *format);  /* attach to geometry */
s32 fn_80462C28(void);
void fn_80462C20(s32 mode);
void fn_80462C30(RwGameCubeVertexFormat *format, RwGameCubeRenderNode *node);
void fn_804950AC(RwMatrix *ltm, s32 normals);
RpAtomic *fn_804639AC(RpAtomic *atomic, RwGameCubeRenderNode *node);     /* atomic instance */
RpAtomic *fn_80463840(RpAtomic *atomic, RwGameCubeRenderNode *node);     /* atomic reinstance */
void *fn_80463428(void *object, RwGameCubeRenderNode *node);
RpWorldSector *fn_80463F10(RpWorldSector *sector, RwGameCubeRenderNode *node); /* sector instance */
void *fn_80463DF4(void *object, RwGameCubeRenderNode *node);
RxPipeline *fn_8046335C(u32 pluginId, s32 type, RwGameCubeNodeCallBack instance,
                        RwGameCubeNodeCallBack reinstance, RwGameCubeNodeCallBack lighting,
                        RwGameCubeNodeCallBack render); /* atomic pipeline create */
RxPipeline *fn_80463D28(u32 pluginId, s32 type, RwGameCubeNodeCallBack instance,
                        RwGameCubeNodeCallBack reinstance, RwGameCubeNodeCallBack lighting,
                        RwGameCubeNodeCallBack render); /* world sector pipeline create */

/* RwCpp69 */
void fn_804A8120(RwGameCubeRenderNode *node, RwGameCubeVertexFormat *format, s32 numVertices);
s32 fn_804A8478(RwGameCubeRenderNode *node);

void fn_804A3DB0(RwGameCubeMesh *mesh, RwGameCubeDisplayList *list, RwMatrix *objectMatrix,
                 RwGameCubeLightState *state, s32 envMap);
void GXSetTevKAlphaSel(s32 stage, s32 sel);
void GXSetTevColorIn(s32 stage, s32 a, s32 b, s32 c, s32 d);
void GXSetTevAlphaIn(s32 stage, s32 a, s32 b, s32 c, s32 d);
void GXSetTevColorOp(s32 stage, s32 op, s32 bias, s32 scale, u8 clamp, s32 outReg);
void GXSetTevAlphaOp(s32 stage, s32 op, s32 bias, s32 scale, u8 clamp, s32 outReg);
void GXSetTevIndirect(s32 stage, s32 indStage, s32 format, s32 biasSel, s32 matrixSel, s32 wrapS,
                      s32 wrapT, u8 addPrev, u8 utcLod, s32 alphaSel);
void GXSetTevDirect(s32 stage);
void GXSetTevKColor(s32 id, GXColor color);
RwTexture *fn_804A1A08(RwTexture *baseTexture, RwTexture *heightTexture);
void fn_804A457C(void *mesh, RwGameCubeDisplayList *list, void *object, RwMatrix *ltm,
                 RwGameCubeRenderNode *node);

/* ---- GameCube render callbacks ---- */

/* Sets the channel colours of a material for the lighting set-up in `state`. */
void fn_804A2C90(RwGameCubeLightState *state, RpMaterial *material) {
    GXColor color;

    if (state->numLights > 0) {
        if (state->ambientLit && !(state->flags & 8)) {
            f32 scale = 255.0f * material->surfaceProps.ambient;
            color.r = (u8)(scale * state->ambient.red);
            color.g = (u8)(scale * state->ambient.green);
            color.b = (u8)(scale * state->ambient.blue);
            color.a = 0xFF;
            fn_80396A6C(4, color);
        }
        if (state->flags & 0x40) {
            fn_80396B54(4, *(GXColor *)&material->color);
        }
    } else if (state->flags & 8) {
        if (state->flags & 0x40) {
            fn_80396B54(4, *(GXColor *)&material->color);
        }
    } else if (state->ambientLit) {
        f32 ambient = material->surfaceProps.ambient;
        if (state->flags & 0x40) {
            color.r = (u8)(material->color.red * (ambient * state->ambient.red));
            color.g = (u8)(material->color.green * (ambient * state->ambient.green));
            color.b = (u8)(material->color.blue * (ambient * state->ambient.blue));
            color.a = material->color.alpha;
        } else {
            ambient *= 255.0f;
            color.r = (u8)(ambient * state->ambient.red);
            color.g = (u8)(ambient * state->ambient.green);
            color.b = (u8)(ambient * state->ambient.blue);
            color.a = 0xFF;
        }
        fn_80396B54(4, color);
    } else if (state->flags & 0x40) {
        color.r = color.g = color.b = 0;
        color.a = material->color.alpha;
        fn_80396B54(4, color);
    }
}

/* Forgets what the callbacks below have set, so the next mesh sets everything again. */
void fn_804A2F08(void) {
    MatFXGameCubeState.tevSet = 0;
    MatFXGameCubeState.lightingSet = 0;
    MatFXGameCubeState.material = NULL;
}

static void MatFXSetLighting(const RwGameCubeLightState *state) {
    if (!MatFXGameCubeState.lightingSet) {
        u32 flags = state->flags;
        if (state->numLights > 0) {
            s32 ambSrc;
            if (flags & 8) {
                ambSrc = 1;
            } else {
                ambSrc = 0;
                if (!state->ambientLit) {
                    fn_80396A6C(4, MatFXBlack);
                }
            }
            fn_80396C3C(1);
            fn_80396C78(4, 1, ambSrc, 0, state->lightMask, 2, 1);
            if (!(flags & 0x40)) {
                fn_80396B54(4, MatFXWhite);
            }
        } else if (flags & 8) {
            if (flags & 0x40) {
                fn_80396C3C(1);
                fn_80396C78(4, 1, 1, 0, 0, 0, 2);
            } else {
                fn_80396C3C(1);
                fn_80396C78(4, 0, 0, 1, 0, 0, 2);
            }
        } else {
            if (!state->ambientLit && !(flags & 0x40)) {
                fn_80396B54(4, MatFXBlack);
            }
            fn_80396C3C(1);
            fn_80396C78(4, 0, 0, 0, 0, 0, 2);
        }
        MatFXGameCubeState.lightingSet = 1;
    }
}

#define MatFXSetMaterial(state, mesh) \
    do { \
        if (MatFXGameCubeState.material != (mesh)->material) { \
            fn_804A2C90((state), (mesh)->material); \
            MatFXGameCubeState.material = (mesh)->material; \
        } \
    } while (0)

static void MatFXSetTev(const RwGameCubeLightState *state) {
    if (!MatFXGameCubeState.tevSet) {
        fn_803989A0(1);
        if (state->flags & 0x84) {
            fn_803951BC(1);
            fn_80394F3C(0, 1, 4, 0x3C, 0, 0x7D);
            fn_80398804(0, 0, 0, 4);
            fn_803982A8(0, 0);
        } else {
            fn_803951BC(0);
            fn_80398804(0, 0xFF, 0xFF, 4);
            fn_803982A8(0, 4);
        }
        MatFXGameCubeState.tevSet = 1;
    }
}

static void MatFXSetTexture(RwTexture *texture) {
    fn_80499C38(texture, 0);
    if (texture != NULL && texture->raster != NULL) {
        fn_80497778((RWGAMECUBERASTEREXT(texture->raster->parent)->flags & 1) ^ 1);
    } else {
        fn_80497778(1);
    }
}

/* Loads the texture matrix that turns normals into environment map coordinates. */
static void MatFXLoadEnvMapMatrix(f32 mtx[3][4], RwMatrix *objectMatrix, RwFrame *frame, u32 id) {
    RwMatrix objectFrameMatrix;
    RwMatrix frameMatrix;
    RwMatrix *matrix;

    if (frame != NULL) {
        fn_8047F0B0(&frameMatrix, RwFrameGetLTM(frame));
        fn_8047ECB4(&frameMatrix, &frameMatrix);
        matrix = &frameMatrix;
    } else {
        matrix = &lbl_805E31CC;
    }
    if (objectMatrix != NULL) {
        fn_8047EBB8(&objectFrameMatrix, objectMatrix, matrix);
        fn_8047ECB4(&objectFrameMatrix, &objectFrameMatrix);
        matrix = &objectFrameMatrix;
    }
    mtx[0][0] = -0.5f * matrix->right.x;
    mtx[0][1] = -0.5f * matrix->up.x;
    mtx[0][2] = -0.5f * matrix->at.x;
    mtx[0][3] = 0.5f;
    mtx[1][0] = -0.5f * matrix->right.y;
    mtx[1][1] = -0.5f * matrix->up.y;
    mtx[1][2] = -0.5f * matrix->at.y;
    mtx[1][3] = 0.5f;
    fn_803992BC(mtx, id, 1);
}

static void MatFXLoadBumpMatrix(f32 mtx[3][4], RwMatrix *objectMatrix, RwFrame *frame, f32 scale) {
    RwMatrix objectFrameMatrix;
    RwMatrix frameMatrix;
    RwMatrix *matrix;

    if (frame != NULL) {
        fn_8047F0B0(&frameMatrix, RwFrameGetLTM(frame));
        fn_8047ECB4(&frameMatrix, &frameMatrix);
        matrix = &frameMatrix;
    } else {
        matrix = &lbl_805E31CC;
    }
    if (objectMatrix != NULL) {
        fn_8047EBB8(&objectFrameMatrix, objectMatrix, matrix);
        fn_8047ECB4(&objectFrameMatrix, &objectFrameMatrix);
        matrix = &objectFrameMatrix;
    }
    mtx[0][0] = scale * matrix->right.x;
    mtx[0][1] = scale * matrix->up.x;
    mtx[0][2] = scale * matrix->at.x;
    mtx[0][3] = 0.0f;
    mtx[1][0] = scale * matrix->right.y;
    mtx[1][1] = scale * matrix->up.y;
    mtx[1][2] = scale * matrix->at.y;
    mtx[1][3] = 0.0f;
    fn_803992BC(mtx, 0x1E, 1);
}

static void MatFXLoadUVTransform(f32 mtx[3][4], const RwMatrix *matrix, s32 texGenSrc) {
    mtx[0][0] = matrix->right.x;
    mtx[0][1] = matrix->up.x;
    mtx[0][2] = 0.0f;
    mtx[0][3] = matrix->pos.x;
    mtx[1][0] = matrix->right.y;
    mtx[1][1] = matrix->up.y;
    mtx[1][2] = 0.0f;
    mtx[1][3] = matrix->pos.y;
    fn_803992BC(mtx, 0x1E, 1);
    fn_80394F3C(0, 1, texGenSrc, 0x1E, 0, 0x7D);
}

/* Dual pass: the mesh with its own texture, then again with the second texture and blend modes. */
void fn_804A2F24(RwGameCubeMesh *mesh, RwGameCubeDisplayList *list, RwGameCubeLightState *state) {
    MatFXMaterialData *data = MATFXMATERIALGETDATA(mesh->material);
    s32 srcBlend;
    s32 dstBlend;

    MatFXSetLighting(state);
    MatFXSetMaterial(state, mesh);
    MatFXSetTev(state);
    MatFXSetTexture(mesh->material->texture);
    fn_80399000(list->data, list->size);
    RwEngineInstance->renderStateGet(10, &srcBlend);
    RwEngineInstance->renderStateGet(11, &dstBlend);
    RwEngineInstance->renderStateSet(10, (void *)data->data[0].u.dual.srcBlendMode);
    RwEngineInstance->renderStateSet(11, (void *)data->data[0].u.dual.dstBlendMode);
    if (state->flags & 0x80) {
        fn_80394F3C(0, 1, 5, 0x3C, 0, 0x7D);
        MatFXGameCubeState.tevSet = 0;
    }
    MatFXSetTexture(data->data[0].u.dual.texture);
    fn_80399000(list->data, list->size);
    RwEngineInstance->renderStateSet(10, (void *)srcBlend);
    RwEngineInstance->renderStateSet(11, (void *)dstBlend);
}

/* Dual pass with UV transforms: slot 0 holds the two matrices, slot 1 the second texture. */
void fn_804A3308(RwGameCubeMesh *mesh, RwGameCubeDisplayList *list, RwGameCubeLightState *state) {
    MatFXMaterialData *data = MATFXMATERIALGETDATA(mesh->material);
    s32 srcBlend;
    s32 dstBlend;
    f32 mtx[3][4];

    MatFXSetLighting(state);
    MatFXSetMaterial(state, mesh);
    MatFXSetTev(state);
    if (data->data[0].u.uv.baseTransform != NULL) {
        MatFXLoadUVTransform(mtx, data->data[0].u.uv.baseTransform, 4);
    }
    MatFXSetTexture(mesh->material->texture);
    fn_80399000(list->data, list->size);
    if (state->flags & 0x80) {
        if (data->data[0].u.uv.dualTransform != NULL) {
            MatFXLoadUVTransform(mtx, data->data[0].u.uv.dualTransform, 5);
            MatFXGameCubeState.tevSet = 0;
        } else {
            fn_80394F3C(0, 1, 5, 0x3C, 0, 0x7D);
            MatFXGameCubeState.tevSet = 0;
        }
    } else {
        if (data->data[0].u.uv.dualTransform != NULL) {
            MatFXLoadUVTransform(mtx, data->data[0].u.uv.dualTransform, 4);
            MatFXGameCubeState.tevSet = 0;
        } else if (data->data[0].u.uv.baseTransform != NULL) {
            fn_80394F3C(0, 1, 4, 0x3C, 0, 0x7D);
        }
    }
    RwEngineInstance->renderStateGet(10, &srcBlend);
    RwEngineInstance->renderStateGet(11, &dstBlend);
    RwEngineInstance->renderStateSet(10, (void *)data->data[1].u.dual.srcBlendMode);
    RwEngineInstance->renderStateSet(11, (void *)data->data[1].u.dual.dstBlendMode);
    MatFXSetTexture(data->data[1].u.dual.texture);
    fn_80399000(list->data, list->size);
    RwEngineInstance->renderStateSet(10, (void *)srcBlend);
    RwEngineInstance->renderStateSet(11, (void *)dstBlend);
}

/* Environment map: the mesh untextured, then the environment texture added over it with texture
 * coordinates generated from the normals in the environment frame's space. */
void fn_804A388C(RwGameCubeMesh *mesh, RwGameCubeDisplayList *list, RwMatrix *objectMatrix,
                 RwGameCubeLightState *state) {
    MatFXMaterialData *data = MATFXMATERIALGETDATA(mesh->material);
    s32 srcBlend;
    s32 dstBlend;
    GXColor color = {255, 255, 255, 255};
    f32 mtx[3][4];

    MatFXSetLighting(state);
    MatFXSetMaterial(state, mesh);
    MatFXSetTev(state);
    fn_80499C38(mesh->material->texture, 0);
    fn_80497778(1);
    fn_80399000(list->data, list->size);
    RwEngineInstance->renderStateGet(10, &srcBlend);
    RwEngineInstance->renderStateGet(11, &dstBlend);
    RwEngineInstance->renderStateSet(10, (void *)(data->data[0].u.env.useFrameBufferAlpha ? 7 : 2));
    RwEngineInstance->renderStateSet(11, (void *)2);
    fn_80396C78(4, 0, 0, 0, 0, 2, 2);
    fn_80396C78(5, 0, 0, 0, 0, 2, 2);
    if (data->data[0].u.env.coef < 1.0f) {
        color.r = color.g = color.b = (u8)(255.9f * data->data[0].u.env.coef);
        color.a = 0xFF;
    } else {
        color.r = color.g = color.b = color.a = 0xFF;
    }
    fn_80396B54(4, color);
    fn_803951BC(1);
    fn_80394F3C(0, 1, 1, 0x1E, 0, 0x7D);
    fn_803989A0(1);
    fn_803982A8(0, 0);
    fn_80398804(0, 0, 0, 4);
    MatFXGameCubeState.material = NULL;
    MatFXGameCubeState.lightingSet = 0;
    MatFXGameCubeState.tevSet = 0;
    fn_80499C38(data->data[0].u.env.texture, 0);
    MatFXLoadEnvMapMatrix(mtx, objectMatrix, data->data[0].u.env.frame, 0x1E);
    fn_80399000(list->data, list->size);
    RwEngineInstance->renderStateSet(10, (void *)srcBlend);
    RwEngineInstance->renderStateSet(11, (void *)dstBlend);
}

/* Bump map, with the environment map added when `envMap` is set: the bump texture's texture
 * coordinates are offset along the normals (texture matrix 0x1E, scaled by the bump coefficient
 * over the bump texture's width) and the TEV takes the difference of the two lookups. */
void fn_804A3DB0(RwGameCubeMesh *mesh, RwGameCubeDisplayList *list, RwMatrix *objectMatrix,
                 RwGameCubeLightState *state, s32 envMap) {
    MatFXMaterialData *data = MATFXMATERIALGETDATA(mesh->material);
    f32 bumpMtx[3][4];
    f32 envMtx[3][4];
    s32 lastStage;

    MatFXSetLighting(state);
    MatFXSetMaterial(state, mesh);
    MatFXGameCubeState.tevSet = 0;
    fn_80499C38(data->data[0].u.bump.bumpTexture, 0);
    fn_80497778(1);
    MatFXLoadBumpMatrix(bumpMtx, objectMatrix, data->data[0].u.bump.frame,
                        -data->data[0].u.bump.invBumpWidth * data->data[0].u.bump.coef);
    if (envMap) {
        MatFXMaterialData *envData = MATFXMATERIALGETDATA(mesh->material);
        fn_80499C38(envData->data[1].u.env.texture, 1);
        MatFXLoadEnvMapMatrix(envMtx, objectMatrix, envData->data[1].u.env.frame, 0x21);
        fn_803951BC(3);
        fn_80394F3C(0, 1, 4, 0x3C, 0, 0x7D);
        fn_80394F3C(1, 1, 1, 0x1E, 0, 0x7D);
        fn_80394F3C(2, 1, 1, 0x21, 0, 0x7D);
    } else {
        fn_803951BC(2);
        fn_80394F3C(0, 1, 4, 0x3C, 0, 0x7D);
        fn_80394F3C(1, 1, 1, 0x1E, 0, 0x7D);
    }
    fn_80398804(0, 0, 0, 4);
    GXSetTevKAlphaSel(0, 0);
    GXSetTevColorIn(0, 0xF, 8, 0xA, 0xF);
    GXSetTevColorOp(0, 0, 0, 0, 1, 1);
    GXSetTevAlphaIn(0, 6, 7, 4, 7);
    GXSetTevAlphaOp(0, 0, 0, 0, 1, 0);
    fn_80398804(1, 1, 0, 0xFF);
    GXSetTevIndirect(1, 0, 0, 0, 0, 0, 0, 1, 0, 0);
    GXSetTevColorIn(1, 0xF, 1, 2, 0xF);
    GXSetTevColorOp(1, 0, 0, 0, 1, 0);
    GXSetTevAlphaIn(1, 4, 7, 7, 7);
    GXSetTevAlphaOp(1, 0, 0, 0, 1, 0);
    if (envMap) {
        MatFXMaterialData *envData = MATFXMATERIALGETDATA(mesh->material);
        GXColor color;
        if (envData->data[1].u.env.coef < 1.0f) {
            color.a = (u8)(255.9f * envData->data[1].u.env.coef);
        } else {
            color.a = 0xFF;
        }
        GXSetTevKColor(0, color);
        fn_80398804(2, 0xFF, 0xFF, 0xFF);
        GXSetTevKAlphaSel(2, 0x1C);
        GXSetTevColorIn(2, 0xF, 1, 2, 0);
        GXSetTevColorOp(2, 0, 0, 0, 1, 0);
        if (envData->data[1].u.env.useFrameBufferAlpha) {
            GXSetTevAlphaIn(2, 7, 6, 0, 7);
        } else {
            GXSetTevAlphaIn(2, 6, 7, 7, 7);
        }
        GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
        lastStage = 3;
        fn_80398804(3, 2, 1, 4);
        GXSetTevColorIn(3, 0xF, 1, 8, 0);
        GXSetTevColorOp(3, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(3, 5, 7, 7, 7);
        GXSetTevAlphaOp(3, 0, 0, 0, 1, 0);
    } else {
        lastStage = 2;
        fn_80398804(2, 0xFF, 0xFF, 4);
        GXSetTevColorIn(2, 0xF, 1, 2, 0);
        GXSetTevColorOp(2, 0, 0, 0, 1, 0);
        GXSetTevAlphaIn(2, 5, 7, 7, 7);
        GXSetTevAlphaOp(2, 0, 0, 0, 1, 0);
    }
    fn_803989A0(lastStage + 1);
    fn_80399000(list->data, list->size);
    GXSetTevDirect(1);
}

/* fn_804A457C (0xADC bytes, the per-mesh dispatcher) is not decompiled yet. */

/* Atomic instance callback: instances with the generic code and adds the plugin's own streams. */
RpAtomic *fn_804A5058(RpAtomic *atomic, RwGameCubeRenderNode *node) {
    if (fn_804A8478(node)) {
        RpGeometry *geometry = atomic->geometry;
        RwGameCubeVertexFormat *format =
            *(RwGameCubeVertexFormat **)((u8 *)geometry + lbl_805F1520);
        s32 mode;

        if (format == NULL) {
            fn_80463104(geometry, lbl_805F16B8);
            format = lbl_805F16B8;
        }
        mode = fn_80462C28();
        if (mode == 1) {
            fn_80462C20(0);
        }
        atomic = fn_804639AC(atomic, node);
        if (mode == 1) {
            fn_80462C20(1);
        }
        if (atomic != NULL) {
            fn_804A8120(node, format, geometry->numVertices);
        }
        return atomic;
    }
    return fn_804639AC(atomic, node);
}

/* Atomic reinstance callback. */
RpAtomic *fn_804A5124(RpAtomic *atomic, RwGameCubeRenderNode *node) {
    RpGeometry *geometry = atomic->geometry;
    RwGameCubeVertexFormat *format;
    s32 rebuild = 0;

    format = *(RwGameCubeVertexFormat **)((u8 *)geometry + lbl_805F1520);
    if (format != NULL && format->skinned && (geometry->flags & 0x10)) {
        u32 locked = geometry->lockedSinceLastInst & 0x17;
        u32 morphing = (geometry->numMorphTargets != 1) && (atomic->interpolatorFlags & 1);
        rebuild = (locked || morphing);
    }
    if (!fn_80463840(atomic, node)) return NULL;
    if (rebuild) {
        fn_804A8120(node, format, geometry->numVertices);
    }
    return atomic;
}

/* World sector instance callback. */
RpWorldSector *fn_804A5204(RpWorldSector *sector, RwGameCubeRenderNode *node) {
    RwGameCubeVertexFormat *format;

    if (!fn_80463F10(sector, node)) return NULL;
    format = *(RwGameCubeVertexFormat **)((u8 *)RwEngineInstance->curWorld + lbl_805F1524);
    if (format != NULL && format->skinned) {
        fn_804A8120(node, format, sector->numVertices);
    }
    return sector;
}

/* Render callback of both pipelines: sets the matrix and draws every mesh with its effect. */
void *fn_804A5280(void *object, RwGameCubeRenderNode *node) {
    RwGameCubeInstance *instance = (RwGameCubeInstance *)(node->resEntry + 0x18);
    RwGameCubeDisplayList *list;
    s32 numMeshes;
    u8 *mesh;
    RwGameCubeVertexFormat *format;
    RwMatrix *ltm;
    RwMatrix matrix;

    list = (RwGameCubeDisplayList *)&instance->streams[1] + (instance->numStreams - 1);
    instance->serial = lbl_805EDF40;
    if (((RpAtomic *)object)->type == 1) {
        ltm = RwFrameGetLTM(((RpAtomic *)object)->frame);
        format = *(RwGameCubeVertexFormat **)((u8 *)((RpAtomic *)object)->geometry + lbl_805F1520);
    } else {
        format = *(RwGameCubeVertexFormat **)((u8 *)RwEngineInstance->curWorld + lbl_805F1524);
        if (format != NULL && format->positionType != 4) {
            RpWorldSector *sector = (RpWorldSector *)object;
            f32 cell;
            matrix.right.x = matrix.up.y = matrix.at.z = 1.0f;
            matrix.right.y = matrix.right.z = matrix.up.x = 0.0f;
            matrix.up.z = matrix.at.x = matrix.at.y = 0.0f;
            matrix.pos.x = matrix.pos.y = matrix.pos.z = 0.0f;
            matrix.flags |= 0x20003;
            cell = 1.0f / (f32)(1 << format->positionShift);
            matrix.pos.x = sector->origin.x - (f32)fmod(sector->origin.x, cell);
            matrix.pos.y = sector->origin.y - (f32)fmod(sector->origin.y, cell);
            matrix.pos.z = sector->origin.z - (f32)fmod(sector->origin.z, cell);
            ltm = &matrix;
        } else {
            ltm = NULL;
        }
    }
    fn_80462C30(format, node);
    if (node->flags & 0x10) {
        fn_804950AC(ltm, 1);
    } else {
        fn_804950AC(ltm, 0);
    }
    MatFXGameCubeState.tevSet = 0;
    MatFXGameCubeState.lightingSet = 0;
    MatFXGameCubeState.material = NULL;
    numMeshes = node->meshHeader->numMeshes;
    mesh = (u8 *)node->meshHeader + 0x10;
    while (numMeshes-- != 0) {
        fn_804A457C(mesh, list, object, ltm, node);
        list++;
        mesh += 0xC;
    }
    return object;
}

/* Creates the default vertex format and the two pipelines (called from the engine open). */
s32 fn_804A5494(void) {
    lbl_805F16B8 = fn_8046301C();
    fn_80462F88(lbl_805F16B8, 4, 1);
    MatFXAtomicPipeline = fn_8046335C(0x120, 1, (RwGameCubeNodeCallBack)fn_804A5058,
                               (RwGameCubeNodeCallBack)fn_804A5124, fn_80463428, fn_804A5280);
    MatFXWorldSectorPipeline = fn_80463D28(0x120, 2, (RwGameCubeNodeCallBack)fn_804A5204, NULL, fn_80463DF4,
                               fn_804A5280);
    return 1;
}

/* Destroys them (called from the engine close). */
s32 fn_804A5524(void) {
    if (lbl_805F16B8 != NULL) {
        fn_804630C0(lbl_805F16B8);
        lbl_805F16B8 = NULL;
    }
    if (MatFXAtomicPipeline != NULL) {
        fn_8049B800(MatFXAtomicPipeline);
        MatFXAtomicPipeline = NULL;
    }
    if (MatFXWorldSectorPipeline != NULL) {
        fn_8049B800(MatFXWorldSectorPipeline);
        MatFXWorldSectorPipeline = NULL;
    }
    return 1;
}

/* Gives an atomic the plugin's pipeline (RpMatFXAtomicEnableEffects). */
RpAtomic *fn_804A558C(RpAtomic *atomic) {
    atomic->pipeline = MatFXAtomicPipeline;
    return atomic;
}

/* Gives a world sector the plugin's pipeline (RpMatFXWorldSectorEnableEffects). */
RpWorldSector *fn_804A5598(RpWorldSector *sector) {
    sector->pipeline = MatFXWorldSectorPipeline;
    return sector;
}

/* Blend mode check of the dual pass effect: every mode is supported here. */
s32 fn_804A55A4(MatFXEffectData *effect, s32 renderState) {
    return 1;
}

/* Builds the bump texture of a material (the generic part's platform hook). */
RwTexture *fn_804A55AC(RwTexture *baseTexture, RwTexture *heightTexture) {
    return fn_804A1A08(baseTexture, heightTexture);
}

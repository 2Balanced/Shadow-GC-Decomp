/* RenderWare material effects plugin (rpmatfx), 0x804A29A8..0x804A3DB0: the last accessors of the
 * plugin's main file (environment map coefficient, dual pass, UV transform) and, from 0x804A2C90,
 * the start of its GameCube render file: the channel colour set-up shared by the per-mesh render
 * callbacks and the dual pass, dual pass with UV transform and environment map callbacks (the
 * bump map ones follow in RwCpp67).  Functions keep their fn_ symbols; comments give the public
 * API name where the behaviour fits.
 * This cut has no .sdata2 split, so float constants that are spelled as literals reference the
 * compiler's @N instead of retail's lbl_; where the extern spelling keeps the instructions they
 * are named (lbl_805FB470/478/47C).  fn_804A2C90 (int to float constant) and fn_804A388C (-0.5f
 * and 0.5f: naming them swaps f1/f2) are instruction-identical with those relocations left. */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

#ifdef __cplusplus
extern "C" {
#endif
extern const GXColor lbl_805FB450; /* opaque white */
extern const GXColor lbl_805FB454; /* black */
extern const GXColor lbl_805FB474; /* the environment pass's initial colour */
/* Float constants of this file's .sdata2, which no split of this cut covers yet. */
extern const f32 lbl_805FB470; /* 0.0f */
extern const f32 lbl_805FB478; /* 1.0f */
extern const f32 lbl_805FB47C; /* 255.9f */

void fn_804A55A4(MatFXEffectData *effect, s32 renderState);

/* RpMatFXMaterialGetEnvMapCoefficient */
f32 fn_804A29A8(const RpMaterial *material) {
    return MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTENVMAP)->u.env.coef;
}

/* RpMatFXMaterialSetDualTexture */
RpMaterial *fn_804A29F0(RpMaterial *material, RwTexture *texture) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTDUAL);
    texture->refCount++;
    if (effect->u.dual.texture != NULL) {
        fn_8048CD8C(effect->u.dual.texture);
        effect->u.dual.texture = NULL;
    }
    effect->u.dual.texture = texture;
    fn_804A55A4(effect, 2);
    fn_804A55A4(effect, 1);
    return material;
}

/* RpMatFXMaterialSetDualBlendModes */
RpMaterial *fn_804A2AB0(RpMaterial *material, s32 srcBlendMode, s32 dstBlendMode) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTDUAL);
    effect->u.dual.srcBlendMode = srcBlendMode;
    effect->u.dual.dstBlendMode = dstBlendMode;
    fn_804A55A4(effect, 10);
    fn_804A55A4(effect, 11);
    return material;
}

/* RpMatFXMaterialGetDualTexture */
RwTexture *fn_804A2B44(const RpMaterial *material) {
    return MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTDUAL)->u.dual.texture;
}

/* RpMatFXMaterialGetDualBlendModes */
RpMaterial *fn_804A2B8C(RpMaterial *material, s32 *srcBlendMode, s32 *dstBlendMode) {
    MatFXEffectData *effect = MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTDUAL);
    *srcBlendMode = effect->u.dual.srcBlendMode;
    *dstBlendMode = effect->u.dual.dstBlendMode;
    return material;
}

/* RpMatFXMaterialSetUVTransformMatrices */
RpMaterial *fn_804A2BE0(RpMaterial *material, RwMatrix *baseTransform, RwMatrix *dualTransform) {
    MatFXEffectData *effect =
        MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTUVTRANSFORM);
    effect->u.uv.baseTransform = baseTransform;
    effect->u.uv.dualTransform = dualTransform;
    return material;
}

/* RpMatFXMaterialGetUVTransformMatrices */
RpMaterial *fn_804A2C2C(RpMaterial *material, RwMatrix **baseTransform, RwMatrix **dualTransform) {
    MatFXEffectData *effect =
        MatFXFindEffect(MATFXMATERIALGETDATA(material), rpMATFXEFFECTUVTRANSFORM);
    if (baseTransform != NULL) *baseTransform = effect->u.uv.baseTransform;
    if (dualTransform != NULL) *dualTransform = effect->u.uv.dualTransform;
    return material;
}

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
    lbl_805E32C8.tevSet = 0;
    lbl_805E32C8.lightingSet = 0;
    lbl_805E32C8.material = NULL;
}

static inline void MatFXSetLighting(const RwGameCubeLightState *state) {
    if (!lbl_805E32C8.lightingSet) {
        u32 flags = state->flags;
        if (state->numLights > 0) {
            s32 ambSrc;
            if (flags & 8) {
                ambSrc = 1;
            } else {
                ambSrc = 0;
                if (!state->ambientLit) {
                    fn_80396A6C(4, lbl_805FB454);
                }
            }
            fn_80396C3C(1);
            fn_80396C78(4, 1, ambSrc, 0, state->lightMask, 2, 1);
            if (!(flags & 0x40)) {
                fn_80396B54(4, lbl_805FB450);
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
                fn_80396B54(4, lbl_805FB454);
            }
            fn_80396C3C(1);
            fn_80396C78(4, 0, 0, 0, 0, 0, 2);
        }
        lbl_805E32C8.lightingSet = 1;
    }
}

#define MatFXSetMaterial(state, mesh) \
    do { \
        if (lbl_805E32C8.material != (mesh)->material) { \
            fn_804A2C90((state), (mesh)->material); \
            lbl_805E32C8.material = (mesh)->material; \
        } \
    } while (0)

static inline void MatFXSetTev(const RwGameCubeLightState *state) {
    if (!lbl_805E32C8.tevSet) {
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
        lbl_805E32C8.tevSet = 1;
    }
}

static inline void MatFXSetTexture(RwTexture *texture) {
    fn_80499C38(texture, 0);
    if (texture != NULL && texture->raster != NULL) {
        fn_80497778((RWGAMECUBERASTEREXT(texture->raster->parent)->flags & 1) ^ 1);
    } else {
        fn_80497778(1);
    }
}

static inline void MatFXLoadUVTransform(f32 mtx[3][4], const RwMatrix *matrix, s32 texGenSrc) {
    mtx[0][0] = matrix->right.x;
    mtx[0][1] = matrix->up.x;
    mtx[0][2] = lbl_805FB470;
    mtx[0][3] = matrix->pos.x;
    mtx[1][0] = matrix->right.y;
    mtx[1][1] = matrix->up.y;
    mtx[1][2] = lbl_805FB470;
    mtx[1][3] = matrix->pos.y;
    fn_803992BC(mtx, 0x1E, 1);
    fn_80394F3C(0, 1, texGenSrc, 0x1E, 0, 0x7D);
}

/* Loads the texture matrix that turns normals into environment map coordinates. */
static inline void MatFXLoadEnvMapMatrix(RwMatrix *objectMatrix, RwFrame *frame) {
    RwMatrix objectFrameMatrix;
    RwMatrix frameMatrix;
    f32 mtx[3][4];
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
    fn_803992BC(mtx, 0x1E, 1);
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
        lbl_805E32C8.tevSet = 0;
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
            lbl_805E32C8.tevSet = 0;
        } else {
            fn_80394F3C(0, 1, 5, 0x3C, 0, 0x7D);
            lbl_805E32C8.tevSet = 0;
        }
    } else {
        if (data->data[0].u.uv.dualTransform != NULL) {
            MatFXLoadUVTransform(mtx, data->data[0].u.uv.dualTransform, 4);
            lbl_805E32C8.tevSet = 0;
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
    GXColor color = lbl_805FB474;

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
    if (data->data[0].u.env.coef < lbl_805FB478) {
        color.r = color.g = color.b = (u8)(lbl_805FB47C * data->data[0].u.env.coef);
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
    lbl_805E32C8.material = NULL;
    lbl_805E32C8.lightingSet = 0;
    lbl_805E32C8.tevSet = 0;
    fn_80499C38(data->data[0].u.env.texture, 0);
    MatFXLoadEnvMapMatrix(objectMatrix, data->data[0].u.env.frame);
    fn_80399000(list->data, list->size);
    RwEngineInstance->renderStateSet(10, (void *)srcBlend);
    RwEngineInstance->renderStateSet(11, (void *)dstBlend);
}
#ifdef __cplusplus
}
#endif

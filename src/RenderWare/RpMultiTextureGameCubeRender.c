/* RenderWare multi-texture effects, GameCube rendering, 0x804A6128..0x804A6C74: the engine
 * plugin 0x129 (eight bytes of globals: frames whose matrices texture coordinates can be made
 * relative to), the GX set-up an effect describes (fn_804A62E4) and the render callback of a
 * multi-texture material (fn_804A6A08, called by RpMatFXGameCube.c's dispatcher).
 * Data: .sbss 0x805F16C8 (the plugin offset, padded to 8 bytes), .sdata2 0x805FB488..0x805FB498.
 * Built with cflags_rw_r5 like RpMatFX.c: fn_804A6170 is too large to be expanded in fn_804A62E4.
 * Functions keep their fn_ symbols.
 * Status: fn_804A6128, fn_804A616C, fn_804A6A08 and fn_804A6C30 match; near: fn_804A6170 (retail
 * routes the identity matrix's address through r0), fn_804A62E4 (stack slots and saved registers). */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

/* The plugin's globals: frames whose inverse matrix the texture coordinate generation can use,
 * found by number. */
typedef struct GcnMatGlobals {
    u32 numFrames;    /* 0x00 */
    RwFrame **frames; /* 0x04 */
} GcnMatGlobals;

/* The platform's extra data of a multi-texture material: callbacks around the draw. */
typedef struct MTCallbacks {
    void (*preRender)(RpMaterial *material, void *a, void *b, void *data);  /* 0x00 */
    void (*postRender)(RpMaterial *material, void *a, void *b, void *data); /* 0x04 */
    void *data;                                                             /* 0x08 */
} MTCallbacks;

static s32 GcnMatGlobalsOffset;

void GXSetTexCoordScaleManually(s32 coord, u8 enable, u16 scaleS, u16 scaleT);
void GXSetNumIndStages(u8 count);
void GXSetIndTexCoordScale(s32 stage, s32 scaleS, s32 scaleT);
void GXSetTevDirect(s32 stage);
typedef struct GXColorS10 {
    s16 r, g, b, a;
} GXColorS10;

void GXSetTevColorS10(s32 id, GXColorS10 color);
void GXSetTevKColor(s32 id, GXColor color);
void GXSetIndTexMtx(s32 id, f32 offset[2][3], s8 scaleExp);
void GXSetIndTexOrder(s32 stage, s32 coord, s32 map);
void GXSetTevIndirect(s32 stage, s32 indStage, s32 format, s32 biasSel, s32 matrixSel, s32 wrapS,
                      s32 wrapT, u8 addPrev, u8 utcLod, s32 alphaSel);
void GXSetTevColorIn(s32 stage, s32 a, s32 b, s32 c, s32 d);
void GXSetTevAlphaIn(s32 stage, s32 a, s32 b, s32 c, s32 d);
void GXSetTevKColorSel(s32 stage, s32 sel);
void GXSetTevKAlphaSel(s32 stage, s32 sel);
void GXSetTevColorOp(s32 stage, s32 op, s32 bias, s32 scale, u8 clamp, s32 outReg);
void GXSetTevAlphaOp(s32 stage, s32 op, s32 bias, s32 scale, u8 clamp, s32 outReg);
void fn_80378924(f32 a[3][4], f32 b[3][4], f32 ab[3][4]); /* PSMTXConcat */
GcnMatSettings *fn_804A60D8(GcnMatSettings *settings, const u32 *packed);
RwMatrix *fn_804A6170(s32 type, RwMatrix *matrix, RwMatrix *out);

void *fn_804A6128(void *instance, s32 offset, s32 size) {
    memset((u8 *)RwEngineInstance + GcnMatGlobalsOffset, 0, 8);
    return instance;
}


void *fn_804A616C(void *instance, s32 offset, s32 size) {
    return instance;
}

RwMatrix *fn_804A6170(s32 type, RwMatrix *matrix, RwMatrix *out) {
    if (type >= 16) {
        GcnMatGlobals *globals = (GcnMatGlobals *)((u8 *)RwEngineInstance + GcnMatGlobalsOffset);
        u32 index = type - 16;
        RwFrame **frames = globals->frames;
        if (index < globals->numFrames && frames != NULL && frames[index] != NULL) {
            RwFrame *frame = frames[index];
            if (matrix != NULL) {
                RwMatrix inverse;
                fn_8047F0B0(&inverse, RwFrameGetLTM(frame));
                fn_8047EBB8(out, matrix, &inverse);
            } else {
                fn_8047F0B0(out, RwFrameGetLTM(frame));
            }
            return out;
        }
        if (matrix != NULL) return matrix;
        out->right.x = out->up.y = out->at.z = 1.0f;
        out->right.y = out->right.z = out->up.x = 0.0f;
        out->up.z = out->at.x = out->at.y = 0.0f;
        out->pos.x = out->pos.y = out->pos.z = 0.0f;
        out->flags |= 0x20003;
        return out;
    }
    if (type == 3) {
        if (matrix != NULL) {
            fn_8047EBB8(out, matrix, &lbl_805E31CC);
            return out;
        }
        return &lbl_805E31CC;
    }
    if (type == 2) {
        if (matrix != NULL) return matrix;
        return NULL;
    }
    return NULL;
}

/* Sets the GX state the instance data describes: TEV colours and constants, the texture and
 * indirect matrices (made relative to `matrix` where their source asks for it), indirect stages,
 * TEV stages and texture coordinate generation; textureFlags renumbers texture sources. */
u8 *fn_804A62E4(u8 *instance, u32 numTextures, u8 *textureFlags, RwMatrix *matrix) {
    RwMatrix product;
    f32 envMtx[2][4];
    RwMatrix sourceMtx;
    f32 rwMtx[3][4];
    f32 texMtx[3][4];
    RwMatrix indSourceMtx;
    f32 indMtx[2][3];
    GcnMatSettings settings;
    s8 coordMap[8];
    GcnMatInstanceBody *body = (GcnMatInstanceBody *)fn_804A60D0(instance);
    s32 colorChannel;
    s32 i;

    if (body->unk00 & 1) {
        GXSetTevColorS10(1, ((GXColorS10 *)body->bytes0E)[0]);
        GXSetTevColorS10(2, ((GXColorS10 *)body->bytes0E)[1]);
        GXSetTevColorS10(3, ((GXColorS10 *)body->bytes0E)[2]);
        GXSetTevColorS10(0, ((GXColorS10 *)body->bytes0E)[3]);
    }
    if (body->unk00 & 2) {
        GXSetTevKColor(0, ((GXColor *)body->bytes2E)[0]);
        GXSetTevKColor(1, ((GXColor *)body->bytes2E)[1]);
        GXSetTevKColor(2, ((GXColor *)body->bytes2E)[2]);
        GXSetTevKColor(3, ((GXColor *)body->bytes2E)[3]);
    }
    if (body->unk00 & 0x20) {
        RwMatrix *m;
        if (matrix != NULL) {
            m = fn_8047EBB8(&product, matrix, &lbl_805E31CC);
        } else {
            m = &lbl_805E31CC;
        }
        envMtx[0][0] = -0.5f * m->right.x;
        envMtx[0][1] = -0.5f * m->up.x;
        envMtx[0][2] = -0.5f * m->at.x;
        envMtx[0][3] = 0.5f;
        envMtx[1][0] = -0.5f * m->right.y;
        envMtx[1][1] = -0.5f * m->up.y;
        envMtx[1][2] = -0.5f * m->at.y;
        envMtx[1][3] = 0.5f;
        fn_803992BC(envMtx, 0x1E, 1);
    }
    for (i = 0; i < body->numC; i++) {
        GcnMatElementC *element = &body->arrayC[i];
        RwMatrix *m = fn_804A6170(element->unk00[1], matrix, &sourceMtx);
        if (m != NULL) {
            rwMtx[0][0] = m->right.x;
            rwMtx[0][1] = m->up.x;
            rwMtx[0][2] = m->at.x;
            rwMtx[1][0] = m->right.y;
            rwMtx[1][1] = m->up.y;
            rwMtx[1][2] = m->at.y;
            rwMtx[2][0] = m->right.z;
            rwMtx[2][1] = m->up.z;
            rwMtx[2][2] = m->at.z;
            if (element->unk00[2] & 2) {
                rwMtx[0][3] = m->pos.x;
                rwMtx[1][3] = m->pos.y;
                rwMtx[2][3] = m->pos.z;
            } else {
                rwMtx[0][3] = rwMtx[1][3] = rwMtx[2][3] = 0.0f;
            }
            fn_80378924((f32(*)[4])element->matrix, rwMtx, texMtx);
            fn_803992BC(texMtx, element->unk00[0], element->unk00[2] & 1);
        } else {
            fn_803992BC((f32(*)[4])element->matrix, element->unk00[0], element->unk00[2] & 1);
        }
    }
    for (i = 0; i < body->numE; i++) {
        GcnMatElementE *element = &body->arrayE[i];
        RwMatrix *m = fn_804A6170(element->unk00[1], matrix, &indSourceMtx);
        f32(*offset)[3];
        if (m != NULL) {
            f32 *v = element->values;
            indMtx[0][0] = v[0] * m->right.x + v[1] * m->right.y + v[2] * m->right.z;
            indMtx[0][1] = v[0] * m->up.x + v[1] * m->up.y + v[2] * m->up.z;
            indMtx[0][2] = v[0] * m->at.x + v[1] * m->at.y + v[2] * m->at.z;
            indMtx[1][0] = v[3] * m->right.x + v[4] * m->right.y + v[5] * m->right.z;
            indMtx[1][1] = v[3] * m->up.x + v[4] * m->up.y + v[5] * m->up.z;
            indMtx[1][2] = v[3] * m->at.x + v[4] * m->at.y + v[5] * m->at.z;
            offset = indMtx;
        } else {
            offset = (f32(*)[3])element->values;
        }
        GXSetIndTexMtx(element->unk00[0], offset, (s8)element->unk24);
    }
    colorChannel = 1;
    if (body->unk00 & 0x10) colorChannel = 2;
    for (i = 0; i < body->numTexGens; i++) {
        coordMap[i] = -1;
    }
    if (body->numIndStages != 0) {
        GXSetNumIndStages(body->numIndStages);
        for (i = 0; i < body->numIndStages; i++) {
            GcnMatIndStage *stage = &body->indStages[i];
            if (!(stage->unk00[0] & 1)) {
                GXSetIndTexOrder(i, stage->unk00[1], stage->unk00[2]);
                GXSetIndTexCoordScale(i, stage->unk00[3], stage->unk00[4]);
                coordMap[stage->unk00[1]] = stage->unk00[2];
            }
        }
    }
    if (body->numTevStages != 0) {
        fn_803989A0(body->numTevStages);
        for (i = 0; i < body->numTevStages; i++) {
            GcnMatTevStage *stage = &body->tevStages[i];
            if (!(stage->unk44[6] & 1)) {
                s32 color;
                if (stage->indirect != 0) {
                    fn_804A60D8(&settings, &stage->indirect);
                    GXSetTevIndirect(i, settings.field0, settings.field1, settings.field2,
                                     settings.field3, settings.field4, settings.field5,
                                     settings.field6, settings.field7, settings.field8);
                } else {
                    GXSetTevDirect(i);
                }
                GXSetTevColorIn(i, stage->unk00[0], stage->unk00[1], stage->unk00[2], stage->unk00[3]);
                GXSetTevAlphaIn(i, stage->unk00[4], stage->unk00[5], stage->unk00[6], stage->unk00[7]);
                color = stage->unk44[5];
                if (color <= 5 && colorChannel == 2) color = 4;
                fn_80398804(i, stage->unk44[3], stage->unk44[4], color);
                GXSetTevKColorSel(i, stage->unk44[1]);
                GXSetTevKAlphaSel(i, stage->unk44[2]);
                GXSetTevColorOp(i, stage->unk00[8], stage->unk00[9], stage->unk00[10], stage->unk2C,
                                stage->unk30[0]);
                GXSetTevAlphaOp(i, stage->unk30[1], stage->unk30[2], stage->unk30[3], stage->unk40,
                                stage->unk44[0]);
                coordMap[stage->unk44[3]] = stage->unk44[4];
            }
        }
    }
    fn_803951BC(body->numTexGens);
    for (i = 0; i < body->numTexGens; i++) {
        GcnMatTexGen *texGen = &body->texGens[i];
        if (!(texGen->flags & 1)) {
            s32 src = texGen->src;
            if (textureFlags != NULL && src >= 4 && src <= 11 && coordMap[i] >= 0) {
                src = textureFlags[coordMap[i]] + 4;
            }
            fn_80394F3C(i, texGen->type, src, texGen->unk0C, (texGen->flags >> 1) & 1, texGen->unk10);
            if (texGen->flags & 4) {
                GXSetTexCoordScaleManually(i, 1, texGen->unk14, texGen->unk16);
            }
        }
    }
    if (body->unk00 & 8) {
        fn_80497778(0);
    } else {
        fn_80497778(1);
    }
    if (body->unk00 & 4) {
        RwEngineInstance->renderStateSet(10, (void *)body->unk54);
        RwEngineInstance->renderStateSet(11, (void *)body->unk58);
    }
    return instance;
}

/* Draws a mesh with a multi-texture material: its textures, the instance data's texture
 * coordinate and TEV set-up around the display list, and the definition's callbacks. */
void fn_804A6A08(RwGameCubeDisplayList *list, RpMaterial *material, void *a, void *b, void *object) {
    MTData *data = (MTData *)fn_804A90C8(material, 6);
    MTCallbacks *callbacks = (MTCallbacks *)data->extra;
    s32 srcBlend;
    s32 dstBlend;
    u32 i;

    RwEngineInstance->renderStateGet(10, &srcBlend);
    RwEngineInstance->renderStateGet(11, &dstBlend);
    for (i = 0; i < data->numTextures; i++) {
        fn_80499C38(data->textures[i], i);
    }
    if (callbacks->preRender != NULL) {
        callbacks->preRender(material, a, b, callbacks->data);
    }
    if (data->effect != NULL) {
        GcnMatInstanceBody *body;
        s32 j;
        fn_804A62E4(data->effect, data->numTextures, data->textureCoords, object);
        fn_80399000(list->data, list->size);
        body = (GcnMatInstanceBody *)fn_804A60D0(data->effect);
        for (j = 0; j < body->numTexGens; j++) {
            if (body->texGens[j].flags & 4) {
                GXSetTexCoordScaleManually(j, 0, 0, 0);
            }
        }
        if (body->numIndStages != 0) {
            GXSetNumIndStages(0);
            for (j = 0; j < body->numIndStages; j++) {
                GXSetIndTexCoordScale(j, 0, 0);
            }
            for (j = 0; j < body->numTevStages; j++) {
                if (body->tevStages[j].indirect != 0) {
                    GXSetTevDirect(j);
                }
            }
        }
    } else {
        fn_80399000(list->data, list->size);
    }
    if (callbacks->postRender != NULL) {
        callbacks->postRender(material, a, b, callbacks->data);
    }
    RwEngineInstance->renderStateSet(10, (void *)srcBlend);
    RwEngineInstance->renderStateSet(11, (void *)dstBlend);
}

/* Registers plugin 0x129's 8 bytes of engine globals. */
s32 fn_804A6C30(void) {
    GcnMatGlobalsOffset = fn_80485D78(8, 0x129, fn_804A6128, fn_804A616C);
    return GcnMatGlobalsOffset >= 0;
}

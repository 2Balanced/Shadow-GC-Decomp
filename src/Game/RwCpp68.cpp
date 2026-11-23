/* RenderWare, 0x804A560C..0x804A6F8C: the GameCube side of the material effects plugin's
 * rendering (the callbacks fn_804A5F08 registers) and the small engine plugin 0x129 that
 * RpMatFXPluginAttach attaches (fn_804A6C74).  Functions keep their fn_ symbols; comments say
 * what they do.  Definitions stand in reverse address order: the compiler emits them in reverse.
 * Status: fn_804A560C, fn_804A5A94, fn_804A5F08, fn_804A60D0, fn_804A60D8, fn_804A6128,
 * fn_804A616C, fn_804A6C30 and fn_804A6C74 match (the stream write needs its instance declared
 * const: only then are the header loads scheduled above the buffer stores and the array bases and
 * counts hoisted out of the loops).  Near: fn_804A6170 (retail routes the identity matrix's
 * address through r0), fn_804A6CDC/fn_804A6E34 (the two strip vertices' registers
 * swapped), fn_804A6A08 (loop counters and their strength-reduced offsets trade registers, with
 * retail's offset copied from the zeroed counter), fn_804A5F48 (GC/2.7 reassociates the size
 * sums' +3 onto the products), fn_804A62E4 (retail's instructions; its stack slots, saved
 * registers and the f1/f2 of the -0.5f/0.5f constants differ: named as externs those two trade
 * registers, as in RwCpp66's fn_804A388C). */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

#ifdef __cplusplus
extern "C" {
#endif
extern s32 lbl_805F16C8; /* offset of plugin 0x129's globals in RwEngineInstance */
extern const f32 lbl_805FB488; /* 1.0f */
extern const f32 lbl_805FB48C; /* 0.0f */

s32 fn_804A8E90(void);
s32 fn_804A8EFC(s32 index, u32 pluginId, u32 size);
s32 fn_804A912C(u32 index, void *instance, void *render, void *dispose, u32 flags);
GcnMatInstance *fn_804A5A94(RwStream *stream);
const GcnMatInstance *fn_804A560C(const GcnMatInstance *instance, RwStream *stream);
void fn_804A55CC(void);
void *fn_804A6128(void *instance, s32 offset, s32 size);
void *fn_804A616C(void *instance, s32 offset, s32 size);
s32 fn_804A5F08(void);
s32 fn_804A6C30(void);

/* The vertex data of one triangle of an indexed strip: where its positions, its normal (and
 * binormal and tangent) and its texture coordinates are. */
typedef struct GcnStripTriangle {
    u8 *pos[3];   /* 0x00 */
    u8 *nbt;      /* 0x0C: the first vertex's */
    u8 *uv[3];    /* 0x10 */
    u8 posSize;   /* 0x1C: bytes per position component */
    u8 nbtSize;   /* 0x1D: bytes per normal component */
    u8 uvSize;    /* 0x1E: bytes per texture coordinate component */
} GcnStripTriangle;

/* Strip triangle `i` of 16-bit indices `stride` bytes apart; degenerate triangles that only
 * restart the strip are stepped over, and odd triangles have their winding flipped. */
void fn_804A6E34(GcnStripTriangle *tri, const u8 *indices, u32 stride, u8 *pos, u8 *nbt, u8 *uv,
                 s32 i) {
#define INDEX(k) (*(const u16 *)(indices + stride * (k)))
    u32 v0 = INDEX(i);
    u16 v2;
    u16 v1;

    if (i < 2) {
        if (i == 0) {
            v1 = INDEX(1);
            v2 = INDEX(2);
        } else {
            v2 = INDEX(0);
            v1 = INDEX(2);
        }
    } else {
        v1 = INDEX(i - 2);
        v2 = INDEX(i - 1);
        if (v2 == v1) {
            u16 next = INDEX(i + 1);
            if (v0 == next) {
                v1 = INDEX(i + 2);
                i++;
                if (v0 == v1) {
                    v1 = INDEX(i + 2);
                    i++;
                }
                v2 = INDEX(i + 2);
            } else {
                v1 = v2;
                v2 = next;
            }
        }
        if (i & 1) {
            u16 swap = v1;
            v1 = v2;
            v2 = swap;
        }
    }
#undef INDEX
    tri->pos[0] = pos + v0 * (tri->posSize * 3);
    tri->pos[1] = pos + v1 * (tri->posSize * 3);
    tri->pos[2] = pos + v2 * (tri->posSize * 3);
    tri->nbt = nbt + v0 * (tri->nbtSize * 9);
    tri->uv[0] = uv + v0 * (tri->uvSize * 2);
    tri->uv[1] = uv + v1 * (tri->uvSize * 2);
    tri->uv[2] = uv + v2 * (tri->uvSize * 2);
}

/* The same for 8-bit indices. */
void fn_804A6CDC(GcnStripTriangle *tri, const u8 *indices, u32 stride, u8 *pos, u8 *nbt, u8 *uv,
                 s32 i) {
#define INDEX(k) (indices[stride * (k)])
    u32 v0 = INDEX(i);
    u8 v2;
    u8 v1;

    if (i < 2) {
        if (i == 0) {
            v1 = INDEX(1);
            v2 = INDEX(2);
        } else {
            v2 = INDEX(0);
            v1 = INDEX(2);
        }
    } else {
        v1 = INDEX(i - 2);
        v2 = INDEX(i - 1);
        if (v2 == v1) {
            u8 next = INDEX(i + 1);
            if (v0 == next) {
                v1 = INDEX(i + 2);
                i++;
                if (v0 == v1) {
                    v1 = INDEX(i + 2);
                    i++;
                }
                v2 = INDEX(i + 2);
            } else {
                v1 = v2;
                v2 = next;
            }
        }
        if (i & 1) {
            u8 swap = v1;
            v1 = v2;
            v2 = swap;
        }
    }
#undef INDEX
    tri->pos[0] = pos + v0 * (tri->posSize * 3);
    tri->pos[1] = pos + v1 * (tri->posSize * 3);
    tri->pos[2] = pos + v2 * (tri->posSize * 3);
    tri->nbt = nbt + v0 * (tri->nbtSize * 9);
    tri->uv[0] = uv + v0 * (tri->uvSize * 2);
    tri->uv[1] = uv + v1 * (tri->uvSize * 2);
    tri->uv[2] = uv + v2 * (tri->uvSize * 2);
}

void GXSetTexCoordScaleManually(s32 coord, u8 enable, u16 scaleS, u16 scaleT);
void GXSetNumIndStages(u8 count);
void GXSetIndTexCoordScale(s32 stage, s32 scaleS, s32 scaleT);
void GXSetTevDirect(s32 stage);

/* The material plugin data fn_804A90C8(material, 6) returns (plugin 0x12C, see RwCpp69). */
typedef struct MTCallbacks {
    void (*preRender)(RpMaterial *material, void *a, void *b, void *data);  /* 0x00 */
    void (*postRender)(RpMaterial *material, void *a, void *b, void *data); /* 0x04 */
    void *data;                                                             /* 0x08 */
} MTCallbacks;

typedef struct MTData {
    void *def;              /* 0x00 */
    u32 numTextures;        /* 0x04 */
    RwTexture *textures[8]; /* 0x08 */
    u8 textureFlags[8];     /* 0x28 */
    u8 *instance;           /* 0x30: a GcnMatInstance */
    MTCallbacks *callbacks; /* 0x34 */
} MTData;

void *fn_804A90C8(void *object, s32 index);

/* The packed material settings of the plugin's stream data, unpacked. */
typedef struct GcnMatSettings {
    u32 field0;  /* 0x00: bits 0..1 */
    u32 field1;  /* 0x04: bits 2..3 */
    u32 field2;  /* 0x08: bits 4..6 */
    u32 field3;  /* 0x0C: bits 7..10 */
    u32 field4;  /* 0x10: bits 11..13 */
    u32 field5;  /* 0x14: bits 14..16 */
    u8 field6;   /* 0x18: bit 17 */
    u8 field7;   /* 0x19: bit 18 */
    u32 field8;  /* 0x1C: bits 19..20 */
} GcnMatSettings;

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

extern const f32 lbl_805FB490; /* -0.5f */
extern const f32 lbl_805FB494; /* 0.5f */

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
        envMtx[0][0] = lbl_805FB490 * m->right.x;
        envMtx[0][1] = lbl_805FB490 * m->up.x;
        envMtx[0][2] = lbl_805FB490 * m->at.x;
        envMtx[0][3] = lbl_805FB494;
        envMtx[1][0] = lbl_805FB490 * m->right.y;
        envMtx[1][1] = lbl_805FB490 * m->up.y;
        envMtx[1][2] = lbl_805FB490 * m->at.y;
        envMtx[1][3] = lbl_805FB494;
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
                rwMtx[0][3] = rwMtx[1][3] = rwMtx[2][3] = lbl_805FB48C;
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
    MTCallbacks *callbacks = data->callbacks;
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
    if (data->instance != NULL) {
        GcnMatInstanceBody *body;
        s32 j;
        fn_804A62E4(data->instance, data->numTextures, data->textureFlags, object);
        fn_80399000(list->data, list->size);
        body = (GcnMatInstanceBody *)fn_804A60D0(data->instance);
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

/* Attaches plugin 0x129 and the callbacks it needs; RpMatFXPluginAttach calls it. */
s32 fn_804A6C74(void) {
    if (!fn_804A8E90()) return FALSE;
    if (!fn_804A8EFC(6, 0x129, 12)) return FALSE;
    if (fn_804A5F08() == FALSE) return FALSE;
    return fn_804A6C30();
}

/* Registers plugin 0x129's 8 bytes of engine globals. */
s32 fn_804A6C30(void) {
    lbl_805F16C8 = fn_80485D78(8, 0x129, fn_804A6128, fn_804A616C);
    return lbl_805F16C8 >= 0;
}


/* Plugin 0x129's globals: frames whose inverse matrix the texture coordinate generation can
 * use, found by number. */
typedef struct GcnMatGlobals {
    u32 numFrames;    /* 0x00 */
    RwFrame **frames; /* 0x04 */
} GcnMatGlobals;


/* The texture matrix for a texture coordinate source: 16 and up are the registered frames'
 * inverse matrices (times `matrix` when there is one), 3 the identity, 2 `matrix` itself. */
RwMatrix *fn_804A6170(s32 type, RwMatrix *matrix, RwMatrix *out) {
    if (type >= 16) {
        GcnMatGlobals *globals = (GcnMatGlobals *)((u8 *)RwEngineInstance + lbl_805F16C8);
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
        out->right.x = out->up.y = out->at.z = lbl_805FB488;
        out->right.y = out->right.z = out->up.x = lbl_805FB48C;
        out->up.z = out->at.x = out->at.y = lbl_805FB48C;
        out->pos.x = out->pos.y = out->pos.z = lbl_805FB48C;
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

void *fn_804A616C(void *instance, s32 offset, s32 size) {
    return instance;
}

void *fn_804A6128(void *instance, s32 offset, s32 size) {
    memset((u8 *)RwEngineInstance + lbl_805F16C8, 0, 8);
    return instance;
}

GcnMatSettings *fn_804A60D8(GcnMatSettings *settings, const u32 *packed) {
    u32 bits = *packed;
    settings->field0 = bits & 3;
    settings->field1 = (bits >> 2) & 3;
    settings->field2 = (bits >> 4) & 7;
    settings->field3 = (bits >> 7) & 15;
    settings->field4 = (bits >> 11) & 7;
    settings->field5 = (bits >> 14) & 7;
    settings->field6 = (bits >> 17) & 1;
    settings->field7 = (bits >> 18) & 1;
    settings->field8 = (bits >> 19) & 3;
    return settings;
}

u8 *fn_804A60D0(u8 *object) {
    return object + 0x30;
}

#define RWALIGN4(x) (((x) + 3) & ~3)


void *fn_804A932C(void *object, s32 type);

GcnMatInstance *fn_804A5F48(u32 numTevStages, u32 numTexGens, u32 numC, u32 numIndStages,
                            u32 numE) {
    u32 size;
    GcnMatInstance *instance;
    u8 *next;

    size = RWALIGN4(sizeof(GcnMatInstance) + numTexGens * sizeof(GcnMatTexGen));
    size = RWALIGN4(size + numTevStages * sizeof(GcnMatTevStage));
    size = RWALIGN4(size + numIndStages * sizeof(GcnMatIndStage));
    size = RWALIGN4(size + numC * sizeof(GcnMatElementC));
    size = RWALIGN4(size + numE * sizeof(GcnMatElementE));
    instance = (GcnMatInstance *)RwEngineInstance->memAlloc(size, 0x30129);
    if (instance == NULL) {
        RWERROR(0x120, (0x80000013, size));
        return NULL;
    }
    memset(instance, 0, size);
    fn_804A932C(instance, 6);
    instance->body.countTevStages = numTevStages;
    instance->body.countTexGens = numTexGens;
    instance->body.countIndStages = numIndStages;
    instance->body.countC = numC;
    instance->body.countE = numE;
    instance->body.numTevStages = instance->body.countTevStages;
    instance->body.numTexGens = instance->body.countTexGens;
    instance->body.numIndStages = instance->body.countIndStages;
    instance->body.numC = instance->body.countC;
    instance->body.numE = instance->body.countE;
    next = (u8 *)(instance + 1);
    if (numC != 0) {
        instance->body.arrayC = (GcnMatElementC *)next;
        next += numC * sizeof(GcnMatElementC);
    }
    if (numTexGens != 0) {
        instance->body.texGens = (GcnMatTexGen *)next;
        next += numTexGens * sizeof(GcnMatTexGen);
    }
    if (numE != 0) {
        instance->body.arrayE = (GcnMatElementE *)next;
        next += numE * sizeof(GcnMatElementE);
    }
    if (numIndStages != 0) {
        instance->body.indStages = (GcnMatIndStage *)next;
        next += numIndStages * sizeof(GcnMatIndStage);
    }
    if (numTevStages != 0) {
        instance->body.tevStages = (GcnMatTevStage *)next;
    }
    return instance;
}

GcnMatInstance *fn_804A5F48(u32 numTevStages, u32 numTexGens, u32 numC, u32 numIndStages,
                            u32 numE);
void fn_8047D254(void *data, u32 size); /* the stream's 32-bit words to native order */

/* Stream read of the instance data (resource type 6). */
GcnMatInstance *fn_804A5A94(RwStream *stream) {
    u8 header[0x38];
    u8 tevBuffer[0x1C];
    u8 texGenBuffer[0xC];
    u8 indBuffer[8];
    u8 cBuffer[4];
    u8 eBuffer[4];
    GcnMatInstance *instance;
    GcnMatInstanceBody *body;
    u32 i;

    if (fn_80480D4C(stream, header, sizeof(header)) != sizeof(header)) return NULL;
    instance = fn_804A5F48(header[1], header[0], header[0x34], header[0x36], header[0x35]);
    if (instance == NULL) return NULL;
    body = (GcnMatInstanceBody *)fn_804A60D0((u8 *)instance);
    body->unk00 = header[2];
    body->unk54 = (header[3] >> 4) & 0xF;
    body->unk58 = header[3] & 0xF;
    memcpy(body->bytes2E, &header[0x24], 0x10);
    fn_8047D254(&header[4], 0x20);
    memcpy(body->bytes0E, &header[4], 0x20);
    for (i = 0; i < body->numTexGens; i++) {
        GcnMatTexGen *texGen = &body->texGens[i];
        if (fn_80480D4C(stream, texGenBuffer, sizeof(texGenBuffer)) != sizeof(texGenBuffer)) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        texGen->flags = texGenBuffer[0];
        texGen->type = texGenBuffer[1];
        texGen->src = texGenBuffer[2];
        texGen->unk0C = texGenBuffer[3];
        texGen->unk10 = texGenBuffer[4];
        fn_8047D254(&texGenBuffer[8], 4);
        texGen->unk14 = *(u16 *)&texGenBuffer[8];
        texGen->unk16 = *(u16 *)&texGenBuffer[10];
    }
    for (i = 0; i < body->numTevStages; i++) {
        GcnMatTevStage *stage = &body->tevStages[i];
        if (fn_80480D4C(stream, tevBuffer, sizeof(tevBuffer)) != sizeof(tevBuffer)) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        stage->unk00[0] = tevBuffer[0];
        stage->unk00[1] = tevBuffer[1];
        stage->unk00[2] = tevBuffer[2];
        stage->unk00[3] = tevBuffer[3];
        stage->unk00[4] = tevBuffer[4];
        stage->unk00[5] = tevBuffer[5];
        stage->unk00[6] = tevBuffer[6];
        stage->unk00[7] = tevBuffer[7];
        stage->unk00[8] = tevBuffer[8];
        stage->unk00[9] = tevBuffer[9];
        stage->unk00[10] = tevBuffer[10];
        stage->unk2C = tevBuffer[11];
        stage->unk30[0] = tevBuffer[12];
        stage->unk30[1] = tevBuffer[13];
        stage->unk30[2] = tevBuffer[14];
        stage->unk30[3] = tevBuffer[15];
        stage->unk40 = tevBuffer[16];
        stage->unk44[0] = tevBuffer[17];
        stage->unk44[1] = tevBuffer[18];
        stage->unk44[2] = tevBuffer[19];
        stage->unk44[3] = tevBuffer[20];
        stage->unk44[4] = tevBuffer[21];
        stage->unk44[5] = tevBuffer[22];
        stage->unk44[6] = tevBuffer[23];
        fn_8047D254(&tevBuffer[24], 4);
        stage->indirect = *(u32 *)&tevBuffer[24];
        if (stage->unk44[6] & 0x80) {
            stage->unk44[4] |= 0x100;
            stage->unk44[6] &= ~0x80;
        }
    }
    for (i = 0; i < body->numC; i++) {
        GcnMatElementC *element = &body->arrayC[i];
        if (fn_80480D4C(stream, cBuffer, sizeof(cBuffer)) != sizeof(cBuffer) ||
            !fn_8047D784(stream, element->matrix, sizeof(element->matrix))) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        element->unk00[0] = cBuffer[0];
        element->unk00[1] = cBuffer[1];
        element->unk00[2] = cBuffer[2];
    }
    for (i = 0; i < body->numE; i++) {
        GcnMatElementE *element = &body->arrayE[i];
        if (fn_80480D4C(stream, eBuffer, sizeof(eBuffer)) != sizeof(eBuffer) ||
            !fn_8047D784(stream, element->values, sizeof(element->values))) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        element->unk00[0] = eBuffer[0];
        element->unk00[1] = eBuffer[1];
        element->unk00[2] = eBuffer[2];
        element->unk24 = (s8)eBuffer[3];
    }
    for (i = 0; i < body->numIndStages; i++) {
        GcnMatIndStage *stage = &body->indStages[i];
        if (fn_80480D4C(stream, indBuffer, sizeof(indBuffer)) != sizeof(indBuffer)) {
            RwEngineInstance->memFree(instance);
            return NULL;
        }
        stage->unk00[0] = indBuffer[0];
        stage->unk00[1] = indBuffer[1];
        stage->unk00[2] = indBuffer[2];
        stage->unk00[3] = indBuffer[3];
        stage->unk00[4] = indBuffer[4];
    }
    return instance;
}

RwStream *fn_8047CEF0(RwStream *stream, u32 type, u32 size, u32 version, u32 buildNum);
void fn_8047D1C8(void *data, u32 size); /* native 32-bit words to the stream's order */
void fn_8047D158(void *data, u32 size);

/* The bytes the instance data takes in a stream (fn_804A55CC, expanded here). */
static inline u32 GcnMatInstanceStreamSize(const GcnMatInstance *instance) {
    u32 size = instance->body.numC * 0x34 + 0x38;
    size += instance->body.numTexGens * 0x0C;
    size += instance->body.numE * 0x1C;
    size += instance->body.numIndStages << 3;
    size += instance->body.numTevStages * 0x1C;
    return size;
}

/* Stream write of the instance data (resource type 6). */
const GcnMatInstance *fn_804A560C(const GcnMatInstance *instance, RwStream *stream) {
    u8 header[0x38];
    u8 tevBuffer[0x1C];
    u8 texGenBuffer[0xC];
    u8 indBuffer[8];
    u8 cBuffer[4];
    u8 eBuffer[4];
    u32 i;

    if (!fn_8047CEF0(stream, 3, GcnMatInstanceStreamSize(instance), 0x37002, 0x37)) {
        return NULL;
    }
    header[2] = instance->body.unk00;
    header[1] = instance->body.numTevStages;
    header[0] = instance->body.numTexGens;
    header[0x34] = instance->body.numC;
    header[0x35] = instance->body.numE;
    header[0x36] = instance->body.numIndStages;
    header[3] = ((instance->body.unk54 & 0xF) << 4) | (instance->body.unk58 & 0xF);
    memcpy(&header[0x24], instance->body.bytes2E, 0x10);
    memcpy(&header[4], instance->body.bytes0E, 0x20);
    fn_8047D1C8(&header[4], 0x20);
    if (!fn_80480ED0(stream, header, sizeof(header))) return NULL;
    for (i = 0; i < instance->body.numTexGens; i++) {
        const GcnMatTexGen *texGen = &instance->body.texGens[i];
        texGenBuffer[0] = texGen->flags;
        texGenBuffer[1] = texGen->type;
        texGenBuffer[2] = texGen->src;
        texGenBuffer[3] = texGen->unk0C;
        texGenBuffer[4] = texGen->unk10;
        texGenBuffer[5] = 0;
        texGenBuffer[6] = 0;
        texGenBuffer[7] = 0;
        *(u16 *)&texGenBuffer[8] = texGen->unk14;
        *(u16 *)&texGenBuffer[10] = texGen->unk16;
        fn_8047D1C8(&texGenBuffer[8], 4);
        if (!fn_80480ED0(stream, texGenBuffer, sizeof(texGenBuffer))) return NULL;
    }
    for (i = 0; i < instance->body.numTevStages; i++) {
        const GcnMatTevStage *stage = &instance->body.tevStages[i];
        tevBuffer[0] = stage->unk00[0];
        tevBuffer[1] = stage->unk00[1];
        tevBuffer[2] = stage->unk00[2];
        tevBuffer[3] = stage->unk00[3];
        tevBuffer[4] = stage->unk00[4];
        tevBuffer[5] = stage->unk00[5];
        tevBuffer[6] = stage->unk00[6];
        tevBuffer[7] = stage->unk00[7];
        tevBuffer[8] = stage->unk00[8];
        tevBuffer[9] = stage->unk00[9];
        tevBuffer[10] = stage->unk00[10];
        tevBuffer[11] = stage->unk2C;
        tevBuffer[12] = stage->unk30[0];
        tevBuffer[13] = stage->unk30[1];
        tevBuffer[14] = stage->unk30[2];
        tevBuffer[15] = stage->unk30[3];
        tevBuffer[16] = stage->unk40;
        tevBuffer[17] = stage->unk44[0];
        tevBuffer[18] = stage->unk44[1];
        tevBuffer[19] = stage->unk44[2];
        tevBuffer[20] = stage->unk44[3];
        tevBuffer[21] = stage->unk44[4];
        tevBuffer[22] = stage->unk44[5];
        tevBuffer[23] = stage->unk44[6];
        *(u32 *)&tevBuffer[24] = stage->indirect;
        fn_8047D158(&tevBuffer[24], 4);
        if (stage->unk44[4] & 0x100) {
            tevBuffer[23] |= 0x80;
        }
        if (!fn_80480ED0(stream, tevBuffer, sizeof(tevBuffer))) return NULL;
    }
    for (i = 0; i < instance->body.numC; i++) {
        const GcnMatElementC *element = &instance->body.arrayC[i];
        cBuffer[0] = element->unk00[0];
        cBuffer[1] = element->unk00[1];
        cBuffer[2] = element->unk00[2];
        cBuffer[3] = 0;
        if (!fn_80480ED0(stream, cBuffer, sizeof(cBuffer)) ||
            !fn_8047D350(stream, element->matrix, sizeof(element->matrix))) {
            return NULL;
        }
    }
    for (i = 0; i < instance->body.numE; i++) {
        const GcnMatElementE *element = &instance->body.arrayE[i];
        eBuffer[0] = element->unk00[0];
        eBuffer[1] = element->unk00[1];
        eBuffer[2] = element->unk00[2];
        eBuffer[3] = element->unk24;
        if (!fn_80480ED0(stream, eBuffer, sizeof(eBuffer)) ||
            !fn_8047D350(stream, element->values, sizeof(element->values))) {
            return NULL;
        }
    }
    for (i = 0; i < instance->body.numIndStages; i++) {
        const GcnMatIndStage *stage = &instance->body.indStages[i];
        indBuffer[0] = stage->unk00[0];
        indBuffer[1] = stage->unk00[1];
        indBuffer[2] = stage->unk00[2];
        indBuffer[3] = stage->unk00[3];
        indBuffer[4] = stage->unk00[4];
        indBuffer[5] = 0;
        indBuffer[6] = 0;
        indBuffer[7] = 0;
        if (!fn_80480ED0(stream, indBuffer, sizeof(indBuffer))) return NULL;
    }
    return instance;
}

s32 fn_804A5F08(void) {
    return fn_804A912C(6, fn_804A5A94, fn_804A560C, fn_804A55CC, 0);
}
#ifdef __cplusplus
}
#endif

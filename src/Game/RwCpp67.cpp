/* RenderWare material effects plugin (rpmatfx), GameCube render file, 0x804A3DB0..0x804A560C:
 * the bump map render callback, the per-mesh dispatcher, the atomic and world sector instance and
 * render callbacks, the creation of the plugin's two pipelines and the small hooks the generic
 * part of the plugin calls.  That file starts at 0x804A2C90 in RwCpp66 and ends at 0x804A55CC;
 * the last function of this cut, fn_804A55CC, belongs to the file of RwCpp68.  Functions keep
 * their fn_ symbols.  fn_804A5280 is instruction-identical (its float constants, among them the
 * int to float one, sit in the unsplit .sdata2); fn_804A3DB0 and fn_804A457C are not written. */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

#ifdef __cplusplus
extern "C" {
#endif
extern RwGameCubeVertexFormat *lbl_805F16B8; /* the plugin's default vertex format */
extern RxPipeline *lbl_805F16BC;             /* its atomic pipeline */
extern RxPipeline *lbl_805F16C0;             /* its world sector pipeline */
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

RwTexture *fn_804A1A08(RwTexture *baseTexture, RwTexture *heightTexture);
void fn_804A457C(void *mesh, RwGameCubeDisplayList *list, void *object, RwMatrix *ltm,
                 RwGameCubeRenderNode *node);

/* fn_804A3DB0 (0x7CC bytes, the bump map render callback) and fn_804A457C (0xADC bytes, the
 * per-mesh dispatcher) are not decompiled yet. */

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
    lbl_805E32C8.tevSet = 0;
    lbl_805E32C8.lightingSet = 0;
    lbl_805E32C8.material = NULL;
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
    lbl_805F16BC = fn_8046335C(0x120, 1, (RwGameCubeNodeCallBack)fn_804A5058,
                               (RwGameCubeNodeCallBack)fn_804A5124, fn_80463428, fn_804A5280);
    lbl_805F16C0 = fn_80463D28(0x120, 2, (RwGameCubeNodeCallBack)fn_804A5204, NULL, fn_80463DF4,
                               fn_804A5280);
    return 1;
}

/* Destroys them (called from the engine close). */
s32 fn_804A5524(void) {
    if (lbl_805F16B8 != NULL) {
        fn_804630C0(lbl_805F16B8);
        lbl_805F16B8 = NULL;
    }
    if (lbl_805F16BC != NULL) {
        fn_8049B800(lbl_805F16BC);
        lbl_805F16BC = NULL;
    }
    if (lbl_805F16C0 != NULL) {
        fn_8049B800(lbl_805F16C0);
        lbl_805F16C0 = NULL;
    }
    return 1;
}

/* Gives an atomic the plugin's pipeline (RpMatFXAtomicEnableEffects). */
RpAtomic *fn_804A558C(RpAtomic *atomic) {
    atomic->pipeline = lbl_805F16BC;
    return atomic;
}

/* Gives a world sector the plugin's pipeline (RpMatFXWorldSectorEnableEffects). */
RpWorldSector *fn_804A5598(RpWorldSector *sector) {
    sector->pipeline = lbl_805F16C0;
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

/* ---- next source file (0x804A55CC): see RwCpp68 ---- */

/* The size of the block a node of fn_804A912C's kind 6 needs for `header`'s element counts. */
u32 fn_804A55CC(u8 *header) {
    u32 size = header[0x3B] * 0x34 + 0x38;
    size += header[0x39] * 0x0C;
    size += header[0x3D] * 0x1C;
    size += header[0x3C] << 3;
    size += header[0x3A] * 0x1C;
    return size;
}
#ifdef __cplusplus
}
#endif

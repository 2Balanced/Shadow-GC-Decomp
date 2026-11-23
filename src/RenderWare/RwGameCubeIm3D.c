/* RenderWare 3.7: the immediate-mode 3D module as the GameCube driver builds it (0x8049A82C..
 * 0x8049B0CB). RwIm3DTransform, RwIm3DEnd, RwIm3DRenderIndexedPrimitive, RwIm3DRenderPrimitive,
 * the transform and render pipeline accessors, and the module's open and close (an engine
 * plugin: its globals hold the pipelines and the transformed vertices). The file owns the module's
 * globals pointer and RwModuleInfo (.sbss 0x805F1680..0x805F168B). Built as C with the driver's
 * flags (-O4,p, no automatic inlining, writable strings).
 *
 * Names are inferred from behaviour; definitions are in retail address order. */
#include <dolphin/types.h>
#include "Game/rw/r7.h"

void *memset(void *dst, int c, unsigned long n);

extern RwDlGlobals *lbl_805F15B4;     /* RwEngineInstance */

RwBool fn_8049F8AC(void **pipeline);  /* creates the driver's Im3D transform pipeline */
void fn_8049F948(void **pipeline);    /* destroys it */
RwBool fn_8049FA1C(void **pipelines); /* creates the driver's Im3D render pipelines */
void fn_8049F990(void **pipelines);   /* destroys them */
RwDlPipeline *fn_8049B674(RwDlPipeline *pipeline, void *data, RwBool heapReset); /* RxPipelineExecute */
void *fn_8049B66C(void);              /* RxHeapGetGlobalHeap */

/* .sbss (defined in reverse: MWCC lays them out from the last definition). */
RwDlModuleInfo lbl_805F1684;          /* the module's RwModuleInfo */
RwDlIm3DGlobals *lbl_805F1680;        /* its globals */

#define RW_IM3D_GLOBALS ((RwDlIm3DGlobals *)((u8 *)lbl_805F15B4 + lbl_805F1684.globalsOffset))

/* RwIm3DTransform(vertices, numVerts, ltm, flags). */
void *fn_8049A82C(void *verts, RwUInt32 numVerts, void *ltm, RwUInt32 flags) {
    if (numVerts > 0x10000) {
        RW_ERROR(0x32);
    } else {
        RW_IM3D_GLOBALS->stash.numVerts = numVerts;
        RW_IM3D_GLOBALS->stash.verts = verts;
        RW_IM3D_GLOBALS->stash.stride = 0x24;
        RW_IM3D_GLOBALS->stash.render.ltm = ltm;
        RW_IM3D_GLOBALS->stash.render.flags = flags | 0x18;
        if (fn_8049B674(RW_IM3D_GLOBALS->transformPipeline, &RW_IM3D_GLOBALS->stash, 1) != NULL) {
            return verts;
        }
    }
    return NULL;
}

/* RwIm3DEnd. */
RwBool fn_8049A90C(void) {
    if (RW_IM3D_GLOBALS->stash.verts == NULL) {
        return 0;
    }
    memset(&RW_IM3D_GLOBALS->stash, 0, sizeof(RwDlIm3DStash));
    return 1;
}

/* RwIm3DRenderIndexedPrimitive: runs the render pipeline of the primitive type on the vertices
 * of the last RwIm3DTransform; triangle and line lists drop a trailing partial primitive. */
RwBool fn_8049A95C(RwInt32 primType, void *indices, RwInt32 numIndices) {
    if (RW_IM3D_GLOBALS->stash.verts != NULL) {
        RwDlIm3DRender *render = &RW_IM3D_GLOBALS->stash.render;
        render->pipeline = NULL;
        render->primType = primType;
        render->indices = indices;
        render->numIndices = numIndices;
        switch (primType) {
        case 3:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[0];
            render->numIndices = numIndices - numIndices % 3;
            break;
        case 5:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[1];
            break;
        case 4:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[2];
            break;
        case 1:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[3];
            render->numIndices = numIndices - numIndices % 2;
            break;
        case 2:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[4];
            break;
        default:
            RW_ERROR_STRING(0x25, primType);
            break;
        }
        if (fn_8049B674((RwDlPipeline *)render->pipeline, render, 0) != NULL) return 1;
    } else {
        RW_ERROR(0x23);
    }
    return 0;
}

/* RwIm3DRenderPrimitive: as fn_8049A95C, on all the vertices in order. */
RwBool fn_8049AAF8(RwInt32 primType) {
    RwBool active = RW_IM3D_GLOBALS->stash.verts != NULL;
    fn_8049B66C();
    if (active) {
        RwDlIm3DRender *render = &RW_IM3D_GLOBALS->stash.render;
        render->pipeline = NULL;
        render->primType = primType;
        render->indices = NULL;
        render->numIndices = RW_IM3D_GLOBALS->stash.numVerts;
        switch (primType) {
        case 3:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[0];
            break;
        case 5:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[1];
            break;
        case 4:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[2];
            break;
        case 1:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[3];
            break;
        case 2:
            render->pipeline = RW_IM3D_GLOBALS->renderPipelines[4];
            break;
        default:
            RW_ERROR_STRING(0x25, primType);
            break;
        }
        if (fn_8049B674((RwDlPipeline *)render->pipeline, render, 0) != NULL) return 1;
    } else {
        RW_ERROR(0x23);
    }
    return 0;
}

/* RwIm3DSetTransformPipeline: NULL puts the driver's own back. */
void *fn_8049AC90(void *pipeline) {
    if (pipeline != NULL) {
        RW_IM3D_GLOBALS->transformPipeline = pipeline;
    } else {
        RwDlIm3DGlobals *globals = RW_IM3D_GLOBALS;
        if (globals->platformTransformPipeline != NULL) {
            globals->transformPipeline = globals->platformTransformPipeline;
        } else {
            globals->transformPipeline = NULL;
        }
    }
    return RW_IM3D_GLOBALS->transformPipeline;
}

/* RwIm3DSetRenderPipeline: the render pipeline of a primitive type; NULL puts the driver's own
 * back (retail returns the driver's own pipeline for four of the six types). */
void *fn_8049ACE0(void *pipeline, RwInt32 primType) {
    if (pipeline != NULL) {
        switch (primType) {
        case 3:
            RW_IM3D_GLOBALS->renderPipelines[0] = pipeline;
            return pipeline;
        case 5:
            RW_IM3D_GLOBALS->renderPipelines[1] = pipeline;
            return pipeline;
        case 4:
            RW_IM3D_GLOBALS->renderPipelines[2] = pipeline;
            return pipeline;
        case 1:
            RW_IM3D_GLOBALS->renderPipelines[3] = pipeline;
            return pipeline;
        case 2:
            RW_IM3D_GLOBALS->renderPipelines[4] = pipeline;
            return pipeline;
        case 6:
            RW_IM3D_GLOBALS->renderPipelines[5] = pipeline;
            return pipeline;
        default:
            RW_ERROR(0x25);
            break;
        }
    } else {
        RwDlIm3DGlobals *globals;
        switch (primType) {
        case 3:
            globals = RW_IM3D_GLOBALS;
            if (globals->platformRenderPipelines[0] != NULL) {
                globals->renderPipelines[0] = globals->platformRenderPipelines[0];
            } else {
                globals->renderPipelines[0] = NULL;
            }
            return RW_IM3D_GLOBALS->renderPipelines[0];
        case 5:
            globals = RW_IM3D_GLOBALS;
            if (globals->platformRenderPipelines[1] != NULL) {
                globals->renderPipelines[1] = globals->platformRenderPipelines[1];
            } else {
                globals->renderPipelines[1] = NULL;
            }
            return RW_IM3D_GLOBALS->renderPipelines[1];
        case 4:
            globals = RW_IM3D_GLOBALS;
            if (globals->platformRenderPipelines[2] != NULL) {
                globals->renderPipelines[2] = globals->platformRenderPipelines[2];
            } else {
                globals->renderPipelines[2] = NULL;
            }
            return RW_IM3D_GLOBALS->platformRenderPipelines[2];
        case 1:
            globals = RW_IM3D_GLOBALS;
            if (globals->platformRenderPipelines[3] != NULL) {
                globals->renderPipelines[3] = globals->platformRenderPipelines[3];
            } else {
                globals->renderPipelines[3] = NULL;
            }
            return RW_IM3D_GLOBALS->platformRenderPipelines[3];
        case 2:
            globals = RW_IM3D_GLOBALS;
            if (globals->platformRenderPipelines[4] != NULL) {
                globals->renderPipelines[4] = globals->platformRenderPipelines[4];
            } else {
                globals->renderPipelines[4] = NULL;
            }
            return RW_IM3D_GLOBALS->platformRenderPipelines[4];
        case 6:
            globals = RW_IM3D_GLOBALS;
            if (globals->platformRenderPipelines[5] != NULL) {
                globals->renderPipelines[5] = globals->platformRenderPipelines[5];
            } else {
                globals->renderPipelines[5] = NULL;
            }
            return RW_IM3D_GLOBALS->platformRenderPipelines[5];
        default:
            RW_ERROR(0x25);
            break;
        }
    }
    return NULL;
}

/* The immediate-mode 3D module's close (an engine plugin destructor). */
void *fn_8049AF94(void *instance, RwInt32 offset, RwInt32 size) {
    fn_8049F990(RW_IM3D_GLOBALS->platformRenderPipelines);
    fn_8049F948(&RW_IM3D_GLOBALS->platformTransformPipeline);
    lbl_805F1684.numInstances--;
    return instance;
}

/* The immediate-mode 3D module's open (an engine plugin constructor): the driver's pipelines. */
void *fn_8049AFF8(void *instance, RwInt32 offset, RwInt32 size) {
    RwBool ok;
    lbl_805F1684.globalsOffset = offset;
    lbl_805F1680 = (RwDlIm3DGlobals *)((u8 *)lbl_805F15B4 + offset);
    lbl_805F1684.numInstances++;
    memset((u8 *)lbl_805F15B4 + offset, 0, sizeof(RwDlIm3DGlobals));
    ok = fn_8049F8AC(&RW_IM3D_GLOBALS->platformTransformPipeline);
    if (ok) ok = fn_8049FA1C(RW_IM3D_GLOBALS->platformRenderPipelines);
    if (ok) return instance;
    fn_8049F990(RW_IM3D_GLOBALS->platformRenderPipelines);
    fn_8049F948(&RW_IM3D_GLOBALS->platformTransformPipeline);
    lbl_805F1684.numInstances--;
    return NULL;
}

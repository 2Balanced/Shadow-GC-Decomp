/* RenderWare 0x8049A95C..0x8049C1DF: the end of the driver's immediate-mode 3D module (its open,
 * close and transform pipeline functions) and the start of PowerPipe's core (the pipeline module,
 * RxPipelineCreate/Destroy, packets and the temporary memory stack). Names are inferred from
 * behaviour; definitions are in retail address order.
 *
 * Not written yet: fn_8049ACE0, fn_8049B350, fn_8049BA30 and fn_8049BDF0.
 * NonMatching: fn_8049B674 (10 rows), fn_8049B8BC and fn_8049B92C (5 rows each), fn_8049BC4C
 * (15 rows); see the notes there. The rest match. */
#include <string.h>
#include <dolphin/types.h>
#include "Game/rw/r7.h"

extern "C" {
extern RwDlGlobals *lbl_805F15B4;     /* RwEngineInstance */
extern RwDlModuleInfo lbl_805F1684;   /* the immediate-mode 3D module */
extern RwDlIm3DGlobals *lbl_805F1680; /* its globals */
extern RwInt32 lbl_805F1690;          /* offset of PowerPipe's globals in the engine instance */
extern RwBool lbl_805F1698;           /* PowerPipe is open */
extern void *lbl_805F169C;            /* the global RxHeap */
extern RwUInt32 lbl_805EDF48;         /* its size */
extern RwInt32 lbl_805EDF4C;
extern RwInt32 lbl_805EDF50;          /* pipelines per free list block */
extern RwInt32 lbl_805EDF54;          /* blocks to preallocate */
extern u8 lbl_805E3278[0x28];         /* the pipeline free list's own record */
extern u8 *lbl_805EDF58[2];           /* the temporary memory: [0] top, [1] bottom */

RwBool fn_8049F8AC(void **pipeline);    /* creates the driver's Im3D transform pipeline */
void fn_8049F948(void **pipeline);    /* destroys it */
RwBool fn_8049FA1C(void **pipelines);   /* creates the driver's Im3D render pipelines */
void fn_8049F990(void **pipelines);   /* destroys them */
void *fn_8049F168(RwUInt32 size);     /* RxHeapCreate */
void fn_8049F0CC(void *heap);         /* RxHeapDestroy */
void fn_8049EA2C(void *heap, void *block); /* RxHeapFree */
void fn_8049F2A4(void *list);
void *fn_8047FC04(RwInt32 size, RwInt32 entries, RwInt32 align, RwInt32 blocks, void *freeList,
                  RwUInt32 hint);     /* RwFreeListCreateAndPreallocateSpace */
void fn_8047FCDC(void *freeList);     /* RwFreeListDestroy */
void fn_8049B350(RwDlPipelineNode *node, RwDlPipeline *pipeline);

#define RW_IM3D_GLOBALS ((RwDlIm3DGlobals *)((u8 *)lbl_805F15B4 + lbl_805F1684.globalsOffset))
#define RW_PIPE_GLOBAL(type, offset) (*(type *)((u8 *)lbl_805F15B4 + lbl_805F1690 + (offset)))

RwDlPipeline *fn_8049B674(RwDlPipeline *pipeline, void *data, RwBool heapReset);
void *fn_8049B66C(void);

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

RwBool fn_8049B258(void);

/* PowerPipe's open (an engine plugin constructor). */
void *fn_8049B0CC(void *instance, RwInt32 offset, RwInt32 size) {
    lbl_805F1690 = offset;
    if (fn_8049B258()) return instance;
    return NULL;
}

RwBool fn_8049B1F8(void);

/* PowerPipe's close (an engine plugin destructor). */
void *fn_8049B110(void *instance) {
    fn_8049B1F8();
    return instance;
}

RwBool fn_8049B140(void) {
    return 1;
}

void fn_8049B148(void) {
}

/* Frees the data of a packet's clusters and marks the packet empty. */
void fn_8049B14C(RwDlPacket *packet) {
    RwUInt32 count;
    RwDlCluster *cluster = packet->clusters;
    packet->pipeline->embeddedPacketState = 1;
    count = packet->numClusters;
    do {
        if (cluster->clusterRef != NULL) {
            if (cluster->data != NULL && !(cluster->flags & 2)) {
                fn_8049EA2C(lbl_805F169C, cluster->data);
            }
            *(RwUInt32 *)cluster = 0;
            cluster->data = NULL;
            cluster->numAlloced = 0;
            cluster->numUsed = 0;
            cluster->clusterRef = NULL;
        }
        cluster++;
    } while (--count);
    packet->flags = 0;
}

/* Shuts PowerPipe down: the pipeline free list and the global heap. */
RwBool fn_8049B1F8(void) {
    if (lbl_805F1698) {
        fn_8047FCDC(RW_PIPE_GLOBAL(void *, 0));
        RW_PIPE_GLOBAL(void *, 0) = NULL;
        fn_8049F0CC(lbl_805F169C);
        lbl_805F169C = NULL;
        lbl_805F1698 = 0;
    }
    return 1;
}

/* Starts PowerPipe: the global heap and the pipeline free list. */
RwBool fn_8049B258(void) {
    if (!lbl_805F1698) {
        lbl_805F169C = fn_8049F168(lbl_805EDF48);
        if (lbl_805F169C == NULL) return 0;
        RW_PIPE_GLOBAL(void *, 0) =
            fn_8047FC04(sizeof(RwDlPipeline), lbl_805EDF50, 4, lbl_805EDF54, lbl_805E3278, 0x40409);
        if (RW_PIPE_GLOBAL(void *, 0) == NULL) {
            fn_8049F0CC(lbl_805F169C);
            lbl_805F169C = NULL;
            return 0;
        }
        RW_PIPE_GLOBAL(RwInt32, 0x38) = lbl_805EDF4C;
        fn_8049F2A4(&RW_PIPE_GLOBAL(u8, 4));
        RW_PIPE_GLOBAL(RwInt32, 0x34) = 0;
        RW_PIPE_GLOBAL(RwInt32, 0x30) = 0;
        lbl_805F1698 = 1;
        return 1;
    }
    return 0;
}

/* RxHeapGetGlobalHeap */
void *fn_8049B66C(void) {
    return lbl_805F169C;
}

/* The pipeline being executed and the parameters its nodes get (RxPipelineNodeParam). */
typedef struct RwDlExecution {
    RwDlPipeline *pipeline; /* 0x00 */
    u32 unk04;              /* 0x04 */
    RwBool result;          /* 0x08 */
    u32 unk0C;              /* 0x0C */
    void *dataParam;        /* 0x10 */
    void *heap;             /* 0x14 */
} RwDlExecution;

extern RwDlExecution lbl_805E3260;
void fn_8049ECFC(void *heap); /* RxHeapReset */

typedef void *(*RwDlNodeBody)(RwDlPipelineNode *node, void **params);

/* RxPipelineExecute: runs the first node, which calls the others.
 * NonMatching (10 rows): retail forms the context's address after the heap reset yet keeps it
 * in r31 and the parameters in r30; formed there, it gets r30 and the pipeline r31 (21 rows).
 * Retail also stores the node's zero result itself where this loads a 0. Probes:
 * probes/Game_RwCpp61_R5.py. */
RwDlPipeline *fn_8049B674(RwDlPipeline *pipeline, void *data, RwBool heapReset) {
    RwDlExecution *exec;
    void **params;
    exec = &lbl_805E3260;
    params = &exec->dataParam;
    if (heapReset && *(RwInt32 *)((u8 *)lbl_805F169C + 0x18) != 0) fn_8049ECFC(lbl_805F169C);
    exec->result = 1;
    exec->pipeline = pipeline;
    exec->dataParam = data;
    exec->heap = lbl_805F169C;
    pipeline->embeddedPacketState = 0;
    if (((RwDlNodeBody)pipeline->nodes->nodeDef->nodeMethods[0])(pipeline->nodes, params) == NULL) {
        exec->result = 0;
    }
    if (pipeline->embeddedPacketState > 1) {
        pipeline->embeddedPacketState = 2;
        fn_8049B14C(pipeline->embeddedPacket);
    }
    *params = NULL;
    lbl_805E3260.pipeline = NULL;
    exec->heap = NULL;
    if (exec->result) return pipeline;
    return NULL;
}


/* RxPipelineCreate */
RwDlPipeline *fn_8049B76C(void) {
    RwDlPipeline *pipeline =
        (RwDlPipeline *)lbl_805F15B4->freeListAlloc(RW_PIPE_GLOBAL(void *, 0), 0x30409);
    if (pipeline != NULL) {
        memset(pipeline, 0, sizeof(RwDlPipeline));
        pipeline->locked = 0;
        return pipeline;
    }
    RW_ERROR_STRING(0x80000013, sizeof(RwDlPipeline));
    return NULL;
}

/* RxPipelineDestroy */
void fn_8049B800(RwDlPipeline *pipeline) {
    if (pipeline != NULL) {
        RwUInt32 numNodes;
        RwUInt32 i;
        RwDlPipelineNode *node = pipeline->nodes;
        i = 0;
        numNodes = pipeline->numNodes;
        for (; i < numNodes; i++) {
            fn_8049B350(node, pipeline);
            node++;
        }
        pipeline->nodes = NULL;
        if (pipeline->superBlock != NULL) {
            lbl_805F15B4->free(pipeline->superBlock);
            pipeline->superBlock = NULL;
            pipeline->superBlockSize = 0;
        }
        lbl_805F15B4->freeListFree(RW_PIPE_GLOBAL(void *, 0), pipeline);
    }
}

/* Temporary memory from the top of the stack.
 * NonMatching, like fn_8049B92C: retail keeps the aligned size in r4 and the stack pointer in r5,
 * this the other way round (5 rows each). Array, struct, integer and pointer forms of the stack,
 * the alignment in the parameter, a local, each expression or an inline function, and compound
 * or plain assignments were tried: probes/Game_RwCpp61*.py. */
void *fn_8049B8BC(RwUInt32 size) {
    size = (size + 3) & ~3;
    lbl_805EDF58[0] -= size;
    if (lbl_805EDF58[0] < lbl_805EDF58[1]) {
        lbl_805EDF58[0] += size;
        RW_ERROR(0x80000013);
        return NULL;
    }
    return lbl_805EDF58[0];
}

/* Temporary memory from the bottom of the stack. */
void *fn_8049B92C(RwUInt32 size) {
    size = (size + 3) & ~3;
    lbl_805EDF58[1] += size;
    if (lbl_805EDF58[1] > lbl_805EDF58[0]) {
        lbl_805EDF58[1] -= size;
        RW_ERROR(0x80000013);
        return NULL;
    }
    return lbl_805EDF58[1] - size;
}

/* Sets up a new node at the end of an unlocked pipeline: its output and sort data live in the
 * pipeline's node block, after the nodes.
 * NonMatching (15 rows): retail adds the node block's base first, (nodes + max * 40) + n * 128;
 * every form tried (probes/Game_RwCpp61_R6.py) adds it last. */
static inline RwUInt32 *RwDlNodeBlockEnd(RwDlPipeline *pipeline) {
    return (RwUInt32 *)(pipeline->nodes + RW_PIPE_GLOBAL(RwUInt32, 0x38));
}

RwBool fn_8049BC4C(RwDlPipeline *pipeline, RwDlPipelineNode *node, RwDlNodeDefinition *nodeDef) {
    RwBool ok = 1;
    RwUInt32 numOutputs = nodeDef->numOutputs;
    memset(node, 0, sizeof(RwDlPipelineNode));
    if (numOutputs > 32) {
        RW_ERROR(0x29);
        ok = 0;
    }
    if (nodeDef->numClustersOfInterest > 32) {
        RW_ERROR(0x28);
        ok = 0;
    }
    if (numOutputs >= RW_PIPE_GLOBAL(RwUInt32, 0x38)) {
        RW_ERROR(0x2A);
        ok = 0;
    }
    if (ok) {
        RwUInt32 i;
        RwUInt32 *outputs;
        RwDlTopSortData *sort;
        outputs = (RwUInt32 *)(pipeline->nodes + RW_PIPE_GLOBAL(RwUInt32, 0x38)) + pipeline->numNodes * 32;
        node->outputs = outputs;
        node->numOutputs = numOutputs;
        for (i = 0; i < node->numOutputs; i++) outputs[i] = (RwUInt32)-1;
        sort = (RwDlTopSortData *)((RwUInt32 *)(pipeline->nodes + RW_PIPE_GLOBAL(RwUInt32, 0x38)) + RW_PIPE_GLOBAL(RwUInt32, 0x38) * 32) + pipeline->numNodes;
        sort->word[0] = 0;
        sort->word[1] = 0;
        sort->word[2] = 0;
        node->topSortData = sort;
        node->initializationData = NULL;
        node->initializationDataSize = 0;
        node->nodeDef = nodeDef;
        pipeline->numNodes++;
    }
    return ok;
}


/* The number of distinct clusters the nodes of a pipeline use. */
RwUInt32 fn_8049B9A4(RwDlPipeline *pipeline) {
    void *prev;
    void *next;
    RwUInt32 count;
    count = 0;
    next = NULL;
    for (;;) {
        RwUInt32 i;
        prev = next;
        next = (void *)-1;
        for (i = 0; i < pipeline->numNodes; i++) {
            RwDlNodeDefinition *def = pipeline->nodes[i].nodeDef;
            RwUInt32 j;
            for (j = 0; j < def->numClustersOfInterest; j++) {
                void *cluster = def->clustersOfInterest[j].clusterDef;
                if (cluster > prev && cluster < next) next = cluster;
            }
        }
        if (next == (void *)-1) break;
        count++;
    }
    return count;
}
}

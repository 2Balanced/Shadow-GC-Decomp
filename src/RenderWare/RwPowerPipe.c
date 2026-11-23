/* RenderWare 3.7 PowerPipe: pipelines (0x8049B0CC..0x8049D933). The pipeline module's open and
 * close (an engine plugin: the global RxHeap and the pipeline free list), packets, RxPipelineExecute,
 * RxPipelineCreate and its destruction, the temporary memory the unlocking pass allocates from,
 * the node set-up of locked pipelines, RxLockedPipeUnlock, RxPipelineFindNodeByName,
 * RxLockedPipeAddFragment, RxLockedPipeAddPath and the requirements lists of the unlocking pass.
 * The file owns the module's settings and the temporary memory's bounds (.sdata 0x805EDF48..
 * 0x805EDF5F), the execution context and the free list's record (.bss 0x805E3260..0x805E329F) and
 * the module's globals offset, heap and open flag (.sbss 0x805F1690..0x805F169F).
 *
 * Built as C with the driver's flags plus deferred inlining, as the rest of PowerPipe (old
 * Game/RwCpp63, agent r8, wave 40): retail emits these functions in the reverse of their
 * definition order and lays the .bss out in the reverse of its definition order (the execution
 * context, used by fn_8049B674, sits before the free list fn_8049B258 creates, which only
 * deferred code generation gives within one file), so the definitions below stand in reverse
 * address order.
 *
 * Boundary: the dependency chase that follows (0x8049D934..0x8049EA2B) is treated as a file of its
 * own (old Game/RwCpp63, range r8). fn_8049D408..fn_8049D4FC, the requirements list helpers, are
 * called only from it, and fn_8049D934 inlines a copy of fn_8049D408's search: they may belong
 * to that file instead (then it would start at 0x8049D408).
 *
 * Names are inferred from behaviour.
 * Not written yet: fn_8049B350, fn_8049BA30, fn_8049BDF0, fn_8049C1E0 (RxLockedPipeUnlock),
 * fn_8049CCB8, fn_8049D0D0 (RxLockedPipeAddFragment) and fn_8049D4FC.
 * NonMatching: fn_8049B674 (10 rows), fn_8049B8BC and fn_8049B92C (5 rows each), fn_8049BC4C
 * (15 rows); see the notes there. */
#include <dolphin/types.h>
#include "Game/rw/r7.h"

void *memset(void *dst, int c, unsigned long n);

extern RwDlGlobals *lbl_805F15B4;  /* RwEngineInstance */

void *fn_8049F168(RwUInt32 size);  /* RxHeapCreate */
void fn_8049F0CC(void *heap);      /* RxHeapDestroy */
void fn_8049EA2C(void *heap, void *block); /* RxHeapFree */
void fn_8049ECFC(void *heap);      /* RxHeapReset */
void fn_8049F2A4(void *vector);    /* RxRenderStateVectorSetDefaultRenderStateVector */
void *fn_8047FC04(RwInt32 size, RwInt32 entries, RwInt32 align, RwInt32 blocks, void *freeList,
                  RwUInt32 hint);  /* RwFreeListCreateAndPreallocateSpace */
void fn_8047FCDC(void *freeList);  /* RwFreeListDestroy */
void fn_8049B350(RwDlPipelineNode *node, RwDlPipeline *pipeline);

/* .sbss (defined in reverse: MWCC lays them out from the last definition). */
void *lbl_805F169C;                /* the global RxHeap */
RwBool lbl_805F1698;               /* PowerPipe is open */
RwDlModuleInfo lbl_805F1690;       /* the module's globals offset in RwEngineInstance */

/* .sdata */
RwUInt32 lbl_805EDF48 = 0x1000;    /* the global heap's size */
RwInt32 lbl_805EDF4C = 0x40;       /* the largest number of nodes a pipeline has */
RwInt32 lbl_805EDF50 = 0x40;       /* pipelines per free list block */
RwInt32 lbl_805EDF54 = 1;          /* free list blocks to preallocate */
u8 *lbl_805EDF58[2] = {0, 0};      /* the temporary memory: [0] top, [1] bottom */

/* The pipeline being executed and the parameters its nodes get (RxPipelineNodeParam). */
typedef struct RwDlExecution {
    RwDlPipeline *pipeline; /* 0x00 */
    u32 unk04;              /* 0x04 */
    RwBool result;          /* 0x08 */
    u32 unk0C;              /* 0x0C */
    void *dataParam;        /* 0x10 */
    void *heap;             /* 0x14 */
} RwDlExecution;

/* .bss (deferred code generation lays it out in the reverse of the definition order). */
static u8 sPipelineFreeList[0x28]; /* the pipeline free list's own record */
static RwDlExecution sExecution;   /* the pipeline being executed */

#define RW_PIPE_GLOBAL(type, offset) (*(type *)((u8 *)lbl_805F15B4 + lbl_805F1690.globalsOffset + (offset)))

/* An entry of the requirements lists (0x24 bytes). */
typedef struct RwDlRequirement {
    u32 word[9];
} RwDlRequirement;

typedef struct RwDlRequirementList {
    RwUInt32 count;            /* 0x00 */
    u32 unk04[2];
    RwDlRequirement *entries;  /* 0x0C */
} RwDlRequirementList;

/* The index of a cluster in a node's interests, or -1. */
RwUInt32 fn_8049D4B8(RwUInt32 *io, void *clusterDef) {
    RwUInt32 i;
    for (i = 0; i < io[0]; i++) {
        if (((RwDlClusterRef *)io[1])[i].clusterDef == clusterDef) return i;
    }
    return (RwUInt32)-1;
}

/* Removes an entry from a requirements list (the last one takes its place). Built as C the
 * structure copy is word by word and matches (as C++ it missed 11 rows). */
void fn_8049D43C(RwDlRequirementList *list, RwDlRequirement *entry) {
    if (entry != &list->entries[list->count - 1]) {
        *entry = (list->entries + list->count)[-1];
    }
    list->count--;
}

/* The entry of a requirements list for a cluster, or NULL. */
u32 fn_8049D408(u32 *object, u32 value) {
    u32 count = object[0];
    if (count != 0) {
        u32 *entries = (u32 *)object[3];
        u32 i;
        for (i = 0; i < count; i++) {
            if (entries[0] == value) {
                return (u32)entries;
            }
            entries += 9;
        }
    }
    return 0;
}

/* The index of a node in its pipeline, or -1. */
static inline RwUInt32 RwDlNodeIndex(RwDlPipeline *pipeline, RwDlPipelineNode *node) {
    RwUInt32 index = node - pipeline->nodes;
    if (&pipeline->nodes[index] == node && index < pipeline->numNodes) return index;
    return (RwUInt32)-1;
}

/* RxLockedPipeAddPath: connects an unconnected output of a locked pipeline to a node of it. */
RwDlPipeline *fn_8049D368(RwDlPipeline *pipeline, RwUInt32 *index, RwDlPipelineNode *node) {
    if (pipeline != NULL && pipeline->locked && index != NULL && *index == (RwUInt32)-1 &&
        node != NULL && node->nodeDef != NULL) {
        RwUInt32 found = RwDlNodeIndex(pipeline, node);
        if (found != (RwUInt32)-1) {
            *index = found;
            return pipeline;
        }
    }
    return NULL;
}

/* RxPipelineFindNodeByName: the first node after `start` (or the first of all) whose
 * definition has that name. */
RwDlPipelineNode *fn_8049CFCC(RwDlPipeline *pipeline, const char *name, RwDlPipelineNode *start,
                              RwInt32 *nodeIndex) {
    RwBool valid = 0;
    if (pipeline != NULL && name != NULL && pipeline->numNodes != 0) valid = 1;
    if (valid) {
        RwDlPipelineNode *node = pipeline->nodes;
        RwInt32 count = pipeline->numNodes;
        if (start != NULL) {
            while (node != start && count > 0) {
                node++;
                count--;
            }
            node++;
            count--;
        }
        while (count > 0) {
            if (node->nodeDef != NULL && lbl_805F15B4->strcmp(node->nodeDef->name, name) == 0) {
                if (nodeIndex != NULL) *nodeIndex = pipeline->numNodes - count;
                return node;
            }
            node++;
            count--;
        }
    }
    if (nodeIndex != NULL) *nodeIndex = -1;
    return NULL;
}

/* Sets up a new node at the end of an unlocked pipeline: its output and sort data live in the
 * pipeline's node block, after the nodes.
 * NonMatching (15 rows): retail adds the node block's base first, (nodes + max * 40) + n * 128;
 * every form tried (probes/Game_RwCpp61_R6.py) adds it last. */
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
        sort = (RwDlTopSortData *)((RwUInt32 *)(pipeline->nodes + RW_PIPE_GLOBAL(RwUInt32, 0x38)) +
                                   RW_PIPE_GLOBAL(RwUInt32, 0x38) * 32) + pipeline->numNodes;
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

void fn_8049B14C(RwDlPacket *packet);

typedef void *(*RwDlNodeBody)(RwDlPipelineNode *node, void **params);

/* RxPipelineExecute: runs the first node, which calls the others.
 * NonMatching (10 rows): retail forms the context's address after the heap reset yet keeps it
 * in r31 and the parameters in r30; formed there, it gets r30 and the pipeline r31 (21 rows).
 * Retail also stores the node's zero result itself where this loads a 0. Probes:
 * probes/Game_RwCpp61_R5.py. */
RwDlPipeline *fn_8049B674(RwDlPipeline *pipeline, void *data, RwBool heapReset) {
    RwDlExecution *exec;
    void **params;
    exec = &sExecution;
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
    sExecution.pipeline = NULL;
    exec->heap = NULL;
    if (exec->result) return pipeline;
    return NULL;
}

/* RxHeapGetGlobalHeap */
void *fn_8049B66C(void) {
    return lbl_805F169C;
}

/* Starts PowerPipe: the global heap and the pipeline free list. */
RwBool fn_8049B258(void) {
    if (!lbl_805F1698) {
        lbl_805F169C = fn_8049F168(lbl_805EDF48);
        if (lbl_805F169C == NULL) return 0;
        RW_PIPE_GLOBAL(void *, 0) =
            fn_8047FC04(sizeof(RwDlPipeline), lbl_805EDF50, 4, lbl_805EDF54, sPipelineFreeList, 0x40409);
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

void fn_8049B148(void) {
}

RwBool fn_8049B140(void) {
    return 1;
}

/* PowerPipe's close (an engine plugin destructor). */
void *fn_8049B110(void *instance) {
    fn_8049B1F8();
    return instance;
}

/* PowerPipe's open (an engine plugin constructor). */
void *fn_8049B0CC(void *instance, RwInt32 offset, RwInt32 size) {
    lbl_805F1690.globalsOffset = offset;
    if (fn_8049B258()) return instance;
    return NULL;
}

/* RenderWare PowerPipe internals, 0x8049D934..0x8049EA2C: the tail of the dependency chase run
 * when a locked pipeline is unlocked (five passes over the pipeline's nodes and the driver
 * fn_8049E9AC).  The chase's source file starts at 0x8049D408, in range r7: its helpers
 * fn_8049D408, fn_8049D43C and fn_8049D4B8 are expanded inside fn_8049D4FC (twice, recursively)
 * and inside fn_8049D934, so they share one file.  Wave 41 left this part in the old size cut;
 * the heap and the render state vector that followed it are now RenderWare/RxHeap.c and
 * RenderWare/RxRenderStateVector.c.
 * Functions keep their fn_ symbols; comments give the public API name where the behaviour fits.
 * Definitions stand in reverse address order: the compiler emits them in reverse.
 * Status: fn_8049E438, fn_8049E4F4 and fn_8049E9AC match; fn_8049E1A4 is a draft (register
 * allocation of the merge loop).  fn_8049D934, fn_8049DE44 and fn_8049E54C are not written yet. */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

#ifdef __cplusplus
extern "C" {
#endif
void *fn_8049B8BC(u32 size); /* scratch allocations of the pipeline unlock (two bump allocators) */
void *fn_8049B92C(u32 size);
u32 fn_8049B9A4(RxPipeline *pipeline);
u32 fn_8049D934(RxPipeline *pipeline);
u32 fn_8049DE44(RxPipeline *pipeline);
u32 fn_8049E1A4(RxPipeline *pipeline, RxDepGroup **list);
u32 fn_8049E438(RxPipeline *pipeline, RxDepGroup **list);
u32 fn_8049E54C(RxPipeline *pipeline, RxDepGroup *list);
void fn_8049E4F4(RxClusterDefinition *clusterDef, void *data, RxPipelineCluster **first);

/* The entry of a node's requirement list, or NULL past its end. */
#define DepReqEntry(req, i) ((i) < (req)->numEntries ? &(req)->entries[i] : NULL)

/* The entry of a node's requirement list for a cluster definition. */
static inline RxDepEntry *DepReqFind(RxDepReq *req, RxClusterDefinition *clusterDef) {
    u32 i;
    RxDepEntry *entry = req->entries;
    for (i = 0; i < req->numEntries; i++) {
        if (entry->clusterDef == clusterDef) return entry;
        entry++;
    }
    return NULL;
}

static inline RxDepGroup *DepGroupCreate(RxDepGroup **list) {
    RxDepGroup *group = (RxDepGroup *)fn_8049B8BC(sizeof(RxDepGroup));
    if (group != NULL) {
        group->chain = NULL;
        group->entries = NULL;
        group->parent = NULL;
        group->next = *list;
        *list = group;
        return group;
    }
    return NULL;
}


/* _rxChaseDependencies: the passes that work out where every cluster lives in the packet; each
 * returns 0 or the error code it reported. */
u32 fn_8049E9AC(RxPipeline *pipeline) {
    RxDepGroup *list = NULL;
    u32 result;

    if ((result = fn_8049D934(pipeline)) == 0 && (result = fn_8049DE44(pipeline)) == 0 &&
        (result = fn_8049E1A4(pipeline, &list)) == 0 &&
        (result = fn_8049E438(pipeline, &list)) == 0) {
        result = fn_8049E54C(pipeline, list);
    }
    return result;
}

/* Appends the RxPipelineCluster of a definition to the array that starts at *first (the
 * records come out back to back from the scratch allocator). */
void fn_8049E4F4(RxClusterDefinition *clusterDef, void *data, RxPipelineCluster **first) {
    RxPipelineCluster *cluster = (RxPipelineCluster *)fn_8049B92C(sizeof(RxPipelineCluster));
    if (*first == NULL) *first = cluster;
    cluster->clusterRef = clusterDef;
    cluster->creationAttributes = clusterDef->defaultAttributes;
}

/* Gives every set of entries a packet slot no entry of it finds taken at its node, and the
 * pipeline as many slots as that needs. */
u32 fn_8049E438(RxPipeline *pipeline, RxDepGroup **list) {
    RxDepGroup *group;
    u32 taken;
    u32 numSlots;
    RxDepGroup *root;
    RxDepEntry *entry;
    u32 slot;
    RxDepGroup *group2;
    RxDepEntry *entry2;

    numSlots = 0;
    for (root = *list; root != NULL; root = root->next) {
        taken = 0;
        group = root;
        do {
            for (entry = group->entries; entry != NULL; entry = entry->next) {
                taken |= entry->req->slotsInUse;
            }
            group = group->chain;
        } while (group != NULL);
        for (slot = 0; taken & 1; slot++) taken >>= 1;
        if (slot >= numSlots) numSlots = slot + 1;
        group2 = root;
        do {
            for (entry2 = group2->entries; entry2 != NULL; entry2 = entry2->next) {
                entry2->slot = slot;
                entry2->req->slotsInUse |= 1 << slot;
            }
            group2 = group2->chain;
        } while (group2 != NULL);
    }
    pipeline->packetNumClusterSlots = numSlots;
    return 0;
}

/* Puts every entry in a set: a cluster that leaves a node by an output joins the set of the
 * same cluster at the node that output leads to; sets that meet are merged. */
u32 fn_8049E1A4(RxPipeline *pipeline, RxDepGroup **list) {
    u32 numNodes = pipeline->numNodes;
    RxPipelineNode *node = pipeline->nodes;

    do {
        u32 i;
        u32 out;
        for (i = 0; i < node->topSortData->req->numEntries; i++) {
            RxDepEntry *entry = DepReqEntry(node->topSortData->req, i);
            if (entry->group == NULL) {
                if ((entry->group = DepGroupCreate(list)) == NULL) {
                    RWERROR(1, (0x20));
                    return 0x20;
                }
                entry->next = entry->group->entries;
                entry->group->entries = entry;
            }
        }
        for (out = 0; out < node->numOutputs; out++) {
            if (node->outputs[out] != (u32)-1) {
                RxPipelineNode *dest = &pipeline->nodes[node->outputs[out]];
                u32 mask = 1 << out;
                u32 j;
                for (j = 0; j < dest->topSortData->req->numEntries; j++) {
                    RxDepEntry *entry = DepReqEntry(dest->topSortData->req, j);
                    if (entry->required != 0) {
                        RxDepEntry *source = DepReqFind(node->topSortData->req, entry->clusterDef);
                        if (source != NULL && (source->outputMask & mask)) {
                            if (entry->group == NULL) {
                                entry->next = source->group->entries;
                                source->group->entries = entry;
                                entry->group = source->group;
                            } else {
                                RxDepGroup *a = entry->group;
                                RxDepGroup *b = source->group;
                                while (a->parent != NULL) a = a->parent;
                                while (b->parent != NULL) b = b->parent;
                                if (a != b) {
                                    RxDepGroup *last;
                                    RxDepGroup **link;
                                    for (last = a; last->chain != NULL; last = last->chain) {
                                    }
                                    last->chain = b;
                                    b->parent = a;
                                    for (link = list; *link != b; link = &(*link)->next) {
                                    }
                                    *link = b->next;
                                }
                            }
                        }
                    }
                }
            }
        }
        node++;
    } while (--numNodes != 0);
    return 0;
}

#ifdef __cplusplus
}
#endif

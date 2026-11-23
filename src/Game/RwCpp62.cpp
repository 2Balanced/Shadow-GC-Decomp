/* RenderWare 0x8049C1E0..0x8049D933: PowerPipe's pipeline construction (locking, node lookup,
 * the requirements lists the unlocking pass builds). Names are inferred from behaviour;
 * definitions are in retail address order.
 *
 * Not written yet: fn_8049C1E0, fn_8049CCB8, fn_8049D0D0 and fn_8049D4FC.
 * NonMatching: fn_8049D43C (11 rows). The rest match. */
#include <dolphin/types.h>
#include "Game/rw/r7.h"

extern "C" {
extern RwDlGlobals *lbl_805F15B4; /* RwEngineInstance */

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

/* The index of a node in its pipeline, or -1. */
static inline RwUInt32 RwDlNodeIndex(RwDlPipeline *pipeline, RwDlPipelineNode *node) {
    RwUInt32 index = node - pipeline->nodes;
    if (&pipeline->nodes[index] == node && index < pipeline->numNodes) return index;
    return (RwUInt32)-1;
}

/* Sets an unset node index of a locked pipeline from a node of it. */
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

u32 fn_8049D408(u32 *object, u32 value) {
    u32 count = object[0];
    if (count != 0) {
        u32 *entries = (u32 *)object[3];
        for (u32 i = 0; i < count; i++) {
            if (entries[0] == value) {
                return (u32)entries;
            }
            entries += 9;
        }
    }
    return 0;
}

/* An entry of the requirements lists (0x24 bytes). */
typedef struct RwDlRequirement {
    u32 word[9];
} RwDlRequirement;

typedef struct RwDlRequirementList {
    RwUInt32 count;            /* 0x00 */
    u32 unk04[2];
    RwDlRequirement *entries;  /* 0x0C */
} RwDlRequirementList;

/* Removes an entry from a requirements list (the last one takes its place).
 * NonMatching (11 rows): retail copies from entries + count * 36 with displacements -36..-4;
 * every address form tried (probes/Game_RwCpp62.py) folds the -36 into the base instead. */
void fn_8049D43C(RwDlRequirementList *list, RwDlRequirement *entry) {
    if (entry != &list->entries[list->count - 1]) {
        *entry = (list->entries + list->count)[-1];
    }
    list->count--;
}

/* The index of a cluster in a node's interests, or -1. */
RwUInt32 fn_8049D4B8(RwUInt32 *io, void *clusterDef) {
    RwUInt32 i;
    for (i = 0; i < io[0]; i++) {
        if (((RwDlClusterRef *)io[1])[i].clusterDef == clusterDef) return i;
    }
    return (RwUInt32)-1;
}
}

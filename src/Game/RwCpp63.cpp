/* RenderWare PowerPipe internals, 0x8049D934..0x8049F3F8.  Two source files meet in this cut:
 *   0x8049D934..0x8049EA2C  the dependency chase run when a locked pipeline is unlocked: five
 *                           passes over the pipeline's nodes and the driver fn_8049E9AC
 *   0x8049EA2C..0x8049F2A4  the pipelines' heap (RxHeapFree, _rxHeapReset, RxHeapDestroy,
 *                           RxHeapCreate)
 *   0x8049F2A4              RxRenderStateVectorSetDefaultRenderStateVector
 * Functions keep their fn_ symbols; comments give the public API name where the behaviour fits.
 * Definitions stand in reverse address order: the compiler emits them in reverse.
 * Status: fn_8049E438, fn_8049E4F4, fn_8049E9AC, fn_8049ECFC, fn_8049F0CC, fn_8049F168 match.
 * fn_8049EA2C (RxHeapFree) has retail's instructions but its saved registers permuted (retail:
 * free entry r31, header r30, heap r29; ~25 spellings tried); fn_8049F2A4 differs only in where
 * the scheduler puts the colour load (3 rows); fn_8049E1A4 is a draft (register allocation of
 * the merge loop).  fn_8049D934, fn_8049DE44 and fn_8049E54C are not written yet. */
#include <dolphin/types.h>
#include "Game/rw/r8.h"

#ifdef __cplusplus
extern "C" {
#endif
extern s32 lbl_805F1690;           /* offset of the pipeline module's globals in RwEngineInstance */
extern const RwRGBA lbl_805FB430;  /* opaque white */

typedef struct RxPipelineGlobals {
    u32 unk0;
    RxRenderStateVector defaultRenderState; /* 0x04 */
} RxPipelineGlobals;

#define RXPIPELINEGLOBALS() ((RxPipelineGlobals *)((u8 *)RwEngineInstance + lbl_805F1690))

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

/* RxRenderStateVectorSetDefaultRenderStateVector */
RxRenderStateVector *fn_8049F2A4(RxRenderStateVector *rsvp) {
    if (rsvp != NULL) {
        if (RwEngineInstance->engineStatus == 3) {
            *rsvp = RXPIPELINEGLOBALS()->defaultRenderState;
            return rsvp;
        }
        if (rsvp != &RXPIPELINEGLOBALS()->defaultRenderState) {
            RWERROR(1, (0x80000018));
            return NULL;
        }
        {
            RwRGBA white = lbl_805FB430;
            rsvp->flags = 7;
            rsvp->shadeMode = 2;
            rsvp->srcBlend = 5;
            rsvp->destBlend = 6;
            rsvp->textureRaster = NULL;
            rsvp->addressModeU = 1;
            rsvp->addressModeV = 1;
            rsvp->filterMode = 2;
            rsvp->borderColor = white;
            rsvp->fogType = 0;
            rsvp->fogColor = white;
        }
        return rsvp;
    }
    RWERROR(1, (0x80000016));
    return NULL;
}

/* A super block: one allocation holding the descriptor and 128-byte aligned block space. */
static inline RxHeapSuperBlock *HeapSuperBlockCreate(u32 size) {
    RxHeapSuperBlock *sb = (RxHeapSuperBlock *)RwEngineInstance->memAlloc(
        size + sizeof(RxHeapSuperBlock) + 127, 0x1040409);
    if (sb != NULL) {
        sb->start = (void *)(((u32)(sb + 1) + 127) & ~127);
        sb->size = size;
        sb->next = NULL;
    }
    return sb;
}

static inline void HeapSuperBlockDestroy(RxHeapSuperBlock *sb) {
    if (sb != NULL) {
        RwEngineInstance->memFree(sb);
    }
}

s32 fn_8049ECFC(RxHeap *heap);

/* RxHeapCreate */
RxHeap *fn_8049F168(u32 size) {
    RxHeap *heap;
    RxHeapSuperBlock *sb;

    if (size < 1024) size = 1024;
    heap = (RxHeap *)RwEngineInstance->memAlloc(sizeof(RxHeap), 0x40409);
    if (heap != NULL) {
        size = (size + 31) & ~31;
        if (size < 128) size = 128;
        sb = HeapSuperBlockCreate(size);
        if (sb != NULL) {
            heap->superBlockSize = size;
            heap->head = sb;
            heap->freeBlocks = NULL;
            heap->entriesAlloced = 0;
            heap->entriesUsed = 0;
            heap->dirty = TRUE;
            if (!heap->dirty ? TRUE : fn_8049ECFC(heap)) return heap;
            HeapSuperBlockDestroy(sb);
        }
        RwEngineInstance->memFree(heap);
    }
    return NULL;
}

/* RxHeapDestroy */
void fn_8049F0CC(RxHeap *heap) {
    if (heap != NULL) {
        RxHeapSuperBlock *sb;
        if (heap->freeBlocks != NULL) {
            RwEngineInstance->memFree(heap->freeBlocks);
            heap->freeBlocks = NULL;
        }
        sb = heap->head;
        while (sb != NULL) {
            RxHeapSuperBlock *next = sb->next;
            HeapSuperBlockDestroy(sb);
            sb = next;
        }
        RwEngineInstance->memFree(heap);
    }
}

/* Takes an entry of the free block table, growing the table by 32 entries when it is full. */
static inline RxHeapFreeEntry *HeapFreeEntryAlloc(RxHeap *heap) {
    u32 used = heap->entriesUsed;
    RxHeapFreeEntry *entries = heap->freeBlocks;

    if (heap->entriesAlloced <= used) {
        u32 alloced = heap->entriesAlloced + 32;
        heap->entriesAlloced = alloced;
        entries = (RxHeapFreeEntry *)RwEngineInstance->memRealloc(
            entries, alloced * sizeof(RxHeapFreeEntry), 0x1030409);
        if (entries == NULL) {
            RWERROR(1, (0x80000013, heap->entriesAlloced * sizeof(RxHeapFreeEntry)));
            heap->entriesAlloced -= 32;
        } else {
            if (entries != heap->freeBlocks && used != 0) {
                RxHeapFreeEntry *entry = entries;
                do {
                    entry->ptr->freeEntry = entry;
                    entry++;
                } while (--used != 0);
            }
            heap->freeBlocks = entries;
        }
    }
    if (entries != NULL) {
        entries += heap->entriesUsed++;
    }
    return entries;
}

/* Lays a super block out as one free block between two empty end markers and links it after
 * the previous super block's end marker. */
static inline s32 HeapSuperBlockInit(RxHeap *heap, RxHeapSuperBlock *sb, RxHeapSuperBlock *prev) {
    RxHeapFreeEntry *entry = HeapFreeEntryAlloc(heap);

    if (entry != NULL) {
        RxHeapBlockHeader *first = (RxHeapBlockHeader *)sb->start;
        RxHeapBlockHeader *block = first + 1;
        RxHeapBlockHeader *last = (RxHeapBlockHeader *)((u8 *)first + sb->size) - 1;

        first->prev = NULL;
        first->next = NULL;
        first->size = 0;
        first->freeEntry = NULL;
        *last = *first;
        first->next = block;
        block->prev = first;
        block->next = last;
        last->prev = block;
        block->size = (u8 *)last - (u8 *)(block + 1);
        block->freeEntry = entry;
        entry->ptr = block;
        entry->size = block->size;
        if (prev != NULL) {
            RxHeapBlockHeader *prevLast = (RxHeapBlockHeader *)((u8 *)prev->start + prev->size) - 1;
            prevLast->next = first;
            first->prev = prevLast;
        }
        return TRUE;
    }
    return FALSE;
}

/* _rxHeapReset */
s32 fn_8049ECFC(RxHeap *heap) {
    RxHeapSuperBlock *prev;
    RxHeapSuperBlock *sb;

    heap->entriesUsed = 0;
    prev = NULL;
    sb = heap->head->next;
    while (sb != NULL) {
        if (!HeapSuperBlockInit(heap, sb, prev)) return FALSE;
        if (prev == NULL) heap->headBlock = (RxHeapBlockHeader *)sb->start;
        prev = sb;
        sb = sb->next;
    }
    sb = heap->head;
    if (!HeapSuperBlockInit(heap, sb, prev)) return FALSE;
    if (prev == NULL) heap->headBlock = (RxHeapBlockHeader *)sb->start;
    heap->dirty = FALSE;
    return TRUE;
}

/* RxHeapFree: returns a block, merging it with free neighbours. */
void fn_8049EA2C(RxHeap *heap, void *block) {
    RxHeapBlockHeader *hdr = (RxHeapBlockHeader *)block - 1;
    s32 prevFree = (hdr->prev != NULL && hdr->prev->freeEntry != NULL);
    s32 nextFree = (hdr->next != NULL && hdr->next->freeEntry != NULL);

    if (prevFree) {
        if (nextFree) {
            RxHeapFreeEntry *entry = hdr->next->freeEntry;
            if (&heap->freeBlocks[heap->entriesUsed - 1] != entry) {
                *entry = heap->freeBlocks[heap->entriesUsed - 1];
                entry->ptr->freeEntry = entry;
            }
            heap->entriesUsed--;
            hdr->prev->size += hdr->size + hdr->next->size + 2 * sizeof(RxHeapBlockHeader);
            hdr->prev->freeEntry->size = hdr->prev->size;
            hdr->prev->next = hdr->next->next;
            if (hdr->next->next != NULL) hdr->next->next->prev = hdr->prev;
        } else {
            hdr->prev->size += hdr->size + sizeof(RxHeapBlockHeader);
            hdr->prev->freeEntry->size = hdr->prev->size;
            hdr->prev->next = hdr->next;
            if (hdr->next != NULL) hdr->next->prev = hdr->prev;
        }
    } else if (nextFree) {
        hdr->size += hdr->next->size + sizeof(RxHeapBlockHeader);
        hdr->freeEntry = hdr->next->freeEntry;
        hdr->next->freeEntry->ptr = hdr;
        hdr->next->freeEntry->size = hdr->size;
        hdr->next = hdr->next->next;
        if (hdr->next != NULL) hdr->next->prev = hdr;
    } else {
        RxHeapFreeEntry *entry = HeapFreeEntryAlloc(heap);
        if (entry != NULL) {
            entry->ptr = hdr;
            entry->size = hdr->size;
            hdr->freeEntry = entry;
        }
    }
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

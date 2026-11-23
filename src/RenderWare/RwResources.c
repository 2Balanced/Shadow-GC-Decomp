/* RenderWare resource arena (RwResources*), 0x8048053C..0x80480C6C: the module's open and close
 * (engine plugin 0x40B), freeing and using entries, allocating an entry (throwing out the oldest
 * ones when the arena is full) and emptying the arena.
 *
 * One source file: its .sbss is the module's RwModuleInfo (0x805F1578). Its first function is in
 * range r5 and it runs into old Game/RwCpp45 (r6); the stream module, which starts at 0x80480C6C
 * with an RwModuleInfo, .sdata and .bss of its own (0x805F1580, 0x805EDEA8, 0x805E2AA8), is r6's.
 * fn_80480878 and fn_80480B08 come from Game/RwCpp45 (agent r6's code, types renamed to r5.h's).
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * C, built with cflags_rw_r5 (-O4,p, no stmw/lmw, automatic inlining in definition order). */
#include "Game/rw/r5.h"

void *fn_80482A94(void *heap, u32 size); /* allocates from the arena heap */
void fn_804829DC(void *mem);             /* frees to it */

/* The resources module's bookkeeping. */
R5ModuleInfo lbl_805F1578;

#define R5RESOURCESGLOBALS (*(R5ResourcesGlobals *)((u8 *)lbl_805F15B4 + lbl_805F1578.globalsOffset))
#define RWRESOURCESGLOBAL(var) (R5RESOURCESGLOBALS.var)

RwBool fn_80480B08(void);
int fn_80482984(void *heap, u32 size); /* initialises the arena heap */
void fn_804829D4(void *heap);           /* releases it */

/* Sets the arena up (inlined into the module open). */
static R5ResourcesGlobals *ResourcesInit(R5ResourcesGlobals *res, u32 size) {
    if (size != 0) {
        res->memHeap = RWSRCGLOBAL(rwmalloc)(size, 0x4040B);
        if (res->memHeap == 0) {
            R5ERROR(1, (0x80000013, size));
            return 0;
        }
        if (!fn_80482984(res->memHeap, size)) {
            RWSRCGLOBAL(rwfree)(res->memHeap);
            R5ERROR(1, (0xC, 0));
            return 0;
        }
    } else {
        res->memHeap = 0;
    }
    res->entriesA.link.next = &res->entriesA.link;
    res->entriesA.link.prev = &res->entriesA.link;
    res->entriesB.link.next = &res->entriesB.link;
    res->entriesB.link.prev = &res->entriesB.link;
    res->usedEntries = &res->entriesA;
    res->freeEntries = &res->entriesB;
    res->maxSize = size;
    res->currentSize = 0;
    res->reusageSize = 0;
    return res;
}

/* Opens the resources module: the arena of RwEngineInit's size. */
void *fn_8048053C(void *instance, s32 offset) {
    lbl_805F1578.globalsOffset = offset;
    if (ResourcesInit(&R5RESOURCESGLOBALS, RWSRCGLOBAL(resArenaInitSize)) == 0) {
        return 0;
    }
    lbl_805F1578.numInstances++;
    return instance;
}

/* Closes it: empties the arena and frees it. */
void *fn_80480690(void *instance) {
    fn_80480B08();
    if (RWRESOURCESGLOBAL(memHeap) != 0) {
        fn_804829D4(RWRESOURCESGLOBAL(memHeap));
        RWSRCGLOBAL(rwfree)(RWRESOURCESGLOBAL(memHeap));
        RWRESOURCESGLOBAL(memHeap) = 0;
    }
    lbl_805F1578.numInstances--;
    return instance;
}

/* RwResourcesFreeResEntry: tells the owner, then gives the entry back to the arena (or frees it
 * when it was allocated outside the arena). Inlined into the functions below. */
RwBool fn_8048071C(R5ResEntry *entry) {
    if (entry->destroyNotify) {
        entry->destroyNotify(entry);
    }
    if (entry->ownerRef) {
        *entry->ownerRef = 0;
    }
    if (entry->link.next) {
        entry->link.prev->next = entry->link.next;
        entry->link.next->prev = entry->link.prev;
        RWRESOURCESGLOBAL(currentSize) -= entry->size;
        fn_804829DC(entry);
    } else {
        RWSRCGLOBAL(rwfree)(entry);
    }
    return 1;
}

/* Starts a new frame of the arena: this frame's entries join last frame's, which become the ones
 * thrown out first, and the emptied list collects the next frame's. */
void fn_804807CC(void) {

    R5LinkList *newer = RWRESOURCESGLOBAL(usedEntries);
    R5LinkList *older = RWRESOURCESGLOBAL(freeEntries);

    if (older->link.next != &older->link) {
        if (newer->link.next == &newer->link) {
            newer->link.next = older->link.next;
            newer->link.next->prev = &newer->link;
            newer->link.prev = older->link.prev;
            newer->link.prev->next = &newer->link;
            older->link.next = &older->link;
            older->link.prev = &older->link;
        } else {
            R5Link *last = newer->link.prev;
            R5Link *first = older->link.next;

            last->next = first;
            first->prev = last;
            first = older->link.prev;
            first->next = &newer->link;
            newer->link.prev = first;
            older->link.next = &older->link;
            older->link.prev = &older->link;
        }
    }
    RWRESOURCESGLOBAL(usedEntries) = older;
    RWRESOURCESGLOBAL(freeEntries) = newer;
    RWRESOURCESGLOBAL(reusageSize) = 0;
}


/* RwResourcesAllocateResEntry: entries are taken from the arena heap; when it is full the oldest
 * entry of last frame's list, then of this frame's, is thrown out and the allocation retried. */
R5ResEntry *fn_80480878(void *owner, R5ResEntry **ownerRef, s32 size,
                        R5ResEntryDestroyNotify destroyNotify) {
    R5ResEntry *entry;
    RwBool failed = 0;

    while (!failed) {
        entry = (R5ResEntry *)fn_80482A94(RWRESOURCESGLOBAL(memHeap), sizeof(R5ResEntry) + size);
        if (entry) {
            entry->link.next = RWRESOURCESGLOBAL(usedEntries)->link.next;
            entry->link.prev = &RWRESOURCESGLOBAL(usedEntries)->link;
            RWRESOURCESGLOBAL(usedEntries)->link.next->prev = &entry->link;
            RWRESOURCESGLOBAL(usedEntries)->link.next = &entry->link;
            entry->owner = owner;
            entry->size = size;
            entry->ownerRef = ownerRef;
            entry->destroyNotify = destroyNotify;
            RWRESOURCESGLOBAL(currentSize) += size;
            if (ownerRef) {
                *ownerRef = entry;
            }
            return entry;
        } else {
            R5ResourcesGlobals *res = &R5RESOURCESGLOBALS;
            R5Link *link = res->freeEntries->link.prev;
            if (link != &res->freeEntries->link) {
                entry = (R5ResEntry *)link;
                fn_8048071C(entry);
            } else {
                entry = (R5ResEntry *)res->usedEntries->link.prev;
                if (&entry->link != &res->usedEntries->link) {
                    res->reusageSize += entry->size;
                    fn_8048071C(entry);
                } else {
                    failed = 1;
                }
            }
        }
    }
    if (ownerRef) {
        *ownerRef = 0;
    }
    R5ERROR(1, (0xC, size));
    return 0;
}

/* RwResourcesEmptyArena. */
RwBool fn_80480B08(void) {
    R5Link *link;
    R5Link *end;

    RWRESOURCESGLOBAL(entriesA).link.prev->next = RWRESOURCESGLOBAL(entriesB).link.next;
    link = RWRESOURCESGLOBAL(entriesA).link.next;
    end = &RWRESOURCESGLOBAL(entriesB).link;
    while (link != end) {
        R5ResEntry *entry = (R5ResEntry *)link;
        link = link->next;
        fn_8048071C(entry);
    }
    RWRESOURCESGLOBAL(entriesA).link.next = &RWRESOURCESGLOBAL(entriesA).link;
    RWRESOURCESGLOBAL(entriesA).link.prev = &RWRESOURCESGLOBAL(entriesA).link;
    RWRESOURCESGLOBAL(entriesB).link.next = &RWRESOURCESGLOBAL(entriesB).link;
    RWRESOURCESGLOBAL(entriesB).link.prev = &RWRESOURCESGLOBAL(entriesB).link;
    RWRESOURCESGLOBAL(reusageSize) = 0;
    return 1;
}

/* RenderWare core: the plugin registry (0x80481B98-0x8048222C).
 * Module open and close (the registry entry free list), the default plugin callbacks,
 * _rwPluginRegistryGetPluginOffset, _rwPluginRegistryAddPlugin and the object init, deinit and
 * copy walks. The file owns the registry list, its count and the entry free list (.sbss), the
 * free list parameters (.sdata) and the free list itself (.bss).
 * Boundary: the vector module after it starts at 0x8048222C with its own constant pool.
 * C, cflags_rw_core (configure.py). */
#include "Game/rw/r6.h"

RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        RwFreeList *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
void *fn_8047FDB0(RwFreeList *list, u32 hint);
RwFreeList *fn_804800DC(RwFreeList *list, void (*callback)(void *entry, void *data), void *data);
void *fn_80485D64(void);

static RwFreeList *pluginEntryFreeList;
static u32 numPluginRegistries;
static RwPluginRegistry **pluginRegistries;
static s32 pluginEntryFreeListBlockSize = 0x40;
static s32 pluginEntryFreeListPreallocBlocks = 1;
static RwFreeList pluginEntryFreeListSpace;

/* _rwPluginRegistryOpen. */
RwBool fn_80481B98(void) {
    pluginEntryFreeList = fn_8047FC04(sizeof(RwPluginRegistryEntry), pluginEntryFreeListBlockSize, 4, pluginEntryFreeListPreallocBlocks,
                               &pluginEntryFreeListSpace, 0x40000);
    if (!pluginEntryFreeList) {
        return FALSE;
    }
    numPluginRegistries = 0;
    return TRUE;
}

/* Free-list walker of the registry close: resets the entry's registry and frees the entry. */
void fn_80481BF4(void *object, void *data) {
    RwPluginRegistryEntry *entry = (RwPluginRegistryEntry *)object;

    if (entry->parentRegistry->firstRegEntry) {
        entry->parentRegistry->sizeOfStruct = entry->parentRegistry->origSizeOfStruct;
        entry->parentRegistry->firstRegEntry = NULL;
        entry->parentRegistry->lastRegEntry = NULL;
    }
    RwEngineInstance->memoryFree((RwFreeList *)data, entry);
}

/* _rwPluginRegistryClose. */
RwBool fn_80481C58(void) {
    if (pluginEntryFreeList) {
        fn_804800DC(pluginEntryFreeList, fn_80481BF4, pluginEntryFreeList);
        if (RwEngineInstance->memoryAlloc != fn_8047FDB0) {
            u32 i;
            for (i = 0; i < numPluginRegistries; i++) {
                RwPluginRegistryEntry *entry = pluginRegistries[i]->firstRegEntry;
                RwPluginRegistry *reg = NULL;
                if (entry) {
                    reg = entry->parentRegistry;
                }
                while (entry) {
                    RwPluginRegistryEntry *next = entry->nextRegEntry;
                    RwEngineInstance->memoryFree(NULL, entry);
                    entry = next;
                }
                if (reg && reg->firstRegEntry) {
                    reg->sizeOfStruct = reg->origSizeOfStruct;
                    reg->firstRegEntry = NULL;
                    reg->lastRegEntry = NULL;
                }
            }
            if (pluginRegistries) {
                RwEngineInstance->memoryFuncs.rwfree(pluginRegistries);
                pluginRegistries = NULL;
            }
        }
        fn_8047FCDC(pluginEntryFreeList);
        pluginEntryFreeList = NULL;
    }
    return TRUE;
}

/* Default constructor, destructor and copy callbacks of a plugin: nothing to do. */
void *fn_80481D8C(void *object, s32 offset, s32 size) {
    return object;
}

void *fn_80481D90(void *object, s32 offset, s32 size) {
    return object;
}

void *fn_80481D94(void *dst, const void *src, s32 offset, s32 size) {
    return dst;
}

/* _rwPluginRegistryGetPluginOffset. */
s32 fn_80481D98(const RwPluginRegistry *reg, u32 pluginID) {
    RwPluginRegistryEntry *entry = reg->firstRegEntry;
    while (entry) {
        if (entry->pluginID == pluginID) {
            return entry->offset;
        }
        entry = entry->nextRegEntry;
    }
    return -1;
}

/* _rwPluginRegistryAddPlugin. */
s32 fn_80481DC8(RwPluginRegistry *reg, s32 size, u32 pluginID, RwPluginObjectConstructor constructCB,
                RwPluginObjectDestructor destructCB, RwPluginObjectCopy copyCB) {
    RwPluginRegistryEntry *entry;
    s32 newSize;

    if (!pluginEntryFreeList) {
        return -1;
    }
    if (fn_80485D64()) {
        RWERROR((0x80000017));
        return -1;
    }
    if (RwEngineInstance->memoryAlloc != fn_8047FDB0) {
        u32 i;
        for (i = 0; i < numPluginRegistries; i++) {
            if (reg == pluginRegistries[i]) {
                break;
            }
        }
        if (numPluginRegistries == i) {
            RwPluginRegistry **list = (RwPluginRegistry **)RwEngineInstance->memoryFuncs.rwmalloc(
                (numPluginRegistries + 1) * sizeof(RwPluginRegistry *), 0x40000);
            u32 j = 0;
            if (pluginRegistries) {
                for (; j < numPluginRegistries; j++) {
                    list[j] = pluginRegistries[j];
                }
                RwEngineInstance->memoryFuncs.rwfree(pluginRegistries);
                pluginRegistries = NULL;
            }
            list[j] = reg;
            pluginRegistries = list;
            numPluginRegistries++;
        }
    }
    entry = reg->firstRegEntry;
    while (entry) {
        if (entry->pluginID == pluginID) {
            RWERROR((0x80000017));
            return entry->offset;
        }
        entry = entry->nextRegEntry;
    }
    newSize = reg->sizeOfStruct + ((size + 3) & ~3);
    if (reg->maxSizeOfStruct && newSize > reg->maxSizeOfStruct) {
        return -1;
    }
    entry = (RwPluginRegistryEntry *)RwEngineInstance->memoryAlloc(pluginEntryFreeList, 0x40000);
    if (entry) {
        entry->offset = reg->sizeOfStruct;
        reg->sizeOfStruct = newSize;
        entry->size = size;
        entry->pluginID = pluginID;
        entry->readCB = NULL;
        entry->writeCB = NULL;
        entry->getSizeCB = NULL;
        entry->alwaysCB = NULL;
        entry->rightsCB = NULL;
        entry->constructCB = constructCB ? constructCB : fn_80481D8C;
        entry->destructCB = destructCB ? destructCB : fn_80481D90;
        entry->copyCB = copyCB ? copyCB : fn_80481D94;
        entry->errStrCB = NULL;
        entry->nextRegEntry = NULL;
        entry->prevRegEntry = NULL;
        entry->parentRegistry = reg;
        if (!reg->firstRegEntry) {
            reg->firstRegEntry = entry;
            reg->lastRegEntry = entry;
        } else {
            reg->lastRegEntry->nextRegEntry = entry;
            entry->prevRegEntry = reg->lastRegEntry;
            reg->lastRegEntry = entry;
        }
        return entry->offset;
    }
    return -1;
}

/* _rwPluginRegistryInitObject: a constructor that fails unwinds the ones before it. */
const RwPluginRegistry *fn_8048209C(const RwPluginRegistry *reg, void *object) {
    RwPluginRegistryEntry *entry = reg->firstRegEntry;

    while (entry) {
        if (!entry->constructCB(object, entry->offset, entry->size)) {
            entry = entry->prevRegEntry;
            while (entry) {
                entry->destructCB(object, entry->offset, entry->size);
                entry = entry->prevRegEntry;
            }
            return NULL;
        }
        entry = entry->nextRegEntry;
    }
    return reg;
}

/* _rwPluginRegistryDeInitObject. */
const RwPluginRegistry *fn_80482144(const RwPluginRegistry *reg, void *object) {
    RwPluginRegistryEntry *entry = reg->lastRegEntry;

    while (entry) {
        entry->destructCB(object, entry->offset, entry->size);
        entry = entry->prevRegEntry;
    }
    return reg;
}

/* _rwPluginRegistryCopyObject. */
const RwPluginRegistry *fn_804821B0(const RwPluginRegistry *reg, void *dst, const void *src) {
    RwPluginRegistryEntry *entry = reg->firstRegEntry;

    while (entry) {
        entry->copyCB(dst, src, entry->offset, entry->size);
        entry = entry->nextRegEntry;
    }
    return reg;
}

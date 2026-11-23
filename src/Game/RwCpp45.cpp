/* RenderWare core, 0x80480C6C..0x804823EF (a size cut, not a source file):
 *   (the resource arena's tail, 0x80480878..0x80480C6C, moved to src/RenderWare/RwResources.c in
 *   wave 41: that source file starts in range r5)
 *   0x80480C6C  streams: module open/close, RwStreamRead/Write/Skip/Close/Open
 *   0x804815F0  plugin registry: stream callbacks, chunk read/write/skip, open/close, add plugin,
 *               object init/deinit/copy
 *   0x8048222C  vector helpers: normalize, length, table-driven sqrt and inverse sqrt
 * Functions keep their fn_ names because game code calls them by those; each comment gives the
 * public RenderWare function the behaviour corresponds to. */
#pragma cplusplus off
#include <dolphin/types.h>
#include "Game/rw/r6.h"

extern RwModuleInfo lbl_805F1578; /* resources module */
extern RwModuleInfo lbl_805F1580; /* stream module */
extern RwPluginRegistry **lbl_805F1588; /* registries seen by the add-plugin call */
extern u32 lbl_805F158C;                /* their count */
extern RwFreeList *lbl_805F1590;        /* registry entry free list */
extern RwModuleInfo lbl_805F1598;       /* vector module: sqrt tables */
extern s32 lbl_805EDEA8;
extern s32 lbl_805EDEAC;
extern char lbl_805EDEB0[4]; /* "rb" */
extern char lbl_805EDEB4[4]; /* "wb" */
extern char lbl_805EDEB8[4]; /* "ab" */
extern s32 lbl_805EDEC0;
extern s32 lbl_805EDEC4;
extern u8 lbl_805E2AA8[];
extern u8 lbl_805E2AD0[];
extern const f32 lbl_805FB2B4;

void *fn_80482A94(void *heap, u32 size);
void fn_804829DC(void *mem);
RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        void *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
void *fn_8047FDB0(RwFreeList *list, u32 hint);
RwFreeList *fn_804800DC(RwFreeList *list, void (*callback)(void *entry, void *data), void *data);
void *fn_8047A178(const char *name, const char *mode);
s32 fn_8047A388(void *file);
u32 fn_8047A3E0(void *buffer, u32 size, u32 count, void *file);
u32 fn_8047A428(const void *buffer, u32 size, u32 count, void *file);
s32 fn_8047A470(void *file, s32 offset, s32 origin);
s32 fn_8047A650(void *file);
s32 fn_8047A7C0(void *file);
RwBool fn_8047CDB4(RwStream *stream, u32 *type, u32 *length, u32 *version, u32 *build);
RwStream *fn_8047CEF0(RwStream *stream, u32 type, s32 size, u32 version, u32 build);
RwBool fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
void *fn_80485D64(void);
f32 fn_80482340(f32 value);
f32 fn_80482394(f32 value);
RwStream *fn_804810FC(RwStream *stream, u32 offset);

#define RWSTREAMGLOBAL (RWGLOBALAT(RwFreeList *, lbl_805F1580.globalsOffset))

/* _rwStreamModuleOpen. */
void *fn_80480C6C(void *instance, s32 offset) {
    lbl_805F1580.globalsOffset = offset;
    RWSTREAMGLOBAL = fn_8047FC04(sizeof(RwStream), lbl_805EDEA8, 4, lbl_805EDEAC, lbl_805E2AA8,
                                 0x40404);
    if (!RWSTREAMGLOBAL) {
        return NULL;
    }
    lbl_805F1580.numInstances++;
    return instance;
}

/* _rwStreamModuleClose. */
void *fn_80480CF8(void *instance) {
    if (RWSTREAMGLOBAL) {
        fn_8047FCDC(RWSTREAMGLOBAL);
    }
    lbl_805F1580.numInstances--;
    return instance;
}

/* RwStreamRead. */
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length) {
    switch (stream->type) {
    case 1:
    case 2: {
        void *file = stream->Type.fpFile;
        u32 bytesRead = fn_8047A3E0(buffer, 1, length, file);
        if (bytesRead != length) {
            if (fn_8047A650(file)) {
                RWERROR((5));
            } else {
                RWERROR((0x8000001A));
            }
        }
        return bytesRead;
    }
    case 3: {
        RwStreamMemory *memory = &stream->Type.memory;
        if (length > memory->nSize - memory->position) {
            length = memory->nSize - memory->position;
            RWERROR((5));
        }
        memcpy(buffer, memory->memBlock + memory->position, length);
        memory->position += length;
        return length;
    }
    case 4:
        return stream->Type.custom.sfnread(stream->Type.custom.data, buffer, length);
    default:
        RWERROR((0xE));
        return 0;
    }
}

/* RwStreamWrite. */
RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length) {
    switch (stream->type) {
    case 1:
    case 2: {
        u32 written = fn_8047A428(buffer, 1, length, stream->Type.fpFile);
        if (written != length) {
            RWERROR((0x8000001C));
            return NULL;
        }
        return stream;
    }
    case 3: {
        RwStreamMemory *memory = &stream->Type.memory;
        if (!memory->memBlock) {
            memory->memBlock = (u8 *)RwEngineInstance->memoryFuncs.rwmalloc(0x200, 0x30404);
            if (!memory->memBlock) {
                RWERROR((0x80000013, 0x200));
                return NULL;
            }
            memory->nSize = 0x200;
        }
        if (memory->nSize - memory->position < length) {
            u32 newSize = length + memory->nSize;
            u8 *newBlock;
            if (length < 0x200) {
                newSize = memory->nSize + 0x200;
            }
            newBlock =
                (u8 *)RwEngineInstance->memoryFuncs.rwrealloc(memory->memBlock, newSize, 0x1030404);
            if (!newBlock) {
                RWERROR((0x80000013, newSize - memory->nSize));
                return NULL;
            }
            memory->memBlock = newBlock;
            memory->nSize = newSize;
        }
        memcpy(memory->memBlock + memory->position, buffer, length);
        memory->position += length;
        return stream;
    }
    case 4:
        if (stream->Type.custom.sfnwrite(stream->Type.custom.data, buffer, length)) {
            return stream;
        }
        return NULL;
    default:
        RWERROR((0xE));
        return NULL;
    }
}

/* RwStreamSkip. */
RwStream *fn_804810FC(RwStream *stream, u32 offset) {
    if (!offset) {
        return stream;
    }
    switch (stream->type) {
    case 1:
    case 2: {
        void *file = stream->Type.fpFile;
        if (fn_8047A470(file, offset, 1)) {
            if (fn_8047A650(file)) {
                RWERROR((5));
            }
            return NULL;
        }
        return stream;
    }
    case 3: {
        RwStreamMemory *memory = &stream->Type.memory;
        u32 newPosition = memory->position + offset;
        if (newPosition > memory->nSize) {
            memory->position = memory->nSize;
            RWERROR((5));
            return NULL;
        }
        memory->position = newPosition;
        return stream;
    }
    case 4:
        if (stream->Type.custom.sfnskip(stream->Type.custom.data, offset)) {
            return stream;
        }
        return NULL;
    default:
        RWERROR((0xE));
        return NULL;
    }
}

/* RwStreamClose. */
RwBool fn_8048124C(RwStream *stream, RwMemory *memory) {
    RwBool result;

    switch (stream->type) {
    case 1:
        result = TRUE;
        break;
    case 2:
        result = !fn_8047A388(stream->Type.fpFile);
        break;
    case 3:
        if (stream->accessType != 1 && memory) {
            memory->start = stream->Type.memory.memBlock;
            memory->length = stream->Type.memory.position;
        }
        result = TRUE;
        break;
    case 4:
        if (stream->Type.custom.sfnclose) {
            stream->Type.custom.sfnclose(stream->Type.custom.data);
        }
        result = TRUE;
        break;
    default:
        RWERROR((0xE));
        return FALSE;
    }
    if (stream->rwOwned) {
        RwEngineInstance->memoryFree(RWSTREAMGLOBAL, stream);
    }
    return result;
}

static inline RwStream *StreamOpenMemory(RwStream *stream, s32 accessType,
                                         const RwMemory *memory) {
    RwStream *opened = NULL;

    switch (accessType) {
    case 1:
        stream->Type.memory.position = 0;
        opened = stream;
        stream->Type.memory.nSize = memory->length;
        stream->Type.memory.memBlock = memory->start;
        break;
    case 2:
        stream->Type.memory.position = 0;
        opened = stream;
        stream->Type.memory.nSize = 0;
        stream->Type.memory.memBlock = NULL;
        break;
    case 3:
        opened = stream;
        stream->Type.memory.position = memory->length;
        stream->Type.memory.nSize = memory->length;
        stream->Type.memory.memBlock = memory->start;
        break;
    default:
        RWERROR((0xD));
        break;
    }
    return opened;
}

static inline void *StreamOpenFile(const char *name, s32 accessType) {
    void *file = NULL;

    switch (accessType) {
    case 1:
        file = fn_8047A178(name, lbl_805EDEB0);
        break;
    case 2:
        file = fn_8047A178(name, lbl_805EDEB4);
        break;
    case 3:
        file = fn_8047A178(name, lbl_805EDEB8);
        break;
    default:
        RWERROR((0xD));
        break;
    }
    return file;
}

/* _rwStreamInitialize, which retail only has inlined into RwStreamOpen. */
static inline RwStream *StreamInitialize(RwStream *stream, RwBool rwOwned, s32 type,
                                         s32 accessType, const void *data) {
    RwStream *result = NULL;

    if (!stream) {
        return result;
    }
    stream->type = type;
    stream->accessType = accessType;
    stream->rwOwned = rwOwned;
    switch (type) {
    case 1:
        if (fn_8047A7C0((void *)data) == -1) {
            result = NULL;
        } else {
            stream->Type.fpFile = (void *)data;
            result = stream;
        }
        break;
    case 2: {
        void *file = StreamOpenFile((const char *)data, accessType);
        if (file) {
            stream->Type.fpFile = file;
            result = stream;
        } else {
            RWERROR((0x80000002, data));
        }
        break;
    }
    case 3:
        result = StreamOpenMemory(stream, accessType, (const RwMemory *)data);
        break;
    case 4:
        memcpy(&stream->Type.custom, data, sizeof(RwStreamCustom));
        result = stream;
        break;
    default:
        RWERROR((0xE));
        break;
    }
    return result;
}

/* RwStreamOpen. */
RwStream *fn_80481368(s32 type, s32 accessType, const void *data) {
    RwStream *stream = (RwStream *)RwEngineInstance->memoryAlloc(RWSTREAMGLOBAL, 0x30404);

    if (!StreamInitialize(stream, TRUE, type, accessType, data)) {
        RwEngineInstance->memoryFree(RWSTREAMGLOBAL, stream);
        stream = NULL;
    }
    return stream;
}

static inline RwPluginRegistryEntry *FindEntry(const RwPluginRegistry *reg, u32 pluginID) {
    RwPluginRegistryEntry *entry = reg->firstRegEntry;
    while (entry) {
        if (entry->pluginID == pluginID) {
            break;
        }
        entry = entry->nextRegEntry;
    }
    return entry;
}

/* _rwPluginRegistryAddPluginStream. */
s32 fn_804815F0(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB) {
    RwPluginRegistryEntry *entry = FindEntry(reg, pluginID);
    if (entry) {
        entry->readCB = readCB;
        entry->writeCB = writeCB;
        entry->getSizeCB = getSizeCB;
        return entry->offset;
    }
    return -1;
}

/* _rwPluginRegistryAddPlgnStrmlwysCB. */
s32 fn_80481634(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkAlwaysCallBack alwaysCB) {
    RwPluginRegistryEntry *entry = FindEntry(reg, pluginID);
    if (entry) {
        entry->alwaysCB = alwaysCB;
        return entry->offset;
    }
    return -1;
}

/* _rwPluginRegistryAddPlgnStrmRightsCB. */
s32 fn_80481670(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkRightsCallBack rightsCB) {
    RwPluginRegistryEntry *entry = FindEntry(reg, pluginID);
    if (entry) {
        entry->rightsCB = rightsCB;
        return entry->offset;
    }
    return -1;
}

/* _rwPluginRegistryReadDataChunks. */
const RwPluginRegistry *fn_804816AC(const RwPluginRegistry *reg, RwStream *stream, void *object) {
    u32 length;
    u32 version;

    if (!fn_8047CF50(stream, 3, &length, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        RwPluginRegistryEntry *entry;
        while (length > 0) {
            u32 type;
            u32 chunkLength;
            if (!fn_8047CDB4(stream, &type, &chunkLength, NULL, NULL)) {
                return NULL;
            }
            entry = reg->firstRegEntry;
            while (entry) {
                if (entry->pluginID == type) {
                    break;
                }
                entry = entry->nextRegEntry;
            }
            if (entry && entry->readCB) {
                if (!entry->readCB(stream, chunkLength, object, entry->offset, entry->size)) {
                    return NULL;
                }
            } else {
                if (!fn_804810FC(stream, chunkLength)) {
                    return NULL;
                }
            }
            length -= chunkLength + 12;
        }
        entry = reg->firstRegEntry;
        while (entry) {
            if (entry->alwaysCB) {
                if (!entry->alwaysCB(object, entry->offset, entry->size)) {
                    return NULL;
                }
            }
            entry = entry->nextRegEntry;
        }
        return reg;
    }
    RWERROR((0x80000004));
    return NULL;
}

/* _rwPluginRegistryInvokeRights. */
const RwPluginRegistry *fn_80481870(const RwPluginRegistry *reg, u32 pluginID, void *object,
                                    u32 extra) {
    RwPluginRegistryEntry *entry = FindEntry(reg, pluginID);
    if (entry && entry->rightsCB) {
        if (entry->rightsCB(object, entry->offset, entry->size, extra)) {
            return reg;
        }
        return NULL;
    }
    return NULL;
}

static inline s32 RegistryGetSize(const RwPluginRegistry *reg, const void *object) {
    s32 total = 0;
    RwPluginRegistryEntry *entry = reg->firstRegEntry;

    while (entry) {
        if (entry->getSizeCB) {
            s32 size = entry->getSizeCB(object, entry->offset, entry->size);
            if (size > 0) {
                total = size + total;
                total += 12;
            }
        }
        entry = entry->nextRegEntry;
    }
    return total;
}

/* _rwPluginRegistryGetSize. */
s32 fn_804818FC(const RwPluginRegistry *reg, const void *object) {
    s32 total = 0;
    RwPluginRegistryEntry *entry = reg->firstRegEntry;

    while (entry) {
        if (entry->getSizeCB) {
            s32 size = entry->getSizeCB(object, entry->offset, entry->size);
            if (size > 0) {
                total = size + total;
                total += 12;
            }
        }
        entry = entry->nextRegEntry;
    }
    return total;
}

/* _rwPluginRegistryWriteDataChunks. */
const RwPluginRegistry *fn_80481980(const RwPluginRegistry *reg, RwStream *stream,
                                    const void *object) {
    RwPluginRegistryEntry *entry;
    s32 total = RegistryGetSize(reg, object);

    if (!fn_8047CEF0(stream, 3, total, 0x37002, 0x37)) {
        return NULL;
    }
    entry = reg->firstRegEntry;
    while (entry) {
        if (entry->getSizeCB && entry->writeCB) {
            s32 size = entry->getSizeCB(object, entry->offset, entry->size);
            if (size > 0) {
                if (!fn_8047CEF0(stream, entry->pluginID, size, 0x37002, 0x37)) {
                    return NULL;
                }
                if (!entry->writeCB(stream, size, object, entry->offset, entry->size)) {
                    return NULL;
                }
            }
        }
        entry = entry->nextRegEntry;
    }
    return reg;
}

/* _rwPluginRegistrySkipDataChunks. */
const RwPluginRegistry *fn_80481AD4(const RwPluginRegistry *reg, RwStream *stream) {
    u32 length;

    if (!fn_8047CF50(stream, 3, &length, NULL)) {
        return NULL;
    }
    while (length > 0) {
        u32 chunkLength;
        if (!fn_8047CDB4(stream, NULL, &chunkLength, NULL, NULL)) {
            return NULL;
        }
        if (!fn_804810FC(stream, chunkLength)) {
            return NULL;
        }
        length -= chunkLength + 12;
    }
    return reg;
}

/* _rwPluginRegistryOpen. */
RwBool fn_80481B98(void) {
    lbl_805F1590 = fn_8047FC04(sizeof(RwPluginRegistryEntry), lbl_805EDEC0, 4, lbl_805EDEC4,
                               lbl_805E2AD0, 0x40000);
    if (!lbl_805F1590) {
        return FALSE;
    }
    lbl_805F158C = 0;
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
    if (lbl_805F1590) {
        fn_804800DC(lbl_805F1590, fn_80481BF4, lbl_805F1590);
        if (RwEngineInstance->memoryAlloc != fn_8047FDB0) {
            u32 i;
            for (i = 0; i < lbl_805F158C; i++) {
                RwPluginRegistryEntry *entry = lbl_805F1588[i]->firstRegEntry;
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
            if (lbl_805F1588) {
                RwEngineInstance->memoryFuncs.rwfree(lbl_805F1588);
                lbl_805F1588 = NULL;
            }
        }
        fn_8047FCDC(lbl_805F1590);
        lbl_805F1590 = NULL;
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

    if (!lbl_805F1590) {
        return -1;
    }
    if (fn_80485D64()) {
        RWERROR((0x80000017));
        return -1;
    }
    if (RwEngineInstance->memoryAlloc != fn_8047FDB0) {
        u32 i;
        for (i = 0; i < lbl_805F158C; i++) {
            if (reg == lbl_805F1588[i]) {
                break;
            }
        }
        if (lbl_805F158C == i) {
            RwPluginRegistry **list = (RwPluginRegistry **)RwEngineInstance->memoryFuncs.rwmalloc(
                (lbl_805F158C + 1) * sizeof(RwPluginRegistry *), 0x40000);
            u32 j = 0;
            if (lbl_805F1588) {
                for (; j < lbl_805F158C; j++) {
                    list[j] = lbl_805F1588[j];
                }
                RwEngineInstance->memoryFuncs.rwfree(lbl_805F1588);
                lbl_805F1588 = NULL;
            }
            list[j] = reg;
            lbl_805F1588 = list;
            lbl_805F158C++;
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
    entry = (RwPluginRegistryEntry *)RwEngineInstance->memoryAlloc(lbl_805F1590, 0x40000);
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

/* RwV3dNormalize. */
f32 fn_8048222C(RwV3d *out, const RwV3d *in) {
    f32 length2 = in->x * in->x + in->y * in->y + in->z * in->z;
    f32 length = fn_80482340(length2);
    f32 recip = fn_80482394(length2);

    out->x = in->x * recip;
    out->y = in->y * recip;
    out->z = in->z * recip;
    if (length <= lbl_805FB2B4) {
        RWERROR((0x19));
    }
    return length;
}

/* RwV3dLength. */
f32 fn_80482300(const RwV3d *in) {
    return fn_80482340(in->x * in->x + in->y * in->y + in->z * in->z);
}


/* _rwSqrt: mantissa table lookup. */
f32 fn_80482340(f32 value) {
    volatile RwSplitBits bits;

    bits.nReal = value;
    if (bits.nUInt != 0) {
        bits.nUInt += 0x800;
        bits.nUInt = ((bits.nUInt >> 1) & 0x3FC00000) +
                     RWGLOBALAT(RwSqrtTables, lbl_805F1598.globalsOffset)
                         .sqrtTab[(bits.nUInt >> 12) & 0xFFF];
    }
    return bits.nReal;
}

/* _rwInvSqrt. */
f32 fn_80482394(f32 value) {
    volatile RwSplitBits bits;

    bits.nReal = value;
    if (bits.nUInt != 0) {
        bits.nUInt += 0x800;
        bits.nUInt = ((~bits.nUInt >> 1) & 0x3FC00000) +
                     RWGLOBALAT(RwSqrtTables, lbl_805F1598.globalsOffset)
                         .invSqrtTab[(bits.nUInt >> 12) & 0xFFF];
    }
    return bits.nReal;
}

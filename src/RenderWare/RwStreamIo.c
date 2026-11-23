/* RenderWare core: the stream module (0x80480C6C-0x804815F0).
 * Module open and close (the stream free list), RwStreamRead, RwStreamWrite, RwStreamSkip,
 * RwStreamClose and RwStreamOpen. The file owns the module globals (.sbss), the free list
 * parameters and the fopen modes (.sdata) and the free list itself (.bss).
 * Boundary: the resource arena file before it ends at 0x80480C6C (its .sbss lbl_805F1578 is padded
 * to 8); the plugin data chunk functions after it are another file: compiled together with
 * automatic inlining they would inline RwStreamSkip, which retail calls out of line.
 * Functions keep their fn_ names because game code calls them by those; each comment gives the
 * public RenderWare function the behaviour corresponds to. C, cflags_rw_core (configure.py).
 * NonMatching: fn_80481368 (RwStreamOpen), 26 rows: the inlined initialiser's result and the
 * stream type trade r28/r29 (seven shapes of the initialiser tried, inline and out of line). */
#include "Game/rw/r6.h"

RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        RwFreeList *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
void *fn_8047A178(const char *name, const char *mode);
s32 fn_8047A388(void *file);
u32 fn_8047A3E0(void *buffer, u32 size, u32 count, void *file);
u32 fn_8047A428(const void *buffer, u32 size, u32 count, void *file);
s32 fn_8047A470(void *file, s32 offset, s32 origin);
s32 fn_8047A650(void *file);
s32 fn_8047A7C0(void *file);

static RwModuleInfo streamModule;
static s32 streamFreeListBlockSize = 0x10;
static s32 streamFreeListPreallocBlocks = 1;
static RwFreeList streamFreeList;

#define RWSTREAMGLOBAL (RWGLOBALAT(RwFreeList *, streamModule.globalsOffset))

/* _rwStreamModuleOpen. */
void *fn_80480C6C(void *instance, s32 offset) {
    streamModule.globalsOffset = offset;
    RWSTREAMGLOBAL = fn_8047FC04(sizeof(RwStream), streamFreeListBlockSize, 4,
                                 streamFreeListPreallocBlocks, &streamFreeList, 0x40404);
    if (!RWSTREAMGLOBAL) {
        return NULL;
    }
    streamModule.numInstances++;
    return instance;
}

/* _rwStreamModuleClose. */
void *fn_80480CF8(void *instance) {
    if (RWSTREAMGLOBAL) {
        fn_8047FCDC(RWSTREAMGLOBAL);
    }
    streamModule.numInstances--;
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

static inline RwStream *StreamOpenMemory(RwStream *stream, s32 accessType, const RwMemory *memory) {
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
        file = fn_8047A178(name, "rb");
        break;
    case 2:
        file = fn_8047A178(name, "wb");
        break;
    case 3:
        file = fn_8047A178(name, "ab");
        break;
    default:
        RWERROR((0xD));
        break;
    }
    return file;
}

/* _rwStreamInitialize, which retail only has inlined into RwStreamOpen. */
static inline RwStream *StreamInitialize(RwStream *stream, RwBool rwOwned, s32 type, s32 accessType,
                                  const void *data) {
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

/* RenderWare, RwCpp43 (0x8047D2C4-0x8047ECD4): the end of the stream module that RwCpp42 starts
 * (16-bit byte swaps, the typed writes and reads with their byte-order conversions inlined, and
 * RwStreamReadChunkHeaderInfo), the error module (RwErrorSet, RwErrorGet, _rwerror and the
 * modules' open and close counters), rwSList (growable arrays), then the matrix module
 * (fn_8047E100 on). Written from the retail code; the fn_ symbols are kept because game code calls
 * them by address. Built as C: -O4,p, no stmw/lmw, automatic inlining in definition order. */
#include "Game/rw/r5.h"

#ifdef __cplusplus
extern "C" {
#endif

void *memcpy(void *dst, const void *src, u32 n);
RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length); /* RwStreamWrite */

extern R5ModuleInfo lbl_805F1550;
extern R5ModuleInfo lbl_805F1558; /* the error module's */

/* The stream module's byte-order conversions (defined in RwCpp42, which is the same source file;
 * retail inlines them here). */
static void *MemLittleEndian32(void *mem, u32 size) {
    u32 *p = (u32 *)mem;
    u32 n;

    for (n = size >> 2; n != 0; n--) {
        __stwbrx(*p, p, 0);
        p++;
    }
    return mem;
}

static void *MemNative32(void *mem, u32 size) {
    u32 *p = (u32 *)mem;
    u32 n;

    for (n = size >> 2; n != 0; n--) {
        __stwbrx(*p, p, 0);
        p++;
    }
    return mem;
}

/* RwMemNative16. */
void *fn_8047D2C4(void *mem, u32 size) {
    u32 n;
    u16 *p = (u16 *)mem;

    for (n = size >> 1; n != 0; n--) {
        *p = (*p >> 8) | (*p << 8);
        p++;
    }
    return mem;
}

/* RwStreamWriteReal: through a 256-byte buffer, converted to the stream's byte order. */
RwStream *fn_8047D350(RwStream *stream, const f32 *reals, u32 numBytes) {
    u8 buffer[0x100];

    while (numBytes != 0) {
        u32 size = 0x100;

        if (numBytes < 0x100) {
            size = numBytes;
        }
        memcpy(buffer, reals, size);
        MemLittleEndian32(buffer, size);
        if (fn_80480ED0(stream, buffer, size) == 0) {
            return 0;
        }
        numBytes -= size;
        reals = (const f32 *)((const u8 *)reals + size);
    }
    return stream;
}

/* RwStreamWriteInt32. */
RwStream *fn_8047D458(RwStream *stream, const s32 *ints, u32 numBytes) {
    u8 buffer[0x100];

    while (numBytes != 0) {
        u32 size = 0x100;

        if (numBytes < 0x100) {
            size = numBytes;
        }
        memcpy(buffer, ints, size);
        MemLittleEndian32(buffer, size);
        if (fn_80480ED0(stream, buffer, size) == 0) {
            return 0;
        }
        numBytes -= size;
        ints = (const s32 *)((const u8 *)ints + size);
    }
    return stream;
}

/* RwStreamWriteInt16. */
RwStream *fn_8047D560(RwStream *stream, const s16 *ints, u32 numBytes) {
    u8 buffer[0x100];

    while (numBytes != 0) {
        u32 size = 0x100;

        if (numBytes < 0x100) {
            size = numBytes;
        }
        memcpy(buffer, ints, size);
        fn_8047D2C4(buffer, size);
        if (fn_80480ED0(stream, buffer, size) == 0) {
            return 0;
        }
        numBytes -= size;
        ints = (const s16 *)((const u8 *)ints + size);
    }
    return stream;
}

/* RwStreamReadInt16. */
RwStream *fn_8047D684(RwStream *stream, s16 *ints, u32 numBytes) {
    if (fn_80480D4C(stream, ints, numBytes) == 0) {
        R5ERROR(1, (0x8000001A));
        return 0;
    }
    fn_8047D2C4(ints, numBytes);
    return stream;
}

/* RwStreamReadReal. */
RwStream *fn_8047D784(RwStream *stream, f32 *reals, u32 numBytes) {
    if (fn_80480D4C(stream, reals, numBytes) == 0) {
        R5ERROR(1, (0x8000001A));
        return 0;
    }
    MemNative32(reals, numBytes);
    return stream;
}

/* RwStreamReadInt32. */
RwStream *fn_8047D868(RwStream *stream, s32 *ints, u32 numBytes) {
    if (fn_80480D4C(stream, ints, numBytes) == 0) {
        R5ERROR(1, (0x8000001A));
        return 0;
    }
    MemNative32(ints, numBytes);
    return stream;
}

RwBool fn_8047CCF8(const R5ChunkHeaderInfo *info);

/* _rwStreamReadChunkHeader (RwCpp42's fn_8047CDB4, inlined here). */
static RwBool ReadChunkHeader(RwStream *stream, u32 *type, u32 *length, u32 *version, u32 *buildNum) {
    R5ChunkHeader header;
    R5ChunkHeaderInfo info;
    RwBool ok = fn_80480D4C(stream, &header, sizeof(header)) == sizeof(header);

    if (!ok) {
        R5ERROR(1, (0x8000001A));
        return 0;
    }
    MemNative32(&header, sizeof(header));
    info.type = header.type;
    info.length = header.length;
    if (!(header.libraryID & 0xFFFF0000)) {
        info.version = header.libraryID << 8;
        info.buildNum = 0;
    } else {
        info.version = (((header.libraryID >> 14) & 0x3FF00) + 0x30000) | ((header.libraryID >> 16) & 0x3F);
        info.buildNum = header.libraryID & 0xFFFF;
    }
    info.isComplex = fn_8047CCF8(&info);
    if (type != 0) {
        *type = info.type;
    }
    if (length != 0) {
        *length = info.length;
    }
    if (buildNum != 0) {
        *buildNum = info.buildNum;
    }
    if (version != 0) {
        *version = info.version;
    }
    return 1;
}

/* RwStreamReadChunkHeaderInfo. */
RwStream *fn_8047D94C(RwStream *stream, R5ChunkHeaderInfo *headerInfo) {
    u32 type;
    u32 length;
    u32 build;
    u32 version;

    if (!ReadChunkHeader(stream, &type, &length, &version, &build)) {
        return 0;
    }
    headerInfo->type = type;
    headerInfo->length = length;
    headerInfo->version = version;
    headerInfo->buildNum = build;
    headerInfo->isComplex = fn_8047CCF8(headerInfo);
    return stream;
}

/* The error module ---------------------------------------------------------------------------- */

void *fn_8047DB10(void *instance) {
    lbl_805F1550.numInstances++;
    return instance;
}

void *fn_8047DB24(void *instance) {
    lbl_805F1550.numInstances--;
    return instance;
}

/* Opens the error module: no error yet. */
void *fn_8047DB38(void *instance, s32 offset) {
    lbl_805F1558.globalsOffset = offset;
    lbl_805F1558.numInstances++;
    ((RwError *)((u8 *)lbl_805F15B4 + offset))->pluginID = 0;
    ((RwError *)((u8 *)lbl_805F15B4 + lbl_805F1558.globalsOffset))->errorCode = 0x80000000;
    return instance;
}

/* Closes it. */
void *fn_8047DB70(void *instance) {
    lbl_805F1558.numInstances--;
    return instance;
}

#define R5ERRORGLOBAL ((RwError *)((u8 *)lbl_805F15B4 + lbl_805F1558.globalsOffset))

/* RwErrorSet: keeps the first error until it is read. Retail never inlines the error functions. */
#pragma auto_inline off
RwError *fn_8047DB84(RwError *code) {
    if (R5ERRORGLOBAL->pluginID == 0 && R5ERRORGLOBAL->errorCode == 0x80000000) {
        if (code->errorCode & 0x80000000) {
            R5ERRORGLOBAL->pluginID = 0;
        } else {
            R5ERRORGLOBAL->pluginID = code->pluginID;
        }
        R5ERRORGLOBAL->errorCode = code->errorCode;
    }
    return code;
}

/* RwErrorGet: the error, which is cleared. */
RwError *fn_8047DBE4(RwError *code) {
    *code = *R5ERRORGLOBAL;
    R5ERRORGLOBAL->pluginID = 0;
    R5ERRORGLOBAL->errorCode = 0x80000000;
    return code;
}

/* _rwerror: the error code (the arguments are only used by debug builds). */
s32 fn_8047DC28(s32 code, ...) {
    R5VaList ap;

    __builtin_va_info(&ap);
    return code;
}

#pragma auto_inline reset

/* rwSList ------------------------------------------------------------------------------------- */

struct RwSList {
    /* 0x00 */ u8 *entries;
    /* 0x04 */ s32 numElementsFilled;
    /* 0x08 */ s32 numElementsAlloced;
    /* 0x0C */ s32 entrySize;
};

/* rwSListCreate. */
RwSList *fn_8047DC90(s32 entrySize, u32 hint) {
    RwSList *list = (RwSList *)RWSRCGLOBAL(rwmalloc)(sizeof(RwSList), 0x30000);

    if (list == 0) {
        R5ERROR(1, (0x80000013, sizeof(RwSList)));
        return 0;
    }
    list->numElementsFilled = 0;
    list->numElementsAlloced = 20;
    list->entrySize = entrySize;
    list->entries = (u8 *)RWSRCGLOBAL(rwmalloc)(list->numElementsAlloced * list->entrySize, hint | 0x1000000);
    if (list->entries == 0) {
        R5ERROR(1, (0x80000013, list->numElementsAlloced * list->entrySize));
        RWSRCGLOBAL(rwfree)(list);
        return 0;
    }
    return list;
}

/* rwSListEmpty. */
RwSList *fn_8047DDB0(RwSList *list) {
    list->numElementsFilled = 0;
    return list;
}

/* rwSListGetNewEntry: grows the array by a quarter when it is full. */
void *fn_8047DDBC(RwSList *list, u32 hint) {
    if (list->numElementsFilled >= list->numElementsAlloced) {
        u8 *entries = (u8 *)RWSRCGLOBAL(rwrealloc)(
            list->entries, list->entrySize * (list->numElementsAlloced + list->numElementsAlloced / 4), hint);

        if (entries == 0) {
            R5ERROR(1, (0x80000013, list->entrySize * (list->numElementsAlloced + list->numElementsAlloced / 4)));
            return 0;
        }
        list->entries = entries;
        list->numElementsAlloced += list->numElementsAlloced / 4;
    }
    {
        u8 *entry = list->entries + list->numElementsFilled * list->entrySize;

        list->numElementsFilled++;
        return entry;
    }
}

/* rwSListGetNewEntries. */
void *fn_8047DEA0(RwSList *list, s32 count, u32 hint) {
    if (list->numElementsFilled + count >= list->numElementsAlloced) {
        u8 *entries = (u8 *)RWSRCGLOBAL(rwrealloc)(
            list->entries, list->entrySize * (list->numElementsAlloced + count + list->numElementsAlloced / 4), hint);

        if (entries == 0) {
            R5ERROR(1, (0x80000013,
                        list->entrySize * (list->numElementsAlloced + count + list->numElementsAlloced / 4)));
            return 0;
        }
        list->entries = entries;
        list->numElementsAlloced = list->numElementsAlloced + count + list->numElementsAlloced / 4;
    }
    {
        u8 *entry = list->entries + list->numElementsFilled * list->entrySize;

        list->numElementsFilled += count;
        return entry;
    }
}

/* rwSListDestroyEndEntries. */
RwSList *fn_8047DF9C(RwSList *list, s32 count) {
    list->numElementsFilled -= count;
    return list;
}

/* rwSListGetArray. */
void *fn_8047DFAC(RwSList *list) {
    if (list->numElementsFilled != 0) {
        return list->entries;
    }
    return 0;
}

/* rwSListDestroy. */
RwBool fn_8047DFC8(RwSList *list) {
    if (list->entries != 0 && list->numElementsAlloced != 0) {
        RWSRCGLOBAL(rwfree)(list->entries);
        list->entries = 0;
        list->numElementsAlloced = 0;
    }
    RWSRCGLOBAL(rwfree)(list);
    return 1;
}

/* rwSListGetNumEntries. */
s32 fn_8047E03C(RwSList *list) {
    return list->numElementsFilled;
}

/* rwSListToArray: frees the list and hands its entries over. */
void *fn_8047E044(RwSList *list) {
    void *array = 0;

    if (list->numElementsFilled != 0) {
        array = list->entries;
    } else if (list->entries != 0) {
        RWSRCGLOBAL(rwfree)(list->entries);
        list->entries = 0;
        array = 0;
    }
    RWSRCGLOBAL(rwfree)(list);
    return array;
}

/* rwSListGetEntry. */
void *fn_8047E0CC(RwSList *list, s32 index) {
    return list->entries + list->entrySize * index;
}

/* rwSListGetBegin. */
void *fn_8047E0E0(RwSList *list) {
    return list->entries;
}

/* rwSListGetEnd. */
void *fn_8047E0E8(RwSList *list) {
    return list->entries + list->entrySize * list->numElementsFilled;
}

/* The matrix module -------------------------------------------------------------------------- */

extern const f32 lbl_805FB2A4; /* 1.0f */
extern s32 lbl_805EDE98;       /* matrices per free list block */
extern s32 lbl_805EDE9C;       /* blocks to preallocate */
extern u8 lbl_805E2A58[0x28];  /* the matrix free list's own storage */
extern R5ModuleInfo lbl_805F1560; /* the matrix module's */
void *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks, void *inPlace,
                  u32 hint);   /* RwFreeListCreateAndPreallocateSpace */
RwBool fn_8047FCDC(void *freeList); /* RwFreeListDestroy */
RwMatrix *fn_8047E100(RwMatrix *dst, const RwMatrix *a, const RwMatrix *b);

#define R5MATRIXGLOBAL ((R5MatrixGlobals *)((u8 *)lbl_805F15B4 + lbl_805F1560.globalsOffset))

/* The determinant of the rotation part. */
f32 fn_8047E704(const RwMatrix *matrix) {
    RwV3d cross;

    cross.x = matrix->up.y * matrix->at.z - matrix->up.z * matrix->at.y;
    cross.y = matrix->up.z * matrix->at.x - matrix->up.x * matrix->at.z;
    cross.z = matrix->up.x * matrix->at.y - matrix->up.y * matrix->at.x;
    return cross.x * matrix->right.x + cross.y * matrix->right.y + cross.z * matrix->right.z;
}

/* How far the axes are from orthogonal: the sum of their squared dot products. */
f32 fn_8047E750(const RwMatrix *matrix) {
    f32 upAt = matrix->up.x * matrix->at.x + matrix->up.y * matrix->at.y + matrix->up.z * matrix->at.z;
    f32 atRight = matrix->at.x * matrix->right.x + matrix->at.y * matrix->right.y + matrix->at.z * matrix->right.z;
    f32 rightUp = matrix->right.x * matrix->up.x + matrix->right.y * matrix->up.y + matrix->right.z * matrix->up.z;

    return upAt * upAt + atRight * atRight + rightUp * rightUp;
}

/* How far the axes are from unit length. */
f32 fn_8047E7A8(const RwMatrix *matrix) {
    f32 rightError;
    RwV3d at;
    RwV3d right;
    RwV3d up;
    f32 atError;
    f32 upError;

    up.x = matrix->up.x * matrix->up.x;
    up.y = matrix->up.y * matrix->up.y;
    right.x = matrix->right.x * matrix->right.x;
    right.y = matrix->right.y * matrix->right.y;
    up.z = matrix->up.z * matrix->up.z;
    right.z = matrix->right.z * matrix->right.z;
    at.x = matrix->at.x * matrix->at.x;
    at.y = matrix->at.y * matrix->at.y;
    at.z = matrix->at.z * matrix->at.z;
    upError = up.x + up.y + up.z - lbl_805FB2A4;
    rightError = right.x + right.y + right.z - lbl_805FB2A4;
    atError = at.x + at.y + at.z - lbl_805FB2A4;
    return rightError * rightError + upError * upError + atError * atError;
}

/* Closes the matrix module. */
void *fn_8047E828(void *instance) {
    if (R5MATRIXGLOBAL->freeList != 0) {
        fn_8047FCDC(R5MATRIXGLOBAL->freeList);
        R5MATRIXGLOBAL->freeList = 0;
    }
    lbl_805F1560.numInstances--;
    return instance;
}

RwBool fn_8047E960(const RwV3d *tolerance);

/* Opens the matrix module: the free list of matrices, the default optimizations and tolerances. */
void *fn_8047E88C(void *instance, s32 offset) {
    lbl_805F1560.globalsOffset = offset;
    R5MATRIXGLOBAL->freeList = fn_8047FC04(sizeof(RwMatrix), lbl_805EDE98, 4, lbl_805EDE9C, lbl_805E2A58, 0x4000D);
    if (R5MATRIXGLOBAL->freeList == 0) {
        instance = 0;
    } else {
        RwV3d tolerance = {0.01f, 0.01f, 0.01f};

        R5MATRIXGLOBAL->optimizations = 0x20000;
        R5MATRIXGLOBAL->multiply = fn_8047E100;
        fn_8047E960(&tolerance);
        lbl_805F1560.numInstances++;
    }
    return instance;
}

void fn_8047EBA0(u32 *object) {
    object[3] &= 0xFFFDFFFC;
}

void fn_8047E298(void);

void fn_8047ECB4(void) {
    fn_8047E298();
}

/* RwMatrixSetTolerance. */
RwBool fn_8047E960(const RwV3d *tolerance) {
    R5MATRIXGLOBAL->tolerance = *tolerance;
    return 1;
}

#ifdef __cplusplus
}
#endif

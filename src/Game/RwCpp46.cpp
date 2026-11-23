/* RenderWare core, 0x804823F0..0x80483F8B (a size cut, not a source file):
 *   0x804823F0  rest of the vector module: RwV2dLength, the three paired-single point/vector
 *               transforms, module close and open (the sqrt tables)
 *   0x80482984  arena heap of the resource module: init, free, allocate
 *   0x80482BC4  chunk group module open/close (free list of 0x21-byte entries, hint 0x40412)
 *   0x80482CA4  string module: case-insensitive compare, upper/lower, strchr/strrchr, the
 *               engine's string table, string chunks in streams
 *   0x80483444  path test, RwBBoxCalculate
 *   0x80483554  binary streams of cameras, frame lists and textures
 * NonMatching: fn_80482458, fn_804824A4 and fn_80482504 are hand-written paired-single assembly
 * in retail (psq_l / ps_madds0 / ps_madds1 / psq_st with one point prefetched); GC/2.7 has no C
 * form for them, so they stay scalar C++ here. */
#pragma cplusplus off
#include <dolphin/types.h>
#include "Game/rw/r6.h"

extern RwModuleInfo lbl_805F1598; /* vector module */
extern RwModuleInfo lbl_805F15A0; /* chunk group module */
extern const f32 lbl_805FB2B0;    /* 1.0f */
extern const f32 lbl_805FB2B4;    /* 0.0f */
extern const f64 lbl_805FB2B8;    /* 0.5 */
extern const f64 lbl_805FB2C0;    /* 3.0 */
extern const f64 lbl_805FB2C8;    /* 0.0 */
extern const char lbl_805FB2D0[8]; /* "" */
extern const f32 lbl_805FB2D8;
extern const f32 lbl_805FB2DC;
extern s32 lbl_805EDCC8[]; /* __float_nan */
extern s32 lbl_805EDEC8;
extern s32 lbl_805EDECC;
extern s32 lbl_805EDED0;
extern u8 lbl_805E2AF8[];
extern RwPluginRegistry lbl_8056DF70; /* camera plugins */
extern RwPluginRegistry lbl_8056DFE8; /* frame plugins */
extern RwPluginRegistry lbl_8056E030; /* texture plugins */

RwFreeList *fn_8047FC04(s32 entrySize, s32 entriesPerBlock, s32 alignment, s32 preallocBlocks,
                        void *inPlace, u32 hint);
RwBool fn_8047FCDC(RwFreeList *list);
RwBool fn_8047CDB4(RwStream *stream, u32 *type, u32 *length, u32 *version, u32 *build);
RwStream *fn_8047CEF0(RwStream *stream, u32 type, s32 size, u32 version, u32 build);
RwBool fn_8047CF50(RwStream *stream, u32 type, u32 *length, u32 *version);
void *fn_8047D158(void *mem, u32 size);
void *fn_8047D254(void *mem, u32 size);
f32 fn_8047E704(const RwMatrix *matrix);
f32 fn_8047E750(const RwMatrix *matrix);
f32 fn_8047E7A8(const RwMatrix *matrix);
u32 fn_80480D4C(RwStream *stream, void *buffer, u32 length);
RwStream *fn_80480ED0(RwStream *stream, const void *buffer, u32 length);
RwStream *fn_804810FC(RwStream *stream, u32 offset);
s32 fn_804815F0(RwPluginRegistry *reg, u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB);
const RwPluginRegistry *fn_804816AC(const RwPluginRegistry *reg, RwStream *stream, void *object);
s32 fn_804818FC(const RwPluginRegistry *reg, const void *object);
const RwPluginRegistry *fn_80481980(const RwPluginRegistry *reg, RwStream *stream,
                                    const void *object);
const RwPluginRegistry *fn_80481AD4(const RwPluginRegistry *reg, RwStream *stream);
RwCamera *fn_80485A58(void);
RwCamera *fn_80485504(RwCamera *camera, const RwV2d *offset);
RwCamera *fn_80485550(RwCamera *camera, f32 nearClip);
RwCamera *fn_8048560C(RwCamera *camera, f32 farClip);
RwCamera *fn_8048584C(RwCamera *camera, u32 projection);
RwCamera *fn_80485950(RwCamera *camera, const RwV2d *window);
RwFrame *fn_80486D00(void);
RwBool fn_80487208(RwFrame *frame);
RwFrame *fn_8048722C(RwFrame *frame);
RwFrame *fn_804872D8(RwFrame *frame);
RwFrame *fn_804872E0(RwFrame *parent, RwFrame *child);
RwFrame *fn_80487348(RwFrame *parent, RwFrame *child);
RwBool fn_8048C9A8(RwBool enable);
RwBool fn_8048C9C0(void);
RwBool fn_8048C9D4(RwBool enable);
RwBool fn_8048C9EC(void);
RwTexture *fn_8048D130(const char *name, const char *mask);
int vsprintf(char *, const char *, void *);
char *fn_803ACABC(char *, const char *, u32);
char *fn_803AC6C0(const char *, const char *);
s32 fn_803AC8DC(const char *, const char *, u32);
s32 fn_803AB5B8(const char *, const char *, ...);
char *fn_80482DBC(char *text, int c);
char *fn_80482DF0(char *text, int c);
char *fn_80482D3C(char *text);
char *fn_80482D7C(char *text);
s32 fn_80482CA4(const char *a, const char *b);
u32 fn_80482F64(const char *string);
const char *fn_80482FA4(const char *string, RwStream *stream);

#define RWVECTORGLOBAL(var) (RWGLOBALAT(RwSqrtTables, lbl_805F1598.globalsOffset).var)
#define RWCHUNKGROUPGLOBAL (RWGLOBALAT(RwFreeList *, lbl_805F15A0.globalsOffset))

/* RwV2dLength. */
f32 fn_804823F0(const RwV2d *in) {
    volatile RwSplitBits bits;

    bits.nReal = in->x * in->x + in->y * in->y;
    if (bits.nUInt != 0) {
        bits.nUInt += 0x800;
        bits.nUInt = ((bits.nUInt >> 1) & 0x3FC00000) +
                     RWVECTORGLOBAL(sqrtTab)[(bits.nUInt >> 12) & 0xFFF];
    }
    return bits.nReal;
}

/* RwV3dTransformPoint: out = in * matrix + matrix->pos. */
RwV3d *fn_80482458(RwV3d *out, const RwV3d *in, const RwMatrix *matrix) {
    f32 x = matrix->pos.x + in->x * matrix->right.x;
    f32 y = matrix->pos.y + in->x * matrix->right.y;
    f32 z = matrix->pos.z + in->x * matrix->right.z;

    x = x + in->y * matrix->up.x;
    y = y + in->y * matrix->up.y;
    z = z + in->y * matrix->up.z;
    x = x + in->z * matrix->at.x;
    y = y + in->z * matrix->at.y;
    z = z + in->z * matrix->at.z;
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

/* RwV3dTransformPoints. Retail's loop is count-controlled: it always transforms one point. */
RwV3d *fn_804824A4(RwV3d *out, const RwV3d *in, s32 count, const RwMatrix *matrix) {
    RwV3d *result = out;

    do {
        f32 x = matrix->pos.x + in->x * matrix->right.x;
        f32 y = matrix->pos.y + in->x * matrix->right.y;
        f32 z = matrix->pos.z + in->x * matrix->right.z;

        x = x + in->y * matrix->up.x;
        y = y + in->y * matrix->up.y;
        z = z + in->y * matrix->up.z;
        x = x + in->z * matrix->at.x;
        y = y + in->z * matrix->at.y;
        z = z + in->z * matrix->at.z;
        out->x = x;
        out->y = y;
        out->z = z;
        out++;
        in++;
    } while (--count != 0);
    return result;
}

/* RwV3dTransformVector: the rotation part only. */
RwV3d *fn_80482504(RwV3d *out, const RwV3d *in, const RwMatrix *matrix) {
    f32 x = in->x * matrix->right.x;
    f32 y = in->x * matrix->right.y;
    f32 z = in->x * matrix->right.z;

    x = x + in->y * matrix->up.x;
    y = y + in->y * matrix->up.y;
    z = z + in->y * matrix->up.z;
    x = x + in->z * matrix->at.x;
    y = y + in->z * matrix->at.y;
    z = z + in->z * matrix->at.z;
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

/* _rwVectorClose. */
void *fn_80482548(void *instance) {
    if (RWVECTORGLOBAL(invSqrtTab)) {
        RwEngineInstance->memoryFuncs.rwfree(RWVECTORGLOBAL(invSqrtTab));
        RWVECTORGLOBAL(invSqrtTab) = NULL;
    }
    if (RWVECTORGLOBAL(sqrtTab)) {
        RwEngineInstance->memoryFuncs.rwfree(RWVECTORGLOBAL(sqrtTab));
        RWVECTORGLOBAL(sqrtTab) = NULL;
    }
    lbl_805F1598.numInstances--;
    return instance;
}

/* The C library's inline sqrtf: two Newton steps on the reciprocal square root estimate. */
static inline f32 Sqrt(f32 x) {
    if (x > lbl_805FB2B4) {
        f64 guess = __frsqrte((f64)x);
        guess = lbl_805FB2B8 * guess * (lbl_805FB2C0 - guess * guess * x);
        guess = lbl_805FB2B8 * guess * (lbl_805FB2C0 - guess * guess * x);
        return (f32)(x * guess);
    } else if (x < lbl_805FB2C8) {
        return *(f32 *)lbl_805EDCC8;
    }
    return x;
}

/* Both tables hold 0x800 mantissas for odd exponents after 0x800 for even ones. */
static inline RwBool SqrtTableCreate(void) {
    u32 *table = (u32 *)RwEngineInstance->memoryFuncs.rwmalloc(0x4000, 0x40401);

    if (!table) {
        RWERROR((0x80000013, 0x4000));
        return FALSE;
    } else {
        volatile RwSplitBits spin;
        volatile RwSplitBits root;
        u32 *entry;
        s32 i;

        spin.nReal = lbl_805FB2B0;
        entry = table + 0x800;
        for (i = 0; i < 0x800; i++) {
            volatile u32 bits;
            root.nReal = Sqrt(spin.nReal);
            bits = root.nUInt - 0x1FC00000;
            *entry++ = bits;
            spin.nUInt += 0x1000;
        }
        entry = table;
        for (i = 0; i < 0x800; i++) {
            volatile RwSplitBits root2;
            volatile u32 bits;
            root2.nReal = Sqrt(spin.nReal);
            bits = root2.nUInt - 0x20000000;
            *entry++ = bits;
            spin.nUInt += 0x1000;
        }
        RWVECTORGLOBAL(sqrtTab) = table;
    }
    return TRUE;
}

static inline RwBool InvSqrtTableCreate(void) {
    u32 *table = (u32 *)RwEngineInstance->memoryFuncs.rwmalloc(0x4000, 0x40401);

    if (!table) {
        RWERROR((0x80000013, 0x4000));
        return FALSE;
    } else {
        volatile RwSplitBits spin;
        volatile RwSplitBits root;
        u32 *entry;
        s32 i;

        spin.nReal = lbl_805FB2B0;
        entry = table + 0x800;
        for (i = 0; i < 0x800; i++) {
            volatile u32 bits;
            root.nReal = lbl_805FB2B0 / Sqrt(spin.nReal);
            bits = root.nUInt - 0x20000000;
            *entry++ = bits;
            spin.nUInt += 0x1000;
        }
        entry = table;
        for (i = 0; i < 0x800; i++) {
            volatile RwSplitBits root2;
            volatile u32 bits;
            root2.nReal = lbl_805FB2B0 / Sqrt(spin.nReal);
            bits = root2.nUInt - 0x1FC00000;
            *entry++ = bits;
            spin.nUInt += 0x1000;
        }
        RWVECTORGLOBAL(invSqrtTab) = table;
    }
    return TRUE;
}

/* _rwVectorOpen. */
void *fn_804825EC(void *instance, s32 offset) {
    lbl_805F1598.globalsOffset = offset;
    if (!SqrtTableCreate()) {
        return NULL;
    }
    if (!InvSqrtTableCreate()) {
        return NULL;
    }
    lbl_805F1598.numInstances++;
    return instance;
}

/* Heap initialise: the first block header starts on the 32-byte boundary after the heap's two
 * words; fails when less than one 32-byte block would remain. */
RwBool fn_80482984(RwHeap *heap, u32 size) {
    RwHeapBlock *block = (RwHeapBlock *)(((u32)heap + 0x27) & ~0x1F);
    s32 space = (((u32)heap + size) & ~0x1F) - (u32)block - sizeof(RwHeapBlock);

    if (space < 0x20) {
        return FALSE;
    }
    block->heap = heap;
    block->next = NULL;
    block->prev = NULL;
    block->used = 0;
    block->size = space;
    heap->firstBlock = block;
    heap->firstFree = block;
    return TRUE;
}

RwBool fn_804829D4(void) {
    return TRUE;
}

/* Heap free: the block is merged with a free neighbour on either side. */
void fn_804829DC(void *mem) {
    RwHeapBlock *block = (RwHeapBlock *)((u8 *)mem - sizeof(RwHeapBlock));
    RwHeap *heap;
    RwHeapBlock *prev;
    RwHeapBlock *next;

    block->used = 0;
    heap = block->heap;
    prev = block->prev;
    next = block->next;
    if (!heap->firstFree || block < heap->firstFree) {
        heap->firstFree = block;
    }
    if (prev && (~prev->used & 1)) {
        prev->next = next;
        if (next) {
            next->prev = prev;
        }
        prev->size += block->size + sizeof(RwHeapBlock);
        block = prev;
    }
    if (next && (~next->used & 1)) {
        block->next = next->next;
        if (next->next) {
            next->next->prev = block;
        }
        block->size += next->size + sizeof(RwHeapBlock);
    }
}

/* Heap allocate: first fit from the first free block, splitting a block that is more than a
 * header and 32 bytes too large. */
void *fn_80482A94(RwHeap *heap, u32 size) {
    RwHeapBlock *block = heap->firstFree;
    RwHeapBlock *found = NULL;

    size = (size + 0x1F) & ~0x1F;
    while (block && !found) {
        if ((~block->used & 1) && block->size >= size) {
            found = block;
        }
        block = block->next;
    }
    if (!found) {
        return NULL;
    }
    if (found->size > size + 0x40) {
        RwHeapBlock *next = found->next;
        RwHeapBlock *split;
        split = (RwHeapBlock *)((u32)found + size + sizeof(RwHeapBlock));
        if (next && (~next->used & 1)) {
            split->next = next->next;
            split->size = found->next->size + (found->size - size);
        } else {
            split->next = next;
            split->size = found->size - size - sizeof(RwHeapBlock);
        }
        found->next = split;
        split->used = 0;
        split->prev = found;
        if (split->next) {
            split->next->prev = split;
        }
        found->size = size;
        split->heap = found->heap;
    }
    if (found == heap->firstFree) {
        do {
            heap->firstFree = heap->firstFree->next;
        } while (heap->firstFree && (heap->firstFree->used & 1));
    }
    found->used = 1;
    return (u8 *)found + sizeof(RwHeapBlock);
}

/* Chunk group module open. */
void *fn_80482BC4(void *instance, s32 offset) {
    lbl_805F15A0.globalsOffset = offset;
    RWCHUNKGROUPGLOBAL = fn_8047FC04(0x21, lbl_805EDEC8, 4, lbl_805EDECC, lbl_805E2AF8, 0x40412);
    if (!RWCHUNKGROUPGLOBAL) {
        return NULL;
    }
    lbl_805F15A0.numInstances++;
    return instance;
}

/* Chunk group module close. */
void *fn_80482C50(void *instance) {
    if (RWCHUNKGROUPGLOBAL) {
        fn_8047FCDC(RWCHUNKGROUPGLOBAL);
    }
    lbl_805F15A0.numInstances--;
    return instance;
}

/* The string table's case-insensitive compare. */
s32 fn_80482CA4(const char *a, const char *b) {
    if (a && b) {
        char ca;
        char cb;
        do {
            ca = *a;
            cb = *b;
            if (ca >= 'A' && ca <= 'Z') {
                ca += 'a' - 'A';
            }
            if (cb >= 'A' && cb <= 'Z') {
                cb += 'a' - 'A';
            }
            if (ca != cb) {
                return ca - cb;
            }
            a++;
            b++;
        } while (ca && cb);
        if (ca != cb) {
            return ca - cb;
        }
    }
    return 0;
}

/* Upper-case in place. */
char *fn_80482D3C(char *text) {
    char *cursor;

    if (text == NULL) {
        return text;
    }
    cursor = text;
    while (*cursor != 0) {
        char current = *cursor;
        if (current >= 'a' && current <= 'z') {
            *cursor = current - 0x20;
        }
        cursor++;
    }
    return text;
}

/* Lower-case in place. */
char *fn_80482D7C(char *text) {
    char *cursor;

    if (text == NULL) {
        return text;
    }
    cursor = text;
    while (*cursor != 0) {
        char current = *cursor;
        if (current >= 'A' && current <= 'Z') {
            *cursor = current + 0x20;
        }
        cursor++;
    }
    return text;
}

/* strchr. */
char *fn_80482DBC(char *text, int c) {
    char *found = NULL;
    char value = (char)c;
    int current;
    do {
        current = *text;
        if (current == value) {
            found = text;
            break;
        }
        text++;
    } while ((char)current != 0);
    return found;
}

/* strrchr. */
char *fn_80482DF0(char *text, int c) {
    char *found = NULL;
    char value = (char)c;
    int current;
    do {
        current = *text;
        if (current == value) {
            found = text;
        }
        text++;
    } while ((char)current != 0);
    return found;
}

/* _rwStringOpen: fills the engine's string function table. */
RwBool fn_80482E20(void) {
    RwEngineInstance->stringFuncs.vecSprintf = (void *)sprintf;
    RwEngineInstance->stringFuncs.vecVsprintf = (void *)vsprintf;
    RwEngineInstance->stringFuncs.vecStrcpy = strcpy;
    RwEngineInstance->stringFuncs.vecStrncpy = (void *)fn_803ACABC;
    RwEngineInstance->stringFuncs.vecStrcat = (void *)strcat;
    RwEngineInstance->stringFuncs.vecStrncat = (void *)strncat;
    RwEngineInstance->stringFuncs.vecStrrchr = (void *)fn_80482DF0;
    RwEngineInstance->stringFuncs.vecStrchr = (void *)fn_80482DBC;
    RwEngineInstance->stringFuncs.vecStrstr = (void *)fn_803AC6C0;
    RwEngineInstance->stringFuncs.vecStrcmp = (void *)strcmp;
    RwEngineInstance->stringFuncs.vecStrncmp = (void *)fn_803AC8DC;
    RwEngineInstance->stringFuncs.vecStricmp = (void *)fn_80482CA4;
    RwEngineInstance->stringFuncs.vecStrlen = (u32 (*)(const char *))strlen;
    RwEngineInstance->stringFuncs.vecStrupr = (void *)fn_80482D3C;
    RwEngineInstance->stringFuncs.vecStrlwr = (void *)fn_80482D7C;
    RwEngineInstance->stringFuncs.vecStrtok = (void *)strtok;
    RwEngineInstance->stringFuncs.vecSscanf = (void *)fn_803AB5B8;
    return TRUE;
}

/* _rwStringClose. */
void fn_80482F60(void) {
}

/* _rwStringStreamGetSize: the length with its terminator, padded to four bytes. */
u32 fn_80482F64(const char *string) {
    if (!string) {
        string = lbl_805FB2D0;
    }
    return (RwEngineInstance->stringFuncs.vecStrlen(string) + 4) & ~3;
}

static inline u32 StringStreamGetSize(const char *string) {
    if (!string) {
        string = lbl_805FB2D0;
    }
    return (RwEngineInstance->stringFuncs.vecStrlen(string) + 4) & ~3;
}

/* _rwStringStreamWrite. */
const char *fn_80482FA4(const char *string, RwStream *stream) {
    u32 size;

    if (!string) {
        string = lbl_805FB2D0;
    }
    size = StringStreamGetSize(string);
    if (!fn_8047CEF0(stream, 2, size, 0x37002, 0x37)) {
        return NULL;
    }
    if (!fn_80480ED0(stream, string, size)) {
        return NULL;
    }
    return string;
}

static inline char *StringStreamRead(char *string, RwStream *stream, u32 length) {
    char buffer[64];
    u32 count;
    char *out;

    if (!string) {
        string = (char *)RwEngineInstance->memoryFuncs.rwmalloc(length, 0x30002);
        if (!string) {
            RWERROR((0x80000013, length));
            return NULL;
        }
    }
    out = string;
    while (length > 0) {
        u32 i;
        count = (length > 64) ? 64 : length;
        if (count != fn_80480D4C(stream, buffer, count)) {
            return NULL;
        }
        length -= count;
        for (i = 0; i < count; i++) {
            out[i] = buffer[i];
        }
        out += count;
    }
    return string;
}

static inline char *UnicodeStringStreamRead(char *string, RwStream *stream, u32 length) {
    u16 buffer[64];
    u32 count;
    char *out;
    RwBool allocated = FALSE;

    if (!string) {
        string = (char *)RwEngineInstance->memoryFuncs.rwmalloc(length, 0x30002);
        if (!string) {
            RWERROR((0x80000013, length));
            return NULL;
        }
        allocated = TRUE;
    }
    out = string;
    while (length > 0) {
        u32 i;
        count = (length > 128) ? 128 : length;
        if (count != fn_80480D4C(stream, buffer, count)) {
            if (allocated) {
                RwEngineInstance->memoryFuncs.rwfree(string);
            }
            return NULL;
        }
        length -= count;
        for (i = 0; i < count / 2; i++) {
            out[i] = (char)buffer[i];
        }
        out += count / 2;
    }
    return string;
}

/* _rwStringStreamFindAndRead: skips chunks until a string (2) or unicode string (0x13) chunk. */
char *fn_80483060(char *string, RwStream *stream) {
    u32 type;
    u32 length;
    u32 version;

    while (fn_8047CDB4(stream, &type, &length, &version, NULL)) {
        RwBool valid = FALSE;
        if (version >= 0x35000 && version <= 0x37002) {
            valid = TRUE;
        }
        if (!valid) {
            RWERROR((0x80000004));
            return NULL;
        }
        if (type == 2) {
            return StringStreamRead(string, stream, length);
        } else if (type == 0x13) {
            return UnicodeStringStreamRead(string, stream, length);
        }
        if (!fn_804810FC(stream, length)) {
            return NULL;
        }
    }
    return NULL;
}

/* TRUE for a path with a drive letter or a leading backslash. */
RwBool fn_80483444(const char *path) {
    if (path[1] == ':') {
        if ((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) {
            return TRUE;
        }
    }
    return path[0] == '\\';
}

/* RwBBoxCalculate. */
RwBBox *fn_8048349C(RwBBox *box, const RwV3d *verts, s32 numVerts) {
    box->inf = *verts;
    box->sup = *verts;
    verts++;
    numVerts--;
    while (numVerts--) {
        if (box->inf.x > verts->x) {
            box->inf.x = verts->x;
        }
        if (box->inf.y > verts->y) {
            box->inf.y = verts->y;
        }
        if (box->inf.z > verts->z) {
            box->inf.z = verts->z;
        }
        if (box->sup.x < verts->x) {
            box->sup.x = verts->x;
        }
        if (box->sup.y < verts->y) {
            box->sup.y = verts->y;
        }
        if (box->sup.z < verts->z) {
            box->sup.z = verts->z;
        }
        verts++;
    }
    return box;
}

/* RwCameraRegisterPluginStream. */
s32 fn_80483554(u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return fn_804815F0(&lbl_8056DF70, pluginID, readCB, writeCB, getSizeCB);
}

/* RwCameraStreamRead. */
RwCamera *fn_80483590(RwStream *stream) {
    u32 size;
    u32 version;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        rwStreamCamera cam;
        RwCamera *camera;

        memset(&cam, 0, sizeof(cam));
        if (fn_80480D4C(stream, &cam, size) != size) {
            return NULL;
        }
        fn_8047D254(&cam, sizeof(cam));
        camera = fn_80485A58();
        if (!camera) {
            return NULL;
        }
        if (!fn_804816AC(&lbl_8056DF70, stream, camera)) {
            return NULL;
        }
        fn_80485950(camera, &cam.viewWindow);
        fn_80485504(camera, &cam.viewOffset);
        fn_80485550(camera, cam.nearPlane);
        fn_8048560C(camera, cam.farPlane);
        camera->fogPlane = cam.fogPlane;
        fn_8048584C(camera, cam.projection);
        return camera;
    }
    RWERROR((0x80000004));
    return NULL;
}

/* RwFrameRegisterPluginStream. */
s32 fn_804836EC(u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return fn_804815F0(&lbl_8056DFE8, pluginID, readCB, writeCB, getSizeCB);
}

/* _rwFrameListDeinitialize. */
rwFrameList *fn_80483728(rwFrameList *frameList) {
    if (frameList->numFrames) {
        RwEngineInstance->memoryFuncs.rwfree(frameList->frames);
    }
    return frameList;
}

/* _rwFrameListStreamRead: creates the frames, links each to its parent, then reads every
 * frame's plugin data. */
rwFrameList *fn_80483774(RwStream *stream, rwFrameList *frameList) {
    s32 numFrames;
    u32 size;
    u32 version;
    s32 i;
    RwFrame *frame;
    RwMatrix *matrix;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        if (fn_80480D4C(stream, &numFrames, sizeof(numFrames)) != sizeof(numFrames)) {
            return NULL;
        }
        fn_8047D254(&numFrames, sizeof(numFrames));
        frameList->numFrames = numFrames;
        frameList->frames = (RwFrame **)RwEngineInstance->memoryFuncs.rwmalloc(
            sizeof(RwFrame *) * numFrames, 0x3000E);
        if (!frameList->frames) {
            RWERROR((0x80000013, sizeof(RwFrame *) * numFrames));
            return NULL;
        }
        for (i = 0; i < numFrames; i++) {
            rwStreamFrame f;

            if (fn_80480D4C(stream, &f, sizeof(f)) != sizeof(f)) {
                RwEngineInstance->memoryFuncs.rwfree(frameList->frames);
                return NULL;
            }
            fn_8047D254(&f, sizeof(f));
            frame = fn_80486D00();
            if (!frame) {
                RwEngineInstance->memoryFuncs.rwfree(frameList->frames);
                return NULL;
            }
            matrix = &frame->modelling;
            matrix->right = f.right;
            matrix->up = f.up;
            matrix->at = f.at;
            matrix->pos = f.pos;
            if (lbl_805FB2D8 >= fn_8047E7A8(matrix) && lbl_805FB2D8 >= fn_8047E750(matrix) &&
                lbl_805FB2DC <= fn_8047E704(matrix)) {
                matrix->flags &= ~0x20000;
            } else {
                matrix->flags &= ~0x20003;
            }
            frameList->frames[i] = frame;
            if (f.parentIndex >= 0) {
                if (lbl_805EDED0) {
                    fn_80487348(frameList->frames[f.parentIndex], frame);
                } else {
                    fn_804872E0(frameList->frames[f.parentIndex], frame);
                }
            }
        }
    } else {
        RWERROR((0x80000004));
        return NULL;
    }
    for (i = 0; i < numFrames; i++) {
        frame = frameList->frames[i];
        if (!fn_804816AC(&lbl_8056DFE8, stream, frame)) {
            fn_80487208(frameList->frames[0]);
            RwEngineInstance->memoryFuncs.rwfree(frameList->frames);
            return NULL;
        }
        if (frame == fn_804872D8(frame) && lbl_805EDED0 == 1) {
            fn_8048722C(frame);
        }
    }
    return frameList;
}

/* RwTextureRegisterPluginStream. */
s32 fn_80483AFC(u32 pluginID, RwPluginDataChunkReadCallBack readCB,
                RwPluginDataChunkWriteCallBack writeCB,
                RwPluginDataChunkGetSizeCallBack getSizeCB) {
    return fn_804815F0(&lbl_8056E030, pluginID, readCB, writeCB, getSizeCB);
}

static inline u32 TextureStreamGetSize(const RwTexture *texture) {
    u32 size;

    size = fn_80482F64(texture->name) + 0x1C;
    size += fn_80482F64(texture->mask) + 12;
    size += fn_804818FC(&lbl_8056E030, texture) + 12;
    return size;
}

/* RwTextureStreamGetSize. */
u32 fn_80483B38(const RwTexture *texture) {
    return TextureStreamGetSize(texture);
}

/* RwTextureStreamWrite: a texture chunk holding the filter word, both names and the plugins. */
const RwTexture *fn_80483BA0(const RwTexture *texture, RwStream *stream) {
    u32 flags;
    RwBool userMipmaps;

    if (!fn_8047CEF0(stream, 6, TextureStreamGetSize(texture), 0x37002, 0x37)) {
        return NULL;
    }
    if (!fn_8047CEF0(stream, 1, sizeof(flags), 0x37002, 0x37)) {
        return NULL;
    }
    if (texture->raster && !(texture->raster->cFormat & 0x10)) {
        userMipmaps = TRUE;
    } else {
        userMipmaps = FALSE;
    }
    flags = (texture->filterAddressing & 0xFFFF) | ((userMipmaps & 0xFF) << 16);
    fn_8047D158(&flags, sizeof(flags));
    if (!fn_80480ED0(stream, &flags, sizeof(flags))) {
        return NULL;
    }
    if (!fn_80482FA4(texture->name, stream)) {
        return NULL;
    }
    if (!fn_80482FA4(texture->mask, stream)) {
        return NULL;
    }
    if (!fn_80481980(&lbl_8056E030, stream, texture)) {
        return NULL;
    }
    return texture;
}

/* RwTextureStreamRead: the texture is found by name; mipmapping follows the stored filter while
 * it is read and the plugin data is only read for a texture nobody else holds. */
RwTexture *fn_80483D28(RwStream *stream) {
    u32 size;
    u32 version;

    if (!fn_8047CF50(stream, 1, &size, &version)) {
        return NULL;
    }
    if (version >= 0x35000 && version <= 0x37002) {
        u32 flags;
        char name[128];
        char mask[128];
        RwTexture *texture;
        s32 filtering;
        RwBool oldMipmapping;
        RwBool oldAutoMipmapping;
        u32 mipFlags;
        u32 addressingU;
        u32 addressingV;

        memset(&flags, 0, sizeof(flags));
        if (fn_80480D4C(stream, &flags, size) != size) {
            return NULL;
        }
        fn_8047D254(&flags, sizeof(flags));
        filtering = flags & 0xFF;
        addressingV = (flags >> 12) & 0xF;
        addressingU = (flags >> 8) & 0xF;
        if (addressingV == 0) {
            flags |= (addressingU & 0xF) << 12;
        }
        mipFlags = (flags >> 16) & 0xFF;
        oldMipmapping = fn_8048C9C0();
        oldAutoMipmapping = fn_8048C9EC();
        if (filtering == 3 || filtering == 4 || filtering == 5 || filtering == 6) {
            fn_8048C9A8(TRUE);
            if (mipFlags & 1) {
                fn_8048C9D4(FALSE);
            } else {
                fn_8048C9D4(TRUE);
            }
        } else {
            fn_8048C9A8(FALSE);
            fn_8048C9D4(FALSE);
        }
        if (!fn_80483060(name, stream)) {
            fn_8048C9A8(oldMipmapping);
            fn_8048C9D4(oldAutoMipmapping);
            return NULL;
        }
        if (!fn_80483060(mask, stream)) {
            fn_8048C9A8(oldMipmapping);
            fn_8048C9D4(oldAutoMipmapping);
            return NULL;
        }
        texture = fn_8048D130(name, mask);
        if (!texture) {
            fn_80481AD4(&lbl_8056E030, stream);
            fn_8048C9A8(oldMipmapping);
            fn_8048C9D4(oldAutoMipmapping);
            return NULL;
        }
        fn_8048C9A8(oldMipmapping);
        fn_8048C9D4(oldAutoMipmapping);
        if (texture->refCount == 1) {
            texture->filterAddressing = flags & 0xFFFF;
            if (!fn_804816AC(&lbl_8056E030, stream, texture)) {
                return NULL;
            }
        } else {
            if (!fn_80481AD4(&lbl_8056E030, stream)) {
                return NULL;
            }
        }
        return texture;
    }
    RWERROR((0x80000004));
    return NULL;
}

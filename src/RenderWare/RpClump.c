/* RenderWare: the clump module (rwID_CLUMPMODULE, 0x504): atomics and clumps. The module's open
 * and close callbacks (the atomic and clump free lists), the camera and light extensions that list a
 * clump's members, the atomic's stream extension, RpAtomicCreate/Destroy/Clone/SetGeometry, the
 * bounding sphere code, the frame and geometry callbacks, RpClumpCreate/Destroy/Clone, the stream
 * readers (geometry list, atomic, RpClumpStreamRead) and the atomic/clump plugin registration API.
 *
 * 0x80455D0C-0x80458CF8 (.data 0x8056D7E0-0x8056D810, .bss 0x805E1580-0x805E15C8,
 * .sdata 0x805EDDA0-0x805EDDB0, .sbss 0x805F14B8-0x805F14D0, .sdata2 0x805FB110-0x805FB118).
 * RpWorldStream.c comes before it (its module record at 0x805F14B0 closes that file); the geometry
 * module (bageomet, plugin 0x503) follows at 0x80458CF8, so the file runs past this range's end at
 * 0x80458C28: the four registry wrappers at 0x80458C28-0x80458CF8 still address the atomic
 * registry (lbl_8056D7E0), which RpGeometry's code never does.
 *
 * Built as C with cflags_rw_c (configure.py). Most functions are wave-40 drafts (written then as
 * C++ in Game/RwCpp18.cpp and Game/RwCpp19.cpp, the newer uncommitted RwCpp19 where it matched
 * better); the external calls are declared without prototypes because the drafts pass them words
 * and pointers alike, except the variadic error call (crclr) and the calls whose result signedness
 * shows in a compare. fn_80456F0C copies the morph target's bounding sphere as one struct (word
 * pairs, as C copies structs); RpAtomicCreate (fn_80456DD8) and RpClumpCreate (fn_804573A8) match
 * with their list heads initialised as chained assignments (prev = next = &head), which gives
 * retail's scratch registers (probes/RenderWare_RpClump_W41c.py, W41d). fn_80457608
 * (RpClumpAddAtomic) loads the list head after forming
 * the two link addresses, where retail loads it first (7 rows, probes/RenderWare_RpClump_W41b.py).
 * RwCpp19Fail stands in for retail's inlined failure path of fn_80457698 (RpClumpStreamRead, a
 * draft).
 */
#include <dolphin/types.h>

typedef struct ClumpModuleInfo {
    s32 globalsOffset;
    s32 numInstances;
} ClumpModuleInfo;

typedef struct ClumpSphere {
    f32 x;
    f32 y;
    f32 z;
    f32 radius;
} ClumpSphere;

typedef struct ClumpFreeList {
    u32 words[9];
} ClumpFreeList;

extern u8 *lbl_805F15B4;
extern u32 lbl_805F1690;

/* The atomic and clump plugin registries (RwPluginRegistry: size, original size, maximum size,
 * static allocation, first and last entry). */
u32 lbl_8056D7E0[6] = {0x70, 0x70, 0, 0, 0, 0};
u32 lbl_8056D7F8[6] = {0x2C, 0x2C, 0, 0, 0, 0};

static s32 atomicFreeListBlockSize = 0x80;
static s32 atomicFreeListPreallocBlocks = 1;
static s32 clumpFreeListBlockSize = 0x80;
static s32 clumpFreeListPreallocBlocks = 1;

static ClumpFreeList clumpFreeList;
static ClumpFreeList atomicFreeList;

static ClumpModuleInfo clumpModule;
static u32 atomicExtA;
static u32 atomicExtB;
static u32 clumpLightExtOffset;
static u32 clumpCameraExtOffset;

void *RwFrameGetLTM();
u32 fn_8049B674();
void *fn_8045985C();
void *fn_804599C8();
void *fn_8045984C();
s32 fn_8045AAA0();
u32 fn_8045AB1C();
u32 fn_804622F0();
s32 fn_8047CF50();
void fn_8047D254();
u32 fn_8047D458();
u32 fn_8047D868();
u32 fn_80480D4C();
u32 fn_8047DC28(u32, ...);
void fn_8047DB84();
void *fn_8047FC04();
void fn_8047FCDC();
void fn_804800DC();
void fn_8048071C();
s32 fn_804815F0();
u32 fn_804816AC();
void fn_80481870();
s32 fn_80481DC8();
s32 fn_80481634();
s32 fn_80481670();
s32 fn_80481D98();
void fn_8048209C();
void fn_80482144();
void fn_804821B0();
f32 fn_80482340(f32);
void fn_80482458();
u32 fn_80483590();
u32 fn_80483728();
u32 fn_80483774();
s32 fn_804859B8();
void fn_804859F8();
void *fn_80486C5C();
void *fn_80486CA4();
s32 fn_80486CF0();
void fn_80487208();
void fn_8048722C();
void fn_8048D7C0();
void fn_8048D834();
void fn_8045ACD4();
void *memset(void *, int, u32);

void fn_80455D0C();
void fn_80455D2C();
void * fn_80455D4C();
void * fn_80455DC8();
u32 fn_80455E44();
void * fn_80455E6C();
void * fn_80455EB4();
u32 * fn_80455F18();
u8 * fn_804561F0();
void fn_8045620C();
u8 * fn_80456210();
void fn_8045622C();
void fn_80456230();
u8 * fn_80456234();
void fn_80456560();
u32 * fn_804566D0();
u32 * fn_80456994();
u32 * fn_80456A50();
s32 fn_80456B48();
u32 * fn_80456BC4();
u32 * fn_80456C54();
u32 * fn_80456CD0();
u32 * fn_80456D54();
u8 * fn_80456DD8();
u8 * fn_80456F0C();
u32 fn_80456FD8();
u32 fn_80457090();
u32 * fn_804573A8();
u32 fn_80457468();
void fn_80457608();
u8 * fn_80457630();
u8 * fn_80457664();
u32 * fn_80457698();
void fn_80458B6C();
void fn_80458BAC();
void fn_80458BEC();
s32 fn_80458C28();
s32 fn_80458C5C();
s32 fn_80458C90();
u8 * fn_80458CBC();




void fn_80455D0C(void) {
    fn_80457468();
}

void fn_80455D2C(void *object) {
    fn_80456FD8((u8 *)object);
}


void *fn_80455D4C(void *stream, s32 type) {
    if (fn_8047D868(stream, &atomicExtA, 4) == 0) {
        return NULL;
    }
    if (type == 8 && fn_8047D868(stream, &atomicExtB, 4) == 0) {
        return NULL;
    }
    return stream;
}

void *fn_80455DC8(void *stream, void *entry, const u32 *object) {
    if (fn_8047D458(stream, (u8 *)object[0x1B] + 44, 4) == 0) {
        return NULL;
    }
    if (fn_8047D458(stream, (u8 *)object[0x1B] + 48, 4) != 0) {
        return stream;
    }
    return NULL;
}

u32 fn_80455E44(u32 *object) {
    u32 *nested = (u32 *)object[0x1B];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}


void *fn_80455E6C(void *object) {
    if (((u32 *)object)[0x13] & 2) {
        fn_80456560((u8 *)object);
    }
    ((u8 *)object)[3] |= 1;
    return object;
}

void *fn_80455EB4(void *object) {
    void *resource = ((void **)object)[0x1B];
    if (resource == NULL) {
        resource = *(void **)(lbl_805F15B4 + lbl_805F1690 + 0x3C);
    }
    if (fn_8049B674(resource, object, 1) != 0) {
        return object;
    }
    return NULL;
}

static inline void RwCpp18ClearStreamOwner(u32 *owner) {
    u32 offset = 0;
    { s32 i; for (i = 0; i < (s32)owner[1]; i++) {
        fn_8045985C((u32 *)*(u32 *)(owner[0] + offset));
        offset += 4;
    } }
    if (owner[0] != 0) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))((u32 *)owner[0]);
        owner[0] = 0;
    }
}

u32 *fn_80455F18(void *stream, u32 *owner) {
    u32 count;
    u32 length;
    u32 version;
    if (fn_8047CF50(stream, 1, &length, &version) == 0) {
        return NULL;
    }
    if (version < 0x00035000 || version > 0x00037002) {
        goto invalid;
    }
    if (fn_80480D4C(stream, &count, 4) != 4) {
        return NULL;
    }
    fn_8047D254(&count, 4);
    owner[1] = 0;
    if ((s32)count > 0) {
        owner[0] = (u32)((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(
            count * 4, 0x0003000F);
        if (owner[0] == 0) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000013, count * 4);
            fn_8047DB84(error);
            return NULL;
        }
    } else {
        owner[0] = 0;
    }
    { u32 offset = 0;
    { s32 i; for (i = 0; i < (s32)count; i++) {
        if (fn_8047CF50(stream, 15, NULL, &version) == 0) {
            RwCpp18ClearStreamOwner(owner);
            return NULL;
        }
        if (version >= 0x00035000 && version <= 0x00037002) {
            void *entry = fn_804599C8(stream);
            *(u32 *)(owner[0] + offset) = (u32)entry;
            if (entry == NULL) {
                RwCpp18ClearStreamOwner(owner);
                return NULL;
            }
            owner[1]++;
        } else {
            RwCpp18ClearStreamOwner(owner);
            return NULL;
        }
        offset += 4;
    } }
    goto success;
invalid:
    {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        return NULL;
    }
success:
    return owner;
 }}

u8 *fn_804561F0(u8 *base) {
    u32 *target = (u32 *)(base + clumpCameraExtOffset);
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
    return base;
}


void fn_8045620C(void) {
}

u8 *fn_80456210(u8 *base) {
    u32 *target = (u32 *)(base + clumpLightExtOffset);
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
    return base;
}

void fn_8045622C(void) {
}

void fn_80456230(void) {
}


u8 *fn_80456234(void *stream, u32 *firstOwner, u32 *secondOwner) {
    u32 length;
    u32 version;
    u32 header[4];
    if (fn_8047CF50(stream, 1, &length, &version) == 0) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x8000001A);
        fn_8047DB84(error);
        return NULL;
    }
    if (version < 0x00035000 || version > 0x00037002) {
        goto invalid;
    }
    memset(header, 0, 16);
    { u32 read_ok = fn_80480D4C(stream, header, length) == length;
    if (read_ok == 0) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x8000001A);
        fn_8047DB84(error);
        return NULL;
    }
    fn_8047D254(header, 16);
    { u8 *object = fn_80456DD8();
    if (object == NULL) {
        return NULL;
    }
    object[2] = header[2];
    if ((s32)firstOwner[1] != 0) {
        fn_80458CBC(object, ((u32 **)firstOwner[0])[header[0]]);
    }
    if ((s32)secondOwner[1] != 0) {
        fn_80456F0C(object, ((u32 **)secondOwner[0])[header[1]], 0);
    } else {
        if (fn_8047CF50(stream, 15, NULL, &version) == 0) {
            fn_80456FD8(object);
            { u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x8000001A);
            fn_8047DB84(error);
            return NULL;
         }}
        { u32 *nested;
        if (version >= 0x00035000 && version <= 0x00037002) {
            nested = (u32 *)fn_804599C8(stream);
            if (nested == NULL) {
                fn_80456FD8(object);
                { u32 error[2];
                error[0] = 2;
                error[1] = fn_8047DC28(0x8000001A);
                fn_8047DB84(error);
                return NULL;
             }}
            goto nested_valid;
        }
    nested_invalid:
        {
            fn_80456FD8(object);
            { u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            return NULL;
         }}
    nested_valid:
        fn_80456F0C(object, nested, 0);
        fn_8045985C(nested);
     }}
    atomicExtA = 0;
    atomicExtB = 0;
    { int write_ok = fn_804816AC(lbl_8056D7E0, stream, object) != 0;
    if (!write_ok) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x8000001A);
        fn_8047DB84(error);
        return NULL;
    }
    if (atomicExtA != 0) {
        fn_80481870(lbl_8056D7E0, atomicExtA, object, atomicExtB);
    }
    return object;
invalid:
    {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        return NULL;
    }
 } } }}



void fn_80456560(u8 *object) {
    u32 *fields = (u32 *)object;
    s32 first = ((s16 *)object)[40];
    s32 second = ((s16 *)object)[41];
    u32 *current = (u32 *)fields[6];
    if (current == NULL) {
        return;
    }
    if (first != second && first < (s32)current[6] && second < (s32)current[6]) {
        goto interpolate;
    }
    if (first < (s32)current[6] && second < (s32)current[6]) {
        goto copy_indexed;
    }
    {
        u32 *source = (u32 *)current[23];
        fields[7] = source[1];
        fields[8] = source[2];
        fields[9] = source[3];
        fields[10] = source[4];
    }
    goto finished;
copy_indexed:
    {
        u32 *source = (u32 *)((u8 *)current[23] + first * 28);
        fields[7] = source[1];
        fields[8] = source[2];
        fields[9] = source[3];
        fields[10] = source[4];
    }
    goto finished;
interpolate:
    {
        u32 *base = (u32 *)current[23];
        u32 *left = (u32 *)((u8 *)base + first * 28);
        u32 *right = (u32 *)((u8 *)base + second * 28);
        f32 factor = ((f32 *)object)[22] * ((f32 *)object)[23];
        ((f32 *)object)[10] = factor * (((f32 *)right)[4] - ((f32 *)left)[4]) + ((f32 *)left)[4];
        ((f32 *)object)[7] = ((f32 *)right)[1] - ((f32 *)left)[1];
        ((f32 *)object)[8] = ((f32 *)right)[2] - ((f32 *)left)[2];
        ((f32 *)object)[9] = ((f32 *)right)[3] - ((f32 *)left)[3];
        ((f32 *)object)[7] *= factor;
        ((f32 *)object)[8] *= factor;
        ((f32 *)object)[9] *= factor;
        ((f32 *)object)[7] += ((f32 *)left)[1];
        ((f32 *)object)[8] += ((f32 *)left)[2];
        ((f32 *)object)[9] += ((f32 *)left)[3];
    }
finished:
    fields[19] &= ~2;
    object[3] |= 1;
}


u32 *fn_804566D0(u8 *object) {
    u32 *fields = (u32 *)object;
    u32 *child = (u32 *)fields[1];
    if ((fields[19] & 2) != 0 && fields[6] != 0) {
        u32 *current = (u32 *)fields[6];
        s32 first = ((s16 *)object)[40];
        s32 second = ((s16 *)object)[41];
        if (first != second && first < (s32)current[6] && second < (s32)current[6]) {
            goto interpolate;
        }
        if (first < (s32)current[6] && second < (s32)current[6]) {
            goto copy_indexed;
        }
        {
            u32 *source = (u32 *)current[23];
            fields[7] = source[1];
            fields[8] = source[2];
            fields[9] = source[3];
            fields[10] = source[4];
        }
        goto updated;
    copy_indexed:
        {
            u32 *source = (u32 *)((u8 *)current[23] + first * 28);
            fields[7] = source[1];
            fields[8] = source[2];
            fields[9] = source[3];
            fields[10] = source[4];
        }
        goto updated;
    interpolate:
        {
            u32 *base = (u32 *)current[23];
            u32 *left = (u32 *)((u8 *)base + first * 28);
            u32 *right = (u32 *)((u8 *)base + second * 28);
            f32 factor = ((f32 *)object)[22] * ((f32 *)object)[23];
            ((f32 *)object)[10] = factor * (((f32 *)right)[4] - ((f32 *)left)[4]) + ((f32 *)left)[4];
            ((f32 *)object)[7] = ((f32 *)right)[1] - ((f32 *)left)[1];
            ((f32 *)object)[8] = ((f32 *)right)[2] - ((f32 *)left)[2];
            ((f32 *)object)[9] = ((f32 *)right)[3] - ((f32 *)left)[3];
            ((f32 *)object)[7] *= factor;
            ((f32 *)object)[8] *= factor;
            ((f32 *)object)[9] *= factor;
            ((f32 *)object)[7] += ((f32 *)left)[1];
            ((f32 *)object)[8] += ((f32 *)left)[2];
            ((f32 *)object)[9] += ((f32 *)left)[3];
        }
    updated:
        fields[19] &= ~2;
        object[3] |= 1;
    }
    if (fn_80486CF0(child) != 0 || (object[3] & 1) != 0) {
        u32 *matrix = (u32 *)RwFrameGetLTM(child);
        fn_80482458((u32 *)(object + 44), fields + 7, matrix);
        if ((matrix[3] & 3) != 3) {
            f32 *m = (f32 *)matrix;
            f32 first = m[4] * m[4] + m[5] * m[5] + m[6] * m[6];
            f32 second = m[8] * m[8] + m[9] * m[9] + m[10] * m[10];
            f32 third = m[0] * m[0] + m[1] * m[1] + m[2] * m[2];
            f32 best = second >= first ? first : second;
            if (third >= best) {
                third = second >= first ? first : second;
            }
            ((f32 *)object)[14] = ((f32 *)object)[10] * fn_80482340(third);
        } else {
            ((f32 *)object)[14] = ((f32 *)object)[10];
        }
        object[3] &= 0xFE;
    }
    return (u32 *)(object + 44);
}


u32 *fn_80456994(u32 *object) {
    fn_804800DC(*(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset + 4), fn_80455D0C, 0);
    fn_804800DC(*(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset), fn_80455D2C, 0);
    fn_8047FCDC(*(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset));
    fn_8047FCDC(*(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset + 4));
    *(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset) = NULL;
    *(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset + 4) = NULL;
    clumpModule.numInstances--;
    return object;
}


u32 *fn_80456A50(u32 *object, u32 config) {
    clumpModule.globalsOffset = config;
    { u32 *first = fn_8047FC04(*(u32 *)lbl_8056D7E0, atomicFreeListBlockSize, 4, atomicFreeListPreallocBlocks,
                             &atomicFreeList, 0x00040010);
    *(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset) = first;
    if (*(u32 *)(lbl_805F15B4 + clumpModule.globalsOffset) == 0) {
        goto failure;
    }
    { u32 *second = fn_8047FC04(*(u32 *)lbl_8056D7F8, clumpFreeListBlockSize, 4, clumpFreeListPreallocBlocks,
                              &clumpFreeList, 0x00040014);
    *(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset + 4) = second;
    if (*(u32 *)(lbl_805F15B4 + clumpModule.globalsOffset + 4) != 0) {
        clumpModule.numInstances++;
        return object;
    }
    fn_8047FCDC(*(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset));
    *(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset) = NULL;
failure:
    return NULL;
 } }}

s32 fn_80456B48(void) {
    s32 result = fn_804859B8(12, 16, fn_804561F0, fn_8045620C, 0);
    clumpCameraExtOffset = result;
    if (result < 0) {
        return 0;
    }
    result = fn_8045AAA0(12, 16, fn_80456210, fn_8045622C, 0);
    clumpLightExtOffset = result;
    return ((u32)result >> 31) ^ 1;
}

u32 *fn_80456BC4(u32 *object) {
    u32 *result = object;
    u32 *current = (u32 *)result[2];
    u32 *sentinel = result + 2;
    while (current != sentinel) {
        u32 *frame = current - 16;
        if ((((u8 *)frame)[2] & 4) != 0) {
            RwFrameGetLTM((void *)frame[1]);
            if (((u32 *(*)(u32 *))frame[18])(frame) == 0) {
                result = NULL;
            }
        }
        current = (u32 *)current[0];
    }
    return result;
}


u32 *fn_80456C54(u32 *object, u32 (*callback)(u32 *, u32), u32 data) {
    u32 *sentinel = object + 2;
    u32 *current = (u32 *)object[2];
    while (current != sentinel) {
        u32 *next = (u32 *)current[0];
        if (callback(current - 16, data) == 0) {
            return object;
        }
        current = next;
    }
    return object;
}

u32 *fn_80456CD0(u32 *object, u32 (*callback)(u32 *, u32), u32 data) {
    u32 *sentinel = object + 6;
    u32 *current = (u32 *)object[6];
    while (current != sentinel) {
        u32 *entry = (u32 *)((u8 *)current - 4 - clumpCameraExtOffset);
        u32 *next = (u32 *)current[0];
        if (callback(entry, data) == 0) {
            return object;
        }
        current = next;
    }
    return object;
}

u32 *fn_80456D54(u32 *object, u32 (*callback)(u32 *, u32), u32 data) {
    u32 *sentinel = object + 4;
    u32 *current = (u32 *)object[4];
    while (current != sentinel) {
        u32 *entry = (u32 *)((u8 *)current - 4 - clumpLightExtOffset);
        u32 *next = (u32 *)current[0];
        if (callback(entry, data) == 0) {
            return object;
        }
        current = next;
    }
    return object;
}


u8 *fn_80456DD8(void) {
    u8 *object = ((u8 *(*)(u32 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        *(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset), 0x00030014);
    if (object == NULL) {
        return NULL;
    }
    { u32 *fields = (u32 *)object;
    object[0] = 1;
    object[1] = 0;
    object[2] = 0;
    object[3] = 0;
    fields[1] = 0;
    fields[4] = (u32)fn_80455E6C;
    fields[5] = 0;
    object[2] = 5;
    object[3] = 1;
    fn_80458CBC(object, NULL);
    fields[6] = 0;
    { f32 value = 0.0f;
    ((f32 *)object)[10] = value;
    ((f32 *)object)[7] = value;
    ((f32 *)object)[8] = value;
    ((f32 *)object)[9] = value;
    ((f32 *)object)[14] = value;
    ((f32 *)object)[11] = value;
    ((f32 *)object)[12] = value;
    ((f32 *)object)[13] = value;
    fields[18] = (u32)fn_80455EB4;
    if (fields[18] == 0) {
        fields[18] = (u32)fn_80455EB4;
    }
    ((s16 *)object)[40] = 0;
    ((s16 *)object)[41] = 0;
    { f32 other = 1.0f;
    ((f32 *)object)[21] = other;
    ((f32 *)object)[22] = other;
    ((f32 *)object)[23] = 0.0f;
    fields[19] = 3;
    fields[17] = 0;
    fields[16] = 0;
    fields[15] = 0;
    fields[27] = 0;
    fields[26] = fields[25] = (u32)(fields + 25);
    fn_8048209C(lbl_8056D7E0, object);
    return object;
 } } }}


u8 *fn_80456F0C(u8 *object, u32 *nested, u32 flags) {
    u32 *fields = (u32 *)object;
    if (nested != (u32 *)fields[6]) {
        if (nested != NULL) {
            fn_8045984C(nested);
        }
        if (fields[6] != 0) {
            fn_8045985C((u32 *)fields[6]);
        }
        fields[6] = (u32)nested;
        if ((flags & 1) == 0) {
            if (nested != NULL) {
                *(ClumpSphere *)&fields[7] = *(ClumpSphere *)((u8 *)nested[23] + 4);
            }
            { u32 *child = (u32 *)fields[1];
            if (child != NULL && fn_804622F0(object) != 0) {
                fn_8048722C(child);
            }
         }}
    }
    return object;
}

u32 fn_80456FD8(u8 *object) {
    u32 *fields = (u32 *)object;
    fn_80482144(lbl_8056D7E0, object);
    if (fields[5] != 0) {
        fn_8048071C((u32 *)fields[5]);
    }
    if (fields[6] != 0) {
        if (fields[6] != 0) {
            fn_8045985C((u32 *)fields[6]);
        }
        fields[6] = 0;
        { u32 *child = (u32 *)fields[1];
        if (child != NULL && fn_804622F0(object) != 0) {
            fn_8048722C(child);
        }
     }}
    fn_8048D834(object);
    ((void (*)(u32 *, u8 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
        *(u32 **)(lbl_805F15B4 + clumpModule.globalsOffset), object);
    return 1;
}

u32 fn_80457090(u32 *object) {
    u32 *node = fn_804573A8(object);
    if (node == NULL) {
        return 0;
    }
    node[1] = (u32)fn_80486C5C(object[1]);
    if (node[1] == 0) {
        fn_80457468(node);
        return 0;
    }

    { volatile u32 status = 1;
    u32 *sentinel = object + 2;
    u32 *link = (u32 *)object[2];
    while (link != sentinel) {
        u32 next = *link;
        u32 *current = link - 0x10;
        u32 *created = (u32 *)fn_80456DD8();
        if (created == NULL) {
            status = 0;
            break;
        }

        ((u8 *)created)[0] = ((u8 *)current)[0];
        ((u8 *)created)[1] = ((u8 *)current)[1];
        ((u8 *)created)[2] = ((u8 *)current)[2];
        ((u8 *)created)[3] = ((u8 *)current)[3];
        created[1] = 0;
        ((u8 *)created)[2] = ((u8 *)current)[2];
        fn_80458CBC(created, 0);
        { u32 old = current[6];
        if (old != created[6]) {
            if (old != 0) fn_8045984C(old);
            if (created[6] != 0) fn_8045985C(created[6]);
            created[6] = old;
            if (old != 0) {
                u32 *extra = *(u32 **)(old + 0x5C);
                created[7] = extra[1];
                created[8] = extra[2];
                created[9] = extra[3];
                created[0xA] = extra[4];
            }
            if (created[1] != 0 && fn_804622F0(created) != 0) {
                fn_8048722C(created[1]);
            }
        }
        created[7] = current[7];
        created[8] = current[8];
        created[9] = current[9];
        created[0xA] = current[0xA];
        created[0x12] = current[0x12];
        created[0x1B] = current[0x1B];
        fn_804821B0(lbl_8056D7E0, created, current);

        if (created == NULL) {
            status = 0;
            break;
        }
        if (current[1] != 0) {
            fn_80458CBC(created, *(u32 *)(current[1] + 0xA0));
            fn_80457608(node, created);
        } else {
            fn_80482144(lbl_8056D7E0, created);
            if (created[5] != 0) {
                fn_8048071C(created[5]);
            }
            if (created[6] != 0) {
                fn_8045985C(created[6]);
                created[6] = 0;
                if (created[1] != 0 && fn_804622F0(created) != 0) {
                    fn_8048722C(created[1]);
                }
            }
            fn_8048D834(created);
            ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + clumpModule.globalsOffset + 0x11C))(created);
            { u32 statusArgs[2];
            statusArgs[0] = 2;
            statusArgs[1] = fn_8047DC28(5);
            fn_8047DB84(statusArgs);
            status = 0;
         }}
        if (status == 0) {
            break;
        }
        link = (u32 *)next;
     }}

    fn_80486CA4(object[1]);
    if (status == 0) {
        fn_80457468(node);
        return 0;
    }
    node[0xA] = object[0xA];
    fn_804821B0(lbl_8056D7F8, node, object);
    return (u32)node;
 }}

u32 *fn_804573A8(u32 *object) {
    u8 *manager = *(u8 **)(lbl_805F15B4 + clumpModule.globalsOffset + 4);
    u32 *created = (u32 *)((void *(*)(u8 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(manager, 0x30010);
    if (created == NULL) {
        return created;
    }
    ((u8 *)created)[0] = 2;
    ((u8 *)created)[1] = 0;
    ((u8 *)created)[2] = 0;
    ((u8 *)created)[3] = 0;
    created[1] = 0;
    created[1] = 0;
    created[3] = created[2] = (u32)(created + 2);
    created[5] = created[4] = (u32)(created + 4);
    created[7] = created[6] = (u32)(created + 6);
    created[9] = 0;
    created[8] = 0;
    created[0xA] = (u32)fn_80456230;
    fn_8048209C(lbl_8056D7F8, created);
    return created;
}

u32 fn_80457468(u32 *node) {
    fn_80482144(lbl_8056D7F8, node);
    { u32 *sentinel = node + 2;
    u32 *current;
    u32 *link = (u32 *)node[2];
    while (link != sentinel) {
        current = link - 0x10;
        link = *(u32 **)link;
        fn_80482144(lbl_8056D7E0, current);
        if (current[5] != 0) {
            fn_8048071C(current[5]);
        }
        if (current[6] != 0) {
            if (current[6] != 0) {
                fn_8045985C(current[6]);
            }
            current[6] = 0;
            { u32 child = current[1];
            if (child != 0 && fn_804622F0(current) != 0) {
                fn_8048722C(child);
            }
         }}
        fn_8048D834(current);
        { u8 *manager = *(u8 **)(lbl_805F15B4 + clumpModule.globalsOffset);
        ((void (*)(u8 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(manager, current);
        if (current == NULL) break;
     }}

    current = (u32 *)node[4];
    sentinel = node + 4;
    while (current != sentinel) {
        link = (u32 *)((u8 *)(current - 1) - clumpLightExtOffset);
        { u32 *callback = *(u32 **)((u8 *)link + clumpLightExtOffset);
        current = *(u32 **)current;
        fn_80457630((u8 *)callback, (u8 *)link);
        fn_8045ACD4(link);
        if (link == NULL) break;
     }}

    current = (u32 *)node[6];
    sentinel = node + 6;
    while (current != sentinel) {
        link = (u32 *)((u8 *)(current - 1) - clumpCameraExtOffset);
        { u32 *callback = *(u32 **)((u8 *)link + clumpCameraExtOffset);
        current = *(u32 **)current;
        fn_80457664((u8 *)callback, (u8 *)link);
        fn_804859F8(link);
        if (link == NULL) break;
     }}

    if (node[1] != 0) {
        fn_80487208(node[1]);
    }
    { u8 *manager = *(u8 **)(lbl_805F15B4 + clumpModule.globalsOffset + 4);
    ((void (*)(u8 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(manager, node);
    return 1;
 } }}

void fn_80457608(u32 *object, u32 *other) {
    u32 next = object[2];
    u32 *node = object + 2;
    u32 newNext = (u32)other + 0x40;
    other[0x10] = next;
    other[0x11] = (u32)node;
    node = (u32 *)object[2];
    node[1] = newNext;
    object[2] = newNext;
    other[0x0F] = (u32)object;
}

u8 *fn_80457630(u8 *callback, u8 *other) {
    u32 *target = (u32 *)(other + clumpLightExtOffset);
    *(u32 *)target[2] = target[1];
    *(u32 *)(target[1] + 4) = target[2];
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
    return callback;
}

u8 *fn_80457664(u8 *callback, u8 *other) {
    u32 *target = (u32 *)(other + clumpCameraExtOffset);
    *(u32 *)target[2] = target[1];
    *(u32 *)(target[1] + 4) = target[2];
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
    return callback;
}


static u32 *RwCpp19Fail(u32 *node, u32 *ownerA, u32 *ownerB, u32 count) {
    if (ownerB[0] != 0) {
        u32 *table = (u32 *)ownerB[0];
        { u32 i; for (i = 0; i < count; i++) {
            fn_8045985C(table[i]);
        } }
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
    }
    fn_80483728(&ownerA);
    fn_80457468(node);
    { u32 args[2];
    args[0] = 2;
    args[1] = fn_8047DC28(0x8000001A);
    fn_8047DB84(args);
    return NULL;
 }}

u32 *fn_80457698(void *stream) {
    u32 header[3];
    if (fn_8047CF50(stream, 1, NULL, NULL) == 0) {
        u32 args[2];
        args[0] = 2;
        args[1] = fn_8047DC28(0x8000001A);
        fn_8047DB84(args);
        return NULL;
    }
    if (fn_80480D4C(stream, header, 12) != 12) {
        u32 args[2];
        args[0] = 2;
        args[1] = fn_8047DC28(0x8000001A);
        fn_8047DB84(args);
        return NULL;
    }
    fn_8047D254(header, 12);
    { u32 *node = fn_804573A8((u32 *)stream);
    if (node == NULL) {
        return NULL;
    }
    { u32 *ownerA = NULL;
    u32 ownerB[2];

    ownerB[0] = 0;
    ownerB[1] = 0;
    if (fn_80483774(&ownerA) == 0) {
        return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
    }
    node[1] = ownerA[0];
    if (fn_8047CF50(stream, 26, NULL, NULL) == 0) {
        return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
    }
    if (fn_80455F18(stream, ownerB) == 0) {
        return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
    }
    { u32 i; for (i = 0; i < header[0]; i++) {
        if (fn_8047CF50(stream, 20, NULL, NULL) == 0) {
            return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
        }
        { u32 *created = (u32 *)fn_80456234(stream, ownerA, ownerB);
        if (created == NULL) {
            return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
        }
        fn_80457608(node, created);
     }} }
    { u32 i; for (i = 0; i < header[1]; i++) {
        u32 type;
        if (fn_8047CF50(stream, 1, NULL, NULL) == 0 ||
            fn_8047D868(stream, &type, 4) == 0) {
            return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
        }
        if (type == 0) {
            continue;
        }
        if (fn_8047CF50(stream, 18, NULL, NULL) == 0) {
            return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
        }
        { u32 *created = (u32 *)fn_8045AB1C(stream);
        if (created == NULL) {
            return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
        }
        fn_8048D7C0(created, (u32 *)ownerA[type]);
        { u32 *link = (u32 *)((u8 *)created + clumpLightExtOffset);
        node[4] = (u32)link;
        link[2] = (u32)(node + 4);
        node[4] = (u32)(link + 1);
        link[0] = (u32)node;
     } }} }
    { u32 i; for (i = 0; i < header[1]; i++) {
        u32 type;
        if (fn_8047CF50(stream, 1, NULL, NULL) == 0 ||
            fn_8047D868(stream, &type, 4) == 0) {
            return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
        }
        if (type == 0) {
            continue;
        }
        if (fn_8047CF50(stream, 5, NULL, NULL) == 0) {
            return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
        }
        { u32 *created = (u32 *)fn_80483590(stream);
        if (created == NULL) {
            return RwCpp19Fail(node, (u32 *)ownerA, (u32 *)ownerB, (u32)header[2]);
        }
        fn_8048D7C0(created, (u32 *)ownerA[type]);
        { u32 *link = (u32 *)((u8 *)created + clumpCameraExtOffset);
        node[6] = (u32)link;
        link[2] = (u32)(node + 6);
        node[6] = (u32)(link + 1);
        link[0] = (u32)node;
     } }} }
    if (ownerB[0] != 0) {
        u32 *table = (u32 *)ownerB[0];
        { u32 i; for (i = 0; i < header[2]; i++) {
            fn_8045985C(table[i]);
        } }
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
    }
    fn_80483728(&ownerA);
    if (fn_804816AC(lbl_8056D7F8, (u32 *)stream, node) == 0) {
        return RwCpp19Fail(node, ownerA, NULL, 0);
    }
    return node;
 } }}


void fn_80458B6C(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D7E0, first, second, third, fourth, fifth);
}

void fn_80458BAC(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D7F8, first, second, third, fourth, fifth);
}


void fn_80458BEC(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D7E0, first, second, third, fourth);
}

s32 fn_80458C28(u32 pluginID, void *callback) {
    return fn_80481634(lbl_8056D7E0, pluginID, callback);
}

s32 fn_80458C5C(u32 pluginID, void *callback) {
    return fn_80481670(lbl_8056D7E0, pluginID, callback);
}

s32 fn_80458C90(u32 pluginID) {
    return fn_80481D98(lbl_8056D7E0, pluginID);
}

u8 *fn_80458CBC(u8 *object, void *frame) {
    fn_8048D7C0(object, frame);
    object[3] |= 1;
    return object;
}

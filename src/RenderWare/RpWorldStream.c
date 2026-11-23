/* RenderWare: the binary world module (rwID_BINWORLDMODULE, 0x50B): RpWorldStreamRead
 * (fn_804556F4), the readers of its plane sectors (fn_804552AC) and world sectors (fn_80454CE4),
 * the stream callbacks of the two extension words a world and a world sector carry (registered by
 * RpWorldPluginAttach), and the module's open and close callbacks.
 *
 * 0x80454AA4-0x80455D0C (.sbss 0x805F14A0-0x805F14B8, .sdata2 0x805FB108-0x805FB110). RpUVAnim.c
 * comes before it and RpClump.c (the clump module, 0x504) after it: fn_80455CE4 and fn_80455CF8 are
 * the 0x50B module's close and open callbacks (RpWorldPluginAttach registers them as such), so the
 * module record at 0x805F14B0 closes this file's .sbss. The plugin registries the readers walk
 * (lbl_8056D880, lbl_8056D898) belong to the world-sector and world files further on.
 *
 * Built as C with cflags_rw_c (configure.py). The three readers (fn_80454CE4, fn_804552AC and
 * RpWorldStreamRead) are wave-40 drafts; everything else matches.
 */
#include <dolphin/types.h>

typedef struct BinWorldModuleInfo {
    s32 globalsOffset;
    s32 numInstances;
} BinWorldModuleInfo;

extern u8 *lbl_805F15B4;
extern u32 lbl_8056D880[];
extern u32 lbl_8056D898[];

static BinWorldModuleInfo binWorldModule;
static u32 worldExtA;
static u32 worldExtB;
static u32 sectorExtA;
static u32 sectorExtB;

void *memset(void *, int, u32);
s32 fn_8047CF50(void *, u32, u32 *, u32 *);
u32 fn_8047D868(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D684(void *, void *, u32);
u32 fn_80480D4C(void *, void *, u32);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(u32 *);
void fn_8047D254(void *, u32);
void fn_8048209C(void *, void *);
u32 fn_804816AC(void *, void *, void *);
void fn_80481870(void *, u32, void *, u32);
void fn_8045F6E0(u32 *);
void fn_8045F7D0(u32 *);
void fn_8045FCFC(u32 *, u32, u32);
u32 fn_8045FFA0(u32 *);
u32 *fn_8045B924(void *, u32 *);
void fn_8045FBA4(u32 *);
void fn_80460120(u32 *, u32);
u32 *fn_8045B270(u32 *, u32 *);
u32 fn_8045FDB0(u32 *);
u32 *fn_80454CE4(void *stream, u8 **cursor, u32 *object, u32 flags);
u32 *fn_804552AC(void *stream, u8 **cursor, u32 *object, u32 flags);




void *fn_80454AA4(void *stream, s32 type) {
    if (fn_8047D868(stream, &worldExtA, 4) == 0) {
        return NULL;
    }
    if (type == 8 && fn_8047D868(stream, &worldExtB, 4) == 0) {
        return NULL;
    }
    return stream;
}

void *fn_80454B20(void *stream, s32 length, const u32 *object) {
    u32 *nested = (u32 *)object[27];
    if (fn_8047D458(stream, (u8 *)nested + 44, 4) == 0) {
        return NULL;
    }
    nested = (u32 *)object[27];
    if (fn_8047D458(stream, (u8 *)nested + 48, 4) != 0) {
        return stream;
    }
    return NULL;
}

u32 fn_80454B9C(u32 *object) {
    u32 *nested = (u32 *)object[0x1B];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}

void *fn_80454BC4(void *stream, s32 type) {
    if (fn_8047D868(stream, &sectorExtA, 4) == 0) {
        return NULL;
    }
    if (type == 8 && fn_8047D868(stream, &sectorExtB, 4) == 0) {
        return NULL;
    }
    return stream;
}

void *fn_80454C40(void *stream, s32 length, const u32 *object) {
    u32 *nested = (u32 *)object[31];
    if (fn_8047D458(stream, (u8 *)nested + 44, 4) == 0) {
        return NULL;
    }
    nested = (u32 *)object[31];
    if (fn_8047D458(stream, (u8 *)nested + 48, 4) != 0) {
        return stream;
    }
    return NULL;
}

u32 fn_80454CBC(u32 *object) {
    u32 *nested = (u32 *)object[0x1F];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}



#define RWCPP17_ERROR13(value)                                                               \
    do {                                                                                     \
        u32 error[2];                                                                        \
        error[0] = 2;                                                                        \
        error[1] = fn_8047DC28(0x80000013, value);                                           \
        fn_8047DB84(error);                                                                  \
    } while (0)

u32 *fn_80454CE4(void *stream, u8 **cursor, u32 *object, u32 flags) {
    u32 version;
    if (fn_8047CF50(stream, 1, NULL, &version) == 0) {
        return NULL;
    }
    if (version < 0x00035000 || version > 0x00037002) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        return NULL;
    }
    { u32 header[11];
    if (fn_80480D4C(stream, header, 44) != 44) {
        return NULL;
    }
    fn_8047D254(header, 44);
    { u32 *base = (u32 *)*cursor;
    *cursor += lbl_8056D880[0];
    if (base == NULL) {
        RWCPP17_ERROR13(lbl_8056D880[0]);
        return NULL;
    }
    base[0] = 0xFFFFFFFF;
    base[27] = header[3];
    base[28] = header[4];
    base[29] = header[5];
    base[24] = header[6];
    base[25] = header[7];
    base[26] = header[8];
    ((u16 *)base)[64] = (u16)header[0];
    ((u16 *)base)[66] = (u16)header[1];
    ((u16 *)base)[65] = (u16)header[2];
    base[1] = 0;
    base[2] = 0;
    base[12] = 0;
    base[3] = 0;
    memset(base + 4, 0, 32);
    base[31] = 0;
    base[13] = 0;
    base[30] = 0;
    base[14] = (u32)(base + 14);
    base[15] = (u32)(base + 14);
    base[16] = (u32)(base + 16);
    base[17] = (u32)(base + 16);
    { s32 status = 1;
    if ((object[2] & 0x01000000) != 0) {
        goto finalize;
    }
    if (header[2] != 0) {
        u32 bytes = header[2] * 12;
        u32 *entries = (u32 *)*cursor;
        *cursor += bytes;
        base[2] = (u32)entries;
        if (entries == NULL) {
            RWCPP17_ERROR13(bytes);
            status = 0;
        }
        if (status != 0 && fn_8047D784(stream, entries, bytes) == 0) {
            status = 0;
        }
    }
    if (status != 0 && (flags & 0x10) != 0) {
        u32 bytes = header[2] * 4;
        u32 *entries = (u32 *)*cursor;
        *cursor += bytes;
        base[3] = (u32)entries;
        if (entries == NULL) {
            RWCPP17_ERROR13(bytes);
            status = 0;
        }
        if (status != 0 && fn_80480D4C(stream, entries, bytes) != bytes) {
            status = 0;
        }
    }
    if (status != 0 && (flags & 0x08) != 0) {
        u32 bytes = header[2] * 4;
        u32 *entries = (u32 *)*cursor;
        *cursor += bytes;
        base[12] = (u32)entries;
        if (entries == NULL) {
            RWCPP17_ERROR13(bytes);
            status = 0;
        }
        if (status != 0 && fn_80480D4C(stream, entries, bytes) == 0) {
            status = 0;
        }
    }
    if (status != 0 && (s32)object[8] > 0) {
        u32 stride = ((u16 *)base)[65] * 8;
        u32 *current = base;
        { s32 i; for (i = 0; i < (s32)object[8]; i++) {
            u32 *entries = (u32 *)*cursor;
            *cursor += stride;
            current[4] = (u32)entries;
            if (status != 0 && entries == NULL) {
                RWCPP17_ERROR13(stride);
                status = 0;
                break;
            }
            if (status != 0 && fn_8047D784(stream, entries, stride) == 0) {
                status = 0;
                break;
            }
            current++;
        } }
    }
    if (status != 0 && header[1] != 0) {
        u32 bytes = header[1] * 8;
        u32 *entries = (u32 *)*cursor;
        *cursor += bytes;
        base[1] = (u32)entries;
        if (entries == NULL) {
            RWCPP17_ERROR13(bytes);
            status = 0;
        }
        if (status != 0 && fn_8047D684(stream, entries, bytes) == 0) {
            status = 0;
        }
    }
    if (status != 0 && version < 0x00036002) {
        u32 *entries = (u32 *)base[1];
        { u32 i; for (i = 0; i < header[1]; i++) {
            u32 *entry = entries + i * 2;
            u32 first = entry[0];
            u32 second = entry[1];
            u16 *dest = (u16 *)entry;
            dest[0] = (u16)(first >> 16);
            dest[1] = (u16)(second >> 16);
            dest[2] = (u16)first;
            dest[3] = (u16)second;
        } }
    }
finalize:
    if (status != 0) {
        fn_8048209C(lbl_8056D880, base);
        sectorExtA = 0;
        sectorExtB = 0;
        if (fn_804816AC(lbl_8056D880, stream, base) == 0) {
            status = 0;
        }
        if (status != 0 && sectorExtA != 0) {
            fn_80481870(lbl_8056D880, sectorExtA, base, sectorExtB);
        }
    }
    if (status != 0) {
        return base;
    }
    if ((((u8 *)object)[3] & 1) != 0) {
        fn_8045F6E0(base);
    } else {
        fn_8045F7D0(base);
    }
    return NULL;
 } } }}


u32 *fn_804552AC(void *stream, u8 **cursor, u32 *object, u32 flags) {
    u32 length;
    u32 version;
    s32 status = 1;
    if (fn_8047CF50(stream, 1, &length, &version) == 0) {
        return NULL;
    }
    if (version < 0x00035000 || version > 0x00037002) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        return NULL;
    }
    { u32 header[6];
    memset(header, 0, 24);
    if (fn_80480D4C(stream, header, length) != length) {
        return NULL;
    }
    fn_8047D254(header, 24);
    { u32 *base = (u32 *)*cursor;
    *cursor += 24;
    if (base == NULL) {
        RWCPP17_ERROR13(24);
        return NULL;
    }
    base[0] = header[0];
    ((f32 *)base)[1] = ((f32 *)header)[1];
    if ((flags & 0x80000000) != 0) {
        ((f32 *)base)[4] = ((f32 *)header)[4];
        ((f32 *)base)[5] = ((f32 *)header)[5];
    } else {
        ((f32 *)base)[4] = ((f32 *)header)[1];
        ((f32 *)base)[5] = ((f32 *)header)[1];
    }
    if (header[2] != 0) {
        if (fn_8047CF50(stream, 9, NULL, &version) == 0) {
            status = 0;
        }
        if (status != 0 && (version < 0x00035000 || version > 0x00037002)) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            status = 0;
        }
        if (status != 0) {
            u32 result = (u32)fn_80454CE4(stream, cursor, object, flags);
            base[2] = result;
            if (result == 0) {
                status = 0;
            }
        }
    } else {
        if (fn_8047CF50(stream, 10, NULL, &version) == 0) {
            status = 0;
        }
        if (status != 0 && (version < 0x00035000 || version > 0x00037002)) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            status = 0;
        }
        if (status != 0) {
            u32 result = (u32)fn_804552AC(stream, cursor, object, flags);
            base[2] = result;
            if (result == 0) {
                status = 0;
            }
        }
    }
    if (status != 0 && header[3] != 0) {
        if (fn_8047CF50(stream, 9, NULL, &version) == 0) {
            status = 0;
        }
        if (status != 0 && (version < 0x00035000 || version > 0x00037002)) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            status = 0;
        }
        if (status != 0) {
            u32 result = (u32)fn_80454CE4(stream, cursor, object, flags);
            base[3] = result;
            if (result == 0) {
                status = 0;
            }
        }
    } else if (status != 0) {
        if (fn_8047CF50(stream, 10, NULL, &version) == 0) {
            status = 0;
        }
        if (status != 0 && (version < 0x00035000 || version > 0x00037002)) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            status = 0;
        }
        if (status != 0) {
            u32 result = (u32)fn_804552AC(stream, cursor, object, flags);
            base[3] = result;
            if (result == 0) {
                status = 0;
            }
        }
    }
    if (status != 0) {
        return base;
    }
    if (base[2] != 0) {
        if ((((u8 *)object)[3] & 1) != 0) {
            fn_8045F6E0((u32 *)base[2]);
        } else {
            fn_8045F7D0((u32 *)base[2]);
        }
        base[2] = 0;
    }
    return NULL;
 } }}

u32 *fn_804556F4(void *stream) {
    u32 *type;
    u32 length;
    u32 version;
    u32 nestedVersion;
    u32 header[16];
    if (fn_8047CF50(stream, 1, &length, &version) == 0) {
        return NULL;
    }
    if (version < 0x00035000 || version > 0x00037002) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        return NULL;
    }
    memset(header, 0, 64);
    if (fn_80480D4C(stream, header, length) != length) {
        return NULL;
    }
    if ((s32)header[8] > 0) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        return NULL;
    }
    fn_8047D254(header, 64);
    { u32 *object;
    u32 size = lbl_8056D898[0];
    size += header[6] * 24;
    size += header[7] * lbl_8056D880[0];
    { u32 stride;
    u32 flags = header[9];
    u32 masked = flags & 0x00FF0000;
    if (masked != 0) {
        stride = ((s32)masked) >> 16;
    } else if ((flags & 0x80) != 0) {
        stride = 2;
    } else {
        stride = (flags >> 2) & 1;
    }
    if ((flags & 0x01000000) == 0) {
        size += header[5] * 12;
        if ((flags & 0x10) != 0) {
            size += header[5] * 4;
        }
        if ((flags & 0x08) != 0) {
            size += header[5] * 4;
        }
        if (stride != 0) {
            size += stride * header[5] * 8;
        }
        size += header[4] * 8;
    }
    object = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x0003000B);
    if (object == NULL) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000013, size);
        fn_8047DB84(error);
        return NULL;
    }
    memset(object, 0, size);
    type = (u32 *)((u8 *)object + lbl_8056D898[0]);
    ((u8 *)object)[0] = 7;
    ((u8 *)object)[1] = 0;
    ((u8 *)object)[2] = 0;
    ((u8 *)object)[3] = 0;
    object[1] = 0;
    ((u8 *)object)[3] = 1;
    fn_8045FCFC(object, size, 0);
    object[2] = flags;
    object[3] = 2;
    { f32 scale = -1.0f;
    ((f32 *)object)[17] = scale * ((f32 *)header)[1];
    ((f32 *)object)[18] = scale * ((f32 *)header)[2];
    ((f32 *)object)[19] = scale * ((f32 *)header)[3];
    object[8] = stride;
    object[7] = 0;
    object[27] = 0;
    object[11] = (u32)(object + 11);
    object[12] = (u32)(object + 11);
    object[13] = (u32)(object + 13);
    object[14] = (u32)(object + 13);
    object[15] = (u32)(object + 15);
    object[16] = (u32)(object + 15);
    if (fn_8047CF50(stream, 8, NULL, &nestedVersion) == 0) {
        fn_8045FFA0(object);
        return NULL;
    }
    if (nestedVersion < 0x00035000 || nestedVersion > 0x00037002) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        fn_8045FFA0(object);
        return NULL;
    }
    if (fn_8045B924(stream, object + 4) == NULL) {
        fn_8045FFA0(object);
        return NULL;
    }
    if ((s32)header[0] != 0) {
        if (fn_8047CF50(stream, 9, NULL, &nestedVersion) == 0) {
            fn_8045FFA0(object);
            return NULL;
        }
        if (nestedVersion < 0x00035000 || nestedVersion > 0x00037002) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            fn_8045FFA0(object);
            return NULL;
        }
        { u32 result = (u32)fn_80454CE4(stream, (u8 **)&type, object, flags);
        object[7] = result;
        if (result == 0) {
            fn_8045FFA0(object);
            return NULL;
        }
     }} else {
        if (fn_8047CF50(stream, 10, NULL, &nestedVersion) == 0) {
            fn_8045FFA0(object);
            return NULL;
        }
        if (nestedVersion < 0x00035000 || nestedVersion > 0x00037002) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            fn_8045FFA0(object);
            return NULL;
        }
        { u32 result = (u32)fn_804552AC(stream, (u8 **)&type, object, flags);
        object[7] = result;
        if (result == 0) {
            fn_8045FFA0(object);
            return NULL;
        }
     }}
    object[9] = 0;
    object[10] = (u32)(object + 11);
    object[20] = header[10];
    object[21] = header[11];
    object[22] = header[12];
    object[23] = header[13];
    object[24] = header[14];
    object[25] = header[15];
    fn_8045FBA4(object);
    fn_80460120(object, 0);
    { u32 offset = 0;
    { s32 i; for (i = 0; i < (s32)object[5]; i++) {
        u32 *entry = (u32 *)*(u32 *)(object[4] + offset);
        u32 *child = (u32 *)entry[0];
        if (child != NULL) {
            fn_8045B270(entry, child);
        }
        offset += 4;
    } }
    fn_8048209C(lbl_8056D898, object);
    worldExtA = 0;
    worldExtB = 0;
    if (fn_804816AC(lbl_8056D898, stream, object) == 0) {
        fn_8045FFA0(object);
        return NULL;
    }
    if (worldExtA != 0) {
        fn_80481870(lbl_8056D898, worldExtA, object, worldExtB);
    }
    if (fn_8045FDB0(object) == 0) {
        fn_8045FFA0(object);
        return NULL;
    }
    return object;
 } } } }}


void *fn_80455CE4(void *instance) {
    binWorldModule.numInstances--;
    return instance;
}

void *fn_80455CF8(void *instance) {
    binWorldModule.numInstances++;
    return instance;
}

/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * The three stream readers are complete C++ implementations. fn_804556F4 retains size/stride
 * register-allocation differences; fn_80455F18 differs in loop-initializer ordering;
 * fn_80456234 uses the equivalent byte-masked shift for its header read comparison. */
#include <dolphin/types.h>

extern u32 lbl_805F14B0;
extern u32 lbl_805F14B8;
extern u32 lbl_805F14BC;

extern "C" {
s32 fn_8047CF50(void *, u32, u32 *, u32 *);
u32 fn_80480D4C(void *, void *, u32);
void fn_8047D254(void *, u32);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(u32 *);
void *memset(void *, int, u32);
void fn_8045FCFC(u32 *, u32, u32);
u32 fn_8045FFA0(u32 *);
u32 *fn_8045B924(void *, u32 *);
u32 fn_80454CE4(void *, u32 **, u32 *, u32);
u32 fn_804552AC(void *, u32 **, u32 *, u32);
void fn_8045FBA4(u32 *);
void fn_80460120(u32 *, u32);
u32 *fn_8045B270(u32 *, u32 *);
void fn_8048209C(u32 *, u32 *);
u32 fn_804816AC(void *, void *, void *);
void fn_80481870(void *, u32, void *, u32);
u32 fn_8045FDB0(u32 *);
extern u32 lbl_8056D880[];
extern u32 lbl_8056D898[];
extern u32 lbl_805F14AC;
extern u32 lbl_805F14A8;
extern u8 *lbl_805F15B4;
extern f32 lbl_805FB108;

#pragma use_lmw_stmw off
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
    u32 *object;
    u32 size = lbl_8056D898[0];
    size += header[6] * 24;
    size += header[7] * lbl_8056D880[0];
    u32 stride;
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
    f32 scale = lbl_805FB108;
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
        u32 result = fn_80454CE4(stream, &type, object, flags);
        object[7] = result;
        if (result == 0) {
            fn_8045FFA0(object);
            return NULL;
        }
    } else {
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
        u32 result = fn_804552AC(stream, &type, object, flags);
        object[7] = result;
        if (result == 0) {
            fn_8045FFA0(object);
            return NULL;
        }
    }
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
    u32 offset = 0;
    for (s32 i = 0; i < (s32)object[5]; i++) {
        u32 *entry = (u32 *)*(u32 *)(object[4] + offset);
        u32 *child = (u32 *)entry[0];
        if (child != NULL) {
            fn_8045B270(entry, child);
        }
        offset += 4;
    }
    fn_8048209C(lbl_8056D898, object);
    lbl_805F14AC = 0;
    lbl_805F14A8 = 0;
    if (fn_804816AC(lbl_8056D898, stream, object) == 0) {
        fn_8045FFA0(object);
        return NULL;
    }
    if (lbl_805F14AC != 0) {
        fn_80481870(lbl_8056D898, lbl_805F14AC, object, lbl_805F14A8);
    }
    if (fn_8045FDB0(object) == 0) {
        fn_8045FFA0(object);
        return NULL;
    }
    return object;
}
#pragma use_lmw_stmw reset

void fn_8045620C(void) {
}

void fn_8045622C(void) {
}

void fn_80456230(void) {
}

u8 *fn_80456DD8(void);
void fn_80458CBC(u8 *, u32 *);
void fn_80456F0C(u8 *, u32 *, u32);
void fn_80456FD8(void *);
void *fn_804599C8(void *);
void fn_8045985C(u32 *);
extern u8 lbl_8056D7E0[];
extern u32 lbl_805F14C4;
extern u32 lbl_805F14C0;

#pragma use_lmw_stmw off
#pragma optimize_for_size off
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
    u32 read_ok = fn_80480D4C(stream, header, length) == length;
    if (read_ok == 0) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x8000001A);
        fn_8047DB84(error);
        return NULL;
    }
    fn_8047D254(header, 16);
    u8 *object = fn_80456DD8();
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
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x8000001A);
            fn_8047DB84(error);
            return NULL;
        }
        u32 *nested;
        if (version >= 0x00035000 && version <= 0x00037002) {
            nested = (u32 *)fn_804599C8(stream);
            if (nested == NULL) {
                fn_80456FD8(object);
                u32 error[2];
                error[0] = 2;
                error[1] = fn_8047DC28(0x8000001A);
                fn_8047DB84(error);
                return NULL;
            }
            goto nested_valid;
        }
    nested_invalid:
        {
            fn_80456FD8(object);
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            return NULL;
        }
    nested_valid:
        fn_80456F0C(object, nested, 0);
        fn_8045985C(nested);
    }
    lbl_805F14C4 = 0;
    lbl_805F14C0 = 0;
    bool write_ok = fn_804816AC(lbl_8056D7E0, stream, object) != 0;
    if (!write_ok) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x8000001A);
        fn_8047DB84(error);
        return NULL;
    }
    if (lbl_805F14C4 != 0) {
        fn_80481870(lbl_8056D7E0, lbl_805F14C4, object, lbl_805F14C0);
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
}
#pragma optimize_for_size reset
#pragma use_lmw_stmw reset

void fn_80455CE4(void) {
    (&lbl_805F14B0)[1]--;
}

void fn_80455CF8(void) {
    (&lbl_805F14B0)[1]++;
}

u8 *fn_804561F0(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F14B8);
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
    return base;
}

u8 *fn_80456210(u8 *base) {
    u32 *target = (u32 *)(base + lbl_805F14BC);
    target[2] = 0;
    target[1] = 0;
    target[0] = 0;
    return base;
}

void fn_80457468(void);

void fn_80455D0C(void) {
    fn_80457468();
}

void fn_80455D2C(void *object) {
    fn_80456FD8(object);
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

u32 fn_8047D868(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);

#pragma use_lmw_stmw off
void *fn_80455D4C(void *stream, s32 type) {
    if (fn_8047D868(stream, &lbl_805F14C4, 4) == 0) {
        return NULL;
    }
    if (type == 8 && fn_8047D868(stream, &lbl_805F14C0, 4) == 0) {
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

void fn_80456560(void *);
u32 fn_8049B674(void *, void *, u32);
extern u32 lbl_805F1690;

void *fn_80455E6C(void *object) {
    if (((u32 *)object)[0x13] & 2) {
        fn_80456560(object);
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
    for (s32 i = 0; i < (s32)owner[1]; i++) {
        fn_8045985C((u32 *)*(u32 *)(owner[0] + offset));
        offset += 4;
    }
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
    u32 offset = 0;
    for (s32 i = 0; i < (s32)count; i++) {
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
    }
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
}
#pragma use_lmw_stmw reset
}
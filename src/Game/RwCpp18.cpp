/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * All stream readers and object helpers are complete C++ implementations. fn_804556F4 retains
 * size/stride register-allocation differences; fn_80455F18 differs in loop-initializer ordering;
 * fn_80456234 uses the equivalent byte-masked shift for its header read comparison. The frame
 * update and attach helpers retain register/scheduling differences after structural matching. */
#include <dolphin/types.h>

extern u32 lbl_805F14B0;
extern u32 lbl_805F14B8;
extern u32 lbl_805F14BC;
extern u32 lbl_805F14C8;

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
void fn_8048209C(void *, void *);
u32 fn_804816AC(void *, void *, void *);
void fn_80481870(void *, u32, void *, u32);
u32 fn_8045FDB0(u32 *);
extern u32 lbl_8056D880[];
extern u32 lbl_8056D898[];
extern u32 lbl_805F14AC;
extern u32 lbl_805F14A8;
extern u8 *lbl_805F15B4;
extern f32 lbl_805FB108;
extern f32 lbl_805FB110;
extern f32 lbl_805FB114;

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
u8 *fn_80456F0C(u8 *, u32 *, u32);
u32 fn_80456FD8(u8 *);
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

void fn_804800DC(u32 *, void *, u32);
void fn_8047FCDC(u32 *);
s32 fn_804859B8(u32, u32, void *, void *, u32);
s32 fn_8045AAA0(u32, u32, void *, void *, u32);
void *RwFrameGetLTM(void *);
void fn_80455D0C(void);
void fn_80455D2C(void *);
u8 *fn_804561F0(u8 *);
u8 *fn_80456210(u8 *);

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

s32 fn_80486CF0(u32 *);
void fn_80482458(u32 *, u32 *, u32 *);
f32 fn_80482340(f32);
void *RwFrameGetLTM(void *);

#pragma optimize_for_size off
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
#pragma optimize_for_size reset

u32 *fn_80456994(u32 *object) {
    fn_804800DC(*(u32 **)(lbl_805F15B4 + lbl_805F14C8 + 4), fn_80455D0C, 0);
    fn_804800DC(*(u32 **)(lbl_805F15B4 + lbl_805F14C8), fn_80455D2C, 0);
    fn_8047FCDC(*(u32 **)(lbl_805F15B4 + lbl_805F14C8));
    fn_8047FCDC(*(u32 **)(lbl_805F15B4 + lbl_805F14C8 + 4));
    *(u32 **)(lbl_805F15B4 + lbl_805F14C8) = NULL;
    *(u32 **)(lbl_805F15B4 + lbl_805F14C8 + 4) = NULL;
    (&lbl_805F14C8)[1]--;
    return object;
}

u32 *fn_8047FC04(u32, u32, u32, u32, u8 *, u32);
extern u8 lbl_805E1580[];
extern u8 lbl_805E15A4[];
extern u8 lbl_8056D7F8[];
extern u32 lbl_805EDDA0;
extern u32 lbl_805EDDA4;
extern u32 lbl_805EDDA8;
extern u32 lbl_805EDDAC;

u32 *fn_80456A50(u32 *object, u32 config) {
    lbl_805F14C8 = config;
    u32 *first = fn_8047FC04(*(u32 *)lbl_8056D7E0, lbl_805EDDA0, 4, lbl_805EDDA4,
                             lbl_805E1580, 0x00040010);
    *(u32 **)(lbl_805F15B4 + lbl_805F14C8) = first;
    if (*(u32 *)(lbl_805F15B4 + lbl_805F14C8) == 0) {
        goto failure;
    }
    u32 *second = fn_8047FC04(*(u32 *)lbl_8056D7F8, lbl_805EDDA8, 4, lbl_805EDDAC,
                              lbl_805E15A4, 0x00040014);
    *(u32 **)(lbl_805F15B4 + lbl_805F14C8 + 4) = second;
    if (*(u32 *)(lbl_805F15B4 + lbl_805F14C8 + 4) != 0) {
        (&lbl_805F14C8)[1]++;
        return object;
    }
    fn_8047FCDC(*(u32 **)(lbl_805F15B4 + lbl_805F14C8));
    *(u32 **)(lbl_805F15B4 + lbl_805F14C8) = NULL;
failure:
    return NULL;
}

s32 fn_80456B48(void) {
    s32 result = fn_804859B8(12, 16, fn_804561F0, fn_8045620C, 0);
    lbl_805F14B8 = result;
    if (result < 0) {
        return 0;
    }
    result = fn_8045AAA0(12, 16, fn_80456210, fn_8045622C, 0);
    lbl_805F14BC = result;
    return ((u32)result >> 31) ^ 1;
}

#pragma optimize_for_size off
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
#pragma optimize_for_size reset

#pragma use_lmw_stmw off
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
        u32 *entry = (u32 *)((u8 *)current - 4 - lbl_805F14B8);
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
        u32 *entry = (u32 *)((u8 *)current - 4 - lbl_805F14BC);
        u32 *next = (u32 *)current[0];
        if (callback(entry, data) == 0) {
            return object;
        }
        current = next;
    }
    return object;
}

void *fn_80455E6C(void *);
void *fn_80455EB4(void *);

u8 *fn_80456DD8(void) {
    u8 *object = ((u8 *(*)(u32 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        *(u32 **)(lbl_805F15B4 + lbl_805F14C8), 0x00030014);
    if (object == NULL) {
        return NULL;
    }
    u32 *fields = (u32 *)object;
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
    f32 value = lbl_805FB110;
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
    f32 other = lbl_805FB114;
    ((f32 *)object)[21] = other;
    ((f32 *)object)[22] = other;
    ((f32 *)object)[23] = lbl_805FB110;
    fields[19] = 3;
    fields[17] = 0;
    fields[16] = 0;
    fields[15] = 0;
    fields[27] = 0;
    fields[25] = (u32)(fields + 25);
    fields[26] = (u32)(fields + 25);
    fn_8048209C(lbl_8056D7E0, object);
    return object;
}

void fn_8045984C(u32 *);
void fn_8048071C(u32 *);
u32 fn_804622F0(u8 *);
void fn_8048722C(u32 *);
void fn_8048D834(u8 *);
void fn_80482144(u8 *, u8 *);

#pragma optimize_for_size off
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
                u32 *source = (u32 *)nested[23];
                fields[7] = source[1];
                fields[8] = source[2];
                fields[9] = source[3];
                fields[10] = source[4];
            }
            u32 *child = (u32 *)fields[1];
            if (child != NULL && fn_804622F0(object) != 0) {
                fn_8048722C(child);
            }
        }
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
        u32 *child = (u32 *)fields[1];
        if (child != NULL && fn_804622F0(object) != 0) {
            fn_8048722C(child);
        }
    }
    fn_8048D834(object);
    ((void (*)(u32 *, u8 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
        *(u32 **)(lbl_805F15B4 + lbl_805F14C8), object);
    return 1;
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
    fn_80456FD8((u8 *)object);
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

void fn_80456560(u8 *);
u32 fn_8049B674(void *, void *, u32);
extern u32 lbl_805F1690;

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
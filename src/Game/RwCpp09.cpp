/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern u32 lbl_805F1440;
extern u32 lbl_805F1448;
extern u8 *lbl_805F15B4;

extern "C" {
void fn_80445CF4(float *, const float *);
void fn_8044621C(float *, float *, float *, float);
void fn_80445E08(float *, float *, float *, float);
void fn_804468D0(float *, const float *, const float *);
void fn_80446790(float *, const float *);
void *fn_80446620(void *, u32 *);
u32 fn_804466DC(void *, u32 *);
u32 fn_80446780(u32 *);
void fn_8048722C(u32 *);
void fn_8048752C(u32 *, void *, u32 *);
u32 fn_8044758C(u8 *);

void *fn_80446A04(u32 *object, u32 *other) {
    u32 *entry = (u32 *)other[4];
    for (u32 i = 0; i < other[1]; i++) {
        if (fn_8044758C((u8 *)object) == entry[0]) {
            entry[3] = (u32)object;
        }
        entry += 4;
    }
    fn_8048722C(object);
    fn_8048752C(object, (void *)fn_80446A04, other);
    return object;
}

extern u32 lbl_805EDD70;
extern u32 lbl_805EDD74;
extern u32 lbl_805E1510;
void *fn_8047FC04(u32, void *, void *, u32, void *, u32);
void fn_80477344(void *);

void *fn_80446AA8(u32 *object) {
    void *result = fn_8047FC04(0x24, &lbl_805EDD70, &lbl_805EDD74, 4, &lbl_805E1510, 0x40011E);
    ((u32 *)&lbl_805F1440)[1] = (u32)result;
    if (result == NULL) object = NULL;
    u32 callbacks[12];
    callbacks[0] = 1;
    callbacks[1] = 0x24;
    callbacks[2] = 0x24;
    callbacks[3] = (u32)fn_80445CF4;
    callbacks[4] = (u32)fn_8044621C;
    callbacks[5] = (u32)fn_80445E08;
    callbacks[6] = (u32)fn_804468D0;
    callbacks[7] = (u32)fn_80446790;
    callbacks[8] = (u32)fn_80446620;
    callbacks[9] = (u32)fn_804466DC;
    callbacks[10] = (u32)fn_80446780;
    callbacks[11] = 0;
    fn_80477344(callbacks);
    return object;
}

void fn_8047FCDC(void *);

void *fn_80446B90(u32 *object) {
    if (((u32 *)&lbl_805F1440)[1] != 0) {
        fn_8047FCDC((void *)((u32 *)&lbl_805F1440)[1]);
        ((u32 *)&lbl_805F1440)[1] = 0;
    }
    return object;
}

u32 fn_80447418(u32 *);
void *fn_804472AC(u32, u32 *, u32 *, u32, u32 *);
u32 fn_8047D458(void *, void *, u32);

void *fn_80446BFC(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440);
    if (target[1] != 0) {
        u32 *list = (u32 *)target[1];
        u32 *entry = (u32 *)list[4];
        for (u32 i = 0; i < list[1]; i++) {
            entry[3] = 0;
            entry += 4;
        }
        if (list[5] == (u32)object) fn_80447418(list);
        target[1] = 0;
    }
    target[0] = 0xFFFFFFFF;
    return object;
}

void *fn_80446C94(u32 *object, u32 *other) {
    u32 *otherTarget = (u32 *)((u8 *)other + lbl_805F1440);
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440);
    u32 *list = (u32 *)otherTarget[1];
    target[0] = otherTarget[0];
    if (list != NULL && (list[0] & 1) == 0) {
        u32 *source = (u32 *)list[4];
        fn_804472AC(list[1], (u32 *)NULL, (u32 *)NULL, list[0], (u32 *)list[8]);
        u32 *entry = (u32 *)list[4];
        for (u32 i = 0; i < list[1]; i++) {
            entry[3] = 0;
            entry[2] = source[2];
            entry[1] = source[1];
            entry[0] = source[0];
            source += 4;
            entry += 4;
        }
        target[1] = (u32)list;
        list[5] = (u32)object;
    }
    return object;
}

void *fn_80446D84(void *stream, u32 *object, u32 *plugin) {
    u32 value = 0x100;
    if (fn_8047D458(stream, &value, 4) == 0) return NULL;
    u32 *target = (u32 *)((u8 *)plugin + lbl_805F1440);
    if (fn_8047D458(stream, target, 4) == 0) return NULL;
    u32 *list = (u32 *)target[1];
    if (list != NULL && (list[0] & 1) == 0) {
        if (fn_8047D458(stream, list + 1, 4) == 0) return NULL;
        if (fn_8047D458(stream, list, 4) == 0) return NULL;
        if (fn_8047D458(stream, (u32 *)(list[8] + 0x20), 4) == 0) return NULL;
        u32 *entry = (u32 *)list[4];
        for (u32 i = 0; i < list[1]; i++) {
            if (fn_8047D458(stream, entry, 4) == 0) return NULL;
            if (fn_8047D458(stream, entry + 1, 4) == 0) return NULL;
            if (fn_8047D458(stream, entry + 2, 4) == 0) return NULL;
            entry += 4;
        }
    } else {
        value = 0;
        if (fn_8047D458(stream, &value, 4) == 0) return NULL;
    }
    return stream;
}

u32 fn_8047D868(void *, void *, u32);
u32 fn_804779DC(u32, u32);

void *fn_80446F40(void *stream, u32 *plugin) {
    u32 marker;
    if (fn_8047D868(stream, &marker, 4) == 0 || marker != 0x100) return NULL;
    u32 *target = (u32 *)((u8 *)plugin + lbl_805F1440);
    if (fn_8047D868(stream, target, 4) == 0) return NULL;
    u32 count;
    if (fn_8047D868(stream, &count, 4) == 0) return NULL;
    if ((s32)count <= 0) return stream;
    u32 first;
    u32 second;
    if (fn_8047D868(stream, &first, 4) == 0 || fn_8047D868(stream, &second, 4) == 0) return NULL;

    u32 *list = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(0x24, 0x3011E);
    if (list != NULL) {
        memset(list, 0, 0x24);
        list[8] = fn_804779DC(count, second);
        list[0] = first;
        list[5] = (u32)plugin;
        list[1] = count;
        list[6] = (u32)list;
        if ((list[0] & 2) == 0) {
            list[2] = 0;
            list[3] = 0;
        } else {
            u32 bytes = (count << 6) + 0xF;
            void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x3011E);
            list[2] = ((u32)memory + 0xF) & ~0xF;
            list[3] = (u32)memory;
        }
        list[4] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count << 4, 0x3011E);
        u32 *entry = (u32 *)list[4];
        for (u32 i = 0; i < count; i++) {
            if (fn_8047D868(stream, entry, 4) == 0) return NULL;
            if (fn_8047D868(stream, entry + 1, 4) == 0) return NULL;
            if (fn_8047D868(stream, entry + 2, 4) == 0) return NULL;
            entry[3] = 0;
            entry += 4;
        }
    }
    target[1] = (u32)list;
    return stream;
}

u32 fn_8044719C(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440);
    if (target[0] == 0xFFFFFFFF && target[1] == 0) return 0;
    u32 *list = (u32 *)target[1];
    if (list == NULL || (list[0] & 1) != 0) return 0xC;
    return list[1] * 0xC + 0x14;
}

typedef void (*RwCpp09Callback)(void);
void fn_80446BE0(u32 *);
s32 fn_80485D78(u32, u32, RwCpp09Callback, RwCpp09Callback);
s32 fn_804878D4(u32, u32, RwCpp09Callback, RwCpp09Callback, RwCpp09Callback);
s32 fn_804836EC(u32, RwCpp09Callback, RwCpp09Callback, RwCpp09Callback);

u32 fn_80447200(void) {
    s32 result = fn_80485D78(0, 0x11E, (RwCpp09Callback)fn_80446AA8,
                             (RwCpp09Callback)fn_80446B90);
    if (result < 0) return 0;
    result = fn_804878D4(8, 0x11E, (RwCpp09Callback)fn_80446BE0,
                         (RwCpp09Callback)fn_80446BFC, (RwCpp09Callback)fn_80446C94);
    lbl_805F1440 = result;
    if (fn_804836EC(0x11E, (RwCpp09Callback)fn_80446F40,
                    (RwCpp09Callback)fn_80446D84, (RwCpp09Callback)fn_8044719C) < 0) {
        return 0;
    }
    return lbl_805F1440 >= 0;
}

void fn_80477A8C(u32);

void *fn_804472AC(u32 count, u32 *first, u32 *second, u32 flags, u32 *owner) {
    u32 *list = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(0x24, 0x3011E);
    if (list == NULL) return NULL;
    list[8] = fn_804779DC(count, (u32)owner);
    list[0] = flags;
    list[1] = count;
    list[5] = 0;
    if ((flags & 2) == 0) {
        list[2] = 0;
        list[3] = 0;
    } else {
        u32 bytes = (count << 6) + 0xF;
        void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x3011E);
        list[2] = ((u32)memory + 0xF) & ~0xF;
        list[3] = (u32)memory;
    }
    list[4] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count << 4, 0x3011E);
    u32 *entry = (u32 *)list[4];
    for (u32 i = 0; i < count; i++) {
        entry[3] = 0;
        if (second != NULL) entry[2] = second[i];
        entry[1] = 0;
        if (first != NULL) entry[1] = first[i];
        entry += 4;
    }
    list[6] = (u32)list;
    return list;
}

u32 fn_80447418(u32 *list) {
    u32 *owner = (u32 *)list[5];
    if ((list[0] & 1) == 0) {
        if ((list[0] & 2) == 0) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)list[3]);
        }
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)list[4]);
    }
    list[3] = 0;
    list[2] = 0;
    list[4] = 0;
    fn_80477A8C(list[8]);
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x11C))(list);
    if (owner != NULL) owner[1] = 0;
    return 0;
}

void *fn_804474D8(u32 *object) {
    u32 *owner = (u32 *)object[5];
    u32 *entry = (u32 *)object[4];
    for (u32 i = 0; i < object[1]; i++) {
        if (fn_8044758C((u8 *)owner) == entry[0]) {
            entry[3] = (u32)owner;
        }
        entry += 4;
    }
    fn_8048722C(owner);
    fn_8048752C(owner, (void *)fn_80446A04, object);
    return object;
}

void fn_8045B270(u32, u32);

void *fn_804475A8(u32 *object) {
    if (*(u32 *)((u8 *)object + lbl_805F1448) != 0) {
        u32 *owner = (u32 *)object[6];
        fn_8045B270(*(u32 *)(owner[8] + 0), 0);
        if (object[3] != 0) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)object[3]);
        }
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(object);
        *(u32 *)((u8 *)object + lbl_805F1448) = 0;
    }
    return object;
}

extern u32 lbl_805F144C;
void fn_804485BC(void);
void fn_80448610(void);
void fn_80447598(u8 *);
void *fn_804475A8(u32 *);
s32 fn_80458B6C(u32, u32, RwCpp09Callback, RwCpp09Callback, void *);

u32 fn_80447640(void) {
    s32 result = fn_80485D78(4, 0x12F, (RwCpp09Callback)fn_804485BC,
                             (RwCpp09Callback)fn_80448610);
    lbl_805F144C = result;
    if (result < 0) return 0;
    result = fn_80458B6C(4, 0x12F, (RwCpp09Callback)fn_80447598,
                         (RwCpp09Callback)fn_804475A8, NULL);
    lbl_805F1448 = result;
    return ((u32)result >> 31) ^ 1;
}

u32 fn_80447EB4(u32 value) {
    if ((value & 0x8) != 0 && (value & 0x8000) != 0) value &= 0x8000;
    if ((value & 0x20) != 0 && (value & 0x20000) != 0) value &= 0x20000;
    if ((value & 0x40) != 0 && (value & 0x40000) != 0) value &= 0x40000;
    if ((value & 0x80) != 0 && (value & 0x80000) != 0) value &= 0x80000;
    if ((value & 0x100) != 0 && (value & 0x100000) != 0) value &= 0x100000;
    if ((value & 0x8) != 0 && (value & 1) != 0) value &= ~1;
    if ((value & 0x8) != 0 && (value & 0x4) != 0) value &= 0xF;
    if ((value & 0x8) != 0 && (value & 0x20) != 0) value &= 0x3F;
    if ((value & 0x8) == 0 && (value & 1) == 0) value |= 1;
    if ((value & 0xC) == 0) value |= 0x20000000;
    return value;
}

void *fn_80447FC0(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1448);
    if ((s32)target[1] <= 0) return object;
    if (target[7] != 0) {
        ((void (*)(void *, u32))target[7])(target + 0x12, target[0x10]);
        target[0x10] = 0;
    }
    u32 result = 1;
    if (target[8] != 0) {
        result = ((u32 (*)(u32 *, void *, u32))target[8])(object, target + 0x12, target[1]);
    }
    if (result == 1 && target[4] != 0) {
        ((void (*)(u32 *))target[4])(object);
    }
    return object;
}

extern u32 lbl_8056D6F8;
void *fn_804480C8(void *, u32, void *, void *);

extern u32 lbl_8051BCCC;
extern float lbl_805FB050;
extern float lbl_805FB054;
extern float lbl_805FB058;
extern float lbl_805FB05C;
extern float lbl_805FB060;
extern u32 lbl_805FB550;
extern u32 lbl_805FB554;
s32 fn_80456DD8(void);
void fn_80456FD8(void);
void fn_80455EB4(void);
u32 fn_804476B8(u32 *, u32, void *);
u32 fn_80447AF8(u32 *, u32, void *);
void fn_8045B270(u32, u32);

void *fn_80448074(u32 *object, u32 value, void *argument) {
    return fn_804480C8(object, fn_80447EB4(value), argument, &lbl_8056D6F8);
}

void *fn_804480C8(void *parent, u32 flags, void *argument, void *config) {
    if (fn_80456DD8() == 0) {
        fn_80456FD8();
        return NULL;
    }
    u32 *object = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(0x138, 0x3012F);
    if (object == NULL) {
        fn_80456FD8();
        return NULL;
    }

    u32 *owner = (u32 *)parent;
    object[4] = owner[0x12];
    owner[0x12] = (u32)fn_80447FC0;
    if (owner[0x12] == 0) owner[0x12] = (u32)fn_80455EB4;
    object[0] = (u32)parent;
    object[1] = 0;
    object[0x28] = flags;
    object[0x11] = (u32)argument;
    object[0x10] = 0xFFFFFFFF;
    object[0xF] = 0;
    object[3] = 0;
    *(u32 **)((u8 *)owner + lbl_805F1448) = object;
    memcpy(object + 5, config, 0x10);

    u32 result = 0;
    if (object[5] != 0) {
        object[3] = ((u32 (*)(void *, u32, u32, void *))object[5])(object + 0x12, object[0], flags, argument);
    } else if ((flags & 0x20000000) != 0) {
        result = fn_80447AF8(object, flags, argument);
    } else {
        result = fn_804476B8(object, flags, argument);
    }
    if (result != 1 && object[3] == 0) {
        fn_80456FD8();
        return NULL;
    }
    if (object[6] != 0 &&
        ((u32 (*)(void *, void *, u32, u32, void *))object[6])(owner, object + 0x12, object[0], flags,
                                                               argument) != 1) {
        fn_80456FD8();
        return NULL;
    }

    u32 *table = (u32 *)lbl_8051BCCC;
    u32 *ownerData = (u32 *)owner[6];
    fn_8045B270(*(u32 *)(ownerData[8] + 0), 0);
    object[0x2E] = (u32)config;
    object[0x2F] = (u32)argument;
    object[0x2C] = (u32)parent;
    object[0x2D] = result;
    *(float *)(object + 0x30) = lbl_805FB05C;
    *(float *)(object + 0x48) = lbl_805FB060;
    *(float *)(object + 0x43) = lbl_805FB060;
    *(float *)(object + 0x3E) = lbl_805FB060;
    *(float *)(object + 0x42) = lbl_805FB05C;
    *(float *)(object + 0x40) = lbl_805FB05C;
    *(float *)(object + 0x3F) = lbl_805FB05C;
    *(float *)(object + 0x47) = lbl_805FB05C;
    *(float *)(object + 0x46) = lbl_805FB05C;
    *(float *)(object + 0x44) = lbl_805FB05C;
    *(float *)(object + 0x4C) = lbl_805FB05C;
    *(float *)(object + 0x4B) = lbl_805FB05C;
    *(float *)(object + 0x4A) = lbl_805FB05C;
    object[0x41] = flags | 0x20003;
    object[0x31] = flags;
    object[0x32] = flags;
    object[0x33] = flags;
    object[0x34] = flags;
    object[0x35] = flags;
    object[0x36] = table[0];
    object[0x37] = table[1];
    object[0x38] = table[2];
    object[0x39] = table[3];
    object[0x3A] = table[4];
    object[0x3B] = table[5];
    object[0x3C] = table[6];
    object[0x3D] = table[7];
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(0xA, object + 0x29);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(0xB, object + 0x2A);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(0xC, object + 0x2B);
    return owner;
}

u32 fn_80448408(u32 *object, u32 *out, u32 flags, u32 arg4) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1448);
    if ((target[0x28] & flags) != 0) {
        out[0] = 0;
        out[1] = 0;
        return 0;
    }
    switch (flags) {
    case 1:
        out[0] = target[0x13];
        out[1] = target[0x14];
        break;
    case 2:
        out[0] = target[0x1B];
        out[1] = target[0x1C];
        break;
    case 4:
        out[0] = target[0x19];
        out[1] = target[0x1A];
        break;
    case 8:
        out[0] = target[0x15];
        out[1] = target[0x16];
        break;
    case 0x10:
        out[0] = target[0x17];
        out[1] = target[0x18];
        break;
    case 0x20:
        out[0] = target[0x1F];
        out[1] = target[0x20];
        break;
    case 0x40:
        out[0] = target[0x1D];
        out[1] = target[0x1E];
        break;
    case 0x80:
        out[0] = target[0x21];
        out[1] = target[0x22];
        break;
    case 0x100:
        out[0] = target[0x23];
        out[1] = target[0x24];
        break;
    default:
        break;
    }
    if (target[0x27] != 0) out[1] = target[0x27];
    if (out[0] != 0 && (arg4 & 0x40000000) != 0) {
        target[0xF] |= arg4 | flags;
    }
    return 1;
}

u32 fn_8044758C(u8 *object) {
    return *(u32 *)(object + lbl_805F1440);
}

u32 fn_8044757C(u8 *object) {
    return *(u32 *)(object + lbl_805F1440 + 4);
}

void fn_80447598(u8 *base) {
    *(u32 *)(base + lbl_805F1448) = 0;
}

void fn_80446BE0(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440);
    target[1] = 0;
    target[0] = 0xFFFFFFFF;
}

void fn_80456FD8(void);

void fn_804483E8(void) {
    fn_80456FD8();
}

void fn_80448598(u8 *object) {
    u32 *nested = *(u32 **)(object + lbl_805F1448);
    nested[0x10] |= nested[0x0F];
    nested[0x0F] = 0;
}
}

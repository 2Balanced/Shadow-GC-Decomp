/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * fn_8045B8EC returns the found index or -1 with retail's size; GC/2.7 folds its pointer decrement
 * into lwzu where retail uses a separate addi. fn_8045A7C4 and fn_8045AA44 also match size;
 * their remaining differences are copy-register and first-comparison scheduling choices.
 * fn_8045AFA4 matches size and returns; its first data-pointer setup is scheduled after the saves.
 * fn_8045AD34 matches size; its constant loads and assignments use different volatile registers.
 * fn_8045B13C also matches size; its byte temporary and source-table registers differ.
 * fn_8045BFBC matches size and the emitted byte layout; its temporary registers and final two
 * store forms differ. fn_8045A818 implements all three easing branches; GC/2.7 adds seven
 * conversion/spill instructions and reschedules the polynomial temporaries.
 * fn_8045B348 matches size and all cleanup paths; its initializer temporary registers differ. */
#include <dolphin/types.h>

extern "C" {
void fn_8045A7C4(u8 *object, f32 *values) {
    u32 first = ((u32 *)values)[0];
    u32 second = ((u32 *)values)[1];
    *(u32 *)(object + 24) = first;
    u32 third = ((u32 *)values)[2];
    *(u32 *)(object + 28) = second;
    u32 fourth = ((u32 *)values)[3];
    *(u32 *)(object + 32) = third;
    *(u32 *)(object + 36) = fourth;
    if (*(f32 *)(object + 24) == *(f32 *)(object + 28) &&
        *(f32 *)(object + 24) == *(f32 *)(object + 32)) {
        object[3] = 1;
    } else {
        object[3] = 0;
    }
}

extern f32 lbl_805FB120;
extern f32 lbl_805FB128;
double cos(double);

u8 *fn_8045AA44(u8 *object, f32 value) {
    if (value < lbl_805FB120 || value > lbl_805FB128) {
        return NULL;
    }
    *(f32 *)(object + 40) = -(f32)cos(value);
    return object;
}

u32 fn_8045B79C(u32 *object, u32 index) {
    return ((u32 *)object[0])[index];
}

void fn_8045B788(u32 *object) {
    object[2] = 0;
    object[0] = 0;
    object[1] = 0;
}

u32 fn_8045B020(u32 *object) {
    u32 *nested = (u32 *)object[2];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}

extern u8 *lbl_805F15B4;

void fn_8045BF30(u32 first) {
    ((void (*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(first, 0x30502);
}

s32 fn_8045B8EC(u32 *object, u32 value) {
    s32 count = object[1];
    u32 *entries = (u32 *)object[0] + count;
    while (count-- > 0) {
        entries--;
        if (entries[0] == value) {
            return count;
        }
    }
    return -1;
}

extern u8 lbl_8056D828[];
extern u32 lbl_805F14D8;
void fn_80482144(u8 *, u32 *);
void fn_8048D834(u32 *);

u32 fn_8045ACD4(u32 *object) {
    fn_80482144(lbl_8056D828, object);
    fn_8048D834(object);
    ((void (*)(u8 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
        *(u8 **)(lbl_805F15B4 + lbl_805F14D8), object);
    return 1;
}

extern u32 lbl_805F14E8;
extern u8 lbl_8056D840[];
u32 *fn_8047FC04(u32, u32, u32, u32, u32 *, u32);
extern u32 lbl_805EDDB0;
extern u32 lbl_805EDDB4;
extern u32 lbl_805E15C8[];

u32 *fn_8045AE98(u32 *object, u32 value) {
    lbl_805F14D8 = value;
    u32 *table = fn_8047FC04(*(u32 *)lbl_8056D828, lbl_805EDDB0, 4, lbl_805EDDB4,
                             lbl_805E15C8, 0x00040012);
    *(u32 **)(lbl_805F15B4 + lbl_805F14D8) = table;
    if (*(u32 **)(lbl_805F15B4 + lbl_805F14D8) != NULL) {
        (&lbl_805F14D8)[1]++;
        return object;
    }
    return NULL;
}

void fn_8047FCDC(u32 *table);
void fn_8045A780(void);
void fn_8045A760(void);
void fn_8048209C(u8 *, u32 *);
void fn_804800DC(u32 *, u32, u32);
extern f32 lbl_805FB14C;

u32 *fn_8045AD34(u32 type) {
    u32 *object = ((u32 *(*)(u32 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        *(u32 **)(lbl_805F15B4 + lbl_805F14D8), 0x00030012);
    if (object == NULL) {
        return NULL;
    }
    ((u8 *)object)[0] = 3;
    ((u8 *)object)[1] = type;
    ((u8 *)object)[2] = 0;
    ((u8 *)object)[3] = 0;
    object[1] = 0;
    object[4] = (u32)fn_8045A780;
    f32 first = lbl_805FB120;
    *(f32 *)((u8 *)object + 20) = first;
    *(f32 *)((u8 *)object + 40) = first;
    f32 second = lbl_805FB14C;
    *(f32 *)((u8 *)object + 24) = second;
    *(f32 *)((u8 *)object + 28) = second;
    *(f32 *)((u8 *)object + 32) = second;
    *(f32 *)((u8 *)object + 36) = second;
    ((u8 *)object)[3] = 1;
    object[11] = (u32)(object + 11);
    object[12] = (u32)(object + 11);
    object[14] = 0;
    object[13] = 0;
    *(u16 *)((u8 *)object + 60) = *(u16 *)(lbl_805F15B4 + 10) - 1;
    ((u8 *)object)[2] = 3;
    fn_8048209C(lbl_8056D828, object);
    return object;
}

u32 *fn_8045AE20(u32 *object) {
    u32 *table = *(u32 **)(lbl_805F15B4 + lbl_805F14D8);
    fn_804800DC(table, (u32)fn_8045A760, 0);
    fn_8047FCDC(*(u32 **)(lbl_805F15B4 + lbl_805F14D8));
    *(u32 **)(lbl_805F15B4 + lbl_805F14D8) = NULL;
    (&lbl_805F14D8)[1]--;
    return object;
}

u32 fn_8047DC28(u32 code, ...);
void fn_8047DB84(u32 *error);
u32 fn_8047CF50(u8 *, u32, u32 *, u32 *);
u32 fn_80480D4C(u8 *, u8 *, u32);
void fn_8047D254(u8 *, u32);
u32 fn_804816AC(u8 *, u8 *, u32 *);
f32 fn_80482340(f32);
void *memset(void *, s32, u32);

u32 *fn_8045AB1C(u8 *stream) {
    u32 second;
    u32 first;
    u8 buffer[24];
    if ((s32)fn_8047CF50(stream, 1, &second, &first) == 0) {
        return NULL;
    }
    if (first < 0x00035000 || first > 0x00037002) {
        goto invalid;
    }
    memset(buffer, 0, 24);
    if (fn_80480D4C(stream, buffer, second) != second) {
        return NULL;
    }
    fn_8047D254(buffer, 24);
    u32 *object = fn_8045AD34((*(u32 *)(buffer + 20) >> 16) & 0xFF);
    if (object == NULL) {
        return NULL;
    }
    *(f32 *)((u8 *)object + 20) = *(f32 *)(buffer);
    *(f32 *)((u8 *)object + 24) = *(f32 *)(buffer + 4);
    *(f32 *)((u8 *)object + 28) = *(f32 *)(buffer + 8);
    *(f32 *)((u8 *)object + 32) = *(f32 *)(buffer + 12);
    if (first < 0x00030300) {
        f32 value = *(f32 *)(buffer + 16);
        value = value * value;
        f32 denominator = lbl_805FB14C + value;
        *(f32 *)((u8 *)object + 40) = -fn_80482340(lbl_805FB14C / denominator);
    } else {
        *(f32 *)((u8 *)object + 40) = *(f32 *)(buffer + 16);
    }
    if (*(f32 *)((u8 *)object + 24) == *(f32 *)((u8 *)object + 28) &&
        *(f32 *)((u8 *)object + 24) == *(f32 *)((u8 *)object + 32)) {
        ((u8 *)object)[3] = 1;
    } else {
        ((u8 *)object)[3] = 0;
    }
    ((u8 *)object)[2] = *(u32 *)(buffer + 20);
    if (fn_804816AC(lbl_8056D828, stream, object) == 0) {
        return NULL;
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

extern f32 lbl_805FB124;
extern f32 lbl_805FB128;
extern f32 lbl_805FB12C;
extern f32 lbl_805FB130;
extern f32 lbl_805FB134;
extern f32 lbl_805FB138;
extern f32 lbl_805FB13C;
extern f32 lbl_805FB140;
extern f32 lbl_805FB144;
extern f32 lbl_805FB148;
extern f32 lbl_805FB150;
extern f32 lbl_805FB154;
extern f32 lbl_805FB158;
extern f32 lbl_805FB15C;
extern f32 lbl_805FB160;
extern f32 lbl_805FB164;
extern f32 lbl_805FB168;

f32 fn_8045A818(f32 *object) {
    f32 input = object[10];
    f32 value = -input;
    u32 bits = *(u32 *)&value;
    u32 magnitude = bits & 0x7FFFFFFF;
    if ((s32)magnitude >= 0x3F800000) {
        if ((s32)bits > 0) {
            return lbl_805FB120;
        }
        return lbl_805FB124;
    }
    if ((s32)magnitude < 0x3F000000) {
        if ((s32)magnitude <= 0x23000000) {
            return lbl_805FB128;
        }
        f32 f8 = value * value;
        f32 f4 = lbl_805FB148 * f8 + lbl_805FB144;
        f32 f3 = f8 * f4 + lbl_805FB140;
        f32 f1 = lbl_805FB15C * f8 + lbl_805FB158;
        f32 f6 = f8 * f3 + lbl_805FB13C;
        f3 = f8 * f1 + lbl_805FB154;
        f32 f5 = f8 * f6 + lbl_805FB138;
        f32 f2 = f8 * f3 + lbl_805FB150;
        f3 = f8 * f5 + lbl_805FB134;
        f1 = f8 * f2 + lbl_805FB14C;
        f2 = f8 * f3;
        f1 = f2 / f1;
        f32 f0 = lbl_805FB130 - value * f1;
        f0 = value - f0;
        return lbl_805FB12C - f0;
    }
    if ((s32)bits < 0) {
        f32 f0 = lbl_805FB14C + value;
        f32 f31 = lbl_805FB160 * f0;
        f32 f1 = f31;
        f1 = fn_80482340(f1);
        f32 f5 = lbl_805FB148 * f31 + lbl_805FB144;
        f32 f4 = f31 * f5 + lbl_805FB140;
        f32 f2 = lbl_805FB15C * f31 + lbl_805FB158;
        f32 f7 = f31 * f4 + lbl_805FB13C;
        f4 = f31 * f2 + lbl_805FB154;
        f32 f6 = f31 * f7 + lbl_805FB138;
        f4 = f31 * f4 + lbl_805FB150;
        f5 = f31 * f6 + lbl_805FB134;
        f32 f3 = f31 * f4 + lbl_805FB14C;
        f4 = f31 * f5;
        f3 = f4 / f3;
        f2 = f3 * f1 - lbl_805FB130;
        f1 = f1 + f2;
        return lbl_805FB130 - lbl_805FB168 * f1;
    }
    f32 f0 = lbl_805FB14C - value;
    f32 f31 = lbl_805FB160 * f0;
    f32 result = fn_80482340(f31);
    u32 rounded_bits = *(u32 *)&result;
    rounded_bits &= 0xFFFFF000;
    f32 rounded = *(f32 *)&rounded_bits;
    f32 f2 = lbl_805FB148 * f31 + lbl_805FB144;
    f32 f4 = f31 * f2 + lbl_805FB140;
    f4 = f31 * f4 + lbl_805FB13C;
    f2 = lbl_805FB15C * f31 + lbl_805FB158;
    f4 = f31 * f4 + lbl_805FB138;
    f2 = f31 * f2 + lbl_805FB154;
    f4 = f31 * f4 + lbl_805FB134;
    f2 = f31 * f2 + lbl_805FB150;
    f32 f5 = f31 * f4 + lbl_805FB14C;
    f4 = f31 * f2 + lbl_805FB130;
    f2 = f31 - rounded * rounded;
    f0 = result + rounded;
    f5 = f31 * f5;
    f32 f3 = f31 * f4 + lbl_805FB14C;
    f0 = f2 / f0;
    f2 = f5 / f3;
    f0 = f2 * result + f0;
    f0 = rounded + f0;
    return lbl_805FB168 * f0;
}

extern u32 lbl_805EDDB8;
extern u32 lbl_805EDDBC;
extern u32 lbl_805E15F0[];

u32 *fn_8045B048(u32 *object, u32 value) {
    lbl_805F14E8 = value;
    u32 *table = fn_8047FC04(*(u32 *)lbl_8056D840, lbl_805EDDB8, 4, lbl_805EDDBC,
                             lbl_805E15F0, 0x00040007);
    *(u32 **)(lbl_805F15B4 + lbl_805F14E8) = table;
    if (*(u32 **)(lbl_805F15B4 + lbl_805F14E8) == NULL) {
        return NULL;
    }
    (&lbl_805F14E8)[1]++;
    return object;
}

u32 *fn_8045B0D8(u32 *object) {
    u32 *table = *(u32 **)(lbl_805F15B4 + lbl_805F14E8);
    if (table != NULL) {
        fn_8047FCDC(table);
        *(u32 **)(lbl_805F15B4 + lbl_805F14E8) = NULL;
    }
    (&lbl_805F14E8)[1]--;
    return object;
}

void fn_8048CD8C(u32 *value);

u32 *fn_8045B270(u32 *object, u32 *value) {
    if (value != NULL) {
        value[21]++;
    }
    if (object[0] != 0) {
        fn_8048CD8C((u32 *)object[0]);
    }
    object[0] = (u32)value;
    return object;
}

extern u8 lbl_8056D858[];

u32 *fn_8045B13C(void) {
    u32 *object = ((u32 *(*)(u32 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        *(u32 **)(lbl_805F15B4 + lbl_805F14E8), 0x00030007);
    if (object == NULL) {
        return NULL;
    }
    u32 *constants = (u32 *)lbl_8056D858;
    u32 fill = 255;
    u32 one = 1;
    u8 bytes[4];
    bytes[0] = fill;
    bytes[1] = fill;
    bytes[2] = fill;
    bytes[3] = fill;
    *(s16 *)((u8 *)object + 24) = one;
    fill = *(u32 *)bytes;
    object[1] = fill;
    object[0] = 0;
    object[2] = 0;
    object[3] = constants[0];
    object[4] = constants[1];
    object[5] = constants[2];
    fn_8048209C(lbl_8056D840, object);
    return object;
}

extern u32 lbl_805F14E4;
extern u32 lbl_805F14E0;
static inline void RwCpp21CleanupObject(u32 *object) {
    if (*(s16 *)((u8 *)object + 24) == 1) {
        fn_80482144(lbl_8056D840, object);
        if (object[0] != 0) {
            fn_8048CD8C((u32 *)object[0]);
        }
        object[0] = 0;
        ((void (*)(u8 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
            *(u8 **)(lbl_805F15B4 + lbl_805F14E8), object);
    } else {
        (*(s16 *)((u8 *)object + 24))--;
    }
}

u32 fn_80483D28(u8 *);
void fn_80481870(u8 *, u32, u32 *, u32);

u32 *fn_8045B348(u8 *stream) {
    u32 second;
    u32 first;
    volatile u32 saved;
    u8 buffer[28];
    if ((s32)fn_8047CF50(stream, 1, &second, &first) == 0) {
        return NULL;
    }
    if (first < 0x00035000 || first > 0x00037002) {
        goto invalid_first;
    }
    memset(buffer, 0, 28);
    if (fn_80480D4C(stream, buffer, second) != second) {
        return NULL;
    }
    saved = *(u32 *)(buffer + 4);
    fn_8047D254(buffer, 28);
    *(u32 *)(buffer + 4) = saved;
    u32 *object = ((u32 *(*)(u32 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        *(u32 **)(lbl_805F15B4 + lbl_805F14E8), 0x00030007);
    if (object == NULL) {
        object = NULL;
        goto initialized;
    }
    u32 *constants = (u32 *)lbl_8056D858;
    u32 fill = 255;
    u32 one = 1;
    u8 bytes[4];
    bytes[0] = fill;
    bytes[1] = fill;
    bytes[2] = fill;
    bytes[3] = fill;
    *(s16 *)((u8 *)object + 24) = one;
    fill = *(u32 *)bytes;
    object[1] = fill;
    object[0] = 0;
    object[2] = 0;
    object[3] = constants[0];
    object[4] = constants[1];
    object[5] = constants[2];
    fn_8048209C(lbl_8056D840, object);
initialized:
    if (object == NULL) {
        return NULL;
    }
    object[1] = *(u32 *)(buffer + 4);
    object[3] = *(u32 *)(buffer + 16);
    object[4] = *(u32 *)(buffer + 20);
    object[5] = *(u32 *)(buffer + 24);
    object[0] = 0;
    if ((s32)*(u32 *)(buffer + 12) != 0) {
        if ((s32)fn_8047CF50(stream, 6, NULL, &first) == 0) {
            RwCpp21CleanupObject(object);
            return NULL;
        }
        if (first >= 0x00035000 && first <= 0x00037002) {
            object[0] = fn_80483D28(stream);
        } else {
            RwCpp21CleanupObject(object);
            {
                u32 error[2];
                error[0] = 2;
                error[1] = fn_8047DC28(0x80000004);
                fn_8047DB84(error);
                return NULL;
            }
        }
    }
    lbl_805F14E4 = 0;
    lbl_805F14E0 = 0;
    if (fn_804816AC(lbl_8056D840, stream, object) == 0) {
        RwCpp21CleanupObject(object);
        return NULL;
    }
    if (lbl_805F14E4 != 0) {
        fn_80481870(lbl_8056D840, lbl_805F14E4, object, lbl_805F14E0);
    }
    return object;
invalid_first:
    {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        return NULL;
    }
}

u32 fn_8045B1F4(u32 *object) {
    if (*(s16 *)((u8 *)object + 24) == 1) {
        fn_80482144(lbl_8056D840, object);
        fn_8045B270(object, NULL);
        ((void (*)(u8 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
            *(u8 **)(lbl_805F15B4 + lbl_805F14E8), object);
    } else {
        (*(s16 *)((u8 *)object + 24))--;
    }
    return 1;
}

s32 fn_8045B7AC(u32 *object, u32 *value) {
    if ((s32)object[2] > (s32)object[1]) {
        ((u32 *)object[0])[object[1]] = (u32)value;
        (*(s16 *)((u8 *)value + 24))++;
        object[1]++;
        return (s32)object[1] - 1;
    }
    u32 capacity = object[2] + 20;
    u32 error_size = capacity * 4;
    u32 *buffer;
    if (object[0] != 0) {
        buffer = ((u32 *(*)(u32 *, u32, u32))*(u32 *)(lbl_805F15B4 + 0x110))(
            (u32 *)object[0], capacity * 4, 0x01030008);
    } else {
        buffer = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(capacity * 4, 0x01030008);
    }
    if (buffer == NULL) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000013, error_size);
        fn_8047DB84(error);
        return -1;
    }
    object[0] = (u32)buffer;
    object[2] += 20;
    buffer[object[1]] = (u32)value;
    (*(s16 *)((u8 *)value + 24))++;
    object[1]++;
    return (s32)object[1] - 1;
}

u32 *fn_8045B6F0(u32 *object) {
    u32 *current;
    u32 *entries = (u32 *)object[0];
    if (entries != NULL) {
        s32 count = object[1];
        current = entries;
        for (s32 i = 0; i < count; i++) {
            fn_8045B1F4((u32 *)current[0]);
            current[0] = 0;
            current++;
        }
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(entries);
        object[0] = 0;
    }
    object[1] = 0;
    object[2] = 0;
    return object;
}

extern u32 lbl_805F14F0;
extern u32 lbl_805EDDC0;

u32 *fn_8047FBD8(u32 size, u32 count, u32 alignment, u32 tag);

u8 *fn_8045BFBC(u8 *stream, u32 value) {
    lbl_805F14F0 = value;
    if ((s32)(&lbl_805F14F0)[1] == 0) {
        lbl_805EDDC0 = (u32)fn_8047FBD8(12, 50, 4, 0x00040502);
        if (lbl_805EDDC0 == 0) {
            if (lbl_805EDDC0 != 0) {
                fn_8047FCDC((u32 *)lbl_805EDDC0);
                lbl_805EDDC0 = 0;
            }
            return NULL;
        }
    }
    *(s16 *)(lbl_805F15B4 + lbl_805F14F0) = 1;
    (&lbl_805F14F0)[1]++;
    u32 offset = lbl_805F14F0 + 8;
    lbl_805F15B4[offset] = 3;
    offset = lbl_805F14F0 + 9;
    lbl_805F15B4[offset] = 4;
    offset = lbl_805F14F0 + 10;
    lbl_805F15B4[offset] = 5;
    offset = lbl_805F14F0 + 12;
    lbl_805F15B4[offset] = 1;
    offset = lbl_805F14F0 + 16;
    lbl_805F15B4[offset] = 2;
    offset = lbl_805F14F0 + 24;
    lbl_805F15B4[offset] = 6;
    offset = lbl_805F14F0 + 40;
    lbl_805F15B4[offset] = 4;
    offset = lbl_805F14F0 + 41;
    lbl_805F15B4[offset] = 8;
    offset = lbl_805F14F0 + 42;
    lbl_805F15B4[offset] = 0;
    offset = lbl_805F14F0 + 43;
    lbl_805F15B4[offset] = 1;
    offset = lbl_805F14F0 + 44;
    lbl_805F15B4[offset] = 2;
    offset = lbl_805F14F0 + 45;
    lbl_805F15B4[offset] = 16;
    return stream;
}

u32 *fn_8045BF64(u32 *object) {
    if (--(&lbl_805F14F0)[1] == 0 && lbl_805EDDC0 != 0) {
        fn_8047FCDC((u32 *)lbl_805EDDC0);
        lbl_805EDDC0 = 0;
    }
    return object;
}

u32 *fn_8045C144(u32 count) {
    u32 *object = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        lbl_805EDDC0, 0x00030502);
    if (object != NULL) {
        object[1] = 0;
        if (count != 0) {
            u32 size = count * 20;
            object[2] = (u32)((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x01030502);
            if (object[2] == 0) {
                ((void (*)(u32, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(lbl_805EDDC0, object);
                u32 error[2];
                error[0] = 2;
                error[1] = fn_8047DC28(0x80000013, size);
                fn_8047DB84(error);
                return NULL;
            }
            object[0] = count;
        } else {
            object[2] = 0;
            object[0] = 0;
        }
        return object;
    }
    u32 error[2];
    error[0] = 2;
    error[1] = fn_8047DC28(0x80000013, 12);
    fn_8047DB84(error);
    return NULL;
}

u32 fn_8045C26C(u32 *object) {
    if (object[2] != 0) {
        ((void (*)(u32))*(u32 *)(lbl_805F15B4 + 0x10C))(object[2]);
        object[2] = 0;
    }
    ((void (*)(u32, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(lbl_805EDDC0, object);
    return 1;
}

u32 fn_8045C2D4(u32 *object) {
    if (object[0] != 0 || *(u16 *)((u8 *)object + 4) != 0 || *(u16 *)((u8 *)object + 6) != 0 ||
        object[2] != 0 || object[3] != 0) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(object);
    }
    return 1;
}

u32 fn_8047D868(u8 *stream, void *data, u32 size);
u32 fn_8047D458(u8 *stream, void *data, u32 size);

u8 *fn_8045AF28(u8 *stream, s32 type) {
    if (fn_8047D868(stream, &lbl_805F14E4, 4) == 0) {
        return NULL;
    }
    if (type == 8 && fn_8047D868(stream, &lbl_805F14E0, 4) == 0) {
        return NULL;
    }
    return stream;
}

u8 *fn_8045AFA4(u8 *stream, const u32 *object) {
    if (fn_8047D458(stream, (u8 *)object[2] + 44, 4) == 0) {
        return NULL;
    }
    if (fn_8047D458(stream, (u8 *)object[2] + 48, 4) != 0) {
        return stream;
    }
    return NULL;
}

void fn_804815F0(u8 *, u32, u32, u32, u32);

void fn_8045AAE0(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D828, first, second, third, fourth);
}

void fn_8045B30C(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D840, first, second, third, fourth);
}

void fn_80481DC8(u8 *, u32, u32, u32, u32, u32);

void fn_8045AAA0(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D828, first, second, third, fourth, fifth);
}

void fn_8045B2CC(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D840, first, second, third, fourth, fifth);
}
}

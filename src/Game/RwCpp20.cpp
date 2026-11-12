/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * The reallocator, unique-array cleanup, and object constructor are complete C++ implementations.
 * Remaining differences are loop unrolling, register allocation, and paired-single instruction
 * selection rather than missing behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F14D0;
extern u8 *lbl_805F15B4;

extern "C" {
void fn_8045984C(s16 *object) {
    object[7] = object[7] + 1;
}

void fn_80458D10(void) {
    (&lbl_805F14D0)[1]--;
}

void fn_80458CF8(u32 unused, u32 value) {
    lbl_805F14D0 = value;
    (&lbl_805F14D0)[1]++;
}

void fn_8045ACD4(void);

void fn_8045A760(void) {
    fn_8045ACD4();
}

extern u8 lbl_8056D7E0[];
void fn_80481D98(u8 *, u32);

void fn_80458C90(u32 argument) {
    fn_80481D98(lbl_8056D7E0, argument);
}

void fn_80481634(u8 *, u32, u32);
void fn_80481670(u8 *, u32, u32);

void fn_80458C28(u32 first, u32 second) {
    fn_80481634(lbl_8056D7E0, first, second);
}

void fn_80458C5C(u32 first, u32 second) {
    fn_80481670(lbl_8056D7E0, first, second);
}

void fn_8048D7C0(void);

u32 fn_80458CBC(u8 *object) {
    fn_8048D7C0();
    object[3] |= 1;
    return (u32)object;
}

extern u8 lbl_8056D810[];
void fn_804815F0(u8 *, u32, u32, u32, u32);

void fn_8045998C(u32 first, u32 second, u32 third, u32 fourth) {
    fn_804815F0(lbl_8056D810, first, second, third, fourth);
}

void fn_80481DC8(u8 *, u32, u32, u32, u32, u32);

void fn_8045994C(u32 first, u32 second, u32 third, u32 fourth, u32 fifth) {
    fn_80481DC8(lbl_8056D810, first, second, third, fourth, fifth);
}

void fn_8048722C(u32 count);

u32 fn_8045A784(volatile u32 *object, float value) {
    u32 count = object[1];
    ((float *)object)[5] = value;
    if (count != 0) {
        fn_8048722C(count);
    }
    return (u32)object;
}

u32 *fn_80459464(s32, s32, u32);
u32 *fn_804591C4(u32 *);
void fn_8045985C(void *);
extern f32 lbl_805FB118;
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(u32 *);

#pragma optimize_for_size off
s32 fn_80458DB0(u32 *object, u32 index) {
    u32 stride = 28;
    if ((object[2] & 0x01000000) == 0) {
        u32 extra = object[5] * 12;
        stride += extra;
        if ((object[2] & 0x10) != 0) {
            stride += extra;
        }
    }
    u32 oldCount = object[6];
    u32 size = stride * (oldCount + index);
    u32 *block;
    if (object[23] != 0) {
        block = ((u32 *(*)(u32 *, u32, u32))*(u32 *)(lbl_805F15B4 + 0x110))(
            (u32 *)object[23], size, 0x0003000F);
    } else {
        block = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x0003000F);
    }
    if (block == NULL) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000013, size);
        fn_8047DB84(error);
        return -1;
    }
    if (object[23] != 0) {
        u32 oldBytes = stride * oldCount;
        u8 *source = (u8 *)block + oldBytes - 1;
        u8 *dest = source + index * 28;
        for (u32 remaining = oldCount * 28; remaining != 0; remaining--) {
            *dest-- = *source--;
        }
    }
    object[6] += index;
    object[23] = (u32)block;
    u32 *links = (u32 *)((u8 *)block + object[6] * 28);
    u32 *cursor = links;
    for (u32 i = 0; i < object[6]; i++) {
        links[5] = 0;
        links[6] = 0;
        if ((object[2] & 0x01000000) == 0 && object[5] != 0) {
            links[5] = (u32)cursor;
            cursor = (u32 *)((u8 *)cursor + object[5] * 12);
            if ((object[2] & 0x10) != 0) {
                links[6] = (u32)cursor;
                cursor = (u32 *)((u8 *)cursor + object[5] * 12);
            }
        }
        links += 7;
    }
    u32 first = object[6] - index;
    f32 constant = lbl_805FB118;
    for (u32 i = first; i < object[6]; i++) {
        f32 *entry = (f32 *)((u8 *)block + i * 28);
        entry[1] = constant;
        entry[2] = constant;
        entry[3] = constant;
        entry[4] = constant;
        ((u32 *)entry)[0] = (u32)object;
    }
    return object[6] - index;
}
#pragma optimize_for_size reset

u32 fn_8045C144(u32);
u32 fn_8045B79C(u32 *, u32);
u32 fn_8045C340(u32 *, u32 *, u16, u16, u16, u16, u32);
u32 *fn_8045F1FC(u32 *, u32);
void fn_8045C26C(u32 *);

static inline u32 RwCpp20AppendUnique(u32 *array, u32 *count, u32 value) {
    u32 i = 0;
    while ((u16)i < (u16)*count) {
        if (array[i] == value) {
            break;
        }
        i++;
    }
    if ((u16)i == (u16)*count) {
        array[i] = value;
        (*count)++;
    }
    return i;
}

#pragma use_lmw_stmw off
u32 *fn_804591C4(u32 *object) {
    if (object[21] != 0) {
        return object;
    }
    u32 *nested = (u32 *)fn_8045C144(object[4]);
    if (nested == NULL) {
        return NULL;
    }
    if ((s32)object[9] > 0) {
        u32 size = object[9] * 4;
        u32 *first = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x00010006);
        u32 *second = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x00010503);
        u32 *third = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x00010503);
        u32 firstCount = 0;
        u32 secondCount = 0;
        u32 thirdCount = 0;
        for (s32 i = 0; i < (s32)object[4]; i++) {
            u16 *node = (u16 *)(object[11] + i * 8);
            u32 *entry = (u32 *)fn_8045B79C(object + 8, node[3]);
            u32 value = entry[0];
            u32 child = value != 0 ? entry[0] : 0;
            u32 other = entry[2];
            RwCpp20AppendUnique(first, &firstCount, value);
            RwCpp20AppendUnique(second, &secondCount, child);
            u32 otherIndex = RwCpp20AppendUnique(third, &thirdCount, other);
            fn_8045C340(nested, entry, node[0], node[1], node[2], node[3], otherIndex);
        }
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(first);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(second);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(third);
    }
    u32 *result;
    if ((object[2] & 1) != 0) {
        result = fn_8045F1FC(nested, 1);
    } else {
        result = fn_8045F1FC(nested, 0);
    }
    if (result != NULL) {
        object[21] = (u32)result;
        return object;
    }
    fn_8045C26C(nested);
    return NULL;
}
#pragma use_lmw_stmw reset

u32 fn_8045B788(u32 *);
void fn_8045B6F0(u32 *);
void fn_8048209C(void *, void *);
void *memset(void *, int, u32);

#pragma use_lmw_stmw off
u32 *fn_80459464(s32 first, s32 second, u32 flags) {
    if (first < 0 || first >= 0x00010000 || second < 0) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(6);
        fn_8047DB84(error);
        return NULL;
    }
    u32 masked = flags & 0x00FF0000;
    u32 stride;
    if (masked != 0) {
        stride = ((s32)masked) >> 16;
    } else if ((flags & 0x80) != 0) {
        stride = 2;
    } else {
        stride = (flags >> 2) & 1;
    }
    u32 baseFlags = (flags & 0xFF) & 0xFF7B;
    u32 extra = stride != 0 ? 0x80 : 0;
    if (stride == 1) {
        extra = 4;
    }
    u32 objectFlags = baseFlags | extra;
    u32 size = *(u32 *)lbl_8056D810;
    if ((flags & 0x01000000) == 0) {
        if ((objectFlags & 0x10) != 0) {
            size += first * 4;
        }
        if (stride != 0) {
            size += stride * first * 8;
        }
        size += second * 8;
    }
    u32 *object = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x0003000F);
    if (object == NULL) {
        return NULL;
    }
    if (fn_8045B788(object + 8) == 0) {
        return NULL;
    }
    object[23] = 0;
    ((u8 *)object)[0] = 8;
    ((u8 *)object)[1] = 0;
    ((u8 *)object)[2] = 0;
    ((u8 *)object)[3] = 0;
    object[1] = 0;
    object[22] = 0;
    ((s16 *)object)[6] = 0;
    ((s16 *)object)[7] = 1;
    object[21] = 0;
    object[7] = stride;
    memset(object + 13, 0, 32);
    object[12] = 0;
    object[11] = 0;
    object[4] = second;
    object[2] = objectFlags | (flags & 0xF0);
    object[5] = first;
    if ((flags & 0x01000000) == 0) {
        u32 *cursor = (u32 *)((u8 *)object + *(u32 *)lbl_8056D810);
        if ((objectFlags & 0x10) != 0 && first != 0) {
            object[12] = (u32)cursor;
            cursor = (u32 *)((u8 *)cursor + first * 4);
        }
        if (stride != 0 && first != 0) {
            for (u32 i = 0; i < stride; i++) {
                object[13 + i] = (u32)cursor;
                cursor = (u32 *)((u8 *)cursor + first * 8);
            }
        }
        if (second != 0) {
            object[11] = (u32)cursor;
            if (second > 0) {
                s16 *entries = (s16 *)cursor;
                for (s32 i = 0; i < second; i++) {
                    entries[i] = -1;
                }
            }
        }
    }
    if (fn_80458DB0(object, 1) < 0) {
        fn_8045B6F0(object + 8);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(object);
        return NULL;
    }
    fn_8048209C(lbl_8056D810, object);
    return object;
}
#pragma use_lmw_stmw reset

void *fn_80458D24(f32 value) {
    void *object = fn_80459464(NULL, NULL, NULL);
    if (object != NULL) {
        f32 *entry = ((f32 **)object)[0x17];
        f32 constant = lbl_805FB118;
        entry[1] = constant;
        entry[2] = constant;
        entry[3] = constant;
        entry[4] = value;
        if (fn_804591C4((u32 *)object) == 0) {
            fn_8045985C(object);
            return NULL;
        }
    }
    return object;
}

s32 fn_8045B8EC(void *, u32);
s32 fn_8045B7AC(void *, u32);

void *fn_8045905C(void *object, void *output, u32 mode) {
    if (mode != 0) {
        if (fn_8045B8EC((u8 *)object + 0x20, mode) < 0) {
            return NULL;
        }
        if (fn_8045B7AC((u8 *)object + 0x20, mode) < 0) {
            return NULL;
        }
        *(s16 *)((u8 *)output + 6) = -1;
    } else {
        *(s16 *)((u8 *)output + 6) = 0;
    }
    return object;
}

void fn_8045C2D4(u32);

void *fn_8045916C(u32 *object, u32 value) {
    u16 *field = (u16 *)object + 6;
    *field |= value;
    if ((value & 1) != 0 && object[0x15] != 0) {
        fn_8045C2D4(object[0x15]);
        object[0x15] = 0;
    }
    return object;
}

#pragma use_lmw_stmw off
void *fn_804590E8(void *object, void *callback, void *data) {
    s32 count;
    s32 i = 0;
    u32 offset = 0;
    count = ((u32 *)object)[9];
    while (i < count) {
        if (((u32 (*)(u32, void *))callback)(((u32 **)object)[8][offset], data) != 0) {
            return object;
        }
        i++;
        offset++;
    }
    return object;
}
#pragma use_lmw_stmw reset
}

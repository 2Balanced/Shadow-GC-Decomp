/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * fn_8045CA38 has retail's instruction sequence and size; GC/2.7 assigns one extra saved register
 * to the final field temporaries. Direct, cached, and scoped comparison forms were measured. */
#include <dolphin/types.h>

extern s16 *lbl_805F15B4;
extern u32 lbl_805F14F0;

extern u32 lbl_805FB170;

extern "C" {
s32 fn_8045CA38(u32 **left_argument, u32 **right_argument) {
    s32 left_value = 0;
    s32 right_value = 0;
    u32 *left_child = (u32 *)(*left_argument)[2];
    u32 *right_child = (u32 *)(*right_argument)[2];
    if (left_child == right_child) {
        return 0;
    }
    if (left_child != NULL) {
        u32 *nested = (u32 *)left_child[0];
        if (nested != NULL) {
            s32 type = (*(u8 *)(*(u32 *)nested + 35) & 0xF) << 8;
            if (type == 0x100 || type == 0x300 || type == 0x500) {
                left_value = lbl_805FB170;
            }
        }
        if (((u8 *)left_child)[7] != 0xFF) {
            left_value |= lbl_805FB170;
        }
    }
    if (right_child != NULL) {
        u32 *nested = (u32 *)right_child[0];
        if (nested != NULL) {
            s32 type = (*(u8 *)(*(u32 *)nested + 35) & 0xF) << 8;
            if (type == 0x100 || type == 0x300 || type == 0x500) {
                right_value = lbl_805FB170;
            }
        }
        if (((u8 *)right_child)[7] != 0xFF) {
            right_value |= lbl_805FB170;
        }
    }
    left_value |= (*(u16 *)((u8 *)*left_argument + 16) > *(u16 *)((u8 *)*right_argument + 16)) ? 8 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 16) < *(u16 *)((u8 *)*right_argument + 16)) ? 8 : 0;
    left_value |= (*(u16 *)((u8 *)*left_argument + 18) > *(u16 *)((u8 *)*right_argument + 18)) ? 4 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 18) < *(u16 *)((u8 *)*right_argument + 18)) ? 4 : 0;
    left_value |= (*(u16 *)((u8 *)*left_argument + 14) > *(u16 *)((u8 *)*right_argument + 14)) ? 2 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 14) < *(u16 *)((u8 *)*right_argument + 14)) ? 2 : 0;
    left_value |= (*(u16 *)((u8 *)*left_argument + 12) > *(u16 *)((u8 *)*right_argument + 12)) ? 1 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 12) < *(u16 *)((u8 *)*right_argument + 12)) ? 1 : 0;
    return left_value - right_value;
}

void fn_8045D20C(u32 *object, u32 **array, s32 index) {
    if (index < 4) {
        object[7] = 1;
        return;
    }
    object[6] = 1;
    u32 *head = array[((u8 *)object)[32]];
    if (head == object) {
        array[((u8 *)object)[32]] = (u32 *)head[4];
        if (array[((u8 *)object)[32]] != NULL) {
            array[((u8 *)object)[32]][5] = 0;
        }
    } else {
        if (object[4] != 0) {
            ((u32 *)object[4])[5] = object[5];
        }
        if (object[5] != 0) {
            ((u32 *)object[5])[4] = object[4];
        }
    }
    for (u8 i = 0; i < 3; i++) {
        u32 *child;
        u32 *candidate = NULL;
        u32 *container = (u32 *)object[i + 1];
        child = (u32 *)container[1];
        if (child != NULL && child != object && (s32)child[6] == 0) {
            candidate = child;
        } else {
            child = (u32 *)container[2];
            if (child != NULL && (s32)child[6] == 0) {
                candidate = child;
            }
        }
        if (candidate != NULL) {
            head = array[((u8 *)candidate)[32]];
            if (head == candidate) {
                array[((u8 *)candidate)[32]] = (u32 *)head[4];
                if (array[((u8 *)candidate)[32]] != NULL) {
                    array[((u8 *)candidate)[32]][5] = 0;
                }
            } else {
                if (candidate[4] != 0) {
                    ((u32 *)candidate[4])[5] = candidate[5];
                }
                if (candidate[5] != 0) {
                    ((u32 *)candidate[5])[4] = candidate[4];
                }
            }
            ((u8 *)candidate)[32]--;
            candidate[4] = (u32)array[((u8 *)candidate)[32]];
            if (candidate[4] != 0) {
                ((u32 *)candidate[4])[5] = (u32)candidate;
            }
            array[((u8 *)candidate)[32]] = candidate;
            candidate[5] = 0;
        }
    }
}

u32 fn_8045C9C8(u32 *object, u8 *entry) {
    if (entry[0] == 8 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    if (entry[0] == 7 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    goto variable_size;
fixed_size:
    return *(u16 *)((u8 *)object + 4) * 8 + 12;
variable_size:
    {
        u32 count = *(u16 *)((u8 *)object + 4);
        u32 result = count * 8;
        result += object[2] * 4;
        result += 12;
        return result;
    }
}

u32 *fn_8045C4E4(u32 *object, u32 (*callback)(u8 *, u32 *, u32), u32 argument) {
    s32 count = *(u16 *)((u8 *)object + 4);
    u8 *entry = (u8 *)object + object[3];
    entry += 16;
    while (count-- != 0) {
        if (callback(entry, object, argument) == 0) {
            return object;
        }
        entry += 12;
    }
    return object;
}

void fn_8045CA20(void) {
    s16 *target = (s16 *)((u8 *)lbl_805F15B4 + lbl_805F14F0);
    *target += 1;
}
}

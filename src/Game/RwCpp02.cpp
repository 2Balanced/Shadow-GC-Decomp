/* RenderWare list, callback, and owner helpers. Function and member names are inferred from
 * behavior. Definitions are in reverse address order because deferred inlining emits the last one first. */
#include <dolphin/types.h>

struct RwCpp02Object {
    /* 0x00 */ u32 m_flags;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u8 field_0x10[0x24];
    /* 0x34 */ u32 field_0x34;
    /* 0x38 */ u8 field_0x38[0xCC];
    /* 0x104 */ u32 field_0x104;
};

struct RwCpp02Owner {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ void *m_field_0xC;
};

extern u8 *lbl_805F1404;
extern float lbl_805FAE70;
extern float lbl_805FAE74;
extern float lbl_805FAE78;
extern float lbl_805FAE7C;
extern float lbl_805FAE80;
extern float lbl_805FAE88;
extern float lbl_805FAE90;
extern float lbl_805FAE98;
extern double lbl_805FAF10;
extern double lbl_805FAF18;
extern double lbl_805FAF20;

extern "C" {
void fn_8043B44C(void **, void (*)(void *));
void **fn_8043B3F4(void **, int (*)(void *));
void fn_8043B4B0(void *, void **, void (*)(void *));
int fn_8043B810(void *);
void fn_8043B89C(void *);
void *fn_8043B560(u32, void **, void (*)(void *));
void *fn_8043B3DC(void *, void *);
void fn_80386348(u32, u32);
void fn_8038A338(u32);
void fn_8043CA7C(...);
void fn_80386844(u32);
void fn_8043C084(void *);

void fn_8043C170(u32 *object, int value) {
    s64 wide = value < 0 ? (s64)value * (s64)object[7] : (s64)value * (s64)object[6];
    if ((int)wide < 0) {
        wide = -wide;
    }
    s64 rounded = ((int)wide >> 13) + ((int)wide < 0 && (wide & 0x1FFF) != 0 ? 1 : 0);
    u32 quotients = (u32)rounded / 0xC;
    u32 sign = (u32)wide >> 0x1F;
    wide = (wide & 0x1FFF) * 0x80000 - sign;
    u64 folded = (((wide & 0x7FFFF) << 13) | ((u32)wide >> 0x13)) + sign;
    wide = folded << 7;
    u32 scaled = (u32)wide | (u32)(folded >> 0x19);
    s64 remainder = ((int)scaled >> 13) + ((int)scaled < 0 && (wide & 0x1F80) != 0 ? 1 : 0);
    while ((int)rounded > 0xB) {
        rounded -= 0xC;
        quotients--;
    }
    while ((int)rounded > 0) {
        rounded--;
    }
    u32 steps = (u32)(remainder - 1) >> 4;
    while (remainder > 0x10) {
        remainder -= 0x10;
        steps--;
    }
    while (remainder > 0) {
        remainder--;
    }
    fn_80386844(object[0xD]);
}

void fn_8043C12C(void **list, void *data) {
    u32 *object = (u32 *)fn_8043B560(8, list, NULL);
    if (object != NULL) {
        object[1] = (u32)data;
    }
}

void *fn_8043C10C(void *first, void *second) {
    return fn_8043B3DC(first, second);
}

void fn_8043C084(void *param) {
    if (param == NULL) {
        return;
    }
    RwCpp02Object *object = *(RwCpp02Object **)((u8 *)param + 4);
    if (object == NULL) {
        return;
    }
    object->field_0x104 = 0;
    if (object->field_0x34 != 0) {
        fn_80386348(object->field_0x34, 0);
        fn_8038A338(object->field_0x34);
    }
    object->m_flags &= 0x7FFFFFFF;
    object->m_flags &= 0xFFFFFFFE;
    object->m_flags &= 0xFFFFFFFD;
    object->field_0x8 = 0;
}

void fn_8043C05C(void **list) {
    fn_8043B44C(list, fn_8043C084);
}

int fn_8043BB18(void) {
    int count = 0;
    if (lbl_805F1404 != NULL) {
        for (int i = 0; i < 64; i++) {
            if (*(u8 *)(lbl_805F1404 + i * 0x14 + 0x4354) != 0) {
                count++;
            }
        }
    }
    return count;
}

void fn_8043BB54(RwCpp02Owner *owner) {
    if (owner == NULL) {
        return;
    }
    void **list = (void **)((u8 *)owner->m_field_0xC + 0xA0);
    if (*(void **)list == NULL) {
        return;
    }
    *(RwCpp02Owner **)(lbl_805F1404 + 0xFDFC) = owner;
    void *found = (void *)fn_8043B3F4(list, fn_8043B810);
    if (found != NULL) {
        fn_8043B4B0(found, list, fn_8043B89C);
    }
}

void *fn_8043BCDC(int param) {
    int index = 0;
    for (int i = 0; i < 64; i++) {
        int value = *(int *)(lbl_805F1404 + 0x4C) + index;
        int sign = value >> 31;
        int rotated = ((sign * 0x40) | ((u32)(value * 0x4000000 + sign) >> 0x1A)) - sign;
        int offset = rotated * 0x14;
        if (*(u8 *)(lbl_805F1404 + offset + 0x4354) != 0 &&
            *(int *)(lbl_805F1404 + offset + 0x435C) <= param) {
            fn_8043BB54((RwCpp02Owner *)(lbl_805F1404 + offset + 0x4354));
            *(int *)(lbl_805F1404 + 0x4C) = rotated + 1;
            return lbl_805F1404 + offset + 0x4354;
        }
        index++;
    }
    return NULL;
}

void *fn_8043BBC8(int param, int count) {
    void *result = NULL;
    if (count < 1) {
        return fn_8043BCDC(param);
    }
    while (count > 0) {
        int used = 0;
        int index = 0;
        for (int i = 0; i < 64; i++) {
            int value = *(int *)(lbl_805F1404 + 0x4C) + index;
            int sign = value >> 31;
            int rotated = ((sign * 0x40) | ((u32)(value * 0x4000000 + sign) >> 0x1A)) - sign;
            int offset = rotated * 0x14;
            if (*(u8 *)(lbl_805F1404 + offset + 0x4354) != 0 &&
                *(int *)(lbl_805F1404 + offset + 0x435C) <= param) {
                void *entry = lbl_805F1404 + offset + 0x4354;
                if (entry != NULL) {
                    void *inner = NULL;
                    while ((inner = fn_8043B3DC((u8 *)entry + 0x10, inner)) != NULL) {
                        if (*(u32 *)((u8 *)inner + 4) != 0) {
                            used++;
                        }
                    }
                }
                count -= used;
                fn_8043BB54((RwCpp02Owner *)entry);
                *(int *)(lbl_805F1404 + 0x4C) = rotated + 1;
                result = entry;
                break;
            }
            index++;
        }
        if (count == param) {
            return result;
        }
    }
    return result;
}

void fn_8043BD80(u32 *object) {
    if (object == NULL || object[0xD] == 0) {
        return;
    }
    u32 flags = *object;
    if ((flags & 0x80000000) != 0 && (flags & 1) != 0 && (flags & 0x40000000) == 0) {
        fn_8043CA7C(object[0x39], object, object[0x3A], object[0x3B], object[0x3C],
                    object[0x3D], object[0x3E], object[0x3F]);
        *object = flags & 0xFFFFFFFE;
    }
}

void fn_8043BE04(u32 *object) {
    if (object == NULL || object[0xD] == 0) {
        return;
    }
    u32 flags = *object;
    if ((flags & 0x80000000) != 0 && (flags & 0x40000000) == 0) {
        *object = flags | 1;
        fn_80386348(object[0xD], 0);
        fn_8038A338(object[0xD]);
    }
}

void fn_8043BE6C(u32 *object) {
    float maximum = lbl_805FAE78;
    float middle = lbl_805FAE74;
    float rate = lbl_805FAE70;
    if ((*object & 1) != 0) {
        return;
    }
    u32 state = object[0x36];
    if (state != 3) {
        if (state > 2) {
            if (state != 5) {
                if (state > 4) {
                    goto done;
                }
                object[0x36] = state + 1;
            }
            middle = lbl_805FAE78;
            rate = lbl_805FAE70;
            *(float *)(object + 0x2B) -= *(float *)(object + 0x35);
            *(float *)(object + 0x2C) = rate * *(float *)(object + 0x2B) - rate;
            if (*(float *)(object + 0x2B) <= middle) {
                *(float *)(object + 0x2B) = middle;
                *(float *)(object + 0x2C) = rate * middle - rate;
            }
            goto done;
        }
        if (state == 1) {
            if (*(float *)(object + 0x31) < *(float *)(object + 0x2C)) {
                *(float *)(object + 0x2B) -= *(float *)(object + 0x33);
                *(float *)(object + 0x2C) = rate * *(float *)(object + 0x2B) - rate;
                if (*(float *)(object + 0x2B) <= maximum) {
                    *(float *)(object + 0x2B) = maximum;
                    *(float *)(object + 0x2C) = rate * maximum - rate;
                }
                if (*(float *)(object + 0x31) < *(float *)(object + 0x2C)) {
                    goto done;
                }
                object[0x36]++;
            } else {
                object[0x36] = 2;
            }
        } else if ((int)state < 1) {
            if ((int)state >= 0) {
                *(float *)(object + 0x2B) += *(float *)(object + 0x32);
                *(float *)(object + 0x2C) = rate * *(float *)(object + 0x2B) - rate;
                if (middle <= *(float *)(object + 0x2B)) {
                    *(float *)(object + 0x2B) = middle;
                    *(float *)(object + 0x2C) = rate * middle - rate;
                    object[0x36]++;
                }
            }
            goto done;
        }
        object[0x36]++;
    }
    middle = lbl_805FAE78;
    rate = lbl_805FAE70;
    *(float *)(object + 0x2B) -= *(float *)(object + 0x34);
    *(float *)(object + 0x2C) = rate * *(float *)(object + 0x2B) - rate;
    if (*(float *)(object + 0x2B) <= middle) {
        *(float *)(object + 0x2B) = middle;
        *(float *)(object + 0x2C) = rate * middle - rate;
    }
done:
    *(float *)(object + 0x2A) += lbl_805FAE7C;
}

int fn_8043C9D0(int bank, u32 index, int value) {
    int count = 0;
    u8 *table = lbl_805F1404 + bank * 8 + 0x5378;
    if (table == NULL) {
        return 0;
    }
    if (index >= *(u32 *)(table + 8) >> 0x18) {
        return 0;
    }
    u32 group = *(u32 *)(table + (*(u32 *)(table + 8) & 0xFFFFFF) + index * 4);
    int offset = 0;
    for (u32 i = 0; i < (group >> 0x18); i++) {
        u32 item = *(u32 *)(table + (group & 0xFFFFFF) + offset);
        int childOffset = 0;
        for (u32 j = 0; j < (item >> 0x18); j++) {
            u8 *entry = table + (item & 0xFFFFFF) + childOffset;
            if ((int)(u8)entry[8] <= value && value <= (int)(u8)entry[9]) {
                count++;
            }
            childOffset += 0x40;
        }
        offset += 0x10;
    }
    return count;
}

void fn_8043C89C(void *param, int bank, u32 index, int value, u32 data) {
    int count = 0;
    u8 *table = lbl_805F1404 + bank * 8 + 0x5378;
    if (table == NULL || index >= *(u32 *)(table + 8) >> 0x18) {
        return;
    }
    u32 first = *(u32 *)(table + 4);
    u32 group = *(u32 *)(table + (*(u32 *)(table + 8) & 0xFFFFFF) + index * 4);
    int offset = 0;
    for (u32 i = 0; i < (group >> 0x18); i++) {
        u32 *groupEntry = (u32 *)(table + (group & 0xFFFFFF) + offset);
        u32 item = *groupEntry;
        int childOffset = 0;
        for (u32 j = 0; j < (item >> 0x18); j++) {
            u8 *entry = table + (item & 0xFFFFFF) + childOffset;
            if ((int)(u8)entry[8] <= value && value <= (int)(u8)entry[9]) {
                void *found = fn_8043B3DC(param, (void *)count);
                if (found != NULL && *(u32 *)((u8 *)found + 4) != 0) {
                    fn_8043CA7C(bank, *(u32 *)((u8 *)found + 4), (u32 *)groupEntry,
                                (u32)table, (u32)entry, value, (u32)(table + (first & 0xFFFFFF)), data);
                }
            }
            childOffset += 0x40;
        }
        offset += 0x10;
    }
}

u32 fn_8043D20C(int value, float factor) {
    float scale = lbl_805FAE74;
    bool negative = false;
    if (value < 0) {
        negative = true;
        value = -value;
    }
    int whole = value / 0x1FFF;
    int remainder = ((value % 0x1FFF) << 7) / 0x1FFE;
    int count = whole / 0xC;
    while (whole >= 0xC) {
        scale *= lbl_805FAE80;
        whole -= 0xC;
    }
    while (count > 0) {
        scale *= lbl_805FAE88;
        count--;
    }
    int steps = (remainder - 1) >> 4;
    while (remainder > 0x10) {
        scale *= lbl_805FAE90;
        remainder -= 0x10;
        steps--;
    }
    while (steps > 0) {
        scale *= lbl_805FAE98;
        steps--;
    }
    factor = negative ? factor / scale : factor * scale;
    u32 result = (u32)factor;
    if (result < 0x200) {
        result = 0x200;
    }
    if (result > 0x40000) {
        result = 0x3FFFF;
    }
    return result;
}

float fn_8043D318(int value) {
    float result = lbl_805FAF10;
    if (value > 0) {
        int count = value >> 8;
        for (int i = 0; i < count; i++) {
            result *= lbl_805FAE88;
        }
        count = (value + 1) >> 1;
        for (int i = 0; i < count; i++) {
            result *= lbl_805FAE98;
        }
    }
    if (value < 0) {
        int count = -value >> 8;
        for (int i = 0; i < count; i++) {
            result /= lbl_805FAE88;
        }
        count = (1 - value) >> 1;
        for (int i = 0; i < count; i++) {
            result /= lbl_805FAE98;
        }
    }
    return result;
}

float fn_8043D3BC(int first, int second, int third) {
    float result = lbl_805FAF18;
    if (second < 0) {
        first++;
        second += 0x80;
    }
    int value = first - third;
    if (value < 0) {
        value = -value;
        int count = value / 0xC;
        while (value >= 0xC) {
            result *= lbl_805FAE80;
            value -= 0xC;
            count--;
        }
        while (value > 0) {
            result *= lbl_805FAE88;
            value--;
        }
    } else {
        int count = value / 0xC;
        while (value >= 0xC) {
            result *= lbl_805FAF20;
            value -= 0xC;
            count--;
        }
        while (value > 0) {
            result /= lbl_805FAE88;
            value--;
        }
    }
    int steps = (second - 1) >> 4;
    while (second > 0x10) {
        result *= lbl_805FAE90;
        second -= 0x10;
        steps--;
    }
    while (second > 0) {
        result *= lbl_805FAE98;
        second--;
    }
    return result;
}

int fn_8043D49C(void) {
    int count = 0;
    if (lbl_805F1404 != NULL) {
        for (int i = 0; i < 64; i++) {
            if (*(int *)(lbl_805F1404 + i * 0x10C + 0x54) != 0) {
                count++;
            }
        }
    }
    return count;
}
}

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
extern double lbl_805FAE80;
extern double lbl_805FAE88;
extern double lbl_805FAE90;
extern double lbl_805FAE98;
extern float lbl_805FAEA0;
extern double lbl_805FAF10;
extern double lbl_805FAF18;
extern double lbl_805FAF20;

struct RwGlobalFlags {
    static const unsigned kGainTrimOff = 0x2C;
    static const unsigned kFlagAOff = 0xFDFA;
    static const unsigned kFlagBOff = 0xFDFB;
};

struct RwCpp02BankEntry {
    u8 *table;
    u32 next;
};

struct RwSoundChannel {
    int field000;
    int field004;
    int field008;
    int field00C;
    float field010;
    int field014;
    int field018;
    int field01C;
    int field020;
    int field024;
    int field028;
    float field02C;
    float field030;
    void *field034;
    char reserved038[0x4C];
    short s84;
    short pad86;
    short s88;
    short pad8A;
    short s8C;
    short pad8E;
    short s90;
    short pad92;
    short s94;
    short pad96;
    short s98;
    short pad9A;
    short s9C;
    short pad9E;
    short padA0;
    short padA2;
    short sA4;
    short padA6;
    int padA8;
    int padAC;
    float field0B0;
    char reserved0B4[8];
    int padBC;
    char reserved0C0[0x1C];
    int field0DC;
    int field0E0;
    char reserved0E4[0x18];
    int padFC;
    int field100;
    int pad104;
    void *field108;
};

struct RwPatchHeader {
    int field000;
    int field004;
    float field008;
    float field00C;
    char reserved010[0x2C];
    float field03C;
};

struct RwPatchSlot {
    char reserved000[0xB4];
    int field0B4;
    int field0B8;
    int field0BC;
    int padC0;
    int field0C4;
    int field0C8;
    int padCC;
    int padD0;
    float field0D4;
    float field0D8;
};

extern float lbl_805FAEA4;
extern float lbl_805FAEA8;
extern float lbl_805FAEAC;
extern float lbl_805FAEB0;
extern float lbl_805FAEB4;
extern float lbl_805FAEB8;
extern float lbl_805FAEBC;
extern float lbl_805FAEC0;
extern float lbl_805FAEC4;
extern float lbl_805FAEC8;
extern float lbl_805FAECC;
extern float lbl_805FAED0;
extern float lbl_805FAED4;
extern float lbl_805FAED8;

extern "C" {
unsigned char lbl_8056D098[];
float fn_8043B3A0(float);
void fn_803863A4(void *, const void *);
void fn_8043B44C(void **, void (*)(void *));
void **fn_8043B3F4(void **, int (*)(void *));
void fn_8043B4B0(void *, void **, void (*)(void *));
int fn_8043B810(void *);
void fn_8043B89C(void *);
void *fn_8043B560(u32, void **, void (*)(void *));
void *fn_8043B3DC(void *, void *);
void fn_80386348(u32, u32);
void fn_8038A338(u32);
void fn_8043CA7C(u32, u32, u32, u32, u32, int, u32, u32);
void fn_80386844(u32, float);
void fn_8043C084(void *);

void fn_8043C2A4(RwSoundChannel *self, RwPatchHeader *arg2) {
    if ((arg2->field000 & 1) == 0) return;

    const unsigned char *tbl = lbl_8056D098;
    const unsigned char *selPtr = (const unsigned char *)self->field108;
    unsigned slotOffset = selPtr[2] * 0x48;
    char *slot = (char *)arg2 + slotOffset;
    RwPatchSlot *e = (RwPatchSlot *)slot;

    self->s84 = 0;
    self->s88 = 0;
    self->s8C = 0;
    self->s90 = 0;
    self->s94 = 0;
    self->s98 = 0;
    self->s9C = 0;
    self->padA0 = 0;
    self->sA4 = 0;
    self->pad86 = 0;
    self->pad8A = 0;
    self->pad8E = 0;
    self->pad92 = 0;
    self->pad96 = 0;
    self->pad9A = 0;
    self->pad9E = 0;
    self->padA2 = 0;
    self->padA6 = 0;

    int prod = (e->field0B4 + 1) * (e->field0B8 + 1);
    int q;
    {
        int t = prod - 1;
        int s = t / 0x80;
        if (s < 0) s = 0;
        if (s > 0x7F) s = 0x7F;
        q = s + 1;
    }
    int prod2 = q * self->field100 * (0x100 - self->field028);
    int idx = (int)(lbl_805FAEA4 * arg2->field008) + (((prod2 - 1) >> 14) & 0xFF);
    if (idx < 0) idx = 0;
    if (idx > 0xFF) idx = 0xFF;
    unsigned char lutVal = tbl[idx];
    int mask = lutVal - 3;
    if (mask > 0xFF) mask = 0xFF;

    int iv1 = self->field020 << 2;
    if (iv1 < 0) iv1 = 0;
    if (iv1 > 0x3F) iv1 = 0x3F;
    int lut1 = tbl[0x1A0 + iv1];
    float dbSum = lbl_805FAE78;
    if (lut1 == 0) {
        dbSum += lbl_805FAEA8;
    } else {
        dbSum += (float)(-(15 - lut1) * 3);
    }
    dbSum += *(const float *)(lbl_805F1404 + RwGlobalFlags::kGainTrimOff);
    if (mask & 0x01) dbSum += lbl_805FAEAC;
    if (mask & 0x02) dbSum += lbl_805FAEB0;
    if (mask & 0x04) dbSum += lbl_805FAEB4;
    if (mask & 0x08) dbSum += lbl_805FAEB8;
    if (mask & 0x10) dbSum += lbl_805FAEBC;
    if (mask & 0x20) dbSum += lbl_805FAEC0;
    if (mask & 0x40) dbSum += lbl_805FAEC4;
    if (mask & 0x80) dbSum += lbl_805FAEC8;
    dbSum += self->field0B0;

    const float gain = fn_8043B3A0(dbSum);

    int pan = self->field024 + e->field0BC - 0x40;
    if (pan < 0) pan = 0;
    if (pan > 0x7F) pan = 0x7F;
    unsigned bv = tbl[0x120 + pan];
    const float *pairTbl = (const float *)(tbl + 0x1E0);
    float f31 = pairTbl[bv ^ 0x10];
    float f30 = pairTbl[bv];

    if (lbl_805F1404[RwGlobalFlags::kFlagAOff] != 0) {
        float merged = lbl_805FAECC * (f30 + f31);
        if (merged > lbl_805FAE74) merged = lbl_805FAE74;
        f31 = merged;
        f30 = merged;
    }

    self->field02C = f30;
    f30 *= gain;
    self->field030 = f31;
    f31 *= gain;
    *(float *)(slot + 0xD8) = f30;
    *(float *)(slot + 0xD4) = f31;

    if (lbl_805F1404[RwGlobalFlags::kFlagBOff] == 0) {
        float scale = lbl_805FAED0;
        self->s84 = (short)(int)((self->field010 * *(float *)(slot + 0xD8)) * scale);
        self->s88 = (short)(int)((self->field010 * *(float *)(slot + 0xD4)) * scale);
        self->s94 = 0;
        self->s98 = 0;
    } else {
        float scale = lbl_805FAED0;
        int c8 = e->field0C8;
        float x1, x2;
        if ((float)c8 < lbl_805FAE78) {
            x1 = self->field010 * *(float *)(slot + 0xD8);
            x2 = self->field010 * *(float *)(slot + 0xD4);
        } else {
            float scaleGain = self->field010 * gain;
            const float *curveTbl = (const float *)(tbl + 0x460);
            x1 = curveTbl[0x7F - c8] * scaleGain;
            x2 = curveTbl[c8] * scaleGain;
        }
        if (e->field0C4 <= 0x40) {
            int scaled1 = (int)(scale * x1);
            int scaled2 = (int)(scale * x2);
            self->s84 = (short)scaled1;
            self->s88 = (short)scaled2;
            self->s94 = 0;
            self->s98 = 0;
        } else {
            int scaled1 = (int)(scale * x1);
            int scaled2 = (int)(scale * x2);
            self->s84 = 0;
            self->s88 = 0;
            self->s94 = (short)scaled1;
            self->s98 = (short)scaled2;
        }
    }

    float k2 = lbl_805FAED4;
    if (self->field0DC == 0) k2 = lbl_805FAE74;
    int iv2 = self->field0E0 << 2;
    if (iv2 < 0) iv2 = 0;
    if (iv2 > 0x3F) iv2 = 0x3F;
    int lut2 = tbl[0x1A0 + iv2];
    float db2 = lbl_805FAE78;
    if (lut2 == 0) {
        db2 += lbl_805FAEA8;
    } else {
        db2 += (float)(-(15 - lut2) * 3);
    }
    float gain2 = fn_8043B3A0(db2);
    float scale = lbl_805FAED0;
    float p = k2 * gain2 + arg2->field03C;
    if (p < lbl_805FAE78) p = lbl_805FAE78;
    if (p > lbl_805FAE74) p = lbl_805FAE74;

    float f4 = f30 * p;
    float f5 = f31 * p;
    float wet1 = scale * f4;
    float wet2 = scale * f5;

    self->s8C = (short)(int)wet1;
    self->s90 = (short)(int)wet2;
    if (lbl_805F1404[RwGlobalFlags::kFlagBOff] == 0) {
        self->s8C = (short)(int)(scale * f4);
        self->s90 = (short)(int)(scale * f5);
        self->sA4 = 0;
        self->s9C = 0;
    } else {
        float ramp = (lbl_805FAED8 - (float)e->field0C4) / lbl_805FAED8;
        float ramp1 = scale * (ramp * f4);
        float ramp2 = scale * (ramp * f5);
        self->s8C = (short)(int)ramp1;
        self->s90 = (short)(int)ramp2;
        self->sA4 = (short)(int)(scale * (ramp * f4));
        self->sA4 = (short)(int)(scale * (ramp * f5));
    }

    fn_803863A4(self->field034, (char *)self + 0x84);
}

void fn_8043C170(u32 *object, int value, float input) {
    int wide;
    if (value >= 0) {
        wide = value * (int)object[6];
    } else {
        wide = value * (int)object[7];
    }

    int magnitude = wide;
    int negative = 0;
    float result = lbl_805FAE74;
    if (wide < 0) {
        negative = 1;
        magnitude = -wide;
    }

    int rounded = magnitude / 0x2000;
    u32 shifted = ((u32)magnitude & 0x1FFF) * 0x80000;
    u32 quotients = (u32)rounded / 0xC;
    u32 sign = (u32)magnitude >> 0x1F;
    double factor = lbl_805FAE80;
    shifted -= sign;
    u32 folded = __rlwinm(shifted, 13, 0, 31);
    folded += sign;
    int remainder = (int)(folded << 7) / 0x2000;

    while (rounded >= 0xC) {
        result = (float)(result * factor);
        rounded -= 0xC;
        quotients--;
    }
    while (rounded > 0) {
        result = (float)(result * lbl_805FAE88);
        rounded--;
    }
    u32 steps = (u32)(remainder - 1) >> 4;
    while (remainder > 0x10) {
        result = (float)(result * lbl_805FAE90);
        remainder -= 0x10;
        steps--;
    }
    while (remainder > 0) {
        result = (float)(result * lbl_805FAE98);
        remainder--;
    }
    if (negative != 0) {
        result = lbl_805FAE74 / result;
    }

    float output = result * *(float *)(object + 5);
    output /= lbl_805FAEA0;
    output *= input;
    fn_80386844(object[0xD], output);
}

void *fn_8043C12C(void **list, void *data) {
    u32 *object = (u32 *)fn_8043B560(8, list, NULL);
    if (object != NULL) {
        object[1] = (u32)data;
    }
    return object;
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
    u8 *global = lbl_805F1404;
    int index = 0;
    for (int i = 0; i < 64; i++) {
        int current = *(int *)(global + 0x4C) + index;
        u32 shifted = (u32)(current * 0x4000000);
        current = (int)((u32)current >> 0x1F);
        shifted -= (u32)current;
        shifted = __rlwinm(shifted, 6, 0, 31);
        int offset;
        int rotated = (int)shifted + current;
        offset = rotated * 0x14;
        u8 *entry = global + offset;
        if (entry[0x4354] != 0 && *(int *)(entry + 0x435C) <= param) {
            fn_8043BB54((RwCpp02Owner *)(global + offset + 0x4354));
            *(int *)(lbl_805F1404 + 0x4C) = rotated + 1;
            return lbl_805F1404 + offset + 0x4354;
        }
        index++;
    }
    return NULL;
}

static inline void RwCpp02AllocateCounted(int param, int count, u32 &result) {
    int remaining = count;
    int rotated;
    int offset;
    u8 *entry;
    int used;
    while (remaining > 0) {
        int i = 0;
        u8 *global = lbl_805F1404;
        for (; i < 64; i++) {
            int current = *(int *)(global + 0x4C) + i;
            u32 shifted = (u32)(current * 0x4000000);
            current = (int)((u32)current >> 0x1F);
            shifted -= (u32)current;
            shifted = __rlwinm(shifted, 6, 0, 31);
            rotated = (int)shifted + current;
            offset = rotated * 0x14;
            u8 *check = global + offset;
            if (check[0x4354] != 0 && *(int *)(check + 0x435C) <= param) {
                used = 0;
                int entryOffset = offset + 0x4354;
                entry = global + entryOffset;
                if (entry != NULL) {
                    void *inner = NULL;
                    while ((inner = fn_8043B3DC(entry + 0x10, inner)) != NULL) {
                        if (*(u32 *)((u8 *)inner + 4) != 0) {
                            used++;
                        }
                    }
                }
                remaining -= used;
                fn_8043BB54((RwCpp02Owner *)(lbl_805F1404 + offset + 0x4354));
                *(int *)(lbl_805F1404 + 0x4C) = rotated + 1;
                result = (u32)(lbl_805F1404 + offset + 0x4354);
                break;
            }
        }
        if (remaining == count) {
            break;
        }
    }
}

void *fn_8043BBC8(int param, int count) {
    u32 result = 0;
    if (count <= 0) {
        return fn_8043BCDC(param);
    }
    RwCpp02AllocateCounted(param, count, result);
    return (void *)result;
}

void fn_8043BD80(u32 *object) {
    if (object == NULL) {
        return;
    }
    if (object[0xD] == 0) {
        return;
    }
    if ((*object & 0x80000000) != 0 && (*object & 1) != 0 && (*object & 0x40000000) == 0) {
        fn_8043CA7C(object[0x39], (u32)object, object[0x3A], object[0x3B], object[0x3C],
                    object[0x3D], object[0x3E], object[0x3F]);
        *object &= 0xFFFFFFFE;
    }
}

void fn_8043BE04(u32 *object) {
    if (object == NULL) {
        return;
    }
    if (object[0xD] == 0) {
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
    if ((*object & 1) != 0) {
        return;
    }

    int state = (int)object[0x36];
    float scaled;
    switch (state) {
    case 0: {
        float rate = lbl_805FAE70;
        float value = *(float *)(object + 0x2B);
        value += *(float *)(object + 0x32);
        *(float *)(object + 0x2B) = value;
        value = *(float *)(object + 0x2B);
        scaled = -(rate - value * rate);
        *(float *)(object + 0x2C) = scaled;
        if (*(float *)(object + 0x2B) >= lbl_805FAE74) {
            *(float *)(object + 0x2B) = lbl_805FAE74;
            scaled = -(rate - lbl_805FAE74 * rate);
            *(float *)(object + 0x2C) = scaled;
            object[0x36]++;
        }
        break;
    }
    case 1:
        if (*(float *)(object + 0x2C) <= *(float *)(object + 0x31)) {
            object[0x36]++;
        } else {
            float rate = lbl_805FAE70;
            float value = *(float *)(object + 0x2B);
            value -= *(float *)(object + 0x33);
            *(float *)(object + 0x2B) = value;
            value = *(float *)(object + 0x2B);
            scaled = -(rate - value * rate);
            *(float *)(object + 0x2C) = scaled;
            if (*(float *)(object + 0x2B) <= lbl_805FAE78) {
                *(float *)(object + 0x2B) = lbl_805FAE78;
                scaled = -(rate - lbl_805FAE78 * rate);
                *(float *)(object + 0x2C) = scaled;
            }
            if (!(*(float *)(object + 0x2C) <= *(float *)(object + 0x31))) {
                goto done;
            }
            object[0x36]++;
        }
    case 2:
        object[0x36]++;
    case 3: {
        float rate = lbl_805FAE70;
        float value = *(float *)(object + 0x2B);
        value -= *(float *)(object + 0x34);
        *(float *)(object + 0x2B) = value;
        value = *(float *)(object + 0x2B);
        scaled = -(rate - value * rate);
        *(float *)(object + 0x2C) = scaled;
        if (*(float *)(object + 0x2B) <= lbl_805FAE78) {
            *(float *)(object + 0x2B) = lbl_805FAE78;
            scaled = -(rate - lbl_805FAE78 * rate);
            *(float *)(object + 0x2C) = scaled;
        }
        break;
    }
    case 4:
        object[0x36]++;
    case 5: {
        float rate = lbl_805FAE70;
        float value = *(float *)(object + 0x2B);
        value -= *(float *)(object + 0x35);
        *(float *)(object + 0x2B) = value;
        value = *(float *)(object + 0x2B);
        scaled = -(rate - value * rate);
        *(float *)(object + 0x2C) = scaled;
        if (*(float *)(object + 0x2B) <= lbl_805FAE78) {
            *(float *)(object + 0x2B) = lbl_805FAE78;
            scaled = -(rate - lbl_805FAE78 * rate);
            *(float *)(object + 0x2C) = scaled;
        }
        break;
    }
    default:
        break;
    }

done:
    *(float *)(object + 0x2A) += lbl_805FAE7C;
}

static inline void RwCpp02CountGroups(u8 *table, u32 group, int outerCount, int value, int &count) {
    u8 *items = table + __rlwinm(group, 0, 8, 31);
    int offset = 0;
    int i = 0;
    while (i < outerCount) {
        u32 item = *(u32 *)((u8 *)items + offset);
        int childCount = (int)(item >> 0x18);
        int childOffset = 0;
        for (int j = 0; j < childCount; j++) {
            u8 *entry = table + (item & 0xFFFFFF) + childOffset;
            if ((int)(u8)entry[8] <= value && value <= (int)(u8)entry[9]) {
                count++;
            }
            childOffset += 0x40;
        }
        i++;
        offset += 0x10;
    }
}

int fn_8043C9D0(int bank, u32 index, int value) {
    u8 *global = lbl_805F1404;
    int count = 0;
    u8 *table = ((RwCpp02BankEntry *)(global + 0x5378))[bank].table;
    if (table != NULL) {
        u32 header = *(u32 *)(table + 8);
        if ((header >> 0x18) > index) {
            u32 *groupBase = (u32 *)(table + __rlwinm(header, 0, 8, 31));
            u32 group = groupBase[index];
            int outerCount = (int)(group >> 0x18);
            RwCpp02CountGroups(table, group, outerCount, value, count);
        }
    }
    return count;
}

static inline void RwCpp02WalkGroups(u8 *table, u32 group, u8 *firstBase, void *param, int bank,
                                     int value, u32 data, u32 &cursor) {
    int i = 0;
    int outerCount = (int)(group >> 0x18);
    u8 *items = table + __rlwinm(group, 0, 8, 31);
    int offset = 0;
    while (i < outerCount) {
        u32 *groupEntry = (u32 *)(items + offset);
        int j = 0;
        u32 item = *groupEntry;
        int childOffset = 0;
        u32 itemOffset = __rlwinm(item, 0, 8, 31);
        int childCount = (int)(item >> 0x18);
        u8 *itemBase = table + itemOffset;
        while (j < childCount) {
            u8 *entry = itemBase + childOffset;
            if ((int)(u8)entry[8] <= value && value <= (int)(u8)entry[9]) {
                cursor = (u32)fn_8043B3DC(param, (void *)cursor);
                if (cursor != 0 && *(u32 *)(cursor + 4) != 0) {
                    fn_8043CA7C(bank, *(u32 *)(cursor + 4), (u32)groupEntry,
                                (u32)table, (u32)entry, value, (u32)firstBase, data);
                }
            }
            j++;
            childOffset += 0x40;
        }
        i++;
        offset += 0x10;
    }
}

void fn_8043C89C(void *param, int bank, u32 index, int value, u32 data) {
    u32 cursor = 0;
    u8 *global = lbl_805F1404;
    u8 *table = ((RwCpp02BankEntry *)(global + 0x5378))[bank].table;
    if (table != NULL) {
        u32 header = *(u32 *)(table + 8);
        if ((header >> 0x18) > index) {
            u32 headerOffset = __rlwinm(header, 0, 8, 31);
            u32 first = *(u32 *)(table + 4);
            u32 *groupBase = (u32 *)(table + headerOffset);
            u32 group = groupBase[index];
            u8 *firstBase = table + __rlwinm(first, 0, 8, 31);
            RwCpp02WalkGroups(table, group, firstBase, param, bank, value, data, cursor);
        }
    }
}

u32 fn_8043D20C(int value, float factor) {
    float scale = lbl_805FAE74;
    int whole;
    int negative = 0;
    if (value < 0) {
        negative = 1;
        value = -value;
    }
    whole = value / 0x1FFF;
    int remainder = ((value % 0x1FFF) << 7) / 0x1FFE;
    int count = whole / 0xC;
    while (whole >= 0xC) {
        scale *= lbl_805FAE80;
        whole -= 0xC;
    }
    while (whole > 0) {
        scale *= lbl_805FAE88;
        whole--;
    }
    int steps = (remainder - 1) >> 4;
    while (remainder > 0x10) {
        scale *= lbl_805FAE90;
        remainder -= 0x10;
        steps--;
    }
    while (remainder > 0) {
        scale *= lbl_805FAE98;
        remainder--;
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
    double result = lbl_805FAF10;
    if (value > 0) {
        int count = (int)((u32)value >> 8);
        while (value >= 0x100) {
            result *= lbl_805FAE88;
            value -= 0x100;
            count--;
        }
        count = (int)((u32)(value + 1) >> 1);
        while (value > 0) {
            result *= lbl_805FAE98;
            value -= 2;
            count--;
        }
    }
    if (value < 0) {
        int count = (int)((u32)-value >> 8);
        while (value <= -0x100) {
            result /= lbl_805FAE88;
            value += 0x100;
            count--;
        }
        count = (int)((u32)(1 - value) >> 1);
        while (value < 0) {
            result /= lbl_805FAE98;
            value += 2;
            count--;
        }
    }
    return (float)result;
}

float fn_8043D3BC(int first, int second, int third) {
    double result = lbl_805FAF18;
    if (second < 0) {
        first++;
        second += 0x80;
    }
    int value = first - third;
    if (value >= 0) {
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
    } else {
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
    return (float)result;
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

u32 *fn_8043D4D8(u32, u32, u32);

static inline u8 *RwCpp02FindFreeEntry(u8 *global) {
    int rotated;
    for (int i = 0; i < 64; i++) {
        int current = *(int *)(global + 0x4C) + i;
        u32 shifted = (u32)(current * 0x4000000);
        current = (int)((u32)current >> 0x1F);
        shifted -= (u32)current;
        shifted = __rlwinm(shifted, 6, 0, 31);
        rotated = (int)shifted + current;
        int offset = rotated * 0x14;
        if (*(u8 *)(global + offset + 0x4354) == 0) {
            *(int *)(global + 0x4C) = rotated + 1;
            return lbl_805F1404 + offset + 0x4354;
        }
    }
    return NULL;
}

u8 *fn_8043B954(u8 *bank, int index, int value, int data, int param) {
    u32 entry = 0;
    u8 *slot = bank + index * 0x48;
    u32 bankIndex = *(u32 *)(slot + 0xAC);
    u32 soundIndex = *(u32 *)(slot + 0xB0);
    int count = fn_8043C9D0(bankIndex, soundIndex, value);
    if (count <= 0) {
        return NULL;
    }

    int active = fn_8043D49C();
    u8 *global = lbl_805F1404;
    int available = count - (*(int *)(global + 0x38) - active);
    if (available < 0) {
        available = 0;
    }

    if (available != 0) {
        entry = (u32)fn_8043BBC8(param, available);
        if (entry == NULL) {
            return NULL;
        }
    } else {
        entry = (u32)RwCpp02FindFreeEntry(global);
        if (entry == NULL) {
            entry = (u32)fn_8043BCDC(param);
            if (entry == NULL) {
                return NULL;
            }
        }
    }

    int i = 0;
    *(u8 **)((u8 *)entry + 0xC) = bank;
    ((u8 *)entry)[3] = (u8)value;
    ((u8 *)entry)[2] = (u8)index;
    ((u8 *)entry)[4] = (u8)data;
    *(int *)((u8 *)entry + 8) = param;
    ((u8 *)entry)[1] = (u8)count;
    ((u8 *)entry)[0] = 1;
    *(u32 *)((u8 *)entry + 0x10) = 0;

    while (i < count) {
        u32 *object = fn_8043D4D8(entry, 5, 0);
        if (object == NULL) {
            fn_8043BB54((RwCpp02Owner *)entry);
            return NULL;
        }
        if (fn_8043C12C((void **)(entry + 0x10), object) == NULL) {
            fn_8043BB54((RwCpp02Owner *)entry);
            return NULL;
        }
        i++;
    }

    fn_8043C89C((void *)(entry + 0x10), bankIndex, soundIndex, value, data);
    return (u8 *)entry;
}
}

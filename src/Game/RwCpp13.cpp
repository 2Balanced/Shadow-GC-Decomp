/* RenderWare vertex conversion routines. The object and stream layouts are inferred from the
 * callers; the functions retain their address names until their interfaces are known. */
#include <dolphin/types.h>
#include <string.h>

extern u32 lbl_805FB0C0;
extern u8 lbl_805FB0C4;
extern u32 lbl_805FB0C8;
extern u8 lbl_805FB0CC;
extern u32 lbl_805FB0B8;
extern u8 lbl_805FB0BC;
extern u32 lbl_8056D708[];
extern u32 lbl_805EDDD8;
extern u32 lbl_805F1520;
extern u8 *lbl_805F15B4;
extern u32 lbl_805F1658;
extern float lbl_805FB0D0;
extern u16 lbl_805EDF40;

extern "C" {
void fn_80482458(void *, const void *, const void *);
void fn_80482504(void *, const void *, const void *);
void fn_80451080(void *, void *, void *);
void fn_804513C4(void *, void *, void *);
void fn_804517C8(void *, void *, void *);
void DCFlushRange(void *, u32);
void fn_80394F2C(void);
u32 fn_8044CCAC(u8 *);
void *fn_8044F9E4(void *, void *, void *);
void *fn_8046C0D4(void *, void *, void *);
void *fn_80450938(void *, void *, void *);
void *RwFrameGetLTM(void *);
u32 fn_8044CF68(u32 *, u32 *, u32 *, u32 **);
void fn_8044D344(u8 *, u32 *, const void *, u32 *);
void fn_8044D554(u32 *, u8 *, void *, u8 *, u8 *, u32);
void fn_80463840(void *, void *);
u32 fn_803991E8(void *);
u32 fn_80399238(void *, void *);
void *fn_8044E408(u32 *, u32);
u32 fn_8044BDBC(u32 *);
void fn_8046288C(void);
u32 fn_8047CEF0(void *, u32, u32, u32, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);
u32 fn_8044BB18(void *, void *);
s32 fn_8047CF50(void *, u32, u32 *, u32 *);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
u32 fn_8047D868(void *, void *, u32);
u32 fn_80480D4C(void *, void *, u32);
void *fn_8044BBFC(void *, u32 *);
void *fn_8044CCC0(void *, void *);
void fn_80462C30(void *, void *);
void fn_804950AC(void *, u32);
void fn_80394500(u32, u32);
void fn_80462E1C(void *);
u32 fn_804655D4(u32, u32, u32, u32);
void fn_80499C38(void *, u32);
void fn_80497778(u32);
void fn_80399000(void *, void *);
}

extern "C" {
static asm u32 RwCpp13ReadGqr5(void) {
    nofralloc
    mfspr r3, 917
    blr
}

static asm void RwCpp13WriteGqr5(register u32 value) {
    nofralloc
    mtspr 917, value
    blr
}

static asm void RwCpp13WriteGqr6(register u32 value) {
    nofralloc
    mtspr 918, value
    blr
}

static asm void RwCpp13WriteGqr7(register u32 value) {
    nofralloc
    mtspr 919, value
    blr
}
}

extern "C" void fn_8044D6E8(u32 *object, u8 *matrices, void *arg3, void *arg4, u8 *outA,
                            u8 *outB, u8 *format, u32 count) {
    u8 local[0x80];
    *(u32 *)(local + 0x24) = (u32)outA;
    *(u32 *)(local + 0x28) = (u32)outB;
    *(u32 *)(local + 0x2C) = (u32)arg3;
    *(u32 *)(local + 0x30) = (u32)arg4;
    *(u32 *)(local + 0x40) = count;

    u32 gqr6 = 0;
    u32 gqr7 = 0;
    if (format != NULL) {
        *(u32 *)(local + 0x14) = lbl_805FB0C0;
        *(u8 *)(local + 0x18) = lbl_805FB0C4;
        *(u32 *)(local + 0xC) = lbl_805FB0C8;
        *(u8 *)(local + 0x10) = lbl_805FB0CC;
        *(u32 *)(local + 0x1C) = lbl_805FB0B8;
        *(u8 *)(local + 0x20) = lbl_805FB0BC;

        u32 indexA = format[0];
        u32 indexB = format[1];
        u32 valueA = format[0xC];
        u32 valueB = format[0xD];
        *(u32 *)(local + 0x34) = *(u8 *)(local + 0x14 + indexA);
        *(u32 *)(local + 0x38) = *(u8 *)(local + 0x14 + indexB);

        gqr6 = *(u8 *)(local + 0x1C + indexA);
        gqr6 = (gqr6 & ~0xFF00) | ((valueA & 0xFF) << 8);
        gqr6 = ((gqr6 & 0xFFFF) << 16) | (gqr6 & 0xFFFF);
        gqr7 = *(u8 *)(local + 0x1C + indexB);
        gqr7 = (gqr7 & ~0xFF00) | ((valueB & 0xFF) << 8);
        gqr7 = ((gqr7 & 0xFFFF) << 16) | (gqr7 & 0xFFFF);
        *(u32 *)(local + 0x3C) = valueB != 0 ? *(u32 *)(local + 0x38) * 6 : 0;
    } else {
        *(u32 *)(local + 0x34) = 4;
        *(u32 *)(local + 0x38) = 4;
        *(u32 *)(local + 0x3C) = 0;
    }

    u32 savedGqr5 = RwCpp13ReadGqr5();
    RwCpp13WriteGqr5(0x07040000);
    RwCpp13WriteGqr6(gqr6);
    RwCpp13WriteGqr7(gqr7);

    switch (object[4]) {
    case 1:
        for (u32 i = 0; i < count; i++) {
            u32 matrixIndex = *(u32 *)(object[5] + i * 4) & 0xFF;
            u8 *matrix = matrices + matrixIndex * 0x40;
            fn_80482458(outA + i * 0xC, (u8 *)arg3 + i * 0xC, matrix);
            fn_80482504(outB + i * 0xC, (u8 *)arg4 + i * 0xC, matrix);
        }
        break;
    case 2:
        fn_80451080(matrices, object, local + 0x24);
        break;
    case 3:
        fn_804513C4(matrices, object, local + 0x24);
        break;
    case 4:
        fn_804517C8(matrices, object, local + 0x24);
        break;
    default:
        break;
    }

    RwCpp13WriteGqr5(savedGqr5);
    u32 strideA = *(u32 *)(local + 0x34);
    u32 strideB = *(u32 *)(local + 0x38);
    u32 extra = *(u32 *)(local + 0x3C);
    u32 bytes = ((count * (strideA * 3) + 0x1F) & ~0x1F) + count * (extra + strideB * 3);
    DCFlushRange(outA, bytes);
    fn_80394F2C();
}

extern "C" void *fn_8044D924(u32 *object, u32 *out) {
    u32 *owner = (u32 *)object[6];
    u32 *record = (u32 *)fn_8044CCAC((u8 *)owner);
    void *arg2;
    void *arg3;
    if (owner[6] == 1) {
        arg2 = owner;
        arg3 = (u8 *)owner + 0x58;
    } else {
        arg2 = object;
        arg3 = (u8 *)object + 0x14;
    }

    void *result;
    if (record[4] > 1 || record[11] == 3) {
        if ((owner[2] & 0x02000000) != 0 && lbl_805EDDD8 == 1) {
            result = fn_8044F9E4(owner, arg2, arg3);
        } else {
            result = fn_8046C0D4(owner, arg2, arg3);
        }
    } else if ((owner[2] & 0x02000000) != 0 && lbl_805EDDD8 == 1) {
        result = fn_8044F9E4(owner, arg2, arg3);
    } else {
        result = fn_80450938(owner, arg2, arg3);
    }
    *out = (u32)result;
    *(u16 *)((u8 *)owner + 0xC) = 0;
    return object;
}

extern "C" void *fn_8044DA3C(u32 *object, u32 *arg2) {
    u32 *owner = (u32 *)object[6];
    u32 *record = *(u32 **)((u8 *)owner + lbl_8056D708[2]);
    if (record[4] > 1 || record[11] == 3) {
        u32 *node = (u32 *)*arg2;
        u32 *data = (u32 *)node[3];
        u32 *state = (u32 *)((u8 *)node + 0x18);
        u32 *first;
        u32 *second;
        if (((u8 *)data)[0] == 1) {
            u32 *entry = (u32 *)((u8 *)data + lbl_8056D708[1]);
            first = entry + 1;
            second = entry + 2;
        } else {
            first = record + 7;
            second = record + 8;
        }

        if (*first == 0) {
            if ((owner[2] & 0x01000000) == 0) fn_80463840(object, arg2);
            *first = state[3];
            state[3] = 0;
            if ((owner[2] & 0x10) != 0) {
                *second = state[5];
                state[5] = 0;
            }
        } else if ((owner[2] & 0x01000000) == 0) {
            u32 savedFirst = state[3];
            state[3] = *first;
            if ((owner[2] & 0x10) != 0) {
                u32 savedSecond = state[5];
                state[5] = *second;
                fn_80463840(object, arg2);
                state[5] = savedSecond;
            } else {
                fn_80463840(object, arg2);
            }
            state[3] = savedFirst;
        }

        if (fn_8044CF68(record, object, state, (u32 **)arg2) == 0) return NULL;
        void *frameMatrix = RwFrameGetLTM((void *)object[1]);
        fn_8044D344((u8 *)lbl_8056D708[3], record, frameMatrix,
                    *(u32 **)((u8 *)object + lbl_8056D708[1]));
        if ((owner[2] & 0x10) != 0) {
            fn_8044D6E8(record, (u8 *)lbl_8056D708[3], (void *)*first, (void *)*second,
                        (u8 *)state[3], (u8 *)state[5],
                        *(u8 **)((u8 *)owner + lbl_805F1520), owner[5]);
        } else {
            fn_8044D554(record, (u8 *)lbl_8056D708[3], (void *)*first, (u8 *)state[3],
                        *(u8 **)((u8 *)owner + lbl_805F1520), owner[5]);
        }
    } else {
        void *frameMatrix = RwFrameGetLTM((void *)object[1]);
        fn_8044D344((u8 *)lbl_8056D708[3], record, frameMatrix,
                    *(u32 **)((u8 *)object + lbl_8056D708[1]));
    }
    return object;
}

extern "C" u32 fn_8044DC4C(const float *input, void *out, int transform) {
    float matrix[12];
    matrix[0] = -input[0];
    matrix[1] = -input[4];
    matrix[2] = -input[8];
    matrix[3] = -input[12];
    matrix[4] = input[1];
    matrix[5] = input[5];
    matrix[6] = input[9];
    matrix[7] = input[13];
    matrix[8] = -input[2];
    matrix[9] = -input[6];
    matrix[10] = -input[10];
    matrix[11] = -input[14];
    u32 result = fn_803991E8(matrix);
    if (transform != 0) return fn_80399238(matrix, out);
    return result;
}

extern "C" void fn_8044DD1C(u32 *object, u32 index, int transform) {
    u8 *table = (u8 *)object[0x10];
    u32 pair = index * 2;
    u32 first = table[pair];
    u32 outerCount = table[pair + 1];
    u32 outputIndex = 0;
    u32 tableOffset = (first & 0x7F) * 2;

    for (u32 outer = 0; (outer & 0xFF) < outerCount; outer++) {
        u8 *entry = (u8 *)object[0x11] + (outer & 0xFF) * 2 + tableOffset;
        u32 matrixIndex = entry[0];
        u32 innerCount = entry[1];
        u32 matrixOffset = matrixIndex * 0x40;
        for (u32 inner = 0; inner < innerCount; inner++) {
            const float *input = (const float *)(lbl_8056D708[3] + matrixOffset);
            float matrix[12];
            matrix[0] = -input[0];
            matrix[1] = -input[4];
            matrix[2] = -input[8];
            matrix[3] = -input[12];
            matrix[4] = input[1];
            matrix[5] = input[5];
            matrix[6] = input[9];
            matrix[7] = input[13];
            matrix[8] = -input[2];
            matrix[9] = -input[6];
            matrix[10] = -input[10];
            matrix[11] = -input[14];
            fn_803991E8(matrix);
            if (transform != 0) fn_80399238(matrix, (void *)outputIndex);
            outputIndex += 3;
            matrixOffset += 0x40;
        }
    }
}

extern "C" void *fn_8044E6BC(u32 *object) {
    u32 *record = *(u32 **)((u8 *)object + lbl_8056D708[2]);
    if (record == NULL) return object;
    u32 flags = object[2];
    if ((flags & 0x01000000) != 0) {
        if ((flags & 0x10) != 0) {
            *(u16 *)((u8 *)object + 0xC) |= 6;
        } else {
            *(u16 *)((u8 *)object + 0xC) |= 2;
        }
        record[7] = 0;
        record[8] = 0;
        return object;
    }
    return fn_8044E408(record, object[5]) != NULL ? object : NULL;
}

extern "C" void *fn_8044E758(u32 *object) {
    u32 *record = *(u32 **)((u8 *)object + lbl_8056D708[2]);
    if (record[9] != 0) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)record[9]);
        record[9] = 0;
        record[10] = 0;
    }
    record[7] = 0;
    record[8] = 0;
    return object;
}

extern "C" u32 fn_8044E7D0(u32 *object) {
    u32 *record = *(u32 **)((u8 *)object + lbl_8056D708[2]);
    u32 result = record[1] + 0x14;
    if (record[4] > 1) result += object[5] * (record[4] * 2);
    result += record[0] * 0x40;
    if (record[7] != 0) {
        u32 *target = (u32 *)object[0x16];
        u32 *header = *(u32 **)(target[9] - 4);
        header[4] = 0;
        target[9] = record[7];
        if ((object[2] & 0x10) != 0) target[11] = record[8];
        target[5] = (u32)fn_8046288C;
        record[7] = 0;
        record[8] = 0;
    }
    return result + fn_8044BDBC(record);
}

extern "C" u32 fn_8044EE3C(u32 *object) {
    u32 *entry = (u32 *)((u8 *)object + lbl_8056D708[1]);
    if (entry[1] != 0) {
        u32 *target = (u32 *)object[5];
        u32 *owner = (u32 *)object[6];
        u32 *header = *(u32 **)(target[9] - 4);
        header[4] = 0;
        target[9] = entry[1];
        if ((owner[2] & 0x10) != 0) target[11] = entry[2];
        target[5] = (u32)fn_8046288C;
        entry[1] = 0;
        entry[2] = 0;
    }
    return 0;
}

extern "C" void *fn_8044E89C(void *stream, u32 *object) {
    u32 *record = *(u32 **)((u8 *)object + lbl_8056D708[2]);
    u32 size = record[1] + 0x14;
    if (record[4] > 1) size += object[5] * (record[4] * 2);
    size += record[0] * 0x40;
    if (record[7] != 0) {
        u32 *target = (u32 *)object[0x16];
        u32 *header = *(u32 **)(target[9] - 4);
        header[4] = 0;
        target[9] = record[7];
        if ((object[2] & 0x10) != 0) target[11] = record[8];
        target[5] = (u32)fn_8046288C;
        record[7] = 0;
        record[8] = 0;
    }
    size += fn_8044BDBC(record);
    if (fn_8047CEF0(stream, 1, size - 0xC, 0x37002, 0x37) == 0) return NULL;

    u32 marker = 6;
    if (fn_8047D458(stream, &marker, 4) == 0) return NULL;
    u32 header = ((record[1] & 0xFF) << 8) | ((record[4] & 0xFF) << 16) | (record[0] & 0xFF);
    u32 count = object[5];
    u32 channels = record[4];
    if (fn_8047D458(stream, &header, 4) == 0) return NULL;
    if (fn_80480ED0(stream, (void *)record[2], record[1]) == 0) return NULL;
    if (channels > 1) {
        u32 bytes = channels * count;
        if (fn_80480ED0(stream, (void *)record[10], bytes) == 0) return NULL;
        if (fn_80480ED0(stream, (void *)record[9], bytes) == 0) return NULL;
    }
    if (fn_80480ED0(stream, (void *)record[3], record[0] * 0x40) == 0) return NULL;
    return fn_8044BB18(stream, record) != 0 ? stream : NULL;
}

extern "C" void *fn_8044E408(u32 *object, u32 count) {
    object[7] = 0;
    object[8] = 0;
    object[9] = 0;
    object[10] = 0;

    for (u32 group = 0; group < count; group++) {
        for (u32 pass = 0; pass < 3; pass++) {
            for (u32 j = 0; j < 3 - pass; j++) {
                float *values = (float *)((u8 *)object[6] + group * 0x10 + pass * 4 + j * 4);
                if (values[1] > values[0]) {
                    float temporary = values[0];
                    values[0] = values[1];
                    values[1] = temporary;

                    u32 *packed = (u32 *)((u8 *)object[5] + group * 4);
                    u32 shiftA = pass * 8;
                    u32 shiftB = (pass + j + 1) * 8;
                    u32 value = *packed;
                    u32 byteA = (value >> shiftA) & 0xFF;
                    u32 byteB = (value >> shiftB) & 0xFF;
                    *packed = (value & ~(0xFF << shiftA) & ~(0xFF << shiftB)) |
                              (byteA << shiftB) | (byteB << shiftA);
                }
            }
        }
    }

    if (object[4] > 1) {
        u32 channels = object[4];
        u32 bytes = count * channels * 2;
        object[9] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes + 3, 0x30116);
        u8 *encoded = (u8 *)object[9];
        for (u32 group = 0; group < count; group++) {
            u32 sum = 0;
            for (u32 channel = 0; channel < channels; channel++) {
                float value = *(float *)((u8 *)object[6] + group * 0x10 + channel * 4);
                s32 converted = (s32)(lbl_805FB0D0 * value);
                *encoded++ = (u8)converted;
                sum += (u8)converted;
            }
            if ((sum & 0xFF) < 0x80) {
                for (u32 channel = 0; channel < channels && (sum & 0xFF) != 0x80; channel++) {
                    u8 *value = encoded - channels + channel;
                    sum++;
                    if ((sum & 0xFF) != 0x80) *value = (u8)(*value + 1);
                }
            }
        }

        object[10] = (u32)(encoded + 3) & ~3;
        u8 *unpacked = (u8 *)object[10];
        for (u32 group = 0; group < count; group++) {
            u32 packed = *(u32 *)((u8 *)object[5] + group * 4);
            for (u32 channel = 0; channel < channels; channel++) {
                *unpacked++ = (u8)(packed >> (channel * 8));
            }
        }
    }
    return object;
}

extern "C" void *fn_8044EAD0(void *stream, u32 *object) {
    u32 local[8];
    if (fn_8047CF50(stream, 1, local + 4, local + 5) == 0) return NULL;
    if (local[5] < 0x35000 || local[5] > 0x37002) {
        local[6] = 0x116;
        local[7] = fn_8047DC28(0x80000004, local[5]);
        fn_8047DB84(local + 6);
        return NULL;
    }
    if (fn_8047D868(stream, local + 2, 4) == 0 || local[2] != 6) return NULL;

    u32 *record = (u32 *)((void *(*)(void *, u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        (void *)lbl_8056D708[6], 0x4C, 0x30116);
    memset(record, 0, 0x4C);
    if (fn_8047D868(stream, local + 3, 4) == 0) return NULL;
    record[0] = local[3] & 0xFF;
    record[1] = (local[3] >> 16) & 0xFF;
    record[4] = (local[3] >> 8) & 0xFF;
    local[4] -= 8;
    u32 count = object[5];
    u32 channels = record[4];

    if (channels > 1) {
        record[9] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(local[4] + 5, 0x30116);
        record[10] = (record[9] + channels * count + 3) & ~3;
        record[3] = (record[10] + channels * count + 3) & ~3;
        record[2] = record[3] + record[0] * 0x40;
        if (fn_80480D4C(stream, (void *)record[2], record[1]) != record[1]) return NULL;
        u32 bytes = channels * count;
        if (fn_80480D4C(stream, (void *)record[10], bytes) != bytes) return NULL;
        if (fn_80480D4C(stream, (void *)record[9], bytes) != bytes) return NULL;
        if (fn_80480D4C(stream, (void *)record[3], record[0] * 0x40) != record[0] * 0x40) return NULL;
    } else {
        record[9] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(local[4] + 3, 0x30116);
        record[3] = (record[9] + 3) & ~3;
        record[2] = record[3] + record[0] * 0x40;
        if (fn_80480D4C(stream, (void *)record[2], record[1]) != record[1]) return NULL;
        if (fn_80480D4C(stream, (void *)record[3], record[0] * 0x40) != record[0] * 0x40) return NULL;
    }
    if (fn_8044BBFC(stream, record) == NULL) return NULL;
    fn_8044CCC0(object, record);
    return stream;
}

static inline void RwCpp13TransformMatrix(const float *input, u32 outputIndex, u32 transform) {
    float matrix[12];
    matrix[0] = -input[0];
    matrix[1] = -input[4];
    matrix[2] = -input[8];
    matrix[3] = -input[12];
    matrix[4] = input[1];
    matrix[5] = input[5];
    matrix[6] = input[9];
    matrix[7] = input[13];
    matrix[8] = -input[2];
    matrix[9] = -input[6];
    matrix[10] = -input[10];
    matrix[11] = -input[14];
    fn_803991E8(matrix);
    if (transform != 0) fn_80399238(matrix, (void *)outputIndex);
}

static inline void RwCpp13InitialMatrices(u32 *record, u32 *node) {
    if (record[4] != 1 || record[13] != 0) return;
    u32 outputIndex = 0;
    u8 *indices = (u8 *)record[2];
    for (u32 i = 0; i < record[1]; i++) {
        const float *input = (const float *)(lbl_8056D708[3] + indices[i] * 0x40);
        RwCpp13TransformMatrix(input, outputIndex, (node[2] >> 4) & 1);
        outputIndex += 3;
    }
}

static inline void RwCpp13NestedMatrices(u32 *record, u32 *node, u32 remaining) {
    u32 *descriptor = (u32 *)node[1];
    u32 total = *(u16 *)((u8 *)descriptor + 4);
    u32 pair = (total - (remaining + 1)) * 2;
    u8 *table = (u8 *)record[16] + pair;
    u32 first = table[0];
    u32 outerCount = table[1];
    u32 tableOffset = (first & 0x7F) * 2;
    u32 outputIndex = 0;
    u32 transform = (node[2] >> 4) & 1;

    for (u32 outer = 0; (outer & 0xFF) < outerCount; outer++) {
        u8 *entry = (u8 *)record[17] + (outer & 0xFF) * 2 + tableOffset;
        u32 matrixOffset = entry[0] * 0x40;
        u32 innerCount = entry[1];
        for (u32 inner = 0; inner < innerCount; inner++) {
            const float *input = (const float *)(lbl_8056D708[3] + matrixOffset);
            RwCpp13TransformMatrix(input, outputIndex, transform);
            outputIndex += 3;
            matrixOffset += 0x40;
        }
    }
}

extern "C" void *fn_8044DE74(u32 *context, u32 *node) {
    u32 *nodeData = (u32 *)node[0];
    u32 *callbackPair = (u32 *)((u8 *)nodeData + 0x18 + (nodeData[8] - 1) * 8 + 0x14);
    *(u16 *)((u8 *)nodeData + 0x18) = lbl_805EDF40;
    void *frameMatrix = RwFrameGetLTM((void *)context[1]);
    u32 *owner = (u32 *)context[6];
    u32 *format = *(u32 **)((u8 *)owner + lbl_805F1520);
    fn_80462C30(format, node);
    u32 *record = (u32 *)fn_8044CCAC((u8 *)owner);
    if (record[4] > 1 || record[11] == 3) {
        fn_804950AC(frameMatrix, (node[2] >> 4) & 1);
    } else {
        fn_80394500(0, 1);
    }
    if (format == NULL) fn_80462E1C(owner);

    u32 callback = fn_804655D4(node[2], node[8], node[7], nodeData[7] & 1);
    u32 *descriptor = (u32 *)node[1];
    u32 remaining = *(u16 *)((u8 *)descriptor + 4);
    u32 *entry = (u32 *)((u8 *)descriptor + 0x10);
    RwCpp13InitialMatrices(record, node);

    bool lookup = (node[2] & 0x84) != 0;
    if (!lookup) fn_80497778(1);
    while (remaining != 0) {
        if (lookup) {
            u32 *value = *(u32 **)entry[2];
            fn_80499C38(value, 0);
            if (value != NULL && value[0] != 0) {
                u32 *child = *(u32 **)value[0];
                u32 flags = *(u32 *)((u8 *)child + lbl_805F1658 + 0x14);
                fn_80497778((flags & 1) ^ 1);
            } else {
                fn_80497778(1);
            }
        }

        if (callback != 0) {
            u32 *data = (u32 *)entry[2];
            ((void (*)(void *, void *, float))callback)(node + 3, data + 1,
                                                         *(float *)((u8 *)data + 0xC));
        }
        if (record[4] == 1 && record[13] != 0) {
            RwCpp13NestedMatrices(record, node, remaining);
        }
        fn_80399000((void *)callbackPair[0], (void *)callbackPair[1]);
        callbackPair += 2;
        entry += 3;
        remaining--;
    }
    return context;
}

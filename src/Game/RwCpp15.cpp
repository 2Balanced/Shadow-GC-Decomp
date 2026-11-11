/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * NonMatching: the interpolation routines below remain scalar C++; retail uses paired-single
 * arithmetic that GC/2.7 does not generate from these C++ forms. No inline assembly is used.
 * fn_80451E6C also retains one merged range branch, and fn_80450938 retains a larger stack frame
 * and register-allocation differences. */
#include <dolphin/types.h>
#include <string.h>

extern u32 lbl_805F1490;
extern u32 lbl_8056D708[];
extern u32 lbl_805F1520;
extern u16 lbl_805EDF40;
extern u16 lbl_805F1678;
extern u8 *lbl_805F15B4;
extern u32 lbl_805F16B8;

struct RwCpp15Pair {
    float x;
    float y;
};

#define RWCPP15_MUL0(out, a, b)                                                                            \
    do {                                                                                                   \
        (out).x = (a).x * (b).x;                                                                            \
        (out).y = (a).y * (b).x;                                                                            \
    } while (0)
#define RWCPP15_MADD0(out, a, b, c)                                                                        \
    do {                                                                                                   \
        (out).x = (a).x * (b).x + (c).x;                                                                    \
        (out).y = (a).y * (b).x + (c).y;                                                                    \
    } while (0)
#define RWCPP15_MADD1(out, a, b, c)                                                                        \
    do {                                                                                                   \
        (out).x = (a).x * (b).y + (c).x;                                                                    \
        (out).y = (a).y * (b).y + (c).y;                                                                    \
    } while (0)

extern "C" {
u32 fn_8046335C(u32, u32, void *, void *, void *, void *);
void fn_8049B800(void *);
s32 fn_80458C90(u32);
s32 fn_804A2064(void *);
u32 fn_8044CCAC(u8 *);
void *fn_8044D924(u32 *, u32 *);
void *fn_8044DA3C(u32 *, u32 *);
void *fn_8044DE74(u32 *, u32 *);
void *fn_80451A6C(u32 *, u32 *);
void *fn_80451B20(u32 *, u32 *);
void *fn_80451BCC(u32 *, u32 *);
void fn_80463428(void);
s32 fn_804A8478(void *);
void fn_80463104(void *, u32);
s32 fn_80462C28(void);
void fn_80462C20(u32);
void fn_804A8120(void *, void *, u32);
void *RwFrameGetLTM(void *);
void fn_80462C30(void *, void *);
void fn_804950AC(void *, u32);
void fn_80394500(u32, u32);
u32 fn_804A2F08(u32);
u32 fn_8044DC4C(const float *, void *, int);
void fn_8044DD1C(u32 *, u32, int);
void fn_804A457C(u32 *, u32 *, u32 *, void *, u32 *);
u32 fn_8047D868(void *, void *, u32);
u32 fn_80480D4C(void *, void *, u32);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D350(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);
u32 fn_80450448(u32 *);
u32 fn_8045066C(u32 *, u32 *, u32);
void fn_80462E1C(void *);
u32 fn_8046EE8C(u32);
u32 fn_8046EEA0(u32, u32 *);
u32 fn_8046B094(u32, u32, u32);
void fn_8046B0E0(u8 *, u32, u32, u8 *);
void *fn_8046ACBC(u32);
void fn_8046E594(u32, u8 *, u32 *, u32, u32, void *, u32, u32, u32);
void fn_8046F208(u32, u8 *, u32 *, u32);
void fn_8046F6CC(u32, u8 *, u32 *, u32, u32);
void *fn_80480878(void *, void *, u32, void *);
void fn_8046288C(void);
void DCFlushRange(void *, u32);
void fn_80394F2C(void);

u32 fn_80450938(u32 *object, u32 arg2, u32 *out) {
    u32 record = fn_8044CCAC((u8 *)arg2);
    u32 context = fn_80450448(object);
    if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) {
        fn_80462E1C((void *)context);
    }

    u32 second[52];
    second[9] = object[5];
    second[35] = ((u32 *)object[0x17])[5];
    if ((object[2] & 0x10) != 0) {
        second[10] = object[5];
        second[36] = ((u32 *)object[0x17])[6];
    }
    if ((object[2] & 0x8) != 0) {
        second[11] = object[5];
        second[37] = object[0x0C];
    }
    if ((object[2] & 0x84) != 0) {
        for (s32 i = 0; i < (s32)object[7]; i++) {
            second[13 + i] = object[5];
            second[39 + i] = object[0x0D + i];
        }
    }

    u32 base = fn_8046EE8C(context);
    u32 vertices = *(u16 *)((u8 *)object[0x15] + 4);
    u32 stride = (object[2] & 1) != 0 ? 0x98 : 0x90;
    u32 size = base + vertices * 8 + 0x1F;
    u32 sizeOffset = 0;
    for (u32 i = 0; i < vertices; i++) {
        u32 *descriptor = (u32 *)((u8 *)object[0x15] + sizeOffset);
        size += fn_8046B094(context, 1, descriptor[5]);
        sizeOffset += 0xC;
    }
    size += fn_8046EEA0(context, second);

    u32 *block;
    if ((object[2] & 0x02000000) != 0) {
        block = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size + 0x18, 0x30116);
        block[0] = 0;
        block[1] = 0;
        block[2] = size;
        block[3] = arg2;
        block[4] = (u32)out;
        block[5] = (u32)fn_8046288C;
        *out = (u32)block;
    } else {
        block = (u32 *)fn_80480878((void *)arg2, out, size, (void *)fn_8046288C);
    }

    u8 *payload = (u8 *)(block + 6);
    memset(payload, 0, size);
    *(u16 *)(payload + 0) = lbl_805F1678;
    *(u16 *)(payload + 2) = *(u16 *)((u8 *)object[0x15] + 6);
    *(u32 *)(payload + 4) = 0;

    if ((object[2] & 0x8) != 0) {
        u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
        if (format == NULL) {
            fn_80462E1C(format);
            *(u32 *)(payload + 4) &= ~1;
            u8 *bytes = (u8 *)object[0x0C];
            for (s32 i = 0; i < (s32)object[5]; i++) {
                if (bytes[i * 4 + 3] < 0xFF) {
                    *(u32 *)(payload + 4) |= 1;
                    break;
                }
            }
        } else if (format[0xA] > 2) {
            *(u32 *)(payload + 4) |= 1;
        } else {
            *(u32 *)(payload + 4) &= ~1;
        }
    }

    u8 *data = payload + base;
    u8 *end = (u8 *)(((u32)data + vertices * 8 + 0x1F) & ~0x1F);
    u32 descriptorOffset = 0;
    u8 *currentData = data;
    for (u32 vertex = 0; vertex < vertices; vertex++) {
        u32 *descriptor = (u32 *)((u8 *)object[0x15] + descriptorOffset);
        u32 part = fn_8046B094(context, 1, descriptor[5]);
        fn_8046B0E0(currentData, vertex, part, end);
        u32 lookup = fn_8045066C((u32 *)record, descriptor + 4, vertex);
        if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) {
            fn_80462E1C((void *)lookup);
        }

        u32 values[26];
        values[0] = lookup;
        values[9] = descriptor[4];
        if ((object[2] & 0x10) != 0) {
            values[10] = descriptor[4];
        }
        if ((object[2] & 0x8) != 0) {
            values[11] = descriptor[4];
        }
        if ((object[2] & 0x84) != 0) {
            for (s32 i = 0; i < (s32)object[7]; i++) {
                values[13 + i] = descriptor[4];
            }
        }

        void *acbc = fn_8046ACBC(context);
        fn_8046E594(context, currentData, values, descriptor[5], 0, acbc, 1, stride, 0);
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)lookup);
        end += part;
        currentData += 8;
        descriptorOffset += 0xC;
    }

    fn_8046F208(context, payload, second, (u32)end);
    fn_8046F6CC(context, payload, second, 0, 0);
    DCFlushRange(payload, size);
    fn_80394F2C();
    return (u32)block;
}

u32 fn_80451F48(u32 object) {
    (&lbl_805F1490)[1]++;
    return object;
}

u32 fn_80451F5C(u32 object) {
    (&lbl_805F1490)[1]--;
    return object;
}

void fn_80450F68(float *out, u32 *format, u32 *data) {
    s32 count = (s32)data[7];
    if (count <= 0) return;

    u32 stride = data[4];
    u32 input = data[2] - stride;
    u32 output = data[0] - stride;
    RwCpp15Pair f8 = *(const RwCpp15Pair *)(input + stride);
    input += stride;
    RwCpp15Pair f9 = *(const RwCpp15Pair *)(input + stride * 2);
    input += stride * 2;
    const u16 *indices = (const u16 *)format[0x28];
    const s16 *weights = (const s16 *)format[0x24];

    for (s32 i = 0; i < count; i++) {
        u16 index = indices[i];
        const RwCpp15Pair *m = (const RwCpp15Pair *)((u8 *)out + ((index >> 14) & 0x3FC));
        RwCpp15Pair f10, f11;
        RWCPP15_MADD0(f10, m[0], f8, m[6]);
        RWCPP15_MADD0(f11, m[1], f8, m[7]);
        RWCPP15_MADD1(f10, m[2], f8, f10);
        RWCPP15_MADD1(f11, m[3], f8, f11);
        float weight = (float)weights[i] / 32768.0f;
        RwCpp15Pair w = {weight, weight};
        RWCPP15_MADD0(f10, m[4], f9, f10);
        RWCPP15_MADD0(f11, m[5], f9, f11);
        f10.x *= weight;
        f10.y *= weight;
        f11.x *= weight;
        f11.y *= weight;

        if (weight > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + ((index & 0xFF) << 6));
            RwCpp15Pair f12, f13;
            RWCPP15_MADD0(f12, m[0], f8, m[6]);
            RWCPP15_MADD0(f13, m[1], f8, m[7]);
            RWCPP15_MADD1(f12, m[2], f8, f12);
            RWCPP15_MADD1(f13, m[3], f8, f13);
            RWCPP15_MADD0(f12, m[4], f9, f12);
            RWCPP15_MADD0(f13, m[5], f9, f13);
            RWCPP15_MADD1(f10, f12, w, f10);
            RWCPP15_MADD1(f11, f13, w, f11);
        }

        *(RwCpp15Pair *)(output + stride) = f10;
        output += stride;
        *(RwCpp15Pair *)(output + stride * 2) = f11;
        output += stride * 2;
        f8 = *(const RwCpp15Pair *)(input + stride);
        input += stride;
        f9 = *(const RwCpp15Pair *)(input + stride * 2);
        input += stride * 2;
    }
}

void fn_80451254(float *out, u32 *format, u32 *data) {
    s32 count = (s32)data[7];
    if (count <= 0) return;

    u32 stride = data[4];
    u32 input = data[2] - stride;
    u32 output = data[0] - stride;
    const u8 *indices = (const u8 *)format[0x28];
    const s16 *weights = (const s16 *)format[0x24];
    RwCpp15Pair f8 = *(const RwCpp15Pair *)(input + stride);
    input += stride;
    RwCpp15Pair f9 = *(const RwCpp15Pair *)(input + stride * 2);
    input += stride * 2;

    for (s32 i = 0; i < count; i++) {
        RwCpp15Pair w0 = {(float)weights[i * 3] / 32768.0f, (float)weights[i * 3 + 1] / 32768.0f};
        RwCpp15Pair w1 = {(float)weights[i * 3 + 2] / 32768.0f,
                          (float)weights[i * 3 + 3] / 32768.0f};
        const RwCpp15Pair *m = (const RwCpp15Pair *)((u8 *)out + indices[i * 3] * 0x40);
        RwCpp15Pair f10, f11;
        RWCPP15_MADD0(f10, m[0], f8, m[6]);
        RWCPP15_MADD0(f11, m[1], f8, m[7]);
        RWCPP15_MADD1(f10, m[2], f8, f10);
        RWCPP15_MADD1(f11, m[3], f8, f11);
        RWCPP15_MADD0(f10, m[4], f9, f10);
        RWCPP15_MADD0(f11, m[5], f9, f11);
        f10.x *= w0.x;
        f10.y *= w0.y;
        f11.x *= w0.x;
        f11.y *= w0.y;

        if (w0.y > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + indices[i * 3 + 1] * 0x40);
            RwCpp15Pair f12, f13;
            RWCPP15_MADD0(f12, m[0], f8, m[6]);
            RWCPP15_MADD0(f13, m[1], f8, m[7]);
            RWCPP15_MADD1(f12, m[2], f8, f12);
            RWCPP15_MADD1(f13, m[3], f8, f13);
            RWCPP15_MADD0(f12, m[4], f9, f12);
            RWCPP15_MADD0(f13, m[5], f9, f13);
            RWCPP15_MADD1(f10, f12, w0, f10);
            RWCPP15_MADD1(f11, f13, w0, f11);
        }
        if (w1.x > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + indices[i * 3 + 2] * 0x40);
            RwCpp15Pair f12, f13;
            RWCPP15_MADD0(f12, m[0], f8, m[6]);
            RWCPP15_MADD0(f13, m[1], f8, m[7]);
            RWCPP15_MADD1(f12, m[2], f8, f12);
            RWCPP15_MADD1(f13, m[3], f8, f13);
            RWCPP15_MADD0(f12, m[4], f9, f12);
            RWCPP15_MADD0(f13, m[5], f9, f13);
            RWCPP15_MADD0(f10, f12, w1, f10);
            RWCPP15_MADD0(f11, f13, w1, f11);
        }

        *(RwCpp15Pair *)(output + stride) = f10;
        output += stride;
        *(RwCpp15Pair *)(output + stride * 2) = f11;
        output += stride * 2;
        f8 = *(const RwCpp15Pair *)(input + stride);
        input += stride;
        f9 = *(const RwCpp15Pair *)(input + stride * 2);
        input += stride * 2;
    }
}

void fn_80451618(float *out, u32 *format, u32 *data) {
    s32 count = (s32)data[7];
    if (count <= 0) return;

    u32 stride = data[4];
    u32 input = data[2] - stride;
    u32 output = data[0] - stride;
    const u16 *indices = (const u16 *)format[0x28];
    const s16 *weights = (const s16 *)format[0x24];
    RwCpp15Pair f8 = *(const RwCpp15Pair *)(input + stride);
    input += stride;
    RwCpp15Pair f9 = *(const RwCpp15Pair *)(input + stride * 2);
    input += stride * 2;

    for (s32 i = 0; i < count; i++) {
        u16 index = indices[i];
        RwCpp15Pair w0 = {(float)weights[i * 2] / 32768.0f, (float)weights[i * 2 + 1] / 32768.0f};
        RwCpp15Pair w1 = {(float)weights[i * 2 + 2] / 32768.0f,
                          (float)weights[i * 2 + 3] / 32768.0f};
        const RwCpp15Pair *m = (const RwCpp15Pair *)((u8 *)out + ((index >> 14) & 0x3FC));
        RwCpp15Pair f10, f11;
        RWCPP15_MADD0(f10, m[0], f8, m[6]);
        RWCPP15_MADD0(f11, m[1], f8, m[7]);
        RWCPP15_MADD1(f10, m[2], f8, f10);
        RWCPP15_MADD1(f11, m[3], f8, f11);
        RWCPP15_MADD0(f10, m[4], f9, f10);
        RWCPP15_MADD0(f11, m[5], f9, f11);
        f10.x *= w0.x;
        f10.y *= w0.y;
        f11.x *= w0.x;
        f11.y *= w0.y;

        if (w0.y > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + ((index >> 10) & 0x3FC));
            RwCpp15Pair f12, f13;
            RWCPP15_MADD0(f12, m[0], f8, m[6]);
            RWCPP15_MADD0(f13, m[1], f8, m[7]);
            RWCPP15_MADD1(f12, m[2], f8, f12);
            RWCPP15_MADD1(f13, m[3], f8, f13);
            RWCPP15_MADD0(f12, m[4], f9, f12);
            RWCPP15_MADD0(f13, m[5], f9, f13);
            RWCPP15_MADD1(f10, f12, w0, f10);
            RWCPP15_MADD1(f11, f13, w0, f11);
        }
        if (w1.x > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + ((index >> 30) & 0x3FC));
            RwCpp15Pair f12, f13;
            RWCPP15_MADD0(f12, m[0], f8, m[6]);
            RWCPP15_MADD0(f13, m[1], f8, m[7]);
            RWCPP15_MADD1(f12, m[2], f8, f12);
            RWCPP15_MADD1(f13, m[3], f8, f13);
            RWCPP15_MADD0(f12, m[4], f9, f12);
            RWCPP15_MADD0(f13, m[5], f9, f13);
            RWCPP15_MADD0(f10, f12, w1, f10);
            RWCPP15_MADD0(f11, f13, w1, f11);
        }
        if (w1.y > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + ((index & 0xFF) << 6));
            RwCpp15Pair f12, f13;
            RWCPP15_MADD0(f12, m[0], f8, m[6]);
            RWCPP15_MADD0(f13, m[1], f8, m[7]);
            RWCPP15_MADD1(f12, m[2], f8, f12);
            RWCPP15_MADD1(f13, m[3], f8, f13);
            RWCPP15_MADD0(f12, m[4], f9, f12);
            RWCPP15_MADD0(f13, m[5], f9, f13);
            RWCPP15_MADD1(f10, f12, w1, f10);
            RWCPP15_MADD1(f11, f13, w1, f11);
        }

        *(RwCpp15Pair *)(output + stride) = f10;
        output += stride;
        *(RwCpp15Pair *)(output + stride * 2) = f11;
        output += stride * 2;
        f8 = *(const RwCpp15Pair *)(input + stride);
        input += stride;
        f9 = *(const RwCpp15Pair *)(input + stride * 2);
        input += stride * 2;
    }
}

void fn_804517C8(float *out, u32 *format, u32 *data) {
    s32 count = (s32)data[7];
    if (count <= 0) return;

    u32 strideA = data[4];
    u32 strideB = data[5];
    u32 inputA = data[2] - strideA;
    u32 inputB = data[3] - strideB;
    u32 outputA = data[0] - strideA;
    u32 outputB = data[1] - strideB;
    u32 extra = data[6];
    const u16 *indices = (const u16 *)format[0x28];
    const s16 *weights = (const s16 *)format[0x24];
    RwCpp15Pair f8 = *(const RwCpp15Pair *)(inputA + strideA);
    inputA += strideA;
    RwCpp15Pair f9 = *(const RwCpp15Pair *)(inputA + strideA * 2);
    inputA += strideA * 2;
    RwCpp15Pair f10 = *(const RwCpp15Pair *)(inputB + strideB);
    inputB += strideB;
    RwCpp15Pair f11 = *(const RwCpp15Pair *)(inputB + strideB * 2);
    inputB += strideB * 2;

    for (s32 i = 0; i < count; i++) {
        inputB += extra;
        u16 index = indices[i];
        RwCpp15Pair w0 = {(float)weights[i * 2] / 32768.0f, (float)weights[i * 2 + 1] / 32768.0f};
        RwCpp15Pair w1 = {(float)weights[i * 2 + 2] / 32768.0f,
                          (float)weights[i * 2 + 3] / 32768.0f};
        const RwCpp15Pair *m = (const RwCpp15Pair *)((u8 *)out + ((index >> 14) & 0x3FC));
        RwCpp15Pair f24, f25, f26, f27;
        RWCPP15_MADD0(f26, m[0], f10, m[6]);
        RWCPP15_MADD0(f27, m[1], f10, m[7]);
        RWCPP15_MADD0(f24, m[0], f8, m[6]);
        RWCPP15_MADD0(f25, m[1], f8, m[7]);
        RWCPP15_MADD1(f26, m[2], f10, f26);
        RWCPP15_MADD1(f27, m[3], f10, f27);
        RWCPP15_MADD1(f24, m[2], f8, f24);
        RWCPP15_MADD1(f25, m[3], f8, f25);
        RWCPP15_MADD0(f26, m[4], f11, f26);
        RWCPP15_MADD0(f27, m[5], f11, f27);
        RWCPP15_MADD0(f24, m[4], f9, f24);
        RWCPP15_MADD0(f25, m[5], f9, f25);
        f26.x *= w0.x;
        f26.y *= w0.y;
        f27.x *= w0.x;
        f27.y *= w0.y;
        f24.x *= w0.x;
        f24.y *= w0.y;
        f25.x *= w0.x;
        f25.y *= w0.y;

        if (w0.y > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + ((index >> 10) & 0x3FC));
            RwCpp15Pair f28, f29, f30, f31;
            RWCPP15_MADD0(f30, m[0], f10, m[6]);
            RWCPP15_MADD0(f31, m[1], f10, m[7]);
            RWCPP15_MADD0(f28, m[0], f8, m[6]);
            RWCPP15_MADD0(f29, m[1], f8, m[7]);
            RWCPP15_MADD1(f30, m[2], f10, f30);
            RWCPP15_MADD1(f31, m[3], f10, f31);
            RWCPP15_MADD1(f28, m[2], f8, f28);
            RWCPP15_MADD1(f29, m[3], f8, f29);
            RWCPP15_MADD0(f30, m[4], f11, f30);
            RWCPP15_MADD0(f31, m[5], f11, f31);
            RWCPP15_MADD0(f28, m[4], f9, f28);
            RWCPP15_MADD0(f29, m[5], f9, f29);
            RWCPP15_MADD1(f26, f30, w0, f26);
            RWCPP15_MADD1(f27, f31, w0, f27);
            RWCPP15_MADD1(f24, f28, w0, f24);
            RWCPP15_MADD1(f25, f29, w0, f25);
        }
        if (w1.x > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + ((index >> 30) & 0x3FC));
            RwCpp15Pair f28, f29, f30, f31;
            RWCPP15_MADD0(f30, m[0], f10, m[6]);
            RWCPP15_MADD0(f31, m[1], f10, m[7]);
            RWCPP15_MADD0(f28, m[0], f8, m[6]);
            RWCPP15_MADD0(f29, m[1], f8, m[7]);
            RWCPP15_MADD1(f30, m[2], f10, f30);
            RWCPP15_MADD1(f31, m[3], f10, f31);
            RWCPP15_MADD1(f28, m[2], f8, f28);
            RWCPP15_MADD1(f29, m[3], f8, f29);
            RWCPP15_MADD0(f30, m[4], f11, f30);
            RWCPP15_MADD0(f31, m[5], f11, f31);
            RWCPP15_MADD0(f28, m[4], f9, f28);
            RWCPP15_MADD0(f29, m[5], f9, f29);
            RWCPP15_MADD0(f26, f30, w1, f26);
            RWCPP15_MADD0(f27, f31, w1, f27);
            RWCPP15_MADD0(f24, f28, w1, f24);
            RWCPP15_MADD0(f25, f29, w1, f25);
        }
        if (w1.y > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + ((index & 0xFF) << 6));
            RwCpp15Pair f28, f29, f30, f31;
            RWCPP15_MADD0(f30, m[0], f10, m[6]);
            RWCPP15_MADD0(f31, m[1], f10, m[7]);
            RWCPP15_MADD0(f28, m[0], f8, m[6]);
            RWCPP15_MADD0(f29, m[1], f8, m[7]);
            RWCPP15_MADD1(f30, m[2], f10, f30);
            RWCPP15_MADD1(f31, m[3], f10, f31);
            RWCPP15_MADD1(f28, m[2], f8, f28);
            RWCPP15_MADD1(f29, m[3], f8, f29);
            RWCPP15_MADD0(f30, m[4], f11, f30);
            RWCPP15_MADD0(f31, m[5], f11, f31);
            RWCPP15_MADD0(f28, m[4], f9, f28);
            RWCPP15_MADD0(f29, m[5], f9, f29);
            RWCPP15_MADD1(f26, f30, w1, f26);
            RWCPP15_MADD1(f27, f31, w1, f27);
            RWCPP15_MADD1(f24, f28, w1, f24);
            RWCPP15_MADD1(f25, f29, w1, f25);
        }

        *(RwCpp15Pair *)(outputB + strideB) = f26;
        outputB += strideB;
        *(RwCpp15Pair *)(outputB + strideB * 2) = f27;
        outputB += strideB * 2 + extra;
        *(RwCpp15Pair *)(outputA + strideA) = f24;
        outputA += strideA;
        *(RwCpp15Pair *)(outputA + strideA * 2) = f25;
        outputA += strideA * 2;

        f8 = *(const RwCpp15Pair *)(inputA + strideA);
        inputA += strideA;
        f9 = *(const RwCpp15Pair *)(inputA + strideA * 2);
        inputA += strideA * 2;
        f10 = *(const RwCpp15Pair *)(inputB + strideB);
        inputB += strideB;
        f11 = *(const RwCpp15Pair *)(inputB + strideB * 2);
        inputB += strideB * 2;
    }
}

void fn_804513C4(float *out, u32 *format, u32 *data) {
    s32 count = (s32)data[7];
    if (count <= 0) return;

    u32 strideA = data[4];
    u32 strideB = data[5];
    u32 inputA = data[2] - strideA;
    u32 inputB = data[3] - strideB;
    u32 outputA = data[0] - strideA;
    u32 outputB = data[1] - strideB;
    u32 extra = data[6];
    const u8 *indices = (const u8 *)format[0x28];
    const s16 *weights = (const s16 *)format[0x24];
    RwCpp15Pair f8 = *(const RwCpp15Pair *)(inputA + strideA);
    inputA += strideA;
    RwCpp15Pair f9 = *(const RwCpp15Pair *)(inputA + strideA * 2);
    inputA += strideA * 2;
    RwCpp15Pair f10 = *(const RwCpp15Pair *)(inputB + strideB);
    inputB += strideB;
    RwCpp15Pair f11 = *(const RwCpp15Pair *)(inputB + strideB * 2);
    inputB += strideB * 2;

    for (s32 i = 0; i < count; i++) {
        inputB += extra;
        RwCpp15Pair w0 = {(float)weights[i * 3] / 32768.0f, (float)weights[i * 3 + 1] / 32768.0f};
        RwCpp15Pair w1 = {(float)weights[i * 3 + 2] / 32768.0f,
                          (float)weights[i * 3 + 3] / 32768.0f};
        const RwCpp15Pair *m = (const RwCpp15Pair *)((u8 *)out + indices[i * 3] * 0x40);
        RwCpp15Pair f24, f25, f26, f27;
        RWCPP15_MADD0(f26, m[0], f10, m[6]);
        RWCPP15_MADD0(f27, m[1], f10, m[7]);
        RWCPP15_MADD0(f24, m[0], f8, m[6]);
        RWCPP15_MADD0(f25, m[1], f8, m[7]);
        RWCPP15_MADD1(f26, m[2], f10, f26);
        RWCPP15_MADD1(f27, m[3], f10, f27);
        RWCPP15_MADD1(f24, m[2], f8, f24);
        RWCPP15_MADD1(f25, m[3], f8, f25);
        RWCPP15_MADD0(f26, m[4], f11, f26);
        RWCPP15_MADD0(f27, m[5], f11, f27);
        RWCPP15_MADD0(f24, m[4], f9, f24);
        RWCPP15_MADD0(f25, m[5], f9, f25);
        f26.x *= w0.x;
        f26.y *= w0.y;
        f27.x *= w0.x;
        f27.y *= w0.y;
        f24.x *= w0.x;
        f24.y *= w0.y;
        f25.x *= w0.x;
        f25.y *= w0.y;

        if (w0.y > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + indices[i * 3 + 1] * 0x40);
            RwCpp15Pair f28, f29, f30, f31;
            RWCPP15_MADD0(f30, m[0], f10, m[6]);
            RWCPP15_MADD0(f31, m[1], f10, m[7]);
            RWCPP15_MADD0(f28, m[0], f8, m[6]);
            RWCPP15_MADD0(f29, m[1], f8, m[7]);
            RWCPP15_MADD1(f30, m[2], f10, f30);
            RWCPP15_MADD1(f31, m[3], f10, f31);
            RWCPP15_MADD1(f28, m[2], f8, f28);
            RWCPP15_MADD1(f29, m[3], f8, f29);
            RWCPP15_MADD0(f30, m[4], f11, f30);
            RWCPP15_MADD0(f31, m[5], f11, f31);
            RWCPP15_MADD0(f28, m[4], f9, f28);
            RWCPP15_MADD0(f29, m[5], f9, f29);
            RWCPP15_MADD1(f26, f30, w0, f26);
            RWCPP15_MADD1(f27, f31, w0, f27);
            RWCPP15_MADD1(f24, f28, w0, f24);
            RWCPP15_MADD1(f25, f29, w0, f25);
        }
        if (w1.x > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + indices[i * 3 + 2] * 0x40);
            RwCpp15Pair f28, f29, f30, f31;
            RWCPP15_MADD0(f30, m[0], f10, m[6]);
            RWCPP15_MADD0(f31, m[1], f10, m[7]);
            RWCPP15_MADD0(f28, m[0], f8, m[6]);
            RWCPP15_MADD0(f29, m[1], f8, m[7]);
            RWCPP15_MADD1(f30, m[2], f10, f30);
            RWCPP15_MADD1(f31, m[3], f10, f31);
            RWCPP15_MADD1(f28, m[2], f8, f28);
            RWCPP15_MADD1(f29, m[3], f8, f29);
            RWCPP15_MADD0(f30, m[4], f11, f30);
            RWCPP15_MADD0(f31, m[5], f11, f31);
            RWCPP15_MADD0(f28, m[4], f9, f28);
            RWCPP15_MADD0(f29, m[5], f9, f29);
            RWCPP15_MADD0(f26, f30, w1, f26);
            RWCPP15_MADD0(f27, f31, w1, f27);
            RWCPP15_MADD0(f24, f28, w1, f24);
            RWCPP15_MADD0(f25, f29, w1, f25);
        }

        *(RwCpp15Pair *)(outputB + strideB) = f26;
        outputB += strideB;
        *(RwCpp15Pair *)(outputB + strideB * 2) = f27;
        outputB += strideB * 2 + extra;
        *(RwCpp15Pair *)(outputA + strideA) = f24;
        outputA += strideA;
        *(RwCpp15Pair *)(outputA + strideA * 2) = f25;
        outputA += strideA * 2;

        f8 = *(const RwCpp15Pair *)(inputA + strideA);
        inputA += strideA;
        f9 = *(const RwCpp15Pair *)(inputA + strideA * 2);
        inputA += strideA * 2;
        f10 = *(const RwCpp15Pair *)(inputB + strideB);
        inputB += strideB;
        f11 = *(const RwCpp15Pair *)(inputB + strideB * 2);
        inputB += strideB * 2;
    }
}

void fn_80451080(float *out, u32 *format, u32 *data) {
    s32 count = (s32)data[7];
    if (count <= 0) return;

    u32 strideA = data[4];
    u32 strideB = data[5];
    u32 inputA = data[2] - strideA;
    u32 inputB = data[3] - strideB;
    u32 outputA = data[0] - strideA;
    u32 outputB = data[1] - strideB;
    u32 extra = data[6];
    const u16 *indices = (const u16 *)format[0x28];
    const s16 *weights = (const s16 *)format[0x24];
    RwCpp15Pair f8 = *(const RwCpp15Pair *)(inputA + strideA);
    inputA += strideA;
    RwCpp15Pair f9 = *(const RwCpp15Pair *)(inputA + strideA * 2);
    inputA += strideA * 2;
    RwCpp15Pair f10 = *(const RwCpp15Pair *)(inputB + strideB);
    inputB += strideB;
    RwCpp15Pair f11 = *(const RwCpp15Pair *)(inputB + strideB * 2);
    inputB += strideB * 2;

    for (s32 i = 0; i < count; i++) {
        inputB += extra;
        u16 index = indices[i];
        const RwCpp15Pair *m = (const RwCpp15Pair *)((u8 *)out + ((index >> 14) & 0x3FC));
        RwCpp15Pair f24, f25, f26, f27;
        RWCPP15_MADD0(f26, m[0], f10, m[6]);
        RWCPP15_MADD0(f27, m[1], f10, m[7]);
        RWCPP15_MADD0(f24, m[0], f8, m[6]);
        RWCPP15_MADD0(f25, m[1], f8, m[7]);
        RWCPP15_MADD1(f26, m[2], f10, f26);
        RWCPP15_MADD1(f27, m[3], f10, f27);
        RWCPP15_MADD1(f24, m[2], f8, f24);
        RWCPP15_MADD1(f25, m[3], f8, f25);
        RWCPP15_MADD0(f26, m[4], f11, f26);
        RWCPP15_MADD0(f27, m[5], f11, f27);
        RWCPP15_MADD0(f24, m[4], f9, f24);
        RWCPP15_MADD0(f25, m[5], f9, f25);
        float weight = (float)weights[i] / 32768.0f;
        RwCpp15Pair w = {weight, weight};
        f26.x *= weight;
        f26.y *= weight;
        f27.x *= weight;
        f27.y *= weight;
        f24.x *= weight;
        f24.y *= weight;
        f25.x *= weight;
        f25.y *= weight;

        if (weight > 0.0f) {
            m = (const RwCpp15Pair *)((u8 *)out + ((index & 0xFF) << 6));
            RwCpp15Pair f28, f29, f30, f31;
            RWCPP15_MADD0(f30, m[0], f10, m[6]);
            RWCPP15_MADD0(f31, m[1], f10, m[7]);
            RWCPP15_MADD0(f28, m[0], f8, m[6]);
            RWCPP15_MADD0(f29, m[1], f8, m[7]);
            RWCPP15_MADD1(f30, m[2], f10, f30);
            RWCPP15_MADD1(f31, m[3], f10, f31);
            RWCPP15_MADD1(f28, m[2], f8, f28);
            RWCPP15_MADD1(f29, m[3], f8, f29);
            RWCPP15_MADD0(f30, m[4], f11, f30);
            RWCPP15_MADD0(f31, m[5], f11, f31);
            RWCPP15_MADD0(f28, m[4], f9, f28);
            RWCPP15_MADD0(f29, m[5], f9, f29);
            RWCPP15_MADD1(f26, f30, w, f26);
            RWCPP15_MADD1(f27, f31, w, f27);
            RWCPP15_MADD1(f24, f28, w, f24);
            RWCPP15_MADD1(f25, f29, w, f25);
        }

        *(RwCpp15Pair *)(outputB + strideB) = f26;
        outputB += strideB;
        *(RwCpp15Pair *)(outputB + strideB * 2) = f27;
        outputB += strideB * 2 + extra;
        *(RwCpp15Pair *)(outputA + strideA) = f24;
        outputA += strideA;
        *(RwCpp15Pair *)(outputA + strideA * 2) = f25;
        outputA += strideA * 2;

        f8 = *(const RwCpp15Pair *)(inputA + strideA);
        inputA += strideA;
        f9 = *(const RwCpp15Pair *)(inputA + strideA * 2);
        inputA += strideA * 2;
        f10 = *(const RwCpp15Pair *)(inputB + strideB);
        inputB += strideB;
        f11 = *(const RwCpp15Pair *)(inputB + strideB * 2);
        inputB += strideB * 2;
    }
}

void fn_80450D6C(float *out, const float *matB, const float *matA, const float *initial,
                  const u8 *indices, u32 count) {
    const RwCpp15Pair *v = (const RwCpp15Pair *)initial;
    for (s32 n = (s32)count - 1; n >= 0; n--) {
        u32 index = indices[n];
        const RwCpp15Pair *a = (const RwCpp15Pair *)(matA + index * 16);
        const RwCpp15Pair *b = (const RwCpp15Pair *)(matB + index * 16);
        RwCpp15Pair *dest = (RwCpp15Pair *)(out + index * 16);
        RwCpp15Pair f0 = v[0], f1 = v[1], f2 = v[2], f3 = v[3];
        RwCpp15Pair f4 = v[4], f5 = v[5], f6 = v[6], f7 = v[7];
        RwCpp15Pair f8 = a[0], f9 = a[1], f10 = a[2], f11 = a[3];
        RwCpp15Pair f12 = a[4], f13 = a[5], f14 = a[6], f15 = a[7];
        RwCpp15Pair f16 = b[0], f17 = b[1], f18 = b[2], f19 = b[3];
        RwCpp15Pair f20 = b[4], f21 = b[5], f22 = b[6], f23 = b[7];
        RwCpp15Pair f24, f25, f26, f27, f28, f29, f30, f31;
        RWCPP15_MUL0(f24, f0, f8);
        RWCPP15_MUL0(f25, f1, f8);
        RWCPP15_MUL0(f26, f0, f10);
        RWCPP15_MUL0(f27, f1, f10);
        RWCPP15_MADD1(f24, f2, f8, f24);
        RWCPP15_MADD1(f25, f3, f8, f25);
        RWCPP15_MADD1(f26, f2, f10, f26);
        RWCPP15_MADD1(f27, f3, f10, f27);
        RWCPP15_MADD0(f24, f4, f9, f24);
        RWCPP15_MADD0(f25, f5, f9, f25);
        RWCPP15_MADD0(f26, f4, f11, f26);
        RWCPP15_MADD0(f27, f5, f11, f27);
        RWCPP15_MUL0(f28, f0, f12);
        RWCPP15_MUL0(f29, f1, f12);
        RWCPP15_MADD0(f30, f0, f14, f6);
        RWCPP15_MADD0(f31, f1, f14, f7);
        RWCPP15_MADD1(f28, f2, f12, f28);
        RWCPP15_MADD1(f29, f3, f12, f29);
        RWCPP15_MADD1(f30, f2, f14, f30);
        RWCPP15_MADD1(f31, f3, f14, f31);
        RWCPP15_MADD0(f28, f4, f13, f28);
        RWCPP15_MADD0(f29, f5, f13, f29);
        RWCPP15_MADD0(f30, f4, f15, f30);
        RWCPP15_MADD0(f31, f5, f15, f31);
        RwCpp15Pair o0, o1, o2, o3, o4, o5, o6, o7;
        RWCPP15_MUL0(o0, f24, f16);
        RWCPP15_MUL0(o1, f25, f16);
        RWCPP15_MUL0(o2, f24, f18);
        RWCPP15_MUL0(o3, f25, f18);
        RWCPP15_MADD1(o0, f26, f16, o0);
        RWCPP15_MADD1(o1, f27, f16, o1);
        RWCPP15_MADD1(o2, f26, f18, o2);
        RWCPP15_MADD1(o3, f27, f18, o3);
        RWCPP15_MADD0(o0, f28, f17, o0);
        RWCPP15_MADD0(o1, f29, f17, o1);
        RWCPP15_MADD0(o2, f28, f19, o2);
        RWCPP15_MADD0(o3, f29, f19, o3);
        RWCPP15_MUL0(o4, f24, f20);
        RWCPP15_MUL0(o5, f25, f20);
        RWCPP15_MADD0(o6, f24, f22, f30);
        RWCPP15_MADD0(o7, f25, f22, f31);
        RWCPP15_MADD1(o4, f26, f20, o4);
        RWCPP15_MADD1(o5, f27, f20, o5);
        RWCPP15_MADD1(o6, f26, f22, o6);
        RWCPP15_MADD1(o7, f27, f22, o7);
        RWCPP15_MADD0(o4, f28, f21, o4);
        RWCPP15_MADD0(o5, f29, f21, o5);
        RWCPP15_MADD0(o6, f28, f23, o6);
        RWCPP15_MADD0(o7, f29, f23, o7);
        dest[0] = o0;
        dest[1] = o1;
        dest[2] = o2;
        dest[3] = o3;
        dest[4] = o4;
        dest[5] = o5;
        dest[6] = o6;
        dest[7] = o7;
    }
}

void *fn_80451A6C(u32 *context, u32 *node) {
    if (fn_804A8478(node) != 0) {
        u32 *owner = (u32 *)context[6];
        if (*(u32 *)((u8 *)owner + lbl_805F1520) == 0) {
            fn_80463104(owner, lbl_805F16B8);
        }
        s32 state = fn_80462C28();
        if (state == 1) fn_80462C20(0);
        context = (u32 *)fn_8044D924(context, node);
        if (state == 1) fn_80462C20(1);
        return context;
    }
    return fn_8044D924(context, node);
}

void *fn_80451B20(u32 *context, u32 *node) {
    if (fn_8044DA3C(context, node) == NULL) return NULL;
    u32 *owner = (u32 *)context[6];
    u8 *format = *(u8 **)((u8 *)owner + lbl_805F1520);
    u32 *record = (u32 *)fn_8044CCAC((u8 *)owner);
    if (record[4] > 1 && format != NULL && format[0xD] != 0 && (owner[2] & 0x10) != 0) {
        fn_804A8120(node, format, owner[5]);
    }
    return context;
}

void *fn_80451BCC(u32 *context, u32 *node) {
    u32 *nodeData = (u32 *)node[0];
    u8 *callbackBytes = (u8 *)nodeData + 0x18;
    callbackBytes += (nodeData[8] - 1) * 8;
    callbackBytes += 0x14;
    u32 *callbackPair = (u32 *)callbackBytes;
    *(u16 *)((u8 *)nodeData + 0x18) = lbl_805EDF40;
    u32 *owner = (u32 *)context[6];
    u32 *descriptor = *(u32 **)((u8 *)owner + 0x54);
    void *frameMatrix = RwFrameGetLTM((void *)context[1]);
    u32 *format = *(u32 **)((u8 *)owner + lbl_805F1520);
    fn_80462C30(format, node);
    u32 *record = (u32 *)fn_8044CCAC((u8 *)owner);
    if (record[4] > 1) {
        fn_804950AC(frameMatrix, (node[2] >> 4) & 1);
    } else {
        fn_80394500(0, 1);
    }

    u32 last = (u32)record;
    if (record[4] == 1 && record[13] == 0) {
        u32 outputIndex = 0;
        for (u32 i = 0; i < record[1]; i++) {
            const float *input = (const float *)(lbl_8056D708[3] + ((u8 *)record[2])[i] * 0x40);
            last = fn_8044DC4C(input, (void *)outputIndex, (node[2] >> 4) & 1);
            outputIndex += 3;
        }
    }
    fn_804A2F08(last);

    s32 remaining = *(u16 *)((u8 *)descriptor + 4);
    u32 *entry = (u32 *)((u8 *)descriptor + 0x10);
    while (remaining != 0) {
        if (record[4] == 1 && record[13] != 0) {
            u32 total = *(u16 *)((u8 *)node[1] + 4);
            fn_8044DD1C(record, total - (remaining + 1), (node[2] >> 4) & 1);
        }
        fn_804A457C(entry, callbackPair, context, frameMatrix, node);
        callbackPair += 2;
        entry += 3;
        remaining--;
    }
    return context;
}

u32 fn_80451D60(u32 mode) {
    u32 *callbacks = lbl_8056D708;
    if ((mode & 1) != 0) {
        callbacks[10] = fn_8046335C(0x116, 1, (void *)fn_8044D924, (void *)fn_8044DA3C,
                                    (void *)fn_80463428, (void *)fn_8044DE74);
    }
    if ((mode & 2) != 0) {
        callbacks[11] = fn_8046335C(0x116, 2, (void *)fn_80451A6C, (void *)fn_80451B20,
                                    (void *)fn_80463428, (void *)fn_80451BCC);
    }
    return 1;
}

u32 fn_80451E0C(void) {
    u32 *callbacks = lbl_8056D708;
    if (callbacks[10] != 0) {
        fn_8049B800((void *)callbacks[10]);
        callbacks[10] = 0;
    }
    if (callbacks[11] != 0) {
        fn_8049B800((void *)callbacks[11]);
        callbacks[11] = 0;
    }
    return 1;
}

u32 *fn_80451E6C(u32 *object, s32 mode) {
    u32 *record = (u32 *)fn_8044CCAC((u8 *)object[6]);
    s32 value;
    if (mode == 1) goto modeOne;
    if (mode >= 1) {
        if (mode >= 3) goto modeDefault;
        goto modeTwo;
    }
    goto modeDefault;

modeOne:
    record[11] = 1;
    value = 1;
    goto modeDone;
modeTwo:
    record[11] = 2;
    value = 2;
    goto modeDone;
modeDefault:
    if (fn_80458C90(0x120) > 0 && fn_804A2064(object) != 0) {
        record[11] = 2;
        value = 2;
    } else {
        record[11] = 1;
        value = 1;
    }
modeDone:
    object[0x1B] = lbl_8056D708[9 + value];
    return object;
}

void *fn_80451F70(u32 *out, void *stream) {
    s32 count;
    if (fn_8047D868(stream, &count, 4) == 0) return NULL;
    if (count > 0) {
        out[0] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count, 0x3011F);
        if (out[0] == 0) return NULL;
        if (fn_80480D4C(stream, (void *)out[0], count) == 0) return NULL;
    }
    if (fn_8047D868(stream, out + 1, 4) == 0) return NULL;
    if (fn_8047D868(stream, out + 2, 4) == 0) return NULL;

    switch (out[1]) {
    case 1:
        out[3] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(out[2] * 4, 0x3011F);
        if (out[3] == 0) return NULL;
        if (fn_8047D868(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 2:
        out[3] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(out[2] * 4, 0x3011F);
        if (out[3] == 0) return NULL;
        if (fn_8047D784(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 3: {
        out[3] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(out[2] * 4, 0x3011F);
        if (out[3] == 0) return NULL;
        u32 *entries = (u32 *)out[3];
        for (s32 i = 0; i < (s32)out[2]; i++) {
            if (fn_8047D868(stream, &count, 4) == 0) return NULL;
            if (count > 0) {
                entries[i] = (u32)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count, 0x3011F);
                if (entries[i] == 0) return NULL;
                if (fn_80480D4C(stream, (void *)entries[i], count) == 0) return NULL;
            } else {
                entries[i] = 0;
            }
        }
        break;
    }
    default:
        return NULL;
    }
    return stream;
}

void *fn_80452228(u32 *out, void *stream) {
    s32 size = out[0] != 0 ? ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))((void *)out[0]) + 1 : 0;
    if (fn_8047D458(stream, &size, 4) == 0) return NULL;
    if (size > 0 && fn_80480ED0(stream, (void *)out[0], size) == 0) return NULL;
    if (fn_8047D458(stream, out + 1, 4) == 0) return NULL;
    if (fn_8047D458(stream, out + 2, 4) == 0) return NULL;

    switch (out[1]) {
    case 1:
        if (fn_8047D458(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 2:
        if (fn_8047D350(stream, (void *)out[3], out[2] * 4) == 0) return NULL;
        break;
    case 3: {
        u32 *entries = (u32 *)out[3];
        for (s32 i = 0; i < (s32)out[2]; i++) {
            size = entries[i] != 0 ? ((u32 (*)(void *))*(u32 *)(lbl_805F15B4 + 0xF4))((void *)entries[i]) + 1 : 0;
            if (fn_8047D458(stream, &size, 4) == 0) return NULL;
            if (size > 0 && fn_80480ED0(stream, (void *)entries[i], size) == 0) return NULL;
        }
        break;
    }
    default:
        return NULL;
    }
    return stream;
}
}

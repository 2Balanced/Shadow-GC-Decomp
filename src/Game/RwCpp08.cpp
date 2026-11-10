/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <dolphin/types.h>

extern u32 lbl_805F1438;

extern "C" {
s32 fn_80462C28(void);
void fn_80462C20(u32);
void fn_804639AC(u32 *, u32);

void *fn_80445934(u32 *object, u32 argument) {
    u32 *owner = (u32 *)object[6];
    if ((owner[2] & 0x02000000) != 0) {
        s32 mode = fn_80462C28();
        if (mode == 1) fn_80462C20(0);
        fn_804639AC(object, argument);
        if (mode == 1) fn_80462C20(1);
    } else {
        fn_804639AC(object, argument);
    }
    return object;
}

void fn_804459C0(u8 *destination, u8 *source, float *coefficients, float *delta, u32 *groups,
                 u32 step, u32 stride) {
    destination -= step;
    source -= step;
    if ((s32)groups[1] <= 0) return;
    float scale = *delta;
    for (u32 group = 0; group < groups[1]; group++) {
        u8 mask = *(u8 *)(groups[0] + group);
        u32 count = mask & 0x7F;
        if ((mask & 0x80) != 0) {
            for (u32 i = 0; i < count; i++) {
                *(float *)(destination + step) = *(float *)(source + step) + scale * coefficients[0];
                *(float *)(destination + step + 4) = *(float *)(source + step + 4) + scale * coefficients[1];
                *(float *)(destination + step * 2) = *(float *)(source + step * 2) + scale * coefficients[2];
                coefficients += 3;
                destination += stride;
                source += stride;
            }
        } else {
            for (u32 i = 0; i < count; i++) {
                *(float *)(destination + step) = *(float *)(source + step);
                *(float *)(destination + step + 4) = *(float *)(source + step + 4);
                *(float *)(destination + step * 2) = *(float *)(source + step * 2);
                destination += stride;
                source += stride;
            }
        }
    }
}

void fn_80445AE8(u8 *destination, u8 *source, float *coefficients, float *delta, u32 *groups,
                 u32 step, u32 stride) {
    destination -= step;
    source -= step;
    if ((s32)groups[1] <= 0) return;
    float scale = *delta;
    for (u32 group = 0; group < groups[1]; group++) {
        u8 mask = *(u8 *)(groups[0] + group);
        u32 count = mask & 0x7F;
        if ((mask & 0x80) != 0) {
            for (u32 i = 0; i < count; i++) {
                *(float *)(destination + step) = *(float *)(source + step) + scale * coefficients[0];
                *(float *)(destination + step + 4) = *(float *)(source + step + 4) + scale * coefficients[1];
                *(float *)(destination + step * 2) = *(float *)(source + step * 2) + scale * coefficients[2];
                coefficients += 3;
                destination += stride;
                source += stride;
            }
        } else {
            u32 skip = count * (step * 3 + stride);
            destination += skip;
            source += skip;
        }
    }
}

void fn_80445C00(u8 *destination, u8 *source, u32 step, u32 stride, s32 count) {
    if (count <= 0) return;
    destination -= step;
    source -= step;
    for (s32 i = 0; i < count; i++) {
        *(float *)(destination + step) = *(float *)(source + step);
        *(float *)(destination + step + 4) = *(float *)(source + step + 4);
        *(float *)(destination + step * 2) = *(float *)(source + step * 2);
        destination += stride;
        source += stride;
    }
}

typedef void (*RwCpp08Callback)(void);
void *fn_80444E7C(void);
void *fn_8046335C(u32, void *, RwCpp08Callback, RwCpp08Callback, RwCpp08Callback, RwCpp08Callback);
void fn_80463428(void);
void fn_80462914(void);
void fn_8049B800(void *);

u32 fn_80445C38(void *object) {
    void *result = fn_8046335C(0x122, object, (RwCpp08Callback)fn_80445934,
                               (RwCpp08Callback)fn_80444E7C, (RwCpp08Callback)fn_80463428,
                               (RwCpp08Callback)fn_80462914);
    if (result == NULL) return 0;
    ((u32 *)&lbl_805F1438)[1] = (u32)result;
    return 1;
}

u32 fn_80445C9C(void) {
    if (((u32 *)&lbl_805F1438)[1] != 0) {
        fn_8049B800((void *)((u32 *)&lbl_805F1438)[1]);
        ((u32 *)&lbl_805F1438)[1] = 0;
    }
    return 1;
}

extern float lbl_805FAFEC;
extern float lbl_805FAFE8;
extern float lbl_805FAFF0;
extern float lbl_805FAFF4;
extern float lbl_805FAFF8;
extern float lbl_805FAFFC;
extern float lbl_805FB000;
extern float lbl_805FB004;
extern float lbl_805FB008;
extern float lbl_805FB00C;
extern float lbl_805FB010;
extern float lbl_805FB014;
extern float lbl_805FB018;
extern float lbl_805FB01C;
extern float lbl_805FB020;
extern float lbl_805FB024;
extern float lbl_805FB028;
extern float lbl_805FB02C;
extern float lbl_805FB030;
extern float lbl_805FB034;
extern float lbl_805FB038;
extern float lbl_805FB03C;
extern float lbl_805FB040;
extern float lbl_805FB044;
extern float lbl_805FB048;
extern float lbl_805FB04C;
float fn_80482340(float);

void fn_80445CF4(float *out, const float *q) {
    float x = q[2];
    float y = q[3];
    float z = q[4];
    float w = q[5];
    float two = lbl_805FAFEC;
    float one = lbl_805FAFE8;
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float wz = w * z;
    float yz = y * z;
    float wx = w * x;
    float zx = z * x;
    float wy = w * y;

    out[0] = one - two * (yy + zz);
    out[1] = two * (xy + wz);
    out[2] = two * (zx - wy);
    *(u32 *)(out + 3) = 3;
    out[4] = two * (xy - wz);
    out[5] = one - two * (xx + zz);
    out[6] = two * (yz + wx);
    out[8] = two * (zx + wy);
    out[9] = two * (yz - wx);
    out[10] = one - two * (xx + yy);
    out[12] = q[6];
    out[13] = q[7];
    out[14] = q[8];
}

static inline float RwCpp08AnglePolynomial(float dot) {
    float x = dot * dot;
    float a = lbl_805FB01C * x + lbl_805FB018;
    float b = x * a + lbl_805FB014;
    float c = lbl_805FB02C * x + lbl_805FB028;
    float d = x * b + lbl_805FB010;
    float e = x * c + lbl_805FB024;
    float f = x * d + lbl_805FB00C;
    float g = x * e + lbl_805FB020;
    float h = x * f + lbl_805FB008;
    float i = x * g + 1.0f;
    float j = x * h;
    float k = j / i;
    float l = lbl_805FB004 - dot * k;
    float m = dot - l;
    return lbl_805FB000 - m;
}

static inline float RwCpp08AngleSqrt(float dot, int negative) {
    float x = lbl_805FB030 * (negative ? 1.0f + dot : 1.0f - dot);
    float root = fn_80482340(x);
    if (negative) {
        float a = lbl_805FB01C * x + lbl_805FB018;
        float b = x * a + lbl_805FB014;
        float c = lbl_805FB02C * x + lbl_805FB028;
        float d = x * b + lbl_805FB010;
        float e = x * c + lbl_805FB024;
        float f = x * d + lbl_805FB00C;
        float g = x * e + lbl_805FB008;
        float h = x * g;
        float i = h / 1.0f;
        float j = i * root - lbl_805FB004;
        float k = lbl_805FB004 + j;
        return lbl_805FB034 - 2.0f * k;
    }

    u32 rootBits = *(u32 *)&root;
    rootBits &= 0xFFFFF000;
    float adjusted = *(float *)&rootBits;
    float a = lbl_805FB01C * x + lbl_805FB018;
    float b = x * a + lbl_805FB014;
    float c = lbl_805FB02C * x + lbl_805FB028;
    float d = x * b + lbl_805FB010;
    float e = x * c + lbl_805FB024;
    float f = x * d + lbl_805FB00C;
    float g = x * e + lbl_805FB008;
    float h = x * f + lbl_805FB020;
    float numerator = x - adjusted * adjusted;
    float denominator = root + adjusted;
    float first = numerator / denominator;
    float second = (x * g) / (x * h + 1.0f);
    return 2.0f * (adjusted + second * root + first);
}

static inline float RwCpp08Angle(float dot) {
    u32 bits = *(u32 *)&dot;
    u32 magnitude = bits << 1;
    if (magnitude >= 0x3F800000) {
        return (bits & 0x80000000) ? lbl_805FAFF8 : lbl_805FAFF0;
    }
    if (magnitude < 0x3F000000) {
        if (magnitude <= 0x23000000) return lbl_805FAFFC;
        return RwCpp08AnglePolynomial(dot);
    }
    return RwCpp08AngleSqrt(dot, (bits & 0x80000000) != 0);
}

static inline void RwCpp08NormalizeWeights(float angle, float *first, float *second) {
    float angle2 = angle * angle;
    float f0 = lbl_805FB04C * angle2 + lbl_805FB048;
    float f3 = angle2 * f0 + lbl_805FB044;
    float f1 = lbl_805FB04C * (*first * *first) + lbl_805FB048;
    float f2 = lbl_805FB04C * (*second * *second) + lbl_805FB048;
    f3 = angle2 * f3 + lbl_805FB040;
    f1 = *first * *first * f1 + lbl_805FB044;
    f2 = *second * *second * f2 + lbl_805FB044;
    f3 = angle2 * f3 + lbl_805FB03C;
    float f7 = angle2 * angle;
    f1 = *first * *first * f1 + lbl_805FB040;
    f3 = angle2 * f3 + lbl_805FB038;
    f2 = *second * *second * f2 + lbl_805FB040;
    f1 = *first * *first * f1 + lbl_805FB03C;
    float f6 = f7 * f3 + angle;
    f2 = *second * *second * f2 + lbl_805FB03C;
    float f3a = *first * *first * *first;
    float scale = 1.0f / f6;
    float f2a = *first * *first * f1 + lbl_805FB038;
    float f1a = *second * *second * *second;
    f2 = *second * *second * f2 + lbl_805FB038;
    *first = (f3a * f2a + *first) * scale;
    *second = (f1a * f2 + *second) * scale;
}

void fn_80445E08(float *out, float *a, float *b, float t) {
    float ratio = (t - a[1]) / (b[1] - a[1]);
    out[6] = a[6] + ratio * (b[6] - a[6]);
    out[7] = a[7] + ratio * (b[7] - a[7]);
    out[8] = a[8] + ratio * (b[8] - a[8]);

    float dot = a[3] * b[3] + a[2] * b[2] + a[4] * b[4] + a[5] * b[5];
    if (dot < 0.0f) {
        dot = -dot;
        b[2] = -b[2];
        b[3] = -b[3];
        b[4] = -b[4];
        b[5] = -b[5];
    }
    float angle = RwCpp08Angle(dot);
    float first = (1.0f - ratio) * angle;
    float second = ratio * angle;
    RwCpp08NormalizeWeights(angle, &first, &second);
    out[2] = second * b[2] + first * a[2];
    out[3] = second * b[3] + first * a[3];
    out[4] = second * b[4] + first * a[4];
    out[5] = second * b[5] + first * a[5];
}

u32 fn_80446780(u32 *object) {
    return object[1] * 0x24;
}

void fn_80445CE0(u32 *object, u32 index) {
    object[0x1B] = *(u32 *)((u8 *)&lbl_805F1438 + index * 4);
}
}

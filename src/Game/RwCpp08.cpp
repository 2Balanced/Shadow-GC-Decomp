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

u32 fn_80446780(u32 *object) {
    return object[1] * 0x24;
}

void fn_80445CE0(u32 *object, u32 index) {
    object[0x1B] = *(u32 *)((u8 *)&lbl_805F1438 + index * 4);
}
}

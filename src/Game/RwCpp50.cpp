/* RenderWare texture/resource operations and fixed-point region sampling, 0x8048915C..0x8048A96B.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern s32 lbl_805F15C0;
extern s32 lbl_805F15C8;
extern s32 DAT_805f15cc;
extern u32 lbl_8056E018;
extern u32 lbl_805EDEF8;
extern u32 lbl_805EDEFC;
extern u32 lbl_805E2CE8;
extern f32 lbl_805FB328;
extern f32 lbl_805FB32C;
extern f32 lbl_805FB330;
extern f32 lbl_805FB348;
extern f32 lbl_805FB34C;
extern f32 lbl_805FB350;
extern f32 lbl_805FB310;
extern f32 lbl_805FB314;
extern f32 lbl_805FB318;

u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void fn_80482144(void *, void *);
void fn_80481DC8(void *, void *, void *, void *, void *, void *);
void fn_804807CC(void);
void fn_8048209C(void *, void *);
void fn_8047FCDC(void *);
void *fn_8047FC04(u32, u32, u32, u32, u32, u32);
double pow(double, double);

#define RW_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))
#define RW_ERROR(value)                                                                          \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28(value);                                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)

u32 fn_8048915C(u32 *dest, u32 *source) {
    u32 kind = (source[3] << 8) | dest[3];
    if (kind == 0x404 || kind == 0x808 || kind == 0x2020) return 1;
    u32 width = dest[1];
    u32 rows = dest[2];
    u8 *inputPixels = (u8 *)source[5];
    u8 *outputPixels = (u8 *)dest[5];
    u32 *palette = (u32 *)source[6];
    if (kind == 0x408) {
        for (u32 row = 0; row < rows; row++) {
            memcpy(outputPixels, inputPixels, width);
            inputPixels += source[4];
            outputPixels += dest[4];
        }
        return 1;
    }
    if (kind == 0x420 || kind == 0x820) {
        for (u32 row = 0; row < rows; row++) {
            u32 *output = (u32 *)outputPixels;
            for (u32 x = 0; x < width; x++) output[x] = palette[inputPixels[x]];
            inputPixels += source[4];
            outputPixels += dest[4];
        }
        return 1;
    }
    RW_ERROR(0x80000009);
    return 0;
}

void *fn_804893B8(u32 *dest, u32 *source) {
    if (dest[3] == source[3]) {
        if (dest[6] != 0 && source[6] != 0 && dest[3] < 9) {
            memcpy((void *)dest[6], (void *)source[6], (1 << dest[3]) * 4);
        }
        u8 *input = (u8 *)source[5];
        u8 *output = (u8 *)dest[5];
        for (u32 row = 0; row < dest[2]; row++) {
            memcpy(output, input, ((dest[3] + 7) >> 3) * dest[1]);
            input += source[4];
            output += dest[4];
        }
    } else if (fn_8048915C(dest, source) == 0) {
        dest = NULL;
    }
    if (dest != NULL) dest[0] = (dest[0] & ~2) | (source[0] & 2);
    return dest;
}

void fn_804894B8(u32 *texture) {
    u32 kind = texture[3];
    if (kind != 4 && kind != 8 && kind != 0x20) {
        RW_ERROR(0x80000008);
        return;
    }
    u8 *table = lbl_805F15B4 + lbl_805F15C0 + 0xC;
    if (kind == 0x20) {
        u8 *pixels = (u8 *)texture[5];
        if (pixels == NULL) {
            RW_ERROR(0x80000016);
            return;
        }
        for (u32 row = 0; row < texture[2]; row++) {
            for (u32 x = 0; x < texture[1]; x++) {
                u8 red = pixels[0];
                pixels[0] = table[pixels[0]];
                pixels[1] = table[pixels[1]];
                pixels[2] = table[pixels[2]];
                pixels[3] = red;
                pixels += 4;
            }
            pixels += texture[4] - texture[1] * 4;
        }
    } else {
        u8 *palette = (u8 *)texture[6];
        if (palette == NULL) {
            RW_ERROR(0x80000016);
            return;
        }
        for (u32 i = 0; i < (1u << kind); i++) {
            u8 red = palette[0];
            palette[0] = table[palette[0]];
            palette[1] = table[palette[1]];
            palette[2] = table[palette[2]];
            palette[3] = red;
            palette += 4;
        }
    }
    texture[0] |= 2;
}

u32 fn_80489830(f32 gamma) {
    u8 *base = lbl_805F15B4 + lbl_805F15C0;
    *(f32 *)(base + 0x20C) = gamma;
    base[0xC] = 0;
    base[0x10C] = 0;
    f32 inverse = lbl_805FB310 / gamma;
    for (u32 i = 1; i < 0x100; i++) {
        f32 value = (f32)i / lbl_805FB314;
        base[0xC + i] = (u8)(lbl_805FB318 + lbl_805FB314 * (f32)pow(value, inverse));
        base[0x10C + i] = (u8)(lbl_805FB318 + lbl_805FB314 * (f32)pow(value, gamma));
    }
    return 1;
}

u32 *fn_804899A8(u32 *object, u8 *resource) {
    if (RW_API(u32 (*)(void), 0x60)() == 0) return NULL;
    if ((resource[0x22] & 1) != 0) object[0] |= 2;
    return object;
}

void *fn_80489A1C(void *object, u32 *resource) {
    if (RW_API(u32 (*)(void), 0x64)() == 0) return NULL;
    if ((resource[0] & 2) != 0) ((u8 *)object)[0x22] |= 1;
    return object;
}

void *fn_80489A90(void *context, u32 kind, u32 *width, u32 *height, void **a, u32 *b) {
    u8 info[0x24];
    if (RW_API(u32 (*)(void *, void *, u32), 0x6C)(info, context, kind) == 0) return NULL;
    *b = (u32)info[0x20] | ((u32)info[0x23] << 8);
    *width = *(u32 *)(info + 0xC);
    *height = *(u32 *)(info + 0x10);
    *a = *(void **)(info + 0x14);
    return context;
}

void fn_80489B2C(u8 *object, u16 *width, u16 *height) {
    *width = *(s16 *)(object + 0x1C);
    *height = *(s16 *)(object + 0x1E);
}

void *fn_80489B40(void *object) {
    RW_API(void (*)(u32, void *, u32), 0x88)(0, object, 0);
    return object;
}

void *fn_80489B88(void *object) {
    RW_API(void (*)(u32, void *, u32), 0xA8)(0, object, 0);
    ((u8 *)object)[0x22] &= 0xE7;
    return object;
}

u32 fn_80489BDC(void *object) {
    fn_80482144(&lbl_8056E018, object);
    RW_API(void (*)(u32, void *, u32), 0x5C)(0, object, 0);
    RW_API(void (*)(void *, void *), 0x11C)(*(void **)(lbl_805F15B4 + lbl_805F15C8 + 0x60), object);
    return 1;
}

void fn_80489C54(void *a, void *b, void *c, void *d, void *e) {
    fn_80481DC8(&lbl_8056E018, a, b, c, d, e);
}

u32 fn_80489C94(void *object, u32 mode) {
    u32 result[2];
    if (RW_API(u32 (*)(void *, void *, u32), 0xA4)(result, object, mode) == 0) result[0] = 0;
    return result[0];
}

u32 fn_80489CE4(u8 *object) {
    if ((object[0x23] & 0x80) == 0) return 1;
    u32 result[2];
    if (RW_API(u32 (*)(void *, void *, u32), 0xB8)(result, object, 0) == 0) result[0] = 0xFFFFFFFF;
    return result[0];
}

void *fn_80489D44(void *a, void *b, void *c) {
    u32 (*callback)(void *, void *, void *) = RW_API(u32 (*)(void *, void *, void *), 0x98);
    fn_804807CC();
    return callback(a, b, c) != 0 ? a : NULL;
}

u32 *fn_80489DC4(u32 *object, u32 *other, u32 *rect) {
    if ((((u8 *)object)[0x21] & 0x80) == 0) return NULL;
    object[3] = rect[2];
    object[4] = rect[3];
    *(u16 *)(object + 7) = *(u16 *)(other + 7) + (u16)rect[0];
    *(u16 *)((u8 *)object + 0x1E) = *(u16 *)((u8 *)other + 0x1E) + (u16)rect[1];
    if (RW_API(u32 (*)(void), 0x78)() == 0) return NULL;
    object[0] = other[0];
    return object;
}

void *fn_80489E74(u32 width, u32 height, void *a, void *b) {
    u8 *object = (u8 *)RW_API(void *(*)(void *, u32), 0x118)(
        *(void **)(lbl_805F15B4 + lbl_805F15C8 + 0x60), 0x30407);
    if (object == NULL) return NULL;
    object[0x22] = 0;
    object[0x21] = 0;
    *(u32 *)(object + 0xC) = width;
    *(u32 *)(object + 0x10) = height;
    *(u16 *)(object + 0x1C) = 0;
    *(u16 *)(object + 0x1E) = 0;
    *(void **)(object + 0x14) = a;
    *(u32 *)object = (u32)object;
    *(u32 *)(object + 4) = 0;
    *(u32 *)(object + 8) = 0;
    if (RW_API(u32 (*)(u32, void *, void *), 0x58)(0, object, b) == 0) {
        RW_API(void (*)(void *, void *), 0x11C)(*(void **)(lbl_805F15B4 + lbl_805F15C8 + 0x60), object);
        return NULL;
    }
    fn_8048209C(&lbl_8056E018, object);
    return object;
}

u32 fn_80489F70(void *object, u8 group, s32 mode) {
    u32 result[2];
    if (RW_API(u32 (*)(void *, void *, u32), 0x84)(result, object, mode + group * 0x100) == 0) {
        result[0] = 0;
    }
    return result[0];
}

void *fn_80489FC0(void *object) {
    void *memory = *(void **)(lbl_805F15B4 + lbl_805F15C8 + 0x60);
    if (memory != NULL) {
        fn_8047FCDC(memory);
        *(u32 *)(lbl_805F15B4 + lbl_805F15C8 + 0x60) = 0;
    }
    DAT_805f15cc--;
    return object;
}

void *fn_8048A02C(void *object, s32 offset) {
    lbl_805F15C8 = offset;
    memset(lbl_805F15B4 + offset + 0x2C, 0, 0x34);
    *(u32 *)(lbl_805F15B4 + offset + 0x38) = 0;
    *(u32 *)(lbl_805F15B4 + offset + 0x3C) = 0;
    *(u32 *)(lbl_805F15B4 + offset + 0x40) = 0;
    *(u8 *)(lbl_805F15B4 + offset + 0x4D) = 0x80;
    *(u32 *)(lbl_805F15B4 + offset + 0x30) = 0;
    *(u32 *)(lbl_805F15B4 + offset + 0x34) = 0;
    *(u8 *)(lbl_805F15B4 + offset + 0x4C) = 0;
    *(u32 *)(lbl_805F15B4 + offset + 0x28) = 0;
    *(u32 *)(lbl_805F15B4 + offset) = (u32)(lbl_805F15B4 + offset + 0x2C);
    void *memory = fn_8047FC04(lbl_8056E018, lbl_805EDEF8, 4, lbl_805EDEFC, lbl_805E2CE8, 0x40407);
    *(void **)(lbl_805F15B4 + offset + 0x60) = memory;
    if (memory == NULL) return NULL;
    DAT_805f15cc++;
    return object;
}

void fn_8048A178(u32 *texture, u32 xStart, u32 xEnd, u32 y, f32 *out) {
    s32 first = (s32)xStart >> 16;
    s32 last = (s32)xEnd >> 16;
    u8 *pixel = (u8 *)texture[5] + ((s32)y >> 16) * texture[4] + first * 4;
    f32 width = lbl_805FB328 * (s32)(xEnd - xStart);
    if (first == last) {
        for (u32 i = 0; i < 4; i++) out[i] = lbl_805FB32C * pixel[i] * width;
    } else {
        s32 next = (first + 1) << 16;
        f32 weight = lbl_805FB328 * (next - (s32)xStart);
        for (u32 i = 0; i < 4; i++) out[i] = lbl_805FB32C * pixel[i] * weight;
        pixel += 4;
        for (s32 x = first + 1; x < last; x++) {
            for (u32 i = 0; i < 4; i++) out[i] += lbl_805FB32C * pixel[i];
            pixel += 4;
        }
        weight = lbl_805FB328 * ((s32)xEnd - (last << 16));
        for (u32 i = 0; i < 4; i++) out[i] += lbl_805FB32C * pixel[i] * weight;
    }
    f32 scale = lbl_805FB330 / width;
    for (u32 i = 0; i < 4; i++) out[i] *= scale;
}

void fn_8048A50C(u32 *texture, u32 xStart, u32 xEnd, u32 yStart, u32 yEnd, f32 *out) {
    s32 first = (s32)yStart >> 16;
    s32 last = (s32)yEnd >> 16;
    if (first == last) {
        fn_8048A178(texture, xStart, xEnd, yStart, out);
        return;
    }
    f32 total[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    for (s32 row = first; row <= last; row++) {
        f32 sample[4];
        fn_8048A178(texture, xStart, xEnd, (u32)(row << 16), sample);
        f32 weight = 1.0f;
        if (row == first) weight = lbl_805FB328 * (((first + 1) << 16) - (s32)yStart);
        else if (row == last) weight = lbl_805FB328 * ((s32)yEnd - (last << 16));
        for (u32 i = 0; i < 4; i++) total[i] += sample[i] * weight;
    }
    f32 scale = lbl_805FB330 / (lbl_805FB328 * (s32)(yEnd - yStart));
    for (u32 i = 0; i < 4; i++) out[i] = total[i] * scale;
}

void *fn_8048A790(u32 *dest, u32 *source) {
    dest[0] |= source[0] & 2;
    s32 stepX = (s32)(lbl_805FB348 * ((f32)dest[1] / (f32)source[1]));
    s32 stepY = (s32)(lbl_805FB348 * ((f32)dest[2] / (f32)source[2]));
    s32 y = 0;
    for (u32 row = 0; row < dest[2]; row++) {
        u8 *output = (u8 *)dest[5] + dest[4] * row;
        s32 x = 0;
        for (u32 column = 0; column < dest[1]; column++) {
            f32 color[4];
            fn_8048A50C(source, x, x + stepX - 1, y, y + stepY - 1, color);
            output[0] = (u8)(lbl_805FB350 * color[0] + lbl_805FB34C);
            output[1] = (u8)(lbl_805FB350 * color[1] + lbl_805FB34C);
            output[2] = (u8)(lbl_805FB350 * color[2] + lbl_805FB34C);
            output[3] = (u8)(lbl_805FB350 * color[3] + lbl_805FB34C);
            output += 4;
            x += stepX;
        }
        y += stepY;
    }
    return dest;
}
}

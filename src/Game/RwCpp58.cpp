/* RenderWare stream tiling, raster formats, and initialization, 0x804959C0..0x80497473.
 * Names are inferred from behavior; definitions remain in retail address order.
 * Remaining: register allocation in fn_804959C0, fn_80495C54, and fn_804960B4.
 * Swept declaration order/scope/types, O3/O4, lifetime/strength passes, and GC 2.6/2.7. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern s32 lbl_805F1658;
extern s32 lbl_805F1670;
extern u32 *lbl_805F1664;
extern u32 *lbl_805F1660;
extern u8 *lbl_805F164C;
extern s32 lbl_805F15E0;
extern u16 lbl_805EDF40;
extern u16 lbl_805F1678;
static const char lbl_8056E7B0[] = "rwRASTERTYPECAMERATEXTURE can not be palletized";
static const char s_rwRASTERFORMAT888_invalid_format_8056e7e0[] = "rwRASTERFORMAT888 invalid format";
static const char s_rwRASTERFORMAT8888_invalid_forma_8056e804[] = "rwRASTERFORMAT8888 invalid format";
static const char s_rwRASTERFORMATLUM8_invalid_forma_8056e828[] = "rwRASTERFORMATLUM8 invalid format";
static const char s_rwRASTERFORMAT16_invalid_format_8056e84c[] = "rwRASTERFORMAT16 invalid format";
static const char s_rwRASTERFORMAT24_invalid_format_8056e86c[] = "rwRASTERFORMAT24 invalid format";
static const char s_rwRASTERFORMAT32_invalid_format_8056e88c[] = "rwRASTERFORMAT32 invalid format";
static const char lbl_8056E930[] = "Failed to create surface for texture";
extern u32 lbl_805FB3D0;
extern f32 lbl_805FB3D4;
extern f32 lbl_805FB3D8;
extern f32 lbl_805FB3DC;
extern u32 lbl_805E3210[20];

u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
s32 fn_80491AB4(s32);
s32 fn_80495428(u32 *, u32);
void fn_8049572C(void *, u32, u8 *, u32, u32, u32);
void DCFlushRange(void *, u32);
void DCInvalidateRange(void *, u32);
void fn_80397774(void);
void fn_8039561C(u16);
s32 fn_8049A78C(u16);
void fn_8039744C(u32, void *, void *, u32);
void fn_80398DAC(u32, u32, u32);
void fn_80398DE0(u32);
void fn_80398734(u32, u32, u32, u32, u32);
void fn_80398D00(u32, u32, u32, u32);
void fn_80395D48(u32);
void fn_80396C78(u32, u32, u32, u32, u32, u32, u32);
void fn_80396B54(u32, void *);
void fn_80398D54(u32);
void fn_80398D80(u32);
void fn_8039616C(void *, u32);
void fn_80399288(u32);
void fn_80499BC8(void);
void *fn_8048CCDC(void *);
void *fn_80489E74(u32, u32, void *, void *);
u32 fn_80489F70(void *, u8, s32);
void fn_80489B40(void *);
void fn_80489BDC(void *);
void *fn_8048CA00(u32 *, u32);
void fn_8048D2C0(u32 *, u32);
u32 fn_8048CD8C(u32 *);
u32 fn_80496A20(u32 *, s8);
u32 fn_8049714C(u32 *, u32, u32);

#define RW_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))
#define RW_ERROR(value)                                                                          \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28(value);                                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)
#define RW_ERROR_SIZED(value, size)                                                                  \
    do {                                                                                             \
        u32 error[2];                                                                                \
        error[0] = 1;                                                                                \
        error[1] = fn_8047DC28((value), (size));                                                     \
        fn_8047DB84(error);                                                                          \
    } while (0)
#define RW_ERROR_STRING(value, text)                                                             \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28((value), (u32)&(text));                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)

void fn_804959C0(void *dest, void *source, s32 width, s32 height, u32 format, s32 stride) {
    switch (format - 4) {
    case 0: {
        u32 destOffset = 0;
        s32 blocks = ((width + 7) & ~7) >> 3;
        s32 row;
        for (row = 0; row < height; row++) {
            u32 blockRow = blocks * (row >> 3);
            u8 *out = (u8 *)dest + destOffset;
            u32 sourceRow = (row & 7) * 8;
            u32 block = 0;
            s32 outOffset;
            outOffset = block;
            for (s32 x = 0; x < width; x += 8) {
                u32 sourceOffset = (((blockRow + block) * 0x40) + sourceRow) >> 1;
                memcpy(out + outOffset, (u8 *)source + sourceOffset, 4);
                block++;
                outOffset += 4;
            }
            destOffset += stride;
        }
        break;
    }
    case 1:
        goto invalid;
    case 4: {
        u32 destOffset = 0;
        s32 blocks = ((width + 7) & ~7) >> 3;
        s32 row;
        for (row = 0; row < height; row++) {
            u32 blockRow = blocks * (row >> 2);
            u8 *out = (u8 *)dest + destOffset;
            u32 sourceRow = (row & 3) * 8;
            u32 block = 0;
            for (s32 outOffset = 0; outOffset < width; outOffset += 8) {
                memcpy(out + outOffset,
                       (u8 *)source + ((blockRow + block) * 0x20) + sourceRow, 8);
                block++;
            }
            destOffset += stride;
        }
        break;
    }
    case 0xC: {
        u32 destOffset = 0;
        s32 blocks = ((width + 3) & ~3) >> 2;
        s32 row;
        for (row = 0; row < height; row++) {
            u32 blockRow = blocks * (row >> 2);
            u32 sourceRow = (row & 3) * 4;
            u8 *out = (u8 *)dest + destOffset;
            s32 x = 0;
            u32 block = 0;
            while (x < width) {
                u32 sourceOffset = (((blockRow + block) * 0x10) + sourceRow) * 2;
                memcpy(out, (u8 *)source + sourceOffset, 8);
                block++;
                out += 8;
                x += 4;
            }
            destOffset += stride;
        }
        break;
    }
    case 0x1C: {
        u32 destOffset = 0;
        s32 blocks = ((width + 3) & ~3) >> 2;
        for (s32 row = 0; row < height; row++) {
            u32 blockRow = blocks * (row >> 2);
            u8 *out = (u8 *)dest + destOffset;
            u32 sourceRow = (row & 3) * 4;
            u32 block = 0;
            s32 outOffset = 0;
            for (s32 x = 0; x < width; x++) {
                u32 pixel = x & 3;
                if (pixel == 0) block = (blockRow + (x >> 2)) * 0x10;
                u32 sourceOffset = block * 4 + (sourceRow + pixel) * 2;
                *(u16 *)(out + outOffset) = *(u16 *)((u8 *)source + sourceOffset);
                *(u16 *)(out + outOffset + 2) = *(u16 *)((u8 *)source + sourceOffset + 0x20);
                outOffset += 4;
            }
            destOffset += stride;
        }
        break;
    }
    default:
invalid:
        RW_ERROR(0x8000000C);
        break;
    }
}

u32 fn_80495C54(u32 *out, u32 *object, u32 mode) {
    u32 *base = (u32 *)object[0];
    u32 *viewport = (u32 *)((u8 *)base + lbl_805F1658);
    u32 flags = mode;
    mode = (mode >> 8) & 0xFF;
    u8 level = mode;
    s32 kind = ((u8 *)object)[0x20] & 7;
    switch (kind) {
    case 0:
    case 4:
    case 5: {

    u32 total = 0;
    u32 remaining = level;
    while ((u8)remaining-- != 0) {
        total += fn_80495428(object, remaining);
    }
    total += viewport[7];

    u32 shift = (flags >> 8) & 0xFF;
    s32 width = 1;
    s32 shiftedWidth = (s32)((u32 *)object[0])[3] >> shift;
    if (shiftedWidth != 0) width = shiftedWidth;
    s32 pitch;
    switch (object[5] - 4) {
    case 0:
        pitch = ((width + 7) & ~7) >> 1;
        break;
    case 1:
        goto invalid_pitch;
    case 4:
        pitch = (width + 7) & ~7;
        break;
    case 0xC:
        pitch = ((width + 3) & ~3) * 2;
        break;
    case 0x1C:
        pitch = ((width + 3) & ~3) * 4;
        break;
    default:
invalid_pitch:
        RW_ERROR(0x8000000C);
        pitch = 0;
        break;
    }

    u32 external = flags & 8;
    if (external == 0) {
        s32 height = 1;
        s32 shiftedHeight = (s32)base[4] >> shift;
        if (shiftedHeight != 0) height = shiftedHeight;
        pitch *= height;
        viewport[10] = (u32)RW_API(void *(*)(u32, u32), 0x108)(pitch, 0x30411);
        if (viewport[10] == 0) return 0;
    }

    if (base == object) {
        s32 lockedWidth = 1;
        s32 lockedHeight = 1;
        s32 shiftedWidth = (s32)object[3] >> shift;
        s32 shiftedHeight = (s32)object[4] >> shift;
        if (shiftedWidth != 0) lockedWidth = shiftedWidth;
        if (shiftedHeight != 0) lockedHeight = shiftedHeight;
        object[10] = object[3];
        object[11] = object[4];
        object[3] = lockedWidth;
        object[4] = lockedHeight;
        object[1] = external == 0 ? viewport[10] : total;
    } else {
        if (external != 0) {
            RW_ERROR(0x8000000E);
            return 0;
        }
        switch (object[5] - 4) {
        case 0:
            object[1] = viewport[10] + pitch * *(s16 *)((u8 *)object + 0x1E) +
                        (*(s16 *)((u8 *)object + 0x1C) >> 1);
            break;
        case 1:
            goto invalid_subraster;
        case 4:
            object[1] = viewport[10] + pitch * *(s16 *)((u8 *)object + 0x1E) +
                        *(s16 *)((u8 *)object + 0x1C);
            break;
        case 0xC:
            object[1] = viewport[10] + pitch * *(s16 *)((u8 *)object + 0x1E) +
                        *(s16 *)((u8 *)object + 0x1C) * 2;
            break;
        case 0x1C:
            object[1] = viewport[10] + pitch * *(s16 *)((u8 *)object + 0x1E) +
                        *(s16 *)((u8 *)object + 0x1C) * 4;
            break;
        default:
invalid_subraster:
            RW_ERROR(0x8000000C);
            return 0;
        }
    }

    viewport[9] = total;
    object[6] = pitch;
    ((u8 *)viewport)[0x33] = level;
    if ((flags & 2) != 0) {
        if ((((u8 *)object)[0x20] & 7) == 5) {
            DCFlushRange((void *)viewport[9], fn_80495428(object, level));
        }
        ((u8 *)object)[0x22] |= 2;
        ((u8 *)base)[0x22] |= 2;
        if (external == 0) {
            fn_804959C0((void *)viewport[10], (void *)viewport[9], base[3], base[4], object[5], object[6]);
        }
    }
    if ((flags & 1) != 0) {
        ((u8 *)object)[0x22] |= 4;
        ((u8 *)base)[0x22] |= 4;
        if (*(u16 *)((u8 *)viewport + 0x30) == lbl_805EDF40) {
            fn_8039561C(*(volatile u16 *)&lbl_805EDF40);
            lbl_805EDF40 = (lbl_805EDF40 + 1) % 0xE000;
        }
        while (fn_8049A78C(*(u16 *)((u8 *)viewport + 0x30)) == 0) {}
    }
    if (external != 0) {
        ((u8 *)object)[0x22] |= 0x20;
        ((u8 *)base)[0x22] |= 0x20;
    }
    break;
    }
    case 3:
    default:
        RW_ERROR(0x8000000E);
        return 0;
    }
    out[0] = object[1];
    return 1;
}

u32 fn_804960B4(u32 param1, u32 *object) {
    s32 kind = ((u8 *)object)[0x20] & 7;
    u32 *base = (u32 *)object[0];
    u32 *viewport = (u32 *)((u8 *)base + lbl_805F1658);
    switch (kind) {
    case 0:
    case 4:
    case 5: {
        if ((((u8 *)object)[0x22] & 4) != 0) {
            if ((((u8 *)object)[0x22] & 0x20) == 0) {
                fn_8049572C((void *)viewport[9], viewport[10], (u8 *)base[3], base[4], object[5], object[6]);
            }
            DCFlushRange((void *)viewport[9], fn_80495428(object, ((u8 *)viewport)[0x33]));
            fn_80397774();
        }
        if (base == object) {
            object[3] = object[10];
            object[4] = object[11];
        }
        if ((((u8 *)object)[0x22] & 0x20) == 0) {
            RW_API(void (*)(void *), 0x10C)((void *)viewport[10]);
            viewport[10] = 0;
        }
        object[6] = 0;
        object[1] = 0;
        if ((((u8 *)object)[0x22] & 4) != 0 && (((u8 *)object)[0x23] & 0x10) != 0 &&
            ((u8 *)viewport)[0x33] == 0) {
            ((u8 *)viewport)[0x33] = 0xFF;
            ((u8 *)object)[0x22] &= -0x27;
            ((u8 *)base)[0x22] &= -0x27;
            fn_8048D2C0(object, 0);
        } else {
            ((u8 *)viewport)[0x33] = 0xFF;
            ((u8 *)object)[0x22] &= -0x27;
            ((u8 *)base)[0x22] &= -0x27;
        }
        break;
    }
    case 3:
    default:
        RW_ERROR(0x80000011);
        return 0;
    }
    return 1;
}

u32 fn_80496268(u32 *out, u32 *object, u32 mode) {
    s32 kind = ((u8 *)object)[0x20] & 7;
    switch (kind) {
    case 0:
    case 4:
        if (object == (u32 *)object[0] && object[2] == 0) {
            u32 *palette = (u32 *)((u8 *)object + lbl_805F1658);
            if ((mode & 2) != 0) ((u8 *)object)[0x22] |= 8;
            if ((mode & 1) != 0) ((u8 *)object)[0x22] |= 0x10;
            object[2] = palette[8];
            out[0] = object[2];
        }
        return 1;
    case 3:
    case 5:
    default:
        RW_ERROR(0x8000000E);
        return 0;
    }
}

u32 fn_80496338(u32 param1, u32 *object) {
    s32 kind = ((u8 *)object)[0x20] & 7;
    switch (kind) {
    case 0:
    case 4:
        if (object == (u32 *)object[0]) {
            u32 *palette = (u32 *)((u8 *)object + lbl_805F1658);
            if ((((u8 *)object)[0x22] & 0x10) != 0) {
                DCFlushRange((void *)palette[8], (1 << object[5]) * 2);
            }
            ((u8 *)object)[0x22] &= ~0x18;
            object[2] = 0;
        }
        return 1;
    case 3:
    case 5:
    default:
        RW_ERROR(0x80000011);
        return 0;
    }
}

u32 fn_80496408(u32 *object, u32 format) {
    u32 *viewport = (u32 *)(object[0] + lbl_805F1658);
    u32 other = format & 0xFF00;
    s32 sub;
    ((u8 *)object)[0x20] = format & 7;
    ((u8 *)object)[0x21] = format & 0xF8;
    u8 kind = ((u8 *)object)[0x20];

    switch (kind) {
    case 0:
    case 4:
        goto common;
    case 5:
        if ((other & 0x6000) != 0) {
            RW_ERROR_STRING(2, lbl_8056E7B0);
            return 0;
        }
        goto common;
    case 1:
    case 2:
        goto low_kind;
    case 3:
    default:
        goto kind_error;
    }

common:
    sub = other & 0xF00;
    if (sub != 0) goto dispatch;

case0: {
        switch (object[5]) {
        case 0:
            if (kind == 5) {
                viewport[3] = 4;
                other |= 0x200;
                viewport[5] = 0;
                object[5] = 0x10;
            } else {
                other |= 0x100;
                viewport[3] = 5;
                viewport[5] = 1;
                if ((other & 0x4000) != 0) object[5] = 4;
                else if ((other & 0x2000) != 0) object[5] = 8;
                else object[5] = 0x10;
            }
            break;
        case 1:
            goto invalid_format;
        case 4:
            if ((other & 0x4000) != 0) {
                viewport[3] = 8;
                other |= 0x100;
                viewport[4] = 2;
                viewport[5] = 1;
            } else {
                viewport[3] = 0;
                viewport[5] = 0;
            }
            break;
        case 8:
            if ((other & 0x2000) != 0) {
                viewport[3] = 9;
                other |= 0x100;
                viewport[4] = 2;
                viewport[5] = 1;
            } else {
                viewport[3] = 1;
                viewport[5] = 0;
            }
            break;
        case 0x10:
            viewport[3] = 5;
            other |= 0x100;
            viewport[5] = 1;
            break;
        case 0x20:
            viewport[3] = 6;
            other |= 0x500;
            viewport[5] = 1;
            break;
        default:
invalid_format:
            RW_ERROR(0x8000000C);
            return 0;
        }
        goto done;
    }
dispatch:
    if (sub == 0x600) goto case600;
    if (sub < 0x600) {
        if (sub == 0x300) goto case300;
        if (sub < 0x300) {
            if (sub == 0x200) goto case200;
            if (sub < 0x200) {
                if (sub == 0x100) goto case100;
            }
            goto invalid_sub;
        }
        if (sub == 0x500) goto case500;
        if (sub < 0x500) {
            if (sub == 0x400) goto case400;
        }
        goto invalid_sub;
    }
    if (sub == 0x900) goto case900;
    if (sub < 0x900) {
        if (sub == 0x800) goto case800;
        if (sub < 0x800) {
            if (sub == 0x700) goto case700;
        }
        goto invalid_sub;
    }
    if (sub == 0xA00) goto caseA00;
    if (sub != 0xA00) goto invalid_sub;

caseA00: {
        if ((other & 0x4000) != 0) {
            viewport[3] = 8;
            viewport[4] = 2;
            object[5] = 4;
        } else if ((other & 0x2000) != 0) {
            viewport[3] = 9;
            viewport[4] = 2;
            object[5] = 8;
        } else {
            viewport[3] = 5;
            object[5] = 0x10;
        }
        viewport[5] = 0;
        goto done;
    }
case200: {
        if ((other & 0x4000) != 0) {
            viewport[3] = 8;
            viewport[4] = 1;
            object[5] = 4;
        } else if ((other & 0x2000) != 0) {
            viewport[3] = 9;
            viewport[4] = 1;
            object[5] = 8;
        } else {
            viewport[3] = 4;
            object[5] = 0x10;
        }
        viewport[5] = 0;
        goto done;
    }
case100:
case300: {
        if ((other & 0x4000) != 0) {
            viewport[3] = 8;
            viewport[4] = 2;
            object[5] = 4;
        } else if ((other & 0x2000) != 0) {
            viewport[3] = 9;
            viewport[4] = 2;
            object[5] = 8;
        } else {
            viewport[3] = 5;
            object[5] = 0x10;
        }
        viewport[5] = 1;
        goto done;
    }
case600: {
        if ((other & 0x6000) != 0) {
            RW_ERROR_STRING(2, s_rwRASTERFORMAT888_invalid_format_8056e7e0);
            return 0;
        }
        viewport[3] = 6;
        viewport[5] = 0;
        object[5] = 0x20;
        goto done;
    }
case500: {
        if ((other & 0x6000) != 0) {
            RW_ERROR_STRING(2, s_rwRASTERFORMAT8888_invalid_forma_8056e804);
            return 0;
        }
        viewport[3] = 6;
        viewport[5] = 1;
        object[5] = 0x20;
        goto done;
    }
case400: {
        RW_ERROR_STRING(2, s_rwRASTERFORMATLUM8_invalid_forma_8056e828);
        return 0;
    }
case700: {
        RW_ERROR_STRING(2, s_rwRASTERFORMAT16_invalid_format_8056e84c);
        return 0;
    }
case800: {
        RW_ERROR_STRING(2, s_rwRASTERFORMAT24_invalid_format_8056e86c);
        return 0;
    }
case900: {
        RW_ERROR_STRING(2, s_rwRASTERFORMAT32_invalid_format_8056e88c);
        return 0;
    }
invalid_sub:
    RW_ERROR(0x8000000D);
    return 0;

low_kind:
    if ((other & 0xF00) == 0) {
        if (lbl_805F164C[0x19] != 0) {
            viewport[3] = 4;
            other |= 0x200;
            viewport[5] = 0;
            object[5] = 0x10;
        } else {
            viewport[3] = 6;
            other |= 0x600;
            viewport[5] = 0;
            object[5] = 0x20;
        }
        goto done;
    }
    if (other == 0x700 && lbl_805F164C[0x19] != 0) {
        if (other != 0x800) goto done;
        if (lbl_805F164C[0x19] != 0) goto done;
    }
    RW_ERROR(0x8000000D);
    return 0;

kind_error:
    RW_ERROR(0x8000000D);
    return 0;

done:
    ((u8 *)object)[0x23] = (other >> 8) & 0xFF;
    return 1;
}

u32 fn_80496A20(u32 *object, s8 level) {
    u32 total = 0;
    u32 *viewport = (u32 *)((u8 *)object + lbl_805F1658);
    ((u8 *)viewport)[0x32] = level - 1;
    u8 value = *(u8 *)(object[0] + lbl_805F1658 + 0x32);
    volatile u32 levels;
    if (value != 0xFF) {
        levels = value + 1;
    } else {
        u32 computed;
        if (((((u8 *)object)[0x23] << 8) & 0x8000) != 0) {
            if ((s32)object[3] > (s32)object[4]) {
                computed = ((fn_80491AB4(object[3]) & 0xFF) + 1) & 0xFF;
            } else {
                computed = ((fn_80491AB4(object[4]) & 0xFF) + 1) & 0xFF;
            }
        } else {
            computed = 1;
        }
        levels = computed & 0xFF;
    }

    while ((s32)levels-- != 0) {
        total += fn_80495428(object, levels & 0xFF);
    }

    if (((((u8 *)object)[0x23] << 8) & 0x6000) != 0) {
        u32 bytes = total + (1 << object[5]) * 2 + 0x1F;
        viewport[6] = (u32)RW_API(void *(*)(u32, u32), 0x108)(bytes, 0x30411);
        if (viewport[6] == 0) {
            RW_ERROR_SIZED(0x80000013, bytes);
            return 0;
        }
        viewport[7] = (viewport[6] + 0x1F) & ~0x1F;
        viewport[8] = viewport[7] + total;
        fn_8039744C((u32)viewport, (void *)viewport[8], (void *)viewport[4], (1 << object[5]) & 0xFFFF);
    } else {
        viewport[6] = (u32)RW_API(void *(*)(u32, u32), 0x108)(total + 0x1F, 0x30411);
        if (viewport[6] == 0) {
            RW_ERROR_SIZED(0x80000013, total + 0x1F);
            return 0;
        }
        viewport[7] = (viewport[6] + 0x1F) & ~0x1F;
        if (((u8 *)object)[0x20] == 5) DCInvalidateRange((void *)viewport[7], total);
    }
    return 1;
}

u32 fn_80496C58(u32 param1, u32 *object, u32 format) {
    u32 *viewport = (u32 *)((u8 *)object + lbl_805F1658);
    object[6] = 0;
    viewport[3] = 0xFF;
    viewport[4] = 0xFF;
    viewport[5] = 0;
    viewport[6] = 0;
    viewport[7] = 0;
    viewport[8] = 0;
    viewport[9] = 0;
    viewport[10] = 0;
    viewport[11] = 0;
    *(u16 *)((u8 *)viewport + 0x30) = lbl_805F1678;
    ((u8 *)viewport)[0x32] = 0xFF;
    ((u8 *)viewport)[0x33] = 0xFF;
    if ((s32)fn_80496408(object, format) == 0) return 0;
    if ((s32)object[3] == 0 || (s32)object[4] == 0) goto zero_dimension;

    u8 kind = ((u8 *)object)[0x20];
    u32 success;
    switch (kind) {
    case 0:
    case 4:
    case 5: {
        if ((((u8 *)object)[0x21] & 0x80) != 0) goto return_success;

        u32 level;
        if (((((u8 *)object)[0x23] << 8) & 0x8000) == 0) {
            level = 1;
        } else if ((s32)object[3] > (s32)object[4]) {
            level = ((fn_80491AB4(object[3]) & 0xFF) + 1) & 0xFF;
        } else {
            level = ((fn_80491AB4(object[4]) & 0xFF) + 1) & 0xFF;
        }

        u32 total = 0;
        viewport = (u32 *)((u8 *)object + lbl_805F1658);
        ((u8 *)viewport)[0x32] = level - 1;
        u8 value = *(u8 *)(object[0] + lbl_805F1658 + 0x32);
        volatile u32 levels;
        if (value != 0xFF) {
            levels = value + 1;
        } else {
            u32 computed;
            if (((((u8 *)object)[0x23] << 8) & 0x8000) != 0) {
                if ((s32)object[3] > (s32)object[4]) {
                    computed = ((fn_80491AB4(object[3]) & 0xFF) + 1) & 0xFF;
                } else {
                    computed = ((fn_80491AB4(object[4]) & 0xFF) + 1) & 0xFF;
                }
            } else {
                computed = 1;
            }
            levels = computed & 0xFF;
        }

        while ((s32)levels-- != 0) {
            total += fn_80495428(object, levels & 0xFF);
        }

        if (((((u8 *)object)[0x23] << 8) & 0x6000) != 0) {
            u32 bytes = total + (1 << object[5]) * 2 + 0x1F;
            viewport[6] = (u32)RW_API(void *(*)(u32, u32), 0x108)(bytes, 0x30411);
            if (viewport[6] == 0) {
                RW_ERROR_SIZED(0x80000013, bytes);
                success = 0;
                goto allocation_done;
            }
            viewport[7] = (viewport[6] + 0x1F) & ~0x1F;
            viewport[8] = viewport[7] + total;
            fn_8039744C((u32)viewport, (void *)viewport[8], (void *)viewport[4], (1 << object[5]) & 0xFFFF);
        } else {
            viewport[6] = (u32)RW_API(void *(*)(u32, u32), 0x108)(total + 0x1F, 0x30411);
            if (viewport[6] == 0) {
                RW_ERROR_SIZED(0x80000013, total + 0x1F);
                success = 0;
                goto allocation_done;
            }
            viewport[7] = (viewport[6] + 0x1F) & ~0x1F;
            if (((u8 *)object)[0x20] == 5) DCInvalidateRange((void *)viewport[7], total);
        }
        success = 1;
allocation_done:
        if ((s32)success != 0) goto return_success;
        RW_ERROR_STRING(2, lbl_8056E930);
        return 0;
    }
    case 1:
    case 2:
        ((u8 *)object)[0x21] = 0x80;
        goto return_success;
    case 3:
    default:
        RW_ERROR(0x8000000D);
        return 0;
    }

zero_dimension:
    ((u8 *)object)[0x21] = 0x80;
    return 1;

return_success:
    return 1;
}

u32 fn_80497008(u32 param1, u32 *object) {
    if ((u32 *)object[0] == object && (((u8 *)object)[0x21] & 0x80) == 0) {
        u8 kind = ((u8 *)object)[0x20];
        switch (kind) {
        case 1:
        case 2:
            break;
        case 0:
        case 4:
        case 5: {
            u32 *viewport = (u32 *)((u8 *)object + lbl_805F1658);
            if (*(u16 *)((u8 *)viewport + 0x30) == lbl_805EDF40) {
                fn_8039561C(*(volatile u16 *)&lbl_805EDF40);
                lbl_805EDF40 = (lbl_805EDF40 + 1) % 0xE000;
            }
            while (fn_8049A78C(*(u16 *)((u8 *)viewport + 0x30)) == 0) {}
            if (lbl_805F1664 != NULL && object == (u32 *)*lbl_805F1664) {
                fn_8049714C(lbl_805F1664, 0, 0);
            }
            RW_API(void (*)(void *), 0x10C)(*(void **)((u8 *)viewport + 0x18));
            break;
        }
        case 3:
        default:
            RW_ERROR(0x8000000D);
            return 0;
        }
    }
    return 1;
}

u32 fn_8049714C(u32 *object, u32 value, u32 param3) {
    object[0] = value;
    *(u32 *)((u8 *)object + lbl_805F1670 + 0x20) = 0x1000000;
    return 1;
}

u32 fn_80497168(u8 *dest, u8 *source) {
    *(u32 *)(dest + 0x18) = *(u32 *)(source + 0x18);
    *(u32 *)(dest + 0x14) = *(u32 *)(source + 0x14);
    dest[0x20] = source[0x20];
    dest[0x23] = source[0x23];
    *(u32 *)(dest + 4) = 0;
    return 1;
}

void fn_80497198(void) {
    volatile u32 color = lbl_805FB3D0;
    f32 v2 = lbl_805FB3DC;
    f32 v1 = lbl_805FB3D8;
    f32 v0 = lbl_805FB3D4;
    lbl_805E3210[4] = 0;
    lbl_805E3210[5] = 1;
    lbl_805E3210[6] = 0;
    ((u8 *)lbl_805E3210)[0x1C] = 0;
    ((u8 *)lbl_805E3210)[0x1D] = 0;
    ((u8 *)lbl_805E3210)[0x1E] = 0;
    ((u8 *)lbl_805E3210)[0x1F] = 0;
    ((f32 *)lbl_805E3210)[9] = v0;
    ((f32 *)lbl_805E3210)[0xA] = v1;
    ((f32 *)lbl_805E3210)[0xB] = v2;
    ((f32 *)lbl_805E3210)[0xC] = v1;
    lbl_805E3210[8] = 0;
    lbl_805E3210[0] = 1;
    lbl_805E3210[1] = 1;
    lbl_805E3210[0xF] = 1;
    lbl_805E3210[0xD] = 5;
    lbl_805E3210[0xE] = 6;
    lbl_805E3210[3] = 2;
    lbl_805E3210[2] = 3;
    fn_80398DAC(1, 3, 1);
    fn_80398DE0(1);
    lbl_805E3210[0x10] = 4;
    ((u8 *)lbl_805E3210)[0x4C] = 0;
    lbl_805E3210[0x12] = 0;
    lbl_805E3210[0x11] = 7;
    ((u8 *)lbl_805E3210)[0x4D] = 0;
    ((u8 *)lbl_805E3210)[0x4E] = 0;
    fn_80398734(7, 0, 0, 7, 0);
    fn_80398D00(1, 4, 5, 0);
    fn_80395D48(1);
    fn_80398D00(1, 4, 5, 0);
    fn_80396C78(2, 0, 0, 0, 0, 0, 2);
    fn_80396C78(3, 0, 0, 0, 0, 0, 2);
    fn_80396C78(0, 0, 0, 0, 0, 0, 2);
    fn_80396C78(1, 0, 0, 0, 0, 0, 2);
    u32 color2 = color;
    fn_80396B54(2, &color2);
    u32 color3 = color;
    fn_80396B54(3, &color3);
    u32 color4 = color;
    fn_80396B54(0, &color4);
    u32 color5 = color;
    fn_80396B54(1, &color5);
    fn_80398D54(1);
    fn_80398D80(1);
    u32 color6 = color;
    fn_8039616C(&color6, 0xFFFFFF);
    fn_80399288(0);
    fn_80499BC8();
    lbl_805F1664 = (u32 *)fn_8048CCDC(0);
    lbl_805F1664[0x14] = (lbl_805F1664[0x14] & 0xFFFFFF00) | 2;
    lbl_805F1664[0x14] = (lbl_805F1664[0x14] & 0xFFFF00FF) | 0x1100;
    lbl_805F1660 = (u32 *)fn_80489E74(4, 4, (void *)0x10, (void *)0x204);
    void *buffer = (void *)fn_80489F70(lbl_805F1660, 0, 9);
    memset(buffer, 0xFF, 0x20);
    fn_80489B40(lbl_805F1660);
}

void fn_8049742C(void) {
    fn_80489BDC(lbl_805F1660);
    lbl_805F1660 = NULL;
    fn_8048CA00(lbl_805F1664, 0);
    fn_8048CD8C(lbl_805F1664);
    lbl_805F1664 = NULL;
}
}

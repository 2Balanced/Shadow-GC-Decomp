/* RenderWare pixel formats, palette conversion, and texture packet writers, 0x80490E30..0x804929EF.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u32 *lbl_805F1664;
extern u32 lbl_8056E060[];
extern u32 lbl_8056E0C8;
extern u32 lbl_8056E0F0;
extern u32 lbl_8056E1A8;

u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
void fn_8049043C(void);
void fn_80490724(void);
void fn_80395B28(u32, u32, u32);

#define RW_ERROR(value)                                                                          \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28(value);                                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)
#define RW_ERROR_STRING(value, text)                                                             \
    do {                                                                                         \
        u32 error[2];                                                                            \
        error[0] = 1;                                                                            \
        error[1] = fn_8047DC28((value), (u32)&(text));                                           \
        fn_8047DB84(error);                                                                      \
    } while (0)

static inline void rwWriteTexture55(u32 *entry) {
    if (*lbl_805F1664 == 0) {
        *(volatile f32 *)0xCC008000 = *(f32 *)(entry + 2);
        *(volatile f32 *)0xCC008004 = *(f32 *)(entry + 3);
        *(volatile f32 *)0xCC008008 = *(f32 *)(entry + 4);
        *(volatile u8 *)0xCC00800C = ((u8 *)(entry + 3))[0];
        *(volatile u8 *)0xCC00800D = ((u8 *)(entry + 3))[1];
        *(volatile u8 *)0xCC00800E = ((u8 *)(entry + 3))[2];
        *(volatile u8 *)0xCC00800F = ((u8 *)(entry + 3))[3];
        *(volatile f32 *)0xCC008010 = *(f32 *)(entry + 4);
        *(volatile f32 *)0xCC008014 = *(f32 *)(entry + 5);
    } else {
        *(volatile u32 *)0xCC008000 = entry[5];
    }
}

u32 fn_80490E30(u32 type, u8 *data, u32 count) {
    fn_8049043C();
    fn_80395B28(lbl_8056E060[type], 0, count & 0xFFFF);
    if (type == 3) {
        u32 entries = count / 3;
        while (entries-- != 0) {
            if (*lbl_805F1664 == 0) rwWriteTexture55((u32 *)(data + 0x38));
            else *(volatile u32 *)0xCC008000 = *(u32 *)(data + 0x44);
            data += 0x48;
        }
    } else if (type == 1) {
        u32 entries = count >> 1;
        while (entries-- != 0) {
            if (*lbl_805F1664 == 0) rwWriteTexture55((u32 *)(data + 0x20));
            else *(volatile u32 *)0xCC008000 = *(u32 *)(data + 0x2C);
            data += 0x30;
        }
    } else if (type == 2 || type == 4 || type == 5) {
        u32 entries = count >> 1;
        while (entries-- != 0) {
            if (*lbl_805F1664 == 0) rwWriteTexture55((u32 *)(data + 0x20));
            else *(volatile u32 *)0xCC008000 = *(u32 *)(data + 0x2C);
            data += 0x30;
        }
        if ((count & 1) != 0) {
            if (*lbl_805F1664 == 0) rwWriteTexture55((u32 *)(data + 8));
            else *(volatile u32 *)0xCC008000 = *(u32 *)(data + 0x14);
        }
    } else {
        RW_ERROR(0x25);
    }
    fn_80490724();
    return 1;
}

u32 fn_8049140C(u32 type, u32 *textures, u32 unused, u16 *indices, u32 count) {
    fn_8049043C();
    fn_80395B28(lbl_8056E060[type], 0, count & 0xFFFF);
    if (type == 3) {
        u32 entries = count / 3;
        while (entries-- != 0) {
            u32 *entry = (u32 *)((u8 *)textures + indices[1] * 0x18);
            rwWriteTexture55(entry);
            indices += 3;
        }
    } else if (type == 1) {
        u32 entries = count >> 1;
        while (entries-- != 0) {
            u32 *entry = (u32 *)((u8 *)textures + indices[1] * 0x18);
            rwWriteTexture55(entry);
            indices += 2;
        }
    } else if (type == 2 || type == 4 || type == 5) {
        u32 entries = count >> 1;
        while (entries-- != 0) {
            u32 *entry = (u32 *)((u8 *)textures + indices[1] * 0x18);
            rwWriteTexture55(entry);
            indices += 2;
        }
        if ((count & 1) != 0) rwWriteTexture55((u32 *)((u8 *)textures + indices[0] * 0x18));
    } else {
        RW_ERROR(0x25);
    }
    fn_80490724();
    return 1;
}

s32 fn_80491AB4(s32 value) {
    s32 result = -1;
    for (; value != 0; value >>= 1) result++;
    return result;
}

u32 fn_80491AD4(const u8 *color) {
    u32 red = color[0];
    u32 green = color[1];
    u32 blue = color[2];
    u32 packed = ((red & 0xF8) << 7) | 0x8000;
    packed = __rlwimi(packed, green, 2, 22, 26);
    return __rlwimi(packed, blue, 29, 27, 31);
}

u32 fn_80491AF4(const u8 *color) {
    return __rlwimi(
        __rlwimi((u32)(color[1] & 0xFC) << 3, color[0], 8, 16, 20),
        color[2], 29, 27, 31);
}

u32 fn_80491B14(const u8 *color) {
    if (color[3] != 0xFF) {
        return (u32)(color[2] >> 4) | (color[1] & 0xF0) |
               ((u32)(color[3] & 0xE0) << 7) | ((u32)(color[0] & 0xF0) << 4);
    }
    return (u32)(color[2] >> 3) | ((u32)(color[1] & 0xF8) << 2) |
           ((u32)(color[0] & 0xF8) << 7) | 0x8000;
}

u32 fn_80491B64(const u8 *color) {
    return (u32)color[2] | ((u32)color[0] << 16) | 0xFF000000 | ((u32)color[1] << 8);
}

u32 fn_80491B88(const u8 *color) {
    return (u32)color[2] | ((u32)color[1] << 8) | ((u32)color[3] << 24) | ((u32)color[0] << 16);
}

u32 fn_80491BAC(u32 *out, const u8 *color, u32 format) {
    switch (format & 0xF00) {
    case 0x000:
    case 0x200: *out = fn_80491AF4(color); break;
    case 0x100:
    case 0x300: *out = fn_80491B14(color); break;
    case 0x400: RW_ERROR_STRING(2, lbl_8056E0C8); break;
    case 0x500: *out = fn_80491B88(color); break;
    case 0x600: *out = fn_80491B64(color); break;
    case 0xA00: *out = fn_80491AD4(color); break;
    default: RW_ERROR(0x8000000D); break;
    }
    return 1;
}

void fn_80491D98(u8 *out, u32 packed) {
    out[0] = (packed >> 7) & 0xF8;
    out[1] = (packed >> 2) & 0xF8;
    out[2] = packed << 3;
    out[3] = 0xFF;
}

void fn_80491DBC(u8 *out, u32 packed) {
    out[0] = (packed >> 8) & 0xF8;
    out[1] = (packed >> 3) & 0xFC;
    out[2] = packed << 3;
    out[3] = 0xFF;
}

void fn_80491DE0(u8 *out, u32 packed) {
    if ((packed & 0x8000) != 0) {
        out[0] = (packed >> 7) & 0xF8;
        out[1] = (packed >> 2) & 0xF8;
        out[2] = packed << 3;
        out[3] = 0xFF;
    } else {
        out[0] = (packed >> 4) & 0xF0;
        out[1] = packed & 0xF0;
        out[2] = packed << 4;
        out[3] = 0;
    }
}

void fn_80491E30(u8 *out, u32 packed) {
    if ((packed & 0x8000) != 0) {
        out[0] = (packed >> 7) & 0xF8;
        out[1] = (packed >> 2) & 0xF8;
        out[2] = packed << 3;
        out[3] = 0xFF;
    } else {
        out[0] = (packed >> 4) & 0xF0;
        out[1] = packed & 0xF0;
        out[2] = packed << 4;
        out[3] = (packed >> 7) & 0xE0;
    }
}

void fn_80491E80(u8 *out, u32 packed) {
    out[3] = 0xFF;
    out[0] = (packed >> 8) & 0xFF;
    out[1] = (packed >> 16) & 0xFF;
    out[2] = packed & 0xFF;
}

void fn_80491EA0(u8 *out, u32 packed) {
    out[3] = (packed >> 24) & 0xFF;
    out[0] = (packed >> 8) & 0xFF;
    out[1] = (packed >> 16) & 0xFF;
    out[2] = packed & 0xFF;
}

u32 fn_80491EC0(u8 *out, const u32 *color, u32 format) {
    u32 packed = *color;
    switch (format & 0xF00) {
    case 0x000: fn_80491DBC(out, packed); break;
    case 0x100: fn_80491DE0(out, packed); break;
    case 0x200: fn_80491DBC(out, packed); break;
    case 0x300: fn_80491E30(out, packed); break;
    case 0x400: RW_ERROR_STRING(2, lbl_8056E0C8); break;
    case 0x500: fn_80491EA0(out, packed); break;
    case 0x600: fn_80491E80(out, packed); break;
    case 0xA00: fn_80491D98(out, packed); break;
    default: RW_ERROR(0x8000000D); break;
    }
    return 1;
}

static void (*rwReader(u32 format))(u8 *, u32) {
    switch (format & 0xF) {
    case 1: return fn_80491DE0;
    case 2: return fn_80491DBC;
    case 3: return fn_80491E30;
    case 5: return fn_80491EA0;
    case 6: return fn_80491E80;
    case 10: return fn_80491D98;
    case 4: RW_ERROR_STRING(2, lbl_8056E0C8); break;
    default: RW_ERROR(0x8000000D); break;
    }
    return NULL;
}

void fn_804920C8(u8 *dest, u8 *source) {
    void (*reader)(u8 *, u32) = rwReader(source[0x23]);
    if (*(u32 *)(source + 0x14) != 4) {
        RW_ERROR_STRING(2, lbl_8056E0F0);
        return;
    }
    u8 *palette = *(u8 **)(source + 0x18);
    for (u32 i = 0; i < 0x10; i++) reader(palette + i * 4, *(u16 *)(*(u32 *)(source + 8) + i * 2));
    for (u32 row = 0; row < *(u32 *)(source + 0x10); row++) {
        u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
        u8 *output = *(u8 **)(dest + 4) + *(u32 *)(dest + 0x18) * row;
        for (u32 x = 0; x < *(u32 *)(source + 0xC); x += 2) {
            output[0] = input[0] >> 4;
            output[1] = input[0] & 0xF;
            input++;
            output += 2;
        }
    }
}

void fn_8049231C(u8 *dest, u8 *source) {
    void (*reader)(u8 *, u32) = rwReader(source[0x23]);
    u32 kind = *(u32 *)(source + 0x14);
    if (kind == 4) {
        u8 *palette = *(u8 **)(source + 0x18);
        for (u32 i = 0; i < 0x10; i++) reader(palette + i * 4, *(u16 *)(*(u32 *)(source + 8) + i * 2));
        for (u32 row = 0; row < *(u32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u8 *output = *(u8 **)(dest + 4) + *(u32 *)(dest + 0x18) * row;
            for (u32 x = 0; x < *(u32 *)(source + 0xC); x += 2) {
                output[0] = input[0] >> 4;
                output[1] = input[0] & 0xF;
                input++;
                output += 2;
            }
        }
    } else if (kind == 8) {
        u8 *palette = *(u8 **)(source + 0x18);
        for (u32 i = 0; i < 0x100; i++) reader(palette + i * 4, *(u16 *)(*(u32 *)(source + 8) + i * 2));
        for (u32 row = 0; row < *(u32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u8 *output = *(u8 **)(dest + 4) + *(u32 *)(dest + 0x18) * row;
            for (u32 x = 0; x < *(u32 *)(source + 0xC); x++) *output++ = *input++;
        }
    } else {
        RW_ERROR_STRING(2, lbl_8056E1A8);
    }
}

void fn_80492610(u8 *dest, u8 *source) {
    void (*reader)(u8 *, u32) = rwReader(source[0x23]);
    u32 kind = *(u32 *)(source + 0x14);
    if (kind == 4) {
        u32 palette[16];
        for (u32 i = 0; i < 0x10; i++) reader((u8 *)(palette + i), *(u16 *)(*(u32 *)(source + 8) + i * 2));
        for (u32 row = 0; row < *(u32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u32 *output = (u32 *)(*(u32 *)(dest + 4) + *(u32 *)(dest + 0x18) * row);
            for (u32 x = 0; x < *(u32 *)(source + 0xC); x += 2) {
                output[0] = palette[input[0] >> 4];
                output[1] = palette[input[0] & 0xF];
                input++;
                output += 2;
            }
        }
    } else if (kind == 8) {
        u32 palette[256];
        for (u32 i = 0; i < 0x100; i++) reader((u8 *)(palette + i), *(u16 *)(*(u32 *)(source + 8) + i * 2));
        for (u32 row = 0; row < *(u32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u32 *output = (u32 *)(*(u32 *)(dest + 4) + *(u32 *)(dest + 0x18) * row);
            for (u32 x = 0; x < *(u32 *)(source + 0xC); x++) *output++ = palette[*input++];
        }
    } else if (kind == 0x10 || kind == 0x20) {
        for (u32 row = 0; row < *(u32 *)(source + 0x10); row++) {
            u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u8 *output = *(u8 **)(dest + 4) + *(u32 *)(dest + 0x18) * row;
            for (u32 x = 0; x < *(u32 *)(source + 0xC); x++) {
                reader(output, *(u32 *)input);
                input += 4;
                output += 4;
            }
        }
    } else {
        RW_ERROR(0x8000000C);
    }
}
}

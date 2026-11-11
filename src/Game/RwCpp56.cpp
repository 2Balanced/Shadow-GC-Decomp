/* RenderWare texture conversion and video-mode helpers, 0x804929F0..0x80493E8B.
 * Names are inferred from behavior; definitions remain in retail address order. */
#include <dolphin/types.h>

extern "C" {
extern s32 lbl_805F1658;
extern u32 lbl_8056E0C8;
extern f32 lbl_805FB3A8;
extern u32 s_Invalid_pixel_format_8056e3a8;
extern u32 s_Invalid_TV_format_8056e3c0;

u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
u32 fn_80489F70(void *, u32, u32);
u32 fn_80489C94(void *, u32);
void fn_80489B88(void *);
void fn_80489B40(void *);
void fn_8049231C(void *, void *);
void fn_804920C8(void *, void *);
void fn_80492610(void *, void *);
void *fn_80487D08(u32, u32, u32);
void fn_80487E18(void);
int fn_8048FED8(void *);
void fn_8048DEB0(void *, f32, void *);
void fn_8048F5D0(u32, u32, void *);
void fn_8048FAB0(u32, u32, u32, u32, void *, void *);
void fn_804903F8(void *);
void fn_80487DB0(void *);
u32 fn_80491AF4(const void *);
u32 fn_80491AD4(const void *);
u32 fn_80491B14(const void *);
u32 fn_80491B64(const void *);
u32 fn_80491B88(const void *);
int fn_8049A78C(u16);
int VIGetTvFormat(void);
void fn_804953F0(void);
void fn_80499BF8(void);

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

#define RW_SELECT_READER(object, reader)                                                          \
    do {                                                                                         \
        switch ((object)[0x23] & 0xF) {                                                            \
        case 1:                                                                                 \
        case 3: (reader) = fn_80491B14; break;                                                    \
        case 2: (reader) = fn_80491AF4; break;                                                    \
        case 5: (reader) = fn_80491B88; break;                                                    \
        case 6: (reader) = fn_80491B64; break;                                                    \
        case 10: (reader) = fn_80491AD4; break;                                                   \
        case 4: RW_ERROR_STRING(2, lbl_8056E0C8); break;                                                              \
        default: RW_ERROR(0x8000000D); break;                                                     \
        }                                                                                        \
    } while (0)

u32 fn_804929F0(u8 *object, u8 *source) {
    bool restore = false;
    bool unlock = (object[0x22] & 2) == 0;
    if (unlock) fn_80489F70(source, 0, 2);
    if ((object[0x23] & 0x60) != 0 && (object[0x22] & 8) == 0) {
        fn_80489C94(source, 2);
        restore = true;
    }
    u32 kind = *(u32 *)(object + 0xC);
    if (kind == 8) {
        fn_8049231C(object, source);
    } else if (kind == 4) {
        fn_804920C8(object, source);
    } else if (kind == 0x20) {
        fn_80492610(object, source);
    } else {
        RW_ERROR(0x80000008);
    }
    if (restore) fn_80489B88(source);
    if (unlock) fn_80489B40(source);
    return 1;
}

void *fn_80492B24(u8 *object, u32 flags) {
    void *result = fn_80487D08(*(u32 *)(object + 4), *(u32 *)(object + 8), flags);
    if (result == NULL) return NULL;
    u8 buffer[0x4000];
    fn_80487E18();
    if (fn_8048FED8(buffer) == 0) return NULL;
    fn_8048DEB0(buffer, lbl_805FB3A8, object);
    fn_8048F5D0(*(u32 *)((u8 *)result + 0x18), 1 << flags, buffer);
    fn_8048FAB0(*(u32 *)((u8 *)result + 0x14), *(u32 *)((u8 *)result + 0x10),
                *(u32 *)((u8 *)result + 0xC), 0, buffer, object);
    fn_804903F8(buffer);
    return result;
}

void fn_80492BE8(u32 *object, u8 *source) {
    u32 (*reader)(const void *) = NULL;
    RW_SELECT_READER((u8 *)object, reader);
    u8 *frame = (u8 *)(object[0] + lbl_805F1658);
    u32 kind = *(u32 *)(source + 0xC);
    switch (kind) {
    case 4:
        for (u32 row = 0; row < object[4]; row++) {
            u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u8 *output = (u8 *)object[1] + object[6] * row;
            for (u32 x = 0; x < object[3]; x += 2) {
                *output++ = (input[0] << 4) | (input[1] & 0xF);
                input += 2;
            }
        }
        if (*(s8 *)(frame + 0x33) == 0) {
            u16 *palettePtr = (u16 *)object[2];
            const u8 *paletteInput = *(u8 **)(source + 0x18);
            for (u32 i = 0; i < 0x10; i++) {
                *palettePtr++ = (u16)reader(paletteInput);
                paletteInput += 4;
            }
        }
        break;
    case 8:
    case 0x20: {
        void *nested = fn_80492B24(source, object[5]);
        if (nested != NULL) {
            fn_80492BE8(object, (u8 *)nested);
            fn_80487DB0(nested);
        }
        break;
    }
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

void fn_80492E40(u32 *object, u8 *source) {
    u32 (*reader)(const void *) = NULL;
    RW_SELECT_READER((u8 *)object, reader);
    u32 kind = *(u32 *)(source + 0xC);
    if (kind == 0x20) {
        void *nested = fn_80492B24(source, object[5]);
        if (nested != NULL) {
            fn_80492E40(object, (u8 *)nested);
            fn_80487DB0(nested);
        }
        return;
    }
    if (kind != 4 && kind != 8) {
        RW_ERROR(0x80000008);
        return;
    }
    for (u32 row = 0; row < object[4]; row++) {
        u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
        u8 *output = (u8 *)object[1] + object[6] * row;
        for (u32 x = 0; x < object[3]; x++) *output++ = *input++;
    }
    u8 *frame = (u8 *)(object[0] + lbl_805F1658);
    if (*(s8 *)(frame + 0x33) == 0) {
        u16 *palette = (u16 *)object[2];
        for (u32 i = 0; i < (1u << (kind & 0x3F)); i++) {
            palette[i] = (u16)reader(*(u8 **)(source + 0x18) + i * 4);
        }
    }
}

void fn_8049309C(u32 *object, u8 *source) {
    u32 (*reader)(const void *) = NULL;
    RW_SELECT_READER((u8 *)object, reader);
    u32 kind = *(u32 *)(source + 0xC);
    switch (kind) {
    case 4:
    case 8: {
        u16 palette[256];
        u16 *palettePtr = palette;
        const u8 *paletteInput = *(u8 **)(source + 0x18);
        for (u32 i = 0; i < (1u << (kind & 0x3F)); i++) {
            *palettePtr++ = (u16)reader(paletteInput);
            paletteInput += 4;
        }
        for (u32 row = 0; row < object[4]; row++) {
            u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u16 *output = (u16 *)(object[1] + object[6] * row);
            for (u32 x = 0; x < object[3]; x++) *output++ = palette[*input++];
        }
        break;
    }
    case 0x20:
        for (u32 row = 0; row < object[4]; row++) {
            const u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u16 *output = (u16 *)(object[1] + object[6] * row);
            for (u32 x = 0; x < object[3]; x++) {
                *output++ = (u16)reader(input);
                input += 4;
            }
        }
        break;
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

void fn_80493330(u32 *object, u8 *source) {
    u32 (*reader)(const void *) = NULL;
    RW_SELECT_READER((u8 *)object, reader);
    u32 kind = *(u32 *)(source + 0xC);
    switch (kind) {
    case 4:
    case 8: {
        u32 palette[256];
        u32 *palettePtr = palette;
        const u8 *paletteInput = *(u8 **)(source + 0x18);
        for (u32 i = 0; i < (1u << (kind & 0x3F)); i++) {
            *palettePtr++ = reader(paletteInput);
            paletteInput += 4;
        }
        for (u32 row = 0; row < object[4]; row++) {
            u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u32 *output = (u32 *)(object[1] + object[6] * row);
            for (u32 x = 0; x < object[3]; x++) *output++ = palette[*input++];
        }
        break;
    }
    case 0x20:
        for (u32 row = 0; row < object[4]; row++) {
            const u8 *input = *(u8 **)(source + 0x14) + *(u32 *)(source + 0x10) * row;
            u32 *output = (u32 *)(object[1] + object[6] * row);
            for (u32 x = 0; x < object[3]; x++) {
                *output++ = reader(input);
                input += 4;
            }
        }
        break;
    default:
        RW_ERROR(0x80000008);
        break;
    }
}

u32 fn_804935C4(u32 *object, u8 *source) {
    u8 flags = ((u8 *)object)[0x23];
    bool unlock = (((u8 *)object)[0x22] & 4) == 0;
    bool restore = false;
    if (unlock && (u32)fn_80489F70(object, 0, 5) == 0) return 0;
    if ((flags & 0x60) != 0 && (restore = (((u8 *)object)[0x22] & 0x10) != 0, !restore) &&
        (u32)fn_80489C94(object, 5) == 0) return 0;
    switch (object[5]) {
    case 4: fn_80492BE8(object, source); break;
    case 8: fn_80492E40(object, source); break;
    case 0x10: fn_8049309C(object, source); break;
    case 0x20: fn_80493330(object, source); break;
    case 0x18:
    default: RW_ERROR(0x8000000C); break;
    }
    if ((flags & 0x60) != 0 && !restore) fn_80489B88(object);
    if (unlock) fn_80489B40(object);
    return 1;
}

u32 fn_8049375C(u8 *source) {
    s32 kind = *(s32 *)(source + 0xC);
    s32 anyAlpha = 0;
    if (kind == 4 || kind == 8) {
        u8 *indices = *(u8 **)(source + 0x14);
        for (u32 row = 0; row < *(u32 *)(source + 8); row++) {
            u8 *input = indices;
            for (u32 x = 0; x < *(u32 *)(source + 4); x++) {
                u8 alpha = *(u8 *)(*(u32 *)(source + 0x18) + *input * 4 + 3);
                if (alpha != 0xFF) {
                    anyAlpha = true;
                    if (alpha > 0xF) return kind == 4 ? 0x4300 : 0x2300;
                }
                input++;
            }
            indices += *(u32 *)(source + 0x10);
        }
    } else {
        u8 *pixels = *(u8 **)(source + 0x14);
        for (u32 row = 0; row < *(u32 *)(source + 8); row++) {
            u8 *input = pixels;
            for (u32 x = 0; x < *(u32 *)(source + 4); x++) {
                u8 alpha = input[3];
                if (alpha != 0xFF) {
                    anyAlpha = true;
                    if (alpha > 0xF) return 0x300;
                }
                input += 4;
            }
            pixels += *(u32 *)(source + 0x10);
        }
    }
    u32 result = anyAlpha ? 0x100 : 0x200;
    if (kind == 4) result |= 0x4000;
    else if (kind == 8) result |= 0x2000;
    return result;
}

#pragma use_lmw_stmw off
u32 fn_80493888(u8 *object, u8 *source, u32 mode) {
    u32 originalMode = mode;
    mode = (u8)(mode & 7);
    object[0x20] = mode;
    *(u32 *)(object + 0x14) = 0;
    switch (mode) {
    case 0:
    case 4:
    case 5:
        goto common;
    case 1:
    case 2:
        goto direct;
    default:
        goto error;
    }
common:
    {
        s32 sourceWidth = *(s32 *)(source + 4);
        s32 width = 0x400;
        if (sourceWidth <= 0x400) width = sourceWidth;
        *(u32 *)(object + 0xC) = width;
        s32 sourceHeight = *(s32 *)(source + 8);
        s32 height = 0x400;
        if (sourceHeight <= 0x400) height = sourceHeight;
        *(u32 *)(object + 0x10) = height;
    }
    if ((originalMode & 0x8000) != 0) {
        s32 logWidth = -1;
        for (s32 value = *(s32 *)(object + 0xC); value != 0; value >>= 1) logWidth++;
        *(u32 *)(object + 0xC) = 1U << logWidth;
        s32 logHeight = -1;
        for (s32 value = *(s32 *)(object + 0x10); value != 0; value >>= 1) logHeight++;
        *(u32 *)(object + 0x10) = 1U << logHeight;
    }
    object[0x23] = (u8)((s32)((originalMode & 0x9000) | fn_8049375C(source)) >> 8);
    return 1;
direct:
    object[0x23] = 0;
    *(u32 *)(object + 0xC) = *(u32 *)(source + 4);
    *(u32 *)(object + 0x10) = *(u32 *)(source + 8);
    return 1;
error:
    RW_ERROR(0x8000000D);
    return 0;
}
#pragma use_lmw_stmw on

extern u32 lbl_805E3178[];
extern s32 lbl_805F1608;
extern s32 lbl_805F1604;
extern s32 lbl_805F1620;
extern s32 lbl_805EDF34;
extern s32 lbl_805F160C;
extern s32 lbl_805F1618;
extern s32 lbl_805F1630;
extern s32 lbl_805F15F0;
extern s32 lbl_805F161C;
extern s32 lbl_805F1628;
extern s32 lbl_805EDF38;
extern u8 lbl_805EDF39;
extern u16 lbl_805F1638[];
extern s32 lbl_805F1614;
extern s32 lbl_805F15F8;
extern s32 lbl_805F15FC;
extern s32 lbl_805EDF30;
extern s32 lbl_805F1624;

void fn_80394228(void);
void fn_80394174(u32);
void fn_80378568(void *);
void fn_8037F020(s32);
void fn_8037F094(s32);
void fn_8037EEF0(void);

void fn_804939DC(void) {
    if (*(s32 *)(lbl_805E3178 + lbl_805F1608 * 2) == lbl_805F1604) {
        lbl_805F1620 = 1;
        return;
    }
    s32 next = (lbl_805F1608 + 1) - ((lbl_805F1608 + 1) / lbl_805EDF34) * lbl_805EDF34;
    if (next == lbl_805F160C) {
        fn_80394228();
        lbl_805F1618 = 0;
    } else {
        fn_80394174(lbl_805E3178[next * 2 + 1]);
    }
    lbl_805F1608 = next;
    fn_80378568(&lbl_805F1630);
    if (lbl_805F15F0 == 0) {
        lbl_805F161C = 1;
    } else if (lbl_805F1628 == 1) {
        lbl_805F161C = 1;
        lbl_805F1628 = 0;
    } else {
        lbl_805F1628 = 1;
    }
}

void fn_80493ABC(void) {
    lbl_805EDF38++;
    if (lbl_805F161C == 1 && fn_8049A78C(lbl_805F1638[lbl_805F1614]) != 0 &&
        lbl_805EDF39 <= lbl_805EDF38) {
        lbl_805EDF38 = 0;
        bool same = lbl_805F1604 == lbl_805F15F8;
        lbl_805F1604 = lbl_805F15F8;
        if (same) lbl_805F1604 = lbl_805F15FC;
        fn_8037F020(lbl_805F1604);
        if (lbl_805EDF30 != 0) {
            fn_8037F094(0);
            lbl_805EDF30 = 0;
        }
        fn_8037EEF0();
        lbl_805F1624 = 1;
        lbl_805F1614 = (lbl_805F1614 + 1) - ((lbl_805F1614 + 1) / lbl_805EDF34) * lbl_805EDF34;
        lbl_805F161C = 0;
    }
}

void fn_80493B98(void) {
    if (lbl_805F1620 != 0 && lbl_805F1624 != 0) {
        s32 next = (lbl_805F1608 + 1) - ((lbl_805F1608 + 1) / lbl_805EDF34) * lbl_805EDF34;
        if (next == lbl_805F160C) {
            fn_80394228();
            lbl_805F1618 = 0;
        } else {
            fn_80394174(lbl_805E3178[next * 2 + 1]);
        }
        lbl_805F1608 = next;
        fn_80378568(&lbl_805F1630);
        if (lbl_805F15F0 == 0) {
            lbl_805F161C = 1;
        } else if (lbl_805F1628 == 1) {
            lbl_805F161C = 1;
            lbl_805F1628 = 0;
        } else {
            lbl_805F1628 = 1;
        }
        lbl_805F1620 = 0;
    }
    lbl_805F1624 = 0;
}

u32 fn_80493C7C(void) {
    return 0;
}

extern u32 lbl_805E3190[14];
extern u8 *lbl_805F164C;
extern s32 lbl_805F15EC;
extern s32 lbl_805F15E4;
extern s32 lbl_805F15E8;
extern u32 DAT_8056e390;
extern u32 DAT_8056e394;
extern u32 DAT_8056e398;
extern u8 lbl_80567838[];
extern u8 lbl_805677C0[];
extern u8 lbl_80567874[];

void fn_80493C84(u32 *params, s32 mode) {
    if (params != NULL) {
        for (u32 i = 0; i < 14; i++) lbl_805E3190[i] = params[i];
        lbl_805F164C = (u8 *)lbl_805E3190;
        lbl_805F15EC = 0x2A;
        u32 format = (params[6] >> 16) & 0xFF;
        lbl_805F15E4 = mode;
        if (format != 0) lbl_805F15E4 = 2;
        DAT_8056e390 = params[1] >> 16;
        DAT_8056e394 = params[1] & 0xFFFF;
        lbl_805F15E8 = lbl_805F15E4;
        if (mode != 2) {
            if (mode < 2 && mode >= 0) {
                DAT_8056e398 = 0x18;
                lbl_805F15EC = 0x2A;
                return;
            }
            RW_ERROR_STRING(2, s_Invalid_pixel_format_8056e3a8);
            return;
        }
        DAT_8056e398 = 0x10;
        lbl_805F15EC = 0x2A;
        return;
    }
    s32 tv = VIGetTvFormat();
    if (tv == 2) {
        lbl_805F164C = lbl_80567838;
        lbl_805F15EC = 2;
        return;
    }
    if (tv < 2) {
        if (tv == 0) {
            lbl_805F164C = lbl_805677C0;
            lbl_805F15EC = 0;
            return;
        }
        if (tv >= 0) {
            lbl_805F164C = lbl_80567874;
            lbl_805F15EC = 1;
            return;
        }
    } else if (tv == 5) {
        lbl_805F164C = lbl_80567874;
        lbl_805F15EC = 1;
        return;
    }
    RW_ERROR_STRING(2, s_Invalid_TV_format_8056e3c0);
}

u32 fn_80493E64(void) {
    fn_804953F0();
    fn_80499BF8();
    return 1;
}
}

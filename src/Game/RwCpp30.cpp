/* RenderWare vertex-array element conversion, 0x804683AC..0x804699F0.
 * Structure and names are inferred from the retail instruction streams. */
#include <dolphin/types.h>

extern "C" {
extern f32 lbl_805FB1F8; /* 64.0f */
extern f32 lbl_805FB1FC; /* 16384.0f */
extern f32 lbl_805FB200;
extern f64 lbl_805FB208;

#define RW_F32_BYTE(dst, src) do { \
    (dst)[0] = (u8)(s32)(scale * (src)[0]); \
    (dst)[1] = (u8)(s32)(scale * (src)[1]); \
    (dst)[2] = (u8)(s32)(scale * (src)[2]); \
} while (0)
#define RW_F32_SHORT(dst, src) do { \
    *(u16 *)((dst) + 0) = (u16)(s32)(scale * (src)[0]); \
    *(u16 *)((dst) + 2) = (u16)(s32)(scale * (src)[1]); \
    *(u16 *)((dst) + 4) = (u16)(s32)(scale * (src)[2]); \
} while (0)
#define RW_F32_WORD(dst, src) do { \
    *(u32 *)((dst) + 0) = ((const u32 *)(src))[0]; \
    *(u32 *)((dst) + 4) = ((const u32 *)(src))[1]; \
    *(u32 *)((dst) + 8) = ((const u32 *)(src))[2]; \
} while (0)

#define RW_S8_BYTE(dst, src) do { \
    (dst)[0] = (u8)(s32)(normalize * (f32)(src)[0] * scale); \
    (dst)[1] = (u8)(s32)(normalize * (f32)(src)[1] * scale); \
    (dst)[2] = (u8)(s32)(normalize * (f32)(src)[2] * scale); \
} while (0)
#define RW_S8_SHORT(dst, src) do { \
    *(u16 *)((dst) + 0) = (u16)(s32)(normalize * (f32)(src)[0] * scale); \
    *(u16 *)((dst) + 2) = (u16)(s32)(normalize * (f32)(src)[1] * scale); \
    *(u16 *)((dst) + 4) = (u16)(s32)(normalize * (f32)(src)[2] * scale); \
} while (0)
#define RW_S8_WORD(dst, src) do { \
    value = normalize * (f32)(src)[0]; \
    *(u32 *)((dst) + 0) = *(u32 *)&value; \
    value = normalize * (f32)(src)[1]; \
    *(u32 *)((dst) + 4) = *(u32 *)&value; \
    value = normalize * (f32)(src)[2]; \
    *(u32 *)((dst) + 8) = *(u32 *)&value; \
} while (0)

#define RW_PACK8(PACK) do { \
    u8 *out1 = out + stride; \
    u8 *out2 = out1 + stride; \
    u8 *out3 = out2 + stride; \
    u8 *out4 = out3 + stride; \
    PACK(out, source); \
    PACK(out1, source + 4); \
    PACK(out2, source + 8); \
    PACK(out3, source + 12); \
    PACK(out4, source + 16); \
    u8 *out5 = out4 + stride; \
    u8 *out6 = out5 + stride; \
    u8 *out7 = out6 + stride; \
    PACK(out5, source + 20); \
    PACK(out6, source + 24); \
    PACK(out7, source + 28); \
    out = out7 + stride; \
    source += 32; \
} while (0)
#define RW_PACK_ONE(PACK) do { \
    PACK(out, source); \
    out += stride; \
    source += 4; \
} while (0)
#define RW_PACK8_F32(PACK) do { \
    u8 *out1 = out + stride; \
    u8 *out2 = out1 + stride; \
    u8 *out3 = out2 + stride; \
    u8 *out4 = out3 + stride; \
    u8 *out5 = out4 + stride; \
    u8 *out6 = out5 + stride; \
    u8 *out7 = out6 + stride; \
    PACK(out, source); \
    PACK(out1, source + 3); \
    PACK(out2, source + 6); \
    PACK(out3, source + 9); \
    PACK(out4, source + 12); \
    PACK(out5, source + 15); \
    PACK(out6, source + 18); \
    PACK(out7, source + 21); \
    out = out7 + stride; \
    source += 24; \
} while (0)
#define RW_PACK_ONE_F32(PACK) do { \
    PACK(out, source); \
    out += stride; \
    source += 3; \
} while (0)

#define RW_CONVERT_S8(wordBytes) \
    s32 bytes = 0; \
    switch (type) { \
    case 1: \
        if (stride == 0) stride = 3; \
        { \
            u8 *out = dest; \
            const f32 normalize = lbl_805FB200; \
            const f32 scale = lbl_805FB1F8; \
            for (s32 i = 0; i < vertices; i++) { \
                RW_PACK_ONE(RW_S8_BYTE); \
            } \
        } \
        bytes = vertices * 3; \
        break; \
    case 3: \
        if (stride == 0) stride = 6; \
        { \
            u8 *out = dest; \
            const f32 normalize = lbl_805FB200; \
            const f32 scale = lbl_805FB1FC; \
            for (s32 i = 0; i < vertices; i++) { \
                RW_PACK_ONE(RW_S8_SHORT); \
            } \
        } \
        bytes = vertices * 6; \
        break; \
    case 4: \
        if (stride == 0) stride = 0xC; \
        { \
            u8 *out = dest; \
            f32 value; \
            if (vertices > 0) { \
                s32 i = 0; \
                s32 remaining = vertices - 8; \
                if (vertices > 8) { \
                    const f32 normalize = lbl_805FB200; \
                    for (; i < remaining; i += 8) { \
                        RW_PACK8(RW_S8_WORD); \
                    } \
                } \
                { \
                    const f32 normalize = lbl_805FB200; \
                    for (; i < vertices; i++) { \
                        RW_PACK_ONE(RW_S8_WORD); \
                    } \
                } \
            } \
        } \
        bytes = vertices * wordBytes; \
        break; \
    case 0: \
    case 2: \
        break; \
    } \
    return bytes;

#pragma scheduling reset
s32 fn_804692C0(u8 *dest, const s8 *source, s32 type, s32 vertices, u32 stride) {
    RW_CONVERT_S8(0x24)
}

#pragma scheduling off
s32 fn_80468ADC(u8 *dest, const f32 *source, s32 type, s32 vertices, u32 stride) {
    s32 bytes = 0;

    switch (type) {
    case 1:
        if (stride == 0) stride = 3;
        {
            u8 *out = dest;
            if (vertices > 0) {
                s32 i = 0;
                s32 remaining = vertices - 8;
                if (vertices > 8) {
                    const f32 scale = lbl_805FB1F8;
                    for (; i < remaining; i += 8) {
                        RW_PACK8_F32(RW_F32_BYTE);
                    }
                }
                {
                    const f32 scale = lbl_805FB1F8;
                    for (; i < vertices; i++) {
                        RW_PACK_ONE_F32(RW_F32_BYTE);
                    }
                }
            }
        }
        bytes = vertices * 3;
        break;
    case 3:
        if (stride == 0) stride = 6;
        {
            u8 *out = dest;
            if (vertices > 0) {
                s32 i = 0;
                s32 remaining = vertices - 8;
                if (vertices > 8) {
                    const f32 scale = lbl_805FB1FC;
                    for (; i < remaining; i += 8) {
                        RW_PACK8_F32(RW_F32_SHORT);
                    }
                }
                {
                    const f32 scale = lbl_805FB1FC;
                    for (; i < vertices; i++) {
                        RW_PACK_ONE_F32(RW_F32_SHORT);
                    }
                }
            }
        }
        bytes = vertices * 6;
        break;
    case 4:
        if (stride == 0) stride = 0xC;
        {
            u8 *out = dest;
            if (vertices > 0) {
                s32 i = 0;
                s32 remaining = vertices - 8;
                if (vertices > 8) {
                    for (; i < remaining; i += 8) {
                        RW_PACK8_F32(RW_F32_WORD);
                    }
                }
                for (; i < vertices; i++) {
                    RW_PACK_ONE_F32(RW_F32_WORD);
                }
            }
        }
        bytes = vertices * 0x24;
        break;
    case 0:
    case 2:
        break;
    }
    return bytes;
}

#pragma scheduling reset
s32 fn_804683AC(u8 *dest, const s8 *source, s32 type, s32 vertices, u32 stride) {
    RW_CONVERT_S8(0xC)
}
#pragma scheduling reset
}

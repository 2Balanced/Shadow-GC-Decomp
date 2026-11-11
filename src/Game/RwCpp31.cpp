/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
void fn_8046B0E0(u32 *metadata, u32 unused, u32 size, u8 *buffer) {
    metadata[0] = (u32)buffer;
    metadata[1] = size;
    memset(buffer, 0, size);
}

u32 fn_8046693C(u32 *format);
u32 fn_804669A0(u32 *format);
u32 fn_804669EC(u32 *format, u8 channel);
u32 fn_80466A54(u32 *format, u8 channel);

u32 fn_8046ACBC(u32 *format) {
    s32 i;
    u32 total = 0;
    for (i = 0; i < 0x15; i++) {
        u32 mode;
        switch (i) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
            if ((1U << i) & format[4]) total++;
            break;
        case 9:
            mode = (format[4] >> 9) & 3;
            if (mode == 1) {
                total += fn_8046693C(format);
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        case 10:
            mode = (format[4] >> 11) & 3;
            if (((format[1] >> 9) & 1) == 1) {
                u32 multiplier = 3;
                if ((format[1] >> 31) == 0) multiplier = 1;
                if (mode == 1) {
                    total += multiplier * fn_804669A0(format);
                } else if (mode == 2) {
                    total += multiplier;
                } else if (mode == 3) {
                    total += multiplier * 2;
                }
            } else {
                if (mode == 1) {
                    total += fn_804669A0(format);
                } else if (mode == 2) {
                    total++;
                } else if (mode == 3) {
                    total += 2;
                }
            }
            break;
        case 11:
        case 12:
            mode = (format[4] >> (2 * (i - 11) + 13)) & 3;
            if (mode == 1) {
                total += fn_804669EC(format, (u8)(i - 11));
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        case 13:
            mode = format[5] & 3;
            if (mode == 1) {
                total += fn_80466A54(format, 0);
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        case 14:
            mode = (format[5] >> 2) & 3;
            if (mode == 1) {
                total += fn_80466A54(format, 1);
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        case 15:
            mode = (format[5] >> 4) & 3;
            if (mode == 1) {
                total += fn_80466A54(format, 2);
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        case 16:
            mode = (format[5] >> 6) & 3;
            if (mode == 1) {
                total += fn_80466A54(format, 3);
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        case 17:
            mode = (format[5] >> 8) & 3;
            if (mode == 1) {
                total += fn_80466A54(format, 4);
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        case 18:
            mode = (format[5] >> 10) & 3;
            if (mode == 1) {
                total += fn_80466A54(format, 5);
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        case 19:
            mode = (format[5] >> 12) & 3;
            if (mode == 1) {
                total += fn_80466A54(format, 6);
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        case 20:
            mode = (format[5] >> 14) & 3;
            if (mode == 1) {
                total += fn_80466A54(format, 7);
            } else if (mode == 2) {
                total++;
            } else if (mode == 3) {
                total += 2;
            }
            break;
        }
    }
    return total;
}

u32 fn_8046B094(u32 *format, u32 first, u32 second) {
    u32 product = second * fn_8046ACBC(format);
    u32 total = first * 3;
    total += product;
    return (total + 0x1F) & ~0x1F;
}

extern u8 *lbl_805F15B4;
extern u32 lbl_805F1520;
void fn_80462E1C(void);
u32 *fn_80466194(u32 *descriptors, u32 streams, u32 vertices);
u32 *fn_80465ED4(u32 *descriptors, u8 *inputs, u32 streams, u32 vertices);
u32 fn_80465CDC(u32 *deduplicated, u32 *assignment, u32 streams, u32 descriptor);

struct RwCpp31Descriptor {
    u32 data;
    u32 type;
    s8 links[0x10];
};

static inline void rwCpp31InitDescriptor(RwCpp31Descriptor *descriptor, u32 data, u32 type) {
    descriptor->data = data;
    descriptor->type = type;
    descriptor->links[0] = -1;
}

#define RW31_STORE_STREAM() \
    do { \
        table[stream + tableIndex] = *(u32 *)(source + 0x10); \
        stream++; \
    } while (0)
#define RW31_STORE_STREAM8() \
    do { \
        RW31_STORE_STREAM(); \
        RW31_STORE_STREAM(); \
        RW31_STORE_STREAM(); \
        RW31_STORE_STREAM(); \
        RW31_STORE_STREAM(); \
        RW31_STORE_STREAM(); \
        RW31_STORE_STREAM(); \
        RW31_STORE_STREAM(); \
    } while (0)

int fn_8046B110(u32 *object, u32 **out1, u32 **out2) {
    RwCpp31Descriptor descriptors[16];
    if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) {
        fn_80462E1C();
    }
    s32 count = 1;
    u32 *inputs = (u32 *)object[0x17];
    rwCpp31InitDescriptor(&descriptors[0], inputs[5], 8);
    if ((object[2] & 0x10) != 0) {
        descriptors[count].data = inputs[6];
        descriptors[count].type = 8;
        descriptors[count].links[0] = -1;
        count++;
    }
    if ((object[2] & 0x08) != 0) {
        descriptors[count].data = object[0xC];
        descriptors[count].type = 4;
        descriptors[count].links[0] = -1;
        count++;
    }
    if ((object[2] & 0x84) != 0) {
        RwCpp31Descriptor *descriptor = &descriptors[count];
        u32 *input = object;
        for (s32 i = 0; i < (s32)object[7]; i++) {
            rwCpp31InitDescriptor(descriptor, input[0xD], 7);
            descriptor++;
            input++;
            count++;
        }
    }
    u32 *deduplicated = fn_80466194((u32 *)descriptors, count, object[5]);
    if (deduplicated == NULL) {
        return 0;
    }
    *out1 = fn_80465ED4(deduplicated, (u8 *)descriptors, count, object[5]);
    if (*out1 == NULL) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(deduplicated);
        return 0;
    }

    s32 entries = *(u16 *)((u8 *)object[0x15] + 4);
    u32 entryBytes = entries * 4;
    u32 tableBytes = count * entryBytes;
    u32 *table = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(tableBytes, 0x1050D);
    if (table == NULL) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(*out1);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(deduplicated);
        return 0;
    }
    memset(table, 0, tableBytes);
    s32 tableIndex = 0;
    for (s32 entry = 0; entry < entries; entry++) {
        u8 *source = (u8 *)object[0x15] + entry * 0xC;
        s32 stream = 0;
        if (count > 0) {
            s32 remaining = count - 8;
            if (count > 8) {
                for (; stream < remaining; ) {
                    RW31_STORE_STREAM8();
                }
            }
            for (; stream < count; ) {
                RW31_STORE_STREAM();
            }
        }
        tableIndex += count;
    }

    *out2 = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(entryBytes, 0x1050D);
    if (*out2 == NULL) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(*out1);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(deduplicated);
        return 0;
    }
    tableIndex = 0;
    for (s32 entry = 0; entry < entries; entry++) {
        (*out2)[entry] = fn_80465CDC(
            deduplicated, table + tableIndex, count,
            *(u32 *)(object[0x15] + entry * 0xC + 0x14));
        if ((*out2)[entry] == 0) {
            s32 byteOffset = entry * 4;
            while (--entry > 0) {
                byteOffset -= 4;
                ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(
                    *(u32 **)((u8 *)*out2 + byteOffset));
            }
            return 0;
        }
        tableIndex += count;
    }
    ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
    ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(deduplicated);
    return 1;
}

#define RW31_FIRST_PACK_A(dst, src) do { \
    *(u16 *)(dst) = (u16)(((src)[0] << 8 & 0xF800) | ((src)[1] << 3 & 0x7E0) | ((src)[2] >> 3 & 0x1F)); \
} while (0)
#define RW31_FIRST_COPY3(dst, src) do { \
    (dst)[0] = (src)[0]; \
    (dst)[1] = (src)[1]; \
    (dst)[2] = (src)[2]; \
} while (0)
#define RW31_FIRST_PACK_B(dst, src) do { \
    *(u16 *)(dst) = (u16)(((src)[0] << 8 & 0xF000) | ((src)[1] << 4 & 0xF00) | \
                          ((src)[2] & 0xF0) | ((src)[3] >> 4 & 0xF)); \
} while (0)
#define RW31_FIRST_PACK_C(dst, src) do { \
    (dst)[0] = (u8)(((src)[0] & 0xFC) | ((src)[1] >> 6 & 3)); \
    (dst)[1] = (u8)(((src)[1] << 2 & 0xF0) | ((src)[2] >> 4 & 0xF)); \
    (dst)[2] = (u8)(((src)[2] << 4 & 0xC0) | ((src)[3] >> 2 & 0x3F)); \
} while (0)
#define RW31_FIRST_WORD(dst, src) do { \
    *(u32 *)(dst) = ((const u32 *)(src))[0]; \
} while (0)
#define RW31_FIRST_UNROLL(PACK) do { \
    PACK(out, source); \
    u8 *out1 = out + stride; \
    PACK(out1, source + 4); \
    u8 *out2 = out1 + stride; \
    PACK(out2, source + 8); \
    u8 *out3 = out2 + stride; \
    PACK(out3, source + 12); \
    u8 *out4 = out3 + stride; \
    PACK(out4, source + 16); \
    u8 *out5 = out4 + stride; \
    PACK(out5, source + 20); \
    u8 *out6 = out5 + stride; \
    PACK(out6, source + 24); \
    u8 *out7 = out6 + stride; \
    PACK(out7, source + 28); \
    out = out7 + stride; \
    source += 32; \
} while (0)
#define RW31_FIRST_ONE(PACK) do { \
    PACK(out, source); \
    out += stride; \
    source += 4; \
} while (0)
#define RW31_FIRST_CONVERT(PACK) do { \
    u8 *out = dest; \
    if (vertices > 0) { \
        s32 i = 0; \
        s32 remaining = vertices - 8; \
        if (vertices > 8) { \
            for (; i < remaining; i += 8) { \
                RW31_FIRST_UNROLL(PACK); \
            } \
        } \
        for (; i < vertices; i++) { \
            RW31_FIRST_ONE(PACK); \
        } \
    } \
} while (0)

#pragma scheduling off
s32 fn_804699F0(u8 *dest, const u8 *source, s32 type, s32 vertices, u32 stride) {
    s32 bytes = 0;
    switch (type) {
    case 0:
        if (stride == 0) stride = 2;
        RW31_FIRST_CONVERT(RW31_FIRST_PACK_A);
        bytes = vertices * 2;
        break;
    case 1:
        if (stride == 0) stride = 3;
        RW31_FIRST_CONVERT(RW31_FIRST_COPY3);
        bytes = vertices * 3;
        break;
    case 3:
        if (stride == 0) stride = 2;
        RW31_FIRST_CONVERT(RW31_FIRST_PACK_B);
        bytes = vertices * 2;
        break;
    case 4:
        if (stride == 0) stride = 3;
        RW31_FIRST_CONVERT(RW31_FIRST_PACK_C);
        bytes = vertices * 3;
        break;
    case 2:
    case 5:
        if (stride == 0) stride = 4;
        RW31_FIRST_CONVERT(RW31_FIRST_WORD);
        bytes = vertices * 4;
        break;
    }
    return bytes;
}
#pragma scheduling reset

#define RW31_PAIR_BYTE(dst, src) do { \
    (dst)[0] = (u8)(s32)((src)[0] * scale); \
    (dst)[1] = (u8)(s32)((src)[1] * scale); \
} while (0)
#define RW31_PAIR_SHORT(dst, src) do { \
    *(u16 *)(dst) = (u16)(s32)((src)[0] * scale); \
    *(u16 *)((dst) + 2) = (u16)(s32)((src)[1] * scale); \
} while (0)
#define RW31_PAIR_WORD(dst, src) do { \
    *(u32 *)(dst) = ((const u32 *)(src))[0]; \
    *(u32 *)((dst) + 4) = ((const u32 *)(src))[1]; \
} while (0)
#define RW31_PAIR_UNROLL(PACK) do { \
    u8 *out1 = out + stride; \
    u8 *out2 = out1 + stride; \
    u8 *out3 = out2 + stride; \
    u8 *out4 = out3 + stride; \
    u8 *out5 = out4 + stride; \
    u8 *out6 = out5 + stride; \
    u8 *out7 = out6 + stride; \
    PACK(out, source); \
    PACK(out1, source + 2); \
    PACK(out2, source + 4); \
    PACK(out3, source + 6); \
    PACK(out4, source + 8); \
    PACK(out5, source + 10); \
    PACK(out6, source + 12); \
    PACK(out7, source + 14); \
    out = out7 + stride; \
    source += 16; \
} while (0)
#define RW31_PAIR_ONE(PACK) do { \
    PACK(out, source); \
    out += stride; \
    source += 2; \
} while (0)
#define RW31_PAIR_CONVERT(PACK) do { \
    u8 *out = dest; \
    if (vertices > 0) { \
        s32 i = 0; \
        s32 remaining = vertices - 8; \
        if (vertices > 8) { \
            for (; i < remaining; i += 8) { \
                RW31_PAIR_UNROLL(PACK); \
            } \
        } \
        for (; i < vertices; i++) { \
            RW31_PAIR_ONE(PACK); \
        } \
    } \
} while (0)

#pragma scheduling off
s32 fn_8046A270(u8 *dest, const f32 *source, s32 type, s32 vertices, u32 stride, f32 scale) {
    s32 bytes = 0;
    switch (type) {
    case 0:
        if (stride == 0) stride = 2;
        RW31_PAIR_CONVERT(RW31_PAIR_BYTE);
        bytes = vertices * 2;
        break;
    case 1:
        if (stride == 0) stride = 2;
        RW31_PAIR_CONVERT(RW31_PAIR_BYTE);
        bytes = vertices * 2;
        break;
    case 2:
        if (stride == 0) stride = 4;
        RW31_PAIR_CONVERT(RW31_PAIR_SHORT);
        bytes = vertices * 4;
        break;
    case 3:
        if (stride == 0) stride = 4;
        RW31_PAIR_CONVERT(RW31_PAIR_SHORT);
        bytes = vertices * 4;
        break;
    case 4:
        if (stride == 0) stride = 8;
        RW31_PAIR_CONVERT(RW31_PAIR_WORD);
        bytes = vertices * 8;
        break;
    }
    return bytes;
}
#pragma scheduling reset
}

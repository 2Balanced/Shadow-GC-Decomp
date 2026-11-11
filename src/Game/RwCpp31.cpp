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
    u32 i;
    u32 total = 0;
    for (i = 0; i <= 0x14; i++) {
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
    return (first * 3 + second * fn_8046ACBC(format) + 0x1F) & ~0x1F;
}

extern u8 *lbl_805F15B4;
extern u32 lbl_805F1520;
void fn_80462E1C(void);
u32 *fn_80466194(u32 *descriptors, u32 streams, u32 vertices);
u32 *fn_80465ED4(u32 *descriptors, u8 *inputs, u32 streams, u32 vertices);
u32 *fn_80465CDC(u32 *deduplicated, u32 *assignment, u32 streams, u32 *descriptor);

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
        table[stream + count * entry] = *(u32 *)(object[0x54] + entry * 0xC + 0x10); \
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

int fn_8046B110(u32 *object, u32 **out1, u32 ***out2) {
    RwCpp31Descriptor descriptors[16];
    if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) {
        fn_80462E1C();
    }
    u32 count = 1;
    u32 *inputs = (u32 *)object[0x5C];
    rwCpp31InitDescriptor(&descriptors[0], inputs[5], 8);
    if ((object[2] & 0x10) != 0) {
        rwCpp31InitDescriptor(&descriptors[count++], inputs[6], 8);
    }
    if ((object[2] & 0x08) != 0) {
        rwCpp31InitDescriptor(&descriptors[count++], object[0x30], 4);
    }
    if ((object[2] & 0x84) != 0) {
        for (u32 i = 0; i < object[7]; i++) {
            rwCpp31InitDescriptor(&descriptors[count++], object[0x34 + i], 7);
        }
    }
    u32 *deduplicated = fn_80466194((u32 *)descriptors, count, object[5]);
    if (deduplicated == NULL) {
        return 0;
    }
    *out1 = fn_80465ED4((u32 *)descriptors, (u8 *)object[0x5C], count, object[5]);
    if (*out1 == NULL) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(deduplicated);
        return 0;
    }

    u32 entries = *(u16 *)((u8 *)object[0x54] + 4);
    u32 tableBytes = count * entries * 4;
    u32 *table = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(tableBytes, 0x3050D);
    if (table == NULL) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(*out1);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(deduplicated);
        return 0;
    }
    memset(table, 0, tableBytes);
    for (u32 entry = 0; entry < entries; entry++) {
        u32 stream = 0;
        u32 remaining = count - 8;
        if (count > 8) {
            for (; stream < remaining; ) {
                RW31_STORE_STREAM8();
            }
        }
        for (; stream < count; ) {
            RW31_STORE_STREAM();
        }
    }

    u32 **converted = ((u32 **(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(entries * 4, 0x1050D);
    *out2 = converted;
    if (converted == NULL) {
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(*out1);
        ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(deduplicated);
        return 0;
    }
    for (u32 entry = 0; entry < entries; entry++) {
        for (u32 stream = 0; stream < count; stream++) {
            converted[stream] = fn_80465CDC(deduplicated, table + stream * 4, count,
                                            (u32 *)(object[0x54] + entry * 0xC + 0x14));
            if (converted[stream] == NULL) {
                for (u32 i = 0; i < entry; i++) {
                    ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(converted[i]);
                }
                ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
                ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(*out1);
                ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(deduplicated);
                return 0;
            }
        }
    }
    ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
    ((void (*)(u32 *))*(u32 *)(lbl_805F15B4 + 0x10C))(deduplicated);
    return 1;
}
}

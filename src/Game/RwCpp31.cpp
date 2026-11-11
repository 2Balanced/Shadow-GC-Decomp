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
}

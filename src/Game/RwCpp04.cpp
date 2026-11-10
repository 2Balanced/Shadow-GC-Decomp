/* RenderWare command/stream wrappers, 0x8043EFF0..0x8044091F.
 * Function and global names follow the current symbol map; structure field names are unknown.
 * Definitions stay in retail address order for deferred inline emission. */
#include <string.h>
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F1404;
extern u8 *lbl_805F15B4;
extern f32 lbl_805FAF68;

u32 OSDisableInterrupts(void);
void OSRestoreInterrupts(u32);
void *__va_arg(void *, int);

void fn_8043E004(u32 *, u32);
void fn_8043E088(u32 *);
u32 *fn_8043E0E8(void);
void fn_8043E228(u8 *, int);
void fn_8043E2B4(int, int);
void fn_8043E47C(int);
void fn_8043E494(void);
u32 fn_8043E6F4(u32, u32, u32, u32);
int fn_8043AF64(u32, void *, u32, u32, u8);
int fn_8043B05C(u32, void *, u32, u32, u8);
void fn_8043B6D0(void *);
void fn_8043B794(void *);
void fn_8043B840(void **);
void *fn_8047CEF0(void *, u32, u32, u32, u32);
int fn_8047CF50(void *, u32, u32, void *);
void fn_8047D158(void *, u32);
void fn_8047D1C8(void *, u32);
void fn_8047D254(void *, u32);
void fn_8047D2C4(void *, u32);
u32 fn_8047D560(void *, void *, u32);
u32 fn_8047D684(void *, void *, u32);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
u32 fn_80480D4C(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);

typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} va_list[1];
#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_end(ap) ((void)0)

int fn_8043EFF0(u32 *object, s8 command, s8 index, s8 flags) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    int result;
    if (object == NULL) {
        result = 0x07000103;
    } else {
        u32 offset = 0;
        result = 0x07000102;
        for (int i = 0; i < 8; i++) {
            if (object == (u32 *)(lbl_805F1404 + offset + 0x5478)) {
                if (*(s32 *)object != 0) {
                    result = 0;
                } else {
                    result = 0x07000102;
                }
                break;
            }
            offset += 0x1530;
        }
    }
    if (result != 0) return result;
    if (command < 0 || command > 0x1F) return 0x03000001;
    if (index < 0 || index > 0x7F) return 0x03000001;
    extern int fn_8043FDF8(u32 *, int, u32, u32);
    fn_8043FDF8(object, command, index, flags);
    return 0;
}

int fn_8043F0D4(void) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    for (int i = 0; i < 8; i++) {
        extern int fn_8043F13C(u32 *);
        fn_8043F13C((u32 *)(lbl_805F1404 + i * 0x1530 + 0x5478));
    }
    return 0;
}

int fn_8043F13C(u32 *object) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    int result;
    if (object == NULL) {
        result = 0x07000103;
    } else {
        u32 offset = 0;
        result = 0x07000102;
        for (int i = 0; i < 8; i++) {
            if (object == (u32 *)(lbl_805F1404 + offset + 0x5478)) {
                if (*(s32 *)object != 0) {
                    result = 0;
                } else {
                    result = 0x07000102;
                }
                break;
            }
            offset += 0x1530;
        }
    }
    if (result != 0) return result;
    for (int i = 0; i < 0x10; i++) fn_8043E228((u8 *)object, i);
    return 0;
}

int fn_8043F1FC(void) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    fn_8043E2B4(0, 0);
    return 0;
}

int fn_8043F240(void) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    fn_8043E2B4(0, 2);
    return 0;
}

int fn_8043F284(void) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    u32 interrupt = OSDisableInterrupts();
    ((u32 *)lbl_805F1404)[2]++;
    OSRestoreInterrupts(interrupt);
    fn_8043E494();
    interrupt = OSDisableInterrupts();
    ((u32 *)lbl_805F1404)[2]--;
    OSRestoreInterrupts(interrupt);
    return 0;
}

int fn_8043F2F0(void) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    u32 interrupt = OSDisableInterrupts();
    ((u32 *)lbl_805F1404)[2]++;
    OSRestoreInterrupts(interrupt);
    fn_8043E494();
    interrupt = OSDisableInterrupts();
    ((u32 *)lbl_805F1404)[2]--;
    OSRestoreInterrupts(interrupt);
    return 0;
}

int fn_8043F35C(u32 *object, u8 *command, u32 flags) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    int result;
    if (object == NULL) {
        result = 0x07000103;
    } else {
        u32 offset = 0;
        result = 0x07000102;
        for (int i = 0; i < 8; i++) {
            if (object == (u32 *)(lbl_805F1404 + offset + 0x5478)) {
                if (*(s32 *)object != 0) {
                    result = 0;
                } else {
                    result = 0x07000102;
                }
                break;
            }
            offset += 0x1530;
        }
    }
    if (result != 0) return result;
    u32 value = ((u32)command[0] << 24) | ((u32)command[1] << 16) |
                ((u32)command[2] << 8) | (flags & 0xFF);
    fn_8043E004(object, value);
    return 0;
}

int fn_8043F41C(u32 *object, s8 *out, u32 type, ...) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    int result;
    if (object == NULL) {
        result = 0x07000103;
    } else {
        u32 offset = 0;
        result = 0x07000102;
        for (int i = 0; i < 8; i++) {
            if (object == (u32 *)(lbl_805F1404 + offset + 0x5478)) {
                if (*(s32 *)object != 0) {
                    result = 0;
                } else {
                    result = 0x07000102;
                }
                break;
            }
            offset += 0x1530;
        }
    }
    if (result != 0) return result;
    va_list args;
    va_start(args, type);
    out[0] = (s8)type;
    out[1] = (s8)*(u32 *)__va_arg(args, 1);
    switch (type & 0xF0) {
    case 0x80:
    case 0x90:
    case 0xA0:
    case 0xB0:
    case 0xD0:
    case 0xE0:
        out[2] = (s8)*(u32 *)__va_arg(args, 1);
        break;
    default:
        out[2] = 0;
        break;
    }
    out[3] = 0;
    va_end(args);
    return 0;
}

int fn_8043F5AC(void) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    for (int i = 0; i < 0x40; i++) {
        extern int fn_8043F60C(u32 *);
        fn_8043F60C((u32 *)(lbl_805F1404 + i * 0x1530 + 0x5478));
    }
    return 0;
}

int fn_8043F60C(u32 *object) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    int result;
    if (object == NULL) {
        result = 0x07000103;
    } else {
        u32 offset = 0;
        result = 0x07000102;
        for (int i = 0; i < 8; i++) {
            if (object == (u32 *)(lbl_805F1404 + offset + 0x5478)) {
                if (*(s32 *)object != 0) {
                    result = 0;
                }
                break;
            }
            offset += 0x1530;
        }
    }
    if (result != 0) return result;
    fn_8043E088(object);
    return 0;
}

int fn_8043F6AC(u32 **out) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    u32 *object = fn_8043E0E8();
    if (object == NULL) return 0x07000101;
    *out = object;
    return 0;
}

int fn_8043F70C(u8 *object) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    int result;
    if (object == NULL) {
        result = 0x07000103;
    } else {
        u32 offset = 0;
        result = 0x07000102;
        for (int i = 0; i < 0x40; i++) {
            if (object == (u8 *)(lbl_805F1404 + offset + 0x13F6C)) {
                if (*object != 0) {
                    result = 0;
                } else {
                    result = 0x07000102;
                }
                break;
            }
            offset += 0x14;
        }
    }
    if (result != 0) return result;
    u32 *stream = *(u32 **)(object + 4);
    u32 *commands = stream + 8;
    if (stream[0] != 0x67636178 || stream[1] != 0x4D4C5420 ||
        stream[4] != 0x67636178 || stream[5] != 0x4D4C544D) {
        return 0x09000001;
    }
    result = 0;
    int count = stream[7] >> 4;
    while (count-- > 0) {
        if (commands[1] == 0xFFFFFFFF) break;
        int status = -1;
        u32 argument = (commands[1] >> 24) & 0xF;
        int kind = (commands[0] >> 24) & 0xF;
        switch (kind) {
        case 4:
            status = (int)fn_8043AF64(argument, (u8 *)commands + commands[2],
                                      *(u32 *)(object + 0xC), *(u32 *)(object + 0x10),
                                      *(u8 *)(lbl_805F1404 + 0x1446C));
            break;
        case 1:
            status = (int)fn_8043B05C(argument, (u8 *)commands + commands[2],
                                      *(u32 *)(object + 0xC), *(u32 *)(object + 0x10),
                                      *(u8 *)(lbl_805F1404 + 0x1446C));
            break;
        }
        if (status < 0) {
            result = 0x09000101;
            break;
        }
        commands += 4;
    }
    return result;
}

int fn_8043F8C4(u8 *object) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    int result;
    if (object == NULL) {
        result = 0x07000103;
    } else {
        u32 offset = 0;
        result = 0x07000102;
        for (int i = 0; i < 0x40; i++) {
            if (object == (u8 *)(lbl_805F1404 + offset + 0x13F6C)) {
                if (*object != 0) {
                    result = 0;
                } else {
                    result = 0x07000102;
                }
                break;
            }
            offset += 0x14;
        }
    }
    if (result != 0) return result;
    if (*object != 0 && *(s8 *)(object + 1) >= 0) {
        while (*(u8 *)(lbl_805F1404 + *(s8 *)(object + 1) + 0x505C) != 0) {}
    }
    *object = 0;
    return 0;
}

int fn_8043F980(u8 *object, u32 a, u32 b, u32 c, u32 d) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    int result;
    if (object == NULL) {
        result = 0x07000103;
    } else {
        u32 offset = 0;
        result = 0x07000102;
        for (int i = 0; i < 0x40; i++) {
            if (object == (u8 *)(lbl_805F1404 + offset + 0x13F6C)) {
                if (*object != 0) {
                    result = 0;
                } else {
                    result = 0x07000102;
                }
                break;
            }
            offset += 0x14;
        }
    }
    if (result != 0) return result;
    *(u32 *)(object + 4) = a;
    *(s8 *)(object + 1) = -1;
    *(u32 *)(object + 8) = b;
    *(u32 *)(object + 0xC) = c;
    *(u32 *)(object + 0x10) = d;
    return 0;
}

int fn_8043FA20(u8 **out) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    u8 *object = (u8 *)(lbl_805F1404 + 0x13F6C);
    for (u32 i = 0; i < 0x40; i++) {
        if (*object == 0) goto found;
        object += 0x14;
    }
    return 0x07000101;
found:
    memset(object, 0, 0x14);
    *object = 1;
    *out = object;
    return 0;
}

int fn_8043FAB0(int value) {
    if (lbl_805F1404 == NULL) return 0x0D000001;
    if (value == 1) goto one;
    if (value >= 1) goto error;
    if (value >= 0) goto zero;
    goto error;
zero:
    fn_8043E47C(1);
    goto done;
one:
    fn_8043E47C(0);
    goto done;
error:
    return 0x03000001;
done:
    return 0;
}

int fn_8043FB24(void *heap, int arg1, int arg2) {
    fn_8043E6F4((u32)heap, 0x30, 0xE1000, 0x800000);
    return 0;
}

void fn_8043FB58(u32 *object) {
    if ((object[0] & 0x80000000) == 0) return;
    u32 interrupt = OSDisableInterrupts();
    u32 nested = OSDisableInterrupts();
    ((u32 *)lbl_805F1404)[2]++;
    OSRestoreInterrupts(nested);
    u32 flags = object[1];
    if ((flags & 2) == 0) {
        object[1] = flags & ~1;
    } else {
        if ((flags & 1) != 0) {
            u64 now = ((u64)((u32 *)lbl_805F1404)[6] << 32) | ((u32 *)lbl_805F1404)[7];
            u64 then = ((u64)object[0x10] << 32) | object[0x11];
            *(u64 *)(object + 0x1A) += now - then;
            *(u64 *)(object + 0x1C) += now - then;
            object[1] &= ~1;
            if (((u32 *)lbl_805F1404)[0x11] == 1) fn_8043B794(object + 0x28);
        }
    }
    nested = OSDisableInterrupts();
    ((u32 *)lbl_805F1404)[2]--;
    OSRestoreInterrupts(nested);
    OSRestoreInterrupts(interrupt);
}

void fn_8043FC98(u32 *object) {
    if ((object[0] & 0x80000000) == 0) return;
    u32 interrupt = OSDisableInterrupts();
    u32 nested = OSDisableInterrupts();
    ((u32 *)lbl_805F1404)[2]++;
    OSRestoreInterrupts(nested);
    if ((object[1] & 1) == 0) {
        object[0x10] = ((u32 *)lbl_805F1404)[6];
        object[0x11] = ((u32 *)lbl_805F1404)[7];
        object[1] |= 1;
        if (object[0x28] != 0) {
            if (((u32 *)lbl_805F1404)[0x11] == 1) {
                fn_8043B6D0(object + 0x28);
            } else {
                fn_8043B840((void **)(object + 0x28));
                object[0x28] = 0;
            }
        }
    }
    nested = OSDisableInterrupts();
    ((u32 *)lbl_805F1404)[2]--;
    OSRestoreInterrupts(nested);
    OSRestoreInterrupts(interrupt);
}

int fn_8043FD70(u32 *object) {
    if ((object[0] & 0x80000000) == 0) return -1;
    u32 interrupt = OSDisableInterrupts();
    object[1] &= ~2;
    if (object[0x28] != 0) {
        fn_8043B840((void **)(object + 0x28));
        object[0x28] = 0;
    }
    memset(object + 0x12, 0, 0x58);
    OSRestoreInterrupts(interrupt);
    return 0;
}

int fn_8043FDF8(u32 *object, int index, u32 command, u32 flags) {
    if ((object[0] & 0x80000000) == 0) return -1;
    u32 *base = (u32 *)lbl_805F1404;
    u32 *table = (u32 *)base[index * 2 + 0x53F8 / 4];
    if (table == NULL) return -1;
    u32 *entry = table;
    while (*entry != 0) {
        if ((*entry >> 24) == command) break;
        entry++;
    }
    if (*entry == 0) return -1;
    u32 *header = (u32 *)((u8 *)table + (*entry & 0xFFFFFF));
    if (header[0] != 0x67636178 || header[1] != 0x4D53424D) return -1;
    u32 interrupt = OSDisableInterrupts();
    object[1] = 0;
    object[1] |= 2;
    if (object[0x28] != 0) {
        fn_8043B840((void **)(object + 0x28));
        object[0x28] = 0;
    }
    u32 offset = *entry & 0xFFFFFF;
    u32 *fields = object + 0x12;
    *(u16 *)fields = *(u16 *)((u8 *)table + offset + 0x10);
    memset(fields, 0, 0x58);
    u32 data = (u32)((u8 *)table + offset + 0x28);
    fields[1] = *(u16 *)((u8 *)table + offset + 0x1E);
    u32 hi = ((u32 *)lbl_805F1404)[6];
    u32 lo = ((u32 *)lbl_805F1404)[7];
    fields[8] = hi;
    fields[9] = lo;
    fields[0x0A] = hi;
    fields[0x0B] = lo;
    *(f32 *)(fields + 6) = lbl_805FAF68;
    fields[0x0F] = flags;
    fields[0x10] = data;
    fields[0x11] = data;
    OSRestoreInterrupts(interrupt);
    return 0;
}

void fn_8043FF5C(u32 *object) {
    if ((object[0] & 0x80000000) == 0) return;
    if ((object[1] & 2) == 0) return;
    if ((object[1] & 1) != 0) return;
    u8 *cursor = (u8 *)object[0x23];
    if (cursor == NULL) return;

    s32 elapsed = ((s32 *)lbl_805F1404)[7] - (s32)object[0x1D];
    ((s32 *)lbl_805F1404)[10] = elapsed;
    if (((s32 *)lbl_805F1404)[10] < 0) ((s32 *)lbl_805F1404)[10] = -((s32 *)lbl_805F1404)[10];

restart:
    *(f32 *)(object + 0x1F) = 60.0f / *(f32 *)(object + 0x18);
    u32 clock = *(volatile u32 *)0x800000F8;
    u32 divisor = (clock >> 2) / 125000;
    u32 frames = ((u32 *)lbl_805F1404)[10] * 8 / divisor;
    *(f32 *)(object + 0x17) = *(f32 *)(object + 0x16);
    f32 value = (f32)(s32)frames;
    value *= *(f32 *)(lbl_805F1404 + 0x13A44);
    value *= (f32)object[0x13];
    value *= *(f32 *)(object + 0x18);
    value /= 60000000.0f;
    *(f32 *)(object + 0x20) = value;
    *(f32 *)(object + 0x16) += *(f32 *)(object + 0x20);

    while (true) {
        u8 *scan = cursor;
        int length = 0;
        int delta = 0;
        for (; length < 4; length++) {
            if ((scan[length] & 0x80) == 0) break;
        }
        if (length == 4) {
            delta = -1;
        } else {
            length++;
            for (int i = 0; i < length; i++) {
                delta = (delta << 7) + (*scan++ & 0x7F);
            }
            cursor = scan;
        }
        if (delta < 0) {
            cursor = NULL;
            break;
        }
        if (*(f32 *)(object + 0x14) + (f32)delta > *(f32 *)(object + 0x16)) break;
        *(f32 *)(object + 0x15) = *(f32 *)(object + 0x14);
        *(f32 *)(object + 0x14) += (f32)delta;

        u8 status;
        if ((*cursor & 0x80) != 0) {
            status = *cursor++;
        } else {
            status = *(u8 *)(object + 0x1E);
        }
        if (status == 0xFF) {
            *(u8 *)(object + 0x1E) = 0xFF;
            if (*(s8 *)cursor == 0x51) {
                u32 tempo = ((u32)cursor[2] << 16) | ((u32)cursor[3] << 8) | cursor[4];
                cursor += 5;
                *(f32 *)(object + 0x18) = 60000000.0f / (f32)tempo;
            } else if (*(s8 *)cursor == 0x2F) {
                cursor = NULL;
                break;
            }
        } else {
            switch (status & 0xF0) {
            case 0x80:
            case 0x90:
            case 0xE0: {
                u32 value = ((u32)status << 24) | ((u32)cursor[0] << 16) |
                            ((u32)cursor[1] << 8) | (object[0x21] & 0xFF);
                fn_8043E004(object, value);
                *(u8 *)(object + 0x1E) = status;
                cursor += 2;
                break;
            }
            case 0xB0:
                if (cursor[0] == 0x1F) {
                    if (object[0x24] == 0) {
                        object[0x24] = (u32)(cursor + 2);
                        object[0x27] = cursor[1];
                        cursor += 2;
                        *(f32 *)(object + 0x25) = *(f32 *)(object + 0x14);
                        *(f32 *)(object + 0x26) = *(f32 *)(object + 0x17);
                        *(f32 *)(object + 0x19) = *(f32 *)(object + 0x18);
                    } else if (*(s32 *)(object + 0x27) == 0) {
                        cursor = (u8 *)object[0x24];
                        *(f32 *)(object + 0x16) = *(f32 *)(object + 0x26);
                        *(f32 *)(object + 0x14) = *(f32 *)(object + 0x25);
                        *(f32 *)(object + 0x18) = *(f32 *)(object + 0x19);
                        goto restart;
                    } else {
                        if (*(s32 *)(object + 0x27) > 0) {
                            --*(s32 *)(object + 0x27);
                            if (*(s32 *)(object + 0x27) > 0) {
                                cursor = (u8 *)object[0x24];
                                *(f32 *)(object + 0x16) = *(f32 *)(object + 0x26);
                                *(f32 *)(object + 0x14) = *(f32 *)(object + 0x25);
                                *(f32 *)(object + 0x18) = *(f32 *)(object + 0x19);
                                goto restart;
                            }
                        }
                        cursor += 2;
                        *(u8 *)(object + 0x1E) = 0;
                    }
                } else {
                    u32 value = ((u32)status << 24) | ((u32)cursor[0] << 16) |
                                ((u32)cursor[1] << 8) | (object[0x21] & 0xFF);
                    fn_8043E004(object, value);
                    *(u8 *)(object + 0x1E) = status;
                    cursor += 2;
                }
                break;
            case 0xA0:
            case 0xC0:
            case 0xD0: {
                u32 value = ((u32)status << 24) | ((u32)cursor[0] << 16);
                fn_8043E004(object, value);
                *(u8 *)(object + 0x1E) = status;
                cursor++;
                break;
            }
            }
        }
        if (*(f32 *)(object + 0x14) < *(f32 *)(object + 0x16)) continue;
        break;
    }

    object[0x23] = (u32)cursor;
    u32 *global = (u32 *)lbl_805F1404;
    object[0x1C] = global[6];
    object[0x1D] = global[7];
    if (cursor == NULL) {
        object[1] &= ~3;
        if (object[0x28] != 0) {
            fn_8043B840((void **)(object + 0x28));
            object[0x28] = 0;
        }
        memset(object + 0x12, 0, 0x58);
    }
}

void fn_80440410(void *object) {
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(object);
}

int fn_8044043C(u32 *header) {
    int size = *(u16 *)((u8 *)header + 0x1E) * 0x10 + 0x30;
    if ((header[0] & 1) != 0) size += *(u16 *)(header + 7) * 2;
    return size;
}

void *fn_80440468(u32 *header, void *context) {
    int size = *(u16 *)((u8 *)header + 0x1E) * 0x10 + 0x30;
    if ((header[0] & 1) != 0) size += *(u16 *)(header + 7) * 2;
    if (fn_8047CEF0(context, 0x2C, size, 0x37002, 0x37) == NULL) return NULL;
    if (fn_8047CEF0(context, 1, size - 0xC, 0x37002, 0x37) == NULL) return NULL;
    u32 local[9];
    local[0] = header[0];
    local[1] = header[4];
    local[2] = header[5];
    local[3] = header[6];
    local[4] = header[1];
    local[5] = header[2];
    local[6] = header[3];
    local[7] = *(u16 *)(header + 7);
    local[8] = *(u16 *)((u8 *)header + 0x1E);
    fn_8047D158(local, 0x24);
    if (fn_80480ED0(context, local, 0x24) == 0) return NULL;
    u16 count = *(u16 *)((u8 *)header + 0x1E);
    if (count != 0) {
        u8 *entries = (u8 *)header[8];
        for (u32 i = 0; i < count; i++) {
            u32 entry[4];
            *(u64 *)entry = *(u64 *)(entries + i * 0x10);
            *(u64 *)(entry + 2) = *(u64 *)(entries + i * 0x10 + 8);
            fn_8047D1C8((u8 *)entry + 2, 2);
            fn_8047D1C8((u8 *)entry + 10, 2);
            fn_8047D158(entry + 1, 4);
            fn_8047D158(entry + 3, 4);
            if (fn_80480ED0(context, entry, 0x10) == 0) return NULL;
        }
    }
    if ((header[0] & 1) != 0 &&
        fn_8047D560(context, (void *)header[9], (u32)*(u16 *)(header + 7) << 1) == 0) {
        return NULL;
    }
    return header;
}

void *fn_80440670(void *context) {
    u32 local[18];
    if (fn_8047CF50(context, 1, 0, local) == 0) return NULL;
    if (fn_80480D4C(context, local + 9, 0x24) != 0x24) return NULL;
    fn_8047D254(local + 9, 0x24);
    u32 flags = local[9];
    u32 nameCount = local[17] & 0xFFFF;
    u32 extraCount = local[16] & 0xFFFF;
    u32 size = (nameCount != 0) ? nameCount * 0x10 + 0x37 : 0x28;
    if ((flags & 1) != 0) size += extraCount * 2 + 1;
    u32 *header = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x3002C);
    if (header == NULL) {
        local[1] = 0x11D;
        local[2] = fn_8047DC28(0x80000013, size);
        fn_8047DB84(local + 1);
    } else {
        header[0] = flags;
        *(u16 *)(header + 7) = (u16)extraCount;
        *(u16 *)((u8 *)header + 0x1E) = (u16)nameCount;
        header[1] = local[13];
        header[2] = local[14];
        header[3] = local[15];
        header[4] = local[10];
        header[5] = local[11];
        header[6] = local[12];
        u8 *next = (u8 *)(header + 10);
        if (nameCount == 0) {
            header[8] = 0;
        } else {
            next = (u8 *)(((u32)header + 0x37) & ~0xF);
            header[8] = (u32)next;
            next += nameCount * 0x10;
        }
        header[9] = ((flags & 1) == 0) ? 0 : ((u32)next + 1) & ~1;
    }
    if (header == NULL) return NULL;
    if (*(u16 *)((u8 *)header + 0x1E) != 0) {
        u32 bytes = (u32)*(u16 *)((u8 *)header + 0x1E) << 4;
        if (fn_80480D4C(context, (void *)header[8], bytes) != bytes) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(header);
            return NULL;
        }
        u8 *entry = (u8 *)header[8];
        for (u32 i = 0; i < *(u16 *)((u8 *)header + 0x1E); i++) {
            fn_8047D2C4(entry + 2, 2);
            fn_8047D2C4(entry + 10, 2);
            fn_8047D254(entry + 4, 4);
            fn_8047D254(entry + 12, 4);
            entry += 0x10;
        }
    }
    if ((header[0] & 1) != 0 &&
        fn_8047D684(context, (void *)header[9], (u32)*(u16 *)(header + 7) << 1) == 0) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(header);
        return NULL;
    }
    return header;
}
}

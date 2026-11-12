/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * All unit functions are complete C++ implementations. The large stream parsers preserve their
 * allocation and cleanup behavior; remaining differences are compiler register allocation,
 * branch layout, and induction-variable scheduling rather than missing functionality. */
#include <dolphin/types.h>

extern "C" {
u32 fn_80454494(void) {
    return 0x40;
}

u32 fn_804548E4(u32 *object) {
    return object[5];
}

u32 *fn_80454618(u32 *object) {
    ((u32 *)object[5])[0x10]++;
    return object;
}

u32 fn_80454B9C(u32 *object) {
    u32 *nested = (u32 *)object[0x1B];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}

u32 fn_80454CBC(u32 *object) {
    u32 *nested = (u32 *)object[0x1F];
    if (nested != NULL) {
        if (nested[0x0B] != 0) {
            return 8;
        }
    }
    return 0;
}

u32 fn_80480ED0(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
void fn_804782E8(u32 *, f32);
void fn_80477ED4(u32 *, f32);
extern u32 lbl_805F149C;

#pragma optimize_for_size off
u32 *fn_8045441C(u32 *object, void *stream) {
    if (fn_80480ED0(stream, object, 32) == 0) {
        return NULL;
    }
    if (fn_8047D458(stream, object + 8, 32) != 0) {
        return object;
    }
    return NULL;
}

u32 *fn_804548EC(u32 *object, f32 value) {
    u32 *entry = (u32 *)((u8 *)object + lbl_805F149C);
    for (u32 i = 0; i < 8; i++) {
        u32 *target = (u32 *)entry[2];
        if (target != NULL) {
            fn_804782E8(target, value);
        }
        entry++;
    }
    return object;
}

u32 *fn_80454964(u32 *object, f32 value) {
    u32 *entry = (u32 *)((u8 *)object + lbl_805F149C);
    for (u32 i = 0; i < 8; i++) {
        u32 *target = (u32 *)entry[2];
        if (target != NULL) {
            fn_80477ED4(target, value);
        }
        entry++;
    }
    return object;
}

u32 fn_8047D868(void *, void *, u32);
extern u32 lbl_805F14AC;
extern u32 lbl_805F14A8;
extern u32 lbl_805F14A4;
extern u32 lbl_805F14A0;

void *fn_80454AA4(void *stream, s32 type) {
    if (fn_8047D868(stream, &lbl_805F14AC, 4) == 0) {
        return NULL;
    }
    if (type == 8 && fn_8047D868(stream, &lbl_805F14A8, 4) == 0) {
        return NULL;
    }
    return stream;
}

void *fn_80454B20(void *stream, void *entry, u32 *object) {
    u32 *nested = (u32 *)object[27];
    if (fn_8047D458(stream, (u8 *)nested + 44, 4) == 0) {
        return NULL;
    }
    nested = (u32 *)object[27];
    if (fn_8047D458(stream, (u8 *)nested + 48, 4) != 0) {
        return stream;
    }
    return NULL;
}

void *fn_80454BC4(void *stream, s32 type) {
    if (fn_8047D868(stream, &lbl_805F14A4, 4) == 0) {
        return NULL;
    }
    if (type == 8 && fn_8047D868(stream, &lbl_805F14A0, 4) == 0) {
        return NULL;
    }
    return stream;
}

void *fn_80454C40(void *stream, void *entry, u32 *object) {
    u32 *nested = (u32 *)object[31];
    if (fn_8047D458(stream, (u8 *)nested + 44, 4) == 0) {
        return NULL;
    }
    nested = (u32 *)object[31];
    if (fn_8047D458(stream, (u8 *)nested + 48, 4) != 0) {
        return stream;
    }
    return NULL;
}

s32 fn_804549DC(u32 *);
u32 fn_80480D4C(void *, void *, u32);
void fn_8047759C(u32 *);
extern u32 lbl_805F1498;
extern u8 *lbl_805F15B4;

u32 fn_80454294(u32 *object) {
    if (fn_804549DC(object) == 0) {
        goto failure;
    }
    u32 *entry = (u32 *)((u8 *)object + lbl_805F149C);
    u32 size = 16;
    if (entry[2] != 0) {
        size = 48;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    entry++;
    if (entry[2] != 0) {
        size += 32;
    }
    goto success;
failure:
    return 0;
success:
    return size;
}

u32 *fn_80454374(void *stream) {
    u32 *object = ((u32 *(*)(u32 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        (u32 *)lbl_805F1498, 0x00030135);
    if (object == NULL) {
        return NULL;
    }
    if (fn_80480D4C(stream, object, 32) == 0) {
        return NULL;
    }
    if (fn_8047D868(stream, object + 8, 32) == 0) {
        return NULL;
    }
    object[16] = 1;
    return object;
}

u32 fn_8045459C(u32 *object) {
    u32 *nested = (u32 *)object[5];
    nested[16] = nested[16] - 1;
    u32 released;
    if (nested[16] != 0) {
        released = 0;
    } else {
        ((void (*)(u32 *, u32 *))*(u32 *)(lbl_805F15B4 + 0x11C))(
            (u32 *)lbl_805F1498, (u32 *)object[5]);
        released = 1;
    }
    if ((s32)released != 0) {
        fn_8047759C(object);
    }
    return 1;
}

u32 *fn_80477468(u32, u32, u32);
void fn_803ACABC(u32 *, u32 *, u32);
void *memcpy(void *, const void *, u32);
extern u32 lbl_8056D77C[];
extern u32 lbl_8056D7AC[];

#pragma use_lmw_stmw off
u32 *fn_8045449C(u32 *source, u32 count, u32 config, u32 *dest, s32 mode) {
    u32 *owner = NULL;
    switch (mode) {
    case 0:
        owner = fn_80477468(lbl_8056D77C[0], config, 0);
        break;
    case 1:
        owner = fn_80477468(lbl_8056D7AC[0], config, 0);
        break;
    default:
        break;
    }
    if (owner == NULL) {
        goto failure;
    }
    u32 *object = ((u32 *(*)(u32 *, u32))*(u32 *)(lbl_805F15B4 + 0x118))(
        (u32 *)lbl_805F1498, 0x00030135);
    if (object == NULL) {
        return NULL;
    }
    fn_803ACABC(object, source, 32);
    ((u8 *)object)[31] = 0;
    memcpy(object + 8, dest, count * 4);
    object[16] = 1;
    u32 *result = owner;
    owner[5] = (u32)object;
    goto success;
failure:
    result = NULL;
success:
    return result;
}
#pragma use_lmw_stmw reset

s32 fn_804549DC(u32 *object) {
    u32 *entry = (u32 *)((u8 *)object + lbl_805F149C);
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    entry++;
    if (entry[2] != 0) return 1;
    return 0;
}

s32 fn_8047CF50(void *, u32, u32 *, u32 *);
u32 fn_804790C4(u32 *);
u32 *fn_804790D4(u32, u32 *);
void fn_80453018(u32 *, u32 *, u32 *, u32 *, f32);
void fn_804779A8(u32 *);
u32 fn_804779DC(u32 *);
s32 fn_80477AB8(u32 *, u32 *);
u32 fn_80453D08(u32 *);
extern u32 lbl_8056D748[];
extern f32 lbl_805FB0F8;
extern f32 lbl_805FB0FC;
extern u32 lbl_805FB558;

#pragma use_lmw_stmw off
void *fn_80453FB0(void *stream, void *entry, u32 *table) {
    u32 flags;
    u32 scratchA;
    u32 scratchB;
    if (fn_8047CF50(stream, 1, NULL, NULL) == 0) {
        goto failure;
    }
    if (fn_8047D868(stream, &flags, 4) == 0) {
        goto failure;
    }
    u32 *base = (u32 *)((u8 *)table + lbl_805F149C);
    u32 *current = base;
    for (u32 i = 0; i < 8; i++) {
        if ((flags & (1 << i)) != 0) {
            u32 buffer[39];
            if (fn_80480D4C(stream, buffer, 32) == 0) {
                goto failure;
            }
            u32 *owner = NULL;
            u32 existing = 0;
            u32 source = fn_804790C4(lbl_8056D748);
            if (source != 0) {
                owner = fn_804790D4(source, buffer);
                if (owner != NULL) {
                    fn_80454618(owner);
                    existing = 1;
                }
            }
            if (existing == 0) {
                u32 *scratch;
                if (source != 0) {
                    scratchA = lbl_805FB558;
                    scratch = &scratchA;
                } else {
                    scratchB = lbl_805FB558;
                    scratch = &scratchB;
                }
                owner = fn_8045449C((u32 *)buffer, 1, 2, scratch, 0);
                if (owner == NULL) {
                    goto failure;
                }
            }
            u32 *config = (u32 *)owner[0];
            u32 *first = (u32 *)owner[4];
            u32 *second = (u32 *)((u8 *)first + config[2]);
            if (existing != 0) {
                buffer[27] |= 0x00020003;
                ((f32 *)buffer)[28] = lbl_805FB0FC;
                ((f32 *)buffer)[34] = lbl_805FB0F8;
                ((f32 *)buffer)[29] = lbl_805FB0F8;
                ((f32 *)buffer)[24] = lbl_805FB0F8;
                ((f32 *)buffer)[26] = lbl_805FB0FC;
                ((f32 *)buffer)[25] = lbl_805FB0FC;
                ((f32 *)buffer)[30] = lbl_805FB0FC;
                ((f32 *)buffer)[32] = lbl_805FB0FC;
                ((f32 *)buffer)[33] = lbl_805FB0FC;
                ((f32 *)buffer)[36] = lbl_805FB0FC;
                ((f32 *)buffer)[37] = lbl_805FB0FC;
                ((f32 *)buffer)[38] = lbl_805FB0FC;
                fn_80453018(owner, second, NULL, buffer + 24, lbl_805FB0F8);
                fn_80453018(owner, first, second, buffer + 24, lbl_805FB0F8);
            } else {
                buffer[11] |= 0x00020003;
                ((f32 *)buffer)[12] = lbl_805FB0FC;
                ((f32 *)buffer)[18] = lbl_805FB0F8;
                ((f32 *)buffer)[13] = lbl_805FB0F8;
                ((f32 *)buffer)[8] = lbl_805FB0F8;
                ((f32 *)buffer)[10] = lbl_805FB0FC;
                ((f32 *)buffer)[9] = lbl_805FB0FC;
                ((f32 *)buffer)[14] = lbl_805FB0FC;
                ((f32 *)buffer)[16] = lbl_805FB0FC;
                ((f32 *)buffer)[17] = lbl_805FB0FC;
                ((f32 *)buffer)[20] = lbl_805FB0FC;
                ((f32 *)buffer)[21] = lbl_805FB0FC;
                ((f32 *)buffer)[22] = lbl_805FB0FC;
                fn_80453018(owner, first, NULL, buffer + 8, lbl_805FB0F8);
                fn_80453018(owner, second, first, buffer + 8, lbl_805FB0F8);
            }
            if (owner == NULL) {
                goto failure;
            }
            fn_804779A8(owner);
            u32 result = fn_804779DC(((u32 **)owner[0])[1]);
            current[2] = result;
            if (result == 0 || fn_80477AB8((u32 *)result, owner) == 0) {
                fn_8045459C(owner);
                return NULL;
            }
        }
        current++;
    }
    if (fn_80453D08(base) != 0) {
        return stream;
    }
failure:
    return NULL;
}
#pragma use_lmw_stmw reset
#pragma optimize_for_size reset

void fn_804532E4(void);
void fn_804535C8(void);
void fn_8047EBA0(u32 *);
void fn_8047F59C(u32 *, u32 *, u32);
void fn_8047EFC8(u32 *, u32 *, u32, f32);
void fn_8047F72C(u32 *, u32 *, u32);
u32 *fn_804A2BE0(u32 *, u32 *, u32 *);
extern u32 lbl_8051BD40[];
extern f32 lbl_805FB100;
extern f32 lbl_805FB104;

#pragma use_lmw_stmw off
u32 *fn_8045462C(u32 *object) {
    u32 *base = (u32 *)((u8 *)object + lbl_805F149C);
    f32 first = lbl_805FB0F8;
    f32 second = lbl_805FB0FC;
    u32 *record = (u32 *)base[0];
    if (record != NULL) {
        ((f32 *)record)[10] = first;
        ((f32 *)record)[5] = first;
        ((f32 *)record)[0] = first;
        ((f32 *)record)[4] = second;
        ((f32 *)record)[2] = second;
        ((f32 *)record)[1] = second;
        ((f32 *)record)[9] = second;
        ((f32 *)record)[8] = second;
        ((f32 *)record)[6] = second;
        ((f32 *)record)[14] = second;
        ((f32 *)record)[13] = second;
        ((f32 *)record)[12] = second;
        record[3] |= 0x00020003;
    }
    record = (u32 *)base[1];
    if (record != NULL) {
        ((f32 *)record)[10] = first;
        ((f32 *)record)[5] = first;
        ((f32 *)record)[0] = first;
        ((f32 *)record)[4] = second;
        ((f32 *)record)[2] = second;
        ((f32 *)record)[1] = second;
        ((f32 *)record)[9] = second;
        ((f32 *)record)[8] = second;
        ((f32 *)record)[6] = second;
        ((f32 *)record)[14] = second;
        ((f32 *)record)[13] = second;
        ((f32 *)record)[12] = second;
        record[3] |= 0x00020003;
    }
    u32 *current = base;
    for (s32 i = 0; i < 8; i++) {
        u32 *entry = (u32 *)current[2];
        if (entry != NULL) {
            u32 *config = (u32 *)entry[0];
            u32 stride = entry[9];
            u32 callback = entry[15];
            s32 count = (s32)entry[11];
            u32 *data = entry + 19;
            u32 *types = (u32 *)(config[5] + 32);
            for (s32 j = 0; j < count; j++) {
                u32 type = types[0];
                types++;
                if (type < 2) {
                    u32 *target = (u32 *)base[type];
                    if (target != NULL) {
                        u32 buffer[15];
                        if (callback == (u32)fn_804532E4) {
                            ((f32 *)buffer)[0] = ((f32 *)data)[2];
                            ((f32 *)buffer)[1] = ((f32 *)data)[3];
                            ((f32 *)buffer)[2] = lbl_805FB0FC;
                            ((f32 *)buffer)[4] = ((f32 *)data)[4];
                            ((f32 *)buffer)[5] = ((f32 *)data)[5];
                            ((f32 *)buffer)[6] = lbl_805FB0FC;
                            ((f32 *)buffer)[8] = lbl_805FB0FC;
                            ((f32 *)buffer)[9] = lbl_805FB0FC;
                            ((f32 *)buffer)[10] = lbl_805FB0FC;
                            ((f32 *)buffer)[12] = ((f32 *)data)[6];
                            ((f32 *)buffer)[13] = ((f32 *)data)[7];
                            ((f32 *)buffer)[14] = lbl_805FB0FC;
                            fn_8047EBA0(buffer);
                        } else if (callback == (u32)fn_804535C8) {
                            u32 extra[3];
                            extra[0] = lbl_8051BD40[6];
                            extra[1] = lbl_8051BD40[7];
                            extra[2] = lbl_8051BD40[8];
                            ((f32 *)buffer)[0] = ((f32 *)data)[3];
                            ((f32 *)buffer)[1] = ((f32 *)data)[5];
                            ((f32 *)buffer)[2] = lbl_805FB0FC;
                            ((f32 *)buffer)[4] = lbl_805FB0FC;
                            ((f32 *)buffer)[5] = ((f32 *)data)[4];
                            ((f32 *)buffer)[6] = lbl_805FB0FC;
                            ((f32 *)buffer)[8] = lbl_805FB0FC;
                            ((f32 *)buffer)[9] = lbl_805FB0FC;
                            ((f32 *)buffer)[10] = lbl_805FB0FC;
                            ((f32 *)buffer)[12] = ((f32 *)data)[6];
                            ((f32 *)buffer)[13] = ((f32 *)data)[7];
                            ((f32 *)buffer)[14] = lbl_805FB0FC;
                            fn_8047EBA0(buffer);
                            fn_8047F59C(buffer, lbl_8051BD40, 2);
                            f32 value = lbl_805FB100 * (((f32 *)data)[2] / lbl_805FB104);
                            fn_8047EFC8(buffer, extra, 2, value);
                            fn_8047F59C(buffer, lbl_8051BD40 + 3, 2);
                        } else {
                            ((void (*)(u32 *, u32 *))callback)(buffer, data);
                        }
                        fn_8047F72C(target, buffer, 1);
                    }
                }
                data = (u32 *)((u8 *)data + stride);
            }
        }
        current++;
    }
    return fn_804A2BE0(object, (u32 *)base[0], (u32 *)base[1]);
}
#pragma use_lmw_stmw reset

u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D684(void *, void *, u32);
void *memset(void *, int, u32);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(u32 *);
void fn_8047D254(void *, u32);
void fn_8048209C(void *, void *);
u32 fn_804816AC(void *, void *, void *);
void fn_80481870(void *, u32, void *, u32);
void fn_8045F6E0(u32 *);
void fn_8045F7D0(u32 *);
extern u32 lbl_8056D880[];

#define RWCPP17_ERROR13(value)                                                               \
    do {                                                                                     \
        u32 error[2];                                                                        \
        error[0] = 2;                                                                        \
        error[1] = fn_8047DC28(0x80000013, value);                                           \
        fn_8047DB84(error);                                                                  \
    } while (0)

#pragma use_lmw_stmw off
u32 *fn_80454CE4(void *stream, u8 **cursor, u32 *object, u32 flags) {
    u32 version;
    if (fn_8047CF50(stream, 1, NULL, &version) == 0) {
        return NULL;
    }
    if (version < 0x00035000 || version > 0x00037002) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        return NULL;
    }
    u32 header[11];
    if (fn_80480D4C(stream, header, 44) != 44) {
        return NULL;
    }
    fn_8047D254(header, 44);
    u32 *base = (u32 *)*cursor;
    *cursor += lbl_8056D880[0];
    if (base == NULL) {
        RWCPP17_ERROR13(lbl_8056D880[0]);
        return NULL;
    }
    base[0] = 0xFFFFFFFF;
    base[27] = header[3];
    base[28] = header[4];
    base[29] = header[5];
    base[24] = header[6];
    base[25] = header[7];
    base[26] = header[8];
    ((u16 *)base)[64] = (u16)header[0];
    ((u16 *)base)[66] = (u16)header[1];
    ((u16 *)base)[65] = (u16)header[2];
    base[1] = 0;
    base[2] = 0;
    base[12] = 0;
    base[3] = 0;
    memset(base + 4, 0, 32);
    base[31] = 0;
    base[13] = 0;
    base[30] = 0;
    base[14] = (u32)(base + 14);
    base[15] = (u32)(base + 14);
    base[16] = (u32)(base + 16);
    base[17] = (u32)(base + 16);
    s32 status = 1;
    if ((object[2] & 0x01000000) != 0) {
        goto finalize;
    }
    if (header[2] != 0) {
        u32 bytes = header[2] * 12;
        u32 *entries = (u32 *)*cursor;
        *cursor += bytes;
        base[2] = (u32)entries;
        if (entries == NULL) {
            RWCPP17_ERROR13(bytes);
            status = 0;
        }
        if (status != 0 && fn_8047D784(stream, entries, bytes) == 0) {
            status = 0;
        }
    }
    if (status != 0 && (flags & 0x10) != 0) {
        u32 bytes = header[2] * 4;
        u32 *entries = (u32 *)*cursor;
        *cursor += bytes;
        base[3] = (u32)entries;
        if (entries == NULL) {
            RWCPP17_ERROR13(bytes);
            status = 0;
        }
        if (status != 0 && fn_80480D4C(stream, entries, bytes) != bytes) {
            status = 0;
        }
    }
    if (status != 0 && (flags & 0x08) != 0) {
        u32 bytes = header[2] * 4;
        u32 *entries = (u32 *)*cursor;
        *cursor += bytes;
        base[12] = (u32)entries;
        if (entries == NULL) {
            RWCPP17_ERROR13(bytes);
            status = 0;
        }
        if (status != 0 && fn_80480D4C(stream, entries, bytes) == 0) {
            status = 0;
        }
    }
    if (status != 0 && (s32)object[8] > 0) {
        u32 stride = ((u16 *)base)[65] * 8;
        u32 *current = base;
        for (s32 i = 0; i < (s32)object[8]; i++) {
            u32 *entries = (u32 *)*cursor;
            *cursor += stride;
            current[4] = (u32)entries;
            if (status != 0 && entries == NULL) {
                RWCPP17_ERROR13(stride);
                status = 0;
                break;
            }
            if (status != 0 && fn_8047D784(stream, entries, stride) == 0) {
                status = 0;
                break;
            }
            current++;
        }
    }
    if (status != 0 && header[1] != 0) {
        u32 bytes = header[1] * 8;
        u32 *entries = (u32 *)*cursor;
        *cursor += bytes;
        base[1] = (u32)entries;
        if (entries == NULL) {
            RWCPP17_ERROR13(bytes);
            status = 0;
        }
        if (status != 0 && fn_8047D684(stream, entries, bytes) == 0) {
            status = 0;
        }
    }
    if (status != 0 && version < 0x00036002) {
        u32 *entries = (u32 *)base[1];
        for (u32 i = 0; i < header[1]; i++) {
            u32 *entry = entries + i * 2;
            u32 first = entry[0];
            u32 second = entry[1];
            u16 *dest = (u16 *)entry;
            dest[0] = (u16)(first >> 16);
            dest[1] = (u16)(second >> 16);
            dest[2] = (u16)first;
            dest[3] = (u16)second;
        }
    }
finalize:
    if (status != 0) {
        fn_8048209C(lbl_8056D880, base);
        lbl_805F14A4 = 0;
        lbl_805F14A0 = 0;
        if (fn_804816AC(lbl_8056D880, stream, base) == 0) {
            status = 0;
        }
        if (status != 0 && lbl_805F14A4 != 0) {
            fn_80481870(lbl_8056D880, lbl_805F14A4, base, lbl_805F14A0);
        }
    }
    if (status != 0) {
        return base;
    }
    if ((((u8 *)object)[3] & 1) != 0) {
        fn_8045F6E0(base);
    } else {
        fn_8045F7D0(base);
    }
    return NULL;
}
#pragma use_lmw_stmw reset

#pragma use_lmw_stmw off
u32 *fn_804552AC(void *stream, u8 **cursor, u32 *object, u32 flags) {
    u32 length;
    u32 version;
    s32 status = 1;
    if (fn_8047CF50(stream, 1, &length, &version) == 0) {
        return NULL;
    }
    if (version < 0x00035000 || version > 0x00037002) {
        u32 error[2];
        error[0] = 2;
        error[1] = fn_8047DC28(0x80000004);
        fn_8047DB84(error);
        return NULL;
    }
    u32 header[6];
    memset(header, 0, 24);
    if (fn_80480D4C(stream, header, length) != length) {
        return NULL;
    }
    fn_8047D254(header, 24);
    u32 *base = (u32 *)*cursor;
    *cursor += 24;
    if (base == NULL) {
        RWCPP17_ERROR13(24);
        return NULL;
    }
    base[0] = header[0];
    ((f32 *)base)[1] = ((f32 *)header)[1];
    if ((flags & 0x80000000) != 0) {
        ((f32 *)base)[4] = ((f32 *)header)[4];
        ((f32 *)base)[5] = ((f32 *)header)[5];
    } else {
        ((f32 *)base)[4] = ((f32 *)header)[1];
        ((f32 *)base)[5] = ((f32 *)header)[1];
    }
    if (header[2] != 0) {
        if (fn_8047CF50(stream, 9, NULL, &version) == 0) {
            status = 0;
        }
        if (status != 0 && (version < 0x00035000 || version > 0x00037002)) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            status = 0;
        }
        if (status != 0) {
            u32 result = (u32)fn_80454CE4(stream, cursor, object, flags);
            base[2] = result;
            if (result == 0) {
                status = 0;
            }
        }
    } else {
        if (fn_8047CF50(stream, 10, NULL, &version) == 0) {
            status = 0;
        }
        if (status != 0 && (version < 0x00035000 || version > 0x00037002)) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            status = 0;
        }
        if (status != 0) {
            u32 result = (u32)fn_804552AC(stream, cursor, object, flags);
            base[2] = result;
            if (result == 0) {
                status = 0;
            }
        }
    }
    if (status != 0 && header[3] != 0) {
        if (fn_8047CF50(stream, 9, NULL, &version) == 0) {
            status = 0;
        }
        if (status != 0 && (version < 0x00035000 || version > 0x00037002)) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            status = 0;
        }
        if (status != 0) {
            u32 result = (u32)fn_80454CE4(stream, cursor, object, flags);
            base[3] = result;
            if (result == 0) {
                status = 0;
            }
        }
    } else if (status != 0) {
        if (fn_8047CF50(stream, 10, NULL, &version) == 0) {
            status = 0;
        }
        if (status != 0 && (version < 0x00035000 || version > 0x00037002)) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000004);
            fn_8047DB84(error);
            status = 0;
        }
        if (status != 0) {
            u32 result = (u32)fn_804552AC(stream, cursor, object, flags);
            base[3] = result;
            if (result == 0) {
                status = 0;
            }
        }
    }
    if (status != 0) {
        return base;
    }
    if (base[2] != 0) {
        if ((((u8 *)object)[3] & 1) != 0) {
            fn_8045F6E0((u32 *)base[2]);
        } else {
            fn_8045F7D0((u32 *)base[2]);
        }
        base[2] = 0;
    }
    return NULL;
}
#pragma use_lmw_stmw reset
#undef RWCPP17_ERROR13
}

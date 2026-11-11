/* RenderWare small callbacks and accessors. Names are inferred from behavior.
 * fn_8045CA38 has retail's instruction sequence and size; GC/2.7 assigns one extra saved register
 * to the final field temporaries. Direct, cached, and scoped comparison forms were measured.
 * fn_8045C340 also matches size; its repeated record stores retain addressing/register drift.
 * fn_8045C56C matches size and the expansion loops; only setup scheduling/register choices differ.
 * fn_8045C770 matches size and all conversion code; its entry-type load is scheduled one store later. */
#include <dolphin/types.h>

extern u8 *lbl_805F15B4;
extern u32 lbl_805F14F0;

extern u32 lbl_805FB170;

struct RwCpp22Record {
    u16 first;
    u16 second;
    u16 third;
    u16 padding;
    u32 value;
    u16 fourth;
    u16 fifth;
    u16 sixth;
    u16 seventh;
};

extern "C" {
u32 fn_8047DC28(u32 code, u32 size, ...);
void fn_8047DB84(u32 *error);

u8 *fn_8045C340(u32 *object, u32 value, u16 first, u16 second, u16 third,
                u16 fourth, u16 fifth, u16 sixth, u16 seventh) {
    if (object[1] >= object[0]) {
        u32 size = (object[1] + 1) * 20;
        u8 *buffer;
        if (object[1] != 0) {
            buffer = ((u8 *(*)(u8 *, u32, u32))*(u32 *)(lbl_805F15B4 + 0x110))(
                (u8 *)object[2], size, 0x01030502);
        } else {
            buffer = ((u8 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x01030502);
        }
        if (buffer == NULL) {
            u32 error[2];
            error[0] = 2;
            error[1] = fn_8047DC28(0x80000013, size);
            fn_8047DB84(error);
            return NULL;
        }
        object[2] = (u32)buffer;
        object[0] = object[1] + 1;
    }
    RwCpp22Record *record = (RwCpp22Record *)(object[2] + object[1] * 20);
    record->value = value;
    record = (RwCpp22Record *)(object[2] + object[1] * 20);
    record->first = first;
    record = (RwCpp22Record *)(object[2] + object[1] * 20);
    record->second = second;
    record = (RwCpp22Record *)(object[2] + object[1] * 20);
    record->third = third;
    record = (RwCpp22Record *)(object[2] + object[1] * 20);
    record->fourth = fourth;
    record = (RwCpp22Record *)(object[2] + object[1] * 20);
    record->fifth = fifth;
    record = (RwCpp22Record *)(object[2] + object[1] * 20);
    record->sixth = sixth;
    record = (RwCpp22Record *)(object[2] + object[1] * 20);
    record->seventh = seventh;
    object[1]++;
    return (u8 *)object[2];
}

u32 fn_8047D458(u8 *stream, void *data, u32 size);
s32 fn_8045B8EC(u32 *object, u32 value);

u8 *fn_8045C56C(u32 *object, u8 *entry, u8 *stream, u32 *search) {
    u32 pair[2];
    u32 header[3];
    u32 first = object[0];
    u32 second = *(u16 *)((u8 *)object + 4);
    u32 third = object[2];
    header[0] = first;
    header[1] = second;
    header[2] = third;
    if (fn_8047D458(stream, header, 12) == 0) {
        return NULL;
    }
    u32 type;
    u32 count = *(u16 *)((u8 *)object + 4);
    u8 *record = (u8 *)object + 16;
    type = entry[0];
    while (count-- != 0) {
        u32 key = ((u32 *)record)[2];
        pair[0] = ((u32 *)record)[1];
        pair[1] = fn_8045B8EC(search, key);
        if ((s32)pair[1] < 0) {
            pair[1] = 0;
        }
        if (fn_8047D458(stream, pair, 8) == 0) {
            return NULL;
        }
        if (type == 8 && !(*(u32 *)(entry + 8) & 0x01000000)) {
            goto convert;
        }
        if (type == 7 && !(*(u32 *)(entry + 8) & 0x01000000)) {
            goto convert;
        }
        goto next_record;
convert:
        {
            u32 remaining = ((u32 *)record)[1];
            u16 *source = (u16 *)((u32 *)record)[0];
            u32 values[256];
            while (remaining != 0) {
                u32 chunk = remaining < 256 ? remaining : 256;
                u32 i = 0;
                while (i < chunk) {
                    values[i] = *source++;
                    i++;
                }
                if (fn_8047D458(stream, values, chunk * 4) == 0) {
                    return NULL;
                }
                remaining -= chunk;
            }
        }
next_record:
        record += 12;
    }
    return stream;
}

u32 fn_8047D868(u8 *stream, void *data, u32 size);
u32 fn_8045B79C(u32 *object, u32 index);

u32 *fn_8045C770(u8 *stream, u8 *entry, u32 *search) {
    u32 pair[2];
    u32 header[3];
    if (fn_8047D868(stream, header, 12) == 0) {
        return NULL;
    }
    u32 size = header[1] * 16 + 16;
    if (entry[0] == 8 && !(*(u32 *)(entry + 8) & 0x01000000)) {
        goto add_values;
    }
    if (entry[0] == 7 && !(*(u32 *)(entry + 8) & 0x01000000)) {
        goto add_values;
    }
    goto size_ready;
add_values:
    size += header[2] * 2;
size_ready:
    {
        u32 *object = ((u32 *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x00030502);
        if (object == NULL) {
            goto done;
        }
        u32 type;
        u8 *record = (u8 *)object + 16;
        u8 *output = record + header[1] * 12;
        object[0] = header[0];
        type = entry[0];
        *(u16 *)((u8 *)object + 4) = header[1];
        *(s16 *)((u8 *)object + 6) = *(s16 *)(lbl_805F15B4 + lbl_805F14F0);
        object[2] = header[2];
        object[3] = 0;
        (*(s16 *)(lbl_805F15B4 + lbl_805F14F0))++;
        u32 count = *(u16 *)((u8 *)object + 4);
        while (count-- != 0) {
            if (fn_8047D868(stream, pair, 8) == 0) {
                return NULL;
            }
            ((u32 *)record)[1] = pair[0];
            ((u32 *)record)[2] = fn_8045B79C(search, pair[1]);
            ((u32 *)record)[0] = (u32)output;
            if (type == 8 && !(*(u32 *)(entry + 8) & 0x01000000)) {
                goto convert;
            }
            if (type == 7 && !(*(u32 *)(entry + 8) & 0x01000000)) {
                goto convert;
            }
            goto next_record;
convert:
            {
                u32 remaining = ((u32 *)record)[1];
                u32 values[256];
                while (remaining != 0) {
                    u32 *input = values;
                    u32 chunk = remaining < 256 ? remaining : 256;
                    if (fn_8047D868(stream, input, chunk * 4) == 0) {
                        return NULL;
                    }
                    remaining -= chunk;
                    while (chunk-- != 0) {
                        *(u16 *)output = *input++;
                        output += 2;
                    }
                }
            }
next_record:
            record += 12;
        }
done:
        return object;
    }
}

s32 fn_8045CA38(u32 **left_argument, u32 **right_argument) {
    s32 left_value = 0;
    s32 right_value = 0;
    u32 *left_child = (u32 *)(*left_argument)[2];
    u32 *right_child = (u32 *)(*right_argument)[2];
    if (left_child == right_child) {
        return 0;
    }
    if (left_child != NULL) {
        u32 *nested = (u32 *)left_child[0];
        if (nested != NULL) {
            s32 type = (*(u8 *)(*(u32 *)nested + 35) & 0xF) << 8;
            if (type == 0x100 || type == 0x300 || type == 0x500) {
                left_value = lbl_805FB170;
            }
        }
        if (((u8 *)left_child)[7] != 0xFF) {
            left_value |= lbl_805FB170;
        }
    }
    if (right_child != NULL) {
        u32 *nested = (u32 *)right_child[0];
        if (nested != NULL) {
            s32 type = (*(u8 *)(*(u32 *)nested + 35) & 0xF) << 8;
            if (type == 0x100 || type == 0x300 || type == 0x500) {
                right_value = lbl_805FB170;
            }
        }
        if (((u8 *)right_child)[7] != 0xFF) {
            right_value |= lbl_805FB170;
        }
    }
    left_value |= (*(u16 *)((u8 *)*left_argument + 16) > *(u16 *)((u8 *)*right_argument + 16)) ? 8 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 16) < *(u16 *)((u8 *)*right_argument + 16)) ? 8 : 0;
    left_value |= (*(u16 *)((u8 *)*left_argument + 18) > *(u16 *)((u8 *)*right_argument + 18)) ? 4 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 18) < *(u16 *)((u8 *)*right_argument + 18)) ? 4 : 0;
    left_value |= (*(u16 *)((u8 *)*left_argument + 14) > *(u16 *)((u8 *)*right_argument + 14)) ? 2 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 14) < *(u16 *)((u8 *)*right_argument + 14)) ? 2 : 0;
    left_value |= (*(u16 *)((u8 *)*left_argument + 12) > *(u16 *)((u8 *)*right_argument + 12)) ? 1 : 0;
    right_value |= (*(u16 *)((u8 *)*left_argument + 12) < *(u16 *)((u8 *)*right_argument + 12)) ? 1 : 0;
    return left_value - right_value;
}

void fn_8045D20C(u32 *object, u32 **array, s32 index) {
    if (index < 4) {
        object[7] = 1;
        return;
    }
    object[6] = 1;
    u32 *head = array[((u8 *)object)[32]];
    if (head == object) {
        array[((u8 *)object)[32]] = (u32 *)head[4];
        if (array[((u8 *)object)[32]] != NULL) {
            array[((u8 *)object)[32]][5] = 0;
        }
    } else {
        if (object[4] != 0) {
            ((u32 *)object[4])[5] = object[5];
        }
        if (object[5] != 0) {
            ((u32 *)object[5])[4] = object[4];
        }
    }
    for (u8 i = 0; i < 3; i++) {
        u32 *child;
        u32 *candidate = NULL;
        u32 *container = (u32 *)object[i + 1];
        child = (u32 *)container[1];
        if (child != NULL && child != object && (s32)child[6] == 0) {
            candidate = child;
        } else {
            child = (u32 *)container[2];
            if (child != NULL && (s32)child[6] == 0) {
                candidate = child;
            }
        }
        if (candidate != NULL) {
            head = array[((u8 *)candidate)[32]];
            if (head == candidate) {
                array[((u8 *)candidate)[32]] = (u32 *)head[4];
                if (array[((u8 *)candidate)[32]] != NULL) {
                    array[((u8 *)candidate)[32]][5] = 0;
                }
            } else {
                if (candidate[4] != 0) {
                    ((u32 *)candidate[4])[5] = candidate[5];
                }
                if (candidate[5] != 0) {
                    ((u32 *)candidate[5])[4] = candidate[4];
                }
            }
            ((u8 *)candidate)[32]--;
            candidate[4] = (u32)array[((u8 *)candidate)[32]];
            if (candidate[4] != 0) {
                ((u32 *)candidate[4])[5] = (u32)candidate;
            }
            array[((u8 *)candidate)[32]] = candidate;
            candidate[5] = 0;
        }
    }
}

u32 fn_8045C9C8(u32 *object, u8 *entry) {
    if (entry[0] == 8 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    if (entry[0] == 7 && (*(u32 *)(entry + 8) & 0x01000000)) {
        goto fixed_size;
    }
    goto variable_size;
fixed_size:
    return *(u16 *)((u8 *)object + 4) * 8 + 12;
variable_size:
    {
        u32 count = *(u16 *)((u8 *)object + 4);
        u32 result = count * 8;
        result += object[2] * 4;
        result += 12;
        return result;
    }
}

u32 *fn_8045C4E4(u32 *object, u32 (*callback)(u8 *, u32 *, u32), u32 argument) {
    s32 count = *(u16 *)((u8 *)object + 4);
    u8 *entry = (u8 *)object + object[3];
    entry += 16;
    while (count-- != 0) {
        if (callback(entry, object, argument) == 0) {
            return object;
        }
        entry += 12;
    }
    return object;
}

void fn_8045CA20(void) {
    s16 *target = (s16 *)((u8 *)lbl_805F15B4 + lbl_805F14F0);
    *target += 1;
}
}

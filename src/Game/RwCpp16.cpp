/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern u32 lbl_805F1458;
extern u32 lbl_805F1460;
extern u32 lbl_805F145C;
extern u32 lbl_805F1464;
extern u32 lbl_805F1468;
extern u32 lbl_805F146C;
extern u32 lbl_805F1470;
extern u32 lbl_805F1474;
extern u32 lbl_805F1478;
extern u32 lbl_805F147C;
extern u32 lbl_805F1480;
extern u32 lbl_805F1484;
extern u32 lbl_805F1488;
extern u32 lbl_805F148C;
extern u32 lbl_805F1498;
extern u32 lbl_805EDD88;
extern u32 lbl_805EDD90;
extern u32 lbl_805EDD94;
extern u32 lbl_8056D77C;
extern u32 lbl_8056D7AC;
extern u8 lbl_8056D748[];
extern u8 *lbl_805F15B4;
extern float lbl_805FB0D8;
extern float lbl_805FB0DC;
extern float lbl_805FB0E0;
extern float lbl_805FB0E4;
extern float lbl_805FB0E8;
extern float lbl_805FB0EC;
extern float lbl_805FB0F0;
extern float lbl_8051BD28[];
extern float lbl_8051BD34[];
extern float lbl_8051BD40[];
extern float lbl_8051BD4C[];

/* Engine instance vtable handed to every plugin; allocation and the string
   helpers live in it (see RwCpp06.cpp). */
static inline void *rwAlloc(u32 size, u32 tag) {
    return ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, tag);
}

static inline void rwFree(void *pointer) {
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(pointer);
}

static inline u32 rwStringLength(const char *string) {
    return ((u32 (*)(const char *))*(u32 *)(lbl_805F15B4 + 0xF4))(string);
}

static inline void rwStringCopy(char *dest, const char *source) {
    ((char *(*)(char *, const char *))*(u32 *)(lbl_805F15B4 + 0xCC))(dest, source);
}

extern "C" {
/* Engine entry points used below; the argument lists come from the call sites
   in this unit. */
u32 fn_80451F48(u32);
u32 fn_80451F5C(u32);
s32 fn_80485D78(u32, u32, void *, void *);
s32 fn_8045994C(u32, u32, void *, void *, void *);
s32 fn_8045998C(u32, void *, void *, void *);
s32 fn_8045F2F8(u32, u32, void *, void *, void *);
s32 fn_8045F338(u32, void *, void *, void *);
s32 fn_804878D4(u32, u32, void *, void *, void *);
s32 fn_804836EC(u32, void *, void *, void *);
s32 fn_804859B8(u32, u32, void *, void *, void *);
s32 fn_80483554(u32, void *, void *, void *);
s32 fn_8045AAA0(u32, u32, void *, void *, void *);
s32 fn_8045AAE0(u32, void *, void *, void *);
s32 fn_8045B2CC(u32, u32, void *, void *, void *);
s32 fn_8045B30C(u32, void *, void *, void *);
u32 fn_8048D280(u32, u32, void *, void *, void *);
s32 fn_80483AFC(u32, void *, void *, void *);

u32 fn_8047D868(void *, void *, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D350(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);
void *fn_8047CEF0(void *, u32, u32, u32, u32);

void *fn_80451F70(u32 *, void *);
void *fn_80452228(u32 *, void *);
void *fn_80454374(void *);
u32 fn_8045441C(u32, void *);
u32 fn_804548E4(u32 *);
u32 *fn_80454618(u32 *);
void fn_8045459C(u32 *);

u32 fn_804779A8(u32 *);
u32 fn_804779DC(u32, u32);
void fn_80477A8C(u32);
void fn_80477BF0(u32 *, u32 *);
void fn_80477344(void *);
void *fn_8047FC04(u32, void *, u32, void *, void *, u32);
void fn_8047FCDC(void *);
void fn_80479040(void *);
u32 fn_8047909C(u32 *);

void fn_8047EBA0(float *);
void fn_8047EFC8(float *, const float *, float, u32);
void fn_8047F59C(float *, const float *, u32);
void *fn_8047F994(void);
void fn_8047F958(void *);

double fn_803B1C3C(double, double);

u32 fn_80452C98(s32);
float *fn_80453820(float *, const float *);

/* Dictionary stream/interpolation callbacks registered by fn_80452CDC,
 * defined further down in this unit. */
u32 fn_80453018(u32 *, u32 *, u32, float, u32 *);
u32 fn_804530CC(void *, u32 *);
u32 fn_804531B0(u32 *, void *);
void fn_804532E4(float *, const float *);
void fn_80453350(float *, const float *, const float *, float);
void fn_804533E0(float *, const float *, const float *, float);
void fn_8045345C(float *, const float *);
void fn_80453500(float *, const float *, const float *);
void fn_804535C8(float *, const float *);
void fn_804536B4(float *, const float *, const float *, float);
void fn_80453770(float *, const float *, const float *, float);

/* Node-array handlers and the chunk 0x135 writer registered by fn_80453A78,
 * defined further down in this unit. */
u32 *fn_80453B64(u32 *);
u32 *fn_80453C04(u32 *, u32 *);
void *fn_80453E24(void *, u32, u8 *);

/* The read and size halves of the chunk 0x135 trio live in the next unit;
 * the size handler walks the same keyframe payload and answers in bytes. */
void *fn_80453FB0(void *, u32, u8 *);
u32 fn_80454294(u8 *);
void fn_80453818(void) {
}

void fn_8045381C(void) {
}

u32 fn_80452C78(u32 *object) {
    return object[0];
}

u32 fn_80452C80(u32 *object) {
    return object[1];
}

u32 fn_80452C88(u32 *object, u32 index) {
    return ((u32 *)object[3])[index];
}

u8 *fn_80452730(u8 *base, u32 offset) {
    u8 *target = base + offset;
    *(u32 *)target = 0;
    *(u32 *)(target + 4) = 0;
    return base;
}

u32 fn_80452BD8(u8 *object, s32 index) {
    object += lbl_805F1458;
    s32 count = *(s32 *)object;
    if (index < count) {
        return *(u32 *)(object + 4) + index * 0x10;
    }
    return 0;
}

u32 fn_80452C4C(u8 *object, s32 index) {
    object += lbl_805F1460;
    s32 count = *(s32 *)object;
    if (index < count) {
        return *(u32 *)(object + 4) + index * 0x10;
    }
    return 0;
}

void fn_80453594(float *dest, const float *source) {
    dest[0] = source[0];
    dest[1] = source[1];
    dest[2] = source[4];
    dest[3] = source[5];
    dest[4] = source[0x0C];
    dest[5] = source[0x0D];
}

void fn_80452434(u8 *, u8 *);

void *fn_80452860(u8 *object, u8 *other, u32 offset) {
    fn_80452434(object + offset, other + offset);
    return object;
}

u32 fn_80454494(u32);

u32 fn_804532A4(u32 *object) {
    u32 result = fn_80454494(object[5]);
    result += 4;
    result += object[1] << 5;
    return result;
}

extern u32 lbl_805F149C;

void *fn_80453B24(u8 *object) {
    memset(object + lbl_805F149C, 0, 0x28);
    return object;
}

/* Chunk 0x11F dictionaries are a header pair { count, entries } followed by
   0x10 byte entries { name, type, count, values }. Names and the values of a
   type 3 entry are separately allocated strings, so a copy has to be deep. */
void fn_80452434(u8 *dest, u8 *source)
{
    u32 *dictionary = (u32 *)dest;
    if (dictionary[1] != 0) {
        for (s32 i = 0, off = i; i < (s32)dictionary[0]; i++, off += 0x10) {
            u32 *entry = (u32 *)(dictionary[1] + off);
            if (entry[0] != 0) {
                rwFree((void *)entry[0]);
                entry[0] = 0;
            }
            if ((s32)entry[1] == 3) {
                u32 *value = (u32 *)entry[3];
                for (s32 j = 0; j < (s32)entry[2]; j++, value++) {
                    if (value[0] != 0) {
                        rwFree((void *)value[0]);
                    }
                }
            }
            if (entry[3] != 0) {
                rwFree((void *)entry[3]);
                entry[3] = 0;
            }
        }
        rwFree((void *)dictionary[1]);
    }
    /* Retail clears the header before the count is copied, so the write to [0]
       is dead; the order is kept because the assembly has it. */
    dictionary[1] = 0;
    dictionary[0] = 0;
    dictionary[0] = ((u32 *)source)[0];
    if ((s32)dictionary[0] > 0) {
        dictionary[1] = (u32)rwAlloc(dictionary[0] << 4, 0x3011F);
        u8 *sourceEntries = (u8 *)((u32 *)source)[1];
        for (s32 i = 0, off = 0; i < (s32)dictionary[0]; i++, off += 0x10) {
            u32 *from = (u32 *)(sourceEntries + off);
            u32 *to = (u32 *)(dictionary[1] + off);
            to[1] = from[1];
            to[2] = from[2];
            to[0] = 0;
            if (from[0] != 0) {
                u32 length = rwStringLength((char *)from[0]);
                to[0] = (u32)rwAlloc(length + 1, 0x30002);
                if (to[0] != 0) {
                    rwStringCopy((char *)to[0], (char *)from[0]);
                }
            }
            if (from[3] != 0) {
                u32 element = fn_80452C98(to[1]);
                u32 bytes = to[2] * element;
                to[3] = (u32)rwAlloc(bytes, 0x3011F);
                if ((s32)to[1] == 3) {
                    u32 *value = (u32 *)from[3];
                    u32 *copy = (u32 *)to[3];
                    for (s32 j = 0; j < (s32)to[2]; j++, value++, copy++) {
                        copy[0] = 0;
                        if (value[0] != 0) {
                            u32 length = rwStringLength((char *)value[0]);
                            copy[0] = (u32)rwAlloc(length + 1, 0x30002);
                            if (copy[0] != 0) {
                                rwStringCopy((char *)copy[0], (char *)value[0]);
                            }
                        }
                    }
                } else {
                    memcpy((void *)to[3], (void *)from[3], bytes);
                }
            }
        }
    }
}

/* Chunk 0x11F destructor: release the entry names, the type 3 value strings and
   the value buffers, then the entry array itself. */
u8 *fn_80452744(u8 *object, u32 offset)
{
    u32 *dictionary = (u32 *)(object + offset);
    if (dictionary[1] != 0) {
        for (s32 i = 0, off = i; i < (s32)dictionary[0]; i++, off += 0x10) {
            u32 *entry = (u32 *)(dictionary[1] + off);
            if (entry[0] != 0) {
                rwFree((void *)entry[0]);
                entry[0] = 0;
            }
            if ((s32)entry[1] == 3) {
                u32 *value = (u32 *)entry[3];
                for (s32 j = 0; j < (s32)entry[2]; j++, value++) {
                    if (value[0] != 0) {
                        rwFree((void *)value[0]);
                    }
                }
            }
            if (entry[3] != 0) {
                rwFree((void *)entry[3]);
                entry[3] = 0;
            }
        }
        rwFree((void *)dictionary[1]);
    }
    dictionary[1] = 0;
    dictionary[0] = 0;
    return object;
}

/* Chunk 0x11F stream read: the count comes first, then one entry at a time. The
   chained stream pointer is never tested, matching retail. */
void *fn_80452898(void *stream, u32 size, u8 *base, u32 offset)
{
    u32 *dictionary = (u32 *)(base + offset);
    void *current = stream;
    if (dictionary != 0) {
        u32 count;
        if (fn_8047D868(stream, &count, 4) != 0) {
            dictionary[0] = count;
            /* Retail allocates unconditionally: a zero count still gets a block. */
            dictionary[1] = (u32)rwAlloc(dictionary[0] << 4, 0x3011F);
            for (s32 i = 0, off = i; i < (s32)dictionary[0]; i++, off += 0x10) {
                current = fn_80451F70((u32 *)(dictionary[1] + off), current);
            }
        }
    }
    return stream;
}

/* Chunk 0x11F stream write: only entries that carry a name are counted and
   written, and the count of those is written first. */
void *fn_8045294C(void *stream, u32 size, u8 *base, u32 offset)
{
    u32 *dictionary = (u32 *)(base + offset);
    void *current = stream;
    if (dictionary != 0 && (s32)dictionary[0] > 0) {
        u32 named = 0;
        for (s32 i = 0, off = 0; i < (s32)dictionary[0]; i++, off += 0x10) {
            if (((u32 *)(dictionary[1] + off))[1] != 0) {
                named++;
            }
        }
        if (named != 0 && fn_8047D458(stream, &named, 4) != 0) {
            for (s32 i = 0, off = 0; i < (s32)dictionary[0]; i++, off += 0x10) {
                u32 *entry = (u32 *)(dictionary[1] + off);
                if (entry[1] != 0) {
                    current = fn_80452228(entry, current);
                }
            }
        }
    }
    return stream;
}

/* Chunk 0x11F stream size: count plus name plus type/count plus the values. */
u32 fn_80452A40(u8 *base, u32 offset)
{
    u32 *dictionary = (u32 *)(base + offset);
    u32 total = 0;
    if (dictionary != 0 && (s32)dictionary[0] > 0) {
        for (s32 i = 0, off = 0; i < (s32)dictionary[0]; i++, off += 0x10) {
            u32 *entry = (u32 *)(dictionary[1] + off);
            if ((s32)entry[1] == 0) {
                continue;
            }
            u32 size = 0;
            if (entry != 0) {
                size = 4;
                if (entry[0] != 0) {
                    size = rwStringLength((char *)entry[0]) + 5;
                }
                size += 8;
                switch ((s32)entry[1]) {
                case 1:
                case 2:
                    size += entry[2] << 2;
                    break;
                case 3: {
                    u32 *value = (u32 *)entry[3];
                    for (s32 j = 0; j < (s32)entry[2]; j++, value++) {
                        size += 4;
                        if (value[0] != 0) {
                            size += rwStringLength((char *)value[0]) + 1;
                        }
                    }
                    break;
                }
                default:
                    break;
                }
            }
            total += size;
        }
    }
    if ((s32)total > 0) {
        total += 4;
    }
    return total;
}

/* How many entries of the first dictionary carry values. */
u32 fn_80452B90(u8 *object)
{
    u32 found = 0;
    u32 offset = found;
    u32 *dictionary = (u32 *)(object + lbl_805F1458);
    for (s32 i = found; i < (s32)dictionary[0]; i++, offset += 0x10) {
        if (*(u32 *)(dictionary[1] + offset + 0x0C) != 0) {
            found++;
        }
    }
    return found;
}

/* The same count for the second dictionary. */
u32 fn_80452C04(u8 *object)
{
    u32 found = 0;
    u32 offset = found;
    u32 *dictionary = (u32 *)(object + lbl_805F1460);
    for (s32 i = found; i < (s32)dictionary[0]; i++, offset += 0x10) {
        if (*(u32 *)(dictionary[1] + offset + 0x0C) != 0) {
            found++;
        }
    }
    return found;
}

/* Byte width of one value for an entry of the given type. */
u32 fn_80452C98(s32 type)
{
    switch (type) {
    case 1:
        return 4;
    case 2:
        return 4;
    case 3:
        return 4;
    default:
        return 0;
    }
}

/* Install the chunk 0x11F plugin. Each registration hands back an id, and the
   guard after a pair tests the *previous* pair's global, which is what retail
   does. The later groups reuse the same callbacks over and over; the pattern is
   a repeating window across the interpolators built below. */
u32 fn_80452CDC(void)
{
    if (fn_80485D78(0, 0x11F, fn_80451F48, fn_80451F5C) < 0) {
        return 0;
    }
    lbl_805F1458 = fn_8045994C(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)lbl_805F1458 < 0) {
        return 0;
    }
    lbl_805F145C = fn_8045998C(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)lbl_805F145C < 0) {
        return 0;
    }
    lbl_805F1460 = fn_8045F2F8(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)lbl_805F1458 < 0) {
        return 0;
    }
    lbl_805F1464 = fn_8045F338(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)lbl_805F145C < 0) {
        return 0;
    }
    lbl_805F1468 = fn_804878D4(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)lbl_805F1468 < 0) {
        return 0;
    }
    lbl_805F146C = fn_804836EC(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)lbl_805F146C < 0) {
        return 0;
    }
    lbl_805F1470 = fn_804859B8(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)lbl_805F1470 < 0) {
        return 0;
    }
    lbl_805F1474 = fn_80483554(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)lbl_805F1474 < 0) {
        return 0;
    }
    lbl_805F1478 = fn_8045AAA0(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)lbl_805F1478 < 0) {
        return 0;
    }
    lbl_805F147C = fn_8045AAE0(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)lbl_805F147C < 0) {
        return 0;
    }
    lbl_805F1480 = fn_8045B2CC(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)lbl_805F1480 < 0) {
        return 0;
    }
    lbl_805F1484 = fn_8045B30C(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)lbl_805F1484 < 0) {
        return 0;
    }
    lbl_805F1488 = fn_8048D280(8, 0x11F, fn_80452730, fn_80452744, fn_80452860);
    if ((s32)lbl_805F1488 < 0) {
        return 0;
    }
    lbl_805F148C = fn_80483AFC(0x11F, fn_80452898, fn_8045294C, fn_80452A40);
    if ((s32)lbl_805F148C < 0) {
        return 0;
    }
    return 1;
}

/* Build one key record: id, time, then the payload for the object's class. The
   payload pointer is the caller's, so the two classes choose the layout. */
u32 fn_80453018(u32 *object, u32 *record, u32 id, float time, u32 *source)
{
    float *key = (float *)record;
    record[0] = id;
    key[1] = time;
    if (**(u32 **)object == lbl_8056D77C) {
        fn_80453594(&key[2], (const float *)source);
        /* Retail branches on this call's leftover r3 here; it is never zero, so
           the branch is not reachable in practice. */
    }
    if (**(u32 **)object == lbl_8056D7AC) {
        if (fn_80453820(&key[2], (const float *)source) != 0) {
            return 0;
        }
    }
    /* Every path leaves zero; upstream keeps that result for its caller. */
    return 0;
}

/* Node stream read: a discarded word, the shared node list, then every node. */
u32 fn_804530CC(void *stream, u32 *record)
{
    u32 discarded;
    if (fn_8047D868(stream, &discarded, 4) == 0) {
        return 0;
    }
    u32 list = (u32)fn_80454374(stream);
    if (list == 0) {
        return 0;
    }
    record[5] = list;
    u8 *base = (u8 *)record[4];
    u8 *node = base;
    for (s32 i = 0; i < (s32)record[1]; i++, node += 0x20) {
        if (fn_8047D784(stream, node + 4, 0x1C) == 0) {
            return 0;
        }
        u32 index;
        if (fn_8047D868(stream, &index, 4) == 0) {
            return 0;
        }
        *(u32 *)node = (u32)(base + (index << 5));
    }
    return (u32)record;
}

/* Node stream write. The arguments are the other way round from the read
   callback, and the discarded read above is mirrored by writing a zero. */
u32 fn_804531B0(u32 *record, void *stream)
{
    u32 zero = 0;
    if (fn_8047D458(stream, &zero, 4) == 0) {
        return 0;
    }
    if (fn_8045441C(record[5], stream) == 0) {
        return 0;
    }
    u8 *base = (u8 *)record[4];
    u8 *node = base;
    s32 count = (s32)record[1];
    for (s32 i = 0; i < count; i++, node += 0x20) {
        if (fn_8047D350(stream, node + 4, 0x1C) == 0) {
            return 0;
        }
        u32 index = (u32)(((s32)(*(u32 *)node - (u32)base)) / 32);
        if (fn_8047D458(stream, &index, 4) == 0) {
            return 0;
        }
    }
    return 1;
}

/* A key record's six payload floats as a 0x40 byte matrix; the untouched words
   3, 7, 11 and 15 are the flags and padding. */
void fn_804532E4(float *dest, const float *source)
{
    float zero = lbl_805FB0D8;
    dest[0] = source[2];
    dest[1] = source[3];
    dest[2] = zero;
    dest[4] = source[4];
    dest[5] = source[5];
    dest[6] = zero;
    dest[8] = zero;
    dest[9] = zero;
    dest[10] = zero;
    dest[12] = source[6];
    dest[13] = source[7];
    dest[14] = zero;
    fn_8047EBA0(dest);
}

/* Linearly blend payload words 2..7 of two records at time. */
#pragma scheduling off
void fn_80453350(float *out, const float *from, const float *to, float time)
{
    float ratio = (time - from[1]) / (to[1] - from[1]);
    for (u32 i = 2; i < 8; i++) {
        out[i] = ratio * (to[i] - from[i]) + from[i];
    }
}
#pragma scheduling reset

/* The same blend for a caller that has already worked out the ratio. */
#pragma scheduling off
void fn_804533E0(float *out, const float *from, const float *to, float ratio)
{
    for (u32 i = 2; i < 8; i++) {
        out[i] = ratio * (to[i] - from[i]) + from[i];
    }
}
#pragma scheduling reset

/* Undo the 2x2 block of a record on out, then take its translation back off. */
void fn_8045345C(float *out, const float *from)
{
    float determinant = from[2] * from[5] - from[3] * from[4];
    if (determinant != lbl_805FB0D8) {
        float inverse = lbl_805FB0DC / determinant;
        float upperRight = -from[3] * inverse;
        float lowerLeft = -from[4] * inverse;
        float lowerRight = from[5] * inverse;
        float m10 = out[4];
        float m11 = out[5];
        float m00 = out[2];
        float m01 = out[3];
        out[2] = lowerRight * m00 + upperRight * m10;
        out[3] = lowerRight * m01 + upperRight * m11;
        /* Retail multiplies both second row terms by the same cofactor; the
           (1,1) term of the adjugate is never computed. */
        out[4] = lowerLeft * m00 + lowerLeft * m10;
        out[5] = lowerLeft * m01 + lowerLeft * m11;
    }
    out[6] = out[6] - from[6];
    out[7] = out[7] - from[7];
}

/* Combine two record payloads; out may be one of the inputs. */
#pragma scheduling off
void fn_80453500(float *out, const float *from, const float *to)
{
    float first = *(volatile f32 *)(from + 3) * *(volatile f32 *)(to + 4);
    out[2] = from[2] * to[2] + first;
    float second = *(volatile f32 *)(from + 3) * *(volatile f32 *)(to + 5);
    out[3] = from[2] * to[3] + second;
    float third = *(volatile f32 *)(from + 5) * *(volatile f32 *)(to + 4);
    out[4] = from[4] * to[2] + third;
    float fourth = *(volatile f32 *)(from + 5) * *(volatile f32 *)(to + 5);
    out[5] = from[4] * to[3] + fourth;
    out[6] = from[6] + to[6];
    out[7] = from[7] + to[7];
}
#pragma scheduling reset

/* Turn a record payload into a matrix and place it: translate, rotate about the
   fixed axis by the payload angle in degrees, translate again. */
void fn_804535C8(float *out, const float *key)
{
    u32 axis[3];
    axis[0] = *(u32 *)&lbl_8051BD28[0];
    axis[1] = *(u32 *)&lbl_8051BD28[1];
    axis[2] = *(u32 *)&lbl_8051BD28[2];
    float zero = lbl_805FB0E0;
    out[0] = key[3];
    out[1] = key[5];
    out[2] = zero;
    out[4] = zero;
    out[5] = key[4];
    out[6] = zero;
    out[8] = zero;
    out[9] = zero;
    out[10] = zero;
    out[12] = key[6];
    out[13] = key[7];
    out[14] = zero;
    fn_8047EBA0(out);
    fn_8047F59C(out, lbl_8051BD40, 2);
    fn_8047EFC8(out, (const float *)axis, lbl_805FB0E4 * (key[2] / lbl_805FB0E8), 2);
    /* Retail leaves r3 as the last call set it rather than returning out. */
    fn_8047F59C(out, lbl_8051BD4C, 2);
}

/* Blend at time, taking the shortest way round for the angle in word 2. */
#pragma scheduling off
void fn_804536B4(float *out, const float *from, const float *to, float time)
{
    float numerator = time - from[1];
    float denominator = to[1] - from[1];
    float delta = to[2] - from[2];
    float ratio = numerator / denominator;
    if (delta < lbl_805FB0EC) {
        delta += lbl_805FB0F0;
    } else if (delta > lbl_805FB0E8) {
        delta -= lbl_805FB0F0;
    }
    out[2] = ratio * delta + from[2];
    for (u32 i = 3; i < 8; i++) {
        out[i] = ratio * (to[i] - from[i]) + from[i];
    }
}
#pragma scheduling reset

/* The wrapped blend for a caller that has already worked out the ratio. */
#pragma scheduling off
void fn_80453770(float *out, const float *from, const float *to, float ratio)
{
    float delta = to[2] - from[2];
    if (delta < lbl_805FB0EC) {
        delta += lbl_805FB0F0;
    } else if (delta > lbl_805FB0E8) {
        delta -= lbl_805FB0F0;
    }
    out[2] = ratio * delta + from[2];
    for (u32 i = 3; i < 8; i++) {
        out[i] = ratio * (to[i] - from[i]) + from[i];
    }
}
#pragma scheduling reset

/* Recover the angle of a record payload: retail copies the whole record into the
   frame, rotates that copy, and reads the angle and the transformed position. */
float *fn_80453820(float *out, const float *key)
{
    float work[16];
    for (u32 i = 0; i < 16; i++) {
        work[i] = key[i];
    }
    float axis[3];
    axis[0] = lbl_8051BD34[0];
    axis[1] = lbl_8051BD34[1];
    axis[2] = lbl_8051BD34[2];
    out[0] = (float)fn_803B1C3C(-(double)work[4], (double)work[0]);
    fn_8047F59C(work, lbl_8051BD40, 2);
    fn_8047EFC8(work, axis, lbl_805FB0E4 * (-out[0] / lbl_805FB0E8), 2);
    fn_8047F59C(work, lbl_8051BD4C, 2);
    out[1] = work[0];
    out[3] = work[1];
    out[2] = work[5];
    out[4] = work[12];
    out[5] = work[13];
    return out;
}

/* Chunk 0x135 constructor: build the class record, then the static sub-objects. */
u32 *fn_80453980(u32 *object)
{
    void *created = fn_8047FC04(0x44, (void *)lbl_805EDD90, 4, (void *)lbl_805EDD94,
                                (void *)lbl_805F1498, 0x40135);
    lbl_805F1498 = (u32)created;
    if (created == 0) {
        object = 0;
    }
    fn_80479040(lbl_8056D748);
    fn_80477344(lbl_8056D748 + 0x34);
    fn_80477344(lbl_8056D748 + 0x64);
    (&lbl_805EDD88)[1]++;
    return object;
}

/* Chunk 0x135 destructor. */
u32 *fn_80453A14(u32 *object)
{
    (&lbl_805EDD88)[1]--;
    fn_8047909C((u32 *)lbl_8056D748);
    if (lbl_805F1498 != 0) {
        fn_8047FCDC((void *)lbl_805F1498);
        lbl_805F1498 = 0;
    }
    return object;
}

/* Install the chunk 0x135 plugin: an 0x28 byte payload with four callbacks, plus
   the stream trio whose read and size halves live in the next unit. */
u32 fn_80453A78(void)
{
    if (fn_80485D78(0, 0x135, fn_80453980, fn_80453A14) < 0) {
        return 0;
    }
    lbl_805F149C = fn_8045B2CC(0x28, 0x135, fn_80453B24, fn_80453B64, fn_80453C04);
    s32 streamResult = fn_8045B30C(0x135, fn_80453FB0, fn_80453E24, fn_80454294);
    s32 result = 0;
    if (streamResult >= 0) {
        if ((s32)lbl_805F149C >= 0) {
            result = 1;
        }
    }
    return result;
}

/* Chunk 0x135 destructor callback: the two frames, then release the nodes. Retail
   never clears the eight node slots here. */
u32 *fn_80453B64(u32 *object)
{
    u32 *data = (u32 *)((u8 *)object + lbl_805F149C);
    for (u32 i = 0; i < 2; i++) {
        if (data[i] != 0) {
            fn_8047F958((void *)data[i]);
        }
        data[i] = 0;
    }
    u32 node;
    for (u32 j = 0; j < 8; j++) {
        node = data[2 + j];
        if (node != 0) {
            fn_8045459C((u32 *)*(u32 *)node);
            fn_80477A8C(node);
        }
    }
    return object;
}

/* Chunk 0x135 copy: every node of the source gets a fresh list holding the same
   element count; on failure the copies made so far are released again. */
u32 *fn_80453C04(u32 *dest, u32 *source)
{
    u32 *destData = (u32 *)((u8 *)dest + lbl_805F149C);
    u32 *sourceData = (u32 *)((u8 *)source + lbl_805F149C);
    u32 slot = 0;
    for (; slot < 8; slot++) {
        u32 node = sourceData[2 + slot];
        if (node == 0) {
            destData[2 + slot] = 0;
            continue;
        }
        u32 list = *(u32 *)node;
        u32 copy = fn_804779DC(fn_804779A8((u32 *)list), *(u32 *)(*(u32 *)list + 4));
        destData[2 + slot] = copy;
        if (copy == 0) {
            break;
        }
        fn_80477BF0((u32 *)copy, (u32 *)node);
        /* Not reached when the copy above failed, which is what retail does. */
        fn_80454618((u32 *)list);
    }
    if (slot == 8) {
        return dest;
    }
    for (u32 i = 0; i < slot; i++) {
        u32 copy = destData[2 + i];
        if (copy != 0) {
            fn_8045459C((u32 *)*(u32 *)copy);
            fn_80477A8C(copy);
        }
    }
    return 0;
}

/* Create or drop the frame for each of the two slots by looking for a node whose
   element list names that slot. The argument is the payload block itself. */
u32 *fn_80453D08(u32 *object)
{
    for (u32 slot = 0; slot < 2; slot++) {
        u32 found = 0;
        s32 element = 0;
        s32 index = 0;
        while (index < 8) {
            u32 node = object[2 + index];
            if (node == 0) {
                index++;
                continue;
            }
            u32 list = *(u32 *)node;
            u32 *elements = *(u32 **)((u8 *)list + 0x14);
            if (*(u32 *)((u8 *)elements + 0x20 + 4 * element) == slot) {
                found = 1;
                break;
            }
            element++;
            if (element < (s32)*(u32 *)((u8 *)node + 0x2C)) {
                continue;
            }
            element = 0;
            index++;
        }
        if (found != 0) {
            if (object[slot] == 0) {
                object[slot] = (u32)fn_8047F994();
                if (object[slot] == 0) {
                    return 0;
                }
            }
        } else if (object[slot] != 0) {
            fn_8047F958((void *)object[slot]);
            object[slot] = 0;
        }
    }
    return object;
}

/* Chunk 0x135 stream write: a presence mask for the eight node slots, then every
   node's 0x20 byte chunk. The length handed to the header call is not used. */
void *fn_80453E24(void *stream, u32 length, u8 *object)
{
    u32 *slots = (u32 *)(object + lbl_805F149C + 0x08);
    u32 mask = 0;
    u32 size = 4;
    for (u32 pass = 0; pass < 2; pass++) {
        for (u32 bit = pass * 4; bit < pass * 4 + 4; bit++) {
            if (slots[bit] != 0) {
                mask |= 1u << bit;
                size += 0x20;
            }
        }
    }
    if (fn_8047CEF0(stream, 1, size, 0x37002, 0x37) == 0) {
        return 0;
    }
    if (fn_8047D458(stream, &mask, 4) == 0) {
        return 0;
    }
    for (u32 i = 0; i < 8; i++) {
        if (slots[i] != 0) {
            /* Retail never checks the result of the node write. */
            fn_80480ED0(stream, (void *)fn_804548E4((u32 *)*(u32 *)slots[i]), 0x20);
        }
    }
    return stream;
}
}

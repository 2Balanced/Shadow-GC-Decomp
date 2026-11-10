/* RenderWare small callbacks and accessors. Names are inferred from behavior. */
#include <string.h>
#include <dolphin/types.h>

extern u32 lbl_8056D708[];
extern u8 *lbl_805F15B4;

extern "C" {
u32 fn_8047D868(void *, void *, u32);
u32 fn_80480D4C(void *, void *, u32);
void fn_8047FCDC(void *);
s32 fn_80485DB8(u32);
void fn_80451D60(u32);
extern u32 lbl_805EDD80;
extern u32 lbl_805EDD84;
extern u32 lbl_805E1538;
void *fn_8047FC04(u32, void *, void *, u32, void *, u32);

void *fn_8044BBFC(void *stream, u32 *object) {
    u32 local[5];
    if (fn_8047D868(stream, local + 2, 4) == 0 ||
        fn_8047D868(stream, local + 4, 4) == 0 ||
        fn_8047D868(stream, local + 3, 4) == 0) return NULL;
    u32 count = local[4];
    if ((s32)count <= 0) return stream;
    if (object[0xF] != 0) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)object[0xF]);
    }
    object[0xC] = 0;
    object[0xD] = 0;
    object[0xE] = 0;
    object[0xF] = 0;
    object[0x10] = 0;
    object[0x11] = 0;
    u32 bytes = count * 2 + local[3] * 2 + object[0];
    void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x30116);
    if (memory == NULL) return NULL;
    memset(memory, 0, bytes);
    object[0xC] = local[2];
    object[0xD] = count;
    object[0xE] = local[3];
    object[0xF] = (u32)memory + object[0];
    object[0x10] = object[0xF] + count * 2;
    object[0x11] = object[0x10] + local[3] * 2;
    u32 *extra = object + 0xC;
    u32 extraBytes = (extra[1] << 1) + (extra[2] << 1) + object[0];
    if (fn_80480D4C(stream, extra, extraBytes) == 0) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(extra);
        return NULL;
    }
    return stream;
}

void *fn_8044C174(u32 *object) {
    if (lbl_8056D708[8] == 0) {
        u32 flags = 1;
        if (fn_80485DB8(0x120) != -1) flags |= 2;
        if (fn_80485DB8(0x12E) != -1) flags |= 4;
        fn_80451D60(flags);
        void *result = fn_8047FC04(0x4C, &lbl_805EDD80, &lbl_805EDD84, 4, &lbl_805E1538, 0x400116);
        lbl_8056D708[6] = (u32)result;
        void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(0x400F, 0x400116);
        lbl_8056D708[4] = (u32)memory;
        memset(memory, 0, 0x400F);
        lbl_8056D708[3] = ((u32)memory + 0xF) & ~0xF;
    }
    lbl_8056D708[8]++;
    return object;
}

void *fn_8044BDF0(u32 count, u32 records, u32 deduplicate, u32 first, u32 *groups, u32 *values,
                  u32 *recordsOut) {
    u32 *object = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))(0x4C, 0x30116);
    if (object == NULL) return NULL;
    memset(object, 0, 0x4C);
    if (first == 0) {
        object[4] = 1;
        u32 *group = groups;
        while (object[4] < 4 && group[0] != 0) {
            if (group[object[4]] != 0) object[4]++;
            group += 4;
        }
    }

    u8 unique[256];
    u32 uniqueCount = 0;
    if (deduplicate == 0) {
        for (u32 groupIndex = 0; groupIndex < count; groupIndex++) {
            u32 *group = groups + groupIndex * 4;
            for (u32 channel = 0; channel < object[4]; channel++) {
                if (group[channel] == 0) continue;
                u32 value = values[groupIndex] >> (channel * 8);
                u8 byte = (u8)value;
                bool present = false;
                for (u32 i = 0; i < uniqueCount; i++) {
                    if (unique[i] == byte) {
                        present = true;
                        break;
                    }
                }
                if (!present) unique[uniqueCount++] = byte;
            }
        }
    }

    u32 bytes = records * 0x40 + uniqueCount + 0xF + count * 0x14;
    void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(bytes, 0x30116);
    if (memory == NULL) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x11C))(object);
        return NULL;
    }
    memset(memory, 0, bytes);
    object[0] = records;
    object[1] = uniqueCount;
    object[2] = (u32)memory;
    object[3] = ((u32)memory + uniqueCount + 0xF) & ~0xF;
    object[5] = object[3] + records * 0x40;
    object[6] = object[5] + count * 4;
    if (count != 0 && uniqueCount != 0) memcpy((void *)object[2], unique, uniqueCount);
    if (recordsOut != NULL && records != 0) {
        u8 *source = (u8 *)recordsOut + records * 0x40;
        u8 *dest = (u8 *)object[3] + records * 0x40;
        for (u32 i = 0; i < records; i++) {
            memcpy(dest, source, 0x40);
            source -= 0x40;
            dest -= 0x40;
        }
    }
    if (values != NULL) memcpy((void *)object[5], values, count * 4);
    if (groups != NULL) memcpy((void *)object[6], groups, count * 0x10);
    return object;
}

void fn_80451E0C(void);

void *fn_8044C280(u32 *object) {
    if (--lbl_8056D708[8] == 0) {
        fn_80451E0C();
        fn_8047FCDC((void *)lbl_8056D708[6]);
        lbl_8056D708[6] = 0;
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)lbl_8056D708[4]);
        lbl_8056D708[4] = 0;
    }
    return object;
}

void fn_8044E758(void);
void *fn_8044CD4C(void *);

void *fn_8044C324(u32 *object) {
    u32 value = *(u32 *)((u8 *)object + lbl_8056D708[2]);
    if (value != 0) {
        fn_8044E758();
        void *result = fn_8044CD4C((void *)value);
        *(u32 *)((u8 *)object + lbl_8056D708[2]) = (u32)result;
    }
    return object;
}

void *fn_8044C390(u32 *object) {
    memset((u8 *)object + lbl_8056D708[1], 0, 0xC);
    return object;
}

s32 fn_80458C90(u32);
u32 fn_8044CCAC(u8 *);
void fn_80451E6C(void *, u32);
u32 fn_8044BDBC(u32 *);
u32 fn_8044E7D0(void *);

u32 fn_8044C414(u32 *object) {
    u32 mode = 1;
    if (fn_80485DB8(0x120) != -1 && *((u8 *)object + fn_80458C90(0x120)) != 0) mode = 2;
    if (object[6] != 0 && fn_8044CCAC((u8 *)object[6]) != 0) {
        if (fn_80485DB8(0x120) == -1 && mode == 2) mode = 1;
        else if (fn_80485DB8(0x12E) == -1 && mode == 3) mode = 1;
        fn_80451E6C(object, mode);
    }
    return 1;
}

u32 fn_8044C4D4(u32 *object, u32 unused0, u32 unused1, u32 mode) {
    if (object[6] != 0 && fn_8044CCAC((u8 *)object[6]) != 0) {
        if (fn_80485DB8(0x120) == -1 && mode == 2) mode = 1;
        else if (fn_80485DB8(0x12E) == -1 && mode == 3) mode = 1;
        fn_80451E6C(object, mode);
    }
    return 1;
}

u32 fn_8044C56C(u32 *object) {
    u32 result = 0;
    u32 value = *(u32 *)((u8 *)object + lbl_8056D708[2]);
    if (value != 0) {
        if ((object[2] & 0x01000000) != 0) {
            result = fn_8044E7D0((void *)value);
        } else {
            u32 *record = (u32 *)value;
            u32 count = object[5];
            result = record[1] + 4 + count * 4 + count * 0x10 + record[0] * 0x40;
            result += fn_8044BDBC(record);
        }
    }
    return result;
}

u32 fn_8047D458(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);
u32 fn_8047D350(void *, void *, u32);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8044BB18(void *, void *);
u32 fn_8044E89C(void *, void *);
u32 fn_8044EAD0(void *, void *);
void *fn_8044CCC0(void *, void *);

void *fn_8044C5FC(void *stream, u32 unused, u32 *object) {
    u32 *record = *(u32 **)((u8 *)object + lbl_8056D708[2]);
    if (record == NULL) return stream;
    if ((object[2] & 0x01000000) != 0) {
        return fn_8044E89C(stream, object) != 0 ? stream : NULL;
    }

    u32 header = ((record[1] & 0xFF) << 8) | ((record[4] & 0xFF) << 16) | (record[0] & 0xFF);
    u32 count = object[5];
    if (fn_8047D458(stream, &header, 4) == 0) return NULL;
    if (fn_80480ED0(stream, (void *)record[2], record[1]) == 0) return NULL;
    if (fn_8047D458(stream, (void *)record[5], count * 4) == 0) return NULL;
    if (fn_8047D350(stream, (void *)record[6], count * 0x10) == 0) return NULL;
    if (fn_8047D350(stream, (void *)record[3], record[0] * 0x40) == 0) return NULL;
    if (fn_8044BB18(stream, record) == 0) return NULL;
    return stream;
}

void *fn_8044C754(void *stream, u32 unused, u32 *object) {
    if ((object[2] & 0x01000000) != 0) {
        return fn_8044EAD0(stream, object) != 0 ? stream : NULL;
    }

    u32 header;
    if (fn_8047D868(stream, &header, 4) == 0) return NULL;
    u32 count = object[5];
    u32 records = header & 0xFF;
    u32 unique = (header >> 16) & 0xFF;
    u32 first = (header >> 8) & 0xFF;
    u32 *record = (u32 *)fn_8044BDF0(count, records, unique, first, NULL, NULL, NULL);
    if (record == NULL) return NULL;
    if (fn_80480D4C(stream, (void *)record[2], unique) != unique) return NULL;
    if (fn_8047D868(stream, (void *)record[5], count * 4) == 0) return NULL;
    if (fn_8047D784(stream, (void *)record[6], count * 0x10) == 0) return NULL;
    record[4] = first;
    if (fn_8047D784(stream, (void *)record[3], record[0] * 0x40) == 0) return NULL;
    if (fn_8044BBFC(stream, record) == NULL) return NULL;
    fn_8044CCC0(object, record);
    return stream;
}

u32 fn_804810FC(void *, u32);
u32 fn_8044E6BC(void *);

void *fn_8044C8C0(void *stream, u32 unused, u32 *object, u32 unused2, u32 size) {
    u32 *owner = (u32 *)object[6];
    if (fn_8044CCAC((u8 *)owner) != 0) {
        return fn_804810FC(stream, size) != 0 ? stream : NULL;
    }

    u32 header;
    if (fn_8047D868(stream, &header, 4) == 0) return NULL;
    u32 count = owner[5];
    u32 *record = (u32 *)fn_8044BDF0(count, header, header, 4, NULL, NULL, NULL);
    if (record == NULL) return NULL;
    if (fn_804810FC(stream, 4) == 0) return NULL;
    if (fn_8047D868(stream, (void *)record[5], count * 4) == 0) return NULL;
    if (fn_8047D784(stream, (void *)record[6], count * 0x10) == 0) return NULL;

    u32 offset = 0;
    for (u32 i = 0; i < record[0]; i++) {
        if (fn_804810FC(stream, 0xC) == 0) return NULL;
        if (fn_8047D784(stream, (void *)(record[3] + offset), 0x40) == 0) return NULL;
        offset += 0x40;
    }

    record[4] = 1;
    for (u32 i = 0; i < count && record[4] < 4; i++) {
        u32 *group = (u32 *)record[6] + i * 4;
        while (record[4] < 4 && group[record[4]] != 0) record[4]++;
    }

    u32 *groups = (u32 *)record[6];
    u32 *values = (u32 *)record[5];
    u8 *unique = (u8 *)record[2];
    record[1] = 0;
    for (u32 i = 0; i < count; i++) {
        for (u32 channel = 0; channel < record[4]; channel++) {
            if (groups[i * 4 + channel] == 0) continue;
            u8 value = (u8)(values[i] >> (channel * 8));
            bool present = false;
            for (u32 j = 0; j < record[1]; j++) {
                if (unique[j] == value) {
                    present = true;
                    break;
                }
            }
            if (!present) unique[record[1]++] = value;
        }
    }
    fn_8044CCC0(owner, record);
    return stream;
}

void *fn_8044CCC0(void *object, void *record) {
    u32 old = *(u32 *)((u8 *)object + lbl_8056D708[2]);
    if (old != (u32)record) {
        if (old != 0) fn_8044E758();
        *(u32 *)((u8 *)object + lbl_8056D708[2]) = (u32)record;
        if (record != NULL && fn_8044E6BC(object) == 0) return NULL;
    }
    return object;
}

typedef void (*RwCpp12Callback)(void);
void fn_8044CB6C(void);
void fn_8044CB70(void);
void fn_8044C3D8(u8 *);
void fn_8044C3FC(u8 *, u8 *);
void fn_8044C30C(u8 *);
void fn_8044C38C(void);
s32 fn_80485D78(u32, u32, RwCpp12Callback, RwCpp12Callback);
s32 fn_80458B6C(u32, u32, RwCpp12Callback, RwCpp12Callback, RwCpp12Callback);
s32 fn_80458BEC(u32, RwCpp12Callback, RwCpp12Callback, RwCpp12Callback);
s32 fn_80458C28(u32, RwCpp12Callback);
s32 fn_80458C5C(u32, RwCpp12Callback);
s32 fn_8045994C(u32, u32, RwCpp12Callback, RwCpp12Callback, RwCpp12Callback);
s32 fn_8045998C(u32, RwCpp12Callback, RwCpp12Callback, RwCpp12Callback);

u32 fn_8044CB90(void) {
    lbl_8056D708[0] = fn_80485D78(0, 0x116, (RwCpp12Callback)fn_8044C174,
                                  (RwCpp12Callback)fn_8044C280);
    lbl_8056D708[1] = fn_80458B6C(0xC, 0x116, (RwCpp12Callback)fn_8044C390,
                                  (RwCpp12Callback)fn_8044C3D8, (RwCpp12Callback)fn_8044C3FC);
    fn_80458BEC(0x116, (RwCpp12Callback)fn_8044C8C0, (RwCpp12Callback)fn_8044CB6C,
                (RwCpp12Callback)fn_8044CB70);
    fn_80458C28(0x116, (RwCpp12Callback)fn_8044C414);
    fn_80458C5C(0x116, (RwCpp12Callback)fn_8044C4D4);
    fn_8045994C(4, 0x116, (RwCpp12Callback)fn_8044C30C, (RwCpp12Callback)fn_8044C324,
                (RwCpp12Callback)fn_8044C38C);
    lbl_8056D708[2] = fn_8045998C(0x116, (RwCpp12Callback)fn_8044C754,
                                  (RwCpp12Callback)fn_8044C5FC, (RwCpp12Callback)fn_8044C56C);
    return 1;
}

void fn_8044C38C(void) {
}

void fn_8044CB6C(void) {
}

u32 fn_8044CDBC(u32 *object) {
    return object[5];
}

void fn_8044CC98(u8 *base, u32 value) {
    *(u32 *)(base + lbl_8056D708[1]) = value;
}

u32 fn_8044CCAC(u8 *object) {
    return *(u32 *)(object + lbl_8056D708[2]);
}

void fn_8044C30C(u8 *base) {
    *(u32 *)(base + lbl_8056D708[2]) = 0;
}

void fn_8044C3FC(u8 *base, u8 *other) {
    u32 offset = lbl_8056D708[1];
    *(u32 *)(base + offset) = *(u32 *)(other + offset);
}

void fn_8044EE3C(void);

void fn_8044CB70(void) {
    fn_8044EE3C();
}

void fn_8044C3D8(u8 *base) {
    u32 offset = lbl_8056D708[1];
    if (*(u32 *)(base + offset) != 0) {
        *(u32 *)(base + offset) = 0;
    }
}

u32 fn_8044BDBC(u32 *object) {
    u32 result = 0x0C;
    if (object[0x0D] != 0) {
        result = object[0] + (object[0x0D] << 1) + (object[0x0E] << 1) + 0x0C;
    }
    return result;
}
}

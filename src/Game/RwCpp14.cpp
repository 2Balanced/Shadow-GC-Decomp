/* RenderWare debug-table construction. Names are inferred from the call sequence; the public
 * entry points retain their address names. */
#include <dolphin/types.h>
#include <string.h>

extern u32 lbl_805E1560[];
extern u32 lbl_805F1520;
extern u8 *lbl_805F15B4;

struct RwCpp14Descriptor {
    u32 data;
    u32 type;
    s8 links[16];
};

extern "C" {
u32 fn_8044CCAC(u8 *);
void *fn_80462E1C(void *);
void fn_8046FC1C(u32 *);
void fn_8046FC48(u32 *, u32);
void fn_8046FC50(u32 *, u32, u32, u32, u32);
void fn_8046FF34(u32 *, u32, u32);
void fn_8047046C(u32 *, u32);
u32 *fn_80466194(u32 *, u32, u32);
u32 *fn_80465ED4(u32 *, u8 *, u32, u32);
u32 fn_80465CDC(u32 *, u32 *, u32, u32);
}

extern "C" void fn_8044F4D0(u32 *object, u32 *table) {
    u32 *record = (u32 *)fn_8044CCAC((u8 *)object);
    u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
    if (format == NULL) format = (u8 *)fn_80462E1C(record);

    fn_8046FC1C(lbl_805E1560);
    fn_8046FC48(lbl_805E1560, 0);
    if (record[4] == 1) fn_8046FF34(lbl_805E1560, 0, 1);
    fn_8046FC50(lbl_805E1560, 9, 1, format[0], format[0xC]);

    u32 value = table[1];
    fn_8046FF34(lbl_805E1560, 9, value < 0xFF ? 2 : 3);
    u32 index = 1;
    if ((object[2] & 0x10) != 0) {
        if (format[0xD] != 0) {
            fn_8046FC50(lbl_805E1560, 0x19, 1, format[1], 0);
            value = table[index * 2 + 1];
            fn_8046FF34(lbl_805E1560, 0x19, value < 0xFF ? 2 : 3);
        } else {
            fn_8046FC50(lbl_805E1560, 0xA, 0, format[1], 0);
            value = table[index * 2 + 1];
            fn_8046FF34(lbl_805E1560, 0xA, value < 0xFF ? 2 : 3);
        }
        index = 2;
    }

    if ((object[2] & 0x8) != 0) {
        u32 byte = format[0xA];
        fn_8046FC50(lbl_805E1560, 0xB, (2 - byte) >> 31, byte, 0);
        value = table[index * 2 + 1];
        fn_8046FF34(lbl_805E1560, 0xB, value < 0xFF ? 2 : 3);
        index++;
    }

    if ((object[2] & 0x84) != 0) {
        u32 *entry = table + index * 2;
        for (u32 i = 0; i < (u32)object[7]; i++) {
            fn_8046FC50(lbl_805E1560, i + 0xD, 1, format[i + 2], format[i + 0xE]);
            value = entry[1];
            fn_8046FF34(lbl_805E1560, i + 0xD, value < 0xFF ? 2 : 3);
            entry += 2;
            index++;
        }
    }
    fn_8047046C(lbl_805E1560, index & 0xFF);
}

extern "C" void fn_8044F740(u32 *object, u16 *tableA, u32 *tableB, u32 count, u32 index) {
    u16 *output = (u16 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count * 2, 0x30116);
    if (object[13] != 0) {
        u8 local[0x28];
        u32 written = 0;
        u8 *table = (u8 *)object[16];
        u32 pair = index * 2;
        u32 first = table[pair];
        u32 outerCount = table[pair + 1];
        u32 tableOffset = (first & 0x7F) * 2;
        for (u32 outer = 0; outer < outerCount; outer++) {
            u8 *entry = (u8 *)object[17] + outer * 2 + tableOffset;
            u32 base = entry[0];
            u32 innerCount = entry[1];
            for (u32 i = 0; i < innerCount; i++) local[written++] = (u8)(base + i);
        }

        for (u32 i = 0; i < count; i++) {
            u32 expected = tableB[tableA[i]] & 0xFF;
            u32 candidate = 0;
            bool found = false;
            for (u32 round = 0; round < 2 && !found; round++) {
                for (u32 j = 0; j < 5; j++) {
                    if (local[round * 5 + j] == expected) {
                        output[i] = (u16)(candidate * 3);
                        found = true;
                        break;
                    }
                    candidate++;
                }
            }
        }
    } else {
        for (u32 i = 0; i < count; i++) {
            u32 expected = tableB[tableA[i]] & 0xFF;
            u8 *candidates = (u8 *)object[2];
            for (u32 j = 0; j < object[1]; j++) {
                if (candidates[j] == expected) {
                    output[i] = (u16)(j * 3);
                    break;
                }
            }
        }
    }
}

extern "C" void fn_80450448(u32 *object) {
    u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
    if (format == NULL) format = (u8 *)fn_80462E1C(object);
    u32 value = object[5];

    fn_8046FC1C(lbl_805E1560);
    fn_8046FC48(lbl_805E1560, 0);
    fn_8046FF34(lbl_805E1560, 0, 1);
    fn_8046FC50(lbl_805E1560, 9, 1, format[0], format[0xC]);
    fn_8046FF34(lbl_805E1560, 9, value < 0xFF ? 2 : 3);

    u32 index = 1;
    if ((object[2] & 0x10) != 0) {
        if (format[0xD] != 0) {
            fn_8046FC50(lbl_805E1560, 0x19, 1, format[1], 0);
            fn_8046FF34(lbl_805E1560, 0x19, value < 0xFF ? 2 : 3);
        } else {
            fn_8046FC50(lbl_805E1560, 0xA, 0, format[1], 0);
            fn_8046FF34(lbl_805E1560, 0xA, value < 0xFF ? 2 : 3);
        }
        index = 2;
    }
    if ((object[2] & 0x8) != 0) {
        u32 byte = format[0xA];
        fn_8046FC50(lbl_805E1560, 0xB, (2 - byte) >> 31, byte, 0);
        fn_8046FF34(lbl_805E1560, 0xB, value < 0xFF ? 2 : 3);
        index++;
    }
    if ((object[2] & 0x84) != 0) {
        for (u32 i = 0; i < (u32)object[7]; i++) {
            fn_8046FC50(lbl_805E1560, i + 0xD, 1, format[i + 2], format[i + 0xE]);
            fn_8046FF34(lbl_805E1560, i + 0xD, value < 0xFF ? 2 : 3);
            index++;
        }
    }
    fn_8047046C(lbl_805E1560, index & 0xFF);
}

extern "C" void fn_8045066C(u32 *object, u32 *descriptor, u32 index) {
    u32 count = descriptor[1];
    u16 *output = (u16 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count * 2, 0x30116);
    u16 *entries = (u16 *)descriptor[0];
    u32 *table = (u32 *)object[5];
    if (object[13] != 0) {
        u8 local[0x28];
        u32 written = 0;
        u8 *source = (u8 *)object[16];
        u32 pair = index * 2;
        u32 first = source[pair];
        u32 outerCount = source[pair + 1];
        u32 tableOffset = (first & 0x7F) * 2;
        for (u32 outer = 0; outer < outerCount; outer++) {
            u8 *entry = (u8 *)object[17] + outer * 2 + tableOffset;
            u32 base = entry[0];
            u32 innerCount = entry[1];
            for (u32 i = 0; i < innerCount; i++) local[written++] = (u8)(base + i);
        }

        for (u32 i = 0; i < count; i++) {
            u32 expected = table[entries[i]] & 0xFF;
            u32 candidate = 0;
            bool found = false;
            for (u32 round = 0; round < 2 && !found; round++) {
                for (u32 j = 0; j < 5; j++) {
                    if (local[round * 5 + j] == expected) {
                        output[i] = (u16)(candidate * 3);
                        found = true;
                        break;
                    }
                    candidate++;
                }
            }
        }
    } else {
        u8 *candidates = (u8 *)object[2];
        for (u32 i = 0; i < count; i++) {
            u32 expected = table[entries[i]] & 0xFF;
            for (u32 j = 0; j < object[1]; j++) {
                if (candidates[j] == expected) {
                    output[i] = (u16)(j * 3);
                    break;
                }
            }
        }
    }
}

extern "C" u32 fn_8044EEAC(u32 *object, u32 *outObject, u32 *outValues) {
    if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) fn_80462E1C(object);

    RwCpp14Descriptor entries[16];
    memset(entries, 0xFF, sizeof(entries));
    u32 *context = (u32 *)object[0x5C];
    u32 streams = 1;
    entries[0].data = context[5];
    entries[0].type = 8;
    if ((object[2] & 0x10) != 0) {
        entries[1].data = context[6];
        entries[1].type = 8;
        streams = 2;
    }
    if ((object[2] & 0x8) != 0) {
        entries[streams].data = object[0x30];
        entries[streams].type = 4;
        streams++;
    }
    if ((object[2] & 0x84) != 0) {
        for (u32 i = 0; i < (u32)object[7]; i++) {
            entries[streams + i].data = object[0x34 + i];
            entries[streams + i].type = 7;
        }
        streams += object[7];
    }

    u32 *record = (u32 *)fn_8044CCAC((u8 *)object);
    u32 base = streams;
    if (record[4] > 1) {
        entries[base].data = record[10];
        switch (record[4]) {
        case 1: entries[base].type = 1; break;
        case 2: entries[base].type = 2; break;
        case 3: entries[base].type = 3; break;
        case 4: entries[base].type = 4; break;
        default: entries[base].type = 0; break;
        }
        entries[base + 1].data = record[9];
        entries[base + 1].type = entries[base].type;
    } else {
        entries[base].data = record[5];
        entries[base].type = 4;
        entries[base + 1].data = record[6];
        entries[base + 1].type = 9;
    }
    streams += 2;

    if ((object[2] & 0x10) != 0) {
        entries[streams - 2].links[0] = 0;
        entries[streams - 2].links[1] = 1;
        entries[streams - 2].links[2] = (s8)(streams - 1);
        entries[streams - 2].links[3] = -1;
        entries[streams - 1].links[0] = 0;
        entries[streams - 1].links[1] = 1;
        entries[streams - 1].links[2] = (s8)(streams - 2);
        entries[streams - 1].links[3] = -1;
    } else {
        entries[streams - 2].links[0] = 0;
        entries[streams - 2].links[1] = (s8)(streams - 1);
        entries[streams - 2].links[2] = -1;
        entries[streams - 1].links[0] = 0;
        entries[streams - 1].links[1] = (s8)(streams - 2);
        entries[streams - 1].links[2] = -1;
    }

    u32 *indices = fn_80466194((u32 *)entries, streams, object[5]);
    if (indices == NULL) return 0;
    u32 *packed = fn_80465ED4(indices, (u8 *)entries, streams, object[5]);
    *outObject = (u32)packed;
    if (packed == NULL) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(indices);
        return 0;
    }

    u32 vertices = *(u16 *)((u8 *)object[0x54] + 4);
    u32 customStreams = streams - 2;
    u32 rowBytes = vertices * 4;
    u32 tableBytes = customStreams * rowBytes;
    u32 *table = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(tableBytes, 0x30116);
    if (table == NULL) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(packed);
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(indices);
        return 0;
    }
    memset(table, 0, tableBytes);
    for (u32 vertex = 0; vertex < vertices; vertex++) {
        u32 *source = (u32 *)((u8 *)object[0x54] + vertex * 0xC);
        for (u32 stream = 0; stream < customStreams; stream++) {
            table[vertex * customStreams + stream] = source[4];
        }
    }

    u32 *result = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(rowBytes, 0x30116);
    *outValues = (u32)result;
    if (result == NULL) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(packed);
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(indices);
        return 0;
    }

    for (u32 vertex = 0; vertex < vertices; vertex++) {
        u32 *source = (u32 *)((u8 *)object[0x54] + vertex * 0xC);
        u32 value = fn_80465CDC(indices, table + vertex * customStreams, customStreams, source[5]);
        result[vertex] = value;
        if (value == 0) {
            for (s32 i = (s32)vertex; i >= 0; i--) {
                ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)result[i]);
            }
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(packed);
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(indices);
            return 0;
        }
    }
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(table);
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(indices);
    return 1;
}

/* RenderWare debug-table construction. Names are inferred from the call sequence; the public
 * entry points retain their address names. */
#include <dolphin/types.h>
#include <string.h>

extern u32 lbl_805E1560[];
extern u32 lbl_805F1520;
extern u8 *lbl_805F15B4;
extern u16 lbl_805F1678;

struct RwCpp14Descriptor {
    u32 data;
    u32 type;
    s8 links[16];
};

static inline u32 RwCpp14ValueType(u32 value) {
    u32 type = 2;
    if (value >= 0xFF) {
        type = 3;
    }
    return type;
}

extern "C" {
u32 fn_8044CCAC(u8 *);
void *fn_80462E1C(void *);
void fn_8046FC1C(u32 *);
void fn_8046FC48(u32 *, u32);
void fn_8046FC50(u32 *, u32, u32, u32, u32);
void fn_8046FF34(u32 *, u32, u32);
u32 fn_8047046C(u32 *, u32);
u32 *fn_80466194(u32 *, u32, u32);
u32 *fn_80465ED4(u32 *, u8 *, u32, u32);
u32 fn_80465CDC(u32 *, u32 *, u32, u32);
u32 fn_8046EE8C(u32);
u32 fn_8046EEA0(u32, u32 *);
u32 fn_8046B094(u32, u32, u32);
void fn_8046D8C8(u32, u32, u32 *, u32 *, u32);
void fn_8046B0E0(u8 *, u32, u32, u8 *);
void *fn_8046ACBC(u32);
void fn_8046E594(u32, u8 *, u32 *, u32, u32, void *, u32, u32, u32);
void fn_8046F208(u32, u8 *, u32 *, u32);
void fn_8046F6CC(u32, u8 *, u32 *, u32, u32);
void fn_8046288C(void);
void DCFlushRange(void *, u32);
void fn_80394F2C(void);
}

extern "C" u32 fn_8044F4D0(u32 *object, u32 *table) {
    u32 *record = (u32 *)fn_8044CCAC((u8 *)object);
    u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
    if (format == NULL) format = (u8 *)fn_80462E1C(record);

    fn_8046FC1C(lbl_805E1560);
    fn_8046FC48(lbl_805E1560, 0);
    if (record[4] == 1) fn_8046FF34(lbl_805E1560, 0, 1);
    fn_8046FC50(lbl_805E1560, 9, 1, format[0], format[0xC]);

    u32 value = table[1];
    fn_8046FF34(lbl_805E1560, 9, RwCpp14ValueType(value));
    u32 index = 1;
    if ((object[2] & 0x10) != 0) {
        if (format[0xD] != 0) {
            fn_8046FC50(lbl_805E1560, 0x19, 1, format[1], 0);
            value = table[index * 2 + 1];
            fn_8046FF34(lbl_805E1560, 0x19, RwCpp14ValueType(value));
        } else {
            fn_8046FC50(lbl_805E1560, 0xA, 0, format[1], 0);
            value = table[index * 2 + 1];
            fn_8046FF34(lbl_805E1560, 0xA, RwCpp14ValueType(value));
        }
        index = 2;
    }

    if ((object[2] & 0x8) != 0) {
        u32 byte = format[0xA];
        fn_8046FC50(lbl_805E1560, 0xB, (2 - byte) >> 31, byte, 0);
        value = table[index * 2 + 1];
        fn_8046FF34(lbl_805E1560, 0xB, RwCpp14ValueType(value));
        index++;
    }

    if ((object[2] & 0x84) != 0) {
        u32 *entry = table + index * 2;
        for (u32 i = 0; (s32)i < (s32)object[7]; i++) {
            fn_8046FC50(lbl_805E1560, i + 0xD, 1, format[i + 2], format[i + 0xE]);
            value = entry[1];
            fn_8046FF34(lbl_805E1560, i + 0xD, RwCpp14ValueType(value));
            entry += 2;
            index++;
        }
    }
    fn_8047046C(lbl_805E1560, index & 0xFF);
    return (u32)lbl_805E1560;
}

extern "C" u32 fn_8044F740(u32 *object, u16 *tableA, u32 *tableB, s32 count, u32 index) {
    u16 *output;
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
            u8 *cursor = local + written;
            for (u32 i = 0; i < innerCount; i++) {
                *cursor++ = (u8)(base + i);
            }
            written += innerCount;
        }

        output = (u16 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count * 2, 0x30116);
        for (s32 i = 0; i < count; i++) {
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
        output = (u16 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count * 2, 0x30116);
        for (s32 i = 0; i < count; i++) {
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
    return (u32)output;
}

extern "C" u32 fn_80450448(u32 *object) {
    u32 value = object[5];
    u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
    if (format == NULL) format = (u8 *)fn_80462E1C(object);

    fn_8046FC1C(lbl_805E1560);
    fn_8046FC48(lbl_805E1560, 0);
    fn_8046FF34(lbl_805E1560, 0, 1);
    fn_8046FC50(lbl_805E1560, 9, 1, format[0], format[0xC]);
    fn_8046FF34(lbl_805E1560, 9, RwCpp14ValueType(value));

    u32 index = 1;
    if ((object[2] & 0x10) != 0) {
        if (format[0xD] != 0) {
            fn_8046FC50(lbl_805E1560, 0x19, 1, format[1], 0);
            fn_8046FF34(lbl_805E1560, 0x19, RwCpp14ValueType(value));
        } else {
            fn_8046FC50(lbl_805E1560, 0xA, 0, format[1], 0);
            fn_8046FF34(lbl_805E1560, 0xA, RwCpp14ValueType(value));
        }
        index = 2;
    }
    if ((object[2] & 0x8) != 0) {
        u32 byte = format[0xA];
        fn_8046FC50(lbl_805E1560, 0xB, (2 - byte) >> 31, byte, 0);
        fn_8046FF34(lbl_805E1560, 0xB, RwCpp14ValueType(value));
        index++;
    }
    if ((object[2] & 0x84) != 0) {
        for (u32 i = 0; (s32)i < (s32)object[7]; i++) {
            fn_8046FC50(lbl_805E1560, i + 0xD, 1, format[i + 2], format[i + 0xE]);
            fn_8046FF34(lbl_805E1560, i + 0xD, RwCpp14ValueType(value));
            index++;
        }
    }
    fn_8047046C(lbl_805E1560, index & 0xFF);
    return (u32)lbl_805E1560;
}

extern "C" u32 fn_8045066C(u32 *object, u32 *descriptor, u32 index) {
    u32 count = descriptor[1];
    u16 *output;
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
            u8 *cursor = local + written;
            for (u32 i = 0; i < innerCount; i++) {
                *cursor++ = (u8)(base + i);
            }
            written += innerCount;
        }

        output = (u16 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count * 2, 0x30116);
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
        output = (u16 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count * 2, 0x30116);
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
    return (u32)output;
}

extern "C" u32 fn_8044EEAC(u32 *object, u32 *outObject, u32 *outValues) {
    if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) fn_80462E1C(object);

    RwCpp14Descriptor entries[16];
    u32 *context = (u32 *)object[0x17];
    u32 streams = 1;
    entries[0].data = context[5];
    entries[0].type = 8;
    entries[0].links[0] = -1;
    if ((object[2] & 0x10) != 0) {
        entries[1].data = context[6];
        entries[1].type = 8;
        entries[1].links[0] = -1;
        streams = 2;
    }
    if ((object[2] & 0x8) != 0) {
        entries[streams].data = object[0x0C];
        entries[streams].type = 4;
        entries[streams].links[0] = -1;
        streams++;
    }
    if ((object[2] & 0x84) != 0) {
        for (u32 i = 0; (s32)i < (s32)object[7]; i++) {
            entries[streams + i].data = object[0x0D + i];
            entries[streams + i].type = 7;
            entries[streams + i].links[0] = -1;
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
        entries[base].links[0] = -1;
        entries[base + 1].links[0] = -1;
    } else {
        entries[base].data = record[5];
        entries[base].type = 4;
        entries[base + 1].data = record[6];
        entries[base + 1].type = 9;
        entries[base].links[0] = -1;
        entries[base + 1].links[0] = -1;
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

    u32 vertices = *(u16 *)((u8 *)object[0x15] + 4);
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
        u32 *source = (u32 *)((u8 *)object[0x15] + vertex * 0xC);
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
        u32 *source = (u32 *)((u8 *)object[0x15] + vertex * 0xC);
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

extern "C" u32 fn_8044F9E4(u32 *object, u32 arg2, u32 *out) {
    u32 packedStorage;
    u32 valuesStorage;
    fn_8044EEAC(object, &packedStorage, &valuesStorage);
    u32 *packed = (u32 *)packedStorage;
    u32 *values = (u32 *)valuesStorage;
    u32 context = fn_8044F4D0(object, packed);

    u32 second[52];
    u32 streams = 1;
    u32 optional = 1;
    second[9] = packed[1];
    second[35] = packed[0];
    if ((object[2] & 0x10) != 0) {
        second[9 + optional] = packed[optional * 2 + 1];
        second[35 + optional] = packed[optional * 2];
        optional++;
        streams++;
    }
    if ((object[2] & 0x8) != 0) {
        second[9 + optional] = packed[optional * 2 + 1];
        second[35 + optional] = packed[optional * 2];
        optional++;
        streams++;
    }
    if ((object[2] & 0x84) != 0) {
        u32 i = 0;
        u32 *packedPair = packed + 8;
        u32 *secondA = second + 13;
        u32 *secondB = second + 39;
        for (; i + 8 <= object[7]; i += 8) {
            for (u32 j = 0; j < 8; j++) {
                secondA[j] = packedPair[j * 2 + 1];
                secondB[j] = packedPair[j * 2];
            }
            secondA += 8;
            secondB += 8;
            packedPair += 16;
            streams += 8;
        }
        for (; i < object[7]; i++) {
            *secondA++ = packedPair[1];
            *secondB++ = packedPair[0];
            packedPair += 2;
            streams++;
        }
    }

    u32 base = fn_8046EE8C(context);
    u32 vertices = *(u16 *)((u8 *)object[0x15] + 4);
    u32 stride = (object[2] & 1) != 0 ? 0x98 : 0x90;
    u32 size = base + vertices * 8 + 0x1F;
    for (u32 i = 0; i < vertices; i++) {
        u32 *descriptor = (u32 *)((u8 *)object[0x15] + i * 0xC);
        if ((object[2] & 1) != 0) {
            u32 tempA;
            u32 tempB;
            fn_8046D8C8(*(u32 *)values[i], descriptor[5], &tempA, &tempB, 1);
            size += fn_8046B094(context, tempA, tempB);
        } else {
            size += fn_8046B094(context, 1, descriptor[5]);
        }
    }
    size += fn_8046EEA0(context, second);

    u32 *block = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size + 0x18, 0x30116);
    block[0] = 0;
    block[1] = 0;
    block[2] = size;
    block[3] = arg2;
    block[4] = (u32)out;
    block[5] = (u32)fn_8046288C;
    *out = (u32)block;
    u8 *payload = (u8 *)(block + 6);
    memset(payload, 0, size);
    *(u16 *)(payload + 0) = lbl_805F1678;
    *(u16 *)(payload + 2) = *(u16 *)((u8 *)object[0x15] + 6);
    *(u32 *)(payload + 4) = 0;

    if ((object[2] & 0x8) != 0) {
        u8 *format = *(u8 **)((u8 *)object + lbl_805F1520);
        if (format == NULL) {
            *(u32 *)(payload + 4) &= ~1;
            u8 *bytes = (u8 *)object[0x0C];
            for (s32 i = 0; i < (s32)object[5]; i++) {
                if (bytes[i * 4 + 3] < 0xFF) {
                    *(u32 *)(payload + 4) |= 1;
                    break;
                }
            }
        } else if (format[0xA] > 2) {
            *(u32 *)(payload + 4) |= 1;
        } else {
            *(u32 *)(payload + 4) &= ~1;
        }
    }

    u8 *data = payload + base;
    u8 *end = (u8 *)(((u32)data + vertices * 8 + 0x1F) & ~0x1F);
    u32 *record = (u32 *)fn_8044CCAC((u8 *)object);
    u32 descriptorOffset = 0;
    u32 valuesIndex = 0;
    u8 *currentData = data;
    for (u32 vertex = 0; vertex < vertices; vertex++) {
        u32 *descriptor = (u32 *)((u8 *)object[0x15] + descriptorOffset);
        u32 tempA;
        u32 tempB;
        if ((object[2] & 1) != 0) {
            fn_8046D8C8(*(u32 *)values[valuesIndex / 4], descriptor[5], &tempA, &tempB, 1);
        } else {
            tempA = descriptor[5];
            tempB = 1;
        }
        u32 part = fn_8046B094(context, tempA, tempB);
        fn_8046B0E0(currentData, vertex, part, end);

        u32 *vertexValues = (u32 *)values[valuesIndex / 4];
        u32 *lookup = NULL;
        if (record[4] <= 1) {
            u32 streamIndex = 1 + ((object[2] & 0x10) != 0 ? 1 : 0) + ((object[2] & 0x8) != 0 ? 1 : 0) +
                              ((object[2] & 0x84) != 0 ? object[7] : 0);
            lookup = (u32 *)fn_8044F740(record, (u16 *)vertexValues[0],
                                        (u32 *)packed[streamIndex], descriptor[5], vertex);
        }
        if (*(u32 *)((u8 *)object + lbl_805F1520) == 0) {
            fn_80462E1C(lookup);
        }
        if (lookup != NULL) {
            second[0] = (u32)lookup;
        }
        second[9] = vertexValues[0];
        if ((object[2] & 0x10) != 0) {
            second[10] = vertexValues[1];
        }
        if ((object[2] & 0x8) != 0) {
            second[11] = vertexValues[2];
        }
        if ((object[2] & 0x84) != 0) {
            u32 i = 0;
            u32 *valuesPair = vertexValues + 4;
            u32 *secondValues = second + 13;
            for (; i + 8 <= (u8)object[7]; i += 8) {
                for (u32 j = 0; j < 8; j++) {
                    secondValues[j] = valuesPair[j];
                }
                secondValues += 8;
                valuesPair += 8;
            }
            for (; i < (u8)object[7]; i++) {
                *secondValues++ = *valuesPair++;
            }
        }
        void *acbc = fn_8046ACBC(context);
        fn_8046E594(context, currentData, second, descriptor[5], object[2] & 1, acbc, 1, stride, 0);
        if (lookup != NULL) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(lookup);
        }
        end += part;
        currentData += 8;
        descriptorOffset += 0xC;
        valuesIndex += 4;
    }

    fn_8046F208(context, payload, second, (u32)end);
    fn_8046F6CC(context, payload, second, 0, 0);
    if (record[4] > 1) {
        u32 streamIndex = 1 + ((object[2] & 0x10) != 0 ? 1 : 0) + ((object[2] & 0x8) != 0 ? 1 : 0) +
                          ((object[2] & 0x84) != 0 ? object[7] : 0);
        memcpy((void *)record[10], (void *)packed[streamIndex], record[4] * packed[streamIndex + 1]);
        memcpy((void *)record[9], (void *)packed[streamIndex + 2], record[4] * packed[streamIndex + 3]);
    }
    object[5] = packed[1];
    DCFlushRange(payload, size);
    fn_80394F2C();
    for (u32 i = 0; i < vertices; i++) {
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))((void *)values[i]);
    }
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(values);
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(packed);
    return (u32)block;
}

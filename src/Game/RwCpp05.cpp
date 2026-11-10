/* RenderWare stream object helpers and chunk readers. Names are inferred from behavior. */
#include <dolphin/types.h>

extern "C" {
extern u8 *lbl_805F15B4;
extern s32 lbl_805F1410;
extern s32 lbl_805F1414;
extern s32 lbl_805F1418;
extern s32 lbl_805F141C;
extern s32 lbl_805F1420;

void fn_80440410(u32);
void *fn_8044043C(u32);
void *fn_80440468(u32, void *);
u32 fn_80485D6C(void);
u32 fn_8047D458(void *, void *, u32);
u32 fn_8047D868(void *, void *, u32);
s32 fn_8047CF50(void *, u32, u32, u32);
void *fn_80440670(void *);
void *fn_80440920(void *, u32, u32, void *, s32 *);
u32 fn_8047D784(void *, void *, u32);
u32 fn_804810FC(void *, u32);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);

struct RwCpp05Node {
    u32 flags;
    u32 bounds[6];
    u16 valueCount;
    u16 recordCount;
    u8 *records;
    u16 *tail;
};

struct RwCpp05Record {
    u8 kind;
    u8 id;
    u16 index;
    u32 field4;
    u8 otherKind;
    u8 field9;
    u16 value;
    u32 fieldC;
};

struct RwCpp05Initial {
    u8 kind;
    u8 id;
    u16 index;
    u32 field4;
};

#define RW05_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))

static inline u32 RwCpp05NodeSize(u32 value, u32 count) {
    u32 size = (u16)count != 0 ? ((u16)count << 4) + 0x37 : 0x28;
    size += (u16)value << 1;
    return size + 1;
}

void *fn_80440920(void *stream, u32 value, u32 count, void *boundsData, s32 *status) {
    *status = 0;

    u32 size = RwCpp05NodeSize(value, count);
    void *allocation = RW05_API(void *(*)(u32, u32), 0x108)(size, 0x3002C);
    if (allocation == NULL) {
        u32 error[2];
        error[0] = 0x11D;
        error[1] = fn_8047DC28(0x80000013, size);
        fn_8047DB84(error);
        return NULL;
    }
    RwCpp05Node *node = (RwCpp05Node *)allocation;

    node->flags = 1;
    node->valueCount = (u16)value;
    node->recordCount = (u16)count;
    const u32 *bounds = (const u32 *)boundsData;
    node->bounds[0] = bounds[0];
    node->bounds[1] = bounds[1];
    node->bounds[2] = bounds[2];
    node->bounds[3] = bounds[3];
    node->bounds[4] = bounds[4];
    node->bounds[5] = bounds[5];

    u8 *next = (u8 *)node + 0x28;
    if ((u16)count != 0) {
        next = (u8 *)(((u32)next + 0xF) & ~0xF);
        node->records = next;
        next += (u16)count << 4;
    } else {
        node->records = NULL;
    }
    node->tail = (u16 *)(((u32)next + 1) & ~1);

    if (node->recordCount != 0) {
        RwCpp05Record *record = (RwCpp05Record *)node->records;
        for (u32 i = 0; i < node->recordCount; i++) {
            u32 first;
            u32 second;
            if (fn_8047D868(stream, &first, 4) == 0 || fn_8047D868(stream, &second, 4) == 0 ||
                fn_8047D784(stream, (u8 *)record + 4, 4) == 0 ||
                fn_8047D784(stream, (u8 *)record + 0xC, 4) == 0) {
                RW05_API(void (*)(void *), 0x10C)(node);
                return NULL;
            }
            record->otherKind = (first >> 8) & 0xFF;
            record->kind = record->otherKind | 1;
            record->value = (u16)second;
            record->index = (u16)(second >> 0x10);
            if (((first >> 0x10) & 0xFF) == 2) {
                record->id = 0xFF;
            } else {
                record->id = 0;
            }
            if ((first & 0xFF) == 2) {
                record->field9 = 0xFF;
            } else {
                record->field9 = 0;
            }
            record++;
        }

        RwCpp05Initial initial;
        initial.id = 0xFF;
        initial.index = 0;
        u32 *links[32];
        s32 depth = 1;
        RwCpp05Record *current = (RwCpp05Record *)&initial;
        while (depth != 0) {
            if (current->id != 0xFF) {
                u32 packed;
                if (fn_8047D868(stream, &packed, 4) == 0) {
                    RW05_API(void (*)(void *), 0x10C)(node);
                    return NULL;
                }
                u32 id = packed >> 0x10;
                if (id > 0xEF) {
                    u32 error[2];
                    *status = 1;
                    RW05_API(void (*)(void *), 0x10C)(node);
                    error[0] = 0x11D;
                    error[1] = fn_8047DC28(1);
                    fn_8047DB84(error);
                    return NULL;
                }
                current->id = (u8)id;
                depth--;
                current->index = (u16)packed;
                current = (RwCpp05Record *)links[depth];
            } else {
                u8 *next = node->records + current->index * 0x10;
                links[depth] = (u32 *)(next + 8);
                depth++;
                current = (RwCpp05Record *)next;
            }
        }
    } else if (fn_804810FC(stream, 4) == 0) {
        RW05_API(void (*)(void *), 0x10C)(node);
        return NULL;
    }

    if ((node->flags & 1) != 0) {
        for (u32 i = 0; i < node->valueCount; i++) {
            u32 packed;
            if (fn_8047D868(stream, &packed, 4) == 0) {
                RW05_API(void (*)(void *), 0x10C)(node);
                return NULL;
            }
            node->tail[i] = (u16)packed;
        }
    }
    return node;
}

void fn_80440D28(void) {
    lbl_805F1410++;
    *(u32 *)(lbl_805F15B4 + lbl_805F1414) = 0;
}

void fn_80440D48(void) {
    lbl_805F1410--;
}

u32 *fn_80440D58(u32 *object, u32 offset) {
    u32 value = *(u32 *)((u8 *)object + offset);
    if (value != 0) {
        fn_80440410(value);
        *(u32 *)((u8 *)object + offset) = 0;
    }
    return object;
}

void fn_80440DA8(u32 *object, u32 offset) {
    *(u32 *)((u8 *)object + offset) = 0;
}

void *fn_80440DB4(const u32 *object, u32 offset) {
    if (*(const u32 *)((const u8 *)object + offset) != 0) {
        return (u8 *)fn_8044043C(*(const u32 *)((const u8 *)object + offset)) + 0x10;
    }
    return NULL;
}

void *fn_80440DEC(void *stream, u32 arg2, u32 *object, u32 offset) {
    u32 value = *(u32 *)((u8 *)object + offset);
    if (value != 0) {
        u32 context = fn_80485D6C();
        if (fn_8047D458(stream, &context, 4) == 0) {
            return NULL;
        }
        if (fn_80440468(*(u32 *)((u8 *)object + offset), stream) == NULL) {
            return NULL;
        }
    }
    return stream;
}

void *fn_80440E80(void *stream, u32 arg2, u32 *object, u32 offset) {
    u32 type;
    u32 value;
    float values[6];
    s32 status;

    if (fn_8047D868(stream, &type, 4) == 0) {
        return NULL;
    }
    if (type < 0x36001) {
        if (fn_8047D868(stream, &value, 4) == 0) {
            return NULL;
        }
        u8 *bytes = (u8 *)object;
        if (bytes[0] == 8) {
            const float *source = *(const float **)(bytes + 0x5C);
            values[3] = source[1] - source[4];
            values[0] = source[1] + source[4];
            values[4] = source[2] - source[4];
            values[1] = source[2] + source[4];
            values[5] = source[3] - source[4];
            values[2] = source[3] + source[4];
        } else {
            ((u32 *)values)[0] = *(u32 *)(bytes + 0x60);
            ((u32 *)values)[1] = *(u32 *)(bytes + 0x64);
            ((u32 *)values)[2] = *(u32 *)(bytes + 0x68);
            ((u32 *)values)[3] = *(u32 *)(bytes + 0x6C);
            ((u32 *)values)[4] = *(u32 *)(bytes + 0x70);
            ((u32 *)values)[5] = *(u32 *)(bytes + 0x74);
        }
        status = 0;
        *(u32 *)((u8 *)object + offset) =
            (u32)fn_80440920(stream, value, type - 1, values, &status);
        if (status != 0) {
            return stream;
        }
    } else {
        if (fn_8047CF50(stream, 0x2C, 0, 0) == 0) {
            return NULL;
        }
        *(u32 *)((u8 *)object + offset) = (u32)fn_80440670(stream);
    }
    if (*(u32 *)((u8 *)object + offset) != 0) {
        return stream;
    }
    return NULL;
}

void fn_80441018(u32 *object) {
    *(u32 *)((u8 *)object + lbl_805F1418) = 0;
}

typedef void (*RwCpp05Callback)(void);

static inline u32 RwCpp05Positive(s32 value) {
    return (u32)(-value & ~value) >> 0x1F;
}

static inline u32 RwCpp05Nonnegative(s32 value) {
    return ((u32)value >> 0x1F) ^ 1;
}

s32 fn_80485D78(u32, u32, RwCpp05Callback, RwCpp05Callback);
s32 fn_80458B6C(u32, u32, RwCpp05Callback, void *, void *);
s32 fn_8045F2F8(u32, u32, RwCpp05Callback, RwCpp05Callback, void *);
s32 fn_8045F338(u32, RwCpp05Callback, RwCpp05Callback, RwCpp05Callback);
s32 fn_8045994C(u32, u32, RwCpp05Callback, RwCpp05Callback, void *);
s32 fn_8045998C(u32, RwCpp05Callback, RwCpp05Callback, RwCpp05Callback);

typedef u32 (*RwCpp05EmitCallback)(u32 *, u32 *, float *, void *, float);

s32 fn_8047BDD0(void *, void *, void *, void *);
float fn_80482394(float);
extern float lbl_805FAF88;
extern float lbl_805FAF8C;
extern float lbl_805FAF90;
extern float lbl_805FAF94;
extern float lbl_805FAF98;

#define RW05_EMIT_TRIANGLES(input, entry, node, baseA, baseB, start, span, extra, callback, user)                 \
    do {                                                                                                         \
        u32 rwEnd = (start) + (span);                                                                             \
        for (u32 rwI = (start); rwI < rwEnd; rwI++) {                                                            \
            u32 rwValue = (node)->tail != NULL ? (node)->tail[rwI] : rwI;                                         \
            rwValue <<= 3;                                                                                        \
            u16 *rwIndices = (u16 *)((baseA) + rwValue);                                                          \
            u32 *rwA = (u32 *)((baseB) + rwIndices[0] * 0xC);                                                     \
            u32 *rwB = (u32 *)((baseB) + rwIndices[1] * 0xC);                                                     \
            u32 *rwC = (u32 *)((baseB) + rwIndices[2] * 0xC);                                                     \
            if ((extra) != 0 && fn_8047BDD0((input), rwA, rwB, rwC) == 0) continue;                               \
            float rwAbx = *(float *)rwB - *(float *)rwA;                                                          \
            float rwAcx = *(float *)rwC - *(float *)rwA;                                                          \
            float rwAby = *(float *)(rwB + 1) - *(float *)(rwA + 1);                                              \
            float rwAcy = *(float *)(rwC + 1) - *(float *)(rwA + 1);                                              \
            float rwAbz = *(float *)(rwB + 2) - *(float *)(rwA + 2);                                              \
            float rwAcz = *(float *)(rwC + 2) - *(float *)(rwA + 2);                                              \
            float rwX = rwAby * rwAcz - rwAbz * rwAcy;                                                            \
            float rwY = rwAbz * rwAcx - rwAbx * rwAcz;                                                            \
            float rwZ = rwAbx * rwAcy - rwAby * rwAcx;                                                            \
            float rwScale = fn_80482394(rwX * rwX + rwY * rwY + rwZ * rwZ);                                      \
            float rwNormal[3];                                                                                    \
            rwNormal[0] = rwX * rwScale;                                                                          \
            rwNormal[1] = rwY * rwScale;                                                                          \
            rwNormal[2] = rwZ * rwScale;                                                                          \
            if ((callback)((input), (entry), rwNormal, (user), lbl_805FAF8C) == 0) return;                       \
        }                                                                                                         \
    } while (0)

void fn_80441188(u32 *owner, u32 *input, RwCpp05EmitCallback callback, void *user) {
    float work[208];
    work[0] = *(float *)input;
    work[1] = *(float *)(input + 1);
    work[2] = *(float *)(input + 2);
    float rangeX = *(float *)(input + 3) - work[0];
    float rangeY = *(float *)(input + 4) - work[1];
    float rangeZ = *(float *)(input + 5) - work[2];
    work[3] = rangeX;
    work[4] = rangeY;
    work[5] = rangeZ;
    work[6] = rangeX == 0.0f ? lbl_805FAF8C : lbl_805FAF88 / rangeX;
    work[7] = rangeY == 0.0f ? lbl_805FAF8C : lbl_805FAF88 / rangeY;
    work[8] = rangeZ == 0.0f ? lbl_805FAF8C : lbl_805FAF88 / rangeZ;
    float endX = *(float *)(input + 3);
    float endY = *(float *)(input + 4);
    float endZ = *(float *)(input + 5);
    float startX = work[0];
    float startY = work[1];
    float startZ = work[2];
    if (endX < 0.0f) endX = -endX;
    if (endY < 0.0f) endY = -endY;
    if (endZ < 0.0f) endZ = -endZ;
    if (startX < 0.0f) startX = -startX;
    if (startY < 0.0f) startY = -startY;
    if (startZ < 0.0f) startZ = -startZ;
    work[9] = lbl_805FAF90 * (endX >= startX ? endX : startX);
    work[10] = lbl_805FAF90 * (endY >= startY ? endY : startY);
    work[11] = lbl_805FAF90 * (endZ >= startZ ? endZ : startZ);
    *(u32 *)(work + 12) = owner[7];
    work[13] = lbl_805FAF8C;
    work[14] = lbl_805FAF88;

    u32 *cursor = (u32 *)(work + 15);
    while (cursor > (u32 *)(work + 12)) {
        cursor -= 3;
        u32 *entry = (u32 *)cursor[0];
        while ((s32)entry[0] >= 0) {
            u32 offset = entry[0];
            float origin = *(float *)((u8 *)work + offset);
            float interval = *(float *)((u8 *)(work + 3) + offset);
            float inverse = *(float *)((u8 *)(work + 6) + offset);
            float limit = *(float *)((u8 *)(work + 9) + offset);
            float f1 = *(float *)(entry + 2);
            float f0 = *(float *)(entry + 1);
            float f4 = f1 * interval + origin;
            float f3 = f0 * interval + origin;
            float f6 = *(float *)(entry + 5) - limit;
            float f5 = *(float *)(entry + 4) + limit;
            if (interval > lbl_805FAF8C) {
                if (f6 > f4) {
                    cursor[0] = entry[2];
                } else if (f5 < f3) {
                    cursor[0] = entry[3];
                } else {
                    cursor += 3;
                    cursor[0] = entry[2];
                    ((float *)cursor)[1] = ((float *)cursor)[-2];
                    ((float *)cursor)[2] = ((float *)cursor)[-1];
                    if (f5 < f4) {
                        ((float *)cursor)[2] += inverse * (f5 - f4);
                    }
                    if (f6 > f3) {
                        ((float *)cursor)[-2] += inverse * (f6 - f3);
                    }
                    cursor[-3] = entry[3];
                }
            } else {
                if (f5 < f4) {
                    cursor[0] = entry[3];
                } else if (f6 > f3) {
                    cursor[0] = entry[2];
                } else {
                    cursor += 3;
                    cursor[0] = entry[3];
                    ((float *)cursor)[1] = ((float *)cursor)[-2];
                    ((float *)cursor)[2] = ((float *)cursor)[-1];
                    if (f6 > f4) {
                        ((float *)cursor)[2] += inverse * (f6 - f4);
                    }
                    if (f5 < f3) {
                        ((float *)cursor)[-2] += inverse * (f5 - f3);
                    }
                    cursor[-3] = entry[2];
                }
            }
            entry = (u32 *)cursor[0];
        }
        RwCpp05Node *node = (RwCpp05Node *)((u8 *)entry + lbl_805F1420);
        if (node == NULL) {
            continue;
        }
        u32 baseA = entry[1];
        u32 baseB = entry[2];
        u32 *callbackEntry = entry;
        u32 linkWords[128];
        RwCpp05Record *initial = (RwCpp05Record *)linkWords;
        initial->id = 0xFF;
        initial->index = 0;
        if (node->recordCount == 0) {
            initial->id = (u8)node->valueCount;
        }
        linkWords[2] = (u32)initial;
        ((float *)linkWords)[3] = ((float *)cursor)[1];
        ((float *)linkWords)[4] = ((float *)cursor)[2];
        u32 *linkBase = linkWords + 2;
        u32 *linkCursor = linkWords + 5;

        while (linkCursor > linkBase) {
            linkCursor -= 3;
            bool descend = true;
            while (descend) {
                descend = false;
                RwCpp05Record *record = (RwCpp05Record *)linkCursor[0];
                if (record->id != 0xFF) {
                    u32 start = record->index;
                    u32 end = start + record->id;
                    for (u32 i = start; i < end; i++) {
                        do {
                            u32 value = node->tail != NULL ? node->tail[i] : i;
                            value <<= 3;
                            u16 *indices = (u16 *)(baseA + value);
                            u32 *a = (u32 *)(baseB + indices[0] * 0xC);
                            u32 *b = (u32 *)(baseB + indices[2] * 0xC);
                            u32 *c = (u32 *)(baseB + indices[1] * 0xC);
                            float a0 = *(float *)a;
                            float a1 = *(float *)(a + 1);
                            float a2 = *(float *)(a + 2);
                            float b0 = *(float *)b;
                            float b1 = *(float *)(b + 1);
                            float b2 = *(float *)(b + 2);
                            float c0 = *(float *)c;
                            float c1 = *(float *)(c + 1);
                            float c2 = *(float *)(c + 2);
                            float sx = *(float *)input;
                            float sy = *(float *)(input + 1);
                            float sz = *(float *)(input + 2);
                            float rx = *(float *)(input + 3) - sx;
                            float ry = *(float *)(input + 4) - sy;
                            float rz = *(float *)(input + 5) - sz;
                            float d0 = b0 - a0;
                            float d1 = b1 - a1;
                            float d2 = b2 - a2;
                            float e0 = c0 - a0;
                            float e1 = c1 - a1;
                            float e2 = c2 - a2;
                            float crossX = rz * d0 - rx * d2;
                            float crossY = ry * d2 - rz * d1;
                            float crossZ = rx * d1 - ry * d0;
                            float det = e1 * crossX + e0 * crossY + e2 * crossZ;
                            if (det == lbl_805FAF94) {
                                break;
                            }
                            float limit = lbl_805FAF98 * -det;
                            float remaining = det - limit;
                            float sxrel = sx - a0;
                            float syrel = sy - a1;
                            float szrel = sz - a2;
                            float bary0 = syrel * crossX + sxrel * crossY + szrel * crossZ;
                            if (bary0 < limit || bary0 > remaining) {
                                break;
                            }
                            float q0 = szrel * e0 - sxrel * e2;
                            float q1 = syrel * e2 - szrel * e1;
                            float q2 = sxrel * e1 - syrel * e0;
                            float bary1 = ry * q0 + rx * q1 + rz * q2;
                            if (bary1 < limit || bary0 + bary1 > remaining) {
                                break;
                            }
                            float bary2 = d1 * q0 + d0 * q1 + d2 * q2;
                            if (bary2 < limit || bary2 > remaining) {
                                break;
                            }
                            float scale = bary2 / det;
                            float n0 = d1 * e2 - d2 * e1;
                            float n1 = d2 * e0 - d0 * e2;
                            float n2 = d0 * e1 - d1 * e0;
                            float normalScale = fn_80482394(n0 * n0 + n1 * n1 + n2 * n2);
                            float normal[3];
                            normal[0] = n0 * normalScale;
                            normal[1] = n1 * normalScale;
                            normal[2] = n2 * normalScale;
                            if (callback(input, callbackEntry, normal, user, scale) == 0) {
                                return;
                            }
                        } while (0);
                    }
                } else {
                    record = (RwCpp05Record *)(node->records + record->index * 0x10);
                    u32 kind = record->kind & 0xC;
                    float *linkFloats = (float *)linkCursor;
                    float *recordFloats = (float *)record;
                    float f3 = linkFloats[2];
                    float f2 = linkFloats[1];
                    float f1 = recordFloats[3];
                    float f0 = recordFloats[1];
                    float f6 = *(float *)((u8 *)(work + 9) + kind);
                    float f5 = *(float *)((u8 *)(work + 3) + kind);
                    float f4 = *(float *)((u8 *)work + kind);
                    float f7 = f1 - f6;
                    f1 = f3 * f5 + f4;
                    f3 = f2 * f5 + f4;
                    f4 = f0 + f6;
                    if (f5 > lbl_805FAF8C) {
                        if (f7 > f1) {
                            linkCursor[0] = (u32)record;
                        } else if (f4 < f3) {
                            linkCursor[0] = (u32)record + 8;
                        } else {
                            float inverse = *(float *)((u8 *)(work + 6) + kind);
                            linkCursor += 3;
                            linkCursor[0] = (u32)record;
                            ((float *)linkCursor)[1] = ((float *)linkCursor)[-2];
                            ((float *)linkCursor)[2] = ((float *)linkCursor)[-1];
                            if (f4 < f1) {
                                ((float *)linkCursor)[2] += inverse * (f4 - f1);
                            }
                            linkCursor[-3] = (u32)record + 8;
                            if (f7 > f3) {
                                ((float *)linkCursor)[-2] += inverse * (f7 - f3);
                            }
                            descend = true;
                        }
                    } else {
                        if (f4 < f1) {
                            linkCursor[0] = (u32)record + 8;
                        } else if (f7 > f3) {
                            linkCursor[0] = (u32)record;
                        } else {
                            float inverse = *(float *)((u8 *)(work + 6) + kind);
                            linkCursor += 3;
                            linkCursor[0] = (u32)record + 8;
                            ((float *)linkCursor)[1] = ((float *)linkCursor)[-2];
                            ((float *)linkCursor)[2] = ((float *)linkCursor)[-1];
                            if (f7 > f1) {
                                ((float *)linkCursor)[2] += inverse * (f7 - f1);
                            }
                            linkCursor[-3] = (u32)record;
                            if (f4 < f3) {
                                ((float *)linkCursor)[-2] += inverse * (f4 - f3);
                            }
                            descend = true;
                        }
                    }
                    descend = true;
                }
            }
        }
    }
}

void fn_80441BF8(u32 *owner, u32 *input, RwCpp05EmitCallback callback, void *user) {
    u32 values[7];
    values[0] = input[0];
    values[1] = input[1];
    values[2] = input[2];
    values[3] = input[3];
    values[4] = input[4];
    values[5] = input[5];
    values[6] = owner[7];

    u32 *cursor = values + 7;
    while (cursor > values + 6) {
        cursor--;
        u32 *entry = (u32 *)*cursor;
        while ((s32)entry[0] >= 0) {
            if (*(float *)(entry + 5) > *(float *)((u8 *)values + entry[0])) {
                *cursor = entry[2];
            } else {
                *cursor = entry[3];
                if (*(float *)(entry + 4) >= *(float *)((u8 *)(values + 3) + entry[0])) {
                    cursor++;
                    *cursor = entry[2];
                }
            }
            entry = (u32 *)*cursor;
        }

        RwCpp05Node *node = (RwCpp05Node *)((u8 *)entry + lbl_805F1420);
        if (node == NULL) {
            continue;
        }
        u32 baseA = entry[1];
        u32 baseB = entry[2];

        u32 boundsA[6];
        u32 boundsB[6];
        for (u32 i = 0; i < 6; i++) {
            boundsA[i] = input[i];
            boundsB[i] = input[i];
        }
        u32 linkWords[124];
        RwCpp05Record *initial = (RwCpp05Record *)linkWords;
        initial->id = 0xFF;
        initial->index = 0;
        if (node->recordCount == 0) {
            initial->id = (u8)node->valueCount;
        }
        linkWords[2] = (u32)initial;
        u32 *linkBase = linkWords + 2;
        u32 *linkCursor;
        u32 flags = 0;

        if (*(float *)(node->bounds + 3) <= *(float *)input || *(float *)(node->bounds + 4) <= *(float *)(input + 1) ||
            *(float *)(node->bounds + 5) <= *(float *)(input + 2) || *(float *)(node->bounds + 0) >= *(float *)(input + 3) ||
            *(float *)(node->bounds + 1) >= *(float *)(input + 4) || *(float *)(node->bounds + 2) >= *(float *)(input + 5)) {
            linkCursor = linkBase;
        } else {
            if (*(float *)(node->bounds + 4) < *(float *)(input + 4)) flags |= 4;
            if (*(float *)(node->bounds + 3) < *(float *)(input + 3)) flags |= 1;
            if (*(float *)(node->bounds + 5) < *(float *)(input + 5)) flags |= 0x10;
            if (*(float *)(node->bounds + 0) > *(float *)input) flags |= 2;
            if (*(float *)(node->bounds + 1) > *(float *)(input + 1)) flags |= 8;
            if (*(float *)(node->bounds + 2) > *(float *)(input + 2)) flags |= 0x20;
            linkWords[3] = flags;
            linkCursor = linkWords + 4;
        }

        while (linkCursor > linkBase) {
            linkCursor -= 2;
            bool descend = true;
            while (descend) {
                descend = false;
                RwCpp05Record *record = (RwCpp05Record *)linkCursor[0];
                u32 extra = linkCursor[1];
                if (record->id != 0xFF) {
                    RW05_EMIT_TRIANGLES(input, entry, node, baseA, baseB, record->index,
                                        record->id, extra, callback, user);
                } else if (extra == 0) {
                    RwCpp05Record *first = (RwCpp05Record *)(node->records + record->index * 0x10);
                    while (first->id == 0xFF) {
                        first = (RwCpp05Record *)(node->records + first->index * 0x10);
                    }
                    RwCpp05Record *second = (RwCpp05Record *)((u8 *)first + 8);
                    while (second->id == 0xFF) {
                        second = (RwCpp05Record *)(node->records + second->index * 0x10 + 8);
                    }
                    u32 span = (u16)(second->index + second->id - first->index);
                    RW05_EMIT_TRIANGLES(input, entry, node, baseA, baseB, first->index,
                                        span, 0, callback, user);
                } else {
                    record = (RwCpp05Record *)(node->records + record->index * 0x10);
                    u32 kind = record->kind & 0xC;
                    if (*(float *)((u32 *)record + 3) > *(float *)((u8 *)boundsA + kind)) {
                        linkCursor[0] = (u32)record;
                    } else {
                        linkCursor[0] = (u32)record + 8;
                        if (*(float *)((u32 *)record + 1) >= *(float *)((u8 *)(boundsA + 3) + kind)) {
                            linkCursor += 2;
                            linkCursor[0] = (u32)record;
                            linkCursor[1] = linkCursor[-1];
                            if (*(float *)((u32 *)record + 1) < *(float *)((u8 *)boundsB + kind)) {
                                linkCursor[1] &= ~(2 << (kind >> 1));
                            }
                            if (*(float *)((u32 *)record + 3) >= *(float *)((u8 *)(boundsB + 3) + kind)) {
                                linkCursor[-1] &= ~(1 << (kind >> 1));
                            }
                        }
                    }
                    descend = true;
                }
            }
        }
    }
}

u32 fn_80441028(void) {
    s32 result = fn_80485D78(4, 0x11D, fn_80440D28, fn_80440D48);
    lbl_805F1414 = result;
    result = RwCpp05Positive(result);
    if (result != 0) {
        result = fn_80458B6C(4, 0x11D, (RwCpp05Callback)fn_80441018, NULL, NULL);
        lbl_805F1418 = result;
        result = RwCpp05Positive(result);
        if (result != 0) {
            result = fn_8045F2F8(4, 0x11D, (RwCpp05Callback)fn_80440DA8, (RwCpp05Callback)fn_80440D58, NULL);
            lbl_805F1420 = result;
            result = RwCpp05Nonnegative(result);
            if (result != 0) {
                result = fn_8045F338(0x11D, (RwCpp05Callback)fn_80440E80,
                                     (RwCpp05Callback)fn_80440DEC,
                                     (RwCpp05Callback)fn_80440DB4);
            }
            result = RwCpp05Nonnegative(result);
            result = result != 0;
            u32 proceed = result != 0;
            result = proceed;
            if (proceed != 0) {
                result = fn_8045994C(4, 0x11D, (RwCpp05Callback)fn_80440DA8, (RwCpp05Callback)fn_80440D58, NULL);
                lbl_805F141C = result;
                result = RwCpp05Nonnegative(result);
                if (result != 0) {
                    result = fn_8045998C(0x11D, (RwCpp05Callback)fn_80440E80,
                                         (RwCpp05Callback)fn_80440DEC,
                                         (RwCpp05Callback)fn_80440DB4);
                }
                result = RwCpp05Nonnegative(result);
            }
        }
    }
    u32 output = result != 0;
    return output;
}
}

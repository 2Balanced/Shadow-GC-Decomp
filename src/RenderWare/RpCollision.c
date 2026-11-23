/* RenderWare: RpCollision (plugin 0x11D, rwID_COLLISPLUGIN), 0x80440410..0x80442B98: collision
 * trees of world sectors and geometries: the tree's stream functions, the plugin's engine, atomic,
 * world-sector and geometry callbacks and its attach function, then the line, box and sphere
 * queries over a world and their dispatcher. One file: its .sbss (0x805F1410..0x805F1428, the
 * instance count and four plugin offsets) ends padded to 8 bytes before plugin 0x122's offset, and
 * its .sdata2 pool (0x805FAF88..0x805FAFA0) is used only by the queries.
 * Built as C without lmw/stmw at -O4,p, inlining only what is declared inline, in definition order
 * (cflags_rw_c, 12 of 18 functions; the game's flags give the same code, while the
 * auto-inlining sets cflags_rw_r3, _r5 and _r6 inline the small static helpers fn_80440410 and
 * fn_8044043C, which retail calls out of line, and lose two). Names are inferred from behavior. NonMatching: fn_80440468, fn_80440670,
 * fn_80440920, fn_80441188, fn_80441BF8 and fn_80442210 (drafts). */
#include <dolphin/types.h>
#include "Game/rw/r1.h"

extern u8 *lbl_805F15B4;

s32 lbl_805F1410;
s32 lbl_805F1414;
s32 lbl_805F1418;
s32 lbl_805F141C;
s32 lbl_805F1420;

typedef void (*R1Callback)(void);

u32 fn_80485D6C(void);
s32 fn_80485D78(u32, u32, R1Callback, R1Callback);
s32 fn_80458B6C(u32, u32, R1Callback, void *, void *);
s32 fn_8045F2F8(u32, u32, R1Callback, R1Callback, void *);
s32 fn_8045F338(u32, R1Callback, R1Callback, R1Callback);
s32 fn_8045994C(u32, u32, R1Callback, R1Callback, void *);
s32 fn_8045998C(u32, R1Callback, R1Callback, R1Callback);
void *fn_8047CEF0(void *, u32, u32, u32, u32);
s32 fn_8047CF50(void *, u32, u32, void *);
void fn_8047D158(void *, u32);
void fn_8047D1C8(void *, u32);
void fn_8047D254(void *, u32);
void fn_8047D2C4(void *, u32);
u32 fn_8047D458(void *, void *, u32);
u32 fn_8047D560(void *, void *, u32);
u32 fn_8047D684(void *, void *, u32);
u32 fn_8047D784(void *, void *, u32);
u32 fn_8047D868(void *, void *, u32);
u32 fn_8047DC28(u32, ...);
void fn_8047DB84(void *);
u32 fn_80480D4C(void *, void *, u32);
u32 fn_80480ED0(void *, void *, u32);
u32 fn_804810FC(void *, u32);
s32 fn_8047BDD0(void *, void *, void *, void *);
s32 fn_8047B720(void *, void *, void *, void *, void *, void *);
float fn_80482394(float);
R1Sphere *fn_804566D0(void *);

void fn_80440410(void *object) {
    ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(object);
}

int fn_8044043C(u32 *header) {
    int size = *(u16 *)((u8 *)header + 0x1E) * 0x10 + 0x30;
    if ((header[0] & 1) != 0) size += *(u16 *)(header + 7) * 2;
    return size;
}

void *fn_80440468(u32 *header, void *context) {
    int size;
    u32 local[9];
    u16 count;
    size = *(u16 *)((u8 *)header + 0x1E) * 0x10 + 0x30;
    if ((header[0] & 1) != 0) size += *(u16 *)(header + 7) * 2;
    if (fn_8047CEF0(context, 0x2C, size, 0x37002, 0x37) == NULL) return NULL;
    if (fn_8047CEF0(context, 1, size - 0xC, 0x37002, 0x37) == NULL) return NULL;
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
    count = *(u16 *)((u8 *)header + 0x1E);
    if (count != 0) {
        u8 *entries = (u8 *)header[8];
        u32 i;
        for (i = 0; i < count; i++) {
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
    u32 flags;
    u32 nameCount;
    u32 extraCount;
    u32 size;
    u32 *header;
    if (fn_8047CF50(context, 1, 0, local) == 0) return NULL;
    if (fn_80480D4C(context, local + 9, 0x24) != 0x24) return NULL;
    fn_8047D254(local + 9, 0x24);
    flags = local[9];
    nameCount = local[17] & 0xFFFF;
    extraCount = local[16] & 0xFFFF;
    size = (nameCount != 0) ? nameCount * 0x10 + 0x37 : 0x28;
    if ((flags & 1) != 0) size += extraCount * 2 + 1;
    header = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(size, 0x3002C);
    if (header == NULL) {
        local[1] = 0x11D;
        local[2] = fn_8047DC28(0x80000013, size);
        fn_8047DB84(local + 1);
    } else {
        u8 *next;
        header[0] = flags;
        *(u16 *)(header + 7) = (u16)extraCount;
        *(u16 *)((u8 *)header + 0x1E) = (u16)nameCount;
        header[1] = local[13];
        header[2] = local[14];
        header[3] = local[15];
        header[4] = local[10];
        header[5] = local[11];
        header[6] = local[12];
        next = (u8 *)(header + 10);
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
        u8 *entry;
        u32 i;
        if (fn_80480D4C(context, (void *)header[8], bytes) != bytes) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(header);
            return NULL;
        }
        entry = (u8 *)header[8];
        for (i = 0; i < *(u16 *)((u8 *)header + 0x1E); i++) {
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

typedef struct RwCpp05Node {
    u32 flags;
    u32 bounds[6];
    u16 valueCount;
    u16 recordCount;
    u8 *records;
    u16 *tail;
} RwCpp05Node;

typedef struct RwCpp05Record {
    u8 kind;
    u8 id;
    u16 index;
    u32 field4;
    u8 otherKind;
    u8 field9;
    u16 value;
    u32 fieldC;
} RwCpp05Record;

typedef struct RwCpp05Initial {
    u8 kind;
    u8 id;
    u16 index;
    u32 field4;
} RwCpp05Initial;

#define RW05_API(type, offset) ((type)*(u32 *)(lbl_805F15B4 + (offset)))

void *fn_80440920(void *stream, u32 value, u32 count, void *boundsData, s32 *status) {
    u32 size;
    void *allocation;
    RwCpp05Node *node;
    const u32 *bounds;
    u8 *next;
    *status = 0;

    size = 0x28;
    if ((u16)count != 0) {
        size = ((u16)count << 4) + 0x37;
    }
    size += ((u16)value << 1);
    size += 1;
    allocation = RW05_API(void *(*)(u32, u32), 0x108)(size, 0x3002C);
    if (allocation == NULL) {
        u32 error[2];
        error[0] = 0x11D;
        error[1] = fn_8047DC28(0x80000013, size);
        fn_8047DB84(error);
        return NULL;
    }
    node = (RwCpp05Node *)allocation;

    node->flags = 1;
    node->valueCount = (u16)value;
    node->recordCount = (u16)count;
    bounds = (const u32 *)boundsData;
    node->bounds[0] = bounds[0];
    node->bounds[1] = bounds[1];
    node->bounds[2] = bounds[2];
    node->bounds[3] = bounds[3];
    node->bounds[4] = bounds[4];
    node->bounds[5] = bounds[5];

    next = (u8 *)node + 0x28;
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
        u32 i;
        RwCpp05Initial initial;
        u32 *links[32];
        s32 depth;
        RwCpp05Record *current;
        for (i = 0; i < node->recordCount; i++) {
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

        initial.id = 0xFF;
        initial.index = 0;
        depth = 1;
        current = (RwCpp05Record *)&initial;
        while (depth != 0) {
            if (current->id != 0xFF) {
                u32 packed;
                u32 id;
                if (fn_8047D868(stream, &packed, 4) == 0) {
                    RW05_API(void (*)(void *), 0x10C)(node);
                    return NULL;
                }
                id = packed >> 0x10;
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
                u8 *child = node->records + current->index * 0x10;
                links[depth] = (u32 *)(child + 8);
                depth++;
                current = (RwCpp05Record *)child;
            }
        }
    } else if (fn_804810FC(stream, 4) == 0) {
        RW05_API(void (*)(void *), 0x10C)(node);
        return NULL;
    }

    if ((node->flags & 1) != 0) {
        u32 i;
        for (i = 0; i < node->valueCount; i++) {
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

/* The plugin's engine constructor and destructor (registered with fn_80485D78). */
void *fn_80440D28(void *instance, s32 offset, s32 size) {
    lbl_805F1410++;
    *(u32 *)(lbl_805F15B4 + lbl_805F1414) = 0;
    return instance;
}

void *fn_80440D48(void *instance, s32 offset, s32 size) {
    lbl_805F1410--;
    return instance;
}

u32 *fn_80440D58(u32 *object, u32 offset) {
    u32 value = *(u32 *)((u8 *)object + offset);
    if (value != 0) {
        fn_80440410((void *)value);
        *(u32 *)((u8 *)object + offset) = 0;
    }
    return object;
}

void fn_80440DA8(u32 *object, u32 offset) {
    *(u32 *)((u8 *)object + offset) = 0;
}

void *fn_80440DB4(const u32 *object, u32 offset) {
    if (*(const u32 *)((const u8 *)object + offset) != 0) {
        return (u8 *)fn_8044043C((u32 *)*(const u32 *)((const u8 *)object + offset)) + 0x10;
    }
    return NULL;
}

/* Stream write callback: the engine version, then the collision tree. */
void *fn_80440DEC(void *stream, s32 length, const void *object, s32 offset, s32 size) {
    u32 value = *(const u32 *)((const u8 *)object + offset);
    if (value != 0) {
        u32 context = fn_80485D6C();
        if (fn_8047D458(stream, &context, 4) == 0) {
            return NULL;
        }
        if (fn_80440468((u32 *)*(const u32 *)((const u8 *)object + offset), stream) == NULL) {
            return NULL;
        }
    }
    return stream;
}

/* Stream read callback: a pre-3.6 tree is rebuilt (fn_80440920) inside the object's box, a newer
 * one read (fn_80440670). */
void *fn_80440E80(void *stream, s32 length, void *object, s32 offset, s32 size) {
    u32 version;
    u32 type;
    R1BBox box;
    s32 status;

    if (fn_8047D868(stream, &version, 4) == 0) {
        return NULL;
    }
    if (version < 0x36001) {
        if (fn_8047D868(stream, &type, 4) == 0) {
            return NULL;
        }
        if (((R1Geometry *)object)->m_type == 8) {
            R1Sphere *sphere = &((R1Geometry *)object)->m_morphTarget->m_boundingSphere;
            box.inf.x = sphere->center.x - sphere->radius;
            box.inf.y = sphere->center.y - sphere->radius;
            box.inf.z = sphere->center.z - sphere->radius;
            box.sup.x = sphere->center.x + sphere->radius;
            box.sup.y = sphere->center.y + sphere->radius;
            box.sup.z = sphere->center.z + sphere->radius;
        } else {
            box = ((R1WorldSector *)object)->m_boundingBox;
        }
        status = 0;
        *(void **)((u8 *)object + offset) = fn_80440920(stream, type, version - 1, &box, &status);
        if (status != 0) {
            return stream;
        }
    } else {
        if (fn_8047CF50(stream, 0x2C, 0, 0) == 0) {
            return NULL;
        }
        *(void **)((u8 *)object + offset) = fn_80440670(stream);
    }
    if (*(void **)((u8 *)object + offset) != NULL) {
        return stream;
    }
    return NULL;
}

void fn_80441018(u32 *object) {
    *(u32 *)((u8 *)object + lbl_805F1418) = 0;
}

static inline u32 RwCpp05Positive(s32 value) {
    return (u32)(-value & ~value) >> 0x1F;
}

static inline u32 RwCpp05Nonnegative(s32 value) {
    return ((u32)value >> 0x1F) ^ 1;
}

static inline int RwCpp05Nonzero(int value) {
    return (u32)(-value | value) >> 0x1F;
}

int fn_80441028(void) {
    int success;
    s32 offset = fn_80485D78(4, 0x11D, (R1Callback)fn_80440D28, (R1Callback)fn_80440D48);
    lbl_805F1414 = offset;
    success = RwCpp05Positive(offset);
    if (success) {
        int attached;
        offset = fn_80458B6C(4, 0x11D, (R1Callback)fn_80441018, NULL, NULL);
        lbl_805F1418 = offset;
        success = RwCpp05Positive(offset);
        if (success) {
            offset = fn_8045F2F8(4, 0x11D, (R1Callback)fn_80440DA8, (R1Callback)fn_80440D58, NULL);
            lbl_805F1420 = offset;
            success = RwCpp05Nonnegative(offset);
            if (success) {
                success = RwCpp05Nonnegative(fn_8045F338(0x11D, (R1Callback)fn_80440E80,
                                                         (R1Callback)fn_80440DEC,
                                                         (R1Callback)fn_80440DB4));
            }
            success = RwCpp05Nonzero(success);
        }
        attached = RwCpp05Nonzero(success);
        if (attached) {
            offset = fn_8045994C(4, 0x11D, (R1Callback)fn_80440DA8, (R1Callback)fn_80440D58, NULL);
            lbl_805F141C = offset;
            success = RwCpp05Nonnegative(offset);
            if (success) {
                success = RwCpp05Nonnegative(fn_8045998C(0x11D, (R1Callback)fn_80440E80,
                                                         (R1Callback)fn_80440DEC,
                                                         (R1Callback)fn_80440DB4));
            }
            attached = RwCpp05Nonzero(success);
        }
        success = attached;
    }
    return success;
}

typedef u32 (*RwCpp05EmitCallback)(u32 *, u32 *, float *, void *, float);

#define RW05_EMIT_TRIANGLES(input, entry, node, baseA, baseB, start, span, extra, callback, user)  \
    do {                                                                                          \
        u16 rwStart = (u16)(start);                                                               \
        s32 rwEnd = (s32)rwStart + (s32)(span);                                                   \
        u16 rwI;                                                                                  \
        for (rwI = rwStart; (s32)rwI < rwEnd; rwI++) {                                            \
            u32 rwValue = (node)->tail != NULL ? (node)->tail[rwI] : rwI;                          \
            u16 *rwIndices;                                                                       \
            u32 *rwA;                                                                             \
            u32 *rwB;                                                                             \
            u32 *rwC;                                                                             \
            float rwAbx, rwAcx, rwAby, rwAcy, rwAbz, rwAcz, rwX, rwY, rwZ, rwScale;               \
            float rwNormal[3];                                                                    \
            rwValue <<= 3;                                                                        \
            rwIndices = (u16 *)((baseA) + rwValue);                                               \
            rwA = (u32 *)((baseB) + rwIndices[0] * 0xC);                                          \
            rwB = (u32 *)((baseB) + rwIndices[1] * 0xC);                                          \
            rwC = (u32 *)((baseB) + rwIndices[2] * 0xC);                                          \
            if ((extra) != 0 && fn_8047BDD0((input), rwA, rwB, rwC) == 0) continue;                \
            rwAbx = *(float *)rwB - *(float *)rwA;                                                \
            rwAcx = *(float *)rwC - *(float *)rwA;                                                \
            rwAby = *(float *)(rwB + 1) - *(float *)(rwA + 1);                                    \
            rwAcy = *(float *)(rwC + 1) - *(float *)(rwA + 1);                                    \
            rwAbz = *(float *)(rwB + 2) - *(float *)(rwA + 2);                                    \
            rwAcz = *(float *)(rwC + 2) - *(float *)(rwA + 2);                                    \
            rwX = rwAby * rwAcz - rwAbz * rwAcy;                                                  \
            rwY = rwAbz * rwAcx - rwAbx * rwAcz;                                                  \
            rwZ = rwAbx * rwAcy - rwAby * rwAcx;                                                  \
            rwScale = fn_80482394(rwX * rwX + rwY * rwY + rwZ * rwZ);                              \
            rwNormal[0] = rwX * rwScale;                                                          \
            rwNormal[1] = rwY * rwScale;                                                          \
            rwNormal[2] = rwZ * rwScale;                                                          \
            if ((callback)((input), (entry), rwNormal, (user), 0.0f) == 0) return;                 \
        }                                                                                         \
    } while (0)

/* Line query over the world's sectors and their trees. NonMatching draft. */
void fn_80441188(u32 *owner, u32 *input, RwCpp05EmitCallback callback, void *user) {
    float work[208];
    float rangeX, rangeY, rangeZ;
    float endX, endY, endZ;
    float startX, startY, startZ;
    u32 *cursor;
    *(u32 *)(work + 0) = input[0];
    *(u32 *)(work + 1) = input[1];
    *(u32 *)(work + 2) = input[2];
    rangeX = *(float *)(input + 3) - work[0];
    rangeY = *(float *)(input + 4) - work[1];
    rangeZ = *(float *)(input + 5) - work[2];
    work[3] = rangeX;
    work[4] = rangeY;
    work[5] = rangeZ;
    work[6] = rangeX == 0.0f ? 0.0f : 1.0f / rangeX;
    work[7] = rangeY == 0.0f ? 0.0f : 1.0f / rangeY;
    work[8] = rangeZ == 0.0f ? 0.0f : 1.0f / rangeZ;
    endX = *(float *)(input + 3);
    endY = *(float *)(input + 4);
    endZ = *(float *)(input + 5);
    startX = work[0];
    startY = work[1];
    startZ = work[2];
    if (endX < 0.0f) endX = -endX;
    if (endY < 0.0f) endY = -endY;
    if (endZ < 0.0f) endZ = -endZ;
    if (startX < 0.0f) startX = -startX;
    if (startY < 0.0f) startY = -startY;
    if (startZ < 0.0f) startZ = -startZ;
    work[9] = 0.000001f * (endX >= startX ? endX : startX);
    work[10] = 0.000001f * (endY >= startY ? endY : startY);
    work[11] = 0.000001f * (endZ >= startZ ? endZ : startZ);
    *(u32 *)(work + 12) = owner[7];
    work[13] = 0.0f;
    work[14] = 1.0f;

    cursor = (u32 *)(work + 15);
    while (cursor > (u32 *)(work + 12)) {
        u32 *entry;
        RwCpp05Node *node;
        u32 baseA;
        u32 baseB;
        u32 *callbackEntry;
        u32 linkWords[136];
        RwCpp05Record *initial;
        u32 *linkBase;
        u32 *linkCursor;
        cursor -= 3;
        entry = (u32 *)cursor[0];
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
            if (interval > 0.0f) {
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
        node = (RwCpp05Node *)((u8 *)entry + lbl_805F1420);
        if (node == NULL) {
            continue;
        }
        baseA = entry[1];
        baseB = entry[2];
        callbackEntry = entry;
        initial = (RwCpp05Record *)linkWords;
        initial->id = 0xFF;
        initial->index = 0;
        if (node->recordCount == 0) {
            initial->id = (u8)node->valueCount;
        }
        linkWords[2] = (u32)initial;
        ((float *)linkWords)[3] = ((float *)cursor)[1];
        ((float *)linkWords)[4] = ((float *)cursor)[2];
        linkBase = linkWords + 2;
        linkCursor = linkWords + 5;

        while (linkCursor > linkBase) {
            linkCursor -= 3;
            for (;;) {
                RwCpp05Record *record = (RwCpp05Record *)linkCursor[0];
                if (record->id != 0xFF) {
                    u32 start = record->index;
                    u32 end = start + record->id;
                    u32 i;
                    for (i = start; i < end; i++) {
                        do {
                            u32 value = node->tail != NULL ? node->tail[i] : i;
                            u16 *indices;
                            u32 *a, *b, *c;
                            float a0, a1, a2, b0, b1, b2, c0, c1, c2;
                            float sx, sy, sz, rx, ry, rz;
                            float d0, d1, d2, e0, e1, e2;
                            float crossX, crossY, crossZ, det, limit, remaining;
                            float sxrel, syrel, szrel, bary0, q0, q1, q2, bary1, bary2;
                            float scale, n0, n1, n2, normalScale;
                            float normal[3];
                            value <<= 3;
                            indices = (u16 *)(baseA + value);
                            a = (u32 *)(baseB + indices[0] * 0xC);
                            b = (u32 *)(baseB + indices[2] * 0xC);
                            c = (u32 *)(baseB + indices[1] * 0xC);
                            a0 = *(float *)a;
                            a1 = *(float *)(a + 1);
                            a2 = *(float *)(a + 2);
                            b0 = *(float *)b;
                            b1 = *(float *)(b + 1);
                            b2 = *(float *)(b + 2);
                            c0 = *(float *)c;
                            c1 = *(float *)(c + 1);
                            c2 = *(float *)(c + 2);
                            sx = *(float *)input;
                            sy = *(float *)(input + 1);
                            sz = *(float *)(input + 2);
                            rx = *(float *)(input + 3) - sx;
                            ry = *(float *)(input + 4) - sy;
                            rz = *(float *)(input + 5) - sz;
                            d0 = b0 - a0;
                            d1 = b1 - a1;
                            d2 = b2 - a2;
                            e0 = c0 - a0;
                            e1 = c1 - a1;
                            e2 = c2 - a2;
                            crossX = rz * d0 - rx * d2;
                            crossY = ry * d2 - rz * d1;
                            crossZ = rx * d1 - ry * d0;
                            det = e1 * crossX + e0 * crossY + e2 * crossZ;
                            if (det == 0.00000001f) {
                                break;
                            }
                            limit = 0.00001f * -det;
                            remaining = det - limit;
                            sxrel = sx - a0;
                            syrel = sy - a1;
                            szrel = sz - a2;
                            bary0 = syrel * crossX + sxrel * crossY + szrel * crossZ;
                            if (bary0 < limit || bary0 > remaining) {
                                break;
                            }
                            q0 = szrel * e0 - sxrel * e2;
                            q1 = syrel * e2 - szrel * e1;
                            q2 = sxrel * e1 - syrel * e0;
                            bary1 = ry * q0 + rx * q1 + rz * q2;
                            if (bary1 < limit || bary0 + bary1 > remaining) {
                                break;
                            }
                            bary2 = d1 * q0 + d0 * q1 + d2 * q2;
                            if (bary2 < limit || bary2 > remaining) {
                                break;
                            }
                            scale = bary2 / det;
                            n0 = d1 * e2 - d2 * e1;
                            n1 = d2 * e0 - d0 * e2;
                            n2 = d0 * e1 - d1 * e0;
                            normalScale = fn_80482394(n0 * n0 + n1 * n1 + n2 * n2);
                            normal[0] = n0 * normalScale;
                            normal[1] = n1 * normalScale;
                            normal[2] = n2 * normalScale;
                            if (callback(input, callbackEntry, normal, user, scale) == 0) {
                                return;
                            }
                        } while (0);
                    }
                    break;
                } else {
                    u32 kind;
                    float *linkFloats;
                    float *recordFloats;
                    float f3, f2, f1, f0, f6, f5, f4, f7;
                    record = (RwCpp05Record *)(node->records + record->index * 0x10);
                    kind = record->kind & 0xC;
                    linkFloats = (float *)linkCursor;
                    recordFloats = (float *)record;
                    f3 = linkFloats[2];
                    f2 = linkFloats[1];
                    f1 = recordFloats[3];
                    f0 = recordFloats[1];
                    f6 = *(float *)((u8 *)(work + 9) + kind);
                    f5 = *(float *)((u8 *)(work + 3) + kind);
                    f4 = *(float *)((u8 *)work + kind);
                    f7 = f1 - f6;
                    f1 = f3 * f5 + f4;
                    f3 = f2 * f5 + f4;
                    f4 = f0 + f6;
                    if (f5 > 0.0f) {
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
                        }
                    }
                }
            }
        }
    }
}

/* Box query over the world's sectors and their trees. NonMatching draft. */
void fn_80441BF8(u32 *owner, u32 *input, RwCpp05EmitCallback callback, void *user) {
    u32 values[7];
    u32 *cursor;
    values[0] = input[0];
    values[1] = input[1];
    values[2] = input[2];
    values[3] = input[3];
    values[4] = input[4];
    values[5] = input[5];
    values[6] = owner[7];

    cursor = values + 7;
    while (cursor > values + 6) {
        u32 *entry;
        RwCpp05Node *node;
        u32 baseA;
        u32 baseB;
        u32 boundsA[6];
        u32 boundsB[6];
        u32 i;
        u32 linkWords[124];
        RwCpp05Record *initial;
        u32 *linkBase;
        u32 *linkCursor;
        u32 flags;
        cursor--;
        entry = (u32 *)*cursor;
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

        node = (RwCpp05Node *)((u8 *)entry + lbl_805F1420);
        if (node == NULL) {
            continue;
        }
        baseA = entry[1];
        baseB = entry[2];

        for (i = 0; i < 6; i++) {
            boundsA[i] = input[i];
            boundsB[i] = input[i];
        }
        initial = (RwCpp05Record *)linkWords;
        initial->id = 0xFF;
        initial->index = 0;
        if (node->recordCount == 0) {
            initial->id = (u8)node->valueCount;
        }
        linkWords[2] = (u32)initial;
        linkBase = linkWords + 2;
        flags = 0;

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
            for (;;) {
                RwCpp05Record *record = (RwCpp05Record *)linkCursor[0];
                u32 extra = linkCursor[1];
                if (record->id != 0xFF) {
                    RW05_EMIT_TRIANGLES(input, entry, node, baseA, baseB, record->index,
                                        record->id, extra, callback, user);
                    break;
                } else if (extra == 0) {
                    RwCpp05Record *first = (RwCpp05Record *)(node->records + record->index * 0x10);
                    RwCpp05Record *second;
                    u32 span;
                    while (first->id == 0xFF) {
                        first = (RwCpp05Record *)(node->records + first->index * 0x10);
                    }
                    second = (RwCpp05Record *)((u8 *)first + 8);
                    while (second->id == 0xFF) {
                        second = (RwCpp05Record *)(node->records + second->index * 0x10 + 8);
                    }
                    span = (u16)(second->index + second->id - first->index);
                    RW05_EMIT_TRIANGLES(input, entry, node, baseA, baseB, first->index,
                                        span, 0, callback, user);
                    break;
                } else {
                    u32 kind;
                    record = (RwCpp05Record *)(node->records + record->index * 0x10);
                    kind = record->kind & 0xC;
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
                }
            }
        }
    }
}

typedef struct RwCollisionV3d {
    float x;
    float y;
    float z;
} RwCollisionV3d;

typedef struct RwCollisionBBox {
    RwCollisionV3d sup;
    RwCollisionV3d inf;
} RwCollisionBBox;

typedef struct RwCollisionSphere {
    RwCollisionV3d center;
    float radius;
} RwCollisionSphere;

typedef struct RwCollisionLine {
    RwCollisionV3d start;
    RwCollisionV3d end;
} RwCollisionLine;

typedef struct RwCollisionIntersection {
    union {
        RwCollisionLine line;
        RwCollisionSphere sphere;
        RwCollisionBBox box;
    } t;
    s32 type;
} RwCollisionIntersection;

typedef struct RwCollisionTriangle {
    RwCollisionV3d normal;
    RwCollisionV3d point;
    s32 index;
    RwCollisionV3d *vertices[3];
} RwCollisionTriangle;

typedef struct RwCollisionRpTriangle {
    u16 vertIndex[3];
    s16 matIndex;
} RwCollisionRpTriangle;

typedef struct RwCollisionNode {
    u8 type;
    u8 count;
    u16 index;
} RwCollisionNode;

typedef struct RwCollisionSplit {
    RwCollisionNode left;
    float leftValue;
    RwCollisionNode right;
    float rightValue;
} RwCollisionSplit;

typedef struct RwCollisionTree {
    u32 flags;
    RwCollisionBBox bbox;
    u16 numTriangles;
    u16 numSplits;
    RwCollisionSplit *splits;
    u16 *map;
} RwCollisionTree;

typedef struct RwCollisionSector {
    s32 type;
} RwCollisionSector;

typedef struct RwCollisionPlaneSector {
    s32 type;
    float value;
    RwCollisionSector *leftSubTree;
    RwCollisionSector *rightSubTree;
    float leftValue;
    float rightValue;
} RwCollisionPlaneSector;

typedef struct RwCollisionWorldSector {
    s32 type;
    RwCollisionRpTriangle *triangles;
    RwCollisionV3d *vertices;
} RwCollisionWorldSector;

typedef struct RwCollisionWorld {
    u8 field_0x0[0x1C];
    RwCollisionSector *rootSector;
} RwCollisionWorld;

typedef RwCollisionTriangle *(*RwCollisionCallback)(RwCollisionIntersection *, RwCollisionWorldSector *,
                                                     RwCollisionTriangle *, float, void *);

typedef struct RwCollisionSectorWalk {
    RwCollisionBBox box;
    RwCollisionSector *stack[64];
    u32 field_0x118;
    RwCollisionSector **sp;
} RwCollisionSectorWalk;

typedef struct RwCollisionTreeEntry {
    RwCollisionNode *node;
    u32 flags;
} RwCollisionTreeEntry;

typedef struct RwCollisionTreeWalk {
    RwCollisionTree *tree;
    RwCollisionBBox box;
    RwCollisionBBox outer;
    RwCollisionNode root;
    RwCollisionTreeEntry stack[33];
    RwCollisionTreeEntry *top;
} RwCollisionTreeWalk;

/* Sphere intersection against every triangle in the world's collision trees. NonMatching draft. */
void *fn_80442210(void *worldArg, void *sphereArg, void *intersectionArg, void *callbackArg, void *data) {
    RwCollisionWorld *world = (RwCollisionWorld *)worldArg;
    RwCollisionSphere *sphere = (RwCollisionSphere *)sphereArg;
    RwCollisionIntersection *intersection = (RwCollisionIntersection *)intersectionArg;
    RwCollisionCallback callback = (RwCollisionCallback)callbackArg;
    const float radius = sphere->radius;
    const float inverseRadius = 1.0f / radius;
    const float halfRadius = 0.57f * radius;
    RwCollisionBBox box;
    RwCollisionBBox outer;
    RwCollisionSectorWalk sectors;

    box.sup.x = sphere->center.x + radius;
    box.sup.y = sphere->center.y + radius;
    box.sup.z = sphere->center.z + radius;
    box.inf.x = sphere->center.x - radius;
    box.inf.y = sphere->center.y - radius;
    box.inf.z = sphere->center.z - radius;

    outer.sup.x = sphere->center.x + halfRadius;
    outer.sup.y = sphere->center.y + halfRadius;
    outer.sup.z = sphere->center.z + halfRadius;
    outer.inf.x = sphere->center.x - halfRadius;
    outer.inf.y = sphere->center.y - halfRadius;
    outer.inf.z = sphere->center.z - halfRadius;

    sectors.box = box;
    sectors.stack[0] = world->rootSector;
    sectors.sp = &sectors.stack[1];

    while (sectors.sp > sectors.stack) {
        RwCollisionWorldSector *sector;
        RwCollisionTree *tree;
        float dx;
        float dy;
        float dz;
        float distance2;
        RwCollisionTreeWalk walk;
        sectors.sp--;
        while ((*sectors.sp)->type >= 0) {
            RwCollisionPlaneSector *plane = (RwCollisionPlaneSector *)*sectors.sp;
            u32 axis = (u32)plane->type;
            if (plane->rightValue > *(float *)((u8 *)&sectors.box.sup + axis)) {
                *sectors.sp = plane->leftSubTree;
            } else {
                *sectors.sp = plane->rightSubTree;
                if (plane->leftValue >= *(float *)((u8 *)&sectors.box.inf + axis)) {
                    sectors.sp++;
                    *sectors.sp = plane->leftSubTree;
                }
            }
        }

        sector = (RwCollisionWorldSector *)*sectors.sp;
        tree = *(RwCollisionTree **)((u8 *)sector + lbl_805F1420);
        if (tree == NULL) continue;

        if (sphere->center.x < tree->bbox.inf.x) {
            dx = tree->bbox.inf.x - sphere->center.x;
        } else if (sphere->center.x > tree->bbox.sup.x) {
            dx = sphere->center.x - tree->bbox.sup.x;
        } else {
            dx = 0.0f;
        }
        if (sphere->center.y < tree->bbox.inf.y) {
            dy = tree->bbox.inf.y - sphere->center.y;
        } else if (sphere->center.y > tree->bbox.sup.y) {
            dy = sphere->center.y - tree->bbox.sup.y;
        } else {
            dy = 0.0f;
        }
        if (sphere->center.z < tree->bbox.inf.z) {
            dz = tree->bbox.inf.z - sphere->center.z;
        } else if (sphere->center.z > tree->bbox.sup.z) {
            dz = sphere->center.z - tree->bbox.sup.z;
        } else {
            dz = 0.0f;
        }
        distance2 = dx * dx;
        distance2 += dy * dy;
        distance2 += dz * dz;
        if (distance2 > radius * radius) continue;
        if (tree->bbox.inf.x > box.sup.x || tree->bbox.inf.y > box.sup.y || tree->bbox.inf.z > box.sup.z ||
            tree->bbox.sup.x < box.inf.x || tree->bbox.sup.y < box.inf.y || tree->bbox.sup.z < box.inf.z) {
            continue;
        }

        walk.tree = tree;
        walk.box = box;
        walk.outer = outer;
        walk.root.index = 0;
        walk.root.count = 0xFF;
        if (tree->numSplits == 0) walk.root.count = (u8)tree->numTriangles;
        walk.stack[0].node = &walk.root;
        walk.stack[0].flags = (tree->bbox.inf.y < outer.inf.y ? 4 : 0) |
                              (tree->bbox.inf.x < outer.inf.x ? 1 : 0) |
                              (tree->bbox.inf.z < outer.inf.z ? 0x10 : 0) |
                              (tree->bbox.sup.x > outer.sup.x ? 2 : 0) |
                              (tree->bbox.sup.y > outer.sup.y ? 8 : 0) |
                              (tree->bbox.sup.z > outer.sup.z ? 0x20 : 0);
        walk.top = &walk.stack[1];

        while (walk.top > walk.stack) {
            u16 start = 0;
            u16 count = 0;
            u32 leafFlags = 0;
            s32 end;
            u16 i;
            walk.top--;
            for (;;) {
                RwCollisionTreeEntry *entry = walk.top;
                RwCollisionNode *node = entry->node;
                RwCollisionSplit *split;
                u32 axis;
                if (node->count != 0xFF) {
                    start = node->index;
                    count = node->count;
                    leafFlags = entry->flags;
                    break;
                }
                if (entry->flags == 0) {
                    RwCollisionSplit *first;
                    RwCollisionNode *last;
                    split = &tree->splits[node->index];
                    first = split;
                    while (first->left.count == 0xFF) first = &tree->splits[first->left.index];
                    last = &split->right;
                    while (last->count == 0xFF) last = &tree->splits[last->index].right;
                    start = first->left.index;
                    count = (u16)(last->index + last->count - first->left.index);
                    leafFlags = 0;
                    break;
                }
                split = &tree->splits[node->index];
                axis = split->left.type & 0xC;
                if (split->rightValue > *(float *)((u8 *)&walk.box.sup + axis)) {
                    entry->node = &split->left;
                    continue;
                }
                entry->node = &split->right;
                if (split->leftValue >= *(float *)((u8 *)&walk.box.inf + axis)) {
                    walk.top++;
                    walk.top->node = &split->left;
                    walk.top->flags = walk.top[-1].flags;
                    if (split->leftValue <= *(float *)((u8 *)&walk.outer.sup + axis)) {
                        walk.top->flags &= ~(2 << (axis >> 1));
                    }
                    if (split->rightValue >= *(float *)((u8 *)&walk.outer.inf + axis)) {
                        walk.top[-1].flags &= ~(1 << (axis >> 1));
                    }
                }
            }

            end = (s32)start + (s32)count;
            for (i = start; (s32)i < end; i++) {
                u16 triangleIndex = tree->map != NULL ? tree->map[i] : i;
                RwCollisionRpTriangle *triangle = &sector->triangles[triangleIndex];
                RwCollisionV3d *v0 = &sector->vertices[triangle->vertIndex[0]];
                RwCollisionV3d *v1 = &sector->vertices[triangle->vertIndex[1]];
                RwCollisionV3d *v2 = &sector->vertices[triangle->vertIndex[2]];
                RwCollisionTriangle collision;
                float distance;
                if (leafFlags != 0) {
                    if (fn_8047B720(sphere, v0, v1, v2, &collision.normal, &distance) == 0) continue;
                } else {
                    float ax = v1->x - v0->x;
                    float ay = v1->y - v0->y;
                    float az = v1->z - v0->z;
                    float bx = v2->x - v0->x;
                    float by = v2->y - v0->y;
                    float bz = v2->z - v0->z;
                    float scale;
                    collision.normal.x = ay * bz - az * by;
                    collision.normal.y = az * bx - ax * bz;
                    collision.normal.z = ax * by - ay * bx;
                    scale = fn_80482394(collision.normal.x * collision.normal.x +
                                        collision.normal.y * collision.normal.y +
                                        collision.normal.z * collision.normal.z);
                    collision.normal.x *= scale;
                    collision.normal.y *= scale;
                    collision.normal.z *= scale;
                    distance = (sphere->center.x - v0->x) * collision.normal.x +
                               (sphere->center.y - v0->y) * collision.normal.y +
                               (sphere->center.z - v0->z) * collision.normal.z;
                }
                distance *= inverseRadius;
                collision.point = *v0;
                collision.index = triangleIndex;
                collision.vertices[0] = v0;
                collision.vertices[1] = v1;
                collision.vertices[2] = v2;
                if (callback(intersection, sector, &collision, distance, data) == NULL) return NULL;
            }
        }
    }
    return world;
}

/* Calls back for every collision triangle of the world the intersection (type at 0x18: 1 line,
 * 3 sphere, 4 box, 5 an atomic's bounding sphere) touches. */
void *fn_80442AA4(void *world, u32 *intersection, void *callback, void *data) {
    switch (intersection[6]) {
    case 1:
        fn_80441188(world, intersection, callback, data);
        return world;
    case 3:
        fn_80442210(world, intersection, intersection, callback, data);
        return world;
    case 4:
        fn_80441BF8(world, intersection, callback, data);
        return world;
    case 5: {
        R1Sphere sphere = *fn_804566D0((void *)intersection[0]);
        fn_80442210(world, &sphere, intersection, callback, data);
        return world;
    }
    }
    return world;
}

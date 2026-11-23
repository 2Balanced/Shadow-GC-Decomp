/* RenderWare, 0x80446A04..0x804485BC: plugin 0x11E (frame hierarchies, what the game's units call
 * RpHAnimHierarchy: attach, create/destroy, the frame plugin and stream callbacks) and plugin 0x12F
 * (the particle tank's lock and callbacks, RpPTankLockStruct in Game/EffectParticleRW3.cpp).
 * Names are inferred from behavior. 22 of 27 functions match; left: fn_804472AC (6 rows: the key
 * frame size and the new hierarchy do not share r31), fn_804474D8 (5 rows: the documented
 * later-compiler floor, the zeroed counter copied into the strength-reduced offset), and the
 * drafts fn_804476B8, fn_80447AF8, fn_804480C8. */
#include <string.h>
#include <dolphin/types.h>
#include "Game/rw/r1.h"

extern s32 lbl_805F1440;
extern u32 lbl_805F1448;
extern u8 *lbl_805F15B4;

extern "C" {
void fn_80445CF4(float *, const float *);
void fn_8044621C(float *, float *, float *, float);
void fn_80445E08(float *, float *, float *, float);
void fn_804468D0(float *, const float *, const float *);
void fn_80446790(float *, const float *);
void *fn_80446620(void *, u32 *);
u32 fn_804466DC(void *, u32 *);
u32 fn_80446780(u32 *);
void fn_8048722C(void *);
void fn_8048752C(void *, void *, void *);
int fn_8044758C(void *);

/* Frame callback: points the hierarchy's nodes whose ID is the frame's at the frame, then recurses
 * into the frame's children. */
void *fn_80446A04(void *frame, R1Hierarchy *hierarchy) {
    for (int i = 0; i < hierarchy->m_nodeCount; i++) {
        if (hierarchy->m_nodes[i].m_id == fn_8044758C(frame)) {
            hierarchy->m_nodes[i].m_frame = frame;
        }
    }
    fn_8048722C(frame);
    fn_8048752C(frame, (void *)fn_80446A04, hierarchy);
    return frame;
}

extern u32 lbl_805EDD70;
extern u32 lbl_805EDD74;
extern u8 lbl_805E1510[]; // the free list (in .bss) fn_80446AA8 creates
void *fn_8047FC04(u32, u32, u32, u32, void *, u32);
void fn_80477344(void *);

void *fn_80446AA8(u32 *object) {
    void *result = fn_8047FC04(0x24, lbl_805EDD70, 4, lbl_805EDD74, lbl_805E1510, 0x4011E);
    ((u32 *)&lbl_805F1440)[1] = (u32)result;
    if (result == NULL) object = NULL;
    u32 callbacks[12];
    callbacks[0] = 1;
    callbacks[1] = 0x24;
    callbacks[2] = 0x24;
    callbacks[3] = (u32)fn_80445CF4;
    callbacks[4] = (u32)fn_8044621C;
    callbacks[5] = (u32)fn_80445E08;
    callbacks[6] = (u32)fn_804468D0;
    callbacks[7] = (u32)fn_80446790;
    callbacks[8] = (u32)fn_80446620;
    callbacks[9] = (u32)fn_804466DC;
    callbacks[10] = (u32)fn_80446780;
    callbacks[11] = 0;
    fn_80477344(callbacks);
    return object;
}

void fn_8047FCDC(void *);

void *fn_80446B90(u32 *object) {
    if (((u32 *)&lbl_805F1440)[1] != 0) {
        fn_8047FCDC((void *)((u32 *)&lbl_805F1440)[1]);
        ((u32 *)&lbl_805F1440)[1] = 0;
    }
    return object;
}

int fn_80447418(R1Hierarchy *);
R1Hierarchy *fn_804472AC(int, int *, int *, int, int);
u32 fn_8047D458(void *, void *, u32);

/* Plugin 0x11E's frame destructor: detaches the frame from its hierarchy. */
void *fn_80446BFC(void *frame, s32 offset, s32 size) {
    R1FrameHierarchyData *data = (R1FrameHierarchyData *)((u8 *)frame + lbl_805F1440);
    if (data->m_hierarchy != NULL) {
        R1Hierarchy *hierarchy = data->m_hierarchy;
        for (int i = 0; i < hierarchy->m_nodeCount; i++) {
            hierarchy->m_nodes[i].m_frame = NULL;
        }
        if (hierarchy->m_parentFrame == frame) {
            fn_80447418(hierarchy);
        }
        data->m_hierarchy = NULL;
    }
    data->m_id = -1;
    return frame;
}

/* Plugin 0x11E's frame copy callback: copies the node ID and, for a hierarchy root, the
 * hierarchy. */
void *fn_80446C94(void *dstFrame, const void *srcFrame, s32 offset, s32 size) {
    R1Hierarchy *srcHierarchy;
    R1FrameHierarchyData *dst;
    const R1FrameHierarchyData *src = (const R1FrameHierarchyData *)((const u8 *)srcFrame + lbl_805F1440);
    dst = (R1FrameHierarchyData *)((u8 *)dstFrame + lbl_805F1440);
    srcHierarchy = src->m_hierarchy;
    dst->m_id = src->m_id;
    if (srcHierarchy != NULL && (srcHierarchy->m_flags & 1) == 0) {
        R1Hierarchy *hierarchy = fn_804472AC(srcHierarchy->m_nodeCount, NULL, NULL, srcHierarchy->m_flags,
                                             *(int *)((u8 *)srcHierarchy->m_interpolator + 0x20));
        for (int i = 0; i < hierarchy->m_nodeCount; i++) {
            hierarchy->m_nodes[i].m_frame = NULL;
            hierarchy->m_nodes[i].m_flags = srcHierarchy->m_nodes[i].m_flags;
            hierarchy->m_nodes[i].m_index = srcHierarchy->m_nodes[i].m_index;
            hierarchy->m_nodes[i].m_id = srcHierarchy->m_nodes[i].m_id;
        }
        dst->m_hierarchy = hierarchy;
        hierarchy->m_parentFrame = dstFrame;
    }
    return dstFrame;
}

/* Plugin 0x11E's frame stream write callback: the frame's node ID and, for the root of a
 * hierarchy, the hierarchy's nodes. */
void *fn_80446D84(void *stream, s32 length, const void *frame, s32 offset, s32 size) {
    R1FrameHierarchyData *data;
    int i;
    R1HierarchyNode *node;
    R1Hierarchy *hierarchy;
    u32 version = 0x100;
    if (fn_8047D458(stream, &version, 4) == 0) {
        return NULL;
    }
    data = (R1FrameHierarchyData *)((const u8 *)frame + lbl_805F1440);
    if (fn_8047D458(stream, &data->m_id, 4) == 0) {
        return NULL;
    }
    hierarchy = data->m_hierarchy;
    if (hierarchy != NULL && (hierarchy->m_flags & 1) == 0) {
        if (fn_8047D458(stream, &hierarchy->m_nodeCount, 4) == 0) {
            return NULL;
        }
        if (fn_8047D458(stream, &hierarchy->m_flags, 4) == 0) {
            return NULL;
        }
        if (fn_8047D458(stream, (u8 *)hierarchy->m_interpolator + 0x20, 4) == 0) {
            return NULL;
        }
        node = hierarchy->m_nodes;
        for (i = 0; i < hierarchy->m_nodeCount; i++) {
            if (fn_8047D458(stream, &node->m_id, 4) == 0) {
                return NULL;
            }
            if (fn_8047D458(stream, &node->m_index, 4) == 0) {
                return NULL;
            }
            if (fn_8047D458(stream, &node->m_flags, 4) == 0) {
                return NULL;
            }
            node++;
        }
    } else {
        u32 none = 0;
        if (fn_8047D458(stream, &none, 4) == 0) {
            return NULL;
        }
    }
    return stream;
}

u32 fn_8047D868(void *, void *, u32);
void *fn_804779DC(int, int);

/* Plugin 0x11E's frame stream read callback. */
void *fn_80446F40(void *stream, s32 length, void *frame, s32 offset, s32 size) {
    int nodeCount;
    s32 version;
    int flags;
    int keyFrameSize;
    R1FrameHierarchyData *data = (R1FrameHierarchyData *)((u8 *)frame + lbl_805F1440);
    if (fn_8047D868(stream, &version, 4) == 0) {
        return NULL;
    }
    if (version != 0x100) {
        return NULL;
    }
    if (fn_8047D868(stream, &data->m_id, 4) == 0) {
        return NULL;
    }
    if (fn_8047D868(stream, &nodeCount, 4) == 0) {
        return NULL;
    }
    if (nodeCount > 0) {
        if (fn_8047D868(stream, &flags, 4) == 0) {
            return NULL;
        }
        if (fn_8047D868(stream, &keyFrameSize, 4) == 0) {
            return NULL;
        }
        R1Hierarchy *hierarchy =
            ((R1Hierarchy *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))((&lbl_805F1440)[1], 0x3011E);
        memset(hierarchy, 0, sizeof(R1Hierarchy));
        if (hierarchy != NULL) {
            hierarchy->m_interpolator = fn_804779DC(nodeCount, keyFrameSize);
            hierarchy->m_flags = flags;
            hierarchy->m_parentFrame = frame;
            hierarchy->m_nodeCount = nodeCount;
            hierarchy->m_parentHierarchy = hierarchy;
            if ((hierarchy->m_flags & 2) != 0) {
                hierarchy->m_matrices = NULL;
                hierarchy->m_matricesUnaligned = NULL;
            } else {
                void *memory =
                    ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))((nodeCount << 6) + 0xF, 0x3011E);
                hierarchy->m_matrices = (void *)(((u32)memory + 0xF) & ~0xF);
                hierarchy->m_matricesUnaligned = memory;
            }
            hierarchy->m_nodes =
                (R1HierarchyNode *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(nodeCount << 4, 0x3011E);
            int i;
            R1HierarchyNode *node = hierarchy->m_nodes;
            for (i = 0; i < hierarchy->m_nodeCount; i++) {
                if (fn_8047D868(stream, &node->m_id, 4) == 0) {
                    return NULL;
                }
                if (fn_8047D868(stream, &node->m_index, 4) == 0) {
                    return NULL;
                }
                if (fn_8047D868(stream, &node->m_flags, 4) == 0) {
                    return NULL;
                }
                node->m_frame = NULL;
                node++;
            }
        }
        data->m_hierarchy = hierarchy;
    }
    return stream;
}

/* Plugin 0x11E's frame stream size callback. */
s32 fn_8044719C(const void *frame, s32 offset, s32 size) {
    const R1FrameHierarchyData *data = (const R1FrameHierarchyData *)((const u8 *)frame + lbl_805F1440);
    int used = 0;
    if (data->m_id != -1 || data->m_hierarchy != NULL) {
        used = 1;
    }
    if (used) {
        s32 result = 0xC;
        R1Hierarchy *hierarchy = data->m_hierarchy;
        if (hierarchy != NULL && (hierarchy->m_flags & 1) == 0) {
            result = hierarchy->m_nodeCount * 0xC + 0x14;
        }
        return result;
    }
    return 0;
}

typedef void (*RwCpp09Callback)(void);
u32 *fn_80446BE0(u32 *);
s32 fn_80485D78(u32, u32, RwCpp09Callback, RwCpp09Callback);
s32 fn_804878D4(u32, u32, RwCpp09Callback, RwCpp09Callback, RwCpp09Callback);
s32 fn_804836EC(u32, RwCpp09Callback, RwCpp09Callback, RwCpp09Callback);

u32 fn_80447200(void) {
    s32 result = fn_80485D78(0, 0x11E, (RwCpp09Callback)fn_80446AA8,
                             (RwCpp09Callback)fn_80446B90);
    if (result < 0) return 0;
    result = fn_804878D4(8, 0x11E, (RwCpp09Callback)fn_80446BE0,
                         (RwCpp09Callback)fn_80446BFC, (RwCpp09Callback)fn_80446C94);
    lbl_805F1440 = result;
    s32 status = fn_804836EC(0x11E, (RwCpp09Callback)fn_80446F40, (RwCpp09Callback)fn_80446D84,
                             (RwCpp09Callback)fn_8044719C);
    int success = 0;
    if (status >= 0 && lbl_805F1440 >= 0) {
        success = 1;
    }
    return success;
}

u32 fn_80477A8C(void *);

/* Creates a hierarchy of `count` nodes from the hierarchy free list. */
R1Hierarchy *fn_804472AC(int count, int *flagsTable, int *ids, int flags, int keyFrameSize) {
    R1Hierarchy *hierarchy =
        ((R1Hierarchy *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x118))((&lbl_805F1440)[1], 0x3011E);
    hierarchy->m_interpolator = fn_804779DC(count, keyFrameSize);
    hierarchy->m_flags = flags;
    hierarchy->m_nodeCount = count;
    hierarchy->m_parentFrame = NULL;
    if ((flags & 2) == 0) {
        void *memory = ((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))((count << 6) + 0xF, 0x3011E);
        hierarchy->m_matrices = (void *)(((u32)memory + 0xF) & ~0xF);
        hierarchy->m_matricesUnaligned = memory;
    } else {
        hierarchy->m_matrices = NULL;
        hierarchy->m_matricesUnaligned = NULL;
    }
    hierarchy->m_nodes =
        (R1HierarchyNode *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(count << 4, 0x3011E);
    for (int i = 0; i < count; i++) {
        hierarchy->m_nodes[i].m_frame = NULL;
        if (ids != NULL) {
            hierarchy->m_nodes[i].m_id = ids[i];
        }
        hierarchy->m_nodes[i].m_index = i;
        if (flagsTable != NULL) {
            hierarchy->m_nodes[i].m_flags = flagsTable[i];
        }
    }
    hierarchy->m_parentHierarchy = hierarchy;
    return hierarchy;
}

/* Destroys a hierarchy: its node and matrix arrays unless it shares them, its interpolator, the
 * hierarchy itself (back to the free list), and the parent frame's link to it. */
int fn_80447418(R1Hierarchy *hierarchy) {
    void *parentFrame = hierarchy->m_parentFrame;
    if ((hierarchy->m_flags & 1) == 0) {
        if ((hierarchy->m_flags & 2) == 0) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(hierarchy->m_matricesUnaligned);
        }
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(hierarchy->m_nodes);
    }
    hierarchy->m_matricesUnaligned = NULL;
    hierarchy->m_matrices = NULL;
    hierarchy->m_nodes = NULL;
    fn_80477A8C(hierarchy->m_interpolator);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x11C))((&lbl_805F1440)[1], hierarchy);
    if (parentFrame != NULL) {
        ((R1FrameHierarchyData *)((u8 *)parentFrame + lbl_805F1440))->m_hierarchy = NULL;
    }
    return 0;
}

/* Attaches the hierarchy to its parent frame's tree: points every node at the frame with its ID. */
R1Hierarchy *fn_804474D8(R1Hierarchy *hierarchy) {
    void *frame;
    int i;
    frame = hierarchy->m_parentFrame;
    for (i = 0; i < hierarchy->m_nodeCount; i++) {
        if (hierarchy->m_nodes[i].m_id == fn_8044758C(frame)) {
            hierarchy->m_nodes[i].m_frame = frame;
        }
    }
    fn_8048722C(frame);
    fn_8048752C(frame, (void *)fn_80446A04, hierarchy);
    return hierarchy;
}

void fn_8045B270(u32, u32);

/* Plugin 0x12F's destructor: frees the object's plugin data. */
void *fn_804475A8(void *object, s32 offset, s32 size) {
    if (*(u32 **)((u8 *)object + lbl_805F1448) != NULL) {
        fn_8045B270(**(u32 **)(*(u8 **)((u8 *)object + 0x18) + 0x20), 0);
        if ((*(u32 **)((u8 *)object + lbl_805F1448))[3] != 0) {
            ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(
                (void *)(*(u32 **)((u8 *)object + lbl_805F1448))[3]);
        }
        ((void (*)(void *))*(u32 *)(lbl_805F15B4 + 0x10C))(*(u32 **)((u8 *)object + lbl_805F1448));
        *(u32 **)((u8 *)object + lbl_805F1448) = NULL;
    }
    return object;
}

extern u32 lbl_805F144C;
void fn_804485BC(void);
void fn_80448610(void);
void fn_80447598(u8 *);
void *fn_804475A8(void *, s32, s32);
s32 fn_80458B6C(u32, u32, RwCpp09Callback, RwCpp09Callback, void *);

u32 fn_80447640(void) {
    s32 result = fn_80485D78(4, 0x12F, (RwCpp09Callback)fn_804485BC,
                             (RwCpp09Callback)fn_80448610);
    lbl_805F144C = result;
    if (result >= 0) {
        lbl_805F1448 = fn_80458B6C(4, 0x12F, (RwCpp09Callback)fn_80447598,
                                   (RwCpp09Callback)fn_804475A8, NULL);
        return ((u32)lbl_805F1448 >> 31) ^ 1;
    }
    return 0;
}

u32 fn_80447EB4(u32 flags) {
    int bit0 = flags & 1;
    int bit2 = (flags >> 2) & 1;
    int bit3 = (flags >> 3) & 1;
    int bit5 = (flags >> 5) & 1;
    int bit6 = (flags >> 6) & 1;
    int bit7 = (flags >> 7) & 1;
    int bit8 = (flags >> 8) & 1;
    int bit15 = (flags >> 15) & 1;
    int bit17 = (flags >> 17) & 1;
    int bit18 = (flags >> 18) & 1;
    int bit19 = (flags >> 19) & 1;
    int bit20 = (flags >> 20) & 1;

    if (bit3 == 1 && bit15 == 1) {
        flags &= 0x8000;
    }
    if (bit5 == 1 && bit17 == 1) {
        flags &= 0x20000;
    }
    if (bit6 == 1 && bit18 == 1) {
        flags &= 0x40000;
    }
    if (bit7 == 1 && bit19 == 1) {
        flags &= 0x80000;
    }
    if (bit8 == 1 && bit20 == 1) {
        flags &= 0x100000;
    }
    if (bit3 == 1 && bit0 == 1) {
        flags &= ~1;
    }
    if (bit3 == 1 && bit2 == 1) {
        flags &= ~4;
    }
    if (bit3 == 1 && bit5 == 1) {
        flags &= ~0x20;
    }
    if (bit3 == 0 && bit0 == 0) {
        flags |= 1;
    }
    if ((flags & 0x30000000) == 0) {
        flags |= 0x20000000;
    }
    return flags;
}

/* Runs plugin 0x12F's callbacks for the object: the first (then clears its argument), the
 * second, and the third when the second returned 1 (or there is no second). */
void *fn_80447FC0(void *object) {
    int result = 1;
    u32 *data = *(u32 **)((u8 *)object + lbl_805F1448);
    if ((s32)data[1] > 0) {
        if (data[7] != 0) {
            ((void (*)(void *, void *, u32, u32))data[7])(object, data + 0x12, data[1], data[0x10]);
            data[0x10] = 0;
        }
        if (data[8] != 0) {
            result = ((int (*)(void *, void *, u32))data[8])(object, data + 0x12, data[1]);
        }
        if (result == 1) {
            ((void (*)(void *))data[4])(object);
        }
    }
    return object;
}

extern u8 lbl_8056D6F8[];
void *fn_804480C8(void *, u32, void *, void *);

extern u32 lbl_8051BCCC;
extern const float lbl_805FB050;
extern const float lbl_805FB054;
extern const float lbl_805FB058;
extern const float lbl_805FB05C;
extern const float lbl_805FB060;
extern u32 lbl_805FB550;
extern u32 lbl_805FB554;
s32 fn_80456DD8(void);
void fn_80456FD8(void);
void fn_80455EB4(void);
u32 fn_804476B8(u32 *, u32, void *);
u32 fn_80447AF8(u32 *, u32, void *);
void fn_8045B270(u32, u32);

void *fn_80448074(u32 *object, u32 value, void *argument) {
    return fn_804480C8(object, fn_80447EB4(value), argument, lbl_8056D6F8);
}

extern u32 lbl_8051BCA8;

static inline u32 RwCpp09Process(u32 *object, u32 flags, int multiply) {
    static const u16 indices[9] = {0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D, 0x1F, 0x21, 0x23};
    static const u16 sizes[9] = {0xC, 0x40, 0xC, 8, 4, 0x10, 4, 0x10, 0x20};
    static const u16 bits[9] = {1, 8, 0x10, 4, 2, 0x40, 0x20, 0x80, 0x100};
    u32 *table = (u32 *)lbl_8051BCA8;
    u32 result = 0;
    object[0x28] &= ~0x1FF;
    object[0x26] = 0;
    object[0x27] = 0;
    if (multiply) object[0xF] = 0;

    for (u32 i = 0; i < 9; i++) {
        if ((flags & bits[i]) == 0) continue;
        object[0x26]++;
        object[0x28] |= bits[i];
        if (i == 0) {
            if (multiply) result = object[0] * table[0];
        } else {
            result += multiply ? object[0] * table[i] : table[i];
        }
    }

    if (!multiply) {
        object[0x27] = (result + 3) & ~3;
        result = object[0] * object[0x27];
    }
    u32 *raw = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(result + 0x10, 0x3012F);
    if (raw == NULL) return 0;
    u32 base = ((u32)raw + 0xF) & ~0xF;
    for (u32 i = 0; i < 9; i++) {
        if ((flags & bits[i]) == 0) continue;
        object[indices[i]] = base + result;
        object[indices[i] + 1] = sizes[i];
        if (i != 8) result += multiply ? object[0] * table[i] : table[i];
    }
    object[0x3] = (u32)raw;
    return 1;
}

u32 fn_804476B8(u32 *object, u32 flags, void *argument) {
    return RwCpp09Process(object, flags, 1);
}

u32 fn_80447AF8(u32 *object, u32 flags, void *argument) {
    return RwCpp09Process(object, flags, 0);
}

void *fn_804480C8(void *parent, u32 flags, void *argument, void *config) {
    if (fn_80456DD8() == 0) {
        fn_80456FD8();
        return NULL;
    }
    u32 *object = (u32 *)((void *(*)(u32, u32))*(u32 *)(lbl_805F15B4 + 0x108))(0x138, 0x3012F);
    if (object == NULL) {
        fn_80456FD8();
        return NULL;
    }

    u32 *owner = (u32 *)parent;
    object[4] = owner[0x12];
    owner[0x12] = (u32)fn_80447FC0;
    if (owner[0x12] == 0) owner[0x12] = (u32)fn_80455EB4;
    object[0] = (u32)parent;
    object[1] = 0;
    object[0x28] = flags;
    object[0x11] = (u32)argument;
    object[0x10] = 0xFFFFFFFF;
    object[0xF] = 0;
    object[3] = 0;
    *(u32 **)((u8 *)owner + lbl_805F1448) = object;
    memcpy(object + 5, config, 0x10);

    u32 result = 0;
    if (object[5] != 0) {
        object[3] = ((u32 (*)(void *, u32, u32, void *))object[5])(object + 0x12, object[0], flags, argument);
    } else if ((flags & 0x20000000) != 0) {
        result = fn_80447AF8(object, flags, argument);
    } else {
        result = fn_804476B8(object, flags, argument);
    }
    if (result != 1 && object[3] == 0) {
        fn_80456FD8();
        return NULL;
    }
    if (object[6] != 0 &&
        ((u32 (*)(void *, void *, u32, u32, void *))object[6])(owner, object + 0x12, object[0], flags,
                                                               argument) != 1) {
        fn_80456FD8();
        return NULL;
    }

    u32 *table = (u32 *)lbl_8051BCCC;
    u32 *ownerData = (u32 *)owner[6];
    fn_8045B270(*(u32 *)(ownerData[8] + 0), 0);
    object[0x2E] = (u32)config;
    object[0x2F] = (u32)argument;
    object[0x2C] = (u32)parent;
    object[0x2D] = result;
    *(float *)(object + 0x30) = lbl_805FB05C;
    *(float *)(object + 0x48) = lbl_805FB060;
    *(float *)(object + 0x43) = lbl_805FB060;
    *(float *)(object + 0x3E) = lbl_805FB060;
    *(float *)(object + 0x42) = lbl_805FB05C;
    *(float *)(object + 0x40) = lbl_805FB05C;
    *(float *)(object + 0x3F) = lbl_805FB05C;
    *(float *)(object + 0x47) = lbl_805FB05C;
    *(float *)(object + 0x46) = lbl_805FB05C;
    *(float *)(object + 0x44) = lbl_805FB05C;
    *(float *)(object + 0x4C) = lbl_805FB05C;
    *(float *)(object + 0x4B) = lbl_805FB05C;
    *(float *)(object + 0x4A) = lbl_805FB05C;
    object[0x41] = flags | 0x20003;
    object[0x31] = flags;
    object[0x32] = flags;
    object[0x33] = flags;
    object[0x34] = flags;
    object[0x35] = flags;
    object[0x36] = table[0];
    object[0x37] = table[1];
    object[0x38] = table[2];
    object[0x39] = table[3];
    object[0x3A] = table[4];
    object[0x3B] = table[5];
    object[0x3C] = table[6];
    object[0x3D] = table[7];
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(0xA, object + 0x29);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(0xB, object + 0x2A);
    ((void (*)(u32, void *))*(u32 *)(lbl_805F15B4 + 0x24))(0xC, object + 0x2B);
    return owner;
}

/* A lock of one of the object's arrays: data and stride. Copied as C copies it (see R1BBoxBits). */
struct R1Pair {
    u32 m_first;
    u32 m_second;
};

struct R1PairBits {
    u32 w[2];
};

/* Copies the object's (plugin 0x12F) pair for `type` (one bit) to `out`; with bit 30 of `flags`
 * set, a present pair also marks the type in the object's flags. 0 when the type is not set. */
int fn_80448408(void *object, R1Pair *out, u32 type, u32 flags) {
    u32 *data = *(u32 **)((u8 *)object + lbl_805F1448);
    if ((data[0x28] & type) == 0) {
        out->m_first = 0;
        out->m_second = 0;
        return 0;
    }
    switch (type) {
    case 1:
        *(R1PairBits *)out = *(R1PairBits *)(data + 0x13);
        break;
    case 2:
        *(R1PairBits *)out = *(R1PairBits *)(data + 0x1B);
        break;
    case 4:
        *(R1PairBits *)out = *(R1PairBits *)(data + 0x19);
        break;
    case 8:
        *(R1PairBits *)out = *(R1PairBits *)(data + 0x15);
        break;
    case 0x10:
        *(R1PairBits *)out = *(R1PairBits *)(data + 0x17);
        break;
    case 0x20:
        *(R1PairBits *)out = *(R1PairBits *)(data + 0x1F);
        break;
    case 0x40:
        *(R1PairBits *)out = *(R1PairBits *)(data + 0x1D);
        break;
    case 0x80:
        *(R1PairBits *)out = *(R1PairBits *)(data + 0x21);
        break;
    case 0x100:
        *(R1PairBits *)out = *(R1PairBits *)(data + 0x23);
        break;
    }
    if ((s32)data[0x27] != 0) {
        out->m_second = data[0x27];
    }
    if (out->m_first != 0 && (flags & 0x40000000) == 0x40000000) {
        (*(u32 **)((u8 *)object + lbl_805F1448))[0xF] |= flags | type;
    }
    return 1;
}

/* The frame's node ID (the first word of plugin 0x11E's frame data). */
int fn_8044758C(void *frame) {
    return *(int *)((u8 *)frame + lbl_805F1440);
}

u32 fn_8044757C(u8 *object) {
    return *(u32 *)(object + lbl_805F1440 + 4);
}

void fn_80447598(u8 *base) {
    *(u32 *)(base + lbl_805F1448) = 0;
}

u32 *fn_80446BE0(u32 *object) {
    u32 *target = (u32 *)((u8 *)object + lbl_805F1440);
    target[1] = 0;
    target[0] = 0xFFFFFFFF;
    return object;
}

void fn_80456FD8(void);

void fn_804483E8(void) {
    fn_80456FD8();
}

void *fn_80448598(void *object) {
    u32 *data = *(u32 **)((u8 *)object + lbl_805F1448);
    data[0x10] |= data[0x0F];
    data[0x0F] = 0;
    return object;
}
}

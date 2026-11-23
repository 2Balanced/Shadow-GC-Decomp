/* The motion player (no class with RTTI; file name and all names inferred, after its messages
 * "Cannot find bone data.", "Illegal bonetype." and "Cannot get PlayWork."). A play work holds
 * the root transform of a model, its nodes, and a set of motion layers ("bones" in this file:
 * 0x130-byte records with their own frame, speed and blend state). Layers are addressed by slot
 * through a slot map, and a second list keeps them in blend order. The unit was built with C++
 * exceptions on (its functions have extab entries); definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 * Several tree walkers (fn_8041966C, fn_8041AF1C, fn_8041C5A8, fn_8041CC18 to fn_8041D448) are
 * recursive functions that retail inlines into themselves one level deep; they are written here as
 * the function plus an inline copy of its body that calls the function.
 * Levers that matched functions here:
 * - one counter per loop where a loop keeps its counter across calls (a counter shared by several
 *   loops is split into ranges that are allocated before every declared local), and the
 *   declaration order of the locals that live in saved registers (first declared, highest register);
 * - "for (i = 0, again = 0; ...)" where retail loads the second zero after the counter;
 * - the per-layer entries of a blended channel taken as a flat array of pointers, "k = index * 2",
 *   the key data from entries[k] and the blend from entries[++k] (retail computes (k + 1) * 4);
 * - the inline Lerp (weight register before the complement), an inline accessor whose int result
 *   is a value of its own (LayerCount), an inline call that takes its arguments into registers
 *   before the call is set up (CopyMatrix), and a parameter of another type than the local it is
 *   copied to (s32 layers, int count), which keeps the copy;
 * - a running pointer kept in separate locals instead of "p +=", sizeof terms in retail's order,
 *   "x -= y" for a reload of x, a struct assignment for a reloading float copy and a word copy
 *   (MotionFrameWords) where retail moves the frame record through integer registers;
 * - "case 0:" sharing the default label, REMOVE_SLOT_ONE where the slot map is closed up by the
 *   counter that found the slot, and a difference written out where a float local would be
 *   given its register after the temporaries (fn_8041AB3C).
 * The functions that reach two or more of the .bss work areas address them from one base, which
 * the compiler names "...bss.0"; objdiff reports those at 99% against retail's lbl_805E12E0 although
 * every instruction and relocation target is retail's.
 * Wave 40 matched seven more: fn_80418044 (copy propagation off and the position flag declared
 * first), fn_8041CC18, fn_8041CD6C, fn_8041CE74 and fn_8041D31C (the model is a pointer to const,
 * so GC/2.7 schedules its first load early, as retail does), and fn_80415EA4 (its loop stands in
 * the inline AdvanceLayers) and fn_80420590 (its matrices are pointers to non-const);
 * probes/Game_MotionPlayWork.py.
 * Open (10 of 100 functions):
 * - fn_8041DBD4 and fn_8041ED8C (the blended rotation channels), fn_8041BE30, fn_8041A83C and
 *   fn_80416198 have retail's instructions with other saved registers; no declaration order
 *   reaches retail's (for fn_80416198 not with copy propagation off either);
 * - fn_8041CFB0 (const model: 3 rows left) forms the weight scale's address with the base and the
 *   scaled index in each other's registers; fn_8041FAD8 and fn_8041FB4C (the entry array and the
 *   list trade r4 and r5; the entry in one expression, through inline functions, the list in a
 *   local and const pointers do not change that);
 * - fn_804165D8 and fn_804175C8 still differ in structure. */
#include <dolphin/types.h>
#include <cmath>
#include <string.h>
#include "Game/MathTypes.h"
#include "Game/MotionHalfKeyTrack.h"
#include "Game/Quaternion.h"
#include "Game/UnkMatrix_800B1304.h"
#include "size_assert.h"

/* The blend state of a layer. */
struct MotionBlend {
    /* 0x00 */ u32 m_flags;
    /* 0x04 */ s32 m_mode;
    /* 0x08 */ s32 m_type;
    /* 0x0C */ float m_weight;
    /* 0x10 */ float m_weight2;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ float m_time;
    /* 0x1C */ float m_rate;
    /* 0x20 */ float m_elapsed;
    /* 0x24 */ u32 field_0x24;
    /* 0x28 */ u32 field_0x28;
};
SIZE_ASSERT(MotionBlend, 0x2C);

/* The frame range, speed and current frame of a layer. */
struct MotionFrames {
    /* 0x0 */ float m_start;
    /* 0x4 */ float m_end;
    /* 0x8 */ float m_speed;
    /* 0xC */ float m_frame;
};

/* The same record as words: retail copies it with integer loads and stores. */
struct MotionFrameWords {
    u32 w[4];
};

/* A frame of a motion's or bone data's tree; as the frame's header, its rest pose. */
struct MotionBody {
    /* 0x00 */ u32 m_key;
    /* 0x04 */ u16 m_type;
    /* 0x06 */ u16 m_flags;
    /* 0x08 */ u16 field_0x8;
    /* 0x0A */ u16 field_0xA;
    /* 0x0C */ u16 field_0xC;
    /* 0x0E */ u16 field_0xE;
    /* 0x10 */ Vec3f m_position;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ float m_rotation[4];
    /* 0x30 */ Vec3f m_scale;
    /* 0x3C */ u8 field_0x3C[0xC];
    /* 0x48 */ MotionBody *m_child;
    /* 0x4C */ MotionBody *m_next;
    /* 0x50 */ char m_name[0x20];
};

/* The root motion of a layer: a position and three tracks. */
struct MotionRootMotion {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ union {
        float m_scale;
        MotionBody *m_body;
    };
    /* 0x14 */ void *m_link[3];
    /* 0x20 */ HalfKeyTrack m_tracks[3];
};

/* What a layer plays: a list of sources updated in turn. */
struct MotionSource {
    /* 0x00 */ void (*m_update)(void *data);
    /* 0x04 */ void (*m_update2)(void *data);
    /* 0x08 */ MotionSource *m_next;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ MotionRootMotion m_rootMotion;
};

/* A motion layer. */
struct MotionLayer {
    /* 0x000 */ Mtx34f m_matrix;
    /* 0x030 */ Mtx34f m_prevMatrix;
    /* 0x060 */ Mtx34f m_baseMatrix;
    /* 0x090 */ u32 m_flags;
    /* 0x094 */ u32 m_flags2;
    /* 0x098 */ float m_minFrame;
    /* 0x09C */ float m_maxFrame;
    /* 0x0A0 */ MotionFrames m_frames;
    /* 0x0B0 */ Vec3f m_basePosition;
    /* 0x0BC */ u32 field_0xBC;
    /* 0x0C0 */ Vec3f m_baseRotation;
    /* 0x0CC */ u32 field_0xCC;
    /* 0x0D0 */ Vec3f m_prevRoot;
    /* 0x0DC */ u32 field_0xDC;
    /* 0x0E0 */ Vec3f m_root;
    /* 0x0EC */ u32 field_0xEC;
    /* 0x0F0 */ Vec3f m_rootMove;
    /* 0x0FC */ u32 field_0xFC;
    /* 0x100 */ MotionFrames m_request;
    /* 0x110 */ u16 m_newMotion;
    /* 0x112 */ u8 field_0x112[0xE];
    /* 0x120 */ MotionBlend *m_blend;
    /* 0x124 */ MotionSource *m_source;
    /* 0x128 */ u16 m_motion;
    /* 0x12A */ u16 field_0x12A;
    /* 0x12C */ float m_lastFrame;
};
SIZE_ASSERT(MotionLayer, 0x130);

/* The world matrix and the scale of a model node. */
struct MotionNodeWorld {
    /* 0x00 */ Mtx34f m_matrix;
    /* 0x30 */ Vec3f m_scale;
};

/* A node of the model: its matrices and scale. */
struct MotionNode {
    /* 0x00 */ Mtx34f m_matrix;
    /* 0x30 */ MotionNodeWorld m_world;
    /* 0x6C */ u32 field_0x6C;
};
SIZE_ASSERT(MotionNode, 0x70);



/* How a layer's blend weight is scaled. */
struct MotionWeightScale {
    /* 0x00 */ s32 m_enabled;
    /* 0x04 */ s32 m_pending;
    /* 0x08 */ float m_scale;
    /* 0x0C */ float m_offset;
    /* 0x10 */ float m_newScale;
    /* 0x14 */ float m_newOffset;
    /* 0x18 */ u8 field_0x18[0x8];
};


/* The keys of a channel for one layer. */
struct MotionChannelLayer {
    /* 0x0 */ u8 *m_data;
    /* 0x4 */ MotionBlend *m_blend;
};

/* A channel (callback) of a model frame, called with its data. */
struct MotionCallback {
    /* 0x0 */ void (*m_function)(void *data);
    /* 0x4 */ MotionCallback *m_next;
    /* 0x8 */ void *m_data;
    /* 0xC */ MotionNodeWorld *m_target;
    /* 0x10 */ MotionChannelLayer m_layers[4];
};

struct MotionCallbackArgs {
    /* 0x0 */ Mtx34f *m_matrix;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ u32 field_0x8;
};


/* A frame of a model (the model is its root frame). */
struct MotionModel {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ MotionCallback *m_callbacks;
    /* 0x0C */ MotionBody *m_header;
    /* 0x10 */ MotionNode *m_output;
    /* 0x14 */ MotionBlend *m_layerWeights;
    /* 0x18 */ MotionWeightScale *m_weightScales;
    /* 0x1C */ void (*m_callback)(MotionCallbackArgs *args);
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ MotionModel *m_child;
    /* 0x28 */ MotionModel *m_next;
    /* 0x2C */ u32 field_0x2C;
    /* 0x30 */ u32 field_0x30;
};
SIZE_ASSERT(MotionModel, 0x34);



/* A loaded motion or bone data. */
struct MotionData {
    /* 0x00 */ u32 m_key;
    /* 0x04 */ u16 m_type;
    /* 0x06 */ u16 field_0x6;
    /* 0x08 */ float m_scale;
    /* 0x0C */ u8 field_0xC[0x20];
    /* 0x2C */ MotionBody m_body;
};

/* The layer lists of a play work, as its channels see them. */
struct MotionLayerList {
    /* 0x0 */ s32 m_count;
    /* 0x4 */ s32 *m_order;
    /* 0x8 */ s32 *m_slots;
    /* 0xC */ s32 *m_parents;
};

/* What a blended channel function is called with: the channel's data and per-layer entries. */
struct MotionChannelArgs {
    /* 0x0 */ MotionLayerList *m_list;
    /* 0x4 */ MotionNodeWorld *m_target;
    /* 0x8 */ MotionChannelLayer m_entries[4];
};

/* A play work. */
struct MotionPlayWork {
    /* 0x000 */ s32 m_nodeCount;
    /* 0x004 */ MotionNode *m_nodes;
    /* 0x008 */ MotionData *m_skeleton;
    /* 0x00C */ MotionModel *m_model;
    /* 0x010 */ MotionLayer *m_layers;
    /* 0x014 */ u32 m_flags;
    /* 0x018 */ u32 field_0x18;
    /* 0x01C */ u32 field_0x1C;
    /* 0x020 */ s32 m_layerCount;
    /* 0x024 */ s32 *m_order;
    /* 0x028 */ s32 *m_slots;
    /* 0x02C */ s32 *m_parents;
    /* 0x030 */ Mtx34f m_rootMatrix;
    /* 0x060 */ Mtx34f m_prevRootMatrix;
    /* 0x090 */ Mtx34f m_matrix;
    /* 0x0C0 */ Mtx34f m_prevMatrix;
    /* 0x0F0 */ Vec3f m_position;
    /* 0x0FC */ u32 field_0xFC;
    /* 0x100 */ Vec3f m_rotation;
    /* 0x10C */ u32 field_0x10C;
    /* 0x110 */ Vec3f field_0x110;
    /* 0x11C */ u32 field_0x11C;
    /* 0x120 */ Vec3f field_0x120;
    /* 0x12C */ u32 field_0x12C;
    /* 0x130 */ Vec3f field_0x130;
    /* 0x13C */ u32 field_0x13C;
    /* 0x140 */ Vec3f field_0x140;
    /* 0x14C */ u32 field_0x14C;
    /* 0x150 */ Vec3f field_0x150;
    /* 0x15C */ u32 field_0x15C;
};
SIZE_ASSERT(MotionPlayWork, 0x160);

/* The matrix stack (shared with the units before this one). */
struct MotionMatrixStack {
    /* 0x0 */ Mtx34f *m_matrices;
    /* 0x4 */ s32 m_size;
    /* 0x8 */ s32 m_top;
};

extern MotionMatrixStack *lbl_805F13A8;

extern "C" {
void fn_8040C204(MotionMatrixStack *stack);
void fn_8040C244(MotionMatrixStack *stack);
Mtx34f *fn_80411700(MotionMatrixStack *stack);
void fn_80411548(Vec3f *out, const Vec3f *from, const Vec3f *offset);
int fn_80411818(u16 motion, int target, float *frames, void *arg, int stopped);
int fn_80411DEC(u16 motion, int target, float *frames, void *arg, int stopped);
void fn_80413240(u16 motion);
void fn_8041CC18(const MotionModel *model, int layer);
int fn_8041CFB0(const MotionModel *model, int index, u32 key, float scale, float offset);
int fn_8041D1A0(MotionModel *model, u32 key, void (*callback)(MotionCallbackArgs *args), u32 arg);
MotionNode *fn_8041D31C(const MotionModel *model, u32 key);
void fn_8041AB3C(MotionPlayWork *work, int index);
int fn_804125F4(MotionFrames *frames, u16 motion);
u32 fn_804123DC(u16 motion);
MotionData *fn_804106FC(u32 key);
void fn_8041B6FC(MotionModel *model, int index);
void fn_8041B2BC(MotionPlayWork *work, MotionBody *body, int index, u16 motion, float ratio);
MotionModel *fn_8041D448(MotionModel *model, u32 key);
HalfKeyTrackData *fn_804124DC(u16 motion);
void fn_804110FC(void *p);
int fn_803A953C(const char *format, ...);
MotionPlayWork *fn_8041B980(MotionData *data, s32 layers);
void fn_8041CA78(s32 *sourceSize, u32 *channelSize, s32 *nodeCount, int layers, MotionBody *body);
void *fn_80411134(int heap, u32 size);
void fn_8041C854(MotionSource **cursor, MotionSource **last, MotionBody *body);
void fn_8041C2E0(MotionBody *root, MotionBody *body, MotionSource **source, MotionSource *first);
void fn_8041BE30(MotionPlayWork *work, MotionModel **cursor, MotionBlend *blends, u8 **scales,
                 MotionNode **nodes, MotionSource **sources, MotionBody *body);
void fn_8041FD6C(void *data);
char *fn_803AC6C0(char *string, const char *find);
void fn_8040B790(Mtx34f *m, const Vec3f *scale);
void fn_8041116C(Mtx34f *out, const Mtx34f *m);
void fn_8042062C(Mtx34f *out, const Mtx34f *from, const Mtx34f *to, float t);
void fn_804208EC(Quatf *out, const Mtx34f *m);
void fn_80420724(Quatf *out, const Quatf *from, const Quatf *to, float t);
void fn_80420B60(Mtx34f *out, const Quatf *q);
u32 fn_80410684(const char *name);
MotionBody *fn_8041C5A8(MotionBody *body, MotionSource **cursor, u32 key);
void fn_804201C0(void *data);
void fn_8041FF0C(void *data);
void fn_8041FF08(void *data);
void fn_80420158(void *data);
void fn_804202B8(void *data);
void fn_80420308(void *data);
void fn_804203AC(void *data);
void fn_80420438(void *data);
void fn_8042043C(void *data);
void fn_804204C8(void *data);
void fn_8041AF1C(MotionModel *model, int index);
void fn_8041AFCC(MotionModel *model, int index);
void fn_804175C8(MotionPlayWork *work);
void fn_80418044(MotionPlayWork *work);
void fn_804188A0(MotionPlayWork *work);
void fn_80418A4C(MotionPlayWork *work);
void fn_804192EC(MotionPlayWork *work);
void fn_80419F5C(MotionPlayWork *work);
}

extern "C" {
void fn_8040BAC4(Mtx34f *m, const Mtx34f *by);
void fn_8040BCD4(Mtx34f *m);
void fn_804113E8(Mtx34f *m, const Mtx34f *from);
void fn_8041157C(Vec3f *out, const Mtx34f *m, const Vec3f *v);
void fn_8041CD6C(const MotionModel *model, int layer);
void fn_804114D0(Vec3f *v, int flags);
void fn_80420590(Mtx34f *out, Mtx34f *from, Mtx34f *to, float t);
void fn_804186A0(MotionPlayWork *work, MotionModel *model);
}

/* Work areas of the layer blending, one entry per layer. */
static UnkMatrix_800B1304 lbl_805E12E0[5];
static UnkMatrix_800B1304 s_matrices[4];
static Vec3f s_roots[4];
static s32 s_isParent[4];
static s32 s_done[4];
static s32 s_rootSet[4];
/* The blend weight of each layer while the model is walked. */
static float lbl_805E14F0[4];

extern "C" {
void fn_8041966C(MotionModel *model);
void fn_80413318(u16 motion);
void fn_8041A440(MotionPlayWork *work);
void fn_8041A83C(MotionPlayWork *work, MotionModel *model);
void fn_80411328(Vec3f *scale, const Mtx34f *m);
int fn_8041B170(MotionPlayWork *work, int index, u16 motion);
void fn_80419BA8(Vec3f *out, MotionLayer *layer, MotionFrames *frames, int flag);
void fn_8041AED0(MotionPlayWork *work, int slot);
u32 fn_8041AD94(MotionFrames *frames, MotionFrames *request, float minFrame, float maxFrame);
void fn_80419D5C(MotionPlayWork *work, int index);
void fn_804198E4(MotionPlayWork *work, int index);
void fn_8041CE74(const MotionModel *model, int layer);
void fn_804194B8(MotionModel *model);
void fn_8041D580(void *data);
void fn_8041D89C(void *data);
void fn_8041DBD4(void *data);
void fn_8041E124(void *data);
void fn_8041E420(void *data);
void fn_8041E480(void *data);
void fn_8041E5E0(void *data);
void fn_8041E618(void *data);
void fn_8041E9C4(void *data);
void fn_8041ED8C(void *data);
void fn_8041F290(void *data);
void fn_8041F6F0(void *data);
void fn_8041F7D0(void *data);
void fn_8041FA20(void *data);
void fn_8041FAD8(void *data);
void fn_8041FB4C(void *data);
void fn_8041FCC0(void *data);
void fn_8041FD0C(void *data);
void fn_8041FD70(void *data);
void fn_8041FED0(void *data);
}

/* The half-key track functions for the current direction of play. */
float (*lbl_805F13D0)(HalfKeyTrack *track);
float (*lbl_805F13D4)(HalfKeyTrack *track);
void (*lbl_805F13D8)(float *value, float *slope, HalfKeyTrack *track);

/* The time scale of the motions (.sdata). */
float lbl_805EDD60 = 1.0f;

static MotionLayer *lbl_805F13DC;
static MotionLayer *lbl_805F13E0;

/* The top of the matrix stack. */
inline Mtx34f *TopMatrix() {
    return &lbl_805F13A8->m_matrices[lbl_805F13A8->m_top];
}

/* The layer in a slot, or null. */
inline MotionLayer *GetLayer(MotionPlayWork *work, int slot) {
    int index = work->m_slots[slot];
    if (index == -1) {
        return 0;
    }
    return &work->m_layers[index];
}

/* The layer in a slot if it is playing, or null. */
inline MotionLayer *GetPlayingLayer(MotionPlayWork *work, int slot) {
    MotionLayer *layer;
    int index = work->m_slots[slot];
    if (index == -1) {
        return 0;
    }
    layer = &work->m_layers[index];
    if (layer->m_flags & 4) {
        return layer;
    }
    return 0;
}

/* Removes a layer index from the slot map, closing the gap. */
#define REMOVE_SLOT(work, index, i, j)                                                             \
    for (i = 0; i < work->m_layerCount; i++) {                                                     \
        if (work->m_slots[i] == -1) {                                                              \
            break;                                                                                 \
        }                                                                                          \
        if (index == work->m_slots[i]) {                                                           \
            work->m_slots[i] = -1;                                                                 \
            for (j = i; j < work->m_layerCount - 1; j++) {                                         \
                work->m_slots[j] = work->m_slots[j + 1];                                           \
                work->m_slots[j + 1] = -1;                                                         \
            }                                                                                      \
            break;                                                                                 \
        }                                                                                          \
    }

/* The same with one counter: the gap is closed by the counter that found the slot. */
#define REMOVE_SLOT_ONE(work, index, i)                                                            \
    for (i = 0; i < work->m_layerCount; i++) {                                                     \
        if (work->m_slots[i] == -1) {                                                              \
            break;                                                                                 \
        }                                                                                          \
        if (index == work->m_slots[i]) {                                                           \
            work->m_slots[i] = -1;                                                                 \
            for (; i < work->m_layerCount - 1; i++) {                                              \
                work->m_slots[i] = work->m_slots[i + 1];                                           \
                work->m_slots[i + 1] = -1;                                                         \
            }                                                                                      \
            break;                                                                                 \
        }                                                                                          \
    }

/* Sets the channel functions of one model frame for single playing. */
inline void SetSingleChannels(MotionModel *model) {
    MotionCallback *channel = model->m_callbacks;
    if (model->field_0x0 & 0x400) {
        channel->m_function = fn_8041FAD8;
        channel = channel->m_next;
    }
    if (model->field_0x0 & 7) {
        channel->m_function = fn_8041FCC0;
    } else {
        channel->m_function = fn_8041FED0;
    }
    channel = channel->m_next;
    if (!(model->field_0x0 & 0x40)) {
        if (model->field_0x0 & 0x38) {
            channel->m_function = fn_8041FB4C;
        } else {
            channel->m_function = fn_8041FD70;
        }
    }
    channel = channel->m_next;
    if (model->field_0x0 & 0x380) {
        channel->m_function = fn_8041FAD8;
    } else {
        channel->m_function = fn_8041FD0C;
    }
}

/* Restarts the three tracks of a channel. */
#define RESET_TRACKS(tracks)                                                                       \
    if ((tracks)[0].m_cursor) {                                                                    \
        fn_80413B30(&(tracks)[0]);                                                                 \
    }                                                                                              \
    if ((tracks)[1].m_cursor) {                                                                    \
        fn_80413B30(&(tracks)[1]);                                                                 \
    }                                                                                              \
    if ((tracks)[2].m_cursor) {                                                                    \
        fn_80413B30(&(tracks)[2]);                                                                 \
    }

/* Moves cursor past the sources of one body. */
#define SKIP_SOURCES(body, cursor)                                                                 \
    if ((body)->m_flags & 0x400) {                                                                 \
        *(cursor) = (*(cursor))->m_next;                                                           \
    }                                                                                              \
    if ((body)->m_flags & 7) {                                                                     \
        *(cursor) = (*(cursor))->m_next;                                                           \
    }                                                                                              \
    if ((body)->m_flags & 0x40) {                                                                  \
        *(cursor) = (*(cursor))->m_next;                                                           \
    } else if ((body)->m_flags & 0x38) {                                                           \
        *(cursor) = (*(cursor))->m_next;                                                           \
    }                                                                                              \
    if ((body)->m_flags & 0x380) {                                                                 \
        *(cursor) = (*(cursor))->m_next;                                                           \
    }

/* fn_8041C5A8 once more, so that it recurses one level deeper per call. */
inline MotionBody *FindBody(MotionBody *body, MotionSource **cursor, u32 key) {
    MotionBody *found;

    if (key == body->m_key) {
        return body;
    }
    SKIP_SOURCES(body, cursor);
    if (body->m_child && (found = fn_8041C5A8(body->m_child, cursor, key))) {
        return found;
    }
    if (body->m_next && (found = fn_8041C5A8(body->m_next, cursor, key))) {
        return found;
    }
    return 0;
}

/* The bodies of fn_8041CC18 to fn_8041D448 once more, so that each recurses one level deeper
 * per call. */
inline void ResetWeightScale(MotionModel *model, int index) {
    model->m_weightScales[index].m_enabled = 0;
    model->m_weightScales[index].m_pending = 0;
    model->m_weightScales[index].m_scale = 1.0f;
    model->m_weightScales[index].m_offset = 0.0f;
    model->m_weightScales[index].m_newScale = 1.0f;
    model->m_weightScales[index].m_newOffset = 0.0f;
    if (model->m_child) {
        fn_8041CC18(model->m_child, index);
    }
    if (model->m_next) {
        fn_8041CC18(model->m_next, index);
    }
}

inline void DropWeightScale(MotionModel *model, int index) {
    model->m_weightScales[index].m_pending = 0;
    model->m_weightScales[index].m_newScale = 1.0f;
    model->m_weightScales[index].m_newOffset = 0.0f;
    if (model->m_child) {
        fn_8041CD6C(model->m_child, index);
    }
    if (model->m_next) {
        fn_8041CD6C(model->m_next, index);
    }
}

inline void ApplyWeightScale(MotionModel *model, int index) {
    if (model->m_weightScales[index].m_pending) {
        model->m_weightScales[index].m_enabled = 1;
        model->m_weightScales[index].m_scale = model->m_weightScales[index].m_newScale;
        model->m_weightScales[index].m_offset = model->m_weightScales[index].m_newOffset;
    }
    if (model->m_child) {
        fn_8041CE74(model->m_child, index);
    }
    if (model->m_next) {
        fn_8041CE74(model->m_next, index);
    }
}

inline int SetWeightScale(MotionModel *model, int index, u32 key, float scale, float offset) {
    if (key == model->m_header->m_key) {
        model->m_weightScales[index].m_newScale = scale;
        model->m_weightScales[index].m_newOffset = offset;
        model->m_weightScales[index].m_pending = 1;
        return 1;
    }
    if (model->m_child && fn_8041CFB0(model->m_child, index, key, scale, offset)) {
        return 1;
    }
    if (model->m_next && fn_8041CFB0(model->m_next, index, key, scale, offset)) {
        return 1;
    }
    return 0;
}

inline int SetCallback(MotionModel *model, u32 key, void (*callback)(MotionCallbackArgs *args),
                       u32 arg) {
    if (key == model->m_header->m_key) {
        model->m_callback = callback;
        model->field_0x20 = arg;
        return 1;
    }
    if (model->m_child && fn_8041D1A0(model->m_child, key, callback, arg)) {
        return 1;
    }
    if (model->m_next && fn_8041D1A0(model->m_next, key, callback, arg)) {
        return 1;
    }
    return 0;
}

inline MotionNode *FindNode(MotionModel *model, u32 key) {
    MotionNode *found;

    if (key == model->m_header->m_key) {
        return model->m_output;
    }
    if (model->m_child && (found = fn_8041D31C(model->m_child, key))) {
        return found;
    }
    if (model->m_next && (found = fn_8041D31C(model->m_next, key))) {
        return found;
    }
    return 0;
}

inline MotionModel *FindFrame(MotionModel *model, u32 key) {
    MotionModel *found;

    if (key == model->m_header->m_key) {
        return model;
    }
    if (model->m_child && (found = fn_8041D448(model->m_child, key))) {
        return found;
    }
    if (model->m_next && (found = fn_8041D448(model->m_next, key))) {
        return found;
    }
    return 0;
}

/* Rotates the top of the matrix stack by angles in 1/65536 of a turn, Z first. */
/* One rotation of ROTATE_ZYX; the matrix is taken before the angle is converted. */
#define ROT(FN, v)                                                                                 \
    {                                                                                              \
        Mtx34f *m = TopMatrix();                                                                   \
        FN(m, 6.2831855f * (s32)(u16)(v) / 65536.0f);                                              \
    }
#define ROTATE_ZYX(x, y, z)                                                                            if (z) {                                                                                               ROT(fn_8040B80C, z);                                                                           }                                                                                                  if (y) {                                                                                               ROT(fn_8040B848, y);                                                                           }                                                                                                  if (x) {                                                                                               ROT(fn_8040B884, x);                                                                           }

// DEFINITIONS
/* Blends the rotations of two matrices through quaternions. */
extern "C" void fn_8042062C(Mtx34f *out, const Mtx34f *from, const Mtx34f *to, float t) {
    Quatf blended;
    Quatf b;
    Quatf a;

    fn_804208EC(&a, from);
    fn_804208EC(&b, to);
    fn_80420724(&blended, &a, &b, t);
    fn_80420B60(out, &blended);
    fn_8041116C(out, out);
}

/* Blends two matrices: rotation by fn_8042062C, translation linearly. */
/* The matrices are pointers to non-const: GC/2.7 then leaves their loads behind the stores into
 * the local matrix, as retail has them (wave 40). */
extern "C" void fn_80420590(Mtx34f *out, Mtx34f *from, Mtx34f *to, float t) {
    Mtx34f matrix;

    fn_8042062C(&matrix, from, to, t);
    matrix.values[0][3] = t * from->values[0][3] + (1.0f - t) * to->values[0][3];
    matrix.values[1][3] = t * from->values[1][3] + (1.0f - t) * to->values[1][3];
    matrix.values[2][3] = t * from->values[2][3] + (1.0f - t) * to->values[2][3];
    fn_8040C148(out, &matrix);
}

/* Update of a translation source, scaled. */
extern "C" void fn_804204C8(void *data) {
    MotionRootMotion *motion = (MotionRootMotion *)data;
    HalfKeyTrack *tracks = motion->m_tracks;

    if (tracks[0].m_cursor) {
        motion->m_position.x = lbl_805F13D0(&tracks[0]);
    }
    if (tracks[1].m_cursor) {
        motion->m_position.y = lbl_805F13D0(&tracks[1]);
    }
    if (tracks[2].m_cursor) {
        motion->m_position.z = lbl_805F13D0(&tracks[2]);
    }
    if (motion->m_scale > 0.0f) {
        motion->m_position.x = motion->m_position.x * motion->m_scale;
        motion->m_position.y = motion->m_position.y * motion->m_scale;
        motion->m_position.z = motion->m_position.z * motion->m_scale;
    }
}

/* Update of a rotation source. */
extern "C" void fn_8042043C(void *data) {
    MotionRootMotion *motion = (MotionRootMotion *)data;
    HalfKeyTrack *tracks = motion->m_tracks;

    if (tracks[0].m_cursor) {
        motion->m_position.x = lbl_805F13D0(&tracks[0]);
    }
    if (tracks[1].m_cursor) {
        motion->m_position.y = lbl_805F13D0(&tracks[1]);
    }
    if (tracks[2].m_cursor) {
        motion->m_position.z = lbl_805F13D0(&tracks[2]);
    }
}

/* Update of a rotation source that is not in Euler angles (nothing to do). */
extern "C" void fn_80420438(void *data) {
}

/* Update of a scale source. */
extern "C" void fn_804203AC(void *data) {
    MotionRootMotion *motion = (MotionRootMotion *)data;
    HalfKeyTrack *tracks = (HalfKeyTrack *)&motion->m_scale;

    if (tracks[0].m_cursor) {
        motion->m_position.x = lbl_805F13D0(&tracks[0]);
    }
    if (tracks[1].m_cursor) {
        motion->m_position.y = lbl_805F13D0(&tracks[1]);
    }
    if (tracks[2].m_cursor) {
        motion->m_position.z = lbl_805F13D0(&tracks[2]);
    }
}

/* Update of a pre-scale source: the inverse of its tracks. */
extern "C" void fn_80420308(void *data) {
    MotionRootMotion *motion = (MotionRootMotion *)data;
    HalfKeyTrack *tracks = (HalfKeyTrack *)&motion->m_scale;

    if (tracks[0].m_cursor) {
        motion->m_position.x = 1.0f / lbl_805F13D0(&tracks[0]);
    }
    if (tracks[1].m_cursor) {
        motion->m_position.y = 1.0f / lbl_805F13D0(&tracks[1]);
    }
    if (tracks[2].m_cursor) {
        motion->m_position.z = 1.0f / lbl_805F13D0(&tracks[2]);
    }
}

/* Second update of a translation source: mirrors the position. */
extern "C" void fn_804202B8(void *data) {
    MotionRootMotion *motion = (MotionRootMotion *)data;
    MotionBody *body = (MotionBody *)motion->m_link[0];

    if (body->field_0xA & 1) {
        motion->m_position.x = -motion->m_position.x;
    }
    if (body->field_0xA & 2) {
        motion->m_position.y = -motion->m_position.y;
    }
    if (body->field_0xA & 4) {
        motion->m_position.z = -motion->m_position.z;
    }
}

/* Second update of a mirrored translation source: swaps the positions of the two sides,
 * mirrored. */
extern "C" void fn_804201C0(void *data) {
    MotionRootMotion *motion = (MotionRootMotion *)data;
    MotionBody *body = (MotionBody *)motion->m_link[0];
    MotionBody *mirror = (MotionBody *)motion->m_link[1];
    MotionSource *otherSource = (MotionSource *)motion->m_link[2];
    Vec3f *other;
    Vec3f swap;

    if (body->field_0xA & 1) {
        motion->m_position.x = -motion->m_position.x;
    }
    if (body->field_0xA & 2) {
        motion->m_position.y = -motion->m_position.y;
    }
    if (body->field_0xA & 4) {
        motion->m_position.z = -motion->m_position.z;
    }
    if (mirror->m_flags & 7) {
        other = &otherSource->m_rootMotion.m_position;
        if (mirror->field_0xA & 1) {
            other->x = -other->x;
        }
        if (mirror->field_0xA & 2) {
            other->y = -other->y;
        }
        if (mirror->field_0xA & 4) {
            other->z = -other->z;
        }
        fn_800091D0(&swap, &motion->m_position);
        fn_800091D0(&motion->m_position, other);
        fn_800091D0(other, &swap);
    }
}

/* Second update of a rotation source: mirrors the rotation about the rest pose. */
extern "C" void fn_80420158(void *data) {
    MotionRootMotion *motion = (MotionRootMotion *)data;
    MotionBody *body = motion->m_body;

    if (body->field_0xA & 8) {
        motion->m_position.x = 2.0f * body->m_rotation[0] - motion->m_position.x;
    }
    if (body->field_0xA & 0x10) {
        motion->m_position.y = 2.0f * body->m_rotation[1] - motion->m_position.y;
    }
    if (body->field_0xA & 0x20) {
        motion->m_position.z = 2.0f * body->m_rotation[2] - motion->m_position.z;
    }
}

/* Second update of a mirrored rotation source: swaps the rotations of the two sides, mirrored. */
extern "C" void fn_8041FF0C(void *data) {
    MotionRootMotion *motion = (MotionRootMotion *)data;
    MotionBody *body = motion->m_body;
    MotionBody *mirror = (MotionBody *)motion->m_link[0];
    MotionSource *otherSource = (MotionSource *)motion->m_link[1];
    Vec3f *other;
    Vec3f swap;

    if (mirror->m_flags & 0x38) {
        other = &otherSource->m_rootMotion.m_position;
        if (body->field_0xA & 8) {
            motion->m_position.x = mirror->m_rotation[0] - (motion->m_position.x - body->m_rotation[0]);
        } else {
            motion->m_position.x = mirror->m_rotation[0] + (motion->m_position.x - body->m_rotation[0]);
        }
        if (body->field_0xA & 0x10) {
            motion->m_position.y = mirror->m_rotation[1] - (motion->m_position.y - body->m_rotation[1]);
        } else {
            motion->m_position.y = mirror->m_rotation[1] + (motion->m_position.y - body->m_rotation[1]);
        }
        if (body->field_0xA & 0x20) {
            motion->m_position.z = mirror->m_rotation[2] - (motion->m_position.z - body->m_rotation[2]);
        } else {
            motion->m_position.z = mirror->m_rotation[2] + (motion->m_position.z - body->m_rotation[2]);
        }
        if (mirror->field_0xA & 8) {
            other->x = body->m_rotation[0] - (other->x - mirror->m_rotation[0]);
        } else {
            other->x = body->m_rotation[0] + (other->x - mirror->m_rotation[0]);
        }
        if (mirror->field_0xA & 0x10) {
            other->y = body->m_rotation[1] - (other->y - mirror->m_rotation[1]);
        } else {
            other->y = body->m_rotation[1] + (other->y - mirror->m_rotation[1]);
        }
        if (mirror->field_0xA & 0x20) {
            other->z = body->m_rotation[2] - (other->z - mirror->m_rotation[2]);
        } else {
            other->z = body->m_rotation[2] + (other->z - mirror->m_rotation[2]);
        }
        fn_800091D0(&swap, &motion->m_position);
        fn_800091D0(&motion->m_position, other);
        fn_800091D0(other, &swap);
    } else {
        if (body->field_0xA & 8) {
            motion->m_position.x = 2.0f * body->m_rotation[0] - motion->m_position.x;
        }
        if (body->field_0xA & 0x10) {
            motion->m_position.y = 2.0f * body->m_rotation[1] - motion->m_position.y;
        }
        if (body->field_0xA & 0x20) {
            motion->m_position.z = 2.0f * body->m_rotation[2] - motion->m_position.z;
        }
    }
}

/* Second update of a rotation source (nothing to do). */
extern "C" void fn_8041FF08(void *data) {
}

/* Translation channel of a frame without keys. */
extern "C" void fn_8041FED0(void *data) {
    fn_8040B59C(TopMatrix(), *(Vec3f **)data);
}

/* Rotation channel of a frame without keys. */
extern "C" void fn_8041FD70(void *data) {
    Vec3f *angles = *(Vec3f **)data;
    int x = 65536.0f * angles->x / 6.2831855f;
    int y = 65536.0f * angles->y / 6.2831855f;
    int z = 65536.0f * angles->z / 6.2831855f;

    ROTATE_ZYX(x, y, z);
}

/* Rotation channel of a frame without keys whose rotation is not in Euler angles. */
extern "C" void fn_8041FD6C(void *data) {
}

/* Scale channel of a frame without keys. */
extern "C" void fn_8041FD0C(void *data) {
    Vec3f *scale = *(Vec3f **)data;

    if (1.0f != scale->x || 1.0f != scale->y || 1.0f != scale->z) {
        fn_8040B790(TopMatrix(), scale);
    }
}

/* Translation channel of a single layer. */
extern "C" void fn_8041FCC0(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    MotionChannelLayer *entry = args->m_entries;
    Mtx34f *top = TopMatrix();

    entry += args->m_list->m_order[0];
    fn_8040B59C(top, (Vec3f *)entry->m_data);
}

/* Rotation channel (Euler angles) of a single layer. */
extern "C" void fn_8041FB4C(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    MotionChannelLayer *entry = args->m_entries;
    Vec3f *angles;
    int x;
    int y;
    int z;

    entry += args->m_list->m_order[0];
    angles = (Vec3f *)entry->m_data;
    x = 65536.0f * angles->x / 6.2831855f;
    y = 65536.0f * angles->y / 6.2831855f;
    z = 65536.0f * angles->z / 6.2831855f;
    ROTATE_ZYX(x, y, z);
}

/* Scale channel of a single layer. */
extern "C" void fn_8041FAD8(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    MotionChannelLayer *entry = args->m_entries;
    Vec3f *scale;

    entry += args->m_list->m_order[0];
    scale = (Vec3f *)entry->m_data;
    if (1.0f != scale->x || 1.0f != scale->y || 1.0f != scale->z) {
        fn_8040B790(TopMatrix(), scale);
    }
}

/* Translation channel of a root frame: also moves the layer matrices. */
extern "C" void fn_8041FA20(void *data) {
    Vec3f *offset = *(Vec3f **)data;
    int i;

    fn_8040C204(lbl_805F13A8);
    for (i = 0; i < 5; i++) {
        fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[i]);
        fn_8040B59C(TopMatrix(), offset);
        fn_8040C148(&lbl_805E12E0[i], fn_80411700(lbl_805F13A8));
    }
    fn_8040C244(lbl_805F13A8);
    fn_8040B59C(TopMatrix(), offset);
}

/* Rotation channel of a root frame: also rotates the layer matrices. */
extern "C" void fn_8041F7D0(void *data) {
    Vec3f *angles = *(Vec3f **)data;
    int x = 65536.0f * angles->x / 6.2831855f;
    int y = 65536.0f * angles->y / 6.2831855f;
    int z = 65536.0f * angles->z / 6.2831855f;
    Mtx34f *saved;
    int i;

    fn_8040C204(lbl_805F13A8);
    for (i = 0; i < 5; i++) {
        fn_8040C148(fn_80411700(lbl_805F13A8), saved = &lbl_805E12E0[i]);
        ROTATE_ZYX(x, y, z);
        fn_8040C148(saved, fn_80411700(lbl_805F13A8));
    }
    fn_8040C244(lbl_805F13A8);
    ROTATE_ZYX(x, y, z);
}

/* Scale channel of a root frame: also scales the layer matrices. */
extern "C" void fn_8041F6F0(void *data) {
    Vec3f *scale = *(Vec3f **)data;
    int i;

    if (1.0f != scale->x || 1.0f != scale->y || 1.0f != scale->z) {
        fn_8040C204(lbl_805F13A8);
        for (i = 0; i < 5; i++) {
            fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[i]);
            fn_8040B790(TopMatrix(), scale);
            fn_8040C148(&lbl_805E12E0[i], fn_80411700(lbl_805F13A8));
        }
        fn_8040C244(lbl_805F13A8);
        fn_8040B790(TopMatrix(), scale);
    }
}

/* The blend of two vectors: w of a over b. */
inline void Lerp(Vec3f *dst, const Vec3f *a, const Vec3f *b, float w) {
    dst->x = w * a->x + (1.0f - w) * b->x;
    dst->y = w * a->y + (1.0f - w) * b->y;
    dst->z = w * a->z + (1.0f - w) * b->z;
}

/* Blended translation channel of a root frame: moves the layer matrices, then blends their
 * positions. */
extern "C" void fn_8041F290(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    void **entries = (void **)args->m_entries;
    int m;
    int n;
    int again;
    MotionNodeWorld *target;
    Vec3f *src;
    int i;
    MotionLayerList *list = args->m_list;
    Mtx34f matrix;
    Vec3f result;
    Vec3f position;
    Vec3f basePosition;
    Vec3f offset;
    int j;
    int index;
    int other;
    int parent;
    int k;
    Vec3f *dst;
    Vec3f *base;

    target = args->m_target;
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    fn_8040C204(lbl_805F13A8);
    for (n = 0; n < list->m_count; n++) {
        index = list->m_order[n];
        if (index == -1) {
            break;
        }
        k = index * 2;
        src = (Vec3f *)entries[k];
        if ((((MotionBlend *)entries[++k])->m_flags & 1) && n == 0) {
            offset.x = target->m_matrix.values[0][3];
            offset.y = target->m_matrix.values[1][3];
            offset.z = target->m_matrix.values[2][3];
            fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[4]);
            fn_8040B59C(TopMatrix(), &offset);
            fn_8040C148(&lbl_805E12E0[4], fn_80411700(lbl_805F13A8));
        }
        fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[n]);
        fn_8040B59C(TopMatrix(), src);
        fn_8040C148(&lbl_805E12E0[n], fn_80411700(lbl_805F13A8));
    }
    do {
        for (i = 0, again = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (again && list->m_parents[index] == -1) {
                s_isParent[index] = 1;
                continue;
            }
            s_isParent[index] = 0;
            for (j = i + 1; j < list->m_count; j++) {
                other = list->m_order[j];
                if (other == -1) {
                    break;
                }
                if (s_done[other] == 0 && index == list->m_parents[other]) {
                    s_isParent[index] = 1;
                    again = 1;
                    break;
                }
            }
        }
        for (m = 0; m < list->m_count; m++) {
            index = list->m_order[m];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            position.x = lbl_805E12E0[m].values[0][3];
            position.y = lbl_805E12E0[m].values[1][3];
            position.z = lbl_805E12E0[m].values[2][3];
            src = &position;
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_800091D0(&s_roots[index], src);
                    s_rootSet[index] = 1;
                }
                continue;
            }
            s_done[index] = 1;
            parent = list->m_parents[index];
            if (parent == -1) {
                base = &result;
                dst = base;
            } else {
                base = &s_roots[parent];
                s_rootSet[parent] = 1;
                dst = base;
            }
            if (s_rootSet[index] != 0) {
                src = &s_roots[index];
            }
            k = index * 2;
            if (((MotionBlend *)entries[++k])->m_flags & 1) {
                if (m == 0) {
                    basePosition.x = lbl_805E12E0[4].values[0][3];
                    basePosition.y = lbl_805E12E0[4].values[1][3];
                    basePosition.z = lbl_805E12E0[4].values[2][3];
                    base = &basePosition;
                }
                Lerp(dst, src, base, lbl_805E14F0[index]);
            } else {
                fn_800091D0(dst, src);
            }
        }
    } while (again);
    fn_8040C244(lbl_805F13A8);
    fn_8040C148(&matrix, fn_80411700(lbl_805F13A8));
    matrix.values[0][3] = result.x;
    matrix.values[1][3] = result.y;
    matrix.values[2][3] = result.z;
    fn_8040C148(fn_80411700(lbl_805F13A8), &matrix);
}

/* Blended rotation channel of a root frame: rotates the layer matrices, then blends them. */
extern "C" void fn_8041ED8C(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    int n;
    int again;
    MotionNodeWorld *target;
    int m;
    Vec3f *angles;
    int i;
    MotionLayerList *list = args->m_list;
    void **entries = (void **)args->m_entries;
    Mtx34f matrix;
    Mtx34f result;
    int j;
    int index;
    int other;
    int parent;
    int k;
    int y;
    int z;
    int x;
    Mtx34f *dst;
    Mtx34f *base;
    Mtx34f *src;

    target = args->m_target;

    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    fn_8040C204(lbl_805F13A8);
    for (n = 0; n < list->m_count; n++) {
        index = list->m_order[n];
        if (index == -1) {
            break;
        }
        k = index * 2;
        angles = (Vec3f *)entries[k];
        if ((((MotionBlend *)entries[++k])->m_flags & 1) && n == 0) {
            fn_8040C148(&matrix, &target->m_matrix);
            matrix.values[0][3] = matrix.values[1][3] = matrix.values[2][3] = 0.0f;
            fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[4]);
            fn_8040BAC4(TopMatrix(), &matrix);
            fn_8040C148(&lbl_805E12E0[4], fn_80411700(lbl_805F13A8));
        }
        fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[n]);
        x = 65536.0f * angles->x / 6.2831855f;
        y = 65536.0f * angles->y / 6.2831855f;
        z = 65536.0f * angles->z / 6.2831855f;
        ROTATE_ZYX(x, y, z);
        fn_8040C148(&lbl_805E12E0[n], fn_80411700(lbl_805F13A8));
    }
    do {
        for (i = 0, again = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (again && list->m_parents[index] == -1) {
                s_isParent[index] = 1;
                continue;
            }
            s_isParent[index] = 0;
            for (j = i + 1; j < list->m_count; j++) {
                other = list->m_order[j];
                if (other == -1) {
                    break;
                }
                if (s_done[other] == 0 && index == list->m_parents[other]) {
                    s_isParent[index] = 1;
                    again = 1;
                    break;
                }
            }
        }
        for (m = 0; m < list->m_count; m++) {
            index = list->m_order[m];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_8040C148(&s_matrices[index], &lbl_805E12E0[m]);
                    s_rootSet[index] = 1;
                }
                continue;
            }
            s_done[index] = 1;
            parent = list->m_parents[index];
            if (parent == -1) {
                base = &result;
                dst = base;
            } else {
                base = &s_matrices[parent];
                s_rootSet[parent] = 1;
                dst = base;
            }
            if (s_rootSet[index] != 0) {
                src = &s_matrices[index];
            } else {
                src = &lbl_805E12E0[m];
            }
            k = index * 2;
            if (((MotionBlend *)entries[++k])->m_flags & 1) {
                if (m == 0) {
                    base = &lbl_805E12E0[4];
                }
                fn_8042062C(dst, src, base, lbl_805E14F0[index]);
            } else {
                fn_8040C148(dst, src);
            }
        }
    } while (again);
    fn_8040C244(lbl_805F13A8);
    fn_804113E8(0, &result);
}

/* Blended scale channel of a root frame: as fn_8041D89C, also applied to the layer matrices. */
extern "C" void fn_8041E9C4(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    void **entries = (void **)args->m_entries;
    int m;
    int again;
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    int i;
    Vec3f result;
    int j;
    int index;
    int other;
    int parent;
    int k;
    Vec3f *base;
    Vec3f *src;
    Vec3f *key;
    Vec3f *dst;

    result.x = result.y = result.z = 1.0f;
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        for (i = 0, again = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (again && list->m_parents[index] == -1) {
                s_isParent[index] = 1;
                continue;
            }
            s_isParent[index] = 0;
            for (j = i + 1; j < list->m_count; j++) {
                other = list->m_order[j];
                if (other == -1) {
                    break;
                }
                if (s_done[other] == 0 && index == list->m_parents[other]) {
                    s_isParent[index] = 1;
                    again = 1;
                    break;
                }
            }
        }
        for (m = 0; m < list->m_count; m++) {
            index = list->m_order[m];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            k = index * 2;
            key = (Vec3f *)entries[k];
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_800091D0(&s_roots[index], key);
                    s_rootSet[index] = 1;
                }
                continue;
            }
            s_done[index] = 1;
            parent = list->m_parents[index];
            if (parent == -1) {
                base = &result;
                dst = base;
            } else {
                base = &s_roots[parent];
                s_rootSet[parent] = 1;
                dst = base;
            }
            if (s_rootSet[index] != 0) {
                src = &s_roots[index];
            } else {
                src = key;
            }
            if (((MotionBlend *)entries[++k])->m_flags & 1) {
                if (m == 0) {
                    fn_80411328(&result, &target->m_matrix);
                    base = &result;
                }
                Lerp(dst, src, base, lbl_805E14F0[index]);
            } else {
                fn_800091D0(dst, src);
            }
        }
    } while (again);
    if (1.0f != result.x || 1.0f != result.y || 1.0f != result.z) {
        fn_8040C204(lbl_805F13A8);
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[i]);
            fn_8040B790(TopMatrix(), &result);
            fn_8040C148(&lbl_805E12E0[i], fn_80411700(lbl_805F13A8));
        }
        fn_8040C244(lbl_805F13A8);
        fn_8040B790(TopMatrix(), &result);
    }
}

/* Blended pre-scale channel of a root frame: as fn_8041D580, also applied to the layer matrices. */
extern "C" void fn_8041E618(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    void **entries = (void **)args->m_entries;
    int m;
    int again;
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    int i;
    Vec3f result;
    int j;
    int index;
    int other;
    int parent;
    int k;
    Vec3f *dst;
    Vec3f *base;
    Vec3f *src;

    result.x = result.y = result.z = 1.0f;
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        for (i = 0, again = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (again && list->m_parents[index] == -1) {
                s_isParent[index] = 1;
                continue;
            }
            s_isParent[index] = 0;
            for (j = i + 1; j < list->m_count; j++) {
                other = list->m_order[j];
                if (other == -1) {
                    break;
                }
                if (s_done[other] == 0 && index == list->m_parents[other]) {
                    s_isParent[index] = 1;
                    again = 1;
                    break;
                }
            }
        }
        for (m = 0; m < list->m_count; m++) {
            index = list->m_order[m];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            k = index * 2;
            src = (Vec3f *)entries[k];
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_800091D0(&s_roots[index], src);
                    s_rootSet[index] = 1;
                }
                continue;
            }
            s_done[index] = 1;
            parent = list->m_parents[index];
            if (parent == -1) {
                base = &result;
                dst = base;
            } else {
                base = &s_roots[parent];
                s_rootSet[parent] = 1;
                dst = base;
            }
            if (s_rootSet[index] != 0) {
                src = &s_roots[index];
            }
            if (((MotionBlend *)entries[++k])->m_flags & 1) {
                if (m == 0) {
                    base = &target->m_scale;
                }
                Lerp(dst, src, base, lbl_805E14F0[index]);
            } else {
                fn_800091D0(dst, src);
            }
        }
    } while (again);
    if (1.0f != result.x || 1.0f != result.y || 1.0f != result.z) {
        fn_8040C204(lbl_805F13A8);
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[i]);
            fn_8040B790(TopMatrix(), &result);
            fn_8040C148(&lbl_805E12E0[i], fn_80411700(lbl_805F13A8));
        }
        fn_8040C244(lbl_805F13A8);
        fn_8040B790(TopMatrix(), &result);
    }
}

/* Translation channel. */
extern "C" void fn_8041E5E0(void *data) {
    fn_8040B59C(TopMatrix(), *(Vec3f **)data);
}

/* Rotation channel (Euler angles). */
extern "C" void fn_8041E480(void *data) {
    Vec3f *angles = *(Vec3f **)data;
    int x = 65536.0f * angles->x / 6.2831855f;
    int y = 65536.0f * angles->y / 6.2831855f;
    int z = 65536.0f * angles->z / 6.2831855f;

    ROTATE_ZYX(x, y, z);
}

/* Scale channel. */
extern "C" void fn_8041E420(void *data) {
    Vec3f *scale = *(Vec3f **)data;

    if (1.0f != scale->x || 1.0f != scale->y || 1.0f != scale->z) {
        fn_8040B790(TopMatrix(), scale);
    }
}

/* Blended translation channel: blends the positions of the layers and applies them. */
extern "C" void fn_8041E124(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    void **entries = (void **)args->m_entries;
    int m;
    int again;
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    int i;
    Vec3f result;
    Vec3f position;
    int j;
    int index;
    int other;
    int parent;
    int k;
    Vec3f *dst;
    Vec3f *base;
    Vec3f *src;

    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        for (i = 0, again = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (again && list->m_parents[index] == -1) {
                s_isParent[index] = 1;
                continue;
            }
            s_isParent[index] = 0;
            for (j = i + 1; j < list->m_count; j++) {
                other = list->m_order[j];
                if (other == -1) {
                    break;
                }
                if (s_done[other] == 0 && index == list->m_parents[other]) {
                    s_isParent[index] = 1;
                    again = 1;
                    break;
                }
            }
        }
        for (m = 0; m < list->m_count; m++) {
            index = list->m_order[m];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            k = index * 2;
            src = (Vec3f *)entries[k];
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_800091D0(&s_roots[index], src);
                    s_rootSet[index] = 1;
                }
                continue;
            }
            s_done[index] = 1;
            parent = list->m_parents[index];
            if (parent == -1) {
                base = &result;
                dst = base;
            } else {
                base = &s_roots[parent];
                s_rootSet[parent] = 1;
                dst = base;
            }
            if (s_rootSet[index] != 0) {
                src = &s_roots[index];
            }
            if (((MotionBlend *)entries[++k])->m_flags & 1) {
                if (m == 0) {
                    position.x = target->m_matrix.values[0][3];
                    position.y = target->m_matrix.values[1][3];
                    position.z = target->m_matrix.values[2][3];
                    base = &position;
                }
                Lerp(dst, src, base, lbl_805E14F0[index]);
            } else {
                fn_800091D0(dst, src);
            }
        }
    } while (again);
    fn_8040B59C(TopMatrix(), &result);
}

/* Blended rotation channel: blends the Euler rotations of the layers as matrices. */
extern "C" void fn_8041DBD4(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    int m;
    int again;
    void **entries = (void **)args->m_entries;
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    int y;
    Mtx34f *dst;
    int x;
    int index;
    int z;
    Mtx34f *src;
    int k;
    Mtx34f matrix;
    Mtx34f result;
    int i;
    int j;
    int other;
    int parent;
    Vec3f *angles;

    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    fn_8040C204(lbl_805F13A8);
    do {
        for (i = 0, again = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (again && list->m_parents[index] == -1) {
                s_isParent[index] = 1;
                continue;
            }
            s_isParent[index] = 0;
            for (j = i + 1; j < list->m_count; j++) {
                other = list->m_order[j];
                if (other == -1) {
                    break;
                }
                if (s_done[other] == 0 && index == list->m_parents[other]) {
                    s_isParent[index] = 1;
                    again = 1;
                    break;
                }
            }
        }
        for (m = 0; m < list->m_count; m++) {
            index = list->m_order[m];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            k = index * 2;
            angles = (Vec3f *)entries[k];
            x = 65536.0f * angles->x / 6.2831855f;
            y = 65536.0f * angles->y / 6.2831855f;
            z = 65536.0f * angles->z / 6.2831855f;
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_8040B6F8(TopMatrix());
                    ROTATE_ZYX(x, y, z);
                    fn_8040C148(&s_matrices[index], fn_80411700(lbl_805F13A8));
                    s_rootSet[index] = 1;
                }
                continue;
            }
            s_done[index] = 1;
            parent = list->m_parents[index];
            if (parent == -1) {
                dst = &result;
            } else {
                dst = &s_matrices[parent];
                s_rootSet[parent] = 1;
            }
            if (s_rootSet[index] != 0) {
                src = &s_matrices[index];
            } else {
                fn_8040B6F8(TopMatrix());
                ROTATE_ZYX(x, y, z);
                fn_8040C148(&matrix, fn_80411700(lbl_805F13A8));
                src = &matrix;
            }
            if (((MotionBlend *)entries[++k])->m_flags & 1) {
                if (m == 0) {
                    fn_8041116C(dst, &target->m_matrix);
                }
                fn_8042062C(dst, src, dst, lbl_805E14F0[index]);
            } else {
                fn_8040C148(dst, src);
            }
        }
    } while (again);
    fn_8040C244(lbl_805F13A8);
    fn_8040BAC4(TopMatrix(), &result);
}

/* Blended scale channel: blends the scales of the layers and applies them. */
extern "C" void fn_8041D89C(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    void **entries = (void **)args->m_entries;
    int m;
    int again;
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    int i;
    Vec3f result;
    int j;
    int index;
    int other;
    int parent;
    int k;
    Vec3f *base;
    Vec3f *src;
    Vec3f *key;
    Vec3f *dst;

    result.x = result.y = result.z = 1.0f;
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        for (i = 0, again = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (again && list->m_parents[index] == -1) {
                s_isParent[index] = 1;
                continue;
            }
            s_isParent[index] = 0;
            for (j = i + 1; j < list->m_count; j++) {
                other = list->m_order[j];
                if (other == -1) {
                    break;
                }
                if (s_done[other] == 0 && index == list->m_parents[other]) {
                    s_isParent[index] = 1;
                    again = 1;
                    break;
                }
            }
        }
        for (m = 0; m < list->m_count; m++) {
            index = list->m_order[m];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            k = index * 2;
            key = (Vec3f *)entries[k];
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_800091D0(&s_roots[index], key);
                    s_rootSet[index] = 1;
                }
                continue;
            }
            s_done[index] = 1;
            parent = list->m_parents[index];
            if (parent == -1) {
                base = &result;
                dst = base;
            } else {
                base = &s_roots[parent];
                s_rootSet[parent] = 1;
                dst = base;
            }
            if (s_rootSet[index] != 0) {
                src = &s_roots[index];
            } else {
                src = key;
            }
            if (((MotionBlend *)entries[++k])->m_flags & 1) {
                if (m == 0) {
                    fn_80411328(&result, &target->m_matrix);
                    base = &result;
                }
                Lerp(dst, src, base, lbl_805E14F0[index]);
            } else {
                fn_800091D0(dst, src);
            }
        }
    } while (again);
    if (1.0f != result.x || 1.0f != result.y || 1.0f != result.z) {
        fn_8040B790(TopMatrix(), &result);
    }
}

/* Blended pre-scale channel: blends the scales of the layers and applies them. */
extern "C" void fn_8041D580(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    void **entries = (void **)args->m_entries;
    int m;
    int again;
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    int i;
    Vec3f result;
    int j;
    int index;
    int other;
    int parent;
    int k;
    Vec3f *dst;
    Vec3f *base;
    Vec3f *src;

    result.x = result.y = result.z = 1.0f;
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        for (i = 0, again = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (again && list->m_parents[index] == -1) {
                s_isParent[index] = 1;
                continue;
            }
            s_isParent[index] = 0;
            for (j = i + 1; j < list->m_count; j++) {
                other = list->m_order[j];
                if (other == -1) {
                    break;
                }
                if (s_done[other] == 0 && index == list->m_parents[other]) {
                    s_isParent[index] = 1;
                    again = 1;
                    break;
                }
            }
        }
        for (m = 0; m < list->m_count; m++) {
            index = list->m_order[m];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            k = index * 2;
            src = (Vec3f *)entries[k];
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_800091D0(&s_roots[index], src);
                    s_rootSet[index] = 1;
                }
                continue;
            }
            s_done[index] = 1;
            parent = list->m_parents[index];
            if (parent == -1) {
                base = &result;
                dst = base;
            } else {
                base = &s_roots[parent];
                s_rootSet[parent] = 1;
                dst = base;
            }
            if (s_rootSet[index] != 0) {
                src = &s_roots[index];
            }
            if (((MotionBlend *)entries[++k])->m_flags & 1) {
                if (m == 0) {
                    base = &target->m_scale;
                }
                Lerp(dst, src, base, lbl_805E14F0[index]);
            } else {
                fn_800091D0(dst, src);
            }
        }
    } while (again);
    if (1.0f != result.x || 1.0f != result.y || 1.0f != result.z) {
        fn_8040B790(TopMatrix(), &result);
    }
}

/* The model frame with key. */
extern "C" MotionModel *fn_8041D448(MotionModel *model, u32 key) {
    MotionModel *found;

    if (key == model->m_header->m_key) {
        return model;
    }
    if (model->m_child && (found = FindFrame(model->m_child, key))) {
        return found;
    }
    if (model->m_next && (found = FindFrame(model->m_next, key))) {
        return found;
    }
    return 0;
}

/* The node of the model frame with key. */
extern "C" MotionNode *fn_8041D31C(const MotionModel *model, u32 key) {
    MotionNode *found;

    if (key == model->m_header->m_key) {
        return model->m_output;
    }
    if (model->m_child && (found = FindNode(model->m_child, key))) {
        return found;
    }
    if (model->m_next && (found = FindNode(model->m_next, key))) {
        return found;
    }
    return 0;
}

/* Sets the callback of the model frame with key. */
extern "C" int fn_8041D1A0(MotionModel *model, u32 key, void (*callback)(MotionCallbackArgs *args),
                           u32 arg) {
    if (key == model->m_header->m_key) {
        model->m_callback = callback;
        model->field_0x20 = arg;
        return 1;
    }
    if (model->m_child && SetCallback(model->m_child, key, callback, arg)) {
        return 1;
    }
    if (model->m_next && SetCallback(model->m_next, key, callback, arg)) {
        return 1;
    }
    return 0;
}

/* Requests a weight scale for a layer in the model frame with key. */
extern "C" int fn_8041CFB0(const MotionModel *model, int index, u32 key, float scale, float offset) {
    MotionWeightScale *weightScale;

    if (key == model->m_header->m_key) {
        weightScale = &model->m_weightScales[index];
        weightScale->m_newScale = scale;
        weightScale->m_newOffset = offset;
        weightScale->m_pending = 1;
        return 1;
    }
    if (model->m_child && SetWeightScale(model->m_child, index, key, scale, offset)) {
        return 1;
    }
    if (model->m_next && SetWeightScale(model->m_next, index, key, scale, offset)) {
        return 1;
    }
    return 0;
}

/* Applies the pending weight scale of a layer in a model frame and the frames below and after
 * it. */
extern "C" void fn_8041CE74(const MotionModel *model, int index) {
    MotionWeightScale *scales = model->m_weightScales;

    if (scales[index].m_pending) {
        scales[index].m_enabled = 1;
        scales[index].m_scale = scales[index].m_newScale;
        scales[index].m_offset = scales[index].m_newOffset;
    }
    if (model->m_child) {
        ApplyWeightScale(model->m_child, index);
    }
    if (model->m_next) {
        ApplyWeightScale(model->m_next, index);
    }
}

/* Drops the pending weight scale of a layer in a model frame and the frames below and after it. */
extern "C" void fn_8041CD6C(const MotionModel *model, int index) {
    MotionWeightScale *scale = &model->m_weightScales[index];

    scale->m_pending = 0;
    scale->m_newScale = 1.0f;
    scale->m_newOffset = 0.0f;
    if (model->m_child) {
        DropWeightScale(model->m_child, index);
    }
    if (model->m_next) {
        DropWeightScale(model->m_next, index);
    }
}

/* Resets the weight scale of a layer in a model frame and all the frames below and after it. */
extern "C" void fn_8041CC18(const MotionModel *model, int index) {
    MotionWeightScale *scales = model->m_weightScales;
    MotionModel *child = model->m_child;

    scales[index].m_enabled = 0;
    scales[index].m_pending = 0;
    scales[index].m_scale = 1.0f;
    scales[index].m_offset = 0.0f;
    scales[index].m_newScale = 1.0f;
    scales[index].m_newOffset = 0.0f;
    if (child) {
        ResetWeightScale(child, index);
    }
    if (model->m_next) {
        ResetWeightScale(model->m_next, index);
    }
}

/* Adds up the source and channel storage and the nodes a body tree needs for layers layers. */
extern "C" void fn_8041CA78(s32 *sourceSize, u32 *channelSize, s32 *nodeCount, int layers,
                            MotionBody *body) {
    u32 channels;
    u32 sources = 0;

    channels = 0x3C;
    if (body->m_flags & 0x400) {
        channels = (layers * 2 + 2) * 4 + 0x3C;
        sources = 0xE0;
    }
    channels += 8;
    if (body->m_flags & 7) {
        sources += 0xF0;
        if (body->m_type == 4) {
            channels += (layers * 2 + 5) * 4;
        } else {
            channels += (layers * 2 + 2) * 4;
        }
    } else {
        channels += 4;
    }
    channels += 8;
    if ((body->m_flags & 0x40) || (body->m_flags & 0x38)) {
        sources += 0xF0;
        channels += (layers * 2 + 2) * 4;
    } else {
        channels += 4;
    }
    channels += 8;
    if (body->m_flags & 0x380) {
        sources += 0xE0;
        channels += (layers * 2 + 2) * 4;
    } else {
        channels += 4;
    }
    switch (body->m_type) {
    case 3:
    case 4:
        (*nodeCount)++;
        break;
    default:
        if (body->m_flags != 0 || (body->field_0x8 & 4)) {
            (*nodeCount)++;
        }
        break;
    }
    *sourceSize += sources;
    *channelSize += channels;
    if (body->m_child) {
        fn_8041CA78(sourceSize, channelSize, nodeCount, layers, body->m_child);
    }
    if (body->m_next) {
        fn_8041CA78(sourceSize, channelSize, nodeCount, layers, body->m_next);
    }
}

/* Links the sources of a body tree into a list and sets their update functions. */
extern "C" void fn_8041C854(MotionSource **cursor, MotionSource **last, MotionBody *body) {
    MotionSource *next;
    MotionSource *source;

    if (body->m_flags & 0x400) {
        (*cursor)->m_update = fn_80420308;
        source = *cursor;
        source->m_next = 0;
        next = (MotionSource *)((u8 *)source + 0xE0);
        if (*last) {
            (*last)->m_next = *cursor;
        }
        *last = *cursor;
        *cursor = next;
    }
    if (body->m_flags & 7) {
        (*cursor)->m_update = fn_804204C8;
        (*cursor)->m_update2 = fn_804202B8;
        (*cursor)->m_next = 0;
        source = *cursor;
        source->m_rootMotion.m_link[0] = body;
        next = (MotionSource *)((u8 *)source + 0xF0);
        if (*last) {
            (*last)->m_next = *cursor;
        }
        *last = *cursor;
        *cursor = next;
    }
    if (body->m_flags & 0x40) {
        (*cursor)->m_update = fn_80420438;
        (*cursor)->m_update2 = fn_8041FF08;
        (*cursor)->m_next = 0;
        source = *cursor;
        source->m_rootMotion.m_body = body;
        next = (MotionSource *)((u8 *)source + 0xF0);
        if (*last) {
            (*last)->m_next = *cursor;
        }
        *last = *cursor;
        *cursor = next;
    } else if (body->m_flags & 0x38) {
        (*cursor)->m_update = fn_8042043C;
        (*cursor)->m_update2 = fn_80420158;
        (*cursor)->m_next = 0;
        source = *cursor;
        source->m_rootMotion.m_body = body;
        next = (MotionSource *)((u8 *)source + 0xF0);
        if (*last) {
            (*last)->m_next = *cursor;
        }
        *last = *cursor;
        *cursor = next;
    }
    if (body->m_flags & 0x380) {
        (*cursor)->m_update = fn_804203AC;
        (*cursor)->m_next = 0;
        next = (MotionSource *)((u8 *)*cursor + 0xE0);
        if (*last) {
            (*last)->m_next = *cursor;
        }
        *last = *cursor;
        *cursor = next;
    }
    if (body->m_child) {
        fn_8041C854(cursor, last, body->m_child);
    }
    if (body->m_next) {
        fn_8041C854(cursor, last, body->m_next);
    }
}

/* The body with key in a tree, moving cursor past the sources of the bodies before it. */
extern "C" MotionBody *fn_8041C5A8(MotionBody *body, MotionSource **cursor, u32 key) {
    MotionBody *found;

    if (key == body->m_key) {
        return body;
    }
    SKIP_SOURCES(body, cursor);
    if (body->m_child && (found = FindBody(body->m_child, cursor, key))) {
        return found;
    }
    if (body->m_next && (found = FindBody(body->m_next, cursor, key))) {
        return found;
    }
    return 0;
}

/* Links the sources of mirrored ("_l"/"_r") bodies so that the left one follows the right one. */
extern "C" void fn_8041C2E0(MotionBody *body, MotionBody *root, MotionSource **cursor,
                            MotionSource *first) {
    MotionSource *other;
    char name[36];
    MotionBody *mirror;
    u32 key;
    MotionSource *source;
    char *suffix;

    if (body->field_0xA & 0x8000) {
        strcpy(name, body->m_name);
        suffix = fn_803AC6C0(name, "_l");
        suffix[1] = 'r';
        key = fn_80410684(name);
        other = first;
        mirror = fn_8041C5A8(root, &other, key);
        if (mirror) {
            if (body->m_flags & 0x400) {
                *cursor = (*cursor)->m_next;
            }
            if (mirror->m_flags & 0x400) {
                other = other->m_next;
            }
            if (body->m_flags & 7) {
                source = *cursor;
                source->m_rootMotion.m_link[1] = mirror;
                source->m_rootMotion.m_link[2] = other;
                (*cursor)->m_update2 = fn_804201C0;
                other->m_update2 = 0;
                if (mirror->m_flags & 7) {
                    other = other->m_next;
                }
                *cursor = (*cursor)->m_next;
            } else if (mirror->m_flags & 7) {
                other = other->m_next;
            }
            if ((body->m_flags & 0x40) || (mirror->m_flags & 0x40)) {
                if (body->m_flags & 0x40) {
                    *cursor = (*cursor)->m_next;
                }
                if (mirror->m_flags & 0x40) {
                    other = other->m_next;
                }
            } else if (body->m_flags & 0x38) {
                source = *cursor;
                source->m_rootMotion.m_link[0] = mirror;
                source->m_rootMotion.m_link[1] = other;
                (*cursor)->m_update2 = fn_8041FF0C;
                other->m_update2 = 0;
                if (mirror->m_flags & 0x38) {
                    other = other->m_next;
                }
                *cursor = (*cursor)->m_next;
            } else if (mirror->m_flags & 0x38) {
                other = other->m_next;
            }
            if (body->m_flags & 0x380) {
                *cursor = (*cursor)->m_next;
            }
            if (mirror->m_flags & 0x380) {
                other = other->m_next;
            }
        }
    } else {
        SKIP_SOURCES(body, cursor);
    }
    if (body->m_child) {
        fn_8041C2E0(body->m_child, root, cursor, first);
    }
    if (body->m_next) {
        fn_8041C2E0(body->m_next, root, cursor, first);
    }
}

/* Builds the model frame of a bone body and of the bodies below and after it: its node, its
 * channels and their per-layer key storage. */
extern "C" void fn_8041BE30(MotionPlayWork *work, MotionModel **cursor, MotionBlend *blends,
                            u8 **scales, MotionNode **nodes, MotionSource **sources,
                            MotionBody *body) {
    MotionModel *model = *cursor;
    int count;
    MotionNodeWorld *target;
    MotionCallback *channel;
    MotionSource *source;
    MotionRootMotion *motion;
    MotionChannelLayer *entry;
    int i;

    model->m_header = body;
    model->field_0x0 = 0;
    model->m_layerWeights = blends;
    model->m_weightScales = (MotionWeightScale *)*scales;
    model->m_callbacks = (MotionCallback *)(model + 1);
    count = work->m_layerCount;
    *scales += count * 32;
    switch (body->m_type) {
    case 3:
    case 4:
        model->m_output = *nodes;
        target = &model->m_output->m_world;
        (*nodes)++;
        break;
    default:
        if (body->m_flags != 0 || (body->field_0x8 & 4)) {
            model->m_output = *nodes;
            target = &model->m_output->m_world;
            (*nodes)++;
        } else {
            model->m_output = 0;
            target = 0;
        }
        break;
    }
    channel = model->m_callbacks;
    if (body->m_flags & 0x400) {
        model->field_0x0 |= 0x400;
        entry = channel->m_layers;
        channel->m_data = &work->m_layerCount;
        channel->m_target = target;
        for (i = 0; i < count; i++) {
            source = sources[i];
            sources[i] = source->m_next;
            motion = &source->m_rootMotion;
            entry->m_data = (u8 *)motion;
            motion->m_position.x = 1.0f;
            motion->m_position.y = 1.0f;
            motion->m_position.z = 1.0f;
            entry->m_blend = &blends[i];
            entry++;
        }
        channel->m_function = fn_8041FAD8;
        channel->m_next = (MotionCallback *)entry;
        channel = channel->m_next;
    }
    if (body->m_flags & 7) {
        model->field_0x0 |= 7;
        entry = channel->m_layers;
        channel->m_data = &work->m_layerCount;
        channel->m_target = target;
        for (i = 0; i < count; i++) {
            source = sources[i];
            sources[i] = source->m_next;
            entry->m_data = (u8 *)&source->m_rootMotion;
            memcpy(&source->m_rootMotion, &body->m_position, sizeof(Vec3f));
            entry->m_blend = &blends[i];
            entry++;
        }
        if (body->m_type == 4) {
            if (body->m_child->m_type == 4) {
                ((void **)entry)[0] = *nodes;
                ((void **)entry)[1] = &body->m_child->m_position;
                ((void **)entry)[2] = &body->m_child->m_child->m_position;
                entry = (MotionChannelLayer *)((u8 *)entry + 0xC);
            } else {
                ((void **)entry)[0] = 0;
                ((void **)entry)[1] = &body->m_child->m_position;
                ((void **)entry)[2] = 0;
                entry = (MotionChannelLayer *)((u8 *)entry + 0xC);
            }
        }
        if (body->m_type != 4) {
            channel->m_function = fn_8041FCC0;
        }
    } else {
        channel->m_data = &body->m_position;
        entry = (MotionChannelLayer *)((u8 *)channel + 0xC);
        if (body->m_type != 4) {
            channel->m_function = fn_8041FED0;
        }
    }
    channel->m_next = (MotionCallback *)entry;
    channel = channel->m_next;
    if ((body->m_flags & 0x40) || (body->m_flags & 0x38)) {
        if (body->m_flags & 0x40) {
            model->field_0x0 |= 0x40;
        }
        if (body->m_flags & 0x38) {
            model->field_0x0 |= 0x38;
        }
        channel->m_data = &work->m_layerCount;
        entry = channel->m_layers;
        channel->m_target = target;
        for (i = 0; i < count; i++) {
            source = sources[i];
            sources[i] = source->m_next;
            entry->m_data = (u8 *)&source->m_rootMotion;
            memcpy(&source->m_rootMotion, body->m_rotation, 0x10);
            entry->m_blend = &blends[i];
            entry++;
        }
        channel->m_function = fn_8041FB4C;
    } else {
        channel->m_data = body->m_rotation;
        entry = (MotionChannelLayer *)((u8 *)channel + 0xC);
        switch (body->field_0xC) {
        case 0:
        default:
            channel->m_function = fn_8041FD70;
            break;
        case 6:
            channel->m_function = fn_8041FD6C;
            break;
        }
    }
    channel->m_next = (MotionCallback *)entry;
    channel = channel->m_next;
    if (body->m_flags & 0x380) {
        model->field_0x0 |= 0x380;
        channel->m_data = &work->m_layerCount;
        entry = channel->m_layers;
        channel->m_target = target;
        for (i = 0; i < count; i++) {
            source = sources[i];
            sources[i] = source->m_next;
            entry->m_data = (u8 *)&source->m_rootMotion;
            memcpy(&source->m_rootMotion, &body->m_scale, sizeof(Vec3f));
            entry->m_blend = &blends[i];
            entry++;
        }
        channel->m_function = fn_8041FAD8;
    } else {
        channel->m_data = &body->m_scale;
        channel->m_function = fn_8041FD0C;
        entry = (MotionChannelLayer *)((u8 *)channel + 0xC);
    }
    channel->m_next = 0;
    *cursor = (MotionModel *)entry;
    if (body->m_child) {
        model->m_child = *cursor;
        fn_8041BE30(work, cursor, blends, scales, nodes, sources, body->m_child);
    }
    if (body->m_next) {
        model->m_next = *cursor;
        fn_8041BE30(work, cursor, blends, scales, nodes, sources, body->m_next);
    }
}

/* A matrix copy whose arguments are taken before the call is set up: the destination is computed
 * into its own register and moved afterwards, as retail does for one copy in fn_8041B980. */
inline void CopyMatrix(Mtx34f *dst, const Mtx34f *src) {
    fn_8040C148(dst, src);
}

/* Allocates and builds a play work with layers layers (1 to 4) for bone data. */
extern "C" MotionPlayWork *fn_8041B980(MotionData *data, s32 layers) {
    MotionSource *sources[4];
    s32 sourceSize;
    u32 channelSize;
    s32 nodeCount;
    MotionSource *last;
    MotionSource *source;
    MotionSource *first;
    MotionModel *model;
    MotionNode *nodes;
    u8 *scales;
    MotionPlayWork *work;
    int count = layers;
    u8 *slots;
    u8 *parents;
    u8 *p;
    u8 *end;
    MotionBlend *blends;
    MotionNode *node;
    u8 *tail;
    int i;
    int orderSize;
    int slotSize;
    int blendSize;
    MotionBody *body = &data->m_body;

    sourceSize = 0;
    channelSize = 0;
    nodeCount = 0;
    if (count < 1) {
        count = 1;
    } else if (count > 4) {
        count = 4;
    }
    fn_8041CA78(&sourceSize, &channelSize, &nodeCount, count, body);
    orderSize = count * 8;
    slotSize = count * 4;
    blendSize = count * sizeof(MotionBlend);
    work = (MotionPlayWork *)fn_80411134(
        1, sizeof(MotionPlayWork) + nodeCount * sizeof(Mtx34f) * 2 + nodeCount * 0x10 + count * sizeof(MotionLayer) +
               sourceSize * count + orderSize + slotSize + orderSize +
               blendSize + count * (data->field_0x6 * 32) + channelSize);
    if (!work) {
        fn_803A953C("Cannot get PlayWork.\n");
        return 0;
    }
    work->m_skeleton = data;
    work->m_nodeCount = nodeCount;
    work->m_nodes = (MotionNode *)(work + 1);
    work->m_layers = (MotionLayer *)(work->m_nodes + work->m_nodeCount);
    p = (u8 *)(work->m_layers + count);
    for (i = 0; i < count; i++) {
        if (sourceSize) {
            work->m_layers[i].m_source = (MotionSource *)p;
            p += sourceSize;
        } else {
            work->m_layers[i].m_source = 0;
        }
    }
    work->m_layerCount = count;
    slots = p + orderSize;
    parents = slots + slotSize;
    end = parents + orderSize;
    work->m_order = (s32 *)p;
    work->m_slots = (s32 *)slots;
    work->m_parents = (s32 *)parents;
    for (i = 0; i < count * 2; i++) {
        work->m_order[i] = -1;
        work->m_parents[i] = -1;
    }
    for (i = 0; i < count; i++) {
        work->m_slots[i] = -1;
    }
    blends = (MotionBlend *)end;
    work->m_order[0] = 0;
    work->m_slots[0] = 0;
    for (i = 0; i < count; i++) {
        work->m_layers[i].m_blend = &blends[i];
        work->m_layers[i].m_frames.m_speed = 1.0f;
        work->m_layers[i].m_request.m_speed = 1.0f;
        work->m_layers[i].m_request.m_frame = -1.0f;
        work->m_layers[i].m_request.m_start = -1.0f;
        work->m_layers[i].m_request.m_end = -1.0f;
    }
    tail = end + blendSize;
    work->m_model = (MotionModel *)(tail + count * (data->field_0x6 * 32));
    for (i = 0; i < count; i++) {
        first = work->m_layers[i].m_source;
        source = first;
        last = 0;
        if (first) {
            fn_8041C854(&source, &last, body);
            source = first;
            fn_8041C2E0(body, body, &source, first);
        }
    }
    scales = tail;
    nodes = work->m_nodes;
    model = work->m_model;
    for (i = 0; i < work->m_layerCount; i++) {
        sources[i] = work->m_layers[i].m_source;
    }
    fn_8041BE30(work, &model, blends, &scales, &nodes, sources, body);
    fn_8040C204(lbl_805F13A8);
    fn_8040B6F8(TopMatrix());
    node = work->m_nodes;
    for (int n = 0; n < work->m_nodeCount; n++) {
        fn_8040C148(&node->m_matrix, fn_80411700(lbl_805F13A8));
        fn_8040C148(&node->m_world.m_matrix, fn_80411700(lbl_805F13A8));
        node++;
    }
    fn_8040C148(&work->m_matrix, fn_80411700(lbl_805F13A8));
    fn_8040C148(&work->m_prevMatrix, fn_80411700(lbl_805F13A8));
    fn_8040C148(&work->m_rootMatrix, fn_80411700(lbl_805F13A8));
    fn_8040C148(&work->m_prevRootMatrix, fn_80411700(lbl_805F13A8));
    for (int n = 0; n < count; n++) {
        CopyMatrix(&work->m_layers[n].m_prevMatrix, fn_80411700(lbl_805F13A8));
        fn_8040C148(&work->m_layers[n].m_matrix, fn_80411700(lbl_805F13A8));
        fn_8040C148(&work->m_layers[n].m_baseMatrix, fn_80411700(lbl_805F13A8));
    }
    fn_8040C244(lbl_805F13A8);
    for (int n = 0; n < count; n++) {
        fn_8041CC18(work->m_model, n);
    }
    return work;
}

/* Creates a play work for the bone data of key. */
extern "C" MotionPlayWork *fn_8041B908(u32 key, int layers) {
    MotionData *data = fn_804106FC(key);

    if (!data) {
        fn_803A953C("Cannot find bone data.\n");
        return 0;
    }
    if ((int)data->m_type != 0xFF) {
        fn_803A953C("Illegal bonetype.\n");

        return 0;
    }
    return fn_8041B980(data, layers);
}

/* Releases a play work and the motions of its playing layers. */
extern "C" void fn_8041B884(MotionPlayWork *work) {
    int i;
    int index;
    MotionLayer *layer;

    if (!work) {
        return;
    }
    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        if (layer->m_flags & 4) {
            fn_80413240(layer->m_motion);
        }
    }
    fn_804110FC(work);
}

/* Clears the key tracks of a layer in a model frame and all the frames below and after it. */
extern "C" void fn_8041B6FC(MotionModel *model, int index) {
    MotionCallback *channel = model->m_callbacks;
    u8 *entry;
    HalfKeyTrack *tracks;
    Vec3f *scale;
    MotionChannelLayer *slot;

    if (model->field_0x0 & 0x400) {
        slot = channel->m_layers;
        slot += index;
        entry = slot->m_data;
        scale = (Vec3f *)entry;
        tracks = (HalfKeyTrack *)(entry + 0x10);
        scale->x = scale->y = scale->z = 1.0f;
        fn_80413E10(&tracks[0]);
        fn_80413E10(&tracks[1]);
        fn_80413E10(&tracks[2]);
        channel = channel->m_next;
    }
    if (model->field_0x0 & 7) {
        slot = channel->m_layers;
        slot += index;
        entry = slot->m_data;
        memcpy(entry, &model->m_header->m_position, sizeof(Vec3f));
        tracks = (HalfKeyTrack *)(entry + 0x20);
        fn_80413E10(&tracks[0]);
        fn_80413E10(&tracks[1]);
        fn_80413E10(&tracks[2]);
    }
    channel = channel->m_next;
    if (!(model->field_0x0 & 0x40) && (model->field_0x0 & 0x38)) {
        slot = channel->m_layers;
        slot += index;
        entry = slot->m_data;
        memcpy(entry, &model->m_header->m_rotation, 0x10);
        tracks = (HalfKeyTrack *)(entry + 0x20);
        fn_80413E10(&tracks[0]);
        fn_80413E10(&tracks[1]);
        fn_80413E10(&tracks[2]);
    }
    channel = channel->m_next;
    if (model->field_0x0 & 0x380) {
        slot = channel->m_layers;
        slot += index;
        entry = slot->m_data;
        memcpy(entry, &model->m_header->m_scale, sizeof(Vec3f));
        tracks = (HalfKeyTrack *)(entry + 0x10);
        fn_80413E10(&tracks[0]);
        fn_80413E10(&tracks[1]);
        fn_80413E10(&tracks[2]);
    }
    if (model->m_child) {
        fn_8041B6FC(model->m_child, index);
    }
    if (model->m_next) {
        fn_8041B6FC(model->m_next, index);
    }
}

/* Starts the key tracks of a layer in the channels of the model frames a motion body animates. */
extern "C" void fn_8041B2BC(MotionPlayWork *work, MotionBody *body, int index, u16 motion,
                            float ratio) {
    MotionChannelLayer *slot;
    MotionModel *frame = fn_8041D448(work->m_model, body->m_key);
    MotionCallback *channel = 0;
    u8 *entry;
    HalfKeyTrack *tracks;

    if (frame) {
        channel = frame->m_callbacks;
    }
    if (body->m_flags & 0x400) {
        if (frame && (frame->field_0x0 & 0x400)) {
            slot = channel->m_layers;
            slot += index;
            entry = slot->m_data;
            tracks = (HalfKeyTrack *)(entry + 0x10);
            memcpy(entry, &frame->m_header->m_scale, sizeof(Vec3f));
            fn_80413E38(&tracks[0], fn_804124DC(motion));
            fn_80413E38(&tracks[1], fn_804124DC(0));
            fn_80413E38(&tracks[2], fn_804124DC(0));
            channel = channel->m_next;
            motion = 0;
        } else {
            fn_804124DC(motion);
            fn_804124DC(0);
            fn_804124DC(0);
            motion = 0;
        }
    }
    if (body->m_flags & 7) {
        if (frame && (frame->field_0x0 & 7)) {
            slot = channel->m_layers;
            slot += index;
            entry = slot->m_data;
            *(float *)(entry + 0x10) = ratio;
            tracks = (HalfKeyTrack *)(entry + 0x20);
            memcpy(entry, &frame->m_header->m_position, sizeof(Vec3f));
            if (body->m_flags & 1) {
                fn_80413E38(&tracks[0], fn_804124DC(motion));
                motion = 0;
            }
            if (body->m_flags & 2) {
                fn_80413E38(&tracks[1], fn_804124DC(motion));
                motion = 0;
            }
            if (body->m_flags & 4) {
                fn_80413E38(&tracks[2], fn_804124DC(motion));
                motion = 0;
            }
        } else {
            if (body->m_flags & 1) {
                fn_804124DC(motion);
                motion = 0;
            }
            if (body->m_flags & 2) {
                fn_804124DC(motion);
                motion = 0;
            }
            if (body->m_flags & 4) {
                fn_804124DC(motion);
                motion = 0;
            }
        }
    }
    if (channel) {
        channel = channel->m_next;
    }
    if (!(body->m_flags & 0x40) && (body->m_flags & 0x38)) {
        if (frame && (frame->field_0x0 & 0x38)) {
            slot = channel->m_layers;
            slot += index;
            entry = slot->m_data;
            tracks = (HalfKeyTrack *)(entry + 0x20);
            memcpy(entry, &frame->m_header->m_rotation, 0x10);
            if (body->m_flags & 8) {
                fn_80413E38(&tracks[0], fn_804124DC(motion));
                motion = 0;
            }
            if (body->m_flags & 0x10) {
                fn_80413E38(&tracks[1], fn_804124DC(motion));
                motion = 0;
            }
            if (body->m_flags & 0x20) {
                fn_80413E38(&tracks[2], fn_804124DC(motion));
                motion = 0;
            }
        } else {
            if (body->m_flags & 8) {
                fn_804124DC(motion);
                motion = 0;
            }
            if (body->m_flags & 0x10) {
                fn_804124DC(motion);
                motion = 0;
            }
            if (body->m_flags & 0x20) {
                fn_804124DC(motion);
                motion = 0;
            }
        }
    }
    if (channel) {
        channel = channel->m_next;
    }
    if (body->m_flags & 0x380) {
        if (frame && (frame->field_0x0 & 0x380)) {
            slot = channel->m_layers;
            slot += index;
            entry = slot->m_data;
            tracks = (HalfKeyTrack *)(entry + 0x10);
            memcpy(entry, &frame->m_header->m_scale, sizeof(Vec3f));
            if (body->m_flags & 0x80) {
                fn_80413E38(&tracks[0], fn_804124DC(motion));
                motion = 0;
            }
            if (body->m_flags & 0x100) {
                fn_80413E38(&tracks[1], fn_804124DC(motion));
                motion = 0;
            }
            if (body->m_flags & 0x200) {
                fn_80413E38(&tracks[2], fn_804124DC(motion));
                motion = 0;
            }
        } else {
            if (body->m_flags & 0x80) {
                fn_804124DC(motion);
                motion = 0;
            }
            if (body->m_flags & 0x100) {
                fn_804124DC(motion);
                motion = 0;
            }
            if (body->m_flags & 0x200) {
                fn_804124DC(motion);
                motion = 0;
            }
        }
    }
    if (body->m_child) {
        fn_8041B2BC(work, body->m_child, index, motion, ratio);
    }
    if (body->m_next) {
        fn_8041B2BC(work, body->m_next, index, motion, ratio);
    }
}

/* Loads a motion into a layer. */
extern "C" int fn_8041B170(MotionPlayWork *work, int index, u16 motion) {
    float ratio = -1.0f;
    u32 key;
    MotionData *data;
    MotionBody *body;
    MotionFrames *frames = &work->m_layers[index].m_frames;

    fn_8041B6FC(work->m_model, index);

    if (!fn_804125F4(frames, motion)) {
        work->m_layers[index].m_minFrame = 0.0f;
        work->m_layers[index].m_maxFrame = 1.0f;
        return 0;
    }
    work->m_layers[index].m_minFrame = frames->m_start;
    work->m_layers[index].m_maxFrame = frames->m_end;
    fn_8041575C((const float *)frames);
    key = fn_804123DC(motion);
    data = fn_804106FC(key);
    if (!data) {
        return 0;
    }
    body = &data->m_body;
    if (key != work->m_skeleton->m_key && std::fabs(data->m_scale) > 0.0001f &&
        std::fabs(work->m_skeleton->m_scale) > 0.0001f) {
        ratio = work->m_skeleton->m_scale / data->m_scale;
    }
    fn_8041B2BC(work, body, index, motion, ratio);
    return 1;
}

/* Restarts the tracks of a layer in the channels of a model frame. */
extern "C" void fn_8041AFCC(MotionModel *model, int index) {
    MotionCallback *channel = model->m_callbacks;
    HalfKeyTrack *tracks;
    MotionChannelLayer *entry;

    if (model->field_0x0 & 0x400) {
        entry = channel->m_layers;
        entry += index;
        tracks = (HalfKeyTrack *)(entry->m_data + 0x10);
        RESET_TRACKS(tracks);
        channel = channel->m_next;
    }
    if (model->field_0x0 & 7) {
        entry = channel->m_layers;
        entry += index;
        tracks = (HalfKeyTrack *)(entry->m_data + 0x20);
        RESET_TRACKS(tracks);
    }
    channel = channel->m_next;
    if (!(model->field_0x0 & 0x40) && (model->field_0x0 & 0x38)) {
        entry = channel->m_layers;
        entry += index;
        tracks = (HalfKeyTrack *)(entry->m_data + 0x20);
        RESET_TRACKS(tracks);
    }
    channel = channel->m_next;
    if (model->field_0x0 & 0x380) {
        entry = channel->m_layers;
        entry += index;
        tracks = (HalfKeyTrack *)(entry->m_data + 0x10);
        RESET_TRACKS(tracks);
    }
}

/* Restarts the tracks of a layer in a model frame and all the frames below and after it. */
extern "C" void fn_8041AF1C(MotionModel *model, int index) {
    MotionModel *other;

    fn_8041AFCC(model, index);
    other = model->m_child;
    if (other) {
        fn_8041AFCC(other, index);
        if (other->m_child) {
            fn_8041AF1C(other->m_child, index);
        }
        if (other->m_next) {
            fn_8041AF1C(other->m_next, index);
        }
    }
    other = model->m_next;
    if (other) {
        fn_8041AFCC(other, index);
        if (other->m_child) {
            fn_8041AF1C(other->m_child, index);
        }
        if (other->m_next) {
            fn_8041AF1C(other->m_next, index);
        }
    }
}

/* Restarts the tracks of a layer when its frame jumped. */
extern "C" void fn_8041AED0(MotionPlayWork *work, int index) {
    fn_8041575C((const float *)&work->m_layers[index].m_frames);
    fn_8041AF1C(work->m_model, index);
}

/* Applies a request to frames, within [minFrame, maxFrame]; bit 0 of the result is set when the
 * play direction or the frame jumped, bit 4 when the frame was moved. */
extern "C" u32 fn_8041AD94(MotionFrames *frames, MotionFrames *request, float minFrame,
                           float maxFrame) {
    u32 result = 0;
    float speed;
    float start;
    float end;
    float frame;

    if (frames->m_speed * (speed = request->m_speed) < 0.0001f) {
        result |= 1;
    }
    frames->m_speed = speed;
    if (request->m_start < 0.0f) {
        start = frames->m_start;
    } else {
        if (minFrame > request->m_start) {
            request->m_start = minFrame;
        }
        start = request->m_start;
    }
    if (request->m_end < 0.0f) {
        end = frames->m_end;
    } else {
        if (maxFrame < request->m_end) {
            request->m_end = maxFrame;
        }
        end = request->m_end;
    }
    if (end < start) {
        end = start;
    }
    if (request->m_frame < 0.0f) {
        frame = frames->m_frame;
        if (frame < start) {
            frame = start;
            result |= 0x10;
            if (speed < 0.0001f) {
                result |= 1;
            }
        }
        if (frame > end) {
            frame = end;
            result |= 0x10;
            if (speed > -0.0001f) {
                result |= 1;
            }
        }
    } else {
        frame = request->m_frame;
        if (frame < start) {
            frame = start;
        }
        if (frame > end) {
            frame = end;
        }
        if ((speed < 0.0001f && frame > frames->m_frame) ||
            (speed > -0.0001f && frame < frames->m_frame)) {
            result |= 1;
        }
        result |= 0x10;
    }
    frames->m_start = start;
    frames->m_end = end;
    frames->m_frame = frame;
    return result;
}

/* Keeps a layer blended (type 1) onto its parent in step: its requested frame or speed is
 * passed on to the parent, scaled to the parent's frame range. */
extern "C" void fn_8041AB3C(MotionPlayWork *work, int index) {
    int type = 3;
    MotionLayer *layer = &work->m_layers[index];
    MotionLayer *base;
    int parent;
    u32 flags;
    float minFrame;
    float maxFrame;
    MotionFrames baseFrames;
    MotionFrames frames;

    if (work->m_layerCount == 1) {
        return;
    }
    parent = work->m_parents[index];
    if (parent == -1) {
        return;
    }
    flags = layer->m_flags2;
    if (flags & 0x200) {
        type = layer->m_blend->m_mode;
    } else if (layer->m_blend->m_flags & 1) {
        type = layer->m_blend->m_type;
    }
    if (type != 1) {
        return;
    }
    if (!(flags & 0x40) && !(flags & 0x20)) {
        return;
    }
    base = &work->m_layers[parent];
    if (!(base->m_flags & 4) && !(base->m_flags2 & 0x100)) {
        return;
    }
    if (layer->m_newMotion) {
        fn_804125F4(&frames, layer->m_newMotion);
        minFrame = frames.m_start;
        maxFrame = frames.m_end;
    } else if (flags & 0x100) {
        fn_804125F4(&frames, 1);
        minFrame = frames.m_start;
        maxFrame = frames.m_end;
    } else {
        frames.m_start = layer->m_frames.m_start;
        frames.m_end = layer->m_frames.m_end;
        frames.m_speed = layer->m_frames.m_speed;
        frames.m_frame = layer->m_frames.m_frame;
        minFrame = layer->m_minFrame;
        maxFrame = layer->m_maxFrame;
    }
    fn_8041AD94(&frames, &layer->m_request, minFrame, maxFrame);
    if (base->m_newMotion) {
        fn_804125F4(&baseFrames, base->m_newMotion);
        minFrame = baseFrames.m_start;
        maxFrame = baseFrames.m_end;
    } else if (base->m_flags2 & 0x100) {
        fn_804125F4(&baseFrames, 1);
        minFrame = baseFrames.m_start;
        maxFrame = baseFrames.m_end;
    } else {
        baseFrames.m_start = base->m_frames.m_start;
        baseFrames.m_end = base->m_frames.m_end;
        baseFrames.m_speed = base->m_frames.m_speed;
        baseFrames.m_frame = base->m_frames.m_frame;
        minFrame = base->m_minFrame;
        maxFrame = base->m_maxFrame;
    }
    if (base->m_flags2 & 0x78) {
        fn_8041AD94(&baseFrames, &base->m_request, minFrame, maxFrame);
    }
    if (frames.m_end - frames.m_start < 0.0001f) {
        return;
    }
    if (layer->m_flags2 & 0x20) {
        base->m_request.m_frame = (baseFrames.m_end - baseFrames.m_start) *
                                      ((frames.m_frame - frames.m_start) / (frames.m_end - frames.m_start)) +
                                  baseFrames.m_start;
        base->m_flags2 |= 1;
        base->m_flags2 |= 0x20;
    }
    if (layer->m_flags2 & 0x40) {
        base->m_request.m_speed = frames.m_speed * ((baseFrames.m_end - baseFrames.m_start) /
                                                    (frames.m_end - frames.m_start));
        base->m_flags2 |= 1;
        base->m_flags2 |= 0x40;
    }
}

/* Walks a model frame and its children and siblings, running their channels and storing their
 * matrices. */
extern "C" void fn_8041A83C(MotionPlayWork *work, MotionModel *model) {
    int root = 0;
    MotionCallback *channel = model->m_callbacks;
    int scaled = 0;
    MotionBody *header = model->m_header;
    int index;
    MotionWeightScale *scale;
    int i;
    Vec3f *scaleOut;
    Mtx34f rootMatrix;
    Mtx34f matrix;
    Mtx34f own;
    float saved[4];

    if (header->m_type == 1 && (header->m_flags & 7)) {
        root = 1;
        fn_8040C204(lbl_805F13A8);
        fn_8040BCD4(TopMatrix());
        fn_8040C148(&rootMatrix, fn_80411700(lbl_805F13A8));
        fn_8040C244(lbl_805F13A8);
    }
    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        scale = &model->m_weightScales[index];
        if (scale->m_enabled) {
            float weight = model->m_layerWeights[index].m_weight2;
            saved[index] = lbl_805E14F0[index];
            scaled = 1;
            lbl_805E14F0[index] = weight * scale->m_scale + scale->m_offset;
        }
    }
    if (model->m_output) {
        if (!root) {
            fn_8040B6F8(TopMatrix());
        }
        if (model->field_0x0 & 0x400) {
            scaleOut = &channel->m_target->m_scale;
            fn_8040C204(lbl_805F13A8);
            fn_8040B6F8(TopMatrix());
            if (channel->m_function) {
                channel->m_function(&channel->m_data);
            }
            fn_8040C148(&matrix, fn_80411700(lbl_805F13A8));
            fn_80411328(scaleOut, &matrix);
            fn_8040C244(lbl_805F13A8);
            channel = channel->m_next;
        } else {
            scaleOut = &channel->m_target->m_scale;
            scaleOut->x = 1.0f;
            scaleOut->y = 1.0f;
            scaleOut->z = 1.0f;
        }
        while (channel) {
            if (channel->m_function) {
                channel->m_function(&channel->m_data);
            }
            channel = channel->m_next;
        }
        if (root) {
            fn_8040C204(lbl_805F13A8);
            fn_8040C148(&own, fn_80411700(lbl_805F13A8));
            fn_8040C148(fn_80411700(lbl_805F13A8), &rootMatrix);
            fn_8040BAC4(TopMatrix(), &own);
            fn_8040C148(&model->m_output->m_world.m_matrix, fn_80411700(lbl_805F13A8));
            fn_8040C244(lbl_805F13A8);
        } else {
            fn_8040C148(&model->m_output->m_world.m_matrix, fn_80411700(lbl_805F13A8));
        }
    }
    if (model->m_child) {
        fn_8041A83C(work, model->m_child);
    }
    if (scaled) {
        for (i = 0; i < work->m_layerCount; i++) {
            index = work->m_order[i];
            if (index == -1) {
                break;
            }
            lbl_805E14F0[index] = saved[index];
        }
    }
    if (model->m_next) {
        fn_8041A83C(work, model->m_next);
    }
}

/* The layer count of a play work as an int: the converted result is a value of its own, which
 * the register allocator serves before the function's ordinary locals. */
inline int LayerCount(MotionPlayWork *work) {
    return work->m_layerCount;
}

/* Takes the blended matrices of the cross-fading layers of a play work as their own. */
extern "C" void fn_8041A440(MotionPlayWork *work) {
    int count = LayerCount(work);
    int n = 1;
    int i;
    int j;
    int index;
    int other;
    int again;
    int parent;
    MotionLayer *layer;
    Mtx34f *dst;
    Mtx34f *to;
    Mtx34f *src;

    for (i = 1; i < count * 2; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        if (!(layer->m_flags & 4)) {
            break;
        }
        if (layer->m_blend->m_type == 0) {
            break;
        }
        n++;
    }
    work->m_layerCount = n;
    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        if (i == 0 && (layer->m_blend->m_flags & 1)) {
            fn_8040C148(&lbl_805E12E0[4], &work->m_rootMatrix);
        }
        fn_8040C148(&lbl_805E12E0[i], &layer->m_matrix);
    }
    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        for (i = 0, again = 0; i < work->m_layerCount; i++) {
            index = work->m_order[i];
            if (index == -1) {
                break;
            }
            if (again && work->m_parents[index] == -1) {
                s_isParent[index] = 1;
                continue;
            }
            s_isParent[index] = 0;
            for (j = i + 1; j < work->m_layerCount; j++) {
                other = work->m_order[j];
                if (other == -1) {
                    break;
                }
                if (s_done[other] == 0 && index == work->m_parents[other]) {
                    s_isParent[index] = 1;
                    again = 1;
                    break;
                }
            }
        }
        for (i = 0; i < work->m_layerCount; i++) {
            index = work->m_order[i];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            layer = &work->m_layers[index];
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_8040C148(&s_matrices[index], &layer->m_matrix);
                    s_rootSet[index] = 1;
                }
                continue;
            }
            s_done[index] = 1;
            parent = work->m_parents[index];
            if (parent == -1) {
                to = &work->m_rootMatrix;
                dst = to;
            } else {
                to = &s_matrices[parent];
                s_rootSet[parent] = 1;
                dst = to;
            }
            if (s_rootSet[index] != 0) {
                src = &s_matrices[index];
            } else {
                src = &layer->m_matrix;
            }
            if (layer->m_blend->m_flags & 1) {
                if (i == 0) {
                    to = &work->m_rootMatrix;
                }
                fn_80420590(dst, src, to, lbl_805E14F0[index]);
            } else {
                fn_8040C148(dst, src);
            }
        }
    } while (again);
    work->m_flags |= 0x10000;
    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        lbl_805E14F0[index] = work->m_layers[index].m_blend->m_weight2;
    }
    fn_8040C204(lbl_805F13A8);
    fn_8040C148(fn_80411700(lbl_805F13A8), &work->m_rootMatrix);
    fn_8041A83C(work, work->m_model);
    fn_8040C244(lbl_805F13A8);
    work->m_layerCount = count;
}

/* Settles the blends of a play work: finished cross-fades and newly requested layers drop the
 * layers they replace. */
extern "C" void fn_80419F5C(MotionPlayWork *work) {
    int i;
    int n;
    int pos;
    int index;
    int other;
    int ok;
    int j;
    MotionBlend *blend;
    MotionLayer *layer;

    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        blend = layer->m_blend;
        if (blend->m_flags & 2) {
            fn_8041A440(work);
            for (n = 0, pos = -1; n < work->m_layerCount * 2; n++) {
                other = work->m_order[n];
                if (other == -1) {
                    break;
                }
                if (pos == -1) {
                    if (n > i) {
                        if (!(work->m_layers[other].m_flags & 4)) {
                            pos = n;
                        } else if (work->m_layers[other].m_blend->m_type == 0) {
                            pos = n;
                        }
                    }
                    if (pos == -1) {
                        work->m_parents[other] = work->m_parents[other + work->m_layerCount];
                        work->m_parents[other + work->m_layerCount] = -1;
                        work->m_layers[other].m_blend->m_flags &= ~2;
                        if (!(work->m_layers[other].m_flags2 & 0x4000)) {
                            work->m_layers[other].m_flags &= ~1;
                        }
                        if (!(work->m_layers[other].m_flags2 & 0x1000)) {
                            work->m_layers[other].m_flags &= ~2;
                        }
                        work->m_layers[other].m_flags &= ~4;
                        if (work->m_layers[other].m_flags & 0x10) {
                            fn_8041CC18(work->m_model, other);
                            work->m_layers[other].m_flags &= ~0x10;
                        }
                        if (work->m_layers[other].m_motion) {
                            fn_80413240(work->m_layers[other].m_motion);
                        }
                    }
                }
                work->m_order[n] = -1;
                if (pos != -1) {
                    work->m_order[n - pos] = other;
                }
            }
            i = -1;
            continue;
        }
        if (!(layer->m_flags2 & 1)) {
            continue;
        }
        if (layer->m_flags2 & 0x200) {
            ok = 1;
            switch (blend->m_mode) {
            case 0:
            default:
                if (blend->m_time < 0.0001f) {
                    ok = 0;
                }
                break;
            case 1:
            case 2:
                break;
            }
            if (layer->m_newMotion == 0) {
                layer->m_newMotion = 1;
            }
            if (ok) {
                layer->m_blend->m_type = layer->m_blend->m_mode;
                layer->m_blend->m_weight2 = layer->m_blend->m_weight;
                blend = layer->m_blend;
                switch (blend->m_type) {
                case 0:
                default:
                    blend->m_elapsed = 0.0f;
                    break;
                case 1:
                case 2:
                    break;
                }
                layer->m_blend->m_flags |= 1;
            } else {
                layer->m_flags2 &= ~0x200;
                layer->m_blend->m_flags &= ~2;
            }
        }
        if (layer->m_newMotion == 0) {
            continue;
        }
        if (layer->m_flags2 & 0x200) {
            continue;
        }
        if (layer->m_flags2 & 0x8000) {
            continue;
        }
        for (n = 0, pos = -1; n < work->m_layerCount * 2; n++) {
            other = work->m_order[n];
            if (other == -1) {
                break;
            }
            if (pos == -1) {
                if (other == index) {
                    pos = n;
                } else {
                    work->m_layers[other].m_blend->m_flags &= ~2;
                    if (!(work->m_layers[other].m_flags2 & 0x4000)) {
                        work->m_layers[other].m_flags &= ~1;
                    }
                    if (!(work->m_layers[other].m_flags2 & 0x1000)) {
                        work->m_layers[other].m_flags &= ~2;
                    }
                    work->m_layers[other].m_flags &= ~4;
                    if (work->m_layers[other].m_flags & 0x10) {
                        fn_8041CC18(work->m_model, other);
                        work->m_layers[other].m_flags &= ~0x10;
                    }
                    if (work->m_layers[other].m_motion) {
                        fn_80413240(work->m_layers[other].m_motion);
                    }
                    REMOVE_SLOT_ONE(work, other, j);
                }
                work->m_parents[other] = -1;
                work->m_layers[other].m_blend->m_flags &= ~1;
            }
            work->m_order[n] = -1;
            if (pos != -1) {
                work->m_order[n - pos] = other;
            }
        }
        i = 0;
    }
}

/* Starts the requested motion on a layer. */
extern "C" void fn_80419D5C(MotionPlayWork *work, int index) {
    MotionLayer *layer = &work->m_layers[index];

    fn_80413318(layer->m_newMotion);
    if (layer->m_flags & 4) {
        fn_80413240(layer->m_motion);
    }
    memset(&layer->m_prevRoot, 0, sizeof(Vec3f));
    if (!(layer->m_flags2 & 2)) {
        fn_800091D0(&layer->m_basePosition, &work->m_position);
    } else {
        layer->m_flags |= 0x100000;
    }
    if (!(layer->m_flags2 & 4)) {
        fn_800091D0(&layer->m_baseRotation, &work->m_rotation);
        fn_8040C148(&layer->m_baseMatrix, &work->m_matrix);
    } else {
        layer->m_flags |= 0x200000;
    }
    layer->m_flags |= 4;
    layer->m_flags |= 0x10000;
    layer->m_flags |= 0x20000;
    layer->m_flags |= 0x80000;
    layer->m_flags |= 0x1000000;
    if (!fn_8041B170(work, index, layer->m_newMotion)) {
        fn_80413240(layer->m_newMotion);
        layer->m_motion = 0;
        return;
    }
    layer->m_motion = layer->m_newMotion;
    if (layer->m_flags2 & 0x800) {
        if (fn_80411DEC(layer->m_motion, 2, 0, 0, 0)) {
            layer->m_flags &= ~8;
        } else {
            layer->m_flags |= 8;
        }
    } else {
        if (fn_80411DEC(layer->m_motion, 2, 0, 0, 0)) {
            layer->m_flags |= 8;
        } else {
            layer->m_flags &= ~8;
        }
    }
    if (!(layer->m_flags2 & 0x1000)) {
        if (fn_80411DEC(layer->m_motion, 1, 0, 0, 0)) {
            layer->m_flags |= 2;
        } else {
            layer->m_flags &= ~2;
        }
    }
}

/* The root position of a layer at frames; reset restarts its tracks first. */
extern "C" void fn_80419BA8(Vec3f *out, MotionLayer *layer, MotionFrames *frames, int reset) {
    MotionRootMotion *root = &layer->m_source->m_rootMotion;
    HalfKeyTrack *tracks = root->m_tracks;

    if (frames->m_speed >= 0.0f) {
        lbl_805F13D0 = fn_8041525C;
        lbl_805F13D4 = fn_80414FD0;
        lbl_805F13D8 = fn_80414CBC;
    } else if (frames->m_speed < 0.0f) {
        lbl_805F13D0 = fn_804149E0;
        lbl_805F13D4 = fn_8041474C;
        lbl_805F13D8 = fn_80414438;
    }
    fn_8041575C((const float *)frames);
    fn_800091D0(out, &root->m_position);
    if (tracks[0].m_cursor) {
        if (reset) {
            fn_80413B30(&tracks[0]);
        }
        out->x = lbl_805F13D0(&tracks[0]);
        if (root->m_scale > 0.0f) {
            out->x = out->x * root->m_scale;
        }
        if (layer->m_flags & 8) {
            out->x = -out->x;
        }
    }
    if (tracks[1].m_cursor) {
        if (reset) {
            fn_80413B30(&tracks[1]);
        }
        out->y = lbl_805F13D0(&tracks[1]);
        if (root->m_scale > 0.0f) {
            out->y = out->y * root->m_scale;
        }
    }
    if (tracks[2].m_cursor) {
        if (reset) {
            fn_80413B30(&tracks[2]);
        }
        out->z = lbl_805F13D0(&tracks[2]);
        if (root->m_scale > 0.0f) {
            out->z = out->z * root->m_scale;
        }
    }
}

/* Puts a layer blended onto its parent back in step with it. */
extern "C" void fn_804198E4(MotionPlayWork *work, int index) {
    MotionLayer *layer = &work->m_layers[index];
    MotionLayer *base;
    int parent;
    MotionFrames frames;
    Vec3f move;
    MotionFrames *baseFrames;

    switch (layer->m_blend->m_type) {
    case 0:
    case 2:
        break;
    case 1:
        parent = work->m_parents[index];
        if (parent == -1) {
            break;
        }
        base = &work->m_layers[parent];
        if (!(base->m_flags & 4)) {
            break;
        }
        *(MotionFrameWords *)&frames = *(MotionFrameWords *)&layer->m_frames;
        baseFrames = &base->m_frames;
        frames.m_frame = layer->m_frames.m_start;
        frames.m_frame += (layer->m_frames.m_end - layer->m_frames.m_start) *
                          ((baseFrames->m_frame - baseFrames->m_start) /
                           (baseFrames->m_end - baseFrames->m_start));
        if (base->m_flags & 0x80000) {
            fn_80419BA8(&base->m_prevRoot, base, baseFrames, 0);
            fn_80419BA8(&layer->m_prevRoot, layer, &frames, 0);
            if ((base->m_flags & 1) || (base->m_flags2 & 0x400)) {
                fn_800091D0(&layer->m_basePosition, &base->m_basePosition);
            } else {
                fn_8040C204(lbl_805F13A8);
                fn_8040B6F8(TopMatrix());
                fn_8040B59C(TopMatrix(), &base->m_basePosition);
                fn_8040BAC4(TopMatrix(), &base->m_baseMatrix);
                fn_8040B59C(TopMatrix(), &base->m_prevRoot);
                move.x = -layer->m_prevRoot.x;
                move.y = -layer->m_prevRoot.y;
                move.z = -layer->m_prevRoot.z;
                fn_8040B59C(TopMatrix(), &move);
                fn_804114D0(&layer->m_basePosition, 0);
                fn_8040C244(lbl_805F13A8);
            }
            fn_800091D0(&layer->m_baseRotation, &base->m_baseRotation);
            fn_8040C148(&layer->m_baseMatrix, &base->m_baseMatrix);
        } else {
            fn_8040C204(lbl_805F13A8);
            fn_8040C148(fn_80411700(lbl_805F13A8), &base->m_matrix);
            fn_80419BA8(&move, base, &base->m_frames, 0);
            fn_8040B59C(TopMatrix(), &move);
            fn_80419BA8(&layer->m_prevRoot, layer, &frames, 0);
            move.x = -layer->m_prevRoot.x;
            move.y = -layer->m_prevRoot.y;
            move.z = -layer->m_prevRoot.z;
            fn_8040B59C(TopMatrix(), &move);
            fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
            fn_8040C244(lbl_805F13A8);
            layer->m_flags &= ~0x80000;
        }
        break;
    }
}

/* Sets the channel functions of a model frame and of all the frames below and after it for
 * single playing. */
extern "C" void fn_8041966C(MotionModel *model) {
    MotionModel *other;

    SetSingleChannels(model);
    other = model->m_child;
    if (other) {
        SetSingleChannels(other);
        if (other->m_child) {
            fn_8041966C(other->m_child);
        }
        if (other->m_next) {
            fn_8041966C(other->m_next);
        }
    }
    other = model->m_next;
    if (other) {
        SetSingleChannels(other);
        if (other->m_child) {
            fn_8041966C(other->m_child);
        }
        if (other->m_next) {
            fn_8041966C(other->m_next);
        }
    }
}

/* Sets the channel functions of a model frame and of all the frames below and after it for
 * blended playing. */
extern "C" void fn_804194B8(MotionModel *model) {
    MotionCallback *channel = model->m_callbacks;
    MotionBody *header = model->m_header;

    if (header->m_type == 1 && (header->m_flags & 7)) {
        if (model->field_0x0 & 0x400) {
            channel->m_function = fn_8041E618;
            channel = channel->m_next;
        }
        if (model->field_0x0 & 7) {
            channel->m_function = fn_8041F290;
        } else {
            channel->m_function = fn_8041FA20;
        }
        channel = channel->m_next;
        if (!(model->field_0x0 & 0x40)) {
            if (model->field_0x0 & 0x38) {
                channel->m_function = fn_8041ED8C;
            } else {
                channel->m_function = fn_8041F7D0;
            }
        }
        channel = channel->m_next;
        if (model->field_0x0 & 0x380) {
            channel->m_function = fn_8041E9C4;
        } else {
            channel->m_function = fn_8041F6F0;
        }
    } else {
        if (model->field_0x0 & 0x400) {
            channel->m_function = fn_8041D580;
            channel = channel->m_next;
        }
        if (model->field_0x0 & 7) {
            channel->m_function = fn_8041E124;
        } else {
            channel->m_function = fn_8041E5E0;
        }
        channel = channel->m_next;
        if (!(model->field_0x0 & 0x40)) {
            if (model->field_0x0 & 0x38) {
                channel->m_function = fn_8041DBD4;
            } else {
                channel->m_function = fn_8041E480;
            }
        }
        channel = channel->m_next;
        if (model->field_0x0 & 0x380) {
            channel->m_function = fn_8041D89C;
        } else {
            channel->m_function = fn_8041E420;
        }
    }
    if (model->m_child) {
        fn_804194B8(model->m_child);
    }
    if (model->m_next) {
        fn_804194B8(model->m_next);
    }
}

/* Applies the requests (new motion, frames) made to the layers of a play work. */
extern "C" void fn_804192EC(MotionPlayWork *work) {
    int i;
    int changed = 0;
    int blended = 0;
    int index;
    MotionLayer *layer;
    u32 result;

    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        if (!(layer->m_flags2 & 1)) {
            continue;
        }
        if (layer->m_newMotion) {
            fn_80419D5C(work, index);
            if (layer->m_blend->m_flags & 1) {
                blended = 1;
            }
            changed = 1;
        }
        if (!(layer->m_flags & 4)) {
            continue;
        }
        if (layer->m_flags2 & 0x78) {
            MotionFrames *frames = &layer->m_frames;
            MotionFrames *request = &layer->m_request;
            if (!(layer->m_flags2 & 0x40)) {
                request->m_speed = frames->m_speed;
            }
            if (!(layer->m_flags2 & 8)) {
                request->m_start = -1.0f;
            }
            if (!(layer->m_flags2 & 0x10)) {
                request->m_end = -1.0f;
            }
            if (!(layer->m_flags2 & 0x20)) {
                request->m_frame = -1.0f;
            }
            result = fn_8041AD94(frames, request, layer->m_minFrame, layer->m_maxFrame);
            if (result & 1) {
                layer->m_flags |= 0x40000;
            }
            if (result & 0x10) {
                layer->m_flags |= 0x20000;
            }
        }
        if (layer->m_flags2 & 0x2000) {
            fn_8041CE74(work->m_model, index);
            layer->m_flags |= 0x10;
        }
        if (layer->m_flags & 0x10000) {
            fn_80419BA8(&layer->m_prevRoot, layer, &layer->m_frames, 0);
            if (layer->m_blend->m_flags & 1) {
                fn_804198E4(work, index);
            }
        }
    }
    if (blended) {
        fn_804194B8(work->m_model);
    } else if (changed) {
        fn_8041966C(work->m_model);
    }
}

/* Advances the blends and the frames of the playing layers of a play work. */
extern "C" void fn_80418A4C(MotionPlayWork *work) {
    int i;
    int n;
    int ended;
    int index;
    int other;
    int pos;
    int j;
    MotionLayer *layer;
    MotionLayer *base;
    MotionBlend *blend;
    MotionBody *header;
    float frame;
    MotionFrames frames;
    Vec3f atEnd;
    Vec3f atStart;
    Vec3f move;
    Vec3f loop;
    Vec3f root;
    Vec3f loop2;
    Vec3f root2;

    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        if (!(layer->m_flags & 4)) {
            continue;
        }
        blend = layer->m_blend;
        ended = 0;
        if (blend->m_flags & 1) {
            switch (blend->m_type) {
            default:
                if (blend->m_weight2 < 0.0f) {
                    blend->m_weight2 = 0.0f;
                }
                if (layer->m_blend->m_weight2 > 1.0f) {
                    layer->m_blend->m_weight2 = 1.0f;
                }
                break;
            case 0:
                blend->m_elapsed = blend->m_rate * lbl_805EDD60 + blend->m_elapsed;
                layer->m_blend->m_weight2 = layer->m_blend->m_elapsed / layer->m_blend->m_time;
                if (layer->m_blend->m_weight2 > 0.9999f) {
                    layer->m_flags |= 0x800000;
                    for (n = 0, pos = -1; n < work->m_layerCount; n++) {
                        other = work->m_order[n];
                        if (other == -1) {
                            break;
                        }
                        if (pos == -1) {
                            if (other == index) {
                                pos = n;
                            } else {
                                work->m_layers[other].m_blend->m_flags &= ~2;
                                work->m_layers[other].m_flags &= ~1;
                                work->m_layers[other].m_flags &= ~2;
                                work->m_layers[other].m_flags &= ~4;
                                if (work->m_layers[other].m_flags & 0x10) {
                                    fn_8041CC18(work->m_model, other);
                                    work->m_layers[other].m_flags &= ~0x10;
                                }
                                if (work->m_layers[other].m_motion) {
                                    fn_80413240(work->m_layers[other].m_motion);
                                }
                                REMOVE_SLOT_ONE(work, other, j);
                            }
                            work->m_parents[other] = -1;
                            work->m_layers[other].m_blend->m_flags &= ~1;
                        }
                        work->m_order[n] = -1;
                        if (pos != -1) {
                            work->m_order[n - pos] = other;
                        }
                    }
                    i = 0;
                    if (work->m_order[1] == -1) {
                        fn_8041966C(work->m_model);
                    }
                }
                break;
            case 1:
                if (blend->m_weight2 < 0.0f) {
                    blend->m_weight2 = 0.0f;
                }
                if (layer->m_blend->m_weight2 > 1.0f) {
                    layer->m_blend->m_weight2 = 1.0f;
                }
                other = work->m_parents[index];
                if (other == -1) {
                    break;
                }
                base = &work->m_layers[other];
                if (!(base->m_flags & 4)) {
                    break;
                }
                if (base->m_flags & 1) {
                    layer->m_flags |= 1;
                } else {
                    layer->m_flags &= ~1;
                }
                if (base->m_flags & 2) {
                    layer->m_flags |= 2;
                } else {
                    layer->m_flags &= ~2;
                }
                if (base->m_flags2 & 0x400) {
                    layer->m_flags2 |= 0x400;
                } else {
                    layer->m_flags2 &= ~0x400;
                }
                frame = (layer->m_frames.m_end - layer->m_frames.m_start) *
                            ((base->m_frames.m_frame - base->m_frames.m_start) /
                             (base->m_frames.m_end - base->m_frames.m_start)) +
                        layer->m_frames.m_start;
                if (base->m_frames.m_speed >= 0.0f) {
                    if (frame < layer->m_frames.m_frame) {
                        layer->m_frames.m_speed = layer->m_frames.m_end - layer->m_frames.m_frame;
                        layer->m_frames.m_speed =
                            layer->m_frames.m_speed + (frame - layer->m_frames.m_start);
                    } else {
                        layer->m_frames.m_speed = frame - layer->m_frames.m_frame;
                    }
                } else if (base->m_frames.m_speed < 0.0f) {
                    if (frame > layer->m_frames.m_frame) {
                        layer->m_frames.m_speed = layer->m_frames.m_start - layer->m_frames.m_frame;
                        layer->m_frames.m_speed =
                            layer->m_frames.m_speed + (frame - layer->m_frames.m_end);
                    } else {
                        layer->m_frames.m_speed = frame - layer->m_frames.m_frame;
                    }
                }
                if (lbl_805EDD60 > 0.0001f) {
                    layer->m_frames.m_speed = layer->m_frames.m_speed / lbl_805EDD60;
                }
                layer->m_flags &= ~0x20000;
                break;
            }
        }
        if (layer->m_flags & 0x40000) {
            ended = 1;
        }
        if (!(layer->m_flags & 0x20000)) {
            layer->m_frames.m_frame = layer->m_frames.m_speed * lbl_805EDD60 + layer->m_frames.m_frame;
            if (layer->m_flags & 2) {
                if (std::fabs(layer->m_frames.m_end - layer->m_frames.m_start) < 0.0001f) {
                    layer->m_frames.m_frame = layer->m_frames.m_end;
                } else if (layer->m_frames.m_speed > 0.0f) {
                    if (layer->m_frames.m_frame >= layer->m_frames.m_end) {
                        header = work->m_model->m_header;
                        if (header->m_type == 1 && (header->m_flags & 7)) {
                            frames = layer->m_frames;
                            frames.m_frame = frames.m_end;
                            fn_80419BA8(&atEnd, layer, &frames, ended);
                            frames.m_frame = frames.m_start;
                            fn_80419BA8(&atStart, layer, &frames, 1);
                            fn_8040CD74(&loop, &atEnd, &atStart);
                            fn_800091D0(&move, &loop);
                            if (!(work->m_flags & 1)) {
                                move.y = 0.0f;
                            }
                            fn_8040C204(lbl_805F13A8);
                            fn_8040C148(fn_80411700(lbl_805F13A8), &layer->m_matrix);
                            while (layer->m_frames.m_frame >= layer->m_frames.m_end) {
                                layer->m_frames.m_frame -= layer->m_frames.m_end - layer->m_frames.m_start;
                                fn_8040CD74(&root, &layer->m_prevRoot, &move);
                                fn_800091D0(&layer->m_prevRoot, &root);
                                fn_8040B59C(TopMatrix(), &move);
                            }
                            fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
                            fn_8040C244(lbl_805F13A8);
                        } else {
                            while (layer->m_frames.m_frame >= layer->m_frames.m_end) {
                                layer->m_frames.m_frame -= layer->m_frames.m_end - layer->m_frames.m_start;
                            }
                        }
                        ended = 1;
                        layer->m_flags |= 0x400000;
                    }
                } else if (layer->m_frames.m_speed < 0.0f) {
                    if (layer->m_frames.m_frame <= layer->m_frames.m_start) {
                        header = work->m_model->m_header;
                        if (header->m_type == 1 && (header->m_flags & 7)) {
                            frames = layer->m_frames;
                            frames.m_frame = frames.m_start;
                            fn_80419BA8(&atStart, layer, &frames, ended);
                            frames.m_frame = frames.m_end;
                            fn_80419BA8(&atEnd, layer, &frames, 1);
                            fn_8040CD74(&loop2, &atStart, &atEnd);
                            fn_800091D0(&move, &loop2);
                            if (!(work->m_flags & 1)) {
                                move.y = 0.0f;
                            }
                            fn_8040C204(lbl_805F13A8);
                            fn_8040C148(fn_80411700(lbl_805F13A8), &layer->m_matrix);
                            while (layer->m_frames.m_frame <= layer->m_frames.m_start) {
                                layer->m_frames.m_frame += layer->m_frames.m_end - layer->m_frames.m_start;
                                fn_8040CD74(&root2, &layer->m_prevRoot, &move);
                                fn_800091D0(&layer->m_prevRoot, &root2);
                                fn_8040B59C(TopMatrix(), &move);
                            }
                            fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
                            fn_8040C244(lbl_805F13A8);
                        } else {
                            while (layer->m_frames.m_frame <= layer->m_frames.m_start) {
                                layer->m_frames.m_frame += layer->m_frames.m_end - layer->m_frames.m_start;
                            }
                        }
                        ended = 1;
                        layer->m_flags |= 0x400000;
                    }
                }
            } else if (layer->m_frames.m_speed >= 0.0f) {
                if (layer->m_frames.m_frame >= layer->m_frames.m_end) {
                    layer->m_frames.m_frame = layer->m_frames.m_end;
                    layer->m_flags |= 0x400000;
                }
            } else if (layer->m_frames.m_speed < 0.0f) {
                if (layer->m_frames.m_frame <= layer->m_frames.m_start) {
                    layer->m_frames.m_frame = layer->m_frames.m_start;
                    layer->m_flags |= 0x400000;
                }
            }
        }
        if (ended) {
            fn_8041AED0(work, index);
        }
    }
}

/* Advances the half-key tracks of the playing layers of a play work. */
extern "C" void fn_804188A0(MotionPlayWork *work) {
    int i;
    int index;
    MotionLayer *layer;
    MotionSource *source;
    MotionFrames frames;

    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        if (!(layer->m_flags & 4)) {
            continue;
        }
        if (layer->m_frames.m_speed >= 0.0f) {
            lbl_805F13D0 = fn_8041525C;
            lbl_805F13D4 = fn_80414FD0;
            lbl_805F13D8 = fn_80414CBC;
        } else if (layer->m_frames.m_speed < 0.0f) {
            lbl_805F13D0 = fn_804149E0;
            lbl_805F13D4 = fn_8041474C;
            lbl_805F13D8 = fn_80414438;
        }
        if (work->m_flags & 8) {
            *(MotionFrameWords *)&frames = *(MotionFrameWords *)&layer->m_frames;
            frames.m_frame = (int)frames.m_frame;
            fn_8041575C((const float *)&frames);
        } else {
            fn_8041575C((const float *)&layer->m_frames);
        }
        for (source = layer->m_source; source; source = source->m_next) {
            if (source->m_update) {
                source->m_update(&source->m_rootMotion);
            }
        }
        if (layer->m_flags & 8) {
            for (source = layer->m_source; source; source = source->m_next) {
                if (source->m_update2) {
                    source->m_update2(&source->m_rootMotion);
                }
            }
        }
    }
}

/* Walks a model frame and its children and siblings, running their callbacks and storing their
 * matrices. */
extern "C" void fn_804186A0(MotionPlayWork *work, MotionModel *model) {
    int scaled = 0;
    MotionCallback *callback = model->m_callbacks;
    int index;
    MotionWeightScale *scale;
    int i;
    float saved[4];
    MotionCallbackArgs args;

    if (model->m_next) {
        fn_8040C204(lbl_805F13A8);
    }
    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        scale = &model->m_weightScales[index];
        if (scale->m_enabled) {
            float weight = model->m_layerWeights[index].m_weight2;
            saved[index] = lbl_805E14F0[index];
            scaled = 1;
            lbl_805E14F0[index] = weight * scale->m_scale + scale->m_offset;
        }
    }
    while (callback) {
        if (callback->m_function) {
            callback->m_function(&callback->m_data);
        }
        callback = callback->m_next;
    }
    if (model->m_callback) {
        args.field_0x8 = model->m_header->m_key;
        args.m_matrix = TopMatrix();
        args.field_0x4 = model->field_0x20;
        model->m_callback(&args);
    }
    if (model->m_output) {
        fn_8040C148(&model->m_output->m_matrix, fn_80411700(lbl_805F13A8));
    }
    if (work->m_flags & 2) {
        if (model->m_next) {
            fn_8040C244(lbl_805F13A8);
        }
        return;
    }
    if (model->m_child) {
        fn_804186A0(work, model->m_child);
    }
    if (scaled) {
        for (i = 0; i < work->m_layerCount; i++) {
            index = work->m_order[i];
            if (index == -1) {
                break;
            }
            lbl_805E14F0[index] = saved[index];
        }
    }
    if (model->m_next) {
        fn_8040C244(lbl_805F13A8);
        fn_804186A0(work, model->m_next);
    }
}

/* Builds the matrices of the layers of a play work and blends them, then walks the model. Copy
 * propagation is off for this function, and the two flags are declared position first: then
 * GC/2.7 gives the locals retail's saved registers (wave 40, probes/Game_MotionPlayWork.py). */
#pragma opt_propagation off
extern "C" void fn_80418044(MotionPlayWork *work) {
    int fixPosition;
    int fixRotation;
    int i;
    int index;
    MotionLayer *layer;
    MotionBody *header;
    Vec3f move;
    Mtx34f other;
    Mtx34f current;
    Mtx34f blended;
    Mtx34f matrix;
    Mtx34f rotation;
    Mtx34f prevMatrix;
    Mtx34f prevRotation;

    fn_8040B59C(TopMatrix(), &work->m_position);
    fn_8040BAC4(TopMatrix(), &work->m_matrix);
    header = work->m_model->m_header;
    if (header->m_type == 1 && (header->m_flags & 7)) {
        fixPosition = 0;
        fixRotation = 0;
        for (i = 0; i < work->m_layerCount; i++) {
            index = work->m_order[i];
            if (index == -1) {
                break;
            }
            layer = &work->m_layers[index];
            if (!(layer->m_flags & 4)) {
                continue;
            }
            if (layer->m_flags & 0x100000) {
                fixPosition = 1;
            }
            if (layer->m_flags & 0x200000) {
                fixRotation = 1;
            }
            fn_800091D0(&layer->m_root, &layer->m_source->m_rootMotion.m_position);
            if (!(layer->m_flags & 1) && !(layer->m_flags2 & 0x400)) {
                continue;
            }
            fn_8040C204(lbl_805F13A8);
            if (layer->m_flags & 0x80000) {
                move.x = -layer->m_root.x;
                if (work->m_flags & 1) {
                    move.y = -layer->m_root.y;
                } else {
                    move.y = 0.0f;
                }
                move.z = -layer->m_root.z;
                fn_8040B6F8(TopMatrix());
                fn_8040B59C(TopMatrix(), &layer->m_basePosition);
                fn_8040BAC4(TopMatrix(), &layer->m_baseMatrix);
                fn_8040B59C(TopMatrix(), &move);
                fn_804114D0(&layer->m_basePosition, 0);
            } else {
                move.x = layer->m_prevRoot.x - layer->m_root.x;
                if (work->m_flags & 1) {
                    move.y = layer->m_prevRoot.y - layer->m_root.y;
                } else {
                    move.y = 0.0f;
                }
                move.z = layer->m_prevRoot.z - layer->m_root.z;
                fn_8040C148(fn_80411700(lbl_805F13A8), &layer->m_matrix);
                fn_8040B59C(TopMatrix(), &move);
                fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
            }
            fn_8040C244(lbl_805F13A8);
        }
        fn_8040C148(&current, fn_80411700(lbl_805F13A8));
        for (i = 0; i < work->m_layerCount; i++) {
            index = work->m_order[i];
            if (index == -1) {
                break;
            }
            layer = &work->m_layers[index];
            if (i != 0 && !(layer->m_flags & 4)) {
                continue;
            }
            fn_8040C204(lbl_805F13A8);
            if (layer->m_flags & 0x80000) {
                fn_8040B6F8(TopMatrix());
                fn_8040B59C(TopMatrix(), &layer->m_basePosition);
                fn_8040BAC4(TopMatrix(), &layer->m_baseMatrix);
            } else {
                if (fixPosition || fixRotation) {
                    fn_8040C148(&matrix, fn_80411700(lbl_805F13A8));
                    if (fixPosition) {
                        matrix.values[0][3] = work->field_0x110.x;
                        matrix.values[1][3] = work->field_0x110.y;
                        matrix.values[2][3] = work->field_0x110.z;
                    }
                    if (fixRotation) {
                        fn_8040C204(lbl_805F13A8);
                        fn_8040B6F8(TopMatrix());
                        fn_8040BAC4(TopMatrix(), &work->m_prevMatrix);
                        fn_8040C148(&rotation, fn_80411700(lbl_805F13A8));
                        fn_8040C244(lbl_805F13A8);
                        fn_804113E8(&matrix, &rotation);
                        layer->m_flags |= 0x80000;
                        layer->m_flags |= 0x1000000;
                    }
                    fn_8040C148(fn_80411700(lbl_805F13A8), &matrix);
                }
                fn_8040BAC4(TopMatrix(), &layer->m_matrix);
            }
            fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
            fn_8040C148(&current, &layer->m_matrix);
            fn_8040C148(&lbl_805E12E0[i], &current);
            fn_8040C244(lbl_805F13A8);
            if (i == 0) {
                if (layer->m_blend->m_flags & 1) {
                    fn_8040C204(lbl_805F13A8);
                    if (fixPosition || fixRotation) {
                        fn_8040C148(&prevMatrix, fn_80411700(lbl_805F13A8));
                        if (fixPosition) {
                            prevMatrix.values[0][3] = work->field_0x110.x;
                            prevMatrix.values[1][3] = work->field_0x110.y;
                            prevMatrix.values[2][3] = work->field_0x110.z;
                        }
                        if (fixRotation) {
                            fn_8040C204(lbl_805F13A8);
                            fn_8040B6F8(TopMatrix());
                            fn_8040BAC4(TopMatrix(), &work->m_prevMatrix);
                            fn_8040C148(&prevRotation, fn_80411700(lbl_805F13A8));
                            fn_8040C244(lbl_805F13A8);
                            fn_804113E8(&prevMatrix, &prevRotation);
                            work->m_flags |= 0x10000;
                        }
                        fn_8040C148(fn_80411700(lbl_805F13A8), &prevMatrix);
                    }
                    fn_8040BAC4(TopMatrix(), &work->m_rootMatrix);
                    fn_8040C148(&work->m_rootMatrix, fn_80411700(lbl_805F13A8));
                    fn_8040C148(&lbl_805E12E0[4], &work->m_rootMatrix);
                    fn_8040C244(lbl_805F13A8);
                    fn_80420590(&current, &current, &work->m_rootMatrix, layer->m_blend->m_weight2);
                }
            } else {
                fn_80420590(&current, &current, &blended, layer->m_blend->m_weight2);
            }
            fn_8040C148(&blended, &current);
        }
        fn_8040C148(fn_80411700(lbl_805F13A8), &current);
    }
    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        lbl_805E14F0[index] = work->m_layers[index].m_blend->m_weight2;
    }
    fn_804186A0(work, work->m_model);
}
#pragma opt_propagation reset

/* Applies the layers of a play work to their matrices and moves the root. */
extern "C" void fn_804175C8(MotionPlayWork *work) {
    int moved = 0;
    int i;
    int j;
    int index;
    int again;
    int parent;
    MotionLayer *layer;
    MotionLayer *base;
    MotionBody *header;
    MotionWeightScale *scale;
    float weight;
    Vec3f *dst;
    Vec3f *src;
    Vec3f rootMove;
    Vec3f position;
    Vec3f root;
    Vec3f layerRoot;
    Vec3f baseRoot;
    Vec3f offset;

    header = work->m_model->m_header;
    if (header->m_type == 1 && (header->m_flags & 7)) {
        work->field_0x130.x = work->m_nodes->m_matrix.values[0][3];
        work->field_0x130.y = work->m_nodes->m_matrix.values[1][3];
        work->field_0x130.z = work->m_nodes->m_matrix.values[2][3];
        if (work->m_flags & 1) {
            work->field_0x140.x = work->field_0x130.x - work->m_position.x;
            work->field_0x140.y = work->field_0x130.y - work->m_position.y;
            work->field_0x140.z = work->field_0x130.z - work->m_position.z;
            fn_800091D0(&work->m_position, &work->field_0x130);
        } else {
            work->field_0x140.x = work->field_0x130.x - work->m_position.x;
            work->field_0x140.y = 0.0f;
            work->field_0x140.z = work->field_0x130.z - work->m_position.z;
            work->m_position.x = work->field_0x130.x;
            work->m_position.z = work->field_0x130.z;
        }
        fn_800091D0(&work->field_0x110, &work->m_position);
        fn_800091D0(&work->field_0x120, &work->m_rotation);
        fn_8040C148(&work->m_prevMatrix, &work->m_matrix);
    }
    fn_8040C204(lbl_805F13A8);
    fn_8040B6F8(TopMatrix());
    fn_8040B59C(TopMatrix(), &work->m_position);
    fn_8040BAC4(TopMatrix(), &work->m_matrix);
    fn_8040BCD4(TopMatrix());
    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        if (i != 0 && !(layer->m_flags & 4)) {
            continue;
        }
        fn_8040C204(lbl_805F13A8);
        fn_8040BAC4(TopMatrix(), &layer->m_matrix);
        fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
        fn_8040C244(lbl_805F13A8);
        if (layer->m_flags & 0x1000000) {
            fn_8040C148(&layer->m_prevMatrix, &layer->m_matrix);
        } else {
            fn_804113E8(&layer->m_matrix, &layer->m_prevMatrix);
        }
        header = work->m_model->m_header;
        if (header->m_type == 1 && (header->m_flags & 7)) {
            if (work->m_flags & 1) {
                layer->m_rootMove.x = layer->m_root.x - layer->m_prevRoot.x;
                layer->m_rootMove.y = layer->m_root.y - layer->m_prevRoot.y;
                layer->m_rootMove.z = layer->m_root.z - layer->m_prevRoot.z;
            } else {
                layer->m_rootMove.x = layer->m_root.x - layer->m_prevRoot.x;
                layer->m_rootMove.y = 0.0f;
                layer->m_rootMove.z = layer->m_root.z - layer->m_prevRoot.z;
            }
            fn_800091D0(&layer->m_prevRoot, &layer->m_root);
        }
        if (!(layer->m_blend->m_flags & 1)) {
            continue;
        }
        if (i == 0) {
            fn_8040C204(lbl_805F13A8);
            fn_8040BAC4(TopMatrix(), &work->m_rootMatrix);
            fn_8040C148(&work->m_rootMatrix, fn_80411700(lbl_805F13A8));
            fn_8040C244(lbl_805F13A8);
            if (work->m_flags & 0x10000) {
                fn_8040C148(&work->m_prevRootMatrix, &work->m_rootMatrix);
            } else {
                fn_804113E8(&work->m_rootMatrix, &work->m_prevRootMatrix);
            }
        }
        switch (layer->m_blend->m_type) {
        case 0:
            if (work->m_flags & 4) {
                header = work->m_model->m_header;
                if (header->m_type == 1 && (header->m_flags & 7)) {
                    moved = 1;
                }
            }
            break;
        case 1:
            parent = work->m_parents[index];
            if (parent == -1) {
                break;
            }
            base = &work->m_layers[parent];
            if (!(base->m_flags & 4)) {
                break;
            }
            weight = layer->m_blend->m_weight2;
            scale = &work->m_model->m_weightScales[index];
            if (scale->m_enabled) {
                weight = weight * scale->m_scale + scale->m_offset;
            }
            fn_800091D0(&baseRoot, &base->m_rootMove);
            fn_800091D0(&layerRoot, &layer->m_rootMove);
            fn_8040C204(lbl_805F13A8);
            fn_8040C148(fn_80411700(lbl_805F13A8), &base->m_matrix);
            offset.x = weight * (layerRoot.x - baseRoot.x);
            offset.y = weight * (layerRoot.y - baseRoot.y);
            offset.z = weight * (layerRoot.z - baseRoot.z);
            fn_8040B59C(TopMatrix(), &offset);
            fn_8040C148(&base->m_matrix, fn_80411700(lbl_805F13A8));
            fn_8040C148(fn_80411700(lbl_805F13A8), &layer->m_matrix);
            offset.x = (1.0f - weight) * (baseRoot.x - layerRoot.x);
            offset.y = (1.0f - weight) * (baseRoot.y - layerRoot.y);
            offset.z = (1.0f - weight) * (baseRoot.z - layerRoot.z);
            fn_8040B59C(TopMatrix(), &offset);
            fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
            fn_8040C244(lbl_805F13A8);
            layer->m_frames.m_speed =
                base->m_frames.m_speed * (layer->m_frames.m_end - layer->m_frames.m_start) / (base->m_frames.m_end - base->m_frames.m_start);
            break;
        }
    }
    if (moved) {
        fn_800091D0(&rootMove, &work->field_0x140);
        fn_8041157C(&rootMove, 0, &rootMove);
        fn_8040C204(lbl_805F13A8);
        for (i = 0; i < work->m_layerCount; i++) {
            index = work->m_order[i];
            if (index == -1) {
                break;
            }
            s_done[index] = 0;
            s_rootSet[index] = 0;
        }
        do {
            again = 0;
            for (i = 0; i < work->m_layerCount; i++) {
                index = work->m_order[i];
                if (index == -1) {
                    break;
                }
                if (again && work->m_parents[index] == -1) {
                    s_isParent[index] = 1;
                    continue;
                }
                s_isParent[index] = 0;
                for (j = i + 1; j < work->m_layerCount; j++) {
                    parent = work->m_order[j];
                    if (parent == -1) {
                        break;
                    }
                    if (s_done[parent] == 0 && index == work->m_parents[parent]) {
                        s_isParent[index] = 1;
                        again = 1;
                        break;
                    }
                }
            }
            for (i = 0; i < work->m_layerCount; i++) {
                index = work->m_order[i];
                if (index == -1) {
                    break;
                }
                if (s_done[index] != 0) {
                    continue;
                }
                layer = &work->m_layers[index];
                fn_800091D0(&root, &layer->m_rootMove);
                fn_8040C148(fn_80411700(lbl_805F13A8), &layer->m_matrix);
                fn_8041157C(&root, 0, &root);
                if (s_isParent[index] != 0) {
                    if (s_rootSet[index] == 0) {
                        fn_800091D0(&s_roots[index], &root);
                        s_rootSet[index] = 1;
                    }
                    continue;
                }
                s_done[index] = 1;
                parent = work->m_parents[index];
                if (parent == -1) {
                    dst = &s_roots[index];
                } else {
                    s_rootSet[parent] = 1;
                    dst = &s_roots[parent];
                }
                if (s_rootSet[index] != 0) {
                    src = &s_roots[index];
                } else {
                    src = &root;
                }
                if ((layer->m_blend->m_flags & 1) && parent != -1) {
                    weight = layer->m_blend->m_weight2;
                    scale = &work->m_model->m_weightScales[index];
                    if (scale->m_enabled) {
                        weight = weight * scale->m_scale + scale->m_offset;
                    }
                    dst->x = weight * src->x + (1.0f - weight) * dst->x;
                    dst->y = weight * src->y + (1.0f - weight) * dst->y;
                    dst->z = weight * src->z + (1.0f - weight) * dst->z;
                } else {
                    fn_800091D0(dst, src);
                }
            }
        } while (again);
        for (i = 0; i < work->m_layerCount; i++) {
            index = work->m_order[i];
            if (index == -1) {
                break;
            }
            layer = &work->m_layers[index];
            if (!(layer->m_flags & 4)) {
                continue;
            }
            if (i == 0) {
                fn_8040B6F8(TopMatrix());
                fn_8040B59C(TopMatrix(), &rootMove);
                fn_8040BAC4(TopMatrix(), &work->m_rootMatrix);
                fn_8040C148(&work->m_rootMatrix, fn_80411700(lbl_805F13A8));
            }
            if (work->m_parents[index] == -1) {
                fn_800091D0(&position, &s_roots[index]);
            }
            root.x = rootMove.x - position.x;
            root.y = rootMove.y - position.y;
            root.z = rootMove.z - position.z;
            fn_8040B6F8(TopMatrix());
            fn_8040B59C(TopMatrix(), &root);
            fn_8040BAC4(TopMatrix(), &layer->m_matrix);
            fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
        }
        fn_8040C244(lbl_805F13A8);
    }
    fn_8040C244(lbl_805F13A8);
    for (i = 0; i < work->m_layerCount; i++) {
        layer = &work->m_layers[i];
        layer->m_newMotion = 0;
        layer->m_request.m_start = -1.0f;
        layer->m_request.m_end = -1.0f;
        layer->m_request.m_speed = layer->m_frames.m_speed;
        layer->m_request.m_frame = -1.0f;
        if (layer->m_flags2 & 0x2000) {
            fn_8041CD6C(work->m_model, i);
        }
        layer->m_flags2 = 0;
    }
    work->m_flags &= 0xFFFFFF;
    memset(&work->field_0x150, 0, sizeof(Vec3f));
}

extern "C" void fn_80417568(MotionPlayWork *work, u16 value) {
    MotionLayer *layer;

    if (work == 0 || value == 0) {
        return;
    }
    layer = GetLayer(work, 0);
    if (!layer) {
        return;
    }
    layer->m_newMotion = value;
    layer->m_flags2 |= 1;
    layer->m_flags2 |= 0x100;
}

extern "C" void fn_804174F0(MotionPlayWork *work, int slot, u16 value) {
    MotionLayer *layer;

    if (work == 0 || value == 0) {
        return;
    }
    if (!work) {
        return;
    }
    layer = GetLayer(work, slot);
    if (!layer) {
        return;
    }
    layer->m_newMotion = value;
    layer->m_flags2 |= 1;
    layer->m_flags2 |= 0x100;
    layer->m_flags2 |= 0x8000;
}

/* Sets the position and the rotation of a play work; order 1 rotates in Y, X, Z order. */
extern "C" void fn_804173A8(MotionPlayWork *work, const Vec3f *position, const Vec3f *rotation,
                            int order) {
    if (!work) {
        return;
    }
    if (position) {
        fn_800091D0(&work->m_position, position);
    }
    if (!rotation) {
        return;
    }
    fn_800091D0(&work->m_rotation, rotation);
    fn_8040C204(lbl_805F13A8);
    fn_8040B6F8(TopMatrix());
    if (order != 1) {
        fn_8040B80C(TopMatrix(), rotation->z);
        fn_8040B848(TopMatrix(), rotation->y);
        fn_8040B884(TopMatrix(), rotation->x);
    } else {
        fn_8040B848(TopMatrix(), rotation->y);
        fn_8040B884(TopMatrix(), rotation->x);
        fn_8040B80C(TopMatrix(), rotation->z);
    }
    fn_8040C148(&work->m_matrix, fn_80411700(lbl_805F13A8));
    fn_8040C244(lbl_805F13A8);
}

/* Sets the root matrix of a play work. */
extern "C" void fn_8041734C(MotionPlayWork *work, const Mtx34f *matrix) {
    if (work) {
        if (matrix) {
            work->m_rotation.x = 0.0f;
            work->m_rotation.y = 0.0f;
            work->m_rotation.z = 0.0f;
            fn_8040C148(&work->m_matrix, matrix);
            work->m_matrix.values[0][3] = 0.0f;
            work->m_matrix.values[1][3] = 0.0f;
            work->m_matrix.values[2][3] = 0.0f;
        }
    }
}

/* Sets the root matrix of a play work and its position from matrix. */
extern "C" void fn_804172D8(MotionPlayWork *work, const Mtx34f *matrix) {
    if (work) {
        if (matrix) {
            work->m_position.x = matrix->values[0][3];
            work->m_position.y = matrix->values[1][3];
            work->m_position.z = matrix->values[2][3];
            work->m_rotation.x = 0.0f;
            work->m_rotation.y = 0.0f;
            work->m_rotation.z = 0.0f;
            fn_8040C148(&work->m_matrix, matrix);
            work->m_matrix.values[0][3] = 0.0f;
            work->m_matrix.values[1][3] = 0.0f;
            work->m_matrix.values[2][3] = 0.0f;
        }
    }
}

/* Gets a flag of a play work (which 11-15) or of the layer of a slot (which 0-10), and with op 0
 * sets or with op 2 clears it. */
extern "C" int fn_80416F28(MotionPlayWork *work, int slot, int which, int op) {
    int result = 0;
    MotionLayer *layer;

    if (work == 0 || which >= 16) {
        return 0;
    }
    switch (which) {
    case 11:
        result = (work->m_flags & 1) != 0;
        switch (op) {
        case 0:
            work->m_flags |= 1;
            break;
        case 2:
            work->m_flags &= ~1;
            break;
        }
        return result;
    case 12:
        result = (work->m_flags >> 1) & 1;
        switch (op) {
        case 0:
            work->m_flags |= 2;
            break;
        case 2:
            work->m_flags &= ~2;
            break;
        }
        return result;
    case 13:
        result = (work->m_flags >> 2) & 1;
        switch (op) {
        case 0:
            work->m_flags |= 4;
            break;
        case 2:
            work->m_flags &= ~4;
            break;
        }
        return result;
    case 14:
        result = (work->m_flags >> 14) & 1;
        switch (op) {
        case 0:
            work->m_flags |= 0x1000000;
            break;
        case 2:
            work->m_flags &= ~0x1000000;
            break;
        }
        return result;
    case 15:
        result = (work->m_flags >> 3) & 1;
        switch (op) {
        case 0:
            work->m_flags |= 8;
            break;
        case 2:
            work->m_flags &= ~8;
            break;
        }
        return result;
    default:
        layer = GetLayer(work, slot);
        if (!layer) {
            return 0;
        }
        switch (which) {
        case 0:
            result = layer->m_flags & 1;
            switch (op) {
            case 0:
                layer->m_flags |= 1;
                layer->m_flags2 |= 0x4000;
                break;
            case 2:
                layer->m_flags &= ~1;
                layer->m_flags2 |= 0x4000;
                break;
            }
            break;
        case 1:
            result = (layer->m_flags >> 1) & 1;
            switch (op) {
            case 0:
                layer->m_flags |= 2;
                layer->m_flags2 |= 0x1000;
                break;
            case 2:
                layer->m_flags &= ~2;
                layer->m_flags2 |= 0x1000;
                break;
            }
            break;
        case 2:
            result = (layer->m_flags2 >> 10) & 1;
            switch (op) {
            case 0:
                layer->m_flags2 |= 0x400;
                break;
            case 2:
                layer->m_flags2 &= ~0x400;
                break;
            }
            break;
        case 3:
            result = (layer->m_flags >> 22) & 1;
            break;
        case 4:
            result = (layer->m_flags >> 2) & 1;
            break;
        case 5:
            result = layer->m_blend->m_flags & 1;
            break;
        case 6:
            result = (layer->m_flags >> 23) & 1;
            break;
        case 7:
            result = (layer->m_flags >> 3) & 1;
            switch (op) {
            case 0:
                layer->m_flags2 |= 0x800;
                break;
            case 2:
                layer->m_flags2 &= ~0x800;
                break;
            }
            break;
        case 8:
            result = (layer->m_flags2 >> 9) & 1;
            break;
        case 9:
            result = (layer->m_flags2 >> 8) & 1;
            break;
        case 10:
            result = (layer->m_flags >> 16) & 1;
            break;
        }
        return result;
    }
}

extern "C" void fn_80416ED0(MotionPlayWork *work, int slot, float value) {
    MotionLayer *layer;

    if (!work) {
        return;
    }
    layer = GetLayer(work, slot);
    if (!layer) {
        return;
    }
    layer->m_request.m_start = value;
    layer->m_flags2 |= 1;
    layer->m_flags2 |= 8;
}

extern "C" void fn_80416E78(MotionPlayWork *work, int slot, float value) {
    MotionLayer *layer;

    if (!work) {
        return;
    }
    layer = GetLayer(work, slot);
    if (!layer) {
        return;
    }
    layer->m_request.m_end = value;
    layer->m_flags2 |= 1;
    layer->m_flags2 |= 0x10;
}

extern "C" void fn_80416E20(MotionPlayWork *work, int slot, float value) {
    MotionLayer *layer;

    if (!work) {
        return;
    }
    layer = GetLayer(work, slot);
    if (!layer) {
        return;
    }
    layer->m_request.m_frame = value;
    layer->m_flags2 |= 1;
    layer->m_flags2 |= 0x20;
}

extern "C" void fn_80416DC8(MotionPlayWork *work, int slot, float value) {
    MotionLayer *layer;

    if (!work) {
        return;
    }
    layer = GetLayer(work, slot);
    if (!layer) {
        return;
    }
    layer->m_request.m_speed = value;
    layer->m_flags2 |= 1;
    layer->m_flags2 |= 0x40;
}

/* Starts a layer in a free slot: mode 0 alone, 1 or 2 blended from slot from (1 only onto
 * itself), above slot to. */
extern "C" int fn_804165D8(MotionPlayWork *work, int mode, int from, int to) {
    int doIt = 0;
    int reused = 0;
    int type = 3;
    int count;
    int free;
    int i;
    int j;
    int k;
    int n2;
    int index;
    int pos;
    int found;
    int prev;
    int next;
    MotionLayer *layer;
    MotionBlend *blend;
    u32 flags;

    if (!work) {
        return 0;
    }
    if (from < 0) {
        from = 0;
    }
    if (to < 0) {
        to = from;
    }
    count = work->m_layerCount;
    free = -1;
    n2 = count * 2;
    for (i = 0; i < count; i++) {
        for (j = 0; j < n2; j++) {
            if (i == work->m_order[j]) {
                break;
            }
            if (work->m_order[j] == -1) {
                break;
            }
        }
        if (work->m_order[j] == -1) {
            free = i;
            break;
        }
    }
    if (free == -1) {
        for (j = 0; j < count; j++) {
            index = work->m_order[j];
            if (index == -1) {
                break;
            }
            layer = &work->m_layers[index];
            if ((layer->m_blend->m_flags & 2) && !(layer->m_flags2 & 0x200)) {
                free = index;
                reused = 1;
                break;
            }
        }
    }
    switch (mode) {
    default:
        return 0;
    case 1:
    case 2:
        from = work->m_slots[from];
        if (from == -1) {
            return 0;
        }
        to = work->m_slots[to];
        if (to == -1) {
            return 0;
        }
        if (from != to && from != work->m_parents[to]) {
            return 0;
        }
        if (free == -1) {
            doIt = 1;
            n2 = count * 2;
            pos = -1;
            for (j = 0; j < n2; j++) {
                index = work->m_order[j];
                if (index == -1) {
                    break;
                }
                if (index == from) {
                    pos = j;
                }
            }
            if (pos == -1) {
                return 0;
            }
            found = 0;
            for (j = 0; j < n2; j++) {
                index = work->m_order[j];
                if (index == -1) {
                    break;
                }
                layer = &work->m_layers[index];
                if (found) {
                    if (layer->m_flags2 & 0x200) {
                        break;
                    }
                    blend = layer->m_blend;
                    if (!(blend->m_flags & 1) || blend->m_type == 0) {
                        break;
                    }
                }
                blend = layer->m_blend;
                flags = blend->m_flags;
                if (!(flags & 2)) {
                    if (layer->m_flags2 & 0x200) {
                        break;
                    }
                    if (!(flags & 1)) {
                        found = 1;
                    } else if (blend->m_type == 0) {
                        found = 1;
                    }
                    if (found && j == pos) {
                        doIt = 0;
                        break;
                    }
                }
            }
        }
        break;
    case 0:
        if (work->m_flags & 0x1000000) {
            doIt = 1;
        } else if (free == -1) {
            doIt = 1;
        }
        break;
    }
    if (doIt) {
        found = 0;
        for (j = 0; j < work->m_layerCount; j++) {
            index = work->m_order[j];
            if (index == -1) {
                break;
            }
            layer = &work->m_layers[index];
            if (found) {
                if (layer->m_flags2 & 0x200) {
                    break;
                }
                if (!(work->m_flags & 0x1000000)) {
                    blend = layer->m_blend;
                    if (!(blend->m_flags & 1) || blend->m_type == 0) {
                        break;
                    }
                }
            }
            blend = layer->m_blend;
            flags = blend->m_flags;
            if (!(flags & 2)) {
                if (layer->m_flags2 & 0x200) {
                    break;
                }
                if (work->m_flags & 0x1000000) {
                    found = 1;
                } else if (!(flags & 1)) {
                    found = 1;
                } else if (blend->m_type == 0) {
                    found = 1;
                }
                if (found) {
                    blend->m_flags |= 2;
                    layer->m_flags &= ~1;
                    layer->m_flags &= ~2;
                    layer->m_flags2 &= ~0x4000;
                    layer->m_flags2 &= ~0x1000;
                    REMOVE_SLOT_ONE(work, index, i);
                }
            }
        }
        if (!found) {
            return 0;
        }
        for (j = 0; j < work->m_layerCount; j++) {
            index = work->m_order[j];
            if (index == -1) {
                break;
            }
            layer = &work->m_layers[index];
            if ((layer->m_blend->m_flags & 2) && !(layer->m_flags2 & 0x200)) {
                free = index;
                reused = 1;
                break;
            }
        }
    }
    if (free == -1) {
        return 0;
    }
    switch (mode) {
    default:
        return 0;
    case 1:
    case 2:
        if (mode == 1 && from != to) {
            return 0;
        }
        layer = &work->m_layers[from];
        if (layer->m_flags2 & 0x200) {
            type = layer->m_blend->m_mode;
        } else if (layer->m_blend->m_flags & 1) {
            type = layer->m_blend->m_type;
        }
        if (type == 1) {
            return 0;
        }
        count = work->m_layerCount;
        n2 = count * 2;
        found = 0;
        for (j = 0; j < n2; j++) {
            index = work->m_order[j];
            if (index == -1) {
                break;
            }
            layer = &work->m_layers[index];
            blend = layer->m_blend;
            flags = blend->m_flags;
            if (!(flags & 2)) {
                type = 3;
                if (layer->m_flags2 & 0x200) {
                    if (from == work->m_parents[index + count]) {
                        type = blend->m_mode;
                    }
                } else if (flags & 1) {
                    if (from == work->m_parents[index]) {
                        type = blend->m_type;
                    }
                }
                if (type == 1) {
                    if (mode == 1) {
                        return 0;
                    }
                    if (found) {
                        break;
                    }
                }
                if (index == to) {
                    found = 1;
                }
            }
        }
        pos = -1;
        for (j = 0; j < n2; j++) {
            index = work->m_order[j];
            if (index == -1) {
                break;
            }
            if (to == index) {
                pos = j;
            }
        }
        prev = free;
        for (j = pos + 1; j < work->m_layerCount * 2; j++) {
            next = work->m_order[j];
            work->m_order[j] = prev;
            prev = next;
            if (next == -1) {
                break;
            }
        }
        if (reused) {
            work->m_parents[free + work->m_layerCount] = from;
        } else {
            work->m_parents[free] = from;
        }
        break;
    case 0:
        for (j = 0; j < work->m_layerCount * 2; j++) {
            if (work->m_order[j] == -1) {
                work->m_order[j] = free;
                break;
            }
        }
        if (reused) {
            work->m_parents[free + work->m_layerCount] = -1;
        } else {
            work->m_parents[free] = -1;
        }
        break;
    }
    prev = free;
    for (j = 0; j < work->m_layerCount; j++) {
        next = work->m_slots[j];
        work->m_slots[j] = prev;
        prev = next;
        if (next == -1) {
            break;
        }
    }
    layer = &work->m_layers[free];
    layer->m_blend->m_mode = mode;
    layer->m_blend->m_time = 0.0f;
    layer->m_blend->m_rate = 1.0f;
    layer->m_flags2 |= 1;
    layer->m_flags2 |= 0x100;
    layer->m_flags2 |= 0x200;
    return 1;
}


/* Stops the layer of a slot. */
extern "C" int fn_80416198(MotionPlayWork *work, int slot) {
    int index;
    MotionLayer *layer;
    u32 blended;
    int i;
    int type = 3;
    int pos;
    int n;

    if (!work) {
        return 0;
    }
    index = work->m_slots[slot];
    if (index == -1) {
        return 0;
    }
    layer = &work->m_layers[index];
    blended = layer->m_flags2 & 0x200;
    if (blended) {
        type = layer->m_blend->m_mode;
    } else if (layer->m_blend->m_flags & 1) {
        type = layer->m_blend->m_type;
    }
    i = 0;
    pos = -1;
    for (; i < work->m_layerCount * 2; i++) {
        n = work->m_order[i];
        if (n == -1) {
            break;
        }
        if (n == index) {
            pos = i;
        }
    }

    if (pos == -1) {
        return 0;
    }
    switch (type) {
    default:
        return 0;
    case 1:
    case 2:
        if (blended) {
            layer->m_flags2 &= ~0x100;
            layer->m_flags2 &= ~0x200;
            work->m_parents[index + work->m_layerCount] = -1;
        } else if (layer->m_blend->m_flags & 1) {
            layer->m_flags &= ~4;
            layer->m_flags &= ~1;
            layer->m_flags &= ~2;
            layer->m_flags2 &= ~0x4000;
            layer->m_flags2 &= ~0x1000;
            if (layer->m_flags & 0x10) {
                fn_8041CC18(work->m_model, index);
                layer->m_flags &= ~0x10;
            }
            if (layer->m_motion) {
                fn_80413240(layer->m_motion);
            }
            work->m_parents[index] = -1;
        }
        for (i = 0; i < work->m_layerCount * 2; i++) {
            n = work->m_order[i];
            if (n == -1) {
                break;
            }
            if (i >= pos) {
                work->m_order[i] = -1;
                if (i != pos) {
                    work->m_order[i - 1] = n;
                }
            }
        }
        REMOVE_SLOT_ONE(work, index, i);
        break;
    case 0:
        for (n = 0; n < work->m_layerCount * 2; n++) {
            index = work->m_order[n];
            if (index == -1) {
                break;
            }
            layer = &work->m_layers[index];
            work->m_order[n] = -1;
            if (n >= pos) {
                if (n == pos) {
                    layer->m_flags2 &= ~0x200;
                    layer->m_blend->m_flags &= ~2;
                }
                work->m_order[n - pos] = index;
            } else {
                if (!(layer->m_blend->m_flags & 2)) {
                    layer->m_flags &= ~4;
                    layer->m_flags &= ~1;
                    layer->m_flags &= ~2;
                    layer->m_flags2 &= ~0x4000;
                    layer->m_flags2 &= ~0x1000;
                    if (layer->m_flags & 0x10) {
                        fn_8041CC18(work->m_model, index);
                        layer->m_flags &= ~0x10;
                    }
                    if (layer->m_motion) {
                        fn_80413240(layer->m_motion);
                    }
                    REMOVE_SLOT_ONE(work, index, i);
                }
                layer->m_blend->m_flags &= ~2;
            }
        }
        break;
    }
    return 1;
}

extern "C" void fn_80416148(MotionPlayWork *work, int slot, float time) {
    MotionLayer *layer;

    if (!work) {
        return;
    }
    if (time < 0.0f) {
        return;
    }
    layer = GetLayer(work, slot);
    if (!layer) {
        return;
    }
    layer->m_blend->m_time = time;
}

extern "C" void fn_804160F8(MotionPlayWork *work, int slot, float rate) {
    MotionLayer *layer;

    if (!work) {
        return;
    }
    if (rate < 0.0f) {
        return;
    }
    layer = GetLayer(work, slot);
    if (!layer) {
        return;
    }
    layer->m_blend->m_rate = rate;
}

/* Sets the blend weight of a slot, clamped to [0, 1]. */
extern "C" void fn_80416014(MotionPlayWork *work, int slot, float weight) {
    MotionLayer *layer;
    MotionBlend *blend;

    if (!work) {
        return;
    }
    layer = GetLayer(work, slot);
    if (!layer) {
        return;
    }
    if ((layer->m_flags2 & 0x200) &&
        ((blend = layer->m_blend)->m_mode == 1 || blend->m_mode == 2)) {
        blend->m_weight = weight;
        if (layer->m_blend->m_weight < 0.0f) {
            layer->m_blend->m_weight = 0.0f;
        }
        if (layer->m_blend->m_weight > 1.0f) {
            layer->m_blend->m_weight = 1.0f;
        }
    } else {
        blend = layer->m_blend;
        if (blend->m_type == 1 || blend->m_type == 2) {

            blend->m_weight2 = weight;
            if (layer->m_blend->m_weight2 < 0.0f) {
                layer->m_blend->m_weight2 = 0.0f;
            }
            if (layer->m_blend->m_weight2 > 1.0f) {
                layer->m_blend->m_weight2 = 1.0f;
            }
        }
    }
}

extern "C" void fn_80415F98(MotionPlayWork *work, int slot, u32 key, float scale, float offset) {
    MotionLayer *layer;
    int index;

    if (work) {
        index = work->m_slots[slot];
        if (index == -1) {
            layer = 0;
        } else {
            layer = &work->m_layers[index];
        }
        if (layer) {
            fn_8041CFB0(work->m_model, index, key, scale, offset);
            layer->m_flags2 |= 1;
            layer->m_flags2 |= 0x2000;
        }
    }
}

/* Clears the per-frame flags of the layers of a play work and advances them in blend order
 * (name inferred). An inline function with its locals in the loop: GC/2.7 then copies the
 * zeroed counter into the loop's offset (mr) as retail does, where the loop written in
 * fn_80415EA4 itself loads the zero again (wave 40, probes/Game_MotionPlayWork.py). */
inline void AdvanceLayers(MotionPlayWork *work) {
    for (int i = 0; i < work->m_layerCount; i++) {
        int index = work->m_order[i];
        if (index == -1) {
            break;
        }
        MotionLayer *layer = &work->m_layers[index];
        layer->m_flags &= 0xFFFF;
        fn_8041AB3C(work, index);
        if (layer->m_flags & 4) {
            layer->m_lastFrame = layer->m_frames.m_frame;
        }
    }
}

/* Updates a play work. */
extern "C" void fn_80415EA4(MotionPlayWork *work) {
    if (!work) {
        return;
    }
    fn_8040C204(lbl_805F13A8);
    fn_8040B6F8(TopMatrix());
    work->m_flags &= ~0xFF0000;
    AdvanceLayers(work);
    fn_80419F5C(work);
    fn_804192EC(work);
    fn_80418A4C(work);
    fn_804188A0(work);
    fn_80418044(work);
    fn_804175C8(work);
    fn_8040C244(lbl_805F13A8);
}

extern "C" MotionNode *fn_80415E70(MotionPlayWork *work, u32 key) {
    if (work) {
        return fn_8041D31C(work->m_model, key);
    }
    return 0;
}

extern "C" void fn_80415E34(MotionPlayWork *work, Vec3f *out) {
    if (work == 0 || out == 0) {
        return;
    }
    fn_800091D0(out, &work->m_position);
}

extern "C" void fn_80415DF8(MotionPlayWork *work, Vec3f *out) {
    if (work == 0 || out == 0) {
        return;
    }
    fn_800091D0(out, &work->m_rotation);
}

extern "C" void fn_80415DBC(MotionPlayWork *work, Vec3f *out) {
    if (work == 0 || out == 0) {
        return;
    }
    fn_800091D0(out, &work->field_0x140);
}

/* The frames of a slot if it is playing. */
extern "C" void fn_80415D40(MotionPlayWork *work, int slot, float *frames) {
    MotionLayer *layer;

    if (work == 0 || frames == 0) {
        return;
    }
    layer = GetPlayingLayer(work, slot);
    if (!layer) {
        return;
    }
    frames[0] = layer->m_frames.m_start;
    frames[1] = layer->m_frames.m_end;
    frames[2] = layer->m_frames.m_speed;
    frames[3] = layer->m_frames.m_frame;
}

/* The motion of a slot if it is playing, else 0. */
extern "C" u16 fn_80415CDC(MotionPlayWork *work, int slot) {
    MotionLayer *layer;
    if (!work) {
        return 0;
    }
    layer = GetPlayingLayer(work, slot);
    if (layer == 0) {
        return 0;
    } else {
        return layer->m_motion;
    }
}


/* Moves a play work by offset. */
extern "C" void fn_80415BDC(MotionPlayWork *work, const Vec3f *offset) {
    int i;
    MotionNode *node;
    MotionBody *header;

    if (work == 0 || offset == 0) {
        return;
    }
    node = work->m_nodes;
    for (i = 0; i < work->m_nodeCount; i++) {
        node->m_matrix.values[0][3] += offset->x;
        node->m_matrix.values[1][3] += offset->y;
        node->m_matrix.values[2][3] += offset->z;
        node++;
    }
    fn_80411548(&work->field_0x150, &work->field_0x150, offset);
    header = work->m_model->m_header;
    if (header->m_type == 1 && (header->m_flags & 7)) {
        fn_80411548(&work->field_0x130, &work->field_0x130, offset);
        fn_80411548(&work->m_position, &work->m_position, offset);
        fn_80411548(&work->field_0x110, &work->field_0x110, offset);
    } else {
        fn_80411548(&work->m_position, &work->m_position, offset);
    }
}


/* The same as fn_80415854 for the second kind of target. */
extern "C" int fn_80415A18(MotionPlayWork *work, int slot, int target, void *arg, int count) {
    u16 motion = 0;
    int stopped = 0;
    float frames[4];
    int result;
    MotionLayer *layer;

    if (work) {
        lbl_805F13DC = GetPlayingLayer(work, slot);
        if (!lbl_805F13DC) {
            return 0;
        }
        motion = lbl_805F13DC->m_motion;
    }
    layer = lbl_805F13DC;
    if (!layer) {
        return 0;
    }
    frames[0] = layer->m_frames.m_start;
    frames[1] = layer->m_frames.m_end;
    frames[2] = layer->m_frames.m_speed;
    frames[3] = layer->m_frames.m_frame;
    frames[2] = frames[2] * lbl_805EDD60;
    if (layer->m_flags & 0x20000) {
        if (std::fabs(frames[2]) > 0.0f) {
            stopped = 1;
        }
        frames[2] = 0.0f;
    } else if (layer->m_flags & 0x400000) {
        if (layer->m_flags & 2) {
            if (frames[2] >= 0.0f) {
                if (std::fabs(layer->m_lastFrame - frames[1]) < 0.0001f) {
                    frames[2] += 0.0001f;
                }
            } else {
                if (std::fabs(layer->m_lastFrame - frames[0]) < 0.0001f) {
                    frames[2] -= 0.0001f;
                }
            }
        } else {
            frames[2] = frames[3] - layer->m_lastFrame;
        }
    }
    result = fn_80411DEC(motion, target, frames, arg, stopped);
    while (count-- != 0 && result != 0) {
        result = fn_80411DEC(0, target, frames, arg, stopped);
    }
    return result;
}

/* Plays the motion of a slot onto target, count more times while it keeps playing. */
extern "C" int fn_80415854(MotionPlayWork *work, int slot, int target, void *arg, int count) {
    u16 motion = 0;
    int stopped = 0;
    float frames[4];
    int result;
    MotionLayer *layer;

    if (work) {
        lbl_805F13E0 = GetPlayingLayer(work, slot);
        if (!lbl_805F13E0) {
            return 0;
        }
        motion = lbl_805F13E0->m_motion;
    }
    layer = lbl_805F13E0;
    if (!layer) {
        return 0;
    }
    frames[0] = layer->m_frames.m_start;
    frames[1] = layer->m_frames.m_end;
    frames[2] = layer->m_frames.m_speed;
    frames[3] = layer->m_frames.m_frame;
    frames[2] = frames[2] * lbl_805EDD60;
    if (layer->m_flags & 0x20000) {
        if (std::fabs(frames[2]) > 0.0f) {
            stopped = 1;
        }
        frames[2] = 0.0f;
    } else if (layer->m_flags & 0x400000) {
        if (layer->m_flags & 2) {
            if (frames[2] >= 0.0f) {
                if (std::fabs(layer->m_lastFrame - frames[1]) < 0.0001f) {
                    frames[2] += 0.0001f;
                }
            } else {
                if (std::fabs(layer->m_lastFrame - frames[0]) < 0.0001f) {
                    frames[2] -= 0.0001f;
                }
            }
        } else {
            frames[2] = frames[3] - layer->m_lastFrame;
        }
    }
    result = fn_80411818(motion, target, frames, arg, stopped);
    while (count-- != 0 && result != 0) {
        result = fn_80411818(0, target, frames, arg, stopped);
    }
    return result;
}

extern "C" s32 fn_8041583C(MotionPlayWork *work) {
    if (work) {
        return work->m_layerCount;
    }
    return 0;
}

extern "C" int fn_80415808(MotionPlayWork *work, u32 key,
                           void (*callback)(MotionCallbackArgs *args), u32 arg) {
    if (work) {
        return fn_8041D1A0(work->m_model, key, callback, arg);
    }
    return 0;
}

/* Sets the time scale of the motions (not below 0) and returns the old one. */
extern "C" float fn_804157E8(float scale) {
    float old = lbl_805EDD60;
    if (scale < 0.0f) {
        scale = 0.0f;
    }
    lbl_805EDD60 = scale;
    return old;
}

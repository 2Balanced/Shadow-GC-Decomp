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
 * Open: the Euler-angle channels (fn_8041DBD4, fn_8041E480, fn_8041FB4C, ...) convert float to int
 * and back through one shared stack slot, which GC/2.7 never does (docs/research-notes.md);
 * fn_80420590 keeps its loads after the stores into its local matrix in retail while GC/2.7 hoists
 * them; most of the other functions differ only in register numbers or in a few scheduled rows. */
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
};

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
void fn_8041CC18(MotionModel *model, int layer);
int fn_8041CFB0(MotionModel *model, int index, u32 key, float scale, float offset);
int fn_8041D1A0(MotionModel *model, u32 key, void (*callback)(MotionCallbackArgs *args), u32 arg);
MotionNode *fn_8041D31C(MotionModel *model, u32 key);
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
MotionPlayWork *fn_8041B980(MotionData *data, int layers);
void fn_8041CA78(u32 *sourceSize, u32 *channelSize, s32 *nodeCount, int layers, MotionBody *body);
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
void fn_8041CD6C(MotionModel *model, int layer);
void fn_804114D0(Vec3f *v, int flags);
void fn_80420590(Mtx34f *out, const Mtx34f *from, const Mtx34f *to, float t);
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
void fn_8041CE74(MotionModel *model, int layer);
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
#define ROTATE_ZYX(x, y, z)                                                                        \
    if (z) {                                                                                       \
        fn_8040B80C(TopMatrix(), 6.2831855f * (u16)(z) / 65536.0f);                                \
    }                                                                                              \
    if (y) {                                                                                       \
        fn_8040B848(TopMatrix(), 6.2831855f * (u16)(y) / 65536.0f);                                \
    }                                                                                              \
    if (x) {                                                                                       \
        fn_8040B884(TopMatrix(), 6.2831855f * (u16)(x) / 65536.0f);                                \
    }

// DEFINITIONS
/* Blends the rotations of two matrices through quaternions. */
extern "C" void fn_8042062C(Mtx34f *out, const Mtx34f *from, const Mtx34f *to, float t) {
    Quatf a;
    Quatf b;
    Quatf blended;

    fn_804208EC(&a, from);
    fn_804208EC(&b, to);
    fn_80420724(&blended, &a, &b, t);
    fn_80420B60(out, &blended);
    fn_8041116C(out, out);
}

/* Blends two matrices: rotation by fn_8042062C, translation linearly. */
extern "C" void fn_80420590(Mtx34f *out, const Mtx34f *from, const Mtx34f *to, float t) {
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
    MotionBody *mirror = (MotionBody *)motion->m_link[0];
    MotionBody *body = motion->m_body;
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

    fn_8040B59C(TopMatrix(), (Vec3f *)args->m_entries[args->m_list->m_order[0]].m_data);
}

/* Rotation channel (Euler angles) of a single layer. */
extern "C" void fn_8041FB4C(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    Vec3f *angles = (Vec3f *)args->m_entries[args->m_list->m_order[0]].m_data;
    int x = 65536.0f * angles->x / 6.2831855f;
    int y = 65536.0f * angles->y / 6.2831855f;
    int z = 65536.0f * angles->z / 6.2831855f;

    ROTATE_ZYX(x, y, z);
}

/* Scale channel of a single layer. */
extern "C" void fn_8041FAD8(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    Vec3f *scale = (Vec3f *)args->m_entries[args->m_list->m_order[0]].m_data;

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
    int i;

    fn_8040C204(lbl_805F13A8);
    for (i = 0; i < 5; i++) {
        fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[i]);
        ROTATE_ZYX(x, y, z);
        fn_8040C148(&lbl_805E12E0[i], fn_80411700(lbl_805F13A8));
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

/* Blended translation channel of a root frame: moves the layer matrices, then blends their
 * positions. */
extern "C" void fn_8041F290(void *data) {
    MotionChannelArgs *args = (MotionChannelArgs *)data;
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    MotionChannelLayer *entries = args->m_entries;
    Mtx34f matrix;
    Vec3f result;
    Vec3f position;
    Vec3f basePosition;
    Vec3f offset;
    int i;
    int j;
    int index;
    int other;
    int again;
    int parent;
    float weight;
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
    fn_8040C204(lbl_805F13A8);
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        src = (Vec3f *)entries[index].m_data;
        if ((entries[index].m_blend->m_flags & 1) && i == 0) {
            offset.x = target->m_matrix.values[0][3];
            offset.y = target->m_matrix.values[1][3];
            offset.z = target->m_matrix.values[2][3];
            fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[4]);
            fn_8040B59C(TopMatrix(), &offset);
            fn_8040C148(&lbl_805E12E0[4], fn_80411700(lbl_805F13A8));
        }
        fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[i]);
        fn_8040B59C(TopMatrix(), src);
        fn_8040C148(&lbl_805E12E0[i], fn_80411700(lbl_805F13A8));
    }
    do {
        again = 0;
        for (i = 0; i < list->m_count; i++) {
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
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            position.x = lbl_805E12E0[i].values[0][3];
            position.y = lbl_805E12E0[i].values[1][3];
            position.z = lbl_805E12E0[i].values[2][3];
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
            if (entries[index].m_blend->m_flags & 1) {
                if (i == 0) {
                    basePosition.x = lbl_805E12E0[4].values[0][3];
                    basePosition.y = lbl_805E12E0[4].values[1][3];
                    basePosition.z = lbl_805E12E0[4].values[2][3];
                    base = &basePosition;
                }
                weight = lbl_805E14F0[index];
                dst->x = weight * src->x + (1.0f - weight) * base->x;
                dst->y = weight * src->y + (1.0f - weight) * base->y;
                dst->z = weight * src->z + (1.0f - weight) * base->z;
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
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    MotionChannelLayer *entries = args->m_entries;
    Mtx34f result;
    Mtx34f matrix;
    int i;
    int j;
    int index;
    int other;
    int again;
    int parent;
    int x;
    int y;
    int z;
    Vec3f *angles;
    Mtx34f *dst;
    Mtx34f *base;
    Mtx34f *src;

    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    fn_8040C204(lbl_805F13A8);
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        angles = (Vec3f *)entries[index].m_data;
        if ((entries[index].m_blend->m_flags & 1) && i == 0) {
            fn_8040C148(&matrix, &target->m_matrix);
            matrix.values[2][3] = matrix.values[1][3] = matrix.values[0][3] = 0.0f;
            fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[4]);
            fn_8040BAC4(TopMatrix(), &matrix);
            fn_8040C148(&lbl_805E12E0[4], fn_80411700(lbl_805F13A8));
        }
        fn_8040C148(fn_80411700(lbl_805F13A8), &lbl_805E12E0[i]);
        x = 65536.0f * angles->x / 6.2831855f;
        y = 65536.0f * angles->y / 6.2831855f;
        z = 65536.0f * angles->z / 6.2831855f;
        ROTATE_ZYX(x, y, z);
        fn_8040C148(&lbl_805E12E0[i], fn_80411700(lbl_805F13A8));
    }
    do {
        again = 0;
        for (i = 0; i < list->m_count; i++) {
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
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            if (s_isParent[index] != 0) {
                if (s_rootSet[index] == 0) {
                    fn_8040C148(&s_matrices[index], &lbl_805E12E0[i]);
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
                src = &lbl_805E12E0[i];
            }
            if (entries[index].m_blend->m_flags & 1) {
                if (i == 0) {
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
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    MotionChannelLayer *entries = args->m_entries;
    Vec3f result;
    int i;
    int j;
    int index;
    int other;
    int again;
    int parent;
    float weight;
    Vec3f *dst;
    Vec3f *base;
    Vec3f *src;

    result.z = result.y = result.x = 1.0f;
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        again = 0;
        for (i = 0; i < list->m_count; i++) {
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
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            src = (Vec3f *)entries[index].m_data;
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
                dst = &result;
            } else {
                dst = &s_roots[parent];
                s_rootSet[parent] = 1;
            }
            if (s_rootSet[index] != 0) {
                src = &s_roots[index];
            }
            if (entries[index].m_blend->m_flags & 1) {
                base = dst;
                if (i == 0) {
                    fn_80411328(&result, &target->m_matrix);
                    base = &result;
                }
                weight = lbl_805E14F0[index];
                dst->x = weight * src->x + (1.0f - weight) * base->x;
                dst->y = weight * src->y + (1.0f - weight) * base->y;
                dst->z = weight * src->z + (1.0f - weight) * base->z;
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
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    MotionChannelLayer *entries = args->m_entries;
    Vec3f result;
    int i;
    int j;
    int index;
    int other;
    int again;
    int parent;
    float weight;
    Vec3f *dst;
    Vec3f *base;
    Vec3f *src;

    result.z = result.y = result.x = 1.0f;
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        again = 0;
        for (i = 0; i < list->m_count; i++) {
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
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            src = (Vec3f *)entries[index].m_data;
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
            if (entries[index].m_blend->m_flags & 1) {
                if (i == 0) {
                    base = &target->m_scale;
                }
                weight = lbl_805E14F0[index];
                dst->x = weight * src->x + (1.0f - weight) * base->x;
                dst->y = weight * src->y + (1.0f - weight) * base->y;
                dst->z = weight * src->z + (1.0f - weight) * base->z;
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
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    MotionChannelLayer *entries = args->m_entries;
    Vec3f result;
    Vec3f position;
    int i;
    int j;
    int index;
    int other;
    int again;
    int parent;
    float weight;
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
        again = 0;
        for (i = 0; i < list->m_count; i++) {
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
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            src = (Vec3f *)entries[index].m_data;
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
            if (entries[index].m_blend->m_flags & 1) {
                if (i == 0) {
                    position.x = target->m_matrix.values[0][3];
                    position.y = target->m_matrix.values[1][3];
                    position.z = target->m_matrix.values[2][3];
                    base = &position;
                }
                weight = lbl_805E14F0[index];
                dst->x = weight * src->x + (1.0f - weight) * base->x;
                dst->y = weight * src->y + (1.0f - weight) * base->y;
                dst->z = weight * src->z + (1.0f - weight) * base->z;
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
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    MotionChannelLayer *entries = args->m_entries;
    Mtx34f result;
    Mtx34f matrix;
    int i;
    int j;
    int index;
    int other;
    int again;
    int parent;
    int x;
    int y;
    int z;
    Vec3f *angles;
    Mtx34f *dst;
    Mtx34f *src;

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
        again = 0;
        for (i = 0; i < list->m_count; i++) {
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
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            angles = (Vec3f *)entries[index].m_data;
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
                s_rootSet[parent] = 1;
                dst = &s_matrices[parent];
            }
            if (s_rootSet[index] != 0) {
                src = &s_matrices[index];
            } else {
                fn_8040B6F8(TopMatrix());
                ROTATE_ZYX(x, y, z);
                fn_8040C148(&matrix, fn_80411700(lbl_805F13A8));
                src = &matrix;
            }
            if (entries[index].m_blend->m_flags & 1) {
                if (i == 0) {
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
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    MotionChannelLayer *entries = args->m_entries;
    Vec3f result;
    int i;
    int j;
    int index;
    int other;
    int again;
    int parent;
    float weight;
    Vec3f *dst;
    Vec3f *base;
    Vec3f *src;

    result.z = result.y = result.x = 1.0f;
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        again = 0;
        for (i = 0; i < list->m_count; i++) {
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
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            src = (Vec3f *)entries[index].m_data;
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
                dst = &result;
            } else {
                dst = &s_roots[parent];
                s_rootSet[parent] = 1;
            }
            if (s_rootSet[index] != 0) {
                src = &s_roots[index];
            }
            if (entries[index].m_blend->m_flags & 1) {
                base = dst;
                if (i == 0) {
                    fn_80411328(&result, &target->m_matrix);
                    base = &result;
                }
                weight = lbl_805E14F0[index];
                dst->x = weight * src->x + (1.0f - weight) * base->x;
                dst->y = weight * src->y + (1.0f - weight) * base->y;
                dst->z = weight * src->z + (1.0f - weight) * base->z;
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
    MotionLayerList *list = args->m_list;
    MotionNodeWorld *target = args->m_target;
    MotionChannelLayer *entries = args->m_entries;
    Vec3f result;
    int i;
    int j;
    int index;
    int other;
    int again;
    int parent;
    float weight;
    Vec3f *dst;
    Vec3f *base;
    Vec3f *src;

    result.z = result.y = result.x = 1.0f;
    for (i = 0; i < list->m_count; i++) {
        index = list->m_order[i];
        if (index == -1) {
            break;
        }
        s_done[index] = 0;
        s_rootSet[index] = 0;
    }
    do {
        again = 0;
        for (i = 0; i < list->m_count; i++) {
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
        for (i = 0; i < list->m_count; i++) {
            index = list->m_order[i];
            if (index == -1) {
                break;
            }
            if (s_done[index] != 0) {
                continue;
            }
            src = (Vec3f *)entries[index].m_data;
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
            if (entries[index].m_blend->m_flags & 1) {
                if (i == 0) {
                    base = &target->m_scale;
                }
                weight = lbl_805E14F0[index];
                dst->x = weight * src->x + (1.0f - weight) * base->x;
                dst->y = weight * src->y + (1.0f - weight) * base->y;
                dst->z = weight * src->z + (1.0f - weight) * base->z;
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
extern "C" MotionNode *fn_8041D31C(MotionModel *model, u32 key) {
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
extern "C" int fn_8041CFB0(MotionModel *model, int index, u32 key, float scale, float offset) {
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
extern "C" void fn_8041CE74(MotionModel *model, int index) {
    MotionWeightScale *scale = &model->m_weightScales[index];

    if (scale->m_pending) {
        model->m_weightScales[index].m_enabled = 1;
        scale->m_scale = scale->m_newScale;
        scale->m_offset = scale->m_newOffset;
    }
    if (model->m_child) {
        ApplyWeightScale(model->m_child, index);
    }
    if (model->m_next) {
        ApplyWeightScale(model->m_next, index);
    }
}

/* Drops the pending weight scale of a layer in a model frame and the frames below and after it. */
extern "C" void fn_8041CD6C(MotionModel *model, int index) {
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
extern "C" void fn_8041CC18(MotionModel *model, int index) {
    MotionWeightScale *scale = &model->m_weightScales[index];

    scale->m_enabled = 0;
    scale->m_pending = 0;
    scale->m_scale = 1.0f;
    scale->m_offset = 0.0f;
    scale->m_newScale = 1.0f;
    scale->m_newOffset = 0.0f;
    if (model->m_child) {
        ResetWeightScale(model->m_child, index);
    }
    if (model->m_next) {
        ResetWeightScale(model->m_next, index);
    }
}

/* Adds up the source and channel storage and the nodes a body tree needs for layers layers. */
extern "C" void fn_8041CA78(u32 *sourceSize, u32 *channelSize, s32 *nodeCount, int layers,
                            MotionBody *body) {
    u32 sources = 0;
    u32 channels;

    if (body->m_flags & 0x400) {
        channels = (layers * 2 + 2) * 4 + 0x3C;
        sources = 0xE0;
    } else {
        channels = 0x3C;
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

    if (body->m_flags & 0x400) {
        (*cursor)->m_update = fn_80420308;
        (*cursor)->m_next = 0;
        next = (MotionSource *)((u8 *)*cursor + 0xE0);
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
        (*cursor)->m_rootMotion.m_link[0] = body;
        next = (MotionSource *)((u8 *)*cursor + 0xF0);
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
        (*cursor)->m_rootMotion.m_body = body;
        next = (MotionSource *)((u8 *)*cursor + 0xF0);
        if (*last) {
            (*last)->m_next = *cursor;
        }
        *last = *cursor;
        *cursor = next;
    } else if (body->m_flags & 0x38) {
        (*cursor)->m_update = fn_8042043C;
        (*cursor)->m_update2 = fn_80420158;
        (*cursor)->m_next = 0;
        (*cursor)->m_rootMotion.m_body = body;
        next = (MotionSource *)((u8 *)*cursor + 0xF0);
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

    if (body->field_0xA & 0x8000) {
        strcpy(name, body->m_name);
        fn_803AC6C0(name, "_l")[1] = 'r';
        other = first;
        mirror = fn_8041C5A8(root, &other, fn_80410684(name));
        if (mirror) {
            if (body->m_flags & 0x400) {
                *cursor = (*cursor)->m_next;
            }
            if (mirror->m_flags & 0x400) {
                other = other->m_next;
            }
            if (body->m_flags & 7) {
                (*cursor)->m_rootMotion.m_link[1] = mirror;
                (*cursor)->m_rootMotion.m_link[2] = other;
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
                (*cursor)->m_rootMotion.m_link[0] = mirror;
                (*cursor)->m_rootMotion.m_link[1] = other;
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
    MotionChannelLayer *entry;
    MotionSource *source;
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
        (*nodes)++;
        target = &model->m_output->m_world;
        break;
    default:
        if (body->m_flags != 0 || (body->field_0x8 & 4)) {
            model->m_output = *nodes;
            (*nodes)++;
            target = &model->m_output->m_world;
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
            entry->m_data = (u8 *)&source->m_rootMotion;
            source->m_rootMotion.m_position.x = 1.0f;
            source->m_rootMotion.m_position.y = 1.0f;
            source->m_rootMotion.m_position.z = 1.0f;
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
        case 6:
            channel->m_function = fn_8041FD6C;
            break;
        default:
            channel->m_function = fn_8041FD70;
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

/* Allocates and builds a play work with layers layers (1 to 4) for bone data. */
extern "C" MotionPlayWork *fn_8041B980(MotionData *data, int layers) {
    MotionSource *sources[4];
    u32 sourceSize;
    u32 channelSize;
    s32 nodeCount;
    MotionSource *last;
    MotionSource *source;
    MotionModel *model;
    MotionNode *nodes;
    u8 *scales;
    MotionBody *body = &data->m_body;
    MotionPlayWork *work;
    u8 *p;
    MotionBlend *blends;
    MotionNode *node;
    int i;

    sourceSize = 0;
    channelSize = 0;
    nodeCount = 0;
    if (layers < 1) {
        layers = 1;
    } else if (layers > 4) {
        layers = 4;
    }
    fn_8041CA78(&sourceSize, &channelSize, &nodeCount, layers, body);
    work = (MotionPlayWork *)fn_80411134(
        1, nodeCount * 0x60 + nodeCount * 0x10 + layers * sizeof(MotionLayer) +
               sourceSize * layers + layers * 8 + layers * 4 + layers * 8 +
               layers * sizeof(MotionBlend) + layers * (data->field_0x6 * 32) + channelSize +
               sizeof(MotionPlayWork));
    if (!work) {
        fn_803A953C("Cannot get PlayWork.\n");
        return 0;
    }
    work->m_skeleton = data;
    work->m_nodeCount = nodeCount;
    work->m_nodes = (MotionNode *)(work + 1);
    work->m_layers = (MotionLayer *)(work->m_nodes + work->m_nodeCount);
    p = (u8 *)(work->m_layers + layers);
    for (i = 0; i < layers; i++) {
        if (sourceSize) {
            work->m_layers[i].m_source = (MotionSource *)p;
            p += sourceSize;
        } else {
            work->m_layers[i].m_source = 0;
        }
    }
    work->m_layerCount = layers;
    work->m_order = (s32 *)p;
    work->m_slots = (s32 *)(p + layers * 8);
    work->m_parents = (s32 *)(p + layers * 8 + layers * 4);
    blends = (MotionBlend *)(p + layers * 8 + layers * 4 + layers * 8);
    for (i = 0; i < layers * 2; i++) {
        work->m_order[i] = -1;
        work->m_parents[i] = -1;
    }
    for (i = 0; i < layers; i++) {
        work->m_slots[i] = -1;
    }
    work->m_order[0] = 0;
    work->m_slots[0] = 0;
    for (i = 0; i < layers; i++) {
        work->m_layers[i].m_blend = &blends[i];
        work->m_layers[i].m_frames.m_speed = 1.0f;
        work->m_layers[i].m_request.m_speed = 1.0f;
        work->m_layers[i].m_request.m_frame = -1.0f;
        work->m_layers[i].m_request.m_start = -1.0f;
        work->m_layers[i].m_request.m_end = -1.0f;
    }
    scales = (u8 *)(blends + layers);
    work->m_model = (MotionModel *)(scales + layers * (data->field_0x6 * 32));
    for (i = 0; i < layers; i++) {
        source = work->m_layers[i].m_source;
        last = 0;
        if (source) {
            fn_8041C854(&source, &last, body);
            source = work->m_layers[i].m_source;
            fn_8041C2E0(body, body, &source, work->m_layers[i].m_source);
        }
    }
    nodes = work->m_nodes;
    model = work->m_model;
    for (i = 0; i < work->m_layerCount; i++) {
        sources[i] = work->m_layers[i].m_source;
    }
    fn_8041BE30(work, &model, blends, &scales, &nodes, sources, body);
    fn_8040C204(lbl_805F13A8);
    fn_8040B6F8(TopMatrix());
    node = work->m_nodes;
    for (i = 0; i < work->m_nodeCount; i++) {
        fn_8040C148(&node->m_matrix, fn_80411700(lbl_805F13A8));
        fn_8040C148(&node->m_world.m_matrix, fn_80411700(lbl_805F13A8));
        node++;
    }
    fn_8040C148(&work->m_matrix, fn_80411700(lbl_805F13A8));
    fn_8040C148(&work->m_prevMatrix, fn_80411700(lbl_805F13A8));
    fn_8040C148(&work->m_rootMatrix, fn_80411700(lbl_805F13A8));
    fn_8040C148(&work->m_prevRootMatrix, fn_80411700(lbl_805F13A8));
    for (i = 0; i < layers; i++) {
        fn_8040C148(&work->m_layers[i].m_prevMatrix, fn_80411700(lbl_805F13A8));
        fn_8040C148(&work->m_layers[i].m_matrix, fn_80411700(lbl_805F13A8));
        fn_8040C148(&work->m_layers[i].m_baseMatrix, fn_80411700(lbl_805F13A8));
    }
    fn_8040C244(lbl_805F13A8);
    for (i = 0; i < layers; i++) {
        fn_8041CC18(work->m_model, i);
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
    if (data->m_type != 0xFF) {
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

    if (model->field_0x0 & 0x400) {
        entry = channel->m_layers[index].m_data;
        scale = (Vec3f *)entry;
        tracks = (HalfKeyTrack *)(entry + 0x10);
        scale->x = scale->y = scale->z = 1.0f;
        fn_80413E10(&tracks[0]);
        fn_80413E10(&tracks[1]);
        fn_80413E10(&tracks[2]);
        channel = channel->m_next;
    }
    if (model->field_0x0 & 7) {
        entry = channel->m_layers[index].m_data;
        memcpy(entry, &model->m_header->m_position, sizeof(Vec3f));
        tracks = (HalfKeyTrack *)(entry + 0x20);
        fn_80413E10(&tracks[0]);
        fn_80413E10(&tracks[1]);
        fn_80413E10(&tracks[2]);
    }
    channel = channel->m_next;
    if (!(model->field_0x0 & 0x40) && (model->field_0x0 & 0x38)) {
        entry = channel->m_layers[index].m_data;
        memcpy(entry, &model->m_header->m_rotation, 0x10);
        tracks = (HalfKeyTrack *)(entry + 0x20);
        fn_80413E10(&tracks[0]);
        fn_80413E10(&tracks[1]);
        fn_80413E10(&tracks[2]);
    }
    channel = channel->m_next;
    if (model->field_0x0 & 0x380) {
        entry = channel->m_layers[index].m_data;
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
    MotionModel *frame = fn_8041D448(work->m_model, body->m_key);
    MotionCallback *channel = 0;
    u8 *entry;
    HalfKeyTrack *tracks;

    if (frame) {
        channel = frame->m_callbacks;
    }
    if (body->m_flags & 0x400) {
        if (frame && (frame->field_0x0 & 0x400)) {
            entry = channel->m_layers[index].m_data;
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
            entry = channel->m_layers[index].m_data;
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
            entry = channel->m_layers[index].m_data;
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
            entry = channel->m_layers[index].m_data;
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
    MotionFrames *frames = &work->m_layers[index].m_frames;
    u32 key;
    MotionData *data;

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
    if (key != work->m_skeleton->m_key && std::fabs(data->m_scale) > 0.0001f &&
        std::fabs(work->m_skeleton->m_scale) > 0.0001f) {
        ratio = work->m_skeleton->m_scale / data->m_scale;
    }
    fn_8041B2BC(work, &data->m_body, index, motion, ratio);
    return 1;
}

/* Restarts the tracks of a layer in the channels of a model frame. */
extern "C" void fn_8041AFCC(MotionModel *model, int index) {
    MotionCallback *channel = model->m_callbacks;
    HalfKeyTrack *tracks;

    if (model->field_0x0 & 0x400) {
        tracks = (HalfKeyTrack *)(channel->m_layers[index].m_data + 0x10);
        RESET_TRACKS(tracks);
        channel = channel->m_next;
    }
    if (model->field_0x0 & 7) {
        tracks = (HalfKeyTrack *)(channel->m_layers[index].m_data + 0x20);
        RESET_TRACKS(tracks);
    }
    channel = channel->m_next;
    if (!(model->field_0x0 & 0x40) && (model->field_0x0 & 0x38)) {
        tracks = (HalfKeyTrack *)(channel->m_layers[index].m_data + 0x20);
        RESET_TRACKS(tracks);
    }
    channel = channel->m_next;
    if (model->field_0x0 & 0x380) {
        tracks = (HalfKeyTrack *)(channel->m_layers[index].m_data + 0x10);
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
    float speed = request->m_speed;
    float start;
    float end;
    float frame;

    if (frames->m_speed * speed < 0.0001f) {
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
    float length;
    MotionFrames frames;
    MotionFrames baseFrames;

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
    length = frames.m_end - frames.m_start;
    if (length < 0.0001f) {
        return;
    }
    if (layer->m_flags2 & 0x20) {
        base->m_request.m_frame = (baseFrames.m_end - baseFrames.m_start) *
                                      ((frames.m_frame - frames.m_start) / length) +
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
    int scaled = 0;
    int root = 0;
    MotionCallback *channel = model->m_callbacks;
    MotionBody *header = model->m_header;
    int i;
    int index;
    MotionWeightScale *scale;
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
            saved[index] = lbl_805E14F0[index];
            scaled = 1;
            lbl_805E14F0[index] =
                model->m_layerWeights[index].m_weight2 * scale->m_scale + scale->m_offset;
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
            channel->m_target->m_scale.x = 1.0f;
            channel->m_target->m_scale.y = 1.0f;
            channel->m_target->m_scale.z = 1.0f;
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

/* Takes the blended matrices of the cross-fading layers of a play work as their own. */
extern "C" void fn_8041A440(MotionPlayWork *work) {
    int count = work->m_layerCount;
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
    int k;
    MotionLayer *layer;
    MotionBlend *blend;

    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        blend = layer->m_blend;
        if (blend->m_flags & 2) {
            fn_8041A440(work);
            pos = -1;
            for (n = 0; n < work->m_layerCount * 2; n++) {
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
            case 1:
            case 2:
                break;
            default:
                if (blend->m_time < 0.0001f) {
                    ok = 0;
                }
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
                case 1:
                case 2:
                    break;
                default:
                    blend->m_elapsed = 0.0f;
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
        pos = -1;
        for (n = 0; n < work->m_layerCount * 2; n++) {
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
                    REMOVE_SLOT(work, other, j, k);
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
    float t;
    MotionFrames frames;
    Vec3f move;

    switch (layer->m_blend->m_type) {
    case 1:
        parent = work->m_parents[index];
        if (parent == -1) {
            break;
        }
        base = &work->m_layers[parent];
        if (!(base->m_flags & 4)) {
            break;
        }
        frames = layer->m_frames;
        frames.m_frame = layer->m_frames.m_start;
        t = (base->m_frames.m_frame - base->m_frames.m_start) /
            (base->m_frames.m_end - base->m_frames.m_start);
        frames.m_frame += (layer->m_frames.m_end - layer->m_frames.m_start) * t;
        if (base->m_flags & 0x80000) {
            fn_80419BA8(&base->m_prevRoot, base, &base->m_frames, 0);
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
    int changed = 0;
    int blended = 0;
    int i;
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
    int pos;
    int index;
    int other;
    int ended;
    int j;
    int k;
    MotionLayer *layer;
    MotionLayer *base;
    MotionBlend *blend;
    MotionBody *header;
    float frame;
    float t;
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
                    pos = -1;
                    for (n = 0; n < work->m_layerCount; n++) {
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
                                REMOVE_SLOT(work, other, j, k);
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
                t = (base->m_frames.m_frame - base->m_frames.m_start) /
                    (base->m_frames.m_end - base->m_frames.m_start);
                frame = (layer->m_frames.m_end - layer->m_frames.m_start) * t + layer->m_frames.m_start;
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
                            frames.m_start = layer->m_frames.m_start;
                            frames.m_end = layer->m_frames.m_end;
                            frames.m_speed = layer->m_frames.m_speed;
                            frames.m_frame = layer->m_frames.m_frame;
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
                                layer->m_frames.m_frame =
                                    layer->m_frames.m_frame -
                                    (layer->m_frames.m_end - layer->m_frames.m_start);
                                fn_8040CD74(&root, &layer->m_prevRoot, &move);
                                fn_800091D0(&layer->m_prevRoot, &root);
                                fn_8040B59C(TopMatrix(), &move);
                            }
                            fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
                            fn_8040C244(lbl_805F13A8);
                        } else {
                            while (layer->m_frames.m_frame >= layer->m_frames.m_end) {
                                layer->m_frames.m_frame =
                                    layer->m_frames.m_frame -
                                    (layer->m_frames.m_end - layer->m_frames.m_start);
                            }
                        }
                        ended = 1;
                        layer->m_flags |= 0x400000;
                    }
                } else if (layer->m_frames.m_speed < 0.0f) {
                    if (layer->m_frames.m_frame <= layer->m_frames.m_start) {
                        header = work->m_model->m_header;
                        if (header->m_type == 1 && (header->m_flags & 7)) {
                            frames.m_start = layer->m_frames.m_start;
                            frames.m_end = layer->m_frames.m_end;
                            frames.m_speed = layer->m_frames.m_speed;
                            frames.m_frame = layer->m_frames.m_frame;
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
                                layer->m_frames.m_frame =
                                    layer->m_frames.m_frame +
                                    (layer->m_frames.m_end - layer->m_frames.m_start);
                                fn_8040CD74(&root2, &layer->m_prevRoot, &move);
                                fn_800091D0(&layer->m_prevRoot, &root2);
                                fn_8040B59C(TopMatrix(), &move);
                            }
                            fn_8040C148(&layer->m_matrix, fn_80411700(lbl_805F13A8));
                            fn_8040C244(lbl_805F13A8);
                        } else {
                            while (layer->m_frames.m_frame <= layer->m_frames.m_start) {
                                layer->m_frames.m_frame =
                                    layer->m_frames.m_frame +
                                    (layer->m_frames.m_end - layer->m_frames.m_start);
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
            frames = layer->m_frames;
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
    int i;
    int index;
    MotionWeightScale *scale;
    MotionCallbackArgs args;
    float saved[4];

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
            saved[index] = lbl_805E14F0[index];
            scaled = 1;
            lbl_805E14F0[index] =
                model->m_layerWeights[index].m_weight2 * scale->m_scale + scale->m_offset;
        }
    }
    while (callback) {
        if (callback->m_function) {
            callback->m_function(&callback->m_data);
        }
        callback = callback->m_next;
    }
    if (model->m_callback) {
        args.m_matrix = TopMatrix();
        args.field_0x4 = model->field_0x20;
        args.field_0x8 = model->m_header->m_key;
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

/* Builds the matrices of the layers of a play work and blends them, then walks the model. */
extern "C" void fn_80418044(MotionPlayWork *work) {
    int fixPosition;
    int fixRotation;
    int i;
    int index;
    MotionLayer *layer;
    MotionBody *header;
    Vec3f move;
    Mtx34f other;
    Mtx34f rotation;
    Mtx34f matrix;
    Mtx34f prevRotation;
    Mtx34f prevMatrix;
    Mtx34f blended;
    Mtx34f current;

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
            if (i != 0) {
                fn_80420590(&current, &current, &blended, layer->m_blend->m_weight2);
            } else if (layer->m_blend->m_flags & 1) {
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
        result = work->m_flags & 1;
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
                    REMOVE_SLOT(work, index, i, k);
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
    int type = 3;
    int index;
    MotionLayer *layer;
    u32 blended;
    int i;
    int j;
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
    pos = -1;
    for (i = 0; i < work->m_layerCount * 2; i++) {
        if (work->m_order[i] == -1) {
            break;
        }
        if (work->m_order[i] == index) {
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
        REMOVE_SLOT(work, index, i, j);
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
                    REMOVE_SLOT(work, index, i, j);
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
    if (layer->m_flags2 & 0x200) {
        blend = layer->m_blend;
        if (blend->m_mode == 1 || blend->m_mode == 2) {
            blend->m_weight = weight;
            if (layer->m_blend->m_weight < 0.0f) {
                layer->m_blend->m_weight = 0.0f;
            }
            if (layer->m_blend->m_weight > 1.0f) {
                layer->m_blend->m_weight = 1.0f;
            }
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

/* Updates a play work. */
extern "C" void fn_80415EA4(MotionPlayWork *work) {
    int i;
    MotionLayer *layer;
    int index;

    if (!work) {
        return;
    }
    fn_8040C204(lbl_805F13A8);
    fn_8040B6F8(TopMatrix());
    work->m_flags &= ~0xFF0000;
    for (i = 0; i < work->m_layerCount; i++) {
        index = work->m_order[i];
        if (index == -1) {
            break;
        }
        layer = &work->m_layers[index];
        layer->m_flags &= 0xFFFF;
        fn_8041AB3C(work, index);
        if (layer->m_flags & 4) {
            layer->m_lastFrame = layer->m_frames.m_frame;
        }
    }
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
    if (!layer) {
        return 0;
    }
    return layer->m_motion;
}

/* Moves a play work by offset. */
extern "C" void fn_80415BDC(MotionPlayWork *work, const Vec3f *offset) {
    MotionNode *node;
    int i;
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

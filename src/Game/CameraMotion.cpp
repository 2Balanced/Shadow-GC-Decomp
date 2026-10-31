/* Camera motion helpers (no class with RTTI; file name and all names inferred): the view window
 * of a pair of view angles (fn_8000F860), the byte swap of a camera motion file's header and key
 * table (fn_8000F8B4), and the evaluation of a camera motion at a frame (fn_8000F9E8), which
 * fills a camera's eight values from the motion's tracks by their kinds. The unit is split by its
 * .sdata2 (the 0.5f at 0x805F17A8, padded to the next unit's constants). Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A track of a camera motion (struct name unknown; 0x28 bytes): its kind in the upper 24 bits of
 * the first word. */
struct UnkCameraTrack_8000F9E8 {
    /* 0x00 */ u32 m_kind;
    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
};
SIZE_ASSERT(UnkCameraTrack_8000F9E8, 0x28);

/* A camera motion: its tracks. */
struct UnkCameraMotion_8000F9E8 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ s32 m_trackCount;
    /* 0x10 */ UnkCameraTrack_8000F9E8 *m_tracks;
};

/* What a motion sets at a frame: eight values and the flags of those set. */
struct UnkCameraMotionState_8000F9E8 {
    /* 0x00 */ float m_values[8];
    /* 0x20 */ u32 m_set;
};

extern "C" {
/* A track's value at a frame (whether the frame is in it); the second takes an angle. */
int fn_80011A34(float *value, UnkCameraTrack_8000F9E8 *track, float frame);
int fn_8001188C(float *value, UnkCameraTrack_8000F9E8 *track, float frame);
/* The float tangent (Game/CameraProjection.cpp). */
float fn_8000F3D4(float x);
}

/* Evaluates motion at frame into state.
 * NonMatching (30 instructions in two runs): register allocation floor. Retail keeps the loop's
 * registers count in r27, state in r28, i in r29 and the tracks in r30; GC/2.7 gives state r27,
 * i r28, count r29 and the tracks r30 no matter how the loop is written. Fourteen shapes tried
 * (count in the condition, the index form, m_set = 0 before the loop, count after m_set, s32 and
 * u32 loop variables, i declared outside the for, i != count, ++i, the tracks indexed), every one
 * leaves the same two runs or makes the diff larger. */
extern "C" void fn_8000F9E8(UnkCameraMotionState_8000F9E8 *state, UnkCameraMotion_8000F9E8 *motion, float frame) {
    UnkCameraTrack_8000F9E8 *track = motion->m_tracks;
    int count = motion->m_trackCount;
    state->m_set = 0;
    for (int i = 0; i < count; i++, track++) {
        switch (track->m_kind & ~0xFF) {
        case 0x100:
            if (fn_80011A34(&state->m_values[0], track, frame) != 0) {
                state->m_set |= 1;
            }
            break;
        case 0x200:
            if (fn_80011A34(&state->m_values[1], track, frame) != 0) {
                state->m_set |= 2;
            }
            break;
        case 0x400:
            if (fn_80011A34(&state->m_values[2], track, frame) != 0) {
                state->m_set |= 4;
            }
            break;
        case 0x40000:
            if (fn_80011A34(&state->m_values[3], track, frame) != 0) {
                state->m_set |= 8;
            }
            break;
        case 0x80000:
            if (fn_80011A34(&state->m_values[4], track, frame) != 0) {
                state->m_set |= 0x10;
            }
            break;
        case 0x100000:
            if (fn_80011A34(&state->m_values[5], track, frame) != 0) {
                state->m_set |= 0x20;
            }
            break;
        case 0x200000:
            if (fn_8001188C(&state->m_values[6], track, frame) != 0) {
                state->m_set |= 0x40;
            }
            break;
        case 0x10000000:
            if (fn_8001188C(&state->m_values[7], track, frame) != 0) {
                state->m_set |= 0x80;
            }
            break;
        }
    }
}

/* Reverses the bytes of a word in place. */
inline void SwapBytes(void *word) {
    u8 *bytes = static_cast<u8 *>(word);
    u8 byte0 = bytes[0];
    u8 byte2 = bytes[2];
    bytes[0] = bytes[3];
    bytes[3] = byte0;
    bytes[2] = bytes[1];
    bytes[1] = byte2;
}

/* Swaps a camera motion file's header and, for a camera file (0xFF), its first table. */
extern "C" void fn_8000F8B4(u32 *header) {
    SwapBytes(&header[0]);
    if ((s32)header[0] != 0xFF) {
        return;
    }
    u32 *table = reinterpret_cast<u32 *>(header[1]);
    SwapBytes(&table[0]);
    SwapBytes(&table[1]);
    SwapBytes(&table[2]);
    SwapBytes(&table[3]);
    SwapBytes(&table[4]);
    SwapBytes(&table[5]);
    SwapBytes(&table[8]);
    SwapBytes(&table[11]);
}

/* The view window of view angles: the tangents of their halves. */
extern "C" void fn_8000F860(Vec2f *out, const Vec2f *angles) {
    out->x = fn_8000F3D4(0.5f * angles->x);
    out->y = fn_8000F3D4(0.5f * angles->y);
}

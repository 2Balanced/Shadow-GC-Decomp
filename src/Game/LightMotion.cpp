/* Light motions (no class with RTTI; file name and all names inferred): the byte swap of a light
 * chunk of a motion file (fn_800106B4; its size depends on the kind of light), the evaluation of
 * a light motion at a frame (fn_80010BF8), which fills a light's values from the motion's tracks
 * by their kinds, and the playing of one onto a light (fn_80010EE0). The unit is split by its
 * .data (the swap's jump table, padded to 8 bytes before the next unit's); it has no constants of
 * its own. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

/* A track of a motion (Game/MotionTrack.cpp; 0x28 bytes): its kind in the upper 24 bits of the
 * first word. */
struct UnkLightTrack_80010BF8 {
    /* 0x00 */ u32 m_kind;
    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
};
SIZE_ASSERT(UnkLightTrack_80010BF8, 0x28);

/* A light motion: its kind in the low half of the first word, and its tracks. */
struct UnkLightMotion_80010BF8 {
    /* 0x00 */ u32 m_kind;
    /* 0x04 */ u8 field_0x4[0xC - 0x4];
    /* 0x0C */ s32 m_trackCount;
    /* 0x10 */ UnkLightTrack_80010BF8 *m_tracks;
};

/* Three angles in 1/65536 of a turn. */
struct UnkLightAngles_80010EE0 {
    s32 x, y, z;
};

/* What a light motion sets at a frame, and the flags of those set. */
struct UnkLightMotionState_80010BF8 {
    /* 0x00 */ u32 m_set;
    /* 0x04 */ float m_color[4];
    /* 0x14 */ float m_position[3];
    /* 0x20 */ UnkLightAngles_80010EE0 m_angles;
    /* 0x2C */ float field_0x2C;
    /* 0x30 */ float field_0x30;
};
SIZE_ASSERT(UnkLightMotionState_80010BF8, 0x34);

/* A light's values. */
struct UnkLightData_80010EE0 {
    /* 0x00 */ u32 m_set;
    /* 0x04 */ float m_color[3];
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ union {
        UnkLightAngles_80010EE0 m_angles;
        float m_position[3];
    };
    /* 0x24 */ float field_0x24;
};

/* A light: its kind in the low byte of the first word. */
struct UnkLight_80010EE0 {
    /* 0x00 */ u32 m_kind;
    /* 0x04 */ UnkLightData_80010EE0 *m_data;
};

/* A light chunk: its kind and its light. */
struct UnkLightChunk_800106B4 {
    /* 0x00 */ u32 m_kind;
    /* 0x04 */ u32 *m_light;
};

extern "C" {
/* A track's value at a frame (whether the frame is in it); the second takes an angle
 * (Game/MotionTrack.cpp). */
int fn_80011A34(float *value, UnkLightTrack_80010BF8 *track, float frame);
int fn_8001188C(void *value, UnkLightTrack_80010BF8 *track, float frame);
void fn_80010BF8(UnkLightMotionState_80010BF8 *state, const UnkLightMotion_80010BF8 *motion, float frame);
}

/* Plays motion at frame on light. */
extern "C" void fn_80010EE0(UnkLight_80010EE0 *light, const UnkLightMotion_80010BF8 *motion, float frame) {
    if ((motion->m_kind & 0xFFFF) != 4) {
        return;
    }
    UnkLightData_80010EE0 *data = light->m_data;
    UnkLightMotionState_80010BF8 state;
    state.m_set = 0;
    fn_80010BF8(&state, motion, frame);
    data->m_set = state.m_set;
    if (state.m_set & 1) {
        data->m_color[0] = state.m_color[0];
    }
    if (state.m_set & 2) {
        data->m_color[1] = state.m_color[1];
    }
    if (state.m_set & 4) {
        data->m_color[2] = state.m_color[2];
    }
    if (state.m_set & 8) {
        data->field_0x14 = state.m_color[3];
    }
    switch (light->m_kind & 0xFF) {
    case 1:
        light->m_data->m_angles = state.m_angles;
        break;
    case 2: {
        UnkLightData_80010EE0 *point = light->m_data;
        if (state.m_set & 0x10) {
            point->m_position[0] = state.m_position[0];
        }
        if (state.m_set & 0x20) {
            point->m_position[1] = state.m_position[1];
        }
        if (state.m_set & 0x40) {
            point->m_position[2] = state.m_position[2];
        }
        if (state.m_set & 0x400) {
            point->field_0x24 = state.field_0x2C;
        }
        if (state.m_set & 0x800) {
            point->field_0x24 = state.field_0x30;
        }
        break;
    }
    }
}

/* Evaluates motion at frame into state. */
extern "C" void fn_80010BF8(UnkLightMotionState_80010BF8 *state, const UnkLightMotion_80010BF8 *motion, float frame) {
    UnkLightTrack_80010BF8 *track = motion->m_tracks;
    state->m_set = 0;
    for (int i = 0; i < motion->m_trackCount; i++, track++) {
        switch (track->m_kind & ~0xFF) {
        case 0x100:
            if (fn_80011A34(&state->m_position[0], track, frame) != 0) {
                state->m_set |= 0x10;
            }
            break;
        case 0x200:
            if (fn_80011A34(&state->m_position[1], track, frame) != 0) {
                state->m_set |= 0x20;
            }
            break;
        case 0x400:
            if (fn_80011A34(&state->m_position[2], track, frame) != 0) {
                state->m_set |= 0x40;
            }
            break;
        case 0x200000:
            if (fn_80011A34(&state->m_color[0], track, frame) != 0) {
                state->m_set |= 1;
            }
            break;
        case 0x400000:
            if (fn_80011A34(&state->m_color[1], track, frame) != 0) {
                state->m_set |= 2;
            }
            break;
        case 0x800000:
            if (fn_80011A34(&state->m_color[2], track, frame) != 0) {
                state->m_set |= 4;
            }
            break;
        case 0x2000000:
            if (fn_80011A34(&state->m_color[3], track, frame) != 0) {
                state->m_set |= 8;
            }
            break;
        case 0x800:
            if (fn_8001188C(&state->m_angles.x, track, frame) != 0) {
                state->m_set |= 0x80;
            }
            break;
        case 0x1000:
            if (fn_8001188C(&state->m_angles.y, track, frame) != 0) {
                state->m_set |= 0x100;
            }
            break;
        case 0x2000:
            if (fn_8001188C(&state->m_angles.z, track, frame) != 0) {
                state->m_set |= 0x200;
            }
            break;
        case 0x4000000:
            if (fn_80011A34(&state->field_0x2C, track, frame) != 0) {
                state->m_set |= 0x400;
            }
            break;
        case 0x8000000:
            if (fn_80011A34(&state->field_0x30, track, frame) != 0) {
                state->m_set |= 0x800;
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

/* Swaps a light chunk: the words common to all lights, then those of its kind. */
extern "C" void fn_800106B4(UnkLightChunk_800106B4 *chunk) {
    SwapBytes(&chunk->m_kind);
    SwapBytes(&chunk->m_light[0]);
    SwapBytes(&chunk->m_light[1]);
    SwapBytes(&chunk->m_light[2]);
    SwapBytes(&chunk->m_light[3]);
    SwapBytes(&chunk->m_light[4]);
    SwapBytes(&chunk->m_light[5]);
    switch (chunk->m_kind) {
    case 1: {
        u32 *light = chunk->m_light;
        SwapBytes(&light[6]);
        SwapBytes(&light[7]);
        SwapBytes(&light[8]);
        break;
    }
    case 2: {
        u32 *light = chunk->m_light;
        SwapBytes(&light[6]);
        SwapBytes(&light[7]);
        SwapBytes(&light[8]);
        SwapBytes(&light[9]);
        SwapBytes(&light[10]);
        break;
    }
    case 4: {
        u32 *light = chunk->m_light;
        SwapBytes(&light[6]);
        SwapBytes(&light[7]);
        SwapBytes(&light[8]);
        SwapBytes(&light[9]);
        SwapBytes(&light[10]);
        SwapBytes(&light[11]);
        SwapBytes(&light[12]);
        SwapBytes(&light[13]);
        SwapBytes(&light[14]);
        SwapBytes(&light[15]);
        break;
    }
    case 8: {
        u32 *light = chunk->m_light;
        SwapBytes(&light[6]);
        SwapBytes(&light[7]);
        SwapBytes(&light[8]);
        SwapBytes(&light[9]);
        SwapBytes(&light[10]);
        SwapBytes(&light[11]);
        SwapBytes(&light[12]);
        SwapBytes(&light[13]);
        SwapBytes(&light[14]);
        SwapBytes(&light[15]);
        SwapBytes(&light[16]);
        break;
    }
    case 0x10: {
        u32 *light = chunk->m_light;
        SwapBytes(&light[5]);
        SwapBytes(&light[6]);
        SwapBytes(&light[7]);
        break;
    }
    }
}

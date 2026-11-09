#ifndef GAME_MOTIONHALFKEYTRACK_H
#define GAME_MOTIONHALFKEYTRACK_H

#include <dolphin/types.h>
#include "size_assert.h"

/* Half-float key tracks (Game/MotionHalfKeyTrack.cpp; no RTTI, names inferred). */

/* A decoded key. */
struct HalfKey {
    /* 0x0 */ float m_time;
    /* 0x4 */ float m_in;
    /* 0x8 */ float m_out;
    /* 0xC */ float m_value;
};

/* The packed keys of a track. */
struct HalfKeyTrackData {
    /* 0x0 */ s32 m_size;
    /* 0x4 */ u16 m_count;
    /* 0x6 */ s8 m_padding;
    /* 0x7 */ u8 field_0x7;
    /* 0x8 */ u16 m_keys[1];
};

/* A track: its cursor in the packed keys and the two keys around the current time. Bits of the
 * flags: 1 the value is constant, 2 the slope is constant, 4 both are, 8 the value is constant
 * between the two keys. */
struct HalfKeyTrack {
    /* 0x00 */ s16 m_index;
    /* 0x02 */ s16 m_count;
    /* 0x04 */ u32 m_flags;
    /* 0x08 */ u16 *m_cursor;
    /* 0x0C */ HalfKeyTrackData *m_data;
    /* 0x10 */ float m_value;
    /* 0x14 */ float m_slope;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ HalfKey m_prev;
    /* 0x30 */ HalfKey m_next;
};
SIZE_ASSERT(HalfKeyTrack, 0x40);

extern "C" {
/* Restarts a track (at its last key when the speed is negative). */
void fn_80413B30(HalfKeyTrack *track);
/* Clears a track. */
void fn_80413E10(HalfKeyTrack *track);
/* Starts a track on packed keys. */
void fn_80413E38(HalfKeyTrack *track, HalfKeyTrackData *data);
/* Value and slope, slope, value of a track at the time, moving backwards, then forwards. */
void fn_80414438(float *value, float *slope, HalfKeyTrack *track);
float fn_8041474C(HalfKeyTrack *track);
float fn_804149E0(HalfKeyTrack *track);
void fn_80414CBC(float *value, float *slope, HalfKeyTrack *track);
float fn_80414FD0(HalfKeyTrack *track);
float fn_8041525C(HalfKeyTrack *track);
/* Sets the start, end, speed and time the tracks are evaluated with. */
void fn_8041575C(const float *params);
/* Resets the shared state to the Hermite functions. */
void fn_80415784(void);
}

#endif

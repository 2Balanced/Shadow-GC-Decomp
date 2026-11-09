/* Half-float key tracks (no class with RTTI; file name and all names inferred). A track walks a
 * stream of keys packed as 16-bit words: a header word (the key's frame in bits 0-13, bit 14 set
 * when the key before it has no out tangent, bit 15 set when this key has none, so that its out
 * tangent repeats its in tangent), then the in tangent, the out tangent (if present) and the
 * value, each as a half float. A track keeps the two keys around the current time and moves its
 * cursor forwards or backwards, one key at a time, as the time moves; between two keys it uses
 * the Hermite functions of the shared state, and before the first or after the last key it holds
 * or extends the end key linearly. The shared state holds the time and the playback speed, whose
 * sign decides in which direction a track starts.
 * The unit was built with C++ exceptions on (its functions have extab entries). Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first).
 * Open: fn_80414120 and fn_804142B0 only get retail's code as inline functions (their early
 * return becomes a branch over a dead jump), but GC/2.7 then emits them as weak copies right after
 * fn_80413B30 instead of after fn_80413E38; fn_8041575C keeps its four copies in load-store order
 * in retail while GC/2.7 always hoists the loads; the Hermite functions fn_804156D8 and
 * fn_80415530 differ only in float register numbers (about 40 source orders tried). */
#include <dolphin/types.h>
#include <string.h>
#include "Game/MotionHalfKeyTrack.h"
#include "size_assert.h"

typedef float (*HalfKeyEvalFn)(const HalfKey *prev, const HalfKey *next, float time);
typedef void (*HalfKeyEvalBothFn)(float *value, float *slope, const HalfKey *prev,
                                  const HalfKey *next, float time);

/* The state shared by all tracks. */
struct HalfKeyState {
    /* 0x00 */ float field_0x0;
    /* 0x04 */ float field_0x4;
    /* 0x08 */ float m_speed;
    /* 0x0C */ float m_time;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ HalfKeyEvalFn m_value;
    /* 0x18 */ HalfKeyEvalFn m_slope;
    /* 0x1C */ HalfKeyEvalBothFn m_valueAndSlope;
};
SIZE_ASSERT(HalfKeyState, 0x20);

HalfKeyState lbl_805E12C0;

/* A half float (1 sign, 5 exponent, 10 mantissa bits) as a float; 0 stays 0. */
inline float HalfToFloat(u16 half) {
    float result = 0.0f;
    if (half != 0) {
        *(u32 *)&result = ((half & 0x8000) << 16) | (((half & 0x7FFF) << 13) + 0x38000000);
    } else {
        return result;
    }
    return result;
}

/* Decodes the key at words into key; next is left after it. */
#define READ_KEY(key, words, next, header)                                                         \
    header = (words)[0];                                                                           \
    next = (words) + 2;                                                                            \
    (key).m_time = (header & 0x3FFF);                                                              \
    (key).m_in = HalfToFloat((words)[1]);                                                          \
    (key).m_out = (header & 0x8000) ? (key).m_in : HalfToFloat(*next++);                           \
    (key).m_value = HalfToFloat(*next++);

/* The number of words of the key before the one with this header word. */
#define PREV_KEY_SIZE(header) ((header & 0x4000) ? 3 : 4)

extern "C" {
void fn_80413B30(HalfKeyTrack *track);
void fn_80414120(HalfKeyTrack *track);
void fn_804142B0(HalfKeyTrack *track);
void fn_80415530(float *value, float *slope, const HalfKey *prev, const HalfKey *next, float time);
float fn_8041564C(const HalfKey *prev, const HalfKey *next, float time);
float fn_804156D8(const HalfKey *prev, const HalfKey *next, float time);
}

/* Resets the shared state to the Hermite functions. */
extern "C" void fn_80415784(void) {
    memset(&lbl_805E12C0, 0, sizeof(HalfKeyState));
    lbl_805E12C0.m_value = fn_804156D8;
    lbl_805E12C0.m_slope = fn_8041564C;
    lbl_805E12C0.m_valueAndSlope = fn_80415530;
    lbl_805E12C0.field_0x10 = 0;
}

/* Sets the first four words of the shared state. */
extern "C" void fn_8041575C(const float *params) {
    lbl_805E12C0.field_0x0 = params[0];
    lbl_805E12C0.field_0x4 = params[1];
    lbl_805E12C0.m_speed = params[2];
    lbl_805E12C0.m_time = params[3];
}

/* The Hermite value between two keys. */
extern "C" float fn_804156D8(const HalfKey *prev, const HalfKey *next, float time) {
    float h00, h01, h10, h11;
    float s = time - prev->m_time;
    float len = next->m_time - prev->m_time;
    float inv = 1.0f / len;
    float s2 = s * s;
    float inv2 = inv * inv;
    float s3 = s2 * s;
    float a = inv2 * s3;
    float b = s2 * inv;
    float d = 2.0f * a * inv;
    float c = 3.0f * s2 * inv2;
    h11 = a - b;
    h10 = s + (h11 - b);
    h00 = 1.0f + (d - c);
    h01 = -d + c;
    return prev->m_value * h00 + next->m_value * h01 + prev->m_out * h10 + next->m_in * h11;
}

/* The Hermite slope between two keys. */
extern "C" float fn_8041564C(const HalfKey *prev, const HalfKey *next, float time) {
    float h00, h01, h10, h11;
    float s = time - prev->m_time;
    float s2 = s * s;
    float inv = 1.0f / (next->m_time - prev->m_time);
    float inv2 = inv * inv;
    float c = 6.0f * s2 * inv2 * inv;
    float d = 6.0f * s * inv2;
    float b = 3.0f * s2 * inv2;
    float f = 2.0f * s * inv;
    h00 = c - d;
    h01 = -c + d;
    h11 = b - f;
    h10 = 1.0f + (h11 - f);
    return prev->m_value * h00 + next->m_value * h01 + prev->m_out * h10 + next->m_in * h11;
}

/* The Hermite value and slope between two keys. */
extern "C" void fn_80415530(float *value, float *slope, const HalfKey *prev, const HalfKey *next,
                            float time) {
    float h00, h01, h10, h11;
    float dh00, dh01, dh10, dh11;
    float s = time - prev->m_time;
    float s2 = s * s;
    float inv = 1.0f / (next->m_time - prev->m_time);
    float inv2 = inv * inv;
    float s3 = s2 * s;
    float a = inv2 * s3;
    float b = s2 * inv;
    float d = 2.0f * a * inv;
    float c = 3.0f * s2 * inv2;
    float c6 = 6.0f * s2 * inv2 * inv;
    float d6 = 6.0f * s * inv2;
    float f = 2.0f * s * inv;
    h11 = a - b;
    h10 = s + (h11 - b);
    h00 = 1.0f + (d - c);
    h01 = -d + c;
    dh00 = c6 - d6;
    dh01 = -c6 + d6;
    dh11 = c - f;
    dh10 = 1.0f + (dh11 - f);
    *value = prev->m_value * h00 + next->m_value * h01 + prev->m_out * h10 + next->m_in * h11;
    *slope = prev->m_value * dh00 + next->m_value * dh01 + prev->m_out * dh10 + next->m_in * dh11;
}

/* The value of a track at the time, moving forwards. */
extern "C" float fn_8041525C(HalfKeyTrack *track) {
    HalfKey *prev = &track->m_prev;
    HalfKey *nextKey = &track->m_next;
    u32 flags = track->m_flags;
    u16 *words = track->m_cursor;
    s16 index = track->m_index;
    s16 count = track->m_count;
    u16 *next;
    u16 header;
    float out;

    if (flags & 1) {
        return track->m_value;
    }
    if (lbl_805E12C0.m_time < 0.0001f + prev->m_time) {
        return prev->m_value;
    }
    if (lbl_805E12C0.m_time < nextKey->m_time - 0.0001f) {
        if (flags & 8) {
            return track->m_value;
        }
        return lbl_805E12C0.m_value(prev, nextKey, lbl_805E12C0.m_time);
    }
    if (lbl_805E12C0.m_time < 0.0001f + nextKey->m_time) {
        return nextKey->m_value;
    }
    while (index < count) {
        *prev = *nextKey;
        READ_KEY(*nextKey, words, next, header);
        words = next;
        index++;
        if (lbl_805E12C0.m_time < 0.0001f + nextKey->m_time) {
            track->m_index = index;
            track->m_cursor = next;
            track->m_flags &= ~1;
            if (lbl_805E12C0.m_time < nextKey->m_time - 0.0001f) {
                if (0.0f == prev->m_out && 0.0f == nextKey->m_in &&
                    prev->m_value == nextKey->m_value) {
                    track->m_flags |= 8;
                    return track->m_value = nextKey->m_value;
                }
                track->m_flags &= ~8;
                return lbl_805E12C0.m_value(prev, nextKey, lbl_805E12C0.m_time);
            }
            return nextKey->m_value;
        }
    }
    track->m_index = index;
    track->m_cursor = words;
    out = nextKey->m_out;
    if (0.0f == out) {
        track->m_flags |= 1;
        return track->m_value = nextKey->m_value;
    }
    return nextKey->m_value + out * (lbl_805E12C0.m_time - nextKey->m_time);
}

/* The slope of a track at the time, moving forwards. */
extern "C" float fn_80414FD0(HalfKeyTrack *track) {
    HalfKey *prev = &track->m_prev;
    HalfKey *nextKey = &track->m_next;
    u16 *words = track->m_cursor;
    s16 index = track->m_index;
    s16 count = track->m_count;
    u16 *next;
    u16 header;

    if (track->m_flags & 2) {
        return track->m_slope;
    }
    if (lbl_805E12C0.m_time < 0.0001f + prev->m_time) {
        if (lbl_805E12C0.m_time < prev->m_time - 0.0001f) {
            return prev->m_in;
        }
        return 0.5f * (prev->m_in + prev->m_out);
    }
    if (lbl_805E12C0.m_time < nextKey->m_time - 0.0001f) {
        return lbl_805E12C0.m_slope(prev, nextKey, lbl_805E12C0.m_time);
    }
    if (lbl_805E12C0.m_time < 0.0001f + nextKey->m_time) {
        return 0.5f * (nextKey->m_in + nextKey->m_out);
    }
    while (index < count) {
        *prev = *nextKey;
        READ_KEY(*nextKey, words, next, header);
        words = next;
        index++;
        if (lbl_805E12C0.m_time < 0.0001f + nextKey->m_time) {
            track->m_index = index;
            track->m_cursor = next;
            track->m_flags &= ~2;
            if (lbl_805E12C0.m_time < nextKey->m_time - 0.0001f) {
                return lbl_805E12C0.m_slope(prev, nextKey, lbl_805E12C0.m_time);
            }
            return 0.5f * (nextKey->m_in + nextKey->m_out);
        }
    }
    track->m_index = index;
    track->m_cursor = words;
    track->m_flags |= 2;
    return track->m_slope = nextKey->m_out;
}

/* The value and the slope of a track at the time, moving forwards. */
extern "C" void fn_80414CBC(float *value, float *slope, HalfKeyTrack *track) {
    HalfKey *prev = &track->m_prev;
    HalfKey *nextKey = &track->m_next;
    u16 *words = track->m_cursor;
    s16 index = track->m_index;
    s16 count = track->m_count;
    u16 *next;
    u16 header;
    float out;

    if (track->m_flags & 4) {
        *value = track->m_value;
        *slope = track->m_slope;
        return;
    }
    if (lbl_805E12C0.m_time < 0.0001f + prev->m_time) {
        *value = prev->m_value;
        if (lbl_805E12C0.m_time < prev->m_time - 0.0001f) {
            *slope = prev->m_in;
        } else {
            *slope = 0.5f * (prev->m_in + prev->m_out);
        }
        return;
    }
    if (lbl_805E12C0.m_time < nextKey->m_time - 0.0001f) {
        lbl_805E12C0.m_valueAndSlope(value, slope, prev, nextKey, lbl_805E12C0.m_time);
        return;
    }
    if (lbl_805E12C0.m_time < 0.0001f + nextKey->m_time) {
        *value = nextKey->m_value;
        *slope = 0.5f * (nextKey->m_in + nextKey->m_out);
        return;
    }
    while (index < count) {
        *prev = *nextKey;
        READ_KEY(*nextKey, words, next, header);
        words = next;
        index++;
        if (lbl_805E12C0.m_time < 0.0001f + nextKey->m_time) {
            track->m_index = index;
            track->m_cursor = next;
            track->m_flags &= ~4;
            if (lbl_805E12C0.m_time < nextKey->m_time - 0.0001f) {
                lbl_805E12C0.m_valueAndSlope(value, slope, prev, nextKey, lbl_805E12C0.m_time);
                return;
            }
            *value = nextKey->m_value;
            *slope = 0.5f * (nextKey->m_in + nextKey->m_out);
            return;
        }
    }
    track->m_index = index;
    track->m_cursor = words;
    out = nextKey->m_out;
    if (0.0f == out) {
        track->m_flags |= 4;
        *value = track->m_value = nextKey->m_value;
        *slope = track->m_slope = nextKey->m_out;
        return;
    }
    *value = nextKey->m_value + out * (lbl_805E12C0.m_time - nextKey->m_time);
    *slope = nextKey->m_out;
}

/* The value of a track at the time, moving backwards. */
extern "C" float fn_804149E0(HalfKeyTrack *track) {
    HalfKey *prev = &track->m_prev;
    HalfKey *nextKey = &track->m_next;
    u32 flags = track->m_flags;
    u16 *words = track->m_cursor;
    s16 index = track->m_index;
    u16 *next;
    u16 header;
    float in;

    if (flags & 1) {
        return track->m_value;
    }
    if (lbl_805E12C0.m_time > nextKey->m_time - 0.0001f) {
        return nextKey->m_value;
    }
    if (lbl_805E12C0.m_time > 0.0001f + prev->m_time) {
        if (flags & 8) {
            return track->m_value;
        }
        return lbl_805E12C0.m_value(prev, nextKey, lbl_805E12C0.m_time);
    }
    if (lbl_805E12C0.m_time > prev->m_time - 0.0001f) {
        return prev->m_value;
    }
    while (index > -1) {
        *nextKey = *prev;
        READ_KEY(*prev, words, next, header);
        index--;
        words -= PREV_KEY_SIZE(header);
        if (lbl_805E12C0.m_time > prev->m_time - 0.0001f) {
            track->m_flags &= ~1;
            if (lbl_805E12C0.m_time > 0.0001f + prev->m_time) {
                if (0.0f == prev->m_out && 0.0f == nextKey->m_in &&
                    prev->m_value == nextKey->m_value) {
                    track->m_flags |= 8;
                    return track->m_value = prev->m_value;
                }
                track->m_flags &= ~8;
                return lbl_805E12C0.m_value(prev, nextKey, lbl_805E12C0.m_time);
            }
            return prev->m_value;
        }
    }
    track->m_index = index;
    track->m_cursor = words;
    in = prev->m_in;
    if (0.0f == in) {
        track->m_flags |= 1;
        return track->m_value = prev->m_value;
    }
    return prev->m_value + in * (lbl_805E12C0.m_time - prev->m_time);
}

/* The slope of a track at the time, moving backwards. */
extern "C" float fn_8041474C(HalfKeyTrack *track) {
    HalfKey *prev = &track->m_prev;
    HalfKey *nextKey = &track->m_next;
    u16 *words = track->m_cursor;
    s16 index = track->m_index;
    u16 *next;
    u16 header;

    if (track->m_flags & 2) {
        return track->m_slope;
    }
    if (lbl_805E12C0.m_time > nextKey->m_time - 0.0001f) {
        if (lbl_805E12C0.m_time > 0.0001f + nextKey->m_time) {
            return nextKey->m_out;
        }
        return 0.5f * (nextKey->m_in + nextKey->m_out);
    }
    if (lbl_805E12C0.m_time > 0.0001f + prev->m_time) {
        return lbl_805E12C0.m_slope(prev, nextKey, lbl_805E12C0.m_time);
    }
    if (lbl_805E12C0.m_time > prev->m_time - 0.0001f) {
        return 0.5f * (prev->m_in + prev->m_out);
    }
    while (index > -1) {
        *nextKey = *prev;
        READ_KEY(*prev, words, next, header);
        index--;
        words -= PREV_KEY_SIZE(header);
        if (lbl_805E12C0.m_time > prev->m_time - 0.0001f) {
            track->m_flags &= ~2;
            if (lbl_805E12C0.m_time > 0.0001f + prev->m_time) {
                return lbl_805E12C0.m_slope(prev, nextKey, lbl_805E12C0.m_time);
            }
            return 0.5f * (prev->m_in + prev->m_out);
        }
    }
    track->m_index = index;
    track->m_cursor = words;
    track->m_flags |= 2;
    return track->m_slope = prev->m_in;
}

/* The value and the slope of a track at the time, moving backwards. */
extern "C" void fn_80414438(float *value, float *slope, HalfKeyTrack *track) {
    HalfKey *prev = &track->m_prev;
    HalfKey *nextKey = &track->m_next;
    u16 *words = track->m_cursor;
    s16 index = track->m_index;
    u16 *next;
    u16 header;
    float in;

    if (track->m_flags & 4) {
        *value = track->m_value;
        *slope = track->m_slope;
        return;
    }
    if (lbl_805E12C0.m_time > nextKey->m_time - 0.0001f) {
        *value = nextKey->m_value;
        if (lbl_805E12C0.m_time > 0.0001f + nextKey->m_time) {
            *slope = nextKey->m_out;
        } else {
            *slope = 0.5f * (nextKey->m_in + nextKey->m_out);
        }
        return;
    }
    if (lbl_805E12C0.m_time > 0.0001f + prev->m_time) {
        lbl_805E12C0.m_valueAndSlope(value, slope, prev, nextKey, lbl_805E12C0.m_time);
        return;
    }
    if (lbl_805E12C0.m_time > prev->m_time - 0.0001f) {
        *value = prev->m_value;
        *slope = 0.5f * (prev->m_in + prev->m_out);
        return;
    }
    while (index > -1) {
        *nextKey = *prev;
        READ_KEY(*prev, words, next, header);
        index--;
        words -= PREV_KEY_SIZE(header);
        if (lbl_805E12C0.m_time > prev->m_time - 0.0001f) {
            track->m_flags &= ~4;
            if (lbl_805E12C0.m_time > 0.0001f + prev->m_time) {
                lbl_805E12C0.m_valueAndSlope(value, slope, prev, nextKey, lbl_805E12C0.m_time);
                return;
            }
            *value = prev->m_value;
            *slope = 0.5f * (prev->m_in + prev->m_out);
            return;
        }
    }
    track->m_index = index;
    track->m_cursor = words;
    in = prev->m_in;
    if (0.0f == in) {
        track->m_flags |= 4;
        *value = track->m_value = prev->m_value;
        *slope = track->m_slope = prev->m_in;
        return;
    }
    *value = prev->m_value + in * (lbl_805E12C0.m_time - prev->m_time);
    *slope = prev->m_in;
}

/* Moves a track's keys forwards until the later one is at or after the time. */
extern "C" inline void fn_804142B0(HalfKeyTrack *track) {
    u16 *words = track->m_cursor;
    s16 index = track->m_index;
    s16 count = track->m_count;
    u16 *next;
    u16 header;

    if (lbl_805E12C0.m_time < 0.0001f + track->m_next.m_time) {
        return;
    }
    while (index < count) {
        track->m_prev = track->m_next;
        READ_KEY(track->m_next, words, next, header);
        words = next;
        index++;
        if (lbl_805E12C0.m_time < 0.0001f + track->m_next.m_time) {
            break;
        }
    }
    track->m_index = index;
    track->m_cursor = words;
}

/* Moves a track's keys backwards until the earlier one is at or before the time. */
extern "C" inline void fn_80414120(HalfKeyTrack *track) {
    u16 *words = track->m_cursor;
    s16 index = track->m_index;
    u16 *next;
    u16 header;

    if (lbl_805E12C0.m_time > track->m_prev.m_time - 0.0001f) {
        return;
    }
    while (index > -1) {
        track->m_next = track->m_prev;
        READ_KEY(track->m_prev, words, next, header);
        index--;
        words -= PREV_KEY_SIZE(header);
        if (lbl_805E12C0.m_time > track->m_prev.m_time - 0.0001f) {
            break;
        }
    }
    track->m_index = index;
    track->m_cursor = words;
}

/* Starts a track on the packed keys data. */
extern "C" void fn_80413E38(HalfKeyTrack *track, HalfKeyTrackData *data) {
    u16 *words;
    u16 *next;
    u16 header;

    track->m_flags = 0;
    track->m_count = data->m_count;
    track->m_data = data;
    if (lbl_805E12C0.m_speed < 0.0f) {
        track->m_index = track->m_count - 1;
        track->m_cursor = (u16 *)((u8 *)data + data->m_size - data->m_padding) -
                          PREV_KEY_SIZE(data->m_keys[0]);
        words = track->m_cursor;
        READ_KEY(track->m_next, words, next, header);
        track->m_cursor -= PREV_KEY_SIZE(header);
        track->m_prev = track->m_next;
        track->m_index--;
        fn_80414120(track);
    } else {
        track->m_index = 0;
        track->m_cursor = data->m_keys;
        words = track->m_cursor;
        READ_KEY(track->m_prev, words, next, header);
        track->m_cursor = next;
        track->m_next = track->m_prev;
        track->m_index++;
        fn_804142B0(track);
    }
}

/* Clears a track. */
extern "C" void fn_80413E10(HalfKeyTrack *track) {
    memset(track, 0, sizeof(HalfKeyTrack));
}

/* Restarts a track at its first key, or at its last when the speed is negative. */
extern "C" void fn_80413B30(HalfKeyTrack *track) {
    HalfKeyTrackData *data = track->m_data;
    u16 *words;
    u16 *next;
    u16 header;

    track->m_flags = 0;
    if (lbl_805E12C0.m_speed < 0.0f) {
        track->m_index = track->m_count - 1;
        track->m_cursor = (u16 *)((u8 *)data + data->m_size - data->m_padding) -
                          PREV_KEY_SIZE(data->m_keys[0]);
        words = track->m_cursor;
        READ_KEY(track->m_next, words, next, header);
        track->m_cursor -= PREV_KEY_SIZE(header);
        track->m_prev = track->m_next;
        track->m_index--;
        fn_80414120(track);
    } else {
        track->m_index = 0;
        track->m_cursor = data->m_keys;
        words = track->m_cursor;
        READ_KEY(track->m_prev, words, next, header);
        track->m_cursor = next;
        track->m_next = track->m_prev;
        track->m_index++;
        fn_804142B0(track);
    }
}

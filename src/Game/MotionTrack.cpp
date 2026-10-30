/* Motion tracks (no class with RTTI; file name and all names inferred): the byte swap of a motion
 * chunk of a motion file and of its tracks' keys (fn_80011244 and the key swaps before it), a
 * track's value at a frame (fn_80011A34 for a float, fn_8001188C for an integer or an angle in
 * 1/65536 of a turn), with linear and Hermite interpolation between keys found by binary search,
 * the advance of a motion's time by its end mode (fn_80011B7C), and the camera or light motion of
 * a motion file (fn_80011CE8). A track's flags hold its key format in the low half (2 and 4
 * linear, 0x20 Hermite) and what happens outside its frames in bits 16-23 (1 nothing, 2 clamp, 4
 * repeat; 8 ping-pong for a motion's time). The unit is split by its .data (the swap's jump table,
 * padded to 8 bytes) and its .sdata2 (0x805F17F8..0x805F1828, which the next unit's "Task" follows).
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CameraModeUser.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A key of a float track: linear and Hermite. */
struct UnkFloatKey_80012190 {
    /* 0x0 */ float m_frame;
    /* 0x4 */ float m_value;
};

struct UnkFloatHermiteKey_80012218 {
    /* 0x0 */ float m_frame;
    /* 0x4 */ float m_value;
    /* 0x8 */ float m_in;
    /* 0xC */ float m_out;
};

/* A key of an integer track: linear and Hermite. */
struct UnkIntKey_800121D4 {
    /* 0x0 */ float m_frame;
    /* 0x4 */ s32 m_value;
};

struct UnkIntHermiteKey_8001225C {
    /* 0x0 */ float m_frame;
    /* 0x4 */ s32 m_value;
    /* 0x8 */ float m_in;
    /* 0xC */ float m_out;
};

/* A key of an angle track: linear and Hermite. */
struct UnkAngleKey_80011DD0 {
    /* 0x0 */ s16 m_frame;
    /* 0x2 */ s16 m_value;
};

struct UnkAngleHermiteKey_80011D68 {
    /* 0x0 */ s16 m_frame;
    /* 0x2 */ s16 m_value;
    /* 0x4 */ float m_in;
    /* 0x8 */ float m_out;
};
SIZE_ASSERT(UnkAngleHermiteKey_80011D68, 0xC);

/* A track: its kind (what it sets in the upper 24 bits, the type of its values in the low byte:
 * 1 float, 8 integer, 0x10 and 0x12 angle), its flags, its frames and its keys. */
struct UnkMotionTrack_80011A34 {
    /* 0x00 */ u32 m_kind;
    /* 0x04 */ u32 m_flags;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ float m_start;
    /* 0x18 */ float m_end;
    /* 0x1C */ s32 m_keyCount;
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ void *m_keys;
};
SIZE_ASSERT(UnkMotionTrack_80011A34, 0x28);

/* A motion: its kind, its flags (the end mode in bits 16-23) and frames, and its tracks. */
struct UnkMotion_80011244 {
    /* 0x00 */ u32 m_kind;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ s32 m_trackCount;
    /* 0x10 */ UnkMotionTrack_80011A34 *m_tracks;
};

/* A chunk of a motion file (Game/MotionFile.cpp). */
struct UnkMotionChunk_800105C8 {
    /* 0x00 */ char m_name[4];
    /* 0x04 */ u32 m_size;
    /* 0x08 */ u32 m_dataOffset;
    /* 0x0C */ u32 field_0xC;
};

extern "C" {
/* The integer absolute value. */
int fn_803A7B20(int x);
/* The chunk named like name from chunks on (Game/MotionFile.cpp). */
UnkMotionChunk_800105C8 *fn_800105C8(void *chunks, const char *name);
void fn_8001102C(UnkFloatHermiteKey_80012218 *key);
void fn_800110B0(UnkFloatKey_80012190 *key);
void fn_800110F4(UnkIntHermiteKey_8001225C *key);
void fn_80011178(UnkAngleHermiteKey_80011D68 *key);
void fn_800111DC(UnkIntKey_800121D4 *key);
void fn_80011220(UnkAngleKey_80011DD0 *key);
void fn_800115DC(s32 *value, const UnkAngleHermiteKey_80011D68 *keys, u32 count, float frame);
void fn_80011758(s32 *value, const UnkAngleKey_80011DD0 *keys, u32 count, float frame);
u32 fn_80011D68(const UnkAngleHermiteKey_80011D68 *keys, u32 count, float frame);
u32 fn_80011DD0(const UnkAngleKey_80011DD0 *keys, u32 count, float frame);
void fn_80011E38(s32 *value, const UnkIntHermiteKey_8001225C *keys, u32 count, float frame);
void fn_80011F44(float *value, const UnkFloatHermiteKey_80012218 *keys, u32 count, float frame);
void fn_80012014(s32 *value, const UnkIntKey_800121D4 *keys, u32 count, float frame);
void fn_800120F0(float *value, const UnkFloatKey_80012190 *keys, u32 count, float frame);
u32 fn_80012190(const UnkFloatKey_80012190 *keys, u32 count, float frame);
u32 fn_800121D4(const UnkIntKey_800121D4 *keys, u32 count, float frame);
u32 fn_80012218(const UnkFloatHermiteKey_80012218 *keys, u32 count, float frame);
u32 fn_8001225C(const UnkIntHermiteKey_8001225C *keys, u32 count, float frame);
}

/* Reverses the bytes of a word, and of a half-word, in place. */
inline void SwapBytes(void *word) {
    u8 *bytes = static_cast<u8 *>(word);
    u8 byte0 = bytes[0];
    u8 byte2 = bytes[2];
    bytes[0] = bytes[3];
    bytes[3] = byte0;
    bytes[2] = bytes[1];
    bytes[1] = byte2;
}

inline void SwapHalf(void *half) {
    u8 *bytes = static_cast<u8 *>(half);
    u8 byte0 = bytes[0];
    bytes[0] = bytes[1];
    bytes[1] = byte0;
}

/* The key at or before frame: a binary search of the keys' frames. */
extern "C" u32 fn_8001225C(const UnkIntHermiteKey_8001225C *keys, u32 count, float frame) {
    u32 low = 0;
    u32 high = count;
    while (high - low > 1) {
        u32 middle = (low + high) >> 1;
        if (frame >= keys[middle].m_frame) {
            low = middle;
        } else {
            high = middle;
        }
    }
    return low;
}

extern "C" u32 fn_80012218(const UnkFloatHermiteKey_80012218 *keys, u32 count, float frame) {
    u32 low = 0;
    u32 high = count;
    while (high - low > 1) {
        u32 middle = (low + high) >> 1;
        if (frame >= keys[middle].m_frame) {
            low = middle;
        } else {
            high = middle;
        }
    }
    return low;
}

extern "C" u32 fn_800121D4(const UnkIntKey_800121D4 *keys, u32 count, float frame) {
    u32 low = 0;
    u32 high = count;
    while (high - low > 1) {
        u32 middle = (low + high) >> 1;
        if (frame >= keys[middle].m_frame) {
            low = middle;
        } else {
            high = middle;
        }
    }
    return low;
}

extern "C" u32 fn_80012190(const UnkFloatKey_80012190 *keys, u32 count, float frame) {
    u32 low = 0;
    u32 high = count;
    while (high - low > 1) {
        u32 middle = (low + high) >> 1;
        if (frame >= keys[middle].m_frame) {
            low = middle;
        } else {
            high = middle;
        }
    }
    return low;
}

/* Linear interpolation of a float track. */
extern "C" void fn_800120F0(float *value, const UnkFloatKey_80012190 *keys, u32 count, float frame) {
    u32 index = fn_80012190(keys, count, frame);
    const UnkFloatKey_80012190 *key = &keys[index];
    if (index < count - 1) {
        const UnkFloatKey_80012190 *next = key + 1;
        float t = (next->m_frame - frame) / (next->m_frame - key->m_frame);
        *value = key->m_value * t + next->m_value * (1.0f - t);
    } else {
        *value = key->m_value;
    }
}

/* Linear interpolation of an integer track. */
extern "C" void fn_80012014(s32 *value, const UnkIntKey_800121D4 *keys, u32 count, float frame) {
    u32 index = fn_800121D4(keys, count, frame);
    const UnkIntKey_800121D4 *key = &keys[index];
    if (index < count - 1) {
        float t = (key[1].m_frame - frame) / (key[1].m_frame - key[0].m_frame);
        *value = (s32)((float)key[0].m_value * t + (float)key[1].m_value * (1.0f - t));
    } else {
        *value = key->m_value;
    }
}

/* Hermite interpolation of a float track. */
extern "C" void fn_80011F44(float *value, const UnkFloatHermiteKey_80012218 *keys, u32 count, float frame) {
    u32 index = fn_80012218(keys, count, frame);
    const UnkFloatHermiteKey_80012218 *key = &keys[index];
    if (index < count - 1) {
        float length = key[1].m_frame - key[0].m_frame;
        float t = (frame - key[0].m_frame) / length;
        float delta = key[1].m_value - key[0].m_value;
        float c = length * key[0].m_out;
        float b = 3.0f * delta - length * (2.0f * key[0].m_out + key[1].m_in);
        float a = length * (key[0].m_out + key[1].m_in) + -2.0f * delta;
        *value = t * (t * (a * t + b) + c) + key[0].m_value;
    } else {
        *value = key->m_value;
    }
}

/* Hermite interpolation of an integer track. */
extern "C" void fn_80011E38(s32 *value, const UnkIntHermiteKey_8001225C *keys, u32 count, float frame) {
    u32 index = fn_8001225C(keys, count, frame);
    const UnkIntHermiteKey_8001225C *key = &keys[index];
    if (index < count - 1) {
        const UnkIntHermiteKey_8001225C *next = key + 1;
        float length = next->m_frame - key->m_frame;
        float delta = (float)(next->m_value - key->m_value);
        float t = (frame - key->m_frame) / length;
        float c = length * key->m_out;
        float b = 3.0f * delta - length * (2.0f * key->m_out + next->m_in);
        float a = length * (key->m_out + next->m_in) + -2.0f * delta;
        *value = (s32)(t * (t * (a * t + b) + c) + (float)key->m_value);
    } else {
        *value = key->m_value;
    }
}

extern "C" u32 fn_80011DD0(const UnkAngleKey_80011DD0 *keys, u32 count, float frame) {
    u32 low = 0;
    u32 high = count;
    while (high - low > 1) {
        u32 middle = (low + high) >> 1;
        if (frame >= (float)keys[middle].m_frame) {
            low = middle;
        } else {
            high = middle;
        }
    }
    return low;
}

extern "C" u32 fn_80011D68(const UnkAngleHermiteKey_80011D68 *keys, u32 count, float frame) {
    u32 low = 0;
    u32 high = count;
    while (high - low > 1) {
        u32 middle = (low + high) >> 1;
        if (frame >= (float)keys[middle].m_frame) {
            low = middle;
        } else {
            high = middle;
        }
    }
    return low;
}

/* The camera (2) or light (4) motion of a swapped motion file, or 0. */
extern "C" UnkCameraData_80012510 *fn_80011CE8(char *file, int kind) {
    UnkMotionChunk_800105C8 *chunk = 0;
    void *chunks = file + reinterpret_cast<u32 *>(file)[3];
    switch (kind) {
    case 2:
        chunk = fn_800105C8(chunks, "NSMC");
        break;
    case 4:
        chunk = fn_800105C8(chunks, "NSML");
        break;
    }
    if (chunk != 0) {
        return reinterpret_cast<UnkCameraData_80012510 *>(reinterpret_cast<u8 *>(chunk) + chunk->m_dataOffset);
    }
    return 0;
}

/* Advances a motion's time by step; at its ends, nothing happens, the time stops, wraps or turns
 * back (step changes sign). Whether the time reached an end. */
extern "C" int fn_80011B7C(UnkCameraData_80012510 *motion, float *time, float *step) {
    int ended = 0;
    float t = *time;
    t += *step;
    if (t >= motion->m_endTime && *step > 0.0f) {
        switch (motion->field_0x0 & 0xFF0000) {
        case 0x10000:
            break;
        case 0x20000:
            t = motion->m_endTime;
            break;
        case 0x40000: {
            float length = motion->m_endTime - motion->m_startTime;
            do {
                t -= length;
            } while (t > motion->m_endTime);
            break;
        }
        case 0x80000:
            if (*step > 0.0f) {
                t = 2.0f * motion->m_endTime - t;
                *step = -*step;
            } else {
                t = motion->m_endTime;
            }
            break;
        }
        ended = 1;
    } else if (t <= motion->m_startTime && *step < 0.0f) {
        switch (motion->field_0x0 & 0xFF0000) {
        case 0x10000:
            break;
        case 0x20000:
            t = motion->m_startTime;
            break;
        case 0x40000: {
            float length = motion->m_endTime - motion->m_startTime;
            do {
                t += length;
            } while (t < motion->m_startTime);
            break;
        }
        case 0x80000:
            if (*step < 0.0f) {
                t = 2.0f * motion->m_startTime - t;
                *step = -*step;
            } else {
                t = motion->m_startTime;
            }
            break;
        }
        ended = 1;
    }
    *time = t;
    return ended;
}

/* The value of a float track at frame, placed into its frames by its mode; whether it has one. */
extern "C" int fn_80011A34(float *value, const UnkMotionTrack_80011A34 *track, float frame) {
    if (frame < track->m_start) {
        switch (track->m_flags & 0xFF0000) {
        case 0x10000:
            break;
        case 0x20000:
            frame = track->m_start;
            break;
        case 0x40000: {
            do {
                frame += track->m_end - track->m_start;
            } while (frame < track->m_start);
            break;
        }
        }
    } else if (frame > track->m_end) {
        switch (track->m_flags & 0xFF0000) {
        case 0x10000:
            break;
        case 0x20000:
            frame = track->m_end;
            break;
        case 0x40000: {
            do {
                frame -= track->m_end - track->m_start;
            } while (frame > track->m_end);
            break;
        }
        }
    }
    if (frame < track->m_start || frame > track->m_end) {
        return 0;
    }
    switch (track->m_flags & 0xFFFF) {
    case 2:
    case 4:
        fn_800120F0(value, static_cast<const UnkFloatKey_80012190 *>(track->m_keys), track->m_keyCount, frame);
        break;
    case 0x20:
        fn_80011F44(value, static_cast<const UnkFloatHermiteKey_80012218 *>(track->m_keys), track->m_keyCount, frame);
        break;
    }
    return 1;
}

/* The value of an integer or angle track at frame, as fn_80011A34. */
extern "C" int fn_8001188C(s32 *value, const UnkMotionTrack_80011A34 *track, float frame) {
    if (frame < track->m_start) {
        switch (track->m_flags & 0xFF0000) {
        case 0x10000:
            break;
        case 0x20000:
            frame = track->m_start;
            break;
        case 0x40000: {
            do {
                frame += track->m_end - track->m_start;
            } while (frame < track->m_start);
            break;
        }
        }
    } else if (frame > track->m_end) {
        switch (track->m_flags & 0xFF0000) {
        case 0x10000:
            break;
        case 0x20000:
            frame = track->m_end;
            break;
        case 0x40000: {
            do {
                frame -= track->m_end - track->m_start;
            } while (frame > track->m_end);
            break;
        }
        }
    }
    if (frame < track->m_start || frame > track->m_end) {
        return 0;
    }
    if (track->m_kind & 0x10) {
        switch (track->m_flags & 0xFFFF) {
        case 2:
        case 4:
            fn_80011758(value, static_cast<const UnkAngleKey_80011DD0 *>(track->m_keys), track->m_keyCount, frame);
            break;
        case 0x20:
            fn_800115DC(value, static_cast<const UnkAngleHermiteKey_80011D68 *>(track->m_keys), track->m_keyCount, frame);
            break;
        }
    } else {
        switch (track->m_flags & 0xFFFF) {
        case 2:
            fn_80012014(value, static_cast<const UnkIntKey_800121D4 *>(track->m_keys), track->m_keyCount, frame);
            break;
        case 4:
            return 0;
        case 0x20:
            fn_80011E38(value, static_cast<const UnkIntHermiteKey_8001225C *>(track->m_keys), track->m_keyCount, frame);
            break;
        }
    }
    return 1;
}

/* Linear interpolation of an angle track, the short way round. */
extern "C" void fn_80011758(s32 *value, const UnkAngleKey_80011DD0 *keys, u32 count, float frame) {
    u32 index = fn_80011DD0(keys, count, frame);
    const UnkAngleKey_80011DD0 *key = &keys[index];
    if (index < count - 1) {
        float t = ((float)key[1].m_frame - frame) / (float)(key[1].m_frame - key[0].m_frame);
        s32 from = key[0].m_value;
        s32 to = key[1].m_value;
        if (fn_803A7B20(to - from) > 0x8000) {
            to += to >= 0 ? -0x10000 : 0x10000;
        }
        *value = (s32)((float)from * t + (float)to * (1.0f - t));
    } else {
        *value = key->m_value;
    }
}

/* Hermite interpolation of an angle track, the short way round. */
extern "C" void fn_800115DC(s32 *value, const UnkAngleHermiteKey_80011D68 *keys, u32 count, float frame) {
    u32 index = fn_80011D68(keys, count, frame);
    const UnkAngleHermiteKey_80011D68 *key = &keys[index];
    if (index < count - 1) {
        const UnkAngleHermiteKey_80011D68 *next = key + 1;
        float length = (float)(next->m_frame - key->m_frame);
        s32 from = key->m_value;
        s32 to = next->m_value;
        if (fn_803A7B20(to - from) > 0x8000) {
            to += to >= 0 ? -0x10000 : 0x10000;
        }
        float delta = (float)(to - from);
        float t = (frame - (float)key->m_frame) / length;
        float c = length * key->m_out;
        float b = 3.0f * delta - length * (2.0f * key->m_out + next->m_in);
        float a = length * (key->m_out + next->m_in) + -2.0f * delta;
        *value = (s32)(t * (t * (a * t + b) + c) + (float)from);
    } else {
        *value = key->m_value;
    }
}

/* Swaps a motion chunk: the motion, its tracks and their keys. */
extern "C" void fn_80011244(UnkMotion_80011244 *motion) {
    SwapBytes(&motion->m_kind);
    SwapBytes(&motion->field_0x4);
    SwapBytes(&motion->field_0x8);
    SwapBytes(&motion->m_trackCount);
    UnkMotionTrack_80011A34 *track = motion->m_tracks;
    for (int i = 0; i < motion->m_trackCount; i++, track++) {
        SwapBytes(&track->m_kind);
        SwapBytes(&track->m_flags);
        SwapBytes(&track->field_0x8);
        SwapBytes(&track->field_0xC);
        SwapBytes(&track->field_0x10);
        SwapBytes(&track->m_start);
        SwapBytes(&track->m_end);
        SwapBytes(&track->m_keyCount);
        SwapBytes(&track->field_0x20);
        switch (track->m_kind & 0xFF) {
        case 1:
            switch (track->m_flags & 0xFFFF) {
            case 2:
            case 4: {
                UnkFloatKey_80012190 *key = static_cast<UnkFloatKey_80012190 *>(track->m_keys);
                for (int j = 0; j < track->m_keyCount; key++, j++) {
                    fn_800110B0(key);
                }
                break;
            }
            case 0x20: {
                UnkFloatHermiteKey_80012218 *key = static_cast<UnkFloatHermiteKey_80012218 *>(track->m_keys);
                for (int j = 0; j < track->m_keyCount; key++, j++) {
                    fn_8001102C(key);
                }
                break;
            }
            }
            break;
        case 8:
            switch (track->m_flags & 0xFFFF) {
            case 2:
            case 4: {
                UnkIntKey_800121D4 *key = static_cast<UnkIntKey_800121D4 *>(track->m_keys);
                for (int j = 0; j < track->m_keyCount; key++, j++) {
                    fn_800111DC(key);
                }
                break;
            }
            case 0x20: {
                UnkIntHermiteKey_8001225C *key = static_cast<UnkIntHermiteKey_8001225C *>(track->m_keys);
                for (int j = 0; j < track->m_keyCount; key++, j++) {
                    fn_800110F4(key);
                }
                break;
            }
            }
            break;
        case 0x10:
        case 0x12:
            switch (track->m_flags & 0xFFFF) {
            case 2:
            case 4: {
                UnkAngleKey_80011DD0 *key = static_cast<UnkAngleKey_80011DD0 *>(track->m_keys);
                for (int j = 0; j < track->m_keyCount; key++, j++) {
                    fn_80011220(key);
                }
                break;
            }
            case 0x20: {
                UnkAngleHermiteKey_80011D68 *key = static_cast<UnkAngleHermiteKey_80011D68 *>(track->m_keys);
                for (int j = 0; j < track->m_keyCount; key++, j++) {
                    fn_80011178(key);
                }
                break;
            }
            }
            break;
        }
    }
}

/* The key swaps. */
extern "C" void fn_80011220(UnkAngleKey_80011DD0 *key) {
    SwapHalf(&key->m_frame);
    SwapHalf(&key->m_value);
}

extern "C" void fn_800111DC(UnkIntKey_800121D4 *key) {
    SwapBytes(&key->m_frame);
    SwapBytes(&key->m_value);
}

extern "C" void fn_80011178(UnkAngleHermiteKey_80011D68 *key) {
    SwapHalf(&key->m_frame);
    SwapHalf(&key->m_value);
    SwapBytes(&key->m_in);
    SwapBytes(&key->m_out);
}

extern "C" void fn_800110F4(UnkIntHermiteKey_8001225C *key) {
    SwapBytes(&key->m_frame);
    SwapBytes(&key->m_value);
    SwapBytes(&key->m_in);
    SwapBytes(&key->m_out);
}

extern "C" void fn_800110B0(UnkFloatKey_80012190 *key) {
    SwapBytes(&key->m_frame);
    SwapBytes(&key->m_value);
}

extern "C" void fn_8001102C(UnkFloatHermiteKey_80012218 *key) {
    SwapBytes(&key->m_frame);
    SwapBytes(&key->m_value);
    SwapBytes(&key->m_in);
    SwapBytes(&key->m_out);
}

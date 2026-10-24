/* The sound manager's channels (UnkSoundChannel_801D98FC, Game/SoundManager.h). A channel plays one
 * sound through the back end (IAudio), on the group and voice its index selects. Each frame
 * fn_801D898C starts a new sound, follows its volume (from the distance to the player), pan and
 * surround value (from its direction to the camera) and pitch (from the change of that distance),
 * and stops the sound when its time runs out. Class and function names are invented ("(name
 * inferred)"). Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: fn_801D93FC (99.7%) differs only in two registers: retail keeps the sound mode in
 * r7 and the boost in r6, where GC/2.7 swaps them. */
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/SoundManager.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace Camera {
/* CameraManagerUnit (class name from the RTTI of its shared_ptr control block; see
 * Game/CameraManagerUnit.h). */
class CameraManagerUnit;
} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

class UnkCameraManager_80009548;

/* The game state (struct name unknown): the sound output mode at 0x78 (0 stereo, 1 mono, 2
 * surround; see Game/SystemSettings.cpp) and the two-player flag at 0xB2. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x78];
    /* 0x078 */ s32 m_soundMode;
    /* 0x07C */ u8 field_0x7C[0xB2 - 0x7C];
    /* 0x0B2 */ u8 m_twoPlayer;
    /* 0x0B3 */ u8 field_0xB3[0x150 - 0xB3];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The first player's number (see Game/BkLarva.cpp). */
extern s32 lbl_805EE588;

/* MSL's float overloads, which the game calls out of line (<cmath> is not included: its sqrtf
 * constants would be emitted into this unit's .sdata2). */
namespace std {
float sin(float x);
float cos(float x);
float fabs(float x);
} // namespace std

extern "C" {
/* The sound manager (see Game/AudioManagerTask.cpp). */
UnkSound_80053164 *fn_80053164(void);
/* The distance from position to the player, and how far entry can be heard (behaviours inferred;
 * see Game/AudioListener.cpp). */
float fn_801D9964(const Vec3f *position);
float fn_801D9BCC(const UnkSoundBankEntry *entry);
/* The character of a player number (see Game/BkLarva.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int id);
/* The angle of (x, z) (see Game/BlackBullCtrlUnit.cpp). */
float fn_80016260(float x, float z);
/* The camera manager, a camera's position and a camera (see Game/HintRing.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, int index);
boost::shared_ptr<SonicteamUSA::System::Camera::CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
Vec3f fn_8000A610(const u8 *camera);
}

/* The angles of camera index (name inferred; the shared out-of-line copy Game/HintRing.cpp
 * defines). */
inline Vec3f GetCameraAngles(UnkCameraManager_80009548 *manager, int index) {
    return fn_8000A610(reinterpret_cast<const u8 *>(fn_8000D0F0(manager, index).get()) + 0x8);
}

/* fn_8040CD74 returns a - b by value (as in Game/ElecBar.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* a - b (name inferred; as in Game/BlackBull0412ModeCtrlUnit.cpp). */
inline Vec3f Sub(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* Or-ed into field_0x9 when the sound starts (name inferred). */
const u32 kChannelStarted = 0x80;

/* The group and voice of the channel's index: 16 voices to a group up to index 0x26, then one
 * group each (name inferred). */
inline void GetGroupVoice(const UnkSoundChannel_801D98FC *channel, s8 &group, u8 &voice) {
    if (channel->m_index > 0x26) {
        group = channel->m_index - 0x23;
        voice = 0;
    } else {
        group = channel->m_index >> 4;
        voice = channel->m_index & 0xF;
    }
}

/* volume, raised by the bank entry's volume (name inferred). */
inline s8 ScaleVolume(const UnkSoundChannel_801D98FC *channel, s8 volume) {
    if (channel->m_entry->m_volume == 0) {
        return volume;
    }
    float scaled = 127.0f * (0.01f * channel->m_entry->m_volume) + volume;
    if (scaled < -127.0f) {
        scaled = -127.0f;
    } else if (scaled >= 127.0f) {
        scaled = 127.0f;
    }
    return scaled;
}

/* Clears the channel (name inferred). */
inline void ResetChannel(UnkSoundChannel_801D98FC *channel) {
    channel->m_bank = -1;
    channel->m_program = -1;
    channel->m_priority = 0;
    channel->field_0x4 = 0;
    channel->field_0x6 = 0;
    channel->field_0x5 = 0;
    channel->field_0x7 = 0;
    channel->field_0x8 = 0;
    channel->field_0x9 = 0;
    channel->m_flags = 0;
    channel->m_pitch = 0;
    channel->field_0xC = 0;
    channel->field_0xE = 0;
    channel->m_age = 0;
    channel->m_id = 0;
    channel->m_owner = 0;
    channel->m_position.x = channel->m_position.y = channel->m_position.z = 0.0f;
    channel->m_range = 0.0f;
    channel->m_distance = 0.0f;
    channel->m_entry = 0;
}

UnkSoundChannel_801D98FC::UnkSoundChannel_801D98FC() {
    ResetChannel(this);
}

void UnkSoundChannel_801D98FC::fn_801D9888(s8 index) {
    if (index >= 0) {
        m_index = index;
    }
    ResetChannel(this);
}

void UnkSoundChannel_801D98FC::fn_801D984C(s8 *group, u8 *voice) {
    if (m_index > 0x26) {
        *group = m_index - 0x23;
        *voice = 0;
    } else {
        *group = m_index >> 4;
        *voice = m_index & 0xF;
    }
}

void UnkSoundChannel_801D98FC::fn_801D978C(int id, UnkSoundBankEntry *entry, u8 priority) {
    m_entry = entry;
    m_bank = m_entry->field_0x8;
    m_program = m_entry->field_0x9;
    m_priority = priority;
    field_0x4 = 0;
    field_0x5 = 0;
    m_pitch = 0;
    field_0x8 = 0;
    field_0x7 = 0;
    m_flags = 0;
    if (m_entry->m_time != 0) {
        field_0xC = field_0xE = m_entry->m_time;
    } else {
        field_0xC = field_0xE = 0x78;
    }
    m_age = 0;
    m_id = id;
    m_owner = 0;
    m_position.x = m_position.y = m_position.z = 0.0f;
    m_range = fn_801D9BCC(m_entry);
    m_distance = 0.0f;
}

void UnkSoundChannel_801D98FC::fn_801D9780() {
    m_flags = 5;
}

void UnkSoundChannel_801D98FC::fn_801D9714(u16 flags, s16 time) {
    int t;
    if (time == 0) {
        if (m_entry->m_time == 0) {
            t = 0x78;
        } else {
            t = m_entry->m_time;
        }
    } else {
        t = time;
    }
    if (flags & 4) {
        field_0xC = t;
        field_0xE = t;
        m_flags |= 0x4000;
    } else {
        field_0xC = t;
        if (flags & 2) {
            m_flags |= 2;
        }
    }
}

void UnkSoundChannel_801D98FC::fn_801D9688(u16 flags, void *source) {
    /* Retail tests bit 0 twice (probably two flag names of the same value). */
    if ((flags & 1) && source != 0 && (flags & 1)) {
        fn_800091D0(&m_position, static_cast<const Vec3f *>(source));
        m_flags |= 0x10;
        if (!(m_entry->m_flags & 8)) {
            m_flags |= 0x40;
        }
        if (flags & 0x20) {
            m_flags |= 0x100;
        }
    }
}

bool UnkSoundChannel_801D98FC::fn_801D9608() {
    if (m_entry == 0 || !(m_flags & 0x10)) {
        return false;
    }
    if (m_entry->m_flags & 0x10) {
        float range = fn_801D9BCC(m_entry);
        if (fn_801D9964(&m_position) > range) {
            return true;
        }
    }
    return false;
}

void UnkSoundChannel_801D98FC::fn_801D95A4(u16 flags, s16 time) {
    int t;
    if (time == 0) {
        if (m_entry->m_time == 0) {
            t = 0x78;
        } else {
            t = m_entry->m_time;
        }
    } else {
        t = time;
    }
    if (flags & 4) {
        field_0xE = t;
        m_flags |= 0x4000;
    }
    if (flags & 2) {
        field_0xC = t;
        m_flags |= 2;
    }
}

void UnkSoundChannel_801D98FC::fn_801D956C(void *source) {
    if ((m_flags & 0x10) && source != 0) {
        fn_800091D0(&m_position, static_cast<const Vec3f *>(source));
    }
}

/* NonMatching: the mode and the boost are in each other's registers (see the top of the file). */
s8 UnkSoundChannel_801D98FC::fn_801D93FC() {
    float range = m_range;
    float distance = fn_801D9964(&m_position);
    if (distance >= range) {
        return -0x7F;
    }
    s32 mode = lbl_8057D6A0.m_soundMode;
    s16 boost = mode == 2 ? (s16)0x1E : (s16)0;
    s16 volume;
    if (distance < 60.0f) {
        volume = 0;
    } else {
        float raw = (distance - 60.0f) / (m_range - 60.0f);
        float t = raw;
        if (raw < 0.0f) {
            t = 0.0f;
        }
        t *= boost + 0x7F;
        volume = boost - t;
    }
    volume += field_0x7;
    if (volume < -0x7F) {
        volume = -0x7F;
    }
    if (mode == 2) {
        if (volume > field_0x8 + boost) {
            volume = field_0x8 + boost;
        }
        if (volume > 0x7F) {
            volume = 0x7F;
        }
    } else if (volume > field_0x8) {
        volume = field_0x8;
    }
    return volume;
}

s8 UnkSoundChannel_801D98FC::fn_801D9214(s8 *surround) {
    UnkGameState_8057D6A0 *state = &lbl_8057D6A0;
    if (state->m_soundMode == 1) {
        return 0;
    }
    if (state->m_twoPlayer) {
        return 0;
    }
    Vec3f camera;
    Vec3f offset(0.0f, 0.0f, 0.0f);
    fn_800091D0(&camera, fn_80010244(fn_80009548(), 0));
    float yaw = GetCameraAngles(fn_80009548(), 0).y;
    fn_800091D0(&offset, &(const Vec3f &)Sub(m_position, camera));
    offset.y = 0.0f;
    float pan = fn_8040CF50(&offset);
    if (state->m_soundMode == 2) {
        if (surround != 0) {
            fn_80016260(-offset.z, -offset.x);
            float rear = 0.0f;
            *surround = rear;
        }
        return 0;
    }
    float angle = fn_80016260(offset.z, offset.x);
    angle = 16384.0f + (angle + yaw);
    float side = 0.7f * std::sin(angle);
    if (pan < 50.0f) {
        pan = 100.0f * (side * (pan / 50.0f));
    } else {
        pan = 100.0f * side;
    }
    if (pan < -127.0f) {
        pan = -127.0f;
    } else if (pan > 127.0f) {
        pan = 127.0f;
    }
    if (state->m_soundMode == 2 && surround != 0) {
        *surround = std::fabs(127.0f * std::cos(angle));
    }
    return pan;
}

s16 UnkSoundChannel_801D98FC::fn_801D9068() {
    Vec3f listener(0.0f, 0.0f, 0.0f);
    Vec3f offset;
    float pitch = m_pitch;
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    if (!player.expired()) {
        boost::shared_ptr<CharInfo> character = player.lock();
        fn_800091D0(&listener, &character->m_position);
    }
    {
        Vec3f delta;
        fn_8040CD74(&delta, &listener, &m_position);
        fn_800091D0(&offset, &delta);
    }
    float distance = fn_8040CF50(&offset);
    if (m_distance == 0.0f && distance != 0.0f) {
        m_distance = distance;
        return 0;
    }
    if (m_distance > 800.0f) {
        m_distance = distance;
        return pitch;
    }
    pitch = 20.0f * (m_distance - distance);
    if (pitch > 6144.0f) {
        pitch = 6144.0f;
    } else if (pitch < -6144.0f) {
        pitch = -6144.0f;
    }
    m_distance = distance;
    return pitch;
}

void UnkSoundChannel_801D98FC::fn_801D8FAC() {
    s8 group;
    u8 voice;
    GetGroupVoice(this, group, voice);
    if (lbl_8057D6A0.m_soundMode == 2) {
        if (field_0x5 != 0) {
            fn_80053164()->fn_801D5DF0(field_0x5, group, voice);
        } else {
            fn_80053164()->fn_801D5E28(field_0x4, group, voice);
        }
    } else {
        fn_80053164()->fn_801D5E28(field_0x4, group, voice);
    }
}

void UnkSoundChannel_801D98FC::fn_801D898C() {
    s8 group;
    u8 voice;
    GetGroupVoice(this, group, voice);
    if (m_flags & 1) {
        m_distance = 0.0f;
        if (m_index < 0x26) {
            fn_80053164()->fn_801D5D10(group, voice, m_bank, m_program);
        }
        if (m_flags & 0x10) {
            field_0x6 = fn_801D93FC();
            if (m_flags & 0x400) {
                m_flags &= ~0x10;
                m_flags &= ~0x400;
            }
        } else if (m_flags & 0x20) {
            field_0x6 = field_0x7;
            m_flags &= ~0x20;
        } else {
            field_0x6 = field_0x7;
        }
        fn_80053164()->fn_801D5DB8(ScaleVolume(this, field_0x6), group, voice);
        if (m_flags & 0x40) {
            field_0x4 = fn_801D9214(&field_0x5);
            fn_801D8FAC();
        }
        if (m_flags & 0x80) {
            m_flags &= ~0x80;
            fn_801D8FAC();
        }
        if (m_flags & 0x100) {
            m_pitch = fn_801D9068();
        }
        if (m_flags & 0x200) {
            m_flags &= ~0x200;
        }
        fn_80053164()->fn_801D5D80(m_pitch, group, voice);
        fn_80053164()->fn_801D5E98(m_bank, m_program, m_priority, group, voice, m_range);
        m_flags &= ~1;
        field_0x9 |= kChannelStarted;
        m_age++;
        return;
    }
    if (field_0xC != 0) {
        if (m_flags & 0x10) {
            s8 volume = fn_801D93FC();
            if (field_0x6 != volume) {
                fn_80053164()->fn_801D5DB8(ScaleVolume(this, volume), group, voice);
                field_0x6 = volume;
            }
            if ((m_entry->m_flags & 0x10) && field_0x6 < -0x6E) {
                field_0xC = 1;
            }
        }
        if (m_flags & 0x20) {
            s8 volume = field_0x7;
            if (field_0x6 != volume) {
                fn_80053164()->fn_801D5DB8(ScaleVolume(this, volume), group, voice);
                field_0x6 = volume;
            }
            m_flags &= ~0x20;
        }
        if (m_flags & 0x40) {
            s8 surround = 0;
            s8 pan = fn_801D9214(&surround);
            if (field_0x4 != pan || field_0x5 != surround) {
                fn_801D8FAC();
                field_0x4 = pan;
                field_0x5 = surround;
            }
        }
        if (m_flags & 0x80) {
            fn_80053164()->fn_801D5E28(field_0x4, group, voice);
            m_flags &= ~0x80;
        }
        if (m_flags & 0x100) {
            s16 pitch = fn_801D9068();
            s16 old = m_pitch;
            if (old != pitch) {
                fn_80053164()->fn_801D5D80(old, group, voice);
                m_pitch = pitch;
            }
        }
        if (m_flags & 0x200) {
            fn_80053164()->fn_801D5D80(m_pitch, group, voice);
            m_flags &= ~0x200;
        }
        field_0xC--;
        m_age++;
    }
    if (field_0xC == 0) {
        if (m_flags & 0x4000) {
            m_flags |= 1;
            field_0xC = field_0xE;
            field_0x9 = 0x1F;
            m_flags &= ~0x4000;
            m_flags |= 4;
        } else if (m_flags & 4) {
            if (field_0x4 != 0 || field_0x5 != 0) {
                fn_80053164()->fn_801D5D48(group, voice);
                field_0x4 = 0;
                field_0x5 = 0;
            }
            if (m_pitch != 0) {
                fn_80053164()->fn_801D5D80(0, group, voice);
                m_pitch = 0;
            }
            fn_80053164()->fn_801D5E60(group, voice);
            m_owner = 0;
            m_id = 0;
            field_0xC = 0;
            m_flags = 0;
            m_age = 0;
            m_entry = 0;
        }
    }
}

void UnkSoundChannel_801D98FC::fn_801D88EC() {
    s8 group;
    u8 voice;
    GetGroupVoice(this, group, voice);
    fn_80053164()->fn_801D5D48(group, voice);
    field_0x4 = 0;
    field_0x5 = 0;
    fn_80053164()->fn_801D5E60(group, voice);
    m_owner = 0;
    m_id = 0;
    field_0xC = 0;
    m_flags = 0;
    m_age = 0;
    m_entry = 0;
}

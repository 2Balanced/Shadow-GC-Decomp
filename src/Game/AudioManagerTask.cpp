/* AudioManagerTask (class name from RTTI): the task that runs the sound manager once a frame. The
 * unit holds the sound manager (UnkSound_80053164, Game/SoundManager.h): its 43 channels (32 for
 * sounds, 5 for voices), the back end (IAudio, an AudioGCN), the bank set, and the functions other
 * units call by their fn_ names to play, stop and pause sounds, voices and music. While the mode is
 * 1 or 2, or flag 0 or 1 is set, only the ids in the two tables below play (behaviour inferred).
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first).
 *
 * NonMatching: UnkSound_80053164::FindFreeChannel (95%: retail keeps the priority in r4 and clears
 * the channel's priority byte again before the equality test). Every other function matches. */
#include "Game/IAudio.h"
#include "Game/SoundManager.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* The sound manager (a static object in the unit at 0x8004DFB0). */
UnkSound_80053164 *fn_80053164(void);
/* The music and voice code at 0x801D7500..0x801D84xx (behaviours unknown unless noted). */
void fn_801D7500(void);
void fn_801D7544(int volume);
void fn_801D756C(void);
void fn_801D75B0(const char *name);
void fn_801D763C(int mode);
void fn_801D77AC(void);
void fn_801D7830(int voice);
void fn_801D7A50(int id, int arg2, int arg3);
void fn_801D7BE0(void);
void fn_801D7CB8(void);
void fn_801D7DA0(void);
void fn_801D7E10(const char *name);
void fn_801D7EAC(void);
const char *fn_801D7ED8(void);
void fn_801D7F2C(const char *name);
void fn_801D7F74(void);
void fn_801D7FF0(void);
void fn_801D80AC(void);
void fn_801D82EC(void);
void fn_801D830C(void);
void fn_801D83AC(void);
/* The audible distance of an entry and the distance of a source (behaviours inferred). */
float fn_801D9BCC(UnkSoundBankEntry *entry);
float fn_801D9964(void *source);
/* Records the song being played (behaviour inferred). */
void *fn_800BB74C(void);
void fn_801E4F70(void *object, const char *name);
}

/* The ids that still play while flag 0 is set (name inferred). */
static const u16 s_voiceIds[6] = {0xE000, 0xE001, 0xE002, 0xE003, 0xE004, 0xE005};

/* The ids that still play while flag 1 is set (name inferred). */
static u16 s_soundIds[10] = {0xE000, 0xE001, 0xE002, 0xE003, 0xE004,
                             0xE005, 0xE006, 0xE007, 0xE008, 0xE009};

/* The pans fn_801D5300 alternates between (name inferred). */
static s8 s_pans[16] = {-0x40, 0x40, -0x40, -0x40, 0x40, 0x40, -0x40, -0x40,
                        0x40,  0x40, -0x40, -0x40, 0x40, 0x40, 0,     0};

/* Whether sound id may play while flag 0 is set (name inferred). */
inline bool IsVoiceAllowed(UnkSoundFlags &flags, u16 id) {
    if (!fn_80014E34(&flags.m_bits, 0)) {
        return true;
    }
    if ((((u32)id >> 12) & 0xF) != 0xE) {
        return false;
    }
    for (int i = 0; i < 6; i++) {
        if (id == s_voiceIds[i]) {
            return true;
        }
    }
    return false;
}

/* Whether sound id may play while flag 1 is set (name inferred). */
inline bool IsSoundAllowed(UnkSoundFlags &flags, u16 id) {
    if (!fn_80014E34(&flags.m_bits, 1)) {
        return true;
    }
    if ((((u32)id >> 12) & 0xF) != 0xE) {
        return false;
    }
    for (int i = 0; i < 10; i++) {
        if (id == s_soundIds[i]) {
            return true;
        }
    }
    return false;
}

/* Stops every channel (name inferred). */
inline void StopChannels(UnkSound_80053164 *sound) {
    s8 i;
    UnkSoundChannel_801D98FC *channel = sound->m_channels;
    for (i = 0; i < 0x2B; i++) {
        channel->field_0xC = 0;
        channel->m_entry = 0;
        channel->m_flags |= 4;
        if (sound->m_mode != 0) {
            channel->field_0x6 = -0x80;
        }
        channel->fn_801D88EC();
        channel++;
    }
    sound->m_nextChannel = 0;
    sound->m_nextVoice = 0x26;
    if (sound->m_audio != 0) {
        sound->m_audio->vf_0x34();
    }
    sound->m_mode = 0;
}

/* Resets every channel (name inferred). */
inline void ResetChannels(UnkSound_80053164 *sound) {
    for (int i = 0; i < 0x2B; i++) {
        sound->m_channels[i].fn_801D9888(i);
    }
}

void IAudio::vf_0x10() {}

void IAudio::vf_0x0C() {}

void UnkSound_80053164::Init() {
    m_audio = new AudioGCN;
    fn_801D83AC();
    m_channels = new UnkSoundChannel_801D98FC[0x2B];
    if (m_audio != 0) {
        m_audio->vf_0x54();
    }
    ResetChannels(this);
}

/* Shuts the sound down (name inferred). */
extern "C" void fn_801D6760(UnkSound_80053164 *sound) {
    if (sound->m_channels != 0) {
        delete[] sound->m_channels;
        sound->m_channels = 0;
    }
    fn_801D830C();
    delete sound->m_audio;
    sound->m_audio = 0;
}

void UnkSound_80053164::LoadBanks() {
    if (m_audio != 0) {
        m_audio->vf_0x1C(0, 0);
        m_banks = new UnkSoundBank_801D8890;
        m_banks->fn_801D8680(-1, 0);
        m_banks->fn_801D8680(-1, 2);
        m_banks->fn_801D8680(-1, 3);
        m_banks->fn_801D8680(-1, 1);
        m_banks->fn_801D8680(-1, 6);
        m_banks->fn_801D8680(-1, 4);
        m_banks->fn_801D8680(-1, 5);
    }
    if (m_audio != 0) {
        m_audio->vf_0x54();
    }
    ResetChannels(this);
}

/* Unloads the banks (name inferred). */
extern "C" void fn_801D658C(UnkSound_80053164 *sound) {
    if (sound->m_audio != 0) {
        sound->m_audio->vf_0x20(0);
    }
    if (sound->m_banks != 0) {
        sound->m_banks->fn_801D8600(5);
        sound->m_banks->fn_801D8600(4);
        sound->m_banks->fn_801D8600(6);
        sound->m_banks->fn_801D8600(1);
        sound->m_banks->fn_801D8600(3);
        sound->m_banks->fn_801D8600(2);
        sound->m_banks->fn_801D8600(0);
        delete sound->m_banks;
        sound->m_banks = 0;
    }
}

extern "C" void fn_801D656C(UnkSound_80053164 *sound) {
    fn_801D80AC();
}

extern "C" void fn_801D654C(UnkSound_80053164 *sound) {
    fn_801D7FF0();
}

/* Stops everything and loads the banks of stage (name inferred). */
extern "C" void fn_801D6340(UnkSound_80053164 *sound, int stage, int arg2, u8 keepMusic) {
    StopChannels(sound);
    if (sound->m_audio != 0) {
        sound->m_audio->vf_0x54();
    }
    ResetChannels(sound);
    if (!keepMusic) {
        fn_801D7EAC();
    }
    if (sound->m_audio != 0 && stage != -1) {
        sound->m_audio->vf_0x1C(3, arg2);
        sound->m_audio->vf_0x1C(5, arg2);
        sound->m_audio->vf_0x1C(4, arg2);
        sound->m_audio->vf_0x1C(6, arg2);
        sound->m_audio->vf_0x1C(2, arg2);
        sound->m_audio->vf_0x1C(1, arg2);
        sound->m_audio->vf_0x1C(7, arg2);
        sound->m_banks->fn_801D8680(stage, 7);
    }
    sound->m_flags.Reset();
    sound->m_mode = 0;
    sound->m_timer = 0.0f;
}

/* Stops everything and unloads the stage's banks (name inferred). */
extern "C" void fn_801D615C(UnkSound_80053164 *sound, u8 keepMusic) {
    StopChannels(sound);
    if (sound->m_audio != 0) {
        sound->m_audio->vf_0x54();
    }
    ResetChannels(sound);
    sound->m_flags.Reset();
    sound->m_mode = 0;
    sound->m_timer = 0.0f;
    if (!keepMusic) {
        fn_801D7EAC();
    }
    if (sound->m_audio != 0) {
        sound->m_audio->vf_0x20(7);
        sound->m_audio->vf_0x20(1);
        sound->m_audio->vf_0x20(2);
        sound->m_audio->vf_0x20(6);
        sound->m_audio->vf_0x20(4);
        sound->m_audio->vf_0x20(5);
        sound->m_audio->vf_0x20(3);
    }
    if (sound->m_banks != 0) {
        sound->m_banks->fn_801D8600(7);
    }
}

UnkSound_80053164::UnkSound_80053164() {
    m_audio = 0;
    m_nextVoice = 0;
    m_nextChannel = 0;
}

UnkSound_80053164::~UnkSound_80053164() {}

void UnkSound_80053164::Update(float dt) {
    m_timer -= dt;
    if (m_timer <= 0.0f) {
        m_timer = 0.0f;
    }
    UpdateChannels();
    fn_801D82EC();
}

void IAudio::Update() {}

void UnkSound_80053164::UpdateChannels() {
    if (m_banks != 0) {
        m_banks->fn_801D844C();
    }
    if (m_mode == 2) {
        return;
    }
    UnkSoundChannel_801D98FC *channel = m_channels;
    for (int i = 0; i < 0x2B; i++) {
        if (channel->m_entry == 0) {
            channel++;
            continue;
        }
        if (m_mode == 1 && !IsVoiceAllowed(m_flags, (u32)channel->m_id)) {
            channel++;
            continue;
        }
        if (m_flags.Test(1) && !IsSoundAllowed(m_flags, (u32)channel->m_id) &&
            (channel->field_0xC > 10 || channel->field_0xC < 0)) {
            channel->field_0xC = 10;
        }
        channel->fn_801D898C();
        channel++;
    }
    if (m_audio != 0) {
        m_audio->Update();
    }
}

void UnkSound_80053164::fn_801D5E98(s8 bank, s8 program, u8 priority, s8 group, u8 voice,
                                    float range) {
    if (m_audio != 0) {
        m_audio->vf_0x24(bank, program, priority, group, voice, range);
    }
}

void UnkSound_80053164::fn_801D5E60(s8 group, u8 voice) {
    if (m_audio != 0) {
        m_audio->vf_0x30(group, voice);
    }
}

void UnkSound_80053164::fn_801D5E28(s8 pan, s8 group, u8 voice) {
    if (m_audio != 0) {
        m_audio->vf_0x3C(pan, group, voice);
    }
}

void UnkSound_80053164::fn_801D5DF0(s8 surround, s8 group, u8 voice) {
    if (m_audio != 0) {
        m_audio->vf_0x44(surround, group, voice);
    }
}

void UnkSound_80053164::fn_801D5DB8(s8 volume, s8 group, u8 voice) {
    if (m_audio != 0) {
        m_audio->vf_0x38(volume, group, voice);
    }
}

void UnkSound_80053164::fn_801D5D80(s16 pitch, s8 group, u8 voice) {
    if (m_audio != 0) {
        m_audio->vf_0x4C(pitch, group, voice);
    }
}

void UnkSound_80053164::fn_801D5D48(s8 group, u8 voice) {
    if (m_audio != 0) {
        m_audio->vf_0x48(group, voice);
    }
}

void UnkSound_80053164::fn_801D5D10(s8 group, u8 voice, s8 bank, s8 program) {
    if (m_audio != 0) {
        m_audio->vf_0x50(group, voice, bank, program);
    }
}

UnkSoundChannel_801D98FC *UnkSound_80053164::AllocChannel(int id, u8 priority, void *source) {
    if (!IsSoundAllowed(m_flags, id)) {
        return 0;
    }
    if (m_banks != 0) {
        UnkSoundChannel_801D98FC *channel;
        UnkSoundBankEntry *entry = m_banks->fn_801D852C(id);
        if (entry == 0) {
            return 0;
        }
        if (entry == 0) {
            channel = 0;
        } else if ((entry->m_flags & 0x10) && source != 0 &&
                   fn_801D9964(source) > fn_801D9BCC(entry)) {
            channel = 0;
        } else {
            if (entry->m_priority == 10) {
                priority = 0;
            } else if (entry->m_priority > priority) {
                priority = entry->m_priority + 1;
            } else {
                priority = priority + 1;
            }
            s8 index = FindFreeChannel(entry, priority);
            if (index < 0) {
                channel = 0;
            } else {
                channel = &m_channels[index];
                channel->fn_801D978C(id, entry, priority);
            }
        }
        return channel;
    }
    return 0;
}

UnkSoundChannel_801D98FC *UnkSound_80053164::FindChannel(u16 id, void *owner) {
    if (!IsSoundAllowed(m_flags, id)) {
        return 0;
    }
    UnkSoundChannel_801D98FC *channels = m_channels;
    s8 i;
    s8 found = -1;
    UnkSoundChannel_801D98FC *channel = channels;
    for (i = 0; i < 0x2B; channel++, i++) {
        if (channel->m_owner == owner && channel->m_id == id) {
            found = i;
            break;
        }
    }
    if (found < 0) {
        return 0;
    }
    return &channels[found];
}

s8 UnkSound_80053164::FindFreeChannel(UnkSoundBankEntry *entry, u8 priority) {
    if (entry == 0) {
        return -1;
    }
    s8 found = -1;
    u16 id = entry->m_id;
    if (entry->m_flags & 2) {
        s8 index = m_nextVoice;
        s8 count = 0x2B;
        UnkSoundChannel_801D98FC *channel = &m_channels[index];
        s8 oldest = -1;
        s8 lower = -1;
        u16 age;
        while (count > 0x26) {
            if (channel->m_entry == 0) {
                found = index;
                break;
            }
            if (channel->m_priority < priority) {
                lower = index;
            } else if (priority == channel->m_priority) {
                if (oldest < 0) {
                    age = channel->m_age;
                    lower = index;
                } else if (age < channel->m_age) {
                    oldest = index;
                    age = channel->m_age;
                }
            }
            index++;
            count--;
            channel++;
            if (index >= 0x2B) {
                index = 0x26;
                channel = &m_channels[0x26];
            }
        }
        if (found < 0) {
            if (lower > 0) {
                found = lower;
            } else if (oldest > 0) {
                found = oldest;
            } else {
                return -1;
            }
        }
        m_nextVoice = found + 1;
        if (m_nextVoice >= 0x2B) {
            m_nextVoice = 0x26;
        }
    } else {
        s8 index = m_nextChannel;
        s8 lower = -1;
        s8 oldest = -1;
        u16 age = 0;
        s8 count = 0x20;
        UnkSoundChannel_801D98FC *channels = m_channels;
        UnkSoundChannel_801D98FC *channel = &channels[index];
        while (found < 0 && count > 0) {
            if (channel->m_id == 0) {
                found = index;
            } else if (channel->m_priority < priority) {
                lower = index;
            } else if (lower < 0 && channel->m_priority == priority &&
                       (oldest < 0 || channel->m_age > age)) {
                oldest = index;
                age = channel->m_age;
            }
            index++;
            count--;
            channel++;
            if (index >= 0x20) {
                index = 0;
                channel = channels;
            }
        }
        if (found < 0) {
            if (lower >= 0) {
                found = lower;
            } else {
                found = oldest;
            }
        }
        m_nextChannel = found + 1;
        if (m_nextChannel >= 0x20) {
            m_nextChannel = 0;
        }
        if (found >= 0) {
            UnkSoundChannel_801D98FC *channel = &m_channels[found];
            if (m_mode == 2) {
                channel->field_0x6 = -0x7F;
            } else if (m_mode == 1 && !IsVoiceAllowed(m_flags, (u32)channel->m_id) &&
                       IsVoiceAllowed(m_flags, id) == true) {
                channel->field_0x6 = -0x7F;
            }
            u8 right;
            s8 left;
            channel->fn_801D984C(&left, &right);
            if (channel->field_0x4 != 0 || channel->field_0x5 != 0) {
                channel->field_0x4 = 0;
                channel->field_0x5 = 0;
                if (m_audio != 0) {
                    m_audio->vf_0x48(left, right);
                }
            }
            if (m_audio != 0) {
                m_audio->vf_0x30(left, right);
            }
        }
    }
    return found;
}

/* Starts sound id with no source (name inferred). */
extern "C" bool fn_801D560C(UnkSound_80053164 *sound, int id, u8 volume, void *owner) {
    if ((u16)id == 0xFFFF) {
        return false;
    }
    UnkSoundChannel_801D98FC *channel = sound->AllocChannel(id, 0, 0);
    if (channel == 0) {
        return false;
    }
    channel->fn_801D9780();
    channel->field_0x6 = volume;
    channel->m_owner = owner;
    return true;
}

/* Starts sound id at source, or updates the one owner plays (name inferred). */
extern "C" bool fn_801D54F4(UnkSound_80053164 *sound, int id, void *source, void *owner,
                            u16 flags) {
    if ((u16)id == 0xFFFF) {
        return false;
    }
    if (owner != 0) {
        UnkSoundChannel_801D98FC *channel = sound->FindChannel(id, owner);
        if (channel != 0) {
            sound->ApplyFlags(channel, flags);
            channel->fn_801D956C(source);
            if (channel->fn_801D9608() == true) {
                return true;
            }
            channel->fn_801D95A4(flags, 0);
            return true;
        }
    }
    UnkSoundChannel_801D98FC *channel = sound->AllocChannel(id, 0, source);
    if (channel == 0) {
        return false;
    }
    sound->ApplyFlags(channel, flags);
    channel->fn_801D9780();
    channel->fn_801D9688(flags, source);
    channel->fn_801D9714(flags, 0);
    channel->m_owner = owner;
    return true;
}

/* Starts sound id at source with a volume, or updates the one owner plays (name inferred). */
extern "C" bool fn_801D53C4(UnkSound_80053164 *sound, int id, void *source, void *owner,
                            int flags, u8 volume, int arg6) {
    u16 f;
    if ((u16)id == 0xFFFF) {
        return false;
    }
    if (owner != 0) {
        UnkSoundChannel_801D98FC *channel = sound->FindChannel(id, owner);
        if (channel != 0) {
            channel->fn_801D956C(source);
            channel->field_0x6 = volume;
            if (channel->fn_801D9608() == true) {
                return true;
            }
            channel->fn_801D95A4(flags, arg6);
            return true;
        }
    }
    UnkSoundChannel_801D98FC *channel;
    if (flags & 2) {
        channel = sound->AllocChannel(id, 1, source);
    } else {
        channel = sound->AllocChannel(id, 0, source);
    }
    if (channel == 0) {
        return false;
    }
    channel->fn_801D9780();
    f = flags;
    channel->fn_801D9688(f, source);
    channel->fn_801D9714(f, arg6);
    channel->field_0x6 = volume;
    channel->m_owner = owner;
    return true;
}

/* Plays sound 0xE00B at most every 1/30 s, alternating sides (name inferred). */
extern "C" bool fn_801D5300(UnkSound_80053164 *sound) {
    static int s_side;
    static s8 s_sideSet;
    if (!s_sideSet) {
        s_side = 1;
        s_sideSet = 1;
    }
    if (sound->m_timer > 0.0f) {
        return false;
    }
    UnkSoundChannel_801D98FC *channel = sound->AllocChannel(0xE00B, 0, 0);
    if (channel == 0) {
        return false;
    }
    channel->m_flags = 0x85;
    channel->field_0x8 = 0;
    channel->field_0x7 = 0;
    channel->field_0x4 = s_pans[s_side];
    s_side = s_side ? 0 : 1;
    sound->m_timer = 1.0f / 30.0f;
    return true;
}

void UnkSound_80053164::ApplyFlags(UnkSoundChannel_801D98FC *channel, u16 &flags) {
    if (flags & 0x40) {
        if (channel->IsLooping()) {
            flags |= 2;
        }
        if (channel->HasFlag4()) {
            flags |= 1;
        }
    }
}

/* Plays a song (name inferred). */
extern "C" void fn_801D524C(UnkSound_80053164 *sound, const char *name) {
    fn_801D7F2C(name);
    fn_801E4F70(fn_800BB74C(), name);
}

extern "C" void fn_801D522C(UnkSound_80053164 *sound) {
    fn_801D7EAC();
}

extern "C" void fn_801D520C(UnkSound_80053164 *sound) {
    fn_801D7CB8();
}

extern "C" void fn_801D51EC(UnkSound_80053164 *sound) {
    fn_801D7BE0();
}

/* The song playing (name inferred). */
extern "C" const char *fn_801D51CC(UnkSound_80053164 *sound) {
    return fn_801D7ED8();
}

/* Plays voice id (name inferred). */
extern "C" void fn_801D51A0(UnkSound_80053164 *sound, int id, s8 arg2, int arg3) {
    fn_801D7A50(id, arg2, arg3);
}

extern "C" void fn_801D517C(UnkSound_80053164 *sound, int voice) {
    fn_801D7830(voice);
}

extern "C" void fn_801D515C(UnkSound_80053164 *sound) {
    fn_801D77AC();
}

/* Plays and stops a jingle (names inferred; see Game/ActStageController.cpp). */
extern "C" void fn_801D5138(UnkSound_80053164 *sound, const char *name) {
    fn_801D7E10(name);
}

extern "C" void fn_801D5118(UnkSound_80053164 *sound) {
    fn_801D7DA0();
}

extern "C" void fn_801D50F4(UnkSound_80053164 *sound, const char *name) {
    fn_801D75B0(name);
}

extern "C" void fn_801D50D4(UnkSound_80053164 *sound) {
    fn_801D756C();
}

/* Pauses the channels (name inferred). */
extern "C" void fn_801D4FBC(UnkSound_80053164 *sound, int mode) {
    sound->m_flags.Set(0);
    sound->m_mode = mode;
    for (int i = 0; i < 0x2B; i++) {
        UnkSoundChannel_801D98FC *channel = &sound->m_channels[i];
        if (channel->m_entry != 0 &&
            (sound->m_mode == 2 || IsVoiceAllowed(sound->m_flags, (u32)channel->m_id) != true)) {
            s8 left;
            u8 right;
            channel->fn_801D984C(&left, &right);
            sound->m_audio->vf_0x5C(left, right);
        }
    }
}

/* Resumes the channels (name inferred). */
extern "C" void fn_801D4EA0(UnkSound_80053164 *sound) {
    int i;
    for (i = 0; i < 0x2B; i++) {
        UnkSoundChannel_801D98FC *channel = &sound->m_channels[i];
        if (channel->m_entry != 0 &&
            (sound->m_mode == 2 || IsVoiceAllowed(sound->m_flags, (u32)channel->m_id) != true)) {
            s8 left;
            u8 right;
            channel->fn_801D984C(&left, &right);
            sound->m_audio->vf_0x64(channel->field_0x6, left, right);
        }
    }
    sound->m_flags.Clear(0);
    sound->m_mode = 0;
}

extern "C" void fn_801D4E78(UnkSound_80053164 *sound) {
    sound->m_flags.Set(1);
}

extern "C" void fn_801D4E50(UnkSound_80053164 *sound) {
    sound->m_flags.Clear(1);
}

extern "C" void fn_801D4E20(UnkSound_80053164 *sound, int event) {
    if (event == 0) {
        sound->m_flags.Set(1);
    }
}

extern "C" void fn_801D4DF0(UnkSound_80053164 *sound, int event) {
    if (event == 0) {
        sound->m_flags.Clear(1);
    }
}

/* Stops every channel and switches the back end to mode (name inferred). */
extern "C" void fn_801D4D0C(UnkSound_80053164 *sound, s32 mode) {
    StopChannels(sound);
    if (sound->m_audio != 0) {
        sound->m_audio->vf_0x58(mode);
    }
    fn_801D763C(mode);
    fn_801D7F74();
}

/* Stops every channel (name inferred). The other functions that do this expand the same code
 * from StopChannels; retail allocates registers here as for a body of its own. */
extern "C" void fn_801D4C5C(UnkSound_80053164 *sound) {
    s8 i;
    UnkSoundChannel_801D98FC *channel = sound->m_channels;
    for (i = 0; i < 0x2B; i++) {
        channel->field_0xC = 0;
        channel->m_entry = 0;
        channel->m_flags |= 4;
        if (sound->m_mode != 0) {
            channel->field_0x6 = -0x80;
        }
        channel->fn_801D88EC();
        channel++;
    }
    sound->m_nextChannel = 0;
    sound->m_nextVoice = 0x26;
    if (sound->m_audio != 0) {
        sound->m_audio->vf_0x34();
    }
    sound->m_mode = 0;
}

extern "C" void fn_801D4C38(UnkSound_80053164 *sound, int volume) {
    fn_801D7544(volume);
}

extern "C" void fn_801D4C18(UnkSound_80053164 *sound) {
    fn_801D7500();
}

class AudioManagerTask : public Task {
  public:
    AudioManagerTask(Task *parent);
    virtual ~AudioManagerTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(AudioManagerTask, 0x28);

const char *AudioManagerTask::s_taskName = "AudioManagerTask";

AudioManagerTask::AudioManagerTask(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
}

AudioManagerTask::~AudioManagerTask() {}

void AudioManagerTask::Tick(float dt) {
    fn_80053164()->Update(dt);
}

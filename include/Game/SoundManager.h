#ifndef GAME_SOUNDMANAGER_H
#define GAME_SOUNDMANAGER_H

/* The sound manager fn_80053164 returns (class name unknown; a static object at 0x80570B74), its 43
 * channels and its sound banks. The manager's code is in Game/AudioManagerTask.cpp; the channels'
 * and the banks' code follows at 0x801D6914..0x801D9C0C. Class names are invented; field names
 * marked "(meaning inferred)" follow the code that uses them. */

#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

class IAudio;

extern "C" {
/* A 32-bit flag word (see Game/EggLastMech.h). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
}

/* A sound's entry in a bank (struct name unknown). */
struct UnkSoundBankEntry {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ u32 m_id;
    /* 0x08 */ u8 field_0x8;
    /* 0x09 */ s8 field_0x9; /* negative: disabled (meaning inferred) */
    /* 0x0A */ s16 m_volume; /* percent of full volume added to the channel's (meaning inferred) */
    /* 0x0C */ u8 m_range; /* scaled into the audible distance (meaning inferred) */
    /* 0x0D */ u8 m_time; /* frames to play; 0 means 120 (meaning inferred) */
    /* 0x0E */ u8 m_flags; /* 0x1 loops, 0x2 a voice, 0x4, 0x10 limited by distance (meanings inferred) */
    /* 0x0F */ u8 m_priority;
    /* 0x10 */ s32 m_playing; /* instances started since the last reset (meaning inferred) */
    /* 0x14 */ s32 m_maxPlaying; /* 0: no limit (meaning inferred) */
};

/* The reader of a loaded data file (struct name unknown; see Game/HintController.cpp). */
struct UnkPath_8040E61C;

/* A bank: a loaded data file and its entries (struct name unknown). */
struct UnkSoundBankGroup {
    UnkSoundBankGroup() : m_count(0), m_entries(0), m_file(0), m_reader(0) {}

    /* 0x0 */ s32 m_count;
    /* 0x4 */ UnkSoundBankEntry *m_entries;
    /* 0x8 */ void *m_file;
    /* 0xC */ UnkPath_8040E61C *m_reader;
};

/* The sound banks (class name unknown; code in Game/AudioDataBank.cpp). */
class UnkSoundBank_801D8890 {
  public:
    UnkSoundBank_801D8890();
    ~UnkSoundBank_801D8890();
    /* Resets the entries' play counts (behaviour inferred). */
    void fn_801D844C();
    /* The bank of sound id, or 0 (behaviour inferred). */
    UnkSoundBankGroup *GetGroup(int id);
    /* The entry of sound id, or 0 when it may not play now (behaviour inferred). */
    UnkSoundBankEntry *fn_801D852C(int id);
    /* Unloads bank group (behaviour inferred). */
    bool fn_801D8600(int group);
    /* Loads bank group, for stage when group is 7 (behaviour inferred). */
    bool fn_801D8680(int stage, int group);
    /* Loads file as bank group (behaviour inferred). */
    bool LoadFile(const char *file, int group);

    /* 0x00 */ UnkSoundBankGroup m_groups[8];
};
SIZE_ASSERT(UnkSoundBank_801D8890, 0x80);

/* A sound channel (class name unknown; 0x34 bytes; code in Game/AudioChannel.cpp). */
class UnkSoundChannel_801D98FC {
  public:
    UnkSoundChannel_801D98FC();
    ~UnkSoundChannel_801D98FC() {}
    void fn_801D88EC();
    void fn_801D898C();
    /* Sends the pan, or the surround value in surround mode (behaviour inferred). */
    void fn_801D8FAC();
    /* The pitch shift from the change in distance to the player (behaviour inferred). */
    s16 fn_801D9068();
    /* The pan from the direction of the camera, and the surround value (behaviour inferred). */
    s8 fn_801D9214(s8 *surround);
    /* The volume from the distance to the player (behaviour inferred). */
    s8 fn_801D93FC();
    void fn_801D956C(void *source);
    void fn_801D95A4(u16 flags, s16 arg2);
    bool fn_801D9608();
    void fn_801D9688(u16 flags, void *source);
    void fn_801D9714(u16 flags, s16 arg2);
    void fn_801D9780();
    void fn_801D978C(int id, UnkSoundBankEntry *entry, u8 priority);
    /* The channel's group and voice in the back end (behaviour inferred). */
    void fn_801D984C(s8 *group, u8 *voice);
    void fn_801D9888(s8 index);

    /* Whether the entry loops / has flag 0x4 (names inferred). */
    bool IsLooping() const {
        return m_entry != 0 && (m_entry->m_flags & 1) != 0;
    }
    bool HasFlag4() const {
        return m_entry != 0 && (m_entry->m_flags & 4) != 0;
    }

    /* 0x00 */ s8 m_index; /* selects the group and voice (meaning inferred) */
    /* 0x01 */ s8 m_bank; /* the entry's bank and program (meanings inferred) */
    /* 0x02 */ s8 m_program;
    /* 0x03 */ u8 m_priority;
    /* 0x04 */ s8 field_0x4;
    /* 0x05 */ s8 field_0x5;
    /* 0x06 */ s8 field_0x6;
    /* 0x07 */ s8 field_0x7; /* volume (meaning inferred) */
    /* 0x08 */ s8 field_0x8; /* volume limit (meaning inferred) */
    /* 0x09 */ u8 field_0x9;
    /* 0x0A */ s16 m_pitch; /* (meaning inferred) */
    /* 0x0C */ s16 field_0xC; /* frames left */
    /* 0x0E */ s16 field_0xE;
    /* 0x10 */ u16 m_age; /* compared to pick the channel to take over (meaning inferred) */
    /* 0x12 */ u16 m_id;
    /* 0x14 */ u32 m_flags;
    /* 0x18 */ void *m_owner;
    /* 0x1C */ Vec3f m_position;
    /* 0x28 */ float m_range; /* how far the sound is heard (meaning inferred) */
    /* 0x2C */ float m_distance; /* the distance to the player last frame (meaning inferred) */
    /* 0x30 */ UnkSoundBankEntry *m_entry;
};
SIZE_ASSERT(UnkSoundChannel_801D98FC, 0x34);

/* The manager's flag word (struct name unknown). */
struct UnkSoundFlags {
    UnkSoundFlags() {
        fn_80014E04(&m_bits);
    }
    void Reset() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* The sound manager (class name unknown). Most of its functions are reached by their fn_ names
 * from other units (extern "C" in Game/AudioManagerTask.cpp). */
class UnkSound_80053164 {
  public:
    UnkSound_80053164();
    ~UnkSound_80053164();

    void Update(float dt);
    void UpdateChannels();
    UnkSoundChannel_801D98FC *FindChannel(u16 id, void *owner);
    UnkSoundChannel_801D98FC *AllocChannel(int id, u8 priority, void *source);
    s8 FindFreeChannel(UnkSoundBankEntry *entry, u8 priority);
    void ApplyFlags(UnkSoundChannel_801D98FC *channel, u16 &flags);
    void Init();
    void LoadBanks();
    /* Forwarders to the back end, used by the channels (names unknown; see IAudio). */
    void fn_801D5D10(s8 group, u8 voice, s8 bank, s8 program);
    void fn_801D5D48(s8 group, u8 voice);
    void fn_801D5D80(s16 pitch, s8 group, u8 voice);
    void fn_801D5DB8(s8 volume, s8 group, u8 voice);
    void fn_801D5DF0(s8 surround, s8 group, u8 voice);
    void fn_801D5E28(s8 pan, s8 group, u8 voice);
    void fn_801D5E60(s8 group, u8 voice);
    void fn_801D5E98(s8 bank, s8 program, u8 priority, s8 group, u8 voice, float range);

    /* 0x00 */ IAudio *m_audio;
    /* 0x04 */ UnkSoundChannel_801D98FC *m_channels;
    /* 0x08 */ UnkSoundBank_801D8890 *m_banks;
    /* 0x0C */ s8 m_nextChannel;
    /* 0x0D */ s8 m_nextVoice;
    /* 0x10 */ s32 m_mode; /* 1 and 2 hold back sounds (meaning inferred) */
    /* 0x14 */ UnkSoundFlags m_flags;
    /* 0x18 */ float m_timer;
};
SIZE_ASSERT(UnkSound_80053164, 0x1C);

#endif

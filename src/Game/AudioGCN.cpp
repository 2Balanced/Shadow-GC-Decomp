/* AudioGCN (class name from RTTI; base IAudio, Game/IAudio.h): the GameCube sound back end. It sets
 * up the sound library and its heap, keeps a sequence player for each of eight groups, and turns
 * the sound manager's channel calls into MIDI-like messages for the players of groups 0 to 2 (the
 * others go to the library's own calls). The bank files of the stages are found by stage number in
 * a table. File name from the class; the library functions keep their fn_ names, and their
 * meanings, the slot meanings and the member names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 24 of 25 functions match. In vf_0x50 (98.9%) retail keeps the group's player
 * pointer in r31 and the voice in r30, GC/2.7 the other way round. Declaring the voice s8 (in
 * IAudio and in the caller, UnkSound_80053164::fn_801D5D10) matches it, which suggests it is an s8
 * in the game; the caller's mangled name (with u8) is already in symbols.txt, so the declaration
 * stays as it is. */
#define AUDIOGCN_MEMBERS
#include "Game/IAudio.h"
#include <dolphin/types.h>
#include "size_assert.h"

SIZE_ASSERT(AudioGCN, 0x2C);

/* The game state (struct name unknown; see Game/GameSystem.cpp), as far as this unit uses it. */
struct UnkGameState_8057D6A0 {
    /* 0x00 */ u8 field_0x0[0x40];
    /* 0x40 */ s32 m_voiceLanguage; /* 0: Japanese (meaning inferred) */
    /* 0x44 */ u8 field_0x44[0x78 - 0x44];
    /* 0x78 */ s32 m_soundMode; /* 2: surround (meaning inferred) */
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A message for a sequence player (struct name unknown; size from the stack frames). */
struct UnkAudioMessage_8043F41C {
    /* 0x0 */ u8 field_0x0[0x8];
};

extern "C" {
/* The sound library (behaviour inferred). */
void fn_80381B54(void *config, int count);
void fn_80383684(void);
void fn_80381438(int arg);
void fn_80383850(void);
void fn_803893E8(void);
void fn_80385154(void (*callback)(void));
void fn_8043E394(void);
void fn_803894B0(void);
void fn_803838C0(void);
void fn_8043EBFC(void);
void fn_803894B4(int arg);
void fn_8043FAB0(int arg);
void fn_8043E430(int arg);
u32 fn_80376F58(void);
void fn_8043F240(int arg1, int arg2);
void fn_8043F284(int arg);
void fn_8043F1FC(void);
void fn_8043F2F0(void);
void fn_8043F0D4(void);
int fn_8043FB24(void *heap, int arg1, int arg2);
void fn_8043F6AC(void **player);
u32 fn_8043FA20(void **sound);
int fn_8043F980(void *sound, void *data, s32 size, int arg3, int arg4);
u32 fn_8043F70C(void *sound);
void fn_8043F8C4(void *sound);
/* Builds a message, and sends it to a sequence player. */
void fn_8043F41C(void *player, UnkAudioMessage_8043F41C *message, u8 status, ...);
void fn_8043F35C(void *player, UnkAudioMessage_8043F41C *message, int priority);
/* The library's own channel calls. */
void fn_8043EC3C(void *player, s8 volume);
void fn_8043ECDC(void *player);
void fn_8043ED7C(void *player, int volume, int arg2);
void fn_8043EE74(void *player, int pitch, int arg2);
void fn_8043EF48(void *player, int pan, int arg2);
void fn_8043EF50(void *player);
void fn_8043EFF0(void *player, int bank, int program, int priority);

/* The heap (see Game/GameSystem.cpp) and the file system (behaviour inferred). */
void *fn_80049B90(u32 size, int arg1);
void fn_80049B34(void *block);
void *fn_800371D8(void);
void *fn_8004509C(void *fileSystem, const char *path, s32 *size);
}

/* A stage's sound bank file (struct name unknown). */
struct UnkAudioBank_8053E1B8 {
    /* 0x0 */ s32 m_stage;
    /* 0x4 */ const char *m_file;
};

/* The sound library's setup (struct name and contents unknown). */
static s32 lbl_8053E1A8[4] = {0, 0, 0, 0};

/* The bank file of each stage, until -1. */
static UnkAudioBank_8053E1B8 lbl_8053E1B8[] = {
    {100, "sound/B71_CITY.mlt"},      {202, "sound/B71_CITY.mlt"},       {400, "sound/B71_CITY.mlt"},
    {200, "sound/B72_ELEC.mlt"},      {403, "sound/B72_ELEC.mlt"},       {201, "sound/B73_NAZCA.mlt"},
    {402, "sound/B73_NAZCA.mlt"},     {300, "sound/B74_PARK.mlt"},       {302, "sound/B74_PARK.mlt"},
    {301, "sound/B75_JUNGLE.mlt"},    {404, "sound/B75_JUNGLE.mlt"},     {502, "sound/B75_JUNGLE.mlt"},
    {401, "sound/B76_ARK2.mlt"},      {504, "sound/B76_ARK2.mlt"},       {500, "sound/B77_ARK1.mlt"},
    {503, "sound/B77_ARK1.mlt"},      {603, "sound/B77_ARK1.mlt"},       {501, "sound/B78_BASE.mlt"},
    {600, "sound/B78_BASE.mlt"},      {602, "sound/B78_BASE.mlt"},       {601, "sound/B79_DOOM.mlt"},
    {604, "sound/B79_DOOM.mlt"},      {700, "sound/B79_DOOM.mlt"},       {210, "sound/B7B_BKBOSS.mlt"},
    {412, "sound/B7B_BKBOSS.mlt"},    {310, "sound/B7C_EGGBOSS.mlt"},    {411, "sound/B7C_EGGBOSS.mlt"},
    {511, "sound/B7C_EGGBOSS.mlt"},   {410, "sound/B7D_GUNBOSS.mlt"},    {510, "sound/B7D_GUNBOSS.mlt"},
    {611, "sound/B7E_LASTGUN.mlt"},   {613, "sound/B7E_LASTGUN.mlt"},    {618, "sound/B7E_LASTGUN.mlt"},
    {612, "sound/B7F_LASTEGG.mlt"},   {614, "sound/B7F_LASTEGG.mlt"},    {615, "sound/B7F_LASTEGG.mlt"},
    {610, "sound/B80_LASTBK.mlt"},    {616, "sound/B80_LASTBK.mlt"},     {617, "sound/B80_LASTBK.mlt"},
    {710, "sound/B81_FINALDOOM.mlt"}, {800, "sound/B78_BASE.mlt"},       {801, "sound/B78_BASE.mlt"},
    {802, "sound/B78_BASE.mlt"},      {-1, ""},
};

inline void UnkAudioState_801F1E24::Stop() {
    if (m_playing) {
        fn_8043F1FC();
        fn_8043F284(2);
        m_current = 0;
        m_previous = 0;
        m_playing = 0;
    }
}

AudioGCN::AudioGCN() {
    fn_80381B54(lbl_8053E1A8, 4);
    fn_80383684();
    fn_80381438(0);
    fn_80383850();
    fn_803893E8();
    fn_80385154(fn_8043E394);
    m_heap = fn_80049B90(0xE1000, 2);
    if (fn_8043FB24(m_heap, 0, 0) == 0) {
        for (s8 i = 0; i < 8; i++) {
            fn_8043F6AC(&m_groups[i]);
        }
        if (fn_80376F58() == 0) {
            vf_0x58(1);
        } else {
            vf_0x58(0);
        }
    }
    m_state.Start();
}

AudioGCN::~AudioGCN() {
    m_state.Stop();
    fn_8043EBFC();
    if (m_heap) {
        fn_80049B34(m_heap);
        m_heap = 0;
    }
    fn_803894B0();
    fn_803838C0();
}

bool AudioGCN::vf_0x18(const char *file) {
    if (!file) {
        return false;
    }
    s32 size;
    void *sound = 0;
    void *data = fn_8004509C(fn_800371D8(), file, &size);
    if (!data) {
        return false;
    }
    u32 error = fn_8043FA20(&sound);
    if (error == 0 && fn_8043F980(sound, data, size, -1, 0) == 0) {
        error = fn_8043F70C(sound);
    }
    if (sound) {
        fn_8043F8C4(sound);
    }
    fn_80049B34(data);
    if (error) {
        return false;
    }
    return true;
}

bool AudioGCN::vf_0x1C(int group, int arg2) {
    m_state.Stop();
    bool loaded = false;
    switch (group) {
    case 0:
        loaded = vf_0x18("sound/B0_SYSTEM.mlt");
        break;
    case 1:
        loaded = vf_0x18("sound/B1_COMMON.mlt");
        break;
    case 2:
        loaded = vf_0x18("sound/B2_WEAPONS.mlt");
        break;
    case 3:
        if (lbl_8057D6A0.m_voiceLanguage == 0) {
            loaded = vf_0x18("sound/B3_SHADOW_J.mlt");
        } else {
            loaded = vf_0x18("sound/B3_SHADOW_E.mlt");
        }
        break;
    case 4:
        if (lbl_8057D6A0.m_voiceLanguage == 0) {
            loaded = vf_0x18("sound/B4_SUBHERO_J.mlt");
        } else {
            loaded = vf_0x18("sound/B4_SUBHERO_E.mlt");
        }
        break;
    case 5:
        if (lbl_8057D6A0.m_voiceLanguage == 0) {
            loaded = vf_0x18("sound/B5_SUBDARK_J.mlt");
        } else {
            loaded = vf_0x18("sound/B5_SUBDARK_E.mlt");
        }
        break;
    case 6:
        loaded = vf_0x18("sound/B6_ENEMY.mlt");
        break;
    case 7:
        for (int i = 0; lbl_8053E1B8[i].m_stage != -1; i++) {
            if (arg2 == lbl_8053E1B8[i].m_stage) {
                loaded = vf_0x18(lbl_8053E1B8[i].m_file);
                break;
            }
        }
        break;
    }
    m_state.Start();
    return loaded;
}

bool AudioGCN::vf_0x20(int group) {
    m_state.Stop();
    return false;
}

void AudioGCN::vf_0x24(s8 bank, s8 program, u8 priority, s8 group, u8 voice, float range) {
    s8 level;
    if (priority == 3) {
        level = 0xF;
    } else if (priority == 2) {
        level = 2;
    } else {
        level = 1;
    }
    if (group >= 3) {
        fn_8043EFF0(m_groups[group], bank, program, level);
    } else {
        UnkAudioMessage_8043F41C message;
        fn_8043F41C(m_groups[group], &message, voice | 0x90, 0x30, 0x7F);
        fn_8043F35C(m_groups[group], &message, level);
    }
}

void AudioGCN::vf_0x28() {}

void AudioGCN::vf_0x2C() {}

void AudioGCN::vf_0x30(s8 group, u8 voice) {
    if (group >= 3) {
        fn_8043EF50(m_groups[group]);
    } else {
        UnkAudioMessage_8043F41C message;
        fn_8043F41C(m_groups[group], &message, voice | 0x80, 0x30, 0);
        fn_8043F35C(m_groups[group], &message, 0);
    }
}

void AudioGCN::vf_0x34() {
    m_state.Stop();
    fn_8043F2F0();
}

void AudioGCN::vf_0x38(s8 volume, s8 group, u8 voice) {
    if (group >= 3) {
        fn_8043ED7C(m_groups[group], volume, 0);
    } else {
        s16 level = volume;
        if (level > 0) {
            level = (s8)(0.2126f * volume);
        }
        level += 100;
        if (level < 1) {
            level = 1;
        }
        if (level > 0x7F) {
            level = 0x7F;
        }
        UnkAudioMessage_8043F41C message;
        fn_8043F41C(m_groups[group], &message, voice | 0xB0, 7, (s8)level);
        fn_8043F35C(m_groups[group], &message, 0);
    }
}

void AudioGCN::vf_0x3C(s8 pan, s8 group, u8 voice) {
    if (group >= 3) {
        fn_8043EF48(m_groups[group], pan, 0);
    } else {
        s16 level = (s8)(64.0f + 0.5f * pan);
        if (level > 0x7F) {
            level = 0x7F;
        } else if (level < 0) {
            level = 0;
        }
        UnkAudioMessage_8043F41C message;
        fn_8043F41C(m_groups[group], &message, voice | 0xB0, 0xA, (s8)level);
        fn_8043F35C(m_groups[group], &message, 0);
    }
}

void AudioGCN::vf_0x40(s8 value, s8 group, u8 voice) {
    if (group < 3) {
        s16 level = (s8)(64.0f + 0.5f * value);
        if (level > 0x7F) {
            level = 0x7F;
        } else if (level < 0) {
            level = 0;
        }
        UnkAudioMessage_8043F41C message;
        fn_8043F41C(m_groups[group], &message, voice | 0xB0, 0x3F, (s8)level);
        fn_8043F35C(m_groups[group], &message, 0);
    }
}

void AudioGCN::vf_0x44(s8 surround, s8 group, u8 voice) {
    if (group < 3) {
        UnkAudioMessage_8043F41C message;
        /* The cast keeps surround in r4 until the call, as retail does. */
        fn_8043F41C(m_groups[group], &message, voice | 0xB0, 0x3E, (s8)surround);
        fn_8043F35C(m_groups[group], &message, 0);
    }
}

void AudioGCN::vf_0x48(s8 group, u8 voice) {
    if (lbl_8057D6A0.m_soundMode == 2) {
        vf_0x44(0, group, voice);
        vf_0x3C(0, group, voice);
    } else {
        vf_0x3C(0, group, voice);
    }
}

void AudioGCN::vf_0x4C(s16 pitch, s8 group, u8 voice) {
    if (pitch > 0x3FFF) {
        pitch = 0x3FFF;
    } else if (pitch < -0x4000) {
        pitch = -0x4000;
    }
    if (group >= 3) {
        fn_8043EE74(m_groups[group], pitch, 0);
    } else {
        s16 value = pitch + 0x4000;
        UnkAudioMessage_8043F41C message;
        fn_8043F41C(m_groups[group], &message, voice | 0xE0, value & 0x7F, value >> 8);
        fn_8043F35C(m_groups[group], &message, 0);
    }
}

void AudioGCN::vf_0x5C(s8 group, u8 voice) {
    if (group >= 3) {
        fn_8043ECDC(m_groups[group]);
    } else {
        vf_0x38(-0x80, group, voice);
    }
}

void AudioGCN::vf_0x60() {}

void AudioGCN::vf_0x64(s8 volume, s8 group, u8 voice) {
    if (group >= 3) {
        fn_8043EC3C(m_groups[group], volume);
    } else {
        vf_0x38(volume, group, voice);
    }
}

void AudioGCN::vf_0x68() {}

void AudioGCN::vf_0x54() {
    fn_8043F0D4();
    for (s8 group = 0; group < 8; group++) {
        /* A u8 counter tested as an s8: retail passes it to vf_0x48 unconverted. */
        for (u8 voice = 0; (s8)voice < 16; voice++) {
            vf_0x48(group, voice);
            UnkAudioMessage_8043F41C message;
            fn_8043F41C(m_groups[group], &message, 0x79, 0);
            fn_8043F35C(m_groups[group], &message, 0);
        }
    }
}

void AudioGCN::vf_0x50(s8 group, u8 voice, s8 bank, s8 program) {
    UnkAudioMessage_8043F41C message;
    fn_8043F41C(m_groups[group], &message, (s8)voice | 0xB0, 0x20, bank);
    fn_8043F35C(m_groups[group], &message, 0);
    fn_8043F41C(m_groups[group], &message, (s8)voice | 0xC0, program);
    fn_8043F35C(m_groups[group], &message, 0);
}void AudioGCN::vf_0x58(int mode) {
    if (fn_80376F58() == 0) {
        mode = 1;
    }
    if (fn_80376F58() == 1 && mode == 1) {
        mode = 0;
    }
    switch (mode) {
    case 1:
        fn_803894B4(0);
        fn_8043FAB0(0);
        fn_8043E430(0);
        break;
    case 0:
        fn_803894B4(1);
        fn_8043FAB0(1);
        fn_8043E430(0);
        break;
    case 2:
        fn_803894B4(1);
        fn_8043FAB0(1);
        fn_8043E430(1);
        break;
    }
}

void UnkAudioState_801F1E24::Shift() {
    m_previous = m_current;
    m_current = 0;
}

void UnkAudioState_801F1E24::Start() {
    if (!m_playing) {
        fn_8043F240(0, 0);
        Shift();
        fn_8043F284(2);
        m_current = 0;
        m_previous = 0;
        m_playing = 1;
    }
}

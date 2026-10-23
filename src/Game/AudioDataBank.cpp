/* The sound banks of the sound manager (UnkSoundBank_801D8890, Game/SoundManager.h): eight banks
 * loaded from sound/AudioData*.bin (system, gadgets, weapons, the player and its hero and dark
 * variants, enemies, and the stage's own), and the lookup of a sound id's entry, which also
 * counts how many instances of the sound play. Class and function names are invented ("(name
 * inferred)"). Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: fn_801D852C (99.5%) swaps the registers of the entry pointer and the 16-bit id;
 * everything else matches. */
#include "Game/SoundManager.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The file system (class name unknown; see Game/HintController.cpp). */
class UnkFileSystem_800371D8;

extern "C" {
/* Loads a file and frees loaded data (behaviours inferred; see Game/HintController.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
void *fn_8004509C(UnkFileSystem_800371D8 *fileSystem, const char *path, u32 *size);
void fn_80049B34(void *data);
/* Builds and destroys a file's reader (see Game/HintController.cpp). */
UnkPath_8040E61C *fn_8040E61C(UnkPath_8040E61C *path, void *data);
void fn_8040E57C(UnkPath_8040E61C *path, int free);
}

/* The reader's view of a bank file (struct name unknown). */
struct UnkPath_8040E61C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ s32 *m_header; /* the entries, then their count (meaning inferred) */
    /* 0x18 */ u8 field_0x18[0x28 - 0x18];
};

/* A stage's bank file (struct name unknown). */
struct UnkStageBank {
    /* 0x0 */ s32 m_stage;
    /* 0x4 */ const char *m_file;
};

/* The stages' bank files, ended by -1 (name inferred). */
static UnkStageBank s_stageBanks[] = {
    {5, "sound/AudioDataStageCity.bin"},       {8, "sound/AudioDataStageCity.bin"},
    {12, "sound/AudioDataStageCity.bin"},      {7, "sound/AudioDataStageCanyon.bin"},
    {14, "sound/AudioDataStageCanyon.bin"},    {10, "sound/AudioDataStageJungle.bin"},
    {16, "sound/AudioDataStageJungle.bin"},    {19, "sound/AudioDataStageJungle.bin"},
    {9, "sound/AudioDataStagePark.bin"},       {11, "sound/AudioDataStagePark.bin"},
    {6, "sound/AudioDataStageElec.bin"},       {15, "sound/AudioDataStageElec.bin"},
    {13, "sound/AudioDataStageArk2.bin"},      {21, "sound/AudioDataStageArk2.bin"},
    {17, "sound/AudioDataStageArk1.bin"},      {20, "sound/AudioDataStageArk1.bin"},
    {25, "sound/AudioDataStageArk1.bin"},      {18, "sound/AudioDataStageBase.bin"},
    {22, "sound/AudioDataStageBase.bin"},      {24, "sound/AudioDataStageBase.bin"},
    {23, "sound/AudioDataStageDoom.bin"},      {26, "sound/AudioDataStageDoom.bin"},
    {27, "sound/AudioDataStageDoom.bin"},      {28, "sound/AudioDataStageBKBoss.bin"},
    {32, "sound/AudioDataStageBKBoss.bin"},    {30, "sound/AudioDataStageGunBoss.bin"},
    {33, "sound/AudioDataStageGunBoss.bin"},   {29, "sound/AudioDataStageEggBoss.bin"},
    {31, "sound/AudioDataStageEggBoss.bin"},   {34, "sound/AudioDataStageEggBoss.bin"},
    {35, "sound/AudioDataStageLastBKBoss.bin"}, {41, "sound/AudioDataStageLastBKBoss.bin"},
    {42, "sound/AudioDataStageLastBKBoss.bin"}, {36, "sound/AudioDataStageLastGunBoss.bin"},
    {38, "sound/AudioDataStageLastGunBoss.bin"}, {43, "sound/AudioDataStageLastGunBoss.bin"},
    {37, "sound/AudioDataStageLastEggBoss.bin"}, {39, "sound/AudioDataStageLastEggBoss.bin"},
    {40, "sound/AudioDataStageLastEggBoss.bin"}, {44, "sound/AudioDataStageFinalBoss.bin"},
    {45, "sound/AudioDataStageBase.bin"},      {46, "sound/AudioDataStageBase.bin"},
    {47, "sound/AudioDataStageBase.bin"},      {-1, ""},
};

/* Unloads bank group (name inferred). */
inline bool UnloadGroup(UnkSoundBank_801D8890 *bank, int group) {
    if (group < 0 || group >= 8) {
        return false;
    }
    UnkSoundBankGroup *g = &bank->m_groups[group];
    if (g->m_file != 0) {
        fn_8040E57C(g->m_reader, 1);
        g->m_reader = 0;
        fn_80049B34(g->m_file);
        g->m_file = 0;
        g->m_entries = 0;
        g->m_count = 0;
    }
    return true;
}

UnkSoundBank_801D8890::UnkSoundBank_801D8890() {}

UnkSoundBank_801D8890::~UnkSoundBank_801D8890() {}

bool UnkSoundBank_801D8890::LoadFile(const char *file, int group) {
    if (group < 0 || group >= 8) {
        return false;
    }
    UnkSoundBankGroup *g = &m_groups[group];
    if (g->m_file != 0) {
        UnloadGroup(this, group);
    }
    u32 size;
    void *data = fn_8004509C(fn_800371D8(), file, &size);
    if (data == 0) {
        return false;
    }
    g->m_file = data;
    UnkPath_8040E61C *reader = (UnkPath_8040E61C *)operator new(0x28);
    if (reader != 0) {
        reader = fn_8040E61C(reader, data);
    }
    g->m_reader = reader;
    s32 *header = g->m_reader->m_header;
    g->m_entries = (UnkSoundBankEntry *)header[0];
    g->m_count = header[1];
    return true;
}

bool UnkSoundBank_801D8890::fn_801D8680(int stage, int group) {
    switch (group) {
    case 0:
        return LoadFile("sound/AudioDataSystem.bin", group);
    case 1:
        return LoadFile("sound/AudioDataGadget.bin", group);
    case 2:
        return LoadFile("sound/AudioDataWeapon.bin", group);
    case 3:
        return LoadFile("sound/AudioDataPlayer.bin", group);
    case 4:
        return LoadFile("sound/AudioDataPlayerHero.bin", group);
    case 5:
        return LoadFile("sound/AudioDataPlayerDark.bin", group);
    case 6:
        return LoadFile("sound/AudioDataEnemy.bin", group);
    case 7:
        s32 entry;
        for (int i = 0; (entry = s_stageBanks[i].m_stage) != -1; i++) {
            if (stage == entry) {
                return LoadFile(s_stageBanks[i].m_file, group);
            }
        }
        break;
    }
    return false;
}

bool UnkSoundBank_801D8890::fn_801D8600(int group) {
    return UnloadGroup(this, group);
}

UnkSoundBankEntry *UnkSoundBank_801D8890::fn_801D852C(int id) {
    UnkSoundBankGroup *group = GetGroup(id);
    if (group == 0) {
        return 0;
    }
    s32 key = (u16)id;
    UnkSoundBankEntry *entry;
    UnkSoundBankEntry *found = 0;
    for (int i = 0; i < group->m_count; i++) {
        entry = &group->m_entries[i];
        if ((s32)entry->m_id == key) {
            found = entry;
            break;
        }
    }
    if (found == 0) {
        return 0;
    }
    if (found->field_0x9 < 0) {
        return 0;
    }
    if (found->m_maxPlaying != 0) {
        if (found->m_playing < found->m_maxPlaying) {
            found->m_playing++;
            return found;
        }
        return 0;
    }
    return found;
}

UnkSoundBankGroup *UnkSoundBank_801D8890::GetGroup(int id) {
    int group;
    switch ((id >> 12) & 0xF) {
    case 0xE:
        group = 0;
        break;
    case 1:
        group = 1;
        break;
    case 2:
        group = 2;
        break;
    case 3:
        group = 3;
        break;
    case 4:
        group = 4;
        break;
    case 5:
        group = 5;
        break;
    case 6:
        group = 6;
        break;
    case 7:
        group = 7;
        break;
    case 8:
        group = 7;
        break;
    case 9:
        group = 7;
        break;
    default:
        group = -1;
        break;
    }
    if (group < 0) {
        return 0;
    }
    return &m_groups[group];
}

void UnkSoundBank_801D8890::fn_801D844C() {
    for (int i = 0; i < 8; i++) {
        UnkSoundBankGroup *group = &m_groups[i];
        for (int j = 0; j < group->m_count; j++) {
            group->m_entries[j].m_playing = 0;
        }
    }
}

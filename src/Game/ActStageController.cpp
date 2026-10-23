/* AudioController::ActStageController (class name from RTTI; its task name is "ActStageController"):
 * the task that plays a stage's music. Its constructor picks the song of the stage (a table of
 * stages that share songs, otherwise "sng_stg<id>.adx") and clears it when the file is missing;
 * Tick starts it once the stage is running, unless something holds the music off. The unit also
 * holds the jingles (fn_801D3838 stops one, fn_801D3888 plays one) and the hold flags. Behaviours
 * are inferred; names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <stdio.h>
#include <string.h>

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 m_language; /* 0 picks the Japanese jingles (meaning inferred) */
    /* 0x044 */ u8 field_0x44[0xC0 - 0x44];
    /* 0x0C0 */ u8 field_0xC0; /* set: no stage music (meaning unknown) */
    /* 0x0C1 */ u8 field_0xC1[0x150 - 0xC1];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The file system fn_800371D8 returns (class name unknown; see Game/CsdOneShot.cpp). Only the
 * existence test is used here, called with the path alone. */
class UnkFileSystem_800371D8 {
  public:
    /* 0x0 */ u32 field_0x0;
    /* Whether a file exists (name inferred). */
    bool Exists(const char *path) {
        return exists(path);
    }

    /* 0x4 */ bool (*exists)(const char *path);
};

class UnkObject_800B74B4;

extern "C" {
/* A 32-bit flag word: fn_80014E04 clears it, fn_80014E10 clears it too, fn_80014E1C sets a bit,
 * fn_8006127C clears one and fn_80014E34 tests one (see Game/EggLastMech.h). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The sound manager: stopping and playing a jingle, playing a song (behaviours inferred). */
void *fn_80053164(void);
void fn_801D5118(void *sound);
void fn_801D5138(void *sound, const char *name);
void fn_801D524C(void *sound, const char *name);
/* The file system (see Game/CsdOneShot.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
/* The stage table and the index of a stage's entry (see Game/StageTable.h). */
UnkObject_800B74B4 *fn_800B74B4(void);
int fn_80176C04(UnkObject_800B74B4 *object, int id);
/* Whether the stage is running (behaviour inferred; see Game/StageManager.h). */
u8 fn_80177CF8(void);
}

/* The flag word behind the hold flags (struct name unknown). */
struct UnkAudioFlags_805EF960 {
    UnkAudioFlags_805EF960() {
        fn_80014E04(&m_bits);
    }

    u32 m_bits;
};

namespace AudioController {

class ActStageController : public Task {
  public:
    ActStageController(Task *parent, s32 stageId);
    virtual ~ActStageController();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(ActStageController, 0x28);

} // namespace AudioController

using namespace AudioController;

/* The jingles, Japanese and English (name inferred). */
static const char *s_jingles[6][2] = {
    {"sng_jin_cblast_j.adx", "sng_jin_cblast_e.adx"},
    {"sng_jin_cctrl_j.adx", "sng_jin_cctrl_e.adx"},
    {"sng_jin_ccslow_j.adx", "sng_jin_ccslow_e.adx"},
    {"sng_jin_awakedark.adx", "sng_jin_awakedark.adx"},
    {"sng_jin_awakehero.adx", "sng_jin_awakehero.adx"},
    {"sng_jin_invincible.adx", "sng_jin_invincible.adx"},
};

/* A stage whose song is not its own (struct name unknown). */
struct UnkStageSong {
    /* 0x0 */ s32 m_stage;
    /* 0x4 */ const char *m_song;
};

/* The stages that share a song, ended by -1 (name inferred). */
static UnkStageSong s_stageSongs[] = {
    {0x20, "sng_stg0210.adx"}, {0x1F, "sng_stg0310.adx"}, {0x22, "sng_stg0310.adx"},
    {0x29, "sng_stg0610.adx"}, {0x2A, "sng_stg0610.adx"}, {0x26, "sng_stg0611.adx"},
    {0x2B, "sng_stg0611.adx"}, {0x27, "sng_stg0612.adx"}, {0x28, "sng_stg0612.adx"},
    {0x2D, "sng_stg0800.adx"}, {0x2E, "sng_stg0800.adx"}, {0x2F, "sng_stg0800.adx"},
    {-1, ""},
};

const char *ActStageController::s_taskName = "ActStageController";

/* The stage, whether its song started, the hold flags (bit 0 holds the music off, bit 1 is held
 * by a jingle) and the song (names inferred). */
static s32 s_stageId;
static u8 s_playing;
static UnkAudioFlags_805EF960 s_flags;
static char s_song[0x40];

ActStageController::ActStageController(Task *parent, s32 stageId) : Task(parent) {
    m_name = s_taskName;
    s_stageId = stageId;
    s_playing = 0;
    fn_80014E10(&s_flags.m_bits);
    int stage = fn_80176C04(fn_800B74B4(), s_stageId);
    const char *song = 0;
    s32 entry;
    for (int i = 0; (entry = s_stageSongs[i].m_stage) != -1; i++) {
        if (stage == entry) {
            song = s_stageSongs[i].m_song;
            break;
        }
    }
    if (song != 0) {
        strcpy(s_song, song);
    } else {
        sprintf(s_song, "sng_stg%04d.adx", s_stageId);
    }
    if (!fn_800371D8()->Exists(s_song)) {
        strcpy(s_song, "");
    }
}

ActStageController::~ActStageController() {
    s_stageId = 0;
    s_playing = 0;
    strcpy(s_song, "");
    fn_80014E10(&s_flags.m_bits);
}

void ActStageController::Tick(float dt) {
    if (lbl_8057D6A0.field_0xC0 == 0 && !fn_80014E34(&s_flags.m_bits, 1) && !s_playing &&
        strcmp(s_song, "") != 0 && fn_80177CF8()) {
        if (fn_80014E34(&s_flags.m_bits, 0)) {
            fn_801D524C(fn_80053164(), s_song);
        } else {
            fn_801D524C(fn_80053164(), s_song);
        }
        s_playing = 1;
        fn_8006127C(&s_flags.m_bits, 0);
    }
}

/* Starts the stage music task (name inferred). */
extern "C" void fn_801D3980(Task *parent, s32 stageId) {
    new ActStageController(parent, stageId);
}

/* Lets the stage music start again (name inferred). */
extern "C" void fn_801D3950(void) {
    s_playing = 0;
    fn_80014E1C(&s_flags.m_bits, 0);
}

/* Holds the stage music off (name inferred). */
extern "C" void fn_801D3928(void) {
    fn_80014E1C(&s_flags.m_bits, 1);
}

/* Plays jingle (name inferred). */
extern "C" void fn_801D3888(int jingle) {
    if (jingle < 0 || jingle >= 6) {
        return;
    }
    const char *name;
    if (lbl_8057D6A0.m_language == 0) {
        name = s_jingles[jingle][0];
    } else {
        name = s_jingles[jingle][1];
    }
    if (jingle == 3 || jingle == 4) {
        fn_801D524C(fn_80053164(), name);
    } else {
        fn_801D5138(fn_80053164(), name);
    }
}

/* Stops jingle (name inferred). */
extern "C" void fn_801D3838(int jingle) {
    if (jingle < 0 || jingle >= 6) {
        return;
    }
    if (jingle == 3 || jingle == 4) {
        fn_801D3950();
    } else {
        fn_801D5118(fn_80053164());
    }
}

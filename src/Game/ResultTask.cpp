/* ResultTask (class name from RTTI): the stage result screen. It loads the CSD project
 * "CsdFiles/Result<language>", plays the scene Main's opening motion for the kind of result (hero,
 * dark, normal or boss), adds the score lines one after another with a TGiNumberDisplay (the hero
 * and dark points, the score, the time, the rings, the time bonus and the total), then shows the
 * rank from the stage table's thresholds in the scene Rank. fn_80202688 creates the task and
 * fn_8020261C tells whether it has ended. The file name and the names marked "(name inferred)" are
 * not original; the functions other units call keep their fn_ names. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <cstdio>

#include "Game/CsdRCPtr.h"
#include "Game/CsdSceneCtrl.h"
#include "Game/Render.h"
#include "Game/TGi.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Chao::CSD::CNode;
using Chao::CSD::CScene;
using Chao::CSD::RCPtr;

/* A time split into minutes, seconds and hundredths (class name unknown; as in
 * Game/TGiScoreDisplay.cpp, whose copy of this declaration it is). */
class UnkTime_801E46E4 {
  public:
    UnkTime_801E46E4();
    UnkTime_801E46E4(float time);

    /* 0x0 */ s32 m_minutes;
    /* 0x4 */ s8 m_seconds;
    /* 0x5 */ s8 m_hundredths;
    /* 0x8 */ float m_time;
};
SIZE_ASSERT(UnkTime_801E46E4, 0xC);

/* One stage's entry in the stage table fn_800CBAA4 returns (struct name unknown; only the rank
 * thresholds are declared here: per kind of result, the score or time each rank needs). */
struct UnkStageInfo_800CBAA4 {
    /* 0x00 */ u8 field_0x0[0x60];
    /* 0x60 */ s32 m_rankThresholds[3][7];
    /* 0xB4 */ u8 field_0xB4[0xC0 - 0xB4];
};
SIZE_ASSERT(UnkStageInfo_800CBAA4, 0xC0);

struct UnkStageTable_800CBAA4 {
    /* 0x00 */ UnkStageInfo_800CBAA4 m_stages[1];
};

/* What the file system's load request takes (struct name unknown; as in Game/FogTable.h). */
struct ResultRequest {
    /* 0x00 */ const char *m_path;
    /* 0x04 */ void *m_data;
    /* 0x08 */ u32 m_size;
    /* 0x0C */ void *m_user;
    /* 0x10 */ void (*m_callback)(ResultRequest *request);
};

class UnkFileSystem_800371D8;

extern "C" {
/* The file system, a load request to it, and the release of loaded data (as in Game/FogTable.cpp). */
UnkFileSystem_800371D8 *fn_800371D8(void);
bool fn_80045044(UnkFileSystem_800371D8 *fileSystem, ResultRequest *request, int arg2);
void fn_80049B34(void *data);
/* The language suffixes "_JP", "_EN", ... (indexed by UnkGameState_8057D6A0::m_language). */
extern const char *lbl_8051D528[];
/* Which of an entry's two layouts the screen uses (meaning unknown). */
extern s32 lbl_805EE690;
/* Loads a project from data in memory (behaviour inferred). */
void fn_8036DB70(Chao::CSD::CProject *project, void *data, u32 size, int arg3);
/* A scene of the object a project handle holds, by name (behaviour inferred). */
RCPtr<CScene> fn_8036F470(void *object, const char *name);
/* Assigns a project handle (behaviour inferred). */
void fn_80042A90(UnkCsdHandle_80042DF0 *handle, const UnkCsdHandle_80042DF0 *other);
/* The stage table (see Game/CenFile.cpp). */
UnkStageTable_800CBAA4 *fn_800CBAA4(void);
/* The ring count of a player's record (see Game/TGi.h). */
int fn_8016F280(u8 *rings);
/* Called before the result task is created (behaviour unknown). */
void fn_80177FA4(void);
/* Whether a button was pressed on a pad (as in Game/SavePointSelect.cpp). */
bool fn_80356260(int button, int pad);
/* The sound manager, playing a sound and a voice (as in Game/GadgetUtil.h and
 * Game/MessageTask.cpp). */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
void fn_801D51A0(void *manager, s32 voice, int arg2, int arg3);
}

/* Where and how one line of the screen is drawn (struct name inferred). */
struct ResultLayout {
    /* 0x00 */ s32 m_x;
    /* 0x04 */ s32 m_y;
    /* 0x08 */ float m_scaleX;
    /* 0x0C */ float m_scaleY;
    /* 0x10 */ TGiColorSet m_colors;
    /* 0x50 */ float m_spacing;
    /* 0x54 */ float m_field14; /* passed to TGiNumberDisplay::SetField14 (meaning unknown) */
    /* 0x58 */ s32 m_align;
};
SIZE_ASSERT(ResultLayout, 0x5C);

/* One line of the screen (struct name inferred): what it shows (-1 ends the list), how many frames
 * it waits, a label and two layouts (see lbl_805EE690). */
struct ResultEntry {
    /* 0x00 */ s32 m_kind;
    /* 0x04 */ s32 m_frames;
    /* 0x08 */ const char *m_label;
    /* 0x0C */ ResultLayout m_layouts[2];
};
SIZE_ASSERT(ResultEntry, 0xC4);

static ResultEntry s_heroTable[] = {
    {0, 20, "Hero Score", {
        {252, 158, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 158, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {1, 20, "Dark Score", {
        {252, 192, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 192, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {2, 20, "Normal Score", {
        {252, 226, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 226, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {3, 20, "Time", {
        {252, 273, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 273, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {4, 20, "Ring", {
        {318, 294, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {318, 294, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {5, 20, "TimeBonus", {
        {252, 316, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 316, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {6, 20, "Total", {
        {252, 365, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 365, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {-1, 120, "lol", {
        {82, 413, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {82, 413, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
};
static ResultEntry s_darkTable[] = {
    {1, 20, "Dark Score", {
        {252, 158, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 158, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {0, 20, "Hero Score", {
        {252, 192, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 192, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {2, 20, "Normal Score", {
        {252, 226, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 226, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {3, 20, "Time", {
        {252, 273, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 273, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {4, 20, "Ring", {
        {318, 294, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {318, 294, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {5, 20, "TimeBonus", {
        {252, 316, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 316, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {6, 20, "Total", {
        {252, 365, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 365, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {-1, 120, "lol", {
        {82, 413, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {82, 413, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
};
static ResultEntry s_normalTable[] = {
    {2, 20, "Normal Score", {
        {252, 158, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 158, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {3, 20, "Time", {
        {252, 273, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 273, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {4, 20, "Ring", {
        {318, 294, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {318, 294, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {5, 20, "TimeBonus", {
        {252, 316, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 316, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {6, 20, "Total", {
        {252, 365, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 365, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {-1, 120, "lol", {
        {82, 413, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {82, 413, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
};
static ResultEntry s_bossTable[] = {
    {3, 20, "Time", {
        {252, 192, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {252, 192, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
    {-1, 120, "lol", {
        {82, 413, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
        {82, 413, 1.0f, 1.0f, {{{255, 255, 255, 255}, {128, 128, 128, 255}, {64, 64, 64, 255}, {255, 255, 255, 255}}}, 0.0f, -2.7f, 0},
    }},
};

/* The lines of each kind of result (indexed by ResultTask::m_kind). */
static ResultEntry *s_tables[] = {
    s_heroTable,
    s_darkTable,
    s_normalTable,
    s_bossTable,
};

/* The scene Main's opening motion of each kind of result. */
static const char *s_openMotions[] = {
    "HeroOpen",
    "DarkOpen",
    "NormalOpen",
    "BossOpen",
};

class ResultTask : public Task {
  public:
    enum State {
        STATE_LOADING = 0,
        STATE_OPENING = 1,
        STATE_COUNTING = 2,
        STATE_RANK = 3,
        STATE_WAITING = 4,
        STATE_DONE = 5,
        STATE_FAILED = 7,
    };

    ResultTask(Task *parent, int kind);
    virtual ~ResultTask();
    virtual void Tick(float dt);

    /* Starts loading the project and makes the number display (name inferred). */
    bool Init();
    /* The load request's callback (name inferred). */
    static void OnLoaded(ResultRequest *request);
    /* Draws the lines shown so far (name inferred). */
    void DrawLines();
    /* Shows the next line when its time has come; returns whether the list has ended (name
     * inferred). */
    bool Advance(float dt);
    /* Shows the rank (name inferred). */
    void ShowRank();
    /* Computes the time bonus and the total (name inferred). */
    void ComputeTotal();

    /* 0x28 */ Chao::CSD::CProject m_project;
    /* 0x74 */ UnkCsdSceneCtrl_8003DD20 m_main;
    /* 0x88 */ UnkCsdSceneCtrl_8003DD20 m_rankScene;
    /* 0x9C */ s32 m_kind;
    /* 0xA0 */ s32 m_state;
    /* 0xA4 */ u32 field_0xA4;
    /* 0xA8 */ ResultEntry *m_entries;
    /* 0xAC */ s32 m_shown;
    /* 0xB0 */ float m_timer;
    /* 0xB4 */ float m_soundTimer;
    /* 0xB8 */ float m_voiceTimer;
    /* 0xBC */ s32 m_timeBonus;
    /* 0xC0 */ boost::shared_ptr<TGiNumberDisplay> m_display;
};
SIZE_ASSERT(ResultTask, 0xC8);

static ResultRequest s_request;

/* RCPtr comparison (name inferred): equal when both hold the same object, or both hold none. It
 * returns int: retail tests the stored result as a word, not as a bool. */
template <class T>
inline int operator==(const RCPtr<T> &a, const RCPtr<T> &b) {
    if (a.m_object != 0 && b.m_object != 0) {
        return a.m_object->m_object == b.m_object->m_object;
    }
    return a.m_object == b.m_object;
}

/* Handle comparison (name inferred). */
inline bool operator!=(const UnkCsdHandle_80042DF0 &a, const UnkCsdHandle_80042DF0 &b) {
    void *pa = a.m_object != 0 ? *(void **)a.m_object : 0;
    void *pb = b.m_object != 0 ? *(void **)b.m_object : 0;
    if (pa != pb) {
        return true;
    }
    return false;
}

/* Stands in for the handle's inline assignment (fn_80042A90, out of line in retail): calling an inline
 * function keeps the copy constructor below out of line, as retail has it at 0x802032F0. */
inline void AssignHandle(UnkCsdHandle_80042DF0 *handle, const UnkCsdHandle_80042DF0 &other) {
    fn_80042A90(handle, &other);
}

inline UnkCsdHandle_80042DF0::UnkCsdHandle_80042DF0(const UnkCsdHandle_80042DF0 &other) : m_object(0) {
    AssignHandle(this, other);
}

ResultTask::ResultTask(Task *parent, int kind) : Task(parent), m_kind(kind) {
    m_name = "ResultTask";
    Init();
    m_soundTimer = 1.0833334f;
    m_voiceTimer = 1.0f;
}

ResultTask::~ResultTask() {}

void ResultTask::Tick(float dt) {
    if (m_state != STATE_LOADING) {
        u8 mainEnded = m_main.Update(false);
        u8 rankEnded = m_rankScene.Update(false);
        bool ended = mainEnded + rankEnded != 0;
        if (lbl_8057D6A0.field_0x149) {
            fn_8036D94C(&m_project);
        }
        switch (m_state) {
        case STATE_OPENING:
            if (!ended) {
                m_entries = s_tables[m_kind];
                ComputeTotal();
                m_state = STATE_COUNTING;
            }
            break;
        case STATE_COUNTING:
            DrawLines();
            if (Advance(dt)) {
                ShowRank();
                m_state = STATE_RANK;
            }
            break;
        case STATE_RANK:
            DrawLines();
            m_timer -= dt;
            m_soundTimer -= dt;
            if (m_soundTimer <= 0.0f) {
                fn_801D560C(fn_80053164(), 0xE009, 0, 0);
                m_soundTimer = 10000.0f;
            }
            if (m_timer <= 0.0f) {
                m_state = STATE_WAITING;
                m_timer = 4.0f;
            }
            break;
        case STATE_WAITING:
            m_timer -= dt;
            if (m_timer <= 0.0f || fn_80356260(1, 4)) {
                m_state = STATE_DONE;
            }
            if (m_voiceTimer < 0.0f) {
                if (lbl_8057D6A0.m_rank == 0) {
                    m_voiceTimer = 10000.0f;
                    fn_801D51A0(fn_80053164(), 0x24E, -1, -1);
                }
            } else {
                m_voiceTimer -= dt;
            }
            DrawLines();
            break;
        case 5:
        case 6:
            DrawLines();
            break;
        case STATE_FAILED:
            break;
        }
    }
}

/* NonMatching (96.9%): retail keeps the game state's address in r6 and the format string in r4;
 * GC/2.7 swaps the two registers here. About ten source shapes were tried. */
bool ResultTask::Init() {
    char path[64];
    sprintf(path, "CsdFiles/Result%s", lbl_8051D528[lbl_8057D6A0.m_language]);
    s_request.m_path = fn_800423B8(fn_8003F8D4(), path, 0);
    s_request.m_data = 0;
    s_request.m_size = 0;
    s_request.m_user = this;
    s_request.m_callback = OnLoaded;
    fn_80045044(fn_800371D8(), &s_request, 1);
    m_display = boost::shared_ptr<TGiNumberDisplay>(new TGiNumberDisplay);
    fn_800BCEA8(m_display.get(), lbl_805EE68C, "ad_hpasc1");
    fn_800BCC54(m_display.get(), 64, 64, 16, 18, 4);
    m_state = STATE_LOADING;
    return true;
}

void ResultTask::OnLoaded(ResultRequest *request) {
    ResultTask *task = (ResultTask *)request->m_user;
    if (request->m_data == 0) {
        task->m_state = STATE_FAILED;
        return;
    }
    fn_8036DB70(&task->m_project, request->m_data, request->m_size, 0);
    fn_80049B34(request->m_data);
    UnkCsdHandle_80042DF0 handle = fn_8036D598(&task->m_project, 0);
    if (handle != UnkCsdHandle_80042DF0(0)) {
        task->m_main.SetScene(fn_8036F470(handle.Get(), "Main"));
        task->m_rankScene.SetScene(fn_8036F470(handle.Get(), "Rank"));
        task->m_main.Reset();
        task->m_rankScene.Reset();
        task->m_main.Play(s_openMotions[task->m_kind], 1);
        task->m_state = STATE_OPENING;
        task->m_timer = 1.6666666f;
        task->m_shown = 0;
        return;
    }
    task->m_state = STATE_FAILED;
}

void ResultTask::DrawLines() {
    char text[36];
    for (int i = 0; i < m_shown; i++) {
        ResultLayout *layout = &m_entries[i].m_layouts[lbl_805EE690];
        switch (m_entries[i].m_kind) {
        case 0:
            fn_800BCB7C(m_display.get(), &layout->m_colors);
            fn_800BCB2C(m_display.get(), layout->m_spacing);
            m_display->SetScale(layout->m_scaleX, layout->m_scaleY);
            m_display->SetField14(layout->m_field14);
            sprintf(text, "%8d", fn_80178B5C(fn_80079808(), 0)->m_heroPoints);
            fn_800BC7BC(m_display.get(), text, layout->m_x, layout->m_align, layout->m_y);
            break;
        case 1:
            fn_800BCB7C(m_display.get(), &layout->m_colors);
            fn_800BCB2C(m_display.get(), layout->m_spacing);
            m_display->SetScale(layout->m_scaleX, layout->m_scaleY);
            m_display->SetField14(layout->m_field14);
            sprintf(text, "%8d", fn_80178B5C(fn_80079808(), 0)->m_darkPoints);
            fn_800BC7BC(m_display.get(), text, layout->m_x, layout->m_align, layout->m_y);
            break;
        case 2:
            fn_800BCB7C(m_display.get(), &layout->m_colors);
            fn_800BCB2C(m_display.get(), layout->m_spacing);
            m_display->SetScale(layout->m_scaleX, layout->m_scaleY);
            m_display->SetField14(layout->m_field14);
            sprintf(text, "%8d", fn_80178B5C(fn_80079808(), 0)->m_score);
            fn_800BC7BC(m_display.get(), text, layout->m_x, layout->m_align, layout->m_y);
            break;
        case 3: {
            fn_800BCB7C(m_display.get(), &layout->m_colors);
            fn_800BCB2C(m_display.get(), layout->m_spacing);
            m_display->SetScale(layout->m_scaleX, layout->m_scaleY);
            m_display->SetField14(layout->m_field14);
            UnkTime_801E46E4 time(lbl_8057D6A0.m_time);
            if (time.m_minutes >= 100) {
                sprintf(text, "99:59:99");
            } else {
                sprintf(text, "%02d:%02d:%02d", time.m_minutes, time.m_seconds, time.m_hundredths);
            }
            fn_800BC7BC(m_display.get(), text, layout->m_x, layout->m_align, layout->m_y);
            break;
        }
        case 4:
            fn_800BCB7C(m_display.get(), &layout->m_colors);
            fn_800BCB2C(m_display.get(), layout->m_spacing);
            m_display->SetScale(layout->m_scaleX, layout->m_scaleY);
            m_display->SetField14(layout->m_field14);
            sprintf(text, "%3d", fn_8016F280(fn_80178B5C(fn_80079808(), 0)->m_rings));
            fn_800BC7BC(m_display.get(), text, layout->m_x, layout->m_align, layout->m_y);
            break;
        case 5:
            fn_800BCB7C(m_display.get(), &layout->m_colors);
            fn_800BCB2C(m_display.get(), layout->m_spacing);
            m_display->SetScale(layout->m_scaleX, layout->m_scaleY);
            m_display->SetField14(layout->m_field14);
            sprintf(text, "%8d", m_timeBonus);
            fn_800BC7BC(m_display.get(), text, layout->m_x, layout->m_align, layout->m_y);
            break;
        case 6:
            fn_800BCB7C(m_display.get(), &layout->m_colors);
            fn_800BCB2C(m_display.get(), layout->m_spacing);
            m_display->SetScale(layout->m_scaleX, layout->m_scaleY);
            m_display->SetField14(layout->m_field14);
            sprintf(text, "%8d", lbl_8057D6A0.m_total);
            fn_800BC7BC(m_display.get(), text, layout->m_x, layout->m_align, layout->m_y);
            break;
        }
    }
}

bool ResultTask::Advance(float dt) {
    m_timer -= dt;
    if (m_timer <= 0.0f) {
        m_shown++;
        m_timer = m_entries[m_shown - 1].m_frames / 60.0f;
        if (m_entries[m_shown - 1].m_kind == -1) {
            return true;
        }
        fn_801D560C(fn_80053164(), 0xE007, 0, 0);
    }
    return false;
}

/* NonMatching (98.4%): retail keeps type in r29 for the time-based rank (type = 1) and multiplies
 * it; GC/2.7 folds the constant into the offset (+124). About fifteen source shapes were tried. */
void ResultTask::ShowRank() {
    RCPtr<CNode> node = fn_803666B4(m_rankScene.GetScene().Get(), "rank_alphabet");
    if (node == RCPtr<CNode>(0)) {
        m_state = STATE_FAILED;
        return;
    }
    int rank;
    for (rank = 0; rank < 4; rank++) {
        int type;
        if (lbl_8057D6A0.field_0xB1) {
            type = 1;
            if (lbl_8057D6A0.m_total <= fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].m_rankThresholds[type][rank]) {
                break;
            }
        } else {
            switch (m_kind) {
            case 1:
                type = 0;
                break;
            case 0:
                type = 2;
                break;
            case 2:
                type = 1;
                break;
            }
            if (lbl_8057D6A0.m_total >= fn_800CBAA4()->m_stages[lbl_8057D6A0.m_stage].m_rankThresholds[type][rank]) {
                break;
            }
        }
    }
    fn_8036AD14(node.Get(), rank);
    m_rankScene.Play("RankOpen", 1);
    lbl_8057D6A0.m_rank = rank;
}

void ResultTask::ComputeTotal() {
    void *save = fn_80079808();
    lbl_8057D6A0.m_total = 0;
    m_timeBonus = 0;
    UnkTime_801E46E4 time(lbl_8057D6A0.m_time);
    int seconds = time.m_minutes * 60 + time.m_seconds;
    if (seconds < 90) {
        m_timeBonus = 50000;
    } else {
        m_timeBonus = 50000 - seconds * 80;
        if (m_timeBonus < 0) {
            m_timeBonus = 0;
        }
    }
    switch (m_kind) {
    case 0:
        lbl_8057D6A0.m_total += fn_80178B5C(save, 0)->m_heroPoints;
        lbl_8057D6A0.m_total -= fn_80178B5C(save, 0)->m_darkPoints;
        lbl_8057D6A0.m_total += fn_80178B5C(save, 0)->m_score;
        lbl_8057D6A0.m_total += m_timeBonus;
        break;
    case 1:
        lbl_8057D6A0.m_total -= fn_80178B5C(save, 0)->m_heroPoints;
        lbl_8057D6A0.m_total += fn_80178B5C(save, 0)->m_darkPoints;
        lbl_8057D6A0.m_total += fn_80178B5C(save, 0)->m_score;
        lbl_8057D6A0.m_total += m_timeBonus;
        break;
    case 2:
        lbl_8057D6A0.m_total += fn_80178B5C(save, 0)->m_score;
        lbl_8057D6A0.m_total += m_timeBonus;
        break;
    case 3:
        m_timeBonus = 0;
        lbl_8057D6A0.m_total = seconds;
        break;
    }
    if (lbl_8057D6A0.m_total < 0) {
        lbl_8057D6A0.m_total = 0;
    }
}

/* Creates the result screen of kind (0 hero, 1 dark, 2 normal, 3 boss) in task (name unknown). */
extern "C" bool fn_80202688(Task **task, int kind) {
    fn_80177FA4();
    *task = new ResultTask((Task *)fn_80046178()->field_0x38, kind);
    return true;
}

/* Whether the result screen in task has ended (name unknown). */
extern "C" bool fn_8020261C(Task **task) {
    if (*task != 0) {
        ResultTask *result = dynamic_cast<ResultTask *>(*task);
        if (result != 0) {
            if (result->m_state == ResultTask::STATE_DONE || result->m_state == ResultTask::STATE_FAILED) {
                return true;
            }
            return false;
        }
    }
    return true;
}

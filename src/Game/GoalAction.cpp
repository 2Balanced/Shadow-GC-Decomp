/* StageAction::GoalAction (class name from RTTI): the stage action that ends a stage at a goal. It
 * finds which mission was cleared, fades out, stops the stage and starts the clear event; then it
 * plays the clear voice and jingle, shows the result screen (or, in the stage-select modes, goes
 * straight on), records the result in the save data and asks for the next state (10, saving, or
 * 16, the end). File name inferred. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/Mission.h"
#include "Game/ScreenFade.h"
#include "Game/StageAction.h"
#include "Game/StageTable.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The current stage's record in the game state (struct name unknown; see Game/StageRecord.cpp). */
struct UnkStageRecord_8057D748 {
    /* 0x00 */ s32 m_id;
    /* 0x04 */ s32 m_index;
    /* 0x08 */ u8 field_0x8;
    /* 0x09 */ u8 field_0x9;
    /* 0x0A */ u8 m_flag4;
    /* 0x0B */ u8 field_0xB[0xD - 0xB];
    /* 0x0D */ u8 field_0xD;
    /* 0x0E */ u8 field_0xE[0x10 - 0xE];
    /* 0x10 */ s32 m_total;  /* the result's total (meaning inferred; ResultTask) */
    /* 0x14 */ s32 m_rank;   /* the result's rank (meaning inferred; ResultTask) */
    /* 0x18 */ u8 field_0x18[0x1C - 0x18];
    /* 0x1C */ s32 m_result; /* the mission cleared (meaning inferred) */
    /* 0x20 */ u8 field_0x20[0x24 - 0x20];
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xC];
    /* 0x00C */ s32 m_frameStep; /* frames since the last tick (meaning inferred) */
    /* 0x010 */ u8 field_0x10[0x38 - 0x10];
    /* 0x038 */ s32 m_mode; /* meaning inferred */
    /* 0x03C */ u8 field_0x3C[0x60 - 0x3C];
    /* 0x060 */ s32 field_0x60; /* meaning unknown */
    /* 0x064 */ u8 field_0x64[0x8C - 0x64];
    /* 0x08C */ s32 field_0x8C; /* meaning unknown */
    /* 0x090 */ u8 field_0x90[0x94 - 0x90];
    /* 0x094 */ float m_time; /* meaning inferred */
    /* 0x098 */ u8 field_0x98[0xA8 - 0x98];
    /* 0x0A8 */ UnkStageRecord_8057D748 m_stageRecord;
    /* 0x0CC */ s32 field_0xCC; /* meaning unknown */
    /* 0x0D0 */ u8 field_0xD0;
    /* 0x0D1 */ u8 field_0xD1; /* meaning unknown */
    /* 0x0D2 */ u8 field_0xD2[0xD4 - 0xD2];
    /* 0x0D4 */ s32 field_0xD4[20]; /* meaning unknown */
    /* 0x124 */ u8 field_0x124;
    /* 0x125 */ u8 field_0x125; /* meaning unknown */
    /* 0x126 */ u8 field_0x126[0x128 - 0x126];
    /* 0x128 */ s32 field_0x128; /* meaning unknown */
    /* 0x12C */ u8 field_0x12C[0x148 - 0x12C];
    /* 0x148 */ u8 m_hudVisible; /* meaning inferred */
    /* 0x149 */ u8 field_0x149[0x150 - 0x149];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A time split into minutes, seconds and hundredths (class name unknown; see
 * Game/TGiScoreDisplay.cpp). */
class UnkTime_801E46E4 {
  public:
    UnkTime_801E46E4(float time);

    UnkTime_801E46E4 &operator=(const UnkTime_801E46E4 &other) {
        m_time = other.m_time;
        Split(m_time);
        return *this;
    }

    /* Whether this time is shorter than other (name inferred). */
    bool operator<(const UnkTime_801E46E4 &other) const {
        if (m_minutes < other.m_minutes) {
            return true;
        }
        if (m_minutes > other.m_minutes) {
            return false;
        }
        if (m_seconds < other.m_seconds) {
            return true;
        }
        if (m_seconds > other.m_seconds) {
            return false;
        }
        if (m_hundredths < other.m_hundredths) {
            return true;
        }
        return false;
    }

    /* Sets the minutes, seconds and hundredths from time (name inferred). */
    void Split(float time);

    /* 0x0 */ s32 m_minutes;
    /* 0x4 */ s8 m_seconds;
    /* 0x5 */ s8 m_hundredths;
    /* 0x8 */ float m_time;
};
SIZE_ASSERT(UnkTime_801E46E4, 0xC);

/* The save data fn_800BB74C returns (struct names unknown; member names inferred). */
struct UnkMissionSave_800BB74C {
    /* 0x00 */ u8 m_cleared;
    /* 0x04 */ s32 m_rank; /* 0 is the best rank */
    /* 0x08 */ UnkTime_801E46E4 m_time;
    /* 0x14 */ s32 m_score;
};

struct UnkStageSave_800BB74C {
    /* 0x00 */ s32 m_plays;
    /* 0x04 */ UnkMissionSave_800BB74C m_missions[3];
    /* 0x4C */ u8 field_0x4C[0x60 - 0x4C];
};
SIZE_ASSERT(UnkStageSave_800BB74C, 0x60);

/* The flags fn_801E5828 sets and fn_801E5810 clears (class unknown). */
struct UnkFlags_801E5810 {
    /* 0x0 */ u32 m_bits;
};

/* The record of the stages played in a row (meaning inferred). */
struct UnkPlayHistory_800BB74C {
    /* 0x00 */ s32 m_count;
    /* 0x04 */ s32 m_entries[24];
    /* 0x64 */ s32 m_lives;
    /* 0x68 */ s32 m_result;
    /* 0x6C */ s32 m_plays;
    /* 0x70 */ s32 m_lastLives;
    /* 0x74 */ UnkFlags_801E5810 m_flags;
};

struct UnkSaveData_800BB74C {
    /* 0x0000 */ u8 field_0x0[0x28];
    /* 0x0028 */ s32 field_0x28;
    /* 0x002C */ u8 field_0x2C[0x30 - 0x2C];
    /* 0x0030 */ UnkPlayHistory_800BB74C m_history;
    /* 0x00A8 */ UnkStageSave_800BB74C m_stages[59];
    /* 0x16C8 */ u8 field_0x16C8;
    /* 0x16C9 */ u8 m_allBest; /* every mission has the best rank */
    /* 0x16CA */ u8 field_0x16CA[0x16CC - 0x16CA];
    /* 0x16CC */ s32 m_bestCount;
};

/* A player's record (struct name unknown; see Game/TGi.h). */
struct UnkPlayerInfo_80178B5C {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_lives; /* meaning inferred */
};

/* What fn_801723E0 returns (class name unknown; see Game/PjsEventActionFade.cpp). */
struct UnkEventHolder_801723E0 {
    /* The task, if any (name inferred). */
    Task *GetTask() {
        return m_task ? m_task : 0;
    }

    /* 0x0 */ Task *m_task;
};

/* The event task (Game/PjsEventActionManager.cpp; only its state is read here). */
class TPjsEventActionManager : public Task {
  public:
    /* 0x28 */ s32 m_state; /* 5 once the event is ready (meaning inferred) */
};

/* The stage blocks of the first player (see Game/WarpHole.cpp). */
struct UnkBlockManage_8051CE88 {
    /* 0x000 */ u8 field_0x0[0x40];
    /* 0x040 */ s32 field_0x40;
    /* 0x044 */ u8 field_0x44[0x978 - 0x44];
    /* 0x978 */ u8 field_0x978;
};

extern UnkBlockManage_8051CE88 *lbl_8051CE88[4];

/* A page of a message (see Game/MessageTask.cpp). */
struct UnkHintMessage_80049E1C {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ s32 m_voice;
};

/* The memory card's slot record (struct name unknown). */
struct UnkCardSlot_8025D41C {
    /* 0x0 */ s32 field_0x0;
    /* 0x4 */ s32 m_stageId;
    /* 0x8 */ u8 field_0x8[0xC - 0x8];
};

class UnkHintTarget_801FB180;
class CharacterFileManager {
  public:
    void ReleaseAll();
};

namespace Mission {
/* The missions of the stage (Game/Mission.cpp). */
class MissionManager;
} // namespace Mission

/* The hint manager (class name unknown; see Game/UnkHintManager.h). */
class UnkHintManager_801FB180 {
  public:
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u8 m_taskActive;
};

/* The out-of-line copy of the inline accessor Game/HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

struct UnkCharaList_80074194;
struct UnkCharManager_8006F754;
struct UnkStageData_80079808;
struct UnkSound_80053164;
struct UnkEffectManager_800A31F0;
struct UnkStage_80177D14;
struct UnkFlag_80178678;
struct UnkCard_801D3140;
struct UnkResult_80117578;
struct UnkStage_8003A840 {
    /* 0x0000 */ u8 field_0x0[0x1810];
    /* 0x1810 */ u8 field_0x1810; /* meaning unknown */
};

/* The settings the effect manager hands out and takes back (see Game/ContinueAction.cpp). */
struct UnkEffectSettings_8042E034 {
    /* 0x0 */ u8 m_enabled;
    /* 0x1 */ u8 field_0x1;
    /* 0x4 */ float field_0x4;
    /* 0x8 */ s32 field_0x8;
    /* 0xC */ s32 field_0xC;
};

extern "C" {
UnkSaveData_800BB74C *fn_800BB74C(void);
UnkStageData_80079808 *fn_80079808(void);
UnkPlayerInfo_80178B5C *fn_80178B5C(UnkStageData_80079808 *data, int player);
void fn_801E5828(UnkFlags_801E5810 *flags, int bit);
void fn_801E5810(UnkFlags_801E5810 *flags, int bit);
/* Saves the game (behaviour unknown). */
void fn_8011BDA4(void);
Mission::MissionManager *fn_8006D6A8(void);
boost::weak_ptr<Mission::Mission> fn_800740E4(Mission::MissionManager *manager, int index);
UnkCharaList_80074194 *fn_80074194(void);
void fn_800A8D48(UnkCharaList_80074194 *list);
void fn_800A8CF4(UnkCharaList_80074194 *list);
void fn_800A8D9C(UnkCharaList_80074194 *list);
void fn_800A8DFC(UnkCharaList_80074194 *list);
/* Sets or clears the stage record's flag 0x20 (behaviour inferred). */
bool fn_80203970(UnkStageRecord_8057D748 *record, bool set);
UnkSound_80053164 *fn_80053164(void);
void fn_801D522C(UnkSound_80053164 *sound);
void fn_801D4C5C(UnkSound_80053164 *sound);
void fn_801D4E78(UnkSound_80053164 *sound);
void fn_801D51A0(UnkSound_80053164 *sound, s32 voice, int arg2, int arg3);
void fn_801D524C(UnkSound_80053164 *sound, const char *name);
void fn_801D3928(void);
UnkStage_8003A840 *fn_8003A840(void);
void fn_80047310(UnkStage_8003A840 *stage);
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_8042554C(UnkEffectManager_800A31F0 *manager);
UnkEffectSettings_8042E034 fn_8042E034(UnkEffectManager_800A31F0 *manager);
void fn_8042E098(UnkEffectManager_800A31F0 *manager, const UnkEffectSettings_8042E034 *settings);
UnkCharManager_8006F754 *fn_8006F754(void);
s32 fn_800746E0(UnkCharManager_8006F754 *manager, int mission);
s32 fn_800747E0(UnkCharManager_8006F754 *manager);
void fn_80074CA8(UnkCharManager_8006F754 *manager);
UnkStage_80177D14 *fn_80177D14(void);
void fn_8017743C(UnkStage_80177D14 *stage);
CharacterFileManager *fn_80078C88(void);
UnkCard_801D3140 *fn_801D3140(void);
s32 fn_8025D2C4(UnkCard_801D3140 *card, s32 id);
bool fn_8025D41C(UnkCard_801D3140 *card, UnkCardSlot_8025D41C *slot, int arg2, int arg3);
boost::shared_ptr<UnkHintTarget_801FB180> fn_80177C80(void);
boost::shared_ptr<UnkHintTarget_801FB180> fn_8017792C(const char *name);
UnkHintMessage_80049E1C *fn_80049E1C(UnkHintTarget_801FB180 *target, s32 id, s32 branch, s32 index);
void fn_8017786C(void);
UnkEventHolder_801723E0 *fn_801723E0(void);
void fn_8017430C(UnkEventHolder_801723E0 *holder, s32 event);
void fn_801742E0(UnkEventHolder_801723E0 *holder);
void fn_800C0358(void);
void fn_80172F48(TPjsEventActionManager *manager, bool play);
/* Fades the screen in (Game/PjsEventActionManager.cpp; argument meanings inferred). */
void fn_80173834(int arg0, float time);
UnkResult_80117578 *fn_80117578(void);
/* Creates the result screen of a kind, and whether it has ended (Game/ResultTask.cpp). */
bool fn_80202688(UnkResult_80117578 *result, int kind);
bool fn_8020261C(UnkResult_80117578 *result);
void fn_80177FA4(void);
UnkFlag_80178678 *fn_80178678(void);
bool fn_8025DBC0(UnkFlag_80178678 *flag, int value);
}

namespace StageAction {

GoalAction::GoalAction() {
    fn_800A8D48(fn_80074194());
    fn_800A8CF4(fn_80074194());
    fn_80203970(&lbl_8057D6A0.m_stageRecord, true);
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 1;
    fn_801D522C(fn_80053164());
    Begin();
    m_savedD1 = lbl_8057D6A0.field_0xD1;
    lbl_8057D6A0.field_0xD1 = 0;
    lbl_8057D6A0.m_stageRecord.field_0xD = 1;
    fn_801D3928();
}

GoalAction::~GoalAction() {
    fn_800A8DFC(fn_80074194());
    lbl_8057D6A0.field_0xD1 = m_savedD1;
    fn_80047310(fn_8003A840());
    UnkStage_8003A840 *stage = fn_8003A840();
    stage->field_0x1810 = 1;
    lbl_8057D6A0.m_stageRecord.field_0xD = 0;
    fn_801FAAA4(GetHintManager());
    fn_801D4C5C(fn_80053164());
}

void GoalAction::Execute() {
    switch (m_state) {
    case 0: {
        if (fn_80044E10() != 1) {
            break;
        }
        m_timer -= lbl_8057D6A0.m_frameStep;
        if (m_timer > 0) {
            return;
        }
        fn_802051F8(1.0f);
        UnkStageInfo_804C4A18 *info = fn_80176C3C(fn_800B74B4(), -1);
        fn_800A8D9C(fn_80074194());
        fn_8042554C(fn_800A31F0());
        UnkEffectSettings_8042E034 settings;
        settings = fn_8042E034(fn_800A31F0());
        settings.m_enabled = 0;
        fn_8042E098(fn_800A31F0(), &settings);
        fn_801FAAB4(GetHintManager());
        m_state = 1;
        s32 missionVoice = fn_800746E0(fn_8006F754(), m_result);
        s32 stageVoice = fn_800747E0(fn_8006F754());
        fn_8017743C(fn_80177D14());
        fn_80074CA8(fn_8006F754());
        fn_80078C88()->ReleaseAll();
        s32 event;
        UnkGameState_8057D6A0 *state = &lbl_8057D6A0;
        if (state->field_0x125 || state->m_stageRecord.m_id == 700) {
            event = 0x3D4;
        } else if (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 8) {
            event = 0x39E;
        } else if (info->m_flags & 1) {
            event = fn_8025D2C4(fn_801D3140(), stageVoice);
        } else {
            switch (m_result) {
            case 0:
            case 2:
                event = fn_8025D2C4(fn_801D3140(), missionVoice);
                break;
            case 1:
                event = fn_8025D2C4(fn_801D3140(), 14);
                break;
            }
        }
        if (state->field_0x125) {
            m_voice = 0x24E;
        } else {
            UnkHintMessage_80049E1C *message = 0;
            boost::shared_ptr<UnkHintTarget_801FB180> target = fn_80177C80();
            if (target.get() != 0) {
                message = fn_80049E1C(target.get(), info->m_missions[m_result].field_0x8, 1, 0);
            }
            if (message != 0) {
                m_voice = message->m_voice;
            } else {
                m_voice = -1;
            }
        }
        fn_8017786C();
        UnkHintManager_801FB180 *hints = GetHintManager();
        hints->m_taskActive = 0;
        fn_8017430C(fn_801723E0(), event);
        fn_801D4E78(fn_80053164());
        lbl_8057D6A0.m_hudVisible = 0;
        fn_800C0358();
        break;
    }
    case 1: {
        TPjsEventActionManager *event = (TPjsEventActionManager *)fn_801723E0()->GetTask();
        if (event != 0) {
            if (event->m_state == 5) {
                fn_80172F48(event, true);
                m_state = 2;
                m_frames = 0;
            }
        } else {
            m_state = 2;
            m_frames = 0;
        }
        break;
    }
    case 2:
        m_frames++;
        if (m_frames > 2 && !(lbl_8051CE88[0]->field_0x978 | (lbl_8051CE88[0]->field_0x40 != 0))) {
            TPjsEventActionManager *event = (TPjsEventActionManager *)fn_801723E0()->GetTask();
            if (event != 0) {
                fn_80172F48(event, false);
            }
            fn_80173834(1, 0.0f);
            m_state = 3;
            m_timer = 180;
            lbl_8057D6A0.field_0x60 = 0;
        }
        break;
    case 3:
        m_timer -= lbl_8057D6A0.m_frameStep;
        if (m_timer < 90 && m_prevTimer >= 90 && m_voice >= 0) {
            fn_801D51A0(fn_80053164(), m_voice, -1, -1);
        }
        if (m_timer <= 0) {
            fn_801D524C(fn_80053164(), "sng_jin_roundclear.adx");
            if (lbl_8057D6A0.field_0x125) {
                m_state = 6;
                m_timer = 0;
            } else if (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 0x10) {
                m_state = 5;
                m_timer = 0;
            } else if (!lbl_8057D6A0.m_stageRecord.m_flag4) {
                if (lbl_8057D6A0.m_stageRecord.field_0x9) {
                    fn_80202688(fn_80117578(), 3);
                } else {
                    switch (m_result) {
                    case 0:
                        fn_80202688(fn_80117578(), 1);
                        break;
                    case 2:
                        fn_80202688(fn_80117578(), 0);
                        break;
                    case 1:
                        fn_80202688(fn_80117578(), 2);
                        break;
                    }
                }
                m_state = 4;
            }
        }
        break;
    case 5:
        m_timer += lbl_8057D6A0.m_frameStep;
        if (m_timer > 300) {
            static const RwRGBA s_black = {0, 0, 0, 0};
            fn_80204874(&s_black);
            fn_80173878(0, 1, 1.0f);
            if (fn_80044E10() == 1) {
                UnkCardSlot_8025D41C slot;
                if (fn_8025D41C(fn_801D3140(), &slot, 1, 0) &&
                    slot.m_stageId == lbl_8057D6A0.m_stageRecord.m_id) {
                    m_state = 7;
                    m_timer = 0;
                } else {
                    m_state = 10;
                }
            }
        }
        break;
    case 4:
        if (fn_8020261C(fn_80117578())) {
            static const RwRGBA s_black = {0, 0, 0, 0};
            fn_80204874(&s_black);
            fn_80173878(0, 1, 1.0f);
            UnkCardSlot_8025D41C slot;
            if (fn_8025D41C(fn_801D3140(), &slot, 1, 0) &&
                slot.m_stageId == lbl_8057D6A0.m_stageRecord.m_id) {
                m_state = 7;
                m_timer = 0;
            } else {
                m_state = 10;
            }
        }
        break;
    case 6:
        m_timer += lbl_8057D6A0.m_frameStep;
        if (m_timer > 300) {
            static const RwRGBA s_black = {0, 0, 0, 0};
            fn_80204874(&s_black);
            fn_80173878(0, 1, 1.0f);
            if (fn_80044E10() == 1) {
                m_state = 10;
            }
        }
        break;
    case 7:
        if (fn_80044E10() == 1) {
            if (m_timer == 0) {
                lbl_8057D6A0.field_0xD1 = 1;
                fn_801742E0(fn_801723E0());
                fn_80117578();
                fn_80177FA4();
            } else if (m_timer > 3 && fn_801723E0()->m_task == 0) {
                if (fn_8025DBC0(fn_80178678(), 1)) {
                    m_state = 8;
                    fn_8017792C(0);
                    fn_801FAAB4(GetHintManager());
                } else {
                    m_state = 10;
                }
            }
            m_timer++;
        }
        break;
    case 8: {
        TPjsEventActionManager *event = (TPjsEventActionManager *)fn_801723E0()->GetTask();
        if (event != 0 && event->m_state == 5) {
            fn_80173834(1, 0.0f);
            m_state = 9;
        }
        break;
    }
    case 9:
        if (fn_801723E0()->m_task == 0) {
            fn_80173878(0, 1, 1.0f);
            m_state = 10;
        }
        break;
    case 10:
        if (fn_80044E10() == 1) {
            if (lbl_8057D6A0.m_stageRecord.field_0x9) {
                fn_80074470()->SetResult(6);
            } else {
                switch (m_result) {
                case 0:
                    fn_80074470()->SetResult(3);
                    break;
                case 2:
                    fn_80074470()->SetResult(5);
                    break;
                case 1:
                    fn_80074470()->SetResult(4);
                    break;
                }
            }
            lbl_8057D6A0.field_0xD1 = 1;
            fn_801742E0(fn_801723E0());
            SaveResult();
            switch (lbl_8057D6A0.m_mode) {
            case 6:
            case 7:
            case 8:
                switch (lbl_8057D6A0.m_stageRecord.m_index) {
                case 0x23:
                case 0x24:
                case 0x25:
                case 0x26:
                case 0x27:
                case 0x28:
                case 0x29:
                case 0x2A:
                case 0x2B:
                case 0x2C:
                    if (lbl_8057D6A0.field_0xCC == 1) {
                        fn_80175EEC(fn_80074470(), 16, true);
                    } else {
                        fn_80175EEC(fn_80074470(), 10, true);
                    }
                    break;
                case 0x1B:
                    if (lbl_8057D6A0.field_0x125) {
                        fn_80175EEC(fn_80074470(), 16, true);
                    } else {
                        fn_80175EEC(fn_80074470(), 10, true);
                    }
                    break;
                default:
                    fn_80175EEC(fn_80074470(), 10, true);
                    break;
                }
                break;
            default:
                fn_80175EEC(fn_80074470(), 16, true);
                break;
            }
        }
        break;
    }
    m_prevTimer = m_timer;
}

void GoalAction::Begin() {
    static const RwRGBA s_white = {0xFF, 0xFF, 0xFF, 0xFF};
    fn_80204874(&s_white);
    fn_802051F8(3.0f);
    fn_80173878(0, 1, 1.0f);
    m_state = 0;
    m_timer = 60;
    m_result = 1;
    if (!lbl_8057D6A0.m_stageRecord.field_0x9) {
        for (int i = 0; i < 3; i++) {
            boost::shared_ptr<Mission::Mission> mission = fn_800740E4(fn_8006D6A8(), i).lock();
            if (mission && mission->IsState(1)) {
                m_result = i;
            }
        }
    } else {
        switch (fn_800BB74C()->m_history.m_result) {
        case 3:
            m_result = 0;
            break;
        case 4:
            m_result = 1;
            break;
        case 5:
            m_result = 2;
            break;
        }
    }
    lbl_8057D6A0.m_stageRecord.m_result = m_result;
}

/* NonMatching (99.8%): two register swaps that about 20 source shapes did not change. Retail
 * loads the mission's rank into r3 and the result's rank into r0 (GC/2.7: the other way round),
 * and keeps the table entry in r5 and the save stage in r6 in the all-best loop (GC/2.7: r6 and
 * r5). Everything else in the unit matches. */
void GoalAction::SaveResult() {
    UnkSaveData_800BB74C *save = fn_800BB74C();
    if (save == 0) {
        return;
    }
    UnkPlayHistory_800BB74C *history = &save->m_history;
    if (lbl_8057D6A0.field_0xCC == 1) {
        if (fn_80176C3C(fn_800B74B4(), -1)->m_flags & 0x10) {
            lbl_8057D6A0.field_0x128 = fn_80178B5C(fn_80079808(), 0)->m_lives;
        } else {
            history->m_entries[history->m_count] = lbl_8057D6A0.m_stageRecord.m_index;
            history->m_count++;
            history->m_entries[history->m_count] =
                fn_80176C3C(fn_800B74B4(), -1)->m_missions[m_result].field_0x0;
            if (!lbl_8057D6A0.m_stageRecord.field_0x9) {
                history->m_result = fn_80074470()->field_0x10;
            }
            for (int i = 0; i < 20; i++) {
                if (lbl_8057D6A0.field_0xD4[i] != 0) {
                    fn_801E5828(&history->m_flags, i);
                } else {
                    fn_801E5810(&history->m_flags, i);
                }
            }
            history->m_lives = fn_80178B5C(fn_80079808(), 0)->m_lives;
        }
    }
    if (lbl_8057D6A0.field_0x125) {
        history->m_plays++;
        history->m_lastLives = fn_80178B5C(fn_80079808(), 0)->m_lives;
    } else {
        int mission = lbl_8057D6A0.m_stageRecord.field_0x9 ? 1 : m_result;
        UnkStageSave_800BB74C *stage = &save->m_stages[lbl_8057D6A0.m_stageRecord.m_index];
        stage->m_missions[mission].m_cleared = 1;
        stage->m_plays++;
        if (stage->m_missions[mission].m_rank > lbl_8057D6A0.m_stageRecord.m_rank) {
            stage->m_missions[mission].m_rank = lbl_8057D6A0.m_stageRecord.m_rank;
        }
        if (stage->m_missions[mission].m_score < lbl_8057D6A0.m_stageRecord.m_total) {
            stage->m_missions[mission].m_score = lbl_8057D6A0.m_stageRecord.m_total;
        }
        UnkTime_801E46E4 time(lbl_8057D6A0.m_time);
        if (time < stage->m_missions[mission].m_time) {
            stage->m_missions[mission].m_time = time;
        }
        fn_8011BDA4();
        if (!save->m_allBest) {
            bool allBest = true;
            int count = 0;
            for (int i = 5; i <= 43; i++) {
                if (i == 27) {
                    continue;
                }
                fn_800B74B4();
                const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[i];
                if (info->m_flags & 1) {
                    if (save->m_stages[i].m_missions[1].m_rank == 0) {
                        count++;
                    } else {
                        allBest = false;
                    }
                } else {
                    for (int k = 0; k < 3; k++) {
                        if (info->m_missions[k].m_id != -1) {
                            if (save->m_stages[i].m_missions[k].m_rank == 0) {
                                count++;
                            } else {
                                allBest = false;
                            }
                        }
                    }
                }
            }
            save->m_bestCount = count;
            save->m_allBest = allBest;
        }
    }
    save->field_0x28 = lbl_8057D6A0.field_0x8C;
}

} // namespace StageAction

/* StageAction::ContinueAction (class name from RTTI): the stage action that puts the players back
 * at their last checkpoint after a death. Once the screen has faded out it resets the stage's
 * effects and switches; then, with lives left, it restores the missions and the players and
 * starts the stage over through a TStageInitalizeTask (state 2), and without, it gives up (state
 * 11, StageAction::TryAgainAction). File name inferred. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/ScreenFade.h"
#include "Game/StageAction.h"
#include "Game/StageTable.h"
#include "Game/Task.h"
#include "Game/TrigerCounter.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The current stage's record in the game state (struct name unknown; see Game/StageRecord.cpp). */
struct UnkStageRecord_8057D748 {
    /* 0x00 */ s32 m_id;
    /* 0x04 */ u8 field_0x4[0x24 - 0x4];
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x1C];
    /* 0x01C */ float field_0x1C; /* meaning unknown */
    /* 0x020 */ u8 field_0x20[0x24 - 0x20];
    /* 0x024 */ float field_0x24; /* meaning unknown */
    /* 0x028 */ u8 field_0x28[0x30 - 0x28];
    /* 0x030 */ float field_0x30; /* meaning unknown */
    /* 0x034 */ u8 field_0x34[0xA8 - 0x34];
    /* 0x0A8 */ UnkStageRecord_8057D748 m_stageRecord;
    /* 0x0CC */ u8 field_0xCC[0x150 - 0xCC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stage's start-up task (Game/StageInitalizeTask.cpp; this unit allocates 0x48 bytes). */
class TStageInitalizeTask : public Task {
  public:
    TStageInitalizeTask(Task *parent, u32 waits);

    /* 0x28 */ u8 field_0x28[0x48 - 0x28];
};

namespace Mission {
/* The missions of the stage (Game/Mission.cpp). */
class MissionManager {
  public:
    void RestoreAll();
};
} // namespace Mission

/* The hint manager (see Game/HintRing.cpp). */
struct UnkHintManager_801FB180;
UnkHintManager_801FB180 *GetHintManager();
/* The candlesticks' switch counters (Game/CandleStick.cpp). */
UnkCandleSwitchManager_801660A0 *GetCandleSwitchManager();

/* The effect manager fn_800A31F0 returns (class unknown), and the settings it hands out and takes
 * back (struct name and member meanings unknown). */
struct UnkEffectManager_800A31F0;

struct UnkEffectSettings_8042E034 {
    /* 0x0 */ u8 m_enabled;
    /* 0x1 */ u8 field_0x1;
    /* 0x4 */ float field_0x4;
    /* 0x8 */ s32 field_0x8;
    /* 0xC */ s32 field_0xC;
};

/* The objects the stage code hands around (classes unknown). */
struct UnkObject_800C9B84;
struct UnkCharManager_8006F754;
struct UnkCharaList_80074194;
struct UnkStageData_80079808;

/* A player's record (struct name unknown; see Game/TGi.h). */
struct UnkPlayerInfo_80178B5C {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_lives; /* meaning inferred */
};

extern "C" {
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* Shows no hint any more (behaviour inferred). */
void fn_801FABE4(UnkHintManager_801FB180 *manager);
UnkEffectManager_800A31F0 *fn_800A31F0(void);
/* Ends the effects, and reads and writes the settings (behaviour inferred). */
void fn_8042554C(UnkEffectManager_800A31F0 *manager);
UnkEffectSettings_8042E034 fn_8042E034(UnkEffectManager_800A31F0 *manager);
void fn_8042E098(UnkEffectManager_800A31F0 *manager, const UnkEffectSettings_8042E034 *settings);
/* The switches of the stage, and two resets of them (behaviour inferred). */
UnkObject_800C9B84 *fn_800C9B84(void);
void fn_800CB758(UnkObject_800C9B84 *switches);
void fn_800CB654(UnkObject_800C9B84 *switches);
UnkCharManager_8006F754 *fn_8006F754(void);
void fn_80074C10(UnkCharManager_8006F754 *manager);
void fn_80074B58(UnkCharManager_8006F754 *manager);
UnkStageData_80079808 *fn_80079808(void);
UnkPlayerInfo_80178B5C *fn_80178B5C(UnkStageData_80079808 *data, int player);
void fn_80178B80(UnkStageData_80079808 *data);
Mission::MissionManager *fn_8006D6A8(void);
/* Puts the players at their checkpoints (Game/StageInitalizeTask.cpp). */
void fn_80176E84(bool first);
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int id);
/* Sends character a command (behaviour inferred). */
void fn_800B7EF8(const boost::weak_ptr<CharInfo> &character);
/* Stage resets (behaviour unknown). */
void fn_80119178(void);
void fn_801A24EC(void);
void fn_800289B0(void);
void fn_8004851C(void);
/* Sets or clears the stage record's flag 0x20 (behaviour inferred). */
bool fn_80203970(UnkStageRecord_8057D748 *record, bool set);
}

namespace StageAction {

ContinueAction::ContinueAction() {
    m_state = 0;
    m_timer = 0;
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 1;
    fn_80173878(0, 1, 1.0f);
    fn_80203970(&lbl_8057D6A0.m_stageRecord, true);
    lbl_8057D6A0.field_0x1C = 0.0f;
}

ContinueAction::~ContinueAction() {
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 0;
    lbl_8057D6A0.field_0x1C = 1.0f;
}

void ContinueAction::Execute() {
    switch (m_state) {
    case 0:
        if (fn_80044E10() == 1) {
            fn_801FABE4(GetHintManager());
            fn_8042554C(fn_800A31F0());
            UnkEffectSettings_8042E034 settings;
            settings = fn_8042E034(fn_800A31F0());
            settings.m_enabled = 0;
            fn_8042E098(fn_800A31F0(), &settings);
            fn_800CB758(fn_800C9B84());
            fn_80074C10(fn_8006F754());
            GetCandleSwitchManager()->ResetCounters();
            m_timer = 2;
            m_state = 1;
        }
        break;
    case 1:
        if (m_timer-- < 0) {
            if (fn_80178B5C(fn_80079808(), 0)->m_lives == 0) {
                fn_800CB654(fn_800C9B84());
                fn_80175EEC(fn_80074470(), 11, true);
            } else {
                fn_80178B80(fn_80079808());
                fn_8006D6A8()->RestoreAll();
                fn_80074B58(fn_8006F754());
                fn_80176E84(true);
                for (int i = 0; i < 2; i++) {
                    boost::weak_ptr<CharInfo> character = fn_800A8B74(fn_80074194(), i);
                    boost::shared_ptr<CharInfo> locked = character.lock();
                    if (locked) {
                        fn_800B7EF8(boost::weak_ptr<CharInfo>(locked));
                    }
                }
                m_timer = 2;
                m_state = 2;
            }
        }
        break;
    case 2:
        if (m_timer-- < 0) {
            fn_800CB654(fn_800C9B84());
            new TStageInitalizeTask(fn_801EB304(fn_8001C4F8(), 3), 2);
            fn_80175EEC(fn_80074470(), 2, true);
            fn_80119178();
            fn_801A24EC();
            fn_800289B0();
            fn_8004851C();
            UnkStageInfo_804C4A18 *info =
                fn_80176C3C(fn_800B74B4(), lbl_8057D6A0.m_stageRecord.m_id);
            if (info != 0 && info->m_callbacks[1] != 0) {
                info->m_callbacks[1]();
            }
            lbl_8057D6A0.field_0x30 = 1.0f;
            lbl_8057D6A0.field_0x24 = 1.0f;
        }
        break;
    }
}

/* The u8 cast keeps the comparison's byte mask (rlwinm ...,24,31) that retail has. */
bool ContinueAction::IsDone() {
    return (u8)(m_state == 2);
}

} // namespace StageAction

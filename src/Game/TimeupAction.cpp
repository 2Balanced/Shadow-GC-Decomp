/* StageAction::TimeupAction (class name from RTTI): the stage action when the stage's time runs
 * out. It stops the characters, waits a frame, fades the screen to black and then goes to state 11
 * when the first player has no lives left (meaning inferred) or to state 7 otherwise; a stage whose
 * table entry has flag 4 set goes to state 14 (StageAction::Result2PAction) instead, as in
 * Game/DeadAction.cpp. File name inferred. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/ScreenFade.h"
#include "Game/StageAction.h"
#include "Game/StageTable.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xAC];
    /* 0x0AC */ s32 m_stageIndex; /* index into the stage table (name inferred) */
    /* 0x0B0 */ u8 field_0xB0[0x150 - 0xB0];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A player's record (struct name unknown; see Game/ContinueAction.cpp). */
struct UnkPlayerInfo_80178B5C {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s32 m_lives; /* meaning inferred */
};

/* The objects the stage code hands around (classes unknown). */
struct UnkCharaList_80074194;
struct UnkStageData_80079808;

extern "C" {
UnkCharaList_80074194 *fn_80074194(void);
/* Stop the characters (see Game/GoalAction.cpp). */
void fn_800A8D48(UnkCharaList_80074194 *list);
void fn_800A8CF4(UnkCharaList_80074194 *list);
UnkStageData_80079808 *fn_80079808(void);
UnkPlayerInfo_80178B5C *fn_80178B5C(UnkStageData_80079808 *data, int player);
}

namespace StageAction {

static const RwRGBA s_black = {0, 0, 0, 0};

TimeupAction::TimeupAction() {
    m_state = 0;
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 1;
    fn_800A8D48(fn_80074194());
    fn_800A8CF4(fn_80074194());
    m_timer = 1;
}

TimeupAction::~TimeupAction() {}

void TimeupAction::Execute() {
    int index = lbl_8057D6A0.m_stageIndex;
    fn_800B74B4();
    const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[index];
    if (info != 0 && (info->m_flags & 4)) {
        fn_80175EEC(fn_80074470(), 14, true);
        return;
    }
    switch (m_state) {
    case 0:
        m_timer--;
        if (m_timer == 0) {
            m_state = 1;
            fn_80204874(&s_black);
            fn_80173878(0, 1, 1.0f);
        }
        break;
    case 1:
        if (fn_80044E10() == 1) {
            if (fn_80178B5C(fn_80079808(), 0)->m_lives == 0) {
                fn_80175EEC(fn_80074470(), 11, true);
            } else {
                fn_80175EEC(fn_80074470(), 7, true);
            }
        }
        break;
    }
}

} // namespace StageAction

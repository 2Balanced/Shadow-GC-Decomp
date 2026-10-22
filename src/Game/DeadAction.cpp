/* StageAction::DeadAction (class name from RTTI): the stage action of a player's death. It waits
 * 30 frames, fades the screen to black and then restarts the stage from the last checkpoint
 * (state 8, StageAction::ContinueAction); a stage whose table entry has flag 4 set goes to state 14
 * (StageAction::Result2PAction) instead. File name inferred. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
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

namespace StageAction {

static const RwRGBA s_black = {0, 0, 0, 0};

DeadAction::DeadAction() {
    m_state = 0;
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 1;
    m_timer = 30;
}

DeadAction::~DeadAction() {}

void DeadAction::Execute() {
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
            fn_80173878(1, 1, 1.0f);
        }
        break;
    case 1:
        if (fn_80044E10() == 1) {
            fn_80175EEC(fn_80074470(), 8, true);
        }
        break;
    }
}

} // namespace StageAction

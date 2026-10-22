/* StageAction::InitAction (class name from RTTI): the stage action that starts a stage. Once the
 * start-up task (TStageInitalizeTask) has set the ready flag, it moves on to the two-player
 * selection (state 12), the 0x126 variant (state 13) or play (state 3, StageAction::PlayAction),
 * which also clears the screen fade. File name inferred. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/ScreenFade.h"
#include "Game/StageAction.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x1C];
    /* 0x01C */ float field_0x1C; /* meaning unknown */
    /* 0x020 */ u8 field_0x20[0x60 - 0x20];
    /* 0x060 */ s32 field_0x60; /* meaning unknown */
    /* 0x064 */ u8 field_0x64[0xA8 - 0x64];
    /* 0x0A8 */ u8 m_stageRecord[0xA]; /* the current stage (see Game/StageRecord.cpp) */
    /* 0x0B2 */ u8 m_twoPlayer;         /* meaning inferred */
    /* 0x0B3 */ u8 field_0xB3[0x126 - 0xB3];
    /* 0x126 */ u8 field_0x126; /* meaning unknown */
    /* 0x127 */ u8 field_0x127[0x150 - 0x127];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* What fn_80178678 returns (struct name unknown; see Game/StageManager.cpp). */
struct UnkFlag_80178678;

extern "C" {
/* Sets or clears the stage record's flag 0x20 (behaviour inferred). */
bool fn_80203970(void *record, bool set);
UnkFlag_80178678 *fn_80178678(void);
void fn_8025DBC0(UnkFlag_80178678 *flag, int value);
}

/* Set once the start-up task has placed the players (Game/StageInitalizeTask.cpp; name and meaning
 * inferred). */
s32 lbl_805EFC20;

namespace StageAction {

InitAction::InitAction() {
    fn_80203970(lbl_8057D6A0.m_stageRecord, true);
    lbl_8057D6A0.field_0x1C = 0.0f;
    lbl_805EFC20 = 0;
    lbl_8057D6A0.field_0x60 = 0;
    fn_80203970(lbl_8057D6A0.m_stageRecord, true);
}

InitAction::~InitAction() {}

void InitAction::Execute() {
    switch (lbl_805EFC20) {
    case 0:
        break;
    case 1:
        if (lbl_8057D6A0.m_twoPlayer) {
            fn_80175EEC(fn_80074470(), 12, true);
        } else if (lbl_8057D6A0.field_0x126) {
            fn_80175EEC(fn_80074470(), 13, true);
        } else {
            fn_80175EEC(fn_80074470(), 3, true);
            fn_8025DBC0(fn_80178678(), 0);
            fn_80044648(fn_80044254(), 1.0f);
        }
        break;
    }
}

} // namespace StageAction

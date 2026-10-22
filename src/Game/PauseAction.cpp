/* StageAction::PauseAction (class name from RTTI): the stage action that runs while the game is
 * paused. It only sets the stage record's flag and pauses the sounds while it exists. File name
 * inferred. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/StageAction.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The current stage's record in the game state (struct name unknown; see Game/StageRecord.cpp). */
struct UnkStageRecord_8057D748 {
    /* 0x00 */ s32 m_id;
    /* 0x04 */ u8 field_0x4[0x24 - 0x4];
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ UnkStageRecord_8057D748 m_stageRecord;
    /* 0x0CC */ u8 field_0xCC[0x150 - 0xCC];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

extern "C" {
/* Sets or clears the stage record's flag 0x20 (behaviour inferred). */
bool fn_80203970(UnkStageRecord_8057D748 *record, bool set);
/* The sound manager (see BAGunShip.cpp), and pausing and resuming its sounds (behaviour
 * inferred). */
void *fn_80053164(void);
void fn_801D4FBC(void *manager, int pause);
void fn_801D4EA0(void *manager);
}

namespace StageAction {

PauseAction::PauseAction() {
    fn_80203970(&lbl_8057D6A0.m_stageRecord, true);
    fn_801D4FBC(fn_80053164(), 1);
}

PauseAction::~PauseAction() {
    fn_801D4EA0(fn_80053164());
}

void PauseAction::Execute() {}

} // namespace StageAction

/* StageAction::PlayAction (class name from RTTI): the stage action while the stage is played. Its
 * constructor fades the screen in and sets up the stage state; Execute does nothing. File name
 * inferred. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/ScreenFade.h"
#include "Game/StageAction.h"
#include "Game/StageTable.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x1C];
    /* 0x01C */ float field_0x1C; /* meaning unknown */
    /* 0x020 */ u8 field_0x20[0x60 - 0x20];
    /* 0x060 */ s32 field_0x60; /* meaning unknown */
    /* 0x064 */ s32 field_0x64; /* meaning unknown */
    /* 0x068 */ s32 field_0x68; /* meaning unknown */
    /* 0x06C */ u8 field_0x6C[0xA8 - 0x6C];
    /* 0x0A8 */ s32 m_stageRecord; /* the current stage (see Game/StageRecord.cpp) */
    /* 0x0AC */ s32 m_stageIndex;  /* index into the stage table (name inferred) */
    /* 0x0B0 */ u8 field_0xB0[0x124 - 0xB0];
    /* 0x124 */ u8 field_0x124; /* meaning unknown */
    /* 0x125 */ u8 field_0x125[0x150 - 0x125];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* An object with a word at 0x30 (class unknown). */
struct UnkObject_805EF9F4 {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ s32 field_0x30;
};

extern UnkObject_805EF9F4 *lbl_805EF9F4;

extern "C" {
/* Sets or clears the stage record's flag 0x20 (behaviour inferred). */
bool fn_80203970(void *record, bool set);
/* Fades the screen in (Game/PjsEventActionManager.cpp; argument meanings inferred). */
void fn_80173834(int arg0, float time);
}

namespace StageAction {

PlayAction::PlayAction() {
    fn_80173834(1, 0.0f);
    fn_80203970(&lbl_8057D6A0.m_stageRecord, false);
    lbl_8057D6A0.field_0x1C = 1.0f;
    int index = lbl_8057D6A0.m_stageIndex;
    fn_800B74B4();
    const UnkStageInfo_804C4A18 *info = &lbl_804C4A18[index];
    if (info->m_flags & 2) {
        lbl_8057D6A0.field_0x60 = 1;
        lbl_8057D6A0.field_0x64 = 0;
        lbl_8057D6A0.field_0x68 = 1;
    } else {
        lbl_8057D6A0.field_0x60 = 0;
    }
    if (lbl_805EF9F4 != 0) {
        lbl_805EF9F4->field_0x30 = 1;
    }
    lbl_8057D6A0.field_0x124 = 0;
}

PlayAction::~PlayAction() {}

void PlayAction::Execute() {}

} // namespace StageAction

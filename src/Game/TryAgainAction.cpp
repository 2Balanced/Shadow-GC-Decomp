/* StageAction::TryAgainAction (class name from RTTI): the stage action after a game over. It opens
 * the GiGameOver screen and lets the player choose between trying again and quitting (pads 4 and,
 * on stages flagged for two players, 5), then requests the matching stage state. File name from the
 * class; member and function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/StageAction.h"
#include "Game/StageTable.h"
#include "Game/UnkSingletons.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (only the members used here; see Game/AdvMemoryCheck.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x1C];
    /* 0x01C */ float field_0x1C;
    /* 0x020 */ u8 field_0x20[0xA8 - 0x20];
    /* 0x0A8 */ u8 field_0xA8[0x4];
    /* 0x0AC */ s32 m_stageIndex;
    /* 0x0B0 */ u8 field_0xB0[0x125 - 0xB0];
    /* 0x125 */ u8 field_0x125;
};

struct UnkObject_800BB74C {
    /* 0x00 */ u8 field_0x0[0x9C];
    /* 0x9C */ s32 field_0x9C;
};

class UnkHintManager_801FB180;
class UnkCharaList_80074194;

extern UnkGameState_8057D6A0 lbl_8057D6A0;

UnkHintManager_801FB180 *GetHintManager();

extern "C" {
UnkObject_800BB74C *fn_800BB74C(void);
/* Whether button was pressed on pad (behaviour inferred). */
bool fn_80356260(int button, int pad);
/* The sound manager, and playing sound id through it. */
void *fn_80053164(void);
void fn_801D560C(void *manager, int soundId, int arg2, int arg3);
/* Whether the save message window is hidden (Game/SaveCycleMessage.cpp). */
bool fn_802E5810(UnkObject_802E5B2C *object);
/* The GiGameOver screen (Game/GiGameOver.cpp): open it, show the selection, whether it is
 * closing, confirm the selection and close it (behaviour inferred). */
bool fn_802E3B64(void);
void fn_802E3B0C(u8 retry);
u8 fn_802E3B20(void);
void fn_802E3AEC(void);
bool fn_802E3B3C(void);
/* The hint manager's pause and resume (behaviour inferred). */
void fn_801FAAA4(UnkHintManager_801FB180 *manager);
void fn_801FAAB4(UnkHintManager_801FB180 *manager);
bool fn_80203970(void *state, u8 on);
UnkCharaList_80074194 *fn_80074194(void);
void fn_800A8CF4(UnkCharaList_80074194 *list);
}

inline void PlaySound(int id) {
    fn_801D560C(fn_80053164(), id, 0, 0);
}

namespace StageAction {

TryAgainAction::TryAgainAction() {
    m_state = 0;
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 1;
    fn_80203970(lbl_8057D6A0.field_0xA8, 1);
    lbl_8057D6A0.field_0x1C = 0.0f;
    fn_801FAAB4(GetHintManager());
    fn_80074470()->SetResult(2);
    fn_800A8CF4(fn_80074194());
}

TryAgainAction::~TryAgainAction() {
    lbl_8057D6A0.field_0x1C = 1.0f;
    fn_801FAAA4(GetHintManager());
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 0;
    fn_802E3B3C();
}

void TryAgainAction::Execute() {
    if (lbl_8057D6A0.field_0x125) {
        UnkObject_800BB74C *object = fn_800BB74C();
        object->field_0x9C = 0;
        fn_80175EEC(fn_80074470(), 10, true);
        fn_80074470()->SetResult(2);
        return;
    }
    int index = lbl_8057D6A0.m_stageIndex;
    fn_800B74B4();
    const UnkStageInfo_804C4A18 *stage = reinterpret_cast<const UnkStageInfo_804C4A18 *>(
        reinterpret_cast<const u8 *>(lbl_804C4A18) + index * sizeof(UnkStageInfo_804C4A18));
    switch (m_state) {
    case 0:
        if (fn_802E3B64()) {
            m_retry = 1;
            fn_802E3B0C(m_retry);
            m_state = 1;
        } else {
            m_state = 4;
        }
        break;
    case 1:
        if (!fn_802E3B20() && fn_802E5810(fn_80117514())) {
            bool left = false;
            bool right = false;
            bool confirm = false;
            if (fn_80356260(5, 4)) {
                left = true;
            } else if (fn_80356260(6, 4)) {
                right = true;
            } else if (fn_80356260(0, 4) || fn_80356260(1, 4)) {
                confirm = true;
            }
            if (stage != 0 && (stage->m_flags & 4)) {
                if (fn_80356260(5, 5)) {
                    left = true;
                } else if (fn_80356260(6, 5)) {
                    right = true;
                } else if (fn_80356260(0, 5) || fn_80356260(1, 5)) {
                    confirm = true;
                }
            }
            if (!m_retry && left) {
                m_retry = 1;
                fn_802E3B0C(m_retry);
                PlaySound(0xE002);
            } else if (m_retry && right) {
                m_retry = 0;
                fn_802E3B0C(m_retry);
                PlaySound(0xE002);
            } else if (confirm) {
                PlaySound(0xE003);
                fn_802E3AEC();
                m_state = 2;
            }
        }
        break;
    case 2:
        if (!fn_802E3B20()) {
            if (stage != 0 && (stage->m_flags & 4)) {
                fn_80175EEC(fn_80074470(), 0x10, true);
                if (m_retry) {
                    fn_80074470()->SetResult(6);
                } else {
                    fn_80074470()->SetResult(2);
                }
            } else if (m_retry) {
                fn_80175EEC(fn_80074470(), 7, true);
            } else {
                fn_80175EEC(fn_80074470(), 0x10, true);
            }
            m_state = 3;
        }
        break;
    case 4:
        fn_80175EEC(fn_80074470(), 0x10, true);
        break;
    }
}

} // namespace StageAction

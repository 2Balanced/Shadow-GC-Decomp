/* StageAction::EndAction (class name from RTTI): the stage action that ends a stage. It fades the
 * screen out and, while nothing else holds the stage open, counts down ten frames before asking
 * for state 1 (the end of the stage). File name inferred. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/ScreenFade.h"
#include "Game/StageAction.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x60];
    /* 0x060 */ s32 field_0x60; /* meaning unknown */
    /* 0x064 */ u8 field_0x64[0x150 - 0x64];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The object fn_800BADE8 returns (class unknown; see Game/WarpHole.cpp). */
struct UnkObject_800BADE8 {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ s32 field_0x30; /* meaning unknown */
};

/* The stage (class name unknown; see Game/StageManager.cpp). */
struct UnkStage_8003A840 {
    /* 0x0000 */ u8 field_0x0[0x1810];
    /* 0x1810 */ u8 field_0x1810; /* meaning unknown */
};

struct UnkSound_80053164;
struct UnkStage_80177D14;
struct UnkObject_80014B90;
struct UnkObject_8004B13C;

/* Nonzero while something keeps the stage from ending (meaning unknown). */
extern s32 lbl_805EE194;

extern "C" {
UnkObject_800BADE8 *fn_800BADE8(void);
void fn_801E2F28(UnkObject_800BADE8 *object);
void fn_801E2EDC(UnkObject_800BADE8 *object);
UnkStage_8003A840 *fn_8003A840(void);
/* The sound manager, and two of its switches (behaviour inferred). */
UnkSound_80053164 *fn_80053164(void);
void fn_801D4E78(UnkSound_80053164 *sound);
void fn_801D4E50(UnkSound_80053164 *sound);
UnkStage_80177D14 *fn_80177D14(void);
void fn_80178778(UnkStage_80177D14 *stage);
/* Tests that keep the stage open (behaviour unknown). */
UnkObject_80014B90 *fn_80014B90(void);
bool fn_8025D104(UnkObject_80014B90 *object, int arg1);
UnkObject_8004B13C *fn_8004B13C(void);
bool fn_8004D4D4(UnkObject_8004B13C *object);
}

namespace StageAction {

EndAction::EndAction() {
    m_held = 0;
    m_timer = 10;
    fn_80173878(0, 1, 1.0f);
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 1;
    UnkObject_800BADE8 *object = fn_800BADE8();
    fn_801E2F28(object);
    fn_801E2EDC(object);
    object->field_0x30 = 0;
    UnkStage_8003A840 *stage = fn_8003A840();
    stage->field_0x1810 = 1;
    fn_801D4E78(fn_80053164());
}

EndAction::~EndAction() {
    fn_801D4E50(fn_80053164());
    lbl_8057D6A0.field_0x60 = 0;
    fn_80178778(fn_80177D14());
}

void EndAction::Execute() {
    if (fn_80044E10() == 1) {
        if (fn_8025D104(fn_80014B90(), 2) || !fn_8004D4D4(fn_8004B13C()) || lbl_805EE194 != 0) {
            if (!m_held) {
                m_held = 1;
            }
            m_timer = 10;
        } else {
            m_timer--;
            if (m_timer == 0) {
                fn_80175EEC(fn_80074470(), 1, false);
                StageActionManager *manager = fn_80074470();
                manager->m_locked = 0;
                m_timer = 10;
            }
        }
    }
}

} // namespace StageAction

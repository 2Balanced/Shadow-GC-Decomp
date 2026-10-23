/* StageStaffroll (class name from RTTI; see Game/StageSequence.h): the stage that plays the staff
 * roll of an ending. It starts the staff roll and fades in when created, and once the staff roll
 * reports that it has ended it fades out and reports 8. The file name comes from the class name;
 * the names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/ScreenFade.h"
#include "Game/StageSequence.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* Silences the stage's sounds (behaviour inferred; see Game/StageMovie.cpp). */
void fn_802BEB90(void);
/* Fades the screen in (Game/PjsEventActionManager.cpp; argument meanings inferred). */
void fn_80173834(int arg0, float time);
/* Starts the staff roll of an ending, and whether it has ended (behaviour inferred). */
void fn_80342398(int ending, int arg);
int fn_80342388(void);
}

SIZE_ASSERT(StageStaffroll, 0x8);

StageStaffroll::StageStaffroll(int ending, int arg) : StageBase::Base(3) {
    fn_802BEB90();
    fn_80173834(1, 0.0f);
    fn_80342398(ending, arg);
}

StageStaffroll::~StageStaffroll() {}

int StageStaffroll::Tick() {
    if (fn_80342388() != 0) {
        fn_80173878(0, 1, 1.0f);
        if (fn_80044E10() == 1) {
            return 8;
        }
    }
    return 1;
}

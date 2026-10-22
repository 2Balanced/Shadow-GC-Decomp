/* StageAction::SaveAction (class name from RTTI): the stage action that saves the game. Once the
 * screen has faded it starts a SaveCycle::CycleTask with the save phase, waits for the cycle to end
 * and for the save message window to close, then requests stage state 0x10. File name from the
 * class; member and function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/SaveCycle.h"
#include "Game/StageAction.h"
#include "Game/Task.h"
#include "Game/UnkSingletons.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The message source (class name unknown; see Game/UnkHintManager.h). */
class UnkHintTarget_801FB180;

struct UnkRenderOwner_80046178 {
    /* 0x00 */ u8 field_0x0[0x40];
    /* 0x40 */ Task *m_drawLayer;
};

extern "C" {
/* Screen fades (behaviour inferred; see Game/AdvMemoryCheck.cpp). */
void fn_80173878(int arg0, int arg1, float time);
int fn_80044E10(void);
/* The sound manager, and stopping its sounds (behaviour inferred). */
void *fn_80053164(void);
void fn_801D522C(void *manager);
/* The message source named name (behaviour inferred; see Game/StageManager.cpp). */
boost::shared_ptr<UnkHintTarget_801FB180> fn_8017792C(const char *name);
UnkRenderOwner_80046178 *fn_80046178(void);
/* Whether the cycle task's cycle has ended (behaviour inferred). */
bool fn_802B8DF0(Task *task);
/* Whether the save message window is hidden (Game/SaveCycleMessage.cpp). */
bool fn_802E5810(UnkObject_802E5B2C *object);
}

namespace StageAction {

SaveAction::SaveAction() {
    m_state = 0;
    m_started = 0;
    fn_801D522C(fn_80053164());
    StageActionManager *manager = fn_80074470();
    manager->m_locked = 1;
    fn_80173878(0, 1, 1.0f);
}

SaveAction::~SaveAction() {}

void SaveAction::Execute() {
    if (!m_started) {
        if (fn_80044E10() == 1) {
            fn_8017792C("Save");
            m_task = new SaveCycle::CycleTask(fn_80046178()->m_drawLayer, SaveCycle::PHASE_SAVE);
            m_started = 1;
        }
    } else if (m_state != 0) {
        if (fn_802E5810(fn_80117514())) {
            fn_80175EEC(fn_80074470(), 0x10, true);
        }
    } else if (fn_802B8DF0(m_task)) {
        m_state++;
        m_task->m_flags |= 1;
    }
}

} // namespace StageAction


/* The stage action manager (class name unknown; declared in Game/StageAction.h): fn_80074470
 * returns the game's one. Each state change drops the current StageAction::IAction and creates the
 * state's action in a boost::shared_ptr<IAction>; most states also free the shared_ptr instances
 * of fifteen action types, which this unit carries. File name inferred. Names marked "(name
 * inferred)" are not original, nor are the fn_ names the functions other units call keep.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/StageAction.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void *memset(void *dst, int value, unsigned long count);

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line (boost
 * 1.32's definition, as in Game/Mission.cpp). */
template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

} // namespace boost

extern "C" void fn_80174590(StageActionManager *manager);

StageActionManager::StageActionManager() : m_prevState(0), m_locked(0) {}

/* Clears the holds and starts the stage over (name inferred). */
extern "C" void fn_80175F18(StageActionManager *manager) {
    memset(manager->m_holds, 0, sizeof(manager->m_holds));
    manager->m_locked = 0;
    if (!manager->m_holds[2] && !manager->m_locked) {
        manager->m_state = 2;
    }
}

/* Requests state unless the current one is held, or the manager is locked and force is false
 * (name inferred). */
extern "C" void fn_80175EEC(StageActionManager *manager, int state, bool force) {
    if (manager->m_holds[state]) {
        return;
    }
    if (manager->m_locked && !force) {
        return;
    }
    manager->m_state = state;
}

/* Replaces the action when the state changed (name inferred). */
extern "C" void fn_80174590(StageActionManager *manager) {
    if (manager->m_state != 0 && manager->m_state != manager->m_prevState) {
        switch (manager->m_state) {
        case 1:
            memset(manager->field_0x15, 0, sizeof(manager->field_0x15));
            manager->m_action.reset();
            break;
        case 2:
            memset(manager->field_0x15, 0, sizeof(manager->field_0x15));
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::InitAction);
            break;
        case 3:
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::PlayAction);
            break;
        case 4:
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::EventAction);
            break;
        case 6:
            manager->m_holds[manager->m_state] = 1;
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::GoalAction);
            break;
        case 5:
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::PauseAction);
            break;
        case 7:
            memset(manager->field_0x15, 0, sizeof(manager->field_0x15));
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::RestartAction);
            break;
        case 8:
            memset(manager->field_0x15, 0, sizeof(manager->field_0x15));
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::ContinueAction);
            break;
        case 9:
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::DeadAction);
            break;
        case 10:
            manager->m_holds[manager->m_state] = 1;
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::SaveAction);
            break;
        case 11:
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::TryAgainAction);
            break;
        case 12:
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::ShowAction);
            break;
        case 13:
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::FlashbackAction);
            break;
        case 14:
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::Result2PAction);
            break;
        case 15:
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::TimeupAction);
            break;
        case 16:
            manager->m_holds[manager->m_state] = 1;
            manager->m_action.reset();
            manager->m_action = boost::shared_ptr<StageAction::IAction>(new StageAction::EndAction);
            break;
        }
        manager->m_prevState = manager->m_state;
        if (!manager->m_holds[0]) {
            manager->m_state = 0;
        }
    }
}

/* Runs the state machine and the current action (name inferred). */
extern "C" void fn_80174544(StageActionManager *manager) {
    fn_80174590(manager);
    if (manager->m_action) {
        manager->m_action->Execute();
    }
}

/* Starts the stage (name inferred). */
extern "C" void fn_80174504(StageActionManager *manager) {
    if (!manager->m_holds[1] && !manager->m_locked) {
        manager->m_state = 1;
    }
    fn_80174590(manager);
}

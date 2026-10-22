/* SaveCycle::CycleTask (class name from RTTI): the task that runs a save/load cycle. The cycle
 * (class name unknown) holds the current SaveCycle::CycleBase phase in a boost::shared_ptr; each
 * phase id creates a SaveCycle::Load, Save, Check or CheckModule::Phase, and the cycle drops the
 * phase once its Update returns false. While any phase exists the shared TMemoryCardCtrl task is
 * kept alive. File name from the namespace; names marked "(name inferred)" and all member names
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * NonMatching: 54 of 55 functions match. fn_802E3E44 differs by two swapped instructions: retail
 * loads card->m_phase above the store of the link register (lwz r3, 0(r3) before stw r0,
 * 0x14(r1)); GC/2.7 does not for any source form tried (see Game/VehiclePathMove.cpp and
 * Game/CameraScreenSet.cpp). The data sections match. */
#include <boost/scoped_ptr.hpp>

#include "Game/MemoryCardCtrl.h"
#include "Game/SaveCycle.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line (boost
 * 1.32's definition). */
template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

} // namespace boost

extern "C" {
/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
}

UnkCardCtrlHolder_801E5B80::UnkCardCtrlHolder_801E5B80() {
    m_ctrl = 0;
    m_users = 0;
}

/* Adds a user, creating the controller for the first one (name inferred). */
TMemoryCardCtrl *UnkCardCtrlHolder_801E5B80::Acquire() {
    m_users++;
    if (m_ctrl != 0) {
        return m_ctrl;
    }
    fn_80299FE0();
    m_ctrl = fn_80299F14(fn_801EB304(fn_8001C4F8(), 0));
    return m_ctrl;
}

/* Drops a user, ending the controller after the last one (name inferred). */
bool UnkCardCtrlHolder_801E5B80::Release() {
    m_users--;
    if (m_users != 0) {
        return true;
    }
    if (m_ctrl == 0) {
        return false;
    }
    m_ctrl->m_flags |= 1;
    m_ctrl = 0;
    fn_80299F5C();
    return true;
}

namespace SaveCycle {

CycleBase::CycleBase(int id) : m_id(id) {
    fn_801E5B80()->Acquire();
}

CycleBase::~CycleBase() {
    fn_801E5B80()->Release();
}

bool CycleBase::Update() {
    return false;
}

void CycleBase::Draw() {}

} // namespace SaveCycle

u8 UnkMemoryCard_802E472C::s_cardUsable;
int UnkMemoryCard_802E472C::s_result;

void UnkMemoryCard_802E472C::SetCardUsable(u8 usable) {
    s_cardUsable = usable;
}

extern "C" u8 fn_802E4784(void) {
    return UnkMemoryCard_802E472C::s_cardUsable;
}

void UnkMemoryCard_802E472C::SetResult(int result) {
    s_result = result;
}

int UnkMemoryCard_802E472C::GetResult() {
    return s_result;
}

UnkMemoryCard_802E472C::UnkMemoryCard_802E472C(int id) {
    ChangePhase(id);
    s_cardUsable = 0;
}

UnkMemoryCard_802E472C::~UnkMemoryCard_802E472C() {}

/* Replaces the phase with a new one of phase id, or ends it for id 0 (name inferred). */
void UnkMemoryCard_802E472C::ChangePhase(int id) {
    if (!m_phase || id != m_phase->m_id) {
        m_phase.reset();
        switch (id) {
        case SaveCycle::PHASE_LOAD:
            m_phase = boost::shared_ptr<SaveCycle::CycleBase>(new SaveCycle::Load::Phase);
            break;
        case SaveCycle::PHASE_SAVE:
            m_phase = boost::shared_ptr<SaveCycle::CycleBase>(new SaveCycle::Save::Phase);
            break;
        case SaveCycle::PHASE_CHECK:
            m_phase = boost::shared_ptr<SaveCycle::CycleBase>(new SaveCycle::Check::Phase);
            break;
        case SaveCycle::PHASE_CHECK_MODULE:
            m_phase = boost::shared_ptr<SaveCycle::CycleBase>(new SaveCycle::CheckModule::Phase);
            break;
        }
    }
}

extern "C" void fn_802E3E7C(UnkMemoryCard_802E472C *card) {
    if (card->m_phase && !card->m_phase->Update()) {
        card->ChangePhase(SaveCycle::PHASE_NONE);
    }
}

extern "C" void fn_802E3E44(UnkMemoryCard_802E472C *card) {
    if (card->m_phase) {
        card->m_phase->Draw();
    }
}

extern "C" bool fn_802E3E34(UnkMemoryCard_802E472C *card) {
    return !card->m_phase;
}

namespace SaveCycle {

CycleTask::CycleTask(Task *parent, int id) : Task(parent), m_cycle(new UnkMemoryCard_802E472C(id)) {
    m_name = "CycleTask";
    m_flags |= 0x100;
}

CycleTask::~CycleTask() {}

void CycleTask::Tick(float dt) {
    fn_802E3E7C(m_cycle.get());
    fn_802E3E44(m_cycle.get());
}

} // namespace SaveCycle

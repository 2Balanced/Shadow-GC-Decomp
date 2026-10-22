/* StageBase::Base (class name from RTTI; see Game/StageSequence.h): the base class of the stages
 * the stage sequence plays (StageManager, StageMovie, StageRouteDisplay, StageStaffroll). Its own
 * Tick reports 0 and its vf_0x10 does nothing. The file name is not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/StageSequence.h"

namespace StageBase {

Base::Base(int kind) : m_kind(kind) {}

Base::~Base() {}

int Base::Tick() {
    return 0;
}

void Base::vf_0x10() {}

} // namespace StageBase

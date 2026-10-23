/* AdvReal::AdvBase (class name from RTTI): the base of the front-end phases. Its own frame and
 * draw functions do nothing and report 1, as the phases do. File name from the class.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/AdvBase.h"

namespace AdvReal {

AdvBase::AdvBase() {}

AdvBase::~AdvBase() {}

bool AdvBase::Tick() {
    return 1;
}

bool AdvBase::Draw() {
    return 1;
}

} // namespace AdvReal

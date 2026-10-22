/* StageSequence::StageSequenceBase (class name from RTTI; see Game/StageSequence.h): the base
 * class of the stage sequence's phases. Its own Tick ends at once and its Skip does nothing. The
 * file name is not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/StageSequence.h"

namespace StageSequence {

StageSequenceBase::StageSequenceBase() {}

StageSequenceBase::~StageSequenceBase() {}

bool StageSequenceBase::Tick() {
    return false;
}

void StageSequenceBase::Skip() {}

} // namespace StageSequence

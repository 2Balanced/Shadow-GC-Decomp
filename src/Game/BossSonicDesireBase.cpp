/* Player::Npc::BossSonic::Desire::BossSonicDesireBase (class name from RTTI; declared in
 * Game/BossSonic.h): the base of Boss Sonic's desires, which keeps the AI's work. File name inferred
 * from the class (as Game/DiablonDesireBase.cpp). Split from Game/BossSonicDesireCounterAttack.cpp
 * in wave 41: retail's sections hold a second copy of the BossSonicDesireBase and DesireBase RTTI
 * records and names there, one for each translation unit. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/BossSonic.h"
#include "boost/shared_ptr.hpp"

namespace Player {
namespace Npc {
namespace BossSonic {
namespace Desire {

BossSonicDesireBase::BossSonicDesireBase(const boost::shared_ptr<BossSonicWork> &work, const char *name, int arg)
    : Boss::Desire::DesireBase(name, arg), m_work(work) {}

BossSonicDesireBase::~BossSonicDesireBase() {}

void BossSonicDesireBase::Begin() {}

} // namespace Desire
} // namespace BossSonic
} // namespace Npc
} // namespace Player

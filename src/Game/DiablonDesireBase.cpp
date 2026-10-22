/* Boss::Diablon::Desire::DiablonDesireBase (class name from RTTI; declared in Game/Diablon.h): the
 * base of the Diablon boss's desires, which keeps the boss. File name inferred from the class.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/BossDesire.h"
#include "Game/Diablon.h"
#include "boost/shared_ptr.hpp"

namespace Boss {
namespace Diablon {
namespace Desire {

DiablonDesireBase::DiablonDesireBase(const boost::shared_ptr<Diablon> &owner, const char *name,
                                     int arg)
    : DesireBase(name, arg), m_owner(owner) {}

DiablonDesireBase::~DiablonDesireBase() {}

void DiablonDesireBase::Begin() {}

} // namespace Desire
} // namespace Diablon
} // namespace Boss

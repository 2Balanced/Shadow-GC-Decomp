/* Boss::Diablon::Behavior::BehaviorBase (class name from RTTI; declared in Game/Diablon.h): the
 * base of the Diablon boss's behaviors. The unit holds its constructor and destructor and the
 * shared_ptr<Diablon> instances they use. File name inferred from the class. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/Diablon.h"
#include "boost/shared_ptr.hpp"

namespace Boss {
namespace Diablon {
namespace Behavior {

BehaviorBase::BehaviorBase(const boost::shared_ptr<Diablon> &owner, const char *name, int arg)
    : m_owner(owner), m_name(name), field_0x10(arg) {}

BehaviorBase::~BehaviorBase() {}

} // namespace Behavior
} // namespace Diablon
} // namespace Boss

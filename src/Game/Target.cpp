/* Target (class name from RTTI; Game/Target.h): a point weapons and homing attacks aim at. This unit
 * has the class and the functions that create, test and release targets; the shared_ptr a target
 * is created in deletes it through DeleteTarget, which also has the target manager drop it. File
 * name from the class. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#define BOOST_SP_DELETER_CONSTRUCTOR
#include "Game/Target.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

Target::Target(u32 flags)
    : m_position(0.0f, 0.0f, 0.0f), m_setPosition(0.0f, 0.0f, -1.0f), m_radius(-1.0f), field_0x24(0.0f),
      m_flags(flags) {
    m_flags |= 1;
    m_flags = CompleteFlags(m_flags);
}

Target::Target(const boost::weak_ptr<CharInfo> &owner, u32 flags)
    : m_position(0.0f, 0.0f, 0.0f), m_setPosition(0.0f, 0.0f, -1.0f), m_radius(-1.0f), field_0x24(0.0f),
      m_owner(owner), m_flags(flags) {
    m_flags |= 1;
    m_flags = CompleteFlags(m_flags);
}

Target::~Target() {}

TargetPosition Target::Position() {
    return m_position;
}

TargetPosition Target::GetPosition(const Vec3f *from) {
    return m_position;
}

/* Sets where the target is (name inferred; other units call it by its address). */
extern "C" void fn_801E73B0(Target *target, const Vec3f *position) {
    fn_800091D0(&target->m_setPosition, position);
    target->m_flags |= 2;
}

/* The deleter of the shared_ptrs targets are created in (name inferred). */
static void DeleteTarget(Target *target) {
    delete target;
    fn_801E7A20(fn_80060058());
}

/* Creates a target without an owner (name inferred). */
extern "C" boost::shared_ptr<Target> fn_801E72DC(u32 flags) {
    boost::shared_ptr<Target> target(new Target(flags), DeleteTarget);
    fn_801E7AF4(fn_80060058(), target);
    return target;
}

/* Creates a target for owner (name inferred). */
extern "C" boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, u32 flags) {
    boost::shared_ptr<Target> target(new Target(owner, flags), DeleteTarget);
    fn_801E7AF4(fn_80060058(), target);
    return target;
}

/* Releases a target (name inferred). */
extern "C" void fn_801E7108(boost::shared_ptr<Target> *target) {
    target->reset();
}

/* Whether a target still exists and is active (name inferred). */
extern "C" bool fn_801E7094(const boost::weak_ptr<Target> *target) {
    if (target->expired()) {
        return false;
    }
    /* Retail tests the flag with a branch; GC/2.7 turns the same source into a branchless
     * conversion (addic/subfe). Every form tried gives the latter. */
    u32 active = target->lock()->IsActive();
    if (active == 0) {
        return false;
    }
    return true;
}

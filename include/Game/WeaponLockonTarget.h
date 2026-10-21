#ifndef GAME_WEAPONLOCKONTARGET_H
#define GAME_WEAPONLOCKONTARGET_H

#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* Target (class name from RTTI): units that use it declare its members. */
class Target;

namespace Weapon {

/* LockonTarget (class name from the RTTI of its shared_ptr control block,
 * sp_counted_base_impl<Weapon::LockonTarget *, ...> at 0x8051ED40): what a homing shot follows.
 * Layout from its constructor (0x8006118C); names inferred from the ARKCANNON gadget. */
class LockonTarget {
  public:
    LockonTarget();

    /* Sets the target to follow. An inline function in the original: it calls the inline
     * weak_ptr assignment, so it is emitted as a weak copy (0x80061054). */
    void SetTarget(const boost::weak_ptr<Target> &target) {
        m_target = target;
    }
    /* Creates the lock-on mark task for the target (0x80060F70; behaviour inferred from its
     * allocation of 0x30 bytes). */
    void fn_80060F70();

    /* 0x0 */ boost::weak_ptr<Target> m_target;
    /* 0x8 */ u32 field_0x8;
};
SIZE_ASSERT(LockonTarget, 0xC);

} // namespace Weapon

#endif

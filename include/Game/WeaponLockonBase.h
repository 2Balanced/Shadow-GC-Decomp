#ifndef GAME_WEAPONLOCKONBASE_H
#define GAME_WEAPONLOCKONBASE_H

#include "Game/WeaponBase.h"
#include "Game/WeaponLockonTarget.h"
#include "Game/MathTypes.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class CharInfo;

namespace Weapon {

/* What a lock-on weapon uses to find and hold its targets (class name unknown; a 0x58-byte object
 * built by 0x8006065C with the most targets and deleted through 0x800605B4). Nothing of its
 * layout is used outside its own code. */
class UnkLockonSearch_8006065C;

/* WeaponLockonBase (class name from RTTI; vtable 0x8051ECFC): a weapon that locks on to targets
 * before it fires. Its constructor is at 0x8005FBEC and its destructor at 0x8005FB14; the two
 * last virtual functions are pure. The other methods keep their address names; what is known of
 * them is inferred from Game/WeaponWormShooter.cpp. */
class WeaponLockonBase : public WeaponBase {
  public:
    /* count: the most targets held at once. */
    WeaponLockonBase(const boost::weak_ptr<CharInfo> &owner, int count, int type);
    virtual ~WeaponLockonBase();
    /* Called when a new target is locked on (meaning inferred from the sound WormShooter plays). */
    virtual void vf_0x30() = 0;
    /* Fires the shot of the index-th locked target; returns the time until the next shot
     * (meaning inferred). */
    virtual float vf_0x34(const boost::shared_ptr<LockonTarget> &target, int index) = 0;

    /* Searches for targets in front of muzzle up to range away (0x8005F854). */
    void fn_8005F854(const Mtx34f *muzzle, float range);
    /* Counts n shots fired (0x8005F878). */
    void fn_8005F878(int n);
    /* The number of targets held (0x8005F9C8). */
    int fn_8005F9C8();
    /* The most targets held at once (0x8005F9EC). */
    int fn_8005F9EC();
    /* Per-frame work of the search (0x8005FACC). */
    void fn_8005FACC(float dt);
    /* Starts the frame's search over (0x8005FAF0). */
    void fn_8005FAF0();

    /* 0x38 */ boost::scoped_ptr<UnkLockonSearch_8006065C> m_search;
};
SIZE_ASSERT(WeaponLockonBase, 0x3C);

} // namespace Weapon

#endif

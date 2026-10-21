#ifndef GAME_WEAPONSTICKBASE_H
#define GAME_WEAPONSTICKBASE_H

#include "Game/WeaponBase.h"
#include "Game/MathTypes.h"
#include "boost/scoped_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class CharInfo;

namespace Weapon {

/* What a stick weapon uses to swing and hit (class name unknown; a 0x54-byte object built by
 * 0x80068850 with the reach and deleted through 0x800687FC). Nothing of its layout is used outside
 * its own code. */
class UnkStickSwing_80068850;

/* WeaponStickBase (class name from RTTI; vtable 0x8051F7D4): a weapon swung at close range. The
 * constructor is at 0x80067A8C and the destructor at 0x800679B4; Tick is at 0x80067990 and the
 * slots after vf_0x2C at 0x8006796C, 0x80067948 and 0x800678C4. Names other than the class name
 * are inferred. */
class WeaponStickBase : public WeaponBase {
  public:
    /* reach: how far the swing hits (inferred from the values the Katana passes). */
    WeaponStickBase(const boost::weak_ptr<CharInfo> &owner, int type, float reach);
    virtual ~WeaponStickBase();
    virtual void Tick(float dt);
    /* Starts a swing (meaning inferred). */
    virtual void SwingHeld();
    /* Ends a swing (meaning inferred). */
    virtual void SwingReleased();
    /* Plays the n-th swing sound at the weapon's position. */
    virtual void vf_0x38(int n);

    /* Places the blade between tip and root (0x80067870; meaning inferred). */
    void fn_80067870(const Vec3f *tip, const Vec3f *root);
    /* Whether the swing is in a state the Katana does not fire in (0x80067918; meaning
     * unknown). */
    bool fn_80067918();

    /* 0x38 */ boost::scoped_ptr<UnkStickSwing_80068850> m_swing;
};
SIZE_ASSERT(WeaponStickBase, 0x3C);

} // namespace Weapon

#endif

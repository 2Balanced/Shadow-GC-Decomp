#ifndef GAME_WEAPONCANNONBASE_H
#define GAME_WEAPONCANNONBASE_H

#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class CharInfo;

namespace Weapon {

/* WeaponCannonBase (class name from RTTI; vtable 0x8051E92C): a weapon that fires shells. The
 * constructor is at 0x8005E1EC and the destructor at 0x8005E17C; SetTransform is at 0x8005E130,
 * Tick at 0x8005E0FC and the two slots after vf_0x2C at 0x8005E07C and 0x8005E014. The last slot is
 * pure. Names other than the class name are inferred. */
class WeaponCannonBase : public WeaponBase {
  public:
    /* effects may be null (no model). */
    WeaponCannonBase(const boost::weak_ptr<CharInfo> &owner, int type,
                     const UnkWeaponEffectSet_80063428 *effects);
    virtual ~WeaponCannonBase();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    /* Fires a shell from the muzzle, calls vf_0x38 and counts the shot; returns the time until the
     * next shot (meaning inferred). */
    virtual float ReloadTime();
    /* Where a shell fired now would hit (meaning inferred from fn_80051AF0). */
    virtual Bullet::HitResult AimHit();
    /* Plays the firing effects (meaning inferred from Game/WeaponSpecialHealCannon.cpp). */
    virtual void vf_0x38() = 0;

    /* The shots fired this frame. */
    int ShotCount() const {
        return m_shots;
    }

    /* 0x38 */ s32 m_shots; /* shots this frame (reset by Tick) */
};
SIZE_ASSERT(WeaponCannonBase, 0x3C);

} // namespace Weapon

#endif

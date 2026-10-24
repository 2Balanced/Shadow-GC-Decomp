/* WeaponCannonBase (class name from RTTI: Weapon::WeaponCannonBase): the base of the weapons that
 * fire shells. A shot fires the weapon type's bullet from the model's muzzle and calls vf_0x38 for
 * the firing effects. File name from the class name; method names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/WeaponCannonBase.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "boost/weak_ptr.hpp"

using Weapon::ShotInfo;

extern "C" {
/* The bullet types' records (behaviour inferred; see WeaponLaserRifle.cpp): fires bullet type type
 * with info, and where a shot of type would hit within range (see Game/GunLift.cpp). */
void fn_80051C34(int type, const ShotInfo &info);
Weapon::Bullet::HitResult fn_80051AF0(int type, const ShotInfo *shot, float range);
/* The shot of a weapon from a muzzle matrix (see Game/EmbeddedWeapon.cpp). */
ShotInfo fn_80069540(Weapon::WeaponBase *weapon, const Mtx34f *muzzle);
}

namespace Weapon {

WeaponCannonBase::WeaponCannonBase(const boost::weak_ptr<CharInfo> &owner, int type,
                                   const UnkWeaponEffectSet_80063428 *effects)
    : WeaponBase(owner, type, 2) {
    if (effects != 0) {
        m_model = new UnkWeaponModel_80063428(owner, effects);
    } else {
        m_model = 0;
    }
}

WeaponCannonBase::~WeaponCannonBase() {
    if (m_model != 0) {
        delete m_model;
    }
}

void WeaponCannonBase::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    if (m_model != 0) {
        m_model->fn_800632E8(position, rotation);
    }
}

void WeaponCannonBase::Tick(float dt) {
    m_shots = 0;
    if (m_model != 0) {
        m_model->fn_800632A0(dt);
    }
}

float WeaponCannonBase::ReloadTime() {
    fn_80051C34(m_type, fn_80069540(this, m_model->fn_80063234(0)));
    vf_0x38();
    m_shots++;
    return 0.5f;
}

Bullet::HitResult WeaponCannonBase::AimHit() {
    return fn_80051AF0(m_type, &(const ShotInfo &)fn_80069540(this, m_model->fn_80063234(0)), 2.0f);
}

} // namespace Weapon

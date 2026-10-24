/* WeaponGunBase (class name from RTTI: Weapon::WeaponGunBase): the base of the weapons that fire
 * bullets. It counts the shots fired each frame. File name from the class name; method names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/WeaponBase.h"
#include "boost/weak_ptr.hpp"

namespace Weapon {

WeaponGunBase::WeaponGunBase(const boost::weak_ptr<CharInfo> &owner, int type) : WeaponBase(owner, type, 1) {
    field_0x38 = 0;
}

WeaponGunBase::~WeaponGunBase() {}

void WeaponGunBase::Tick(float dt) {
    field_0x38 = 0;
}

/* Counts n shots fired this frame. */
void WeaponGunBase::fn_8005EB94(int n) {
    field_0x38 += n;
}

} // namespace Weapon

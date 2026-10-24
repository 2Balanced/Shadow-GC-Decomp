/* WeaponBase (class name from RTTI: Weapon::WeaponBase): the base of the weapons characters hold.
 * Its virtual functions read the weapon type's record (Game/WeaponTable.cpp) and show, hide and
 * place the weapon's model. File name from the class name; method names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/WeaponBase.h"
#include "Game/GadgetUtil.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"

extern "C" {
/* The weapon type's float at 0x2C of its record and its recoil (Game/WeaponTable.cpp). */
float fn_8005B8DC(int type);
float fn_8005B904(int type);
/* The range of weapon type type (behaviour inferred from its use as the lock-on range). */
float fn_80051AC8(int type);
}

namespace Weapon {

WeaponBase::WeaponBase(const boost::weak_ptr<CharInfo> &owner, int type, int arg)
    : m_owner(owner), m_type(type), field_0x18(arg), m_model(0), m_rotation(0.0f, 0.0f, 0.0f) {}

WeaponBase::~WeaponBase() {}

void WeaponBase::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    AssignVector(m_position, *position);
    m_rotation = *rotation;
}

} // namespace Weapon

/* Copies a weapon's position and rotation out (either may be null; other units call it by its
 * address). */
extern "C" void fn_8005CF90(const Weapon::WeaponBase *weapon, Vec3f *position, Vec3f *rotation) {
    if (position != 0) {
        AssignVector(*position, weapon->m_position);
    }
    if (rotation != 0) {
        *rotation = weapon->m_rotation;
    }
}

namespace Weapon {

void WeaponBase::Show() {
    if (m_model != 0) {
        m_model->fn_80063130();
    }
}

void WeaponBase::Hide() {
    if (m_model != 0) {
        m_model->fn_80063154();
    }
}

/* The type's range, an offset (meaning unknown), the type's recoil and the float at 0x2C of its
 * record. */
float WeaponBase::vf_0x1C() {
    return fn_80051AC8(m_type);
}

Vec3f WeaponBase::vf_0x20() {
    return Vec3f(0.0f, -1.0f, -2.703125f);
}

float WeaponBase::vf_0x24() {
    return fn_8005B904(m_type);
}

float WeaponBase::vf_0x28() {
    return fn_8005B8DC(m_type);
}

void WeaponBase::Tick(float dt) {}

} // namespace Weapon

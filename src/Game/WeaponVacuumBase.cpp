/* Weapon::WeaponVacuumBase (class name from RTTI): the base of the weapons that pull objects in
 * (VacuumPod, VacuumEgg). It looks for the nearest target in front of its muzzle, holds the
 * VacuumObject it pulls, and lets it go with a push along the muzzle. File name inferred from the
 * class. Names other than the class name are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
/* Enables shared_ptr::reset(Y *) (see boost/shared_ptr.hpp). */
#define BOOST_SP_RESET_POINTER
#include "Game/CharInfo.h"
#include "Game/GadgetUtil.h"
#include "Game/TargetSearchList.h"
#include "Game/VacuumObject.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponVacuumBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI): a point other objects can aim at; the character it belongs to is
 * at 0x28 (as in WeaponVacuumPod.cpp). */
class Target {
  public:
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner;
};

extern "C" {
/* The fn_80014E04 flag word (see GiantLantern.cpp). */
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The weapon's owner (WeaponBase::m_owner; 0x80065034). */
boost::weak_ptr<CharInfo> fn_80065034(Weapon::WeaponBase *weapon);
/* A length scaled by a game setting (behaviour unknown; see EggLastMechCamera.cpp). */
float fn_8005AE20(float value);
/* The translation of a matrix, and point turned by matrix (behaviour inferred). */
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *point);
/* vector * scale, and vector += value (behaviour inferred). */
Vec3f fn_800091A4(const Vec3f *vector, float scale);
void fn_8040CE04(Vec3f *vector, const Vec3f *value);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
}

/* A length (name inferred). As a call it is not a constant expression, so the value below is
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* How fast a released object leaves the muzzle (name inferred). */
static float s_releaseSpeed = Span(300.0f);

namespace Weapon {

WeaponVacuumBase::WeaponVacuumBase(const boost::weak_ptr<CharInfo> &owner, int type)
    : WeaponBase(owner, type, 4) {
    fn_80014E10(&m_flags.m_bits);
    field_0x3C = 0;
    field_0x48 = 0;
    field_0x4C = 0;
    field_0x50 = 0;
    field_0x54 = 0;
}

void WeaponVacuumBase::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void WeaponVacuumBase::Tick(float dt) {
    m_model->fn_800632A0(dt);
    field_0x48 = field_0x4C;
    field_0x4C = 0;
    field_0x50 = field_0x54;
    field_0x54 = 0;
}

void WeaponVacuumBase::fn_80239E94(int count) {
    field_0x3C += count;
}

s32 WeaponVacuumBase::fn_80239E8C() {
    return field_0x3C;
}

bool WeaponVacuumBase::HasObject() {
    return !m_object.expired();
}

bool WeaponVacuumBase::fn_80239DD4() {
    if (m_object.expired()) {
        return false;
    }
    boost::shared_ptr<VacuumObject> object = m_object.lock();
    return fn_80014E34(object->GetFlags(), 0);
}

s32 WeaponVacuumBase::fn_80239DCC() {
    return field_0x48;
}

void WeaponVacuumBase::fn_80239D8C(bool set) {
    if (set) {
        fn_80014E1C(&m_flags.m_bits, 2);
    } else {
        fn_8006127C(&m_flags.m_bits, 2);
    }
}

void WeaponVacuumBase::StartVacuum() {
    fn_80014E1C(&m_flags.m_bits, 0);
    boost::shared_ptr<CharInfo> owner = fn_80065034(this).lock();
    if (owner && owner->field_0x5C != 1) {
        fn_80014E1C(&m_flags.m_bits, 1);
    }
}

void WeaponVacuumBase::StopVacuum() {
    fn_8006127C(&m_flags.m_bits, 0);
    fn_8006127C(&m_flags.m_bits, 1);
}

void WeaponVacuumBase::vf_0x38(int count) {
    field_0x4C += count;
}

void WeaponVacuumBase::vf_0x3C(int count) {
    field_0x54 += count;
}

void WeaponVacuumBase::SetObject(const boost::weak_ptr<VacuumObject> &object) {
    m_object = object;
}

void WeaponVacuumBase::LetGo(const Vec3f &velocity) {
    if (!m_object.expired()) {
        boost::shared_ptr<VacuumObject> object = m_object.lock();
        Mtx34f muzzle;
        fn_8040C148(&muzzle, m_model->fn_80063234(0));
        Vec3f push;
        fn_800091EC(&push, &(const Vec3f &)fn_8040B618(
                               &muzzle, &Vec3f(0.0f, 0.0f, s_releaseSpeed)));
        fn_8040CFF8(&push, &velocity);
        object->fn_80238184(push);
    }
}

boost::weak_ptr<Target> WeaponVacuumBase::FindTarget(float range) {
    const Mtx34f *muzzle = m_model->fn_80063234(0);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040B618(
                                muzzle, &Vec3f(0.0f, 0.0f, 1.0f)));
    fn_8040CE04(&position, &(const Vec3f &)fn_800091A4(&direction, fn_8005AE20(1.0f)));
    boost::shared_ptr<TargetSearchList> list;
    list.reset(fn_801E7930(fn_80060058()));
    if (fn_80014E34(&m_flags.m_bits, 1)) {
        fn_801E8048(list.get(), 1);
        fn_801E7FB4(list.get(), &position, 0.5f, &direction);
    } else {
        fn_801E7FB4(list.get(), &position, 0.866025f, &direction);
    }
    fn_801E8218(list.get(), &position, range);
    fn_801E80C0(list.get(), 0x80);
    boost::shared_ptr<CharInfo> owner = fn_80065034(this).lock();
    if (owner) {
        s32 player = owner->field_0x54;
        if (player != -1) {
            while (true) {
                boost::weak_ptr<Target> target = fn_801E848C(list.get(), &position);
                if (!target.expired()) {
                    boost::shared_ptr<CharInfo> character = target.lock()->m_owner.lock();
                    if (character && player == character->field_0x54) {
                        fn_801E7E48(list.get(), target);
                    } else {
                        return target;
                    }
                } else {
                    return target;
                }
            }
        }
    }
    return fn_801E848C(list.get(), &position);
}

} // namespace Weapon

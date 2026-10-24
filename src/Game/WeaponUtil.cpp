/* The shots of the weapons: the ShotInfo (Game/WeaponBullet.h) a weapon fires from its muzzle
 * matrix, forwards, toward a point or toward a locked target, with the weapon's owner and its side
 * (0 for a player's weapon, whose shots also take the player's velocity as their aim toward a
 * locked target), the two ShotInfo constructors, and the weapon system's two empty hooks (see
 * Game/WeaponTable.cpp). Other units call the functions by their addresses, so they keep those
 * names. The unit has no class of its own; the file name and the other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first).
 *
 * NonMatching: everything matches but the two ShotInfo constructors. Game/WeaponBullet.h declares
 * their side parameter as bool, and GC/2.7 zero-extends a bool (clrlwi) before storing it in the
 * 32-bit m_side; retail stores the register as it is, as for a 32-bit parameter (as does the third
 * constructor at 0x80051FC8). Declaring it so would rename the constructors that about eight
 * matched units call. */
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponLockonTarget.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

using Weapon::LockonTarget;
using Weapon::ShotInfo;
using Weapon::WeaponBase;

extern "C" {
/* The owner of a weapon (Game/WeaponRocketLauncher.cpp). */
boost::weak_ptr<CharInfo> fn_80065034(const WeaponBase *weapon);
/* The translation of a matrix, and a direction taken through a matrix (see Game/WeaponItem.cpp). */
Vec3f fn_8040FB08(const Mtx34f *matrix);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
/* A vector set to zero, and normalised (see Game/BirdBase.cpp and Game/BulletCannon.cpp). */
void fn_8040CE60(Vec3f *vector);
Vec3f fn_8040CEE8(const Vec3f &vector);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The shot of a cannon from muzzle, forwards (other units call it by its address). */
extern "C" ShotInfo fn_80069540(WeaponBase *weapon, const Mtx34f *muzzle) {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040B618(muzzle, &(const Vec3f &)Vec3f(0.0f, 0.0f, 1.0f)));
    boost::shared_ptr<CharInfo> owner = fn_80065034(weapon).lock();
    bool side;
    if (owner.get() != 0 && owner->field_0x5C == 1) {
        side = false;
    } else {
        side = true;
    }
    return ShotInfo(fn_80065034(weapon), side, position, direction);
}

/* The shot of a gun from muzzle, forwards, with aim (other units call it by its address). */
extern "C" ShotInfo fn_80069424(WeaponBase *weapon, const Mtx34f *muzzle, const Vec3f *aim) {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040B618(muzzle, &(const Vec3f &)Vec3f(0.0f, 0.0f, 1.0f)));
    boost::shared_ptr<CharInfo> owner = fn_80065034(weapon).lock();
    bool side;
    if (owner.get() != 0 && owner->field_0x5C == 1) {
        side = false;
    } else {
        side = true;
    }
    return ShotInfo(fn_80065034(weapon), side, position, direction, *aim);
}

namespace Weapon {

ShotInfo::ShotInfo(const boost::weak_ptr<CharInfo> &owner, bool side, const Vec3f &position,
                   const Vec3f &direction, const Vec3f &aim)
    : m_owner(owner), m_position(position), m_direction(direction), m_aim(aim), m_side(side) {}

} // namespace Weapon

/* The shot of a gun from muzzle toward target, with aim (other units call it by its address). */
extern "C" ShotInfo fn_80069298(WeaponBase *weapon, const Mtx34f *muzzle, const Vec3f *aim,
                                const Vec3f *target) {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(((VectorSubtractFn)fn_8040CD74)(target, &position)));
    boost::shared_ptr<CharInfo> owner = fn_80065034(weapon).lock();
    bool side;
    if (owner.get() != 0 && owner->field_0x5C == 1) {
        side = false;
    } else {
        side = true;
    }
    return ShotInfo(fn_80065034(weapon), side, position, direction, *aim);
}

namespace Weapon {

ShotInfo::ShotInfo(const boost::weak_ptr<CharInfo> &owner, const boost::shared_ptr<LockonTarget> &target,
                   bool side, const Vec3f &position, const Vec3f &direction, const Vec3f &aim)
    : m_owner(owner), m_target(target), m_position(position), m_direction(direction), m_aim(aim),
      m_side(side) {}

} // namespace Weapon

/* The shot of a lock-on weapon from muzzle toward target, aimed along a player owner's velocity
 * (other units call it by its address). */
extern "C" ShotInfo fn_800690E4(WeaponBase *weapon, const Mtx34f *muzzle,
                                const boost::shared_ptr<LockonTarget> &target) {
    Vec3f aim;
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(muzzle));
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040B618(muzzle, &(const Vec3f &)Vec3f(0.0f, 0.0f, 1.0f)));
    boost::shared_ptr<CharInfo> owner = fn_80065034(weapon).lock();
    bool side;
    if (owner.get() != 0 && owner->field_0x5C == 1) {
        side = false;
        fn_800091D0(&aim, &owner->field_0x18);
    } else {
        side = true;
        fn_8040CE60(&aim);
    }
    return ShotInfo(fn_80065034(weapon), target, side, position, direction, aim);
}

/* The weapon system's start and end hooks for the shots (both empty; names inferred). */
extern "C" void fn_800690E0(void) {}

extern "C" int fn_800690D8(void) {
    return 0;
}

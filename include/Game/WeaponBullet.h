#ifndef GAME_WEAPONBULLET_H
#define GAME_WEAPONBULLET_H

#include <boost/function.hpp>

#include "Game/WeaponLockonTarget.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class CharInfo;

namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliAttack;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

/* Fn8040CopyVector3, a vector member copied through the out-of-line vector functions. */
#include "Game/Fn8040CopyVector3.h"

namespace Weapon {
namespace Bullet {

/* Where a bullet is and where it flies (struct name from the RTTI name of
 * Game/PlayerShadowWeaponGun.cpp's bound callback, 0x804F30CC; layout from its constructors and
 * assignment, 0x80052390, 0x80052914, 0x80053878 and 0x8005246C; member names inferred). */
struct BulletPosition {
    BulletPosition();
    BulletPosition(const BulletPosition &other);
    BulletPosition(const Vec3f &position, const Vec3f &direction, const Vec3f &rotation);
    BulletPosition &operator=(const BulletPosition &other);

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_direction; /* velocity per second */
    /* 0x18 */ Vec3f m_rotation;
};
SIZE_ASSERT(BulletPosition, 0x24);

/* What a bullet's move hit (struct name unknown; layout from its copy constructor 0x80052844 and
 * destructor 0x800528C4; member names inferred). Both are inline functions that call other inline
 * functions, so they stay out of line (weak copies). */
struct HitResult {
    /* A result with zero vectors and no target, which hit or not (0x80051B58). */
    HitResult(bool hit);
    HitResult(const HitResult &other)
        : m_position(other.m_position), m_normal(other.m_normal), field_0x18(other.field_0x18),
          m_target(other.m_target), field_0x2C(other.field_0x2C), field_0x30(other.field_0x30),
          m_hit(other.m_hit), m_struck(other.m_struck), field_0x38(other.field_0x38) {}

    /* 0x00 */ Fn8040CopyVector3 m_position;
    /* 0x0C */ Fn8040CopyVector3 m_normal;
    /* 0x18 */ Fn8040CopyVector3 field_0x18;
    /* 0x24 */ boost::weak_ptr<CharInfo> m_target;
    /* 0x2C */ u32 field_0x2C;
    /* 0x30 */ u32 field_0x30;
    /* 0x34 */ u8 m_hit;    /* the move hit something */
    /* 0x35 */ u8 m_struck; /* what it hit counts as a hit on a target (meaning inferred) */
    /* 0x38 */ u32 field_0x38;
};
SIZE_ASSERT(HitResult, 0x3C);

/* What a bullet's hit callback receives: the hit and the bullet's state before and after the move
 * (struct name from the same RTTI name, which also gives the non-const reference; member names
 * inferred). Its constructor is an inline function emitted where it is
 * used. */
struct LaserReflectEventArgs {
    LaserReflectEventArgs(const HitResult &result, const BulletPosition &before, const BulletPosition &after)
        : m_result(result), m_before(before), m_after(after) {}

    /* 0x00 */ HitResult m_result;
    /* 0x3C */ BulletPosition m_before;
    /* 0x60 */ BulletPosition m_after;
};
SIZE_ASSERT(LaserReflectEventArgs, 0x84);

/* The callback a bullet calls when it hits something; it returns the bullet's new state (struct name
 * unknown). A struct around the boost::function: its implicit default and copy constructors,
 * assignment and destructor are out of line (0x80051EB4, 0x80221808, 0x80221160 and 0x80051DBC). */
struct HitCallback : public boost::function1<BulletPosition, LaserReflectEventArgs &> {
#ifdef WEAPON_LASER_SET_HIT_CALLBACK
    /* Assignment from the plain function (a weak copy of its own; compiled only where
     * WEAPON_LASER_SET_HIT_CALLBACK is defined, see WeaponLaserBase::SetHitCallback). */
    HitCallback &operator=(const boost::function1<BulletPosition, LaserReflectEventArgs &> &function) {
        boost::function1<BulletPosition, LaserReflectEventArgs &>::operator=(function);
        return *this;
    }
#endif
    /* Whether a function is set (names inferred). Each calls an inline function, so both stay out
     * of line, emitted where they are used. */
    bool HasFunction() const {
        return !empty();
    }
    bool IsSet() const {
        return HasFunction();
    }
    /* Calls the function, or returns a default state when none is set (name inferred). */
    BulletPosition Call(LaserReflectEventArgs &context) const {
        if (empty()) {
            return BulletPosition();
        }
        return (*this)(context);
    }
};
SIZE_ASSERT(HitCallback, 0xC);

} // namespace Bullet

/* What a weapon passes to the bullet it fires (struct name unknown; layout from its constructors
 * 0x80051D10 and 0x800693AC; member names inferred). Its copy constructor is an inline function,
 * first emitted by Game/WeaponLaserRifle.cpp (0x80221790). */
struct ShotInfo {
    /* The shot from position toward direction (0x80069218; parameter names inferred from
     * EggLastMech, which passes a comparison as the side). */
    ShotInfo(const boost::weak_ptr<CharInfo> &owner, const boost::shared_ptr<LockonTarget> &target,
             bool side, const Vec3f &position, const Vec3f &direction,
             const Vec3f &aim);
    /* The same shot without a target (0x800693AC; BA GUNSHIP passes true as the side). */
    ShotInfo(const boost::weak_ptr<CharInfo> &owner, bool side, const Vec3f &position,
             const Vec3f &direction, const Vec3f &aim);
    /* The same shot aimed along the owner's velocity, or with a zero aim when the owner is gone
     * (0x80051FC8; EGG_BALLOON passes true as the side). */
    ShotInfo(const boost::weak_ptr<CharInfo> &owner, bool side, const Vec3f &position,
             const Vec3f &direction);
    /* Written out: an implicit copy constructor calls the members' inline copy constructors out
     * of line. */
    ShotInfo(const ShotInfo &other)
        : m_owner(other.m_owner), m_target(other.m_target), m_position(other.m_position),
          m_direction(other.m_direction), m_aim(other.m_aim), m_side(other.m_side),
          m_callback(other.m_callback) {}

    /* 0x00 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x08 */ boost::shared_ptr<LockonTarget> m_target;
    /* 0x10 */ Fn8040CopyVector3 m_position;
    /* 0x1C */ Fn8040CopyVector3 m_direction;
    /* 0x28 */ Fn8040CopyVector3 m_aim;
    /* 0x34 */ s32 m_side; /* 0 when fired by a player (inferred) */
    /* 0x38 */ Bullet::HitCallback m_callback;
};
SIZE_ASSERT(ShotInfo, 0x44);

namespace Bullet {

/* BulletBase (class name from RTTI; vtable 0x8051D840): a bullet in flight. Its constructor
 * (0x8005230C) registers it with the bullet manager and its destructor (0x80052240) unregisters it.
 * Member names are inferred. */
class BulletBase {
  public:
    BulletBase(const ShotInfo &info);
    virtual ~BulletBase();
    /* Per-frame work. */
    virtual void Tick(float dt) = 0;

    /* 0x04 */ BulletPosition m_state;
    /* 0x28 */ s32 m_side;
    /* 0x2C */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x34 */ s32 m_attackKind; /* 3 for a player's bullet, 6 otherwise */
    /* 0x38 */ float m_distance;  /* how far the bullet has flown */
};
SIZE_ASSERT(BulletBase, 0x3C);

} // namespace Bullet
} // namespace Weapon

#endif

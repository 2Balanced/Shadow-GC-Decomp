/* @unnamed@PlayerShadowWeaponGun_cpp@::ControlGun (class and file name from RTTI): how Shadow
 * handles a gun (Weapon::WeaponGunBase). While the fire button is held it raises the arm and fires,
 * at the locked-on target when there is one, after warning that target; it takes the bullets fired
 * from the ammo and drops the gun once the ammo runs out. Laser guns get a hit callback: types 25
 * and 26 bounce off what they hit, types 27, 63 and 64 also turn toward the nearest target. Method
 * and member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * The callback type's RTTI name (.rodata 0x804F30CC) gives the names Weapon::Bullet::BulletPosition
 * and Weapon::Bullet::LaserReflectEventArgs and the non-const reference. The unused weak copy of
 * dynamic_pointer_cast<WeaponGunBase> (kept at 0x801AB998) leaves a tag object in this unit's
 * .sbss that the linker drops. */
/* Enables boost::static_pointer_cast (see boost/shared_ptr.hpp), and
 * WeaponLaserBase::SetHitCallback (see Game/WeaponBase.h). */
#define BOOST_SP_STATIC_POINTER_CAST
/* Keeps the dead copy of bad_alloc's vtable in .data, as retail has it (see <new>). */
#define MSL_NEW_INLINE_BAD_ALLOC
#define WEAPON_LASER_SET_HIT_CALLBACK
/* shared_ptr.hpp before bind.hpp: weak template instances come out in the order their templates
 * were declared, and retail has static_pointer_cast ahead of boost::bind. */
#include "boost/shared_ptr.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/DebugDraw.h"
#include "Game/Player.h"
#include "Game/PlayerShadowWeapon.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/TargetSearchList.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponBullet.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Target (class name from RTTI; vtable 0x8053DBA8): a point other objects can aim at. Slot 0x08
 * (0x801E73EC) returns the point at 0x8 (the argument, a position to aim from, is unused there);
 * the character it belongs to is at 0x28. */
class Target {
  public:
    virtual Vec3f GetPosition(const Vec3f &from);
    virtual ~Target();

    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner;
};

extern "C" {
/* A bit array: tests, sets and clears bit. */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Whether the motion holds the arm up to aim (behaviour inferred). */
bool fn_800AA640(Player::PlayerMotion *motion);
/* Drops target's character from the list (behaviour inferred). */
void fn_801E7C94(TargetSearchList *list, const boost::weak_ptr<CharInfo> &character);
/* Vector helpers (behaviour inferred): a . b, normalising in place, the normalised vector,
 * vector * scale, |vector|. */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040CE74(Vec3f *vector);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* A range (name inferred). As a call it is not a constant expression, so the value below is
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* How far a homing bullet looks for a new target (name inferred). */
static float s_homingRange = Span(100.0f);

namespace {

using Player::Shadow::ControlWeaponBase;
using Player::Shadow::ShadowWeapon;
using Weapon::Bullet::BulletPosition;
using Weapon::Bullet::LaserReflectEventArgs;

typedef boost::function1<BulletPosition, LaserReflectEventArgs &> HitFunction;

/* The bullet's velocity mirrored at the hit surface (name inferred). */
#define Reflect(context)                                                                    \
    (const Vec3f &)((VectorSubtractFn)fn_8040CD74)(                                         \
        &(context).m_before.m_direction,                                                    \
        &(const Vec3f &)fn_8040CDA8(                                                        \
            &(const Vec3f &)fn_8040CDA8(                                                    \
                reinterpret_cast<const Vec3f *>(&(context).m_result.m_normal),              \
                fn_8040CF90(&(context).m_before.m_direction,                                \
                            reinterpret_cast<const Vec3f *>(&(context).m_result.m_normal))), \
            2.0f))

/* Where the bullet hit (name inferred). */
#define HitPosition(context) reinterpret_cast<const Vec3f *>(&(context).m_result.m_position)

class ControlGun : public ControlWeaponBase {
  public:
    ControlGun(ShadowWeapon *owner, const boost::shared_ptr<Weapon::WeaponBase> &weapon);
    virtual ~ControlGun();
    virtual void Update(float dt);

    /* Tells the locked-on target it is being attacked. */
    void AnnounceAttack();
    /* The laser hit callbacks: bounce, and bounce toward the nearest target. */
    BulletPosition OnHitBounce(LaserReflectEventArgs &context);
    BulletPosition OnHitHoming(LaserReflectEventArgs &context);

    /* 0x08 */ boost::shared_ptr<Weapon::WeaponGunBase> m_gun;
    /* 0x10 */ float m_reload;
    /* 0x14 */ s32 m_shots;
};
SIZE_ASSERT(ControlGun, 0x18);

ControlGun::ControlGun(ShadowWeapon *owner, const boost::shared_ptr<Weapon::WeaponBase> &weapon)
    : ControlWeaponBase(owner) {
    m_gun = boost::dynamic_pointer_cast<Weapon::WeaponGunBase>(weapon);
    m_reload = 0.0f;
    m_shots = 0;
    if (m_gun->field_0x18 == 6) {
        boost::shared_ptr<Weapon::WeaponLaserBase> laser =
            boost::static_pointer_cast<Weapon::WeaponLaserBase>(m_gun);
        switch (laser->TypeId()) {
        case 25:
        case 26:
            laser->SetHitCallback(HitFunction(boost::bind(&ControlGun::OnHitBounce, this, _1)));
            break;
        case 27:
        case 63:
        case 64:
            laser->SetHitCallback(HitFunction(boost::bind(&ControlGun::OnHitHoming, this, _1)));
            break;
        }
    }
    fn_80076CA8(m_owner->m_player->m_stateBits, 0x41);
}

ControlGun::~ControlGun() {
    fn_80076CC8(m_owner->m_player->m_stateBits, 0x41);
    if (m_owner->m_flags.Test(2)) {
        if (m_reload < 0.0f) {
            m_reload = 0.0f;
        }
        m_owner->m_flags.Clear(2);
        m_owner->LowerArm();
    }
}

void ControlGun::Update(float dt) {
    if (m_reload > 0.0f) {
        m_reload -= dt;
        if (m_reload <= 0.0f) {
            m_reload = 0.0f;
        }
    }
    if (m_owner->m_flags.Test(0) && !m_owner->m_flags.Test(2)) {
        if (!m_owner->m_flags.Test(2) && !(m_reload > 0.0f)) {
            m_shots = 0;
            m_owner->m_flags.Set(2);
            m_owner->RaiseArm();
        }
        m_gun->vf_0x3C();
    }
    if (m_owner->m_flags.Test(2) && fn_800AA640(m_owner->m_player->m_motion)) {
        if (m_reload == 0.0f && !fn_8006CD78(m_owner->m_player->m_stateBits, 0x11) &&
            !fn_8006CD78(m_owner->m_player->m_stateBits, 0x12)) {
            AnnounceAttack();
            if (m_owner->m_player->m_target && m_owner->m_player->m_target->HasTarget()) {
                m_reload = m_gun->FireAt(
                    &m_owner->m_player->field_0x18,
                    &(const Vec3f &)m_owner->m_player->m_target->GetTargetPosition());
            } else {
                m_reload = m_gun->Fire(&m_owner->m_player->field_0x18);
            }
        }
        if (!m_owner->m_flags.Test(0) && !fn_8006CD78(m_owner->m_player->m_stateBits, 0x11) &&
            !fn_8006CD78(m_owner->m_player->m_stateBits, 0x12)) {
            m_reload = m_gun->vf_0x38(&m_owner->m_player->field_0x18);
        }
    }
    if (m_gun->ShotCount()) {
        m_shots++;
        m_owner->m_flags.Set(3);
    }
    if (m_owner->m_flags.Test(1) || (!m_owner->m_flags.Test(0) && m_shots > 0)) {
        if (m_owner->m_flags.Test(2)) {
            if (m_reload < 0.0f) {
                m_reload = 0.0f;
            }
            m_owner->m_flags.Clear(2);
            m_owner->LowerArm();
        }
        m_gun->vf_0x40();
    }
    m_owner->AddAmmo(-m_gun->field_0x38);
    if (!m_owner->HasAmmo()) {
        m_owner->DropWeapon();
    }
}

void ControlGun::AnnounceAttack() {
    if (m_owner->m_player->m_target) {
        boost::weak_ptr<Target> target = m_owner->m_player->m_target->GetTarget();
        if (!target.expired()) {
            boost::weak_ptr<CharInfo> character = target.lock()->m_owner;
            if (!character.expired()) {
                character.lock()->PushCommand(
                    new AnnounceAttackCommand(m_owner->m_player, 4, 4));
            }
        }
    }
}

BulletPosition ControlGun::OnHitBounce(LaserReflectEventArgs &context) {
    BulletPosition next(context.m_after);
    if (context.m_result.m_struck) {
        fn_800091D0(&next.m_direction, &Reflect(context));
    } else {
        fn_800091D0(&next.m_direction, &context.m_before.m_direction);
    }
    return next;
}

BulletPosition ControlGun::OnHitHoming(LaserReflectEventArgs &context) {
    BulletPosition next(context.m_after);
    Vec3f direction;
    if (context.m_result.m_struck) {
        fn_800091D0(&direction, &Reflect(context));
    } else {
        fn_800091D0(&direction, &context.m_before.m_direction);
    }
    fn_8040CE74(&direction);
    {
        boost::shared_ptr<TargetSearchList> list(fn_801E7930(fn_80060058()));
        fn_801E80C0(list.get(), 0x800);
        fn_801E7FB4(list.get(), HitPosition(context), 0.0f, &direction);
        fn_801E7C94(list.get(), context.m_result.m_target);
        fn_801E8218(list.get(), HitPosition(context), s_homingRange);
        boost::weak_ptr<Target> target = fn_801E848C(list.get(), HitPosition(context));
        if (!target.expired()) {
            boost::shared_ptr<Target> locked = target.lock();
            UnkSphere_8004E6CC sphere;
            fn_800091D0(&sphere.m_center, &(const Vec3f &)locked->GetPosition(*HitPosition(context)));
            sphere.m_radius = 0.7f;
            fn_802E0F3C(&sphere, &lbl_805F06D8);
            fn_800091D0(&direction,
                        &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                            &(const Vec3f &)locked->GetPosition(*HitPosition(context)), HitPosition(context)));
        }
    }
    float speed = fn_8040CF50(&context.m_before.m_direction);
    fn_800091D0(&next.m_direction,
                &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&direction), speed));
    return next;
}

} // namespace

namespace Player {
namespace Shadow {

ControlWeaponBase *CreateControlGun(ShadowWeapon *owner,
                                    const boost::shared_ptr<Weapon::WeaponBase> &weapon) {
    return new ControlGun(owner, weapon);
}

} // namespace Shadow
} // namespace Player

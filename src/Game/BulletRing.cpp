/* Ring (class name from RTTI: Weapon::Bullet::Ring): a ring-shaped shot (bullet record
 * lbl_8051DEB0). It flies straight with an effect, sweeping a capsule attack collider from where it
 * was to where it is each frame; when its attack hits, it shows its hit effect and is deleted on the
 * next tick; otherwise it disappears after its range. File name inferred from the class name;
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using Weapon::ShotInfo;
using Weapon::Bullet::BulletBase;
using Weapon::Bullet::BulletPosition;
using Weapon::Bullet::HitResult;

extern "C" {
/* The starting state of a bullet fired with info at speed (see Game/BulletLaser.cpp). */
BulletPosition fn_8005A228(const ShotInfo &info, float speed);
/* Sets up an attack collision: its kind, flags and power, and its callbacks
 * (Game/BulletCannonBomb.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int kind, u32 flags, float power, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
/* The collider manager, a capsule collider of radius around position, and its attack collision
 * (see Game/BulletCannonBomb.cpp; the meanings of the other arguments are unknown). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7B4C(void *manager, const Vec3f *position, const Vec3f *rotation,
                                              int arg3, u32 flags, int kind, int arg6, float halfLength,
                                              float radius, boost::weak_ptr<CharInfo> owner,
                                              const Vec3f *offset, const Vec3f *axis, int arg11);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Starts effect id at position, facing direction, for owner (see Game/BulletLaser.cpp). */
boost::weak_ptr<EffectElement> fn_8005E32C(const Vec3f *position, const Vec3f *direction, int id,
                                           const boost::weak_ptr<CharInfo> &owner);
/* Moves and turns an effect (see Game/BulletLaser.cpp). */
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
/* The distance between two points (behaviour inferred). */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CDA8(const Vec3f &vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f &a, const Vec3f &b);   /* a + b */
Vec3f fn_8040CEE8(const Vec3f &vector);              /* normalised */
/* A direction turned around, and into the angles of a rotation (see Game/BulletLaser.cpp). */
Vec3f fn_80016284(const Vec3f &direction);
Vec2f fn_8040F760(const Vec3f &direction);
}

/* A vector constant read by the collision setup (see Game/CatchStick.cpp). */
extern const Vec3f lbl_805E07B8;

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Returns x unchanged (name inferred). Being initialised through a function call, the values below
 * are set up at startup (see Meteor.cpp). */
inline float Plain(float x) {
    return x;
}

/* A bullet type's record (struct name unknown; see Game/BulletLaser.cpp): a central table lists
 * them. */
struct UnkBulletType_80546018 {
    /* 0x00 */ void (*initialize)(void);
    /* 0x04 */ void (*finalize)(void);
    /* 0x08 */ void (*create)(const ShotInfo &info);
    /* What a shot would hit within time seconds (Game/BulletCannon.cpp). */
    /* 0x0C */ HitResult (*hitTest)(const ShotInfo &info, float time);
    /* 0x10 */ float m_reach;
};
SIZE_ASSERT(UnkBulletType_80546018, 0x14);

/* The speed and the radius of a ring. */
static float s_speed = Plain(100.0f);
static const float s_radius = Plain(20.0f);

namespace Weapon {
namespace Bullet {

class Ring : public BulletBase {
  public:
    Ring(const ShotInfo &info, int effect, float damage, float reach);
    virtual ~Ring();
    virtual void Tick(float dt);

    /* The state after flying for dt. */
    BulletPosition Move(const BulletPosition &state, float dt);
    /* The hit effect. */
    void OnHitTarget();
    /* The effect that follows the ring. */
    void StartEffect(int effect);
    void StopEffect();
    void UpdateEffect();
    /* Creates the collider and its attack collision (side is not used). */
    void SetupAttack(float damage, int side, boost::weak_ptr<CharInfo> owner);
    /* Stretches the collider from from to to. */
    void UpdateCollider(const Vec3f &from, const Vec3f &to);
    /* The callbacks of the attack collision. */
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    /* 0x3C */ float m_reach;
    /* 0x40 */ bool m_hit;
    /* 0x44 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x4C */ boost::weak_ptr<CharaColliAttack> m_attack;
    /* 0x54 */ boost::weak_ptr<EffectElement> m_effect;
};
SIZE_ASSERT(Ring, 0x5C);

Ring::Ring(const ShotInfo &info, int effect, float damage, float reach) : BulletBase(info), m_reach(reach) {
    m_state = fn_8005A228(info, s_speed);
    SetupAttack(damage, info.m_side, info.m_owner);
    StartEffect(effect);
    m_hit = false;
}

Ring::~Ring() {
    StopEffect();
}

BulletPosition Ring::Move(const BulletPosition &state, float dt) {
    return BulletPosition(fn_8040CDD0(state.m_position, fn_8040CDA8(state.m_direction, dt)), state.m_direction,
                          state.m_rotation);
}

void Ring::Tick(float dt) {
    if (m_hit) {
        OnHitTarget();
        delete this;
        return;
    }
    BulletPosition next = Move(m_state, dt);
    m_distance += fn_8040D1B4(&next.m_position, &m_state.m_position);
    UpdateCollider(m_state.m_position, next.m_position);
    m_state = next;
    UpdateEffect();
    if (m_distance >= m_reach) {
        delete this;
    }
}

void Ring::OnHitTarget() {
    fn_8005E32C(&m_state.m_position, &(const Vec3f &)fn_80016284(fn_8040CEE8(m_state.m_direction)), 0x126,
                m_owner);
}

void Ring::StartEffect(int effect) {
    m_effect = fn_8005E32C(&m_state.m_position, &m_state.m_direction, effect, m_owner);
}

void Ring::StopEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
        m_effect.reset();
    }
}

void Ring::UpdateEffect() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() != 0 && !effect->IsActive()) {
        m_effect.reset();
    } else if (effect.get() != 0) {
        fn_80425E4C(effect.get(), &m_state.m_position);
        Vec3f rotation(m_state.m_rotation.x, m_state.m_rotation.y, m_state.m_rotation.z);
        fn_80425D94(effect.get(), &rotation);
    }
}

void Ring::SetupAttack(float damage, int side, boost::weak_ptr<CharInfo> owner) {
    void *manager = fn_80021B08();
    Vec2f angles = fn_8040F760(m_state.m_direction);
    m_collider = fn_800C7B4C(manager, &lbl_805E07A0, &(const Vec3f &)Vec3f(angles.x, angles.y, 0.0f), 0, 0x40000,
                             m_attackKind, 0, 0.0f, s_radius, owner, &lbl_805E07A0, &lbl_805E07B8, 0);
    m_attack = fn_800C4374(manager, m_collider);
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack.get() != 0) {
        fn_80054DD0(attack.get(), 5, 0x40, damage, boost::bind(&Ring::OnHit, this, _1),
                    boost::bind(&Ring::OnAttackEnd, this, _1));
        attack->m_owner = owner;
    }
    UpdateCollider(m_state.m_position, m_state.m_position);
}

void Ring::UpdateCollider(const Vec3f &from, const Vec3f &to) {
    if (m_collider.get() != 0) {
        Fn8040CopyVector3 center(fn_8040CDA8(fn_8040CDD0(from, to), 0.5f));
        float length = fn_8040D1B4(&from, &to);
        /* Slot 0x3C, which takes the position here (see Game/BulletCannonBomb.cpp). */
        m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&center));
        m_collider->SetSize(s_radius, 0.0f, 0.5f * length, -2);
    }
}

void Ring::OnHit(AttackCallbackParam &param) {
    /* The direction and speed of the hit at 0x34 and 0x4C (meanings inferred; see Meteor.cpp). */
    AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)), fn_8040CEE8(m_state.m_direction));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = s_speed;
}

void Ring::OnAttackEnd(AttackCallbackParam &param) {
    switch (param.m_result) {
    case 0:
    case 2:
    case 4:
    case 6:
    case 7:
    case 8:
        break;
    case 1:
    case 3:
    case 5:
        m_hit = true;
        break;
    }
}

} // namespace Bullet
} // namespace Weapon

using Weapon::Bullet::Ring;

/* The type's load and release hooks, both empty, and its creation (names inferred). */
static void Initialize() {}

static void Finalize() {}

static void Create(const ShotInfo &info) {
    new Ring(info, 0x123, 4.0f, 400.0f);
}

UnkBulletType_80546018 lbl_8051DEB0 = {Initialize, Finalize, Create, 0, Plain(400.0f)};

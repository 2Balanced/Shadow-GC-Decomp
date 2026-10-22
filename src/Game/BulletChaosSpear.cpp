/* ChaosSpear (class name from RTTI: Weapon::Bullet::ChaosSpear::ChaosSpear): the bullet of the
 * Chaos Spear weapon (Game/WeaponChaosSpear.cpp). It flies straight with a PJS effect scaled by the
 * weapon's charge and an attack collision whose damage grows with the charge, and disappears when
 * it hits something or after its range. File name inferred from the class name. Method and member
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/BulletChaosSpear.h"
#include "Game/CharaColliAttack.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using Weapon::ShotInfo;
using Weapon::Bullet::BulletBase;
using Weapon::Bullet::BulletPosition;
using Weapon::Bullet::HitResult;

extern "C" {
/* The starting state of a bullet fired with info at speed (see BulletLaser.cpp). */
BulletPosition fn_8005A228(const ShotInfo &info, float speed);
/* Moves a bullet from before to after and tests what it hits on the way (behaviour inferred). */
HitResult fn_8005AFE4(int kind, const BulletPosition *before, const BulletPosition *after,
                      boost::shared_ptr<CharaColliAttack> *attack);
/* Creates the attack collision of a bullet (see BulletLaser.cpp). */
boost::shared_ptr<CharaColliAttack> fn_8005A300(int side, boost::weak_ptr<CharInfo> owner, float damage,
                                                const AttackCallback &onHit, const AttackCallback &onEnd);
/* Makes an attack collision ignore another (see BulletLaser.cpp). */
void fn_800532F8(CharaColliAttack *attack, boost::weak_ptr<CharaColliAttack> other);
/* Releases an attack collision (behaviour inferred; see ThrowObject.cpp). */
void fn_80053B40(boost::shared_ptr<CharaColliAttack> *attack);
/* The id effects launched for a character carry (behaviour inferred). */
u32 fn_8005E4B8(const boost::weak_ptr<CharInfo> &owner);
/* Launches a PJS effect, and one at a position facing a direction (see EffectPJS.cpp). */
boost::weak_ptr<EffectElement> fn_803056F8(Effect::PJS::Launcher *launcher, s32 arg, u32 owner);
boost::weak_ptr<EffectElement> fn_8030536C(Effect::PJS::Launcher *launcher, const Vec3f *position,
                                           const Vec3f &direction, s32 arg, u32 owner);
/* Places an effect by a matrix (see EffectPJS.cpp). */
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
/* A direction turned into the angles of a rotation (see EffectPJS.cpp). */
Vec3f fn_80016284(const Vec3f &direction);
Vec2f fn_8040F664(const Vec3f &rotation);
/* The distance between two points (behaviour inferred). */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CDA8(const Vec3f &vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f &a, const Vec3f &b);   /* a + b */
Vec3f fn_8040CEE8(const Vec3f &vector);              /* normalised */
}

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Returns x unchanged (name inferred). Being initialised through a function call, the values below
 * are set up at startup (see Meteor.cpp). */
inline float Plain(float x) {
    return x;
}

static BulletPosition Propagate(const BulletPosition &state, float dt);

namespace Weapon {
namespace Bullet {
namespace ChaosSpear {

/* The speed and the range of a spear. */
static const float s_speed = Plain(500.0f);
static const float s_range = Plain(400.0f);

class ChaosSpear : public BulletBase {
  public:
    ChaosSpear(const ShotInfo &info, const boost::shared_ptr<Effect::PJS::Launcher> &launcher,
               float charge);
    virtual ~ChaosSpear();
    virtual void Tick(float dt);

    /* The effect that follows the spear. */
    void StartEffect();
    void StopEffect();
    void UpdateEffect();
    /* The hit effect. */
    void OnHitTarget(const HitResult &result);
    /* The callbacks of the attack collision. */
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    /* 0x3C */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0x44 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x4C */ float m_scale; /* of the effect */
    /* 0x50 */ boost::shared_ptr<Effect::PJS::Launcher> m_launcher;
    /* 0x58 */ float m_charge;
    /* 0x5C */ float m_damage;
    /* 0x60 */ u8 m_full; /* fully charged */
};
SIZE_ASSERT(ChaosSpear, 0x64);

ChaosSpear::ChaosSpear(const ShotInfo &info, const boost::shared_ptr<Effect::PJS::Launcher> &launcher,
                       float charge)
    : BulletBase(info), m_launcher(launcher), m_charge(charge) {
    m_state = fn_8005A228(info, s_speed);
    if (m_charge >= 0.9995f) {
        m_charge = 1.0f;
        m_full = true;
    } else {
        m_full = false;
    }
    m_damage = 6.0f + 14.0f * m_charge;
    m_attack = fn_8005A300(info.m_side, info.m_owner, m_damage, boost::bind(&ChaosSpear::OnHit, this, _1),
                           boost::bind(&ChaosSpear::OnAttackEnd, this, _1));
    StartEffect();
}

ChaosSpear::~ChaosSpear() {
    if (m_attack.get() != 0) {
        fn_80053B40(&m_attack);
    }
    StopEffect();
}

void ChaosSpear::Tick(float dt) {
    BulletPosition next = Propagate(m_state, dt);
    m_distance += fn_8040D1B4(&next.m_position, &m_state.m_position);
    HitResult result = fn_8005AFE4(m_attackKind, &m_state, &next, &m_attack);
    if (result.m_hit) {
        OnHitTarget(result);
        AssignVector(next.m_position, (const Vec3f &)result.m_position);
    }
    m_state = next;
    UpdateEffect();
    if (result.m_hit || m_distance >= s_range) {
        delete this;
    }
}

} // namespace ChaosSpear
} // namespace Bullet
} // namespace Weapon

/* The state of a bullet after flying for dt (name inferred). */
static BulletPosition Propagate(const BulletPosition &state, float dt) {
    return BulletPosition(fn_8040CDD0(state.m_position, fn_8040CDA8(state.m_direction, dt)), state.m_direction,
                          state.m_rotation);
}

namespace Weapon {
namespace Bullet {
namespace ChaosSpear {

void ChaosSpear::OnHitTarget(const HitResult &result) {
    fn_8030536C(m_launcher.get(), (const Vec3f *)&result.m_position, (const Vec3f &)result.m_normal,
                m_full ? 0x13 : 0x2AB, fn_8005E4B8(m_owner));
}

void ChaosSpear::StartEffect() {
    int effect;
    if (m_full) {
        m_scale = 1.0f;
        effect = 10;
    } else {
        m_scale = 1.0f + m_charge;
        effect = 3;
    }
    m_effect = fn_803056F8(m_launcher.get(), effect, fn_8005E4B8(m_owner));
    UpdateEffect();
}

void ChaosSpear::StopEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
        m_effect.reset();
    }
}

void ChaosSpear::UpdateEffect() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() != 0) {
        Mtx34f matrix;
        Mtx34f place;
        SetTranslationMtx(&place, m_state.m_position);
        fn_8040C148(&matrix, &place);
        Vec2f angles = fn_8040F664(fn_80016284(m_state.m_direction));
        fn_8040B884(&matrix, angles.x);
        fn_8040B848(&matrix, angles.y);
        fn_8040B720(&matrix, m_scale, m_scale, m_scale);
        fn_80425E68(effect.get(), &matrix);
    }
}

void ChaosSpear::OnHit(AttackCallbackParam &param) {
    /* The direction and speed of the hit at 0x34 and 0x4C (meanings inferred; see Meteor.cpp). */
    AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)), fn_8040CEE8(m_state.m_direction));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = s_speed;
}

void ChaosSpear::OnAttackEnd(AttackCallbackParam &param) {
    /* What the attack hit at 0x8 (meaning inferred). */
    fn_800532F8(m_attack.get(), *reinterpret_cast<const boost::weak_ptr<CharaColliAttack> *>(param.field_0x8));
    /* The result of the move being tested, at 0x58 (meaning inferred). */
    HitResult *result = *reinterpret_cast<HitResult **>(reinterpret_cast<u8 *>(&param) + 0x58);
    result->field_0x38 = param.m_result;
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
        result->m_hit = 1;
        break;
    }
}

float GetRange() {
    return s_range;
}

void Fire(const ShotInfo &info, const boost::shared_ptr<Effect::PJS::Launcher> &launcher, float charge) {
    new ChaosSpear(info, launcher, charge);
}

} // namespace ChaosSpear
} // namespace Bullet
} // namespace Weapon

/* BulletGun (class name from RTTI: Weapon::Bullet::BulletGun): the bullet of the guns, in fourteen
 * types (bullet records lbl_8051DB34 to lbl_8051DC38). A bullet flies straight with an effect and
 * an attack collision; when it hits something it shows its hit effect (or, for the type 3 hits,
 * leaves a cannon blast, Game/BulletCannonBomb.cpp), releases its collision on the next frame and
 * is deleted on the one after; otherwise it disappears after its range. Two types have hit tests
 * that fly a bullet ahead of time. File name inferred from the class name; other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * Not yet matching: the static initializer (0x80056A88) has retail's instructions, registers and
 * constants, but GC/2.7 schedules the two stores to the speeds after the last store to the records,
 * where retail stores them right after restoring r28-r31 (between the stores to the fourth and the
 * fifth record). Defining the speeds there instead changes the constant order and the registers;
 * this looks like the scheduling difference of the later compiler described in
 * docs/research-notes.md. Everything else, including the data, matches. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/AttackIgnoreList.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/BulletCannonBomb.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/GeometryLine.h"
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
/* The starting state of a bullet fired with info at speed (see Game/BulletLaser.cpp). */
BulletPosition fn_8005A228(const ShotInfo &info, float speed);
/* Tests what a bullet hits where it starts, with its attack collision (see Game/BulletLaser.cpp). */
HitResult fn_8005B1F8(const ShotInfo &info, int kind, boost::shared_ptr<CharaColliAttack> *attack);
/* Moves a bullet from before to after and tests what it hits on the way, for the player filter, and
 * with the attack collision (see Game/BulletCannon.cpp). */
HitResult fn_8005B2F0(int kind, const BulletPosition *before, const BulletPosition *after, s32 player);
HitResult fn_8005AFE4(int kind, const BulletPosition *before, const BulletPosition *after,
                      boost::shared_ptr<CharaColliAttack> *attack);
/* Creates the attack collision of a bullet (see Game/BulletLaser.cpp). */
boost::shared_ptr<CharaColliAttack> fn_8005A300(int side, boost::weak_ptr<CharInfo> owner, float damage,
                                                const AttackCallback &onHit, const AttackCallback &onEnd);
/* Starts effect id at position, facing direction, for owner (see Game/BulletLaser.cpp). */
boost::weak_ptr<EffectElement> fn_8005E32C(const Vec3f *position, const Vec3f *direction, int id,
                                           const boost::weak_ptr<CharInfo> &owner);
/* Starts the hit effect of bullet kind kind at hit (Game/BulletHitEffect.cpp). */
void fn_802B3FE8(int kind, const HitResult *hit);
/* Moves and turns an effect (see Game/BulletLaser.cpp). */
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
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

/* The speeds of the bullets. */
static const float s_speed = Plain(500.0f);
static const float s_slowSpeed = Plain(300.0f);

extern UnkBulletType_80546018 lbl_8051DB34;
extern UnkBulletType_80546018 lbl_8051DB48;
extern UnkBulletType_80546018 lbl_8051DB5C;
extern UnkBulletType_80546018 lbl_8051DB70;
extern UnkBulletType_80546018 lbl_8051DB84;
extern UnkBulletType_80546018 lbl_8051DB98;
extern UnkBulletType_80546018 lbl_8051DBAC;
extern UnkBulletType_80546018 lbl_8051DBC0;
extern UnkBulletType_80546018 lbl_8051DBD4;
extern UnkBulletType_80546018 lbl_8051DBE8;
extern UnkBulletType_80546018 lbl_8051DBFC;
extern UnkBulletType_80546018 lbl_8051DC10;
extern UnkBulletType_80546018 lbl_8051DC24;
extern UnkBulletType_80546018 lbl_8051DC38;

static BulletPosition Propagate(const BulletPosition &state, float dt);

namespace Weapon {
namespace Bullet {

class BulletGun : public BulletBase {
  public:
    BulletGun(const ShotInfo &info, float damage, float reach, float speed, int effect, int hitKind);
    virtual ~BulletGun();
    virtual void Tick(float dt);

    /* Flies for dt and tests what the bullet hits. */
    void Move(float dt);
    /* Releases the attack collision and the effect. */
    void Release();
    /* The effect that follows the bullet. */
    void StartEffect(int effect);
    void StopEffect();
    void UpdateEffect();
    /* The hit effect, or the cannon blast. */
    void OnHitTarget(const HitResult &result);
    /* The callbacks of the attack collision. */
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    /* 0x3C */ s32 m_phase; /* 0 flying, 1 hit, 2 done */
    /* 0x40 */ float m_reach;
    /* 0x44 */ float m_speed;
    /* 0x48 */ s32 m_attackState; /* 2 while flying, 1 once the attack hit (meaning inferred) */
    /* 0x4C */ s32 m_hitKind;
    /* 0x50 */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0x58 */ boost::weak_ptr<EffectElement> m_effect;
};
SIZE_ASSERT(BulletGun, 0x60);

BulletGun::BulletGun(const ShotInfo &info, float damage, float reach, float speed, int effect, int hitKind)
    : BulletBase(info), m_reach(reach), m_speed(speed), m_hitKind(hitKind) {
    m_state = fn_8005A228(info, m_speed);
    m_attack = fn_8005A300(info.m_side, info.m_owner, damage, boost::bind(&BulletGun::OnHit, this, _1),
                           boost::bind(&BulletGun::OnAttackEnd, this, _1));
    switch (m_hitKind) {
    case 2:
    case 3:
        break;
    default: {
        HitResult result = fn_8005B1F8(info, m_attackKind, &m_attack);
        if (result.m_hit) {
            OnHitTarget(result);
            delete this;
            return;
        }
    } break;
    }
    StartEffect(effect);
    m_phase = 0;
}

BulletGun::~BulletGun() {
    Release();
}

} // namespace Bullet
} // namespace Weapon

/* The state of a bullet after flying for dt (name inferred). */
static BulletPosition Propagate(const BulletPosition &state, float dt) {
    return BulletPosition(fn_8040CDD0(state.m_position, fn_8040CDA8(state.m_direction, dt)), state.m_direction,
                          state.m_rotation);
}

namespace Weapon {
namespace Bullet {

void BulletGun::Tick(float dt) {
    switch (m_phase) {
    case 0:
        Move(dt);
        break;
    case 1:
        Release();
        m_phase++;
        break;
    case 2:
        delete this;
        break;
    }
}

void BulletGun::Move(float dt) {
    BulletPosition next = Propagate(m_state, dt);
    m_distance += fn_8040D1B4(&next.m_position, &m_state.m_position);
    m_attackState = 2;
    HitResult result = fn_8005AFE4(m_attackKind, &m_state, &next, &m_attack);
    if (result.m_hit) {
        OnHitTarget(result);
        AssignVector(next.m_position, (const Vec3f &)result.m_position);
        m_phase++;
    }
    m_state = next;
    UpdateEffect();
    if (m_distance >= m_reach) {
        delete this;
    }
}

void BulletGun::Release() {
    if (m_attack.get() != 0) {
        fn_80053B40(&m_attack);
    }
    StopEffect();
}

void BulletGun::OnHitTarget(const HitResult &result) {
    switch (m_hitKind) {
    case 0:
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0xD7, m_owner);
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0xD8, m_owner);
        fn_802B3FE8(1, &result);
        break;
    case 1:
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0xDD, m_owner);
        fn_802B3FE8(2, &result);
        break;
    case 2:
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0xD7, m_owner);
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0xD8, m_owner);
        fn_802B3FE8(1, &result);
        break;
    case 3:
        StartCannonBomb(*this, 6, m_attack);
        break;
    case 4:
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0x286, m_owner);
        break;
    case 5:
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0x2FE, m_owner);
        break;
    case 6:
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0x305, m_owner);
        break;
    case 7:
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0x279, m_owner);
        fn_802B3FE8(1, &result);
        break;
    }
}

void BulletGun::OnHit(AttackCallbackParam &param) {
    /* The direction and speed of the hit at 0x34 and 0x4C (meanings inferred; see Meteor.cpp). */
    AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)), fn_8040CEE8(m_state.m_direction));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = m_speed;
}

void BulletGun::OnAttackEnd(AttackCallbackParam &param) {
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
        m_attackState = 1;
        result->m_hit = 1;
        break;
    }
}

void BulletGun::StartEffect(int effect) {
    if (effect >= 0) {
        m_effect = fn_8005E32C(&m_state.m_position, &m_state.m_direction, effect, m_owner);
    }
    UpdateEffect();
}

void BulletGun::StopEffect() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() != 0) {
        effect->Retire();
        m_effect.reset();
    }
}

void BulletGun::UpdateEffect() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() != 0) {
        fn_80425E4C(effect.get(), &m_state.m_position);
        Vec3f rotation(m_state.m_rotation.x, m_state.m_rotation.y, m_state.m_rotation.z);
        fn_80425D94(effect.get(), &rotation);
    }
}

} // namespace Bullet
} // namespace Weapon

using Weapon::Bullet::BulletGun;

/* Fires a bullet of each type (names inferred). */
static void Create0(const ShotInfo &info) {
    new BulletGun(info, 2.0f, lbl_8051DB34.m_reach, s_speed, 0xE2, 0);
}

static void Create1(const ShotInfo &info) {
    new BulletGun(info, 2.0f, lbl_8051DB48.m_reach, s_speed, 0x1C3, 0);
}

static void Create2(const ShotInfo &info) {
    new BulletGun(info, 4.0f, lbl_8051DB5C.m_reach, s_speed, 0xE6, 0);
}

static void Create3(const ShotInfo &info) {
    new BulletGun(info, 6.0f, lbl_8051DB70.m_reach, s_speed, 0xE6, 0);
}

static void Create5(const ShotInfo &info) {
    new BulletGun(info, 2.0f, lbl_8051DB98.m_reach, s_speed, 0x185, 0);
}

static void Create6(const ShotInfo &info) {
    new BulletGun(info, 2.0f, lbl_8051DBAC.m_reach, s_speed, 0xEA, 1);
}

static void Create7(const ShotInfo &info) {
    new BulletGun(info, 2.0f, lbl_8051DBC0.m_reach, s_speed, 0xEA, 1);
}

static void Create8(const ShotInfo &info) {
    new BulletGun(info, 5.0f, lbl_8051DBD4.m_reach, s_speed, 0x282, 1);
}

static void Create9(const ShotInfo &info) {
    new BulletGun(info, 6.0f, lbl_8051DBE8.m_reach, s_speed, 0xE6, 2);
}

static void Create10(const ShotInfo &info) {
    new BulletGun(info, 8.0f, lbl_8051DBFC.m_reach, s_speed, 0x130, 3);
}

static void Create11(const ShotInfo &info) {
    new BulletGun(info, 4.0f, lbl_8051DC10.m_reach, s_speed, 0x1FA, 5);
}

static void Create12(const ShotInfo &info) {
    new BulletGun(info, 8.0f, lbl_8051DC24.m_reach, s_speed, 0x2FA, 6);
}

static void Create13(const ShotInfo &info) {
    new BulletGun(info, 32.0f, lbl_8051DC38.m_reach, s_speed, 0x277, 7);
}

/* What a bullet fired with info at speed would hit within time seconds and range (name inferred;
 * see Game/BulletCannon.cpp). */
static HitResult HitTest(const ShotInfo &info, float range, float speed, float time) {
    if (info.m_owner.expired()) {
        return HitResult(false);
    }
    s32 player = info.m_owner.lock()->field_0x54;
    BulletPosition state = fn_8005A228(info, speed);
    BulletPosition next;
    int kind;
    if (info.m_side == 0) {
        kind = 3;
    } else {
        kind = 6;
    }
    float distance = 0.0f;
    while (time > 0.0f) {
        Sonicteam::System::Geometry::Line line;
        next = Propagate(state, 0.25f);
        {
            HitResult result = fn_8005B2F0(kind, &state, &next, player);
            if (result.m_hit) {
                return result;
            }
        }
        distance += fn_8040D1B4(&state.m_position, &next.m_position);
        if (distance > range) {
            break;
        }
        state = next;
        time -= 0.25f;
    }
    return HitResult(false);
}

/* The hit tests of the types that have one (names inferred). */
static HitResult HitTest9(const ShotInfo &info, float time) {
    return HitTest(info, lbl_8051DBE8.m_reach, s_speed, time);
}

static HitResult HitTest10(const ShotInfo &info, float time) {
    return HitTest(info, lbl_8051DBFC.m_reach, s_slowSpeed, time);
}

UnkBulletType_80546018 lbl_8051DB34 = {0, 0, Create0, 0, Plain(250.0f)};
UnkBulletType_80546018 lbl_8051DB48 = {0, 0, Create1, 0, Plain(250.0f)};
UnkBulletType_80546018 lbl_8051DB5C = {0, 0, Create2, 0, Plain(400.0f)};
UnkBulletType_80546018 lbl_8051DB70 = {0, 0, Create3, 0, Plain(600.0f)};
UnkBulletType_80546018 lbl_8051DB84 = {0, 0, Create3, 0, Plain(600.0f)};
UnkBulletType_80546018 lbl_8051DB98 = {0, 0, Create5, 0, Plain(250.0f)};
UnkBulletType_80546018 lbl_8051DBAC = {0, 0, Create6, 0, Plain(250.0f)};
UnkBulletType_80546018 lbl_8051DBC0 = {0, 0, Create7, 0, Plain(250.0f)};
UnkBulletType_80546018 lbl_8051DBD4 = {0, 0, Create8, 0, Plain(400.0f)};
UnkBulletType_80546018 lbl_8051DBE8 = {0, 0, Create9, HitTest9, Plain(2000.0f)};
UnkBulletType_80546018 lbl_8051DBFC = {0, 0, Create10, HitTest10, Plain(2000.0f)};
UnkBulletType_80546018 lbl_8051DC10 = {0, 0, Create11, 0, Plain(100.0f)};
UnkBulletType_80546018 lbl_8051DC24 = {0, 0, Create12, 0, Plain(200.0f)};
UnkBulletType_80546018 lbl_8051DC38 = {0, 0, Create13, 0, Plain(600.0f)};

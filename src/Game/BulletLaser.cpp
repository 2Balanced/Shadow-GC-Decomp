/* BulletLaser (file name from the RTTI of its class, @unnamed@BulletLaser_cpp@::Laser): the laser
 * bullet of the laser weapons, in five levels (bullet records lbl_80546018 to lbl_80546068). A laser
 * flies straight with an effect and an attack collision; when it hits something it either calls
 * the weapon's hit callback for its new course or bounces off, and it disappears after its number
 * of hits or its range. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * Not yet matching: in HitCallback::Call (0x80220E88, a weak copy emitted here) the original loads
 * the function's manager after saving r31, while GC/2.7 schedules that load above the save (the
 * scheduling difference described in docs/research-notes.md for ScatteringRing). Everything else,
 * including the data, matches. */
/* std::runtime_error with the library's size (see <stdexcept>). */
#define MSL_RUNTIME_ERROR_STRING
#include <stdexcept>

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharaColliAttack.h"
#include "Game/VectorDot.h"
#include "Game/WeaponBullet.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using Weapon::ShotInfo;
using Weapon::Bullet::BulletBase;
using Weapon::Bullet::BulletState;
using Weapon::Bullet::HitContext;
using Weapon::Bullet::HitResult;

extern "C" {
/* The starting state of a bullet fired with info at speed (behaviour inferred). */
BulletState fn_8005A228(const ShotInfo &info, float speed);
/* Moves a bullet from before to after and tests what it hits on the way, and tests where a new
 * bullet starts (behaviour inferred). */
HitResult fn_8005A9D0(int kind, const BulletState *before, const BulletState *after,
                      boost::shared_ptr<CharaColliAttack> *attack, int arg5);
HitResult fn_8005B1F8(const ShotInfo &info, int kind, boost::shared_ptr<CharaColliAttack> *attack);
/* Creates the attack collision of a bullet (behaviour inferred). */
boost::shared_ptr<CharaColliAttack> fn_8005A300(int side, boost::weak_ptr<CharInfo> owner, float damage,
                                                const AttackCallback &onHit, const AttackCallback &onEnd);
/* Makes an attack collision ignore another (behaviour inferred). */
void fn_800532F8(CharaColliAttack *attack, boost::weak_ptr<CharaColliAttack> other);
/* Starts effect id at position, facing direction, for owner (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8005E32C(const Vec3f *position,
                                                           const Vec3f *direction, int id,
                                                           const boost::weak_ptr<CharInfo> &owner);
/* Moves and turns an effect (see EnergyCore.cpp; behaviour inferred). */
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
void fn_80425D94(Effect::System::EffectElement *effect, const Vec3f *rotation);
/* Plays sound id at position (behaviour inferred). */
void fn_802B3FE8(int id, const Vec3f *position);
/* The distance between two points (behaviour inferred). */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);

/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CDA8(const Vec3f &vector, float scale);       /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f &a, const Vec3f &b); /* a + b */
Vec3f fn_8040CEE8(const Vec3f &vector);                    /* normalised */
/* A direction turned into the angles of a rotation (behaviour inferred). */
Vec3f fn_80016284(const Vec3f &direction);
Vec2f fn_8040F760(const Vec3f &direction);
Vec3f fn_800576CC(const Vec2f &angles);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f &a, const Vec3f &b);

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Returns x unchanged (name inferred). Being initialised through a function call, the values below
 * are set up at startup (see Meteor.cpp). */
inline float Plain(float x) {
    return x;
}

/* A bullet type's record (struct name unknown; names inferred): a central table lists them. */
struct UnkBulletType_80546018 {
    /* 0x00 */ void (*initialize)(void);
    /* 0x04 */ void (*finalize)(void);
    /* 0x08 */ void (*create)(const ShotInfo &info);
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ float m_reach;
};
SIZE_ASSERT(UnkBulletType_80546018, 0x14);

static BulletState Propagate(const BulletState &state, float dt);

namespace {

/* What each laser level does (struct name unknown; names inferred). */
struct LaserParam {
    /* 0x00 */ float damage;
    /* 0x04 */ float speed;
    /* 0x08 */ float range;
    /* 0x0C */ s32 maxHits;
    /* 0x10 */ s32 effect;
    /* 0x14 */ s32 hitEffect; /* none when negative */
    /* 0x18 */ s32 hitSound;
};

/* The speeds of the levels (meaning inferred). */
static const float lbl_805EFDA4 = Plain(1000.0f);
static const float lbl_805EFDA8 = Plain(500.0f);
static const float lbl_805EFDAC = Plain(1000.0f);
static const float lbl_805EFDB0 = Plain(300.0f);

static const LaserParam lbl_80545F74[] = {
    {3.0f, lbl_805EFDA4, Plain(300.0f), 0, 0x19F, 0x1A3, 3},
    {4.0f, lbl_805EFDA8, Plain(400.0f), 0, 0x1AB, 0x1AC, 4},
    {5.0f, lbl_805EFDAC, Plain(800.0f), 2, 0x1A1, 0x1A4, 5},
    {6.0f, lbl_805EFDB0, Plain(800.0f), 10, 0x1F4, 0x201, 0},
    {10.0f, lbl_805EFDB0, Plain(800.0f), 10, 0x1F4, 0x301, 0},
};

class Laser : public BulletBase {
  public:
    Laser(const ShotInfo &info, int type);
    virtual ~Laser();
    virtual void Tick(float dt);

    /* The effect that follows the laser (names inferred). */
    void StartEffect(int id);
    void StopEffect();
    void UpdateEffect();
    /* Creates the attack collision (name inferred). */
    void SetupAttack();
    /* The hit effect and sound (name inferred). */
    void OnHitTarget(const HitResult &result);
    /* The callbacks of the attack collision (names inferred). */
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    /* Member names are inferred. */
    /* 0x3C */ float m_reach;
    /* 0x40 */ s32 m_type;
    /* 0x44 */ s32 m_hitCount;
    /* 0x48 */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0x50 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x58 */ Weapon::Bullet::HitCallback m_hitCallback;
};
SIZE_ASSERT(Laser, 0x64);

Laser::Laser(const ShotInfo &info, int type) : BulletBase(info), m_type(type) {
    const LaserParam &param = lbl_80545F74[m_type];
    m_reach = param.range;
    m_state = fn_8005A228(info, param.speed);
    m_hitCallback = info.m_callback;
    m_hitCount = 0;
    StartEffect(param.effect);
    SetupAttack();
    HitResult result = fn_8005B1F8(info, m_attackKind, &m_attack);
    if (result.m_hit) {
        OnHitTarget(result);
        delete this;
        return;
    }
}

Laser::~Laser() {
    StopEffect();
}

void Laser::Tick(float dt) {
    BulletState next = Propagate(m_state, dt);
    m_distance += fn_8040D1B4(&next.m_position, &m_state.m_position);
    {
        HitResult result = fn_8005A9D0(m_attackKind, &m_state, &next, &m_attack, 1);
        if (result.m_hit) {
            OnHitTarget(result);
            next = m_state;
            AssignVector(next.m_position, (const Vec3f &)result.m_position);
            if (result.m_struck) {
                m_hitCount++;
                if (m_hitCount >= lbl_80545F74[m_type].maxHits) {
                    delete this;
                    return;
                }
            }
            if (m_hitCallback.IsSet()) {
                HitContext context(result, m_state, next);
                next = m_hitCallback.Call(context);
            } else {
                const Vec3f &normal = (const Vec3f &)result.m_normal;
                fn_800091D0(&next.m_direction,
                            &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                m_state.m_direction,
                                fn_8040CDA8(fn_8040CDA8(normal, Fn8040Dot(m_state.m_direction, normal)), 2.0f)));
            }
            m_state = next;
            UpdateEffect();
            next.m_rotation = fn_800576CC(fn_8040F760(fn_80016284(next.m_direction)));
            StartEffect(lbl_80545F74[m_type].effect);
            if (result.m_struck) {
                SetupAttack();
            }
        }
    }
    m_state = next;
    UpdateEffect();
    if (m_distance >= m_reach) {
        delete this;
    }
}

} // namespace

/* The state of a bullet after flying for dt (name inferred). */
static BulletState Propagate(const BulletState &state, float dt) {
    return BulletState(fn_8040CDD0(state.m_position, fn_8040CDA8(state.m_direction, dt)), state.m_direction,
                       state.m_rotation);
}

namespace {

void Laser::OnHitTarget(const HitResult &result) {
    s32 effect = lbl_80545F74[m_type].hitEffect;
    if (effect >= 0) {
        fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, effect, m_owner);
    }
    fn_802B3FE8(lbl_80545F74[m_type].hitSound, (const Vec3f *)&result.m_position);
}

void Laser::SetupAttack() {
    const LaserParam &param = lbl_80545F74[m_type];
    m_attack = fn_8005A300(m_side, m_owner, param.damage, boost::bind(&Laser::OnHit, this, _1),
                           boost::bind(&Laser::OnAttackEnd, this, _1));
}

void Laser::StartEffect(int id) {
    StopEffect();
    m_effect = fn_8005E32C(&m_state.m_position, &m_state.m_direction, id, m_owner);
}

void Laser::StopEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
        m_effect.reset();
    }
}

void Laser::UpdateEffect() {
    boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
    if (effect.get() != 0) {
        if (!effect->IsActive()) {
            m_effect.reset();
        } else {
            fn_80425E4C(effect.get(), &m_state.m_position);
            Vec3f rotation(m_state.m_rotation.x, m_state.m_rotation.y, m_state.m_rotation.z);
            fn_80425D94(effect.get(), &rotation);
        }
    }
}

void Laser::OnHit(AttackCallbackParam &param) {
    /* The direction and speed of the hit at 0x34 and 0x4C (meanings inferred; see Meteor.cpp). */
    AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)),
                 fn_8040CEE8(m_state.m_direction));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = lbl_80545F74[m_type].speed;
}

void Laser::OnAttackEnd(AttackCallbackParam &param) {
    if (m_attack.get() != 0) {
        /* What the attack hit at 0x8 (meaning inferred). */
        fn_800532F8(m_attack.get(),
                    *reinterpret_cast<const boost::weak_ptr<CharaColliAttack> *>(param.field_0x8));
    }
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

} // namespace

/* Creates a laser of each level (names inferred). */
static void CreateLaser0(const ShotInfo &info) {
    new Laser(info, 0);
}

static void CreateLaser1(const ShotInfo &info) {
    new Laser(info, 1);
}

static void CreateLaser2(const ShotInfo &info) {
    new Laser(info, 2);
}

static void CreateLaser3(const ShotInfo &info) {
    new Laser(info, 3);
}

static void CreateLaser4(const ShotInfo &info) {
    new Laser(info, 4);
}

UnkBulletType_80546018 lbl_80546018 = {0, 0, CreateLaser0, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8054602C = {0, 0, CreateLaser1, 0, Plain(400.0f)};
UnkBulletType_80546018 lbl_80546040 = {0, 0, CreateLaser2, 0, Plain(800.0f)};
UnkBulletType_80546018 lbl_80546054 = {0, 0, CreateLaser3, 0, Plain(800.0f)};
UnkBulletType_80546018 lbl_80546068 = {0, 0, CreateLaser4, 0, Plain(800.0f)};

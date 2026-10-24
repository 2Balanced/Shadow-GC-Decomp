/* Cannon (class name from RTTI: Weapon::Bullet::Cannon): the shell of the cannon weapons, in nine
 * types (bullet records lbl_8051D904 to lbl_8051D990 cover eight of them). A shell flies under
 * gravity with an effect and an attack collision; when it hits something it leaves a blast
 * (Game/BulletCannonBomb.cpp), or for the two healing types a healing blast, and it disappears after
 * its range. The records' hit tests fly a shell ahead of time to see what it would hit. This unit
 * is the first to use many shared inline functions, so it emits them: the weak_ptr and shared_ptr
 * functions of CharaColliAttack and EffectElement, the sound manager's instance (fn_80053164), an
 * attack collision's ignore list (fn_800532F8) and the boost::function1 destructor of attack
 * callbacks. File name inferred from the class name; other names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/AttackIgnoreList.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/TNumEdit.h"
#include "Effect/System/EffectElement.h"
#include "Game/BulletCannonBomb.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/GeometryLine.h"
#include "Game/SoundManager.h"
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
/* Tests what a bullet hits where it starts, with and without its attack collision (behaviour
 * inferred; see Game/BulletLaser.cpp). */
HitResult fn_8005B490(const ShotInfo &info, int kind);
HitResult fn_8005B1F8(const ShotInfo &info, int kind, boost::shared_ptr<CharaColliAttack> *attack);
/* Moves a bullet from before to after and tests what it hits on the way, for the player filter, and
 * with the attack collision (behaviour inferred). */
HitResult fn_8005B2F0(int kind, const BulletPosition *before, const BulletPosition *after, s32 player);
HitResult fn_8005AFE4(int kind, const BulletPosition *before, const BulletPosition *after,
                      boost::shared_ptr<CharaColliAttack> *attack);
/* Creates the attack collision of a bullet (see Game/BulletLaser.cpp). */
boost::shared_ptr<CharaColliAttack> fn_8005A300(int side, boost::weak_ptr<CharInfo> owner, float damage,
                                                const AttackCallback &onHit, const AttackCallback &onEnd);
/* Starts effect id at position, facing direction, for owner (see Game/BulletLaser.cpp). */
boost::weak_ptr<EffectElement> fn_8005E32C(const Vec3f *position, const Vec3f *direction, int id,
                                           const boost::weak_ptr<CharInfo> &owner);
/* Moves and turns an effect (see Game/BulletLaser.cpp). */
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
/* Starts sound id at source (see Game/AudioManagerTask.cpp). */
bool fn_801D54F4(UnkSound_80053164 *sound, int id, void *source, void *owner, u16 flags);
/* The distance between two points (behaviour inferred). */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CDA8(const Vec3f &vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f &a, const Vec3f &b);   /* a + b */
Vec3f fn_8040CEE8(const Vec3f &vector);              /* normalised */
/* A direction turned into the angles of a rotation (see Game/BulletLaser.cpp). */
Vec3f fn_80016284(const Vec3f &direction);
Vec2f fn_8040F760(const Vec3f &direction);
}

namespace HealingUnit {
/* The blast a healing unit leaves (Game/HealingUnit.cpp). */
class HealingBlast {
  public:
    static void Instantiate(const boost::weak_ptr<CharInfo> &owner, const Vec3f &position, int kind);
};
} // namespace HealingUnit

/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * emits the copy for EffectElement (boost 1.32's definition). */
namespace boost {
template <> inline void weak_ptr<EffectElement>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The sound manager's instance: the object (0x80570B74) and its guard (0x805EE010) are the weak
 * statics of this inline function, which an earlier unit is the first to emit (name inferred). */
inline UnkSound_80053164 &SoundManagerObject() {
    static UnkSound_80053164 s_object;
    return s_object;
}

/* The instance fn_80053164 hands out once it is built. */
extern UnkSound_80053164 *lbl_805EE018;

/* The sound manager (other units call it by its address). An inline function that calls another,
 * so it stays out of line; this unit is the first to emit it. */
extern "C" inline UnkSound_80053164 *fn_80053164(void) {
    if (lbl_805EE018 == 0) {
        lbl_805EE018 = &SoundManagerObject();
    }
    return lbl_805EE018;
}

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A rotation made of the angles about x and y that a direction gives (struct name unknown; the
 * game's rotation type, which BulletPosition's rotation stands in for with Vec3f). */
struct UnkRotation {
    UnkRotation(const Vec2f &angles) : x(angles.x), y(angles.y), z(0.0f) {}

    float x;
    float y;
    float z;
};

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
    /* What a shot would hit within time seconds. */
    /* 0x0C */ HitResult (*hitTest)(const ShotInfo &info, float time);
    /* 0x10 */ float m_reach;
};
SIZE_ASSERT(UnkBulletType_80546018, 0x14);

/* The time step of a hit test. */
static float s_step = 0.125f;

/* The range, the speeds and the gravity of a shell. */
static const float s_range = Plain(1000.0f);
static float s_speed = Plain(450.0f);
static float s_slowSpeed = Plain(300.0f);
static const float s_gravity = Plain(-33.0f);

namespace Weapon {
namespace Bullet {

/* What each type does (struct name unknown). */
struct CannonParam {
    /* 0x0 */ float damage;
    /* 0x4 */ float reach;
    /* 0x8 */ s32 effect; /* none when negative */
};

static CannonParam s_params[] = {
    {4.0f, Plain(200.0f), 0x111},  {8.0f, Plain(300.0f), 0x10D},  {16.0f, Plain(300.0f), 0x11F},
    {4.0f, Plain(200.0f), 0x130},  {8.0f, Plain(200.0f), 0x130},  {8.0f, Plain(300.0f), 0x188},
    {99.0f, Plain(9990.0f), 0x188}, {0.0f, Plain(300.0f), 0x1FD},  {0.0f, Plain(300.0f), 0x1FD},
};

class Cannon : public BulletBase {
  public:
    Cannon(const ShotInfo &info, float speed, int type);
    virtual ~Cannon();
    virtual void Tick(float dt);

    /* The effect that follows the shell. */
    void StartEffect();
    void StopEffect();
    void UpdateEffect();
    /* The blast, or the healing blast. */
    void OnHitTarget(const HitResult &result);
    /* The callbacks of the attack collision. */
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    /* 0x3C */ float m_reach;
    /* 0x40 */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0x48 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x50 */ s32 m_type;
};
SIZE_ASSERT(Cannon, 0x54);

} // namespace Bullet
} // namespace Weapon

static BulletPosition InitialState(const ShotInfo &info, float speed);
static BulletPosition Propagate(const BulletPosition &state, float dt);

namespace Weapon {
namespace Bullet {

Cannon::Cannon(const ShotInfo &info, float speed, int type) : BulletBase(info), m_type(type) {
    m_state = InitialState(info, speed);
    m_reach = s_params[m_type].reach;
    m_attack = fn_8005A300(info.m_side, info.m_owner, s_params[m_type].damage, boost::bind(&Cannon::OnHit, this, _1),
                           boost::bind(&Cannon::OnAttackEnd, this, _1));
    HitResult result = fn_8005B1F8(info, m_attackKind, &m_attack);
    if (result.m_hit) {
        AssignVector(m_state.m_position, (const Vec3f &)result.m_position);
        OnHitTarget(result);
        delete this;
        return;
    }
    StartEffect();
}

Cannon::~Cannon() {
    if (m_attack.get() != 0) {
        fn_80053B40(&m_attack);
    }
    StopEffect();
}

void Cannon::Tick(float dt) {
    BulletPosition next = Propagate(m_state, dt);
    m_distance += fn_8040D1B4(&next.m_position, &m_state.m_position);
    HitResult result = fn_8005AFE4(m_attackKind, &m_state, &next, &m_attack);
    if (result.m_hit) {
        m_state = next;
        OnHitTarget(result);
        delete this;
        return;
    }
    m_state = next;
    UpdateEffect();
    if (m_distance >= s_range) {
        delete this;
    }
}

} // namespace Bullet
} // namespace Weapon

/* The state of a shell fired with info at speed (name inferred). */
static BulletPosition InitialState(const ShotInfo &info, float speed) {
    Vec3f velocity;
    AssignVector(velocity, fn_8040CDA8(fn_8040CEE8((const Vec3f &)info.m_direction), speed));
    return BulletPosition((const Vec3f &)info.m_position, velocity,
                          (const Vec3f &)UnkRotation(fn_8040F760(fn_80016284((const Vec3f &)info.m_direction))));
}

namespace Weapon {
namespace Bullet {

BulletPosition::BulletPosition(const Vec3f &position, const Vec3f &direction, const Vec3f &rotation) {
    fn_800091EC(&m_position, &position);
    fn_800091EC(&m_direction, &direction);
    m_rotation = rotation;
}

} // namespace Bullet
} // namespace Weapon

/* The state of a shell after flying for dt under gravity (name inferred). */
static BulletPosition Propagate(const BulletPosition &state, float dt) {
    Vec3f gravity(0.0f, s_gravity, 0.0f);
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040CDD0(fn_8040CDD0(state.m_position, fn_8040CDA8(state.m_direction, dt)),
                                                         fn_8040CDA8(fn_8040CDA8(fn_8040CDA8(gravity, 0.5f), dt), dt)));
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040CDD0(state.m_direction, fn_8040CDA8(gravity, dt)));
    return BulletPosition(position, direction, (const Vec3f &)UnkRotation(fn_8040F760(fn_80016284(direction))));
}

namespace Weapon {
namespace Bullet {

void Cannon::OnHit(AttackCallbackParam &param) {
    /* The direction and speed of the hit at 0x34 and 0x4C, and what was hit at 0x30 (meanings
     * inferred; see Meteor.cpp). */
    AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)), fn_8040CEE8(m_state.m_direction));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = fn_8040CF50(&m_state.m_direction);
    if (0.0f == param.m_damage) {
        /* The result of the move being tested, at 0x58 (meaning inferred). */
        (*reinterpret_cast<HitResult **>(reinterpret_cast<u8 *>(&param) + 0x58))->m_hit = 1;
        param.field_0x8[0x30 - 0x8] = 1;
    }
}

void Cannon::OnAttackEnd(AttackCallbackParam &param) {
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

void Cannon::OnHitTarget(const HitResult &result) {
    boost::shared_ptr<CharaColliAttack> attack;
    switch (result.field_0x38) {
    case 1:
    case 3:
        attack = m_attack;
        break;
    }
    switch (m_type) {
    case 7:
        fn_801D54F4(fn_80053164(), 0xE02E, (void *)&result.m_position, 0, 1);
        HealingUnit::HealingBlast::Instantiate(m_owner, (const Vec3f &)result.m_position, 0);
        break;
    case 8:
        fn_801D54F4(fn_80053164(), 0xE02E, (void *)&result.m_position, 0, 1);
        HealingUnit::HealingBlast::Instantiate(m_owner, (const Vec3f &)result.m_position, 1);
        break;
    default:
        StartCannonBomb(*this, m_type, attack);
        break;
    }
}

void Cannon::StartEffect() {
    s32 effect = s_params[m_type].effect;
    if (effect >= 0) {
        m_effect = fn_8005E32C(&m_state.m_position, &m_state.m_direction, effect, m_owner);
    }
    UpdateEffect();
}

void Cannon::StopEffect() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() != 0) {
        effect->Retire();
        m_effect.reset();
    }
}

void Cannon::UpdateEffect() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() != 0) {
        fn_80425E4C(effect.get(), &m_state.m_position);
        Vec3f rotation(m_state.m_rotation.x, m_state.m_rotation.y, m_state.m_rotation.z);
        fn_80425D94(effect.get(), &rotation);
    }
}

} // namespace Bullet
} // namespace Weapon

using Weapon::Bullet::Cannon;

/* The types' load and release hooks, all empty (names inferred). */
static void Initialize0() {}

static void Initialize1() {}

static void Initialize2() {}

static void Initialize3() {}

static void Initialize4() {}

static void Finalize() {}

/* Fires a shell of each type (names inferred). */
static void Create0(const ShotInfo &info) {
    new Cannon(info, s_slowSpeed, 0);
}

static void Create1(const ShotInfo &info) {
    new Cannon(info, s_speed, 1);
}

static void Create2(const ShotInfo &info) {
    new Cannon(info, s_speed, 2);
}

static void Create3(const ShotInfo &info) {
    new Cannon(info, s_slowSpeed, 3);
}

static void Create4(const ShotInfo &info) {
    new Cannon(info, s_speed, 4);
}

static void Create5(const ShotInfo &info) {
    new Cannon(info, s_speed, 5);
}

static void Create7(const ShotInfo &info) {
    new Cannon(info, s_speed, 7);
}

static void Create8(const ShotInfo &info) {
    new Cannon(info, s_speed, 8);
}

namespace Weapon {
namespace Bullet {

BulletPosition::BulletPosition(const BulletPosition &other) {
    fn_800091EC(&m_position, &other.m_position);
    fn_800091EC(&m_direction, &other.m_direction);
    m_rotation = other.m_rotation;
}

} // namespace Bullet
} // namespace Weapon

/* What a shell fired with info at speed would hit within time seconds and range (name
 * inferred). */
static HitResult HitTest(const ShotInfo &info, float range, float speed, float time) {
    if (info.m_owner.expired()) {
        return HitResult(false);
    }
    s32 player = info.m_owner.lock()->field_0x54;
    BulletPosition state = InitialState(info, speed);
    BulletPosition next;
    float step = s_step;
    int kind;
    if (info.m_side == 0) {
        kind = 3;
    } else {
        kind = 6;
    }
    {
        HitResult start = fn_8005B490(info, kind);
        if (start.m_hit) {
            return start;
        }
    }
    float distance = 0.0f;
    while (time > 0.0f) {
        Sonicteam::System::Geometry::Line line;
        next = Propagate(state, step);
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
        time -= step;
    }
    return HitResult(false);
}

/* The hit tests of the types (names inferred). */
static HitResult HitTest0(const ShotInfo &info, float time) {
    return HitTest(info, s_range, s_slowSpeed, time);
}

static HitResult HitTest1(const ShotInfo &info, float time) {
    return HitTest(info, s_range, s_speed, time);
}

static HitResult HitTest2(const ShotInfo &info, float time) {
    return HitTest(info, s_range, s_speed, time);
}

static HitResult HitTest3(const ShotInfo &info, float time) {
    return HitTest(info, s_range, s_slowSpeed, time);
}

static HitResult HitTest4(const ShotInfo &info, float time) {
    return HitTest(info, s_range, s_speed, time);
}

static HitResult HitTest5(const ShotInfo &info, float time) {
    return HitTest(info, s_range, s_speed, time);
}

UnkBulletType_80546018 lbl_8051D904 = {Initialize0, Finalize, Create0, HitTest0, Plain(200.0f)};
UnkBulletType_80546018 lbl_8051D918 = {Initialize1, Finalize, Create1, HitTest1, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051D92C = {Initialize2, Finalize, Create2, HitTest2, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051D940 = {Initialize3, Finalize, Create3, HitTest3, Plain(200.0f)};
UnkBulletType_80546018 lbl_8051D954 = {Initialize4, Finalize, Create4, HitTest4, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051D968 = {Initialize1, Finalize, Create5, HitTest5, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051D97C = {Initialize1, Finalize, Create7, HitTest1, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051D990 = {Initialize1, Finalize, Create8, HitTest1, Plain(300.0f)};

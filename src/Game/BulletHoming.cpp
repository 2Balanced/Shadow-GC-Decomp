/* Homing (class name from RTTI: Weapon::Bullet::Homing): the homing shot of the weapons that lock
 * on, in eleven types (bullet records lbl_8051DD14 to lbl_8051DDDC) of eight kinds (s_params). A
 * shot turns toward its lock-on target, faster the longer it flies, and carries a model, an effect
 * and an attack collision. While its target is locked, the line it flies along is also tested
 * against the target's own colliders. Once it has lost its target it disappears after its range.
 * File name inferred from the class name; other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * Not yet matching: the static initializer (0x80058970) has retail's instructions, registers and
 * constants, but GC/2.7 restores r31 at the end, where retail restores it right after the store to
 * the first record that uses it (the same scheduling difference as in Game/BulletGun.cpp; see
 * docs/research-notes.md). Everything else, including the data, matches, and linking this unit
 * changes only those instructions. */
#include <boost/function.hpp>

#include "Game/AttackIgnoreList.h"

#include <boost/bind.hpp>

#include "Game/CharaColliBaseCharInfo.h"
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/EffLocusMultiVtx.h"
#include "Game/SoundManager.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponLockonTarget.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBaseCharInfo;
using Weapon::LockonTarget;
using Weapon::ShotInfo;
using Weapon::Bullet::BulletBase;
using Weapon::Bullet::BulletPosition;
using Weapon::Bullet::HitResult;

/* Target (class name from RTTI): a point other objects can aim at (see
 * Game/BulletSatelliteLaser.cpp). Its first virtual function returns the point to aim at from a
 * position; the character it belongs to is at 0x28 (names inferred). */
class Target {
  public:
    virtual Vec3f GetPosition(const Vec3f &from);

    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner;
};

/* The model a shot carries (class name unknown): it follows the shot's position and rotation. */
class UnkBulletModel_8005930C {
  public:
    UnkBulletModel_8005930C(int group, int id);
    ~UnkBulletModel_8005930C();
    void Update(const BulletPosition *state);

    /* 0x0 */ void *m_clump;
    /* 0x4 */ void *m_frame;
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xB2];
    /* 0x0B2 */ u8 m_twoPlayer; /* meaning inferred */
    /* 0x0B3 */ u8 field_0xB3[0x150 - 0xB3];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* A line between two points for the collision test (see Game/BlackBullCtrlUnit.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A collider the line test meets (struct name unknown): its kind flags at 0xC. */
struct UnkLineObject_800587C0 {
    /* 0x0 */ u8 field_0x0[0xC];
    /* 0xC */ u32 m_flags;
};

/* A node of a contact list: the collider at 0x0 and the hit's object at 0x14 (see
 * Game/BirdBase.cpp). */
struct UnkColliContactNode_800587A0 {
    /* 0x00 */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x08 */ u8 field_0x8[0x14 - 0x8];
    /* 0x14 */ void *m_object;
};

struct UnkColliContacts_8001EE04;

/* An iterator over a contact list (see Game/BombServer.cpp). */
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

extern "C" {
/* The sound manager and its play call (see Game/BulletCannon.cpp). */
UnkSound_80053164 *fn_80053164(void);
bool fn_801D54F4(UnkSound_80053164 *sound, u16 id, void *source, void *owner, u16 flags);
/* Tests what a bullet hits on its move from before to after, with its attack collision (see
 * Game/BulletCannon.cpp). */
HitResult fn_8005AFE4(int kind, const BulletPosition *before, const BulletPosition *after,
                      boost::shared_ptr<CharaColliAttack> *attack);
/* Creates the attack collision of a bullet (see Game/BulletLaser.cpp). */
boost::shared_ptr<CharaColliAttack> fn_8005A300(int side, boost::weak_ptr<CharInfo> owner, float damage,
                                                const AttackCallback &onHit, const AttackCallback &onEnd);
/* Starts effect id at position, facing direction, for owner (see Game/BulletLaser.cpp), and the
 * same with a kind (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_8005E32C(const Vec3f *position, const Vec3f *direction, int id,
                                           const boost::weak_ptr<CharInfo> &owner);
boost::weak_ptr<EffectElement> fn_8005E374(int kind, const Vec3f *position, const Vec3f *direction, int id,
                                           const boost::weak_ptr<CharInfo> &owner);
/* Loads model group id (see Game/AirBoltModel.cpp). */
void fn_8006B50C(int id, int arg1);
/* The lock-on: tells it who fires at it, whether it still holds a target, the target, and moves
 * its mark with the shot (behaviour inferred). */
void fn_80060E58(LockonTarget *lockon, boost::weak_ptr<CharInfo> *owner);
bool fn_80061110(LockonTarget *lockon);
const boost::weak_ptr<Target> *fn_80061050(LockonTarget *lockon);
void fn_800610B4(LockonTarget *lockon, const BulletPosition *state);
/* The collider manager, the collision world, a line between two points and the contacts of what
 * a line meets (see Game/BlackBullCtrlUnit.cpp). */
void *fn_80021B08(void);
void *fn_80020D40(void);
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
UnkColliContacts_8001EE04 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int kind,
                                       bool (*filter)(UnkLineObject_800587C0 *object), int arg5);
void fn_80026FB4(UnkColliContacts_8001EE04 *contacts, int arg1);
/* The contacts of a list (see Game/BirdBase.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkColliContacts_8001EE04 *contacts);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkColliContacts_8001EE04 *contacts);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
u32 fn_80026588(UnkColliContactNode_800587A0 *node);
/* The position and normal of a contact's object (see Game/BirdBase.cpp). */
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
/* Whether attack can hit the contact's collider, and the hit (see Game/BirdBase.cpp). */
bool fn_800C3ED4(void *manager, UnkColliContactNode_800587A0 *node);
void fn_800C40C4(void *manager, boost::weak_ptr<CharaColliAttack> attack, UnkColliContactNode_800587A0 *node,
                 HitResult *result);
/* Moves and turns an effect (see Game/BulletLaser.cpp). */
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
/* The distance between two points (behaviour inferred). */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
/* Arc cosine (see Game/BirdBase.cpp). */
float fn_80015D68(float value);
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CDA8(const Vec3f &vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f &a, const Vec3f &b);   /* a + b */
Vec3f fn_8040CEE8(const Vec3f &vector);              /* normalised */
Vec3f fn_8040CFB8(const Vec3f &a, const Vec3f &b);   /* a x b */
float fn_8040CF90(const Vec3f &a, const Vec3f &b);   /* a . b */
float fn_8040CE74(Vec3f *vector);                    /* normalised in place */
void fn_8040CE38(Vec3f *vector, float scale);        /* scaled in place */
/* The rotation by angle about axis, and matrix * point (see Game/AdjustCosRot.cpp). */
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* A direction turned into the angles of a rotation (see Game/BulletLaser.cpp). */
Vec3f fn_80016284(const Vec3f &direction);
Vec2f fn_8040F760(const Vec3f &direction);
}

extern const float lbl_805FAD38; /* pi */

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The rotation of the angles about x and y that fn_8040F760 gives (other units call it by its
 * address). Its body calls the inline Vec3f constructor, so it stays out of line. */
extern "C" inline Vec3f fn_800576CC(const Vec2f &angles) {
    return Vec3f(angles.x, angles.y, 0.0f);
}

/* The character a collider is attached to (other units call it by its address). */
extern "C" inline boost::weak_ptr<CharInfo> fn_800586EC(CharaColliBaseCharInfo *collider) {
    return *reinterpret_cast<const boost::weak_ptr<CharInfo> *>(reinterpret_cast<u8 *>(collider) + 0x30);
}

/* The collider of a contact (other units call it by its address). */
extern "C" inline boost::weak_ptr<CharaColliBase> fn_800587A0(UnkColliContactNode_800587A0 *node) {
    return node->m_collider;
}

/* The colliders the line test of a shot keeps (name inferred). */
inline bool IsLineTarget(UnkLineObject_800587C0 *object) {
    return (object->m_flags & 0xC0018) == 0;
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

/* The flag word at 0x68 (struct name unknown): bit 1 the shot has lost its target, bit 2 its line
 * test looks for the target. */
struct HomingFlags {
    HomingFlags() {
        fn_80014E04(&m_bits);
    }
    void Reset() {
        fn_80014E10(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }

    u32 m_bits;
};

/* The turning rates (degrees per second), their growth (degrees per second squared) and the
 * speeds: of the enemies' shots, of a player's shots in a one-player game, and of the fast kind
 * (kind 4). */
static float s_turnRate = 40.0f;
static float s_speed = Plain(150.0f);
static float s_turnAccel = 0.0f;
static float s_playerTurnRate = 90.0f;
static float s_playerSpeed = Plain(150.0f);
static float s_playerTurnAccel = 90.0f;
static float s_fastTurnRate = 540.0f;
static float s_fastSpeed = Plain(500.0f);
static float s_fastTurnAccel = 90.0f;

namespace Weapon {
namespace Bullet {

/* What each kind does (struct name unknown). */
struct HomingParam {
    /* 0x00 */ s32 m_modelGroup;
    /* 0x04 */ s32 m_model; /* no model when negative */
    /* 0x08 */ s32 m_effectKind;
    /* 0x0C */ s32 m_effect;
    /* 0x10 */ float m_damage;
    /* 0x14 */ s32 m_sound; /* none when negative */
    /* 0x18 */ bool field_0x18;
};

static const HomingParam s_params[] = {
    {2, 0x2E, 0, 0x1F0, 8.0f, 0x201F, true},   {2, 0x2F, 0, 0x1F0, 8.0f, 0x201F, true},
    {2, -1, 0, 0x1C8, 8.0f, 0x2022, false},    {2, -1, 0, 0x1CC, 16.0f, 0x2023, false},
    {2, -1, 0, 0x1C8, 4.0f, 0x2022, false},    {0x15, 4, 0, 0x10D, 4.0f, 0x201F, true},
    {9, 6, 0, 0x1F0, 4.0f, 0x2022, true},      {-1, -1, 3, 0, 4.0f, 0x201F, true},
};

class Homing : public BulletBase {
  public:
    Homing(const ShotInfo &info, float reach, int type);
    virtual ~Homing();
    virtual void Tick(float dt);

    /* The state after dt, turned toward the target. */
    BulletPosition Steer(const BulletPosition &state, float dt);
    /* The hit effect and sound. */
    void OnHitTarget(const HitResult &result);
    /* The effect that follows the shot. */
    void StartEffect(int kind, int effect);
    void StopEffect();
    void UpdateEffect();
    /* The callbacks of the attack collision. */
    void OnHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    /* 0x3C */ boost::shared_ptr<LockonTarget> m_target;
    /* 0x44 */ float m_reach;
    /* 0x48 */ float m_turnRate;  /* radians per second */
    /* 0x4C */ float m_turnAccel; /* radians per second squared */
    /* 0x50 */ s32 m_type;
    /* 0x54 */ UnkBulletModel_8005930C *m_model;
    /* 0x58 */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0x60 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x68 */ HomingFlags m_flags;
};
SIZE_ASSERT(Homing, 0x6C);

} // namespace Bullet
} // namespace Weapon

using Weapon::Bullet::Homing;

/* What a shot moving from before to after hits: while its target is locked, the target's own
 * colliders on the line, otherwise what any bullet hits (name inferred). */
static HitResult LineTest(int kind, const boost::shared_ptr<LockonTarget> *lockon, const BulletPosition *before,
                          const BulletPosition *after, boost::shared_ptr<CharaColliAttack> *attack) {
    if (lockon->get() != 0) {
        boost::shared_ptr<Target> target = fn_80061050(lockon->get())->lock();
        if (target.get() != 0) {
            boost::shared_ptr<CharInfo> owner = target->m_owner.lock();
            if (owner.get() != 0) {
                void *object;
                void *manager = fn_80021B08();
                void *world = fn_80020D40();
                UnkLine_80020A20 line;
                fn_80020A20(&line, &before->m_position, &after->m_position);
                UnkColliContacts_8001EE04 *contacts = fn_80021FE4(world, &line, 3, kind, IsLineTarget, 0);
                if (contacts != 0) {
                    HitResult result(false);
                    for (UnkColliContactIter_8001EDFC it(fn_8001EE04(contacts));
                         it.m_node != fn_8001EDEC(contacts); fn_80027374(&it, 0)) {
                        UnkColliContactNode_800587A0 *node = it.m_node;
                        object = node->m_object;
                        AssignVector((Vec3f &)result.m_position, *fn_8001E9BC(object));
                        AssignVector((Vec3f &)result.m_normal, *fn_800266D4(object));
                        AssignVector((Vec3f &)result.field_0x18,
                                     ((VectorSubtractFn)fn_8040CD74)(&after->m_position, &before->m_position));
                        result.field_0x2C = fn_80026588(node) & 0x0FFFFFFF;
                        result.field_0x30 = fn_80026588(node) >> 28;
                        boost::shared_ptr<CharaColliBase> collider = fn_800587A0(node).lock();
                        boost::shared_ptr<CharInfo> hit;
                        if (collider.get() != 0) {
                            boost::shared_ptr<CharaColliBaseCharInfo> charCollider =
                                boost::dynamic_pointer_cast<CharaColliBaseCharInfo>(collider);
                            if (charCollider.get() != 0) {
                                hit = fn_800586EC(charCollider.get()).lock();
                            }
                        }
                        if (hit == owner && attack->get() != 0 && fn_800C3ED4(manager, node)) {
                            fn_800C40C4(manager, boost::weak_ptr<CharaColliAttack>(*attack), node, &result);
                            result.m_struck = 0;
                        }
                        if (result.m_hit) {
                            break;
                        }
                    }
                    fn_80026FB4(contacts, 1);
                    return result;
                }
                return HitResult(false);
            }
        }
    }
    return fn_8005AFE4(kind, before, after, attack);
}

namespace Weapon {
namespace Bullet {

Homing::Homing(const ShotInfo &info, float reach, int type) : BulletBase(info), m_reach(reach), m_type(type) {
    m_target = info.m_target;
    if (m_target.get() != 0) {
        fn_80060E58(m_target.get(), &m_owner);
    }
    m_flags.Reset();
    float speed;
    if (m_type == 4) {
        speed = s_fastSpeed;
        m_turnRate = s_fastTurnRate * (lbl_805FAD38 / 180.0f);
        m_turnAccel = s_fastTurnAccel * (lbl_805FAD38 / 180.0f);
        m_flags.Set(2);
    } else if (m_side == 0 && lbl_8057D6A0.m_twoPlayer == 0) {
        speed = s_playerSpeed;
        m_turnRate = s_playerTurnRate * (lbl_805FAD38 / 180.0f);
        m_turnAccel = s_playerTurnAccel * (lbl_805FAD38 / 180.0f);
        m_flags.Set(2);
    } else {
        speed = s_speed;
        m_turnRate = s_turnRate * (lbl_805FAD38 / 180.0f);
        m_turnAccel = s_turnAccel * (lbl_805FAD38 / 180.0f);
        m_flags.Set(1);
    }
    AssignVector(m_state.m_position, (const Vec3f &)info.m_position);
    m_state.m_rotation = fn_800576CC(fn_8040F760(fn_80016284((const Vec3f &)info.m_direction)));
    float dot = fn_8040CF90((const Vec3f &)info.m_aim, fn_8040CEE8((const Vec3f &)info.m_direction));
    float along = 0.0f;
    if (dot > along) {
        along = dot;
    }
    AssignVector(m_state.m_direction, fn_8040CDA8(fn_8040CEE8((const Vec3f &)info.m_direction), speed + along));
    if (s_params[m_type].m_model >= 0) {
        m_model = new UnkBulletModel_8005930C(s_params[m_type].m_modelGroup, s_params[m_type].m_model);
    } else {
        m_model = 0;
    }
    m_attack = fn_8005A300(info.m_side, info.m_owner, s_params[m_type].m_damage,
                           boost::bind(&Homing::OnHit, this, _1), boost::bind(&Homing::OnAttackEnd, this, _1));
    if (m_attack.get() != 0) {
        m_attack->m_flags |= 0x800;
    }
    StartEffect(s_params[m_type].m_effectKind, s_params[m_type].m_effect);
}

Homing::~Homing() {
    if (m_model != 0) {
        delete m_model;
    }
    if (m_attack.get() != 0) {
        fn_80053B40(&m_attack);
    }
    StopEffect();
}

} // namespace Bullet
} // namespace Weapon

/* from turned toward to by at most maxAngle radians, as a unit vector (name inferred). */
static Vec3f TurnToward(const Vec3f &from, const Vec3f &to, float maxAngle) {
    Fn8040CopyVector3 axis(fn_8040CEE8(fn_8040CFB8(from, to)));
    if (fn_80015D68(fn_8040CF90(fn_8040CEE8(from), fn_8040CEE8(to))) < maxAngle) {
        return fn_8040CEE8(to);
    }
    Mtx34f rotation;
    fn_8040C148(&rotation, &(const Mtx34f &)fn_8040FD8C((const Vec3f *)&axis, maxAngle));
    return fn_8040CEE8(fn_8040B67C(&rotation, &from));
}

namespace Weapon {
namespace Bullet {

BulletPosition Homing::Steer(const BulletPosition &state, float dt) {
    BulletPosition next;
    if (m_target.get() != 0 && fn_80061110(m_target.get())) {
        boost::shared_ptr<Target> target = fn_80061050(m_target.get())->lock();
        if (target.get() != 0) {
            Vec3f aim;
            fn_800091EC(&aim, &(const Vec3f &)target->GetPosition(state.m_position));
            AssignVector(next.m_direction,
                         TurnToward(state.m_direction, ((VectorSubtractFn)fn_8040CD74)(&aim, &state.m_position),
                                    m_turnRate * dt));
            fn_8040CE74(&next.m_direction);
            fn_8040CE38(&next.m_direction, fn_8040CF50(&state.m_direction));
        } else {
            AssignVector(next.m_direction, state.m_direction);
        }
    } else {
        AssignVector(next.m_direction, state.m_direction);
    }
    AssignVector(next.m_position, fn_8040CDD0(state.m_position, fn_8040CDA8(state.m_direction, dt)));
    next.m_rotation = fn_800576CC(fn_8040F760(fn_80016284(state.m_direction)));
    return next;
}

void Homing::Tick(float dt) {
    m_turnRate += m_turnAccel * dt;
    BulletPosition next = Steer(m_state, dt);
    m_distance += fn_8040D1B4(&next.m_position, &m_state.m_position);
    HitResult result(false);
    if (m_flags.Test(2)) {
        result = LineTest(m_attackKind, &m_target, &m_state, &next, &m_attack);
    } else {
        result = fn_8005AFE4(m_attackKind, &m_state, &next, &m_attack);
    }
    if (result.m_hit) {
        OnHitTarget(result);
        m_state = next;
        AssignVector(m_state.m_position, (const Vec3f &)result.m_position);
        delete this;
        return;
    }
    m_state = next;
    if (m_model != 0) {
        m_model->Update(&m_state);
    }
    UpdateEffect();
    if (m_target.get() != 0 && fn_80061110(m_target.get())) {
        fn_800610B4(m_target.get(), &m_state);
    } else {
        m_flags.Set(1);
    }
    if (m_flags.Test(1) && m_distance >= m_reach) {
        OnHitTarget(HitResult(false, m_state.m_position, fn_8040CEE8(m_state.m_direction), m_state.m_direction));
        delete this;
    }
}

void Homing::OnHitTarget(const HitResult &result) {
    fn_8005E32C((const Vec3f *)&result.m_position, (const Vec3f *)&result.m_normal, 0xED, m_owner);
    s32 sound = s_params[m_type].m_sound;
    if (sound >= 0) {
        fn_801D54F4(fn_80053164(), sound, (void *)&result.m_position, 0, 1);
    }
}

void Homing::StartEffect(int kind, int effect) {
    m_effect = fn_8005E374(kind, &m_state.m_position, &m_state.m_direction, effect, m_owner);
    /* A colour left unused. */
    UnkColor_8040D2EC color(200, 200, 200);
}

void Homing::StopEffect() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() != 0) {
        effect->Retire();
        m_effect.reset();
    }
}

void Homing::UpdateEffect() {
    boost::shared_ptr<EffectElement> effect = m_effect.lock();
    if (effect.get() != 0) {
        fn_80425E4C(effect.get(), &m_state.m_position);
        Vec3f rotation(m_state.m_rotation.x, m_state.m_rotation.y, m_state.m_rotation.z);
        fn_80425D94(effect.get(), &rotation);
    }
}

void Homing::OnHit(AttackCallbackParam &param) {
    /* The direction and speed of the hit at 0x34 and 0x4C (meanings inferred; see Meteor.cpp). */
    AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)), fn_8040CEE8(m_state.m_direction));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = fn_8040CF50(&m_state.m_direction);
}

void Homing::OnAttackEnd(AttackCallbackParam &param) {
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

} // namespace Bullet
} // namespace Weapon

/* The types' load and release hooks (names inferred). */
static void Initialize0() {
    fn_8006B50C(0x2E, 0);
}

static void Initialize1() {
    fn_8006B50C(0x2F, 0);
}

static void Initialize2() {
    fn_8006B50C(0x2F, 0);
}

static void Finalize() {}

extern UnkBulletType_80546018 lbl_8051DD50;
extern UnkBulletType_80546018 lbl_8051DD64;
extern UnkBulletType_80546018 lbl_8051DD78;
extern UnkBulletType_80546018 lbl_8051DD8C;
extern UnkBulletType_80546018 lbl_8051DDA0;
extern UnkBulletType_80546018 lbl_8051DDB4;
extern UnkBulletType_80546018 lbl_8051DDC8;
extern UnkBulletType_80546018 lbl_8051DDDC;

/* Fires a shot of each type (names inferred). */
static void Create0(const ShotInfo &info) {
    new Homing(info, 300.0f, 0);
}

static void Create1(const ShotInfo &info) {
    new Homing(info, 300.0f, 1);
}

static void Create2(const ShotInfo &info) {
    new Homing(info, 300.0f, 1);
}

static void Create3(const ShotInfo &info) {
    new Homing(info, lbl_8051DD50.m_reach, 2);
}

static void Create4(const ShotInfo &info) {
    new Homing(info, lbl_8051DD64.m_reach, 2);
}

static void Create5(const ShotInfo &info) {
    new Homing(info, lbl_8051DD78.m_reach, 3);
}

static void Create6(const ShotInfo &info) {
    new Homing(info, lbl_8051DD8C.m_reach, 4);
}

static void Create7(const ShotInfo &info) {
    new Homing(info, lbl_8051DDA0.m_reach, 5);
}

static void Create8(const ShotInfo &info) {
    new Homing(info, lbl_8051DDB4.m_reach, 6);
}

static void Create9(const ShotInfo &info) {
    new Homing(info, lbl_8051DDC8.m_reach, 1);
}

static void Create10(const ShotInfo &info) {
    new Homing(info, lbl_8051DDDC.m_reach, 7);
}

UnkBulletType_80546018 lbl_8051DD14 = {Initialize0, Finalize, Create0, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051DD28 = {Initialize1, Finalize, Create1, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051DD3C = {Initialize2, Finalize, Create2, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051DD50 = {0, 0, Create3, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051DD64 = {0, 0, Create4, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051DD78 = {0, 0, Create5, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051DD8C = {0, 0, Create6, 0, Plain(2000.0f)};
UnkBulletType_80546018 lbl_8051DDA0 = {0, 0, Create7, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051DDB4 = {0, 0, Create8, 0, Plain(300.0f)};
UnkBulletType_80546018 lbl_8051DDC8 = {0, 0, Create9, 0, Plain(1500.0f)};
UnkBulletType_80546018 lbl_8051DDDC = {0, 0, Create10, 0, Plain(1500.0f)};

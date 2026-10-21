/* The control units both stages of the Black Bull boss share (class names from RTTI:
 * BlackBullCtrlUnit_*): the status update, the movement and turning, the neck and eye aim, the
 * attack callbacks of the body, eyes and the rest, the knock-back, the dizzy spell, the motion
 * attributes, and the fire attacks (the fire ball, the explosion and the ring of fire it leaves, and
 * the walls of fire the ring sends out). File name inferred from the classes; names marked
 * "(name inferred)" and all member names are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define BLACKBULLVARPARAM_VECTORS
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/BlackBullCtrlUnit.h"
#include "Game/BlackBullCtrlUnits.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in DevilDoomCtrlUnit.cpp), and
 * the game's one shared std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls (an overload stands in for it, as in CatchStick.cpp). */
namespace std {
float fabs(float x);
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
} // namespace std

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* What a motion passes to its callbacks (as in DevilDoomCtrlUnit.cpp). */
struct _stMtnCallbackParam {
    /* 0x0 */ void *m_frame;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ s32 m_id;
};

/* The boss's lists (as in Game/DevilDoomCtrlUnit.h). */
struct UnkListNode_80179304;
struct UnkListIter_80179304 {
    UnkListIter_80179304() : m_node(0) {}

    UnkListNode_80179304 *m_node;
};

/* The parts of AttackCallbackParam the units read (as in DevilDoomCtrlUnit.cpp). */
struct BlackBullAttackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ u8 field_0x24[0x34 - 0x24];
    /* 0x34 */ Vec3f m_offset;
    /* 0x40 */ u8 field_0x40[0x50 - 0x40];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

#define Param(param) (*(BlackBullAttackParam *)&(param))

/* The effect manager fn_800A31F0 returns (as in DevilDoomCtrlUnit.cpp). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> LaunchId(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* The boss's effect launcher, read as the PJS launcher it holds (as in HeavyDog0410CtrlUnit.cpp):
 * retail converts it through a local converting constructor. */

/* The game state (as in DevilDoomCtrlUnit.cpp): the frame time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The frame time the effects follow. */
#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

/* The save data (see EggLastMech.cpp): the record at 0x70C keeps the score. */
struct UnkObject_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ u8 m_score[1];
};

/* A line for the collision test (see DiablonPartsBeam.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};
struct UnkLineHits_80021FE4;
struct UnkLineHit_8002735C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

class UnkCharaList_80074194;

/* The player's character id, and the null owner of colliders. */
extern s32 lbl_805EE588;
extern boost::shared_ptr<CharInfo> lbl_805EE698;

extern "C" {
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
/* Turns an effect (behaviour inferred). */
void fn_80425D94(EffectElement *effect, const Vec3f *rotation);
boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation, int id);

void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);

UnkObject_80079808 *fn_80079808(void);
void fn_8016F0CC(void *score, int arg1, int points);

UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
/* Whether the player is on the ground, and whether hit (behaviour inferred). */
bool fn_800B79C0(const boost::shared_ptr<CharInfo> *chara);
bool fn_800B7AD0(const boost::shared_ptr<CharInfo> *chara);
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);

UnkListNode_80179304 *fn_8017935C(UnkList_80179304 *list);
UnkListIter_80179304 fn_80179354(UnkListNode_80179304 *node);
UnkListNode_80179304 *fn_801792D8(UnkList_80179304 *list);
boost::shared_ptr<CharaColliBase> fn_80179304(const UnkListIter_80179304 *it);
UnkListIter_80179304 fn_801792E0(UnkListIter_80179304 *it, int);
EnemyCharInfo *fn_801792D0(EnemyCharInfo *const *body);

void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void *fn_80020D40(void);
UnkLineHits_80021FE4 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                  void *filter, int arg5);
UnkLineHit_8002735C *fn_8002735C(UnkLineHits_80021FE4 *hits);
const Vec3f *fn_8001E9BC(void *object);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);

void fn_80016370(Mtx34f *matrix, void *frame);
void fn_800163F4(void *frame, const Mtx34f *matrix);
void fn_80415808(_stMtnPlayWork *work, int id, void (*callback)(void *, void *), void *data);
void fn_80335844(void *, void *);

float fn_8000FEE4(float value); /* square root */
float fn_8000F3D4(float angle);
float fn_8000F3F8(float value);
float fn_80015D68(float value); /* arc cosine */
float fn_80016260(float x, float z); /* the angle of (x, z) */
float *fn_8040C17C(Mtx34f *matrix, int row, int column);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Mtx34f fn_8040EFA4(const Mtx34f *a, const Mtx34f *b, float t);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040CE74(Vec3f *vector);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CE04(Vec3f *vector, const Vec3f *value);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

inline Vec3f Sub(const Vec3f &a, const Vec3f &b) {
    return ((VectorSubtractFn)fn_8040CD74)(&a, &b);
}

/* Degrees to radians (name inferred). */
inline float ToRadian(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* A wall of fire that runs from position along velocity (behaviour inferred). */
class BlackBullCtrlUnit_WallFire : public BlackBullCtrlUnit, public AttackCallbackContainer {
  public:
    BlackBullCtrlUnit_WallFire(BlackBullVarParam *param, const Vec3f *position,
                               const Vec3f *velocity);
    virtual ~BlackBullCtrlUnit_WallFire();
    virtual int Tick(float dt);
    virtual void OnHit(AttackCallbackParam &param);

    /* 0x10 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ Vec3f m_start;
    /* 0x30 */ Vec3f m_velocity;
    /* 0x3C */ float m_angle;
    /* 0x40 */ boost::shared_ptr<CharaColliBase> m_collider;
};
SIZE_ASSERT(BlackBullCtrlUnit_WallFire, 0x58);

/* The explosion where a fire ball lands (behaviour inferred). */
class BlackBullCtrlUnit_FireExplosion : public BlackBullCtrlUnit, public AttackCallbackContainer {
  public:
    BlackBullCtrlUnit_FireExplosion(BlackBullVarParam *param, const Vec3f *position, float radius);
    virtual ~BlackBullCtrlUnit_FireExplosion();
    virtual int Tick(float dt);
    virtual void OnHit(AttackCallbackParam &param);

    /* 0x10 */ float m_radius; /* negative until the blast starts */
    /* 0x14 */ float m_maxRadius;
    /* 0x18 */ float m_time;
    /* 0x1C */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x24 */ Vec3f m_position;
    /* 0x30 */ boost::shared_ptr<CharaColliBase> m_collider;
};
SIZE_ASSERT(BlackBullCtrlUnit_FireExplosion, 0x48);

/* Pushes the boss back along velocity, slowing down (behaviour inferred). */
class BlackBullCtrlUnit_KnockOff : public BlackBullCtrlUnit {
  public:
    BlackBullCtrlUnit_KnockOff(BlackBullVarParam *param, const Vec3f *velocity);
    virtual ~BlackBullCtrlUnit_KnockOff();
    virtual int Tick(float dt);

    /* 0x0C */ Vec3f m_velocity;
};
SIZE_ASSERT(BlackBullCtrlUnit_KnockOff, 0x28);

/* Turns matrix toward target by at most maxAngle (name inferred; as in DevilDoomCtrlUnit.cpp). */
static void TurnToward(Mtx34f *matrix, const Vec3f *target, float maxAngle) {
    float limit = fn_8000F3D4(maxAngle);
    Vec3f local;
    fn_800091EC(&local, &Ref(fn_8040B67C(&RefM(fn_8040BEF8(matrix)), target)));
    float z = std::fabs(local.z);
    if (z < 0.0001) {
        float length = fn_8000FEE4(local.x * local.x + local.y * local.y);
        if (length < 0.0001) {
            return;
        }
        float scale = limit / length;
        local.x *= scale;
        local.y *= scale;
    } else {
        local.x /= z;
        local.y /= z;
        float length = fn_8000FEE4(local.x * local.x + local.y * local.y);
        if (length > limit) {
            float scale = limit / length;
            local.x *= scale;
            local.y *= scale;
        }
    }
    float pitch = fn_8000F3F8(local.y);
    float yaw = fn_8000F3F8(-local.x);
    fn_8040B884(matrix, pitch);
    fn_8040B848(matrix, yaw);
}

BlackBullCtrlUnit_GetStatus::BlackBullCtrlUnit_GetStatus(BlackBullVarParam *param)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 0;
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara) {
        fn_800091D0(&m_owner->m_prevDestination, &chara->m_position);
    }
}

BlackBullCtrlUnit_GetStatus::~BlackBullCtrlUnit_GetStatus() {}

int BlackBullCtrlUnit_GetStatus::Tick(float dt) {
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara) {
        fn_800091D0(&m_owner->m_target, &chara->m_position);
        if (!m_owner->field_0x122) {
            fn_800091D0(&m_owner->m_testDestination, &chara->m_position);
        }
        if (fn_800B79C0(&chara)) {
            m_owner->field_0x168 = 1;
        } else {
            m_owner->field_0x168 = 0;
        }
        if (fn_800B7AD0(&chara)) {
            m_owner->m_hit = 1;
        } else {
            m_owner->m_hit = 0;
        }
    }
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(m_owner->m_testDestination, m_owner->m_prevDestination)));
    float length = fn_8040CF50(&offset);
    float speed = m_owner->field_0xDC;
    if (length > speed * dt) {
        fn_800091D0(&offset, &Ref(fn_8040CDA8(&Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&offset)), speed)),
                                              dt)));
        fn_800091D0(&m_owner->m_testDestination,
                    &Ref(fn_8040CDD0(&m_owner->m_prevDestination, &offset)));
    }
    fn_800091D0(&m_owner->m_prevDestination, &m_owner->m_testDestination);
    fn_800091D0(&offset, &Ref(Sub(m_owner->m_testDestination, m_owner->m_chara->m_position)));
    offset.y = 0.0f;
    m_owner->field_0x15C = fn_8040CF50(&offset);
    m_owner->m_targetAngle =
        fn_80016260(-m_owner->m_testDestination.x, -m_owner->m_testDestination.z);
    m_owner->field_0x164 = fn_80016260(-offset.x, -offset.z);
    m_owner->m_angle =
        fn_80016260(-m_owner->m_chara->m_position.x, -m_owner->m_chara->m_position.z);
    return false;
}

BlackBullCtrlUnit_AttackCallback::BlackBullCtrlUnit_AttackCallback(BlackBullVarParam *param,
                                                                   int index)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 0;
    UnkListIter_80179304 it;
    it = fn_80179354(fn_8017935C(m_owner->m_colliders));
    for (int i = 0; it.m_node != fn_801792D8(m_owner->m_colliders); fn_801792E0(&it, 0), i++) {
        if (i == index) {
            boost::weak_ptr<CharaColliAttack> attack =
                fn_800C42D0(fn_80021B08(), fn_80179304(&it));
            boost::shared_ptr<CharaColliAttack> collision(attack.lock());
            collision->m_hitCallback = GetHitCallback();
            collision->m_attackCallback = GetDamageCallback();
        }
    }
}

void BlackBullCtrlUnit_AttackCallbackBody::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = Param(param).m_attacker.lock()->m_owner.lock();
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, attacker->m_position)));
    Param(param).m_damage = Param(param).m_attacker.lock()->m_power;
}

void BlackBullCtrlUnit_AttackCallbackBody::OnDamage(AttackCallbackParam &param) {
    if (m_owner->m_invincible) {
        Param(param).m_result = 5;
    } else {
        fn_803358D0(m_owner, new BossCtrlUnit_DamageEff(m_owner));
        Vec3f rotation;
        fn_800091EC(&rotation, &lbl_805E07A0);
        fn_801A1F54(&Param(param).m_position, &rotation, 0x225);
        Param(param).m_result = 1;
    }
}

void BlackBullCtrlUnit_AttackCallbackEye::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = Param(param).m_attacker.lock()->m_owner.lock();
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, attacker->m_position)));
    Param(param).m_damage = Param(param).m_attacker.lock()->m_power;
}

void BlackBullCtrlUnit_AttackCallbackEye::OnDamage(AttackCallbackParam &param) {
    if (m_owner->m_invincible) {
        Param(param).m_result = 5;
        return;
    }
    Param(param).m_result = 1;
    fn_803358D0(m_owner, new BossCtrlUnit_DamageEff(m_owner));
    Vec3f rotation;
    fn_800091EC(&rotation, &lbl_805E07A0);
    fn_801A1F54(&Param(param).m_position, &rotation, 0x69);
    float before = m_owner->m_health;
    m_owner->m_health = before - Param(param).m_damage;
    if (m_owner->m_health < 0.0f) {
        m_owner->m_health = 0.0f;
    }
    float after = m_owner->m_health;
    m_owner->m_damage += Param(param).m_damage;
    fn_8016F0CC(fn_80079808()->m_score, 0, m_owner->m_score * Param(param).m_damage);
    boost::shared_ptr<CharaColliAttack> collision = Param(param).m_attacker.lock();
    if (collision->m_flags & 0xBC0) {
        m_owner->field_0x178 += Param(param).m_damage;
    }
    if (collision->m_flags & 8) {
        Vec3f direction;
        fn_800091EC(&direction, &Param(param).m_offset);
        direction.y = 0.0f;
        fn_8040CE74(&direction);
        fn_8040CE38(&direction, m_owner->field_0xA4);
        fn_803358D0(m_owner, new BlackBullCtrlUnit_KnockOff(m_owner, &direction));
    }
    if (after <= 0.0f) {
        m_owner->m_nextMode = 9;
        return;
    }
    if (before / m_owner->m_maxHealth > 0.5f && after / m_owner->m_maxHealth <= 0.5f) {
        m_owner->m_nextMode = 8;
        return;
    }
    if (collision->m_flags & 8) {
        m_owner->m_nextMode = 7;
        return;
    }
}

void BlackBullCtrlUnit_AttackCallbackOther::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = Param(param).m_attacker.lock()->m_owner.lock();
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, attacker->m_position)));
    Param(param).m_damage = Param(param).m_attacker.lock()->m_power;
}

void BlackBullCtrlUnit_AttackCallbackOther::OnDamage(AttackCallbackParam &param) {
    if (m_owner->m_invincible) {
        Param(param).m_result = 5;
    } else {
        Param(param).m_result = 1;
        fn_803358D0(m_owner, new BossCtrlUnit_DamageEff(m_owner));
        Vec3f rotation;
        fn_800091EC(&rotation, &lbl_805E07A0);
        fn_801A1F54(&Param(param).m_position, &rotation, 0x225);
    }
}

BlackBullCtrlUnit_Move::BlackBullCtrlUnit_Move(BlackBullVarParam *param)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 2;
}

BlackBullCtrlUnit_Move::~BlackBullCtrlUnit_Move() {}

int BlackBullCtrlUnit_Move::Tick(float dt) {
    BlackBullVarParam *owner = m_owner;
    float angle = owner->field_0x164;
    if (!owner->m_motionLocked) {
        Vec3f offset;
        fn_800091EC(&offset, &Ref(Sub(owner->m_destination, owner->m_chara->m_position)));
        Vec3f force;
        fn_800091EC(&force, &Ref(fn_8040CDA8(&offset, m_owner->field_0x74)));
        fn_8040CE04(&force, &Ref(fn_800091A4(&m_owner->m_velocity, m_owner->field_0x78)));
        fn_8040CFF8(&m_owner->m_velocity, &Ref(fn_800091A4(&force, dt)));
        fn_8040CFF8(&fn_801792D0(&m_owner->m_chara)->m_position,
                    &Ref(fn_800091A4(&m_owner->m_velocity, dt)));
        if (m_owner->m_moving && m_owner->m_destinationDistance > m_owner->m_arriveDistance) {
            angle = fn_80016260(-m_owner->m_velocity.x, -m_owner->m_velocity.z);
        }
    } else {
        fn_800091D0(&owner->m_velocity, &lbl_805E07A0);
    }
    m_owner->m_destinationDistance =
        fn_8040CF50(&Ref(Sub(m_owner->m_destination, m_owner->m_chara->m_position)));
    if (!m_owner->m_motionLocked2) {
        float diff = angle - m_owner->m_chara->m_rotation.y;
        while (std::fabs(diff) > lbl_805FAD38) {
            diff += diff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
        }
        float accel = diff * m_owner->field_0x80;
        accel -= m_owner->m_turnRate * m_owner->field_0x84;
        m_owner->m_turnRate += accel * dt;
        m_owner->m_chara->m_rotation.y += m_owner->m_turnRate * dt;
    } else {
        m_owner->m_turnRate = 0.0f;
    }
    m_owner->m_turnSpeed = angle - m_owner->m_chara->m_rotation.y;
    while (std::fabs(m_owner->m_turnSpeed) > lbl_805FAD38) {
        m_owner->m_turnSpeed +=
            m_owner->m_turnSpeed > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
    }
    return false;
}

BlackBullCtrlUnit_KnockOff::BlackBullCtrlUnit_KnockOff(BlackBullVarParam *param,
                                                       const Vec3f *velocity)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 2;
    fn_800091D0(&m_velocity, velocity);
    if (m_owner->field_0xAC > 0.0f) {
        Mtx34f rotation;
        Vec3f direction;
        fn_800091EC(&direction, velocity);
        Vec3f side;
        fn_800091EC(&side, &m_owner->m_chara->m_position);
        float x = std::fabs(side.x);
        float z = std::fabs(side.z);
        if (std::fabs(x - z) < 10.0f) {
            Vec3f alongX(side.x, 0.0f, 0.0f);
            Vec3f alongZ(0.0f, 0.0f, side.z);
            float dotZ = fn_8040CF90(&direction, &alongZ);
            if (fn_8040CF90(&direction, &alongX) > dotZ) {
                fn_800091D0(&side, &alongX);
            } else {
                fn_800091D0(&side, &alongZ);
            }
        } else if (x > z) {
            side.z = 0.0f;
        } else {
            side.x = 0.0f;
        }
        side.y = 0.0f;
        direction.y = 0.0f;
        fn_8040CE74(&direction);
        fn_8040CE74(&side);
        float dot = fn_8040CF90(&direction, &side);
        Vec3f cross;
        fn_800091EC(&cross, &Ref(fn_8040CFB8(&direction, &side)));
        float angle = 0.5f * lbl_805FAD38 - fn_80015D68(dot);
        float limit = ToRadian(m_owner->field_0xAC);
        if (angle > limit) {
            angle = limit;
        }
        if (angle < -limit) {
            angle = -limit;
        }
        fn_8040C148(&rotation, &lbl_805E0720);
        if (cross.y < 0.0f) {
            fn_8040B848(&rotation, angle);
        } else {
            fn_8040B848(&rotation, -angle);
        }
        fn_800091D0(&m_velocity, &Ref(fn_8040B618(&rotation, &m_velocity)));
    }
}

BlackBullCtrlUnit_KnockOff::~BlackBullCtrlUnit_KnockOff() {}

int BlackBullCtrlUnit_KnockOff::Tick(float dt) {
    float speed = fn_8040CF50(&m_velocity);
    if (speed < m_owner->field_0xA8) {
        return true;
    }
    fn_8040CE38(&m_velocity, 1.0f - m_owner->field_0xA8 / speed);
    fn_8040CFF8(&fn_801792D0(&m_owner->m_chara)->m_position, &Ref(fn_800091A4(&m_velocity, dt)));
    return false;
}

BlackBullCtrlUnit_AdjustEye::BlackBullCtrlUnit_AdjustEye(BlackBullVarParam *param)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 2;
    m_callback = GetCallback();
    fn_80415808(m_owner->m_motion, 0x1394, fn_80335844, &m_callback);
    m_blend = 0.0f;
}

BlackBullCtrlUnit_AdjustEye::~BlackBullCtrlUnit_AdjustEye() {
    fn_80415808(m_owner->m_motion, 0x1394, 0, 0);
}

int BlackBullCtrlUnit_AdjustEye::Tick(float dt) {
    if (m_owner->field_0x170) {
        m_blend += m_owner->field_0x9C * dt;
        if (m_blend > 1.0f) {
            m_blend = 1.0f;
        }
    } else {
        m_blend -= m_owner->field_0x9C * dt;
        if (m_blend < 0.0f) {
            m_blend = 0.0f;
        }
    }
    return false;
}

void BlackBullCtrlUnit_AdjustEye::OnMotion(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    Mtx34f aimed;
    fn_8040C148(&aimed, &matrix);
    TurnToward(&aimed, &m_owner->m_testDestination, lbl_805FAD38 / 3.0f);
    Mtx34f blended;
    fn_8040C148(&blended, &RefM(fn_8040EFA4(&matrix, &aimed, m_blend)));
    fn_800163F4(param.m_frame, &blended);
}

inline UnkMotionCallback_802414D4 &UnkMotionCallback_802414D4::operator=(
    const MotionCallbackFunction &f) {
    m_function = f;
    return *this;
}

BlackBullCtrlUnit_AdjustNeck::BlackBullCtrlUnit_AdjustNeck(BlackBullVarParam *param)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 2;
    m_callback = GetCallback();
    fn_80415808(m_owner->m_motion, 0x1392, fn_80335844, &m_callback);
    m_blend = 0.0f;
}

BlackBullCtrlUnit_AdjustNeck::~BlackBullCtrlUnit_AdjustNeck() {
    fn_80415808(m_owner->m_motion, 0x1392, 0, 0);
}

int BlackBullCtrlUnit_AdjustNeck::Tick(float dt) {
    if (m_owner->field_0x171) {
        m_blend += m_owner->field_0xA0 * dt;
        if (m_blend > 1.0f) {
            m_blend = 1.0f;
        }
    } else {
        m_blend -= m_owner->field_0xA0 * dt;
        if (m_blend < 0.0f) {
            m_blend = 0.0f;
        }
    }
    return false;
}

void BlackBullCtrlUnit_AdjustNeck::OnMotion(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    Mtx34f aimed;
    fn_8040C148(&aimed, &matrix);
    Mtx34f blended;
    Vec3f target;
    fn_800091EC(&target, &m_owner->m_testDestination);
    Vec3f neck(*fn_8040C17C(&matrix, 0, 3), *fn_8040C17C(&matrix, 1, 3),
               *fn_8040C17C(&matrix, 2, 3));
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(target, neck)));
    offset.y = 0.0f;
    float near = (1000.0f - fn_8040CF50(&offset)) / 1000.0f;
    float limit = near < 0.0f ? 0.0f : neck.y * near;
    if (target.y > limit) {
        target.y = limit;
    }
    fn_8040B884(&aimed, ToRadian(m_owner->field_0x16C));
    TurnToward(&aimed, &target, lbl_805FAD38 / 6.0f);
    fn_8040B884(&aimed, ToRadian(-m_owner->field_0x16C));
    fn_8040C148(&blended, &RefM(fn_8040EFA4(&matrix, &aimed, m_blend)));
    fn_800163F4(param.m_frame, &blended);
}

BlackBullCtrlUnit_FireBall::BlackBullCtrlUnit_FireBall(BlackBullVarParam *param,
                                                       const Vec3f *position,
                                                       const Vec3f *velocity, int kind)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 2;
    fn_800091D0(&m_position, fn_800091D0(&m_start, position));
    fn_800091D0(&m_velocity, velocity);
    m_radius = m_owner->field_0xCC;
    field_0x14 = 0.0f;
    m_type = kind;
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 9);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        float scale = m_radius / 20.0f;
        fn_8040B720(&matrix, scale, scale, scale);
        fn_80425E68(effect.get(), &matrix);
        effect->SetField4C((u32)&m_position);
        effect->SetField14(FrameTime());
    }
    void *manager = fn_80021B08();
    m_collider = fn_800C7EA8(manager, &m_position, 0, 4, 0, lbl_805EE698, &lbl_805E07A0, m_radius);
    if (m_collider) {
        m_collider->SetField2C(1);
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
        boost::shared_ptr<CharaColliAttack> collision(attack.lock());
        if (collision) {
            collision->m_power = 1.0f;
            collision->m_kind = 4;
            collision->m_flags = 0x40;
            collision->field_0x54 = 1;
            collision->m_hitCallback = GetHitCallback();
            *reinterpret_cast<u32 *>(collision->field_0x3C) = 4;
            collision->field_0x55 = 1;
            collision->m_attackCallback = GetDamageCallback();
        }
    }
}

BlackBullCtrlUnit_FireBall::~BlackBullCtrlUnit_FireBall() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetField4C(0);
        effect->Stop();
        fn_800A4210(effect);
    }
    if (m_collider) {
        m_collider.reset();
    }
}

int BlackBullCtrlUnit_FireBall::Tick(float dt) {
    bool hit = false;
    Vec3f previous;
    fn_800091EC(&previous, &m_position);
    fn_8040CFF8(&m_position, &Ref(fn_800091A4(&m_velocity, dt)));
    UnkLine_80020A20 line;
    fn_80020A20(&line, &previous, &m_position);
    UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 6, 0, 0);
    if (hits != 0) {
        fn_800091D0(&m_position, fn_8001E9BC(fn_8002735C(hits)->m_object));
        hit = true;
        fn_80026FB4(hits, 1);
    }
    if (fn_8040CF50(&Ref(Sub(m_position, m_start))) > m_owner->field_0xD4) {
        return true;
    }
    fn_801D53C4(fn_80053164(), 0x7B37, &m_position, this, 3, 0, 3);
    if (hit) {
        switch (m_type) {
        case 0:
            fn_803358D0(m_owner, new BlackBullCtrlUnit_FireExplosion(m_owner, &m_position,
                                                                      m_owner->field_0xD8));
            break;
        case 1:
            fn_803358D0(m_owner, new BlackBullCtrlUnit_WaveRingFire(m_owner, &m_position, 0));
            break;
        case 2:
            fn_803358D0(m_owner, new BlackBullCtrlUnit_WaveRingFire(m_owner, &m_position, 1));
            break;
        }
        return true;
    }
    if (CharaColliBase *collider = m_collider.get()) {
        collider->SetCharInfo((CharInfo *)&m_position);
    }
    return false;
}

void BlackBullCtrlUnit_FireBall::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, m_position)));
    Param(param).m_damage = attack.lock()->m_power;
}

void BlackBullCtrlUnit_FireBall::OnDamage(AttackCallbackParam &param) {
    Param(param).m_result = 5;
}

BlackBullCtrlUnit_FireExplosion::BlackBullCtrlUnit_FireExplosion(BlackBullVarParam *param,
                                                                 const Vec3f *position,
                                                                 float radius)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 2;
    fn_800091D0(&m_position, position);
    m_maxRadius = radius;
    m_radius = -1.0f;
    m_time = 0.0f;
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x11);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_position);
        float scale = m_maxRadius / 50.0f;
        fn_8040B720(&matrix, scale, scale, scale);
        fn_80425E68(effect.get(), &matrix);
        effect->SetField14(FrameTime());
    }
}

BlackBullCtrlUnit_FireExplosion::~BlackBullCtrlUnit_FireExplosion() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
    if (m_collider) {
        m_collider.reset();
    }
}

int BlackBullCtrlUnit_FireExplosion::Tick(float dt) {
    if (m_collider) {
        m_collider.reset();
    }
    m_time += dt;
    if (m_radius < 0.0f && m_time > 0.167f) {
        m_radius = m_maxRadius;
        fn_801D54F4(fn_80053164(), 0x7B38, &m_position, 0, 1);
    }
    if (m_time > 0.5f) {
        return true;
    }
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(m_owner->m_target, m_position)));
    if (fn_8040CF50(&offset) < m_radius) {
        void *manager = fn_80021B08();
        fn_801D54F4(fn_80053164(), 0x7B39, &m_owner->m_target, 0, 1);
        m_collider = fn_800C7EA8(manager, &m_owner->m_target, 0, 4, 0, lbl_805EE698,
                                 &lbl_805E07A0, 10.0f);
        if (m_collider) {
            m_collider->SetField2C(1);
            boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
            boost::shared_ptr<CharaColliAttack> collision(attack.lock());
            if (collision) {
                collision->m_power = 1.0f;
                collision->m_kind = 4;
                collision->m_flags = 0x40;
                collision->field_0x54 = 1;
                collision->m_hitCallback = GetHitCallback();
            }
        }
    }
    return false;
}

void BlackBullCtrlUnit_FireExplosion::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, m_position)));
    Param(param).m_damage = attack.lock()->m_power;
}

BlackBullCtrlUnit_WaveRingFire::BlackBullCtrlUnit_WaveRingFire(BlackBullVarParam *param,
                                                               const Vec3f *center,
                                                               int spawnWalls)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 2;
    fn_800091D0(&m_center, center);
    m_spawnWalls = spawnWalls;
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 5);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetMatrix(&m_matrix);
        effect->SetField14(FrameTime());
    }
    m_radius = 0.0f;
    m_speed = m_owner->field_0xB4 / 120.0f;
    fn_8040C148(&m_matrix, &lbl_805E0720);
    fn_8040B720(&m_matrix, m_radius, 1.0f, m_radius);
    fn_8040B59C(&m_matrix, &m_center);
    fn_801D54F4(fn_80053164(), 0x7B38, &m_center, 0, 1);
}

BlackBullCtrlUnit_WaveRingFire::~BlackBullCtrlUnit_WaveRingFire() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetField4C(0);
        effect->SetField50(0);
        effect->Stop();
        fn_800A4210(effect);
    }
    if (m_collider) {
        m_collider.reset();
    }
}

int BlackBullCtrlUnit_WaveRingFire::Tick(float dt) {
    if (m_collider) {
        m_collider.reset();
    }
    m_radius += m_speed * dt;
    fn_8040C148(&m_matrix, &lbl_805E0720);
    fn_8040B59C(&m_matrix, &m_center);
    fn_8040B720(&m_matrix, m_radius, 1.0f, m_radius);
    fn_801D53C4(fn_80053164(), 0x7B3A, &m_center, this, 3, 0, 3);
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(m_owner->m_target, m_center)));
    float height = offset.y;
    offset.y = 0.0f;
    float distance = fn_8040CF50(&offset);
    if (120.0f * m_radius > distance && 120.0f * m_radius - m_owner->field_0xC0 < distance &&
        std::fabs(height) < m_owner->field_0xBC) {
        void *manager = fn_80021B08();
        fn_801D54F4(fn_80053164(), 0x7B39, &m_owner->m_target, 0, 1);
        m_collider = fn_800C7EA8(manager, &m_owner->m_target, 0, 4, 0, lbl_805EE698,
                                 &lbl_805E07A0, 10.0f);
        if (m_collider) {
            m_collider->SetField2C(1);
            boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
            boost::shared_ptr<CharaColliAttack> collision(attack.lock());
            if (collision) {
                collision->m_power = 1.0f;
                collision->m_kind = 4;
                collision->m_flags = 0x40;
                collision->field_0x54 = 1;
                collision->m_hitCallback = GetHitCallback();
            }
        }
    }
    if (120.0f * m_radius > m_owner->field_0xB8) {
        if (m_spawnWalls) {
            Vec3f position;
            Vec3f velocity;
            fn_800091D0(&position, &m_center);
            fn_800091D0(&velocity, &lbl_805E07A0);
            if (m_owner->m_chara->m_position.z < 0.0f) {
                position.z += m_owner->field_0xB8 - 60.0f;
                velocity.z = m_owner->field_0xB4;
            } else {
                position.z -= m_owner->field_0xB8 - 60.0f;
                velocity.z = -m_owner->field_0xB4;
            }
            fn_803358D0(m_owner, new BlackBullCtrlUnit_WallFire(m_owner, &position, &velocity));
            fn_800091D0(&position, &m_center);
            fn_800091D0(&velocity, &lbl_805E07A0);
            if (m_owner->m_chara->m_position.x < 0.0f) {
                position.x += m_owner->field_0xB8 - 60.0f;
                velocity.x = m_owner->field_0xB4;
            } else {
                position.x -= m_owner->field_0xB8 - 60.0f;
                velocity.x = -m_owner->field_0xB4;
            }
            fn_803358D0(m_owner, new BlackBullCtrlUnit_WallFire(m_owner, &position, &velocity));
        }
        return true;
    }
    return false;
}

void BlackBullCtrlUnit_WaveRingFire::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, m_center)));
    Param(param).m_damage = attack.lock()->m_power;
}

BlackBullCtrlUnit_WallFire::BlackBullCtrlUnit_WallFire(BlackBullVarParam *param,
                                                       const Vec3f *position,
                                                       const Vec3f *velocity)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 2;
    fn_800091D0(&m_position, fn_800091D0(&m_start, position));
    fn_800091D0(&m_velocity, velocity);
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0xD);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        Vec3f rotation;
        fn_800091EC(&rotation, &lbl_805E07A0);
        rotation.y = m_angle = fn_80016260(-velocity->x, -velocity->z);
        fn_80425D94(effect.get(), &rotation);
        effect->SetField4C((u32)&m_position);
        effect->SetField14(FrameTime());
    }
}

BlackBullCtrlUnit_WallFire::~BlackBullCtrlUnit_WallFire() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetField4C(0);
        effect->SetField50(0);
        effect->Stop();
        fn_800A4210(effect);
    }
    if (m_collider) {
        m_collider.reset();
    }
}

int BlackBullCtrlUnit_WallFire::Tick(float dt) {
    fn_8040CFF8(&m_position, &Ref(fn_800091A4(&m_velocity, dt)));
    fn_801D53C4(fn_80053164(), 0x7B3A, &m_position, this, 3, 0, 3);
    if (m_collider) {
        m_collider.reset();
    }
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B59C(&matrix, &m_position);
    fn_8040B848(&matrix, m_angle);
    Vec3f local;
    fn_800091EC(&local, &Ref(fn_8040B67C(&RefM(fn_8040BEF8(&matrix)), &m_owner->m_target)));
    if (std::fabs(local.x) < 100.0f && std::fabs(local.y) < m_owner->field_0xBC &&
        local.z < m_owner->field_0xC0 - 60.0f && local.z > -60.0f) {
        void *manager = fn_80021B08();
        m_collider = fn_800C7EA8(manager, &m_owner->m_target, 0, 4, 0, lbl_805EE698,
                                 &lbl_805E07A0, 10.0f);
        fn_801D54F4(fn_80053164(), 0x7B39, &m_owner->m_target, 0, 1);
        if (m_collider) {
            m_collider->SetField2C(1);
            boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
            boost::shared_ptr<CharaColliAttack> collision(attack.lock());
            if (collision) {
                collision->m_power = 1.0f;
                collision->m_kind = 4;
                collision->m_flags = 0x40;
                collision->field_0x54 = 1;
                collision->m_hitCallback = GetHitCallback();
            }
        }
    }
    return fn_8040CF50(&m_position) > fn_8000FEE4(1280000.0f);
}

void BlackBullCtrlUnit_WallFire::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset, &m_velocity);
    Param(param).m_damage = attack.lock()->m_power;
}

BlackBullCtrlUnit_Dizzy::BlackBullCtrlUnit_Dizzy(BlackBullVarParam *param)
    : BossCtrlUnit(param), BlackBullCtrlUnit(param) {
    m_kind = 2;
    m_time = m_owner->field_0xEC;
    m_speed = m_owner->field_0xF0 / m_time;
}

BlackBullCtrlUnit_Dizzy::~BlackBullCtrlUnit_Dizzy() {}

int BlackBullCtrlUnit_Dizzy::Tick(float dt) {
    m_time -= dt;
    m_owner->m_chara->m_position.y -= dt * m_speed;
    if (m_time < 0.0f) {
        m_owner->m_dizzyEnded = 1;
        return true;
    }
    if (m_owner->m_damage > m_owner->m_damageLimit) {
        m_owner->m_dizzyEnded = 1;
        return true;
    }
    return false;
}

/* Plays the sound of motion attribute 0x5001's value, and keeps attribute 0x5000's
 * (meanings inferred). */
void BlackBullCtrlUnit_CheckMotionAttribute::OnAttribute(u16 attribute, int value) {
    switch (attribute) {
    case 0x5001:
        switch (*reinterpret_cast<int *>(value)) {
        case 0x2E:
            fn_801D54F4(fn_80053164(), 0x7B2E, m_owner->m_chara, 0, 1);
            break;
        case 0x30:
            fn_801D54F4(fn_80053164(), 0x7B30, m_owner->m_chara, 0, 1);
            break;
        case 0x31:
            fn_801D54F4(fn_80053164(), 0x7B31, m_owner->m_chara, 0, 1);
            break;
        case 0x32:
            fn_801D54F4(fn_80053164(), 0x7B32, m_owner->m_chara, 0, 1);
            break;
        case 0x34:
            fn_801D54F4(fn_80053164(), 0x7B34, m_owner->m_chara, 0, 1);
            break;
        case 0x3C:
            fn_801D54F4(fn_80053164(), 0x7B3C, m_owner->m_chara, 0, 1);
            break;
        case 0x3D:
            fn_801D54F4(fn_80053164(), 0x7B3D, m_owner->m_chara, 0, 1);
            break;
        case 0x3E:
            fn_801D54F4(fn_80053164(), 0x7B3E, m_owner->m_chara, 0, 1);
            break;
        case 0x3F:
            fn_801D54F4(fn_80053164(), 0x7B3F, m_owner->m_chara, 0, 1);
            break;
        }
        break;
    case 0x5000:
        m_owner->m_attribute = value;
        break;
    }
}

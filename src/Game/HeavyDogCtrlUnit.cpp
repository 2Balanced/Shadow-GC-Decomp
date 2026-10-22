/* The control units both stages of the Heavy Dog boss share (class names from RTTI:
 * HeavyDogCtrlUnit_*): the status update, the turning, the energy gauge and canopy models, the
 * engines, the rocket launchers on the boss's back, the barrier, the collider callbacks of the
 * canopy and the body, the motion attributes and the smoke of the destroyed boss. It also defines
 * the units' base, HeavyDogCtrlUnit. File name inferred from the classes; names marked "(name
 * inferred)" and all member names are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define BOSSVARPARAM_CHARA_SHARED_PTR
#define HEAVYDOGVARPARAM_FULL
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/PooledList.h"
#include "Game/BossCtrlUnit.h"
#include "Game/BossDef.h"
#include "Game/BossRootTask.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/HeavyDogCtrlUnit.h"
#include "Game/HeavyDogCtrlUnits.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/WeaponBullet.h"
#include "Game/WeaponLockonTarget.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in BlackBullCtrlUnit.cpp). */
namespace std {
float fabs(float x);
} // namespace std

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* What a motion passes to its callbacks (as in BlackBullCtrlUnit.cpp). */
struct _stMtnCallbackParam {
    /* 0x0 */ void *m_frame;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ u32 m_id;
};

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h and rw/rpworld.h,
 * reduced to the members used; as in HeavyDog0410CtrlUnit.cpp). */
extern "C" {
typedef struct RwRGBAReal {
    float red;
    float green;
    float blue;
    float alpha;
} RwRGBAReal;

typedef struct RwObject {
    u8 type;
    u8 subKind;
    u8 flags;
    u8 privateFlags;
    void *parent;
} RwObject;

typedef struct RwV3d {
    float x;
    float y;
    float z;
} RwV3d;

typedef struct RwMatrix {
    RwV3d right;
    u32 flags;
    RwV3d up;
    u32 pad1;
    RwV3d at;
    u32 pad2;
    RwV3d pos;
    u32 pad3;
} RwMatrix;

typedef struct RwFrame {
    /* 0x00 */ RwObject object;
    /* 0x08 */ u8 inDirtyListLink[0x8];
    /* 0x10 */ RwMatrix modelling;
} RwFrame;

typedef struct RpClump {
    /* 0x00 */ RwObject object; /* the parent is the clump's frame */
} RpClump;
}

/* Target (class name from RTTI): a point characters aim at (as in HeavyDog0410CtrlUnit.cpp); the
 * word at 0x30 holds its flags (bit 0: it can be aimed at; meaning inferred). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* Whether the target can be aimed at, and switching that (names inferred). */
    bool IsEnabled() {
        return m_flags & 1;
    }
    void Enable() {
        m_flags |= 1;
    }
    void Disable() {
        m_flags &= ~1;
    }

    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ u32 m_flags;
};

/* The parts of AttackCallbackParam the units read (as in BlackBullCtrlUnit.cpp). */
struct HeavyDogAttackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ u8 field_0x24[0x34 - 0x24];
    /* 0x34 */ Vec3f m_offset;
    /* 0x40 */ u8 field_0x40[0x50 - 0x40];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

#define Param(param) (*(HeavyDogAttackParam *)&(param))

/* The effect manager fn_800A31F0 returns (as in BlackBullCtrlUnit.cpp). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> LaunchId(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* The game state (as in BlackBullCtrlUnit.cpp): the frame time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

/* The save data (see BlackBullCtrlUnit.cpp): the record at 0x70C keeps the score. */
struct UnkObject_80079808 {
    /* 0x000 */ u8 field_0x0[0x70C];
    /* 0x70C */ u8 m_score[1];
};

/* The world of each stage area (layout as in HeavyDog0410CtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

class UnkCharaList_80074194;

/* A rotation as a quaternion (struct name unknown). */
struct Quat4f {
    Quat4f() {}

    float x;
    float y;
    float z;
    float w;
};

inline const Quat4f &RefQ(const Quat4f &rotation) {
    return rotation;
}

/* The player's character id, and the null owner of colliders. */
extern s32 lbl_805EE588;
extern boost::shared_ptr<CharInfo> lbl_805EE698;

extern "C" {
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation, int id);
/* Launches effect id at position, turned by rotation (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_801D2BEC(int arg0, const Vec3f *position, const Vec3f *rotation,
                                           int id, u32 time);

void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);
void fn_801D51A0(void *sound, s32 voice, int arg2, int arg3);

UnkObject_80079808 *fn_80079808(void);
void fn_8016F0CC(void *score, int arg1, int points);

UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::shared_ptr<CharaColliBase> fn_800C7F7C(void *manager, const boost::weak_ptr<CharInfo> &owner,
                                              const UnkBossColliShape *shapes, _stMtnPlayWork *motion,
                                              int arg4, int arg5);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *target);


/* Vibration (see Vibration.h). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DD9C(int type, const Vec3f *position, int arg2);
void fn_8034DB60(boost::weak_ptr<Vibration::RequestHandle> *handle);

/* RenderWare clumps and frames (see HeavyDog0410CtrlUnit.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
RwFrame *fn_8048722C(RwFrame *frame);
void fn_800389F8(RpClump *clump, s16 arg1);
void fn_80039114(RpClump *clump, const RwRGBAReal *color);
void fn_80016390(RwMatrix *out, const Mtx34f *matrix);
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int arg2);

/* Motions (see BlackBullCtrlUnit.cpp). */
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
void fn_80016370(Mtx34f *matrix, void *frame);
void fn_800163F4(void *frame, const Mtx34f *matrix);
void fn_80415808(_stMtnPlayWork *work, int id, void (*callback)(void *, void *), void *data);
void fn_80335844(void *, void *);

/* Shots: a rocket (see ArkCannon.cpp), fired with fn_80051C34; fn_8005B84C shows the muzzle
 * flash (behaviour inferred); fn_80221BA8 throws a falling part (see Game/Debris). */
void fn_80051D10(void *shot, const boost::weak_ptr<CharInfo> &owner,
                 const boost::shared_ptr<Weapon::LockonTarget> &lockon, int arg3,
                 const Mtx34f *muzzle, const Vec3f *direction);
void fn_80051C34(int type, const Weapon::ShotInfo *info);
void fn_8005B84C(int type, const Mtx34f *matrix);
void fn_80221BA8(const Vec3f *position, const Vec3f *rotation, const Vec3f *velocity, int arg3,
                 int arg4);

float fn_80016260(float x, float z); /* the angle of (x, z) */
Vec3f fn_8040FB08(const Mtx34f *matrix);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Quat4f fn_8040C55C(const Mtx34f *matrix);
Vec3f fn_80410204(const Quat4f &rotation, int arg1);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
}

/* Room for the rocket's Weapon::ShotInfo, which fn_80051D10 constructs (a constructor the shot's
 * header does not declare). */
struct UnkShot_80051D10 {
    Weapon::ShotInfo *Get() {
        return reinterpret_cast<Weapon::ShotInfo *>(this);
    }

    u32 m_data[sizeof(Weapon::ShotInfo) / 4];
};

/* fn_8040CD74 called as returning a - b by value (see BlackBullCtrlUnit.cpp). */
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

/* Puts clump in world (name inferred; as in HeavyDog0410CtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* Moves to the next launcher still mounted (name inferred; written out where used). */
#define NextLauncher()                                                                                 do {                                                                                                   if (++m_owner->m_launcherIndex == m_owner->m_launcherCount) {                                          m_owner->m_launcherIndex = 0;                                                                  }                                                                                              } while (!m_owner->m_launchers[m_owner->m_launcherIndex].m_alive ||                                         m_owner->m_launchers[m_owner->m_launcherIndex].m_destroyed)

inline HeavyDogCtrlUnit::HeavyDogCtrlUnit(HeavyDogVarParam *owner) : BossCtrlUnit(m_owner), m_owner(owner) {}

inline HeavyDogCtrlUnit::~HeavyDogCtrlUnit() {}

HeavyDogCtrlUnit_GetStatus::HeavyDogCtrlUnit_GetStatus(HeavyDogVarParam *param)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_kind = 0;
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara) {
        fn_800091D0(&m_owner->m_prevAim, &chara->m_position);
    }
}

HeavyDogCtrlUnit_GetStatus::~HeavyDogCtrlUnit_GetStatus() {}

int HeavyDogCtrlUnit_GetStatus::Tick(float dt) {
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara) {
        fn_800091D0(&m_owner->m_playerPosition, &chara->m_position);
        fn_800091D0(&m_owner->m_aim, &chara->m_position);
    }
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(m_owner->m_aim, m_owner->m_prevAim)));
    float length = fn_8040CF50(&offset);
    float speed = m_owner->m_targetSpeed;
    if (length > speed * dt) {
        fn_800091D0(&offset, &Ref(fn_8040CDA8(&Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&offset)), speed)),
                                              dt)));
        fn_800091D0(&m_owner->m_aim, &Ref(fn_8040CDD0(&m_owner->m_prevAim, &offset)));
    }
    fn_800091D0(&m_owner->m_prevAim, &m_owner->m_aim);
    fn_800091D0(&offset, &Ref(Sub(m_owner->m_aim, m_owner->m_charaPtr.get()->m_position)));
    m_owner->m_aimHeight = std::fabs(offset.y);
    offset.y = 0.0f;
    m_owner->m_aimDistance = fn_8040CF50(&offset);
    m_owner->m_aimDirection = fn_80016260(-offset.x, -offset.z);
    if (fn_8040CF50(&m_owner->m_aim) < 10.0f) {
        m_owner->m_aimAngle = chara.get()->m_rotation.y;
    } else {
        m_owner->m_aimAngle = fn_80016260(-m_owner->m_aim.x, -m_owner->m_aim.z);
    }
    m_owner->m_angle = fn_80016260(-m_owner->m_charaPtr.get()->m_position.x,
                                   -m_owner->m_charaPtr.get()->m_position.z);
    if (m_owner->m_targetPoint) {
        m_owner->m_targetPoint.get()->fn_800CCE38(&m_owner->m_aim);
    } else if (chara) {
        boost::shared_ptr<Player::PlayerBase> base =
            boost::dynamic_pointer_cast<Player::PlayerBase>(chara);
        if (base && base.get()->m_target) {
            m_owner->m_lockon.get()->SetTarget(fn_80206574(base.get()->m_target));
        }
    }
    return false;
}

HeavyDogCtrlUnit_AttackCallback::HeavyDogCtrlUnit_AttackCallback(HeavyDogVarParam *param, int index)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_kind = 0;
    PooledList<CharaColliBase>::iterator it;
    it = PooledList<CharaColliBase>::iterator(m_owner->m_colliders->Head());
    for (int i = 0; it != m_owner->m_colliders->End(); it++, i++) {
        if (i == index) {
            boost::weak_ptr<CharaColliAttack> attack =
                fn_800C42D0(fn_80021B08(), *it);
            boost::shared_ptr<CharaColliAttack> collision(attack.lock());
            collision->m_hitCallback = GetHitCallback();
            collision->m_attackCallback = GetDamageCallback();
        }
    }
}

void HeavyDogCtrlUnit_AttackCallbackBody::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = Param(param).m_attacker.lock()->m_owner.lock();
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, attacker->m_position)));
    Param(param).m_damage = Param(param).m_attacker.lock()->m_power;
}

void HeavyDogCtrlUnit_AttackCallbackBody::OnDamage(AttackCallbackParam &param) {
    if (m_owner->m_invincible) {
        Param(param).m_result = 5;
    } else {
        Param(param).m_result = 1;
    }
}

void HeavyDogCtrlUnit_AttackCallbackCanopy::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = Param(param).m_attacker.lock()->m_owner.lock();
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, attacker->m_position)));
    Param(param).m_damage = Param(param).m_attacker.lock()->m_power;
}

void HeavyDogCtrlUnit_AttackCallbackCanopy::OnDamage(AttackCallbackParam &param) {
    if (m_owner->m_invincible) {
        Param(param).m_result = 5;
        return;
    }
    Param(param).m_result = 1;
    m_owner->m_damaged = 1;
    fn_803358D0(m_owner, new BossCtrlUnit_DamageEff(m_owner));
    fn_8016F0CC(fn_80079808()->m_score, 1, m_owner->m_canopyScore * Param(param).m_damage);
    float before = m_owner->m_health;
    m_owner->m_health = before - Param(param).m_damage;
    if (m_owner->m_health < 0.0f) {
        m_owner->m_health = 0.0f;
    }
    float after = m_owner->m_health;
    if (m_owner->m_launchersAlive > 1) {
        m_owner->m_launcherDamage -= Param(param).m_damage;
        if (m_owner->m_launcherDamage < 0.0f) {
            if (m_owner->m_launcherIndex == m_owner->m_launcherCount - 1) {
                int i = m_owner->m_launcherIndex;
                do {
                    i++;
                    if (i == m_owner->m_launcherCount) {
                        i = 0;
                    }
                } while (!m_owner->m_launchers[i].m_alive || m_owner->m_launchers[i].m_destroyed);
                m_owner->m_launchers[i].m_destroyed = 1;
            } else {
                m_owner->m_launchers[m_owner->m_launcherIndex].m_destroyed = 1;
                NextLauncher();
            }
            m_owner->m_launcherDamage += m_owner->m_launcherHealth;
            m_owner->m_launchersAlive--;
        }
    }
    if (before / m_owner->m_maxHealth > 0.5f && after / m_owner->m_maxHealth <= 0.5f) {
        m_owner->m_nextMode = 7;
        return;
    }
    if (m_owner->m_health <= 0.0f) {
        m_owner->m_nextMode = 8;
    }
}

HeavyDogCtrlUnit_Rotate::HeavyDogCtrlUnit_Rotate(HeavyDogVarParam *param)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_kind = 2;
}

HeavyDogCtrlUnit_Rotate::~HeavyDogCtrlUnit_Rotate() {}

int HeavyDogCtrlUnit_Rotate::Tick(float dt) {
    HeavyDogVarParam *owner = m_owner;
    float angle = owner->m_aimDirection - owner->m_launchers[owner->m_launcherIndex].m_angle;
    if (!owner->m_fixed && owner->m_turnToVelocity && fn_8040CF50(&owner->m_velocity) > 0.0001 &&
        m_owner->m_targetDistance > m_owner->m_turnDistance) {
        angle = fn_80016260(-m_owner->m_velocity.x, -m_owner->m_velocity.z);
    } else {
        while (std::fabs(angle) > lbl_805FAD38) {
            angle += angle > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
        }
    }
    if (!m_owner->m_turnFixed) {
        float diff = angle - m_owner->m_charaPtr.get()->m_rotation.y;
        while (std::fabs(diff) > lbl_805FAD38) {
            diff += diff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
        }
        m_owner->m_turnRate = ToRadian(m_owner->m_turnSpeed);
        if (std::fabs(diff) < m_owner->m_turnRate * dt) {
            m_owner->m_charaPtr.get()->m_rotation.y += diff;
            m_owner->m_turnRate = 0.0f;
        } else {
            if (diff < 0.0f) {
                m_owner->m_turnRate = -m_owner->m_turnRate;
            }
            m_owner->m_charaPtr.get()->m_rotation.y += m_owner->m_turnRate * dt;
        }
    } else {
        m_owner->m_turnRate = 0.0f;
    }
    m_owner->m_angleDiff = angle - m_owner->m_charaPtr.get()->m_rotation.y;
    while (std::fabs(m_owner->m_angleDiff) > lbl_805FAD38) {
        m_owner->m_angleDiff +=
            m_owner->m_angleDiff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
    }
    return false;
}

/* How many rocket launchers the boss can carry. */
namespace HeavyDog {
extern const s32 kMaxLaunchers;
} // namespace HeavyDog

/* NonMatching (99.9%): retail loads m_owner into r5 and &shapes into r6 for fn_800C7F7C, GC/2.7
 * swaps the two registers. */
HeavyDogCtrlUnit_RocketLauncher::HeavyDogCtrlUnit_RocketLauncher(HeavyDogVarParam *param,
                                                                 int motion)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_kind = 2;
    if (m_owner->m_launcherCount >= HeavyDog::kMaxLaunchers) {
        m_index = -1;
        return;
    }
    m_index = m_owner->m_launcherCount++;
    m_owner->m_launchersAlive++;
    m_motion = motion;
    m_turn = 0.0f;
    m_loaded = 0;
    m_aimed = 0;
    m_aiming = 0;
    m_owner->m_launchers[m_index].m_alive = 1;
    m_owner->m_launchers[m_index].m_destroyed = 0;
    m_owner->m_launchers[m_index].m_fired = 0;
    m_owner->m_launchers[m_index].m_angle = 0.0f;
    if (m_owner->m_charaPtr.get() != 0 && m_owner->m_motion != 0) {
        if (m_owner->m_launcherCount != HeavyDog::kMaxLaunchers) {
            void *manager = fn_80021B08();
            UnkSphere_80035CEC sphere(Vec3f(0.0f, 0.0f, 0.0f), 10.0f);
            UnkBossColliShape shapes[2] = {
                {0, 0, 0x100000, 0},
                {0},
            };
            shapes[0].m_sphere = &sphere;
            shapes[0].field_0xC = m_motion;
            m_collider = fn_800C7F7C(manager, boost::weak_ptr<CharInfo>(m_owner->m_charaPtr),
                                     &shapes[0], m_owner->m_motion, 4, 0);
            if (m_collider) {
                m_collider->SetField2C(1);
                boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
                boost::shared_ptr<CharaColliAttack> collision(attack.lock());
                if (collision) {
                    collision->field_0x54 = 0;
                    *reinterpret_cast<u32 *>(collision->field_0x3C) = 1;
                    collision->field_0x55 = 1;
                    collision->m_attackCallback = GetDamageCallback();
                }
            } else {
                m_index = -1;
            }
            m_target = fn_801E7128(boost::weak_ptr<CharInfo>(m_owner->m_charaPtr), 0x2C00);
            if (!m_target) {
                m_index = -1;
            }
        }
        m_callback = GetCallback();
        fn_80415808(m_owner->m_motion, m_motion, fn_80335844, &m_callback);
    }
    m_reload = 0.0f;
}

HeavyDogCtrlUnit_RocketLauncher::~HeavyDogCtrlUnit_RocketLauncher() {
    m_collider.reset();
    m_target.reset();
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int HeavyDogCtrlUnit_RocketLauncher::Tick(float dt) {
    if (m_index < 0) {
        return true;
    }
    if (!m_owner->m_launchers[m_index].m_alive) {
        if (m_collider) {
            m_collider.reset();
        }
        if (m_target) {
            m_target.reset();
        }
        return false;
    }
    m_dt = dt;
    if (m_owner->m_launchers[m_index].m_destroyed) {
        m_owner->m_launchers[m_index].m_alive = 0;
        Mtx34f matrix;
        fn_80016370(&matrix, fn_80415E70(m_owner->m_motion, m_motion));
        Vec3f position;
        fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
        Vec3f rotation = fn_80410204(fn_8040C55C(&matrix), 0);
        fn_80016370(&matrix, fn_80415E70(m_owner->m_motion, 0xFAA));
        Vec3f velocity;
        fn_800091EC(&velocity, &Ref(Sub(position, fn_8040FB08(&matrix))));
        fn_800091D0(&velocity, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&velocity)), 20.0f)));
        fn_80221BA8(&position, &rotation, &velocity, 0x13, m_owner->m_rocketEffect);
        Vec3f effectRotation;
        fn_800091EC(&effectRotation, &lbl_805E07A0);
        fn_801A1F54(&position, &effectRotation, 0x72);
        m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0xB);
        boost::shared_ptr<EffectElement> effect(m_effect.lock());
        if (effect) {
            fn_80425E4C(effect.get(), &position);
            effect->SetField14(FrameTime());
        }
        return false;
    }
    m_reload += dt;
    if (m_reload > m_owner->m_reloadTime) {
        m_reload = m_owner->m_reloadTime;
        m_loaded = 1;
    }
    if (m_aimed && m_loaded) {
        Mtx34f *node = fn_80415E70(m_owner->m_motion, m_motion + 1);
        Mtx34f muzzle;
        fn_80016370(&muzzle, node);
        fn_8040B884(&muzzle, lbl_805FAD38);
        Vec3f direction(0.0f, 0.0f, 0.0f);
        fn_800091D0(&direction, &Ref(fn_8040B67C(&muzzle, &direction)));
        UnkShot_80051D10 shot;
        fn_80051D10(&shot, boost::weak_ptr<CharInfo>(m_owner->m_charaPtr), m_owner->m_lockon, 1,
                    &muzzle, &direction);
        fn_80051C34(0x13, shot.Get());
        fn_8040B884(&muzzle, lbl_805FAD38);
        fn_8005B84C(0x13, &muzzle);
        m_loaded = 0;
        m_aimed = 0;
        m_aiming = 0;
        m_reload = 0.0f;
        NextLauncher();
        shot.Get()->~ShotInfo();
    } else if (m_owner->m_launchers[m_index].m_fired && !m_aiming &&
               std::fabs(m_owner->m_turnRate) < 0.0001) {
        m_aiming = 1;
    }
    m_owner->m_launchers[m_index].m_fired = 0;
    if (m_target) {
        if (m_owner->m_stopped && m_target->IsEnabled()) {
            m_target->Disable();
        } else if (!m_owner->m_stopped && !m_target->IsEnabled()) {
            m_target->Enable();
        }
    }
    return false;
}

void HeavyDogCtrlUnit_RocketLauncher::OnDamage(AttackCallbackParam &param) {
    if (m_owner->m_invincible) {
        Param(param).m_result = 5;
        return;
    }
    Param(param).m_result = 1;
    m_owner->m_damaged = 1;
    fn_8016F0CC(fn_80079808()->m_score, 1, m_owner->m_launcherScore * Param(param).m_damage);
    fn_803358D0(m_owner, new BossCtrlUnit_DamageEff(m_owner));
    float before = m_owner->m_health;
    m_owner->m_health = before - Param(param).m_damage;
    if (m_owner->m_health < 0.0f) {
        m_owner->m_health = 0.0f;
    }
    float after = m_owner->m_health;
    m_owner->m_launcherDamage -= Param(param).m_damage;
    if (m_owner->m_launcherDamage < 0.0f) {
        m_owner->m_launchers[m_index].m_destroyed = 1;
        if (m_owner->m_launcherIndex == m_index) {
            NextLauncher();
        }
        m_owner->m_launcherDamage += m_owner->m_launcherHealth;
        m_owner->m_launchersAlive--;
    }
    if (before / m_owner->m_maxHealth > 0.5f && after / m_owner->m_maxHealth <= 0.5f) {
        m_owner->m_nextMode = 7;
    }
}

void HeavyDogCtrlUnit_RocketLauncher::OnMotion(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    Vec3f position;
    fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
    if (!m_owner->m_launchers[m_index].m_alive) {
        boost::shared_ptr<EffectElement> effect(m_effect.lock());
        if (effect) {
            fn_80425E4C(effect.get(), &position);
        }
        fn_8040B720(&matrix, 0.0f, 0.0f, 0.0f);
        fn_800163F4(param.m_frame, &matrix);
        return;
    }
    if (m_index != m_owner->m_launcherCount - 1) {
        if (m_index == m_owner->m_launcherIndex && m_aiming) {
            Vec3f offset;
            fn_800091EC(&offset, &Ref(Sub(m_owner->m_aim, position)));
            float height = offset.y;
            offset.y = 0.0f;
            float angle = fn_80016260(fn_8040CF50(&offset), height);
            float target = angle + ToRadian(m_owner->m_launcherAim);
            float limit = ToRadian(45.0f);
            if (target > limit) {
                target = limit;
            }
            m_turn += m_dt * ToRadian(m_owner->m_launcherTurn);
            if (m_turn > target) {
                m_turn = target;
                m_aimed = 1;
            }
        } else {
            m_turn -= m_dt * ToRadian(m_owner->m_launcherTurn);
            if (m_turn < 0.0f) {
                m_turn = 0.0f;
            }
        }
        fn_8040B884(&matrix, m_turn);
        fn_800163F4(param.m_frame, &matrix);
    } else if (m_aiming) {
        m_aimed = 1;
    }
    if (m_target) {
        m_target.get()->fn_800CCE38(&position);
    }
    Mtx34f *body = fn_80415E70(m_owner->m_motion, 0xFAA);
    if (body != 0) {
        fn_80016370(&matrix, body);
        fn_800091D0(&position, &Ref(fn_8040B67C(&RefM(fn_8040BEF8(&matrix)), &position)));
        m_owner->m_launchers[m_index].m_angle = fn_80016260(-position.x, -position.z);
        while (std::fabs(m_owner->m_launchers[m_index].m_angle) > lbl_805FAD38) {
            m_owner->m_launchers[m_index].m_angle +=
                m_owner->m_launchers[m_index].m_angle > 0.0f ? -2.0f * lbl_805FAD38
                                                             : 2.0f * lbl_805FAD38;
        }
    }
}

HeavyDogCtrlUnit_Engine::HeavyDogCtrlUnit_Engine(HeavyDogVarParam *param, int motion)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_kind = 2;
    m_motion = motion;
    m_reverse = 0;
    m_tilt = 0.0f;
    m_tiltSpeed = 0.0f;
    if (m_owner->m_motion != 0) {
        m_callback0 = boost::bind(&HeavyDogCtrlUnit_Engine::OnBody, this, _1);
        fn_80415808(m_owner->m_motion, m_motion, fn_80335844, &m_callback0);
        m_callback1 = boost::bind(&HeavyDogCtrlUnit_Engine::OnEngine, this, _1);
        fn_80415808(m_owner->m_motion, m_motion + 1, fn_80335844, &m_callback1);
        m_callback2 = boost::bind(&HeavyDogCtrlUnit_Engine::OnExhaust, this, _1);
        fn_80415808(m_owner->m_motion, m_motion + 10, fn_80335844, &m_callback2);
        fn_80415808(m_owner->m_motion, m_motion + 11, fn_80335844, &m_callback2);
        fn_80415808(m_owner->m_motion, m_motion + 12, fn_80335844, &m_callback2);
        m_callback3 = boost::bind(&HeavyDogCtrlUnit_Engine::OnNozzle, this, _1);
        fn_80415808(m_owner->m_motion, m_motion + 20, fn_80335844, &m_callback3);
        fn_80415808(m_owner->m_motion, m_motion + 21, fn_80335844, &m_callback3);
    }
}

HeavyDogCtrlUnit_Engine::~HeavyDogCtrlUnit_Engine() {
    boost::shared_ptr<EffectElement> effect;
    for (int i = 0; i < 5; i++) {
        effect = m_effects[i].lock();
        if (effect) {
            effect->Stop();
            fn_800A4210(effect);
        }
    }
}

int HeavyDogCtrlUnit_Engine::Tick(float dt) {
    m_dt = dt;
    return false;
}

/* Keeps the body node where the motion puts it, but unturned (name inferred). */
void HeavyDogCtrlUnit_Engine::OnBody(_stMtnCallbackParam &param) {
    if (m_owner->m_active) {
        Mtx34f matrix;
        fn_80016370(&matrix, param.m_frame);
        fn_800091D0(&m_position, &Ref(fn_8040FB08(&matrix)));
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &m_position);
        fn_800163F4(param.m_frame, &matrix);
    }
}

/* Tilts the engines with the boss's acceleration (name inferred). */
void HeavyDogCtrlUnit_Engine::OnEngine(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    Vec3f position;
    fn_800091EC(&position, &Ref(fn_8040FB08(&matrix)));
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(position, m_position)));
    Vec3f acceleration;
    fn_800091EC(&acceleration, &m_owner->m_acceleration);
    float target = 0.0f;
    if (acceleration.y > m_owner->m_engineTilt) {
        target += ToRadian(20.0f);
    }
    if (acceleration.y < -m_owner->m_engineTilt) {
        target -= ToRadian(20.0f);
    }
    acceleration.y = 0.0f;
    if (fn_8040CF50(&acceleration) < m_owner->m_engineTilt) {
        m_reverse = 0;
        target += ToRadian(40.0f);
    } else if (fn_8040CF90(&acceleration, &offset) < 0.0f) {
        m_reverse = 1;
        target -= ToRadian(40.0f);
    } else {
        m_reverse = 0;
        target += ToRadian(40.0f);
    }
    m_tiltSpeed += target * m_dt;
    m_tilt += m_tiltSpeed * m_dt;
    float low = ToRadian(-20.0f);
    if (m_tilt < low) {
        m_tilt = low;
        m_tiltSpeed = 0.0f;
    }
    if (m_tilt > ToRadian(20.0f)) {
        m_tilt = ToRadian(20.0f);
        m_tiltSpeed = 0.0f;
    }
    fn_8040B884(&matrix, m_tilt);
    fn_800163F4(param.m_frame, &matrix);
}

/* Places the exhaust effect of the node (name inferred). */
void HeavyDogCtrlUnit_Engine::OnExhaust(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    boost::weak_ptr<EffectElement> &slot = m_effects[param.m_id % 10];
    boost::shared_ptr<EffectElement> effect(slot.lock());
    if (effect) {
        fn_80425E68(effect.get(), &matrix);
    } else {
        slot = fn_800A31F0()->Launch(m_owner->m_effects, 0xE);
        boost::shared_ptr<EffectElement> launched(slot.lock());
        if (launched) {
            fn_80425E68(launched.get(), &matrix);
            launched->SetField14(FrameTime());
        }
    }
}

/* Places the reverse thrust effect of the node while the boss backs off (name inferred). */
void HeavyDogCtrlUnit_Engine::OnNozzle(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    boost::weak_ptr<EffectElement> &slot = m_effects[param.m_id % 10 + 3];
    boost::shared_ptr<EffectElement> effect(slot.lock());
    if (effect) {
        if (m_reverse) {
            fn_80425E68(effect.get(), &matrix);
        } else {
            effect->Stop();
            fn_800A4210(effect);
        }
    } else if (m_reverse) {
        slot = fn_800A31F0()->Launch(m_owner->m_effects, 0xE);
        boost::shared_ptr<EffectElement> launched(slot.lock());
        if (launched) {
            fn_80425E68(launched.get(), &matrix);
            launched->SetField14(FrameTime());
        }
    }
}

HeavyDogCtrlUnit_Barrier::HeavyDogCtrlUnit_Barrier(HeavyDogVarParam *param, const Vec3f *position,
                                                   bool effect)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_effectShown = effect;
    m_time = 0.0f;
    m_state = 0;
    m_radius = 0.0f;
    fn_800091D0(&m_position, position);
    if (effect) {
        m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x12);
    } else {
        m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x12);
    }
    boost::shared_ptr<EffectElement> shown(m_effect.lock());
    if (shown) {
        fn_80425E4C(shown.get(), position);
        shown->SetField14(FrameTime());
    }
    m_vibration = fn_8034DD9C(8, &m_position, -2);
}

HeavyDogCtrlUnit_Barrier::~HeavyDogCtrlUnit_Barrier() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
    if (m_collider) {
        m_collider.reset();
    }
    fn_8034DB60(&m_vibration);
}

int HeavyDogCtrlUnit_Barrier::Tick(float dt) {
    m_time += dt;
    m_radius = m_owner->m_barrierRadius;
    fn_801D53C4(fn_80053164(), 0x7D36, &m_position, this, 3, 0, 3);
    switch (m_state) {
    case 0:
        if (m_time < 0.5f) {
            m_radius *= m_time / 0.5f;
        } else {
            m_state = 1;
        }
        break;
    case 1:
        if (m_time > m_owner->m_barrierTime) {
            return true;
        }
        break;
    }
    if (m_collider) {
        m_collider.reset();
    }
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(m_owner->m_playerPosition, m_position)));
    float height = std::fabs(offset.y);
    offset.y = 0.0f;
    if (fn_8040CF50(&offset) < m_radius && height < 0.5f * m_owner->m_barrierHeight) {
        void *manager = fn_80021B08();
        m_collider = fn_800C7EA8(manager, &m_owner->m_playerPosition, 0, 4, 0, lbl_805EE698,
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

void HeavyDogCtrlUnit_Barrier::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, m_position)));
    Param(param).m_damage = attack.lock()->m_power;
}

HeavyDogCtrlUnit_CheckLauncherNum::HeavyDogCtrlUnit_CheckLauncherNum(HeavyDogVarParam *param,
                                                                     int sound)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_kind = 2;
    m_sound = sound;
    m_count = m_owner->m_launchersAlive;
}

HeavyDogCtrlUnit_CheckLauncherNum::~HeavyDogCtrlUnit_CheckLauncherNum() {}

int HeavyDogCtrlUnit_CheckLauncherNum::Tick(float dt) {
    if (m_count > m_owner->m_launchersAlive) {
        fn_801D51A0(fn_80053164(), m_sound, 0, -1);
    }
    m_count = m_owner->m_launchersAlive;
    return false;
}

/* Keeps motion attribute 0x5000's value (meaning inferred). */
void HeavyDogCtrlUnit_CheckMotionAttribute::OnAttribute(u16 attribute, int value) {
    switch (attribute) {
    case 0x5001:
        break;
    case 0x5000:
        m_owner->m_attribute = value;
        break;
    }
}

HeavyDogCtrlUnit_DispCanopy::HeavyDogCtrlUnit_DispCanopy(HeavyDogVarParam *param, int model)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_kind = 3;
    void *data = fn_8001277C(fn_800122D4(), 0x16, model);
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        m_frame = (RwFrame *)m_clump->object.parent;
        fn_800389F8(m_clump, 8);
    }
}

HeavyDogCtrlUnit_DispCanopy::~HeavyDogCtrlUnit_DispCanopy() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
        m_frame = 0;
    }
}

int HeavyDogCtrlUnit_DispCanopy::Tick(float dt) {
    if (m_owner->m_motion == 0 || m_frame == 0 || m_clump == 0) {
        return false;
    }
    Mtx34f *node = fn_80415E70(m_owner->m_motion, 0xFAA);
    RwMatrix *matrix = &m_frame->modelling;
    fn_80016390(matrix, node);
    matrix->pos.x += m_owner->field_0x48.x;
    matrix->pos.y += m_owner->field_0x48.y;
    matrix->pos.z += m_owner->field_0x48.z;
    fn_8048722C(m_frame);
    return false;
}

HeavyDogCtrlUnit_DispEnergy::HeavyDogCtrlUnit_DispEnergy(HeavyDogVarParam *param, int model)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_kind = 3;
    void *data = fn_8001277C(fn_800122D4(), 0x16, model);
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        m_frame = (RwFrame *)m_clump->object.parent;
        fn_800389F8(m_clump, 8);
    }
    m_state = 0;
}

HeavyDogCtrlUnit_DispEnergy::~HeavyDogCtrlUnit_DispEnergy() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
        m_frame = 0;
    }
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int HeavyDogCtrlUnit_DispEnergy::Tick(float dt) {
    if (m_owner->m_motion == 0 || m_frame == 0 || m_clump == 0) {
        return false;
    }
    int state = 0;
    if (m_owner->m_energy < 0.0f) {
        state = 0;
    } else if (m_owner->m_energy > m_owner->m_energyFull) {
        state = 4;
    } else {
        m_owner->m_energy += dt;
        if (m_owner->m_energy > m_owner->m_energyFull) {
            m_owner->m_energy = m_owner->m_energyFull;
            state = 3;
        }
        RwRGBAReal color = {1.0f, 1.0f, 0.0f, 1.0f};
        if (m_owner->m_energy > 0.5f * m_owner->m_energyFull) {
            color.red = 2.0f * (m_owner->m_energyFull - m_owner->m_energy) / m_owner->m_energyFull;
            if (state == 0) {
                state = 2;
            }
        }
        if (m_owner->m_energy < 0.5f * m_owner->m_energyFull) {
            color.green = 2.0f * m_owner->m_energy / m_owner->m_energyFull;
            if (state == 0) {
                state = 1;
            }
        }
        fn_80039114(m_clump, &color);
    }
    Mtx34f *node = fn_80415E70(m_owner->m_motion, 0xFAA);
    RwMatrix *matrix = &m_frame->modelling;
    fn_80016390(matrix, node);
    Vec3f position(matrix->pos.x, matrix->pos.y, matrix->pos.z);
    matrix->pos.x += m_owner->field_0x48.x;
    matrix->pos.y += m_owner->field_0x48.y;
    matrix->pos.z += m_owner->field_0x48.z;
    fn_8048722C(m_frame);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (m_state != state) {
        if (effect) {
            effect->Stop();
        }
        switch (state) {
        case 1:
            m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 1);
            break;
        case 2:
            m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0);
            break;
        case 3:
            m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 2);
            break;
        case 4:
            m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x18);
            break;
        }
        effect = m_effect.lock();
        if (effect) {
            fn_80425E4C(effect.get(), &position);
            effect->SetField14(FrameTime());
        }
        m_state = state;
    } else if (effect) {
        fn_80425E4C(effect.get(), &position);
    }
    return false;
}

HeavyDogCtrlUnit_DestroyEffect::HeavyDogCtrlUnit_DestroyEffect(HeavyDogVarParam *param)
    : BossCtrlUnit(param), HeavyDogCtrlUnit(param) {
    m_kind = 3;
    fn_800091D0(&m_position, &lbl_805E07A0);
    fn_800091D0(&m_rotation, &lbl_805E07A0);
    m_effect = fn_801D2BEC(0, &m_position, &m_rotation, 0x228, FrameTime());
}

HeavyDogCtrlUnit_DestroyEffect::~HeavyDogCtrlUnit_DestroyEffect() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
        fn_800A4210(effect);
    }
}

int HeavyDogCtrlUnit_DestroyEffect::Tick(float dt) {
    Mtx34f *matrix = fn_80415E70(m_owner->m_motion, 0xFAA);
    m_position.x = matrix->values[0][3];
    m_position.y = matrix->values[1][3];
    m_position.z = matrix->values[2][3];
    return false;
}

/* How many rocket launchers the boss carries; read from .sdata2 like a namespace-scope const
 * (see docs/matching-gadgets.md). Retail pools it after the unit's float literals; GC/2.7 puts
 * it first wherever it is defined. */
namespace HeavyDog {
const s32 kMaxLaunchers = 7;
} // namespace HeavyDog

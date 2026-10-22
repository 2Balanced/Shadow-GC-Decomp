/* The control units of the Black Doom boss (class names from RTTI: BlackDoomCtrlUnit and its
 * BlackDoomCtrlUnit_* subclasses): the status update, the body's attack callbacks, the movement
 * around the corners of the arena, the camera, the disk, meteor and boomerang attacks, the motion
 * attributes, and the eye and aura models. File name inferred from the classes; names marked
 * "(name inferred)" and all member names are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). The Black Doom boss task
 * (BlackDoom and BlackDoomVarParam's methods, from 0x80282DBC) is a separate unit.
 *
 * Three source-level workarounds reproduce retail under GC/2.7 (see docs/research-notes.md): the
 * arena corners' static initializer is written out by hand, two OnHit functions turn
 * "opt_common_subs" off, and an unused function pools 1.0f where retail has it. */
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/BlackDoomCtrlUnit.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EffectPJSLauncher.h"
#include "Game/EnemyCharInfo.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/PooledList.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* MSL's std::sin, std::cos and std::fabs for floats, which the game calls out of line (as in
 * DevilDoomCtrlUnit.cpp). */
namespace std {
float sin(float x);
float cos(float x);
float fabs(float x);
} // namespace std

using Effect::System::EffectElement;
using Effect::System::EffectLaunchInfo;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The RenderWare types used here, declared locally (layouts as in rw/rwcore.h, reduced to the
 * members used; as in HeavyDogCtrlUnit.cpp). */
extern "C" {
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

/* An object that keeps a clump in step with a motion (class name unknown; as in
 * DevilDoomCtrlUnit.h). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ u8 field_0x0[0x1C];
};

/* A clump's colour fade (struct name unknown; as in BossRender.cpp). */
struct UnkClumpFade_80027D5C {
    /* 0x00 */ float m_color[4];
    /* 0x10 */ u8 field_0x10[0x24 - 0x10];
    /* 0x24 */ u8 m_mode;
    /* 0x25 */ u8 field_0x25[0x2C - 0x25];

    /* Sets the alpha (name inferred; as in DevilDoomCtrlUnitPsychoObject.cpp). */
    void SetAlpha(float alpha) {
        m_color[3] = alpha;
    }
};
SIZE_ASSERT(UnkClumpFade_80027D5C, 0x2C);

/* Target (class name from RTTI): a point characters aim at (as in HeavyDogCtrlUnit.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x34];
};

/* The parts of AttackCallbackParam the units read (as in BlackBullCtrlUnit.cpp). */
struct BlackDoomAttackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ u8 field_0x24[0x34 - 0x24];
    /* 0x34 */ Vec3f m_offset;
    /* 0x40 */ u8 field_0x40[0x50 - 0x40];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

#define Param(param) (*(BlackDoomAttackParam *)&(param))

/* The effect manager fn_800A31F0 returns (as in DevilDoomCtrlUnit.cpp). */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> LaunchId(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* The game state (as in DevilDoomCtrlUnit.cpp): the frame time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The frame time the effects follow. */
#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

/* The world of each stage area (as in DevilDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

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
/* Effects (see EffectPJS.cpp). */
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_800A4210(const boost::shared_ptr<EffectElement> &effect);
void fn_80425E4C(EffectElement *effect, const Vec3f *position);
boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation, int id);

/* Sounds (see BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4, int arg5,
                 int arg6);

/* The score (see EggLastMech.cpp). */
UnkObject_80079808 *fn_80079808(void);
void fn_8016F0CC(void *score, int arg1, int points);

/* Characters, colliders and targets (see Bomb.cpp and BAGunShip.cpp). */
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
/* Whether the player was hit (behaviour inferred). */
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
boost::shared_ptr<Target> fn_801E72DC(int flags);
EnemyCharInfo *fn_801792D0(EnemyCharInfo *const *body);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg2);

/* The collision test along a line (see DiablonPartsBeam.cpp). */
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void *fn_80020D40(void);
UnkLineHits_80021FE4 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                  void *filter, int arg5);
UnkLineHit_8002735C *fn_8002735C(UnkLineHits_80021FE4 *hits);
const Vec3f *fn_8001E9BC(void *object);
/* The normal of the surface hit (behaviour inferred). */
const Vec3f *fn_8001E9C4(void *object);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);

/* RenderWare clumps and frames, and the models' motions and fades (see
 * DevilDoomCtrlUnit.cpp and BossRender.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
RwFrame *fn_8048722C(RwFrame *frame);
void fn_80016390(RwMatrix *out, const Mtx34f *matrix);
Mtx34f *fn_80415E70(_stMtnPlayWork *work, int node);
void fn_80015980(UnkClumpUpdater_80015A7C *updater, Vec3f *offset);
UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade);
void fn_80038198(RpClump *clump, UnkClumpFade_80027D5C *fade);

/* The resource loader (see EggLastMech.cpp). */
void *fn_800122D4(void);
void *fn_8001277C(void *loader, s32 slot, int arg2);

/* Math (behaviour inferred). */
float fn_8040EE90(void);             /* a random value in [0, 1) */
float fn_8000FEE4(float value);      /* square root */
float fn_8000F3F8(float value);      /* arc sine */
float fn_800DF25C(float value);      /* arc sine */
float fn_80016260(float x, float z); /* the angle of (x, z) */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *vector);
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
Vec3f fn_80016284(const Vec3f *direction);
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

/* The object a line test hit (name inferred). */
inline void *HitObject(UnkLineHits_80021FE4 *hits) {
    return fn_8002735C(hits)->m_object;
}

/* The normal of the surface a line test hit (name inferred). */
inline const Vec3f *HitNormal(void *object) {
    return fn_8001E9C4(object);
}

/* Degrees to radians (name inferred). */
inline float ToRadian(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Puts clump in world (name inferred; as in DevilDoomCtrlUnit.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* A vector filled in member by member (no constructor, so that the corners' constructor below has
 * no inline calls and is inlined into the static initializer). */
struct UnkVector {
    float x;
    float y;
    float z;
};

/* The four corners of the arena the boss moves around (name inferred). Retail builds them in the
 * unit's static initializer, copy-constructing each from a temporary through fn_800091EC (the
 * game's out-of-line Vec3f copy constructor). */
struct BlackDoomCorners {
    BlackDoomCorners() {
        UnkVector corner3;
        UnkVector corner2;
        UnkVector corner1;
        UnkVector corner0;
        corner0.x = 175.0f;
        corner0.y = 0.0f;
        corner0.z = 175.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_corners[0]), reinterpret_cast<Vec3f *>(&corner0));
        corner1.x = -175.0f;
        corner1.y = 0.0f;
        corner1.z = 175.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_corners[1]), reinterpret_cast<Vec3f *>(&corner1));
        corner2.x = -175.0f;
        corner2.y = 0.0f;
        corner2.z = -175.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_corners[2]), reinterpret_cast<Vec3f *>(&corner2));
        corner3.x = 175.0f;
        corner3.y = 0.0f;
        corner3.z = -175.0f;
        fn_800091EC(reinterpret_cast<Vec3f *>(m_corners[3]), reinterpret_cast<Vec3f *>(&corner3));
    }

    float m_corners[4][3];
};

static BlackDoomCorners s_corners;

/* The corner at index. */
#define Corner(index) (*reinterpret_cast<const Vec3f *>(s_corners.m_corners[index]))

BlackDoomCtrlUnit_GetStatus::BlackDoomCtrlUnit_GetStatus(BlackDoomVarParam *param)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param) {
    m_kind = 0;
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara) {
        fn_800091D0(&m_owner->m_prevDestination, &chara->m_position);
    }
}

BlackDoomCtrlUnit_GetStatus::~BlackDoomCtrlUnit_GetStatus() {}

int BlackDoomCtrlUnit_GetStatus::Tick(float dt) {
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> chara = player.lock();
    if (chara) {
        fn_800091D0(&m_owner->m_target, &chara->m_position);
        if (!m_owner->m_targetLocked) {
            fn_800091D0(&m_owner->m_testDestination, &chara->m_position);
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
    float speed = m_owner->m_settings.m_targetSpeed;
    if (length > speed * dt) {
        fn_800091D0(&offset, &Ref(fn_8040CDA8(&Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&offset)), speed)),
                                              dt)));
        fn_800091D0(&m_owner->m_testDestination,
                    &Ref(fn_8040CDD0(&m_owner->m_prevDestination, &offset)));
    }
    fn_800091D0(&m_owner->m_prevDestination, &m_owner->m_testDestination);
    fn_800091D0(&offset, &Ref(Sub(m_owner->m_testDestination, m_owner->m_chara->m_position)));
    offset.y = 0.0f;
    m_owner->m_destinationDistance = fn_8040CF50(&offset);
    m_owner->m_targetAngle = fn_80016260(-offset.x, -offset.z);
    m_owner->m_time += dt;
    m_owner->m_damageTimer += dt;
    if (m_owner->m_damageTimer > m_owner->m_settings.m_damageTime) {
        m_owner->m_recentDamage = 0.0f;
    }
    if (m_owner->m_recentDamage > m_owner->m_settings.m_damageLimit) {
        m_owner->m_damageScale = 0.9f;
    } else {
        m_owner->m_damageScale = 1.0f;
        m_owner->m_damageTimer = 0.0f;
    }
    float ratio = m_owner->m_health / m_owner->m_maxHealth;
    if (ratio <= 0.25f) {
        m_owner->m_healthScale = 0.7f;
    } else if (ratio <= 0.5f) {
        m_owner->m_healthScale = 0.85f;
    } else {
        m_owner->m_healthScale = 1.0f;
    }
    return false;
}

BlackDoomCtrlUnit_AttackCallback::BlackDoomCtrlUnit_AttackCallback(BlackDoomVarParam *param)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param) {
    m_kind = 0;
    PooledList<CharaColliBase>::iterator it;
    it = PooledList<CharaColliBase>::iterator(m_owner->m_colliders->Head());
    for (int i = 0; it != m_owner->m_colliders->End(); it++, i++) {
        if (i == 0) {
            boost::weak_ptr<CharaColliAttack> attack = fn_800C42D0(fn_80021B08(), *it);
            boost::shared_ptr<CharaColliAttack> collision(attack.lock());
            collision->m_hitCallback = GetHitCallback();
            collision->m_attackCallback = GetDamageCallback();
        }
    }
}

BlackDoomCtrlUnit_AttackCallback::~BlackDoomCtrlUnit_AttackCallback() {}

int BlackDoomCtrlUnit_AttackCallback::Tick(float dt) {
    return false;
}

void BlackDoomCtrlUnit_AttackCallback::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharInfo> attacker = Param(param).m_attacker.lock()->m_owner.lock();
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, attacker->m_position)));
    Param(param).m_damage = Param(param).m_attacker.lock()->m_power;
}

void BlackDoomCtrlUnit_AttackCallback::OnDamage(AttackCallbackParam &param) {
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
    m_owner->m_recentDamage += Param(param).m_damage;
    fn_8016F0CC(fn_80079808()->m_score, 0, m_owner->m_settings.m_score * Param(param).m_damage);
    boost::shared_ptr<CharaColliAttack> collision = Param(param).m_attacker.lock();
    if (after <= 0.0f) {
        m_owner->m_nextMode = 0xB;
        return;
    }
    if (before / m_owner->m_maxHealth >= 0.5f && after / m_owner->m_maxHealth < 0.5f) {
        m_owner->m_nextMode = 0xA;
        return;
    }
    if (collision->m_flags & 8) {
        m_owner->m_nextMode = 4;
        return;
    }
}

BlackDoomCtrlUnit_Move::BlackDoomCtrlUnit_Move(BlackDoomVarParam *param)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param) {
    m_kind = 2;
}

BlackDoomCtrlUnit_Move::~BlackDoomCtrlUnit_Move() {}

/* The boss's character. The original reads it through BossVarParam's shared_ptr<BossCharInfo>
 * (see Game/BossCtrlUnit.h), and the inline call moves retail's scheduling here. */
inline EnemyCharInfo *GetChara(BlackDoomVarParam *owner) {
    return owner->m_chara;
}

int BlackDoomCtrlUnit_Move::Tick(float dt) {
    fn_800091D0(&m_owner->m_prevPosition, &GetChara(m_owner)->m_position);
    m_owner->m_prevRotation = m_owner->m_chara->m_rotation.y;
    if (!m_owner->m_moveLocked) {
        float accel = m_owner->m_accel;
        accel -= m_owner->m_speed * m_owner->m_settings.m_drag;
        m_owner->m_speed += accel * dt;
        float step = m_owner->m_speed * dt;
        if (m_owner->m_direction == 0) {
            if (fn_8040EE90() < 0.5f) {
                m_owner->m_direction = -1;
            } else {
                m_owner->m_direction = 1;
            }
        }
        if (m_owner->m_nextCorner == -1) {
            m_owner->m_nextCorner = 4.0f * fn_8040EE90();
        }
        float height = m_owner->m_chara->m_position.y;
        if (m_owner->m_corner == -1) {
            Vec3f offset;
            fn_800091EC(&offset,
                        &Ref(Sub(m_owner->m_chara->m_position, Corner(m_owner->m_nextCorner))));
            offset.y = 0.0f;
            m_owner->m_angle = fn_80016260(-offset.x, -offset.z);
            float distance = m_owner->m_settings.m_radius;
            if (distance < fn_8040CF50(&offset)) {
                distance = fn_8000FEE4(fn_8040CF2C(&offset) - distance * distance);
                float turn = fn_8000F3F8(distance / m_owner->m_settings.m_radius);
                m_owner->m_angle += m_owner->m_direction * turn;
            } else {
                distance -= fn_8040CF50(&offset);
            }
            if (step > distance) {
                m_owner->m_angle += m_owner->m_direction * (distance / m_owner->m_settings.m_radius);
                Vec3f arm(-std::sin(m_owner->m_angle), 0.0f, -std::cos(m_owner->m_angle));
                fn_8040CE38(&arm, m_owner->m_settings.m_radius);
                fn_800091D0(&m_owner->m_chara->m_position,
                            &Ref(fn_8040CDD0(&arm, &Corner(m_owner->m_nextCorner))));
                m_owner->m_corner = m_owner->m_nextCorner;
                int pick;
                if (m_owner->m_prevCorner != -1) {
                    pick = 2.0f * fn_8040EE90();
                } else {
                    pick = 3.0f * fn_8040EE90();
                }
                int count;
                int i;
                for (i = 0, count = 0; i < 4; i++) {
                    if (i == m_owner->m_corner || i == m_owner->m_prevCorner) {
                        continue;
                    }
                    if (count == pick) {
                        m_owner->m_nextCorner = i;
                        break;
                    }
                    count++;
                }
            } else {
                Vec3f arm(-std::sin(m_owner->m_angle), 0.0f, -std::cos(m_owner->m_angle));
                fn_8040CE38(&arm, m_owner->m_settings.m_radius);
                fn_8040CFF8(&arm, &Corner(m_owner->m_nextCorner));
                fn_800091D0(&offset, &Ref(Sub(arm, m_owner->m_chara->m_position)));
                offset.y = 0.0f;
                fn_8040CFF8(&fn_801792D0(&m_owner->m_chara)->m_position,
                            &Ref(fn_800091A4(&Ref(fn_8040CEE8(&offset)), step)));
            }
        } else {
            Vec3f center;
            fn_800091EC(&center, &Ref(fn_8040CDA8(&Ref(fn_8040CDD0(&Corner(m_owner->m_nextCorner),
                                                                   &Corner(m_owner->m_corner))),
                                                  0.5f)));
            Vec3f offset;
            fn_800091EC(&offset, &Ref(Sub(center, Corner(m_owner->m_corner))));
            float angle = fn_80016260(-offset.x, -offset.z);
            float radius = m_owner->m_settings.m_radius;
            angle -= m_owner->m_direction *
                     fn_8000F3F8(fn_8000FEE4(fn_8040CF2C(&offset) - radius * radius) /
                                 m_owner->m_settings.m_radius);
            float diff = angle - m_owner->m_angle;
            while (std::fabs(diff) > lbl_805FAD38) {
                diff += diff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
            }
            if (std::fabs(diff * m_owner->m_settings.m_radius) < step) {
                m_owner->m_angle = angle;
                Vec3f arm(-std::sin(m_owner->m_angle), 0.0f, -std::cos(m_owner->m_angle));
                fn_8040CE38(&arm, m_owner->m_settings.m_radius);
                fn_8040CFF8(&arm, &Corner(m_owner->m_corner));
                step -= diff * m_owner->m_settings.m_radius;
                fn_800091D0(&offset, &Ref(Sub(center, arm)));
                fn_800091D0(&m_owner->m_chara->m_position,
                            &Ref(fn_8040CDD0(&arm, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&offset)),
                                                                     step)))));
                m_owner->m_prevCorner = m_owner->m_corner;
                m_owner->m_corner = -1;
                m_owner->m_direction *= -1;
            } else {
                m_owner->m_angle += m_owner->m_direction * (step / m_owner->m_settings.m_radius);
                Vec3f arm(-std::sin(m_owner->m_angle), 0.0f, -std::cos(m_owner->m_angle));
                fn_8040CE38(&arm, m_owner->m_settings.m_radius);
                fn_800091D0(&m_owner->m_chara->m_position,
                            &Ref(fn_8040CDD0(&arm, &Corner(m_owner->m_corner))));
            }
        }
        m_owner->m_chara->m_position.y = height;
        if (m_owner->m_recentDamage > m_owner->m_settings.m_damageLimit) {
            m_owner->m_chara->m_position.y -= m_owner->m_settings.m_climbSpeed * dt;
            float bottom = m_owner->m_home.y - 40.0f;
            if (m_owner->m_chara->m_position.y < bottom) {
                m_owner->m_chara->m_position.y = bottom;
            }
        } else {
            m_owner->m_chara->m_position.y += m_owner->m_settings.m_climbSpeed * dt;
            EnemyCharInfo *chara = m_owner->m_chara;
            if (chara->m_position.y > m_owner->m_home.y) {
                chara->m_position.y = m_owner->m_home.y;
            }
        }
    } else {
        m_owner->m_speed = 0.0f;
    }
    if (!m_owner->m_turnLocked) {
        float diff = m_owner->m_targetAngle - m_owner->m_chara->m_rotation.y;
        while (std::fabs(diff) > lbl_805FAD38) {
            diff += diff > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
        }
        m_owner->m_turnSpeed = ToRadian(m_owner->m_settings.m_turnSpeedMax);
        if (std::fabs(diff) < m_owner->m_turnSpeed * dt) {
            m_owner->m_chara->m_rotation.y += diff;
            m_owner->m_turnSpeed = 0.0f;
        } else {
            if (diff < 0.0f) {
                m_owner->m_turnSpeed = -m_owner->m_turnSpeed;
            }
            m_owner->m_chara->m_rotation.y += m_owner->m_turnSpeed * dt;
        }
    } else {
        m_owner->m_turnSpeed = 0.0f;
    }
    m_owner->m_angleLeft = m_owner->m_targetAngle - m_owner->m_chara->m_rotation.y;
    while (std::fabs(m_owner->m_angleLeft) > lbl_805FAD38) {
        m_owner->m_angleLeft +=
            m_owner->m_angleLeft > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
    }
    return false;
}

BlackDoomCtrlUnit_FixLookCharCamera::BlackDoomCtrlUnit_FixLookCharCamera(BlackDoomVarParam *param)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param), BossCtrlUnit_FixLookCharCamera(param) {}

/* Whether the camera may follow: not while the boss is hidden, nor while the player is below the
 * boss and near it (meanings inferred). */
bool BlackDoomCtrlUnit_FixLookCharCamera::IsEnabled() {
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(m_owner->m_target, m_owner->m_chara->m_position)));
    float height = offset.y;
    offset.y = 0.0f;
    if (m_owner->m_hidden) {
        return false;
    }
    if (height < 0.0f && fn_8040CF50(&offset) < m_owner->m_settings.m_cameraDistance) {
        return false;
    }
    return true;
}

BlackDoomCtrlUnit_Disk::BlackDoomCtrlUnit_Disk(BlackDoomVarParam *param, const Vec3f *position,
                                               const Vec3f *direction)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param) {
    m_kind = 2;
    fn_800091D0(&m_position, position);
    float speed = fn_8040CF50(direction);
    /* Retail flattens the caller's direction in place. */
    const_cast<Vec3f *>(direction)->y = 0.0f;
    fn_800091D0(&m_velocity, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(direction)), speed)));
    fn_800091D0(&m_offset,
                &Ref(fn_8040CDA8(&Ref(fn_80016284(&Ref(fn_8040CEE8(direction)))), 20.0f)));
    m_offset.y = -25.0f;
    m_velocity.y = -speed;
    fn_801D54F4(fn_80053164(), 0x8032, &m_position, 0, 1);
    UpdateMatrices();
    m_radius = 25.0f;
    m_landed = 0;
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0xF);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetMatrix(&m_effectMatrix);
        effect->SetField14(FrameTime());
    }
}

BlackDoomCtrlUnit_Disk::~BlackDoomCtrlUnit_Disk() {
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

int BlackDoomCtrlUnit_Disk::Tick(float dt) {
    if (m_collider) {
        m_collider.reset();
        return true;
    }
    if (m_landed) {
        m_distance += dt * fn_8040CF50(&m_velocity);
        if (m_distance > m_owner->m_settings.m_diskRange) {
            return true;
        }
    }
    Vec3f previous;
    fn_800091EC(&previous, &m_position);
    fn_8040CFF8(&m_position, &Ref(fn_800091A4(&m_velocity, dt)));
    if (m_position.y + m_offset.y < m_owner->m_home.y - 40.0f) {
        m_position.y = m_owner->m_home.y - 40.0f - m_offset.y;
        m_velocity.y = 0.0f;
    }
    fn_800091D0(&m_prevPosition, &m_position);
    UpdateMatrices();
    UnkLine_80020A20 line;
    fn_80020A20(&line, &previous, &m_position);
    UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 6, 0, 0);
    if (hits != 0) {
        void *object = HitObject(hits);
        boost::shared_ptr<EffectElement> effect(
            fn_800A31F0()->Launch(m_owner->m_effects, 0x1A).lock());
        if (effect) {
            fn_80425E4C(effect.get(), fn_8001E9BC(object));
            effect->SetField14(FrameTime());
        }
        fn_801D54F4(fn_80053164(), 0x8034, &m_position, 0, 1);
        fn_80026FB4(hits, 1);
        m_landed = 1;
        m_distance = 0.0f;
    }
    if (fn_8040CF50(&m_position) > 1500.0f) {
        return true;
    }
    fn_801D53C4(fn_80053164(), 0x8033, &m_position, this, 3, 0, 3);
    Vec3f local;
    fn_800091EC(&local, &Ref(fn_8040B67C(&RefM(fn_8040BEF8(&m_matrix)), &m_owner->m_target)));
    if (fn_8040CF50(&local) < m_radius && std::fabs(local.x) < 10.0f) {
        void *manager = fn_80021B08();
        fn_801D54F4(fn_80053164(), 0x8034, &m_position, 0, 1);
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

void BlackDoomCtrlUnit_Disk::UpdateMatrices() {
    float angle = fn_80016260(-m_velocity.x, -m_velocity.z);
    fn_8040C148(&m_matrix, &lbl_805E0720);
    fn_8040B59C(&m_matrix, &m_position);
    fn_8040B848(&m_matrix, angle);
    fn_8040C148(&m_effectMatrix, &lbl_805E0720);
    fn_8040B59C(&m_effectMatrix, &Ref(fn_8040CDD0(&m_position, &m_offset)));
    fn_8040B848(&m_effectMatrix, angle);
}

/* GC/2.7 keeps the address of the parameter's position in a register across the calls, where
 * retail recomputes it (as in DevilDoomCtrlUnit.cpp's AttackCallbackEye::OnDamage); turning
 * common-subexpression elimination off for this function reproduces retail's code. */
#pragma opt_common_subs off
void BlackDoomCtrlUnit_Disk::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, m_position)));
    Param(param).m_damage = attack.lock()->m_power;
    boost::shared_ptr<EffectElement> effect(fn_800A31F0()->Launch(m_owner->m_effects, 0x1A).lock());
    if (effect) {
        fn_80425E4C(effect.get(), &Param(param).m_position);
        effect->SetField14(FrameTime());
    }
}
#pragma opt_common_subs reset

BlackDoomCtrlUnit_Meteor::BlackDoomCtrlUnit_Meteor(BlackDoomVarParam *param,
                                                   const Vec3f *position, const Vec3f *velocity)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param) {
    m_kind = 2;
    fn_800091D0(&m_position, position);
    fn_800091D0(&m_velocity, velocity);
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 3);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetField4C((u32)&m_position);
        effect->SetField14(FrameTime());
    }
    void *manager = fn_80021B08();
    m_collider = fn_800C7EA8(manager, &m_position, 0, 4, 0, lbl_805EE698, &lbl_805E07A0, 8.0f);
    if (m_collider) {
        m_collider->SetField2C(1);
        m_collider->SetCharInfo((CharInfo *)&m_position);
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
    m_hit = 0;
    m_target = fn_801E72DC(0x800);
    if (m_target) {
        m_target->fn_800CCE38(&m_position);
    }
    fn_8034DE08(2, &m_position, -2);
}

BlackDoomCtrlUnit_Meteor::~BlackDoomCtrlUnit_Meteor() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetField4C(0);
        effect->Stop();
        fn_800A4210(effect);
    }
    effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x12).lock();
    if (effect) {
        fn_80425E4C(effect.get(), &m_position);
        effect->SetField14(FrameTime());
    }
    fn_801D54F4(fn_80053164(), 0x8036, &m_position, 0, 1);
    if (m_collider) {
        m_collider.reset();
    }
    if (m_target) {
        m_target.reset();
    }
}

int BlackDoomCtrlUnit_Meteor::Tick(float dt) {
    if (m_hit) {
        return true;
    }
    Vec3f previous;
    fn_800091EC(&previous, &m_position);
    fn_8040CFF8(&m_position, &Ref(fn_800091A4(&m_velocity, dt)));
    UnkLine_80020A20 line;
    fn_80020A20(&line, &previous, &m_position);
    UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 6, 0, 0);
    if (hits != 0) {
        fn_800091D0(&m_position, fn_8001E9BC(fn_8002735C(hits)->m_object));
        fn_80026FB4(hits, 1);
        return true;
    }
    if (fn_8040CF50(&m_position) > 1500.0f) {
        return true;
    }
    fn_801D53C4(fn_80053164(), 0x8035, &m_position, this, 3, 0, 3);
    if (CharaColliBase *collider = m_collider.get()) {
        collider->SetCharInfo((CharInfo *)&m_position);
    }
    if (Target *target = m_target.get()) {
        target->fn_800CCE38(&m_position);
    }
    return false;
}

void BlackDoomCtrlUnit_Meteor::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, m_position)));
    Param(param).m_damage = attack.lock()->m_power;
    m_hit = 1;
}

void BlackDoomCtrlUnit_Meteor::OnDamage(AttackCallbackParam &param) {
    Param(param).m_result = 1;
    m_hit = 1;
}

BlackDoomCtrlUnit_Boomerang::BlackDoomCtrlUnit_Boomerang(BlackDoomVarParam *param,
                                                         const Vec3f *position,
                                                         const Vec3f *velocity)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param) {
    m_kind = 2;
    fn_800091D0(&m_position, position);
    fn_800091D0(&m_velocity, velocity);
    m_radius = 40.0f;
    m_time = 0.0f;
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0xC);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetField4C((u32)&m_position);
        effect->SetField14(FrameTime());
    }
}

BlackDoomCtrlUnit_Boomerang::~BlackDoomCtrlUnit_Boomerang() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetField4C(0);
        effect->Stop();
        fn_800A4210(effect);
    }
    effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x16).lock();
    if (effect) {
        fn_80425E4C(effect.get(), &m_prevPosition);
        effect->SetField14(FrameTime());
    }
    if (m_collider) {
        m_collider.reset();
    }
}

int BlackDoomCtrlUnit_Boomerang::Tick(float dt) {
    m_time += dt;
    if (m_time > m_owner->m_settings.m_boomerangTime) {
        fn_801D54F4(fn_80053164(), 0x803A, &m_position, 0, 1);
        return true;
    }
    if (m_collider) {
        m_collider.reset();
        return true;
    }
    Vec3f previous;
    fn_800091EC(&previous, &m_position);
    fn_8040CFF8(&m_position, &Ref(fn_800091A4(&m_velocity, dt)));
    fn_800091D0(&m_prevPosition, &m_position);
    UnkLine_80020A20 line;
    fn_80020A20(&line, &previous, &m_position);
    UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 6, 0, 0);
    if (hits != 0) {
        void *object = HitObject(hits);
        fn_800091D0(&m_position, fn_8001E9BC(object));
        boost::shared_ptr<EffectElement> effect(
            fn_800A31F0()->Launch(m_owner->m_effects, 0x16).lock());
        if (effect) {
            fn_80425E4C(effect.get(), &m_position);
            effect->SetField14(FrameTime());
        }
        fn_801D54F4(fn_80053164(), 0x8039, &m_position, 0, 1);
        const Vec3f *normal = HitNormal(object);
        Vec3f axis;
        fn_800091EC(&axis,
                    &Ref(fn_8040CFB8(&Ref(fn_80016284(&Ref(fn_8040CEE8(&m_velocity)))), normal)));
        Mtx34f rotation;
        fn_8040C148(&rotation, &lbl_805E0720);
        fn_8040B848(&rotation, 2.0f * fn_800DF25C(fn_8040CF50(&axis)));
        fn_800091D0(&m_velocity, &Ref(fn_8040B618(&rotation, &Ref(fn_80016284(&m_velocity)))));
    }
    fn_801D53C4(fn_80053164(), 0x8038, &m_position, this, 3, 0, 3);
    Vec3f offset;
    fn_800091EC(&offset, &Ref(Sub(m_owner->m_target, m_position)));
    if (fn_8040CF50(&offset) < m_radius && std::fabs(offset.y) < 10.0f) {
        void *manager = fn_80021B08();
        fn_801D54F4(fn_80053164(), 0x803A, &m_position, 0, 1);
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

/* Not called anywhere: the linker strips it from the game. Retail pools the attack power's 1.0f
 * ahead of Boomerang::Tick's other float literals; compiling a function that uses it at this point
 * of the file makes GC/2.7 do the same (see docs/research-notes.md). */
void BlackDoomSetAttackPower(CharaColliAttack *collision) {
    collision->m_power = 1.0f;
}

/* GC/2.7 keeps the address of the parameter's position in a register across the calls, where
 * retail recomputes it (as in DevilDoomCtrlUnit.cpp's AttackCallbackEye::OnDamage); turning
 * common-subexpression elimination off for this function reproduces retail's code. */
#pragma opt_common_subs off
void BlackDoomCtrlUnit_Boomerang::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    fn_800091D0(&Param(param).m_offset, &Ref(Sub(Param(param).m_position, m_position)));
    Param(param).m_damage = attack.lock()->m_power;
    fn_800091D0(&m_prevPosition, &Param(param).m_position);
}
#pragma opt_common_subs reset

/* Plays the sound of motion attribute 0x5001's value, and keeps attribute 0x5000's (meanings
 * inferred). */
void BlackDoomCtrlUnit_CheckMotionAttribute::OnAttribute(u16 attribute, int value) {
    switch (attribute) {
    case 0x5001:
        switch (*reinterpret_cast<int *>(value)) {
        case 0:
            break;
        case 0x3B:
            fn_801D54F4(fn_80053164(), 0x803B, m_owner->m_chara, 0, 1);
            break;
        }
        break;
    case 0x5000:
        m_owner->m_attribute = reinterpret_cast<const BlackDoomAttribute *>(value);
        break;
    }
}

BlackDoomCtrlUnit_DispEye::BlackDoomCtrlUnit_DispEye(BlackDoomVarParam *param)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param), m_clump(0), m_frame(0), m_fade(0) {
    m_kind = 3;
    void *data = fn_8001277C(fn_800122D4(), 0x16, 6);
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        m_frame = (RwFrame *)m_clump->object.parent;
        UnkClumpFade_80027D5C *fade =
            (UnkClumpFade_80027D5C *)operator new(sizeof(UnkClumpFade_80027D5C));
        if (fade != 0) {
            fade = fn_80027D5C(fade);
        }
        m_fade = fade;
        if (m_fade != 0) {
            m_fade->m_mode = 8;
            m_fade->SetAlpha(m_owner->m_alpha);
            fn_80038198(m_clump, m_fade);
        }
    }
}

BlackDoomCtrlUnit_DispEye::~BlackDoomCtrlUnit_DispEye() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
        m_frame = 0;
    }
    if (m_fade != 0) {
        operator delete(m_fade);
        m_fade = 0;
    }
}

int BlackDoomCtrlUnit_DispEye::Tick(float dt) {
    if (m_owner->m_motion == 0 || m_frame == 0) {
        return false;
    }
    Mtx34f *node = fn_80415E70(m_owner->m_motion, 0x1E);
    RwMatrix *matrix = &m_frame->modelling;
    fn_80016390(matrix, node);
    matrix->pos.x += m_owner->field_0x48.x;
    matrix->pos.y += m_owner->field_0x48.y;
    matrix->pos.z += m_owner->field_0x48.z;
    fn_8048722C(m_frame);
    if (m_fade != 0) {
        m_fade->m_color[3] = m_owner->m_alpha;
    }
    return false;
}

BlackDoomCtrlUnit::BlackDoomCtrlUnit(BlackDoomVarParam *owner) : BossCtrlUnit(m_owner), m_owner(owner) {}

BlackDoomCtrlUnit_DispAura::BlackDoomCtrlUnit_DispAura(BlackDoomVarParam *param)
    : BossCtrlUnit(param), BlackDoomCtrlUnit(param), m_clump(0), m_updater(0), m_fade(0) {
    m_kind = 3;
    void *data = fn_8001277C(fn_800122D4(), 0x16, 7);
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    if (m_clump != 0) {
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        if (m_owner->m_motion != 0) {
            m_updater = new UnkClumpUpdater_80015A7C(m_clump, m_owner->m_motion);
        }
        UnkClumpFade_80027D5C *fade =
            (UnkClumpFade_80027D5C *)operator new(sizeof(UnkClumpFade_80027D5C));
        if (fade != 0) {
            fade = fn_80027D5C(fade);
        }
        m_fade = fade;
        if (m_fade != 0) {
            m_fade->m_mode = 8;
            m_fade->SetAlpha(m_owner->m_alpha);
            fn_80038198(m_clump, m_fade);
        }
    }
}

BlackDoomCtrlUnit::~BlackDoomCtrlUnit() {}

BlackDoomCtrlUnit_DispAura::~BlackDoomCtrlUnit_DispAura() {
    if (m_updater != 0) {
        delete m_updater;
        m_updater = 0;
    }
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
    }
    if (m_fade != 0) {
        operator delete(m_fade);
        m_fade = 0;
    }
}

int BlackDoomCtrlUnit_DispAura::Tick(float dt) {
    if (m_updater != 0) {
        fn_80015980(m_updater, &m_owner->field_0x48);
    }
    if (m_fade != 0) {
        m_fade->m_color[3] = m_owner->m_alpha;
    }
    BlackDoomVarParam *owner = m_owner;
    if (owner->m_alpha > 0.0f) {
        fn_801D53C4(fn_80053164(), 0x8030, &owner->m_chara->m_position, this, 3, 0, 3);
    }
    return false;
}

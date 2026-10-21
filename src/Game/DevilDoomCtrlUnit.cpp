/* The control units of the Devil Doom boss (class names from RTTI: DevilDoomCtrlUnit and its
 * DevilDoomCtrlUnit_* subclasses). Each unit is a BossCtrlUnit that the boss task runs on its
 * DevilDoomVarParam: the eyes, clouds, ball and wings models, the fire breath, the neck and eye
 * aim, the interrupt check, the turning, the attack callbacks of the chest and eyes, and the status
 * update. File name inferred from the classes; names marked "(name inferred)" and all member names
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * The unit is NonMatching: every function and the function order match except six that differ in
 * register use or scheduling under GC/2.7 (129/135 functions): the DoomsEye constructor (a clrlwi
 * before fn_804125F4), AttackCallbackEye::OnDamage and WingControl::OnDamage (GC/2.7 keeps the
 * address of the parameter's position in a register where retail recomputes it; with
 * "#pragma opt_common_subs off" the former matches but for one reloaded pointer),
 * WingControl::GetWingAngle (float register numbering), WingControl's constructor (the argument
 * scheduling of fn_800C7EA8) and GetStatus::Tick (one float register). The Devil Doom boss task
 * (DevilDoom, DevilDoomVarParam and DEVIL_DOOM's type record, 0x8030BCD4-0x8030CC08) is a separate
 * unit: its .sdata2 repeats this unit's float constants. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/DevilDoomCtrlUnit.h"
#include "Game/EnemyCharInfo.h"
#include "Game/GiHPGaugeEnemy.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* MSL's std::sin, std::cos and std::fabs for floats, which the game calls out of line. They are
 * declared here rather than through <cmath>, whose sqrtf adds two double constants to .sdata2
 * that this unit lacks (as in Coaster.cpp). */
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

/* What a motion passes to its callbacks (layout from its use here; names inferred). */
struct _stMtnCallbackParam {
    /* 0x0 */ void *m_frame;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ s32 m_id;
};

/* What fn_80415D40 reports about a motion (layout from its use here). */
struct UnkMotionInfo_80415D40 {
    /* 0x00 */ u8 field_0x0[0xC];
    /* 0x0C */ float m_frame;
    /* 0x10 */ u8 field_0x10[0x8];
};

/* Target (class name from RTTI): the flag word and the value at 0x24 are mapped here (bit 0 makes
 * the target active; see Bomb.cpp). */
class Target {
  public:
    void TurnOn() {
        m_flags |= 1;
    }
    void TurnOff() {
        m_flags &= ~1;
    }
    void SetField24(float value) {
        field_0x24 = value;
    }

    /* 0x00 */ u8 field_0x0[0x24];
    /* 0x24 */ float field_0x24;
    /* 0x28 */ u8 field_0x28[0x30 - 0x28];
    /* 0x30 */ u32 m_flags;
};

/* The parts of AttackCallbackParam the units read (layout as in CharaColliAttack.h; the members
 * between are named here, meanings inferred). */
struct DevilDoomAttackParam {
    /* 0x00 */ boost::weak_ptr<CharaColliAttack> m_attacker;
    /* 0x08 */ u8 field_0x8[0x18 - 0x8];
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ Vec3f m_direction;
    /* 0x30 */ u8 field_0x30[0x34 - 0x30];
    /* 0x34 */ Vec3f m_offset;
    /* 0x40 */ u8 field_0x40[0x50 - 0x40];
    /* 0x50 */ float m_damage;
    /* 0x54 */ s32 m_result;
};

#define Param(param) (*(DevilDoomAttackParam *)&(param))

/* The effect manager fn_800A31F0 returns (as in EffectPJS.cpp); its third virtual function
 * launches effect id from a launch description. */
class UnkEffectManager_800A31F0 {
  public:
    virtual void vf08();
    virtual boost::weak_ptr<EffectElement> Launch(s32 id, s32 arg);
    virtual boost::weak_ptr<EffectElement> Launch(boost::shared_ptr<EffectLaunchInfo> info, s32 id);
};

/* What fn_804125F4 reports about a motion id (layout from its use here). */
struct UnkMotionRange_804125F4 {
    /* 0x0 */ float m_end;
    /* 0x4 */ u8 field_0x4[0x10 - 0x4];
};

/* The game state (struct name unknown; see AdvMemoryCheck.cpp): the frame time at 0x24. */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x24];
    /* 0x024 */ float m_dt;
    /* 0x028 */ u8 field_0x28[0xA8 - 0x28];
    /* 0x0A8 */ u8 m_pause[0x150 - 0xA8];
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The world of each stage area (layout from its use here). */
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

class UnkHintManager_801FB180;
class UnkCharaList_80074194;

/* The player's character id (see Helicopter.cpp) and the null owner of colliders. */
extern s32 lbl_805EE588;
extern boost::shared_ptr<CharInfo> lbl_805EE698;
extern const Vec3f lbl_805E07B8;

extern "C" {
/* Effects (see EffectPJS.cpp). */
UnkEffectManager_800A31F0 *fn_800A31F0(void);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);
/* Launches effect id at position (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_801A1F54(const Vec3f *position, const Vec3f *rotation,
                                           int id);

/* RenderWare clumps and frames (see SecretDoor.cpp). */
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RpClump *fn_80457090(void *data);
void fn_80457468(RpClump *clump);
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
void fn_8003A840(int area);
void fn_80016370(Mtx34f *matrix, void *frame);
void fn_800163F4(void *frame, const Mtx34f *matrix);

/* Motions (behaviour inferred). */
void fn_80415D40(_stMtnPlayWork *work, int arg1, UnkMotionInfo_80415D40 *info);
void fn_80416E20(_stMtnPlayWork *work, int arg1, float frame);
void fn_80416DC8(_stMtnPlayWork *work, int arg1, float dt);
void fn_804160F8(_stMtnPlayWork *work, int arg1, float dt);
void fn_80416014(_stMtnPlayWork *work, int arg1, float value);
void fn_80416148(_stMtnPlayWork *work, int arg1, float value);
void fn_804165D8(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_80415EA4(_stMtnPlayWork *work);
int fn_80416F28(_stMtnPlayWork *work, int arg1, int arg2, int arg3);
void fn_8041B884(_stMtnPlayWork *work);
_stMtnPlayWork *fn_8041B908(void *data, int arg1);
void fn_804173A8(_stMtnPlayWork *work, const Vec3f *position, const Vec3f *rotation,
                 int arg3);
void fn_80417568(_stMtnPlayWork *work, u16 motion);
void *fn_80415E70(_stMtnPlayWork *work, int id);
void fn_80415808(_stMtnPlayWork *work, int id, void (*callback)(void *, void *), void *data);
void fn_80411634(void *frame, void *node, void *part);
UnkMotionRange_804125F4 fn_804125F4(u16 motion);
void fn_80335844(void *, void *);

/* The clump updater's update (behaviour inferred). */
void fn_800159AC(UnkClumpUpdater_80015A7C *updater);

/* The resource loader (see EggLastMech.cpp and EffectPJS.cpp). */
void *fn_800122D4(void);
void *fn_80012684(void *loader, int slot, int index);
void *fn_8001277C(void *loader, s32 slot, int arg2);

/* Sounds (see BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, const void *source, int arg3, int arg4);
void fn_801D53C4(void *manager, int id, const Vec3f *position, void *source, int arg4,
                 int arg5, int arg6);

/* Hints (see Cream.cpp) and the score (see EggLastMech.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
UnkObject_80079808 *fn_80079808(void);
void fn_8016F0CC(void *score, int arg1, int points);
bool fn_802039E8(void *pause);

/* Characters and colliders (see Bomb.cpp and Monster.cpp). */
UnkCharaList_80074194 *fn_80074194(void);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *manager, int id);
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::shared_ptr<CharaColliBase> fn_800C799C(void *manager, const Vec3f *position,
                                              const Vec3f *arg2, int arg3, int arg4, int arg5,
                                              int arg6, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, const Vec3f *rotation,
                                              int arg10, float arg11, float arg12);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
boost::weak_ptr<CharaColliAttack> fn_800C42D0(void *manager, boost::shared_ptr<CharaColliBase> collider);
void fn_801E73B0(Target *target, const Vec3f *position);

/* The boss's lists (behaviour from their bodies). */
UnkListNode_80179304 *fn_8017935C(UnkList_80179304 *list);
UnkListIter_80179304 fn_80179354(UnkListNode_80179304 *node);
UnkListNode_80179304 *fn_801792D8(UnkList_80179304 *list);
boost::shared_ptr<CharaColliBase> fn_80179304(const UnkListIter_80179304 *it);
UnkListIter_80179304 fn_801792E0(UnkListIter_80179304 *it, int);
UnkListNode_80179304 *fn_8017B008(UnkList_80179304 *list);
UnkListIter_80179304 fn_8017B000(UnkListNode_80179304 *node);
UnkListNode_80179304 *fn_8017AF84(UnkList_80179304 *list);
boost::shared_ptr<Target> fn_8017AFB0(const UnkListIter_80179304 *it);
UnkListIter_80179304 fn_8017AF8C(UnkListIter_80179304 *it, int);

/* Math (behaviour inferred). */
float fn_8000FEE4(float value); /* square root */
float fn_8000F3D4(float angle);
float fn_8000F3F8(float value);
float fn_80016260(float x, float z); /* the angle of (x, z) */
Mtx34f fn_8040BEF8(const Mtx34f *matrix);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Mtx34f fn_8040EFA4(const Mtx34f *a, const Mtx34f *b, float t);
void fn_8040B52C(Mtx34f *matrix, float x, float y, float z);
void fn_8040FA20(Mtx34f *matrix, const Vec3f *translation);
void fn_80049290(Vec3f *rotation, const Mtx34f *matrix);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040CE74(Vec3f *vector);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
Vec3f fn_80016284(const Vec3f *direction);
Vec3f fn_80093230(const Vec3f *direction);
}

UnkHintManager_801FB180 *GetHintManager();

/* The last frame of motion (name inferred). NonMatching: retail passes the u16 motion on without
 * the clrlwi GC/2.7 emits here. */
inline float MotionEnd(int motion) {
    return fn_804125F4(motion).m_end;
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Passes a returned vector on by address (as in ArkCannon.cpp's casts). */
inline const Vec3f &Ref(const Vec3f &vector) {
    return vector;
}

/* Degrees to radians (name inferred). */
inline float ToRadian(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Radians to degrees (name inferred). */
inline float ToDegree(float radians) {
    return radians * (180.0f / lbl_805FAD38);
}

/* Likewise for a returned matrix. */
inline const Mtx34f &RefM(const Mtx34f &matrix) {
    return matrix;
}

/* Puts clump in world (name inferred). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* Turns matrix toward target by at most maxAngle (name inferred). */
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
    float pitch = fn_8000F3F8(-local.y);
    float yaw = fn_8000F3F8(local.x);
    fn_8040B884(matrix, pitch);
    fn_8040B848(matrix, yaw);
}

DevilDoomCtrlUnit_GetStatus::DevilDoomCtrlUnit_GetStatus(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 0;
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> character(player.lock());
    if (character) {
        fn_800091D0(&m_owner->m_prevTarget, &character->m_position);
    }
}

DevilDoomCtrlUnit_GetStatus::~DevilDoomCtrlUnit_GetStatus() {}

/* The angle wrapped to [-pi, pi] (name inferred). */
inline float WrapStep(float angle) {
    return angle > 0.0f ? -2.0f * lbl_805FAD38 : 2.0f * lbl_805FAD38;
}

#define WRAP_ANGLE(angle)                                                                              while (std::fabs(angle) > lbl_805FAD38) {                                                              angle += WrapStep(angle);                                                                      }

int DevilDoomCtrlUnit_GetStatus::Tick(float dt) {
    boost::weak_ptr<CharInfo> player = fn_800A8B74(fn_80074194(), lbl_805EE588);
    boost::shared_ptr<CharInfo> character(player.lock());
    if (character) {
        fn_800091D0(&m_owner->m_playerPosition, &character->m_position);
        if (!m_owner->field_0x18D) {
            fn_800091D0(&m_owner->m_target, &character->m_position);
        }
    }
    Vec3f delta;
    fn_800091EC(&delta, &Ref(((VectorSubtractFn)fn_8040CD74)(&m_owner->m_target, &m_owner->m_prevTarget)));
    float length = fn_8040CF50(&delta);
    float speed = m_owner->m_settings.field_0xC;
    if (length > speed * dt) {
        fn_800091D0(&delta, &Ref(fn_8040CDA8(&Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&delta)), speed)), dt)));
        fn_800091D0(&m_owner->m_target, &Ref(fn_8040CDD0(&m_owner->m_prevTarget, &delta)));
    }
    fn_800091D0(&m_owner->m_prevTarget, &m_owner->m_target);
    fn_800091D0(&delta, &Ref(((VectorSubtractFn)fn_8040CD74)(&m_owner->m_target,
                                                             &m_owner->m_chara->m_position)));
    delta.y = 0.0f;
    m_owner->m_distance = fn_8040CF50(&delta);
    m_owner->m_angle = m_owner->m_chara->m_rotation.y;
    if (m_owner->m_inverted) {
        m_owner->m_angle += 0.5f * lbl_805FAD38;
    } else {
        m_owner->m_angle -= 0.5f * lbl_805FAD38;
    }
    WRAP_ANGLE(m_owner->m_angle);
    m_owner->m_targetAngle = fn_80016260(-delta.x, -delta.z);
    m_owner->m_angleDiff = m_owner->m_targetAngle - m_owner->m_angle;
    WRAP_ANGLE(m_owner->m_angleDiff);
    m_owner->m_healthRatio = m_owner->m_health / m_owner->m_maxHealth;
    m_owner->m_healthLevel = 4.0f * m_owner->m_healthRatio;
    if (m_owner->m_healthLevel > 3) {
        m_owner->m_healthLevel = 3;
    }
    if (m_owner->m_healthLevel < 0) {
        m_owner->m_healthLevel = 0;
    }
    m_owner->m_timeRatio = (600.0f - m_owner->m_time) / 600.0f;
    m_owner->m_timeLevel = 4.0f * m_owner->m_timeRatio;
    if (m_owner->m_timeLevel > 3) {
        m_owner->m_timeLevel = 3;
    }
    if (m_owner->m_timeLevel < 0) {
        m_owner->m_timeLevel = 0;
    }
    m_owner->field_0x1A4 -= dt;
    m_owner->field_0x1A8 -= dt;
    m_owner->field_0x630 -= dt;
    m_owner->field_0x62C -= dt;
    return false;
}

DevilDoomCtrlUnit_AttackCallback::DevilDoomCtrlUnit_AttackCallback(DevilDoomVarParam *param, int index)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 0;
    UnkListIter_80179304 it;
    it = fn_80179354(fn_8017935C(m_owner->m_colliders));
    for (int i = 0; it.m_node != fn_801792D8(m_owner->m_colliders); fn_801792E0(&it, 0), i++) {
        if (i == index) {
            boost::weak_ptr<CharaColliAttack> attack = fn_800C42D0(fn_80021B08(), fn_80179304(&it));
            boost::shared_ptr<CharaColliAttack> collision(attack.lock());
            collision->m_attackCallback = GetDamageCallback();
        }
    }
}

void DevilDoomCtrlUnit_AttackCallbackEye::OnDamage(AttackCallbackParam &param) {
    if (m_owner->m_invincible) {
        Param(param).m_result = 5;
        return;
    }
    Param(param).m_result = 1;
    fn_803358D0(m_owner, new BossCtrlUnit_DamageEff(m_owner));
    Vec3f rotation;
    fn_800091EC(&rotation, &lbl_805E07A0);
    fn_801A1F54(&Param(param).m_position, &rotation, 0x69);
    fn_801D54F4(fn_80053164(), 0x8142, &Param(param).m_position, 0, 1);
    float before = m_owner->m_health;
    m_owner->m_health = before - Param(param).m_damage;
    if (m_owner->m_health < 0.0f) {
        m_owner->m_health = 0.0f;
    }
    float after = m_owner->m_health;
    fn_8016F0CC(fn_80079808()->m_score, 0, (float)m_owner->m_settings.m_score * Param(param).m_damage);
    if (after <= 0.0f) {
        m_owner->m_nextMode = 0xE;
    } else if (before / m_owner->m_maxHealth >= 0.5f && after / m_owner->m_maxHealth < 0.5f) {
        m_owner->m_nextMode = 0xD;
    } else {
        m_owner->m_nextMode = 3;
    }
}

void DevilDoomCtrlUnit_AttackCallbackEyeR::OnDamage(AttackCallbackParam &param) {
    if (!m_owner->m_eyeOpen[0]) {
        Param(param).m_result = 5;
    } else {
        DevilDoomCtrlUnit_AttackCallbackEye::OnDamage(param);
    }
}

void DevilDoomCtrlUnit_AttackCallbackEyeL::OnDamage(AttackCallbackParam &param) {
    if (!m_owner->m_eyeOpen[1]) {
        Param(param).m_result = 5;
    } else {
        DevilDoomCtrlUnit_AttackCallbackEye::OnDamage(param);
    }
}

void DevilDoomCtrlUnit_AttackCallbackChest::OnDamage(AttackCallbackParam &param) {
    Param(param).m_result = 5;
}

DevilDoomCtrlUnit_Move::DevilDoomCtrlUnit_Move(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 2;
}

DevilDoomCtrlUnit_Move::~DevilDoomCtrlUnit_Move() {}

int DevilDoomCtrlUnit_Move::Tick(float dt) {
    if (!m_owner->m_fixed) {
        float diff = m_owner->m_targetAngle - m_owner->m_angle;
        WRAP_ANGLE(diff);
        m_owner->m_turnSpeed = ToRadian(m_owner->m_settings.field_0x0);
        if (std::fabs(diff) < m_owner->m_turnSpeed * dt) {
            m_owner->m_turnSpeed = 0.0f;
        } else {
            if (diff < 0.0f) {
                m_owner->m_turnSpeed = -m_owner->m_turnSpeed;
            }
            diff = m_owner->m_turnSpeed * dt;
        }
        m_owner->m_chara->m_rotation.y += diff;
        m_owner->m_angle += diff;
    } else {
        m_owner->m_turnSpeed = 0.0f;
    }
    m_owner->m_angleLeft = m_owner->m_targetAngle - m_owner->m_angle;
    WRAP_ANGLE(m_owner->m_angleLeft);
    return false;
}

DevilDoomCtrlUnit_FixLookCharCamera::DevilDoomCtrlUnit_FixLookCharCamera(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param), BossCtrlUnit_FixLookCharCamera(param) {
    fn_800091D0(&m_offset, &Vec3f(0.0f, 50.0f, 0.0f));
}

bool DevilDoomCtrlUnit_FixLookCharCamera::IsEnabled() {
    return m_owner->m_hidden == 0;
}

/* The frame time the effects follow (name inferred). */
#define FrameTime() ((u32)&lbl_8057D6A0.m_dt)

DevilDoomCtrlUnit_AdjustEye::DevilDoomCtrlUnit_AdjustEye(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 2;
    m_callback = GetCallback();
    fn_80415808(m_owner->m_motion, 0x403, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x3F9, fn_80335844, &m_callback);
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x37);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetMatrix(&m_matrix);
        effect->SetField14(FrameTime());
    }
    m_blend = 0.0f;
}

DevilDoomCtrlUnit_AdjustEye::~DevilDoomCtrlUnit_AdjustEye() {
    fn_80415808(m_owner->m_motion, 0x403, 0, 0);
    fn_80415808(m_owner->m_motion, 0x3F9, 0, 0);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
    }
}

int DevilDoomCtrlUnit_AdjustEye::Tick(float dt) {
    m_dt = dt;
    if (m_owner->m_eyeClosing) {
        m_blend += m_owner->m_settings.m_eyeOpenSpeed * dt;
        if (m_blend > 1.0f) {
            m_blend = 1.0f;
        }
    } else {
        m_blend -= m_owner->m_settings.m_eyeOpenSpeed * dt;
        if (m_blend < 0.0f) {
            m_blend = 0.0f;
        }
    }
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        if (m_owner->m_hidden) {
            effect->Hide();
        } else if (m_owner->m_eyeOpen[0] || m_owner->m_eyeOpen[1]) {
            effect->Show();
        } else {
            effect->Hide();
        }
    }
    return false;
}

void DevilDoomCtrlUnit_AdjustEye::OnMotion(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    bool closed = false;
    int eye = 0;
    switch (param.m_id) {
    case 0x403:
        eye = 0;
        break;
    case 0x3F9:
        eye = 1;
        break;
    }
    if (m_owner->m_eyeOpen[eye]) {
        m_owner->m_eyeAngle[eye] -= ToRadian(m_owner->m_settings.m_eyeSpeed * m_dt);
        if (m_owner->m_eyeAngle[eye] < 0.0f) {
            m_owner->m_eyeAngle[eye] = 0.0f;
            closed = true;
        }
    } else {
        m_owner->m_eyeAngle[eye] += ToRadian(m_owner->m_settings.m_eyeSpeed * m_dt);
        if (m_owner->m_eyeAngle[eye] > lbl_805FAD38) {
            m_owner->m_eyeAngle[eye] = lbl_805FAD38;
        }
    }
    fn_8040B884(&matrix, m_owner->m_eyeAngle[eye]);
    if (closed) {
        Mtx34f aimed;
        fn_8040C148(&aimed, &matrix);
        TurnToward(&aimed, &m_owner->m_target, lbl_805FAD38 / 3.0f);
        fn_8040C148(&matrix, &RefM(fn_8040EFA4(&matrix, &aimed, m_blend)));
    }
    fn_800163F4(param.m_frame, &matrix);
    if (m_owner->m_eyeOpen[eye]) {
        fn_8040C148(&m_matrix, &matrix);
    }
    UnkListIter_80179304 it;
    it = fn_8017B000(fn_8017B008(m_owner->m_targets));
    for (int i = 0; it.m_node != fn_8017AF84(m_owner->m_targets); fn_8017AF8C(&it, 0), i++) {
        if (i == eye) {
            fn_801E73B0(fn_8017AFB0(&it).get(), &Vec3f(matrix.values[0][2], matrix.values[1][2],
                                                                matrix.values[2][2]));
            fn_8017AFB0(&it)->SetField24(0.0f);
        }
    }
}

DevilDoomCtrlUnit_AdjustNeck::DevilDoomCtrlUnit_AdjustNeck(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 2;
    m_callback = GetCallback();
    fn_80415808(m_owner->m_motion, 0x3FE, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x3F4, fn_80335844, &m_callback);
    m_blend = 0.0f;
}

DevilDoomCtrlUnit_AdjustNeck::~DevilDoomCtrlUnit_AdjustNeck() {
    fn_80415808(m_owner->m_motion, 0x3FE, 0, 0);
    fn_80415808(m_owner->m_motion, 0x3F4, 0, 0);
}

int DevilDoomCtrlUnit_AdjustNeck::Tick(float dt) {
    if (m_owner->m_neckUp) {
        m_blend += m_owner->m_settings.m_neckSpeed * dt;
        if (m_blend > 1.0f) {
            m_blend = 1.0f;
        }
    } else {
        m_blend -= m_owner->m_settings.m_neckSpeed * dt;
        if (m_blend < 0.0f) {
            m_blend = 0.0f;
        }
    }
    return false;
}

void DevilDoomCtrlUnit_AdjustNeck::OnMotion(_stMtnCallbackParam &param) {
    Mtx34f matrix;
    fn_80016370(&matrix, param.m_frame);
    int eye = 0;
    switch (param.m_id) {
    case 0x3FE:
        eye = 0;
        break;
    case 0x3F4:
        eye = 1;
        break;
    }
    if (m_owner->m_eyeOpen[eye]) {
        Mtx34f aimed;
        fn_8040C148(&aimed, &matrix);
        TurnToward(&aimed, &m_owner->m_target, lbl_805FAD38 / 6.0f);
        fn_8040C148(&matrix, &RefM(fn_8040EFA4(&matrix, &aimed, m_blend)));
    }
    fn_800163F4(param.m_frame, &matrix);
}

DevilDoomCtrlUnit_CheckInterrupt::DevilDoomCtrlUnit_CheckInterrupt(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 2;
}

DevilDoomCtrlUnit_CheckInterrupt::~DevilDoomCtrlUnit_CheckInterrupt() {}

int DevilDoomCtrlUnit_CheckInterrupt::Tick(float dt) {
    if (!m_owner->m_active) {
        return false;
    }
    if (fn_802039E8(lbl_8057D6A0.m_pause)) {
        return false;
    }
    if (m_owner->m_mode == 5) {
        return false;
    }
    if (m_owner->m_distance < m_owner->m_settings.m_minDistance) {
        m_owner->m_nextMode = 5;
        return false;
    }
    if (m_owner->m_distance > m_owner->m_settings.m_maxDistance) {
        m_owner->m_nextMode = 5;
        return false;
    }
    return false;
}

DevilDoomCtrlUnit_FireBreath::DevilDoomCtrlUnit_FireBreath(DevilDoomVarParam *param,
                                                           const Vec3f *start,
                                                           const Vec3f *direction)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 2;
    fn_800091D0(&m_end, fn_800091D0(&m_start, start));
    fn_800091D0(&m_direction, direction);
    m_flameCount = 0;
    m_hitGround = 0;
    Vec3f up(0.0f, 1.0f, 0.0f);
    Vec3f forward;
    fn_800091EC(&forward, &Ref(fn_8040CEE8(&m_direction)));
    Vec3f side;
    fn_800091EC(&side, &Ref(fn_8040CFB8(&up, &forward)));
    if (fn_8040CF50(&side) < 0.0001) {
        fn_800091D0(&up, &Vec3f(1.0f, 0.0f, 0.0f));
        fn_800091D0(&side, &Ref(fn_8040CEE8(&Ref(fn_8040CFB8(&up, &forward)))));
    } else {
        fn_8040CE74(&side);
    }
    fn_800091D0(&up, &Ref(fn_8040CFB8(&forward, &side)));
    fn_8040C148(&m_endMatrix, &lbl_805E0720);
    fn_8040B59C(&m_endMatrix, &m_end);
    m_endMatrix.values[0][0] = side.x;
    m_endMatrix.values[1][0] = side.y;
    m_endMatrix.values[2][0] = side.z;
    m_endMatrix.values[0][1] = up.x;
    m_endMatrix.values[1][1] = up.y;
    m_endMatrix.values[2][1] = up.z;
    m_endMatrix.values[0][2] = forward.x;
    m_endMatrix.values[1][2] = forward.y;
    m_endMatrix.values[2][2] = forward.z;
    fn_8040C148(&m_matrix, &m_endMatrix);
    fn_8040B52C(&m_matrix, 0.0f, 0.0f, 50.0f);
    m_effect = fn_800A31F0()->Launch(m_owner->m_effects, 0xB);
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->SetMatrix(&m_matrix);
        effect->SetField14(FrameTime());
    }
    m_endEffect = fn_800A31F0()->Launch(m_owner->m_effects, 0xF);
    effect = m_endEffect.lock();
    if (effect) {
        effect->SetMatrix(&m_endMatrix);
        effect->SetField14(FrameTime());
    }
    void *manager = fn_80021B08();
    m_collider = fn_800C799C(manager, &m_end, &lbl_805E07B8, 0, 0, 4, 0, lbl_805EE698, &lbl_805E07A0,
                             &lbl_805E07B8, 0, 0.0f, 20.0f);
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
            collision->field_0x55 = 0;
        }
    }
}

DevilDoomCtrlUnit_FireBreath::~DevilDoomCtrlUnit_FireBreath() {
    boost::shared_ptr<EffectElement> effect(m_effect.lock());
    if (effect) {
        effect->Stop();
    }
    effect = m_endEffect.lock();
    if (effect) {
        effect->Stop();
    }
    for (int i = 0; i < 16; i++) {
        effect = m_flameEffects[i].lock();
        if (effect) {
            effect->Stop();
        }
    }
    if (m_collider) {
        m_collider.reset();
    }
}

int DevilDoomCtrlUnit_FireBreath::Tick(float dt) {
    if (m_hitGround) {
        fn_8040CFF8(&m_end, &Ref(fn_80093230(&m_direction)));
    } else {
        fn_8040CFF8(&m_start, &Ref(fn_80093230(&m_direction)));
    }
    Vec3f delta;
    fn_800091EC(&delta, &Ref(((VectorSubtractFn)fn_8040CD74)(&m_start, &m_end)));
    if (fn_8040CF90(&delta, &m_direction) < 0.0f) {
        return true;
    }
    float length = fn_8040CF50(&delta);
    if (length > 800.0f) {
        length = 800.0f;
        fn_800091D0(&m_end, &Ref(((VectorSubtractFn)fn_8040CD74)(
                                 &m_start, &Ref(fn_8040CDA8(&Ref(fn_8040CEE8(&delta)), 800.0f)))));
    }
    int count = (int)(length / 50.0f) + 1;
    if (count > 16) {
        count = 16;
    }
    boost::shared_ptr<EffectElement> effect;
    while (m_flameCount < count) {
        m_flameEffects[m_flameCount] = fn_800A31F0()->Launch(m_owner->m_effects, 0xD);
        effect = m_flameEffects[m_flameCount].lock();
        if (effect) {
            effect->SetMatrix(&m_flames[m_flameCount]);
            effect->SetField14(FrameTime());
        }
        effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x26).lock();
        if (effect) {
            Mtx34f matrix;
            fn_8040C148(&matrix, &m_matrix);
            fn_8040FA20(&matrix, &m_start);
            fn_80425E68(effect.get(), &matrix);
            effect->SetField14(FrameTime());
        }
        m_flameCount++;
    }
    while (m_flameCount > count) {
        m_flameCount--;
        effect = m_flameEffects[m_flameCount].lock();
        if (effect) {
            effect->Stop();
        }
    }
    fn_8040FA20(&m_endMatrix, &m_end);
    fn_8040C148(&m_matrix, &m_endMatrix);
    fn_8040B52C(&m_matrix, 0.0f, 0.0f, length - 50.0f);
    float nearest = -1.0f;
    for (int i = 0; i < m_flameCount; i++) {
        if (i == 0) {
            fn_8040C148(&m_flames[i], &m_matrix);
            fn_8040B52C(&m_flames[i], 0.0f, 0.0f, 50.0f);
        } else {
            fn_8040C148(&m_flames[i], &m_flames[i - 1]);
            fn_8040B52C(&m_flames[i], 0.0f, 0.0f, -25.0f);
        }
        if (i == m_flameCount - 1) {
            float rest = length - 50.0f * i;
            fn_8040B52C(&m_flames[i], 0.0f, 0.0f, 0.5f * -rest);
            fn_8040B720(&m_flames[i], 1.0f, 1.0f, rest / 50.0f);
        } else {
            fn_8040B52C(&m_flames[i], 0.0f, 0.0f, -25.0f);
        }
        Vec3f position;
        position.x = m_flames[i].values[0][3];
        position.y = m_flames[i].values[1][3];
        position.z = m_flames[i].values[2][3];
        float distance =
            fn_8040CF50(&Ref(((VectorSubtractFn)fn_8040CD74)(&m_owner->m_playerPosition, &position)));
        if (nearest < 0.0f || nearest > distance) {
            nearest = distance;
            fn_800091D0(&m_soundPosition, &position);
        }
    }
    fn_801D53C4(fn_80053164(), 0x813E, &m_soundPosition, this, 3, 0, 3);
    if (fn_8040B67C(&RefM(fn_8040BEF8(&m_endMatrix)), &m_owner->m_target).z < 0.0f) {
        m_hitGround = 1;
    }
    Vec3f center;
    fn_800091EC(&center, &Ref(fn_8040CDA8(&Ref(fn_8040CDD0(&m_start, &m_end)), 0.5f)));
    m_collider.px->SetCharInfo((CharInfo *)(&center));
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    fn_80049290(&rotation, &m_endMatrix);
    m_collider.px->SetRotation(&rotation);
    m_collider.px->SetSize(18.0f, 0.0f, 0.5f * length, -2);
    return false;
}

void DevilDoomCtrlUnit_FireBreath::OnHit(AttackCallbackParam &param) {
    boost::weak_ptr<CharaColliAttack> attack(Param(param).m_attacker);
    boost::shared_ptr<CharaColliAttack> collision(attack.lock());
    fn_800091D0(&Param(param).m_offset, &Ref(((VectorSubtractFn)fn_8040CD74)(
                                            &Param(param).m_position, &m_owner->m_chara->m_position)));
    Param(param).m_damage = attack.lock()->m_power;
}

DevilDoomCtrlUnit_WingControl::DevilDoomCtrlUnit_WingControl(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 2;
    m_motions[0] = fn_8041B908(fn_80012684(fn_800122D4(), 0x16, 7), 2);
    m_motions[1] = fn_8041B908(fn_80012684(fn_800122D4(), 0x16, 8), 2);
    Vec3f position;
    Vec3f rotation;
    rotation.x = rotation.y = rotation.z = position.x = position.y = position.z = 0.0f;
    for (int i = 0; i < 2; i++) {
        fn_804173A8(m_motions[i], &position, &rotation, 0);
    }
    fn_80417568(m_motions[0], m_owner->m_motionBase + 0x18);
    fn_80416DC8(m_motions[0], 0, 0.0f);
    fn_804165D8(m_motions[0], 2, -1, -1);
    fn_80417568(m_motions[0], m_owner->m_motionBase + 0x17);
    fn_80417568(m_motions[1], m_owner->m_motionBase + 0x15);
    fn_80416DC8(m_motions[1], 0, 0.0f);
    fn_804165D8(m_motions[1], 2, -1, -1);
    fn_80417568(m_motions[1], m_owner->m_motionBase + 0x14);
    m_callback = GetCallback();
    fn_80415808(m_owner->m_motion, 0x406, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x407, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x408, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x409, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x410, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x411, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x412, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x413, fn_80335844, &m_callback);
    m_node = fn_80415E70(m_owner->m_motion, 0x3E9);
    fn_800729B0(&m_owner->m_chara->m_commandUnit->m_callback,
                &CharCommandCallback(boost::bind(&DevilDoomCtrlUnit_WingControl::OnCommand, this, _1)));
    void *manager = fn_80021B08();
    m_collider = fn_800C7EA8(manager, &Ref(fn_8040CDD0(&m_owner->m_chara->m_position,
                                                        &Vec3f(0.0f, 35.0f, 0.0f))),
                             0, 4, 0, lbl_805EE698, &lbl_805E07A0, m_owner->m_settings.field_0x104);
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
    }
    for (int i = 0; i < 2; i++) {
        m_gauges[i] = boost::shared_ptr<GiHPGaugeEnemy>(new GiHPGaugeEnemy());
        fn_800BF0B0(m_gauges[i].px, 1.0f);
        fn_800BF038(m_gauges[i].px, 100.0f);
        fn_800BF008(m_gauges[i].px, 500.0f);
    }
    m_open[0] = m_open[1] = 1.0f;
    m_health[0] = m_health[1] = m_owner->m_settings.m_wingTime;
}

/* boost 1.32's reset, which the trimmed shared_ptr.hpp declares out of line: this unit holds the
 * first copy of the instance for GiHPGaugeEnemy. */
template <> inline void boost::shared_ptr<GiHPGaugeEnemy>::reset() {
    this_type().swap(*this);
}

DevilDoomCtrlUnit_WingControl::~DevilDoomCtrlUnit_WingControl() {
    boost::shared_ptr<EffectElement> effect;
    for (int i = 0; i < 2; i++) {
        if (m_motions[i] != 0) {
            fn_8041B884(m_motions[i]);
            m_motions[i] = 0;
        }
        if (m_gauges[i]) {
            m_gauges[i].reset();
        }
        effect = m_effects[i].lock();
        if (effect) {
            effect->Stop();
        }
    }
    if (m_collider) {
        m_collider.reset();
    }
}

int DevilDoomCtrlUnit_WingControl::Tick(float dt) {
    float angle = GetWingAngle();
    float frame = 0.1f * (angle - 10.0f);
    if (m_collider) {
        m_collider.px->SetCharInfo((CharInfo *)(&Ref(
            fn_8040CDD0(&m_owner->m_chara->m_position, &Vec3f(0.0f, 35.0f, 0.0f)))));
    }
    for (int i = 0; i < 2; i++) {
        m_flash[i] -= dt;
        if (m_health[i] > 0.0f) {
            fn_80416E20(m_motions[i], 1, frame);
            if (!m_owner->m_wingsActive) {
                m_flash[i] = 0.0f;
            }
            if (m_flash[i] > 0.0f) {
                float open = m_open[i];
                m_open[i] = open - dt / m_owner->m_settings.field_0x10C;
                if (m_open[i] < 0.1f) {
                    m_open[i] = 0.1f;
                    if (open >= 0.1f) {
                        fn_801D54F4(fn_80053164(), 0x8141, m_owner->m_chara, 0, 1);
                    }
                }
            } else {
                m_open[i] += dt / m_owner->m_settings.field_0x114;
                if (m_open[i] > 1.0f) {
                    m_open[i] = 1.0f;
                }
            }
        } else {
            boost::shared_ptr<EffectElement> effect(m_effects[i].lock());
            if (m_owner->m_hidden && effect) {
                effect->Stop();
            } else if (!m_owner->m_hidden && !effect) {
                m_effects[i] = fn_800A31F0()->Launch(m_owner->m_effects, 0x2B);
                effect = m_effects[i].lock();
                if (effect) {
                    effect->SetMatrix(&m_matrices[i]);
                    effect->SetField14(FrameTime());
                }
            }
        }
        if (m_owner->m_active) {
            if (m_flash[i] > 0.0f) {
                fn_800BF0EC(m_gauges[i].px, 1);
            } else {
                fn_800BF0EC(m_gauges[i].px, 0);
            }
        } else {
            fn_800BF0EC(m_gauges[i].px, 0);
        }
        fn_80416DC8(m_motions[i], 0, lbl_8057D6A0.m_dt);
        fn_804160F8(m_motions[i], 0, lbl_8057D6A0.m_dt);
        fn_80416014(m_motions[i], 0, m_open[i]);
        fn_80415EA4(m_motions[i]);
    }
    return false;
}

void DevilDoomCtrlUnit_WingControl::OnCommand(CharCommand &command) {
    switch (command.TypeId()) {
    case 0x16:
        float minAngle = m_owner->m_settings.m_wingAngle;
        float maxAngle = 180.0f - minAngle;
        float angle = ToDegree(std::fabs(m_owner->m_angleDiff));
        if (angle < minAngle) {
            break;
        }
        if (angle > maxAngle) {
            break;
        }
        float time = m_owner->m_settings.field_0x10C + m_owner->m_settings.field_0x110;
        int side;
        if (m_owner->m_inverted) {
            side = m_owner->m_angleDiff > 0.0f ? 0 : 1;
        } else {
            side = m_owner->m_angleDiff < 0.0f ? 0 : 1;
        }
        m_flash[side] = time;
        break;
    }
    command.Flags() |= 2;
}

void DevilDoomCtrlUnit_WingControl::OnMotion(_stMtnCallbackParam &param) {
    switch (param.m_id) {
    case 0x407: {
        fn_80411634(param.m_frame, m_node, fn_80415E70(m_motions[0], param.m_id));
        Mtx34f *matrix = (Mtx34f *)param.m_frame;
        Vec3f position(matrix->values[0][3], matrix->values[1][3], matrix->values[2][3]);
        fn_800BF068(m_gauges[0].px, &position);
        fn_80016370(&m_matrices[0], param.m_frame);
        break;
    }
    case 0x406:
    case 0x408:
    case 0x409:
        fn_80411634(param.m_frame, m_node, fn_80415E70(m_motions[0], param.m_id));
        break;
    case 0x411: {
        fn_80411634(param.m_frame, m_node, fn_80415E70(m_motions[1], param.m_id));
        Mtx34f *matrix = (Mtx34f *)param.m_frame;
        Vec3f position(matrix->values[0][3], matrix->values[1][3], matrix->values[2][3]);
        fn_800BF068(m_gauges[1].px, &position);
        fn_80016370(&m_matrices[1], param.m_frame);
        break;
    }
    case 0x410:
    case 0x412:
    case 0x413:
        fn_80411634(param.m_frame, m_node, fn_80415E70(m_motions[1], param.m_id));
        break;
    }
}

void DevilDoomCtrlUnit_WingControl::OnDamage(AttackCallbackParam &param) {
    Mtx34f matrix;
    Vec3f offset;
    fn_800091EC(&offset, &Ref(((VectorSubtractFn)fn_8040CD74)(&Param(param).m_position,
                                                              &m_owner->m_chara->m_position)));
    float angle = fn_80016260(-offset.x, -offset.z) - m_owner->m_angle;
    WRAP_ANGLE(angle);
    int side;
    if (m_owner->m_inverted) {
        side = angle > 0.0f ? 0 : 1;
    } else {
        side = angle < 0.0f ? 0 : 1;
    }
    float degrees = ToDegree(std::fabs(angle));
    float wingAngle = m_owner->m_settings.m_wingAngle;
    if (degrees > wingAngle && degrees < 180.0f - wingAngle && m_health[side] > 0.0f &&
        m_flash[side] > 0.0f) {
        Param(param).m_result = 5;
        Vec3f up(0.0f, 1.0f, 0.0f);
        Vec3f forward;
        fn_800091EC(&forward, &Ref(fn_80016284(&Param(param).m_direction)));
        Vec3f right;
        fn_800091EC(&right, &Ref(fn_8040CFB8(&up, &forward)));
        fn_8040CE74(&right);
        fn_800091D0(&up, &Ref(fn_8040CFB8(&forward, &right)));
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B59C(&matrix, &Param(param).m_position);
        matrix.values[0][0] = right.x;
        matrix.values[1][0] = right.y;
        matrix.values[2][0] = right.z;
        matrix.values[0][1] = up.x;
        matrix.values[1][1] = up.y;
        matrix.values[2][1] = up.z;
        matrix.values[0][2] = forward.x;
        matrix.values[1][2] = forward.y;
        matrix.values[2][2] = forward.z;
        boost::weak_ptr<EffectElement> effect = fn_800A31F0()->Launch(m_owner->m_effects, 0x1F);
        boost::shared_ptr<EffectElement> element(effect.lock());
        if (element) {
            fn_80425E68(element.get(), &matrix);
            element->SetField14(FrameTime());
        }
        m_health[side] -= Param(param).m_damage;
        if (m_health[side] < 0.0f) {
            m_health[side] = 0.0f;
            fn_804165D8(m_motions[side], 0, -1, -1);
            fn_80416148(m_motions[side], 0, 70.0f);
            if (side == 0) {
                fn_80417568(m_motions[0], m_owner->m_motionBase + 0x16);
                fn_801FAF00(GetHintManager(), 0x1A6, 0, 0, 0);
            } else {
                fn_80417568(m_motions[1], m_owner->m_motionBase + 0x13);
                fn_801FAF00(GetHintManager(), 0x1A7, 0, 0, 0);
            }
            UnkMotionInfo_80415D40 info;
            fn_80415D40(m_motions[1 - side], 0, &info);
            fn_80416E20(m_motions[side], 0, info.m_frame);
            fn_801D54F4(fn_80053164(), 0x8140, &Param(param).m_position, 0, 1);
        } else {
            fn_801D54F4(fn_80053164(), 0x813F, &Param(param).m_position, 0, 1);
        }
        fn_800BEFD0(m_gauges[side].px, 32.0f);
        fn_800BF0B0(m_gauges[side].px, m_health[side] / m_owner->m_settings.m_wingTime);
    } else {
        Param(param).m_result = 8;
    }
}

float DevilDoomCtrlUnit_WingControl::GetWingAngle() {
    float angle = m_owner->m_angleDiff;
    if ((m_owner->m_inverted && !m_owner->m_flipped) || (!m_owner->m_inverted && m_owner->m_flipped)) {
        angle -= lbl_805FAD38;
    }
    float x = 0.01f * m_owner->m_settings.field_0x108;
    float radius = 0.01f * m_owner->m_settings.field_0x104;
    float s = 0.01f * m_owner->m_distance * std::sin(angle);
    float c = 0.01f * m_owner->m_distance * std::cos(angle);
    float diff = x - c;
    Vec3f b;
    Vec3f a;
    if (std::fabs(diff) < 0.01f) {
        a.x = fn_8000FEE4(radius * radius - x * x);
        a.y = 0.0f;
        a.z = x;
        fn_800091D0(&b, &a);
        b.x = -b.x;
    } else {
        float qa = 1.0f + (s * s) / (diff * diff);
        float qb = s * (-x * s) / (diff * diff);
        float qc = s * (s * (x * x)) / (diff * diff) - radius * radius;
        float disc = qb * qb - qa * qc;
        a.y = 0.0f;
        a.z = (-qb + fn_8000FEE4(disc)) / qa;
        a.x = (-s * a.z + x * s) / diff;
        b.y = 0.0f;
        b.z = (-qb - fn_8000FEE4(disc)) / qa;
        b.x = (-s * b.z + x * s) / diff;
    }
    Vec3f point(s, 0.0f, c);
    float result;
    if (fn_8040CF50(&Ref(((VectorSubtractFn)fn_8040CD74)(&a, &point))) <
        fn_8040CF50(&Ref(((VectorSubtractFn)fn_8040CD74)(&b, &point)))) {
        result = fn_80016260(a.x, a.z);
    } else {
        result = fn_80016260(b.x, b.z);
    }
    result = ToDegree(std::fabs(result)) + m_owner->m_settings.field_0x100;
    if (m_owner->m_flipped) {
        result = 180.0f - result;
    }
    return result;
}

DevilDoomCtrlUnit_BallControl::DevilDoomCtrlUnit_BallControl(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 2;
    m_motion = fn_8041B908(fn_80012684(fn_800122D4(), 0x16, 6), 1);
    Vec3f position;
    Vec3f rotation;
    rotation.x = rotation.y = rotation.z = position.x = position.y = position.z = 0.0f;
    fn_804173A8(m_motion, &position, &rotation, 0);
    fn_80417568(m_motion, m_owner->m_motionBase + 0x12);
    m_callback = GetCallback();
    fn_80415808(m_owner->m_motion, 0x42E, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x42F, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x430, fn_80335844, &m_callback);
    fn_80415808(m_owner->m_motion, 0x431, fn_80335844, &m_callback);
    m_node = fn_80415E70(m_owner->m_motion, 0x3E8);
}

DevilDoomCtrlUnit_BallControl::~DevilDoomCtrlUnit_BallControl() {
    if (m_motion != 0) {
        fn_8041B884(m_motion);
        m_motion = 0;
    }
}

int DevilDoomCtrlUnit_BallControl::Tick(float dt) {
    fn_80416DC8(m_motion, 0, lbl_8057D6A0.m_dt);
    fn_80415EA4(m_motion);
    return false;
}

void DevilDoomCtrlUnit_BallControl::OnMotion(_stMtnCallbackParam &param) {
    fn_80411634(param.m_frame, m_node, fn_80415E70(m_motion, param.m_id));
}

DevilDoomCtrlUnit_Cloud::DevilDoomCtrlUnit_Cloud(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 2;
    for (int i = 0; i < 3; i++) {
        m_clumps[i] = 0;
        m_motions[i] = 0;
        m_updaters[i] = 0;
        void *data = fn_8001277C(fn_800122D4(), 0x16, i + 0x12);
        if (data != 0) {
            m_clumps[i] = fn_80457090(data);
            int area = fn_80169350(fn_80169364(m_clumps[i]));
            fn_8003A840(area);
            SetClumpWorld(m_clumps[i], lbl_8051D208[area].m_world);
            m_motions[i] = fn_8041B908(fn_80012684(fn_800122D4(), 0x16, i + 0xA), 1);
            if (m_motions[i] != 0) {
                m_updaters[i] = new UnkClumpUpdater_80015A7C(m_clumps[i], m_motions[i]);
                fn_80417568(m_motions[i], i + 0x1E + m_owner->m_motionBase);
            }
        }
    }
}

DevilDoomCtrlUnit_Cloud::~DevilDoomCtrlUnit_Cloud() {
    for (int i = 0; i < 3; i++) {
        if (m_updaters[i] != 0) {
            delete m_updaters[i];
            m_updaters[i] = 0;
        }
        if (m_motions[i] != 0) {
            fn_8041B884(m_motions[i]);
            m_motions[i] = 0;
        }
        if (m_clumps[i] != 0) {
            void *world = fn_8046256C(m_clumps[i]);
            if (world != 0) {
                fn_804623EC(world, m_clumps[i]);
            }
            fn_80457468(m_clumps[i]);
            m_clumps[i] = 0;
        }
    }
}

int DevilDoomCtrlUnit_Cloud::Tick(float dt) {
    for (int i = 0; i < 3; i++) {
        if (m_motions[i] != 0) {
            fn_80416DC8(m_motions[i], 0, lbl_8057D6A0.m_dt);
            fn_80415EA4(m_motions[i]);
            if (m_updaters[i] != 0) {
                fn_800159AC(m_updaters[i]);
            }
        }
    }
    return false;
}

void DevilDoomCtrlUnit_CheckMotionAttribute::OnAttribute(u16 attribute, int value) {
    switch (attribute) {
    case 0x5000:
        m_owner->m_attribute = value;
        break;
    case 0x5001:
        break;
    }
}

DevilDoomCtrlUnit_AdjustTarget::DevilDoomCtrlUnit_AdjustTarget(DevilDoomVarParam *param)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 3;
}

DevilDoomCtrlUnit_AdjustTarget::~DevilDoomCtrlUnit_AdjustTarget() {}

int DevilDoomCtrlUnit_AdjustTarget::Tick(float dt) {
    if (m_owner->m_stopped) {
        return false;
    }
    int i = 0;
    UnkListIter_80179304 it;
    for (it = fn_80179354(fn_8017935C(m_owner->m_colliders));
         it.m_node != fn_801792D8(m_owner->m_colliders) && i < 2; fn_801792E0(&it, 0), i++) {
        if (fn_80179304(&it).get() != 0) {
            boost::weak_ptr<CharaColliAttack> attack = fn_800C42D0(fn_80021B08(), fn_80179304(&it));
            boost::shared_ptr<CharaColliAttack> collision(attack.lock());
            if (collision) {
                if (m_owner->m_eyeOpen[i]) {
                    collision.px->field_0x55 = 1;
                } else {
                    collision.px->field_0x55 = 0;
                }
            }
        }
    }
    i = 0;
    UnkListIter_80179304 target;
    for (target = fn_8017B000(fn_8017B008(m_owner->m_targets));
         target.m_node != fn_8017AF84(m_owner->m_targets) && i < 2; fn_8017AF8C(&target, 0), i++) {
        if (fn_8017AFB0(&target).get() != 0) {
            if (m_owner->m_eyeOpen[i]) {
                fn_8017AFB0(&target)->TurnOn();
            } else {
                fn_8017AFB0(&target)->TurnOff();
            }
        }
    }
    return false;
}

DevilDoomCtrlUnit::DevilDoomCtrlUnit(DevilDoomVarParam *owner) : BossCtrlUnit(m_owner), m_owner(owner) {}

DevilDoomCtrlUnit_DoomsEye::DevilDoomCtrlUnit_DoomsEye(DevilDoomVarParam *param, u16 motion)
    : BossCtrlUnit(param), DevilDoomCtrlUnit(param) {
    m_kind = 3;
    void *data = fn_8001277C(fn_800122D4(), 0x16, 4);
    m_clump = 0;
    if (data != 0) {
        m_clump = fn_80457090(data);
    }
    m_motion = 0;
    if (m_clump != 0) {
        m_motion = fn_8041B908(fn_80012684(fn_800122D4(), 0x16, 9), 1);
    }
    m_updater = 0;
    if (m_motion != 0) {
        Vec3f position = m_owner->m_chara->m_position;
        Vec3f rotation = m_owner->m_chara->m_rotation;
        fn_804173A8(m_motion, &position, &rotation, 0);
        m_updater = new UnkClumpUpdater_80015A7C(m_clump, m_motion);
    }
    fn_80417568(m_motion, motion);
    if (m_owner->m_flipped) {
        fn_80416F28(m_motion, 0, 7, 0);
    }
    m_endFrame = MotionEnd(motion);
}

DevilDoomCtrlUnit::~DevilDoomCtrlUnit() {}

DevilDoomCtrlUnit_DoomsEye::~DevilDoomCtrlUnit_DoomsEye() {
    if (m_updater != 0) {
        delete m_updater;
        m_updater = 0;
    }
    if (m_motion != 0) {
        fn_8041B884(m_motion);
        m_motion = 0;
    }
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
    }
}

int DevilDoomCtrlUnit_DoomsEye::Tick(float dt) {
    if (fn_8046256C(m_clump) == 0) {
        UnkMotionInfo_80415D40 info;
        fn_80415D40(m_owner->m_motion, 0, &info);
        if (info.m_frame < m_endFrame) {
            return false;
        }
        int area = fn_80169350(fn_80169364(m_clump));
        fn_8003A840(area);
        SetClumpWorld(m_clump, lbl_8051D208[area].m_world);
        if (info.m_frame > m_endFrame) {
            fn_80416E20(m_motion, 0, info.m_frame);
        }
        fn_801D54F4(fn_80053164(), 0x8143, m_owner->m_chara, 0, 1);
    }
    fn_80416DC8(m_motion, 0, lbl_8057D6A0.m_dt);
    fn_804160F8(m_motion, 0, lbl_8057D6A0.m_dt);
    fn_80415EA4(m_motion);
    if (m_updater != 0) {
        fn_800159AC(m_updater);
    }
    if (fn_80416F28(m_motion, 0, 3, 1) != 0) {
        return true;
    }
    return false;
}

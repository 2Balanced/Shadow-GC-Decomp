#ifndef GAME_BOSSCTRLUNIT_H
#define GAME_BOSSCTRLUNIT_H

/* The base classes of the bosses' control units (class names from RTTI). A boss task keeps a
 * BossVarParam with the boss's state and runs a list of BossCtrlUnit objects on it every frame.
 * The units of one boss (DevilDoomCtrlUnit_*, BlackBullCtrlUnit_*, ...) derive virtually from
 * BossCtrlUnit, so each object holds one BossCtrlUnit at its end; CodeWarrior then gives each
 * class a pointer to that base first, and its vtable pointer second. The callback containers
 * (AttackCallbackContainer, MotionCallbackContainer) hand out boost::function objects bound to
 * their virtual functions. Virtual function and member names are inferred. */

#include <boost/function.hpp>

#include "Game/CharaColliAttack.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct _stMtnPlayWork;
struct RpClump;
struct UnkClumpFade_80027D5C;
struct _stMtnCallbackParam;

template <class T>
class PooledList; /* Game/PooledList.h */
class BossCtrlUnit;
class Target;
class BossCharInfo;
struct UnkBossDef;
namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliBase;
} // namespace CharaColli
namespace Effect {
class EffectLaunchInfoUSA;
} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

#ifdef BOSSVARPARAM_CHARA_SHARED_PTR
/* Defined by the unit that emits BossVarParam's constructor (Game/BlackBull0210.cpp): the members
 * as the constructor sees them. m_chara is a shared_ptr<BossCharInfo>, which the other units
 * read through its pointer, and the vector at 0x48 is copy-constructed out of line (fn_800091EC is
 * Vec3f's copy constructor). */
struct BossVarParamVector {
    BossVarParamVector(const Vec3f &value) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &value);
    }

    float x;
    float y;
    float z;
};
#endif

/* BossVarParam (class name from RTTI; vtable at 0x80547494, constructor 0x802332D4, destructor
 * 0x80232440): the state of a boss that its control units read and change. Only the members the
 * units use are mapped; names and meanings are inferred. The constructor and destructor set up
 * and release the two smart pointers at 0x1C and 0x6C. */
class BossVarParam {
  public:
    BossVarParam();
    virtual ~BossVarParam();

    /* 0x04 */ UnkBossDef *m_def; /* the boss's collision and target tables */
    /* 0x08 */ float m_maxHealth;
    /* 0x0C */ s32 m_mode;     /* the running mode (5: the mode the interrupt check leaves alone) */
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ s32 m_nextMode; /* a mode change the units request */
    /* 0x18 */ u8 m_stopped;   /* the units pause while set */
    /* 0x19 */ u8 m_invincible;
    /* 0x1A */ u8 m_active;
    /* 0x1B */ u8 m_hidden;
#ifdef BOSSVARPARAM_CHARA_SHARED_PTR
    /* 0x1C */ boost::shared_ptr<BossCharInfo> m_charaPtr; /* m_chara, as the original declares it */
#else
    /* 0x1C */ class EnemyCharInfo *m_chara;
    /* 0x20 */ u32 field_0x20;
#endif
    /* 0x24 */ float m_health;
    /* 0x28 */ float m_time;
    /* 0x2C */ RpClump *m_clump; /* the boss's model (BossMotion attaches its motion to it) */
    /* 0x30 */ UnkClumpFade_80027D5C *m_fade; /* the model's fade (BossRender) */
    /* 0x34 */ u32 field_0x34;
    /* 0x38 */ float m_alpha; /* the model's opacity, 0..1 (BossRender) */
    /* 0x3C */ _stMtnPlayWork *m_motion;
    /* 0x40 */ float field_0x40;
    /* 0x44 */ s32 m_motionBase; /* the first of the boss's motion ids */
#ifdef BOSSVARPARAM_CHARA_SHARED_PTR
    /* 0x48 */ BossVarParamVector field_0x48;
#else
    /* 0x48 */ Vec3f field_0x48;
#endif
    /* 0x54 */ PooledList<SonicteamUSA::System::CharaColli::CharaColliBase> *m_colliders; /* BossCharaColli */
    /* 0x58 */ Vec3f field_0x58;
    /* 0x64 */ PooledList<Target> *m_targets;     /* BossTarget */
    /* 0x68 */ PooledList<BossCtrlUnit> *m_units; /* the units BossCtrl runs */
    /* The boss's effects: a shared_ptr<EffectLaunchInfoUSA>, the type of Game/GadgetStageData.cpp's
     * s_stageEffects (constructor 0x80168E54, destructor 0x80168E04). Launching an effect converts
     * it to the shared_ptr<EffectLaunchInfo> the effect manager takes (a local converting
     * constructor in each unit; that type's destructor is 0x80235B00). */
    /* 0x6C */ boost::shared_ptr<SonicteamUSA::System::Effect::EffectLaunchInfoUSA> m_effects;
};

/* BossCtrlUnit (class name from RTTI; vtable 0x80547210): one piece of a boss's behaviour, run by
 * the boss's BossVarParam. Tick runs it every frame (the return value is unused by the callers
 * seen so far). The mode and control units of a boss derive from it virtually, which gives it a
 * hidden fourth word (at 0x0C) that CodeWarrior uses to adjust this in the virtual-base thunks.
 * The fields' meanings are inferred: m_kind is set to 1 by BossModeCtrlUnit and 3 by
 * DevilDoomCtrlUnit_DoomsEye. */
class BossCtrlUnit {
  public:
    BossCtrlUnit(BossVarParam *owner) : m_kind(0), m_owner(owner) {}
    virtual ~BossCtrlUnit() {}
    virtual int Tick(float dt) = 0;

    /* 0x04 */ s32 m_kind;
    /* 0x08 */ BossVarParam *m_owner;
};

/* BossModeCtrlUnit (class name from RTTI; vtable 0x805525A8): the unit that runs a boss's current
 * mode. Tick (0x802925A0) runs the mode and, when the BossVarParam asks for another one, deletes
 * it and calls CreateMode for the next (names inferred). */
class BossModeCtrlUnit : public virtual BossCtrlUnit {
  public:
    BossModeCtrlUnit(BossVarParam *owner);
    virtual ~BossModeCtrlUnit();
    virtual int Tick(float dt);
    virtual BossCtrlUnit *CreateMode(int mode) = 0;

    /* 0x08 */ BossCtrlUnit *m_mode;
};

/* BossCtrlUnit_PlayCameraMotion (class name from RTTI; vtable 0x80552570): plays a camera motion
 * of a boss, at a given position and rotation or at the boss's own (names inferred). */
class BossCtrlUnit_PlayCameraMotion : public BossCtrlUnit {
  public:
    BossCtrlUnit_PlayCameraMotion(BossVarParam *owner, int motion, const Vec3f *position,
                                  const Vec3f *rotation, float arg4, float arg5);
    BossCtrlUnit_PlayCameraMotion(BossVarParam *owner, int motion, float arg2, float arg3);
    virtual ~BossCtrlUnit_PlayCameraMotion();
    virtual int Tick(float dt);

    /* 0x0C */ u8 field_0xC[0x54 - 0xC];
};

/* BossCtrlUnit_DamageEff (class name from RTTI; vtable at 0x8055258C): the damage effect a boss
 * shows when hit. Its constructor is at 0x80292528. */
class BossCtrlUnit_DamageEff : public BossCtrlUnit {
  public:
    BossCtrlUnit_DamageEff(BossVarParam *param);
    virtual ~BossCtrlUnit_DamageEff();
    virtual int Tick(float dt);

    /* 0x0C */ u8 field_0xC[0x14 - 0xC];
};

/* BossCtrlUnit_CheckEvent (class name from RTTI; vtable at 0x805524DC, constructor 0x80290F98):
 * a unit of kind 2 (meaning unknown). */
class BossCtrlUnit_CheckEvent : public BossCtrlUnit {
  public:
    BossCtrlUnit_CheckEvent(BossVarParam *param);
    virtual ~BossCtrlUnit_CheckEvent();
    virtual int Tick(float dt);
};

/* BossCtrlUnit_DispGauge (class name from RTTI; vtable at 0x805524F8, constructor 0x80291090):
 * shows the boss's health gauge (meaning inferred from the name). */
class BossCtrlUnit_DispGauge : public virtual BossCtrlUnit {
  public:
    BossCtrlUnit_DispGauge(BossVarParam *param);
    virtual ~BossCtrlUnit_DispGauge();
    virtual int Tick(float dt);
};

/* BossCtrlUnit_CheckFirstDamage (class name from RTTI; vtable at 0x80552524, constructor
 * 0x802911DC): the constructor stores its two arguments at 0xC and 0x10 (meanings unknown). */
class BossCtrlUnit_CheckFirstDamage : public BossCtrlUnit {
  public:
    BossCtrlUnit_CheckFirstDamage(BossVarParam *param, int arg1, int arg2);
    virtual ~BossCtrlUnit_CheckFirstDamage();
    virtual int Tick(float dt);

    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ s32 field_0x10;
};

/* BossCtrlUnit_Shadow (class name from RTTI; vtable at 0x80552474, constructor 0x802904D8): the
 * boss's shadow (meaning inferred from the name; size from its allocation). */
class BossCtrlUnit_Shadow : public BossCtrlUnit {
  public:
    BossCtrlUnit_Shadow(BossVarParam *param, float size);
    virtual ~BossCtrlUnit_Shadow();
    virtual int Tick(float dt);

    /* 0x0C */ u8 field_0xC[0x38 - 0xC];
};

/* BossCtrlUnit_StageObject (class name from RTTI; vtable at 0x80552490, constructor 0x80290920):
 * an object of the boss's stage at position, turned by rotation; the meanings of the other
 * arguments are unknown. */
class BossCtrlUnit_StageObject : public BossCtrlUnit {
  public:
    BossCtrlUnit_StageObject(BossVarParam *param, const Vec3f *position, const Vec3f *rotation,
                             int arg3, int arg4, const char *arg5, float radius);
    virtual ~BossCtrlUnit_StageObject();
    virtual int Tick(float dt);

    /* 0x0C */ u8 field_0xC[0x28 - 0xC];
};

/* Adds unit to the units the boss runs (behaviour inferred). */
extern "C" void fn_803358D0(BossVarParam *param, BossCtrlUnit *unit);

/* A motion callback (the class name is unknown): it holds a boost::function (a member: its
 * destructor destroys it as one), and its constructor, destructor and assignment from a
 * boost::function are functions of their own at 0x80241858, 0x802414D4 and
 * 0x802416D0. */
typedef boost::function1<void, _stMtnCallbackParam &> MotionCallbackFunction;

class UnkMotionCallback_802414D4 {
  public:
    UnkMotionCallback_802414D4 &operator=(const MotionCallbackFunction &f);

    /* 0x0 */ MotionCallbackFunction m_function;
};

/* MotionCallbackContainer (class name from RTTI; vtable at 0x8055D38C): receives the callbacks of
 * a motion. GetCallback binds OnMotion to this object. */
class MotionCallbackContainer {
  public:
    virtual void OnMotion(_stMtnCallbackParam &param);
    virtual MotionCallbackFunction GetCallback();
};

/* AttackCallbackContainer (class name from RTTI; vtable at 0x8055D348): receives an attack
 * collision's callbacks; the Get functions bind the three handlers to this object. */
class AttackCallbackContainer {
  public:
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnEnd(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual SonicteamUSA::System::CharaColli::AttackCallback GetHitCallback();
    virtual SonicteamUSA::System::CharaColli::AttackCallback GetEndCallback();
    virtual SonicteamUSA::System::CharaColli::AttackCallback GetDamageCallback();
};

/* BossCtrlUnit_CheckMotionAttribute (class name from RTTI; vtable at 0x805524AC): passes each
 * attribute of the boss's motion to OnAttribute. */
class BossCtrlUnit_CheckMotionAttribute : public virtual BossCtrlUnit {
  public:
    BossCtrlUnit_CheckMotionAttribute(BossVarParam *param);
    virtual ~BossCtrlUnit_CheckMotionAttribute();
    virtual int Tick(float dt);
    virtual void OnAttribute(u16 attribute, int value) = 0;
};

/* BossCtrlUnit_FixLookCharCamera (class name from RTTI; vtable at 0x80552540): keeps the camera
 * on the player and the boss. The third virtual function says whether it may (meaning
 * inferred). */
class BossCtrlUnit_FixLookCharCamera : public virtual BossCtrlUnit {
  public:
    BossCtrlUnit_FixLookCharCamera(BossVarParam *param);
    virtual ~BossCtrlUnit_FixLookCharCamera();
    virtual int Tick(float dt);
    virtual bool IsEnabled();

    /* 0x08 */ float field_0x8;
    /* 0x0C */ float field_0xC;
    /* 0x10 */ u8 field_0x10[0x18 - 0x10];
    /* 0x18 */ Vec3f m_offset;
    /* 0x24 */ u8 field_0x24[0x30 - 0x24];
};

#endif

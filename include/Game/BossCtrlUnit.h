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
struct _stMtnCallbackParam;

struct UnkList_80179304;
struct UnkBossDef;
namespace Effect {
namespace System {
class EffectLaunchInfo;
} // namespace System
} // namespace Effect

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
    /* 0x1C */ class EnemyCharInfo *m_chara;
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ float m_health;
    /* 0x28 */ float m_time;
    /* 0x2C */ u8 field_0x2C[0x3C - 0x2C];
    /* 0x3C */ _stMtnPlayWork *m_motion;
    /* 0x40 */ u32 field_0x40;
    /* 0x44 */ s32 m_motionBase; /* the first of the boss's motion ids */
    /* 0x48 */ Vec3f field_0x48;
    /* 0x54 */ UnkList_80179304 *m_colliders;
    /* 0x58 */ u8 field_0x58[0x64 - 0x58];
    /* 0x64 */ UnkList_80179304 *m_targets;
    /* 0x68 */ u32 field_0x68;
    /* 0x6C */ boost::shared_ptr<Effect::System::EffectLaunchInfo> m_effects;
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

/* Adds unit to the units the boss runs (behaviour inferred). */
extern "C" void fn_803358D0(BossVarParam *param, BossCtrlUnit *unit);

/* A motion callback (the class name is unknown): a boost::function whose constructor, destructor
 * and assignment from a boost::function are functions of their own at 0x80241858, 0x802414D4 and
 * 0x802416D0. */
typedef boost::function1<void, _stMtnCallbackParam &> MotionCallbackFunction;

class UnkMotionCallback_802414D4 : public MotionCallbackFunction {
  public:
    UnkMotionCallback_802414D4 &operator=(const MotionCallbackFunction &f);
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

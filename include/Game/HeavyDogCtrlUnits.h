#ifndef GAME_HEAVYDOGCTRLUNITS_H
#define GAME_HEAVYDOGCTRLUNITS_H

/* The control units both Heavy Dog stages use (class names from RTTI: HeavyDogCtrlUnit_*), defined
 * in the unit at 0x802498AC (Game/HeavyDogCtrlUnit.cpp); the boss tasks and mode units create
 * them. The constructors are out of line, except those of the collider callbacks and the motion
 * attribute unit, which are inline and emitted by their users. Needs the full HeavyDogVarParam
 * layout (HEAVYDOGVARPARAM_FULL). Member names and meanings are inferred. */

#include "Game/BossCtrlUnit.h"
#include "Game/HeavyDogCtrlUnit.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct RpClump;
struct RwFrame;
class Target;
namespace Effect {
namespace System {
class EffectElement;
} // namespace System
} // namespace Effect
namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliBase;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA
namespace Vibration {
class RequestHandle;
} // namespace Vibration

/* HeavyDogCtrlUnit_DestroyEffect (vtable at 0x80548C3C): the smoke of the destroyed boss, which
 * follows its body. */
class HeavyDogCtrlUnit_DestroyEffect : public HeavyDogCtrlUnit {
  public:
    HeavyDogCtrlUnit_DestroyEffect(HeavyDogVarParam *param);
    virtual ~HeavyDogCtrlUnit_DestroyEffect();
    virtual int Tick(float dt);

    /* 0x0C */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x14 */ Vec3f m_position;
    /* 0x20 */ Vec3f m_rotation;
};

/* HeavyDogCtrlUnit_AttackCallback (vtable at 0x80548C78): receives the callbacks of one of the
 * boss's attack colliders (index picks the collider). */
class HeavyDogCtrlUnit_AttackCallback : public HeavyDogCtrlUnit, public AttackCallbackContainer {
  public:
    HeavyDogCtrlUnit_AttackCallback(HeavyDogVarParam *param, int index);
    virtual ~HeavyDogCtrlUnit_AttackCallback() {}
    virtual int Tick(float dt) {
        return false;
    }
};

/* HeavyDogCtrlUnit_DispEnergy (vtable at 0x80548D3C): the energy gauge model on the boss. */
class HeavyDogCtrlUnit_DispEnergy : public HeavyDogCtrlUnit {
  public:
    HeavyDogCtrlUnit_DispEnergy(HeavyDogVarParam *param, int model);
    virtual ~HeavyDogCtrlUnit_DispEnergy();
    virtual int Tick(float dt);

    /* 0x0C */ RpClump *m_clump;
    /* 0x10 */ RwFrame *m_frame;
    /* 0x14 */ s32 m_state; /* 0 full .. 4 empty */
    /* 0x18 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
};

/* HeavyDogCtrlUnit_DispCanopy (vtable at 0x80548D70): the canopy model on the boss. */
class HeavyDogCtrlUnit_DispCanopy : public HeavyDogCtrlUnit {
  public:
    HeavyDogCtrlUnit_DispCanopy(HeavyDogVarParam *param, int model);
    virtual ~HeavyDogCtrlUnit_DispCanopy();
    virtual int Tick(float dt);

    /* 0x0C */ RpClump *m_clump;
    /* 0x10 */ RwFrame *m_frame;
};

/* HeavyDogCtrlUnit_CheckMotionAttribute (vtable at 0x80548DB8): keeps the motion's attribute
 * 0x5000 in the HeavyDogVarParam. */
class HeavyDogCtrlUnit_CheckMotionAttribute : public HeavyDogCtrlUnit,
                                              public BossCtrlUnit_CheckMotionAttribute {
  public:
    HeavyDogCtrlUnit_CheckMotionAttribute(HeavyDogVarParam *param)
        : BossCtrlUnit(param), HeavyDogCtrlUnit(param), BossCtrlUnit_CheckMotionAttribute(param) {}
    virtual ~HeavyDogCtrlUnit_CheckMotionAttribute() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_CheckMotionAttribute::Tick(dt);
    }
    virtual void OnAttribute(u16 attribute, int value);
};

/* HeavyDogCtrlUnit_CheckLauncherNum (vtable at 0x80548E04): plays sound when a launcher is
 * added. */
class HeavyDogCtrlUnit_CheckLauncherNum : public HeavyDogCtrlUnit {
  public:
    HeavyDogCtrlUnit_CheckLauncherNum(HeavyDogVarParam *param, int sound);
    virtual ~HeavyDogCtrlUnit_CheckLauncherNum();
    virtual int Tick(float dt);

    /* 0x0C */ s32 m_sound;
    /* 0x10 */ s32 m_count;
};

/* HeavyDogCtrlUnit_Barrier (vtable at 0x80548E40): an electric barrier around position that grows
 * and hits the player when close (behaviour inferred). */
class HeavyDogCtrlUnit_Barrier : public HeavyDogCtrlUnit, public AttackCallbackContainer {
  public:
    HeavyDogCtrlUnit_Barrier(HeavyDogVarParam *param, const Vec3f *position, bool effect);
    virtual ~HeavyDogCtrlUnit_Barrier();
    virtual int Tick(float dt);
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* 0x10 */ u8 m_effectShown;
    /* 0x14 */ s32 m_state;
    /* 0x18 */ float m_time;
    /* 0x1C */ float m_radius;
    /* 0x20 */ Vec3f m_position;
    /* 0x2C */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x34 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x3C */ boost::weak_ptr<Vibration::RequestHandle> m_vibration;
};

/* HeavyDogCtrlUnit_Engine (vtable at 0x80548E98): the boss's engines; motion callbacks place the
 * exhaust effects and tilt the engines with the boss's movement. */
class HeavyDogCtrlUnit_Engine : public HeavyDogCtrlUnit {
  public:
    HeavyDogCtrlUnit_Engine(HeavyDogVarParam *param, int motion);
    virtual ~HeavyDogCtrlUnit_Engine();
    virtual int Tick(float dt);

    void OnBody(_stMtnCallbackParam &param);
    void OnEngine(_stMtnCallbackParam &param);
    void OnExhaust(_stMtnCallbackParam &param);
    void OnNozzle(_stMtnCallbackParam &param);

    /* 0x0C */ s32 m_motion; /* the first node of the callbacks */
    /* 0x10 */ u8 m_reverse;
    /* 0x14 */ Vec3f m_position;
    /* 0x20 */ float m_dt;
    /* 0x24 */ float m_tilt;
    /* 0x28 */ float m_tiltSpeed;
    /* 0x2C */ boost::weak_ptr<Effect::System::EffectElement> m_effects[5];
    /* 0x54 */ UnkMotionCallback_802414D4 m_callback0;
    /* 0x60 */ UnkMotionCallback_802414D4 m_callback1;
    /* 0x6C */ UnkMotionCallback_802414D4 m_callback2;
    /* 0x78 */ UnkMotionCallback_802414D4 m_callback3;
};

/* HeavyDogCtrlUnit_RocketLauncher (vtable at 0x80548EDC): one of the rocket launchers on the
 * boss's back, at the motion node motion. */
class HeavyDogCtrlUnit_RocketLauncher : public HeavyDogCtrlUnit,
                                        public AttackCallbackContainer,
                                        public MotionCallbackContainer {
  public:
    HeavyDogCtrlUnit_RocketLauncher(HeavyDogVarParam *param, int motion);
    virtual ~HeavyDogCtrlUnit_RocketLauncher();
    virtual int Tick(float dt);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnMotion(_stMtnCallbackParam &param);

    /* 0x14 */ float m_dt;
    /* 0x18 */ s32 m_index; /* in HeavyDogVarParam::m_launchers; -1 without one */
    /* 0x1C */ s32 m_motion;
    /* 0x20 */ float m_reload;
    /* 0x24 */ float m_turn;
    /* 0x28 */ u8 m_loaded;
    /* 0x29 */ u8 m_aimed;
    /* 0x2A */ u8 m_aiming;
    /* 0x2C */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x34 */ boost::shared_ptr<Target> m_target;
    /* 0x3C */ UnkMotionCallback_802414D4 m_callback;
    /* 0x48 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
};

/* HeavyDogCtrlUnit_Rotate (vtable at 0x80548F48): turns the boss. */
class HeavyDogCtrlUnit_Rotate : public HeavyDogCtrlUnit {
  public:
    HeavyDogCtrlUnit_Rotate(HeavyDogVarParam *param);
    virtual ~HeavyDogCtrlUnit_Rotate();
    virtual int Tick(float dt);
};

/* The collider callbacks of the canopy and the body (vtables at 0x80548F8C and 0x80548FF8). */
class HeavyDogCtrlUnit_AttackCallbackCanopy : public HeavyDogCtrlUnit_AttackCallback {
  public:
    HeavyDogCtrlUnit_AttackCallbackCanopy(HeavyDogVarParam *param, int index)
        : BossCtrlUnit(param), HeavyDogCtrlUnit_AttackCallback(param, index) {}
    virtual ~HeavyDogCtrlUnit_AttackCallbackCanopy() {}
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

class HeavyDogCtrlUnit_AttackCallbackBody : public HeavyDogCtrlUnit_AttackCallback {
  public:
    HeavyDogCtrlUnit_AttackCallbackBody(HeavyDogVarParam *param, int index)
        : BossCtrlUnit(param), HeavyDogCtrlUnit_AttackCallback(param, index) {}
    virtual ~HeavyDogCtrlUnit_AttackCallbackBody() {}
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

/* HeavyDogCtrlUnit_GetStatus (vtable at 0x80549054): follows the player. */
class HeavyDogCtrlUnit_GetStatus : public HeavyDogCtrlUnit {
  public:
    HeavyDogCtrlUnit_GetStatus(HeavyDogVarParam *param);
    virtual ~HeavyDogCtrlUnit_GetStatus();
    virtual int Tick(float dt);
};

SIZE_ASSERT(HeavyDogCtrlUnit_DestroyEffect, 0x3C);
SIZE_ASSERT(HeavyDogCtrlUnit_AttackCallback, 0x20);
SIZE_ASSERT(HeavyDogCtrlUnit_DispEnergy, 0x30);
SIZE_ASSERT(HeavyDogCtrlUnit_DispCanopy, 0x24);
SIZE_ASSERT(HeavyDogCtrlUnit_CheckMotionAttribute, 0x24);
SIZE_ASSERT(HeavyDogCtrlUnit_CheckLauncherNum, 0x24);
SIZE_ASSERT(HeavyDogCtrlUnit_Barrier, 0x54);
SIZE_ASSERT(HeavyDogCtrlUnit_Engine, 0x94);
SIZE_ASSERT(HeavyDogCtrlUnit_RocketLauncher, 0x60);
SIZE_ASSERT(HeavyDogCtrlUnit_Rotate, 0x1C);
SIZE_ASSERT(HeavyDogCtrlUnit_GetStatus, 0x1C);

#endif

#ifndef GAME_DEVILDOOMCTRLUNIT_H
#define GAME_DEVILDOOMCTRLUNIT_H

/* The control units of the Devil Doom boss (class names from RTTI), defined in
 * Game/DevilDoomCtrlUnit.cpp; the boss task (Game/DevilDoom.cpp) creates them on its
 * DevilDoomVarParam. Member names and meanings are inferred. */

#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/CharCommand.h"
#include "Game/GiHPGaugeEnemy.h"
#include "Game/MathTypes.h"
#include "Game/UnkMatrix_800B1304.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct RpClump;
class Target;

namespace Effect {
namespace System {
class EffectLaunchInfo;
} // namespace System
} // namespace Effect

/* The boss's settings (struct name inferred), at 0x74 in DevilDoomVarParam: constants that its
 * out-of-line constructor (0x8030C86C) sets. Offsets are relative to the struct. */
struct DevilDoomSettings {
    DevilDoomSettings();

    /* 0x000 */ float field_0x0;
    /* 0x004 */ float field_0x4; /* a speed (DevilDoomModeCtrlUnit) */
    /* 0x008 */ s32 m_score;
    /* 0x00C */ float field_0xC;
    /* 0x010 */ float field_0x10; /* an angle in degrees */
    /* 0x014 */ float m_eyeSpeed;
    /* 0x018 */ float m_eyeOpenSpeed;
    /* 0x01C */ float m_neckSpeed;
    /* 0x020 */ float field_0x20; /* a time */
    /* 0x024 */ float m_minDistance;
    /* 0x028 */ float m_maxDistance;
    /* 0x02C */ float field_0x2C;
    /* 0x030 */ float field_0x30;
    /* 0x034 */ float field_0x34;
    /* 0x038 */ float field_0x38;
    /* 0x03C */ s32 field_0x3C;
    /* 0x040 */ float field_0x40;
    /* 0x044 */ float field_0x44;
    /* 0x048 */ float field_0x48;
    /* 0x04C */ float field_0x4C;
    /* 0x050 */ float field_0x50;
    /* 0x054 */ float field_0x54;
    /* 0x058 */ float field_0x58;
    /* 0x05C */ float field_0x5C;
    /* 0x060 */ float field_0x60;
    /* 0x064 */ float field_0x64;
    /* 0x068 */ float field_0x68;
    /* 0x06C */ float field_0x6C;
    /* 0x070 */ float field_0x70;
    /* 0x074 */ float field_0x74;
    /* 0x078 */ float field_0x78;
    /* 0x07C */ float field_0x7C;
    /* 0x080 */ float field_0x80;
    /* 0x084 */ float field_0x84;
    /* 0x088 */ s32 field_0x88;
    /* 0x08C */ float field_0x8C;
    /* 0x090 */ float field_0x90;
    /* 0x094 */ float field_0x94;
    /* 0x098 */ float field_0x98;
    /* 0x09C */ float field_0x9C;
    /* 0x0A0 */ float field_0xA0;
    /* 0x0A4 */ float field_0xA4;
    /* 0x0A8 */ float field_0xA8;
    /* 0x0AC */ float field_0xAC;
    /* 0x0B0 */ float field_0xB0;
    /* 0x0B4 */ float field_0xB4;
    /* 0x0B8 */ float field_0xB8;
    /* 0x0BC */ float field_0xBC;
    /* 0x0C0 */ float field_0xC0;
    /* 0x0C4 */ float field_0xC4;
    /* 0x0C8 */ float field_0xC8;
    /* 0x0CC */ float field_0xCC;
    /* 0x0D0 */ float field_0xD0;
    /* 0x0D4 */ float field_0xD4;
    /* 0x0D8 */ float field_0xD8;
    /* 0x0DC */ float field_0xDC;
    /* 0x0E0 */ float field_0xE0;
    /* 0x0E4 */ s32 field_0xE4;
    /* 0x0E8 */ u32 field_0xE8; /* not set */
    /* 0x0EC */ float field_0xEC;
    /* 0x0F0 */ float field_0xF0;
    /* 0x0F4 */ float field_0xF4;
    /* 0x0F8 */ float m_wingAngle;
    /* 0x0FC */ float m_wingTime;
    /* 0x100 */ float field_0x100;
    /* 0x104 */ float field_0x104;
    /* 0x108 */ float field_0x108;
    /* 0x10C */ float field_0x10C;
    /* 0x110 */ float field_0x110;
    /* 0x114 */ float field_0x114;
};
SIZE_ASSERT(DevilDoomSettings, 0x118);

/* An element of the parts below (struct name unknown): its empty constructor is out of line at
 * 0x8030C868. Each is the place of one of the boss's bits in its ring (member names inferred;
 * see Game/DevilDoomCtrlUnitBit.cpp). The vectors are plain floats, as Vec3f's empty
 * constructor would give the struct a constructor of its own. */
struct UnkDevilDoomPartElement {
    UnkDevilDoomPartElement() {}

    /* 0x00 */ float m_position[3]; /* in the ring */
    /* 0x0C */ float m_rotation[3];
    /* 0x18 */ float m_blend; /* from the bit's previous place */
    /* 0x1C */ u8 m_locked;   /* the bit may not attack */
    /* 0x1D */ u8 m_done;     /* the bit has attacked */
    /* 0x1E */ u8 m_alive;
    /* 0x1F */ u8 m_free;
};
SIZE_ASSERT(UnkDevilDoomPartElement, 0x20);

/* One of the three parts at 0x214 in DevilDoomVarParam (struct name unknown): the rings that
 * carry the boss's bits (member names inferred; see Game/DevilDoomCtrlUnitBit.cpp). */
struct UnkDevilDoomPart {
    /* 0x000 */ float m_position[3];
    /* 0x00C */ float m_rotation[3];
    /* 0x018 */ float m_scale[3];
    /* 0x024 */ Mtx34f m_matrix;
    /* 0x054 */ float m_blend;
    /* 0x058 */ u8 m_closed;
    /* 0x05C */ UnkDevilDoomPartElement m_places[8];
};
SIZE_ASSERT(UnkDevilDoomPart, 0x15C);

/* DevilDoomVarParam (class name from RTTI; vtable at 0x8055B808): the state of the Devil Doom
 * boss, at 0x38 in the boss task (Game/DevilDoom.cpp). */
class DevilDoomVarParam : public BossVarParam {
  public:
    DevilDoomVarParam();
    virtual ~DevilDoomVarParam();

    /* 0x074 */ DevilDoomSettings m_settings;
    /* 0x18C */ u8 m_fixed;
    /* 0x18D */ u8 field_0x18D;
    /* 0x18E */ u8 field_0x18E;
    /* 0x18F */ bool m_inverted;
    /* 0x190 */ bool m_flipped;
    /* 0x191 */ u8 m_eyeOpen[2];
    /* 0x193 */ u8 field_0x193;
    /* 0x194 */ u8 m_eyeClosing;
    /* 0x195 */ u8 m_neckUp;
    /* 0x196 */ u8 field_0x196;
    /* 0x197 */ u8 field_0x197;
    /* 0x198 */ u8 field_0x198;
    /* 0x199 */ u8 field_0x199;
    /* 0x19A */ u8 m_wingsActive;
    /* 0x19B */ u8 field_0x19B;
    /* 0x19C */ s32 field_0x19C;
    /* 0x1A0 */ s32 field_0x1A0;
    /* 0x1A4 */ float field_0x1A4;
    /* 0x1A8 */ float field_0x1A8;
    /* 0x1AC */ float m_healthRatio;
    /* 0x1B0 */ s32 m_healthLevel;
    /* 0x1B4 */ float m_timeRatio;
    /* 0x1B8 */ s32 m_timeLevel;
    /* 0x1BC */ Vec3f field_0x1BC;
    /* 0x1C8 */ Vec3f m_playerPosition;
    /* 0x1D4 */ Vec3f m_target;
    /* 0x1E0 */ Vec3f m_prevTarget;
    /* 0x1EC */ float m_angle;
    /* 0x1F0 */ float m_targetAngle;
    /* 0x1F4 */ float m_distance;
    /* 0x1F8 */ float m_angleDiff;
    /* 0x1FC */ float m_angleLeft;
    /* 0x200 */ float m_turnSpeed;
    /* 0x204 */ float m_eyeAngle[2];
    /* 0x20C */ s32 field_0x20C;
    /* 0x210 */ s32 field_0x210;
    /* 0x214 */ UnkDevilDoomPart m_parts[3];
    /* 0x628 */ s32 field_0x628;
    /* 0x62C */ float field_0x62C;
    /* 0x630 */ float field_0x630;
    /* 0x634 */ u32 m_attribute;
};
SIZE_ASSERT(DevilDoomVarParam, 0x638);

/* DevilDoomCtrlUnit (class name from RTTI; vtable at 0x8055B148): the base of the boss's units. */
class DevilDoomCtrlUnit : public virtual BossCtrlUnit {
  public:
    DevilDoomCtrlUnit(DevilDoomVarParam *owner);
    virtual ~DevilDoomCtrlUnit();

    /* 0x8 */ DevilDoomVarParam *m_owner;
};

/* An object that keeps a clump in a world (class name unknown; size from its allocation). */
class UnkClumpUpdater_80015A7C {
  public:
    UnkClumpUpdater_80015A7C(RpClump *clump, _stMtnPlayWork *motion);
    ~UnkClumpUpdater_80015A7C();

    /* 0x00 */ u8 field_0x0[0x1C];
};

class DevilDoomCtrlUnit_DoomsEye : public DevilDoomCtrlUnit {
  public:
    DevilDoomCtrlUnit_DoomsEye(DevilDoomVarParam *param, u16 motion);
    virtual ~DevilDoomCtrlUnit_DoomsEye();
    virtual int Tick(float dt);

    /* 0x0C */ _stMtnPlayWork *m_motion;
    /* 0x10 */ RpClump *m_clump;
    /* 0x14 */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x18 */ float m_endFrame;
};

class DevilDoomCtrlUnit_AdjustTarget : public DevilDoomCtrlUnit {
  public:
    DevilDoomCtrlUnit_AdjustTarget(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_AdjustTarget();
    virtual int Tick(float dt);
};

class DevilDoomCtrlUnit_CheckMotionAttribute : public DevilDoomCtrlUnit,
                                               public BossCtrlUnit_CheckMotionAttribute {
  public:
    DevilDoomCtrlUnit_CheckMotionAttribute(DevilDoomVarParam *param)
        : BossCtrlUnit(param), DevilDoomCtrlUnit(param), BossCtrlUnit_CheckMotionAttribute(param) {}
    virtual ~DevilDoomCtrlUnit_CheckMotionAttribute() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_CheckMotionAttribute::Tick(dt);
    }
    virtual void OnAttribute(u16 attribute, int value);
};

class DevilDoomCtrlUnit_Cloud : public DevilDoomCtrlUnit {
  public:
    DevilDoomCtrlUnit_Cloud(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_Cloud();
    virtual int Tick(float dt);

    /* 0x0C */ _stMtnPlayWork *m_motions[3];
    /* 0x18 */ RpClump *m_clumps[3];
    /* 0x24 */ UnkClumpUpdater_80015A7C *m_updaters[3];
};

class DevilDoomCtrlUnit_BallControl : public DevilDoomCtrlUnit, public MotionCallbackContainer {
  public:
    DevilDoomCtrlUnit_BallControl(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_BallControl();
    virtual int Tick(float dt);
    virtual void OnMotion(_stMtnCallbackParam &param);

    /* 0x10 */ _stMtnPlayWork *m_motion;
    /* 0x14 */ void *m_node;
    /* 0x18 */ UnkMotionCallback_802414D4 m_callback;
};

class DevilDoomCtrlUnit_WingControl : public DevilDoomCtrlUnit,
                                      public AttackCallbackContainer,
                                      public MotionCallbackContainer {
  public:
    DevilDoomCtrlUnit_WingControl(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_WingControl();
    virtual int Tick(float dt);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnMotion(_stMtnCallbackParam &param);
    void OnCommand(CharCommand &command);
    float GetWingAngle();

    /* 0x14 */ float m_open[2];
    /* 0x1C */ float m_health[2];
    /* 0x24 */ float m_flash[2];
    /* 0x2C */ _stMtnPlayWork *m_motions[2];
    /* 0x34 */ void *m_node;
    /* 0x38 */ UnkMotionCallback_802414D4 m_callback;
    /* 0x44 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x4C */ boost::shared_ptr<GiHPGaugeEnemy> m_gauges[2];
    /* 0x5C */ UnkMatrix_800B1304 m_matrices[2];
    /* 0xBC */ boost::weak_ptr<Effect::System::EffectElement> m_effects[2];
};

class DevilDoomCtrlUnit_FireBreath : public DevilDoomCtrlUnit, public AttackCallbackContainer {
  public:
    DevilDoomCtrlUnit_FireBreath(DevilDoomVarParam *param, const Vec3f *start,
                                 const Vec3f *direction);
    virtual ~DevilDoomCtrlUnit_FireBreath();
    virtual int Tick(float dt);
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* 0x010 */ u8 m_hitGround;
    /* 0x014 */ Vec3f m_start;
    /* 0x020 */ Vec3f m_end;
    /* 0x02C */ Vec3f m_direction;
    /* 0x038 */ Vec3f m_soundPosition;
    /* 0x044 */ Mtx34f m_matrix;
    /* 0x074 */ Mtx34f m_endMatrix;
    /* 0x0A4 */ UnkMatrix_800B1304 m_flames[16];
    /* 0x3A4 */ s32 m_flameCount;
    /* 0x3A8 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x3B0 */ boost::weak_ptr<Effect::System::EffectElement> m_endEffect;
    /* 0x3B8 */ boost::weak_ptr<Effect::System::EffectElement> m_flameEffects[16];
    /* 0x438 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
};

class DevilDoomCtrlUnit_CheckInterrupt : public DevilDoomCtrlUnit {
  public:
    DevilDoomCtrlUnit_CheckInterrupt(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_CheckInterrupt();
    virtual int Tick(float dt);
};

class DevilDoomCtrlUnit_AdjustNeck : public DevilDoomCtrlUnit, public MotionCallbackContainer {
  public:
    DevilDoomCtrlUnit_AdjustNeck(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_AdjustNeck();
    virtual int Tick(float dt);
    virtual void OnMotion(_stMtnCallbackParam &param);

    /* 0x10 */ UnkMotionCallback_802414D4 m_callback;
    /* 0x1C */ float m_blend;
};

class DevilDoomCtrlUnit_AdjustEye : public DevilDoomCtrlUnit, public MotionCallbackContainer {
  public:
    DevilDoomCtrlUnit_AdjustEye(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_AdjustEye();
    virtual int Tick(float dt);
    virtual void OnMotion(_stMtnCallbackParam &param);

    /* 0x10 */ UnkMotionCallback_802414D4 m_callback;
    /* 0x1C */ float m_blend;
    /* 0x20 */ float m_dt;
    /* 0x24 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x2C */ Mtx34f m_matrix;
};

class DevilDoomCtrlUnit_FixLookCharCamera : public DevilDoomCtrlUnit,
                                            public BossCtrlUnit_FixLookCharCamera {
  public:
    DevilDoomCtrlUnit_FixLookCharCamera(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_FixLookCharCamera() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_FixLookCharCamera::Tick(dt);
    }
    virtual bool IsEnabled();
};

class DevilDoomCtrlUnit_Move : public DevilDoomCtrlUnit {
  public:
    DevilDoomCtrlUnit_Move(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_Move();
    virtual int Tick(float dt);
};

class DevilDoomCtrlUnit_AttackCallback : public DevilDoomCtrlUnit, public AttackCallbackContainer {
  public:
    DevilDoomCtrlUnit_AttackCallback(DevilDoomVarParam *param, int index);
    virtual int Tick(float dt) {
        return false;
    }
};

class DevilDoomCtrlUnit_AttackCallbackChest : public DevilDoomCtrlUnit_AttackCallback {
  public:
    DevilDoomCtrlUnit_AttackCallbackChest(DevilDoomVarParam *param, int index)
        : BossCtrlUnit(param), DevilDoomCtrlUnit_AttackCallback(param, index) {}
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

class DevilDoomCtrlUnit_AttackCallbackEye : public DevilDoomCtrlUnit_AttackCallback {
  public:
    DevilDoomCtrlUnit_AttackCallbackEye(DevilDoomVarParam *param, int index)
        : BossCtrlUnit(param), DevilDoomCtrlUnit_AttackCallback(param, index) {}
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

class DevilDoomCtrlUnit_AttackCallbackEyeL : public DevilDoomCtrlUnit_AttackCallbackEye {
  public:
    DevilDoomCtrlUnit_AttackCallbackEyeL(DevilDoomVarParam *param, int index)
        : BossCtrlUnit(param), DevilDoomCtrlUnit_AttackCallbackEye(param, index) {}
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

class DevilDoomCtrlUnit_AttackCallbackEyeR : public DevilDoomCtrlUnit_AttackCallbackEye {
  public:
    DevilDoomCtrlUnit_AttackCallbackEyeR(DevilDoomVarParam *param, int index)
        : BossCtrlUnit(param), DevilDoomCtrlUnit_AttackCallbackEye(param, index) {}
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

/* The objects Devil Doom throws with psychokinesis. */
class DevilDoomCtrlUnit_PsychoObjectControl : public DevilDoomCtrlUnit {
  public:
    DevilDoomCtrlUnit_PsychoObjectControl(DevilDoomVarParam *owner);
    virtual ~DevilDoomCtrlUnit_PsychoObjectControl();
    virtual int Tick(float dt);

    /* Spaces the objects of each row out by their widths (name inferred). */
    void UpdateAngles();

    /* 0x0C */ u8 field_0xC[0x10 - 0xC];
};

/* One of the three groups of bits (index 0-2). */
class DevilDoomCtrlUnit_BitGroupControl : public DevilDoomCtrlUnit {
  public:
    DevilDoomCtrlUnit_BitGroupControl(DevilDoomVarParam *owner, int index);
    virtual ~DevilDoomCtrlUnit_BitGroupControl();
    virtual int Tick(float dt);

    /* The group's place eases from the previous one (the prev members) to the one its part of
     * DevilDoomVarParam asks for (names inferred; see DevilDoomCtrlUnitBit.cpp). */
    /* 0x0C */ u8 m_created; /* the bits have been made */
    /* 0x10 */ float m_time;
    /* 0x14 */ s32 m_group;
    /* 0x18 */ Vec3f m_position;
    /* 0x24 */ Vec3f m_prevPosition;
    /* 0x30 */ Mtx34f m_rotation;
    /* 0x60 */ Mtx34f m_prevRotation;
    /* 0x90 */ Vec3f m_scale;
    /* 0x9C */ Vec3f m_prevScale;
    /* 0xA8 */ Vec3f m_worldPosition;
};

/* The bits' attack. */
class DevilDoomCtrlUnit_BitAttackControl : public DevilDoomCtrlUnit {
  public:
    DevilDoomCtrlUnit_BitAttackControl(DevilDoomVarParam *owner);
    virtual ~DevilDoomCtrlUnit_BitAttackControl();
    virtual int Tick(float dt);

    /* Puts the three groups back in their rings and ends the attack (name inferred). */
    void ResetGroups();

    /* 0x0C */ float m_time;
};

class DevilDoomCtrlUnit_GetStatus : public DevilDoomCtrlUnit {
  public:
    DevilDoomCtrlUnit_GetStatus(DevilDoomVarParam *param);
    virtual ~DevilDoomCtrlUnit_GetStatus();
    virtual int Tick(float dt);
};

/* DevilDoomModeCtrlUnit (class name from RTTI; vtable at 0x8055B84C): the unit that runs the
 * boss's modes, defined in Game/DevilDoomModeCtrlUnit.cpp. Its constructor is inline, defined
 * in Game/DevilDoom.cpp (its only user): defining it here would renumber the other units'
 * compiler-generated @N locals. */
class DevilDoomModeCtrlUnit : public DevilDoomCtrlUnit, public BossModeCtrlUnit {
  public:
    DevilDoomModeCtrlUnit(DevilDoomVarParam *owner);
    virtual ~DevilDoomModeCtrlUnit();
    virtual int Tick(float dt);
    virtual BossCtrlUnit *CreateMode(int mode);
};

#endif

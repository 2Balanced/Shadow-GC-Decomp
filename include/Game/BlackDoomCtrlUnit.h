#ifndef GAME_BLACKDOOMCTRLUNIT_H
#define GAME_BLACKDOOMCTRLUNIT_H

/* The Black Doom boss's state and its control units (class names from RTTI), defined in
 * Game/BlackDoomCtrlUnit.cpp; the boss task and its mode units create them on the boss's
 * BlackDoomVarParam. Only the members the units use are mapped; member names and meanings are
 * inferred. */

#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct RpClump;
struct RwFrame;
class Target;
class UnkClumpUpdater_80015A7C;
namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliBase;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

/* The boss's tuning values (struct name unknown), at 0x74 in BlackDoomVarParam: constants its
 * inline constructor sets, in BlackDoomVarParam's constructor (Game/BlackDoom.cpp). Offsets are
 * relative to the struct; the comments give the offset in BlackDoomVarParam too. */
struct BlackDoomSettings {
    BlackDoomSettings() {
        m_accel = 100.0f;
        m_drag = 1.2f;
        m_radius = 100.0f;
        field_0xC = 20.0f;
        m_turnSpeedMax = 120.0f;
        m_waitTime = 5.0f;
        m_stunTime = 0.5f;
        m_score = 500;
        m_comboLimit = 3;
        m_cameraDistance = 80.0f;
        m_targetSpeed = 280.0f;
        m_stunDamage = 10.0f;
        m_comboTime = 5.0f;
        m_damageLimit = 10.0f;
        m_damageTime = 30.0f;
        m_climbSpeed = 50.0f;
        m_diskRewind = 60.0f;
        m_diskBlend = 30.0f;
        m_diskSpeed = 300.0f;
        m_diskRange = 70.0f;
        m_meteorInterval = 0.3f;
        m_meteorSpeed = 300.0f;
        m_boomerangSpeed = 400.0f;
        field_0x5C = 1.0f;
        field_0x60 = 100.0f;
        m_boomerangTime = 2.0f;
        m_attackCount = -1;
        field_0x6C = 1.0f;
        field_0x70 = 0.2f;
        field_0x74 = 10.0f;
        m_attackTime = 2.0f;
    }

    /* 0x00 (0x074) */ float m_accel;  /* what the modes reset BlackDoomVarParam::m_accel to */
    /* 0x04 (0x078) */ float m_drag;   /* slows the boss's speed down */
    /* 0x08 (0x07C) */ float m_radius; /* of the circle about a corner */
    /* 0x0C (0x080) */ float field_0xC;
    /* 0x10 (0x084) */ float m_turnSpeedMax; /* in degrees a second */
    /* 0x14 (0x088) */ float m_waitTime;     /* WAIT's time, before the scales */
    /* 0x18 (0x08C) */ float m_stunTime;     /* GET_ATK's shortest stun */
    /* 0x1C (0x090) */ s32 m_score;
    /* 0x20 (0x094) */ s32 m_comboLimit;       /* hits a combo may land */
    /* 0x24 (0x098) */ float m_cameraDistance; /* the camera follows the player closer than this */
    /* 0x28 (0x09C) */ float m_targetSpeed;    /* how fast the boss's aim follows the player */
    /* 0x2C (0x0A0) */ float m_stunDamage;     /* damage (m_damage) that ends a combo */
    /* 0x30 (0x0A4) */ float m_comboTime;      /* a combo ends after this long (m_time) */
    /* 0x34 (0x0A8) */ float m_damageLimit;    /* recent damage that slows the boss */
    /* 0x38 (0x0AC) */ float m_damageTime;
    /* 0x3C (0x0B0) */ float m_climbSpeed;
    /* 0x40 (0x0B4) */ float m_diskRewind; /* frames ATK_DISK replays for the second disk */
    /* 0x44 (0x0B8) */ float m_diskBlend;  /* the most frames it blends over */
    /* 0x48 (0x0BC) */ float m_diskSpeed;
    /* 0x4C (0x0C0) */ float m_diskRange;
    /* 0x50 (0x0C4) */ float m_meteorInterval;
    /* 0x54 (0x0C8) */ float m_meteorSpeed;
    /* 0x58 (0x0CC) */ float m_boomerangSpeed;
    /* 0x5C (0x0D0) */ float field_0x5C;
    /* 0x60 (0x0D4) */ float field_0x60;
    /* 0x64 (0x0D8) */ float m_boomerangTime;
    /* 0x68 (0x0DC) */ s32 m_attackCount; /* a fixed number of attacks a round when not negative */
    /* 0x6C (0x0E0) */ float field_0x6C;
    /* 0x70 (0x0E4) */ float field_0x70;
    /* 0x74 (0x0E8) */ float field_0x74;
    /* 0x78 (0x0EC) */ float m_attackTime; /* the time between attacks */
};
SIZE_ASSERT(BlackDoomSettings, 0x7C);

/* One of the boss's queued attacks (struct name unknown): its empty constructor is out of line at
 * 0x802838A0. m_state counts from 0 (free) on; ATK_READY moves it on. Attack i is the one alter
 * ego i (BlackDoomCtrlUnit_AlterEgo) makes: m_position and m_rotation place the alter ego. */
struct BlackDoomAttack {
    BlackDoomAttack() {}

    /* 0x00 */ s32 m_state;
    /* 0x04 */ u8 field_0x4;
    /* 0x08 */ Vec3f m_position;
    /* 0x14 */ float m_rotation;
};
SIZE_ASSERT(BlackDoomAttack, 0x18);

/* The data of the motion attribute 0x5000 (struct name unknown); the attack modes act on it. */
struct BlackDoomAttribute {
    /* 0x0 */ s32 m_state; /* 1: launch effect m_index, 2: the attack ends */
    /* 0x4 */ s32 m_index;
};

/* BlackDoomVarParam (class name from RTTI; vtable at 0x8054E35C, constructor 0x802835B8,
 * destructor 0x80282E24): the state of the Black Doom boss, at 0x38 in the boss task. The boss
 * circles the four corners of its arena (BlackDoomCtrlUnit_Move): it runs around the circle of
 * radius m_settings.m_radius about one corner, then along the tangent to the next. */
class BlackDoomVarParam : public BossVarParam {
  public:
    BlackDoomVarParam();
    virtual ~BlackDoomVarParam();

    /* 0x074 */ BlackDoomSettings m_settings;
    /* 0x0F0 */ Vec3f m_home;      /* where the boss starts; y is the height it floats at */
    /* 0x0FC */ float m_angleLeft; /* to turn to the target, in radians */
    /* 0x100 */ u8 m_moveLocked;
    /* 0x101 */ u8 m_turnLocked;
    /* 0x102 */ u8 m_targetLocked;
    /* 0x104 */ float m_accel;
    /* 0x108 */ float m_speed;
    /* 0x10C */ s32 m_corner;     /* the corner the boss circles, or -1 */
    /* 0x110 */ s32 m_nextCorner; /* the corner the boss heads for, or -1 */
    /* 0x114 */ s32 m_prevCorner; /* the corner the boss last left, or -1 */
    /* 0x118 */ s32 m_direction;  /* 1 or -1 about the corners; 0 until chosen */
    /* 0x11C */ float m_angle;    /* about the current corner */
    /* 0x120 */ float m_turnSpeed;
    /* 0x124 */ Vec3f m_testDestination; /* where the boss aims; the attacks fly at it */
    /* 0x130 */ Vec3f m_prevDestination; /* m_testDestination as it was last frame */
    /* 0x13C */ Vec3f m_target;          /* where the player is */
    /* 0x148 */ float m_destinationDistance;
    /* 0x14C */ float m_targetAngle;
    /* 0x150 */ u8 m_hit;
    /* 0x154 */ float m_damage; /* taken since the last stun */
    /* 0x158 */ float m_time;
    /* 0x15C */ s32 m_comboCount;
    /* 0x160 */ float m_recentDamage; /* the damage taken lately */
    /* 0x164 */ float m_damageTimer;
    /* 0x168 */ float m_damageScale; /* 0.9 while the recent damage is high; scales WAIT's time */
    /* 0x16C */ float m_healthScale; /* by how much health is left */
    /* 0x170 */ u8 m_attackUsed[3];  /* DISK, METEOR and BOOMERANG, this round */
    /* 0x174 */ Vec3f m_prevPosition;
    /* 0x180 */ float m_prevRotation;
    /* 0x184 */ s32 field_0x184;
    /* 0x188 */ s32 m_alterEgos; /* meaning inferred */
    /* 0x18C */ s32 m_attackStep;
    /* 0x190 */ float m_attackTime;
    /* 0x194 */ BlackDoomAttack m_attacks[10];
    /* 0x284 */ const BlackDoomAttribute *m_attribute; /* set by the motion's attribute callback */
};
SIZE_ASSERT(BlackDoomVarParam, 0x288);

/* BlackDoomCtrlUnit (class name from RTTI; vtable at 0x8054DF50, constructor 0x8027F56C,
 * destructor 0x8027F340): the base of the boss's units. */
class BlackDoomCtrlUnit : public virtual BossCtrlUnit {
  public:
    BlackDoomCtrlUnit(BlackDoomVarParam *owner);
    virtual ~BlackDoomCtrlUnit();

    /* 0x8 */ BlackDoomVarParam *m_owner;
};

/* The aura model around the boss (vtable at 0x8054DF30). */
class BlackDoomCtrlUnit_DispAura : public BlackDoomCtrlUnit {
  public:
    BlackDoomCtrlUnit_DispAura(BlackDoomVarParam *param);
    virtual ~BlackDoomCtrlUnit_DispAura();
    virtual int Tick(float dt);

    /* 0x0C */ RpClump *m_clump;
    /* 0x10 */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x14 */ UnkClumpFade_80027D5C *m_fade;
};

/* The eye model on the boss (vtable at 0x8054DF80). */
class BlackDoomCtrlUnit_DispEye : public BlackDoomCtrlUnit {
  public:
    BlackDoomCtrlUnit_DispEye(BlackDoomVarParam *param);
    virtual ~BlackDoomCtrlUnit_DispEye();
    virtual int Tick(float dt);

    /* 0x0C */ RpClump *m_clump;
    /* 0x10 */ RwFrame *m_frame;
    /* 0x14 */ UnkClumpFade_80027D5C *m_fade;
};

/* Plays the sound of the motion's attribute 0x5001, and keeps attribute 0x5000 (vtable at
 * 0x8054DFC8). */
class BlackDoomCtrlUnit_CheckMotionAttribute : public BlackDoomCtrlUnit,
                                               public BossCtrlUnit_CheckMotionAttribute {
  public:
    BlackDoomCtrlUnit_CheckMotionAttribute(BlackDoomVarParam *param)
        : BossCtrlUnit(param), BlackDoomCtrlUnit(param), BossCtrlUnit_CheckMotionAttribute(param) {}
    virtual ~BlackDoomCtrlUnit_CheckMotionAttribute() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_CheckMotionAttribute::Tick(dt);
    }
    virtual void OnAttribute(u16 attribute, int value);
};

/* A boomerang thrown from position along velocity; it bounces off walls and ends after
 * m_boomerangTime (vtable at 0x8054E01C; meanings inferred). */
class BlackDoomCtrlUnit_Boomerang : public BlackDoomCtrlUnit, public AttackCallbackContainer {
  public:
    BlackDoomCtrlUnit_Boomerang(BlackDoomVarParam *param, const Vec3f *position,
                                const Vec3f *velocity);
    virtual ~BlackDoomCtrlUnit_Boomerang();
    virtual int Tick(float dt);
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* 0x10 */ float m_radius;
    /* 0x14 */ u8 field_0x14[0x1C - 0x14];
    /* 0x1C */ float m_time;
    /* 0x20 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x28 */ Vec3f m_velocity;
    /* 0x34 */ Vec3f m_position;
    /* 0x40 */ Vec3f m_prevPosition;
    /* 0x4C */ u8 field_0x4C[0x58 - 0x4C];
    /* 0x58 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
};

/* A meteor falling from position along velocity (vtable at 0x8054E07C; meanings inferred). */
class BlackDoomCtrlUnit_Meteor : public BlackDoomCtrlUnit, public AttackCallbackContainer {
  public:
    BlackDoomCtrlUnit_Meteor(BlackDoomVarParam *param, const Vec3f *position,
                             const Vec3f *velocity);
    virtual ~BlackDoomCtrlUnit_Meteor();
    virtual int Tick(float dt);
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u8 m_hit;
    /* 0x18 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x20 */ Vec3f m_velocity;
    /* 0x2C */ Vec3f m_position;
    /* 0x38 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x40 */ boost::shared_ptr<Target> m_target;
};

/* A disk that slides along the ground from position in direction (vtable at 0x8054E0E0;
 * meanings inferred). */
class BlackDoomCtrlUnit_Disk : public BlackDoomCtrlUnit, public AttackCallbackContainer {
  public:
    BlackDoomCtrlUnit_Disk(BlackDoomVarParam *param, const Vec3f *position,
                           const Vec3f *direction);
    virtual ~BlackDoomCtrlUnit_Disk();
    virtual int Tick(float dt);
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* Places the two matrices at the disk (name inferred). */
    void UpdateMatrices();

    /* 0x10 */ float m_radius;
    /* 0x14 */ u8 m_landed;
    /* 0x18 */ float m_distance; /* travelled since it landed */
    /* 0x1C */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x24 */ Vec3f m_velocity;
    /* 0x30 */ Vec3f m_position;
    /* 0x3C */ Mtx34f m_matrix;
    /* 0x6C */ Mtx34f m_effectMatrix;
    /* 0x9C */ Vec3f m_prevPosition;
    /* 0xA8 */ Vec3f m_offset; /* of the effect from the disk */
    /* 0xB4 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
};

/* Keeps the camera on the player and the boss (vtable at 0x8054E14C). */
class BlackDoomCtrlUnit_FixLookCharCamera : public BlackDoomCtrlUnit,
                                            public BossCtrlUnit_FixLookCharCamera {
  public:
    BlackDoomCtrlUnit_FixLookCharCamera(BlackDoomVarParam *param);
    virtual ~BlackDoomCtrlUnit_FixLookCharCamera() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_FixLookCharCamera::Tick(dt);
    }
    virtual bool IsEnabled();
};

/* Moves the boss around the corners of the arena and turns it to its target (vtable at
 * 0x8054E198). */
class BlackDoomCtrlUnit_Move : public BlackDoomCtrlUnit {
  public:
    BlackDoomCtrlUnit_Move(BlackDoomVarParam *param);
    virtual ~BlackDoomCtrlUnit_Move();
    virtual int Tick(float dt);
};

/* Receives the callbacks of the boss's body collider (vtable at 0x8054E1D4). */
class BlackDoomCtrlUnit_AttackCallback : public BlackDoomCtrlUnit, public AttackCallbackContainer {
  public:
    BlackDoomCtrlUnit_AttackCallback(BlackDoomVarParam *param);
    virtual ~BlackDoomCtrlUnit_AttackCallback();
    virtual int Tick(float dt);
    virtual void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    virtual void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

/* Updates the boss's view of the player and its damage state (vtable at 0x8054E230). */
class BlackDoomCtrlUnit_GetStatus : public BlackDoomCtrlUnit {
  public:
    BlackDoomCtrlUnit_GetStatus(BlackDoomVarParam *param);
    virtual ~BlackDoomCtrlUnit_GetStatus();
    virtual int Tick(float dt);
};

SIZE_ASSERT(BlackDoomCtrlUnit_DispAura, 0x28);
SIZE_ASSERT(BlackDoomCtrlUnit_DispEye, 0x28);
SIZE_ASSERT(BlackDoomCtrlUnit_CheckMotionAttribute, 0x24);
SIZE_ASSERT(BlackDoomCtrlUnit_Boomerang, 0x70);
SIZE_ASSERT(BlackDoomCtrlUnit_Meteor, 0x58);
SIZE_ASSERT(BlackDoomCtrlUnit_Disk, 0xCC);
SIZE_ASSERT(BlackDoomCtrlUnit_FixLookCharCamera, 0x4C);
SIZE_ASSERT(BlackDoomCtrlUnit_Move, 0x1C);
SIZE_ASSERT(BlackDoomCtrlUnit_AttackCallback, 0x20);
SIZE_ASSERT(BlackDoomCtrlUnit_GetStatus, 0x1C);

/* BlackDoomCtrlUnit_AlterEgo (vtable at 0x80557A18; Game/BlackDoomCtrlUnitAlterEgo.cpp): one of
 * the boss's copies. It appears behind the boss (or the alter ego before it), follows it, and
 * fires disks, meteors or a boomerang with it when the boss attacks; m_owner->m_attacks[m_index]
 * holds its state. Member names are inferred. */
class BlackDoomCtrlUnit_AlterEgo : public BlackDoomCtrlUnit {
  public:
    BlackDoomCtrlUnit_AlterEgo(BlackDoomVarParam *param);
    virtual ~BlackDoomCtrlUnit_AlterEgo();
    virtual int Tick(float dt);

    void Turn(float dt);
    void UpdateMotion(float dt);
    void PlaceDiskPoints();
    void PlaceMeteorPoints();

    /* 0x0C */ s32 m_index; /* the alter ego's attack, or -1 */
    /* 0x10 */ float m_alpha;
    /* 0x14 */ RpClump *m_clump;
    /* 0x18 */ UnkClumpFade_80027D5C *m_fade;
    /* 0x1C */ _stMtnPlayWork *m_motion;
    /* 0x20 */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x24 */ const BlackDoomAttribute *m_attribute; /* the motion's last attribute 0x5000 */
    /* 0x28 */ Vec3f m_points[5]; /* where the shots start */
    /* 0x64 */ boost::weak_ptr<Effect::System::EffectElement> m_effects[5];
    /* 0x8C */ float m_timer;
    /* 0x90 */ u8 m_done;
    /* 0x91 */ u8 m_launched;
    /* 0x94 */ s32 m_shots; /* left to fire */
    /* 0x98 */ s32 m_point; /* of the next shot */
};
SIZE_ASSERT(BlackDoomCtrlUnit_AlterEgo, 0xAC);

/* BlackDoomModeCtrlUnit (class name from RTTI; vtable at 0x8054E39C): the unit that runs the
 * boss's modes, defined in Game/BlackDoomModeCtrlUnit.cpp. Its constructor is inline, defined in
 * Game/BlackDoom.cpp (its only user). */
class BlackDoomModeCtrlUnit : public BlackDoomCtrlUnit, public BossModeCtrlUnit {
  public:
    BlackDoomModeCtrlUnit(BlackDoomVarParam *owner);
    virtual ~BlackDoomModeCtrlUnit();
    virtual int Tick(float dt);
    virtual BossCtrlUnit *CreateMode(int mode);
};

#endif

#ifndef GAME_EGGMECACTRLUNIT_H
#define GAME_EGGMECACTRLUNIT_H

/* The Egg Breaker boss's state and control units (class names from RTTI). EggMecaVarParam is
 * defined in Game/EggMeca511.cpp, EggMecaCtrlUnit and its EggMecaCtrlUnit_* subclasses in
 * Game/EggMecaCtrlUnit.cpp, EggMecaModeCtrlUnit and its modes in Game/EggMecaModeCtrlUnit.cpp. The
 * boss tasks (EggMeca310, EggMeca411, EggMeca511) create the units on their EggMecaVarParam; the
 * code tells the three stages apart by the stage number. Member names and meanings are inferred. */

#include "Effect/System/EffectElement.h"
#include "Game/BossCtrlUnit.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

struct RpClump;
struct RwFrame;
struct UnkClumpFade_80027D5C;
class UnkClumpUpdater_80015A7C;

/* The boss's tuning values (struct name unknown; its constructor is out of line at 0x802E9FF0, in
 * Game/EggMeca511.cpp, and sets the defaults the three stages share). Offsets are from the start
 * of the struct; it sits at 0x74 in EggMecaVarParam. */
struct EggMecaSettings {
    EggMecaSettings();

    /* 0x000 */ float m_waitSpeed; /* the turn speeds of the modes (copied to m_turnSpeed) */
    /* 0x004 */ float m_dashSpeed;
    /* 0x008 */ float m_landSpeed;
    /* 0x00C */ float m_shootSpeed;
    /* 0x010 */ float m_laserSpeed;
    /* 0x014 */ float m_laserFireSpeed;
    /* 0x018 */ float m_waitTime;
    /* 0x01C */ float m_stunTime;
    /* 0x020 */ s32 m_score; /* points per unit of damage */
    /* 0x024 */ s32 m_comboLimit;
    /* 0x028 */ float m_targetSpeed; /* how fast the boss's target follows the player */
    /* 0x02C */ float m_damageLimit;
    /* 0x030 */ float m_nearDistance; /* closer than this the boss dashes or jumps */
    /* 0x034 */ float m_farDistance;  /* farther than this it throws the iron ball */
    /* 0x038 */ float m_jumpHeight;
    /* 0x03C */ float m_ironBallHeight; /* the height of the iron ball's throw */
    /* 0x040 */ float field_0x40;
    /* 0x044 */ float m_dashRadius;
    /* 0x048 */ float m_jumpDistance;
    /* 0x04C */ float m_gravity; /* of the boss's jump and of the iron ball */
    /* 0x050 */ float m_dashAngle; /* in degrees */
    /* 0x054 */ float m_circleRadius;
    /* 0x058 */ float m_circleSpeed;
    /* 0x05C */ float m_ironBallRadius;
    /* 0x060 */ float m_ironBallRadius2; /* while EggMecaVarParam::m_moving is set */
    /* 0x064 */ float m_shockWaveRadius;
    /* 0x068 */ float field_0x68;
    /* 0x06C */ float m_shockWaveTime;
    /* 0x070 */ float m_shakeArgs[3]; /* the camera shake of a shock wave */
    /* 0x07C */ float m_ironBallTime;
    /* 0x080 */ float m_ironBallReach;
    /* 0x084 */ float m_laserChargeTime;
    /* 0x088 */ float m_laserFireTime;
    /* 0x08C */ float field_0x8C;
    /* 0x090 */ float m_laserRadius;
    /* 0x094 */ float m_laserLength;
    /* 0x098 */ float m_laserSweep[2]; /* in degrees, for each laser */
    /* 0x0A0 */ float m_laserPower;
    /* 0x0A4 */ float m_turnAngle;  /* in degrees */
    /* 0x0A8 */ float m_turnAngle2; /* in degrees */
    /* 0x0AC */ float m_hintTime;
    /* 0x0B0 */ float m_moveTime;
    /* 0x0B4 */ float m_rollAcceleration;
    /* 0x0B8 */ float m_rollDamping;
    /* 0x0BC */ float m_rollStopSpeed;
    /* 0x0C0 */ float m_bombGrowTime;
    /* 0x0C4 */ float m_bombHeight;
    /* 0x0C8 */ float m_bombDropHeight;
    /* 0x0CC */ s32 m_bombCount;
    /* 0x0D0 */ float m_bombSpeed;
    /* 0x0D4 */ float m_bombAcceleration;
    /* 0x0D8 */ float m_bombDamping;
    /* 0x0DC */ float m_bombSpread; /* in radians */
    /* 0x0E0 */ float m_bombDistance;
    /* 0x0E4 */ float m_bombRadius;
    /* 0x0E8 */ float m_areaHalfSize;  /* stage 310: the boss stays in a square */
    /* 0x0EC */ float m_areaRadius;    /* stage 411: the boss stays in a circle */
    /* 0x0F0 */ float m_areaMinRadius; /* stage 511, below half health: in a ring */
    /* 0x0F4 */ float m_areaMaxRadius;
    /* 0x0F8 */ float m_cameraDistance;
    /* 0x0FC */ float m_cameraHeight;
    /* 0x100 */ float m_cameraOffsetY;
    /* 0x104 */ float m_cameraOffsetZ;
    /* 0x108 */ float field_0x108;
    /* 0x10C */ u8 field_0x10C;
    /* 0x10D */ u8 field_0x10D;
    /* 0x110 */ s32 m_belowHalfCamera;
    /* 0x114 */ s32 m_belowHalfMotion;
    /* 0x118 */ float m_cameraFreeHeight; /* stage 411: a player above it is out of reach */
    /* 0x11C */ float m_cameraFreeRadius;
};

/* EggMecaVarParam (class name from RTTI; vtable at 0x80559870, constructor 0x802E9E04, destructor
 * 0x802E9768, both in Game/EggMeca511.cpp): the state of the Egg Breaker boss. Only the members
 * the units use are mapped; names and meanings are inferred. */
class EggMecaVarParam : public BossVarParam {
  public:
    EggMecaVarParam();
    virtual ~EggMecaVarParam();

    /* 0x074 */ EggMecaSettings m_settings;
    /* 0x194 */ Vec3f m_home; /* where the boss starts */
    /* 0x1A0 */ Vec3f m_start;
    /* 0x1AC */ u32 field_0x1AC;
    /* 0x1B0 */ float m_angleDiff;
    /* 0x1B4 */ u8 m_motionLocked;
    /* 0x1B5 */ u8 m_fixed;       /* the boss does not turn */
    /* 0x1B6 */ u8 m_targetFixed; /* the target stops following the player */
    /* 0x1B7 */ u8 field_0x1B7;
    /* 0x1B8 */ u8 field_0x1B8[0x1C4 - 0x1B8];
    /* 0x1C4 */ float m_turnRate;  /* in radians a second */
    /* 0x1C8 */ float m_turnSpeed; /* in degrees a second; each mode sets it */
    /* 0x1CC */ Vec3f m_target;
    /* 0x1D8 */ Vec3f m_prevTarget;
    /* 0x1E4 */ Vec3f m_playerPosition;
    /* 0x1F0 */ float m_distance;    /* to the target */
    /* 0x1F4 */ float m_targetAngle; /* the direction of the target */
    /* 0x1F8 */ u8 field_0x1F8;
    /* 0x1FC */ float m_damageTaken;
    /* 0x200 */ s32 m_comboCount;
    /* 0x204 */ u8 field_0x204[0x210 - 0x204];
    /* 0x210 */ Vec3f m_destination; /* where a dash ends */
    /* 0x21C */ u32 m_attribute; /* the address of the motion attribute 0x5000's value */
    /* 0x220 */ s32 m_ironBallRequest; /* the state the iron ball is asked to change to, or -1 */
    /* 0x224 */ s32 m_ironBallState;
    /* 0x228 */ Vec3f m_ironBallPosition;
    /* 0x234 */ s32 m_laserRequest[2];
    /* 0x23C */ s32 m_laserState[2];
    /* 0x244 */ float m_hintTime;
    /* 0x248 */ u8 m_hasIronBall; /* the iron ball is in the boss's hand (inferred) */
    /* 0x249 */ u8 m_moving;      /* during ATK_MOVE */
    /* 0x24A */ u8 m_visible;     /* set during START, BELOW_HALF and DEATH */
    /* 0x24B */ u8 m_dead;        /* set during BELOW_HALF and DEATH: the lasers stop */
    /* 0x24C */ u32 field_0x24C;
};

/* EggMecaCtrlUnit (class name from RTTI; vtable at 0x80553F30): the base of the boss's units. */
class EggMecaCtrlUnit : public virtual BossCtrlUnit {
  public:
    EggMecaCtrlUnit(EggMecaVarParam *owner);
    virtual ~EggMecaCtrlUnit();

    /* 0x8 */ EggMecaVarParam *m_owner;
};

/* A vector member copy-constructed through the out-of-line copy constructor (fn_800091EC). */
struct EggMecaVector {
    EggMecaVector(const Vec3f &from) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &from);
    }

    Vec3f *Get() {
        return reinterpret_cast<Vec3f *>(this);
    }

    float x;
    float y;
    float z;
};

/* Keeps the boss inside the stage's area (stage ids 0x136, 0x19B and 0x1FF). */
class EggMecaCtrlUnit_AdjustCircleArea : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_AdjustCircleArea(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_AdjustCircleArea();
    virtual int Tick(float dt);
};

class EggMecaCtrlUnit_DispGuideCircle : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_DispGuideCircle(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_DispGuideCircle();
    virtual int Tick(float dt);
};

/* The boss's aura model. */
class EggMecaCtrlUnit_DispAura : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_DispAura(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_DispAura();
    virtual int Tick(float dt);

    /* 0x0C */ RpClump *m_clump;
    /* 0x10 */ UnkClumpUpdater_80015A7C *m_updater;
    /* 0x14 */ UnkClumpFade_80027D5C *m_fade;
    /* 0x18 */ u8 m_shown;
};

/* The ring of the shock wave where the iron ball lands. */
class EggMecaCtrlUnit_ShockWave : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_ShockWave(EggMecaVarParam *param, int effect);
    virtual ~EggMecaCtrlUnit_ShockWave();
    virtual int Tick(float dt);
    void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* 0x0C */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
    /* 0x14 */ float m_time;
    /* 0x18 */ s32 m_effect;
    /* 0x1C */ Vec3f m_position;
    /* 0x28 */ bool m_small;
};

/* The spin of the boss's upper body. */
class EggMecaCtrlUnit_RollUpper : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_RollUpper(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_RollUpper();
    virtual int Tick(float dt);
    void OnMotion(_stMtnCallbackParam &param);
    void Spring(float damping, float dt, float *angle, float *speed, const float *acceleration);
    float Approach(float target, float angle, float step);

    /* 0x0C */ float m_dt;
    /* 0x10 */ float m_angle;
    /* 0x14 */ float m_speed;
    /* 0x18 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x20 */ UnkMotionCallback_802414D4 m_callback;
};

/* A bomb that circles the boss, then rises and falls. */
class EggMecaCtrlUnit_Bomb : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_Bomb(EggMecaVarParam *param, const Vec3f &center, float angle, float spin);
    virtual ~EggMecaCtrlUnit_Bomb();
    virtual int Tick(float dt);
    void Spring(float damping, float dt, float *height, float *speed, const float *acceleration);
    void OnEnd(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void UpdatePosition();

    /* 0x0C */ EggMecaVector m_center;
    /* 0x18 */ Vec3f m_prevPosition;
    /* 0x24 */ Vec3f m_position;
    /* 0x30 */ float m_height;
    /* 0x34 */ float m_speed;
    /* 0x38 */ float m_angle;
    /* 0x3C */ float m_spin;
    /* 0x40 */ float m_time;
    /* 0x44 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x4C */ s32 m_state;
    /* 0x50 */ s32 m_request;
};

/* One of the boss's two lasers. */
class EggMecaCtrlUnit_Laser : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_Laser(EggMecaVarParam *param, int index);
    virtual ~EggMecaCtrlUnit_Laser();
    virtual int Tick(float dt);
    void OnEnd(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void UpdateBeam();
    void Fire(bool attack);
    void PlaceHitEffect(boost::shared_ptr<Effect::System::EffectElement> effect, bool attack);
    Vec3f GetAngles();
    void Aim();
    void UpdateMuzzle();
    void StopEffects();

    /* 0x0C */ Vec3f m_angles;
    /* 0x18 */ float m_sweep;
    /* 0x1C */ float m_sweepRange;
    /* 0x20 */ Vec3f m_muzzle;
    /* 0x2C */ Vec3f m_hitPosition;
    /* 0x38 */ Vec3f m_prevHitPosition;
    /* 0x44 */ u8 field_0x44[0x5C - 0x44];
    /* 0x5C */ float m_chargeTime;
    /* 0x60 */ float m_fireTime;
    /* 0x64 */ u8 m_hit;
    /* 0x68 */ boost::weak_ptr<Effect::System::EffectElement> m_beam;
    /* 0x70 */ boost::weak_ptr<Effect::System::EffectElement> m_hitEffect;
    /* 0x78 */ s32 m_index;
    /* 0x7C */ float m_length;
    /* 0x80 */ float m_width;
};

/* The iron ball the boss throws. */
class EggMecaCtrlUnit_IronBall : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_IronBall(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_IronBall();
    virtual int Tick(float dt);
    bool GetThrowVelocity(const Vec3f *from, const Vec3f *to, Vec3f *velocity, float height,
                          float gravity);
    void OnEnd(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);

    /* 0x0C */ RpClump *m_clump;
    /* 0x10 */ RwFrame *m_frame;
    /* 0x14 */ RpClump *m_chainClump; /* stage 0x19B only */
    /* 0x18 */ RwFrame *m_chainFrame;
    /* 0x1C */ Vec3f m_prevPosition;
    /* 0x28 */ Vec3f m_velocity;
    /* 0x34 */ Vec3f m_acceleration;
    /* 0x40 */ float m_time;
    /* 0x44 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x4C */ boost::weak_ptr<Effect::System::EffectElement> m_throwEffect;
    /* 0x54 */ UnkClumpFade_80027D5C *m_fade;
    /* 0x58 */ UnkClumpFade_80027D5C *m_chainFade;
    /* 0x5C */ u32 field_0x5C;
    /* 0x60 */ u8 m_started;
    /* 0x61 */ u8 m_shown;
};

/* The light of the boss's eye. */
class EggMecaCtrlUnit_DispEye : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_DispEye(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_DispEye();
    virtual int Tick(float dt);
    void OnMotion(_stMtnCallbackParam &param);

    /* 0x0C */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x14 */ UnkMotionCallback_802414D4 m_callback;
};

/* Eggman's model, which follows the boss's motion. */
class EggMecaCtrlUnit_DispEggman : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_DispEggman(EggMecaVarParam *param, int model);
    virtual ~EggMecaCtrlUnit_DispEggman();
    virtual int Tick(float dt);

    /* 0x0C */ RpClump *m_clump;
    /* 0x10 */ RwFrame *m_frame;
};

/* The canopy's model, which follows the boss's motion. */
class EggMecaCtrlUnit_DispCanopy : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_DispCanopy(EggMecaVarParam *param, int model);
    virtual ~EggMecaCtrlUnit_DispCanopy();
    virtual int Tick(float dt);

    /* 0x0C */ RpClump *m_clump;
    /* 0x10 */ RwFrame *m_frame;
};

class EggMecaCtrlUnit_CheckMotionAttribute : public EggMecaCtrlUnit,
                                             public BossCtrlUnit_CheckMotionAttribute {
  public:
    EggMecaCtrlUnit_CheckMotionAttribute(EggMecaVarParam *param)
        : BossCtrlUnit(param), EggMecaCtrlUnit(param), BossCtrlUnit_CheckMotionAttribute(param) {}
    virtual ~EggMecaCtrlUnit_CheckMotionAttribute() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_CheckMotionAttribute::Tick(dt);
    }
    virtual void OnAttribute(u16 attribute, int value);
};

class EggMecaCtrlUnit_FixLookCharCamera : public EggMecaCtrlUnit,
                                          public BossCtrlUnit_FixLookCharCamera {
  public:
    EggMecaCtrlUnit_FixLookCharCamera(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_FixLookCharCamera() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_FixLookCharCamera::Tick(dt);
    }
    virtual bool IsEnabled();
};

/* The boss's turning toward its target. */
class EggMecaCtrlUnit_Move : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_Move(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_Move();
    virtual int Tick(float dt);
};

/* The callbacks of the boss's body colliders. */
class EggMecaCtrlUnit_AttackCallback : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_AttackCallback(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_AttackCallback();
    virtual int Tick(float dt);
    void OnDamage(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
    void OnHit(SonicteamUSA::System::CharaColli::AttackCallbackParam &param);
};

/* The update of the boss's target, distance and angle to it. */
class EggMecaCtrlUnit_GetStatus : public EggMecaCtrlUnit {
  public:
    EggMecaCtrlUnit_GetStatus(EggMecaVarParam *param);
    virtual ~EggMecaCtrlUnit_GetStatus();
    virtual int Tick(float dt);
};

/* EggMecaModeCtrlUnit (class name from RTTI; vtable at 0x805542F4): the unit that runs the boss's
 * modes (Game/EggMecaModeCtrlUnit.cpp). Its constructor is inline, defined in the stage tasks' units
 * (Game/EggMeca511.cpp); defining it here would renumber Game/EggMecaCtrlUnit.cpp's @N locals. */
class EggMecaModeCtrlUnit : public EggMecaCtrlUnit, public BossModeCtrlUnit {
  public:
    EggMecaModeCtrlUnit(EggMecaVarParam *owner);
    virtual ~EggMecaModeCtrlUnit();
    virtual int Tick(float dt);
    virtual BossCtrlUnit *CreateMode(int mode);
};

SIZE_ASSERT(EggMecaSettings, 0x120);
SIZE_ASSERT(EggMecaVarParam, 0x250);
SIZE_ASSERT(EggMecaModeCtrlUnit, 0x28);
SIZE_ASSERT(EggMecaCtrlUnit_AdjustCircleArea, 0x1C);
SIZE_ASSERT(EggMecaCtrlUnit_ShockWave, 0x3C);
SIZE_ASSERT(EggMecaCtrlUnit_RollUpper, 0x3C);
SIZE_ASSERT(EggMecaCtrlUnit_Bomb, 0x64);
SIZE_ASSERT(EggMecaCtrlUnit_Laser, 0x94);
SIZE_ASSERT(EggMecaCtrlUnit_IronBall, 0x74);
SIZE_ASSERT(EggMecaCtrlUnit_DispEye, 0x30);
SIZE_ASSERT(EggMecaCtrlUnit_DispEggman, 0x24);
SIZE_ASSERT(EggMecaCtrlUnit_DispCanopy, 0x24);
SIZE_ASSERT(EggMecaCtrlUnit_CheckMotionAttribute, 0x24);
SIZE_ASSERT(EggMecaCtrlUnit_FixLookCharCamera, 0x4C);
SIZE_ASSERT(EggMecaCtrlUnit_Move, 0x1C);
SIZE_ASSERT(EggMecaCtrlUnit_AttackCallback, 0x1C);
SIZE_ASSERT(EggMecaCtrlUnit_GetStatus, 0x1C);

#endif

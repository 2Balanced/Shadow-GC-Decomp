#ifndef GAME_HEAVYDOGCTRLUNIT_H
#define GAME_HEAVYDOGCTRLUNIT_H

/* The Heavy Dog boss's state and the bases of its control units (class names from RTTI). The
 * boss has two stages, 0410 and 0510, each with its own BossVarParam subclass and units. Only the
 * members the units use are mapped; member names and meanings are inferred. */

#include "Game/BossCtrlUnit.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

class Task;

#ifdef HEAVYDOGVARPARAM_FULL
#include "boost/shared_ptr.hpp"

/* The full layout, for the units that define the boss's tasks and its shared units
 * (Game/HeavyDogCtrlUnit.cpp, Game/HeavyDog0410.cpp, Game/HeavyDog0510.cpp and the mode units).
 * Defined only there: the other units keep the plain layout below. */
class Target;
namespace Weapon {
class LockonTarget;
} // namespace Weapon

/* One of the rocket launchers on the boss's back (struct name unknown; names inferred). */
struct HeavyDogLauncher {
    /* 0x0 */ u8 m_alive;     /* the launcher is mounted */
    /* 0x1 */ u8 m_destroyed; /* shot off; it falls off the boss */
    /* 0x2 */ u8 m_fired;     /* it fired this frame */
    /* 0x4 */ float m_angle;  /* its turn on the boss's back */
};

class HeavyDogVarParam : public BossVarParam {
  public:
    HeavyDogVarParam();
    virtual ~HeavyDogVarParam();

    /* 0x074 */ float m_spring;
    /* 0x078 */ float m_damping;
    /* 0x07C */ float m_angleSpring;
    /* 0x080 */ float m_angleDamping;
    /* 0x084 */ float m_turnDistance; /* the boss turns along its velocity beyond it */
    /* 0x088 */ float m_soundSpeed;
    /* 0x08C */ float m_engineTilt;
    /* 0x090 */ float m_turnSpeed;    /* in degrees a second */
    /* 0x094 */ float field_0x94;
    /* 0x098 */ float field_0x98;
    /* 0x09C */ s32 m_canopyScore;
    /* 0x0A0 */ s32 m_launcherScore;
    /* 0x0A4 */ s32 field_0xA4;
    /* 0x0A8 */ float field_0xA8;
    /* 0x0AC */ float field_0xAC;
    /* 0x0B0 */ float field_0xB0;
    /* 0x0B4 */ float m_targetSpeed;  /* how fast the boss's aim follows the player */
    /* 0x0B8 */ float m_energyMax;
    /* 0x0BC */ float m_launcherHealth;
    /* 0x0C0 */ float m_reloadTime;
    /* 0x0C4 */ float m_launcherAim;  /* in degrees */
    /* 0x0C8 */ float m_launcherTurn; /* in degrees a second */
    /* 0x0CC */ s32 m_rocketEffect;
    /* 0x0D0 */ float m_barrierRadius;
    /* 0x0D4 */ float m_barrierHeight;
    /* 0x0D8 */ float m_barrierTime;
    /* 0x0DC */ float field_0xDC;
    /* 0x0E0 */ float field_0xE0;
    /* 0x0E4 */ float field_0xE4;
    /* 0x0E8 */ float m_launcherDamage; /* the current launcher's remaining health */
    /* 0x0EC */ Vec3f m_home; /* where the boss starts */
    /* 0x0F8 */ Vec3f m_targetPosition;
    /* 0x104 */ float m_targetAngle;
    /* 0x108 */ float m_targetDistance;
    /* 0x10C */ float m_angleDiff;
    /* 0x110 */ float m_angle;
    /* 0x114 */ u8 m_fixed;
    /* 0x115 */ u8 m_turnFixed;
    /* 0x116 */ u8 m_turnToVelocity;
    /* 0x117 */ u8 m_damaged;
    /* 0x118 */ float m_distance;
    /* 0x11C */ Vec3f m_velocity;
    /* 0x128 */ Vec3f m_acceleration;
    /* 0x134 */ float m_turnRate;
    /* 0x138 */ s32 m_launcherIndex;
    /* 0x13C */ s32 m_launcherCount;
    /* 0x140 */ s32 m_launchersAlive;
    /* 0x144 */ HeavyDogLauncher m_launchers[7];
    /* 0x17C */ Vec3f m_aim;       /* where the boss aims: follows the player */
    /* 0x188 */ Vec3f m_prevAim;
    /* 0x194 */ Vec3f m_playerPosition;
    /* 0x1A0 */ boost::shared_ptr<Target> m_targetPoint;
    /* 0x1A8 */ boost::shared_ptr<Weapon::LockonTarget> m_lockon;
    /* 0x1B0 */ float m_aimDistance;
    /* 0x1B4 */ float m_aimHeight;
    /* 0x1B8 */ float m_aimAngle;
    /* 0x1BC */ float m_aimDirection;
    /* 0x1C0 */ float m_energy;
    /* 0x1C4 */ float m_energyFull;
    /* 0x1C8 */ u8 field_0x1C8;
    /* 0x1CC */ u32 field_0x1CC;
    /* 0x1D0 */ s32 field_0x1D0;
    /* 0x1D4 */ s32 m_attribute;
};

class HeavyDog0510VarParam : public HeavyDogVarParam {
  public:
    HeavyDog0510VarParam();
    virtual ~HeavyDog0510VarParam();

    /* 0x1D8 */ float m_radiusSpeed;
    /* 0x1DC */ float m_bombScale;
    /* 0x1E0 */ float m_bombInterval;
    /* 0x1E4 */ s32 m_bombCount;    /* bombs dropped in a row */
    /* 0x1E8 */ float m_bombSpeed;
    /* 0x1EC */ float m_bombSpread;
    /* 0x1F0 */ float field_0x1F0;
    /* 0x1F4 */ float m_targetRadius;
    /* 0x1F8 */ float m_radius;
    /* 0x1FC */ float m_angleSpeed;
    /* 0x200 */ Task *m_bombLayer;
    /* 0x204 */ u32 field_0x204;
};

class HeavyDog0410VarParam : public HeavyDogVarParam {
  public:
    HeavyDog0410VarParam();
    virtual ~HeavyDog0410VarParam();

    /* 0x1D8 */ float field_0x1D8;
    /* 0x1DC */ float field_0x1DC;
    /* 0x1E0 */ float field_0x1E0;
    /* 0x1E4 */ float field_0x1E4;
    /* 0x1E8 */ float m_mineWaitTime;
    /* 0x1EC */ float m_mineBlinkTime;
    /* 0x1F0 */ float m_mineExplodeTime;
    /* 0x1F4 */ float m_mineSpring;
    /* 0x1F8 */ float m_mineDamping;
    /* 0x1FC */ float field_0x1FC;
};
#else
/* HeavyDogVarParam (class name from RTTI; vtable at 0x80549204): the state the Heavy Dog stages
 * share. */
class HeavyDogVarParam : public BossVarParam {
  public:
    virtual ~HeavyDogVarParam();

    /* 0x074 */ float m_spring;  /* spring constant of the position (the 0510 stage: height) */
    /* 0x078 */ float m_damping;
    /* 0x07C */ float m_angleSpring;   /* spring constant of the angle */
    /* 0x080 */ float m_angleDamping;
    /* 0x084 */ u32 field_0x84;
    /* 0x088 */ float m_soundSpeed;    /* the speed above which the move sound plays */
    /* 0x08C */ u8 field_0x8C[0xB0 - 0x8C];
    /* 0x0B0 */ float field_0xB0;
    /* 0x0B4 */ u8 field_0xB4[0xF8 - 0xB4];
    /* 0x0F8 */ Vec3f m_targetPosition; /* the 0510 stage uses only its height */
    /* 0x104 */ float m_targetAngle;
    /* 0x108 */ float m_targetDistance;
    /* 0x10C */ u32 field_0x10C;
    /* 0x110 */ float m_angle;
    /* 0x114 */ u8 m_fixed;          /* the boss does not move while set */
    /* 0x118 */ float m_distance;    /* the distance moved */
    /* 0x11C */ Vec3f m_velocity;
    /* 0x128 */ Vec3f m_acceleration;
    /* 0x134 */ u8 field_0x134[0x198 - 0x134];
    /* 0x198 */ float m_height; /* the player's height above the boss (meaning inferred) */
};

/* HeavyDog0510VarParam (class name from RTTI; vtable at 0x80549AAC): the state of the 0510
 * stage. Offsets are from the start of the object. */
class HeavyDog0510VarParam : public HeavyDogVarParam {
  public:
    virtual ~HeavyDog0510VarParam();

    /* 0x19C */ u8 field_0x19C[0x1B0 - 0x19C];
    /* 0x1B0 */ float field_0x1B0;
    /* 0x1B4 */ u8 field_0x1B4[0x1D8 - 0x1B4];
    /* 0x1D8 */ float m_radiusSpeed;
    /* 0x1DC */ float m_bombScale;
    /* 0x1E0 */ u8 field_0x1E0[0x1F0 - 0x1E0];
    /* 0x1F0 */ float field_0x1F0;
    /* 0x1F4 */ float m_targetRadius;
    /* 0x1F8 */ float m_radius;
    /* 0x1FC */ float m_angleSpeed;
    /* 0x200 */ Task *m_bombLayer;
};

/* HeavyDog0410VarParam (class name from RTTI; vtable at 0x80549224): the state of the 0410
 * stage. Offsets are from the start of the object. */
class HeavyDog0410VarParam : public HeavyDogVarParam {
  public:
    virtual ~HeavyDog0410VarParam();

    /* 0x19C */ u8 field_0x19C[0x1E8 - 0x19C];
    /* 0x1E8 */ float m_mineWaitTime;    /* before a mine starts blinking */
    /* 0x1EC */ float m_mineBlinkTime;   /* before it explodes */
    /* 0x1F0 */ float m_mineExplodeTime; /* before the exploded mine is removed */
    /* 0x1F4 */ float m_mineSpring;      /* pull towards its target */
    /* 0x1F8 */ float m_mineDamping;
};

#endif

/* HeavyDogCtrlUnit (class name from RTTI; vtable at 0x80548CFC): the base of the boss's units. */
class HeavyDogCtrlUnit : public virtual BossCtrlUnit {
  public:
    HeavyDogCtrlUnit(HeavyDogVarParam *owner);
    virtual ~HeavyDogCtrlUnit();

    /* 0x8 */ HeavyDogVarParam *m_owner;
};

/* HeavyDog0510CtrlUnit (class name from RTTI; vtable at 0x8054994C): the base of the 0510 stage's
 * units. Its constructor (0x80251B5C) and destructor (0x80251790) are out of line. */
class HeavyDog0510CtrlUnit : public HeavyDogCtrlUnit {
  public:
    HeavyDog0510CtrlUnit(HeavyDog0510VarParam *owner);
    virtual ~HeavyDog0510CtrlUnit();

    /* 0xC */ HeavyDog0510VarParam *m_owner;
};

/* HeavyDog0410CtrlUnit (class name from RTTI; vtable at 0x805491DC): the base of the 0410 stage's
 * units. Its constructor (0x8024EB1C) and destructor (0x8024EA8C) are out of line. */
class HeavyDog0410CtrlUnit : public HeavyDogCtrlUnit {
  public:
    HeavyDog0410CtrlUnit(HeavyDog0410VarParam *owner);
    virtual ~HeavyDog0410CtrlUnit();

    /* 0xC */ HeavyDog0410VarParam *m_owner;
};

#endif

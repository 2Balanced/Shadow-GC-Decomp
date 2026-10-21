#ifndef GAME_HEAVYDOGCTRLUNIT_H
#define GAME_HEAVYDOGCTRLUNIT_H

/* The Heavy Dog boss's state and the bases of its control units (class names from RTTI). The
 * boss has two stages, 0410 and 0510, each with its own BossVarParam subclass and units. Only the
 * members the units use are mapped; member names and meanings are inferred. */

#include "Game/BossCtrlUnit.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>

class Task;

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

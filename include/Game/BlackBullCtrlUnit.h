#ifndef GAME_BLACKBULLCTRLUNIT_H
#define GAME_BLACKBULLCTRLUNIT_H

/* The Black Bull boss's state and the bases of its control units (class names from RTTI). The
 * boss has two stages, 0210 and 0412, each with its own BossVarParam subclass and units. Only the
 * members the units use are mapped; member names and meanings are inferred. */

#include "Game/BossCtrlUnit.h"
#include <dolphin/types.h>

/* BlackBullVarParam (class name from RTTI; vtable at 0x805474AC): the state the Black Bull stages
 * share. Offsets are from the start of the object. */
class BlackBullVarParam : public BossVarParam {
  public:
    virtual ~BlackBullVarParam();

    /* 0x074 */ u8 field_0x74[0x88 - 0x74];
    /* 0x088 */ float m_turnMotionSpeed; /* in degrees a second: turning faster plays a turn */
    /* 0x08C */ u8 field_0x8C[0xB0 - 0x8C];
    /* 0x0B0 */ float field_0xB0;
    /* 0x0B4 */ u8 field_0xB4[0xE0 - 0xB4];
    /* 0x0E0 */ float m_turnSpeedMax; /* in degrees a second */
    /* 0x0E4 */ u8 field_0xE4[0x118 - 0xE4];
    /* 0x118 */ float m_turnSpeed;    /* in radians a second */
    /* 0x11C */ u8 field_0x11C[0x120 - 0x11C];
    /* 0x120 */ u8 m_motionLocked;
    /* 0x121 */ u8 m_motionLocked2;
    /* 0x122 */ u8 field_0x122[0x154 - 0x122];
    /* 0x154 */ float m_height; /* the player's height above the boss (meaning inferred) */
    /* 0x158 */ u32 field_0x158;
    /* 0x15C */ float field_0x15C;
};

/* BlackBull0412VarParam (class name from RTTI; vtable at 0x805484B8): the state of the 0412
 * stage. */
class BlackBull0412VarParam : public BlackBullVarParam {
  public:
    virtual ~BlackBull0412VarParam();
};

/* BlackBullCtrlUnit (class name from RTTI; vtable at 0x805471D8): the base of the boss's units. */
class BlackBullCtrlUnit : public virtual BossCtrlUnit {
  public:
    BlackBullCtrlUnit(BlackBullVarParam *owner);
    virtual ~BlackBullCtrlUnit();

    /* 0x8 */ BlackBullVarParam *m_owner;
};

/* BlackBull0412CtrlUnit (class name from RTTI; vtable at 0x8054847C): the base of the 0412 stage's
 * units. Its constructor (0x80243A48) and destructor (0x802439B8) are out of line. */
class BlackBull0412CtrlUnit : public BlackBullCtrlUnit {
  public:
    BlackBull0412CtrlUnit(BlackBull0412VarParam *owner);
    virtual ~BlackBull0412CtrlUnit();

    /* 0xC */ BlackBull0412VarParam *m_owner;
};

#endif

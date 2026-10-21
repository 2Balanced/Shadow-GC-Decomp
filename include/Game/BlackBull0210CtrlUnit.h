#ifndef GAME_BLACKBULL0210CTRLUNIT_H
#define GAME_BLACKBULL0210CTRLUNIT_H

/* The state and the unit bases of the Black Bull boss's 0210 stage (class names from RTTI). The
 * stage's task is in Game/BlackBull0210.cpp, its control units in Game/BlackBull0210CtrlUnit.cpp
 * and its modes in Game/BlackBull0210ModeCtrlUnit.cpp. Member names and meanings are inferred. */

#include "Game/BlackBullCtrlUnit.h"
#include "Game/BossCtrlUnit.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* BlackBull0210VarParam (class name from RTTI; vtable at 0x805474CC): the state of the 0210
 * stage. m_point is the index of the point of the arena the boss heads for (-1: none yet). */
class BlackBull0210VarParam : public BlackBullVarParam {
  public:
    BlackBull0210VarParam();
    virtual ~BlackBull0210VarParam();

    /* 0x18C */ s32 m_point;
};
SIZE_ASSERT(BlackBull0210VarParam, 0x190);

/* BlackBull0210CtrlUnit (class name from RTTI; vtable at 0x805471F4): the base of the 0210
 * stage's units. Its constructor (0x80231E00) and destructor (0x80231BD4) are out of line, in
 * Game/BlackBull0210CtrlUnit.cpp. */
class BlackBull0210CtrlUnit : public BlackBullCtrlUnit {
  public:
    BlackBull0210CtrlUnit(BlackBull0210VarParam *owner);
    virtual ~BlackBull0210CtrlUnit();

    /* 0xC */ BlackBull0210VarParam *m_owner;
};

/* BlackBull0210ModeCtrlUnit (class name from RTTI; vtable at 0x80547528): the unit that runs the
 * stage's modes, defined in Game/BlackBull0210ModeCtrlUnit.cpp. Its constructor is inline,
 * defined in Game/BlackBull0210.cpp, its only user. */
class BlackBull0210ModeCtrlUnit : public BlackBull0210CtrlUnit, public BossModeCtrlUnit {
  public:
    BlackBull0210ModeCtrlUnit(BlackBull0210VarParam *owner);
    virtual ~BlackBull0210ModeCtrlUnit();
    virtual int Tick(float dt);
    virtual BossCtrlUnit *CreateMode(int mode);
};

/* BlackBull0210CtrlUnit_FixLookCharCamera (class name from RTTI; vtable at 0x805471A0): keeps the
 * camera on the player and the boss while the player is near enough. */
class BlackBull0210CtrlUnit_FixLookCharCamera : public BlackBull0210CtrlUnit,
                                                public BossCtrlUnit_FixLookCharCamera {
  public:
    BlackBull0210CtrlUnit_FixLookCharCamera(BlackBull0210VarParam *param);
    virtual ~BlackBull0210CtrlUnit_FixLookCharCamera();
    virtual int Tick(float dt);
    virtual bool IsEnabled();
};

/* BlackBull0210CtrlUnit_MoveMotion (class name from RTTI; vtable at 0x8054723C): picks the boss's
 * motion from how fast it turns. */
class BlackBull0210CtrlUnit_MoveMotion : public BlackBull0210CtrlUnit {
  public:
    BlackBull0210CtrlUnit_MoveMotion(BlackBull0210VarParam *param);
    virtual ~BlackBull0210CtrlUnit_MoveMotion();
    virtual int Tick(float dt);
};

#endif

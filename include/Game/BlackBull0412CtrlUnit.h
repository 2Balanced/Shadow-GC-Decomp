#ifndef GAME_BLACKBULL0412CTRLUNIT_H
#define GAME_BLACKBULL0412CTRLUNIT_H

/* The unit classes of the Black Bull boss's 0412 stage (class names from RTTI), as the stage's
 * task (Game/BlackBull0412.cpp) and mode units (Game/BlackBull0412ModeCtrlUnit.cpp) see them. The
 * camera and motion units are defined in Game/BlackBull0412CtrlUnit.cpp, which declares them
 * itself. Member names and meanings are inferred. */

#include "Game/BlackBullCtrlUnit.h"
#include "Game/BossCtrlUnit.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* BlackBull0412ModeCtrlUnit (class name from RTTI; vtable at 0x80548518): the unit that runs the
 * stage's modes. Its constructor is inline, defined in Game/BlackBull0412.cpp, its only user. */
class BlackBull0412ModeCtrlUnit : public BlackBull0412CtrlUnit, public BossModeCtrlUnit {
  public:
    BlackBull0412ModeCtrlUnit(BlackBull0412VarParam *owner);
    virtual ~BlackBull0412ModeCtrlUnit();
    virtual int Tick(float dt);
    virtual BossCtrlUnit *CreateMode(int mode);
};

/* BlackBull0412CtrlUnit_FixLookCharCamera (vtable at 0x8054CA48) and
 * BlackBull0412CtrlUnit_MoveMotion (vtable at 0x8054CA9C): their constructors are out of line in
 * Game/BlackBull0412CtrlUnit.cpp. */
class BlackBull0412CtrlUnit_FixLookCharCamera : public BlackBull0412CtrlUnit,
                                                public BossCtrlUnit_FixLookCharCamera {
  public:
    BlackBull0412CtrlUnit_FixLookCharCamera(BlackBull0412VarParam *param);
    virtual ~BlackBull0412CtrlUnit_FixLookCharCamera() {}
    virtual int Tick(float dt) {
        return BossCtrlUnit_FixLookCharCamera::Tick(dt);
    }
    virtual bool IsEnabled();

    /* 0x40 */ s32 m_view;
};

class BlackBull0412CtrlUnit_MoveMotion : public BlackBull0412CtrlUnit {
  public:
    BlackBull0412CtrlUnit_MoveMotion(BlackBull0412VarParam *param);
    virtual ~BlackBull0412CtrlUnit_MoveMotion();
    virtual int Tick(float dt);
};

SIZE_ASSERT(BlackBull0412ModeCtrlUnit, 0x2C);
SIZE_ASSERT(BlackBull0412CtrlUnit_FixLookCharCamera, 0x54);
SIZE_ASSERT(BlackBull0412CtrlUnit_MoveMotion, 0x20);

#endif

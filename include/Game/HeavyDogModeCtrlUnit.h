#ifndef GAME_HEAVYDOGMODECTRLUNIT_H
#define GAME_HEAVYDOGMODECTRLUNIT_H

/* The mode units of the Heavy Dog boss's two stages (class names from RTTI), as the stages' tasks
 * (Game/HeavyDog0410.cpp, Game/HeavyDog0510.cpp) and the mode units' own units
 * (Game/HeavyDog0410ModeCtrlUnit.cpp, Game/HeavyDog0510ModeCtrlUnit.cpp) see them. The
 * constructors are inline, defined in the tasks' units, their only users. */

#include "Game/BossCtrlUnit.h"
#include "Game/HeavyDogCtrlUnit.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* HeavyDog0410ModeCtrlUnit (vtable at 0x80549280): runs the 0410 stage's modes. */
class HeavyDog0410ModeCtrlUnit : public HeavyDog0410CtrlUnit, public BossModeCtrlUnit {
  public:
    HeavyDog0410ModeCtrlUnit(HeavyDog0410VarParam *owner);
    virtual ~HeavyDog0410ModeCtrlUnit();
    virtual int Tick(float dt);
    virtual BossCtrlUnit *CreateMode(int mode);
};

/* HeavyDog0510ModeCtrlUnit (vtable at 0x80549610): runs the 0510 stage's modes. */
class HeavyDog0510ModeCtrlUnit : public HeavyDog0510CtrlUnit, public BossModeCtrlUnit {
  public:
    HeavyDog0510ModeCtrlUnit(HeavyDog0510VarParam *owner);
    virtual ~HeavyDog0510ModeCtrlUnit();
    virtual int Tick(float dt);
    virtual BossCtrlUnit *CreateMode(int mode);
};

SIZE_ASSERT(HeavyDog0410ModeCtrlUnit, 0x2C);
SIZE_ASSERT(HeavyDog0510ModeCtrlUnit, 0x2C);

#endif

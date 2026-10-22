#ifndef GAME_MEMORYCARDCTRL_H
#define GAME_MEMORYCARDCTRL_H

#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* TMemoryCardCtrl (class name from RTTI): the task that drives the memory card (its methods lie at
 * 0x80298F0C-0x8029B854). Only what other units call is declared here; the size is from the
 * allocation in fn_80299F14. */
class TMemoryCardCtrl : public Task {
  public:
    /* 0x28 */ u8 field_0x28[0x280 - 0x28];
};
SIZE_ASSERT(TMemoryCardCtrl, 0x280);

extern "C" {
/* Creates the controller below parent (behaviour inferred). */
TMemoryCardCtrl *fn_80299F14(Task *parent);
/* Releases and loads the controller's textures (behaviour inferred). */
void fn_80299F5C(void);
void fn_80299FE0(void);
}

#endif

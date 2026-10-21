#ifndef SHADOW_GAME_UNKSWITCHBANK_800CB69C_H
#define SHADOW_GAME_UNKSWITCHBANK_800CB69C_H

#include <dolphin/types.h>

/* A singleton (fetched with fn_800CB69C) that remembers which numbered stage switches are on, so
 * that a gadget flipping a switch can drive other gadgets listening to the same number. Its class
 * name and layout are not needed yet. */
class UnkSwitchBank_800CB69C;

extern "C" {
UnkSwitchBank_800CB69C *fn_800CB69C(void);
/* Switch number `which`: 1 when on. */
u32 fn_8016904C(UnkSwitchBank_800CB69C *bank, int which);
/* Turns switch number `which` on (on != 0) or off. */
void fn_801690D0(UnkSwitchBank_800CB69C *bank, int which, int on);
}

#endif

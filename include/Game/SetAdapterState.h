#ifndef GAME_SETADAPTERSTATE_H
#define GAME_SETADAPTERSTATE_H

#include <dolphin/types.h>

/* SetAdapterState (class name from RTTI; vtable at 0x80522C00): the base of the states a gadget
 * owner keeps for its gadget across the gadget's lifetime. Only the destructor is known.
 * ElecBomb.cpp keeps its own copy of this declaration. */
class SetAdapterState {
  public:
    virtual ~SetAdapterState() {}
};

/* The gadget owner (class unknown; Gadget.h only declares it). Units that reach the adapter state
 * declare this layout (as in ElecBomb.cpp). */
class UnkPlacement_800C9CD0 {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ SetAdapterState *m_adapterState;
};

extern "C" {
/* Whether the owner is being removed; deleting the owner's adapter state; storing state as the
 * owner's adapter state (behaviour inferred from their bodies). */
bool fn_800C9870(UnkPlacement_800C9CD0 *owner);
void fn_800C9894(UnkPlacement_800C9CD0 *owner);
void fn_800C9930(UnkPlacement_800C9CD0 *owner, SetAdapterState *state);
}

#endif

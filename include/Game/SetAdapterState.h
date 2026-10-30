#ifndef GAME_SETADAPTERSTATE_H
#define GAME_SETADAPTERSTATE_H

#include <dolphin/types.h>

/* SetAdapterState (class name from RTTI; vtable at 0x80522C00): the base of the states a gadget
 * owner keeps for its gadget across the gadget's lifetime. Only the destructor is known.
 * ElecBomb.cpp keeps its own copy of this declaration. */
class SetAdapterState {
  public:
#ifdef SET_ADAPTER_STATE_DTOR_OUT_OF_LINE
    /* Game/SetAdapterState.cpp defines it out of line (0x800C98E8). */
    virtual ~SetAdapterState();
#else
    virtual ~SetAdapterState() {}
#endif
};

#ifdef UNKPLACEMENT_800C9CD0_MEMBERS
class CharInfo;

/* The placement record itself (struct name unknown; layout from Game/SetAdapterState.cpp, which
 * reads it through its owner; names inferred). */
struct UnkPlacementRecord_800C9CD0 {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation; /* in degrees */
    /* 0x18 */ u32 m_flags;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ u16 field_0x20;
    /* 0x22 */ u8 m_switch;
    /* 0x23 */ u8 m_range;
    /* 0x24 */ s32 m_paramSize;
    /* 0x28 */ void *m_params;
};
#endif

/* The gadget owner (class unknown; Gadget.h only declares it). Units that reach the adapter state
 * declare this layout (as in ElecBomb.cpp). Game/SetAdapterState.cpp, which defines its functions,
 * sees the members before the adapter state too (UNKPLACEMENT_800C9CD0_MEMBERS; names inferred). */
class UnkPlacement_800C9CD0 {
  public:
#ifdef UNKPLACEMENT_800C9CD0_MEMBERS
    /* 0x00 */ UnkPlacementRecord_800C9CD0 *m_record;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ Task *m_task;
    /* 0x10 */ s32 m_type; /* index into the gadget type table */
    /* 0x14 */ SetAdapterState *m_adapterState;
    /* 0x18 */ boost::weak_ptr<CharInfo> m_chara;
#else
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ SetAdapterState *m_adapterState;
#endif
};

extern "C" {
/* Whether the owner is being removed; deleting the owner's adapter state; storing state as the
 * owner's adapter state (behaviour inferred from their bodies). */
bool fn_800C9870(UnkPlacement_800C9CD0 *owner);
void fn_800C9894(UnkPlacement_800C9CD0 *owner);
void fn_800C9930(UnkPlacement_800C9CD0 *owner, SetAdapterState *state);
}

#endif

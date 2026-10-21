#ifndef GAME_SETADAPTEREVENTARGS_H
#define GAME_SETADAPTEREVENTARGS_H

#include <dolphin/types.h>

/* SetAdapterEventArgs (class name from RTTI; vtable at 0x8053DE08, destructor 0x801ED468): the base
 * of the arguments passed with fn_800CB47C to a callback for each set object. field_0x0 selects
 * the objects (negative: all of them; otherwise the byte at 0x22 of the object must match, as
 * fn_800C9F68 reads it). Names inferred. CodeWarrior places the vtable pointer after field_0x0. */
class SetAdapterEventArgs {
  public:
    SetAdapterEventArgs() : field_0x0(0) {}

    /* 0x0 */ s32 field_0x0;

    /* 0x4: vtable pointer */
    virtual ~SetAdapterEventArgs() {}
};

#endif

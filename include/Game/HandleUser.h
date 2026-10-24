#ifndef GAME_HANDLEUSER_H
#define GAME_HANDLEUSER_H

#include <dolphin/types.h>
#include "size_assert.h"

/* HandleUser (class name from RTTI; vtable 0x8051CECC): something reachable through a handle
 * (fn_80045A98 returns the HandleUser of a handle, fn_80045A64 gives one its handle). 8 bytes, from
 * Game/CameraRender.cpp's tasks, which are 0x30 bytes with a HandleUser at 0x28. The constructor
 * (0x8003AE70) clears the word at 0x4 and sets the one at 0x6 to -1. The virtual functions are
 * inline: every unit that needs the vtable emits it (TBlockManage's unit has the copy linked,
 * 0x8003AD44 for the destructor, 0x80039ED4 for slot 0x0C, which returns the address of the
 * handle words), which also places the RTTI records of Game/CameraRender.cpp. Names other than the
 * class's are inferred. */
class HandleUser {
  public:
    HandleUser();
    virtual ~HandleUser() {
        KeepOutOfLine();
    }
    virtual void *GetHandle() {
        return &field_0x4;
    }

    /* 0x4 */ u16 field_0x4;
    /* 0x6 */ s16 field_0x6;

  private:
    /* An inline call that keeps the destructor from being inlined into the derived classes'
     * (deferred inlining is one level deep): the game calls its out-of-line copy. */
    static void KeepOutOfLine() {}
};
SIZE_ASSERT(HandleUser, 0x8);

#endif

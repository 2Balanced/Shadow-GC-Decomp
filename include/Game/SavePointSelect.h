#ifndef GAME_SAVEPOINTSELECT_H
#define GAME_SAVEPOINTSELECT_H

#include <boost/function.hpp>

#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* Game/SavePointSelect.cpp defines GAME_SAVEPOINTSELECT_IMPL to see the menu's state; other units
 * keep the placeholder, so that the scoped_ptr template does not renumber their @N locals. */
#ifdef GAME_SAVEPOINTSELECT_IMPL
#include "boost/scoped_ptr.hpp"
#endif

namespace SavePointSelect {

/* Args (class name from the RTTI of the callbacks bound to it): what the save point menu passes to
 * its callbacks. field_0x0 is the chosen entry, -1 for none (meaning inferred from the SAVEPOINT
 * gadget). */
struct Args {
    Args(s32 value) : field_0x0(value) {}

    /* 0x0 */ s32 field_0x0;
};

/* The type of the menu's callback members: a boost::function1 member whose assignment from one is
 * an inline function of its own (class name unknown; the member, not a base, as Game/SavePointSelect.cpp
 * shows: its implicit destructor destroys the function1 as a complete object). */
class SelectHandler {
  public:
    SelectHandler &operator=(const boost::function1<void, Args &> &f) {
        m_function = f;
        return *this;
    }

    /* Whether no function is set, and calling the function if one is (names inferred; both are
     * weak functions of Game/SavePointSelect.cpp). */
    bool IsEmpty() const {
        return m_function.empty();
    }
    void Call(Args &args) const {
        if (!m_function.empty()) {
            m_function(args);
        }
    }

    /* 0x0 */ boost::function1<void, Args &> m_function;
};

/* The menu's state (class name from RTTI; defined in Game/SavePointSelect.cpp). */
class SavePointSelectImpl;

/* SavePointSelectTask (class name from RTTI; vtable at 0x8055D2C0): the save point menu, defined in
 * Game/SavePointSelect.cpp (names inferred). */
class SavePointSelectTask : public Task {
  public:
    /* 0x80331114 (arg's meaning unknown; the gadget passes its priority parameter). */
    SavePointSelectTask(Task *parent, int arg);
    virtual ~SavePointSelectTask();
    virtual void Tick(float dt);

    /* The task's name (a .sdata pointer). */
    static const char *s_taskName;

    /* 0x28 */ SelectHandler m_onSelect;
    /* 0x34 */ SelectHandler m_onCancel;
#ifdef GAME_SAVEPOINTSELECT_IMPL
    /* 0x40 */ boost::scoped_ptr<SavePointSelectImpl> m_impl;
    /* 0x44 */ u32 field_0x44;
#else
    /* 0x40 */ u8 field_0x40[0x48 - 0x40];
#endif
};
SIZE_ASSERT(SavePointSelectTask, 0x48);

} // namespace SavePointSelect

#endif

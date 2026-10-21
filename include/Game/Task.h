#ifndef SHADOW_GAME_TASK_H
#define SHADOW_GAME_TASK_H

#include <dolphin/types.h>
#include "size_assert.h"

/* Task, named by the game's RTTI. Everything that runs once per frame is a Task hanging in one
 * tree: the constructor at 0x8004EFC4 links a new task in below `parent`, the destructor at
 * 0x8004EED4 takes it out again, and the tree walk calls Tick on each task every frame.
 *
 * Offsets come from those two functions. The data members sit in front of the vtable pointer
 * because MWCC puts that pointer at the point where the class declares its first virtual
 * function. Only m_name and m_flags are named after what the game does with them; the tree links
 * are named after how the constructor wires them. */
class Task {
  public:
    Task(Task *parent);

    /* 0x00 */ const char *m_name;
    /* 0x04 */ u16 m_flags; /* 0x01 done, 0x20 deletion in progress */
    /* 0x08 */ Task *m_prevSibling;
    /* 0x0C */ Task *m_nextSibling;
    /* 0x10 */ Task *m_parentTask;
    /* 0x14 */ Task *m_firstChild;

    /* 0x18 holds the vtable pointer. */
    virtual ~Task();
    virtual void Tick(float dt);

    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ u32 field_0x24;
};
SIZE_ASSERT(Task, 0x28);

#endif

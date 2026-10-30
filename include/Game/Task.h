#ifndef SHADOW_GAME_TASK_H
#define SHADOW_GAME_TASK_H

#include <dolphin/types.h>
#include "size_assert.h"

#ifdef TASK_TIME
/* The time a task's Tick took, in timer ticks (Game/Task.cpp, which defines TASK_TIME; class name
 * inferred). Its constructor from a value and its difference are not inlined (GC/2.7 inlines no
 * function with a 64-bit parameter, nor one returning a class by value): Game/Task.cpp has their
 * copies. */
struct TaskTime {
    TaskTime() : m_value(0) {}
    TaskTime(s64 value) : m_value(value) {}

    s64 m_value;
};

inline TaskTime operator-(const TaskTime &a, const TaskTime &b) {
    TaskTime result;
    result.m_value = a.m_value - b.m_value;
    return result;
}
#endif

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
#ifdef TASK_TIME
    /* 0x20 */ TaskTime m_time; /* the time the last Tick took (Game/Task.cpp) */
#else
    /* 0x20 */ u32 field_0x20;
    /* 0x24 */ u32 field_0x24;
#endif
};
SIZE_ASSERT(Task, 0x28);

#endif

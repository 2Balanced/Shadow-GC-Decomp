/* Task (Game/Task.h): its constructor and destructor, which link a task into the tree below its
 * parent and take it out again, and the tree functions other units call by their fn_ names: moving
 * a task to another parent, the per-frame walk that ticks every live task and deletes those marked
 * done, marking a task's descendants done, counting them, and whether a task and all its parents
 * are alive. The walk can time each Tick with a timer (class name unknown) into Task::m_time. The
 * file name and all names other than the class's are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define TASK_TIME
#include "Game/Task.h"
#include <dolphin/types.h>

/* The timer that times the ticks (class name unknown; constructor, destructor and current time,
 * behaviour inferred). */
struct UnkTimer_8040ABC8;
/* An object the shutdown frees (type unknown; nothing here creates it). */
struct UnkObject_805EE1E0 {
    u8 field_0x0;
};

extern "C" {
UnkTimer_8040ABC8 *fn_8040AC3C(UnkTimer_8040ABC8 *timer);
void fn_8040AC00(UnkTimer_8040ABC8 *timer, int free);
TaskTime fn_8040ABC8(UnkTimer_8040ABC8 *timer);

void fn_8004EA40(void);
void fn_8004EA88(void);
bool fn_8004EAC0(Task *task);
void fn_8004EAEC(Task *task, int *count);
void fn_8004EB80(Task *task, Task *parent);
void fn_8004EC68(Task *task, float dt);
void fn_8004EDAC(Task *task, float dt);
void fn_8004EE88(Task *task);
}

/* The depth of the tree walk, whether it times the ticks, the number of tasks, whether the game is
 * paused (other units set it: only tasks with flag 0x100 tick then, and nothing is deleted), the
 * object and the timer the shutdown frees. */
s32 lbl_805EE1D0;
s32 lbl_805EE1D4;
s32 lbl_805EE1D8;
u8 lbl_805EE1DC;
UnkObject_805EE1E0 *lbl_805EE1E0;
UnkTimer_8040ABC8 *lbl_805EE1E4;

static const char *s_taskName = "Task";

/* Links task in as the last child of parent. */
inline void LinkTask(Task *task, Task *parent) {
    if (parent == 0) {
        task->m_prevSibling = task;
        task->m_nextSibling = 0;
    } else if (parent->m_firstChild != 0) {
        Task *first = parent->m_firstChild;
        task->m_prevSibling = first->m_prevSibling;
        task->m_nextSibling = 0;
        first->m_prevSibling->m_nextSibling = task;
        first->m_prevSibling = task;
    } else {
        task->m_prevSibling = task;
        parent->m_firstChild = task;
        task->m_nextSibling = 0;
    }
}

/* Takes task out of its parent's children (the first child's m_prevSibling is the last child). */
inline void UnlinkTask(Task *task) {
    Task *parent = task->m_parentTask;
    if (parent != 0) {
        Task *prev = task->m_prevSibling;
        if (prev == task) {
            parent->m_firstChild = 0;
        } else if (parent->m_firstChild == task) {
            parent->m_firstChild = task->m_nextSibling;
            task->m_prevSibling->m_nextSibling = 0;
            if (task->m_nextSibling != 0) {
                task->m_nextSibling->m_prevSibling = task->m_prevSibling;
            }
        } else {
            prev->m_nextSibling = task->m_nextSibling;
            if (task->m_nextSibling != 0) {
                task->m_nextSibling->m_prevSibling = task->m_prevSibling;
            } else {
                task->m_parentTask->m_firstChild->m_prevSibling = task->m_prevSibling;
            }
        }
    }
}

Task::Task(Task *parent) {
    m_flags = 0;
    m_time = TaskTime(0);
    m_parentTask = parent;
    m_firstChild = 0;
    m_name = s_taskName;
    LinkTask(this, parent);
    lbl_805EE1D8++;
}

Task::~Task() {
    m_flags |= 0x20;
    UnlinkTask(this);
    lbl_805EE1D8--;
}

/* Marks every descendant of task done. */
void fn_8004EE88(Task *task) {
    for (Task *child = task->m_firstChild; child != 0; child = child->m_nextSibling) {
        child->m_flags |= 1;
        fn_8004EE88(child);
    }
}

/* Ticks task, timing the call. */
void fn_8004EDAC(Task *task, float dt) {
    TaskTime start;
    TaskTime end;
    start = fn_8040ABC8(lbl_805EE1E4);
    task->Tick(dt);
    end = fn_8040ABC8(lbl_805EE1E4);
    task->m_time = end - start;
}

/* Ticks the children of task and their descendants, and deletes the children marked done. */
void fn_8004EC68(Task *task, float dt) {
    lbl_805EE1D0++;
    Task *child = task->m_firstChild;
    while (task->m_firstChild != 0 && child != 0) {
        u16 flags = child->m_flags;
        if (flags & 0xF) {
            if ((flags & 1) && !lbl_805EE1DC) {
                child->m_flags = flags & ~1;
                if (!(child->m_flags & 0x20)) {
                    Task *dead = child;
                    child = child->m_nextSibling;
                    while (dead->m_firstChild != 0) {
                        fn_8004EE88(dead);
                        fn_8004EC68(dead, dt);
                    }
                    delete dead;
                    continue;
                }
            }
        } else {
            if (!lbl_805EE1DC || (flags & 0x100)) {
                if (lbl_805EE1D4) {
                    fn_8004EDAC(child, dt);
                } else {
                    child->Tick(dt);
                }
            }
            fn_8004EC68(child, dt);
        }
        child = child->m_nextSibling;
    }
    lbl_805EE1D0--;
}

/* Moves task below parent. */
void fn_8004EB80(Task *task, Task *parent) {
    UnlinkTask(task);
    task->m_parentTask = parent;
    LinkTask(task, parent);
}

/* Adds the number of descendants of task to *count. */
void fn_8004EAEC(Task *task, int *count) {
    Task *grandchild;
    Task *child = task->m_firstChild;
    Task *current;
    while (task->m_firstChild != 0 && child != 0) {
        current = child;
        child = child->m_nextSibling;
        (*count)++;
        grandchild = current->m_firstChild;
        while (current->m_firstChild != 0 && grandchild != 0) {
            Task *next = grandchild;
            grandchild = grandchild->m_nextSibling;
            (*count)++;
            fn_8004EAEC(next, count);
        }
    }
}

/* Whether task and all its parents are alive (none marked done). */
bool fn_8004EAC0(Task *task) {
    while (task != 0) {
        if (task->m_flags & 1) {
            return false;
        }
        task = task->m_parentTask;
    }
    return true;
}

/* Creates the timer (called by Game/GameSystem.cpp). */
void fn_8004EA88(void) {
    UnkTimer_8040ABC8 *timer = (UnkTimer_8040ABC8 *)operator new(1);
    if (timer != 0) {
        timer = fn_8040AC3C(timer);
    }
    lbl_805EE1E4 = timer;
}

/* Frees the object and the timer (called by Game/GameSystem.cpp). */
void fn_8004EA40(void) {
    if (lbl_805EE1E0 != 0) {
        delete lbl_805EE1E0;
        lbl_805EE1E0 = 0;
    }
    fn_8040AC00(lbl_805EE1E4, 1);
    lbl_805EE1E4 = 0;
}

void Task::Tick(float dt) {}

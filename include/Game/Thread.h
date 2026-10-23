#ifndef GAME_THREAD_H
#define GAME_THREAD_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The SDK's OSThread (layout not needed here; 0x318 bytes). */
struct UnkOSThread_80377BFC {
    /* 0x000 */ u8 field_0x0[0x318];
};

/* The SDK's OSThreadQueue. */
struct UnkOSThreadQueue_80377634 {
    /* 0x0 */ void *m_head;
    /* 0x4 */ void *m_tail;
};

namespace SonicteamUSA {
namespace System {

/* Thread (class name from RTTI; vtable at 0x8055F7C0, defined in Game/Thread.cpp): an SDK thread
 * that calls Run until it returns false or the thread is stopped, then OnExit. The constructor
 * starts it at once and registers it with the thread list (fn_801E2178), whose reaper deletes it
 * once it has finished. TExpandBG and TBGRead derive from it. The data members come before the
 * virtual functions, so the vtable pointer sits at 0x328. Names other than the class's are
 * inferred. */
class Thread {
  public:
    /* 0x000 */ s32 m_id;        /* in the thread list */
    /* 0x004 */ bool m_active;   /* cleared to stop the thread */
    /* 0x005 */ bool field_0x5;
    /* 0x006 */ bool m_finished; /* set once OnExit has run */
    /* 0x008 */ s32 m_priority;
    /* 0x00C */ u8 *m_stack;
    /* 0x010 */ UnkOSThread_80377BFC m_thread;

    /* 0x328 holds the vtable pointer. */
    Thread(u32 stackSize);
    virtual ~Thread();
    /* One step of the thread's work; false ends the thread. */
    virtual bool Run();
    virtual void OnExit();
};
SIZE_ASSERT(Thread, 0x32C);

} // namespace System
} // namespace SonicteamUSA

/* The list of running threads (class name unknown; a function-local static of fn_801E2178): up to
 * 128 entries, and the queue its reaper sleeps on. */
struct UnkThreadEntry_80359530 {
    /* 0x0 */ SonicteamUSA::System::Thread *m_thread;
    /* 0x4 */ s32 m_used;
};

class UnkThreadList_801E2178 {
  public:
    UnkThreadList_801E2178();
    ~UnkThreadList_801E2178();

    /* Sleeps until woken, then deletes the threads that have finished. */
    void Reap();
    void Unregister(s32 id);
    /* Adds thread and returns its id. */
    s32 Register(SonicteamUSA::System::Thread *thread);

    /* 0x0 */ UnkThreadEntry_80359530 *m_entries;
    /* 0x4 */ s32 m_next; /* the next free entry */
    /* 0x8 */ UnkOSThreadQueue_80377634 m_queue;
};
SIZE_ASSERT(UnkThreadList_801E2178, 0x10);

extern "C" {
/* The thread list. */
UnkThreadList_801E2178 *fn_801E2178(void);
}

#endif

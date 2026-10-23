/* SonicteamUSA::System::Thread (class name from RTTI; see Game/Thread.h) and the list of running
 * threads (class name unknown): an SDK thread per object, started by the constructor and run
 * through the virtual Run, and the list a reaper walks to delete the threads that have finished.
 * File name from the class. Names other than the class's are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <string.h>

#include "Game/Thread.h"
#include <dolphin/os.h>

extern "C" {
/* The SDK's OSIsThreadTerminated, OSYieldThread, OSCreateThread, OSExitThread, OSCancelThread,
 * OSResumeThread and OSSleepThread (identified by their use here). */
BOOL fn_80377650(UnkOSThread_80377BFC *thread);
void fn_80377BC0(void);
BOOL fn_80377BFC(UnkOSThread_80377BFC *thread, void *(*func)(void *), void *param, void *stack,
                 u32 stackSize, s32 priority, u16 attr);
void fn_80377DE4(void *value);
void fn_80377EC8(UnkOSThread_80377BFC *thread);
s32 fn_80378084(UnkOSThread_80377BFC *thread);
void fn_80378568(UnkOSThreadQueue_80377634 *queue);
void OSInitThreadQueue(UnkOSThreadQueue_80377634 *queue);
}

using SonicteamUSA::System::Thread;

s32 UnkThreadList_801E2178::Register(Thread *thread) {
    BOOL level = OSDisableInterrupts();
    OSDisableScheduler();
    s32 id = m_next;
    m_entries[m_next].m_thread = thread;
    m_entries[m_next].m_used = 1;
    s32 start = m_next;
    s32 i = start;
    do {
        i++;
        if (i >= 0x80) {
            i = 0;
        }
    } while (m_entries[i].m_used != 0 && i != start);
    m_next = i;
    OSEnableScheduler();
    OSRestoreInterrupts(level);
    return id;
}

void UnkThreadList_801E2178::Unregister(s32 id) {
    m_entries[id].m_thread = 0;
    m_entries[id].m_used = 0;
}

void UnkThreadList_801E2178::Reap() {
    fn_80378568(&m_queue);
    for (int i = 0; i < 0x80; i++) {
        UnkThreadEntry_80359530 *entry = &m_entries[i];
        /* Retail tests the flag twice. */
        if (entry->m_used != 0) {
            if (entry->m_used != 0) {
                Thread *thread = entry->m_thread;
                if (thread != 0 && thread->m_finished) {
                    delete thread;
                }
            }
        }
    }
}

UnkThreadList_801E2178::UnkThreadList_801E2178() {
    m_next = 0;
    m_entries = new UnkThreadEntry_80359530[0x80];
    memset(m_entries, 0, 0x80 * sizeof(UnkThreadEntry_80359530));
    OSInitThreadQueue(&m_queue);
}

UnkThreadList_801E2178::~UnkThreadList_801E2178() {
    if (m_entries != 0) {
        delete[] m_entries;
        m_entries = 0;
    }
}

namespace SonicteamUSA {
namespace System {

static void *ThreadMain(void *param);

Thread::Thread(u32 stackSize) {
    u32 size = 0x1000;
    if (stackSize != 0) {
        size = stackSize;
    }
    m_id = -1;
    m_active = true;
    field_0x5 = false;
    m_finished = false;
    m_stack = 0;
    m_priority = 0x10;
    m_stack = new u8[size];
    memset(m_stack, 0xCD, 0x10);
    fn_80377BFC(&m_thread, ThreadMain, this, m_stack + size, size, m_priority, 1);
    fn_80378084(&m_thread);
    m_id = fn_801E2178()->Register(this);
}

Thread::~Thread() {
    m_finished = true;
    if (!fn_80377650(&m_thread)) {
        fn_80377EC8(&m_thread);
    }
    if (m_stack != 0) {
        delete[] m_stack;
        m_stack = 0;
    }
    fn_801E2178()->Unregister(m_id);
}

bool Thread::Run() {
    return true;
}

void Thread::OnExit() {}

/* The thread function: runs the thread until it stops, then exits. */
static void *ThreadMain(void *param) {
    Thread *thread = static_cast<Thread *>(param);
    if (thread != 0 && !thread->m_finished) {
        bool running = true;
        while (running && thread->m_active) {
            running = thread->Run();
            fn_80377BC0();
        }
        thread->m_active = false;
        thread->OnExit();
        thread->m_finished = true;
    }
    fn_80377DE4(0);
    return 0;
}

} // namespace System
} // namespace SonicteamUSA

/* Called around the creation of a thread by TExpandBG and TBGRead (empty in this build; names
 * unknown). */
extern "C" void fn_803591B8(void) {}

extern "C" void fn_803591B4(Thread *thread) {}

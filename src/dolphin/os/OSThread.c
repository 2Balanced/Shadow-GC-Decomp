/* Dolphin SDK OS: threads and the scheduler.
 *
 * Scheduling is strictly by priority (0 is the most urgent). Every ready thread sits in the run
 * queue of its effective priority, and RunQueueBits has bit (31 - priority) set for each
 * non-empty queue, so __cntlzw finds the most urgent one. A thread's effective priority is its
 * base priority, raised to that of the most urgent thread blocked on a mutex it holds. Changes
 * that may need a different thread to run set RunQueueHint; __OSReschedule then switches unless
 * the scheduler is disabled. With nothing ready the CPU idles on IdleContext with interrupts on.
 *
 * Game code calls several of these functions by their placeholder names, which they keep:
 * fn_80377644 OSGetCurrentThread, fn_80377650 OSIsThreadTerminated, fn_80377BC0 OSYieldThread,
 * fn_80377BFC OSCreateThread, fn_80377DE4 OSExitThread, fn_80377EC8 OSCancelThread,
 * fn_80378084 OSResumeThread, fn_8037847C OSSleepThread and fn_80378568 OSWakeupThread. */
#include <dolphin/os.h>

#define OS_ERROR_FPE 16
#define MSR_FE0 0x00000800
#define MSR_FE1 0x00000100
#define FPSCR_ENABLE 0x000000F8
#define FPSCR_NI 0x00000004
#define OS_CONTEXT_STATE_FPSAVED 0x01
#define OS_CONTEXT_STATE_EXC 0x02

#define __OSFPUContext (*(OSContext **)OSPhysicalToCached(0x00D8))
#undef __OSActiveThreadQueue
OSThreadQueue __OSActiveThreadQueue : 0x800000DC;

typedef void (*OSSwitchThreadCallback)(OSThread *from, OSThread *to);

extern u8 _stack_addr[];
extern u8 _stack_end[];
extern OSErrorHandler __OSErrorTable[];
extern u32 __OSFpscrEnableBits;

u32 OSSaveContext(OSContext *context);
void OSInitContext(OSContext *context, u32 pc, u32 newsp);
OSContext *OSGetCurrentContext(void);
u32 OSGetStackPointer(void);
void __OSUnlockAllMutex(OSThread *thread);
void OSInitThreadQueue(OSThreadQueue *queue);
void OSClearStack(u8 val);
void fn_80377DE4(void *val);
void fn_80378568(OSThreadQueue *queue);

/* Doubly linked queue operations; `link` names the OSThreadLink member used. */
#define AddTail(queue, thread, link)                                                               \
    do {                                                                                           \
        OSThread *prev_;                                                                           \
                                                                                                   \
        prev_ = (queue)->tail;                                                                     \
        if (prev_ == NULL) {                                                                       \
            (queue)->head = (thread);                                                              \
        } else {                                                                                   \
            prev_->link.next = (thread);                                                           \
        }                                                                                          \
        (thread)->link.prev = prev_;                                                               \
        (thread)->link.next = NULL;                                                                \
        (queue)->tail = (thread);                                                                  \
    } while (0)

/* Inserts before the first thread of a strictly lower priority. */
#define AddPrio(queue, thread, link)                                                               \
    do {                                                                                           \
        OSThread *prev_;                                                                           \
        OSThread *next_;                                                                           \
                                                                                                   \
        for (next_ = (queue)->head; next_ != NULL && next_->priority <= (thread)->priority;        \
             next_ = next_->link.next) {                                                           \
        }                                                                                          \
        if (next_ == NULL) {                                                                       \
            AddTail(queue, thread, link);                                                          \
        } else {                                                                                   \
            (thread)->link.next = next_;                                                           \
            prev_ = next_->link.prev;                                                              \
            next_->link.prev = (thread);                                                           \
            (thread)->link.prev = prev_;                                                           \
            if (prev_ == NULL) {                                                                   \
                (queue)->head = (thread);                                                          \
            } else {                                                                               \
                prev_->link.next = (thread);                                                       \
            }                                                                                      \
        }                                                                                          \
    } while (0)

#define RemoveItem(queue, thread, link)                                                            \
    do {                                                                                           \
        OSThread *next_;                                                                           \
        OSThread *prev_;                                                                           \
                                                                                                   \
        next_ = (thread)->link.next;                                                               \
        prev_ = (thread)->link.prev;                                                               \
        if (next_ == NULL) {                                                                       \
            (queue)->tail = prev_;                                                                 \
        } else {                                                                                   \
            next_->link.prev = prev_;                                                              \
        }                                                                                          \
        if (prev_ == NULL) {                                                                       \
            (queue)->head = next_;                                                                 \
        } else {                                                                                   \
            prev_->link.next = next_;                                                              \
        }                                                                                          \
    } while (0)

#define RemoveHead(queue, thread, link)                                                            \
    do {                                                                                           \
        OSThread *next_;                                                                           \
                                                                                                   \
        (thread) = (queue)->head;                                                                  \
        next_ = (thread)->link.next;                                                               \
        if (next_ == NULL) {                                                                       \
            (queue)->tail = NULL;                                                                  \
        } else {                                                                                   \
            next_->link.prev = NULL;                                                               \
        }                                                                                          \
        (queue)->head = next_;                                                                     \
    } while (0)

static void DefaultSwitchThreadCallback(OSThread *from, OSThread *to);

static OSSwitchThreadCallback SwitchThreadCallback = DefaultSwitchThreadCallback;

static OSThreadQueue RunQueue[OS_PRIORITY_MAX + 1];
static OSThread IdleThread;
static OSThread DefaultThread;
static OSContext IdleContext;

static volatile u32 RunQueueBits;
static volatile BOOL RunQueueHint;
static volatile s32 Reschedule;

static void DefaultSwitchThreadCallback(OSThread *from, OSThread *to) {
}

/* Turns the code that called OSInit into the default thread (priority 16, running on the
 * linker's stack) and empties every queue. */
void __OSThreadInit(void) {
    OSThread *thread = &DefaultThread;
    int prio;

    thread->state = OS_THREAD_STATE_RUNNING;
    thread->attr = OS_THREAD_ATTR_DETACH;
    thread->priority = thread->base = 16;
    thread->suspend = 0;
    thread->val = (void *)-1;
    thread->mutex = NULL;
    OSInitThreadQueue(&thread->queueJoin);
    thread->queueMutex.head = thread->queueMutex.tail = NULL;

    __OSFPUContext = &thread->context;
    OSClearContext(&thread->context);
    OSSetCurrentContext(&thread->context);
    thread->stackBase = _stack_addr;
    thread->stackEnd = (u32 *)_stack_end;
    *thread->stackEnd = OS_THREAD_STACK_MAGIC;

    SwitchThreadCallback(__OSCurrentThread, thread);
    __OSCurrentThread = thread;
    OSClearStack(0);

    RunQueueBits = 0;
    RunQueueHint = FALSE;
    for (prio = OS_PRIORITY_MIN; prio <= OS_PRIORITY_MAX; ++prio) {
        OSInitThreadQueue(&RunQueue[prio]);
    }

    OSInitThreadQueue(&__OSActiveThreadQueue);
    AddTail(&__OSActiveThreadQueue, thread, linkActive);
    OSClearContext(&IdleContext);
    Reschedule = 0;
}

void OSInitThreadQueue(OSThreadQueue *queue) {
    queue->head = queue->tail = NULL;
}

OSThread *fn_80377644(void) {
    return __OSCurrentThread;
}

BOOL fn_80377650(OSThread *thread) {
    return (thread->state == OS_THREAD_STATE_MORIBUND || thread->state == 0) ? TRUE : FALSE;
}

s32 OSDisableScheduler(void) {
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule++;
    OSRestoreInterrupts(enabled);
    return count;
}

s32 OSEnableScheduler(void) {
    BOOL enabled;
    s32 count;

    enabled = OSDisableInterrupts();
    count = Reschedule--;
    OSRestoreInterrupts(enabled);
    return count;
}

/* Takes a ready thread off its run queue. */
static void UnsetRun(OSThread *thread) {
    OSThreadQueue *queue;

    queue = thread->queue;
    RemoveItem(queue, thread, link);
    if (queue->head == NULL) {
        RunQueueBits &= ~(1u << (OS_PRIORITY_MAX - thread->priority));
    }
    thread->queue = NULL;
}

/* Puts a ready thread at the tail of the run queue of its priority. */
#define SetRun(thread)                                                                             \
    do {                                                                                           \
        (thread)->queue = &RunQueue[(thread)->priority];                                           \
        AddTail((thread)->queue, thread, link);                                                    \
        RunQueueBits |= 1u << (OS_PRIORITY_MAX - (thread)->priority);                              \
        RunQueueHint = TRUE;                                                                       \
    } while (0)

s32 __OSGetEffectivePriority(OSThread *thread) {
    s32 priority;
    OSMutex *mutex;
    OSThread *blocked;

    priority = thread->base;
    for (mutex = thread->queueMutex.head; mutex != NULL; mutex = mutex->link.next) {
        blocked = mutex->queue.head;
        if (blocked != NULL && blocked->priority < priority) {
            priority = blocked->priority;
        }
    }
    return priority;
}

/* SetEffectivePriority, OSCancelThread and OSSuspendThread are built without automatic inlining:
 * they call UnsetRun out of line. */
#pragma dont_inline on
/* Gives `thread` a new effective priority, moving it within its queue. Returns the owner of
 * the mutex the thread waits for, whose priority may change in turn. */
static OSThread *SetEffectivePriority(OSThread *thread, s32 priority) {
    switch (thread->state) {
    case OS_THREAD_STATE_READY:
        UnsetRun(thread);
        thread->priority = priority;
        SetRun(thread);
        break;
    case OS_THREAD_STATE_WAITING:
        RemoveItem(thread->queue, thread, link);
        thread->priority = priority;
        AddPrio(thread->queue, thread, link);
        if (thread->mutex != NULL) {
            return thread->mutex->thread;
        }
        break;
    case OS_THREAD_STATE_RUNNING:
        RunQueueHint = TRUE;
        thread->priority = priority;
        break;
    }
    return NULL;
}
#pragma dont_inline reset

/* Propagates a priority change along the chain of mutex owners. The functions built without
 * automatic inlining (see below) expand this by hand. */
#define UPDATE_PRIORITY(start)                                                                         do {                                                                                                   OSThread *thread_ = (start);                                                                       s32 priority_;                                                                                                                                                                                        do {                                                                                                   if (0 < thread_->suspend) {                                                                            break;                                                                                         }                                                                                                  priority_ = __OSGetEffectivePriority(thread_);                                                     if (thread_->priority == priority_) {                                                                  break;                                                                                         }                                                                                                  thread_ = SetEffectivePriority(thread_, priority_);                                            } while (thread_ != NULL);                                                                     } while (0)

static void UpdatePriority(OSThread *thread) {
    UPDATE_PRIORITY(thread);
}

/* Picks the most urgent ready thread and switches to it. Unless `yield` is set, the running
 * thread keeps the CPU when nothing more urgent is ready. Returns only in the thread that was
 * running (when no switch happens or once it is switched back to). */
static OSThread *SelectThread(BOOL yield) {
    OSContext *currentContext;
    OSThread *currentThread;
    OSThread *nextThread;
    s32 priority;
    OSThreadQueue *queue;

    if (0 < Reschedule) {
        return NULL;
    }

    currentContext = OSGetCurrentContext();
    currentThread = fn_80377644();
    if (currentContext != &currentThread->context) {
        return NULL;
    }

    if (currentThread != NULL) {
        if (currentThread->state == OS_THREAD_STATE_RUNNING) {
            if (!yield) {
                priority = __cntlzw(RunQueueBits);
                if (currentThread->priority <= priority) {
                    return NULL;
                }
            }
            currentThread->state = OS_THREAD_STATE_READY;
            SetRun(currentThread);
        }
        if (!(currentThread->context.state & OS_CONTEXT_STATE_EXC) &&
            OSSaveContext(&currentThread->context)) {
            return NULL;
        }
    }

    if (RunQueueBits == 0) {
        SwitchThreadCallback(__OSCurrentThread, NULL);
        __OSCurrentThread = NULL;
        OSSetCurrentContext(&IdleContext);
        do {
            OSEnableInterrupts();
            while (RunQueueBits == 0) {
            }
            OSDisableInterrupts();
        } while (RunQueueBits == 0);
        OSClearContext(&IdleContext);
    }

    RunQueueHint = FALSE;
    priority = __cntlzw(RunQueueBits);
    queue = &RunQueue[priority];
    RemoveHead(queue, nextThread, link);
    if (queue->head == NULL) {
        RunQueueBits &= ~(1u << (OS_PRIORITY_MAX - priority));
    }
    nextThread->queue = NULL;
    nextThread->state = OS_THREAD_STATE_RUNNING;
    SwitchThreadCallback(__OSCurrentThread, nextThread);
    __OSCurrentThread = nextThread;
    OSSetCurrentContext(&nextThread->context);
    OSLoadContext(&nextThread->context);
    return nextThread;
}

void __OSReschedule(void) {
    if (!RunQueueHint) {
        return;
    }
    SelectThread(FALSE);
}

void fn_80377BC0(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    SelectThread(TRUE);
    OSRestoreInterrupts(enabled);
}

BOOL fn_80377BFC(OSThread *thread, void *(*func)(void *), void *param, void *stack,
                 u32 stackSize, s32 priority, u16 attr) {
    BOOL enabled;
    u32 sp;
    int i;
    OSContext *context;
    u32 unused[2]; /* stack the original reserves but never touches */

    if (priority < OS_PRIORITY_MIN || OS_PRIORITY_MAX < priority) {
        return FALSE;
    }

    thread->state = OS_THREAD_STATE_READY;
    thread->attr = (u16)(attr & OS_THREAD_ATTR_DETACH);
    thread->priority = thread->base = priority;
    thread->suspend = 1;
    thread->val = (void *)-1;
    thread->mutex = NULL;
    OSInitThreadQueue(&thread->queueJoin);
    thread->queueMutex.head = thread->queueMutex.tail = NULL;

    /* An empty frame (back chain and LR save word) at the top of the stack. */
    sp = (u32)stack;
    sp &= ~7;
    sp -= 8;
    ((u32 *)sp)[0] = 0;
    ((u32 *)sp)[1] = 0;
    context = &thread->context;
    OSInitContext(context, (u32)func, sp);
    thread->context.lr = (u32)fn_80377DE4;
    thread->context.gpr[3] = (u32)param;
    thread->stackBase = stack;
    thread->stackEnd = (u32 *)((u32)stack - stackSize);
    *thread->stackEnd = OS_THREAD_STACK_MAGIC;
    thread->error = 0;
    thread->specific[0] = NULL;
    thread->specific[1] = NULL;

    enabled = OSDisableInterrupts();
    if (__OSErrorTable[OS_ERROR_FPE] != NULL) {
        thread->context.srr1 |= MSR_FE0 | MSR_FE1;
        thread->context.state |= OS_CONTEXT_STATE_FPSAVED;
        thread->context.fpscr = (__OSFpscrEnableBits & FPSCR_ENABLE) | FPSCR_NI;
        for (i = 0; i < 32; ++i) {
            *(u64 *)&thread->context.fpr[i] = (u64)0xFFFFFFFFFFFFFFFFLL;
            *(u64 *)&thread->context.psf[i] = (u64)0xFFFFFFFFFFFFFFFFLL;
        }
    }
    AddTail(&__OSActiveThreadQueue, thread, linkActive);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* Ends the calling thread (also reached by returning from the thread function). */
void fn_80377DE4(void *val) {
    BOOL enabled;
    OSThread *currentThread;

    enabled = OSDisableInterrupts();
    currentThread = fn_80377644();
    OSClearContext(&currentThread->context);
    if (currentThread->attr & OS_THREAD_ATTR_DETACH) {
        RemoveItem(&__OSActiveThreadQueue, currentThread, linkActive);
        currentThread->state = 0;
    } else {
        currentThread->state = OS_THREAD_STATE_MORIBUND;
        currentThread->val = val;
    }
    __OSUnlockAllMutex(currentThread);
    fn_80378568(&currentThread->queueJoin);
    RunQueueHint = TRUE;
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

#pragma dont_inline on
void fn_80377EC8(OSThread *thread) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    switch (thread->state) {
    case OS_THREAD_STATE_READY:
        if (!(0 < thread->suspend)) {
            UnsetRun(thread);
        }
        break;
    case OS_THREAD_STATE_RUNNING:
        RunQueueHint = TRUE;
        break;
    case OS_THREAD_STATE_WAITING:
        RemoveItem(thread->queue, thread, link);
        thread->queue = NULL;
        if (!(0 < thread->suspend) && thread->mutex != NULL) {
            UPDATE_PRIORITY(thread->mutex->thread);
        }
        break;
    default:
        OSRestoreInterrupts(enabled);
        return;
    }

    OSClearContext(&thread->context);
    if (thread->attr & OS_THREAD_ATTR_DETACH) {
        RemoveItem(&__OSActiveThreadQueue, thread, linkActive);
        thread->state = 0;
    } else {
        thread->state = OS_THREAD_STATE_MORIBUND;
    }
    __OSUnlockAllMutex(thread);
    fn_80378568(&thread->queueJoin);
    if (RunQueueHint) {
        SelectThread(FALSE);
    }
    OSRestoreInterrupts(enabled);
}
#pragma dont_inline reset

s32 fn_80378084(OSThread *thread) {
    BOOL enabled;
    s32 suspendCount;

    enabled = OSDisableInterrupts();
    suspendCount = thread->suspend--;
    if (thread->suspend < 0) {
        thread->suspend = 0;
    } else if (thread->suspend == 0) {
        switch (thread->state) {
        case OS_THREAD_STATE_READY:
            thread->priority = __OSGetEffectivePriority(thread);
            SetRun(thread);
            break;
        case OS_THREAD_STATE_WAITING:
            RemoveItem(thread->queue, thread, link);
            thread->priority = __OSGetEffectivePriority(thread);
            AddPrio(thread->queue, thread, link);
            if (thread->mutex != NULL) {
                UpdatePriority(thread->mutex->thread);
            }
            break;
        }
        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return suspendCount;
}

#pragma dont_inline on
s32 OSSuspendThread(OSThread *thread) {
    BOOL enabled;
    s32 suspendCount;

    enabled = OSDisableInterrupts();
    suspendCount = thread->suspend++;
    if (suspendCount == 0) {
        switch (thread->state) {
        case OS_THREAD_STATE_RUNNING:
            RunQueueHint = TRUE;
            thread->state = OS_THREAD_STATE_READY;
            break;
        case OS_THREAD_STATE_READY:
            UnsetRun(thread);
            break;
        case OS_THREAD_STATE_WAITING:
            /* A suspended waiter goes to the back of its queue. */
            RemoveItem(thread->queue, thread, link);
            thread->priority = 32;
            AddTail(thread->queue, thread, link);
            if (thread->mutex != NULL) {
                UPDATE_PRIORITY(thread->mutex->thread);
            }
            break;
        }
        if (RunQueueHint) {
            SelectThread(FALSE);
        }
    }
    OSRestoreInterrupts(enabled);
    return suspendCount;
}
#pragma dont_inline reset

void fn_8037847C(OSThreadQueue *queue) {
    BOOL enabled;
    OSThread *currentThread;

    enabled = OSDisableInterrupts();
    currentThread = fn_80377644();
    currentThread->state = OS_THREAD_STATE_WAITING;
    currentThread->queue = queue;
    AddPrio(queue, currentThread, link);
    RunQueueHint = TRUE;
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

void fn_80378568(OSThreadQueue *queue) {
    BOOL enabled;
    OSThread *thread;

    enabled = OSDisableInterrupts();
    while (queue->head != NULL) {
        RemoveHead(queue, thread, link);
        thread->state = OS_THREAD_STATE_READY;
        if (!(0 < thread->suspend)) {
            SetRun(thread);
        }
    }
    __OSReschedule();
    OSRestoreInterrupts(enabled);
}

BOOL OSSetThreadPriority(OSThread *thread, s32 priority) {
    BOOL enabled;

    if (priority < OS_PRIORITY_MIN || OS_PRIORITY_MAX < priority) {
        return FALSE;
    }

    enabled = OSDisableInterrupts();
    if (thread->base != priority) {
        thread->base = priority;
        UpdatePriority(thread);
        __OSReschedule();
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

s32 OSGetThreadPriority(OSThread *thread) {
    return thread->base;
}

/* Fills the unused part of the current thread's stack with `val`. */
void OSClearStack(u8 val) {
    u32 sp;
    u32 *p;
    u32 pattern;

    pattern = ((u32)val << 24) | ((u32)val << 16) | ((u32)val << 8) | (u32)val;
    sp = OSGetStackPointer();
    for (p = __OSCurrentThread->stackEnd + 1; p < (u32 *)sp; ++p) {
        *p = pattern;
    }
}

#ifndef DOLPHIN_OS_H
#define DOLPHIN_OS_H

/* The parts of the Dolphin SDK OS library's interface the decompiled SDK units use. */

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Main RAM is mapped cached at 0x80000000 and uncached at 0xC0000000. */
#define OSPhysicalToCached(paddr) ((void *)((u32)(paddr) + 0x80000000))
#define OSCachedToPhysical(caddr) ((u32)(caddr) - 0x80000000)

typedef s64 OSTime;
typedef u32 OSTick;

/* Low-memory adjustment added to the time base to give the system time. */
#define __OSSystemTime ((OSTime *)OSPhysicalToCached(0x30D8))

/* The register state saved on an exception or thread switch. */
typedef struct OSContext {
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    f64 fpr[32];
    u32 fpscr_pad;
    u32 fpscr;
    u32 srr0;
    u32 srr1;
    u16 mode;
    u16 state;
    u32 gqr[8];
    u32 psf_pad;
    f64 psf[32];
} OSContext;

typedef u16 OSError;
typedef void (*OSErrorHandler)(OSError error, OSContext *context, ...);

#define OS_ERROR_MACHINE_CHECK 1

void OSReport(const char *msg, ...);
void OSDumpContext(OSContext *context);
OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler);

/* First-level exception handlers, indexed by exception number (4: external, 8: decrementer). */
typedef u8 __OSException;
typedef void (*__OSExceptionHandler)(__OSException exception, OSContext *context);
__OSExceptionHandler __OSSetExceptionHandler(__OSException exception, __OSExceptionHandler handler);
__OSExceptionHandler __OSGetExceptionHandler(__OSException exception);

/* Saves what the exception prologue left unsaved (r0-r2, r6-r31, GQR1-7) into `context`; for
 * use at the start of an `asm` second-level handler. GQR1-7 are SPRs 913-919. */
#define OS_EXCEPTION_SAVE_GPRS(context)                                                            \
    stw r0, OSContext.gpr[0](context);                                                             \
    stw r1, OSContext.gpr[1](context);                                                             \
    stw r2, OSContext.gpr[2](context);                                                             \
    stmw r6, OSContext.gpr[6](context);                                                            \
    mfspr r0, 913;                                                                                 \
    stw r0, OSContext.gqr[1](context);                                                             \
    mfspr r0, 914;                                                                                 \
    stw r0, OSContext.gqr[2](context);                                                             \
    mfspr r0, 915;                                                                                 \
    stw r0, OSContext.gqr[3](context);                                                             \
    mfspr r0, 916;                                                                                 \
    stw r0, OSContext.gqr[4](context);                                                             \
    mfspr r0, 917;                                                                                 \
    stw r0, OSContext.gqr[5](context);                                                             \
    mfspr r0, 918;                                                                                 \
    stw r0, OSContext.gqr[6](context);                                                             \
    mfspr r0, 919;                                                                                 \
    stw r0, OSContext.gqr[7](context)

void OSLoadContext(OSContext *context);
void OSClearContext(OSContext *context);
void OSSetCurrentContext(OSContext *context);

s32 OSDisableScheduler(void);
s32 OSEnableScheduler(void);
void __OSReschedule(void);

BOOL OSDisableInterrupts(void);
BOOL OSEnableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);

OSTime OSGetTime(void);
OSTick OSGetTick(void);
OSTime __OSGetSystemTime(void);

/* A one-shot or periodic timer; the handler runs from the decrementer exception. */
typedef struct OSAlarm OSAlarm;
typedef void (*OSAlarmHandler)(OSAlarm *alarm, OSContext *context);
struct OSAlarm {
    OSAlarmHandler handler; /* null while the alarm isn't queued */
    u32 tag;
    OSTime fire;
    OSAlarm *prev;
    OSAlarm *next;
    OSTime period; /* 0 for a one-shot alarm */
    OSTime start;
};

void OSInitAlarm(void);
void OSCreateAlarm(OSAlarm *alarm);
void OSSetAlarm(OSAlarm *alarm, OSTime tick, OSAlarmHandler handler);
void OSCancelAlarm(OSAlarm *alarm);

/* Functions run before a reset, lowest priority value first; called again with final = TRUE. */
typedef BOOL (*OSResetFunction)(BOOL final);
typedef struct OSResetFunctionInfo OSResetFunctionInfo;
struct OSResetFunctionInfo {
    OSResetFunction func;
    u32 priority;
    OSResetFunctionInfo *next;
    OSResetFunctionInfo *prev;
};

void OSRegisterResetFunction(OSResetFunctionInfo *info);

void DCEnable(void);
void DCInvalidateRange(void *addr, u32 nBytes);
void DCFlushRange(void *addr, u32 nBytes);
void DCStoreRange(void *addr, u32 nBytes);
void DCFlushRangeNoSync(void *addr, u32 nBytes);
void ICInvalidateRange(void *addr, u32 nBytes);
void ICFlashInvalidate(void);
void ICEnable(void);
void LCDisable(void);
void L2GlobalInvalidate(void);

void DBPrintf(char *str, ...);

/* Threads. Every thread is on the active list (head at 0x800000DC); a runnable one also sits in
 * the run queue of its priority, a waiting one in the queue it sleeps on. */
typedef struct OSThread OSThread;
typedef struct OSMutex OSMutex;

typedef struct OSThreadQueue {
    OSThread *head;
    OSThread *tail;
} OSThreadQueue;

typedef struct OSThreadLink {
    OSThread *next;
    OSThread *prev;
} OSThreadLink;

typedef struct OSMutexQueue {
    OSMutex *head;
    OSMutex *tail;
} OSMutexQueue;

typedef struct OSMutexLink {
    OSMutex *next;
    OSMutex *prev;
} OSMutexLink;

struct OSMutex {
    OSThreadQueue queue;
    OSThread *thread; /* owner, null while unlocked */
    s32 count;        /* recursion depth */
    OSMutexLink link; /* in the owner's queueMutex */
};

struct OSThread {
    OSContext context;
    u16 state;
    u16 attr;
    s32 suspend;   /* > 0 while suspended */
    s32 priority;  /* effective priority (raised by priority inheritance) */
    s32 base;      /* priority given at creation or by OSSetThreadPriority */
    void *val;     /* exit value */
    OSThreadQueue *queue; /* queue the thread is linked into */
    OSThreadLink link;
    OSThreadQueue queueJoin;
    OSMutex *mutex;          /* mutex the thread is waiting for */
    OSMutexQueue queueMutex; /* mutexes the thread holds */
    OSThreadLink linkActive;
    u8 *stackBase;
    u32 *stackEnd;
    s32 error;
    void *specific[2];
};

/* OSThread.state */
#define OS_THREAD_STATE_READY 1
#define OS_THREAD_STATE_RUNNING 2
#define OS_THREAD_STATE_WAITING 4
#define OS_THREAD_STATE_MORIBUND 8

/* OSThread.attr */
#define OS_THREAD_ATTR_DETACH 0x0001

#define OS_PRIORITY_MIN 0
#define OS_PRIORITY_MAX 31
#define OS_PRIORITY_IDLE OS_PRIORITY_MAX

#define OS_THREAD_STACK_MAGIC 0xDEADBABE

/* Per-thread data the OS keeps in low memory. */
#define __OSActiveThreadQueue (*(OSThreadQueue *)OSPhysicalToCached(0x00DC))
#define __OSCurrentThread (*(OSThread **)OSPhysicalToCached(0x00E4))


#ifdef __cplusplus
}
#endif

#endif

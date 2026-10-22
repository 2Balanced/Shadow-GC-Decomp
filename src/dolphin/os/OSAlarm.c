/* Dolphin SDK OS: one-shot and periodic alarms driven by the decrementer.
 *
 * Every armed alarm sits in a doubly linked list ordered by its fire time. Only the earliest one
 * is ever loaded into the decrementer; when that exception arrives the head is unlinked, periodic
 * alarms are put back at their next due time, and the handler runs. OSSetAbsAlarm,
 * OSSetPeriodicAlarm, OSSetAlarmTag, OSCancelAlarms and OSCheckAlarmQueue are not referenced by
 * the game and were dead-stripped. */
#include <dolphin/base/PPCArch.h>
#include <dolphin/os.h>

#define DECREMENTER_EXCEPTION 8
/* The decrementer interrupts on the 0 -> -1 step, so it holds at most a positive s32. */
#define DECREMENTER_LIMIT 0x80000000
#define DECREMENTER_MAX 0x7FFFFFFF

extern BOOL __DVDTestAlarm(OSAlarm *);

/* Armed alarms, earliest first. */
typedef struct PendingList {
    OSAlarm *first;
    OSAlarm *last;
} PendingList;

static void DecrementerExceptionHandler(__OSException, OSContext *);
static BOOL OnReset(BOOL);

static OSResetFunctionInfo ResetFunctionInfo = {OnReset, 0xFFFFFFFF, NULL, NULL};
static PendingList AlarmQueue;

/* Loads the decrementer with the ticks left until `a` is due (zero when overdue, saturated when
 * it is further away than the counter can express). */
static void SetTimer(OSAlarm *a) {
    OSTime left;

    left = a->fire - __OSGetSystemTime();
    if (left < 0) {
        PPCMtdec(0);
    } else if (left < DECREMENTER_LIMIT) {
        PPCMtdec((u32)left);
    } else {
        PPCMtdec(DECREMENTER_MAX);
    }
}

void OSInitAlarm(void) {
    if (__OSGetExceptionHandler(DECREMENTER_EXCEPTION) == DecrementerExceptionHandler) {
        return;
    }
    AlarmQueue.first = AlarmQueue.last = NULL;
    __OSSetExceptionHandler(DECREMENTER_EXCEPTION, DecrementerExceptionHandler);
    OSRegisterResetFunction(&ResetFunctionInfo);
}

void OSCreateAlarm(OSAlarm *a) {
    a->handler = NULL;
    a->tag = 0;
}

/* Arms `a` for `when` (periodic alarms instead use the first start + n * period still ahead) and
 * links it before the first alarm due strictly later. A new head reprograms the decrementer. */
static void InsertAlarm(OSAlarm *a, OSTime when, OSAlarmHandler fn) {
    OSAlarm *cur;
    OSAlarm *before;

    if (a->period > 0) {
        OSTime now = __OSGetSystemTime();
        when = a->start;
        if (now > a->start) {
            when += a->period * (1 + (now - a->start) / a->period);
        }
    }

    a->handler = fn;
    a->fire = when;

    cur = AlarmQueue.first;
    while (cur != NULL) {
        if (when < cur->fire) {
            a->prev = cur->prev;
            cur->prev = a;
            a->next = cur;
            before = a->prev;
            if (before != NULL) {
                before->next = a;
            } else {
                AlarmQueue.first = a;
                SetTimer(a);
            }
            return;
        }
        cur = cur->next;
    }

    /* Due after everything already queued: append. */
    a->next = NULL;
    before = AlarmQueue.last;
    AlarmQueue.last = a;
    a->prev = before;
    if (before != NULL) {
        before->next = a;
    } else {
        /* The queue was empty. */
        AlarmQueue.first = AlarmQueue.last = a;
        SetTimer(a);
    }
}

void OSSetAlarm(OSAlarm *a, OSTime delay, OSAlarmHandler fn) {
    BOOL level = OSDisableInterrupts();

    a->period = 0;
    InsertAlarm(a, delay + __OSGetSystemTime(), fn);
    OSRestoreInterrupts(level);
}

void OSCancelAlarm(OSAlarm *a) {
    BOOL level = OSDisableInterrupts();
    OSAlarm *after;

    /* Not armed: nothing to unlink. */
    if (a->handler == NULL) {
        OSRestoreInterrupts(level);
        return;
    }

    after = a->next;
    if (after == NULL) {
        AlarmQueue.last = a->prev;
    } else {
        after->prev = a->prev;
    }

    if (a->prev != NULL) {
        a->prev->next = after;
    } else {
        /* It was the head: the decrementer now belongs to its successor. */
        AlarmQueue.first = after;
        if (after != NULL) {
            SetTimer(after);
        }
    }
    a->handler = NULL;
    OSRestoreInterrupts(level);
}

/* Entered from DecrementerExceptionHandler with the interrupted GPRs saved in `ctx`. A spurious
 * or early interrupt just re-arms and resumes; otherwise the head alarm is retired and its
 * handler called on a scratch context (so it may touch the FPU) with the scheduler held. */
static void DecrementerExceptionCallback(register __OSException exc, register OSContext *ctx) {
    OSContext scratch;
    OSAlarm *due;
    OSAlarm *rest;
    OSAlarmHandler fn;
    OSTime now;

    now = __OSGetSystemTime();
    due = AlarmQueue.first;
    if (due == NULL) {
        OSLoadContext(ctx);
    }
    if (due->fire > now) {
        SetTimer(due);
        OSLoadContext(ctx);
    }

    rest = due->next;
    AlarmQueue.first = rest;
    if (rest == NULL) {
        AlarmQueue.last = NULL;
    } else {
        rest->prev = NULL;
    }

    fn = due->handler;
    due->handler = NULL;
    if (due->period > 0) {
        InsertAlarm(due, 0, fn);
    }
    if (AlarmQueue.first != NULL) {
        SetTimer(AlarmQueue.first);
    }

    OSDisableScheduler();
    OSClearContext(&scratch);
    OSSetCurrentContext(&scratch);
    fn(due, ctx);
    OSClearContext(&scratch);
    OSSetCurrentContext(ctx);
    OSEnableScheduler();
    __OSReschedule();
    OSLoadContext(ctx);
}

/* Saves the volatile GPRs into the interrupted context and tail-calls the C callback. */
static asm void DecrementerExceptionHandler(register __OSException exception,
                                            register OSContext *context) {
    nofralloc
    OS_EXCEPTION_SAVE_GPRS(context)
    stwu r1, -8(r1)
    b DecrementerExceptionCallback
}

/* Last reset pass: cancel everything still queued except the DVD driver's own alarms. */
static BOOL OnReset(BOOL final) {
    OSAlarm *a;
    OSAlarm *following;

    if (final) {
        a = AlarmQueue.first;
        following = (a != NULL) ? a->next : NULL;
        for (; a != NULL; a = following, following = (a != NULL) ? a->next : NULL) {
            if (!__DVDTestAlarm(a)) {
                OSCancelAlarm(a);
            }
        }
    }
    return TRUE;
}

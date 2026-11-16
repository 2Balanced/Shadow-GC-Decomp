/* Dolphin SDK DSP: the CPU side of the DSP mailboxes and the public task calls.
 *
 * The CPU and DSP each have a 32-bit mailbox, written as two halfwords (the high half carries the
 * "full" flag). Tasks are queued by priority in dsp_task.c; DSPAssertTask asks the running task to
 * yield to a more urgent one by raising the DSP's CPU-interrupt request. */
#include <dolphin/os_interrupt.h>
#include <dolphin/dsp.h>

volatile u16 __DSPRegs[] : 0xCC005000;

#define DSP_TASK_FLAG_ATTACHED 1
#define DSP_TASK_FLAG_CANCEL 2
#define DSP_TASK_STATE_INIT 0
#define DSP_TASK_STATE_RUN 1

void __DSP_debug_printf(const char *fmt, ...);
void __DSPHandler(__OSInterrupt interrupt, OSContext *context);
void __DSP_boot_task(DSPTaskInfo *task);
void __DSP_insert_task(DSPTaskInfo *task);

extern BOOL __DSP_rude_task_pending;
extern DSPTaskInfo *__DSP_rude_task;
extern DSPTaskInfo *__DSP_tmp_task;
extern DSPTaskInfo *__DSP_last_task;
extern DSPTaskInfo *__DSP_first_task;
extern DSPTaskInfo *__DSP_curr_task;

const char *__DSPVersion = "<< Dolphin SDK - DSP\trelease build: Apr  5 2004 04:15:32 (0x2301) >>";

static BOOL __DSP_init_flag;

u32 DSPCheckMailToDSP(void) { return (__DSPRegs[0] >> 15) & 1; }

u32 DSPCheckMailFromDSP(void) { return (__DSPRegs[2] >> 15) & 1; }

u32 DSPReadMailFromDSP(void) { return (__DSPRegs[2] << 16) | __DSPRegs[3]; }

void DSPSendMailToDSP(u32 mail) {
    __DSPRegs[0] = (u16)(mail >> 16);
    __DSPRegs[1] = (u16)mail;
}

void DSPAssertInt(void) {
    BOOL enabled = OSDisableInterrupts();
    u16 status = __DSPRegs[5];

    status = (u16)((status & ~0xA8) | 0x2);
    __DSPRegs[5] = status;
    OSRestoreInterrupts(enabled);
}

void DSPInit(void) {
    BOOL enabled;
    u16 status;

    __DSP_debug_printf("DSPInit(): Build Date: %s %s\n", "Apr  5 2004", "04:15:32");
    if (__DSP_init_flag == TRUE) {
        return;
    }
    OSRegisterVersion(__DSPVersion);
    enabled = OSDisableInterrupts();
    __OSSetInterruptHandler(7, __DSPHandler);
    __OSUnmaskInterrupts(0x01000000);
    status = __DSPRegs[5];
    status = (u16)((status & ~0xA8) | 0x800);
    __DSPRegs[5] = status;
    status = __DSPRegs[5];
    status = (u16)(status & ~0xAC);
    __DSPRegs[5] = status;
    __DSP_tmp_task = NULL;
    __DSP_curr_task = NULL;
    __DSP_last_task = NULL;
    __DSP_first_task = NULL;
    __DSP_init_flag = TRUE;
    OSRestoreInterrupts(enabled);
}

BOOL DSPCheckInit(void) { return __DSP_init_flag; }

DSPTaskInfo *DSPAddTask(DSPTaskInfo *task) {
    BOOL enabled = OSDisableInterrupts();

    __DSP_insert_task(task);
    task->state = DSP_TASK_STATE_INIT;
    task->flags = DSP_TASK_FLAG_ATTACHED;
    OSRestoreInterrupts(enabled);
    if (task == __DSP_first_task) {
        __DSP_boot_task(task);
    }
    return task;
}

DSPTaskInfo *DSPCancelTask(DSPTaskInfo *task) {
    BOOL enabled = OSDisableInterrupts();

    task->flags |= DSP_TASK_FLAG_CANCEL;
    OSRestoreInterrupts(enabled);
    return task;
}

/* Make `task` run next: at once if it outranks the running task, by interrupting that one. */
DSPTaskInfo *DSPAssertTask(DSPTaskInfo *task) {
    BOOL enabled = OSDisableInterrupts();

    if (__DSP_curr_task == task) {
        __DSP_rude_task = task;
        __DSP_rude_task_pending = TRUE;
        OSRestoreInterrupts(enabled);
        return task;
    }
    if (task->priority < __DSP_curr_task->priority) {
        __DSP_rude_task = task;
        __DSP_rude_task_pending = TRUE;
        if (__DSP_curr_task->state == DSP_TASK_STATE_RUN) {
            DSPAssertInt();
        }
        OSRestoreInterrupts(enabled);
        return task;
    }
    OSRestoreInterrupts(enabled);
    return NULL;
}

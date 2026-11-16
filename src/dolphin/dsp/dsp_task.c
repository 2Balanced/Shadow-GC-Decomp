/* Dolphin SDK DSP: the task switcher.
 *
 * Tasks sit on a priority-ordered list. The running DSP program reports through its mailbox
 * (0xDCD1000x) that it has started, resumed, wants to yield, is done, or wants service; the
 * interrupt handler answers (0xCDD1000x) and, when another task is due, hands the DSP the outgoing
 * task's data-RAM image to save and the incoming task's program to load. */
#include <dolphin/os_interrupt.h>
#include <dolphin/dsp.h>

volatile u16 __DSPRegs[] : 0xCC005000;

#define DSP_TASK_FLAG_CANCEL 2
#define DSP_TASK_STATE_INIT 0
#define DSP_TASK_STATE_RUN 1
#define DSP_TASK_STATE_YIELD 2
#define DSP_TASK_STATE_DONE 3

u32 DSPCheckMailToDSP(void);
u32 DSPCheckMailFromDSP(void);
u32 DSPReadMailFromDSP(void);
void DSPSendMailToDSP(u32 mail);
void __DSP_debug_printf(const char *fmt, ...);

void __DSP_exec_task(DSPTaskInfo *current, DSPTaskInfo *next);
void __DSP_remove_task(DSPTaskInfo *task);

/* Emitted in reverse: __DSP_rude_task_pending ends up lowest in .sbss. */
DSPTaskInfo *__DSP_curr_task;
DSPTaskInfo *__DSP_first_task;
DSPTaskInfo *__DSP_last_task;
DSPTaskInfo *__DSP_tmp_task;
DSPTaskInfo *__DSP_rude_task;
BOOL __DSP_rude_task_pending;

#define SEND(mail)                                                                                 \
    DSPSendMailToDSP(mail);                                                                        \
    while (DSPCheckMailToDSP()) {                                                                  \
    }

void __DSPHandler(__OSInterrupt interrupt, OSContext *context) {
    u32 unused[2]; /* never touched; retail's frame has room for it */
    OSContext exceptionContext;
    u16 status;
    u32 mail;

    status = __DSPRegs[5];
    status = (u16)((status & ~0x28) | 0x80);
    __DSPRegs[5] = status;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);

    while (!DSPCheckMailFromDSP()) {
    }
    mail = DSPReadMailFromDSP();
    if ((__DSP_curr_task->flags & DSP_TASK_FLAG_CANCEL) && mail == 0xDCD10002) {
        mail = 0xDCD10003;
    }

    switch (mail) {
    case 0xDCD10000:
        __DSP_curr_task->state = DSP_TASK_STATE_RUN;
        if (__DSP_curr_task->init_cb) {
            __DSP_curr_task->init_cb(__DSP_curr_task);
        }
        break;
    case 0xDCD10001:
        __DSP_curr_task->state = DSP_TASK_STATE_RUN;
        if (__DSP_curr_task->res_cb) {
            __DSP_curr_task->res_cb(__DSP_curr_task);
        }
        break;
    case 0xDCD10002:
        if (__DSP_rude_task_pending) {
            if (__DSP_curr_task == __DSP_rude_task) {
                SEND(0xCDD10003);
                __DSP_rude_task = NULL;
                __DSP_rude_task_pending = FALSE;
                if (__DSP_curr_task->res_cb) {
                    __DSP_curr_task->res_cb(__DSP_curr_task);
                }
            } else {
                SEND(0xCDD10001);
                __DSP_exec_task(__DSP_curr_task, __DSP_rude_task);
                __DSP_curr_task->state = DSP_TASK_STATE_YIELD;
                __DSP_curr_task = __DSP_rude_task;
                __DSP_rude_task_pending = FALSE;
                __DSP_rude_task = NULL;
            }
        } else if (__DSP_curr_task->next == NULL) {
            if (__DSP_curr_task == __DSP_first_task) {
                SEND(0xCDD10003);
                if (__DSP_curr_task->res_cb) {
                    __DSP_curr_task->res_cb(__DSP_curr_task);
                }
            } else {
                SEND(0xCDD10001);
                __DSP_exec_task(__DSP_curr_task, __DSP_first_task);
                __DSP_curr_task->state = DSP_TASK_STATE_YIELD;
                __DSP_curr_task = __DSP_first_task;
            }
        } else {
            SEND(0xCDD10001);
            __DSP_exec_task(__DSP_curr_task, __DSP_curr_task->next);
            __DSP_curr_task->state = DSP_TASK_STATE_YIELD;
            __DSP_curr_task = __DSP_curr_task->next;
        }
        break;
    case 0xDCD10003:
        if (__DSP_rude_task_pending) {
            if (__DSP_curr_task->done_cb) {
                __DSP_curr_task->done_cb(__DSP_curr_task);
            }
            SEND(0xCDD10001);
            __DSP_exec_task(NULL, __DSP_rude_task);
            __DSP_remove_task(__DSP_curr_task);
            __DSP_curr_task = __DSP_rude_task;
            __DSP_rude_task_pending = FALSE;
            __DSP_rude_task = NULL;
        } else if (__DSP_curr_task->next == NULL) {
            if (__DSP_curr_task == __DSP_first_task) {
                if (__DSP_curr_task->done_cb) {
                    __DSP_curr_task->done_cb(__DSP_curr_task);
                }
                SEND(0xCDD10002);
                __DSP_curr_task->state = DSP_TASK_STATE_DONE;
                __DSP_remove_task(__DSP_curr_task);
            } else {
                if (__DSP_curr_task->done_cb) {
                    __DSP_curr_task->done_cb(__DSP_curr_task);
                }
                SEND(0xCDD10001);
                __DSP_curr_task->state = DSP_TASK_STATE_DONE;
                __DSP_exec_task(NULL, __DSP_first_task);
                __DSP_curr_task = __DSP_first_task;
                __DSP_remove_task(__DSP_last_task);
            }
        } else {
            if (__DSP_curr_task->done_cb) {
                __DSP_curr_task->done_cb(__DSP_curr_task);
            }
            SEND(0xCDD10001);
            __DSP_curr_task->state = DSP_TASK_STATE_DONE;
            __DSP_exec_task(NULL, __DSP_curr_task->next);
            __DSP_curr_task = __DSP_curr_task->next;
            __DSP_remove_task(__DSP_curr_task->prev);
        }
        break;
    case 0xDCD10004:
        if (__DSP_curr_task->req_cb) {
            __DSP_curr_task->req_cb(__DSP_curr_task);
        }
        break;
    }

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

/* Tell the DSP where to save the outgoing task's data RAM and what to load and start next. */
void __DSP_exec_task(DSPTaskInfo *current, DSPTaskInfo *next) {
    if (current) {
        SEND((u32)current->dram_mmem_addr);
        SEND(current->dram_length);
        SEND(current->dram_addr);
    } else {
        SEND(0);
        SEND(0);
        SEND(0);
    }
    SEND((u32)next->iram_mmem_addr);
    SEND(next->iram_length);
    SEND(next->iram_addr);
    if (next->state == DSP_TASK_STATE_INIT) {
        SEND(next->dsp_init_vector);
        SEND(0);
        SEND(0);
        SEND(0);
    } else {
        SEND(next->dsp_resume_vector);
        SEND((u32)next->dram_mmem_addr);
        SEND(next->dram_length);
        SEND(next->dram_addr);
    }
}

/* Cold start: feed the DSP's boot ROM the first task's program. */
void __DSP_boot_task(DSPTaskInfo *task) {
    volatile u32 mail;

    while (!DSPCheckMailFromDSP()) {
    }
    mail = DSPReadMailFromDSP();

    SEND(0x80F3A001);
    SEND((u32)task->iram_mmem_addr);
    SEND(0x80F3C002);
    SEND(task->iram_addr & 0xFFFF);
    SEND(0x80F3A002);
    SEND(task->iram_length);
    SEND(0x80F3B002);
    SEND(0);
    SEND(0x80F3D001);
    SEND(task->dsp_init_vector);

    __DSP_debug_printf("DSP is booting task: 0x%08X\n", task);
    __DSP_debug_printf("__DSP_boot_task()  : IRAM MMEM ADDR: 0x%08X\n", task->iram_mmem_addr);
    __DSP_debug_printf("__DSP_boot_task()  : IRAM DSP ADDR : 0x%08X\n", task->iram_addr);
    __DSP_debug_printf("__DSP_boot_task()  : IRAM LENGTH   : 0x%08X\n", task->iram_length);
    __DSP_debug_printf("__DSP_boot_task()  : DRAM MMEM ADDR: 0x%08X\n", task->dram_length);
    __DSP_debug_printf("__DSP_boot_task()  : Start Vector  : 0x%08X\n", task->dsp_init_vector);
}

/* Appends without regard to priority. Nothing calls it any more; only its message is linked. */
void __DSP_add_task(DSPTaskInfo *task) {
    if (__DSP_last_task == NULL) {
        __DSP_first_task = __DSP_last_task = __DSP_curr_task = task;
        task->next = task->prev = NULL;
    } else {
        __DSP_last_task->next = task;
        task->next = NULL;
        task->prev = __DSP_last_task;
        __DSP_last_task = task;
    }
    task->state = DSP_TASK_STATE_INIT;
    __DSP_debug_printf("__DSP_add_task() : Added task    : 0x%08X\n", task);
}

void __DSP_insert_task(DSPTaskInfo *task) {
    DSPTaskInfo *temp;

    if (__DSP_first_task == NULL) {
        __DSP_first_task = __DSP_last_task = __DSP_curr_task = task;
        task->next = task->prev = NULL;
        return;
    }
    temp = __DSP_first_task;
    while (temp) {
        if (task->priority < temp->priority) {
            task->prev = temp->prev;
            temp->prev = task;
            task->next = temp;
            if (task->prev == NULL) {
                __DSP_first_task = task;
            } else {
                task->prev->next = task;
            }
            break;
        }
        temp = temp->next;
    }
    if (temp == NULL) {
        __DSP_last_task->next = task;
        task->next = NULL;
        task->prev = __DSP_last_task;
        __DSP_last_task = task;
    }
}

void __DSP_remove_task(DSPTaskInfo *task) {
    task->flags = 0;
    task->state = DSP_TASK_STATE_DONE;
    if (__DSP_first_task == task) {
        if (task->next) {
            __DSP_first_task = task->next;
            task->next->prev = NULL;
        } else {
            __DSP_first_task = __DSP_last_task = __DSP_curr_task = NULL;
        }
    } else if (__DSP_last_task == task) {
        __DSP_last_task = task->prev;
        task->prev->next = NULL;
        __DSP_curr_task = __DSP_first_task;
    } else {
        __DSP_curr_task = task->next;
        task->prev->next = task->next;
        task->next->prev = task->prev;
    }
}

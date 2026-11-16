#ifndef DOLPHIN_DSP_H
#define DOLPHIN_DSP_H

/* The Dolphin SDK DSP library: mailboxes to and from the audio DSP and its simple task switcher. */

#include <dolphin/os.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*DSPCallback)(void *task);

/* One program the DSP can run; the CPU side swaps tasks in and out through the mailboxes. */
typedef struct DSPTaskInfo DSPTaskInfo;
struct DSPTaskInfo {
    vu32 state;              /* 0x00 */
    vu32 priority;           /* 0x04 */
    vu32 flags;              /* 0x08 */
    u16 *iram_mmem_addr;     /* 0x0C */
    u32 iram_length;         /* 0x10 */
    u32 iram_addr;           /* 0x14 */
    u16 *dram_mmem_addr;     /* 0x18 */
    u32 dram_length;         /* 0x1C */
    u32 dram_addr;           /* 0x20 */
    u16 dsp_init_vector;     /* 0x24 */
    u16 dsp_resume_vector;   /* 0x26 */
    DSPCallback init_cb;     /* 0x28 */
    DSPCallback res_cb;      /* 0x2C */
    DSPCallback done_cb;     /* 0x30 */
    DSPCallback req_cb;      /* 0x34 */
    DSPTaskInfo *next;       /* 0x38 */
    DSPTaskInfo *prev;       /* 0x3C */
    OSTime t_context;        /* 0x40 */
    OSTime t_task;           /* 0x48 */
};

#ifdef __cplusplus
}
#endif

#endif

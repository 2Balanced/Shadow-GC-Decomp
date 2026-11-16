/* Dolphin SDK OS: reports and error handling.
 *
 * OSReport prints through MSL's vprintf. Every exception the OS does not handle itself ends in
 * __OSUnhandledException, which hands it to the handler the application registered with
 * OSSetErrorHandler, or else dumps the context and halts. Floating-point exceptions get their own
 * error number (OS_ERROR_FPE): installing a handler for it turns on FP exceptions in every
 * thread's saved MSR and FPSCR. */
#include <dolphin/base/PPCArch.h>
#include <dolphin/os.h>

typedef struct __va_list_struct {
    char gpr;
    char fpr;
    char reserved[2];
    char *input_arg_area;
    char *reg_save_area;
} va_list[1];
#define va_start(ap, fmt) ((void)fmt, __builtin_va_info(&ap))
#define va_end(ap) ((void)0)

int vprintf(const char *format, va_list arg);

volatile u16 __DSPRegs[] : 0xCC005000;
volatile u32 __DIRegs[] : 0xCC006000;

#define OS_ERROR_FPE 16
#define OS_ERROR_MAX (OS_ERROR_FPE + 1)

/* Exception numbers this file treats specially. */
#define EXCEPTION_DSI 2
#define EXCEPTION_ISI 3
#define EXCEPTION_ALIGNMENT 5
#define EXCEPTION_PROGRAM 6
#define EXCEPTION_DECREMENTER 8
#define EXCEPTION_MEMORY_PROTECTION 15

#define MSR_FE0 0x00000800
#define MSR_FE1 0x00000100
#define MSR_FP 0x00002000
#define MSR_RI 0x00000002
#define SRR1_FP_EXCEPTION 0x00100000 /* program exception caused by an FP enabled exception */

/* FPSCR: exception enable bits (VE OE UE ZE XE), non-IEEE mode, and every bit that is not a
 * sticky exception flag. */
#define FPSCR_ENABLE 0x000000F8
#define FPSCR_NI 0x00000004
#define FPSCR_CLEAR_EXCEPTIONS 0x6005F8FF

#define OS_CONTEXT_STATE_FPSAVED 0x01

#define __OSFPUContext (*(OSContext **)OSPhysicalToCached(0x00D8))

extern volatile OSTime __OSLastInterruptTime;
extern volatile s16 __OSLastInterrupt;
extern volatile u32 __OSLastInterruptSrr0;

u32 OSGetStackPointer(void);
void OSSaveFPUContext(OSContext *context);

OSErrorHandler __OSErrorTable[OS_ERROR_MAX];
u32 __OSFpscrEnableBits = FPSCR_ENABLE;

void OSReport(const char *msg, ...) {
    va_list marker;

    va_start(marker, msg);
    vprintf(msg, marker);
    va_end(marker);
}

/* The SDK's OSPanic: prints the message, where it came from and a stack trace, then halts. */
void fn_80372404(const char *file, int line, const char *msg, ...) {
    va_list marker;
    u32 i;
    u32 *p;

    OSDisableInterrupts();
    va_start(marker, msg);
    vprintf(msg, marker);
    va_end(marker);
    OSReport(" in \"%s\" on line %d.\n", file, line);

    OSReport("\nAddress:      Back Chain    LR Save\n");
    for (i = 0, p = (u32 *)OSGetStackPointer(); p != NULL && (u32)p != 0xFFFFFFFF && i++ < 16;
         p = (u32 *)*p) {
        OSReport("0x%08x:   0x%08x    0x%08x\n", p, p[0], p[1]);
    }
    PPCHalt();
}

OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler) {
    OSErrorHandler oldHandler;
    BOOL enabled;
    u32 msr;
    u32 fpscr;
    OSThread *thread;
    int i;

    enabled = OSDisableInterrupts();
    oldHandler = __OSErrorTable[error];
    __OSErrorTable[error] = handler;

    if (error == OS_ERROR_FPE) {
        msr = PPCMfmsr();
        PPCMtmsr(msr | MSR_FP);
        fpscr = PPCMffpscr();
        if (handler != NULL) {
            /* Threads that never used the FPU get a signalling-NaN register file, so stray use
             * of an uninitialised register traps. */
            for (thread = __OSActiveThreadQueue.head; thread != NULL;
                 thread = thread->linkActive.next) {
                thread->context.srr1 |= MSR_FE0 | MSR_FE1;
                if ((thread->context.state & OS_CONTEXT_STATE_FPSAVED) == 0) {
                    thread->context.state |= OS_CONTEXT_STATE_FPSAVED;
                    for (i = 0; i < 32; ++i) {
                        *(u64 *)&thread->context.fpr[i] = (u64)0xFFFFFFFFFFFFFFFFLL;
                        *(u64 *)&thread->context.psf[i] = (u64)0xFFFFFFFFFFFFFFFFLL;
                    }
                    thread->context.fpscr = FPSCR_NI;
                }
                thread->context.fpscr |= __OSFpscrEnableBits & FPSCR_ENABLE;
                thread->context.fpscr &= FPSCR_CLEAR_EXCEPTIONS;
            }
            fpscr |= __OSFpscrEnableBits & FPSCR_ENABLE;
            msr |= MSR_FE0 | MSR_FE1;
        } else {
            for (thread = __OSActiveThreadQueue.head; thread != NULL;
                 thread = thread->linkActive.next) {
                thread->context.srr1 &= ~(MSR_FE0 | MSR_FE1);
                thread->context.fpscr &= ~FPSCR_ENABLE;
                thread->context.fpscr &= FPSCR_CLEAR_EXCEPTIONS;
            }
            fpscr &= ~FPSCR_ENABLE;
            msr &= ~(MSR_FE0 | MSR_FE1);
        }
        fpscr &= FPSCR_CLEAR_EXCEPTIONS;
        PPCMtfpscr(fpscr);
        PPCMtmsr(msr);
    }

    OSRestoreInterrupts(enabled);
    return oldHandler;
}

void __OSUnhandledException(__OSException exception, OSContext *context, u32 dsisr, u32 dar) {
    OSTime now;

    now = OSGetTime();

    if (!(context->srr1 & MSR_RI)) {
        OSReport("Non-recoverable Exception %d", exception);
    } else {
        if (exception == EXCEPTION_PROGRAM && (context->srr1 & SRR1_FP_EXCEPTION) &&
            __OSErrorTable[OS_ERROR_FPE] != NULL) {
            u32 fpscr;
            u32 msr;

            exception = OS_ERROR_FPE;

            /* Save the faulting FPU state, then clear the sticky exception bits so the
             * handler's own FP code does not trap again. */
            msr = PPCMfmsr();
            PPCMtmsr(msr | MSR_FP);
            if (__OSFPUContext != NULL) {
                OSSaveFPUContext(__OSFPUContext);
            }
            fpscr = PPCMffpscr();
            fpscr &= FPSCR_CLEAR_EXCEPTIONS;
            PPCMtfpscr(fpscr);
            PPCMtmsr(msr);

            if (__OSFPUContext == context) {
                OSDisableScheduler();
                __OSErrorTable[exception](exception, context, dsisr, dar);
                context->srr1 &= ~MSR_FP;
                __OSFPUContext = NULL;
                context->fpscr &= FPSCR_CLEAR_EXCEPTIONS;
                OSEnableScheduler();
                __OSReschedule();
            } else {
                context->srr1 &= ~MSR_FP;
                __OSFPUContext = NULL;
            }
            OSLoadContext(context);
        }

        if (__OSErrorTable[exception] != NULL) {
            OSDisableScheduler();
            __OSErrorTable[exception](exception, context, dsisr, dar);
            OSEnableScheduler();
            __OSReschedule();
            OSLoadContext(context);
        }

        if (exception == EXCEPTION_DECREMENTER) {
            OSLoadContext(context);
        }

        OSReport("Unhandled Exception %d", exception);
    }

    OSReport("\n");
    OSDumpContext(context);
    OSReport("\nDSISR = 0x%08x                   DAR  = 0x%08x\n", dsisr, dar);
    OSReport("TB = 0x%016llx\n", now);

    switch (exception) {
    case EXCEPTION_DSI:
        OSReport("\nInstruction at 0x%x (read from SRR0) attempted to access invalid address 0x%x "
                 "(read from DAR)\n",
                 context->srr0, dar);
        break;
    case EXCEPTION_ISI:
        OSReport("\nAttempted to fetch instruction from invalid address 0x%x (read from SRR0)\n",
                 context->srr0);
        break;
    case EXCEPTION_ALIGNMENT:
        OSReport("\nInstruction at 0x%x (read from SRR0) attempted to access unaligned address "
                 "0x%x (read from DAR)\n",
                 context->srr0, dar);
        break;
    case EXCEPTION_PROGRAM:
        OSReport("\nProgram exception : Possible illegal instruction/operation at or around 0x%x "
                 "(read from SRR0)\n",
                 context->srr0, dar);
        break;
    case EXCEPTION_MEMORY_PROTECTION:
        OSReport("\n");
        OSReport("AI DMA Address =   0x%04x%04x\n", __DSPRegs[0x18], __DSPRegs[0x19]);
        OSReport("ARAM DMA Address = 0x%04x%04x\n", __DSPRegs[0x10], __DSPRegs[0x11]);
        OSReport("DI DMA Address =   0x%08x\n", __DIRegs[5]);
        break;
    }

    OSReport("\nLast interrupt (%d): SRR0 = 0x%08x  TB = 0x%016llx\n", __OSLastInterrupt,
             __OSLastInterruptSrr0, __OSLastInterruptTime);
    PPCHalt();
}

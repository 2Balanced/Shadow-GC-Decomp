/* Dolphin SDK OS: thread register contexts.
 *
 * An OSContext holds everything a thread switch saves. Integer state is saved and restored on
 * every switch; floating-point state is switched lazily: MSR[FP] stays off for every thread but
 * the one whose registers are live in the FPU (__OSFPUContext), so the first FP instruction of any
 * other thread raises the FPU-unavailable exception, and OSSwitchFPUContext swaps the FPU over
 * then. The current context lives in low memory both as a cached pointer (0x800000D4) and as a
 * physical one (0x800000C0) for the exception vectors, which run with translation off. */
#include <dolphin/base/PPCArch.h>
#include <dolphin/os.h>

#define OS_CURRENT_CONTEXT (*(OSContext *volatile *)OSPhysicalToCached(0x00D4))
#define OS_FPU_CONTEXT (*(OSContext **)OSPhysicalToCached(0x00D8))

/* OSContext.state bits. */
#define OS_CONTEXT_STATE_FPSAVED 0x01u/* fpr/psf/fpscr hold this thread's FPU state */
#define OS_CONTEXT_STATE_EXC 0x02     /* saved by an exception: r5-r12 are live too */

#define FPU_UNAVAILABLE_EXCEPTION 7

/* Restores the FPU state saved in `fpuContext`, if any was saved. */
static asm void __OSLoadFPUContext(register u32 unused, register OSContext *fpuContext) {
    nofralloc
    lhz r5, OSContext.state(fpuContext)
    clrlwi. r5, r5, 31
    beq @done
    lfd f0, OSContext.fpscr_pad(fpuContext)
    mtfsf 0xFF, f0
    mfspr r5, HID2
    rlwinm. r5, r5, 3, 31, 31
    beq @scalars
    psq_l f0, OSContext.psf[0](fpuContext), 0, 0
    psq_l f1, OSContext.psf[1](fpuContext), 0, 0
    psq_l f2, OSContext.psf[2](fpuContext), 0, 0
    psq_l f3, OSContext.psf[3](fpuContext), 0, 0
    psq_l f4, OSContext.psf[4](fpuContext), 0, 0
    psq_l f5, OSContext.psf[5](fpuContext), 0, 0
    psq_l f6, OSContext.psf[6](fpuContext), 0, 0
    psq_l f7, OSContext.psf[7](fpuContext), 0, 0
    psq_l f8, OSContext.psf[8](fpuContext), 0, 0
    psq_l f9, OSContext.psf[9](fpuContext), 0, 0
    psq_l f10, OSContext.psf[10](fpuContext), 0, 0
    psq_l f11, OSContext.psf[11](fpuContext), 0, 0
    psq_l f12, OSContext.psf[12](fpuContext), 0, 0
    psq_l f13, OSContext.psf[13](fpuContext), 0, 0
    psq_l f14, OSContext.psf[14](fpuContext), 0, 0
    psq_l f15, OSContext.psf[15](fpuContext), 0, 0
    psq_l f16, OSContext.psf[16](fpuContext), 0, 0
    psq_l f17, OSContext.psf[17](fpuContext), 0, 0
    psq_l f18, OSContext.psf[18](fpuContext), 0, 0
    psq_l f19, OSContext.psf[19](fpuContext), 0, 0
    psq_l f20, OSContext.psf[20](fpuContext), 0, 0
    psq_l f21, OSContext.psf[21](fpuContext), 0, 0
    psq_l f22, OSContext.psf[22](fpuContext), 0, 0
    psq_l f23, OSContext.psf[23](fpuContext), 0, 0
    psq_l f24, OSContext.psf[24](fpuContext), 0, 0
    psq_l f25, OSContext.psf[25](fpuContext), 0, 0
    psq_l f26, OSContext.psf[26](fpuContext), 0, 0
    psq_l f27, OSContext.psf[27](fpuContext), 0, 0
    psq_l f28, OSContext.psf[28](fpuContext), 0, 0
    psq_l f29, OSContext.psf[29](fpuContext), 0, 0
    psq_l f30, OSContext.psf[30](fpuContext), 0, 0
    psq_l f31, OSContext.psf[31](fpuContext), 0, 0
@scalars:
    lfd f0, OSContext.fpr[0](fpuContext)
    lfd f1, OSContext.fpr[1](fpuContext)
    lfd f2, OSContext.fpr[2](fpuContext)
    lfd f3, OSContext.fpr[3](fpuContext)
    lfd f4, OSContext.fpr[4](fpuContext)
    lfd f5, OSContext.fpr[5](fpuContext)
    lfd f6, OSContext.fpr[6](fpuContext)
    lfd f7, OSContext.fpr[7](fpuContext)
    lfd f8, OSContext.fpr[8](fpuContext)
    lfd f9, OSContext.fpr[9](fpuContext)
    lfd f10, OSContext.fpr[10](fpuContext)
    lfd f11, OSContext.fpr[11](fpuContext)
    lfd f12, OSContext.fpr[12](fpuContext)
    lfd f13, OSContext.fpr[13](fpuContext)
    lfd f14, OSContext.fpr[14](fpuContext)
    lfd f15, OSContext.fpr[15](fpuContext)
    lfd f16, OSContext.fpr[16](fpuContext)
    lfd f17, OSContext.fpr[17](fpuContext)
    lfd f18, OSContext.fpr[18](fpuContext)
    lfd f19, OSContext.fpr[19](fpuContext)
    lfd f20, OSContext.fpr[20](fpuContext)
    lfd f21, OSContext.fpr[21](fpuContext)
    lfd f22, OSContext.fpr[22](fpuContext)
    lfd f23, OSContext.fpr[23](fpuContext)
    lfd f24, OSContext.fpr[24](fpuContext)
    lfd f25, OSContext.fpr[25](fpuContext)
    lfd f26, OSContext.fpr[26](fpuContext)
    lfd f27, OSContext.fpr[27](fpuContext)
    lfd f28, OSContext.fpr[28](fpuContext)
    lfd f29, OSContext.fpr[29](fpuContext)
    lfd f30, OSContext.fpr[30](fpuContext)
    lfd f31, OSContext.fpr[31](fpuContext)
@done:
    blr
}

/* Saves the FPU (and the second paired-single halves when paired singles are on) into
 * `fpuContext` and marks it saved. */
static asm void __OSSaveFPUContext(register u32 unused1, register u32 unused2,
                                   register OSContext *fpuContext) {
    nofralloc
    lhz r3, OSContext.state(fpuContext)
    ori r3, r3, OS_CONTEXT_STATE_FPSAVED
    sth r3, OSContext.state(fpuContext)
    stfd f0, OSContext.fpr[0](fpuContext)
    stfd f1, OSContext.fpr[1](fpuContext)
    stfd f2, OSContext.fpr[2](fpuContext)
    stfd f3, OSContext.fpr[3](fpuContext)
    stfd f4, OSContext.fpr[4](fpuContext)
    stfd f5, OSContext.fpr[5](fpuContext)
    stfd f6, OSContext.fpr[6](fpuContext)
    stfd f7, OSContext.fpr[7](fpuContext)
    stfd f8, OSContext.fpr[8](fpuContext)
    stfd f9, OSContext.fpr[9](fpuContext)
    stfd f10, OSContext.fpr[10](fpuContext)
    stfd f11, OSContext.fpr[11](fpuContext)
    stfd f12, OSContext.fpr[12](fpuContext)
    stfd f13, OSContext.fpr[13](fpuContext)
    stfd f14, OSContext.fpr[14](fpuContext)
    stfd f15, OSContext.fpr[15](fpuContext)
    stfd f16, OSContext.fpr[16](fpuContext)
    stfd f17, OSContext.fpr[17](fpuContext)
    stfd f18, OSContext.fpr[18](fpuContext)
    stfd f19, OSContext.fpr[19](fpuContext)
    stfd f20, OSContext.fpr[20](fpuContext)
    stfd f21, OSContext.fpr[21](fpuContext)
    stfd f22, OSContext.fpr[22](fpuContext)
    stfd f23, OSContext.fpr[23](fpuContext)
    stfd f24, OSContext.fpr[24](fpuContext)
    stfd f25, OSContext.fpr[25](fpuContext)
    stfd f26, OSContext.fpr[26](fpuContext)
    stfd f27, OSContext.fpr[27](fpuContext)
    stfd f28, OSContext.fpr[28](fpuContext)
    stfd f29, OSContext.fpr[29](fpuContext)
    stfd f30, OSContext.fpr[30](fpuContext)
    stfd f31, OSContext.fpr[31](fpuContext)
    mffs f0
    stfd f0, OSContext.fpscr_pad(fpuContext)
    lfd f0, OSContext.fpr[0](fpuContext)
    mfspr r3, HID2
    rlwinm. r3, r3, 3, 31, 31
    beq @done
    psq_st f0, OSContext.psf[0](fpuContext), 0, 0
    psq_st f1, OSContext.psf[1](fpuContext), 0, 0
    psq_st f2, OSContext.psf[2](fpuContext), 0, 0
    psq_st f3, OSContext.psf[3](fpuContext), 0, 0
    psq_st f4, OSContext.psf[4](fpuContext), 0, 0
    psq_st f5, OSContext.psf[5](fpuContext), 0, 0
    psq_st f6, OSContext.psf[6](fpuContext), 0, 0
    psq_st f7, OSContext.psf[7](fpuContext), 0, 0
    psq_st f8, OSContext.psf[8](fpuContext), 0, 0
    psq_st f9, OSContext.psf[9](fpuContext), 0, 0
    psq_st f10, OSContext.psf[10](fpuContext), 0, 0
    psq_st f11, OSContext.psf[11](fpuContext), 0, 0
    psq_st f12, OSContext.psf[12](fpuContext), 0, 0
    psq_st f13, OSContext.psf[13](fpuContext), 0, 0
    psq_st f14, OSContext.psf[14](fpuContext), 0, 0
    psq_st f15, OSContext.psf[15](fpuContext), 0, 0
    psq_st f16, OSContext.psf[16](fpuContext), 0, 0
    psq_st f17, OSContext.psf[17](fpuContext), 0, 0
    psq_st f18, OSContext.psf[18](fpuContext), 0, 0
    psq_st f19, OSContext.psf[19](fpuContext), 0, 0
    psq_st f20, OSContext.psf[20](fpuContext), 0, 0
    psq_st f21, OSContext.psf[21](fpuContext), 0, 0
    psq_st f22, OSContext.psf[22](fpuContext), 0, 0
    psq_st f23, OSContext.psf[23](fpuContext), 0, 0
    psq_st f24, OSContext.psf[24](fpuContext), 0, 0
    psq_st f25, OSContext.psf[25](fpuContext), 0, 0
    psq_st f26, OSContext.psf[26](fpuContext), 0, 0
    psq_st f27, OSContext.psf[27](fpuContext), 0, 0
    psq_st f28, OSContext.psf[28](fpuContext), 0, 0
    psq_st f29, OSContext.psf[29](fpuContext), 0, 0
    psq_st f30, OSContext.psf[30](fpuContext), 0, 0
    psq_st f31, OSContext.psf[31](fpuContext), 0, 0
@done:
    blr
}

asm void OSSaveFPUContext(register OSContext *fpuContext) {
    nofralloc
    addi r5, fpuContext, 0
    b __OSSaveFPUContext
}

/* Makes `context` current. FP stays enabled only if it is the context that owns the FPU. */
asm void OSSetCurrentContext(register OSContext *context) {
    nofralloc
    lis r4, 0x8000
    stw context, 0x00D4(r4)
    clrlwi r5, context, 2
    stw r5, 0x00C0(r4)
    lwz r5, 0x00D8(r4)
    cmpw r5, context
    bne @notOwner
    lwz r6, OSContext.srr1(context)
    ori r6, r6, 0x2000
    stw r6, OSContext.srr1(context)
    mfmsr r6
    ori r6, r6, 0x2
    mtmsr r6
    blr
@notOwner:
    lwz r6, OSContext.srr1(context)
    rlwinm r6, r6, 0, 19, 17
    stw r6, OSContext.srr1(context)
    mfmsr r6
    rlwinm r6, r6, 0, 19, 17
    ori r6, r6, 0x2
    mtmsr r6
    isync
    blr
}

OSContext *OSGetCurrentContext(void) {
    return OS_CURRENT_CONTEXT;
}

/* Saves the caller's non-volatile state; returns 0 now and 1 when the context is loaded later. */
asm u32 OSSaveContext(register OSContext *context) {
    nofralloc
    stmw r13, OSContext.gpr[13](context)
    mfspr r0, GQR1
    stw r0, OSContext.gqr[1](context)
    mfspr r0, GQR2
    stw r0, OSContext.gqr[2](context)
    mfspr r0, GQR3
    stw r0, OSContext.gqr[3](context)
    mfspr r0, GQR4
    stw r0, OSContext.gqr[4](context)
    mfspr r0, GQR5
    stw r0, OSContext.gqr[5](context)
    mfspr r0, GQR6
    stw r0, OSContext.gqr[6](context)
    mfspr r0, GQR7
    stw r0, OSContext.gqr[7](context)
    mfcr r0
    stw r0, OSContext.cr(context)
    mflr r0
    stw r0, OSContext.lr(context)
    stw r0, OSContext.srr0(context)
    mfmsr r0
    stw r0, OSContext.srr1(context)
    mfctr r0
    stw r0, OSContext.ctr(context)
    mfxer r0
    stw r0, OSContext.xer(context)
    stw r1, OSContext.gpr[1](context)
    stw r2, OSContext.gpr[2](context)
    li r0, 0x1
    stw r0, OSContext.gpr[3](context)
    li r3, 0
    blr
}

/* Resumes `context` with rfi. A thread interrupted inside OSDisableInterrupts restarts it from
 * the top, so the MSR it read there is not stale. */
asm void OSLoadContext(register OSContext *context) {
    nofralloc
    lis r4, OSDisableInterrupts@ha
    lwz r6, OSContext.srr0(context)
    addi r5, r4, OSDisableInterrupts@l
    cmplw r6, r5
    ble @srr0Ok
    lis r4, (OSDisableInterrupts + 0xC)@ha
    addi r0, r4, (OSDisableInterrupts + 0xC)@l
    cmplw r6, r0
    bge @srr0Ok
    stw r5, OSContext.srr0(context)
@srr0Ok:
    lwz r0, OSContext.gpr[0](context)
    lwz r1, OSContext.gpr[1](context)
    lwz r2, OSContext.gpr[2](context)
    lhz r4, OSContext.state(context)
    rlwinm. r5, r4, 0, 30, 30
    beq @notExc
    rlwinm r4, r4, 0, 31, 29
    sth r4, OSContext.state(context)
    lmw r5, OSContext.gpr[5](context)
    b @gprsDone
@notExc:
    lmw r13, OSContext.gpr[13](context)
@gprsDone:
    lwz r4, OSContext.gqr[1](context)
    mtspr GQR1, r4
    lwz r4, OSContext.gqr[2](context)
    mtspr GQR2, r4
    lwz r4, OSContext.gqr[3](context)
    mtspr GQR3, r4
    lwz r4, OSContext.gqr[4](context)
    mtspr GQR4, r4
    lwz r4, OSContext.gqr[5](context)
    mtspr GQR5, r4
    lwz r4, OSContext.gqr[6](context)
    mtspr GQR6, r4
    lwz r4, OSContext.gqr[7](context)
    mtspr GQR7, r4
    lwz r4, OSContext.cr(context)
    mtcrf 0xFF, r4
    lwz r4, OSContext.lr(context)
    mtlr r4
    lwz r4, OSContext.ctr(context)
    mtctr r4
    lwz r4, OSContext.xer(context)
    mtxer r4
    mfmsr r4
    rlwinm r4, r4, 0, 17, 15
    rlwinm r4, r4, 0, 31, 29
    mtmsr r4
    lwz r4, OSContext.srr0(context)
    mtsrr0 r4
    lwz r4, OSContext.srr1(context)
    mtsrr1 r4
    lwz r4, OSContext.gpr[4](context)
    lwz r3, OSContext.gpr[3](context)
    rfi
}

asm u32 OSGetStackPointer(void) {
    nofralloc
    mr r3, r1
    blr
}

/* Calls `pc` on the stack ending at `newsp`, then returns on the old stack. */
asm void OSSwitchFiber(register u32 pc, register u32 newsp) {
    nofralloc
    mflr r0
    mr r5, r1
    stwu r5, -8(newsp)
    mr r1, newsp
    stw r0, 4(r5)
    mtlr pc
    blrl
    lwz r5, 0(r1)
    lwz r0, 4(r5)
    mtlr r0
    mr r1, r5
    blr
}

void OSClearContext(OSContext *context) {
    context->mode = 0;
    context->state = 0;
    if (context == OS_FPU_CONTEXT) {
        OS_FPU_CONTEXT = NULL;
    }
}

/* Sets up `context` to start at `pc` on the stack `newsp`, with external interrupts, FP
 * exceptions off, translation on and r2/r13 (the small-data bases) inherited. */
asm void OSInitContext(register OSContext *context, register u32 pc, register u32 newsp) {
    nofralloc
    stw pc, OSContext.srr0(context)
    stw newsp, OSContext.gpr[1](context)
    li r11, 0
    ori r11, r11, 0x9032
    stw r11, OSContext.srr1(context)
    li r0, 0
    stw r0, OSContext.cr(context)
    stw r0, OSContext.xer(context)
    stw r2, OSContext.gpr[2](context)
    stw r13, OSContext.gpr[13](context)
    stw r0, OSContext.gpr[3](context)
    stw r0, OSContext.gpr[4](context)
    stw r0, OSContext.gpr[5](context)
    stw r0, OSContext.gpr[6](context)
    stw r0, OSContext.gpr[7](context)
    stw r0, OSContext.gpr[8](context)
    stw r0, OSContext.gpr[9](context)
    stw r0, OSContext.gpr[10](context)
    stw r0, OSContext.gpr[11](context)
    stw r0, OSContext.gpr[12](context)
    stw r0, OSContext.gpr[14](context)
    stw r0, OSContext.gpr[15](context)
    stw r0, OSContext.gpr[16](context)
    stw r0, OSContext.gpr[17](context)
    stw r0, OSContext.gpr[18](context)
    stw r0, OSContext.gpr[19](context)
    stw r0, OSContext.gpr[20](context)
    stw r0, OSContext.gpr[21](context)
    stw r0, OSContext.gpr[22](context)
    stw r0, OSContext.gpr[23](context)
    stw r0, OSContext.gpr[24](context)
    stw r0, OSContext.gpr[25](context)
    stw r0, OSContext.gpr[26](context)
    stw r0, OSContext.gpr[27](context)
    stw r0, OSContext.gpr[28](context)
    stw r0, OSContext.gpr[29](context)
    stw r0, OSContext.gpr[30](context)
    stw r0, OSContext.gpr[31](context)
    stw r0, OSContext.gqr[0](context)
    stw r0, OSContext.gqr[1](context)
    stw r0, OSContext.gqr[2](context)
    stw r0, OSContext.gqr[3](context)
    stw r0, OSContext.gqr[4](context)
    stw r0, OSContext.gqr[5](context)
    stw r0, OSContext.gqr[6](context)
    stw r0, OSContext.gqr[7](context)
    b OSClearContext
}

void OSDumpContext(OSContext *context) {
    u32 i;
    u32 *p;
    OSContext *current;
    OSContext scratch;
    BOOL enabled;

    OSReport("------------------------- Context 0x%08x -------------------------\n", context);
    for (i = 0; i < 16; ++i) {
        OSReport("r%-2d  = 0x%08x (%14d)  r%-2d  = 0x%08x (%14d)\n", i, context->gpr[i],
                 context->gpr[i], i + 16, context->gpr[i + 16], context->gpr[i + 16]);
    }
    OSReport("LR   = 0x%08x                   CR   = 0x%08x\n", context->lr, context->cr);
    OSReport("SRR0 = 0x%08x                   SRR1 = 0x%08x\n", context->srr0, context->srr1);

    OSReport("\nGQRs----------\n");
    for (i = 0; i < 4; ++i) {
        OSReport("gqr%d = 0x%08x \t gqr%d = 0x%08x\n", i, context->gqr[i], i + 4,
                 context->gqr[i + 4]);
    }

    if (context->state & OS_CONTEXT_STATE_FPSAVED) {
        /* Printing may touch the FPU; give it a throwaway context so the dump cannot clobber
         * the registers being shown. */
        enabled = OSDisableInterrupts();
        current = OSGetCurrentContext();
        OSClearContext(&scratch);
        OSSetCurrentContext(&scratch);

        OSReport("\n\nFPRs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("fr%d \t= %d \t fr%d \t= %d\n", i, (u32)context->fpr[i], i + 1,
                     (u32)context->fpr[i + 1]);
        }
        OSReport("\n\nPSFs----------\n");
        for (i = 0; i < 32; i += 2) {
            OSReport("ps%d \t= 0x%x \t ps%d \t= 0x%x\n", i, (u32)context->psf[i], i + 1,
                     (u32)context->psf[i + 1]);
        }

        OSClearContext(&scratch);
        OSSetCurrentContext(current);
        OSRestoreInterrupts(enabled);
    }

    OSReport("\nAddress:      Back Chain    LR Save\n");
    for (i = 0, p = (u32 *)context->gpr[1]; p != NULL && (u32)p != 0xFFFFFFFF && i++ < 16;
         p = (u32 *)*p) {
        OSReport("0x%08x:   0x%08x    0x%08x\n", p, p[0], p[1]);
    }
}

/* FPU-unavailable exception: enable the FPU for the interrupted thread, save the previous
 * owner's registers, load the new owner's, and return to the faulting instruction. */
static asm void OSSwitchFPUContext(register __OSException exception, register OSContext *context) {
    nofralloc
    mfmsr r5
    ori r5, r5, 0x2000
    mtmsr r5
    isync
    lwz r5, OSContext.srr1(context)
    ori r5, r5, 0x2000
    mtsrr1 r5
    lis r3, 0x8000
    lwz r5, 0x00D8(r3)
    stw context, 0x00D8(r3)
    cmpw r5, context
    beq @restore
    cmpwi r5, 0
    beq @load
    bl __OSSaveFPUContext
@load:
    bl __OSLoadFPUContext
@restore:
    lwz r3, OSContext.cr(context)
    mtcrf 0xFF, r3
    lwz r3, OSContext.lr(context)
    mtlr r3
    lwz r3, OSContext.srr0(context)
    mtsrr0 r3
    lwz r3, OSContext.ctr(context)
    mtctr r3
    lwz r3, OSContext.xer(context)
    mtxer r3
    lhz r3, OSContext.state(context)
    rlwinm r3, r3, 0, 31, 29
    sth r3, OSContext.state(context)
    lwz r5, OSContext.gpr[5](context)
    lwz r3, OSContext.gpr[3](context)
    lwz r4, OSContext.gpr[4](context)
    rfi
}

void __OSContextInit(void) {
    __OSSetExceptionHandler(FPU_UNAVAILABLE_EXCEPTION, OSSwitchFPUContext);
    OS_FPU_CONTEXT = NULL;
    DBPrintf("FPU-unavailable handler installed\n");
}

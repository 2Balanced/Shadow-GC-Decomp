/* Dolphin SDK GX: command FIFOs.
 *
 * The CPU writes commands into one FIFO through the write-gather pipe (the processor interface
 * registers hold its bounds and write pointer) while the graphics processor reads another (the
 * command processor registers hold its bounds, pointers, count and watermarks). When the same
 * FIFO is attached to both they are "linked" and the command processor raises the high and low
 * watermark interrupts: on high watermark the writing thread is suspended until the GP has
 * drained the FIFO to the low watermark. */
#include <dolphin/gx/GXPrivate.h>

static GXFifoObjPriv *CPUFifo;
static GXFifoObjPriv *GPFifo;
OSThread *__GXCurrentThread = NULL;
static GXBool CPGPLinked;
static BOOL GXOverflowSuspendInProgress;
static GXBreakPtCallback BreakPointCB;
u32 __GXOverflowCount = 0;
static void *__GXCurrentBP;

static void GXCPInterruptHandler(__GXOSInterrupt interrupt, OSContext *context) {
    OSContext exceptionContext;

    gx->cpStatus = __cpReg[0];

    if (GET_REG_FIELD(gx->cpEnable, 1, 3) && GET_REG_FIELD(gx->cpStatus, 1, 1)) {
        /* GP drained the FIFO to the low watermark: let the writer continue. */
        OSResumeThread(__GXCurrentThread);
        GXOverflowSuspendInProgress = FALSE;
        __GXWriteFifoIntReset(GX_TRUE, GX_TRUE);
        __GXWriteFifoIntEnable(GX_TRUE, GX_FALSE);
    }

    if (GET_REG_FIELD(gx->cpEnable, 1, 2) && GET_REG_FIELD(gx->cpStatus, 1, 0)) {
        /* FIFO filled to the high watermark: stop the writer until it drains. */
        __GXOverflowCount++;
        __GXWriteFifoIntEnable(GX_FALSE, GX_TRUE);
        __GXWriteFifoIntReset(GX_TRUE, GX_FALSE);
        GXOverflowSuspendInProgress = TRUE;
        OSSuspendThread(__GXCurrentThread);
    }

    if (GET_REG_FIELD(gx->cpEnable, 1, 5) && GET_REG_FIELD(gx->cpStatus, 1, 4)) {
        /* GP reached the breakpoint. */
        SET_REG_FIELD(gx->cpEnable, 1, 5, 0);
        __cpReg[1] = gx->cpEnable;
        if (BreakPointCB) {
            OSClearContext(&exceptionContext);
            OSSetCurrentContext(&exceptionContext);
            BreakPointCB();
            OSClearContext(&exceptionContext);
            OSSetCurrentContext(context);
        }
    }
}

void GXInitFifoBase(GXFifoObj *fifo, void *base, u32 size) {
    GXFifoObjPriv *f = (GXFifoObjPriv *)fifo;

    f->base = base;
    f->top = (u8 *)base + size - 4;
    f->size = size;
    f->count = 0;
    GXInitFifoLimits(fifo, size - 0x4000, (size >> 1) & ~0x1F);
    GXInitFifoPtrs(fifo, base, base);
}

void GXInitFifoPtrs(GXFifoObj *fifo, void *readPtr, void *writePtr) {
    GXFifoObjPriv *f = (GXFifoObjPriv *)fifo;
    BOOL enabled = OSDisableInterrupts();

    f->rdPtr = readPtr;
    f->wrPtr = writePtr;
    f->count = (u8 *)writePtr - (u8 *)readPtr;
    if (f->count < 0) {
        f->count += f->size;
    }
    OSRestoreInterrupts(enabled);
}

void GXInitFifoLimits(GXFifoObj *fifo, u32 hiWatermark, u32 loWatermark) {
    GXFifoObjPriv *f = (GXFifoObjPriv *)fifo;

    f->hiWatermark = hiWatermark;
    f->loWatermark = loWatermark;
}

void GXSetCPUFifo(GXFifoObj *fifo) {
    GXFifoObjPriv *f = (GXFifoObjPriv *)fifo;
    BOOL enabled = OSDisableInterrupts();

    CPUFifo = f;
    if (f == GPFifo) {
        u32 reg;

        __piReg[3] = (u32)f->base & 0x3FFFFFFF;
        __piReg[4] = (u32)f->top & 0x3FFFFFFF;
        reg = 0;
        SET_REG_FIELD(reg, 21, 5, ((u32)f->wrPtr & 0x3FFFFFFF) >> 5);
        SET_REG_FIELD(reg, 1, 26, 0);
        __piReg[5] = reg;
        CPGPLinked = GX_TRUE;
        __GXWriteFifoIntReset(GX_TRUE, GX_TRUE);
        __GXWriteFifoIntEnable(GX_TRUE, GX_FALSE);
        __GXFifoLink(GX_TRUE);
    } else {
        u32 reg;

        if (CPGPLinked) {
            __GXFifoLink(GX_FALSE);
            CPGPLinked = GX_FALSE;
        }
        __GXWriteFifoIntEnable(GX_FALSE, GX_FALSE);
        __piReg[3] = (u32)f->base & 0x3FFFFFFF;
        __piReg[4] = (u32)f->top & 0x3FFFFFFF;
        reg = 0;
        SET_REG_FIELD(reg, 21, 5, ((u32)f->wrPtr & 0x3FFFFFFF) >> 5);
        SET_REG_FIELD(reg, 1, 26, 0);
        __piReg[5] = reg;
    }
    PPCSync();
    OSRestoreInterrupts(enabled);
}

void GXSetGPFifo(GXFifoObj *fifo) {
    GXFifoObjPriv *f = (GXFifoObjPriv *)fifo;
    BOOL enabled = OSDisableInterrupts();
    u32 reg;

    __GXFifoReadDisable();
    __GXWriteFifoIntEnable(GX_FALSE, GX_FALSE);
    GPFifo = f;

    __cpReg[16] = (u32)f->base;
    __cpReg[18] = (u32)f->top;
    __cpReg[24] = f->count;
    __cpReg[26] = (u32)f->wrPtr;
    __cpReg[28] = (u32)f->rdPtr;
    __cpReg[20] = f->hiWatermark;
    __cpReg[22] = f->loWatermark;
    __cpReg[17] = ((u32)f->base & 0x3FFFFFFF) >> 16;
    __cpReg[19] = ((u32)f->top & 0x3FFFFFFF) >> 16;
    __cpReg[25] = f->count >> 16;
    __cpReg[27] = ((u32)f->wrPtr & 0x3FFFFFFF) >> 16;
    __cpReg[29] = ((u32)f->rdPtr & 0x3FFFFFFF) >> 16;
    __cpReg[21] = f->hiWatermark >> 16;
    __cpReg[23] = f->loWatermark >> 16;
    PPCSync();

    if (CPUFifo == GPFifo) {
        CPGPLinked = GX_TRUE;
        __GXWriteFifoIntEnable(GX_TRUE, GX_FALSE);
        __GXFifoLink(GX_TRUE);
    } else {
        CPGPLinked = GX_FALSE;
        __GXWriteFifoIntEnable(GX_FALSE, GX_FALSE);
        __GXFifoLink(GX_FALSE);
    }

    reg = gx->cpEnable;
    SET_REG_FIELD(reg, 1, 1, 0);
    SET_REG_FIELD(reg, 1, 5, 0);
    __cpReg[1] = reg;
    __cpReg[1] = gx->cpEnable;
    __GXWriteFifoIntReset(GX_TRUE, GX_TRUE);
    __GXFifoReadEnable();
    OSRestoreInterrupts(enabled);
}

void GXGetFifoPtrs(GXFifoObj *fifo, void **readPtr, void **writePtr) {
    GXFifoObjPriv *f = (GXFifoObjPriv *)fifo;
    u32 rdPtr;

    if (f == CPUFifo) {
        f->wrPtr = OSPhysicalToCached(__piReg[5] & ~0x04000000);
    }
    if (f == GPFifo) {
        rdPtr = GX_GET_CP_REG_U32(28, 29);
        f->rdPtr = OSPhysicalToCached(rdPtr);
        f->count = GX_GET_CP_REG_U32(24, 25);
    } else {
        f->count = (u8 *)f->wrPtr - (u8 *)f->rdPtr;
        if (f->count < 0) {
            f->count += f->size;
        }
    }
    *readPtr = f->rdPtr;
    *writePtr = f->wrPtr;
}

GXBreakPtCallback GXSetBreakPtCallback(GXBreakPtCallback cb) {
    GXBreakPtCallback oldcb = BreakPointCB;
    BOOL enabled = OSDisableInterrupts();

    BreakPointCB = cb;
    OSRestoreInterrupts(enabled);
    return oldcb;
}

void GXEnableBreakPt(void *breakPt) {
    BOOL enabled = OSDisableInterrupts();

    __GXFifoReadDisable();
    __cpReg[30] = (u32)breakPt;
    __cpReg[31] = ((u32)breakPt & 0x3FFFFFFF) >> 16;
    SET_REG_FIELD(gx->cpEnable, 1, 1, 0);
    SET_REG_FIELD(gx->cpEnable, 1, 5, 0);
    __cpReg[1] = gx->cpEnable;
    SET_REG_FIELD(gx->cpEnable, 1, 1, 1);
    SET_REG_FIELD(gx->cpEnable, 1, 5, 1);
    __cpReg[1] = gx->cpEnable;
    __GXCurrentBP = breakPt;
    __GXFifoReadEnable();
    OSRestoreInterrupts(enabled);
}

void GXDisableBreakPt(void) {
    BOOL enabled = OSDisableInterrupts();

    SET_REG_FIELD(gx->cpEnable, 1, 1, 0);
    SET_REG_FIELD(gx->cpEnable, 1, 5, 0);
    __cpReg[1] = gx->cpEnable;
    __GXCurrentBP = NULL;
    OSRestoreInterrupts(enabled);
}

void __GXFifoInit(void) {
    __OSSetInterruptHandler(17, GXCPInterruptHandler);
    __OSUnmaskInterrupts(0x4000);
    __GXCurrentThread = OSGetCurrentThread();
    GXOverflowSuspendInProgress = FALSE;
    CPUFifo = NULL;
    GPFifo = NULL;
}

void __GXFifoReadEnable(void) {
    SET_REG_FIELD(gx->cpEnable, 1, 0, 1);
    __cpReg[1] = gx->cpEnable;
}

void __GXFifoReadDisable(void) {
    SET_REG_FIELD(gx->cpEnable, 1, 0, 0);
    __cpReg[1] = gx->cpEnable;
}

void __GXFifoLink(u8 link) {
    SET_REG_FIELD(gx->cpEnable, 1, 4, link ? 1 : 0);
    __cpReg[1] = gx->cpEnable;
}

void __GXWriteFifoIntEnable(u8 hiWatermarkEn, u8 loWatermarkEn) {
    SET_REG_FIELD(gx->cpEnable, 1, 2, hiWatermarkEn);
    SET_REG_FIELD(gx->cpEnable, 1, 3, loWatermarkEn);
    __cpReg[1] = gx->cpEnable;
}

void __GXWriteFifoIntReset(u8 hiWatermarkClr, u8 loWatermarkClr) {
    SET_REG_FIELD(gx->cpClr, 1, 0, hiWatermarkClr);
    SET_REG_FIELD(gx->cpClr, 1, 1, loWatermarkClr);
    __cpReg[2] = gx->cpClr;
}

void *GXGetFifoBase(const GXFifoObj *fifo) {
    return ((GXFifoObjPriv *)fifo)->base;
}

void __GXCleanGPFifo(void) {
    GXFifoObj dummyFifo;
    GXFifoObj *gpFifo = GXGetGPFifo();
    GXFifoObj *cpuFifo;
    void *base;
    u32 unused[2]; /* retail reserves two more stack words than these locals need */

    if (!gpFifo) {
        return;
    }

    cpuFifo = GXGetCPUFifo();
    base = GXGetFifoBase(gpFifo);
    dummyFifo = *gpFifo;
    GXInitFifoPtrs(&dummyFifo, base, base);
    GXSetGPFifo(&dummyFifo);
    if (cpuFifo == gpFifo) {
        GXSetCPUFifo(&dummyFifo);
    }
    GXInitFifoPtrs(gpFifo, base, base);
    GXSetGPFifo(gpFifo);
    if (cpuFifo == gpFifo) {
        GXSetCPUFifo(cpuFifo);
    }
}

OSThread *GXSetCurrentGXThread(void) {
    BOOL enabled = OSDisableInterrupts();
    OSThread *prev = __GXCurrentThread;

    __GXCurrentThread = OSGetCurrentThread();
    OSRestoreInterrupts(enabled);
    return prev;
}

GXFifoObj *GXGetCPUFifo(void) {
    return (GXFifoObj *)CPUFifo;
}

GXFifoObj *GXGetGPFifo(void) {
    return (GXFifoObj *)GPFifo;
}

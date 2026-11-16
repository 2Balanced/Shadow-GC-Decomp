/* Dolphin SDK GX: miscellaneous control.
 *
 * FIFO flushing and aborting, draw synchronisation through pixel-engine tokens and the "draw
 * done" (finish) interrupt, and direct writes ("pokes") of pixel-engine registers that bypass
 * the FIFO. */
#include <dolphin/gx/GXPrivate.h>

static GXDrawSyncCallback TokenCB;
static GXDrawDoneCallback DrawDoneCB;
static GXBool DrawDone;
static OSThreadQueue FinishQueue;

/* Processor-interface registers; register 6 resets the graphics FIFO path while it holds 1. */
volatile u32 __GXPIRegs[] : 0xCC003000;
#define PI_FIFO_RESET (__GXPIRegs[6])

void GXSetMisc(GXMiscToken token, u32 val) {
    switch (token) {
    case 1: /* GX_MT_XF_FLUSH */
        gx->vNum = val;
        gx->vNumNot = !gx->vNum;
        gx->bpSentNot = GX_TRUE;
        if (gx->vNum) {
            gx->dirtyState |= GX_DIRTY_VCD;
        }
        break;
    case 2: /* GX_MT_DL_SAVE_CONTEXT */
        gx->dlSaveContext = (val != 0);
        break;
    case 3: /* GX_MT_ABORT_WAIT_COPYOUT */
        gx->abtWaitPECopy = (val != 0);
        break;
    case 0: /* GX_MT_NULL */
    default:
        break;
    }
}

void GXFlush(void) {
    if (gx->dirtyState) {
        __GXSetDirtyState();
    }
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    PPCSync();
}

/* Busy-waits for `clocks` bus clocks (the time base runs at a quarter of the bus clock). */
static inline void __GXAbortWait(u32 clocks) {
    OSTime time0;
    OSTime time1;

    time0 = OSGetTime();
    do {
        time1 = OSGetTime();
    } while (time1 - time0 <= clocks / 4);
}

/* Waits until the pixel engine has stopped requesting memory, i.e. any copy out has finished. */
static inline void __GXAbortWaitPECopyDone(void) {
    u32 peCnt0;
    u32 peCnt1;

    peCnt0 = __GXReadMEMCounterU32(MEM_PE_REQ_LO, MEM_PE_REQ_HI);
    do {
        peCnt1 = peCnt0;
        __GXAbortWait(32);
        peCnt0 = __GXReadMEMCounterU32(MEM_PE_REQ_LO, MEM_PE_REQ_HI);
    } while (peCnt0 != peCnt1);
}

void __GXAbort(void) {
    if (gx->abtWaitPECopy && GXGetGPFifo()) {
        __GXAbortWaitPECopyDone();
    }
    PI_FIFO_RESET = 1;
    __GXAbortWait(200);
    PI_FIFO_RESET = 0;
    __GXAbortWait(20);
}

void GXAbortFrame(void) {
    __GXAbort();
    if (GXGetGPFifo()) {
        __GXCleanGPFifo();
        __GXInitRevisionBits();
        gx->dirtyState = 0;
        GXFlush();
    }
}

void GXSetDrawSync(u16 token) {
    BOOL enabled = OSDisableInterrupts();
    u32 reg;

    reg = token | 0x48000000;
    GX_WRITE_RAS_REG(reg);
    SET_REG_FIELD(reg, 16, 0, token);
    SET_REG_FIELD(reg, 8, 24, 0x47);
    GX_WRITE_RAS_REG(reg);
    GXFlush();
    OSRestoreInterrupts(enabled);
    gx->bpSentNot = GX_FALSE;
}

u16 GXReadDrawSync(void) {
    return __peReg[7];
}

void GXSetDrawDone(void) {
    u32 reg;
    BOOL enabled = OSDisableInterrupts();

    reg = 0x45000002;
    GX_WRITE_RAS_REG(reg);
    GXFlush();
    DrawDone = GX_FALSE;
    OSRestoreInterrupts(enabled);
}

void GXWaitDrawDone(void) {
    BOOL enabled = OSDisableInterrupts();

    while (!DrawDone) {
        OSSleepThread(&FinishQueue);
    }
    OSRestoreInterrupts(enabled);
}

void GXDrawDone(void) {
    GXSetDrawDone();
    GXWaitDrawDone();
}

void GXPixModeSync(void) {
    GX_WRITE_RAS_REG(gx->peCtrl);
    gx->bpSentNot = GX_FALSE;
}

void GXPokeAlphaMode(GXCompare func, u8 threshold) {
    u32 reg;

    reg = (func << 8) | threshold;
    __peReg[3] = reg;
}

void GXPokeAlphaRead(int mode) {
    u32 reg;

    reg = 0;
    SET_REG_FIELD(reg, 2, 0, mode);
    SET_REG_FIELD(reg, 1, 2, 1);
    __peReg[4] = reg;
}

void GXPokeAlphaUpdate(GXBool update_enable) {
    u32 reg;

    reg = __peReg[1];
    SET_REG_FIELD(reg, 1, 4, update_enable);
    __peReg[1] = reg;
}

void GXPokeBlendMode(GXBlendMode type, GXBlendFactor src_factor, GXBlendFactor dst_factor,
                     GXLogicOp op) {
    u32 reg;

    reg = __peReg[1];
    SET_REG_FIELD(reg, 1, 0, (type == 1 || type == 3));
    SET_REG_FIELD(reg, 1, 11, (type == 3));
    SET_REG_FIELD(reg, 1, 1, (type == 2));
    SET_REG_FIELD(reg, 4, 12, op);
    SET_REG_FIELD(reg, 3, 8, src_factor);
    SET_REG_FIELD(reg, 3, 5, dst_factor);
    SET_REG_FIELD(reg, 8, 24, 0x41);
    __peReg[1] = reg;
}

void GXPokeColorUpdate(GXBool update_enable) {
    u32 reg;

    reg = __peReg[1];
    SET_REG_FIELD(reg, 1, 3, update_enable);
    __peReg[1] = reg;
}

void GXPokeDstAlpha(GXBool enable, u8 alpha) {
    u32 reg = 0;

    SET_REG_FIELD(reg, 8, 0, alpha);
    SET_REG_FIELD(reg, 1, 8, enable);
    __peReg[2] = reg;
}

void GXPokeDither(GXBool dither) {
    u32 reg;

    reg = __peReg[1];
    SET_REG_FIELD(reg, 1, 2, dither);
    __peReg[1] = reg;
}

void GXPokeZMode(GXBool compare_enable, GXCompare func, GXBool update_enable) {
    u32 reg = 0;

    SET_REG_FIELD(reg, 1, 0, compare_enable);
    SET_REG_FIELD(reg, 3, 1, func);
    SET_REG_FIELD(reg, 1, 4, update_enable);
    __peReg[0] = reg;
}

GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback cb) {
    GXDrawSyncCallback oldcb = TokenCB;
    BOOL enabled = OSDisableInterrupts();

    TokenCB = cb;
    OSRestoreInterrupts(enabled);
    return oldcb;
}

static void GXTokenInterruptHandler(__GXOSInterrupt interrupt, OSContext *context) {
    u16 token;
    OSContext exceptionContext;
    u32 reg;

    token = __peReg[7];
    if (TokenCB) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        TokenCB(token);
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
    reg = __peReg[5];
    SET_REG_FIELD(reg, 1, 2, 1);
    __peReg[5] = reg;
}

GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback cb) {
    GXDrawDoneCallback oldcb = DrawDoneCB;
    BOOL enabled = OSDisableInterrupts();

    DrawDoneCB = cb;
    OSRestoreInterrupts(enabled);
    return oldcb;
}

static void GXFinishInterruptHandler(__GXOSInterrupt interrupt, OSContext *context) {
    OSContext exceptionContext;
    u32 reg;

    reg = __peReg[5];
    SET_REG_FIELD(reg, 1, 3, 1);
    __peReg[5] = reg;
    DrawDone = GX_TRUE;
    if (DrawDoneCB) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        DrawDoneCB();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
    OSWakeupThread(&FinishQueue);
}

void __GXPEInit(void) {
    u32 reg;

    __OSSetInterruptHandler(18, GXTokenInterruptHandler);
    __OSSetInterruptHandler(19, GXFinishInterruptHandler);
    OSInitThreadQueue(&FinishQueue);
    __OSUnmaskInterrupts(0x2000);
    __OSUnmaskInterrupts(0x1000);
    reg = __peReg[5];
    SET_REG_FIELD(reg, 1, 2, 1);
    SET_REG_FIELD(reg, 1, 3, 1);
    SET_REG_FIELD(reg, 1, 0, 1);
    SET_REG_FIELD(reg, 1, 1, 1);
    __peReg[5] = reg;
}

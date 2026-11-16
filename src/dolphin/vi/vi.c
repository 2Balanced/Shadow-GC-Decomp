/* Dolphin SDK VI: the video interface.
 *
 * Every setting goes into a shadow copy of the VI registers (regs) together with a bit in the
 * 64-bit "changed" mask, register 0 in the top bit. VIFlush copies the changed registers into a
 * second shadow (shdwRegs) and the retrace interrupt handler writes those to the hardware at the
 * start of the next field, so a whole configuration takes effect at once. HorVer keeps the
 * display, pan and frame buffer geometry the registers are derived from.
 *
 * The OS thread calls keep dtk's names (nothing in the binary names them); the macros below give
 * them the public SDK names.
 *
 * Not linked yet, two functions differ in register choice only: in fn_8037E1B4 retail loads the
 * mask of a later `changed` bit one instruction later, which leaves r5 free for the first
 * update; in fn_8037E354 the inlined setPicConfig keeps the interlace mode in r0 and the
 * register value in r6, where this source has them the other way round. */
#include <dolphin/os.h>
#include <dolphin/os_interrupt.h>

volatile u16 __VIRegs[] : 0xCC002000;
u32 __VITvFormatInBootrom : 0x800000CC;

typedef void (*VIRetraceCallback)(u32 retraceCount);
typedef void (*VIPositionCallback)(s16 x, s16 y);

/* The part of GXRenderModeObj VIConfigure reads. */
typedef struct VIRenderMode {
    u32 viTVmode;
    u16 fbWidth;
    u16 efbHeight;
    u16 xfbHeight;
    u16 viXOrigin;
    u16 viYOrigin;
    u16 viWidth;
    u16 viHeight;
    u32 xFBmode;
} VIRenderMode;

typedef struct VITiming {
    u8 equ;
    u16 acv;
    u16 prbOdd;
    u16 prbEven;
    u16 psbOdd;
    u16 psbEven;
    u8 bs1;
    u8 bs2;
    u8 bs3;
    u8 bs4;
    u16 be1;
    u16 be2;
    u16 be3;
    u16 be4;
    u16 nhlines;
    u16 hlw;
    u8 hsy;
    u8 hcs;
    u8 hce;
    u8 hbe640;
    u16 hbs640;
    u8 hbeCCIR656;
    u16 hbsCCIR656;
} VITiming;

typedef struct VIHorVer {
    u16 DispPosX;
    u16 DispPosY;
    u16 DispSizeX;
    u16 DispSizeY;
    u16 AdjustedDispPosX;
    u16 AdjustedDispPosY;
    u16 AdjustedDispSizeY;
    u16 AdjustedPanPosY;
    u16 AdjustedPanSizeY;
    u16 FBSizeX;
    u16 FBSizeY;
    u16 PanPosX;
    u16 PanPosY;
    u16 PanSizeX;
    u16 PanSizeY;
    int FBMode;
    u32 nonInter;
    u32 tv;
    u8 wordPerLine;
    u8 std;
    u8 wpl;
    void *bufAddr;
    u32 tfbb;
    u32 bfbb;
    u8 xof;
    BOOL black;
    BOOL threeD;
    void *rbufAddr;
    u32 rtfbb;
    u32 rbfbb;
    VITiming *timing;
} VIHorVer;

/* Start of the SRAM image: the horizontal display offset lives at 0x10. */
typedef struct VISram {
    u16 checkSum;
    u16 checkSumInv;
    u32 ead0;
    u32 ead1;
    u32 counterBias;
    s8 displayOffsetH;
    u8 ntd;
    u8 language;
    u8 flags;
} VISram;

VISram *__OSLockSram(void);
BOOL __OSUnlockSram(BOOL commit);
void OSInitThreadQueue(OSThreadQueue *queue);
void fn_80372404(const char *file, int line, const char *msg, ...);
#define OSPanic fn_80372404
void fn_8037847C(OSThreadQueue *queue);
#define OSSleepThread fn_8037847C
void fn_80378568(OSThreadQueue *queue);
#define OSWakeupThread fn_80378568
void SIRefreshSamplingRate(void);

#define VI_BITMASK(index) (1ull << (63 - (index)))
#define CLAMP(x, l, h) (((x) > (h)) ? (h) : (((x) < (l)) ? (l) : (x)))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static void fn_8037D434(__OSInterrupt interrupt, OSContext *context);
static VITiming *fn_8037D730(u32 mode);
void fn_8037D7D8(u32 mode);
static void fn_8037DEE0(VIHorVer *HorVer, u32 *tfbb, u32 *bfbb, u32 *rtfbb, u32 *rbfbb);
static void fn_8037E1B4(u16 dispPosY, u16 dispSizeY, u8 equ, u16 acv, u16 prbOdd, u16 prbEven,
                        u16 psbOdd, u16 psbEven, BOOL black);
static void fn_8037F118(u32 *hct, u32 *vct);
static u32 fn_8037F154(void);
void fn_8037F394(u32 hct, u32 vct, s16 *x, s16 *y);
void fn_8037F5B0(s16 *x, s16 *y);

const char *__VIVersion = "<< Dolphin SDK - VI\trelease build: Apr  7 2004 04:13:59 (0x2301) >>";

static VITiming timing[10] = {
    {6, 240, 24, 25, 3, 2, 12, 13, 12, 13, 520, 519, 520, 519, 525, 429, 64, 71, 105, 162, 373, 122, 412},
    {6, 240, 24, 24, 4, 4, 12, 12, 12, 12, 520, 520, 520, 520, 526, 429, 64, 71, 105, 162, 373, 122, 412},
    {5, 287, 35, 36, 1, 0, 13, 12, 11, 10, 619, 618, 617, 620, 625, 432, 64, 75, 106, 172, 380, 133, 420},
    {5, 287, 33, 33, 2, 2, 13, 11, 13, 11, 619, 621, 619, 621, 624, 432, 64, 75, 106, 172, 380, 133, 420},
    {6, 240, 24, 25, 3, 2, 16, 15, 14, 13, 518, 517, 516, 519, 525, 429, 64, 78, 112, 162, 373, 122, 412},
    {6, 240, 24, 24, 4, 4, 16, 14, 16, 14, 518, 520, 518, 520, 526, 429, 64, 78, 112, 162, 373, 122, 412},
    {12, 480, 48, 48, 6, 6, 24, 24, 24, 24, 1038, 1038, 1038, 1038, 1050, 429, 64, 71, 105, 162, 373, 122, 412},
    {12, 480, 44, 44, 10, 10, 24, 24, 24, 24, 1038, 1038, 1038, 1038, 1050, 429, 64, 71, 105, 168, 379, 122, 412},
    {6, 241, 24, 25, 1, 0, 12, 13, 12, 13, 520, 519, 520, 519, 525, 429, 64, 71, 105, 159, 370, 122, 412},
    {12, 480, 48, 48, 6, 6, 24, 24, 24, 24, 1038, 1038, 1038, 1038, 1050, 429, 64, 71, 105, 180, 391, 122, 412},
};

/* Anti-aliasing filter coefficients written to registers 38-51 by VIInit. */
static u16 taps[25] = {496, 476, 430, 372, 297, 219, 142, 70, 12, 226, 203, 192, 196, 207, 222, 236, 252, 8, 15, 19, 19, 15, 12, 8, 1};

static BOOL IsInitialized;
static volatile u32 retraceCount;
static u32 flushFlag;
static OSThreadQueue retraceQueue;
static VIRetraceCallback PreCB;
static VIRetraceCallback PostCB;
static VIPositionCallback PositionCallback;
static u32 encoderType;
static s16 displayOffsetH;
static s16 displayOffsetV;
static volatile u32 changeMode;
static volatile u64 changed;
static volatile u32 shdwChangeMode;
static volatile u64 shdwChanged;
static VITiming *CurrTiming;
static u32 CurrTvMode;
static void *NextBufAddr;
static void *CurrBufAddr;
static u32 FBSet;
static VITiming *timingExtra;

static u16 regs[59];
static u16 shdwRegs[59];
static VIHorVer HorVer;

static inline u32 cntlzd(u64 bit) {
    u32 hi = (u32)(bit >> 32);
    u32 lo = (u32)(bit & 0xFFFFFFFF);
    s32 value = __cntlzw(hi);

    if (value < 32) {
        return value;
    }
    return 32 + __cntlzw(lo);
}

static inline BOOL VISetRegs(void) {
    s32 regIndex;

    if (!(shdwChangeMode == 1 && !fn_8037F154())) {
        while (shdwChanged) {
            regIndex = cntlzd(shdwChanged);
            __VIRegs[regIndex] = shdwRegs[regIndex];
            shdwChanged &= ~VI_BITMASK(regIndex);
        }
        shdwChangeMode = 0;
        CurrTiming = HorVer.timing;
        CurrTvMode = HorVer.tv;
        CurrBufAddr = NextBufAddr;
        return TRUE;
    }
    return FALSE;
}

/* Retrace interrupt: acknowledge the four display interrupts, then on a vertical retrace run the
 * callbacks around the register flush and wake VIWaitForRetrace. */
static void fn_8037D434(__OSInterrupt interrupt, OSContext *context) {
    OSContext exceptionContext;
    u16 reg;
    u32 inter = 0;

    reg = __VIRegs[24];
    if (reg & 0x8000) {
        __VIRegs[24] = reg & ~0x8000;
        inter |= 1;
    }
    reg = __VIRegs[26];
    if (reg & 0x8000) {
        __VIRegs[26] = reg & ~0x8000;
        inter |= 2;
    }
    reg = __VIRegs[28];
    if (reg & 0x8000) {
        __VIRegs[28] = reg & ~0x8000;
        inter |= 4;
    }
    reg = __VIRegs[30];
    if (reg & 0x8000) {
        __VIRegs[30] = reg & ~0x8000;
        inter |= 8;
    }

    if ((inter & 4) || (inter & 8)) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        if (PositionCallback) {
            s16 x, y;

            fn_8037F5B0(&x, &y);
            PositionCallback(x, y);
        }
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
        return;
    }

    retraceCount++;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (PreCB) {
        PreCB(retraceCount);
    }
    if (flushFlag) {
        if (VISetRegs()) {
            flushFlag = 0;
            SIRefreshSamplingRate();
        }
    }
    if (PostCB) {
        OSClearContext(&exceptionContext);
        PostCB(retraceCount);
    }
    OSWakeupThread(&retraceQueue);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

VIRetraceCallback fn_8037D6A8(VIRetraceCallback callback) {
    BOOL enabled;
    VIRetraceCallback old = PreCB;

    enabled = OSDisableInterrupts();
    PreCB = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

VIRetraceCallback fn_8037D6EC(VIRetraceCallback callback) {
    BOOL enabled;
    VIRetraceCallback old = PostCB;

    enabled = OSDisableInterrupts();
    PostCB = callback;
    OSRestoreInterrupts(enabled);
    return old;
}

/* Timing for (TV format << 2) + scan mode. */
static VITiming *fn_8037D730(u32 mode) {
    switch (mode) {
    case 0:
        return &timing[0];
    case 1:
        return &timing[1];
    case 4:
        return &timing[2];
    case 5:
        return &timing[3];
    case 20:
        return &timing[0];
    case 21:
        return &timing[1];
    case 8:
        return &timing[4];
    case 9:
        return &timing[5];
    case 2:
        return &timing[6];
    case 3:
        return &timing[7];
    case 16:
        return &timing[2];
    case 17:
        return &timing[3];
    case 24:
        return &timing[8];
    case 26:
        return &timing[9];
    case 28:
    case 29:
    case 30:
        return timingExtra;
    }
    return NULL;
}

void fn_8037D7D8(u32 mode) {
    volatile u32 a;
    /* Never used: retail reserves three more stack words below the delay counter. */
    u32 unused1;
    u32 unused2;
    u32 unused3;
    VITiming *tm;
    u32 nonInter;
    u32 tv;
    u32 tvBits;
    u16 hct;
    u16 vct;

    nonInter = mode & 3;
    tv = mode >> 2;
    __VITvFormatInBootrom = tv;
    tm = fn_8037D730(mode);

    __VIRegs[1] = 2;
    /* Hold the reset bit for a short busy wait; retail tests the bound once before the loop. */
    a = 0;
    if (a < 1000) {
        for (; a < 1000; a += 8) {
        }
    }
    __VIRegs[1] = 0;

    __VIRegs[3] = tm->hlw;
    __VIRegs[2] = tm->hce | (tm->hcs << 8);
    __VIRegs[5] = tm->hsy | (tm->hbe640 << 7);
    __VIRegs[4] = tm->hbs640 << 1;
    __VIRegs[0] = tm->equ;
    __VIRegs[7] = tm->prbOdd + tm->acv * 2 - 2;
    __VIRegs[6] = tm->psbOdd + 2;
    __VIRegs[9] = tm->prbEven + tm->acv * 2 - 2;
    __VIRegs[8] = tm->psbEven + 2;
    __VIRegs[11] = tm->bs1 | (tm->be1 << 5);
    __VIRegs[10] = tm->bs3 | (tm->be3 << 5);
    __VIRegs[13] = tm->bs2 | (tm->be2 << 5);
    __VIRegs[12] = tm->bs4 | (tm->be4 << 5);
    __VIRegs[36] = 0x2828;
    __VIRegs[27] = 1;
    __VIRegs[26] = 0x1001;

    hct = tm->hlw + 1;
    vct = tm->nhlines / 2 + 1;
    __VIRegs[25] = hct;
    __VIRegs[24] = vct | 0x1000;

    switch (tv) {
    case 1:
    case 2:
    case 3:
        tvBits = tv;
        break;
    default:
        tvBits = 0;
        break;
    }

    if (nonInter <= 1) {
        __VIRegs[1] = ((nonInter << 2) & 4) | 1 | (tvBits << 8);
        __VIRegs[54] = 0;
    } else {
        __VIRegs[1] = (tvBits << 8) | 5;
        __VIRegs[54] = 1;
    }
}

/* Apply the SRAM display offsets to the requested display rectangle and clip it, with the pan
 * rectangle, to the lines the timing can show. */
static inline void AdjustPosition(u16 acv) {
    s32 coeff;
    s32 frac;

    HorVer.AdjustedDispPosX =
        (u16)CLAMP((s16)HorVer.DispPosX + displayOffsetH, 0, 720 - HorVer.DispSizeX);
    coeff = (HorVer.FBMode == 0) ? 2 : 1;
    frac = HorVer.DispPosY & 1;
    HorVer.AdjustedDispPosY = (u16)MAX((s16)HorVer.DispPosY + displayOffsetV, frac);
    HorVer.AdjustedDispSizeY =
        HorVer.DispSizeY + MIN((s16)HorVer.DispPosY + displayOffsetV - frac, 0) -
        MAX((s16)HorVer.DispPosY + (s16)HorVer.DispSizeY + displayOffsetV - ((s16)acv * 2 - frac),
            0);
    HorVer.AdjustedPanPosY =
        HorVer.PanPosY - MIN((s16)HorVer.DispPosY + displayOffsetV - frac, 0) / coeff;
    HorVer.AdjustedPanSizeY =
        HorVer.PanSizeY + MIN((s16)HorVer.DispPosY + displayOffsetV - frac, 0) / coeff -
        MAX((s16)HorVer.DispPosY + (s16)HorVer.DispSizeY + displayOffsetV - ((s16)acv * 2 - frac),
            0) / coeff;
}

void fn_8037D9DC(void) {
    u16 dspCr;
    u32 value;
    u32 tv;
    VISram *sram;

    if (IsInitialized) {
        return;
    }
    OSRegisterVersion(__VIVersion);
    IsInitialized = TRUE;
    encoderType = 1;

    if (!(__VIRegs[1] & 1)) {
        fn_8037D7D8(0);
    }

    retraceCount = 0;
    changed = 0;
    shdwChanged = 0;
    changeMode = 0;
    shdwChangeMode = 0;
    flushFlag = 0;

    __VIRegs[39] = taps[0] | ((taps[1] & 0x3F) << 10);
    __VIRegs[38] = (taps[1] >> 6) | (taps[2] << 4);
    __VIRegs[41] = taps[3] | ((taps[4] & 0x3F) << 10);
    __VIRegs[40] = (taps[4] >> 6) | (taps[5] << 4);
    __VIRegs[43] = taps[6] | ((taps[7] & 0x3F) << 10);
    __VIRegs[42] = (taps[7] >> 6) | (taps[8] << 4);
    __VIRegs[45] = taps[9] | (taps[10] << 8);
    __VIRegs[44] = taps[11] | (taps[12] << 8);
    __VIRegs[47] = taps[13] | (taps[14] << 8);
    __VIRegs[46] = taps[15] | (taps[16] << 8);
    __VIRegs[49] = taps[17] | (taps[18] << 8);
    __VIRegs[48] = taps[19] | (taps[20] << 8);
    __VIRegs[51] = taps[21] | (taps[22] << 8);
    __VIRegs[50] = taps[23] | (taps[24] << 8);
    __VIRegs[56] = 640;

    sram = __OSLockSram();
    displayOffsetH = sram->displayOffsetH;
    displayOffsetV = 0;
    __OSUnlockSram(FALSE);

    dspCr = __VIRegs[1];
    value = __VITvFormatInBootrom;
    HorVer.nonInter = (dspCr & 4) >> 2;
    HorVer.tv = (dspCr & 0x300) >> 8;
    if (value == 1 && HorVer.tv == 0) {
        HorVer.tv = 5;
    }
    tv = (HorVer.tv == 3) ? 0 : HorVer.tv;
    HorVer.timing = fn_8037D730((tv << 2) + HorVer.nonInter);
    regs[1] = dspCr;
    CurrTiming = HorVer.timing;
    CurrTvMode = HorVer.tv;

    HorVer.DispSizeX = 640;
    HorVer.DispSizeY = CurrTiming->acv * 2;
    HorVer.DispPosX = (720 - HorVer.DispSizeX) / 2;
    HorVer.DispPosY = 0;
    AdjustPosition(CurrTiming->acv);

    HorVer.FBSizeX = 640;
    HorVer.FBSizeY = CurrTiming->acv * 2;
    HorVer.PanPosX = 0;
    HorVer.PanPosY = 0;
    HorVer.PanSizeX = 640;
    HorVer.PanSizeY = CurrTiming->acv * 2;
    HorVer.FBMode = 0;
    HorVer.wordPerLine = 40;
    HorVer.std = 40;
    HorVer.wpl = 40;
    HorVer.xof = 0;
    HorVer.black = TRUE;
    HorVer.threeD = FALSE;

    OSInitThreadQueue(&retraceQueue);
    /* Written out (not `&=`): retail forms the register address once to read and again to
     * write. */
    __VIRegs[24] = __VIRegs[24] & 0x7FFF;
    __VIRegs[26] = __VIRegs[26] & 0x7FFF;
    PreCB = NULL;
    PostCB = NULL;
    __OSSetInterruptHandler(24, fn_8037D434);
    __OSUnmaskInterrupts(0x80);
}

void fn_8037DE8C(void) {
    BOOL enabled;
    u32 count;

    enabled = OSDisableInterrupts();
    count = retraceCount;
    do {
        OSSleepThread(&retraceQueue);
    } while (count == retraceCount);
    OSRestoreInterrupts(enabled);
}

/* Top and bottom field base addresses of one frame buffer. */
static inline void GetFbbs(void *bufAddr, u16 panPosX, u16 panPosY, u8 wordPerLine, int mode,
                           u16 dispPosY, u32 *tfbb, u32 *bfbb) {
    u32 bytesPerLine;
    u32 xoffInWords;
    u32 tmp;

    xoffInWords = (u32)panPosX / 16;
    bytesPerLine = (u32)wordPerLine << 5;
    *tfbb = (u32)bufAddr + (xoffInWords << 5) + bytesPerLine * panPosY;
    *bfbb = (mode == 0) ? *tfbb : (*tfbb + bytesPerLine);
    if (dispPosY % 2 == 1) {
        tmp = *tfbb;
        *tfbb = *bfbb;
        *bfbb = tmp;
    }
    *tfbb &= 0x3FFFFFFF;
    *bfbb &= 0x3FFFFFFF;
}

static void fn_8037DEE0(VIHorVer *HorVer, u32 *tfbb, u32 *bfbb, u32 *rtfbb, u32 *rbfbb) {
    u32 shifted;

    GetFbbs(HorVer->bufAddr, HorVer->PanPosX, HorVer->AdjustedPanPosY, HorVer->wordPerLine,
            HorVer->FBMode, HorVer->AdjustedDispPosY, tfbb, bfbb);
    if (HorVer->threeD) {
        GetFbbs(HorVer->rbufAddr, HorVer->PanPosX, HorVer->AdjustedPanPosY, HorVer->wordPerLine,
                HorVer->FBMode, HorVer->AdjustedDispPosY, rtfbb, rbfbb);
    }

    if (*tfbb < 0x01000000 && *bfbb < 0x01000000 && *rtfbb < 0x01000000 && *rbfbb < 0x01000000) {
        shifted = 0;
    } else {
        shifted = 1;
    }
    if (shifted) {
        *tfbb >>= 5;
        *bfbb >>= 5;
        *rtfbb >>= 5;
        *rbfbb >>= 5;
    }

    regs[15] = (u16)*tfbb;
    changed |= VI_BITMASK(15);
    regs[14] = (u16)((*tfbb >> 16) | (HorVer->xof << 8) | (shifted << 12));
    changed |= VI_BITMASK(14);
    regs[19] = (u16)*bfbb;
    changed |= VI_BITMASK(19);
    regs[18] = (u16)(*bfbb >> 16);
    changed |= VI_BITMASK(18);

    if (HorVer->threeD) {
        regs[17] = (u16)*rtfbb;
        changed |= VI_BITMASK(17);
        regs[16] = (u16)(*rtfbb >> 16);
        changed |= VI_BITMASK(16);
        regs[21] = (u16)*rbfbb;
        changed |= VI_BITMASK(21);
        regs[20] = (u16)(*rbfbb >> 16);
        changed |= VI_BITMASK(20);
    }
}

static void fn_8037E1B4(u16 dispPosY, u16 dispSizeY, u8 equ, u16 acv, u16 prbOdd, u16 prbEven,
                        u16 psbOdd, u16 psbEven, BOOL black) {
    u16 actualPrbOdd;
    u16 actualPrbEven;
    u16 actualPsbOdd;
    u16 actualPsbEven;
    u16 actualAcv;
    u16 c;
    u16 d;

    if (regs[54] & 1) {
        c = 1;
        d = 2;
    } else {
        c = 2;
        d = 1;
    }

    if (dispPosY % 2 == 0) {
        actualPrbOdd = prbOdd + d * dispPosY;
        actualPsbOdd = psbOdd + d * ((c * acv - dispSizeY) - dispPosY);
        actualPrbEven = prbEven + d * dispPosY;
        actualPsbEven = psbEven + d * ((c * acv - dispSizeY) - dispPosY);
    } else {
        actualPrbOdd = prbEven + d * dispPosY;
        actualPsbOdd = psbEven + d * ((c * acv - dispSizeY) - dispPosY);
        actualPrbEven = prbOdd + d * dispPosY;
        actualPsbEven = psbOdd + d * ((c * acv - dispSizeY) - dispPosY);
    }

    actualAcv = dispSizeY / c;
    if (black) {
        actualPrbOdd += 2 * actualAcv - 2;
        actualPrbEven += 2 * actualAcv - 2;
        actualPsbOdd += 2;
        actualPsbEven += 2;
        actualAcv = 0;
    }

    regs[0] = equ | (actualAcv << 4);
    changed |= VI_BITMASK(0);
    regs[7] = actualPrbOdd;
    changed |= VI_BITMASK(7);
    regs[6] = actualPsbOdd;
    changed |= VI_BITMASK(6);
    regs[9] = actualPrbEven;
    changed |= VI_BITMASK(9);
    regs[8] = actualPsbEven;
    changed |= VI_BITMASK(8);
}

static inline void setInterruptRegs(VITiming *tm) {
    u16 vct;
    u16 borrow;
    u32 hct;

    vct = tm->nhlines / 2;
    borrow = tm->nhlines % 2;
    hct = borrow ? tm->hlw : 0;
    regs[25] = hct + 1;
    changed |= VI_BITMASK(25);
    regs[24] = (vct + 1) | 0x1000;
    changed |= VI_BITMASK(24);
}

static inline void setPicConfig(u32 nonInter) {
    u32 reg;
    u32 clk;

    reg = regs[1];
    clk = regs[54];
    if (nonInter == 2 || nonInter == 3) {
        reg = (reg & ~4) | 4;
        clk = (clk & ~1) | 1;
    } else {
        reg = (reg & ~4) | ((nonInter & 1) << 2);
        clk = clk & ~1;
    }
    reg = (reg & ~8) | (HorVer.threeD << 3);
    if (HorVer.tv == 1 || HorVer.tv == 2 || HorVer.tv == 3) {
        reg = (reg & ~0x300) | (HorVer.tv << 8);
    } else {
        reg = reg & ~0x300;
    }
    regs[1] = reg;
    regs[54] = clk;
    changed |= VI_BITMASK(1);
    changed |= VI_BITMASK(54);
}

static inline void setScalingRegs(u16 panSizeX, u16 dispSizeX, BOOL threeD) {
    u32 scale;
    u16 size;

    size = threeD ? (panSizeX << 1) : panSizeX;
    if (size < dispSizeX) {
        scale = (256 * (u32)size + (u32)dispSizeX - 1) / dispSizeX;
        regs[37] = scale | 0x1000;
        changed |= VI_BITMASK(37);
        regs[56] = size;
        changed |= VI_BITMASK(56);
    } else {
        regs[37] = 0x100;
        changed |= VI_BITMASK(37);
    }
}

static inline void setHorizontalRegs(VITiming *tm, u16 dispPosX, u16 dispSizeX) {
    u32 hbe;
    u32 hbs;

    regs[3] = tm->hlw;
    changed |= VI_BITMASK(3);
    regs[2] = tm->hce | (tm->hcs << 8);
    changed |= VI_BITMASK(2);
    hbe = tm->hbe640 + dispPosX;
    hbe -= 40;
    hbs = tm->hbs640 + dispPosX;
    hbs += 40;
    hbs -= 720 - dispSizeX;
    regs[5] = tm->hsy | ((hbe & 0x1FF) << 7);
    changed |= VI_BITMASK(5);
    regs[4] = (hbe >> 9) | (hbs << 1);
    changed |= VI_BITMASK(4);
}

static inline void setBurstBlankRegs(VITiming *tm) {
    regs[11] = tm->bs1 | (tm->be1 << 5);
    changed |= VI_BITMASK(11);
    regs[10] = tm->bs3 | (tm->be3 << 5);
    changed |= VI_BITMASK(10);
    regs[13] = tm->bs2 | (tm->be2 << 5);
    changed |= VI_BITMASK(13);
    regs[12] = tm->bs4 | (tm->be4 << 5);
    changed |= VI_BITMASK(12);
}

/* Words per line, pan offset inside the first word and the picture width register. */
static inline void setWordRegs(u16 fbSizeX, int fbMode, u16 panPosX, u16 panSizeX) {
    HorVer.wordPerLine = (fbSizeX + 15) / 16;
    HorVer.std = (fbMode == 0) ? HorVer.wordPerLine : (u8)(2 * HorVer.wordPerLine);
    HorVer.xof = panPosX % 16;
    HorVer.wpl = (HorVer.xof + panSizeX + 15) / 16;
    regs[36] = HorVer.std | (HorVer.wpl << 8);
    changed |= VI_BITMASK(36);
}

void fn_8037E354(const VIRenderMode *rm) {
    static u32 message = FALSE;
    VITiming *tm;
    BOOL enabled;
    u32 newNonInter;
    u32 tvInBootrom;
    u32 tvInGame;

    enabled = OSDisableInterrupts();
    newNonInter = rm->viTVmode & 3;
    if (HorVer.nonInter != newNonInter) {
        changeMode = 1;
        HorVer.nonInter = newNonInter;
    }

    tvInGame = rm->viTVmode >> 2;
    tvInBootrom = __VITvFormatInBootrom;
    if (tvInGame == 4 && !message) {
        message = TRUE;
        OSReport("***************************************\n");
        OSReport(" ! ! ! C A U T I O N ! ! !             \n");
        OSReport("This TV format \"DEBUG_PAL\" is only for \n");
        OSReport("temporary solution until PAL DAC board \n");
        OSReport("is available. Please do NOT use this   \n");
        OSReport("mode in real games!!!                  \n");
        OSReport("***************************************\n");
    }

    switch (tvInBootrom) {
    case 0:
    case 2:
    case 6:
    case 7:
        if (tvInGame == 0 || tvInGame == 2 || tvInGame == 6 || tvInGame == 7) {
            break;
        }
        goto panic;
    case 1:
    case 5:
        if (tvInGame == 1 || tvInGame == 5) {
            break;
        }
    default:
    panic:
        OSPanic("vi.c", 1979,
                "VIConfigure(): Tried to change mode from (%d) to (%d), which is forbidden\n",
                tvInBootrom, tvInGame);
    }

    if (tvInGame == 0 || tvInGame == 2) {
        HorVer.tv = tvInBootrom;
    } else {
        HorVer.tv = tvInGame;
    }

    HorVer.DispPosX = rm->viXOrigin;
    HorVer.DispPosY = (HorVer.nonInter == 1) ? (u16)(rm->viYOrigin * 2) : rm->viYOrigin;
    HorVer.DispSizeX = rm->viWidth;
    HorVer.FBSizeX = rm->fbWidth;
    HorVer.FBSizeY = rm->xfbHeight;
    HorVer.FBMode = rm->xFBmode;
    HorVer.PanSizeX = HorVer.FBSizeX;
    HorVer.PanSizeY = HorVer.FBSizeY;
    HorVer.PanPosX = 0;
    HorVer.PanPosY = 0;
    HorVer.DispSizeY = (HorVer.nonInter == 2)   ? HorVer.PanSizeY
                       : (HorVer.nonInter == 3) ? HorVer.PanSizeY
                       : (HorVer.FBMode == 0)   ? (u16)(HorVer.PanSizeY * 2)
                                                : HorVer.PanSizeY;
    HorVer.threeD = (HorVer.nonInter == 3) ? TRUE : FALSE;

    tm = fn_8037D730((HorVer.tv << 2) + HorVer.nonInter);
    HorVer.timing = tm;
    AdjustPosition(tm->acv);
    if (encoderType == 0) {
        HorVer.tv = 3;
    }

    setInterruptRegs(tm);
    setPicConfig(HorVer.nonInter);
    setScalingRegs(HorVer.PanSizeX, HorVer.DispSizeX, HorVer.threeD);
    setHorizontalRegs(tm, HorVer.AdjustedDispPosX, HorVer.DispSizeX);
    setBurstBlankRegs(tm);
    setWordRegs(HorVer.FBSizeX, HorVer.FBMode, HorVer.PanPosX, HorVer.PanSizeX);
    if (FBSet) {
        fn_8037DEE0(&HorVer, &HorVer.tfbb, &HorVer.bfbb, &HorVer.rtfbb, &HorVer.rbfbb);
    }
    fn_8037E1B4(HorVer.AdjustedDispPosY, HorVer.AdjustedDispSizeY, tm->equ, tm->acv, tm->prbOdd,
                tm->prbEven, tm->psbOdd, tm->psbEven, HorVer.black);
    OSRestoreInterrupts(enabled);
}

void fn_8037EB5C(u16 xOrg, u16 yOrg, u16 width, u16 height) {
    BOOL enabled;
    VITiming *tm;

    enabled = OSDisableInterrupts();
    HorVer.PanPosX = xOrg;
    HorVer.PanPosY = yOrg;
    HorVer.PanSizeX = width;
    HorVer.PanSizeY = height;
    HorVer.DispSizeY = (HorVer.nonInter == 2)   ? HorVer.PanSizeY
                       : (HorVer.nonInter == 3) ? HorVer.PanSizeY
                       : (HorVer.FBMode == 0)   ? (u16)(HorVer.PanSizeY * 2)
                                                : HorVer.PanSizeY;
    tm = HorVer.timing;
    AdjustPosition(tm->acv);
    setScalingRegs(HorVer.PanSizeX, HorVer.DispSizeX, HorVer.threeD);
    setWordRegs(HorVer.FBSizeX, HorVer.FBMode, HorVer.PanPosX, HorVer.PanSizeX);
    if (FBSet) {
        fn_8037DEE0(&HorVer, &HorVer.tfbb, &HorVer.bfbb, &HorVer.rtfbb, &HorVer.rbfbb);
    }
    fn_8037E1B4(HorVer.AdjustedDispPosY, HorVer.DispSizeY, tm->equ, tm->acv, tm->prbOdd,
                tm->prbEven, tm->psbOdd, tm->psbEven, HorVer.black);
    OSRestoreInterrupts(enabled);
}

void fn_8037EEF0(void) {
    u32 unused; /* never used; retail's frame has room for it */
    BOOL enabled;
    s32 regIndex;

    enabled = OSDisableInterrupts();
    shdwChangeMode |= changeMode;
    changeMode = 0;
    shdwChanged |= changed;
    while (changed) {
        regIndex = cntlzd(changed);
        shdwRegs[regIndex] = regs[regIndex];
        changed &= ~VI_BITMASK(regIndex);
    }
    flushFlag = 1;
    NextBufAddr = HorVer.bufAddr;
    OSRestoreInterrupts(enabled);
}

void fn_8037F020(void *fb) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    HorVer.bufAddr = fb;
    FBSet = 1;
    fn_8037DEE0(&HorVer, &HorVer.tfbb, &HorVer.bfbb, &HorVer.rtfbb, &HorVer.rbfbb);
    OSRestoreInterrupts(enabled);
}

void *fn_8037F08C(void) {
    return NextBufAddr;
}

void fn_8037F094(BOOL black) {
    BOOL enabled;
    VITiming *tm;

    enabled = OSDisableInterrupts();
    HorVer.black = black;
    tm = HorVer.timing;
    fn_8037E1B4(HorVer.AdjustedDispPosY, HorVer.DispSizeY, tm->equ, tm->acv, tm->prbOdd,
                tm->prbEven, tm->psbOdd, tm->psbEven, HorVer.black);
    OSRestoreInterrupts(enabled);
}

u32 fn_8037F110(void) {
    return retraceCount;
}

/* Beam position; the vertical count is read on both sides of the horizontal count so the pair is
 * consistent. */
static void fn_8037F118(u32 *hct, u32 *vct) {
    u32 hcount;
    u32 vcount0;
    u32 vcount;

    vcount = __VIRegs[22] & 0x7FF;
    do {
        vcount0 = vcount;
        hcount = __VIRegs[23] & 0x7FF;
        vcount = __VIRegs[22] & 0x7FF;
    } while (vcount0 != vcount);
    *hct = hcount;
    *vct = vcount;
}

static inline u32 getCurrentHalfLine(void) {
    u32 hcount;
    u32 vcount;

    fn_8037F118(&hcount, &vcount);
    return ((vcount - 1) << 1) + ((hcount - 1) / CurrTiming->hlw);
}

static u32 fn_8037F154(void) {
    return (getCurrentHalfLine() < CurrTiming->nhlines) ? 1 : 0;
}

/* The one caller that reaches the beam position reader out of line. */
#pragma dont_inline on
u32 fn_8037F1BC(void) {
    /* Never used: retail keeps a free stack word on either side of the two counters. */
    u32 unused1;
    u32 vct;
    u32 hct;
    u32 unused2;
    u32 nextField;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    fn_8037F118(&hct, &vct);
    nextField = (((vct - 1) << 1) + ((hct - 1) / CurrTiming->hlw) < CurrTiming->nhlines) ? 1 : 0;
    OSRestoreInterrupts(enabled);
    return (nextField ^ 1) ^ (HorVer.AdjustedDispPosY & 1);
}

#pragma dont_inline reset

u32 fn_8037F258(void) {
    u32 halfLine;
    VITiming *tm;
    BOOL enabled;

    tm = CurrTiming;
    enabled = OSDisableInterrupts();
    halfLine = getCurrentHalfLine();
    OSRestoreInterrupts(enabled);
    if (halfLine >= tm->nhlines) {
        halfLine -= tm->nhlines;
    }
    return halfLine >> 1;
}

u32 VIGetTvFormat(void) {
    u32 format;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    switch (CurrTvMode) {
    case 0:
    case 3:
    case 6:
    case 7:
        format = 0;
        break;
    case 1:
    case 4:
        format = 1;
        break;
    case 2:
    case 5:
        format = CurrTvMode;
        break;
    }
    OSRestoreInterrupts(enabled);
    return format;
}

u32 fn_8037F358(void) {
    u32 dtv;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    dtv = __VIRegs[55] & 3;
    OSRestoreInterrupts(enabled);
    return dtv & 1;
}

/* Beam position to picture coordinates; y is -1 outside the active lines. */
void fn_8037F394(u32 hct, u32 vct, s16 *x, s16 *y) {
    u32 halfLine = ((vct - 1) << 1) + ((hct - 1) / CurrTiming->hlw);

    if (HorVer.nonInter == 0) {
        if (halfLine < CurrTiming->nhlines) {
            if (halfLine < CurrTiming->equ * 3 + CurrTiming->prbOdd) {
                *y = -1;
            } else if (halfLine >= CurrTiming->nhlines - CurrTiming->psbOdd) {
                *y = -1;
            } else {
                *y = (s16)((halfLine - CurrTiming->equ * 3 - CurrTiming->prbOdd) & ~1);
            }
        } else {
            halfLine -= CurrTiming->nhlines;
            if (halfLine < CurrTiming->equ * 3 + CurrTiming->prbEven) {
                *y = -1;
            } else if (halfLine >= CurrTiming->nhlines - CurrTiming->psbEven) {
                *y = -1;
            } else {
                *y = (s16)(((halfLine - CurrTiming->equ * 3 - CurrTiming->prbEven) & ~1) + 1);
            }
        }
    } else if (HorVer.nonInter == 1) {
        if (halfLine >= CurrTiming->nhlines) {
            halfLine -= CurrTiming->nhlines;
        }
        if (halfLine < CurrTiming->equ * 3 + CurrTiming->prbOdd) {
            *y = -1;
        } else if (halfLine >= CurrTiming->nhlines - CurrTiming->psbOdd) {
            *y = -1;
        } else {
            *y = (s16)((halfLine - CurrTiming->equ * 3 - CurrTiming->prbOdd) & ~1);
        }
    } else if (HorVer.nonInter == 2) {
        if (halfLine < CurrTiming->nhlines) {
            if (halfLine < CurrTiming->equ * 3 + CurrTiming->prbOdd) {
                *y = -1;
            } else if (halfLine >= CurrTiming->nhlines - CurrTiming->psbOdd) {
                *y = -1;
            } else {
                *y = (s16)(halfLine - CurrTiming->equ * 3 - CurrTiming->prbOdd);
            }
        } else {
            halfLine -= CurrTiming->nhlines;
            if (halfLine < CurrTiming->equ * 3 + CurrTiming->prbEven) {
                *y = -1;
            } else if (halfLine >= CurrTiming->nhlines - CurrTiming->psbEven) {
                *y = -1;
            } else {
                *y = (s16)((halfLine - CurrTiming->equ * 3 - CurrTiming->prbEven) & ~1);
            }
        }
    }
    *x = (s16)(hct - 1);
}

void fn_8037F5B0(s16 *x, s16 *y) {
    u32 hcount;
    u32 vcount;

    fn_8037F118(&hcount, &vcount);
    fn_8037F394(hcount, vcount, x, y);
}

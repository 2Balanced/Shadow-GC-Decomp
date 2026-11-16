/* Dolphin SDK AI: the audio interface.
 *
 * Two audio paths meet here. The DSP's mixed output reaches the AI by DMA from main RAM (the
 * "AID" path, programmed through the DSP interface registers at 0xCC005030), and the disc drive's
 * streamed audio comes in through the AI's own control register block at 0xCC006C00 (the "AIS"
 * path). Changing either sample rate has to resynchronise the sample-rate converter, which
 * __AI_SRC_INIT does by timing one 32 kHz and one 48 kHz stream sample against the time base.
 * Only the calls the game links survive; the DMA queries and stream-callback setters were
 * dead-stripped. */
#include <dolphin/os.h>
#include <dolphin/os_interrupt.h>

volatile u16 __DSPRegs[] : 0xCC005000;
volatile u32 __AIRegs[] : 0xCC006C00;

typedef void (*AISCallback)(u32 count);
typedef void (*AIDCallback)(void);

/* AI control register (__AIRegs[0]) bits. */
#define AICR_PSTAT    0x01 /* stream playing */
#define AICR_AFR      0x02 /* stream sample rate: set = 48 kHz */
#define AICR_AISINTVLD 0x08 /* stream interrupt acknowledge */
#define AICR_SCRESET  0x20 /* reset the stream sample counter */
#define AICR_DSP32    0x40 /* DSP sample rate: set = 32 kHz */

/* DSP control register (__DSPRegs[5]) interrupt bits. */
#define DSPCR_AIDINT  0x08
#define DSPCR_ARINT   0x20
#define DSPCR_DSPINT  0x80

#define AUDIO_DMA_ENABLE 0x8000

u32 __OSBusClock : 0x800000F8;
#define OS_BUS_CLOCK __OSBusClock
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSNanosecondsToTicks(nsec) (((nsec) * (OS_TIMER_CLOCK / 125000)) / 8000)

const char *__AIVersion = "<< Dolphin SDK - AI\trelease build: Apr  5 2004 04:15:02 (0x2301) >>";

static AISCallback __AIS_Callback;
static AIDCallback __AID_Callback;
static u8 *__CallbackStack;
static u8 *__OldStack;
static volatile BOOL __AI_init_flag;
static volatile BOOL __AID_Active;

/* Timing windows for __AI_SRC_INIT, in time-base ticks. */
static OSTime bound_32KHz;
static OSTime bound_48KHz;
static OSTime min_wait;
static OSTime max_wait;
static OSTime buffer;

static void __AI_SRC_INIT(void);
static void __AICallbackStackSwitch(register AIDCallback callback);

u32 AIGetStreamPlayState(void);
u32 AIGetDSPSampleRate(void);
u32 AIGetStreamSampleRate(void);
u8 AIGetStreamVolLeft(void);
u8 AIGetStreamVolRight(void);
void AISetStreamVolLeft(u8 volume);
void AISetStreamVolRight(u8 volume);

AIDCallback AIRegisterDMACallback(AIDCallback callback) {
    AIDCallback previous = __AID_Callback;
    BOOL enabled = OSDisableInterrupts();
    __AID_Callback = callback;
    OSRestoreInterrupts(enabled);
    return previous;
}

/* Point the audio DMA at `start` (32-byte aligned) for `length` bytes. */
void AIInitDMA(u32 start, u32 length) {
    BOOL enabled = OSDisableInterrupts();
    __DSPRegs[24] = (u16)((__DSPRegs[24] & ~0x3FF) | (start >> 16));
    __DSPRegs[25] = (u16)((__DSPRegs[25] & ~0xFFE0) | (0xFFFF & start));
    __DSPRegs[27] = (u16)((__DSPRegs[27] & ~0x7FFF) | (u16)((length >> 5) & 0xFFFF));
    OSRestoreInterrupts(enabled);
}

void AIStartDMA(void) { __DSPRegs[27] |= AUDIO_DMA_ENABLE; }

void AIStopDMA(void) { __DSPRegs[27] &= ~AUDIO_DMA_ENABLE; }

/* Starting a stream from 32 kHz needs the converter resynchronised first; the volumes are muted
 * around it so the resync can't click. */
void AISetStreamPlayState(u32 state) {
    BOOL enabled;
    u8 right;
    u8 left;

    if (state == AIGetStreamPlayState()) {
        return;
    }
    if (AIGetStreamSampleRate() == 0 && state == 1) {
        right = AIGetStreamVolRight();
        left = AIGetStreamVolLeft();
        AISetStreamVolRight(0);
        AISetStreamVolLeft(0);
        enabled = OSDisableInterrupts();
        __AI_SRC_INIT();
        __AIRegs[0] = (__AIRegs[0] & ~AICR_SCRESET) | AICR_SCRESET;
        __AIRegs[0] = (__AIRegs[0] & ~AICR_PSTAT) | 1;
        OSRestoreInterrupts(enabled);
        AISetStreamVolLeft(right);
        AISetStreamVolRight(left);
    } else {
        __AIRegs[0] = (__AIRegs[0] & ~AICR_PSTAT) | state;
    }
}

u32 AIGetStreamPlayState(void) { return __AIRegs[0] & AICR_PSTAT; }

void AISetDSPSampleRate(u32 rate) {
    BOOL enabled;
    u32 playState;
    u32 streamRate;
    u8 left;
    u8 right;

    if (rate == AIGetDSPSampleRate()) {
        return;
    }
    __AIRegs[0] &= ~AICR_DSP32;
    if (rate == 0) {
        left = AIGetStreamVolLeft();
        right = AIGetStreamVolRight();
        playState = AIGetStreamPlayState();
        streamRate = AIGetStreamSampleRate();
        AISetStreamVolLeft(0);
        AISetStreamVolRight(0);
        enabled = OSDisableInterrupts();
        __AI_SRC_INIT();
        __AIRegs[0] = (__AIRegs[0] & ~AICR_SCRESET) | AICR_SCRESET;
        __AIRegs[0] = (__AIRegs[0] & ~AICR_AFR) | (streamRate << 1);
        __AIRegs[0] = (__AIRegs[0] & ~AICR_PSTAT) | playState;
        __AIRegs[0] |= AICR_DSP32;
        OSRestoreInterrupts(enabled);
        AISetStreamVolLeft(left);
        AISetStreamVolRight(right);
    }
}

u32 AIGetDSPSampleRate(void) { return ((__AIRegs[0] >> 6) & 1) ^ 1; }

void __AI_set_stream_sample_rate(u32 rate) {
    BOOL enabled;
    u32 playState;
    u8 left;
    u8 right;
    u32 dspRate;

    if (rate == AIGetStreamSampleRate()) {
        return;
    }
    playState = AIGetStreamPlayState();
    left = AIGetStreamVolLeft();
    right = AIGetStreamVolRight();
    AISetStreamVolRight(0);
    AISetStreamVolLeft(0);
    dspRate = __AIRegs[0] & AICR_DSP32;
    __AIRegs[0] &= ~AICR_DSP32;
    enabled = OSDisableInterrupts();
    __AI_SRC_INIT();
    __AIRegs[0] |= dspRate;
    __AIRegs[0] = (__AIRegs[0] & ~AICR_SCRESET) | AICR_SCRESET;
    __AIRegs[0] = (__AIRegs[0] & ~AICR_AFR) | (rate << 1);
    OSRestoreInterrupts(enabled);
    AISetStreamPlayState(playState);
    AISetStreamVolLeft(left);
    AISetStreamVolRight(right);
}

u32 AIGetStreamSampleRate(void) { return (__AIRegs[0] >> 1) & 1; }

void AISetStreamVolLeft(u8 volume) { __AIRegs[1] = (__AIRegs[1] & ~0xFF) | (volume & 0xFF); }

u8 AIGetStreamVolLeft(void) { return (u8)__AIRegs[1]; }

void AISetStreamVolRight(u8 volume) {
    __AIRegs[1] = (__AIRegs[1] & ~0xFF00) | ((volume & 0xFF) << 8);
}

u8 AIGetStreamVolRight(void) { return (u8)(__AIRegs[1] >> 8); }

void AIResetStreamSampleCount(void) {
    __AIRegs[0] = (__AIRegs[0] & ~AICR_SCRESET) | AICR_SCRESET;
}

static void __AISHandler(__OSInterrupt interrupt, OSContext *context);
static void __AIDHandler(__OSInterrupt interrupt, OSContext *context);

/* `stack`, when given, is where the DMA callback runs instead of the interrupted stack. */
void AIInit(u8 *stack) {
    if (__AI_init_flag == TRUE) {
        return;
    }
    OSRegisterVersion(__AIVersion);
    bound_32KHz = OSNanosecondsToTicks(31524);
    bound_48KHz = OSNanosecondsToTicks(42024);
    min_wait = OSNanosecondsToTicks(42000);
    max_wait = OSNanosecondsToTicks(63000);
    buffer = OSNanosecondsToTicks(3000);
    AISetStreamVolRight(0);
    AISetStreamVolLeft(0);
    AIResetStreamSampleCount();
    __AIRegs[3] = 0;
    __AI_set_stream_sample_rate(1);
    AISetDSPSampleRate(0);
    __AIS_Callback = NULL;
    __AID_Callback = NULL;
    __CallbackStack = stack;
    __OSSetInterruptHandler(5, __AIDHandler);
    __OSUnmaskInterrupts(0x04000000);
    __OSSetInterruptHandler(8, __AISHandler);
    __OSUnmaskInterrupts(0x00800000);
    __AI_init_flag = TRUE;
}

static void __AISHandler(__OSInterrupt interrupt, OSContext *context) {
    OSContext exceptionContext;

    __AIRegs[0] |= AICR_AISINTVLD;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (__AIS_Callback) {
        __AIS_Callback(__AIRegs[2]);
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

/* Acknowledge only the AI DMA interrupt (writing the ARAM and DSP bits back would clear them). */
static void __AIDHandler(__OSInterrupt interrupt, OSContext *context) {
    OSContext exceptionContext;
    u16 status = __DSPRegs[5];

    status = (u16)((status & ~(DSPCR_DSPINT | DSPCR_ARINT)) | DSPCR_AIDINT);
    __DSPRegs[5] = status;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (__AID_Callback && !__AID_Active) {
        __AID_Active = TRUE;
        if (__CallbackStack) {
            __AICallbackStackSwitch(__AID_Callback);
        } else {
            __AID_Callback();
        }
        __AID_Active = FALSE;
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

static asm void __AICallbackStackSwitch(register AIDCallback callback) {
    fralloc
    lis r5, __OldStack@ha
    addi r5, r5, __OldStack@l
    stw r1, 0(r5)
    lis r5, __CallbackStack@ha
    addi r5, r5, __CallbackStack@l
    lwz r1, 0(r5)
    subi r1, r1, 8
    mtlr callback
    blrl
    lis r5, __OldStack@ha
    addi r5, r5, __OldStack@l
    lwz r1, 0(r5)
    frfree
    blr
}

/* Time one 32 kHz and one 48 kHz stream sample; the gap between them says which phase the
 * converter is in, and so how long to wait before the new rate is safe to use. */
static void __AI_SRC_INIT(void) {
    OSTime rising32 = 0;
    OSTime rising48 = 0;
    OSTime diff = 0;
    OSTime unused = 0;
    OSTime wait = 0;
    u32 count = 0;
    u32 count2 = 0;
    u32 done = 0;
    u32 volume = 0;
    u32 tries = 0;
    u32 walking = 0;

    walking = 0;
    tries = 0;
    wait = 0;

    while (!done) {
        __AIRegs[0] = (__AIRegs[0] & ~AICR_SCRESET) | AICR_SCRESET;
        __AIRegs[0] &= ~AICR_AFR;
        __AIRegs[0] = (__AIRegs[0] & ~AICR_PSTAT) | 1;
        count = __AIRegs[2];
        while (count == __AIRegs[2]) {
        }
        rising32 = OSGetTime();
        __AIRegs[0] = (__AIRegs[0] & ~AICR_AFR) | AICR_AFR;
        __AIRegs[0] = (__AIRegs[0] & ~AICR_PSTAT) | 1;
        count2 = __AIRegs[2];
        while (count2 == __AIRegs[2]) {
        }
        rising48 = OSGetTime();
        diff = rising48 - rising32;
        __AIRegs[0] &= ~AICR_AFR;
        __AIRegs[0] &= ~AICR_PSTAT;
        if (diff < bound_32KHz - buffer) {
            wait = min_wait;
            done = 1;
            tries++;
        } else if (diff >= bound_32KHz + buffer && diff < bound_48KHz - buffer) {
            wait = max_wait;
            done = 1;
            tries++;
        } else {
            done = 0;
            walking = 1;
            tries++;
        }
    }
    while (rising48 + wait > OSGetTime()) {
    }
}

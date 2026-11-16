/* Dolphin SDK AX: the output side of the mixer.
 *
 * The DSP mixes one 5 ms frame (160 stereo samples) per audio DMA interrupt into one of the output
 * buffers while the AI plays another; AXOut runs the frame loop from the AI DMA callback. Each
 * frame it syncs the voice blocks, hands the DSP the finished command list by mailbox, runs the aux
 * effects and the user callback, and builds the next command list. In triple-buffered mode the DMA
 * is re-armed from the callback instead. The DSP program itself runs as a DSP-library task. */
#include <dolphin/os.h>
#include <dolphin/dsp.h>
#include <dolphin/ax.h>

typedef void (*AIDCallback)(void);
AIDCallback AIRegisterDMACallback(AIDCallback callback);
void AIInitDMA(u32 start, u32 length);
void AIStartDMA(void);
void AIStopDMA(void);

/* The OS thread-queue calls this unit uses (sleep/wakeup keep their placeholder names). */
void OSInitThreadQueue(OSThreadQueue *queue);
void fn_8037847C(OSThreadQueue *queue);
void fn_80378568(OSThreadQueue *queue);

u32 DSPCheckMailToDSP(void);
void DSPSendMailToDSP(u32 mail);
BOOL DSPCheckInit(void);
void DSPInit(void);
DSPTaskInfo *DSPAddTask(DSPTaskInfo *task);
void DSPCancelTask(DSPTaskInfo *task);
void DSPAssertTask(DSPTaskInfo *task);

void __AXSyncPBs(u32 lessDspCycles);
void __AXPrintStudio(void);
u32 __AXGetCommandListAddress(void);
void __AXServiceCallbackStack(void);
void __AXProcessAux(void);
void __AXNextFrame(void *surroundBuffer, void *outputBuffer);
u32 __AXGetNumVoices(void);

AXPROFILE *__AXGetCurrentProfile(void);

typedef void (*AXUserCallback)(void);

#define AX_OUTPUT_BUFFER_DOUBLE 0
#define AX_OUTPUT_BUFFER_TRIPLE 1

#define AX_FRAME_SAMPLES 160

extern u16 axDspSlave[];
extern u16 axDspSlaveLength;

static u32 __AXOutBuffer[3][AX_FRAME_SAMPLES];
static u32 __AXOutSBuffer[AX_FRAME_SAMPLES];
static u16 __AXDramImage[0x2000];
static DSPTaskInfo task;
static AXPROFILE __AXLocalProfile;

static u32 __AXOutFrame;
static u32 __AXAiDmaFrame;
static u32 __AXOutDspReady;
static volatile OSTime __AXOsTime;
static AXUserCallback __AXUserFrameCallback;
static volatile BOOL __AXDSPInitFlag;
static BOOL __AXDSPDoneFlag;
static BOOL __AXDebugSteppingMode;
static OSThreadQueue __AXOutThreadQueue;
static u32 __AXOutputBufferMode;

void __AXOutNewFrame(u32 lessDspCycles) {
    u32 commandList;
    AXPROFILE *profile;

    __AXLocalProfile.axFrameStart = OSGetTime();
    __AXSyncPBs(lessDspCycles);
    __AXPrintStudio();

    commandList = __AXGetCommandListAddress();
    DSPSendMailToDSP(0xBABE0180);
    while (DSPCheckMailToDSP()) {
    }
    DSPSendMailToDSP(commandList);
    while (DSPCheckMailToDSP()) {
    }

    __AXServiceCallbackStack();
    __AXLocalProfile.auxProcessingStart = OSGetTime();
    __AXProcessAux();
    __AXLocalProfile.auxProcessingEnd = OSGetTime();
    __AXLocalProfile.userCallbackStart = OSGetTime();
    if (__AXUserFrameCallback) {
        __AXUserFrameCallback();
    }
    __AXLocalProfile.userCallbackEnd = OSGetTime();

    __AXNextFrame(__AXOutSBuffer, __AXOutBuffer[__AXOutFrame]);
    __AXOutFrame++;
    if (__AXOutputBufferMode == AX_OUTPUT_BUFFER_TRIPLE) {
        __AXOutFrame %= 3;
    } else {
        __AXOutFrame &= 1;
        AIInitDMA((u32)__AXOutBuffer[__AXOutFrame], sizeof(__AXOutBuffer[0]));
    }

    __AXLocalProfile.axFrameEnd = OSGetTime();
    __AXLocalProfile.axNumVoices = __AXGetNumVoices();
    profile = __AXGetCurrentProfile();
    if (profile) {
        u8 *dst = (u8 *)profile;
        u8 *src = (u8 *)&__AXLocalProfile;
        u32 i = sizeof(AXPROFILE);

        while (i--) {
            *dst++ = *src++;
        }
    }
}

/* AI DMA finished: start the next frame now if the DSP is idle, otherwise ask it to yield. */
void __AXOutAiCallback(void) {
    if (__AXOutDspReady == 0) {
        __AXOsTime = OSGetTime();
    }
    if (__AXOutDspReady == 1) {
        __AXOutDspReady = 0;
        __AXOutNewFrame(0);
    } else {
        __AXOutDspReady = 2;
        DSPAssertTask(&task);
    }
    if (__AXOutputBufferMode == AX_OUTPUT_BUFFER_TRIPLE) {
        AIInitDMA((u32)__AXOutBuffer[__AXAiDmaFrame], sizeof(__AXOutBuffer[0]));
        __AXAiDmaFrame++;
        __AXAiDmaFrame %= 3;
    }
}

static void __AXDSPInitCallback(void *task) { __AXDSPInitFlag = TRUE; }

/* The DSP gave the CPU its turn back; the time it took is subtracted from the next frame. */
static void __AXDSPResumeCallback(void *task) {
    if (__AXOutDspReady == 2) {
        __AXOutDspReady = 0;
        __AXOutNewFrame((u32)(OSGetTime() - __AXOsTime) / 4);
    } else {
        __AXOutDspReady = 1;
    }
}

static void __AXDSPDoneCallback(void *task) {
    __AXDSPDoneFlag = TRUE;
    fn_80378568(&__AXOutThreadQueue);
}

void __AXOutInitDSP(void) {
    task.iram_mmem_addr = axDspSlave;
    task.iram_length = axDspSlaveLength;
    task.iram_addr = 0;
    task.dram_mmem_addr = __AXDramImage;
    task.dram_length = 0x2000;
    task.dram_addr = 0;
    task.dsp_init_vector = 0x10;
    task.dsp_resume_vector = 0x30;
    task.init_cb = __AXDSPInitCallback;
    task.res_cb = __AXDSPResumeCallback;
    task.done_cb = __AXDSPDoneCallback;
    task.req_cb = NULL;
    task.priority = 0;
    __AXDSPInitFlag = FALSE;
    __AXDSPDoneFlag = FALSE;
    OSInitThreadQueue(&__AXOutThreadQueue);
    if (!DSPCheckInit()) {
        DSPInit();
    }
    DSPAddTask(&task);
    while (__AXDSPInitFlag == FALSE) {
    }
}

void __AXOutInit(u32 outputBufferMode) {
    u32 i;
    u32 *buffer;

    __AXOutFrame = 0;
    __AXAiDmaFrame = 0;
    __AXOutputBufferMode = outputBufferMode;
    __AXDebugSteppingMode = FALSE;

    buffer = __AXOutBuffer[0];
    for (i = 0; i < 3 * AX_FRAME_SAMPLES; i++) {
        *buffer = 0;
        buffer++;
    }
    DCFlushRange(__AXOutBuffer, sizeof(__AXOutBuffer));

    buffer = __AXOutSBuffer;
    for (i = 0; i < AX_FRAME_SAMPLES; i++) {
        *buffer = 0;
        buffer++;
    }
    DCFlushRange(__AXOutSBuffer, sizeof(__AXOutSBuffer));

    __AXOutInitDSP();
    AIRegisterDMACallback(__AXOutAiCallback);
    if (__AXOutputBufferMode == AX_OUTPUT_BUFFER_TRIPLE) {
        __AXNextFrame(__AXOutSBuffer, __AXOutBuffer[2]);
    } else {
        __AXNextFrame(__AXOutSBuffer, __AXOutBuffer[1]);
    }
    __AXOutDspReady = 1;
    __AXUserFrameCallback = NULL;

    if (__AXOutputBufferMode == AX_OUTPUT_BUFFER_TRIPLE) {
        AIInitDMA((u32)__AXOutBuffer[__AXAiDmaFrame], sizeof(__AXOutBuffer[0]));
        __AXAiDmaFrame++;
        __AXAiDmaFrame &= 1;
    } else {
        AIInitDMA((u32)__AXOutBuffer[__AXOutFrame], sizeof(__AXOutBuffer[0]));
    }
    AIStartDMA();
}

void __AXOutQuit(void) {
    BOOL enabled = OSDisableInterrupts();

    __AXUserFrameCallback = NULL;
    DSPCancelTask(&task);
    fn_8037847C(&__AXOutThreadQueue);
    AIStopDMA();
    OSRestoreInterrupts(enabled);
}

AXUserCallback AXRegisterCallback(AXUserCallback callback) {
    AXUserCallback previous = __AXUserFrameCallback;
    BOOL enabled = OSDisableInterrupts();

    __AXUserFrameCallback = callback;
    OSRestoreInterrupts(enabled);
    return previous;
}

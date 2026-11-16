/* Dolphin SDK AX: the two auxiliary sends (aux A and aux B).
 *
 * Each send has three frame buffers in rotation: the DSP writes one, the CPU-side effect callback
 * works on the next, and the DSP reads the result back from the third. A frame is 480 samples:
 * left, right and surround, 160 each. In Dolby Pro Logic II mode aux A carries four channels and
 * borrows aux B's surround slot for the fourth; aux B is then unavailable. The small getters hand
 * the command-list builder the buffer addresses for the current frame. */
#include <dolphin/os.h>

/* The RenderWare units call these two by their placeholder symbols. */
#define AXRegisterAuxACallback fn_80384244
#define AXRegisterAuxBCallback fn_80384250

typedef void (*AXAuxCallback)(void *data, void *context);

#define AX_MODE_DPL2 2
#define AUX_FRAME 480
#define AUX_CHANNEL 160

extern u32 __AXClMode;

static s32 __AXBufferAuxA[3][AUX_FRAME];
static s32 __AXBufferAuxB[3][AUX_FRAME];

static AXAuxCallback __AXCallbackAuxA;
static AXAuxCallback __AXCallbackAuxB;
static void *__AXContextAuxA;
static void *__AXContextAuxB;
static s32 *__AXAuxADspWrite;
static s32 *__AXAuxADspRead;
static s32 *__AXAuxBDspWrite;
static s32 *__AXAuxBDspRead;
static u32 __AXAuxDspWritePosition;
static u32 __AXAuxDspReadPosition;
static u32 __AXAuxDspWritePositionDpl2;
static u32 __AXAuxDspReadPositionDpl2;
static u32 __AXAuxCpuReadWritePosition;

void __AXAuxInit(void) {
    u32 i;
    s32 *bufferA;
    s32 *bufferB;

    __AXCallbackAuxA = NULL;
    __AXCallbackAuxB = NULL;
    __AXContextAuxA = NULL;
    __AXContextAuxB = NULL;
    __AXAuxDspWritePosition = 0;
    __AXAuxDspReadPosition = 1;
    __AXAuxDspWritePositionDpl2 = 0;
    __AXAuxDspReadPositionDpl2 = 1;
    __AXAuxCpuReadWritePosition = 2;

    bufferA = __AXBufferAuxA[0];
    bufferB = __AXBufferAuxB[0];
    for (i = 0; i < AUX_FRAME; i++) {
        *bufferA = 0;
        *bufferB = 0;
        bufferA++;
        bufferB++;
    }
}

void __AXAuxQuit(void) {
    __AXCallbackAuxA = NULL;
    __AXCallbackAuxB = NULL;
}

void fn_80383EB0(u32 *addr) {
    if (__AXCallbackAuxA) {
        *addr = (u32)__AXBufferAuxA[__AXAuxDspWritePosition];
    } else {
        *addr = 0;
    }
}

void fn_80383EE4(u32 *addr) {
    *addr = (u32)&__AXBufferAuxB[__AXAuxDspWritePosition][2 * AUX_CHANNEL];
}

void fn_80383F04(u32 *addr) { *addr = (u32)__AXBufferAuxA[__AXAuxDspReadPosition]; }

void fn_80383F20(u32 *addr) {
    *addr = (u32)&__AXBufferAuxA[__AXAuxDspReadPosition][AUX_CHANNEL];
}

void fn_80383F40(u32 *addr) {
    *addr = (u32)&__AXBufferAuxA[__AXAuxDspReadPosition][2 * AUX_CHANNEL];
}

void fn_80383F60(u32 *addr) {
    *addr = (u32)&__AXBufferAuxB[__AXAuxDspReadPosition][2 * AUX_CHANNEL];
}

void fn_80383F80(u32 *addr) {
    if (__AXCallbackAuxB) {
        *addr = (u32)__AXBufferAuxB[__AXAuxDspWritePosition];
    } else {
        *addr = 0;
    }
}

void fn_80383FB4(u32 *addr) { *addr = (u32)__AXBufferAuxB[__AXAuxDspReadPosition]; }

void fn_80383FD0(u32 *addr) { *addr = (u32)__AXBufferAuxB[__AXAuxDspWritePositionDpl2]; }

void fn_80383FEC(u32 *addr) { *addr = (u32)__AXBufferAuxB[__AXAuxDspReadPositionDpl2]; }

/* Runs the effect callbacks on the CPU's frame, then advances all buffer rotations. */
void __AXProcessAux(void) {
    s32 *dpl2[4];
    s32 *data[3];
    s32 *dataB[3];

    __AXAuxADspWrite = __AXBufferAuxA[__AXAuxDspWritePosition];
    __AXAuxADspRead = __AXBufferAuxA[__AXAuxDspReadPosition];
    __AXAuxBDspWrite = __AXBufferAuxB[__AXAuxDspWritePosition];
    __AXAuxBDspRead = __AXBufferAuxB[__AXAuxDspReadPosition];

    if (__AXCallbackAuxA) {
        if (__AXClMode == AX_MODE_DPL2) {
            dpl2[0] = __AXBufferAuxA[__AXAuxCpuReadWritePosition];
            dpl2[1] = &__AXBufferAuxA[__AXAuxCpuReadWritePosition][AUX_CHANNEL];
            dpl2[2] = &__AXBufferAuxA[__AXAuxCpuReadWritePosition][2 * AUX_CHANNEL];
            dpl2[3] = &__AXBufferAuxB[__AXAuxCpuReadWritePosition][2 * AUX_CHANNEL];
            DCInvalidateRange(dpl2[0], sizeof(s32) * AUX_FRAME);
            DCInvalidateRange(dpl2[3], sizeof(s32) * AUX_CHANNEL);
            __AXCallbackAuxA(dpl2, __AXContextAuxA);
            DCFlushRangeNoSync(dpl2[0], sizeof(s32) * AUX_FRAME);
            DCFlushRangeNoSync(dpl2[3], sizeof(s32) * AUX_CHANNEL);
        } else {
            data[0] = __AXBufferAuxA[__AXAuxCpuReadWritePosition];
            data[1] = &__AXBufferAuxA[__AXAuxCpuReadWritePosition][AUX_CHANNEL];
            data[2] = &__AXBufferAuxA[__AXAuxCpuReadWritePosition][2 * AUX_CHANNEL];
            DCInvalidateRange(data[0], sizeof(s32) * AUX_FRAME);
            __AXCallbackAuxA(data, __AXContextAuxA);
            DCFlushRangeNoSync(data[0], sizeof(s32) * AUX_FRAME);
        }
    }

    if (__AXCallbackAuxB && __AXClMode != AX_MODE_DPL2) {
        dataB[0] = __AXBufferAuxB[__AXAuxCpuReadWritePosition];
        dataB[1] = &__AXBufferAuxB[__AXAuxCpuReadWritePosition][AUX_CHANNEL];
        dataB[2] = &__AXBufferAuxB[__AXAuxCpuReadWritePosition][2 * AUX_CHANNEL];
        DCInvalidateRange(dataB[0], sizeof(s32) * AUX_FRAME);
        __AXCallbackAuxB(dataB, __AXContextAuxB);
        DCFlushRangeNoSync(dataB[0], sizeof(s32) * AUX_FRAME);
    }

    __AXAuxDspWritePosition++;
    __AXAuxDspWritePosition %= 3;
    __AXAuxDspReadPosition++;
    __AXAuxDspReadPosition %= 3;
    __AXAuxDspWritePositionDpl2++;
    __AXAuxDspWritePositionDpl2 &= 1;
    __AXAuxDspReadPositionDpl2++;
    __AXAuxDspReadPositionDpl2 &= 1;
    __AXAuxCpuReadWritePosition++;
    __AXAuxCpuReadWritePosition %= 3;
}

void AXRegisterAuxACallback(AXAuxCallback callback, void *context) {
    __AXCallbackAuxA = callback;
    __AXContextAuxA = context;
}

void AXRegisterAuxBCallback(AXAuxCallback callback, void *context) {
    __AXCallbackAuxB = callback;
    __AXContextAuxB = context;
}

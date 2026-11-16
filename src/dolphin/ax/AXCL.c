/* Dolphin SDK AX: the per-frame DSP command list.
 *
 * Every audio frame the CPU builds a short list of 16-bit commands for the DSP microcode: where the
 * studio parameters, voice parameter blocks and aux buffers are, what to mix, and where the final
 * mix goes. The list is double buffered; __AXGetCommandListAddress hands the finished one to the
 * DSP and switches writing to the other. __AXCommandListCycles estimates the DSP time the frame
 * will take. */
#include <dolphin/os.h>

/* The RenderWare units call this one by its placeholder symbol. */
#define AXSetMode fn_80384978

#define AX_MODE_STEREO 0
#define AX_MODE_SURROUND 1
#define AX_MODE_DPL2 2

#define AX_CL_SIZE 0x180 /* commands per list */

u32 __AXGetStudio(void);
u32 __AXGetPBs(void);
void fn_80383EB0(u32 *addr);
void fn_80383EE4(u32 *addr);
void fn_80383F04(u32 *addr);
void fn_80383F20(u32 *addr);
void fn_80383F40(u32 *addr);
void fn_80383F60(u32 *addr);
void fn_80383F80(u32 *addr);
void fn_80383FB4(u32 *addr);
void fn_80383FD0(u32 *addr);
void fn_80383FEC(u32 *addr);

extern u16 __AXCompressorTable[];

static u16 __AXCommandList[2][AX_CL_SIZE];

static u32 __AXCommandListPosition;
static u16 *__AXClWrite;
static u32 __AXCommandListCycles;
static u32 __AXCompressor;
u32 __AXClMode;

#define __AXWriteToCommandList(data)                                                                   *__AXClWrite = (u16)(data);                                                                        __AXClWrite++

u32 __AXGetCommandListCycles(void) { return __AXCommandListCycles; }

u32 __AXGetCommandListAddress(void) {
    u32 address = (u32)__AXCommandList[__AXCommandListPosition];

    __AXCommandListPosition++;
    __AXCommandListPosition &= 1;
    __AXClWrite = __AXCommandList[__AXCommandListPosition];
    return address;
}

/* `surroundBuffer` receives the surround channel, `outputBuffer` the final stereo mix. */
void __AXNextFrame(void *surroundBuffer, void *outputBuffer) {
    u32 data;
    u32 unused[14]; /* never touched; retail's frame has room for it */
    u16 *list = __AXClWrite;

    __AXCommandListCycles = 0x1A9;

    data = __AXGetStudio();
    __AXWriteToCommandList(0x00);
    __AXWriteToCommandList(data >> 16);
    __AXWriteToCommandList(data);
    __AXCommandListCycles += 0x2E44;

    switch (__AXClMode) {
    case AX_MODE_STEREO:
        __AXWriteToCommandList(0x07);
        __AXWriteToCommandList((u32)surroundBuffer >> 16);
        __AXWriteToCommandList(surroundBuffer);
        __AXCommandListCycles += 0x546;
        break;
    case AX_MODE_SURROUND:
        __AXWriteToCommandList(0x11);
        __AXWriteToCommandList((u32)surroundBuffer >> 16);
        __AXWriteToCommandList(surroundBuffer);
        __AXCommandListCycles += 0x5E6;
        break;
    case AX_MODE_DPL2:
        break;
    }

    data = __AXGetPBs();
    __AXWriteToCommandList(0x02);
    __AXWriteToCommandList(data >> 16);
    __AXWriteToCommandList(data);
    __AXWriteToCommandList(0x03);

    if (__AXClMode == AX_MODE_DPL2) {
        fn_80383EB0(&data);
        if (data) {
            __AXWriteToCommandList(0x13);
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
            fn_80383EE4(&data);
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
            fn_80383F04(&data);
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
            fn_80383F20(&data);
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
            fn_80383F40(&data);
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
            fn_80383F60(&data);
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
            __AXCommandListCycles += 0xDED;
        }
        __AXWriteToCommandList(0x10);
        fn_80383FD0(&data);
        __AXWriteToCommandList(data >> 16);
        __AXWriteToCommandList(data);
        fn_80383FEC(&data);
        __AXWriteToCommandList(data >> 16);
        __AXWriteToCommandList(data);
        __AXCommandListCycles += 0xDED;
    } else {
        fn_80383EB0(&data);
        if (data) {
            __AXWriteToCommandList(0x04);
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
            fn_80383F04(&data);
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
            __AXCommandListCycles += 0xDED;
        }
        fn_80383F80(&data);
        if (data) {
            __AXWriteToCommandList(0x05);
            __AXCommandListCycles += 0xDED;
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
            fn_80383FB4(&data);
            __AXWriteToCommandList(data >> 16);
            __AXWriteToCommandList(data);
        }
    }

    if (__AXCompressor) {
        __AXWriteToCommandList(0x12);
        __AXWriteToCommandList(0x8000);
        __AXWriteToCommandList(0x0A);
        __AXWriteToCommandList((u32)__AXCompressorTable >> 16);
        __AXWriteToCommandList(__AXCompressorTable);
        __AXCommandListCycles += 0xBB8;
    }

    __AXWriteToCommandList(0x0E);
    __AXWriteToCommandList((u32)surroundBuffer >> 16);
    __AXWriteToCommandList(surroundBuffer);
    __AXWriteToCommandList((u32)outputBuffer >> 16);
    __AXWriteToCommandList(outputBuffer);
    __AXCommandListCycles += 0x2710;
    __AXWriteToCommandList(0x0F);
    __AXCommandListCycles += 2;

    DCFlushRange(list, sizeof(__AXCommandList[0]));
}

void __AXClInit(void) {
    __AXClMode = AX_MODE_STEREO;
    __AXCommandListPosition = 0;
    __AXClWrite = __AXCommandList[0];
    __AXCompressor = TRUE;
}

void __AXClQuit(void) {}

void AXSetMode(u32 mode) {
    if (__AXClMode != mode) {
        __AXClMode = mode;
    }
}

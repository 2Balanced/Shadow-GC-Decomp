/* Dolphin SDK AX: library start-up and shutdown. AXInitEx brings up each part of the mixer in
 * turn: voice allocation, the voice parameter blocks, the studio block, aux effects, the command
 * list, and finally the output path that starts the DSP. */
#include <dolphin/os_interrupt.h>

void __AXAllocInit(void);
void __AXVPBInit(void);
void __AXSPBInit(void);
void __AXAuxInit(void);
void __AXClInit(void);
void __AXOutInit(u32 outputBufferMode);
void __AXAllocQuit(void);
void __AXVPBQuit(void);
void __AXSPBQuit(void);
void __AXAuxQuit(void);
void __AXClQuit(void);
void __AXOutQuit(void);

const char *__AXVersion = "<< Dolphin SDK - AX\trelease build: Apr  5 2004 04:15:05 (0x2301) >>";

void AXInitEx(u32 outputBufferMode);

void AXInit(void) { AXInitEx(0); }

void AXInitEx(u32 outputBufferMode) {
    OSRegisterVersion(__AXVersion);
    __AXAllocInit();
    __AXVPBInit();
    __AXSPBInit();
    __AXAuxInit();
    __AXClInit();
    __AXOutInit(outputBufferMode);
}

void AXQuit(void) {
    __AXAllocQuit();
    __AXVPBQuit();
    __AXSPBQuit();
    __AXAuxQuit();
    __AXClQuit();
    __AXOutQuit();
}

#ifndef DOLPHIN_OS_INTERRUPT_H
#define DOLPHIN_OS_INTERRUPT_H

/* The OS library's interrupt-dispatch interface as the device libraries (AI, AR, DSP, EXI, SI,
 * CARD, ...) see it: installing a handler for one interrupt number and opening its mask bit.
 * OSInterrupt.c keeps its own copy of these types. */

#include <dolphin/os.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef s16 __OSInterrupt;
typedef u32 OSInterruptMask;
typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, OSContext *context);

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler);
__OSInterruptHandler __OSGetInterruptHandler(__OSInterrupt interrupt);
OSInterruptMask __OSMaskInterrupts(OSInterruptMask mask);
OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask mask);

/* Each library logs its build banner once from its Init function. */
void OSRegisterVersion(const char *id);

#ifdef __cplusplus
}
#endif

#endif

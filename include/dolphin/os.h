#ifndef DOLPHIN_OS_H
#define DOLPHIN_OS_H

/* The parts of the Dolphin SDK OS library's interface the decompiled SDK units use. */

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Main RAM is mapped cached at 0x80000000 and uncached at 0xC0000000. */
#define OSPhysicalToCached(paddr) ((void *)((u32)(paddr) + 0x80000000))
#define OSCachedToPhysical(caddr) ((u32)(caddr) - 0x80000000)

typedef s64 OSTime;
typedef u32 OSTick;

/* Low-memory adjustment added to the time base to give the system time. */
#define __OSSystemTime ((OSTime *)OSPhysicalToCached(0x30D8))

/* The register state saved on an exception or thread switch. */
typedef struct OSContext {
    u32 gpr[32];
    u32 cr;
    u32 lr;
    u32 ctr;
    u32 xer;
    f64 fpr[32];
    u32 fpscr_pad;
    u32 fpscr;
    u32 srr0;
    u32 srr1;
    u16 mode;
    u16 state;
    u32 gqr[8];
    u32 psf_pad;
    f64 psf[32];
} OSContext;

typedef u16 OSError;
typedef void (*OSErrorHandler)(OSError error, OSContext *context, ...);

#define OS_ERROR_MACHINE_CHECK 1

void OSReport(const char *msg, ...);
void OSDumpContext(OSContext *context);
OSErrorHandler OSSetErrorHandler(OSError error, OSErrorHandler handler);

BOOL OSDisableInterrupts(void);
BOOL OSEnableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL level);

OSTime OSGetTime(void);
OSTick OSGetTick(void);

void DCEnable(void);
void DCInvalidateRange(void *addr, u32 nBytes);
void DCFlushRange(void *addr, u32 nBytes);
void DCStoreRange(void *addr, u32 nBytes);
void DCFlushRangeNoSync(void *addr, u32 nBytes);
void ICInvalidateRange(void *addr, u32 nBytes);
void ICFlashInvalidate(void);
void ICEnable(void);
void LCDisable(void);
void L2GlobalInvalidate(void);

void DBPrintf(char *str, ...);

#ifdef __cplusplus
}
#endif

#endif

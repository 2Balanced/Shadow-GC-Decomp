#ifndef DOLPHIN_DVD_PRIVATE_H
#define DOLPHIN_DVD_PRIVATE_H

/* Declarations shared by the DVD library's own units only. */

#include <dolphin/dvd.h>

/* Bus clock, written to low memory by the boot ROM; the time base runs at a quarter of it. */
u32 __OSBusClock : 0x800000F8;
#define DVD_BUS_CLOCK __OSBusClock
#define DVD_TIMER_CLOCK (DVD_BUS_CLOCK / 4)
#define DVDSecondsToTicks(s) ((s) * DVD_TIMER_CLOCK)
#define DVDMillisecondsToTicks(ms) ((ms) * (DVD_TIMER_CLOCK / 1000))
#define DVDMicrosecondsToTicks(us) (((us) * (DVD_TIMER_CLOCK / 125000)) / 8)

/* Drive interface registers (DISR, DICVR, three command words, DMA address and length, DICR,
 * immediate buffer) and the processor interface's reset register. */
volatile u32 __DIRegs[] : 0xCC006000;
volatile u32 __PIRegs[] : 0xCC003000;

typedef s16 __OSInterrupt;

extern u32 __DVDLongFileNameFlag;
extern OSThreadQueue __DVDThreadQueue;

/* OS and MSL functions dtk has not named yet; the macros give them their public names. */
void fn_80372404(const char *file, int line, const char *msg, ...);
#define OSPanic fn_80372404
void fn_8037847C(OSThreadQueue *queue);
#define OSSleepThread fn_8037847C
void fn_80378568(OSThreadQueue *queue);
#define OSWakeupThread fn_80378568
int fn_803A8024(int c);
#define tolower fn_803A8024

u32 __OSMaskInterrupts(u32 mask);
u32 __OSUnmaskInterrupts(u32 mask);

#endif

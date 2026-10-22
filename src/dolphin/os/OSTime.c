/* Dolphin SDK OS: clock reads. OSTime is the full 64-bit PowerPC time base; "system time" is that
 * value plus a correction the OS keeps in low memory (__OSSystemTime, 0x800030D8). Calendar
 * conversion (OSTicksToCalendarTime and its month tables) and __OSSetTime were never referenced by
 * the game, so the linker dropped them. */
#include <dolphin/os.h>

/* The two halves of the time base cannot be read at once. Sample upper, lower, upper; if the two
 * upper samples disagree the lower word wrapped mid-read, so try again. */
asm OSTime OSGetTime(void) {
retry:
    nofralloc
    mftbu r3
    mftb r4
    mftbu r5
    cmpw r3, r5
    bne retry
    blr
}

/* Lower 32 bits of the time base only; enough for short intervals. */
asm OSTick OSGetTick(void) {
    nofralloc
    mftb r3
    blr
}

/* Time base plus the stored correction. Interrupts stay off across the read so a handler cannot
 * change the correction between the two loads. */
OSTime __OSGetSystemTime(void) {
    OSTime *correction = __OSSystemTime;
    BOOL wasEnabled = OSDisableInterrupts();
    OSTime now = OSGetTime() + *correction;

    OSRestoreInterrupts(wasEnabled);
    return now;
}

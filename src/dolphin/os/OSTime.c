/* Dolphin SDK OS: the time base. OSTime is the 64-bit time-base register; the system time adds
 * the adjustment kept in low memory at 0x800030D8. The calendar conversions (OSTicksToCalendarTime
 * and its month tables) and __OSSetTime are unused by the game and were dead-stripped. */
#include <dolphin/os.h>

/* Reads TBU, TBL, TBU again and retries if TBL carried into TBU in between. */
asm OSTime OSGetTime(void) {
jump:
    nofralloc
    mftbu r3
    mftb r4
    mftbu r5
    cmpw r3, r5
    bne jump
    blr
}

asm OSTick OSGetTick(void) {
    nofralloc
    mftb r3
    blr
}

OSTime __OSGetSystemTime(void) {
    BOOL enabled;
    OSTime *timeAdjustAddr;
    OSTime result;

    timeAdjustAddr = __OSSystemTime;
    enabled = OSDisableInterrupts();
    result = OSGetTime() + *timeAdjustAddr;
    OSRestoreInterrupts(enabled);
    return result;
}

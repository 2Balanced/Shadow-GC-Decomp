/* Dolphin SDK OS: the reset switch.
 *
 * Pressing the switch raises the PI reset-switch interrupt; the handler waits up to 100 us for
 * the line to bounce back before believing it, then masks the interrupt and runs the
 * application's reset callback once. Polling code reads the state through the button-state
 * query (still fn_8037649C in the binary; the SDK's OSGetResetButtonState), which debounces the
 * release by 40 ms. A non-zero "game choice" in low memory (0x800030E3, set by the boot menu of a
 * demo disc) makes the button appear pressed every other second once that many minutes have
 * passed since boot, which ends the demo. */
#include <dolphin/os.h>

volatile u32 __PIRegs[] : 0xCC003000;

#define PI_INTSR_RSWST 0x00010000 /* reset switch state: 0 while pressed */
#define PI_INTSR_RSWI 0x00000002  /* reset switch interrupt (write 1 to clear) */
#define OS_INTERRUPTMASK_PI_RSW 0x00000200

u32 __OSBusClock : 0x800000F8;
#define OS_BUS_CLOCK __OSBusClock
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)
#define OSSecondsToTicks(sec) ((sec) * OS_TIMER_CLOCK)
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))
#define OSMicrosecondsToTicks(usec) (((usec) * (OS_TIMER_CLOCK / 125000)) / 8)
#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)

u8 __OSGameChoice : 0x800030E3;
#define GAME_CHOICE __OSGameChoice

typedef void (*OSResetCallback)(void);

extern OSTime __OSStartTime;

u32 __OSMaskInterrupts(u32 mask);

static OSResetCallback ResetCallback;
static BOOL Down;
static BOOL LastState;
static OSTime HoldUp;
static OSTime HoldDown;

void __OSResetSWInterruptHandler(s16 interrupt, OSContext *context) {
    OSResetCallback callback;

    HoldDown = __OSGetSystemTime();
    while (__OSGetSystemTime() - HoldDown < OSMicrosecondsToTicks(100) &&
           !(__PIRegs[0] & PI_INTSR_RSWST)) {
    }

    if (!(__PIRegs[0] & PI_INTSR_RSWST)) {
        LastState = Down = TRUE;
        __OSMaskInterrupts(OS_INTERRUPTMASK_PI_RSW);
        if (ResetCallback != NULL) {
            callback = ResetCallback;
            ResetCallback = NULL;
            callback();
        }
    }
    __PIRegs[0] = PI_INTSR_RSWI;
}

BOOL fn_8037649C(void) {
    BOOL enabled;
    BOOL state;
    u32 reg;
    OSTime now;

    enabled = OSDisableInterrupts();
    now = __OSGetSystemTime();

    reg = __PIRegs[0];
    if (!(reg & PI_INTSR_RSWST)) {
        if (!Down) {
            Down = TRUE;
            state = HoldUp ? TRUE : FALSE;
            HoldDown = now;
        } else {
            state = (HoldUp || OSMicrosecondsToTicks(100) < now - HoldDown) ? TRUE : FALSE;
        }
    } else if (Down) {
        Down = FALSE;
        state = LastState;
        if (state) {
            HoldUp = now;
        } else {
            HoldUp = 0;
        }
    } else if (HoldUp && now - HoldUp < OSMillisecondsToTicks(40)) {
        state = TRUE;
    } else {
        state = FALSE;
        HoldUp = 0;
    }
    LastState = state;

    if (GAME_CHOICE & 0x1F) {
        OSTime fire = (GAME_CHOICE & 0x1F) * 60;

        fire = __OSStartTime + OSSecondsToTicks(fire);
        if (fire < now) {
            now -= fire;
            now = OSTicksToSeconds(now) / 2;
            if ((now & 1) == 0) {
                state = TRUE;
            } else {
                state = FALSE;
            }
        }
    }

    OSRestoreInterrupts(enabled);
    return state;
}

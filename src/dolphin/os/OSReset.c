/* Dolphin SDK OS: resetting the console.
 *
 * Subsystems register reset functions, kept in priority order. OSResetSystem calls them all
 * with final = FALSE until every one agrees (a function returns FALSE while it still has work in
 * flight), then once more with final = TRUE, stops the remaining threads and either hot-resets
 * through the PI reset register, reboots the disc's main program, or simply returns (shutdown).
 * The hot reset code runs from instruction-cache lines loaded by branching through it first, so
 * it keeps running while main memory is reset underneath it. */
#include <dolphin/os.h>

void *memset(void *dst, int value, unsigned long n);

volatile u32 __PIRegs[] : 0xCC003000;
volatile u16 __VIRegs[] : 0xCC002000;

#define OS_RESET_RESTART 0
#define OS_RESET_HOTRESET 1
#define OS_RESET_SHUTDOWN 2

#define OS_RESETCODE_RESTART 0x80000000
#define OS_RESETCODE_SYSTEM_MENU 0xC0010000 /* restart into the IPL's menu */

#define PI_RESET_CODE 9

typedef struct OSResetQueue {
    OSResetFunctionInfo *head;
    OSResetFunctionInfo *tail;
} OSResetQueue;

typedef struct OSExecParams {
    BOOL valid;
    u32 restartCode;
    u32 bootDol;
    void *regionStart;
    void *regionEnd;
    BOOL argsUseDefault;
    void *argsAddr;
} OSExecParams;

/* Only the byte this file touches: bit 6 sends the next boot to the IPL menu. */
typedef struct OSSram {
    u8 pad[0x13];
    u8 flags;
} OSSram;

extern OSExecParams __OSRebootParams;

OSSram *__OSLockSram(void);
BOOL __OSUnlockSram(BOOL commit);
BOOL __OSSyncSram(void);
void __OSStopAudioSystem(void);
BOOL __PADDisableRecalibration(BOOL disable);
void fn_80377EC8(OSThread *thread);
void __OSReboot(u32 resetCode, u32 bootDol);

static OSResetQueue ResetFunctionQueue;
static u32 bootThisDol;

void OSRegisterResetFunction(OSResetFunctionInfo *func) {
    OSResetFunctionInfo *tmp;
    OSResetFunctionInfo *iter;

    for (iter = ResetFunctionQueue.head; iter != NULL && iter->priority <= func->priority;
         iter = iter->next) {
    }

    if (iter == NULL) {
        tmp = ResetFunctionQueue.tail;
        if (tmp == NULL) {
            ResetFunctionQueue.head = func;
        } else {
            tmp->next = func;
        }
        func->prev = tmp;
        func->next = NULL;
        ResetFunctionQueue.tail = func;
        return;
    }

    func->next = iter;
    tmp = iter->prev;
    iter->prev = func;
    func->prev = tmp;
    if (tmp == NULL) {
        ResetFunctionQueue.head = func;
        return;
    }
    tmp->next = func;
}

/* Calls the reset functions in priority order. Once one has refused, the rest of its priority
 * group still runs but lower priorities wait for the next round. */
BOOL __OSCallResetFunctions(BOOL final) {
    OSResetFunctionInfo *info;
    s32 err = 0;
    u32 priority = 0;

    for (info = ResetFunctionQueue.head; info != NULL; info = info->next) {
        if (err && priority != info->priority) {
            break;
        }
        err |= !info->func(final);
        priority = info->priority;
    }
    err |= !__OSSyncSram();
    if (err) {
        return FALSE;
    }
    return TRUE;
}

/* Writes the reset code to the PI reset register. The first pass only branches through the code
 * so every line is in the instruction cache; the second pass runs it, first stalling about
 * 4400 time-base ticks with the bus flushed. */
static asm void Reset(register s32 resetCode) {
    nofralloc
    b @load0
@run0:
    mfspr r8, HID0
    ori r8, r8, 0x0008
    mtspr HID0, r8
    isync
    sync
    nop
    b @run1
@load0:
    b @load1
@run1:
    mftb r5, 268
@wait:
    mftb r6, 268
    subf r7, r5, r6
    cmplwi r7, 0x1124
    blt @wait
    nop
    b @run2
@load1:
    b @load2
@run2:
    lis r8, 0xCC00
    ori r8, r8, 0x3000
    li r4, 3
    stw r4, 0x24(r8)
    stw resetCode, 0x24(r8)
    nop
    b @spin
@load2:
    b @load3
@spin:
    nop
    b @spin
@load3:
    b @run0
}

/* Cancels every thread that is still ready or waiting. */
static void KillThreads(void) {
    OSThread *thread;
    OSThread *next;

    for (thread = __OSActiveThreadQueue.head; thread != NULL; thread = next) {
        next = thread->linkActive.next;
        switch (thread->state) {
        case OS_THREAD_STATE_READY:
        case OS_THREAD_STATE_WAITING:
            fn_80377EC8(thread);
            break;
        default:
            break;
        }
    }
}

void __OSDoHotReset(u32 resetCode) {
    OSDisableInterrupts();
    __VIRegs[1] = 0;
    ICFlashInvalidate();
    Reset(resetCode << 3);
}

/* Built without automatic inlining: the hot-reset sequence is written out in place. */
#pragma dont_inline on
void OSResetSystem(int reset, u32 resetCode, BOOL forceMenu) {
    BOOL disableRecalibration;
    BOOL unused1; /* reserved stack the original never touches */
    BOOL unused2;
    OSSram *sram;

    OSDisableScheduler();

    if (reset == OS_RESET_HOTRESET && forceMenu) {
        sram = __OSLockSram();
        sram->flags |= 0x40;
        __OSUnlockSram(TRUE);
        resetCode = 0;
    }

    if (reset == OS_RESET_SHUTDOWN ||
        (reset == OS_RESET_RESTART &&
         (bootThisDol != 0 || resetCode == OS_RESETCODE_SYSTEM_MENU))) {
        __OSStopAudioSystem();
        disableRecalibration = __PADDisableRecalibration(TRUE);
        while (!__OSCallResetFunctions(FALSE)) {
        }
        while (!__OSSyncSram()) {
        }
        OSDisableInterrupts();
        __OSCallResetFunctions(TRUE);
        LCDisable();
        __PADDisableRecalibration(disableRecalibration);
        KillThreads();
    } else {
        __OSStopAudioSystem();
        while (!__OSCallResetFunctions(FALSE)) {
        }
        while (!__OSSyncSram()) {
        }
        OSDisableInterrupts();
        __OSCallResetFunctions(TRUE);
        LCDisable();
        KillThreads();
    }

    if (reset == OS_RESET_HOTRESET) {
        OSDisableInterrupts();
        __VIRegs[1] = 0;
        ICFlashInvalidate();
        Reset(resetCode << 3);
    } else if (reset == OS_RESET_RESTART) {
        if (forceMenu == TRUE) {
            OSReport("OSResetSystem(): You can't specify TRUE to forceMenu if you restart. "
                     "Ignored\n");
        }
        OSEnableScheduler();
        __OSReboot(resetCode, bootThisDol);
    }

    /* Leave low memory as a cold boot would. */
    memset(OSPhysicalToCached(0x40), 0, 0xCC - 0x40);
    memset(OSPhysicalToCached(0xD4), 0, 0xE8 - 0xD4);
    memset(OSPhysicalToCached(0xF4), 0, 0xF8 - 0xF4);
    memset(OSPhysicalToCached(0x3000), 0, 0xC0);
    memset(OSPhysicalToCached(0x30C8), 0, 0xD4 - 0xC8);
    memset(OSPhysicalToCached(0x30E2), 0, 1);
}
#pragma dont_inline reset

u32 OSGetResetCode(void) {
    if (__OSRebootParams.valid) {
        return OS_RESETCODE_RESTART | __OSRebootParams.restartCode;
    }
    return (__PIRegs[PI_RESET_CODE] & ~7) >> 3;
}

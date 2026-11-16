/* Dolphin SDK DVD: the low-level drive interface. Each DVDLow* call writes one command to the
 * drive interface registers, remembers the callback and arms a timeout; __DVDInterruptHandler
 * turns the interrupt into a cause mask (DVD_INTTYPE_*) and hands it to the callback.
 *
 * Some drives need help with reads that land close to the previous one (work-around type 1):
 * the read is then preceded by a seek, or delayed until the drive's cache has caught up. The
 * pending steps sit in CommandList and are issued one per interrupt. */
#include <dolphin/dvd/__dvd.h>

#define CMD_END -1
#define CMD_READ 1
#define CMD_SEEK 2

#define SECTOR_SHIFT 15 /* the drive caches whole 32 KB blocks */
#define BLOCK_OF(offset) ((offset) >> SECTOR_SHIFT)

typedef struct Command {
    s32 cmd;
    void *addr;
    u32 length;
    u32 offset;
    DVDLowCallback callback;
} Command;

typedef struct Transfer {
    void *addr;
    u32 length;
    u32 offset;
} Transfer;

static BOOL FirstRead = TRUE;

static volatile BOOL StopAtNextInt;
static u32 LastLength;
static DVDLowCallback Callback;
static DVDLowCallback ResetCoverCallback;
static volatile OSTime LastResetEnd;
static vu32 ResetOccurred;
static volatile BOOL WaitingCoverClose;
static BOOL Breaking;
static vu32 WorkAroundType;
static u32 WorkAroundSeekLocation;
static volatile OSTime LastReadFinished;
static OSTime LastReadIssued;
static volatile BOOL LastCommandWasRead;
static vu32 NextCommandNumber;

static Command CommandList[3];
static OSAlarm AlarmForWA;
static OSAlarm AlarmForTimeout;
static OSAlarm AlarmForBreak;
static Transfer Prev;
static Transfer Curr;

static void fn_80378EC4(void *addr, u32 length, u32 offset, DVDLowCallback callback);

void __DVDInitWA(void) {
    NextCommandNumber = 0;
    CommandList[0].cmd = CMD_END;
    __DVDLowSetWAType(0, 0);
    OSInitAlarm();
}

/* Issues the next queued step, if any; TRUE when one was issued. */
static inline BOOL ProcessNextCommand(void) {
    s32 n = NextCommandNumber;

    if (CommandList[n].cmd == CMD_READ) {
        ++NextCommandNumber;
        fn_80378EC4(CommandList[n].addr, CommandList[n].length, CommandList[n].offset, CommandList[n].callback);
        return TRUE;
    } else if (CommandList[n].cmd == CMD_SEEK) {
        ++NextCommandNumber;
        fn_803792EC(CommandList[n].offset, CommandList[n].callback);
        return TRUE;
    }
    return FALSE;
}

void __DVDInterruptHandler(__OSInterrupt interrupt, OSContext *context) {
    DVDLowCallback callback;
    OSContext exceptionContext;
    u32 cause = 0;
    u32 reg;
    u32 intr;
    u32 mask;

    if (LastCommandWasRead) {
        LastReadFinished = __OSGetSystemTime();
        FirstRead = FALSE;
        Prev.addr = Curr.addr;
        Prev.length = Curr.length;
        Prev.offset = Curr.offset;
        if (StopAtNextInt == TRUE) {
            cause |= DVD_INTTYPE_BRK;
        }
    }
    LastCommandWasRead = FALSE;
    StopAtNextInt = FALSE;

    reg = __DIRegs[0];
    mask = reg & 0x2A;
    intr = (reg & 0x54) & (mask << 1);
    if (intr & 0x40) {
        cause |= DVD_INTTYPE_BRK;
    }
    if (intr & 0x10) {
        cause |= DVD_INTTYPE_TC;
    }
    if (intr & 0x04) {
        cause |= DVD_INTTYPE_DE;
    }
    if (cause) {
        ResetOccurred = FALSE;
        OSCancelAlarm(&AlarmForTimeout);
    }
    __DIRegs[0] = intr | mask;

    if (ResetOccurred && (__OSGetSystemTime() - LastResetEnd) < DVDMillisecondsToTicks(200)) {
        reg = __DIRegs[1];
        mask = reg & 0x2;
        intr = (reg & 0x4) & (mask << 1);
        if (intr & 0x4) {
            if (ResetCoverCallback) {
                ResetCoverCallback(DVD_INTTYPE_CVR);
            }
            ResetCoverCallback = NULL;
        }
        __DIRegs[1] = __DIRegs[1];
    } else if (WaitingCoverClose) {
        reg = __DIRegs[1];
        mask = reg & 0x2;
        intr = (reg & 0x4) & (mask << 1);
        if (intr & 0x4) {
            cause |= DVD_INTTYPE_CVR;
        }
        __DIRegs[1] = intr | mask;
        WaitingCoverClose = FALSE;
    } else {
        __DIRegs[1] = 0;
    }

    if ((cause & DVD_INTTYPE_BRK) && !Breaking) {
        cause &= ~DVD_INTTYPE_BRK;
    }

    if (cause & DVD_INTTYPE_TC) {
        if (ProcessNextCommand()) {
            return;
        }
    } else {
        CommandList[0].cmd = CMD_END;
        NextCommandNumber = 0;
    }

    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (cause) {
        callback = Callback;
        Callback = NULL;
        if (callback) {
            callback(cause);
        }
        Breaking = FALSE;
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

static void fn_80378DD0(OSAlarm *alarm, OSContext *context) {
    ProcessNextCommand();
}

static void AlarmHandlerForTimeout(OSAlarm *alarm, OSContext *context) {
    DVDLowCallback callback;
    OSContext exceptionContext;

    __OSMaskInterrupts(0x400);
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    callback = Callback;
    Callback = NULL;
    if (callback) {
        callback(DVD_INTTYPE_TIMEOUT);
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

static inline void SetTimeoutAlarm(OSTime timeout) {
    OSCreateAlarm(&AlarmForTimeout);
    OSSetAlarm(&AlarmForTimeout, timeout, AlarmHandlerForTimeout);
}

static void fn_80378EC4(void *addr, u32 length, u32 offset, DVDLowCallback callback) {
    StopAtNextInt = FALSE;
    Callback = callback;
    LastCommandWasRead = TRUE;
    LastReadIssued = __OSGetSystemTime();

    __DIRegs[2] = 0xA8000000;
    __DIRegs[3] = offset / 4;
    __DIRegs[4] = length;
    __DIRegs[5] = (u32)addr;
    __DIRegs[6] = length;
    LastLength = length;
    __DIRegs[7] = 3;

    if (length > 0xA00000) {
        SetTimeoutAlarm(DVDSecondsToTicks(20));
    } else {
        SetTimeoutAlarm(DVDSecondsToTicks(10));
    }
}

/* TRUE when the read in `curr` starts inside or just past the drive's cache window around
 * where `prev` ended. */
static inline BOOL HitCache(Transfer *curr, Transfer *prev) {
    u32 prevBlock = (prev->offset + prev->length - 1) >> SECTOR_SHIFT;
    u32 currBlock = curr->offset >> SECTOR_SHIFT;
    u32 cacheBlocks = (fn_8037C9EC()->streaming ? TRUE : FALSE) ? 5 : 15;

    if (currBlock > prevBlock - 2 || currBlock < prevBlock + cacheBlocks + 3) {
        return TRUE;
    }
    return FALSE;
}

static void fn_80378FD4(void *addr, u32 length, u32 offset, DVDLowCallback callback) {
    u32 seekOffset = offset & ~0x7FFF;

    if (seekOffset == 0) {
        seekOffset = 0;
    } else {
        seekOffset += WorkAroundSeekLocation;
    }
    CommandList[0].cmd = CMD_SEEK;
    CommandList[0].offset = seekOffset;
    CommandList[0].callback = callback;
    CommandList[1].cmd = CMD_READ;
    CommandList[1].addr = addr;
    CommandList[1].length = length;
    CommandList[1].offset = offset;
    CommandList[1].callback = callback;
    CommandList[2].cmd = CMD_END;
    NextCommandNumber = 0;
    fn_803792EC(seekOffset, callback);
}

BOOL fn_80379054(void *addr, u32 length, u32 offset, DVDLowCallback callback) {
    OSTime sinceLast;
    OSTime wait;

    __DIRegs[6] = length;
    Curr.addr = addr;
    Curr.length = length;
    Curr.offset = offset;

    if (WorkAroundType == 0) {
        CommandList[0].cmd = CMD_END;
        NextCommandNumber = 0;
        fn_80378EC4(addr, length, offset, callback);
    } else if (WorkAroundType == 1) {
        if (FirstRead) {
            fn_80378FD4(addr, length, offset, callback);
        } else if (!HitCache(&Curr, &Prev)) {
            CommandList[0].cmd = CMD_END;
            NextCommandNumber = 0;
            fn_80378EC4(addr, length, offset, callback);
        } else {
            u32 prevBlock = BLOCK_OF(Prev.offset + Prev.length - 1);

            if (prevBlock == BLOCK_OF(Curr.offset) || prevBlock + 1 == BLOCK_OF(Curr.offset)) {
                sinceLast = __OSGetSystemTime() - LastReadFinished;
                if (DVDMillisecondsToTicks(5) < sinceLast) {
                    CommandList[0].cmd = CMD_END;
                    NextCommandNumber = 0;
                    fn_80378EC4(addr, length, offset, callback);
                } else {
                    wait = DVDMillisecondsToTicks(5) - sinceLast + DVDMicrosecondsToTicks(500);
                    CommandList[0].cmd = CMD_READ;
                    CommandList[0].addr = addr;
                    CommandList[0].length = length;
                    CommandList[0].offset = offset;
                    CommandList[0].callback = callback;
                    CommandList[1].cmd = CMD_END;
                    NextCommandNumber = 0;
                    OSCreateAlarm(&AlarmForWA);
                    OSSetAlarm(&AlarmForWA, wait, fn_80378DD0);
                }
            } else {
                fn_80378FD4(addr, length, offset, callback);
            }
        }
    }
    return TRUE;
}

BOOL fn_803792EC(u32 offset, DVDLowCallback callback) {
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xAB000000;
    __DIRegs[3] = offset / 4;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(DVDSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowWaitCoverClose(DVDLowCallback callback) {
    Callback = callback;
    WaitingCoverClose = TRUE;
    StopAtNextInt = FALSE;
    __DIRegs[1] = 2;
    return TRUE;
}

BOOL fn_803793AC(DVDDiskID *diskID, DVDLowCallback callback) {
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xA8000040;
    __DIRegs[3] = 0;
    __DIRegs[4] = sizeof(DVDDiskID);
    __DIRegs[5] = (u32)diskID;
    __DIRegs[6] = sizeof(DVDDiskID);
    __DIRegs[7] = 3;
    SetTimeoutAlarm(DVDSecondsToTicks(10));
    return TRUE;
}

BOOL DVDLowStopMotor(DVDLowCallback callback) {
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xE3000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(DVDSecondsToTicks(10));
    return TRUE;
}

BOOL fn_803794DC(DVDLowCallback callback) {
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0xE0000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(DVDSecondsToTicks(10));
    return TRUE;
}

BOOL fn_80379568(void *info, DVDLowCallback callback) {
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = 0x12000000;
    __DIRegs[4] = 0x20;
    __DIRegs[5] = (u32)info;
    __DIRegs[6] = 0x20;
    __DIRegs[7] = 3;
    SetTimeoutAlarm(DVDSecondsToTicks(10));
    return TRUE;
}

BOOL fn_80379604(u32 subcmd, u32 length, u32 offset, DVDLowCallback callback) {
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = subcmd | 0xE1000000;
    __DIRegs[3] = offset >> 2;
    __DIRegs[4] = length;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(DVDSecondsToTicks(10));
    return TRUE;
}

BOOL fn_8037969C(u32 subcmd, DVDLowCallback callback) {
    Callback = callback;
    StopAtNextInt = FALSE;
    __DIRegs[2] = subcmd | 0xE2000000;
    __DIRegs[7] = 1;
    SetTimeoutAlarm(DVDSecondsToTicks(10));
    return TRUE;
}

BOOL fn_80379728(BOOL enable, u32 size, DVDLowCallback callback) {
    u32 bufEnable;

    Callback = callback;
    StopAtNextInt = FALSE;
    bufEnable = enable ? 0x10000 : 0;
    __DIRegs[2] = size | (0xE4000000 | bufEnable);
    __DIRegs[7] = 1;
    SetTimeoutAlarm(DVDSecondsToTicks(10));
    return TRUE;
}

void DVDLowReset(void) {
    u32 reg;
    OSTime resetStart;

    __DIRegs[1] = 2;
    reg = __PIRegs[9];
    __PIRegs[9] = (reg & ~4) | 1;

    resetStart = __OSGetSystemTime();
    while ((__OSGetSystemTime() - resetStart) < DVDMicrosecondsToTicks(12)) {
    }

    __PIRegs[9] = reg | 4 | 1;
    ResetOccurred = TRUE;
    LastResetEnd = __OSGetSystemTime();
}

BOOL fn_80379880(void) {
    StopAtNextInt = TRUE;
    Breaking = TRUE;
    return TRUE;
}

DVDLowCallback fn_80379894(void) {
    DVDLowCallback old;

    __DIRegs[1] = 0;
    old = Callback;
    WaitingCoverClose = FALSE;
    Callback = NULL;
    return old;
}

void __DVDLowSetWAType(u32 type, u32 location) {
    BOOL enabled = OSDisableInterrupts();

    WorkAroundType = type;
    WorkAroundSeekLocation = location;
    OSRestoreInterrupts(enabled);
}

BOOL fn_803798F4(OSAlarm *alarm) {
    if (alarm == &AlarmForBreak) {
        return TRUE;
    }
    if (alarm == &AlarmForTimeout) {
        return TRUE;
    }
    return FALSE;
}

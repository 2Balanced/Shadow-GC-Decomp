/* Dolphin SDK OS: start-up.
 *
 * OSInit runs once, early in main: it reads the boot information the IPL left in low memory,
 * sets up the arena, installs the first-level exception vectors and brings up every OS
 * subsystem (alarms, interrupts, contexts, caches, EXI, SI, SRAM, threads, audio), then prints
 * the kernel banner and clears the arena. The exception vector copied to each PowerPC exception
 * location saves a few registers and jumps through OSExceptionTable to the second-level handler
 * (OSDefaultExceptionHandler unless something else registered one). */
#include <dolphin/base/PPCArch.h>
#include <dolphin/os.h>

void *memset(void *dst, int value, unsigned long n);
void *memcpy(void *dst, const void *src, unsigned long n);

volatile u32 __DIRegs[] : 0xCC006000;

/* Console type: the top nibble is the family (0 retail, 1 development); development consoles
 * number their board revision in the low bits. */
#define OS_CONSOLE_RETAIL 0x00000000
#define OS_CONSOLE_DEVELOPMENT 0x10000000
#define OS_CONSOLE_TDEV 0x20000000
#define OS_CONSOLE_EMULATOR 0x10000000
#define OS_CONSOLE_PC_EMULATOR 0x10000001
#define OS_CONSOLE_ARTHUR 0x10000002
#define OS_CONSOLE_MINNOW 0x10000003

/* The block the IPL leaves at the bottom of main RAM. */
typedef struct OSBootInfo {
    u8 diskID[0x20];
    u32 magic;
    u32 version;
    u32 memorySize;
    u32 consoleType;
    void *arenaLo;
    void *arenaHi;
    void *FSTLocation;
    u32 FSTMaxLength;
} OSBootInfo;

typedef struct DVDDriveInfo {
    u16 revisionLevel;
    u16 deviceCode;
    u32 releaseDate;
    u8 padding[24];
} DVDDriveInfo;

typedef struct DVDCommandBlock DVDCommandBlock;
typedef void (*DVDCBCallback)(s32 result, DVDCommandBlock *block);
struct DVDCommandBlock {
    DVDCommandBlock *next;
    DVDCommandBlock *prev;
    u32 command;
    s32 state;
    u32 offset;
    u32 length;
    void *addr;
    u32 currTransferSize;
    u32 transferredSize;
    void *id;
    DVDCBCallback callback;
    void *userData;
};

/* What a reboot into this program was asked to preserve; filled in by __OSGetExecParams. */
typedef struct OSExecParams {
    u32 valid;
    u32 restartCode;
    u32 bootDol;
    void *regionStart;
    void *regionEnd;
    u32 argsUseDefault;
    void *argsAddr;
    u32 pad;
} OSExecParams;

#define BI2_DEBUG_FLAG_OFFSET 0x0C
#define BI2_PAD_SPEC_OFFSET 0x24

#define BOOT_INFO ((OSBootInfo *)OSPhysicalToCached(0x0000))
#define BI2_LOCATION (*(u8 **)OSPhysicalToCached(0x00F4))
#define DEBUG_FLAG_COPY (*(u8 *)OSPhysicalToCached(0x30E8))
#define PAD_SPEC_COPY (*(u8 *)OSPhysicalToCached(0x30E9))
#define DEVICE_CODE (*(u16 *)OSPhysicalToCached(0x30E6))
#define DBINT_LOCATION OSPhysicalToCached(0x0060)
#define EXCEPTION_TABLE OSPhysicalToCached(0x3000)

#define EXCEPTION_COUNT 15
#define RESET_SW_INTERRUPT 22
#define PPC_NOP 0x60000000

extern u8 __ArenaLo[];
extern u8 __ArenaHi[];
extern u8 _stack_addr[];

extern u32 __PADSpec;
extern BOOL __DVDLongFileNameFlag;
extern OSTime __OSStartTime;

void __OSGetExecParams(OSExecParams *params);
void OSSetArenaLo(void *addr);
void OSSetArenaHi(void *addr);
void *OSGetArenaLo(void);
void *OSGetArenaHi(void);
void __OSInitSystemCall(void);
void __OSModuleInit(void);
void __OSInterruptInit(void);
void *__OSSetInterruptHandler(s16 interrupt, void *handler);
void __OSResetSWInterruptHandler(s16 interrupt, OSContext *context);
void __OSContextInit(void);
void __OSCacheInit(void);
void EXIInit(void);
void SIInit(void);
void __OSInitSram(void);
void __OSThreadInit(void);
void __OSInitAudioSystem(void);
void __OSInitMemoryProtection(void);
void EnableMetroTRKInterrupts(void);
u32 OSGetResetCode(void);
void DVDInit(void);
BOOL DVDInquiryAsync(DVDCommandBlock *block, DVDDriveInfo *info, DVDCBCallback callback);
BOOL __DBIsExceptionMarked(__OSException exception);
void __OSUnhandledException(__OSException exception, OSContext *context, u32 dsisr, u32 dar);
void OSRegisterVersion(const char *id);

void __OSDBINTSTART(void);
void __OSDBINTEND(void);
void __OSDBJUMPEND(void);
void __OSEVStart(void);
void __OSEVEnd(void);
void __OSEVSetNumber(void);
void __DBVECTOR(void);

static void OSExceptionInit(void);
static void OSExceptionVector(void);
void OSDefaultExceptionHandler(__OSException exception, OSContext *context);

char *__OSVersion = "<< Dolphin SDK - OS\trelease build: Nov 10 2004 06:26:41 (0x2301) >>";

static OSBootInfo *BootInfo;
static u32 *BI2DebugFlag;
static u32 BI2DebugFlagHolder;
__declspec(weak) BOOL __OSIsGcam = FALSE;
static f64 ZeroF;
static f32 ZeroPS[2];
static BOOL AreWeInitialized = FALSE;
static __OSExceptionHandler *OSExceptionTable;
OSTime __OSStartTime;
BOOL __OSInIPL;

static DVDDriveInfo DriveInfo __attribute__((aligned(32)));
static DVDCommandBlock DriveBlock;
OSExecParams __OSRebootParams;

/* Zeroes every floating-point register (and both halves of each paired single when paired
 * singles are enabled) and the FPSCR, so no thread starts with stale values. */
asm void __OSFPRInit(void) {
    nofralloc
    mfmsr r3
    ori r3, r3, 0x2000
    mtmsr r3
    mfspr r3, HID2
    rlwinm. r3, r3, 3, 31, 31
    beq @scalars
    lis r3, ZeroPS@ha
    addi r3, r3, ZeroPS@l
    psq_l f0, 0(r3), 0, 0
    ps_mr f1, f0
    ps_mr f2, f0
    ps_mr f3, f0
    ps_mr f4, f0
    ps_mr f5, f0
    ps_mr f6, f0
    ps_mr f7, f0
    ps_mr f8, f0
    ps_mr f9, f0
    ps_mr f10, f0
    ps_mr f11, f0
    ps_mr f12, f0
    ps_mr f13, f0
    ps_mr f14, f0
    ps_mr f15, f0
    ps_mr f16, f0
    ps_mr f17, f0
    ps_mr f18, f0
    ps_mr f19, f0
    ps_mr f20, f0
    ps_mr f21, f0
    ps_mr f22, f0
    ps_mr f23, f0
    ps_mr f24, f0
    ps_mr f25, f0
    ps_mr f26, f0
    ps_mr f27, f0
    ps_mr f28, f0
    ps_mr f29, f0
    ps_mr f30, f0
    ps_mr f31, f0
@scalars:
    lfd f0, ZeroF
    fmr f1, f0
    fmr f2, f0
    fmr f3, f0
    fmr f4, f0
    fmr f5, f0
    fmr f6, f0
    fmr f7, f0
    fmr f8, f0
    fmr f9, f0
    fmr f10, f0
    fmr f11, f0
    fmr f12, f0
    fmr f13, f0
    fmr f14, f0
    fmr f15, f0
    fmr f16, f0
    fmr f17, f0
    fmr f18, f0
    fmr f19, f0
    fmr f20, f0
    fmr f21, f0
    fmr f22, f0
    fmr f23, f0
    fmr f24, f0
    fmr f25, f0
    fmr f26, f0
    fmr f27, f0
    fmr f28, f0
    fmr f29, f0
    fmr f30, f0
    fmr f31, f0
    mtfsf 0xFF, f0
    blr
}

/* Hardware without boot information is treated as an Arthur development board. */
u32 OSGetConsoleType(void) {
    if (BootInfo == NULL || BootInfo->consoleType == 0) {
        return OS_CONSOLE_ARTHUR;
    }
    return BootInfo->consoleType;
}

/* Records the drive's device code once the inquiry started by OSInit finishes (1 if it failed). */
static void InquiryCallback(s32 result, DVDCommandBlock *block) {
    switch (block->state) {
    case 0:
        DEVICE_CODE = (u16)(0x8000 | DriveInfo.deviceCode);
        break;
    default:
        DEVICE_CODE = 1;
        break;
    }
}

void OSInit(void) {
    u8 *bi2;
    u32 consoleType;

    if (AreWeInitialized != FALSE) {
        return;
    }
    AreWeInitialized = TRUE;

    __OSStartTime = __OSGetSystemTime();
    OSDisableInterrupts();
    __OSGetExecParams(&__OSRebootParams);

    PPCMtmmcr0(0);
    PPCMtmmcr1(0);
    PPCMtpmc1(0);
    PPCMtpmc2(0);
    PPCMtpmc3(0);
    PPCMtpmc4(0);
    PPCDisableSpeculation();
    PPCSetFpNonIEEEMode();

    BI2DebugFlag = NULL;
    BootInfo = BOOT_INFO;
    __DVDLongFileNameFlag = FALSE;

    bi2 = BI2_LOCATION;
    if (bi2 != NULL) {
        BI2DebugFlag = (u32 *)(bi2 + BI2_DEBUG_FLAG_OFFSET);
        __PADSpec = *(u32 *)(bi2 + BI2_PAD_SPEC_OFFSET);
        DEBUG_FLAG_COPY = (u8)*BI2DebugFlag;
        PAD_SPEC_COPY = (u8)__PADSpec;
    } else if (BootInfo->arenaHi != NULL) {
        BI2DebugFlagHolder = DEBUG_FLAG_COPY;
        BI2DebugFlag = &BI2DebugFlagHolder;
        __PADSpec = PAD_SPEC_COPY;
    }

    __DVDLongFileNameFlag = TRUE;

    OSSetArenaLo(BootInfo->arenaLo == NULL ? __ArenaLo : BootInfo->arenaLo);
    if (BootInfo->arenaLo == NULL && BI2DebugFlag != NULL && *BI2DebugFlag < 2) {
        OSSetArenaLo((void *)(((u32)_stack_addr + 31) & ~31));
    }
    OSSetArenaHi(BootInfo->arenaHi == NULL ? __ArenaHi : BootInfo->arenaHi);

    OSExceptionInit();
    __OSInitSystemCall();
    OSInitAlarm();
    __OSModuleInit();
    __OSInterruptInit();
    __OSSetInterruptHandler(RESET_SW_INTERRUPT, __OSResetSWInterruptHandler);
    __OSContextInit();
    __OSCacheInit();
    EXIInit();
    SIInit();
    __OSInitSram();
    __OSThreadInit();
    __OSInitAudioSystem();
    PPCMthid2(PPCMfhid2() & ~0x40000000);
    if (!__OSInIPL) {
        __OSInitMemoryProtection();
    }

    OSReport("\nDolphin OS\n");
    OSReport("Kernel built : %s %s\n", "Nov 10 2004", "06:26:41");
    OSReport("Console Type : ");
    consoleType = OSGetConsoleType();
    switch (consoleType & 0xF0000000) {
    case OS_CONSOLE_RETAIL:
        OSReport("Retail %d\n", consoleType);
        break;
    case OS_CONSOLE_DEVELOPMENT:
    case OS_CONSOLE_TDEV:
        switch (consoleType & 0x0FFFFFFF) {
        case OS_CONSOLE_EMULATOR:
            OSReport("Mac Emulator\n");
            break;
        case OS_CONSOLE_PC_EMULATOR:
            OSReport("PC Emulator\n");
            break;
        case OS_CONSOLE_ARTHUR:
            OSReport("EPPC Arthur\n");
            break;
        case OS_CONSOLE_MINNOW:
            OSReport("EPPC Minnow\n");
            break;
        default:
            OSReport("Development HW%d (%08x)\n", (consoleType & 0x0FFFFFFF) - 3, consoleType);
            break;
        }
        break;
    default:
        OSReport("%08x\n", consoleType);
        break;
    }
    OSReport("Memory %d MB\n", BootInfo->memorySize >> 20);
    OSReport("Arena : 0x%x - 0x%x\n", OSGetArenaLo(), OSGetArenaHi());
    OSRegisterVersion(__OSVersion);

    if (BI2DebugFlag != NULL && *BI2DebugFlag >= 2) {
        EnableMetroTRKInterrupts();
    }

    if (!((OSGetResetCode() & 0x80000000) ? TRUE : FALSE)) {
        memset(OSGetArenaLo(), 0, (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
    } else if (__OSRebootParams.regionStart == NULL) {
        memset(OSGetArenaLo(), 0, (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
    } else if ((u32)OSGetArenaLo() < (u32)__OSRebootParams.regionStart) {
        /* A reboot that asked to keep part of the arena clears only around that region. */
        if ((u32)OSGetArenaHi() <= (u32)__OSRebootParams.regionStart) {
            memset(OSGetArenaLo(), 0, (u32)OSGetArenaHi() - (u32)OSGetArenaLo());
        } else {
            memset(OSGetArenaLo(), 0,
                   (u32)__OSRebootParams.regionStart - (u32)OSGetArenaLo());
            if ((u32)OSGetArenaHi() > (u32)__OSRebootParams.regionEnd) {
                memset(__OSRebootParams.regionEnd, 0,
                       (u32)OSGetArenaHi() - (u32)__OSRebootParams.regionEnd);
            }
        }
    }
    OSEnableInterrupts();

    if (!__OSInIPL) {
        DVDInit();
        if (__OSIsGcam) {
            DEVICE_CODE = 0x9000;
        } else {
            DCInvalidateRange(&DriveInfo, sizeof(DriveInfo));
            DVDInquiryAsync(&DriveBlock, &DriveInfo, InquiryCallback);
        }
    }
}

/* PowerPC exception vector offsets, in exception-number order. */
static u32 __OSExceptionLocations[] = {
    0x00000100, 0x00000200, 0x00000300, 0x00000400, 0x00000500,
    0x00000600, 0x00000700, 0x00000800, 0x00000900, 0x00000C00,
    0x00000D00, 0x00000F00, 0x00001300, 0x00001400, 0x00001700,
};

/* Copies OSExceptionVector to every exception location, patching each copy's exception number
 * into its `li r3, n`. Exceptions the debugger claimed are left alone or routed to it. */
static void OSExceptionInit(void) {
    __OSException exception;
    u32 *opCodeAddr = (u32 *)__OSEVSetNumber;
    u32 oldOpCode = *opCodeAddr;
    void *handlerStart = (void *)__OSEVStart;
    u32 handlerSize = (u32)__OSEVEnd - (u32)__OSEVStart;
    u8 *destAddr = DBINT_LOCATION;
    u32 *ops;
    u32 cb;

    if (*(u32 *)destAddr == 0) {
        u32 size;

        DBPrintf("Installing OSDBIntegrator\n");
        size = (u32)__OSDBINTEND - (u32)__OSDBINTSTART;
        memcpy(destAddr, (void *)__OSDBINTSTART, size);
        DCFlushRangeNoSync(destAddr, size);
        __sync();
        ICInvalidateRange(destAddr, size);
    }

    for (exception = 0; exception < EXCEPTION_COUNT; exception++) {
        if (BI2DebugFlag != NULL && *BI2DebugFlag >= 2 && __DBIsExceptionMarked(exception)) {
            DBPrintf(">>> OSINIT: exception %d commandeered by TRK\n", exception);
            continue;
        }

        *opCodeAddr = oldOpCode | exception;
        if (__DBIsExceptionMarked(exception)) {
            DBPrintf(">>> OSINIT: exception %d vectored to debugger\n", exception);
            memcpy((void *)__DBVECTOR, (void *)__OSDBINTEND,
                   (u32)__OSDBJUMPEND - (u32)__OSDBINTEND);
        } else {
            ops = (u32 *)__DBVECTOR;
            for (cb = 0; cb < (u32)__OSDBJUMPEND - (u32)__OSDBINTEND; cb += 4) {
                *ops++ = PPC_NOP;
            }
        }

        destAddr = OSPhysicalToCached(__OSExceptionLocations[exception]);
        memcpy(destAddr, handlerStart, handlerSize);
        DCFlushRangeNoSync(destAddr, handlerSize);
        __sync();
        ICInvalidateRange(destAddr, handlerSize);
    }

    OSExceptionTable = EXCEPTION_TABLE;
    for (exception = 0; exception < EXCEPTION_COUNT; exception++) {
        __OSSetExceptionHandler(exception, OSDefaultExceptionHandler);
    }
    *opCodeAddr = oldOpCode;
    DBPrintf("Exceptions initialized...\n");
}

/* Copied to 0x60: lets the debugger take over an exception. Saves LR, jumps to the debugger's
 * entry at 0x48 with translation on. */
static asm void __OSDBIntegrator(void) {
    nofralloc
entry __OSDBINTSTART
    li r5, 0x40
    mflr r3
    stw r3, 0xC(r5)
    lwz r3, 0x8(r5)
    oris r3, r3, 0x8000
    mtlr r3
    li r3, 0x30
    mtmsr r3
    blr
}

/* Patched into an exception vector that the debugger claimed. */
asm void __OSDBJump(void) {
    nofralloc
entry __OSDBINTEND
    bla 0x60
entry __OSDBJUMPEND
}

__OSExceptionHandler __OSSetExceptionHandler(__OSException exception, __OSExceptionHandler handler) {
    __OSExceptionHandler old = OSExceptionTable[exception];

    OSExceptionTable[exception] = handler;
    return old;
}

__OSExceptionHandler __OSGetExceptionHandler(__OSException exception) {
    return OSExceptionTable[exception];
}

/* The first-level handler copied to each exception location. Runs with translation off: saves
 * r3-r5 and the special registers into the current context (physical pointer at 0xC0), then
 * returns into the second-level handler for this exception number through the table at 0x3000,
 * or into OSDefaultExceptionHandler when the exception is not recoverable (MSR[RI] clear). */
static asm void OSExceptionVector(void) {
    nofralloc
entry __OSEVStart
    mtsprg 0, r4
    lwz r4, 0x00C0(r0)
    stw r3, OSContext.gpr[3](r4)
    mfsprg r3, 0
    stw r3, OSContext.gpr[4](r4)
    stw r5, OSContext.gpr[5](r4)
    lhz r3, OSContext.state(r4)
    ori r3, r3, 0x2
    sth r3, OSContext.state(r4)
    mfcr r3
    stw r3, OSContext.cr(r4)
    mflr r3
    stw r3, OSContext.lr(r4)
    mfctr r3
    stw r3, OSContext.ctr(r4)
    mfxer r3
    stw r3, OSContext.xer(r4)
    mfsrr0 r3
    stw r3, OSContext.srr0(r4)
    mfsrr1 r3
    stw r3, OSContext.srr1(r4)
    mr r5, r3
entry __DBVECTOR
    nop
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
entry __OSEVSetNumber
    li r3, 0
    lwz r4, 0x00D4(r0)
    rlwinm. r5, r5, 0, 30, 30
    bne @recoverable
    lis r5, OSDefaultExceptionHandler@ha
    addi r5, r5, OSDefaultExceptionHandler@l
    mtsrr0 r5
    rfi
@recoverable:
    rlwinm r5, r3, 2, 22, 29
    lwz r5, 0x3000(r5)
    mtsrr0 r5
    rfi
entry __OSEVEnd
    nop
}

/* Saves the rest of the context and reports the exception as unhandled. */
asm void OSDefaultExceptionHandler(register __OSException exception,
                               register OSContext *context) {
    nofralloc
    OS_EXCEPTION_SAVE_GPRS(context);
    mfdsisr r5
    mfdar r6
    stwu r1, -8(r1)
    b __OSUnhandledException
}

/* Turns on paired singles and locked-cache support and clears the quantization registers. */
void __OSPSInit(void) {
    PPCMthid2(PPCMfhid2() | 0xA0000000);
    ICFlashInvalidate();
    __sync();
    asm {
        li r3, 0
        mtspr GQR0, r3
        mtspr GQR1, r3
        mtspr GQR2, r3
        mtspr GQR3, r3
        mtspr GQR4, r3
        mtspr GQR5, r3
        mtspr GQR6, r3
        mtspr GQR7, r3
    }
}

/* Low byte of the DI configuration register. */
u32 __OSGetDIConfig(void) {
    return (u8)__DIRegs[9];
}

void OSRegisterVersion(const char *id) {
    OSReport("%s\n", id);
}

/* Dolphin SDK DVD: the command queue and its state machine. Requests (DVDCommandBlocks) wait in
 * priority queues (dvdqueue.c); the one being served is `executing`. Each state issues one
 * low-level drive command whose interrupt callback (cbFor*) picks the next state. Drive errors
 * are read back and sorted into "retry", "ask the user" (cover open, no disc, wrong disc) and
 * fatal; a cover that was opened leads through a reset and a disc ID check before the request is
 * resumed.
 *
 * Most functions keep dtk's names: nothing in the binary names them. The game links only part of
 * the library; the rest (streaming setup, disc change, DVDCancelAll and others) was dead-stripped,
 * but the message of the removed DVDChangeDiskAsync stays in the data. */
#include <dolphin/dvd/__dvd.h>

#define DVD_COMMAND_NONE 0
#define DVD_COMMAND_READ 1
#define DVD_COMMAND_SEEK 2
#define DVD_COMMAND_CHANGE_DISK 3
#define DVD_COMMAND_BSREAD 4
#define DVD_COMMAND_READID 5
#define DVD_COMMAND_INITSTREAM 6
#define DVD_COMMAND_CANCELSTREAM 7
#define DVD_COMMAND_STOP_STREAM_AT_END 8
#define DVD_COMMAND_REQUEST_AUDIO_ERROR 9
#define DVD_COMMAND_REQUEST_PLAY_ADDR 10
#define DVD_COMMAND_REQUEST_START_ADDR 11
#define DVD_COMMAND_REQUEST_LENGTH 12
#define DVD_COMMAND_AUDIO_BUFFER_CONFIG 13
#define DVD_COMMAND_INQUIRY 14
#define DVD_COMMAND_BS_CHANGE_DISK 15
#define DVD_COMMAND_STOP_MOTOR 16

#define DVD_RESULT_COVER_CLOSED -4

/* Error codes stored for the IPL when the drive itself gives none. */
#define DVD_INTERNALERROR_NO_DRIVE 0x01234567
#define DVD_INTERNALERROR_TIMEOUT 0x01234568

/* Second part of the disc header: where the boot file and the FST are. */
typedef struct DVDBB2 {
    u32 bootFilePosition;
    u32 FSTPosition;
    u32 FSTLength;
    u32 FSTMaxLength;
    void *FSTAddress;
    u32 userPosition;
    u32 userLength;
    u32 padding0;
} DVDBB2;

typedef struct DVDBootInfo {
    DVDDiskID DVDDiskID;
    u32 magic;
    u32 version;
    u32 memorySize;
    u32 consoleType;
    void *arenaLo;
    void *arenaHi;
    void *FSTLocation;
    u32 FSTMaxLength;
} DVDBootInfo;

typedef void (*stateFunc)(DVDCommandBlock *block);
typedef void (*DVDOptionalCommandChecker)(DVDCommandBlock *block, DVDLowCallback callback);
typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, OSContext *context);

void OSRegisterVersion(const char *id);
__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt interrupt, __OSInterruptHandler handler);
void OSInitThreadQueue(OSThreadQueue *queue);
void *memcpy(void *dest, const void *src, u32 n);
int fn_803A8DA8(const void *a, const void *b, u32 n);
#define memcmp fn_803A8DA8

static void fn_8037A4B8(DVDCommandBlock *block, DVDLowCallback callback) {
}

const char *__DVDVersion = "<< Dolphin SDK - DVD\trelease build: Apr  5 2004 04:14:51 (0x2301) >>";
static BOOL autoInvalidation = TRUE;
static DVDOptionalCommandChecker checkOptionalCommand = fn_8037A4B8;

static DVDBB2 BB2 __attribute__((aligned(32)));
static DVDDiskID CurrDiskID __attribute__((aligned(32)));
static DVDCommandBlock DummyCommandBlock;
static OSAlarm ResetAlarm;

static DVDCommandBlock *executing;
static DVDDiskID *IDShouldBe;
static DVDBootInfo *bootInfo;
static volatile BOOL PauseFlag;
static volatile BOOL PausingFlag;
static volatile BOOL AutoFinishing;
static volatile BOOL FatalErrorFlag;
static vu32 CurrCommand;
static vu32 Canceling;
static DVDCBCallback CancelCallback;
static vu32 ResumeFromHere;
static vu32 CancelLastError;
static vu32 LastError;
static vs32 NumInternalRetry;
static volatile BOOL ResetRequired;
static volatile BOOL FirstTimeInBootrom;
static vu32 MotorState;
static BOOL DVDInitialized;
static stateFunc LastState;

static void fn_8037A594(void);
static void fn_8037A628(u32 intType);
static void cbForStateError(u32 intType);
static void fn_8037A754(void);
static void fn_8037A788(void);
static void fn_8037A864(u32 intType);
static void fn_8037AAC8(u32 intType);
static void fn_8037AB24(u32 intType);
static void fn_8037ABA4(void);
static void fn_8037ABCC(u32 intType);
static void fn_8037AD0C(void);
static void fn_8037ADEC(DVDCommandBlock *block);
static void fn_8037AE20(DVDCommandBlock *block);
static void fn_8037AE54(u32 intType);
static void fn_8037AEBC(DVDCommandBlock *block);
static void fn_8037AEF4(u32 intType);
static void fn_8037AFF0(u32 intType);
static void fn_8037B0C8(u32 intType);
static void fn_8037B1FC(void);
static void fn_8037B2D0(DVDCommandBlock *block);
static void fn_8037B300(u32 intType);
static void fn_8037B364(void);
static void cbForStateMotorStopped(u32 intType);
static void stateReady(void);
static void stateBusy(DVDCommandBlock *block);
static void fn_8037BAA0(u32 intType);

void DVDInit(void) {
    if (DVDInitialized) {
        return;
    }
    OSRegisterVersion(__DVDVersion);
    DVDInitialized = TRUE;
    __DVDFSInit();
    __DVDClearWaitingQueue();
    __DVDInitWA();

    bootInfo = (DVDBootInfo *)OSPhysicalToCached(0);
    MotorState = 0;
    IDShouldBe = &bootInfo->DVDDiskID;
    __OSSetInterruptHandler(21, __DVDInterruptHandler);
    __OSUnmaskInterrupts(0x400);
    OSInitThreadQueue(&__DVDThreadQueue);
    __DIRegs[0] = 0x2A;
    __DIRegs[1] = 0;

    if (bootInfo->magic == 0xE5207C22) {
        OSReport("load fst\n");
        __fstLoad();
    } else if (bootInfo->magic != 0x0D15EA5E) {
        FirstTimeInBootrom = TRUE;
    }
}

/* Reads the new disc's FST over the old one. */
static void fn_8037A594(void) {
    LastState = (stateFunc)fn_8037A594;
    if (bootInfo->FSTMaxLength < BB2.FSTLength) {
        OSPanic(__FILE__, 661, "DVDChangeDisk(): FST in the new disc is too big.   ");
    }
    fn_80379054(bootInfo->FSTLocation, (BB2.FSTLength + 31) & ~31, BB2.FSTPosition, fn_8037A628);
}

static void fn_8037A628(u32 intType) {
    DVDCommandBlock *finished;

    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_TC) {
        NumInternalRetry = 0;
        __DVDFSInit();
        finished = executing;
        executing = &DummyCommandBlock;
        finished->state = DVD_STATE_END;
        if (finished->callback) {
            finished->callback(DVD_RESULT_GOOD, finished);
        }
        stateReady();
    } else {
        fn_8037A788();
    }
}

static void cbForStateError(u32 intType) {
    DVDCommandBlock *finished;

    executing->state = DVD_STATE_FATAL_ERROR;
    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }

    fn_8037D1C4();
    FatalErrorFlag = TRUE;
    finished = executing;
    executing = &DummyCommandBlock;
    if (finished->callback) {
        finished->callback(DVD_RESULT_FATAL_ERROR, finished);
    }
    if (Canceling) {
        Canceling = FALSE;
        if (CancelCallback) {
            CancelCallback(DVD_RESULT_GOOD, finished);
        }
    }
    stateReady();
}

/* The drive stopped answering. */
static void fn_8037A754(void) {
    __DVDStoreErrorCode(DVD_INTERNALERROR_TIMEOUT);
    DVDReset();
    cbForStateError(0);
}

static void fn_8037A788(void) {
    fn_803794DC(fn_8037A864);
}

/* Sorts a drive error: 0 = a state the user has to resolve (cover, disc), 1 = fatal, 2 = retry
 * from the start, 3 = retry the last step. */
static u32 fn_8037A7B0(u32 error) {
    if (error == 0x20400) {
        LastError = error;
        return 1;
    }

    error &= 0xFFFFFF;
    if (error == 0x62800 || error == 0x23A00 || error == 0xB5A01) {
        return 0;
    }

    ++NumInternalRetry;
    if (NumInternalRetry == 2) {
        if (error == LastError) {
            LastError = error;
            return 1;
        } else {
            LastError = error;
            return 2;
        }
    }

    LastError = error;
    if (error == 0x31100 || executing->command == DVD_COMMAND_READID) {
        return 2;
    }
    return 3;
}

/* A pending cancel takes effect: finishes `executing` as canceled and remembers where to resume
 * the drive. */
static inline BOOL CheckCancel(u32 resume) {
    DVDCommandBlock *finished;

    if (Canceling) {
        ResumeFromHere = resume;
        Canceling = FALSE;
        finished = executing;
        executing = &DummyCommandBlock;
        finished->state = DVD_STATE_CANCELED;
        if (finished->callback) {
            finished->callback(DVD_RESULT_CANCELED, finished);
        }
        if (CancelCallback) {
            CancelCallback(DVD_RESULT_GOOD, finished);
        }
        stateReady();
        return TRUE;
    }
    return FALSE;
}

static inline void stateError(u32 error) {
    __DVDStoreErrorCode(error);
    DVDLowStopMotor(cbForStateError);
}

static void fn_8037A864(u32 intType) {
    u32 error;
    u32 status;
    u32 errorCategory;
    u32 resume;

    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_DE) {
        stateError(DVD_INTERNALERROR_NO_DRIVE);
        return;
    }

    error = __DIRegs[8];
    status = error & 0xFF000000;
    errorCategory = fn_8037A7B0(error);
    if (errorCategory == 1) {
        stateError(error);
        return;
    }

    if (errorCategory == 2 || errorCategory == 3) {
        resume = 0;
    } else if (status == 0x01000000) {
        resume = 4;
    } else if (status == 0x02000000) {
        resume = 6;
    } else if (status == 0x03000000) {
        resume = 3;
    } else {
        resume = 5;
    }

    if (CheckCancel(resume)) {
        return;
    }

    if (errorCategory == 2) {
        __DVDStoreErrorCode(error);
        fn_8037ABA4();
        return;
    }
    if (errorCategory == 3) {
        if ((error & 0xFFFFFF) == 0x31100) {
            fn_803792EC(executing->offset, fn_8037AAC8);
        } else {
            LastState(executing);
        }
        return;
    }

    if (status == 0x01000000) {
        executing->state = DVD_STATE_COVER_OPEN;
        fn_8037B364();
    } else if (status == 0x02000000) {
        executing->state = DVD_STATE_COVER_CLOSED;
        fn_8037B1FC();
    } else if (status == 0x03000000) {
        executing->state = DVD_STATE_NO_DISK;
        fn_8037B364();
    } else {
        stateError(DVD_INTERNALERROR_NO_DRIVE);
    }
}

static void fn_8037AAC8(u32 intType) {
    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_TC) {
        fn_8037ABA4();
        return;
    }
    fn_803794DC(fn_8037AB24);
}

static void fn_8037AB24(u32 intType) {
    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_DE) {
        stateError(DVD_INTERNALERROR_NO_DRIVE);
        return;
    }
    stateError(__DIRegs[8]);
}

static void fn_8037ABA4(void) {
    DVDLowStopMotor(fn_8037ABCC);
}

static void fn_8037ABCC(u32 intType) {
    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_DE) {
        stateError(DVD_INTERNALERROR_NO_DRIVE);
        return;
    }

    NumInternalRetry = 0;
    if (CurrCommand == DVD_COMMAND_BSREAD || CurrCommand == DVD_COMMAND_READID ||
        CurrCommand == DVD_COMMAND_AUDIO_BUFFER_CONFIG || CurrCommand == DVD_COMMAND_BS_CHANGE_DISK) {
        ResetRequired = TRUE;
    }
    if (CheckCancel(2)) {
        return;
    }
    executing->state = DVD_STATE_RETRY;
    fn_8037B364();
}

/* After the cover was closed: is it still the disc we expect? */
static void fn_8037AD0C(void) {
    switch (CurrCommand) {
    case DVD_COMMAND_CHANGE_DISK:
        if (fn_8037CFD0(&CurrDiskID, executing->id)) {
            memcpy(IDShouldBe, &CurrDiskID, sizeof(DVDDiskID));
            executing->state = DVD_STATE_BUSY;
            DCInvalidateRange(&BB2, sizeof(DVDBB2));
            LastState = fn_8037AE20;
            fn_8037AE20(executing);
        } else {
            DVDLowStopMotor(fn_8037AEF4);
        }
        break;
    default:
        if (memcmp(&CurrDiskID, IDShouldBe, sizeof(DVDDiskID))) {
            DVDLowStopMotor(fn_8037AEF4);
        } else {
            LastState = fn_8037ADEC;
            fn_8037ADEC(executing);
        }
        break;
    }
}

static void fn_8037ADEC(DVDCommandBlock *block) {
    fn_80379728(IDShouldBe->streaming, 10, fn_8037B0C8);
}

static void fn_8037AE20(DVDCommandBlock *block) {
    fn_80379728(IDShouldBe->streaming, 10, fn_8037AE54);
}

static void fn_8037AE54(u32 intType) {
    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_TC) {
        NumInternalRetry = 0;
        fn_8037AEBC(executing);
    } else {
        fn_8037A788();
    }
}

static void fn_8037AEBC(DVDCommandBlock *block) {
    fn_80379054(&BB2, (sizeof(DVDBB2) + 31) & ~31, 0x420, fn_8037AFF0);
}

static void fn_8037AEF4(u32 intType) {
    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_DE) {
        stateError(DVD_INTERNALERROR_NO_DRIVE);
        return;
    }

    NumInternalRetry = 0;
    if (CheckCancel(1)) {
        return;
    }
    executing->state = DVD_STATE_WRONG_DISK;
    fn_8037B364();
}

static void fn_8037AFF0(u32 intType) {
    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_TC) {
        NumInternalRetry = 0;
        fn_8037A594();
    } else {
        fn_8037A788();
    }
}

static void fn_8037B0C8(u32 intType) {
    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_TC) {
        NumInternalRetry = 0;
        if (CheckCancel(0)) {
            return;
        }
        executing->state = DVD_STATE_BUSY;
        stateBusy(executing);
    } else {
        fn_8037A788();
    }
}

static void AlarmHandler(OSAlarm *alarm, OSContext *context) {
    DVDReset();
    DCInvalidateRange(&CurrDiskID, sizeof(DVDDiskID));
    LastState = fn_8037B2D0;
    fn_8037B2D0(executing);
}

static void fn_8037B1FC(void) {
    DVDCommandBlock *finished;

    switch (CurrCommand) {
    case DVD_COMMAND_READID:
    case DVD_COMMAND_BSREAD:
    case DVD_COMMAND_AUDIO_BUFFER_CONFIG:
    case DVD_COMMAND_BS_CHANGE_DISK:
        __DVDClearWaitingQueue();
        finished = executing;
        executing = &DummyCommandBlock;
        if (finished->callback) {
            finished->callback(DVD_RESULT_COVER_CLOSED, finished);
        }
        stateReady();
        break;
    default:
        MotorState = 0;
        DVDReset();
        OSCreateAlarm(&ResetAlarm);
        OSSetAlarm(&ResetAlarm, DVDMillisecondsToTicks(1150), AlarmHandler);
        break;
    }
}

static void fn_8037B2D0(DVDCommandBlock *block) {
    fn_803793AC(&CurrDiskID, fn_8037B300);
}

static void fn_8037B300(u32 intType) {
    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }
    if (intType & DVD_INTTYPE_TC) {
        NumInternalRetry = 0;
        fn_8037AD0C();
    } else {
        fn_8037A788();
    }
}

static void fn_8037B364(void) {
    DVDLowWaitCoverClose(cbForStateMotorStopped);
}

static void cbForStateMotorStopped(u32 intType) {
    __DIRegs[1] = 0;
    executing->state = DVD_STATE_COVER_CLOSED;
    fn_8037B1FC();
}

static void stateReady(void) {
    DVDCommandBlock *finished;

    if (!__DVDCheckWaitingQueue()) {
        executing = NULL;
        return;
    }
    if (PauseFlag) {
        PausingFlag = TRUE;
        executing = NULL;
        return;
    }

    executing = __DVDPopWaitingQueue();
    if (FatalErrorFlag) {
        executing->state = DVD_STATE_FATAL_ERROR;
        finished = executing;
        executing = &DummyCommandBlock;
        if (finished->callback) {
            finished->callback(DVD_RESULT_FATAL_ERROR, finished);
        }
        stateReady();
        return;
    }

    CurrCommand = executing->command;
    if (ResumeFromHere) {
        switch (ResumeFromHere) {
        case 2:
            executing->state = DVD_STATE_RETRY;
            fn_8037B364();
            break;
        case 3:
            executing->state = DVD_STATE_NO_DISK;
            fn_8037B364();
            break;
        case 4:
            executing->state = DVD_STATE_COVER_OPEN;
            fn_8037B364();
            break;
        case 1:
        case 6:
        case 7:
            executing->state = DVD_STATE_COVER_CLOSED;
            fn_8037B1FC();
            break;
        case 5:
            stateError(CancelLastError);
            break;
        }
        ResumeFromHere = 0;
    } else if (MotorState == 0) {
        executing->state = DVD_STATE_BUSY;
        stateBusy(executing);
    } else {
        fn_8037B1FC();
    }
}

static void stateBusy(DVDCommandBlock *block) {
    DVDCommandBlock *finished;

    LastState = stateBusy;
    switch (block->command) {
    case DVD_COMMAND_READID:
        __DIRegs[1] = __DIRegs[1];
        block->currTransferSize = sizeof(DVDDiskID);
        fn_803793AC(block->addr, fn_8037BAA0);
        break;
    case DVD_COMMAND_READ:
    case DVD_COMMAND_BSREAD:
        if (block->length == 0) {
            finished = executing;
            executing = &DummyCommandBlock;
            finished->state = DVD_STATE_END;
            if (finished->callback) {
                finished->callback(DVD_RESULT_GOOD, finished);
            }
            stateReady();
        } else {
            __DIRegs[1] = __DIRegs[1];
            block->currTransferSize = (block->length - block->transferredSize > 0x80000)
                                          ? 0x80000
                                          : (block->length - block->transferredSize);
            fn_80379054((u8 *)block->addr + block->transferredSize, block->currTransferSize,
                        block->offset + block->transferredSize, fn_8037BAA0);
        }
        break;
    case DVD_COMMAND_SEEK:
        __DIRegs[1] = __DIRegs[1];
        fn_803792EC(block->offset, fn_8037BAA0);
        break;
    case DVD_COMMAND_CHANGE_DISK:
        DVDLowStopMotor(fn_8037BAA0);
        break;
    case DVD_COMMAND_BS_CHANGE_DISK:
        DVDLowStopMotor(fn_8037BAA0);
        break;
    case DVD_COMMAND_INITSTREAM:
        __DIRegs[1] = __DIRegs[1];
        if (AutoFinishing) {
            executing->currTransferSize = 0;
            fn_8037969C(0, fn_8037BAA0);
        } else {
            executing->currTransferSize = 1;
            fn_80379604(0, block->length, block->offset, fn_8037BAA0);
        }
        break;
    case DVD_COMMAND_CANCELSTREAM:
        __DIRegs[1] = __DIRegs[1];
        fn_80379604(0x10000, 0, 0, fn_8037BAA0);
        break;
    case DVD_COMMAND_STOP_STREAM_AT_END:
        __DIRegs[1] = __DIRegs[1];
        AutoFinishing = TRUE;
        fn_80379604(0, 0, 0, fn_8037BAA0);
        break;
    case DVD_COMMAND_REQUEST_AUDIO_ERROR:
        __DIRegs[1] = __DIRegs[1];
        fn_8037969C(0, fn_8037BAA0);
        break;
    case DVD_COMMAND_REQUEST_PLAY_ADDR:
        __DIRegs[1] = __DIRegs[1];
        fn_8037969C(0x10000, fn_8037BAA0);
        break;
    case DVD_COMMAND_REQUEST_START_ADDR:
        __DIRegs[1] = __DIRegs[1];
        fn_8037969C(0x20000, fn_8037BAA0);
        break;
    case DVD_COMMAND_REQUEST_LENGTH:
        __DIRegs[1] = __DIRegs[1];
        fn_8037969C(0x30000, fn_8037BAA0);
        break;
    case DVD_COMMAND_AUDIO_BUFFER_CONFIG:
        __DIRegs[1] = __DIRegs[1];
        fn_80379728((BOOL)block->offset, block->length, fn_8037BAA0);
        break;
    case DVD_COMMAND_INQUIRY:
        __DIRegs[1] = __DIRegs[1];
        block->currTransferSize = 0x20;
        fn_80379568(block->addr, fn_8037BAA0);
        break;
    case DVD_COMMAND_STOP_MOTOR:
        __DIRegs[1] = __DIRegs[1];
        DVDLowStopMotor(fn_8037BAA0);
        break;
    default:
        checkOptionalCommand(block, fn_8037BAA0);
        break;
    }
}

static u32 ImmCommand[] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};

/* Commands that return a 32-bit result in the immediate register. */
static inline BOOL IsImmCommandWithResult(u32 command) {
    u32 i;

    if (command == DVD_COMMAND_REQUEST_AUDIO_ERROR || command == DVD_COMMAND_REQUEST_PLAY_ADDR ||
        command == DVD_COMMAND_REQUEST_START_ADDR || command == DVD_COMMAND_REQUEST_LENGTH) {
        return TRUE;
    }
    for (i = 0; i < sizeof(ImmCommand) / sizeof(ImmCommand[0]); i++) {
        if (command == ImmCommand[i]) {
            return TRUE;
        }
    }
    return FALSE;
}

static u32 DmaCommand[] = {0xFFFFFFFF};

/* Commands that move data by DMA. */
static inline BOOL IsDmaCommand(u32 command) {
    u32 i;

    if (command == DVD_COMMAND_READ || command == DVD_COMMAND_BSREAD ||
        command == DVD_COMMAND_READID || command == DVD_COMMAND_INQUIRY) {
        return TRUE;
    }
    for (i = 0; i < sizeof(DmaCommand) / sizeof(DmaCommand[0]); i++) {
        if (command == DmaCommand[i]) {
            return TRUE;
        }
    }
    return FALSE;
}

static void fn_8037BAA0(u32 intType) {
    DVDCommandBlock *finished;
    s32 result;

    if (intType == DVD_INTTYPE_TIMEOUT) {
        fn_8037A754();
        return;
    }

    if (CurrCommand == DVD_COMMAND_CHANGE_DISK || CurrCommand == DVD_COMMAND_BS_CHANGE_DISK) {
        if (intType & DVD_INTTYPE_DE) {
            stateError(DVD_INTERNALERROR_NO_DRIVE);
            return;
        }
        NumInternalRetry = 0;
        if (CurrCommand == DVD_COMMAND_BS_CHANGE_DISK) {
            ResetRequired = TRUE;
        }
        if (CheckCancel(7)) {
            return;
        }
        executing->state = DVD_STATE_MOTOR_STOPPED;
        fn_8037B364();
        return;
    }

    if (IsDmaCommand(CurrCommand)) {
        executing->transferredSize += executing->currTransferSize - __DIRegs[6];
    }

    if (intType & DVD_INTTYPE_BRK) {
        Canceling = FALSE;
        finished = executing;
        executing = &DummyCommandBlock;
        finished->state = DVD_STATE_CANCELED;
        if (finished->callback) {
            finished->callback(DVD_RESULT_CANCELED, finished);
        }
        if (CancelCallback) {
            CancelCallback(DVD_RESULT_GOOD, finished);
        }
        stateReady();
        return;
    }

    if (intType & DVD_INTTYPE_TC) {
        NumInternalRetry = 0;
        if (CurrCommand == DVD_COMMAND_STOP_MOTOR) {
            MotorState = 1;
            finished = executing;
            executing = &DummyCommandBlock;
            finished->state = DVD_STATE_END;
            if (finished->callback) {
                finished->callback(DVD_RESULT_GOOD, finished);
            }
            stateReady();
            return;
        }
        if (CheckCancel(0)) {
            return;
        }

        if (IsDmaCommand(CurrCommand)) {
            if (executing->transferredSize != executing->length) {
                stateBusy(executing);
                return;
            }
            finished = executing;
            executing = &DummyCommandBlock;
            finished->state = DVD_STATE_END;
            if (finished->callback) {
                finished->callback((s32)finished->transferredSize, finished);
            }
            stateReady();
        } else if (IsImmCommandWithResult(CurrCommand)) {
            if (CurrCommand == DVD_COMMAND_REQUEST_START_ADDR ||
                CurrCommand == DVD_COMMAND_REQUEST_PLAY_ADDR) {
                result = (s32)(__DIRegs[8] << 2);
            } else {
                result = (s32)__DIRegs[8];
            }
            finished = executing;
            executing = &DummyCommandBlock;
            finished->state = DVD_STATE_END;
            if (finished->callback) {
                finished->callback(result, finished);
            }
            stateReady();
        } else if (CurrCommand == DVD_COMMAND_INITSTREAM) {
            if (executing->currTransferSize == 0) {
                if (__DIRegs[8] & 1) {
                    finished = executing;
                    executing = &DummyCommandBlock;
                    finished->state = DVD_STATE_IGNORED;
                    if (finished->callback) {
                        finished->callback(DVD_RESULT_IGNORED, finished);
                    }
                    stateReady();
                } else {
                    AutoFinishing = FALSE;
                    executing->currTransferSize = 1;
                    fn_80379604(0, executing->length, executing->offset, fn_8037BAA0);
                }
            } else {
                finished = executing;
                executing = &DummyCommandBlock;
                finished->state = DVD_STATE_END;
                if (finished->callback) {
                    finished->callback(DVD_RESULT_GOOD, finished);
                }
                stateReady();
            }
        } else {
            finished = executing;
            executing = &DummyCommandBlock;
            finished->state = DVD_STATE_END;
            if (finished->callback) {
                finished->callback(DVD_RESULT_GOOD, finished);
            }
            stateReady();
        }
    } else {
        if (CurrCommand == DVD_COMMAND_INQUIRY) {
            stateError(DVD_INTERNALERROR_NO_DRIVE);
            return;
        }
        if ((CurrCommand == DVD_COMMAND_READ || CurrCommand == DVD_COMMAND_BSREAD ||
             CurrCommand == DVD_COMMAND_READID || CurrCommand == DVD_COMMAND_INQUIRY) &&
            executing->transferredSize == executing->length) {
            if (CheckCancel(0)) {
                return;
            }
            finished = executing;
            executing = &DummyCommandBlock;
            finished->state = DVD_STATE_END;
            if (finished->callback) {
                finished->callback((s32)finished->transferredSize, finished);
            }
            stateReady();
            return;
        }
        fn_8037A788();
    }
}

static inline BOOL issueCommand(s32 prio, DVDCommandBlock *block) {
    BOOL level;
    BOOL result;

    if (autoInvalidation &&
        (block->command == DVD_COMMAND_READ || block->command == DVD_COMMAND_BSREAD ||
         block->command == DVD_COMMAND_READID || block->command == DVD_COMMAND_INQUIRY)) {
        DCInvalidateRange(block->addr, block->length);
    }

    level = OSDisableInterrupts();
    block->state = DVD_STATE_WAITING;
    result = __DVDPushWaitingQueue(prio, block);
    if (executing == NULL && PauseFlag == FALSE) {
        stateReady();
    }
    OSRestoreInterrupts(level);
    return result;
}

BOOL fn_8037C0F8(DVDCommandBlock *block, void *addr, s32 length, s32 offset, DVDCBCallback callback,
                 s32 prio) {
    BOOL idle;

    block->command = DVD_COMMAND_READ;
    block->addr = addr;
    block->length = length;
    block->offset = offset;
    block->transferredSize = 0;
    block->callback = callback;
    idle = issueCommand(prio, block);
    return idle;
}

BOOL fn_8037C1D4(DVDCommandBlock *block, void *addr, s32 length, s32 offset, DVDCBCallback callback) {
    BOOL idle;

    block->command = DVD_COMMAND_BSREAD;
    block->addr = addr;
    block->length = length;
    block->offset = offset;
    block->transferredSize = 0;
    block->callback = callback;
    idle = issueCommand(2, block);
    return idle;
}

BOOL DVDReadDiskID(DVDCommandBlock *block, DVDDiskID *diskID, DVDCBCallback callback) {
    BOOL idle;

    block->command = DVD_COMMAND_READID;
    block->addr = diskID;
    block->length = sizeof(DVDDiskID);
    block->offset = 0;
    block->transferredSize = 0;
    block->callback = callback;
    idle = issueCommand(2, block);
    return idle;
}

BOOL fn_8037C378(DVDCommandBlock *block, DVDCBCallback callback) {
    BOOL idle;

    block->command = DVD_COMMAND_CANCELSTREAM;
    block->callback = callback;
    idle = issueCommand(1, block);
    return idle;
}

/* Dead-stripped; kept for its message, which stays in the data. */
BOOL DVDChangeDiskAsync(DVDCommandBlock *block, DVDDiskID *id, DVDCBCallback callback) {
    BOOL idle;

    if (id->company[0] == '\0' && id->company[1] == '\0') {
        OSPanic(__FILE__, 2215, "DVDChangeDiskAsync(): You can't specify NULL to company name.  \n");
    }
    block->command = DVD_COMMAND_CHANGE_DISK;
    block->id = id;
    block->callback = callback;
    DCInvalidateRange(bootInfo->FSTLocation, bootInfo->FSTMaxLength);
    idle = issueCommand(2, block);
    return idle;
}

BOOL DVDInquiryAsync(DVDCommandBlock *block, void *info, DVDCBCallback callback) {
    BOOL idle;

    block->command = DVD_COMMAND_INQUIRY;
    block->addr = info;
    block->length = 0x20;
    block->transferredSize = 0;
    block->callback = callback;
    idle = issueCommand(2, block);
    return idle;
}

void DVDReset(void) {
    DVDLowReset();
    __DIRegs[0] = 0x2A;
    __DIRegs[1] = __DIRegs[1];
    ResetRequired = FALSE;
    ResumeFromHere = 0;
}

s32 fn_8037C548(const DVDCommandBlock *block) {
    BOOL enabled;
    s32 retVal;

    enabled = OSDisableInterrupts();
    if (block->state == DVD_STATE_COVER_CLOSED) {
        retVal = DVD_STATE_BUSY;
    } else {
        retVal = block->state;
    }
    OSRestoreInterrupts(enabled);
    return retVal;
}

s32 DVDGetDriveStatus(void) {
    BOOL enabled;
    s32 retVal;

    enabled = OSDisableInterrupts();
    if (FatalErrorFlag) {
        retVal = DVD_STATE_FATAL_ERROR;
    } else if (PausingFlag) {
        retVal = DVD_STATE_PAUSING;
    } else if (executing == NULL) {
        retVal = DVD_STATE_END;
    } else if (executing == &DummyCommandBlock) {
        retVal = DVD_STATE_END;
    } else {
        retVal = fn_8037C548(executing);
    }
    OSRestoreInterrupts(enabled);
    return retVal;
}

BOOL fn_8037C640(BOOL autoInval) {
    BOOL prev = autoInvalidation;

    autoInvalidation = autoInval;
    return prev;
}

static inline void DVDPause(void) {
    BOOL level = OSDisableInterrupts();

    PauseFlag = TRUE;
    if (executing == NULL) {
        PausingFlag = TRUE;
    }
    OSRestoreInterrupts(level);
}

void fn_8037C650(void) {
    BOOL level = OSDisableInterrupts();

    PauseFlag = FALSE;
    if (PausingFlag) {
        PausingFlag = FALSE;
        stateReady();
    }
    OSRestoreInterrupts(level);
}

BOOL fn_8037C6A0(DVDCommandBlock *block, DVDCBCallback callback) {
    BOOL enabled;
    DVDLowCallback old;
    s32 state;

    enabled = OSDisableInterrupts();
    state = block->state;
    switch (state) {
    case DVD_STATE_FATAL_ERROR:
    case DVD_STATE_END:
    case DVD_STATE_CANCELED:
        if (callback) {
            callback(DVD_RESULT_GOOD, block);
        }
        break;
    case DVD_STATE_BUSY:
        if (Canceling) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
        Canceling = TRUE;
        CancelCallback = callback;
        if (block->command == DVD_COMMAND_BSREAD || block->command == DVD_COMMAND_READ) {
            fn_80379880();
        }
        break;
    case DVD_STATE_WAITING:
        fn_8037CDD8(block);
        block->state = DVD_STATE_CANCELED;
        if (block->callback) {
            block->callback(DVD_RESULT_CANCELED, block);
        }
        if (callback) {
            callback(DVD_RESULT_GOOD, block);
        }
        break;
    case DVD_STATE_COVER_CLOSED:
        switch (block->command) {
        case DVD_COMMAND_READID:
        case DVD_COMMAND_BSREAD:
        case DVD_COMMAND_AUDIO_BUFFER_CONFIG:
        case DVD_COMMAND_BS_CHANGE_DISK:
            if (callback) {
                callback(DVD_RESULT_GOOD, block);
            }
            break;
        default:
            if (Canceling) {
                OSRestoreInterrupts(enabled);
                return FALSE;
            }
            Canceling = TRUE;
            CancelCallback = callback;
            break;
        }
        break;
    case DVD_STATE_NO_DISK:
    case DVD_STATE_COVER_OPEN:
    case DVD_STATE_WRONG_DISK:
    case DVD_STATE_MOTOR_STOPPED:
    case DVD_STATE_RETRY:
        old = fn_80379894();
        if (old != cbForStateMotorStopped) {
            OSRestoreInterrupts(enabled);
            return FALSE;
        }
        if (block->state == DVD_STATE_NO_DISK) {
            ResumeFromHere = 3;
        }
        if (block->state == DVD_STATE_COVER_OPEN) {
            ResumeFromHere = 4;
        }
        if (block->state == DVD_STATE_WRONG_DISK) {
            ResumeFromHere = 1;
        }
        if (block->state == DVD_STATE_RETRY) {
            ResumeFromHere = 2;
        }
        if (block->state == DVD_STATE_MOTOR_STOPPED) {
            ResumeFromHere = 7;
        }
        executing = &DummyCommandBlock;
        block->state = DVD_STATE_CANCELED;
        if (block->callback) {
            block->callback(DVD_RESULT_CANCELED, block);
        }
        if (callback) {
            callback(DVD_RESULT_GOOD, block);
        }
        stateReady();
        break;
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

static void fn_8037C9C8(s32 result, DVDCommandBlock *block);

s32 fn_8037C91C(DVDCommandBlock *block) {
    BOOL result;
    s32 state;
    BOOL enabled;

    result = fn_8037C6A0(block, fn_8037C9C8);
    if (result == FALSE) {
        return -1;
    }

    enabled = OSDisableInterrupts();
    while (TRUE) {
        state = block->state;
        if (state == DVD_STATE_END || state == DVD_STATE_FATAL_ERROR || state == DVD_STATE_CANCELED) {
            break;
        }
        if (state == DVD_STATE_COVER_CLOSED) {
            /* The command reuses the state's variable: in a variable of its own it lands in r0,
             * where the compiler cannot test BSREAD and READID as one range (retail does). The
             * last two tests are unsigned in retail, hence the casts. */
            state = (s32)block->command;
            if (state == DVD_COMMAND_BSREAD || state == DVD_COMMAND_READID ||
                (u32)state == DVD_COMMAND_AUDIO_BUFFER_CONFIG ||
                (u32)state == DVD_COMMAND_BS_CHANGE_DISK) {
                break;
            }
        }
        OSSleepThread(&__DVDThreadQueue);
    }
    OSRestoreInterrupts(enabled);
    return 0;
}

static void fn_8037C9C8(s32 result, DVDCommandBlock *block) {
    OSWakeupThread(&__DVDThreadQueue);
}

static inline BOOL DVDCancelAllAsync(DVDCBCallback callback) {
    BOOL enabled;
    DVDCommandBlock *p;
    BOOL retVal;

    enabled = OSDisableInterrupts();
    DVDPause();
    while ((p = __DVDPopWaitingQueue()) != NULL) {
        fn_8037C6A0(p, NULL);
    }
    if (executing) {
        retVal = fn_8037C6A0(executing, callback);
    } else {
        retVal = TRUE;
        if (callback) {
            callback(DVD_RESULT_GOOD, NULL);
        }
    }
    fn_8037C650();
    OSRestoreInterrupts(enabled);
    return retVal;
}

DVDDiskID *fn_8037C9EC(void) {
    return (DVDDiskID *)OSPhysicalToCached(0);
}

BOOL fn_8037C9F4(void) {
    BOOL enabled;
    s32 retVal;
    s32 state;
    u32 coverReg;

    enabled = OSDisableInterrupts();
    if (FatalErrorFlag) {
        state = DVD_STATE_FATAL_ERROR;
    } else if (PausingFlag) {
        state = DVD_STATE_PAUSING;
    } else if (executing == NULL) {
        state = DVD_STATE_END;
    } else if (executing == &DummyCommandBlock) {
        state = DVD_STATE_END;
    } else {
        state = executing->state;
    }

    switch (state) {
    case DVD_STATE_BUSY:
    case DVD_STATE_IGNORED:
    case DVD_STATE_CANCELED:
    case DVD_STATE_WAITING:
        retVal = TRUE;
        break;
    case DVD_STATE_FATAL_ERROR:
    case DVD_STATE_RETRY:
    case DVD_STATE_MOTOR_STOPPED:
    case DVD_STATE_COVER_CLOSED:
    case DVD_STATE_NO_DISK:
    case DVD_STATE_COVER_OPEN:
    case DVD_STATE_WRONG_DISK:
        retVal = FALSE;
        break;
    case DVD_STATE_END:
    case DVD_STATE_PAUSING:
        coverReg = __DIRegs[1];
        if (((coverReg >> 2) & 1) || (coverReg & 1)) {
            retVal = FALSE;
        } else if (ResumeFromHere) {
            retVal = FALSE;
        } else {
            retVal = TRUE;
        }
        break;
    }
    OSRestoreInterrupts(enabled);
    return retVal;
}

void fn_8037CAEC(DVDCBCallback callback) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    __DVDClearWaitingQueue();
    if (Canceling) {
        CancelCallback = callback;
    } else {
        if (executing) {
            executing->callback = NULL;
        }
        DVDCancelAllAsync(callback);
    }
    OSRestoreInterrupts(enabled);
}

BOOL __DVDTestAlarm(OSAlarm *alarm) {
    if (alarm == &ResetAlarm) {
        return TRUE;
    }
    return fn_803798F4(alarm);
}

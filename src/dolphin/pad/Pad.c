/* Dolphin SDK PAD: GameCube controllers on the serial interface. A controller is "reset" (its
 * type read, its origin fetched or recalibrated, polling enabled) one channel at a time from
 * chained SI callbacks; PADRead turns the polled response words into PADStatus through the
 * MakeStatus routine of the selected controller spec, relative to the stored origin.
 *
 * Channel sets are bit masks with channel 0 in the top bit. The SI functions keep dtk's names
 * (nothing in the binary names them); the macros below give them the public SDK names. */
#include <dolphin/os.h>

/* Only the functions marked inline are inlined here: PADInit, PADRead and the reset function
 * call PADReset and PADRecalibrate out of line although both are defined above them. */
#pragma auto_inline off

typedef struct PADStatus {
    u16 button;
    s8 stickX;
    s8 stickY;
    s8 substickX;
    s8 substickY;
    u8 triggerLeft;
    u8 triggerRight;
    u8 analogA;
    u8 analogB;
    s8 err;
} PADStatus;

#define PAD_ERR_NONE 0
#define PAD_ERR_NO_CONTROLLER -1
#define PAD_ERR_NOT_READY -2
#define PAD_ERR_TRANSFER -3

#define PAD_CHAN0_BIT 0x80000000
#define PAD_MAX_CONTROLLERS 4
#define RES_WIRELESS_LITE 0x40000

#define SI_ERROR_NO_RESPONSE 0x0008
#define SI_GC_CONTROLLER 0x09000000
#define SI_GC_NOMOTOR 0x20000000
#define SI_GC_STANDARD 0x01000000
#define SI_GC_WIRELESS 0x80000000
#define SI_TYPE_MASK 0x18000000
#define SI_TYPE_GC 0x08000000
#define SI_WIRELESS_CONT_MASK 0x00080000
#define SI_WIRELESS_FIX_ID 0x00100000
#define SI_WIRELESS_IR 0x04000000
#define SI_WIRELESS_LITE 0x00040000
#define SI_WIRELESS_STATE 0x02000000
#define SI_GC_WIRELESS_RECEIVED 0x40000000

typedef void (*SICallback)(s32 chan, u32 error, OSContext *context);
typedef void (*SITypeAndStatusCallback)(s32 chan, u32 type);
typedef void (*PADSamplingCallback)(void);
typedef void (*MakeStatusFunc)(s32 chan, PADStatus *status, u32 data[2]);
typedef void (*__OSInterruptHandler)(s16 interrupt, OSContext *context);

u32 SIGetType(s32 chan);
BOOL SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes,
                SICallback callback, OSTime delay);
BOOL fn_803B49F8(void);
#define SIBusy fn_803B49F8
BOOL fn_803B4A18(s32 chan);
#define SIIsChanBusy fn_803B4A18
BOOL fn_803B512C(__OSInterruptHandler handler);
#define SIRegisterPollingHandler fn_803B512C
BOOL fn_803B51F8(__OSInterruptHandler handler);
#define SIUnregisterPollingHandler fn_803B51F8
u32 fn_803B55AC(s32 chan);
#define SIGetStatus fn_803B55AC
void fn_803B5628(s32 chan, u32 command);
#define SISetCommand fn_803B5628
void fn_803B563C(void);
#define SITransferCommands fn_803B563C
u32 fn_803B56B8(u32 poll);
#define SIEnablePolling fn_803B56B8
u32 fn_803B5754(u32 poll);
#define SIDisablePolling fn_803B5754
BOOL fn_803B5894(s32 chan, void *data);
#define SIGetResponse fn_803B5894
u32 fn_803B5FAC(s32 chan, SITypeAndStatusCallback callback);
#define SIGetTypeAsync fn_803B5FAC
void fn_803B61CC(void);
#define SIRefreshSamplingRate fn_803B61CC

void OSRegisterVersion(const char *id);
void OSSetWirelessID(s32 chan, u16 id);
void *memset(void *dest, int c, u32 n);

/* Low memory: wireless pad fix mode and a game-code dependent flag byte. */
u16 __OSWirelessPadFixMode : 0x800030E0;
u8 __gUnknown800030E3 : 0x800030E3;
extern u32 lbl_805F1348; /* set by another library when the pad fix is wanted */

static void fn_8037F7B4(s32 chan, u32 error, OSContext *context);
static void fn_8037F878(s32 chan, u32 error, OSContext *context);
static void fn_8037F944(s32 chan, u32 error, OSContext *context);
static void fn_8037FA1C(s32 chan, u32 type);
static void fn_8037FD48(s32 chan, u32 type);
BOOL fn_8037FE88(u32 mask);
BOOL fn_8037FF98(u32 mask);
void fn_80380680(u32 spec);
static void fn_803806E0(s32 chan, PADStatus *status, u32 data[2]);
static void fn_80380854(s32 chan, PADStatus *status, u32 data[2]);
static void fn_803809C8(s32 chan, PADStatus *status, u32 data[2]);
static BOOL fn_80380E38(BOOL final);
static void fn_80380EF4(s16 interrupt, OSContext *context);
PADSamplingCallback fn_80380F54(PADSamplingCallback callback);

const char *__PADVersion = "<< Dolphin SDK - PAD\trelease build: Apr  5 2004 04:14:49 (0x2301) >>";

static u32 Type[PAD_MAX_CONTROLLERS];
static PADStatus Origin[PAD_MAX_CONTROLLERS];
static u32 CmdProbeDevice[PAD_MAX_CONTROLLERS];

static BOOL Initialized;
static u32 EnabledBits;
static u32 ResettingBits;
static u32 RecalibrateBits;
static u32 WaitingBits;
static u32 CheckingBits;
static u32 PendingBits;
static u32 BarrelBits;

static s32 ResettingChan = 32;
static u32 XPatchBits = 0xF0000000;
static u32 AnalogMode = 0x00000300;
static u32 Spec = 5;
static MakeStatusFunc MakeStatus = fn_803809C8;
static u32 CmdReadOrigin = 0x41 << 24;
static u32 CmdCalibrate = 0x42 << 24;

static OSResetFunctionInfo ResetFunctionInfo = {fn_80380E38, 127, NULL, NULL};

static PADSamplingCallback SamplingCallback;

static void fn_8037F610(s32 chan) {
    PADStatus *origin;
    u32 chanBit = PAD_CHAN0_BIT >> chan;

    origin = &Origin[chan];
    switch (AnalogMode & 0x00000700) {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x00000100:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x00000200:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        break;
    case 0x00000300:
        break;
    case 0x00000400:
        break;
    }

    origin->stickX -= 128;
    origin->stickY -= 128;
    origin->substickX -= 128;
    origin->substickY -= 128;

    if (XPatchBits & chanBit) {
        if (64 < origin->stickX && (SIGetType(chan) & 0xFFFF0000) == SI_GC_CONTROLLER) {
            origin->stickX = 0;
        }
    }
}

static inline void PADEnable(s32 chan) {
    u32 cmd;
    u32 chanBit;
    u32 data[2];

    chanBit = PAD_CHAN0_BIT >> chan;
    EnabledBits |= chanBit;
    SIGetResponse(chan, data);
    cmd = (0x40 << 16) | AnalogMode;
    SISetCommand(chan, cmd);
    SIEnablePolling(EnabledBits);
}

static inline void PADDisable(s32 chan) {
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    chanBit = PAD_CHAN0_BIT >> chan;
    SIDisablePolling(chanBit);
    EnabledBits &= ~chanBit;
    WaitingBits &= ~chanBit;
    CheckingBits &= ~chanBit;
    PendingBits &= ~chanBit;
    BarrelBits &= ~chanBit;
    OSSetWirelessID(chan, 0);
    OSRestoreInterrupts(enabled);
}

/* Starts resetting the next channel waiting in ResettingBits, if any. */
static inline void DoReset(void) {
    u32 chanBit;

    ResettingChan = __cntlzw(ResettingBits);
    if (ResettingChan == 32) {
        return;
    }
    chanBit = PAD_CHAN0_BIT >> ResettingChan;
    ResettingBits &= ~chanBit;
    memset(&Origin[ResettingChan], 0, sizeof(PADStatus));
    SIGetTypeAsync(ResettingChan, fn_8037FA1C);
}

static void fn_8037F7B4(s32 chan, u32 error, OSContext *context) {
    if (!(error & 0x0F)) {
        fn_8037F610(ResettingChan);
        PADEnable(ResettingChan);
    }
    DoReset();
}

static void fn_8037F878(s32 chan, u32 error, OSContext *context) {
    u32 chanBit = PAD_CHAN0_BIT >> chan;

    if (!(EnabledBits & chanBit)) {
        return;
    }
    if (!(error & 0x0F)) {
        fn_8037F610(chan);
    }
    if (error & SI_ERROR_NO_RESPONSE) {
        PADDisable(chan);
    }
}

static void fn_8037F944(s32 chan, u32 error, OSContext *context) {
    if (!(error & 0x0F)) {
        PADEnable(ResettingChan);
        WaitingBits |= PAD_CHAN0_BIT >> ResettingChan;
    }
    DoReset();
}

static void fn_8037FA1C(s32 chan, u32 type) {
    u32 chanBit;
    u32 recalibrate;
    BOOL rc = TRUE;
    u32 error;

    chanBit = PAD_CHAN0_BIT >> ResettingChan;
    error = type & 0xFF;
    recalibrate = RecalibrateBits & chanBit;
    RecalibrateBits &= ~chanBit;

    if (error & 0x0F) {
        DoReset();
        return;
    }

    type &= ~0xFF;
    Type[ResettingChan] = type;

    if ((type & SI_TYPE_MASK) != SI_TYPE_GC || !(type & SI_GC_STANDARD)) {
        DoReset();
        return;
    }

    if (Spec < 2) {
        PADEnable(ResettingChan);
        DoReset();
        return;
    }

    if (!(type & SI_GC_WIRELESS) || (type & SI_WIRELESS_IR)) {
        if (recalibrate) {
            rc = SITransfer(ResettingChan, &CmdCalibrate, 3, &Origin[ResettingChan], 10, fn_8037F7B4,
                            0);
        } else {
            rc = SITransfer(ResettingChan, &CmdReadOrigin, 1, &Origin[ResettingChan], 10,
                            fn_8037F7B4, 0);
        }
    } else if ((type & SI_WIRELESS_FIX_ID) && !(type & SI_WIRELESS_CONT_MASK) &&
               !(type & SI_WIRELESS_LITE)) {
        if (type & SI_GC_WIRELESS_RECEIVED) {
            rc = SITransfer(ResettingChan, &CmdReadOrigin, 1, &Origin[ResettingChan], 10,
                            fn_8037F7B4, 0);
        } else {
            rc = SITransfer(ResettingChan, &CmdProbeDevice[ResettingChan], 3,
                            &Origin[ResettingChan], 8, fn_8037F944, 0);
        }
    }

    if (!rc) {
        PendingBits |= chanBit;
        DoReset();
        return;
    }
}

static void fn_8037FD48(s32 chan, u32 type) {
    u32 error;
    u32 chanBit;

    chanBit = PAD_CHAN0_BIT >> chan;
    if (!(EnabledBits & chanBit)) {
        return;
    }

    error = type & 0xFF;
    type &= ~0xFF;
    WaitingBits &= ~chanBit;
    CheckingBits &= ~chanBit;

    if (!(error & 0x0F) && (type & SI_GC_WIRELESS) && (type & SI_WIRELESS_FIX_ID) &&
        (type & SI_GC_WIRELESS_RECEIVED) && !(type & SI_WIRELESS_IR) &&
        !(type & SI_WIRELESS_CONT_MASK) && !(type & SI_WIRELESS_LITE)) {
        SITransfer(chan, &CmdReadOrigin, 1, &Origin[chan], 10, fn_8037F878, 0);
    } else {
        PADDisable(chan);
    }
}

BOOL fn_8037FE88(u32 mask) {
    BOOL enabled;
    u32 diableBits;

    enabled = OSDisableInterrupts();
    mask |= PendingBits;
    PendingBits = 0;
    mask &= ~(WaitingBits | CheckingBits);
    ResettingBits |= mask;
    diableBits = ResettingBits & EnabledBits;
    EnabledBits &= ~mask;
    BarrelBits &= ~mask;
    if (Spec == 4) {
        RecalibrateBits |= mask;
    }
    SIDisablePolling(diableBits);
    if (ResettingChan == 32) {
        DoReset();
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL fn_8037FF98(u32 mask) {
    BOOL enabled;
    u32 diableBits;

    enabled = OSDisableInterrupts();
    mask |= PendingBits;
    PendingBits = 0;
    mask &= ~(WaitingBits | CheckingBits);
    ResettingBits |= mask;
    diableBits = ResettingBits & EnabledBits;
    EnabledBits &= ~mask;
    BarrelBits &= ~mask;
    if (!(__gUnknown800030E3 & 0x40)) {
        RecalibrateBits |= mask;
    }
    SIDisablePolling(diableBits);
    if (ResettingChan == 32) {
        DoReset();
    }
    OSRestoreInterrupts(enabled);
    return TRUE;
}

u32 __PADSpec;

BOOL fn_803800AC(void) {
    s32 chan;

    if (Initialized) {
        return TRUE;
    }
    OSRegisterVersion(__PADVersion);
    if (__PADSpec) {
        fn_80380680(__PADSpec);
    }
    Initialized = TRUE;

    if (lbl_805F1348 != 0) {
        OSTime time = OSGetTime();

        __OSWirelessPadFixMode = (u16)((((time) & 0xFFFF) + ((time >> 16) & 0xFFFF) +
                                        ((time >> 32) & 0xFFFF) + ((time >> 48) & 0xFFFF)) &
                                       0x3FFFu);
        RecalibrateBits = PAD_CHAN0_BIT | 0x40000000 | 0x20000000 | 0x10000000;
    }

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; ++chan) {
        CmdProbeDevice[chan] = (0x4D << 24) | (chan << 22) | ((__OSWirelessPadFixMode & 0x3FFFu) << 8);
    }

    SIRefreshSamplingRate();
    OSRegisterResetFunction(&ResetFunctionInfo);
    return fn_8037FE88(PAD_CHAN0_BIT | 0x40000000 | 0x20000000 | 0x10000000);
}

u32 fn_803801FC(PADStatus *status) {
    BOOL enabled;
    s32 chan;
    u32 data[2];
    u32 unused[1]; /* retail keeps one more word of locals under data: never read or written */
    u32 chanBit;
    u32 sr;
    u32 motor;

    enabled = OSDisableInterrupts();
    motor = 0;
    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++, status++) {
        chanBit = PAD_CHAN0_BIT >> chan;

        if (PendingBits & chanBit) {
            fn_8037FE88(0);
            status->err = PAD_ERR_NOT_READY;
            memset(status, 0, 10);
            continue;
        }
        if ((ResettingBits & chanBit) || ResettingChan == chan) {
            status->err = PAD_ERR_NOT_READY;
            memset(status, 0, 10);
            continue;
        }
        if (!(EnabledBits & chanBit)) {
            status->err = PAD_ERR_NO_CONTROLLER;
            memset(status, 0, 10);
            continue;
        }
        if (SIIsChanBusy(chan)) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            continue;
        }

        sr = SIGetStatus(chan);
        if (sr & SI_ERROR_NO_RESPONSE) {
            SIGetResponse(chan, data);
            if (WaitingBits & chanBit) {
                status->err = PAD_ERR_NONE;
                memset(status, 0, 10);
                if (!(CheckingBits & chanBit)) {
                    CheckingBits |= chanBit;
                    SIGetTypeAsync(chan, fn_8037FD48);
                }
                continue;
            }
            PADDisable(chan);
            status->err = PAD_ERR_NO_CONTROLLER;
            memset(status, 0, 10);
            continue;
        }

        if (!(SIGetType(chan) & SI_GC_NOMOTOR)) {
            motor |= chanBit;
        }
        if (!SIGetResponse(chan, data)) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            continue;
        }
        if (data[0] & 0x80000000) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            continue;
        }

        MakeStatus(chan, status, data);
        if (status->button & 0x2000) {
            /* The controller asks for its origin to be read again. */
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, 10);
            SITransfer(chan, &CmdReadOrigin, 1, &Origin[chan], 10, fn_8037F878, 0);
            continue;
        }
        status->err = PAD_ERR_NONE;
        status->button &= ~0x0080;
    }
    OSRestoreInterrupts(enabled);
    return motor;
}

void fn_803804FC(const u32 *commandArray) {
    BOOL enabled;
    int chan;
    u32 command;
    BOOL commit;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    commit = FALSE;
    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++, commandArray++) {
        chanBit = PAD_CHAN0_BIT >> chan;
        if ((EnabledBits & chanBit) && !(SIGetType(chan) & SI_GC_NOMOTOR)) {
            command = *commandArray;
            if (Spec < 2 && command == 2) {
                command = 0;
            }
            if (__gUnknown800030E3 & 0x20) {
                command = 0;
            }
            SISetCommand(chan, (0x40 << 16) | AnalogMode | (command & (0x00000001 | 0x00000002)));
            commit = TRUE;
        }
    }
    if (commit) {
        SITransferCommands();
    }
    OSRestoreInterrupts(enabled);
}

void fn_803805C8(s32 chan, u32 command) {
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    chanBit = PAD_CHAN0_BIT >> chan;
    if ((EnabledBits & chanBit) && !(SIGetType(chan) & SI_GC_NOMOTOR)) {
        if (Spec < 2 && command == 2) {
            command = 0;
        }
        if (__gUnknown800030E3 & 0x20) {
            command = 0;
        }
        SISetCommand(chan, (0x40 << 16) | AnalogMode | (command & (0x00000001 | 0x00000002)));
        SITransferCommands();
    }
    OSRestoreInterrupts(enabled);
}

void fn_80380680(u32 spec) {
    __PADSpec = 0;
    switch (spec) {
    case 0:
        MakeStatus = fn_803806E0;
        break;
    case 1:
        MakeStatus = fn_80380854;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        MakeStatus = fn_803809C8;
        break;
    }
    Spec = spec;
}

static void fn_803806E0(s32 chan, PADStatus *status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0008) ? 0x0100 : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? 0x0200 : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? 0x0400 : 0;
    status->button |= ((data[0] >> 16) & 0x0001) ? 0x0800 : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? 0x1000 : 0;
    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);
    status->triggerLeft = (u8)(data[0] >> 8 & 0xFF);
    status->triggerRight = (u8)(data[0] & 0xFF);
    status->analogA = 0;
    status->analogB = 0;
    if (170 <= status->triggerLeft) {
        status->button |= 0x0040;
    }
    if (170 <= status->triggerRight) {
        status->button |= 0x0020;
    }
    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

static void fn_80380854(s32 chan, PADStatus *status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0080) ? 0x0100 : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? 0x0200 : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? 0x0400 : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? 0x0800 : 0;
    status->button |= ((data[0] >> 16) & 0x0200) ? 0x1000 : 0;
    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);
    status->triggerLeft = (u8)(data[0] >> 8 & 0xFF);
    status->triggerRight = (u8)(data[0] & 0xFF);
    status->analogA = 0;
    status->analogB = 0;
    if (170 <= status->triggerLeft) {
        status->button |= 0x0040;
    }
    if (170 <= status->triggerRight) {
        status->button |= 0x0020;
    }
    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

/* A stick value relative to its origin, limited so the result still fits a signed byte. */
static inline s8 ClampS8(s8 var, s8 org) {
    if (0 < org) {
        s8 min = (s8)(-128 + org);

        if (var < min) {
            var = min;
        }
    } else if (org < 0) {
        s8 max = (s8)(127 + org);

        if (max < var) {
            var = max;
        }
    }
    return var -= org;
}

static inline u8 ClampU8(u8 var, u8 org) {
    if (var < org) {
        var = org;
    }
    return var -= org;
}

static void fn_803809C8(s32 chan, PADStatus *status, u32 data[2]) {
    PADStatus *origin;
    u32 type;

    status->button = (u16)((data[0] >> 16) & 0x3FFF);
    status->stickX = (s8)(data[0] >> 8);
    status->stickY = (s8)(data[0]);

    switch (AnalogMode & 0x00000700) {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(((data[1] >> 12) & 0x0F) << 4);
        status->triggerRight = (u8)(((data[1] >> 8) & 0x0F) << 4);
        status->analogA = (u8)(((data[1] >> 4) & 0x0F) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0F) << 4);
        break;
    case 0x00000100:
        status->substickX = (s8)(((data[1] >> 28) & 0x0F) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0F) << 4);
        status->triggerLeft = (u8)(data[1] >> 16);
        status->triggerRight = (u8)(data[1] >> 8);
        status->analogA = (u8)(((data[1] >> 4) & 0x0F) << 4);
        status->analogB = (u8)(((data[1] >> 0) & 0x0F) << 4);
        break;
    case 0x00000200:
        status->substickX = (s8)(((data[1] >> 28) & 0x0F) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0F) << 4);
        status->triggerLeft = (u8)(((data[1] >> 20) & 0x0F) << 4);
        status->triggerRight = (u8)(((data[1] >> 16) & 0x0F) << 4);
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    case 0x00000300:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(data[1] >> 8);
        status->triggerRight = (u8)(data[1] >> 0);
        status->analogA = 0;
        status->analogB = 0;
        break;
    case 0x00000400:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = 0;
        status->triggerRight = 0;
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1] >> 0);
        break;
    }

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;

    type = Type[chan];
    if ((type & 0xFFFF0000) == SI_GC_CONTROLLER && ((status->button & 0x0080) ^ 0x0080)) {
        BarrelBits |= PAD_CHAN0_BIT >> chan;
        status->stickX = 0;
        status->stickY = 0;
        status->substickX = 0;
        status->substickY = 0;
        return;
    } else {
        BarrelBits &= ~(PAD_CHAN0_BIT >> chan);
    }

    origin = &Origin[chan];
    status->stickX = ClampS8(status->stickX, origin->stickX);
    status->stickY = ClampS8(status->stickY, origin->stickY);
    status->substickX = ClampS8(status->substickX, origin->substickX);
    status->substickY = ClampS8(status->substickY, origin->substickY);
    status->triggerLeft = ClampU8(status->triggerLeft, origin->triggerLeft);
    status->triggerRight = ClampU8(status->triggerRight, origin->triggerRight);
}

static inline BOOL PADSync(void) {
    return ResettingBits == 0 && ResettingChan == 32 && !SIBusy();
}

static BOOL fn_80380E38(BOOL final) {
    static BOOL recalibrated = FALSE;
    BOOL sync;

    if (SamplingCallback) {
        fn_80380F54(NULL);
    }

    if (!final) {
        sync = PADSync();
        if (!recalibrated && sync) {
            recalibrated = fn_8037FF98(PAD_CHAN0_BIT | 0x40000000 | 0x20000000 | 0x10000000);
            return FALSE;
        }
        return sync;
    } else {
        recalibrated = FALSE;
    }
    return TRUE;
}

static void fn_80380EF4(s16 interrupt, OSContext *context) {
    OSContext exceptionContext;

    if (SamplingCallback) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        SamplingCallback();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

PADSamplingCallback fn_80380F54(PADSamplingCallback callback) {
    PADSamplingCallback prev;

    prev = SamplingCallback;
    SamplingCallback = callback;
    if (callback) {
        SIRegisterPollingHandler(fn_80380EF4);
    } else {
        SIUnregisterPollingHandler(fn_80380EF4);
    }
    return prev;
}

BOOL __PADDisableRecalibration(BOOL disable) {
    BOOL enabled;
    BOOL prev;

    enabled = OSDisableInterrupts();
    prev = (__gUnknown800030E3 & 0x40) ? TRUE : FALSE;
    __gUnknown800030E3 &= ~0x40;
    if (disable) {
        __gUnknown800030E3 |= 0x40;
    }
    OSRestoreInterrupts(enabled);
    return prev;
}

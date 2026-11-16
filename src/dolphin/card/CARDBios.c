/* Dolphin SDK CARD: the bottom layer. It owns the per-slot control blocks, sends the card's EXI
 * commands (read a segment, write a page, erase a sector, status) and handles the interrupts and
 * timeouts that finish them, retrying a failed write or erase a few times. */
#include <dolphin/os.h>
#include <dolphin/os_interrupt.h>
#include <dolphin/card_internal.h>

typedef void (*EXICallback)(s32 chan, OSContext *context);
BOOL EXIImm(s32 chan, void *buf, s32 len, u32 type, EXICallback callback);
BOOL EXIDma(s32 chan, void *buf, s32 len, u32 type, EXICallback callback);
BOOL EXISync(s32 chan);
BOOL EXISelect(s32 chan, u32 dev, u32 freq);
BOOL EXIDeselect(s32 chan);
BOOL EXILock(s32 chan, u32 dev, EXICallback unlocked);
BOOL EXIUnlock(s32 chan);
BOOL EXIImmEx(s32 chan, void *buf, s32 len, u32 mode);
EXICallback EXISetExiCallback(s32 chan, EXICallback callback);
BOOL EXIProbe(s32 chan);

u16 fn_80374308(void);
#define OSGetFontEncode fn_80374308
void OSInitThreadQueue(OSThreadQueue *queue);
void DSPInit(void);
s32 CARDUnmount(s32 chan);

u32 __OSBusClock : 0x800000F8;
#define OS_TIMER_CLOCK (__OSBusClock / 4)
#define OSSecondsToTicks(sec) ((sec) * OS_TIMER_CLOCK)
#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))

#define EXI_READ 0
#define EXI_WRITE 1
#define EXI_FREQ_16M 4

/* The address bytes of a card command. */
#define AD1(x) ((u8)(((x) >> 17) & 0x7F))
#define AD1EX(x) ((u8)(AD1(x) | 0x80))
#define AD2(x) ((u8)(((x) >> 9) & 0xFF))
#define AD3(x) ((u8)(((x) >> 7) & 0x03))
#define BA(x) ((u8)((x) & 0x7F))

#define CARD_CMD_READ 0x52
#define CARD_CMD_ERASE_SECTOR 0xF1
#define CARD_CMD_WRITE_PAGE 0xF2
/* Two commands this build never sends but still has timeouts and error paths for. */
#define CARD_CMD_F3 0xF3
#define CARD_CMD_F4 0xF4

#define CARD_MAX_MOUNT_STEP (CARD_NUM_SYSTEM_BLOCK + 2)

const char *__CARDVersion =
    "<< Dolphin SDK - CARD\trelease build: Apr  5 2004 04:15:35 (0x2301) >>";

CARDControl __CARDBlock[2];
CARDDiskID __CARDDiskNone;

static u16 __CARDEncode;
static u16 __CARDFastMode;

static BOOL OnReset(BOOL final);
static s32 Retry(s32 chan);

static OSResetFunctionInfo ResetFunctionInfo = { OnReset, 127 };

void __CARDDefaultApiCallback(s32 chan, s32 result) {}

/* The card was pulled out. */
void __CARDExtHandler(s32 chan, OSContext *context) {
    CARDControl *card;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    if (card->attached) {
        card->attached = FALSE;
        EXISetExiCallback(chan, NULL);
        OSCancelAlarm(&card->alarm);
        callback = card->exiCallback;
        if (callback) {
            card->exiCallback = NULL;
            callback(chan, CARD_RESULT_NOCARD);
        }
        if (card->result != CARD_RESULT_BUSY) {
            card->result = CARD_RESULT_NOCARD;
        }
        callback = card->extCallback;
        if (callback && CARD_MAX_MOUNT_STEP <= card->mountStep) {
            card->extCallback = NULL;
            callback(chan, CARD_RESULT_NOCARD);
        }
    }
}

/* The card's own interrupt: a write or an erase has finished. */
void __CARDExiHandler(s32 chan, OSContext *context) {
    CARDControl *card;
    CARDCallback callback;
    u8 status;
    s32 result;

    card = &__CARDBlock[chan];
    OSCancelAlarm(&card->alarm);
    if (!card->attached) {
        return;
    }
    if (!EXILock(chan, 0, NULL)) {
        result = CARD_RESULT_FATAL_ERROR;
        goto fatal;
    }
    if ((result = __CARDReadStatus(chan, &status)) < 0 || (result = __CARDClearStatus(chan)) < 0) {
        goto error;
    }
    if ((result = (status & 0x18) ? CARD_RESULT_IOERROR : CARD_RESULT_READY) ==
            CARD_RESULT_IOERROR &&
        --card->retry > 0) {
        result = Retry(chan);
        if (result >= 0) {
            return;
        }
        goto fatal;
    }

error:
    EXIUnlock(chan);

fatal:
    callback = card->exiCallback;
    if (callback) {
        card->exiCallback = NULL;
        callback(chan, result);
    }
}

/* The DMA transfer of a read or a write has finished. */
void __CARDTxHandler(s32 chan, OSContext *context) {
    CARDControl *card;
    CARDCallback callback;
    BOOL err;

    card = &__CARDBlock[chan];
    err = !EXIDeselect(chan);
    EXIUnlock(chan);
    callback = card->txCallback;
    if (callback) {
        card->txCallback = NULL;
        callback(chan, (!err && EXIProbe(chan)) ? CARD_RESULT_READY : CARD_RESULT_NOCARD);
    }
}

/* The EXI bus that was busy is now ours. */
void __CARDUnlockedHandler(s32 chan, OSContext *context) {
    CARDControl *card;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    callback = card->unlockCallback;
    if (callback) {
        card->unlockCallback = NULL;
        callback(chan, EXIProbe(chan) ? CARD_RESULT_UNLOCKED : CARD_RESULT_NOCARD);
    }
}

s32 __CARDEnableInterrupt(s32 chan, BOOL enable) {
    BOOL err;
    u32 cmd;

    if (!EXISelect(chan, 0, EXI_FREQ_16M)) {
        return CARD_RESULT_NOCARD;
    }
    cmd = enable ? 0x81010000 : 0x81000000;
    err = FALSE;
    err |= !EXIImm(chan, &cmd, 2, EXI_WRITE, NULL);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);
    return err ? CARD_RESULT_NOCARD : CARD_RESULT_READY;
}

s32 __CARDReadStatus(s32 chan, u8 *status) {
    BOOL err;
    u32 cmd;

    if (!EXISelect(chan, 0, EXI_FREQ_16M)) {
        return CARD_RESULT_NOCARD;
    }
    cmd = 0x83000000;
    err = FALSE;
    err |= !EXIImm(chan, &cmd, 2, EXI_WRITE, NULL);
    err |= !EXISync(chan);
    err |= !EXIImm(chan, status, 1, EXI_READ, NULL);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);
    return err ? CARD_RESULT_NOCARD : CARD_RESULT_READY;
}

s32 __CARDReadVendorID(s32 chan, u16 *vendorID) {
    BOOL err;
    u32 cmd;

    if (!EXISelect(chan, 0, EXI_FREQ_16M)) {
        return CARD_RESULT_NOCARD;
    }
    cmd = 0x85000000;
    err = FALSE;
    err |= !EXIImm(chan, &cmd, 2, EXI_WRITE, NULL);
    err |= !EXISync(chan);
    err |= !EXIImm(chan, vendorID, 2, EXI_READ, NULL);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);
    return err ? CARD_RESULT_NOCARD : CARD_RESULT_READY;
}

s32 __CARDClearStatus(s32 chan) {
    BOOL err;
    u32 cmd;

    if (!EXISelect(chan, 0, EXI_FREQ_16M)) {
        return CARD_RESULT_NOCARD;
    }
    cmd = 0x89000000;
    err = FALSE;
    err |= !EXIImm(chan, &cmd, 1, EXI_WRITE, NULL);
    err |= !EXISync(chan);
    err |= !EXIDeselect(chan);
    return err ? CARD_RESULT_NOCARD : CARD_RESULT_READY;
}

/* The card never raised its interrupt. */
static void TimeoutHandler(OSAlarm *alarm, OSContext *context) {
    s32 chan;
    CARDControl *card;
    CARDCallback callback;

    for (chan = 0; chan < 2; ++chan) {
        card = &__CARDBlock[chan];
        if (alarm == &card->alarm) {
            break;
        }
    }
    if (!card->attached) {
        return;
    }
    EXISetExiCallback(chan, NULL);
    callback = card->exiCallback;
    if (callback) {
        card->exiCallback = NULL;
        callback(chan, CARD_RESULT_IOERROR);
    }
}

static inline void SetupTimeoutAlarm(CARDControl *card) {
    OSCancelAlarm(&card->alarm);
    switch (card->cmd[0]) {
    case CARD_CMD_WRITE_PAGE:
        OSSetAlarm(&card->alarm, OSMillisecondsToTicks(100), TimeoutHandler);
        break;
    case CARD_CMD_F3:
        break;
    case CARD_CMD_F4:
        if (card->pageSize > 0x80) {
            OSSetAlarm(&card->alarm, OSSecondsToTicks((OSTime)2) * (card->cBlock / 0x40),
                       TimeoutHandler);
            break;
        }
        /* fall through */
    case CARD_CMD_ERASE_SECTOR:
        OSSetAlarm(&card->alarm, OSSecondsToTicks((OSTime)2) * (card->sectorSize / 0x2000),
                   TimeoutHandler);
        break;
    }
}

/* Send the remembered command again. */
static s32 Retry(s32 chan) {
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (!EXISelect(chan, 0, EXI_FREQ_16M)) {
        EXIUnlock(chan);
        return CARD_RESULT_NOCARD;
    }
    SetupTimeoutAlarm(card);
    if (!EXIImmEx(chan, card->cmd, card->cmdlen, EXI_WRITE)) {
        EXIDeselect(chan);
        EXIUnlock(chan);
        return CARD_RESULT_NOCARD;
    }
    if (card->cmd[0] == CARD_CMD_READ &&
        !EXIImmEx(chan, (u8 *)card->workArea + sizeof(CARDID), card->latency, EXI_WRITE)) {
        EXIDeselect(chan);
        EXIUnlock(chan);
        return CARD_RESULT_NOCARD;
    }
    if (card->mode == 0xFFFFFFFF) {
        EXIDeselect(chan);
        EXIUnlock(chan);
        return CARD_RESULT_READY;
    }
    if (!EXIDma(chan, card->buffer, (s32)((card->cmd[0] == CARD_CMD_READ) ? 512 : card->pageSize),
                card->mode, __CARDTxHandler)) {
        EXIDeselect(chan);
        EXIUnlock(chan);
        return CARD_RESULT_NOCARD;
    }
    return CARD_RESULT_READY;
}

static void UnlockedCallback(s32 chan, s32 result) {
    CARDCallback callback;
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (result >= 0) {
        card->unlockCallback = UnlockedCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            result = CARD_RESULT_READY;
        } else {
            card->unlockCallback = NULL;
            result = Retry(chan);
        }
    }
    if (result < 0) {
        switch (card->cmd[0]) {
        case CARD_CMD_READ:
            callback = card->txCallback;
            if (callback) {
                card->txCallback = NULL;
                callback(chan, result);
            }
            break;
        case CARD_CMD_WRITE_PAGE:
        case CARD_CMD_F4:
        case CARD_CMD_ERASE_SECTOR:
            callback = card->exiCallback;
            if (callback) {
                card->exiCallback = NULL;
                callback(chan, result);
            }
            break;
        case CARD_CMD_F3:
            break;
        }
    }
}

/* Take the bus and select the card for the command in card->cmd. BUSY means the command will be
 * sent from UnlockedCallback once the bus is free. */
static s32 __CARDStart(s32 chan, CARDCallback txCallback, CARDCallback exiCallback) {
    BOOL enabled;
    CARDControl *card;
    s32 result;

    enabled = OSDisableInterrupts();
    card = &__CARDBlock[chan];
    if (!card->attached) {
        result = CARD_RESULT_NOCARD;
    } else {
        if (txCallback) {
            card->txCallback = txCallback;
        }
        if (exiCallback) {
            card->exiCallback = exiCallback;
        }
        card->unlockCallback = UnlockedCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            result = CARD_RESULT_BUSY;
        } else {
            card->unlockCallback = NULL;
            if (!EXISelect(chan, 0, EXI_FREQ_16M)) {
                EXIUnlock(chan);
                result = CARD_RESULT_NOCARD;
            } else {
                SetupTimeoutAlarm(card);
                result = CARD_RESULT_READY;
            }
        }
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 __CARDReadSegment(s32 chan, CARDCallback callback) {
    CARDControl *card;
    s32 result;

    card = &__CARDBlock[chan];
    card->cmd[0] = CARD_CMD_READ;
    card->cmd[1] = AD1(card->addr);
    card->cmd[2] = AD2(card->addr);
    card->cmd[3] = AD3(card->addr);
    card->cmd[4] = BA(card->addr);
    card->cmdlen = 5;
    card->mode = EXI_READ;
    card->retry = 0;

    result = __CARDStart(chan, callback, NULL);
    if (result == CARD_RESULT_BUSY) {
        result = CARD_RESULT_READY;
    } else if (result >= 0) {
        if (!EXIImmEx(chan, card->cmd, card->cmdlen, EXI_WRITE) ||
            !EXIImmEx(chan, (u8 *)card->workArea + sizeof(CARDID), card->latency, EXI_WRITE) ||
            !EXIDma(chan, card->buffer, 512, card->mode, __CARDTxHandler)) {
            card->txCallback = NULL;
            EXIDeselect(chan);
            EXIUnlock(chan);
            result = CARD_RESULT_NOCARD;
        } else {
            result = CARD_RESULT_READY;
        }
    }
    return result;
}

s32 __CARDWritePage(s32 chan, CARDCallback callback) {
    CARDControl *card;
    s32 result;

    card = &__CARDBlock[chan];
    card->cmd[0] = CARD_CMD_WRITE_PAGE;
    if (card->pageSize > 0x80) {
        card->cmd[1] = AD1EX(card->addr);
    } else {
        card->cmd[1] = AD1(card->addr);
    }
    card->cmd[2] = AD2(card->addr);
    card->cmd[3] = AD3(card->addr);
    card->cmd[4] = BA(card->addr);
    card->cmdlen = 5;
    card->mode = EXI_WRITE;
    card->retry = 3;

    result = __CARDStart(chan, NULL, callback);
    if (result == CARD_RESULT_BUSY) {
        result = CARD_RESULT_READY;
    } else if (result >= 0) {
        if (!EXIImmEx(chan, card->cmd, card->cmdlen, EXI_WRITE) ||
            !EXIDma(chan, card->buffer, card->pageSize, card->mode, __CARDTxHandler)) {
            card->exiCallback = NULL;
            EXIDeselect(chan);
            EXIUnlock(chan);
            result = CARD_RESULT_NOCARD;
        } else {
            result = CARD_RESULT_READY;
        }
    }
    return result;
}

s32 __CARDEraseSector(s32 chan, u32 addr, CARDCallback callback) {
    CARDControl *card;
    s32 result;

    card = &__CARDBlock[chan];
    if (card->pageSize > 0x80) {
        /* This kind of card needs no erase before a write. */
        if (callback) {
            callback(chan, CARD_RESULT_READY);
        }
        return CARD_RESULT_READY;
    }
    card->cmd[0] = CARD_CMD_ERASE_SECTOR;
    card->cmd[1] = AD1(addr);
    card->cmd[2] = AD2(addr);
    card->cmdlen = 3;
    card->mode = 0xFFFFFFFF;
    card->retry = 3;

    result = __CARDStart(chan, NULL, callback);
    if (result == CARD_RESULT_BUSY) {
        result = CARD_RESULT_READY;
    } else if (result >= 0) {
        if (!EXIImmEx(chan, card->cmd, card->cmdlen, EXI_WRITE)) {
            card->exiCallback = NULL;
            result = CARD_RESULT_NOCARD;
        } else {
            result = CARD_RESULT_READY;
        }
        EXIDeselect(chan);
        EXIUnlock(chan);
    }
    return result;
}

void CARDInit(void) {
    int chan;

    if (__CARDBlock[0].diskID && __CARDBlock[1].diskID) {
        return;
    }
    __CARDEncode = OSGetFontEncode();
    OSRegisterVersion(__CARDVersion);
    DSPInit();
    OSInitAlarm();
    for (chan = 0; chan < 2; ++chan) {
        CARDControl *card = &__CARDBlock[chan];

        card->result = CARD_RESULT_NOCARD;
        OSInitThreadQueue(&card->threadQueue);
        OSCreateAlarm(&card->alarm);
    }
    __CARDSetDiskID((CARDDiskID *)OSPhysicalToCached(0));
    OSRegisterResetFunction(&ResetFunctionInfo);
}

u16 __CARDGetFontEncode(void) { return __CARDEncode; }

void __CARDSetDiskID(CARDDiskID *id) {
    __CARDBlock[0].diskID = id ? id : &__CARDDiskNone;
    __CARDBlock[1].diskID = id ? id : &__CARDDiskNone;
}

s32 __CARDGetControlBlock(s32 chan, CARDControl **pcard) {
    BOOL enabled;
    s32 result;
    CARDControl *card;

    card = &__CARDBlock[chan];
    if (chan < 0 || chan >= 2 || card->diskID == NULL) {
        return CARD_RESULT_FATAL_ERROR;
    }
    enabled = OSDisableInterrupts();
    if (!card->attached) {
        result = CARD_RESULT_NOCARD;
    } else if (card->result == CARD_RESULT_BUSY) {
        result = CARD_RESULT_BUSY;
    } else {
        card->result = CARD_RESULT_BUSY;
        result = CARD_RESULT_READY;
        card->apiCallback = NULL;
        *pcard = card;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 __CARDPutControlBlock(CARDControl *card, s32 result) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    if (card->attached) {
        card->result = result;
    } else if (card->result == CARD_RESULT_BUSY) {
        card->result = result;
    }
    OSRestoreInterrupts(enabled);
    return result;
}

s32 CARDGetResultCode(s32 chan) {
    CARDControl *card;

    if (chan < 0 || 2 <= chan) {
        return CARD_RESULT_FATAL_ERROR;
    }
    card = &__CARDBlock[chan];
    return card->result;
}

s32 CARDFreeBlocks(s32 chan, s32 *byteNotUsed, s32 *filesNotUsed) {
    CARDControl *card;
    s32 result;
    u16 *fat;
    CARDDir *dir;
    CARDDir *entry;
    u16 fileNo;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    fat = __CARDGetFatBlock(card);
    dir = __CARDGetDirBlock(card);
    if (fat == NULL || dir == NULL) {
        return __CARDPutControlBlock(card, CARD_RESULT_BROKEN);
    }
    if (byteNotUsed) {
        *byteNotUsed = (s32)(card->sectorSize * fat[CARD_FAT_FREEBLOCKS]);
    }
    if (filesNotUsed) {
        *filesNotUsed = 0;
        for (fileNo = 0; fileNo < CARD_MAX_FILE; fileNo++) {
            entry = &dir[fileNo];
            if (entry->fileName[0] == 0xFF) {
                ++*filesNotUsed;
            }
        }
    }
    return __CARDPutControlBlock(card, CARD_RESULT_READY);
}

/* Before a reset: wait until neither slot is busy. */
static BOOL OnReset(BOOL final) {
    if (!final) {
        if (CARDUnmount(0) == CARD_RESULT_BUSY || CARDUnmount(1) == CARD_RESULT_BUSY) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CARDGetFastMode(void) { return __CARDFastMode ? TRUE : FALSE; }

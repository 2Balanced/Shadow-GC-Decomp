/* Dolphin SDK CARD: mounting. The card is identified from its EXI ID, unlocked (once per insertion;
 * the flash ID is remembered in SRAM so a reset can skip it), and then its five system blocks are
 * read one per step and verified. */
#include <dolphin/os.h>
#include <dolphin/card_internal.h>

typedef void (*EXICallback)(s32 chan, OSContext *context);
BOOL EXILock(s32 chan, u32 dev, EXICallback unlocked);
BOOL EXIUnlock(s32 chan);
s32 EXIGetID(s32 chan, u32 dev, u32 *id);
/* EXI calls still under placeholder names. */
BOOL fn_803B33C4(s32 chan);                          /* probe */
s32 fn_803B3444(s32 chan);                           /* probe, with "still settling" */
u32 fn_803B4050(s32 chan);                           /* channel state */
EXICallback fn_803B31D4(s32 chan, EXICallback cb);   /* set the EXI interrupt callback */
BOOL fn_803B34F8(s32 chan, EXICallback ext);         /* attach */
BOOL fn_803B3604(s32 chan);                          /* detach */

typedef struct OSSramEx {
    u8 flashID[2][12];
    u8 unk18[0x26 - 0x18];
    u8 flashIDCheckSum[2]; /* 0x26 */
} OSSramEx;

OSSramEx *__OSLockSramEx(void);
BOOL __OSUnlockSramEx(BOOL commit);

#define GameChoice (*(u8 *)0x800030E3)
#define CARD_MAX_MOUNT_STEP (CARD_NUM_SYSTEM_BLOCK + 2)
#define EXI_STATE_ATTACHED 0x08

static u32 SectorSizeTable[8] = {
    8 * 1024, 16 * 1024, 32 * 1024, 64 * 1024, 128 * 1024, 256 * 1024, 0, 0,
};

static u32 LatencyTable[8] = {
    4, 8, 16, 32, 64, 128, 256, 512,
};

void __CARDMountCallback(s32 chan, s32 result);
static void DoUnmount(s32 chan, s32 result);

static BOOL IsCard(u32 id) {
    u32 size;
    s32 sectorSize;

    if (id & 0xFFFF0000 && (id != 0x80000004 || __CARDVendorID == 0xFFFF)) {
        return FALSE;
    }
    if ((id & 3) != 0) {
        return FALSE;
    }
    size = id & 0xFC;
    switch (size) {
    case 4:
    case 8:
    case 16:
    case 32:
    case 64:
    case 128:
        break;
    default:
        return FALSE;
    }
    sectorSize = SectorSizeTable[(id & 0x00003800) >> 11];
    if (sectorSize == 0) {
        return FALSE;
    }
    if ((size * 1024 * 1024 / 8) / sectorSize < 8) {
        return FALSE;
    }
    return TRUE;
}

s32 CARDProbeEx(s32 chan, s32 *memSize, s32 *sectorSize) {
    u32 id;
    CARDControl *card;
    BOOL enabled;
    s32 result;
    int probe;

    if (chan < 0 || 2 <= chan) {
        return CARD_RESULT_FATAL_ERROR;
    }
    if (GameChoice & 0x80) {
        return CARD_RESULT_NOCARD;
    }

    card = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();

    probe = fn_803B3444(chan);
    if (probe == -1) {
        result = CARD_RESULT_NOCARD;
    } else if (probe == 0) {
        result = CARD_RESULT_BUSY;
    } else if (card->attached) {
        if (card->mountStep < 1) {
            result = CARD_RESULT_BUSY;
        } else {
            if (memSize) {
                *memSize = card->size;
            }
            if (sectorSize) {
                *sectorSize = card->sectorSize;
            }
            result = CARD_RESULT_READY;
        }
    } else if ((fn_803B4050(chan) & EXI_STATE_ATTACHED)) {
        result = CARD_RESULT_WRONGDEVICE;
    } else if (!EXIGetID(chan, 0, &id)) {
        result = CARD_RESULT_BUSY;
    } else if (IsCard(id)) {
        if (memSize) {
            *memSize = (s32)(id & 0xFC);
        }
        if (sectorSize) {
            *sectorSize = SectorSizeTable[(id & 0x00003800) >> 11];
        }
        result = CARD_RESULT_READY;
    } else {
        result = CARD_RESULT_WRONGDEVICE;
    }

    OSRestoreInterrupts(enabled);
    return result;
}

static s32 DoMount(s32 chan) {
    CARDControl *card;
    u32 id;
    u8 status;
    s32 result;
    OSSramEx *sramEx;
    int i;
    u8 checkSum;
    int step;

    card = &__CARDBlock[chan];
    if (card->mountStep == 0) {
        if (EXIGetID(chan, 0, &id) == 0) {
            result = CARD_RESULT_NOCARD;
        } else if (IsCard(id)) {
            result = CARD_RESULT_READY;
        } else {
            result = CARD_RESULT_WRONGDEVICE;
        }
        if (result < 0) {
            goto error;
        }

        card->cid = id;
        card->size = (u16)(id & 0xFC);
        card->sectorSize = SectorSizeTable[(id & 0x00003800) >> 11];
        card->cBlock = (u16)((card->size * 1024 * 1024 / 8) / card->sectorSize);
        card->latency = LatencyTable[(id & 0x00000700) >> 8];

        result = __CARDReadVendorID(chan, &card->vendorID);
        if (result < 0) {
            goto error;
        }
        if (CARDGetFastMode() && (card->vendorID >> 8) == 0xEC) {
            card->pageSize = 0x200;
        } else {
            card->pageSize = 0x80;
        }

        result = __CARDClearStatus(chan);
        if (result < 0) {
            goto error;
        }
        result = __CARDReadStatus(chan, &status);
        if (result < 0) {
            goto error;
        }
        if (!fn_803B33C4(chan)) {
            result = CARD_RESULT_NOCARD;
            goto error;
        }

        if (!(status & 0x40)) {
            result = __CARDUnlock(chan, card->id);
            if (result < 0) {
                goto error;
            }
            checkSum = 0;
            sramEx = __OSLockSramEx();
            for (i = 0; i < 12; i++) {
                sramEx->flashID[chan][i] = card->id[i];
                checkSum += card->id[i];
            }
            sramEx->flashIDCheckSum[chan] = (u8)~checkSum;
            __OSUnlockSramEx(TRUE);
            return result;
        } else {
            card->mountStep = 1;
            checkSum = 0;
            sramEx = __OSLockSramEx();
            for (i = 0; i < 12; i++) {
                checkSum += sramEx->flashID[chan][i];
            }
            __OSUnlockSramEx(FALSE);
            if (sramEx->flashIDCheckSum[chan] != (u8)~checkSum) {
                result = CARD_RESULT_IOERROR;
                goto error;
            }
        }
    }

    if (card->mountStep == 1) {
        if (card->cid == 0x80000004) {
            u16 vendorID;

            sramEx = __OSLockSramEx();
            vendorID = *(u16 *)sramEx->flashID[chan];
            __OSUnlockSramEx(FALSE);
            if (__CARDVendorID == 0xFFFF || vendorID != __CARDVendorID) {
                result = CARD_RESULT_WRONGDEVICE;
                goto error;
            }
        }
        card->mountStep = 2;
        result = __CARDEnableInterrupt(chan, TRUE);
        if (result < 0) {
            goto error;
        }
        fn_803B31D4(chan, __CARDExiHandler);
        EXIUnlock(chan);
        DCInvalidateRange(card->workArea, CARD_NUM_SYSTEM_BLOCK * CARD_SYSTEM_BLOCK_SIZE);
    }

    step = card->mountStep - 2;
    result = __CARDRead(chan, (u32)card->sectorSize * step, CARD_SYSTEM_BLOCK_SIZE,
                        (u8 *)card->workArea + (CARD_SYSTEM_BLOCK_SIZE * step),
                        __CARDMountCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;

error:
    EXIUnlock(chan);
    DoUnmount(chan, result);
    return result;
}

void __CARDMountCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;

    card = &__CARDBlock[chan];
    switch (result) {
    case CARD_RESULT_READY:
        if (++card->mountStep < CARD_MAX_MOUNT_STEP) {
            result = DoMount(chan);
            if (0 <= result) {
                return;
            }
        } else {
            result = __CARDVerify(card);
        }
        break;
    case CARD_RESULT_UNLOCKED:
        card->unlockCallback = __CARDMountCallback;
        if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
            return;
        }
        card->unlockCallback = NULL;
        result = DoMount(chan);
        if (0 <= result) {
            return;
        }
        break;
    case CARD_RESULT_IOERROR:
    case CARD_RESULT_NOCARD:
        DoUnmount(chan, result);
        break;
    }

    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

s32 CARDMountAsync(s32 chan, void *workArea, CARDCallback detachCallback,
                   CARDCallback attachCallback) {
    CARDControl *card;
    BOOL enabled;

    if (chan < 0 || 2 <= chan) {
        return CARD_RESULT_FATAL_ERROR;
    }
    if (GameChoice & 0x80) {
        return CARD_RESULT_NOCARD;
    }

    card = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();
    if (card->result == CARD_RESULT_BUSY) {
        OSRestoreInterrupts(enabled);
        return CARD_RESULT_BUSY;
    }
    if (!card->attached && (fn_803B4050(chan) & EXI_STATE_ATTACHED)) {
        OSRestoreInterrupts(enabled);
        return CARD_RESULT_WRONGDEVICE;
    }

    card->result = CARD_RESULT_BUSY;
    card->workArea = workArea;
    card->extCallback = detachCallback;
    card->apiCallback = attachCallback ? attachCallback : __CARDDefaultApiCallback;
    card->exiCallback = NULL;

    if (!card->attached && !fn_803B34F8(chan, __CARDExtHandler)) {
        card->result = CARD_RESULT_NOCARD;
        OSRestoreInterrupts(enabled);
        return CARD_RESULT_NOCARD;
    }

    card->mountStep = 0;
    card->attached = TRUE;
    fn_803B31D4(chan, NULL);
    OSCancelAlarm(&card->alarm);
    card->currentDir = NULL;
    card->currentFat = NULL;
    OSRestoreInterrupts(enabled);

    card->unlockCallback = __CARDMountCallback;
    if (!EXILock(chan, 0, __CARDUnlockedHandler)) {
        return CARD_RESULT_READY;
    }
    card->unlockCallback = NULL;
    return DoMount(chan);
}

static void DoUnmount(s32 chan, s32 result) {
    CARDControl *card;
    BOOL enabled;

    card = &__CARDBlock[chan];
    enabled = OSDisableInterrupts();
    if (card->attached) {
        fn_803B31D4(chan, NULL);
        fn_803B3604(chan);
        OSCancelAlarm(&card->alarm);
        card->attached = FALSE;
        card->result = result;
        card->mountStep = 0;
    }
    OSRestoreInterrupts(enabled);
}

s32 CARDUnmount(s32 chan) {
    CARDControl *card;
    s32 result;

    result = __CARDGetControlBlock(chan, &card);
    if (result < 0) {
        return result;
    }
    DoUnmount(chan, CARD_RESULT_NOCARD);
    return CARD_RESULT_READY;
}

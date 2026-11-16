/* Dolphin SDK CARD: consistency checks. The header's check sums and serial, both directory copies
 * and both allocation-table copies are verified; CARDCheckExAsync also repairs what one good copy
 * allows (a damaged twin, blocks no file owns, a wrong free count). */
#include <dolphin/os.h>
#include <dolphin/card_internal.h>

void *memcpy(void *dst, const void *src, unsigned long n);
void *memset(void *dst, int value, unsigned long n);

typedef struct OSSramEx {
    u8 flashID[2][12];
} OSSramEx;

OSSramEx *__OSLockSramEx(void);
BOOL __OSUnlockSramEx(BOOL commit);

#define CARDIsValidBlockNo(card, iBlock)                                                           \
    (CARD_NUM_SYSTEM_BLOCK <= (iBlock) && (iBlock) < (card)->cBlock)

#define __CARDGetDirCheck(dir) ((CARDDirCheck *)&(dir)[CARD_MAX_FILE])

/* Sum of the 16-bit words and sum of their complements; 0xFFFF is stored as 0. */
void __CARDCheckSum(void *ptr, int length, u16 *checksum, u16 *checksumInv) {
    u16 *p;
    int i;

    length /= sizeof(u16);
    *checksum = *checksumInv = 0;
    for (i = 0, p = ptr; i < length; i++, p++) {
        *checksum += *p;
        *checksumInv += ~*p;
    }
    if (*checksum == 0xFFFF) {
        *checksum = 0;
    }
    if (*checksumInv == 0xFFFF) {
        *checksumInv = 0;
    }
}

static s32 VerifyID(CARDControl *card) {
    CARDID *id;
    u16 checksum;
    u16 checksumInv;
    OSSramEx *sramEx;
    OSTime rand;
    int i;

    id = card->workArea;
    if (id->deviceID != 0 || id->size != card->size) {
        return CARD_RESULT_BROKEN;
    }
    __CARDCheckSum(id, sizeof(CARDID) - sizeof(u32), &checksum, &checksumInv);
    if (id->checkSum != checksum || id->checkSumInv != checksumInv) {
        return CARD_RESULT_BROKEN;
    }

    rand = *(OSTime *)&id->serial[12];
    sramEx = __OSLockSramEx();
    for (i = 0; i < 12; i++) {
        rand = (rand * 1103515245 + 12345) >> 16;
        if (id->serial[i] != (u8)(sramEx->flashID[card - __CARDBlock][i] + rand)) {
            __OSUnlockSramEx(FALSE);
            return CARD_RESULT_BROKEN;
        }
        rand = (rand * 1103515245 + 12345) >> 16;
        rand &= 0x7FFF;
    }
    __OSUnlockSramEx(FALSE);

    if (id->encode != fn_8038D0DC()) {
        return CARD_RESULT_ENCODING;
    }
    return CARD_RESULT_READY;
}

/* Returns how many of the two copies are bad; *outCurrent is the copy to use (the newer one when
 * both are good). */
static int VerifyDir(CARDControl *card, int *outCurrent) {
    CARDDir *dir[2];
    CARDDirCheck *check[2];
    u16 checkSum;
    u16 checkSumInv;
    int i;
    int errors;
    int current;

    current = errors = 0;
    for (i = 0; i < 2; i++) {
        dir[i] = (CARDDir *)((u8 *)card->workArea + (1 + i) * CARD_SYSTEM_BLOCK_SIZE);
        check[i] = __CARDGetDirCheck(dir[i]);
        __CARDCheckSum(dir[i], CARD_SYSTEM_BLOCK_SIZE - sizeof(u32), &checkSum, &checkSumInv);
        if (checkSum != check[i]->checkSum || checkSumInv != check[i]->checkSumInv) {
            card->currentDir = NULL;
            current = i;
            ++errors;
        }
    }

    if (0 == errors) {
        if (card->currentDir == NULL) {
            if ((check[0]->checkCode - check[1]->checkCode) < 0) {
                current = 0;
            } else {
                current = 1;
            }
            card->currentDir = dir[current];
            memcpy(dir[current], dir[current ^ 1], CARD_SYSTEM_BLOCK_SIZE);
        } else {
            current = (card->currentDir == dir[0]) ? 0 : 1;
        }
    }
    if (outCurrent) {
        *outCurrent = current;
    }
    return errors;
}

static int VerifyFAT(CARDControl *card, int *outCurrent) {
    u16 *fat[2];
    u16 *fatp;
    u16 nBlock;
    u16 cFree;
    int i;
    u16 checkSum;
    u16 checkSumInv;
    int errors;
    int current;

    current = errors = 0;
    for (i = 0; i < 2; i++) {
        fatp = fat[i] = (u16 *)((u8 *)card->workArea + (3 + i) * CARD_SYSTEM_BLOCK_SIZE);
        __CARDCheckSum(&fatp[CARD_FAT_CHECKCODE], CARD_SYSTEM_BLOCK_SIZE - sizeof(u32), &checkSum,
                       &checkSumInv);
        if (fatp[CARD_FAT_CHECKSUM] != checkSum || fatp[CARD_FAT_CHECKSUMINV] != checkSumInv) {
            card->currentFat = NULL;
            current = i;
            ++errors;
            continue;
        }

        cFree = 0;
        for (nBlock = CARD_NUM_SYSTEM_BLOCK; nBlock < card->cBlock; nBlock++) {
            if (fatp[nBlock] == 0) {
                cFree++;
            }
        }
        if (cFree != fatp[CARD_FAT_FREEBLOCKS]) {
            card->currentFat = NULL;
            current = i;
            ++errors;
            continue;
        }
    }

    if (0 == errors) {
        if (card->currentFat == NULL) {
            if (((s16)fat[0][CARD_FAT_CHECKCODE] - (s16)fat[1][CARD_FAT_CHECKCODE]) < 0) {
                current = 0;
            } else {
                current = 1;
            }
            card->currentFat = fat[current];
            memcpy(fat[current], fat[current ^ 1], CARD_SYSTEM_BLOCK_SIZE);
        } else {
            current = (card->currentFat == fat[0]) ? 0 : 1;
        }
    }
    if (outCurrent) {
        *outCurrent = current;
    }
    return errors;
}

s32 __CARDVerify(CARDControl *card) {
    s32 result;
    int errors;

    result = VerifyID(card);
    if (result < 0) {
        return result;
    }
    errors = VerifyDir(card, NULL);
    errors += VerifyFAT(card, NULL);
    switch (errors) {
    case 0:
        return CARD_RESULT_READY;
    case 1:
        return CARD_RESULT_BROKEN;
    default:
        return CARD_RESULT_BROKEN;
    }
}

/* *xferBytes, when given, is set to the bytes that will be written to repair the card. */
s32 CARDCheckExAsync(s32 chan, s32 *xferBytes, CARDCallback callback) {
    CARDControl *card;
    CARDDir *dir[2];
    u16 *fat[2];
    u16 *map;
    s32 result;
    int errors;
    int currentFat;
    int currentDir;
    s32 fileNo;
    u16 iBlock;
    u16 cBlock;
    u16 cFree;
    BOOL updateFat = FALSE;
    BOOL updateDir = FALSE;
    BOOL updateOrphan = FALSE;

    if (xferBytes) {
        *xferBytes = 0;
    }
    result = fn_8038D11C(chan, &card);
    if (result < 0) {
        return result;
    }
    result = VerifyID(card);
    if (result < 0) {
        return fn_8038D1D4(card, result);
    }

    errors = VerifyDir(card, &currentDir);
    errors += VerifyFAT(card, &currentFat);
    if (1 < errors) {
        return fn_8038D1D4(card, CARD_RESULT_BROKEN);
    }

    dir[0] = (CARDDir *)((u8 *)card->workArea + (1 + 0) * CARD_SYSTEM_BLOCK_SIZE);
    dir[1] = (CARDDir *)((u8 *)card->workArea + (1 + 1) * CARD_SYSTEM_BLOCK_SIZE);
    fat[0] = (u16 *)((u8 *)card->workArea + (3 + 0) * CARD_SYSTEM_BLOCK_SIZE);
    fat[1] = (u16 *)((u8 *)card->workArea + (3 + 1) * CARD_SYSTEM_BLOCK_SIZE);

    switch (errors) {
    case 0:
        break;
    case 1:
        if (!card->currentDir) {
            card->currentDir = dir[currentDir];
            memcpy(dir[currentDir], dir[currentDir ^ 1], CARD_SYSTEM_BLOCK_SIZE);
            updateDir = TRUE;
        } else {
            card->currentFat = fat[currentFat];
            memcpy(fat[currentFat], fat[currentFat ^ 1], CARD_SYSTEM_BLOCK_SIZE);
            updateFat = TRUE;
        }
        break;
    }

    /* Count, in the spare table copy, how many files claim each block. */
    map = fat[currentFat ^ 1];
    memset(map, 0, CARD_SYSTEM_BLOCK_SIZE);
    for (fileNo = 0; fileNo < CARD_MAX_FILE; fileNo++) {
        CARDDir *entry;

        entry = &card->currentDir[fileNo];
        if (entry->gameName[0] == 0xFF) {
            continue;
        }
        for (iBlock = entry->startBlock, cBlock = 0; iBlock != 0xFFFF && cBlock < entry->length;
             iBlock = card->currentFat[iBlock], ++cBlock) {
            if (!CARDIsValidBlockNo(card, iBlock) || 1 < ++map[iBlock]) {
                return fn_8038D1D4(card, CARD_RESULT_BROKEN);
            }
        }
        if (cBlock != entry->length || iBlock != 0xFFFF) {
            return fn_8038D1D4(card, CARD_RESULT_BROKEN);
        }
    }

    cFree = 0;
    for (iBlock = CARD_NUM_SYSTEM_BLOCK; iBlock < card->cBlock; iBlock++) {
        u16 nextBlock;

        nextBlock = card->currentFat[iBlock];
        if (map[iBlock] == 0) {
            if (nextBlock != 0) {
                card->currentFat[iBlock] = 0;
                updateOrphan = TRUE;
            }
            cFree++;
        } else if (!CARDIsValidBlockNo(card, nextBlock) && nextBlock != 0xFFFF) {
            return fn_8038D1D4(card, CARD_RESULT_BROKEN);
        }
    }
    if (cFree != card->currentFat[CARD_FAT_FREEBLOCKS]) {
        card->currentFat[CARD_FAT_FREEBLOCKS] = cFree;
        updateOrphan = TRUE;
    }
    if (updateOrphan) {
        __CARDCheckSum(&card->currentFat[CARD_FAT_CHECKCODE], CARD_SYSTEM_BLOCK_SIZE - sizeof(u32),
                       &card->currentFat[CARD_FAT_CHECKSUM],
                       &card->currentFat[CARD_FAT_CHECKSUMINV]);
    }

    memcpy(fat[currentFat ^ 1], fat[currentFat], CARD_SYSTEM_BLOCK_SIZE);

    if (updateDir) {
        if (xferBytes) {
            *xferBytes = CARD_SYSTEM_BLOCK_SIZE;
        }
        return __CARDUpdateDir(chan, callback);
    }
    if (updateFat | updateOrphan) {
        if (xferBytes) {
            *xferBytes = CARD_SYSTEM_BLOCK_SIZE;
        }
        return __CARDUpdateFatBlock(chan, card->currentFat, callback);
    }

    fn_8038D1D4(card, CARD_RESULT_READY);
    if (callback) {
        BOOL enabled = OSDisableInterrupts();

        callback(chan, CARD_RESULT_READY);
        OSRestoreInterrupts(enabled);
    }
    return CARD_RESULT_READY;
}

s32 CARDCheckAsync(s32 chan, CARDCallback callback) {
    s32 xferBytes;

    return CARDCheckExAsync(chan, &xferBytes, callback);
}

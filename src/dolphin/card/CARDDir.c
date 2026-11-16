/* Dolphin SDK CARD: the directory block. Like the allocation table it exists twice on the card;
 * an update goes to the current copy (erase, then write) and the copies then swap roles. */
#include <dolphin/os.h>
#include <dolphin/card_internal.h>

void *memcpy(void *dst, const void *src, unsigned long n);

CARDDir *__CARDGetDirBlock(CARDControl *card) { return card->currentDir; }

static void WriteCallback(s32 chan, s32 result) {
    CARDControl *card = &__CARDBlock[chan];
    CARDCallback callback;
    CARDDir *dir;
    CARDDir *dirBack;

    if (result >= 0) {
        dir = (CARDDir *)((u8 *)card->workArea + 0x2000);
        dirBack = (CARDDir *)((u8 *)card->workArea + 0x4000);
        if (card->currentDir == dir) {
            card->currentDir = dirBack;
            memcpy(dirBack, dir, CARD_SYSTEM_BLOCK_SIZE);
        } else {
            card->currentDir = dir;
            memcpy(dir, dirBack, CARD_SYSTEM_BLOCK_SIZE);
        }
    }
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->eraseCallback;
    if (callback) {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

static void EraseCallback(s32 chan, s32 result) {
    CARDControl *card = &__CARDBlock[chan];
    CARDCallback callback;
    CARDDir *dir;
    u32 addr;
    u32 unused; /* never touched; retail's frame has room for it */

    if (result < 0) {
        goto error;
    }
    dir = __CARDGetDirBlock(card);
    addr = ((u32)dir - (u32)card->workArea) / CARD_SYSTEM_BLOCK_SIZE * card->sectorSize;
    result = __CARDWrite(chan, addr, CARD_SYSTEM_BLOCK_SIZE, dir, WriteCallback);
    if (result < 0) {
        goto error;
    }
    return;

error:
    if (card->apiCallback == NULL) {
        __CARDPutControlBlock(card, result);
    }
    callback = card->eraseCallback;
    if (callback) {
        card->eraseCallback = NULL;
        callback(chan, result);
    }
}

/* The check code and checksums at the end of a directory block. An inline function, not a macro:
 * retail's ++check->checkCode goes through the directory pointer, which only this form gives. */
static inline CARDDirCheck *__CARDGetDirCheck(CARDDir *dir) {
    return (CARDDirCheck *)&dir[CARD_MAX_FILE];
}

s32 __CARDUpdateDir(s32 chan, CARDCallback callback) {
    CARDControl *card = &__CARDBlock[chan];
    CARDDirCheck *check;
    CARDDir *dir;
    u32 addr;
    u32 unused; /* never touched; retail's frame has room for it */

    if (!card->attached) {
        return CARD_RESULT_NOCARD;
    }
    dir = __CARDGetDirBlock(card);
    check = __CARDGetDirCheck(dir);
    ++check->checkCode;
    __CARDCheckSum(dir, CARD_SYSTEM_BLOCK_SIZE - 4, &check->checkSum, &check->checkSumInv);
    DCStoreRange(dir, CARD_SYSTEM_BLOCK_SIZE);
    card->eraseCallback = callback;
    addr = ((u32)dir - (u32)card->workArea) / CARD_SYSTEM_BLOCK_SIZE * card->sectorSize;
    return __CARDEraseSector(chan, addr, EraseCallback);
}

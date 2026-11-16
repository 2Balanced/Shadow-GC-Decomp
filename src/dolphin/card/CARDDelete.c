/* Dolphin SDK CARD: deleting a file. The directory entry is blanked and written back first; once
 * that is on the card the file's block chain is freed. */
#include <dolphin/card_internal.h>

void *memset(void *dst, int value, unsigned long n);

static void DeleteCallback(s32 chan, s32 result) {
    CARDControl *card = &__CARDBlock[chan];
    CARDCallback callback = card->apiCallback;

    card->apiCallback = NULL;
    if (result < 0) {
        goto error;
    }
    result = __CARDFreeBlock(chan, card->startBlock, callback);
    if (result < 0) {
        goto error;
    }
    return;

error:
    fn_8038D1D4(card, result);
    if (callback) {
        callback(chan, result);
    }
}

s32 CARDDeleteAsync(s32 chan, const char *fileName, CARDCallback callback) {
    CARDControl *card;
    s32 fileNo;
    s32 result;
    CARDDir *dir;
    CARDDir *entry;

    result = fn_8038D11C(chan, &card);
    if (result < 0) {
        return result;
    }
    result = __CARDGetFileNo(card, fileName, &fileNo);
    if (result < 0) {
        return fn_8038D1D4(card, result);
    }
    if (__CARDIsOpened(card, fileNo)) {
        return fn_8038D1D4(card, CARD_RESULT_BUSY);
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    card->startBlock = entry->startBlock;
    memset(entry, 0xFF, sizeof(CARDDir));

    card->apiCallback = callback ? callback : fn_8038BFCC;
    result = __CARDUpdateDir(chan, DeleteCallback);
    if (result < 0) {
        fn_8038D1D4(card, result);
    }
    return result;
}

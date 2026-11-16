/* Dolphin SDK CARD: writing a file, one sector at a time: erase the sector, write it, then follow
 * the allocation chain to the next. When the last sector is done the file's time stamp is updated
 * and the directory written back. */
#include <dolphin/os.h>
#include <dolphin/card_internal.h>

u32 __OSBusClock : 0x800000F8;
#define OS_TIMER_CLOCK (__OSBusClock / 4)

#define OFFSET(n, a) (((u32)(n)) & ((a)-1))

#define CARDIsValidBlockNo(card, iBlock)                                                           \
    (CARD_NUM_SYSTEM_BLOCK <= (iBlock) && (iBlock) < (card)->cBlock)

#define file_offset transfer[0]
#define file_length transfer[1]

static void EraseCallback(s32 chan, s32 result);

static void WriteCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;
    u16 *fat;
    CARDDir *dir;
    CARDDir *entry;
    CARDFileInfo *fileInfo;

    card = &__CARDBlock[chan];
    if (result < 0) {
        goto error;
    }
    fileInfo = card->fileInfo;
    if (fileInfo->file_length < 0) {
        result = CARD_RESULT_CANCELED;
        goto error;
    }

    fileInfo->file_length -= card->sectorSize;
    if (fileInfo->file_length <= 0) {
        dir = __CARDGetDirBlock(card);
        entry = &dir[fileInfo->fileNo];
        entry->time = (u32)(OSGetTime() / OS_TIMER_CLOCK);
        callback = card->apiCallback;
        card->apiCallback = NULL;
        result = __CARDUpdateDir(chan, callback);
    } else {
        fat = __CARDGetFatBlock(card);
        fileInfo->file_offset += card->sectorSize;
        fileInfo->iBlock = fat[fileInfo->iBlock];
        if (!CARDIsValidBlockNo(card, fileInfo->iBlock)) {
            result = CARD_RESULT_BROKEN;
            goto error;
        }
        result = __CARDEraseSector(chan, card->sectorSize * (u32)fileInfo->iBlock, EraseCallback);
    }
    if (result < 0) {
        goto error;
    }
    return;

error:
    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

static void EraseCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;
    CARDFileInfo *fileInfo;

    card = &__CARDBlock[chan];
    if (result < 0) {
        goto error;
    }
    fileInfo = card->fileInfo;
    result = __CARDWrite(chan, card->sectorSize * (u32)fileInfo->iBlock, card->sectorSize,
                         card->buffer, WriteCallback);
    if (result < 0) {
        goto error;
    }
    return;

error:
    callback = card->apiCallback;
    card->apiCallback = NULL;
    __CARDPutControlBlock(card, result);
    callback(chan, result);
}

/* `length` and `offset` must be multiples of the card's sector size. */
s32 CARDWriteAsync(CARDFileInfo *fileInfo, const void *buffer, s32 length, s32 offset,
                   CARDCallback callback) {
    CARDControl *card;
    s32 result;
    CARDDir *dir;
    CARDDir *entry;

    result = __CARDSeek(fileInfo, length, offset, &card);
    if (result < 0) {
        return result;
    }
    if (OFFSET(offset, card->sectorSize) != 0 || OFFSET(length, card->sectorSize) != 0) {
        return __CARDPutControlBlock(card, CARD_RESULT_FATAL_ERROR);
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileInfo->fileNo];
    result = __CARDIsWritable(card, entry);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    DCStoreRange((void *)buffer, (u32)length);
    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;
    card->buffer = (void *)buffer;
    result = __CARDEraseSector(fileInfo->chan, card->sectorSize * (u32)fileInfo->iBlock, EraseCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}

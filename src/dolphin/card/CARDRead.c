/* Dolphin SDK CARD: reading a file. __CARDSeek walks the allocation chain to the block holding
 * the wanted offset; the read then goes block by block, following the chain from the completion
 * callback until the requested length is done. */
#include <dolphin/os.h>
#include <dolphin/card_internal.h>

#define CARD_SEG_SIZE 0x200

#define TRUNC(n, a) (((u32)(n)) & ~((a)-1))
#define OFFSET(n, a) (((u32)(n)) & ((a)-1))

#define CARDIsValidBlockNo(card, iBlock)                                                           \
    (CARD_NUM_SYSTEM_BLOCK <= (iBlock) && (iBlock) < (card)->cBlock)

#define file_offset transfer[0]
#define file_length transfer[1]

s32 __CARDSeek(CARDFileInfo *fileInfo, s32 length, s32 offset, CARDControl **pcard) {
    CARDControl *card;
    CARDDir *dir;
    CARDDir *entry;
    s32 result;
    u16 *fat;

    result = __CARDGetControlBlock(fileInfo->chan, &card);
    if (result < 0) {
        return result;
    }
    if (!CARDIsValidBlockNo(card, fileInfo->iBlock) ||
        card->cBlock * card->sectorSize <= fileInfo->file_offset) {
        return __CARDPutControlBlock(card, CARD_RESULT_FATAL_ERROR);
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileInfo->fileNo];
    if (entry->length * card->sectorSize <= offset ||
        entry->length * card->sectorSize < offset + length) {
        return __CARDPutControlBlock(card, CARD_RESULT_LIMIT);
    }

    card->fileInfo = fileInfo;
    fileInfo->file_length = length;
    if (offset < fileInfo->file_offset) {
        fileInfo->file_offset = 0;
        fileInfo->iBlock = entry->startBlock;
        if (!CARDIsValidBlockNo(card, fileInfo->iBlock)) {
            return __CARDPutControlBlock(card, CARD_RESULT_BROKEN);
        }
    }

    fat = __CARDGetFatBlock(card);
    while (fileInfo->file_offset < TRUNC(offset, card->sectorSize)) {
        fileInfo->file_offset += card->sectorSize;
        fileInfo->iBlock = fat[fileInfo->iBlock];
        if (!CARDIsValidBlockNo(card, fileInfo->iBlock)) {
            return __CARDPutControlBlock(card, CARD_RESULT_BROKEN);
        }
    }

    fileInfo->file_offset = offset;
    *pcard = card;
    return CARD_RESULT_READY;
}

static void ReadCallback(s32 chan, s32 result) {
    CARDControl *card;
    CARDCallback callback;
    u16 *fat;
    CARDFileInfo *fileInfo;
    s32 length;

    card = &__CARDBlock[chan];
    if (result < 0) {
        goto error;
    }
    fileInfo = card->fileInfo;
    if (fileInfo->file_length < 0) {
        result = CARD_RESULT_CANCELED;
        goto error;
    }

    length = (s32)TRUNC(fileInfo->file_offset + card->sectorSize, card->sectorSize) -
             fileInfo->file_offset;
    fileInfo->file_length -= length;
    if (fileInfo->file_length <= 0) {
        goto error;
    }

    fat = __CARDGetFatBlock(card);
    fileInfo->file_offset += length;
    fileInfo->iBlock = fat[fileInfo->iBlock];
    if (!CARDIsValidBlockNo(card, fileInfo->iBlock)) {
        result = CARD_RESULT_BROKEN;
        goto error;
    }
    result = __CARDRead(chan, card->sectorSize * (u32)fileInfo->iBlock,
                        (fileInfo->file_length < card->sectorSize) ? fileInfo->file_length
                                                                   : card->sectorSize,
                        card->buffer, ReadCallback);
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

/* `length` and `offset` must be multiples of 512 bytes. */
s32 CARDReadAsync(CARDFileInfo *fileInfo, void *buffer, s32 length, s32 offset,
                  CARDCallback callback) {
    CARDControl *card;
    s32 result;
    CARDDir *dir;
    CARDDir *entry;

    if (OFFSET(offset, CARD_SEG_SIZE) != 0 || OFFSET(length, CARD_SEG_SIZE) != 0) {
        return CARD_RESULT_FATAL_ERROR;
    }
    result = __CARDSeek(fileInfo, length, offset, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileInfo->fileNo];
    result = __CARDIsReadable(card, entry);
    if (result < 0) {
        return __CARDPutControlBlock(card, result);
    }

    DCInvalidateRange(buffer, (u32)length);
    card->apiCallback = callback ? callback : __CARDDefaultApiCallback;

    offset = (s32)OFFSET(fileInfo->file_offset, card->sectorSize);
    length = (length < card->sectorSize - offset) ? length : card->sectorSize - offset;
    result = __CARDRead(fileInfo->chan, card->sectorSize * (u32)fileInfo->iBlock + offset, length,
                        buffer, ReadCallback);
    if (result < 0) {
        __CARDPutControlBlock(card, result);
    }
    return result;
}

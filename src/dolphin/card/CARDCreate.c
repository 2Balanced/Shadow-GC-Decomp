/* Dolphin SDK CARD: creating a file. A free directory slot is picked and named, the blocks are
 * chained in the allocation table, and once that is on the card the directory entry is filled in
 * and written. */
#include <dolphin/os.h>
#include <dolphin/card_internal.h>

void *memcpy(void *dst, const void *src, unsigned long n);
unsigned long strlen(const char *s);
int fn_803A8DA8(const void *a, const void *b, unsigned long n);  /* memcmp */
char *fn_803ACABC(char *dst, const char *src, unsigned long n);  /* strncpy */

u32 __OSBusClock : 0x800000F8;
#define OS_TIMER_CLOCK (__OSBusClock / 4)

#define CARD_ATTR_PUBLIC 0x04

static void CreateCallbackFat(s32 chan, s32 result) {
    CARDControl *card = &__CARDBlock[chan];
    CARDDir *dir;
    CARDDir *entry;
    CARDCallback callback = card->apiCallback;

    card->apiCallback = NULL;
    if (result < 0) {
        goto error;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[card->freeNo];
    memcpy(entry->gameName, card->diskID->gameName, sizeof(entry->gameName));
    memcpy(entry->company, card->diskID->company, sizeof(entry->company));
    entry->permission = CARD_ATTR_PUBLIC;
    entry->copyTimes = 0;
    entry->startBlock = card->startBlock;
    entry->bannerFormat = 0;
    entry->iconAddr = 0xFFFFFFFF;
    entry->iconFormat = 0;
    entry->iconSpeed = 0;
    entry->commentAddr = 0xFFFFFFFF;
    entry->iconSpeed = (u16)((entry->iconSpeed & ~3) | 1);
    card->fileInfo->transfer[0] = 0;
    card->fileInfo->iBlock = entry->startBlock;
    entry->time = (u32)(OSGetTime() / OS_TIMER_CLOCK);

    result = __CARDUpdateDir(chan, callback);
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

/* `size` is in bytes and must be a whole number of the card's sectors. */
s32 CARDCreateAsync(s32 chan, const char *fileName, u32 size, CARDFileInfo *fileInfo,
                    CARDCallback callback) {
    CARDControl *card;
    CARDDir *dir;
    CARDDir *entry;
    u16 fileNo;
    u16 freeNo;
    u16 *fat;
    s32 result;

    if (strlen(fileName) > CARD_FILENAME_MAX) {
        return CARD_RESULT_NAMETOOLONG;
    }
    result = fn_8038D11C(chan, &card);
    if (result < 0) {
        return result;
    }
    if (size <= 0 || (size % card->sectorSize) != 0) {
        return CARD_RESULT_FATAL_ERROR;
    }

    freeNo = (u16)-1;
    dir = __CARDGetDirBlock(card);
    for (fileNo = 0; fileNo < CARD_MAX_FILE; fileNo++) {
        entry = &dir[fileNo];
        if (entry->gameName[0] == 0xFF) {
            if (freeNo == (u16)-1) {
                freeNo = fileNo;
            }
        } else if (fn_803A8DA8(entry->gameName, card->diskID->gameName, 4) == 0 &&
                   fn_803A8DA8(entry->company, card->diskID->company, 2) == 0 &&
                   __CARDCompareFileName(entry, fileName)) {
            return fn_8038D1D4(card, CARD_RESULT_EXIST);
        }
    }
    if (freeNo == (u16)-1) {
        return fn_8038D1D4(card, CARD_RESULT_NOENT);
    }

    fat = __CARDGetFatBlock(card);
    if (card->sectorSize * fat[CARD_FAT_FREEBLOCKS] < size) {
        return fn_8038D1D4(card, CARD_RESULT_INSSPACE);
    }

    card->apiCallback = callback ? callback : fn_8038BFCC;
    card->freeNo = freeNo;
    entry = &dir[freeNo];
    entry->length = (u16)(size / card->sectorSize);
    fn_803ACABC((char *)entry->fileName, fileName, CARD_FILENAME_MAX);

    card->fileInfo = fileInfo;
    fileInfo->chan = chan;
    fileInfo->fileNo = freeNo;

    result = __CARDAllocBlock(chan, size / card->sectorSize, CreateCallbackFat);
    if (result < 0) {
        return fn_8038D1D4(card, result);
    }
    return result;
}

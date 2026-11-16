/* Dolphin SDK CARD: finding and opening files. A file belongs to the game and maker codes in its
 * directory entry; other games get at it only through its public/company permission bits. */
#include <dolphin/card_internal.h>

int fn_803A8DA8(const void *a, const void *b, unsigned long n); /* memcmp */
#define memcmp fn_803A8DA8

#define __CARDDiskNone lbl_805A1E00
#define __CARDPermMask lbl_805EDC2A

#define CARD_ATTR_PUBLIC 0x04
#define CARD_ATTR_GLOBAL 0x20
#define CARD_ATTR_COMPANY 0x40

#define CARDIsValidBlockNo(card, iBlock)                                                               (CARD_NUM_SYSTEM_BLOCK <= (iBlock) && (iBlock) < (card)->cBlock)

BOOL __CARDCompareFileName(CARDDir *entry, const char *fileName) {
    char *entryName;
    char c1;
    char c2;
    int n;

    entryName = (char *)entry->fileName;
    n = CARD_FILENAME_MAX;
    while (0 <= --n) {
        if ((c1 = *entryName++) != (c2 = *fileName++)) {
            return FALSE;
        } else if (c2 == '\0') {
            return TRUE;
        }
    }
    if (*fileName == '\0') {
        return TRUE;
    }
    return FALSE;
}

s32 __CARDAccess(CARDControl *card, CARDDir *entry) {
    CARDDiskID *diskID = card->diskID;

    if (entry->gameName[0] == 0xFF) {
        return CARD_RESULT_NOFILE;
    }
    if (diskID == &__CARDDiskNone || (memcmp(entry->gameName, diskID->gameName, 4) == 0 &&
                                      memcmp(entry->company, diskID->company, 2) == 0)) {
        return CARD_RESULT_READY;
    }
    return CARD_RESULT_NOPERM;
}

s32 __CARDIsWritable(CARDControl *card, CARDDir *entry) {
    CARDDiskID *diskID = card->diskID;
    s32 result;
    u8 perm;

    result = __CARDAccess(card, entry);
    if (result == CARD_RESULT_NOPERM) {
        perm = (u8)(entry->permission & __CARDPermMask);
        if ((perm & CARD_ATTR_GLOBAL) &&
            (memcmp(entry->gameName, __CARDDiskNone.gameName, 4) == 0 &&
             memcmp(entry->company, __CARDDiskNone.company, 2) == 0)) {
            return CARD_RESULT_READY;
        }
        if ((perm & CARD_ATTR_COMPANY) &&
            (memcmp(entry->gameName, __CARDDiskNone.gameName, 4) == 0 &&
             memcmp(entry->company, diskID->company, 2) == 0)) {
            return CARD_RESULT_READY;
        }
    }
    return result;
}

s32 __CARDIsReadable(CARDControl *card, CARDDir *entry) {
    s32 result = __CARDIsWritable(card, entry);

    if (result == CARD_RESULT_NOPERM && (entry->permission & CARD_ATTR_PUBLIC)) {
        return CARD_RESULT_READY;
    }
    return result;
}

s32 __CARDGetFileNo(CARDControl *card, const char *fileName, s32 *fileNo) {
    CARDDir *dir;
    CARDDir *entry;
    s32 i;
    s32 result;

    if (!card->attached) {
        return CARD_RESULT_NOCARD;
    }
    dir = __CARDGetDirBlock(card);
    for (i = 0; i < CARD_MAX_FILE; i++) {
        entry = &dir[i];
        result = __CARDAccess(card, entry);
        if (result < 0) {
            continue;
        }
        if (__CARDCompareFileName(entry, fileName)) {
            *fileNo = i;
            return CARD_RESULT_READY;
        }
    }
    return CARD_RESULT_NOFILE;
}

s32 CARDOpen(s32 chan, const char *fileName, CARDFileInfo *fileInfo) {
    CARDControl *card;
    CARDDir *dir;
    CARDDir *entry;
    s32 result;
    s32 fileNo;

    fileInfo->chan = -1;
    result = fn_8038D11C(chan, &card);
    if (result < 0) {
        return result;
    }
    result = __CARDGetFileNo(card, fileName, &fileNo);
    if (0 <= result) {
        dir = __CARDGetDirBlock(card);
        entry = &dir[fileNo];
        if (!CARDIsValidBlockNo(card, entry->startBlock)) {
            result = CARD_RESULT_BROKEN;
        } else {
            fileInfo->chan = chan;
            fileInfo->fileNo = fileNo;
            fileInfo->transfer[0] = 0;
            fileInfo->iBlock = entry->startBlock;
        }
    }
    return fn_8038D1D4(card, result);
}

s32 CARDClose(CARDFileInfo *fileInfo) {
    CARDControl *card;
    s32 result;

    result = fn_8038D11C(fileInfo->chan, &card);
    if (result < 0) {
        return result;
    }
    fileInfo->chan = -1;
    return fn_8038D1D4(card, CARD_RESULT_READY);
}

/* No file is ever marked open in this version. */
BOOL __CARDIsOpened(CARDControl *card, s32 fileNo) { return FALSE; }

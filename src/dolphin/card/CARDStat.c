/* Dolphin SDK CARD: a file's status, the part of its directory entry a game may see and change
 * (banner and icon formats, where they and the comment sit in the file). */
#include <dolphin/os.h>
#include <dolphin/card_internal.h>

void *memcpy(void *dst, const void *src, unsigned long n);

u32 __OSBusClock : 0x800000F8;
#define OS_TIMER_CLOCK (__OSBusClock / 4)

/* The named views of CARDStat's raw words. */
#define stat_length sizeAndTime[0]
#define stat_time sizeAndTime[1]
#define stat_offsetBanner derivedOffsets[0]
#define stat_offsetBannerTlut derivedOffsets[1]
#define stat_offsetIcon(i) derivedOffsets[2 + (i)]
#define stat_offsetIconTlut derivedOffsets[10]
#define stat_offsetData derivedOffsets[11]

#define CARD_STAT_BANNER_C8 1
#define CARD_STAT_BANNER_RGB5A3 2
#define CARD_STAT_ICON_C8 1
#define CARD_STAT_ICON_RGB5A3 2

/* Lay the banner, its palette, the icon frames and their shared palette out one after another
 * from the icon address; what follows them is the file's own data. */
static void UpdateIconOffsets(CARDDir *entry, CARDStat *stat) {
    u32 offset;
    BOOL iconTlut;
    int i;

    offset = entry->iconAddr;
    if (offset == 0xFFFFFFFF) {
        stat->bannerFormat = 0;
        stat->iconFormat = 0;
        stat->iconSpeed = 0;
        offset = 0;
    }

    iconTlut = FALSE;
    switch (entry->bannerFormat & 3) {
    case CARD_STAT_BANNER_C8:
        stat->stat_offsetBanner = offset;
        offset += 0xC00;
        stat->stat_offsetBannerTlut = offset;
        offset += 0x200;
        break;
    case CARD_STAT_BANNER_RGB5A3:
        stat->stat_offsetBanner = offset;
        offset += 0x1800;
        stat->stat_offsetBannerTlut = 0xFFFFFFFF;
        break;
    default:
        stat->stat_offsetBanner = 0xFFFFFFFF;
        stat->stat_offsetBannerTlut = 0xFFFFFFFF;
        break;
    }

    for (i = 0; i < CARD_ICON_MAX; ++i) {
        switch ((entry->iconFormat >> (2 * i)) & 3) {
        case CARD_STAT_ICON_C8:
            stat->stat_offsetIcon(i) = offset;
            offset += 0x400;
            iconTlut = TRUE;
            break;
        case CARD_STAT_ICON_RGB5A3:
            stat->stat_offsetIcon(i) = offset;
            offset += 0x800;
            break;
        default:
            stat->stat_offsetIcon(i) = 0xFFFFFFFF;
            break;
        }
    }

    if (iconTlut) {
        stat->stat_offsetIconTlut = offset;
        offset += 0x200;
    } else {
        stat->stat_offsetIconTlut = 0xFFFFFFFF;
    }
    stat->stat_offsetData = offset;
}

s32 CARDGetStatus(s32 chan, s32 fileNo, CARDStat *stat) {
    CARDControl *card;
    CARDDir *dir;
    CARDDir *entry;
    s32 result;

    if (fileNo < 0 || CARD_MAX_FILE <= fileNo) {
        return CARD_RESULT_FATAL_ERROR;
    }
    result = fn_8038D11C(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    result = __CARDIsReadable(card, entry);
    if (result >= 0) {
        memcpy(stat->gameName, entry->gameName, sizeof(stat->gameName));
        memcpy(stat->company, entry->company, sizeof(stat->company));
        stat->stat_length = (u32)entry->length * card->sectorSize;
        memcpy(stat->fileName, entry->fileName, CARD_FILENAME_MAX);
        stat->stat_time = entry->time;
        stat->bannerFormat = entry->bannerFormat;
        stat->iconAddr = entry->iconAddr;
        stat->iconFormat = entry->iconFormat;
        stat->iconSpeed = entry->iconSpeed;
        stat->commentAddr = entry->commentAddr;
        UpdateIconOffsets(entry, stat);
    }
    return fn_8038D1D4(card, result);
}

s32 CARDSetStatusAsync(s32 chan, s32 fileNo, CARDStat *stat, CARDCallback callback) {
    CARDControl *card;
    CARDDir *dir;
    CARDDir *entry;
    s32 result;

    if (fileNo < 0 || CARD_MAX_FILE <= fileNo ||
        (stat->iconAddr != 0xFFFFFFFF && 0x200 <= stat->iconAddr) ||
        (stat->commentAddr != 0xFFFFFFFF &&
         CARD_SYSTEM_BLOCK_SIZE - 0x40 < stat->commentAddr % CARD_SYSTEM_BLOCK_SIZE)) {
        return CARD_RESULT_FATAL_ERROR;
    }
    result = fn_8038D11C(chan, &card);
    if (result < 0) {
        return result;
    }

    dir = __CARDGetDirBlock(card);
    entry = &dir[fileNo];
    result = __CARDIsWritable(card, entry);
    if (result < 0) {
        return fn_8038D1D4(card, result);
    }

    entry->bannerFormat = stat->bannerFormat;
    entry->iconAddr = stat->iconAddr;
    entry->iconFormat = stat->iconFormat;
    entry->iconSpeed = stat->iconSpeed;
    entry->commentAddr = stat->commentAddr;
    UpdateIconOffsets(entry, stat);

    if (entry->iconAddr == 0xFFFFFFFF) {
        entry->iconSpeed = (u16)((entry->iconSpeed & ~3) | 1);
    }
    entry->time = (u32)(OSGetTime() / OS_TIMER_CLOCK);

    result = __CARDUpdateDir(chan, callback);
    if (result < 0) {
        fn_8038D1D4(card, result);
    }
    return result;
}

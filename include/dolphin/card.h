#ifndef DOLPHIN_CARD_H
#define DOLPHIN_CARD_H

/* What the decompiled game code needs from the Dolphin SDK memory card library (CARD): result
 * codes, a few limits, the callback type and the two structures the game fills in. The library's
 * own functions keep their fn_ placeholder names; each unit declares the ones it calls. The names
 * are the SDK's public interface names, so code that uses them reads like any other CARD client. */

#include <dolphin/types.h>

/* Results returned by the CARD calls and passed to their callbacks: zero is success, negative
 * values are failures, CARD_RESULT_BUSY means an operation is still in progress. */
enum {
    CARD_RESULT_FATAL_ERROR = -128,
    CARD_RESULT_CANCELED = -14, CARD_RESULT_ENCODING = -13, CARD_RESULT_NAMETOOLONG = -12,
    CARD_RESULT_LIMIT = -11, CARD_RESULT_NOPERM = -10, CARD_RESULT_INSSPACE = -9,
    CARD_RESULT_NOENT = -8, CARD_RESULT_EXIST = -7, CARD_RESULT_BROKEN = -6,
    CARD_RESULT_IOERROR = -5, CARD_RESULT_NOFILE = -4, CARD_RESULT_NOCARD = -3,
    CARD_RESULT_WRONGDEVICE = -2, CARD_RESULT_BUSY = -1,
    CARD_RESULT_READY = 0, CARD_RESULT_UNLOCKED = 1
};

enum {
    CARD_FILENAME_MAX = 32,          /* bytes in a file name, terminator included if it fits */
    CARD_ICON_MAX = 8,               /* frames of an animated icon */
    CARD_WORKAREA_SIZE = 5 * 0x2000  /* the buffer CARDMount needs */
};

/* Called when an asynchronous CARD operation on slot `chan` finishes. */
typedef void (*CARDCallback)(s32 chan, s32 result);

/* An open file: its slot and directory entry, the current transfer (position and bytes left)
 * and the block that transfer is in. */
typedef struct CARDFileInfo {
    s32 chan;        /* 0x00 */
    s32 fileNo;      /* 0x04 */
    s32 transfer[2]; /* 0x08 */
    u16 iBlock;      /* 0x10 */
} CARDFileInfo;

/* A file's directory entry as CARDGetStatus reports it and CARDSetStatus takes it. Only the
 * fields the game sets are named; the rest are kept as raw words. */
typedef struct CARDStat {
    char fileName[CARD_FILENAME_MAX]; /* 0x00 */
    u32 sizeAndTime[2];               /* 0x20: byte length, then seconds since 2000 */
    u8 gameName[4];                   /* 0x28 */
    u8 company[2];                    /* 0x2C */
    u8 bannerFormat;                  /* 0x2E (0x2F is padding) */
    u32 iconAddr;                     /* 0x30: where banner and icons start in the file */
    u16 iconFormat;                   /* 0x34: two bits per icon frame */
    u16 iconSpeed;                    /* 0x36: two bits per icon frame */
    u32 commentAddr;                  /* 0x38: where the two comment lines are */
    u32 derivedOffsets[12];           /* 0x3C: banner, its palette, CARD_ICON_MAX icons, the icon
                                       * palette and the data, computed by CARDGetStatus */
} CARDStat;

#endif

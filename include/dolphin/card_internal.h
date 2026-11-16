#ifndef DOLPHIN_CARD_INTERNAL_H
#define DOLPHIN_CARD_INTERNAL_H

/* What the Dolphin SDK CARD library's own units share: the per-slot control block and the calls
 * between the units. Only the fields the decompiled units touch are named. */

#include <dolphin/os.h>
#include <dolphin/card.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CARD_SYSTEM_BLOCK_SIZE 0x2000
#define CARD_NUM_SYSTEM_BLOCK 5 /* header, two directory copies, two allocation-table copies */
#define CARD_MAX_FILE 127

/* Words of an allocation-table block ahead of the per-block links. */
enum { CARD_FAT_CHECKSUM, CARD_FAT_CHECKSUMINV, CARD_FAT_CHECKCODE, CARD_FAT_FREEBLOCKS,
       CARD_FAT_LASTSLOT };

/* One directory entry (0x40 bytes). */
typedef struct CARDDir {
    u8 gameName[4];                   /* 0x00 */
    u8 company[2];                    /* 0x04 */
    u8 padding0;                      /* 0x06 */
    u8 bannerFormat;                  /* 0x07 */
    u8 fileName[CARD_FILENAME_MAX];   /* 0x08 */
    u32 time;                         /* 0x28: seconds since 2000 */
    u32 iconAddr;                     /* 0x2C */
    u16 iconFormat;                   /* 0x30 */
    u16 iconSpeed;                    /* 0x32 */
    u8 permission;                    /* 0x34 */
    u8 copyTimes;                     /* 0x35 */
    u16 startBlock;                   /* 0x36 */
    u16 length;                       /* 0x38: in blocks */
    u8 padding1[2];                   /* 0x3A */
    u32 commentAddr;                  /* 0x3C */
} CARDDir;

/* The tail of a directory block, in the place of a 128th entry. */
typedef struct CARDDirCheck {
    u8 padding0[0x40 - 6];
    s16 checkCode;
    u16 checkSum;
    u16 checkSumInv;
} CARDDirCheck;

/* The start of the disc header: the game and maker codes that own a file. */
typedef struct CARDDiskID {
    char gameName[4];
    char company[2];
    u8 unk06[0x20 - 0x06];
} CARDDiskID;

/* The card's header block (the first 512 bytes of system block 0). */
typedef struct CARDID {
    u8 serial[32];   /* 0x000: flash ID mixed with the format time, then the time and settings */
    u16 deviceID;    /* 0x020 */
    u16 size;        /* 0x022 */
    u16 encode;      /* 0x024: font encoding of the file comments */
    u8 padding[470]; /* 0x026 */
    u16 checkSum;    /* 0x1FC */
    u16 checkSumInv; /* 0x1FE */
} CARDID;

typedef struct CARDControl {
    BOOL attached;              /* 0x00 */
    s32 result;                 /* 0x04: of the last operation; BUSY while one runs */
    u16 size;                   /* 0x08: megabits */
    u16 pageSize;               /* 0x0A */
    s32 sectorSize;             /* 0x0C */
    u16 cBlock;                 /* 0x10 */
    u16 vendorID;               /* 0x12 */
    s32 latency;                /* 0x14 */
    u8 id[12];                  /* 0x18: flash ID */
    s32 mountStep;              /* 0x24 */
    s32 formatStep;             /* 0x28 */
    u8 unk2C[0x80 - 0x2C];
    void *workArea;             /* 0x80 */
    CARDDir *currentDir;        /* 0x84 */
    u16 *currentFat;            /* 0x88 */
    u8 unk8C[0xAC - 0x8C];
    s32 repeat;                 /* 0xAC: pages or segments left in the transfer */
    u32 addr;                   /* 0xB0 */
    void *buffer;               /* 0xB4 */
    s32 xferred;                /* 0xB8 */
    u16 freeNo;                 /* 0xBC */
    u16 startBlock;             /* 0xBE */
    CARDFileInfo *fileInfo;     /* 0xC0: the file being created */
    CARDCallback extCallback;   /* 0xC4: card pulled out */
    u8 unkC8[4];
    CARDCallback exiCallback;   /* 0xCC */
    CARDCallback apiCallback;   /* 0xD0 */
    CARDCallback xferCallback;  /* 0xD4 */
    CARDCallback eraseCallback; /* 0xD8 */
    CARDCallback unlockCallback; /* 0xDC: run once the EXI bus is ours */
    OSAlarm alarm;              /* 0xE0 */
    u32 cid;                    /* 0x108: the EXI device ID */
    struct CARDDiskID *diskID;  /* 0x10C: whose files this slot may touch */
} CARDControl;

extern CARDControl __CARDBlock[2];
extern CARDDiskID lbl_805A1E00; /* the all-games disc ID (CARDBios) */
extern u8 lbl_805EDC2A;         /* permission bits honoured for other games' files (CARDBios) */

/* CARDBios and CARDCheck, still under their placeholder names. */
s32 fn_8038CCB0(s32 chan, CARDCallback callback);            /* read one 512-byte segment */
s32 fn_8038CDE4(s32 chan, CARDCallback callback);            /* write one page */
s32 fn_8038CF20(s32 chan, u32 addr, CARDCallback callback);  /* erase one sector */
s32 fn_8038D1D4(CARDControl *card, s32 result);              /* release the control block */
void fn_8038BFCC(s32 chan, s32 result);                      /* the do-nothing API callback */
s32 fn_8038D11C(s32 chan, CARDControl **card);               /* claim the control block */
s32 __CARDGetFileNo(CARDControl *card, const char *fileName, s32 *fileNo);
BOOL __CARDIsOpened(CARDControl *card, s32 fileNo);            /* is the file open? */
s32 __CARDIsWritable(CARDControl *card, CARDDir *entry);          /* may this game write the file? */
s32 __CARDIsReadable(CARDControl *card, CARDDir *entry);          /* may this game read the file? */
u16 fn_8038D0DC(void);                                       /* the console's font encoding */
void __CARDCheckSum(void *data, int length, u16 *checkSum, u16 *checkSumInv);

BOOL __CARDCompareFileName(CARDDir *entry, const char *fileName);
s32 __CARDVerify(CARDControl *card);
s32 __CARDSeek(CARDFileInfo *fileInfo, s32 length, s32 offset, CARDControl **pcard);
s32 __CARDRead(s32 chan, u32 addr, s32 length, void *dst, CARDCallback callback);
s32 __CARDWrite(s32 chan, u32 addr, s32 length, void *src, CARDCallback callback);
u16 *__CARDGetFatBlock(CARDControl *card);
s32 __CARDAllocBlock(s32 chan, u32 cBlock, CARDCallback callback);
s32 __CARDFreeBlock(s32 chan, u16 nBlock, CARDCallback callback);
s32 __CARDUpdateFatBlock(s32 chan, u16 *fat, CARDCallback callback);
CARDDir *__CARDGetDirBlock(CARDControl *card);
s32 __CARDUpdateDir(s32 chan, CARDCallback callback);

#ifdef __cplusplus
}
#endif

#endif

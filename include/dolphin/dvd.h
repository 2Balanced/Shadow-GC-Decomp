#ifndef DOLPHIN_DVD_H
#define DOLPHIN_DVD_H

/* The parts of the Dolphin SDK DVD library's interface the decompiled SDK units use. */

#include <dolphin/os.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The 32-byte header at the start of every disc (and copied to 0x80000000 by the boot ROM). */
typedef struct DVDDiskID {
    char gameName[4];
    char company[2];
    u8 diskNumber;
    u8 gameVersion;
    u8 streaming;
    u8 streamingBufSize;
    u8 padding[22];
} DVDDiskID;

typedef struct DVDCommandBlock DVDCommandBlock;
typedef void (*DVDCBCallback)(s32 result, DVDCommandBlock *block);

/* One queued request. `state` is DVD_STATE_*; while busy, `transferredSize` counts finished
 * bytes and `currTransferSize` the chunk in flight. */
struct DVDCommandBlock {
    DVDCommandBlock *next;
    DVDCommandBlock *prev;
    u32 command;
    s32 state;
    u32 offset;
    u32 length;
    void *addr;
    u32 currTransferSize;
    u32 transferredSize;
    DVDDiskID *id;
    DVDCBCallback callback;
    void *userData;
};

typedef struct DVDFileInfo DVDFileInfo;
typedef void (*DVDCallback)(s32 result, DVDFileInfo *fileInfo);

struct DVDFileInfo {
    DVDCommandBlock cb;
    u32 startAddr;
    u32 length;
    DVDCallback callback;
};

/* DVDCommandBlock.state */
#define DVD_STATE_FATAL_ERROR -1
#define DVD_STATE_END 0
#define DVD_STATE_BUSY 1
#define DVD_STATE_WAITING 2
#define DVD_STATE_COVER_CLOSED 3
#define DVD_STATE_NO_DISK 4
#define DVD_STATE_COVER_OPEN 5
#define DVD_STATE_WRONG_DISK 6
#define DVD_STATE_MOTOR_STOPPED 7
#define DVD_STATE_PAUSING 8
#define DVD_STATE_IGNORED 9
#define DVD_STATE_CANCELED 10
#define DVD_STATE_RETRY 11

/* Results passed to command callbacks and returned by the synchronous calls. */
#define DVD_RESULT_GOOD 0
#define DVD_RESULT_FATAL_ERROR -1
#define DVD_RESULT_IGNORED -2
#define DVD_RESULT_CANCELED -3

#define DVD_MIN_TRANSFER_SIZE 32

/* Completion causes passed to a low-level callback (bit mask). */
#define DVD_INTTYPE_TC    1  /* transfer complete */
#define DVD_INTTYPE_DE    2  /* drive error */
#define DVD_INTTYPE_CVR   4  /* cover opened or closed */
#define DVD_INTTYPE_BRK   8  /* break request acknowledged */
#define DVD_INTTYPE_TIMEOUT 16

typedef void (*DVDLowCallback)(u32 intType);

DVDDiskID *fn_8037C9EC(void); /* the current disc's header */

/* dvdlow.c */
void __DVDInitWA(void);
void __DVDInterruptHandler(s16 interrupt, OSContext *context);
BOOL fn_80379054(void *addr, u32 length, u32 offset, DVDLowCallback callback); /* read */
BOOL fn_803792EC(u32 offset, DVDLowCallback callback);         /* seek */
BOOL DVDLowWaitCoverClose(DVDLowCallback callback);
BOOL fn_803793AC(DVDDiskID *diskID, DVDLowCallback callback); /* read disc ID */
BOOL DVDLowStopMotor(DVDLowCallback callback);
BOOL fn_803794DC(DVDLowCallback callback);                     /* request error */
BOOL fn_80379568(void *info, DVDLowCallback callback);         /* inquiry */
BOOL fn_80379604(u32 subcmd, u32 length, u32 offset, DVDLowCallback callback); /* audio stream */
BOOL fn_8037969C(u32 subcmd, DVDLowCallback callback);         /* audio status */
BOOL fn_80379728(BOOL enable, u32 size, DVDLowCallback callback); /* audio buffer config */
void DVDLowReset(void);
BOOL fn_80379880(void);                                        /* break */
DVDLowCallback fn_80379894(void);                              /* clear callback */
void __DVDLowSetWAType(u32 type, u32 location);
BOOL fn_803798F4(OSAlarm *alarm);                              /* test alarm */

/* dvdfs.c */
void __DVDFSInit(void);
s32 fn_80379964(const char *pathPtr);                          /* path to entry number */
BOOL fn_80379C58(const char *fileName, DVDFileInfo *fileInfo); /* open */
BOOL fn_80379D20(DVDFileInfo *fileInfo);                       /* close */
BOOL fn_80379EA4(s32 entrynum, char *path, u32 maxlen);        /* entry number to path */
BOOL fn_80379FF8(char *path, u32 maxlen);                      /* current directory */
BOOL fn_8037A0BC(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, DVDCallback callback,
                 s32 prio);                                    /* read async */
s32 fn_8037A1AC(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 prio); /* read */
s32 fn_8037A44C(DVDFileInfo *fileInfo);                        /* transferred size */

/* dvd.c */
BOOL fn_8037C0F8(DVDCommandBlock *block, void *addr, s32 length, s32 offset, DVDCBCallback callback,
                 s32 prio); /* absolute read, async */
s32 fn_8037C91C(DVDCommandBlock *block); /* cancel */
void DVDInit(void);
void DVDReset(void);
BOOL DVDReadDiskID(DVDCommandBlock *block, DVDDiskID *diskID, DVDCBCallback callback);
BOOL DVDInquiryAsync(DVDCommandBlock *block, void *info, DVDCBCallback callback);
s32 DVDGetDriveStatus(void);
BOOL __DVDTestAlarm(OSAlarm *alarm);

/* dvdqueue.c */
void __DVDClearWaitingQueue(void);
BOOL __DVDPushWaitingQueue(s32 prio, DVDCommandBlock *block);
DVDCommandBlock *__DVDPopWaitingQueue(void);
BOOL __DVDCheckWaitingQueue(void);
BOOL fn_8037CDD8(DVDCommandBlock *block); /* dequeue */

/* dvderror.c */
void __DVDStoreErrorCode(u32 error);

/* dvdidutils.c */
BOOL fn_8037CFD0(DVDDiskID *id1, DVDDiskID *id2); /* compare disc IDs */

/* dvdFatal.c */
void fn_8037D1C4(void);

/* fstload.c */
void __fstLoad(void);

#ifdef __cplusplus
}
#endif

#endif

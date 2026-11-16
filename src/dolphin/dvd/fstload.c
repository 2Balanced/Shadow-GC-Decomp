/* Dolphin SDK DVD: loads the file system table when the program was started by a debugger loader
 * instead of the boot ROM (DVDInit sees that from the boot magic). The disc ID, then the second
 * boot block, then the FST itself are read by chained callbacks while __fstLoad polls the drive
 * status; the FST lands at the address the disc header asks for, which becomes the arena top. */
#include <dolphin/dvd/__dvd.h>

typedef struct DVDBB2 {
    u32 bootFilePosition;
    u32 FSTPosition;
    u32 FSTLength;
    u32 FSTMaxLength;
    void *FSTAddress;
    u32 userPosition;
    u32 userLength;
    u32 padding0;
} DVDBB2;

typedef struct OSBootInfo {
    DVDDiskID DVDDiskID;
    u32 magic;
    u32 version;
    u32 memorySize;
    u32 consoleType;
    void *arenaLo;
    void *arenaHi;
    void *FSTLocation;
    u32 FSTMaxLength;
} OSBootInfo;

void *OSGetArenaHi(void);
void OSSetArenaHi(void *addr);
void *memcpy(void *dest, const void *src, u32 n);
BOOL fn_8037C1D4(DVDCommandBlock *block, void *addr, s32 length, s32 offset, DVDCBCallback callback);

#define ROUND_UP_32(x) (((u32)(x) + 31) & ~31)

static s32 status;
static DVDBB2 *bb2;
static DVDDiskID *idTmp;
static u8 bb2Buf[sizeof(DVDBB2) + 31];

static void cb(s32 result, DVDCommandBlock *block) {
    if (result > 0) {
        switch (status) {
        case 0:
            status = 1;
            fn_8037C1D4(block, bb2, ROUND_UP_32(sizeof(DVDBB2)), 0x420, cb);
            break;
        case 1:
            status = 2;
            fn_8037C1D4(block, bb2->FSTAddress, ROUND_UP_32(bb2->FSTLength), bb2->FSTPosition, cb);
            break;
        }
    } else if (result == -1) {
    } else if (result == -4) {
        status = 0;
        DVDReset();
        DVDReadDiskID(block, idTmp, cb);
    }
}

void __fstLoad(void) {
    OSBootInfo *bootInfo;
    DVDDiskID *id;
    u8 idTmpBuf[sizeof(DVDDiskID) + 31];
    static DVDCommandBlock block;
    void *arenaHi;

    arenaHi = OSGetArenaHi();
    bootInfo = (OSBootInfo *)OSPhysicalToCached(0);

    idTmp = (DVDDiskID *)ROUND_UP_32(idTmpBuf);
    bb2 = (DVDBB2 *)ROUND_UP_32(bb2Buf);

    DVDReset();
    DVDReadDiskID(&block, idTmp, cb);
    while (DVDGetDriveStatus() != 0) {
    }

    bootInfo->FSTLocation = bb2->FSTAddress;
    bootInfo->FSTMaxLength = bb2->FSTMaxLength;

    id = &bootInfo->DVDDiskID;
    memcpy(id, idTmp, sizeof(DVDDiskID));

    OSReport("\n");
    OSReport("  Game Name ... %c%c%c%c\n", id->gameName[0], id->gameName[1], id->gameName[2],
             id->gameName[3]);
    OSReport("  Company ..... %c%c\n", id->company[0], id->company[1]);
    OSReport("  Disk # ...... %d\n", id->diskNumber);
    OSReport("  Game ver .... %d\n", id->gameVersion);
    OSReport("  Streaming ... %s\n", (id->streaming == 0) ? "OFF" : "ON");
    OSReport("\n");

    OSSetArenaHi(bb2->FSTAddress);
}

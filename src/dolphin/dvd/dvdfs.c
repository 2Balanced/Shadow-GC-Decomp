/* Dolphin SDK DVD: the file-system layer. The boot ROM leaves the disc's file system table (FST)
 * in memory; this unit looks names up in it and turns file-relative reads into absolute ones.
 *
 * FST entry layout, 12 bytes: word 0 holds the directory flag in its top byte and the offset of
 * the name in the string table below it; for a file, words 1 and 2 are the disc position and the
 * length; for a directory, the parent's entry number and the entry number just past its last
 * child. Entry 0 is the root. */
#include <dolphin/dvd/__dvd.h>

typedef struct FSTEntry {
    u32 isDirAndStringOff;
    u32 parentOrPosition;
    u32 nextEntryOrLength;
} FSTEntry;

typedef struct DVDDir {
    u32 entryNum;
    u32 location;
    u32 next;
} DVDDir;

typedef struct DVDDirEntry {
    u32 entryNum;
    BOOL isDir;
    char *name;
} DVDDirEntry;

/* Boot information block at 0x80000000; the FST pointer sits at offset 0x38. */
typedef struct OSBootInfo {
    DVDDiskID diskID;
    u32 magic;
    u32 version;
    u32 memorySize;
    u32 consoleType;
    void *arenaLo;
    void *arenaHi;
    FSTEntry *FSTLocation;
    u32 FSTMaxLength;
} OSBootInfo;

#define entryIsDir(i) (((FstStart[i].isDirAndStringOff & 0xFF000000) == 0) ? FALSE : TRUE)
#define stringOff(i) (FstStart[i].isDirAndStringOff & 0x00FFFFFF)
#define parentDir(i) (FstStart[i].parentOrPosition)
#define nextDir(i) (FstStart[i].nextEntryOrLength)
#define filePosition(i) (FstStart[i].parentOrPosition)
#define fileLength(i) (FstStart[i].nextEntryOrLength)

static OSBootInfo *BootInfo;
static FSTEntry *FstStart;
static char *FstStringStart;
static u32 MaxEntryNum;
static u32 currentDirectory;
OSThreadQueue __DVDThreadQueue;
u32 __DVDLongFileNameFlag;

void __DVDFSInit(void) {
    BootInfo = (OSBootInfo *)OSPhysicalToCached(0);
    FstStart = BootInfo->FSTLocation;
    if (FstStart) {
        MaxEntryNum = FstStart[0].nextEntryOrLength;
        FstStringStart = (char *)&FstStart[MaxEntryNum];
    }
}

/* TRUE when the path component at `path` (ended by '/' or the string's end) is `string`,
 * ignoring case. */
static inline BOOL isSame(const char *path, const char *string) {
    while (*string != '\0') {
        if (tolower(*path++) != tolower(*string++)) {
            return FALSE;
        }
    }
    if (*path == '/' || *path == '\0') {
        return TRUE;
    }
    return FALSE;
}

s32 fn_80379964(const char *pathPtr) {
    const char *ptr;
    char *stringPtr;
    BOOL isDir;
    u32 length;
    u32 dirLookAt;
    u32 i;
    const char *origPathPtr = pathPtr;
    const char *extentionStart;
    BOOL illegal;
    BOOL extention;

    dirLookAt = currentDirectory;

    while (TRUE) {
        if (*pathPtr == '\0') {
            return (s32)dirLookAt;
        } else if (*pathPtr == '/') {
            dirLookAt = 0;
            pathPtr++;
            continue;
        } else if (*pathPtr == '.') {
            if (*(pathPtr + 1) == '.') {
                if (*(pathPtr + 2) == '/') {
                    dirLookAt = parentDir(dirLookAt);
                    pathPtr += 3;
                    continue;
                } else if (*(pathPtr + 2) == '\0') {
                    return (s32)parentDir(dirLookAt);
                }
            } else if (*(pathPtr + 1) == '/') {
                pathPtr += 2;
                continue;
            } else if (*(pathPtr + 1) == '\0') {
                return (s32)dirLookAt;
            }
        }

        if (__DVDLongFileNameFlag == 0) {
            /* Names must be 8.3. */
            extention = FALSE;
            illegal = FALSE;
            for (ptr = pathPtr; *ptr != '\0' && *ptr != '/'; ptr++) {
                if (*ptr == '.') {
                    if (ptr - pathPtr > 8 || extention == TRUE) {
                        illegal = TRUE;
                        break;
                    }
                    extention = TRUE;
                    extentionStart = ptr + 1;
                } else if (*ptr == ' ') {
                    illegal = TRUE;
                }
            }
            if (extention == TRUE && ptr - extentionStart > 3) {
                illegal = TRUE;
            }
            if (illegal) {
                OSPanic(__FILE__, 387,
                        "DVDConvertEntrynumToPath(possibly DVDOpen or DVDChangeDir or DVDOpenDir): "
                        "specified directory or file (%s) doesn't match standard 8.3 format. This is "
                        "a temporary restriction and will be removed soon\n",
                        origPathPtr);
            }
        } else {
            for (ptr = pathPtr; *ptr != '\0' && *ptr != '/'; ptr++) {
            }
        }

        isDir = (*ptr == '\0') ? FALSE : TRUE;
        length = (u32)(ptr - pathPtr);
        ptr = pathPtr;

        for (i = dirLookAt + 1; i < nextDir(dirLookAt); i = entryIsDir(i) ? nextDir(i) : (i + 1)) {
            if (entryIsDir(i) == FALSE && isDir == TRUE) {
                continue;
            }
            stringPtr = FstStringStart + stringOff(i);
            if (isSame(ptr, stringPtr) == TRUE) {
                goto next_hier;
            }
        }
        return -1;

    next_hier:
        if (!isDir) {
            return (s32)i;
        }
        dirLookAt = i;
        pathPtr += length + 1;
    }
}

BOOL fn_80379C58(const char *fileName, DVDFileInfo *fileInfo) {
    s32 entry;
    char currentDir[128];

    entry = fn_80379964(fileName);
    if (entry < 0) {
        fn_80379FF8(currentDir, 128);
        OSReport("Warning: DVDOpen(): file '%s' was not found under %s.\n", fileName, currentDir);
        return FALSE;
    }
    if (entryIsDir(entry)) {
        return FALSE;
    }
    fileInfo->startAddr = filePosition(entry);
    fileInfo->length = fileLength(entry);
    fileInfo->callback = NULL;
    fileInfo->cb.state = DVD_STATE_END;
    return TRUE;
}

BOOL fn_80379D20(DVDFileInfo *fileInfo) {
    fn_8037C91C(&fileInfo->cb);
    return TRUE;
}

/* Copies at most `maxlen` characters of `src`; returns how many were copied. */
static inline u32 myStrncpy(char *dest, const char *src, u32 maxlen) {
    u32 i = maxlen;

    while (i > 0 && *src != '\0') {
        *dest++ = *src++;
        i--;
    }
    return maxlen - i;
}

/* Writes the path of `entry` without a trailing slash; returns its length, or `maxlen` when it
 * didn't fit. */
static u32 fn_80379D44(u32 entry, char *path, u32 maxlen) {
    char *name;
    u32 loc;

    if (entry == 0) {
        return 0;
    }
    name = FstStringStart + stringOff(entry);
    loc = fn_80379D44(parentDir(entry), path, maxlen);
    if (loc == maxlen) {
        return loc;
    }
    *(path + loc++) = '/';
    loc += myStrncpy(path + loc, name, maxlen - loc);
    return loc;
}

BOOL fn_80379EA4(s32 entrynum, char *path, u32 maxlen) {
    u32 loc;

    loc = fn_80379D44((u32)entrynum, path, maxlen);
    if (loc == maxlen) {
        path[maxlen - 1] = '\0';
        return FALSE;
    }
    if (entryIsDir(entrynum)) {
        if (loc == maxlen - 1) {
            path[loc] = '\0';
            return FALSE;
        }
        path[loc++] = '/';
    }
    path[loc] = '\0';
    return TRUE;
}

BOOL fn_80379FF8(char *path, u32 maxlen) {
    return fn_80379EA4((s32)currentDirectory, path, maxlen);
}

static void fn_8037A17C(s32 result, DVDCommandBlock *block);

BOOL fn_8037A0BC(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, DVDCallback callback,
                 s32 prio) {
    if (!(0 <= offset && offset <= fileInfo->length)) {
        OSPanic(__FILE__, 750, "DVDReadAsync(): specified area is out of the file  ");
    }
    if (!(0 <= offset + length && offset + length < fileInfo->length + DVD_MIN_TRANSFER_SIZE)) {
        OSPanic(__FILE__, 756, "DVDReadAsync(): specified area is out of the file  ");
    }
    fileInfo->callback = callback;
    fn_8037C0F8(&fileInfo->cb, addr, length, fileInfo->startAddr + offset, fn_8037A17C, prio);
    return TRUE;
}

static void fn_8037A17C(s32 result, DVDCommandBlock *block) {
    DVDFileInfo *fileInfo = (DVDFileInfo *)block;

    if (fileInfo->callback) {
        fileInfo->callback(result, fileInfo);
    }
}

static void fn_8037A2C4(s32 result, DVDCommandBlock *block);

s32 fn_8037A1AC(DVDFileInfo *fileInfo, void *addr, s32 length, s32 offset, s32 prio) {
    BOOL result;
    DVDCommandBlock *block;
    s32 state;
    BOOL enabled;
    s32 retVal;

    if (!(0 <= offset && offset <= fileInfo->length)) {
        OSPanic(__FILE__, 820, "DVDRead(): specified area is out of the file  ");
    }
    if (!(0 <= offset + length && offset + length < fileInfo->length + DVD_MIN_TRANSFER_SIZE)) {
        OSPanic(__FILE__, 826, "DVDRead(): specified area is out of the file  ");
    }

    block = &fileInfo->cb;
    result = fn_8037C0F8(block, addr, length, fileInfo->startAddr + offset, fn_8037A2C4, prio);
    if (result == FALSE) {
        return -1;
    }

    enabled = OSDisableInterrupts();
    while (TRUE) {
        state = block->state;
        if (state == DVD_STATE_END) {
            retVal = (s32)block->transferredSize;
            break;
        }
        if (state == DVD_STATE_FATAL_ERROR) {
            retVal = DVD_RESULT_FATAL_ERROR;
            break;
        }
        if (state == DVD_STATE_CANCELED) {
            retVal = DVD_RESULT_CANCELED;
            break;
        }
        OSSleepThread(&__DVDThreadQueue);
    }
    OSRestoreInterrupts(enabled);
    return retVal;
}

static void fn_8037A2C4(s32 result, DVDCommandBlock *block) {
    OSWakeupThread(&__DVDThreadQueue);
}

BOOL fn_8037A2E8(const char *dirName, DVDDir *dir) {
    s32 entry;
    char currentDir[128];

    entry = fn_80379964(dirName);
    if (entry < 0) {
        fn_80379FF8(currentDir, 128);
        OSReport("Warning: DVDOpenDir(): file '%s' was not found under %s.\n", dirName, currentDir);
        return FALSE;
    }
    if (!entryIsDir(entry)) {
        return FALSE;
    }
    dir->entryNum = (u32)entry;
    dir->location = (u32)entry + 1;
    dir->next = nextDir(entry);
    return TRUE;
}

BOOL fn_8037A3A8(DVDDir *dir, DVDDirEntry *dirent) {
    u32 loc = dir->location;

    if (loc <= dir->entryNum || dir->next <= loc) {
        return FALSE;
    }
    dirent->entryNum = loc;
    dirent->isDir = entryIsDir(loc);
    dirent->name = FstStringStart + stringOff(loc);
    dir->location = entryIsDir(loc) ? nextDir(loc) : (loc + 1);
    return TRUE;
}

s32 fn_8037A44C(DVDFileInfo *fileInfo) {
    s32 bytes;
    DVDCommandBlock *cb = &fileInfo->cb;

    switch (cb->state) {
    case DVD_STATE_FATAL_ERROR:
    case DVD_STATE_END:
    case DVD_STATE_COVER_CLOSED:
    case DVD_STATE_NO_DISK:
    case DVD_STATE_COVER_OPEN:
    case DVD_STATE_WRONG_DISK:
    case DVD_STATE_MOTOR_STOPPED:
    case DVD_STATE_CANCELED:
    case DVD_STATE_RETRY:
        bytes = (s32)cb->transferredSize;
        break;
    case DVD_STATE_WAITING:
        bytes = 0;
        break;
    case DVD_STATE_BUSY:
        bytes = (s32)(cb->transferredSize + (cb->currTransferSize - __DIRegs[6]));
        break;
    }
    return bytes;
}

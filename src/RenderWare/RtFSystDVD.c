/* RenderWare file system toolkit (RtFSyst), the GameCube DVD file system, 0x8047A7C8..0x8047B720:
 * its file objects, open, close, reads through the file's 32-byte aligned buffer, seek, status and
 * abort, and its constructor (fn_8047B40C), which stores all of them in the file system object.
 *
 * One source file: its .data is the jump table of the read's status switch, the read's messages
 * and the two jump tables of the status function (0x8056DD80..0x8056DEE0, reached through one base
 * register), and its .sdata the last DVD error and the empty root path (0x805EDE90..0x805EDE98).
 *
 * NonMatching: fn_8047AC10 (the read: retail's frame is 0x10 larger and its 64-bit position
 * arithmetic uses other registers).
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address.
 * C.
 * Flags: GC/2.7 -O4,p, no stmw/lmw, automatic inlining, -lang=c, writable pooled strings
 * (cflags_rw_r5). Measured per file against the other RenderWare flag sets: GC 2.0-2.7 give the
 * same code, GC 1.3.2, -O4,s, stmw/lmw and -inline noauto match fewer functions, and the
 * read-only string sets (cflags_rw_r3/r6/c) would move strings out of .data/.sdata. */
#include "Game/rw/r5.h"

void *memcpy(void *dst, const void *src, u32 n);
void OSReport(const char *format, ...);
s32 DVDGetDriveStatus(void);
RwBool fn_80379C58(const char *name, R5DVDFileInfo *info); /* DVDOpen */
RwBool fn_80379D20(R5DVDFileInfo *info);                   /* DVDClose */
RwBool fn_8037A0BC(R5DVDFileInfo *info, void *buffer, s32 length, s32 offset, void *callback, s32 prio);
                                                           /* DVDReadAsyncPrio */
s32 fn_8037C548(R5DVDFileInfo *info);                      /* DVDGetCommandBlockStatus */
s32 fn_8037C6A0(R5DVDFileInfo *info, void *callback);      /* DVDCancelAsync */
void fn_80377BC0(void);                                    /* waits for the drive */

const char *fn_80479698(const char *name);
char *fn_8047971C(char *buffer, s32 size, const char *prefix, const char *name, char separator);
RtFileSystem *fn_80479C74(const char *name);
s32 fn_80479598(RtFileSystem *fs, s32 maxNbOpenFiles, const char *name, const char *deviceName);
void fn_804799C8(RtFile *file);
RwBool fn_80479848(RtFile *file);

/* The last DVD error. */
volatile s32 lbl_805EDE90 = 1;

/* Its file objects. */
RtFile *fn_8047A7C8(RtFileSystem *fs, u32 index) {
    if (index < fs->maxNbOpenFiles) {
        return &fs->files[index];
    }
    return 0;
}

/* Its root path. */
const char *fn_8047A7EC(RtFileSystem *fs) {
    return "";
}

/* Closes it. */
void fn_8047A7F4(RtFileSystem *fs) {
    RWSRCGLOBAL(rwfree)(fs->files);
}

/* Opens a file on the DVD (reading only). Returns an RtFSyst error code. */
s32 fn_8047A824(RtFileSystem *fs, RtFile *file, const char *name, u32 flags) {
    char path[0x100];
    const char *fileName;

    if (flags & 0xE) {
        return 4;
    }
    fileName = fn_80479698(name);
    if (fn_8047971C(path, sizeof(path), fs->getRootPath(fs), fileName, '/') == 0) {
        return 5;
    }
    if (fn_80379C58(path, &file->dvdInfo) == 0) {
        return 3;
    }
    file->status = 2;
    file->field_0x24 = 1;
    file->position = 0;
    file->bufferValid = 0;
    file->bufferPos = 0;
    file->length = file->dvdInfo.length;
    if (flags & 0x10) {
        file->field_0x18 = 1;
        file->status = 3;
    } else {
        file->field_0x18 = 0;
        file->status = 2;
    }
    file->field_0x1C = 2;
    return 1;
}

/* Closes a file on the DVD. */
void fn_8047A938(RtFile *file) {
    fn_80379D20(&file->dvdInfo);
    file->status = 1;
}

/* The completion callback of the reads: keeps the result in the command block. */
void fn_8047A970(s32 result, R5DVDFileInfo *info) {
    info->userData = result;
}

/* Reads `size` bytes (rounded up to 32) at `offset`: returns at once for an asynchronous file,
 * else waits for the drive, reporting its errors through the file system's error callback. */
s32 fn_8047A978(RtFile *file, void *buffer, s32 size, s32 offset, s32 async) {
    s32 length = (size + 31) & ~31;
    RtFileSystem *fs = file->fs;
    s32 status;

    RwBool ok = fn_8037A0BC(&file->dvdInfo, buffer, length, offset, fn_8047A970, file->field_0x1C);

    if (file->field_0x18 != 0) {
        if (ok) {
            file->status = 3;
            lbl_805EDE90 = 1;
            return length;
        }
        return 0;
    }
    do {
        fn_80377BC0();
        status = DVDGetDriveStatus();
        switch (status) {
        case -1:
            file->status = 4;
            fs->status = 7;
            lbl_805EDE90 = 2;
            if (fs->errorCallBack != 0) {
                fs->errorCallBack(2, fs->errorCallBackData);
            }
            break;
        case 4:
            file->status = 4;
            fs->status = 4;
            lbl_805EDE90 = 3;
            if (fs->errorCallBack != 0) {
                fs->errorCallBack(3, fs->errorCallBackData);
            }
            break;
        case 5:
            file->status = 4;
            fs->status = 3;
            lbl_805EDE90 = 4;
            if (fs->errorCallBack != 0) {
                fs->errorCallBack(4, fs->errorCallBackData);
            }
            break;
        case 6:
            file->status = 4;
            fs->status = 5;
            lbl_805EDE90 = 5;
            if (fs->errorCallBack != 0) {
                fs->errorCallBack(5, fs->errorCallBackData);
            }
            break;
        case 11:
            file->status = 4;
            fs->status = 5;
            lbl_805EDE90 = 6;
            if (fs->errorCallBack != 0) {
                fs->errorCallBack(6, fs->errorCallBackData);
            }
            break;
        case 1:
            if (lbl_805EDE90 != 1 || file->status == 4) {
                if (status == 1) {
                    fs->status = 1;
                    lbl_805EDE90 = 8;
                } else {
                    fs->status = 2;
                    lbl_805EDE90 = 1;
                }
            } else {
                file->status = 3;
                lbl_805EDE90 = 1;
            }
            break;
        case 0:
        case 2:
        case 7:
        case 8:
            file->status = 4;
            if (fs->errorCallBack != 0) {
                fs->errorCallBack(7, fs->errorCallBackData);
            }
            break;
        default:
            if (fs->errorCallBack != 0) {
                fs->errorCallBack(7, fs->errorCallBackData);
            }
            break;
        }
    } while (status != 0);
    if (status == 0) {
        s32 result = file->dvdInfo.userData;

        if (result < 0) {
            result = 0;
        }
        file->status = 2;
        return result;
    }
    return 0;
}

/* Reads from a file on the DVD: whole aligned blocks straight into `buffer`, the rest through
 * the file's 32-byte aligned buffer. Returns the number of bytes read. */
u32 fn_8047AC10(RtFile *file, void *buffer, u32 size) {
    s32 retries = 0;
    s32 remaining;
    s32 bufferMisalign;
    s32 fileMisalign;

    file->startPosition = file->position;
    if (file->field_0x18 != 0) {
        file->asyncStatus = 5;
    }
    if (size == 0) {
        return 0;
    }
    if (file->position + size > file->length) {
        size = (u32)file->length - (u32)file->position;
        if (file->field_0x18 != 0) {
            size = (size + 31) & ~31;
        }
    }
    if (size == 0) {
        return 0;
    }
    remaining = size;
    bufferMisalign = (u32)buffer & 31;
    fileMisalign = (u32)file->position & 31;
    if (file->field_0x18 != 0) {
        s32 sizeMisalign = (s32)size % 32;

        if (bufferMisalign == 0 && fileMisalign == 0 && sizeMisalign == 0) {
            while (remaining > 0 && retries < 1) {
                remaining = fn_8047A978(file, buffer, remaining, (u32)file->position, file->field_0x18);
                if (remaining > 0) {
                    file->position += remaining;
                    remaining = 0;
                    retries = 0;
                    file->bufferValid = 0;
                    file->bufferPos = 0;
                } else {
                    retries++;
                }
            }
            if (remaining > 0) {
                return 0;
            }
            return size;
        }
        OSReport("Unaligned async read failed\n");
        if (bufferMisalign != 0) {
            OSReport("Buffer offset not multiple of 32 %p\n", buffer);
        }
        if (fileMisalign != 0) {
            OSReport("File pointer offset not multiple of 32 %d\n", file->position);
        }
        if (sizeMisalign != 0) {
            OSReport("Num bytes to read not multiple of 32 %d\n", remaining);
        }
        return 0;
    }
    while (remaining > 0 && retries < 1) {
        fn_80377BC0();
        if (file->position < 0 || !(file->position < file->length)) {
            return 0;
        }
        if (bufferMisalign == 0 && fileMisalign == 0 && remaining >= 32) {
            s32 count = fn_8047A978(file, buffer, remaining & ~31, (u32)file->position, file->field_0x18);

            if (count > 0) {
                file->position += count;
                remaining -= count;
                buffer = (u8 *)buffer + count;
                retries = 0;
                file->bufferValid = 0;
                file->bufferPos = 0;
            } else {
                retries++;
            }
        } else if (file->field_0x18 != 0) {
            OSReport("Error: Unaligned async read failed\n");
            return 0;
        } else if (file->bufferValid == 1) {
            s32 count = file->bufferSize - file->bufferPos;

            if (remaining < count) {
                count = remaining;
            }
            memcpy(buffer, file->buffer + file->bufferPos, count);
            bufferMisalign = (bufferMisalign + count) % 32;
            file->bufferPos += count;
            if (file->bufferPos >= file->bufferSize) {
                file->bufferValid = 0;
                file->bufferPos = 0;
            }
            file->position += count;
            fileMisalign = (fileMisalign + count) % 32;
            remaining -= count;
            buffer = (u8 *)buffer + count;
        } else {
            s32 offset = (u32)file->position - fileMisalign;
            s32 end = ((u32)file->length + 31) & ~31;
            s32 count = file->bufferSize;

            if (end < count + offset) {
                count = end - offset;
            }
            if (fn_8047A978(file, file->buffer, count, offset, file->field_0x18) > 0) {
                file->bufferPos = fileMisalign;
                file->bufferValid = 1;
                retries = 0;
            } else {
                retries++;
            }
        }
    }
    if (file->field_0x18 == 0) {
        file->status = 2;
    } else {
        file->status = 3;
    }
    return size - remaining;
}

/* Writing is not supported on the DVD. */
u32 fn_8047B050(RtFile *file, const void *buffer, u32 size) {
    file->field_0x24 = 2;
    return 0;
}

/* Moves the file position: from the start (1), the current position (2) or the end (3). */
s64 fn_8047B060(RtFile *file, s32 offset, s32 whence) {
    switch (whence) {
    case 1:
        file->position = offset;
        file->bufferValid = 0;
        file->bufferPos = 0;
        break;
    case 2:
        file->position += offset;
        if (file->bufferValid != 0) {
            file->bufferPos += offset;
            if (file->bufferPos >= file->bufferSize || file->bufferPos < 0) {
                file->bufferValid = 0;
                file->bufferPos = 0;
            }
        }
        break;
    case 3:
        file->position = file->length + offset;
        file->bufferValid = 0;
        file->bufferPos = 0;
        break;
    }
    if (file->field_0x18 == 0) {
        file->status = 2;
    } else {
        file->status = 3;
    }
    return file->position;
}

/* Hands a DVD error to the file system's error callback. */
static void ReportError(RtFileSystem *fs, s32 error) {
    if (fs->errorCallBack != 0) {
        fs->errorCallBack(error, fs->errorCallBackData);
    }
}

/* Updates a file's status from the drive's, ending its asynchronous read when it is done. */
s32 fn_8047B154(RtFile *file, RwBool wait) {
    RtFileSystem *fs = file->fs;
    s32 status = DVDGetDriveStatus();

    switch (status) {
    case -1:
        file->status = 4;
        lbl_805EDE90 = 2;
        ReportError(fs, lbl_805EDE90);
        break;
    case 4:
        file->status = 4;
        lbl_805EDE90 = 3;
        ReportError(fs, lbl_805EDE90);
        break;
    case 5:
        file->status = 4;
        lbl_805EDE90 = 4;
        ReportError(fs, lbl_805EDE90);
        break;
    case 6:
        file->status = 4;
        lbl_805EDE90 = 5;
        ReportError(fs, lbl_805EDE90);
        break;
    case 11:
        file->status = 4;
        lbl_805EDE90 = 6;
        ReportError(fs, lbl_805EDE90);
        break;
    default: {
        RwBool ok;

        if (lbl_805EDE90 != 1 || file->status == 4) {
            if (status == 1) {
                fs->status = 1;
                ok = 0;
                lbl_805EDE90 = 8;
            } else {
                fs->status = 2;
                lbl_805EDE90 = ok = 1;
            }
            if (!ok) {
                break;
            }
        }
        switch (fn_8037C548(&file->dvdInfo)) {
        case 0:
            file->status = 2;
            break;
        case 1:
            file->status = 3;
            break;
        case 10:
            file->status = 2;
            break;
        case -1:
        case 2:
        case 3:
        case 11:
        default:
            file->status = 3;
            break;
        }
        break;
    }
    }
    if (file->field_0x18 != 0 && file->status == 2) {
        if (file->asyncStatus == 5) {
            file->asyncData = (void *)(u32)(file->position - file->startPosition);
        }
        fn_804799C8(file);
    }
    return file->status;
}

/* Abandons a file's read. */
void fn_8047B384(RtFile *file) {
    fn_8037C6A0(&file->dvdInfo, 0);
}

/* A file's status, brought up to date first. */
s32 fn_8047B3AC(RtFile *file) {
    if (file->status != 1) {
        fn_8047B154(file, 0);
    }
    return file->status;
}

RwBool fn_804797F0(RtFileSystem *fs, const char *name);

/* Whether a file exists on the DVD. */
RwBool fn_8047B3EC(RtFileSystem *fs, const char *name) {
    return fn_804797F0(fs, name);
}

/* Creates and registers the DVD file system: `maxNbOpenFiles` file objects, each with its slice
 * of `buffer` (`bufferSize` bytes each). */
RtFileSystem *fn_8047B40C(s32 maxNbOpenFiles, u8 *buffer, s32 bufferSize, const char *deviceName,
                          const char *name) {
    RtFileSystem *fs;
    s32 i;

    if (fn_80479C74(name) != 0) {
        return 0;
    }
    fs = (RtFileSystem *)RWSRCGLOBAL(rwmalloc)(sizeof(RtFileSystem), 0x401BE);
    fs->getFileObject = fn_8047A7C8;
    fs->close = fn_8047A7F4;
    fs->getRootPath = fn_8047A7EC;
    fs->fileOpen = fn_8047A824;
    fs->fileClose = fn_8047A938;
    fs->fileRead = fn_8047AC10;
    fs->fileWrite = fn_8047B050;
    fs->fileSetPosition = fn_8047B060;
    fs->fileSync = (s32 (*)(RtFile *))fn_8047B154;
    fs->fileAbort = fn_8047B384;
    fs->fileIsEOF = fn_80479848;
    fs->fileGetStatus = fn_8047B3AC;
    fs->fileExists = fn_8047B3EC;
    fs->files = (RtFile *)RWSRCGLOBAL(rwcalloc)(maxNbOpenFiles, sizeof(RtFile), 0x401BE);
    {
        RtFile *files = fs->files;

        for (i = 0; i < maxNbOpenFiles; i++) {
            files[i].fs = fs;
            files[i].buffer = buffer + i * bufferSize;
            files[i].bufferSize = bufferSize;
            files[i].bufferValid = 0;
            files[i].bufferPos = 0;
        }
    }
    if (fn_80479598(fs, maxNbOpenFiles, name, deviceName) != 1) {
        RWSRCGLOBAL(rwfree)(fs->files);
        RWSRCGLOBAL(rwfree)(fs);
        return 0;
    }
    return fs;
}

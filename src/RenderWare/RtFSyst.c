/* RenderWare file system toolkit (RtFSyst), 0x80479598..0x8047A7C8: file system objects and the
 * file system manager (register, find by name or device, the default file system), opening files,
 * and the RenderWare file interface on top of it (fopen, fclose, fread, fwrite, fseek, fgets,
 * feof, exists, ftell).
 *
 * One source file: its .bss is the manager (0x805E2A40), its .sbss the default file system
 * (0x805F1548) and its .sdata the access-mode strings of the fopen (0x805EDE78..0x805EDE90). It
 * ends with the ftell (fn_8047A7C0, the generic file position, called by RwStreamOpen); the DVD
 * file system's functions, which the DVD file system's constructor stores, start at 0x8047A7C8.
 *
 * The file interface takes its file as a void * (fread/fwrite match only with the typed pointer as
 * a local of its own, see probes/RenderWare_RtFSyst_R3.py).
 * NonMatching: fn_8047A044 and fn_8047A67C, which inlines it (saved-register permutation of the
 * device-prefix loop; ~150 declaration orders tried), fn_8047A178 (retail keeps `flags |= 2` and
 * `flags |= 8` as two ori, GC/2.7 merges them) and fn_8047A518 (fgets: register allocation).
 *
 * Written from the retail code; the fn_ symbols are kept because game code calls them by address,
 * and each function says which public RenderWare API it is. C.
 * Flags: GC/2.7 -O4,p, no stmw/lmw, automatic inlining, -lang=c, writable pooled strings
 * (cflags_rw_r5). Measured per file against the other RenderWare flag sets: GC 2.0-2.7 give the
 * same code, GC 1.3.2, -O4,s, stmw/lmw and -inline noauto match fewer functions, and the
 * read-only string sets (cflags_rw_r3/r6/c) would move strings out of .data/.sdata. */
#include "Game/rw/r5.h"

u32 strlen(const char *s);
char *fn_803AC6C0(const char *s, const char *t);        /* strstr */
char *fn_803ACABC(char *dst, const char *src, u32 n);   /* strncpy */
s32 OSDisableInterrupts(void);
s32 OSEnableInterrupts(void);
void *memcpy(void *dst, const void *src, u32 n);

/* The file system manager and the default file system. */
RtFSManager lbl_805E2A40;
RtFileSystem *lbl_805F1548;

RwBool fn_80479B48(void);
RtFSManager *fn_80479FC4(void);
RwBool fn_80479D08(const char *name);
s32 fn_804798FC(RtFileSystem *fs, const char *deviceName);

/* RtFileSystemInit. Returns an RtFSyst error code. */
s32 fn_80479598(RtFileSystem *fs, s32 maxNbOpenFiles, const char *name, const char *deviceName) {
    if (fn_80479B48()) {
        s32 i;

        if (fn_80479D08(name)) {
            return 6;
        }
        RWSRCGLOBAL(vecStrcpy)(fs->name, name);
        fs->maxNbOpenFiles = maxNbOpenFiles;
        fs->field_0x10 = 0;
        for (i = 0; i < maxNbOpenFiles; i++) {
            RtFile *file = fs->getFileObject(fs, i);

            file->status = 1;
            file->field_0x18 = fs->field_0x10;
        }
    } else {
        return 2;
    }
    fs->deviceName = 0;
    {
        s32 error = fn_804798FC(fs, deviceName);

        switch (error) {
        case 1:
            fs->status = 2;
            fs->registerCB = 0;
            fs->unregisterCB = 0;
            return 1;
        }
        return error;
    }
}

/* The file name without its device prefix ("dev:"). */
const char *fn_80479698(const char *name) {

    const char *p = name;
    s32 i = 0;

    while (i < (s32)RWSRCGLOBAL(vecStrlen)(name)) {
        if (*p == ':') {
            name = &name[i + 1];
            break;
        }
        i++;
        p++;
    }
    return name;
}


/* Joins a device prefix and a file name into `buffer`, converting the path separators. */
char *fn_8047971C(char *buffer, s32 size, const char *prefix, const char *name, char separator) {

    s32 prefixLength = strlen(prefix);
    s32 nameLength = strlen(name);
    s32 i;

    if (prefixLength + nameLength > size - 1) {
        return 0;
    }
    RWSRCGLOBAL(vecStrcpy)(buffer, prefix);
    for (i = 0; i <= nameLength; i++) {
        if (name[i] == '/' || name[i] == '\\') {
            buffer[prefixLength + i] = separator;
        } else {
            buffer[prefixLength + i] = name[i];
        }
    }
    return buffer;
}


RtFile *fn_80479DCC(RtFileSystem *fs, const char *name, u32 flags, void *callBack, void *callBackData);

/* Whether a file exists in a file system: opens it for reading and closes it again. */
RwBool fn_804797F0(RtFileSystem *fs, const char *name) {
    RtFile *file = fn_80479DCC(fs, name, 1, 0, 0);

    if (file != 0) {
        fs->fileClose(file);
        return 1;
    }
    return 0;
}

/* RtFileIsEOF. */
RwBool fn_80479848(RtFile *file) {
    if (file->position == -1) {
        return 1;
    }
    return file->position >= file->length;
}

/* Calls a file system's register (1) or unregister (2) hook with its name. Retail never inlines
 * it (GC/2.7 would). */
#pragma auto_inline off
void fn_80479894(RtFileSystem *fs, s32 what) {
    switch (what) {
    case 1:
        if (fs->registerCB != 0) {
            fs->registerCB(fs->name);
        }
        break;
    case 2:
        if (fs->unregisterCB != 0) {
            fs->unregisterCB(fs->name);
        }
        break;
    }
}
#pragma auto_inline reset

RtFileSystem *fn_80479FD0(const char *deviceName);

/* RtFileSystemSetDeviceName. */
s32 fn_804798FC(RtFileSystem *fs, const char *deviceName) {
    if (fn_80479FD0(deviceName) == 0) {
        if (fs->deviceName != 0) {
            RWSRCGLOBAL(rwfree)(fs->deviceName);
        }
        fs->deviceName = (char *)RWSRCGLOBAL(rwmalloc)(RWSRCGLOBAL(vecStrlen)(deviceName) + 1, 0x401BE);
        RWSRCGLOBAL(vecStrcpy)(fs->deviceName, deviceName);
    } else {
        return 7;
    }
    return 1;
}

/* Sets whether a file system works asynchronously. */
void fn_804799B8(RtFileSystem *fs, s32 value) {
    fs->field_0x10 = value;
}

s32 fn_804799C0(RtFileSystem *fs) {
    return fs->field_0x10;
}

/* Ends an asynchronous file operation and calls its callback. */
void fn_804799C8(RtFile *file) {

    if (file != 0 && file->field_0x18 != 0) {
        if (file->asyncStatus != 1 && file->asyncCallBack != 0) {
            s32 status = file->asyncStatus;
            void *data;

            if (status == 4) {
                data = (void *)(u32)file->position;
            } else {
                data = file->asyncData;
            }
            file->asyncStatus = 1;
            file->asyncCallBack(file, data, file->status, status, file->asyncCallBackData);
        } else {
            file->asyncStatus = 1;
        }
    }
}


/* RtFSManagerOpen. */
RwBool fn_80479A50(s32 maxNbFS) {
    lbl_805E2A40.numFS = 0;
    lbl_805E2A40.maxNbFS = maxNbFS;
    lbl_805E2A40.errorCallback = 0;
    lbl_805F1548 = 0;
    lbl_805E2A40.fsList = 0;
    lbl_805E2A40.isOpen = 1;
    return 1;
}

/* RtFSManagerClose. */
void fn_80479A80(void) {

    if (lbl_805E2A40.isOpen == 1) {
        RtFileSystem *next;
        RtFileSystem *fs = lbl_805E2A40.fsList;

        while (fs != 0) {
            next = fs->next;
            fs->close(fs);
            RWSRCGLOBAL(rwfree)(fs->deviceName);
            fs->deviceName = 0;
            fn_80479894(fs, 2);
            RWSRCGLOBAL(rwfree)(fs);
            fs = next;
        }
        lbl_805E2A40.isOpen = 0;
        lbl_805E2A40.numFS = 0;
    }
}


/* Whether one more file system may be registered. */
RwBool fn_80479B48(void) {
    s32 max = lbl_805E2A40.maxNbFS;

    if (max > 0 && lbl_805E2A40.numFS + 1 > max) {
        if (lbl_805E2A40.errorCallback != 0) {
            lbl_805E2A40.errorCallback(5);
        }
        return 0;
    }
    return 1;
}

RtFileSystem *fn_80479C74(const char *name);

/* RtFSManagerRegister. */
RwBool fn_80479BAC(RtFileSystem *fs) {
    if (fn_80479B48()) {
        if (fn_80479C74(fs->name) == 0) {
            fn_80479894(fs, 1);
            fs->next = lbl_805E2A40.fsList;
            lbl_805E2A40.fsList = fs;
            lbl_805E2A40.numFS++;
            return 1;
        }
        return 0;
    }
    return 0;
}

/* RtFSManagerGetFileSystemFromName. */
RtFileSystem *fn_80479C74(const char *name) {
    RtFileSystem *fs;

    for (fs = lbl_805E2A40.fsList; fs != 0; fs = fs->next) {
        if (RWSRCGLOBAL(vecStrcmp)(fs->name, name) == 0) {
            return fs;
        }
    }
    if (lbl_805E2A40.errorCallback != 0) {
        lbl_805E2A40.errorCallback(6);
    }
    return 0;
}

/* Whether a file system of this name is registered already. */
RwBool fn_80479D08(const char *name) {
    if (fn_80479C74(name) != 0) {
        if (lbl_805E2A40.errorCallback != 0) {
            lbl_805E2A40.errorCallback(7);
        }
        return 1;
    }
    return 0;
}

/* Opens `name` in a file system: takes a free file object and hands it to the file system. */
RtFile *fn_80479DCC(RtFileSystem *fs, const char *name, u32 flags, void *callBack, void *callBackData) {
    RtFile *file = 0;

    if (fs != 0) {
        s32 i;

        OSDisableInterrupts();
        for (i = 0; i < fs->maxNbOpenFiles; i++) {
            RtFile *f = fs->getFileObject(fs, i);

            if (f->status == 1) {
                file = f;
                f->status = 5;
                break;
            }
        }
        OSEnableInterrupts();
        if (file == 0) {
            fn_80479FC4()->pad10 = 8;
            if (lbl_805E2A40.errorCallback != 0) {
                lbl_805E2A40.errorCallback(8);
            }
            return 0;
        }
        if ((flags & 0x10) || file->field_0x18 != 0) {
            file->asyncStatus = 2;
            file->asyncCallBack = callBack;
            file->asyncCallBackData = callBackData;
        }
        if (fs->field_0x10 != 0) {
            file->asyncStatus = 2;
            flags |= 0x10;
            file->asyncCallBack = callBack;
            file->asyncCallBackData = callBackData;
        }
        if (fs->fileOpen(fs, file, name, flags) != 1) {
            file->status = 1;
            file = 0;
        }
        if (file != 0) {
            file->field_0x1C = 2;
        }
        return file;
    }
    if (lbl_805E2A40.errorCallback != 0) {
        lbl_805E2A40.errorCallback(4);
    }
    return 0;
}

RtFileSystem *fn_8047A044(const char *name);

/* RtFSManagerFileOpen: opens `name` in the file system its device prefix names. */
RtFile *fn_80479F50(const char *name, u32 flags, void *callBack, void *callBackData) {
    RtFileSystem *fs = fn_8047A044(name);

    if (fs != 0) {
        return fn_80479DCC(fs, name, flags, callBack, callBackData);
    }
    return 0;
}

/* RtFSManagerGet. */
RtFSManager *fn_80479FC4(void) {
    return &lbl_805E2A40;
}

/* The file system whose device name is `deviceName`. */
RtFileSystem *fn_80479FD0(const char *deviceName) {
    RtFileSystem *fs;

    for (fs = lbl_805E2A40.fsList; fs != 0; fs = fs->next) {
        if (RWSRCGLOBAL(vecStricmp)(deviceName, fs->deviceName) == 0) {
            return fs;
        }
    }
    return 0;
}

/* RtFSManagerGetFileSystemFromFileName: the file system of the file name's device prefix, else
 * the default one. */
RtFileSystem *fn_8047A044(const char *name) {
    if (lbl_805E2A40.numFS != 0) {
        char device[100];
        const char *p;
        u32 length;
        RtFileSystem *fs;
        char *q;
        u32 i;

        length = RWSRCGLOBAL(vecStrlen)(name);
        p = name;
        q = device;
        for (i = 0; i < length; i++, p++, q++) {
            if (*p == ':') {
                fn_803ACABC(device, name, i + 1);
                q[1] = 0;
                fs = fn_80479FD0(device);
                if (fs != 0) {
                    return fs;
                }
            }
        }
        if (lbl_805F1548 != 0) {
            return lbl_805F1548;
        }
    }
    if (lbl_805E2A40.errorCallback != 0) {
        lbl_805E2A40.errorCallback(6);
    }
    return 0;
    if (lbl_805E2A40.errorCallback != 0) {
        lbl_805E2A40.errorCallback(6);
    }
    return 0;
}

/* RtFSManagerSetDefaultFileSystem. */
void fn_8047A168(RtFileSystem *fs) {
    lbl_805F1548 = fs;
}

/* RtFSManagerGetDefaultFileSystem. */
RtFileSystem *fn_8047A170(void) {
    return lbl_805F1548;
}

/* The file interface's fopen: the access string's flags, then the file system of the name. */
RtFile *fn_8047A178(const char *name, const char *access) {
    u32 flags = 0;
    RwBool plus = 0;
    RwBool binary = 0;
    RtFileSystem *fs;

    if (fn_803AC6C0(access, "+") != 0) {
        plus = 1;
    }
    if (fn_803AC6C0(access, "b") != 0) {
        binary = 1;
    }
    if (fn_803AC6C0(access, "r") != 0) {
        flags |= 1;
        if (plus) {
            flags |= 2;
            flags |= 8;
        }
    }
    if (fn_803AC6C0(access, "w") != 0) {
        flags |= 2;
        flags |= 4;
        if (plus) {
            flags |= 1;
        }
    }
    if (fn_803AC6C0(access, "a") != 0) {
        flags |= 2;
        flags |= 8;
        if (plus) {
            flags |= 1;
        }
        if (binary) {
            flags |= 4;
        }
    }
    fs = fn_8047A044(name);
    if (fs != 0) {
        return fn_80479DCC(fs, name, flags, 0, 0);
    }
    return 0;
}

/* fclose. */
s32 fn_8047A388(RtFile *file) {
    RtFileSystem *fs = file->fs;

    OSDisableInterrupts();
    file->status = 1;
    fs->fileClose(file);
    OSEnableInterrupts();
    return 0;
}

/* fread. */
u32 fn_8047A3E0(void *buffer, u32 size, u32 count, void *fptr) {
    RtFile *file = (RtFile *)fptr;
    RtFileSystem *fs = file->fs;

    return fs->fileRead(file, buffer, size * count) / size;
}

/* fwrite. */
u32 fn_8047A428(const void *buffer, u32 size, u32 count, void *fptr) {
    RtFile *file = (RtFile *)fptr;
    RtFileSystem *fs = file->fs;

    return fs->fileWrite(file, buffer, size * count) / size;
}

/* fseek. */
s32 fn_8047A470(RtFile *file, s32 offset, s32 whence) {
    RtFileSystem *fs = file->fs;

    if (file->field_0x18 != 0) {
        file->asyncStatus = 4;
        file->status = 3;
    }
    switch (whence) {
    case 1:
        fs->fileSetPosition(file, offset, 2);
        break;
    case 2:
        fs->fileSetPosition(file, offset, 3);
        break;
    case 0:
        fs->fileSetPosition(file, offset, 1);
        break;
    default:
        return -1;
    }
    return 0;
}

/* fgets: reads a line, dropping the carriage return before its line feed. */
char *fn_8047A518(char *buffer, s32 maxLength, RtFile *file) {
    s32 i = 0;
    s32 count = file->fs->fileRead(file, buffer, maxLength - 1);

    if (count == 0) {
        return 0;
    }
    while (count > i) {
        char *p = buffer + i;

        if (*p == '\n') {
            buffer[++i] = 0;
            fn_8047A470(file, i - count, 1);
            return buffer;
        }
        if (*p == '\r' && i < count - 1 && p[1] == '\n') {
            memcpy(p, p + 1, count - i - 1);
            count--;
        } else {
            i++;
        }
    }
    buffer[count] = 0;
    return buffer;
}

/* feof. */
RwBool fn_8047A650(RtFile *file) {
    return file->fs->fileIsEOF(file);
}

/* Whether `name` exists in the file system of its device prefix. */
RwBool fn_8047A67C(const char *name) {
    RtFileSystem *fs = fn_8047A044(name);

    if (fs == 0) {
        return 0;
    }
    return fs->fileExists(fs, name);
}

/* ftell. */
u32 fn_8047A7C0(RtFile *file) {
    return file->position;
}

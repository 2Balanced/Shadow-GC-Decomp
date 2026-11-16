#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>
#include <TRK_MINNOW_DOLPHIN/msghndlr.h>

/* The C library's console I/O while a debugger is attached: reads and writes of the standard
 * streams become file requests to the host. The results are MSL's I/O results. */

enum {
    kIoNoError = 0,
    kIoError = 1,
    kIoEOF = 2
};

static inline int __access_file(u32 handle, u8 *data, TRKSize *count, void *ref, u32 command) {
    TRKSize transferred;
    u32 result;

    if (!GetTRKConnected()) {
        return kIoError;
    }
    transferred = *count;
    result = TRKAccessFile(command, handle, &transferred, data);
    *count = transferred;
    switch ((u8)result) {
    case 0:
        return kIoNoError;
    case 2:
        return kIoEOF;
    }
    return kIoError;
}

int __read_console(u32 handle, u8 *data, TRKSize *count, void *ref) {
    if (!GetUseSerialIO()) {
        return kIoError;
    }
    return __access_file(0, data, count, ref, kTRKCmdReadFile);
}

int __TRK_write_console(u32 handle, u8 *data, TRKSize *count, void *ref) {
    if (!GetUseSerialIO()) {
        return kIoError;
    }
    return __access_file(1, data, count, ref, kTRKCmdWriteFile);
}

int __close_console(u32 handle) {
    u8 result;

    if (!GetTRKConnected()) {
        return kIoError;
    }
    result = TRKCloseFile(kTRKCmdCloseFile, handle);
    switch (result) {
    case 0:
        return kIoNoError;
    case 2:
        return kIoEOF;
    }
    return kIoError;
}

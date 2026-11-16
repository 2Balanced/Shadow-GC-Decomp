#ifndef TRK_MINNOW_DOLPHIN_MSGHNDLR_H
#define TRK_MINNOW_DOLPHIN_MSGHNDLR_H

#include <TRK_MINNOW_DOLPHIN/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Commands of the debugger protocol (the byte after a message's size word). */
typedef enum TRKCommand {
    kTRKCmdConnect = 0x01,
    kTRKCmdDisconnect = 0x02,
    kTRKCmdReset = 0x03,
    kTRKCmdVersions = 0x04,
    kTRKCmdSupportMask = 0x05,
    kTRKCmdOverride = 0x07,
    kTRKCmdReadMemory = 0x10,
    kTRKCmdWriteMemory = 0x11,
    kTRKCmdReadRegisters = 0x12,
    kTRKCmdWriteRegisters = 0x13,
    kTRKCmdSetOption = 0x17,
    kTRKCmdContinue = 0x18,
    kTRKCmdStep = 0x19,
    kTRKCmdStop = 0x1A,
    kTRKCmdReplyAck = 0x80,
    kTRKCmdNotifyStopped = 0x90,
    kTRKCmdNotifyException = 0x91,
    kTRKCmdWriteFile = 0xD0,
    kTRKCmdReadFile = 0xD1,
    kTRKCmdOpenFile = 0xD2,
    kTRKCmdCloseFile = 0xD3,
    kTRKCmdPositionFile = 0xD4
} TRKCommand;

#define TRK_MESSAGE_HEADER_SIZE 0x40

/* Every message starts with a 64-byte block whose layout depends on the command. The fields are
 * word-aligned whatever their width. */

/* A reply: the acknowledgement of a command, with its result. */
typedef struct TRKReplyHeader {
    u32 size;       /* 0x0, the whole message */
    u8 command;     /* 0x4, kTRKCmdReplyAck */
    u8 pad5[3];
    u8 error;       /* 0x8, TRKReplyError */
    u8 pad9[3];
    u8 padC[0x34];
} TRKReplyHeader;   /* 0x40 */

/* kTRKCmdReadMemory, kTRKCmdWriteMemory. */
typedef struct TRKMemoryCommand {
    u32 size;       /* 0x0 */
    u8 command;     /* 0x4 */
    u8 pad5[3];
    u8 options;     /* 0x8, TRK_MEMORY_* */
    u8 pad9[3];
    u16 length;     /* 0xC */
    u8 padE[2];
    u32 address;    /* 0x10 */
    u8 pad14[0x2C];
} TRKMemoryCommand;

#define TRK_MEMORY_EXTENDED 0x02  /* a memory space the nub does not have */
#define TRK_MEMORY_USER 0x08      /* access with user rather than supervisor rights */
#define TRK_MEMORY_ARAM 0x40      /* the address is in audio RAM */

/* kTRKCmdReadRegisters, kTRKCmdWriteRegisters. */
typedef struct TRKRegistersCommand {
    u32 size;       /* 0x0 */
    u8 command;     /* 0x4 */
    u8 pad5[3];
    u8 block;       /* 0x8, TRKRegisterBlock */
    u8 pad9[3];
    u16 first;      /* 0xC */
    u8 padE[2];
    u16 last;       /* 0x10 */
    u8 pad12[0x2E];
} TRKRegistersCommand;

typedef enum TRKRegisterBlock {
    kTRKRegistersDefault = 0,
    kTRKRegistersFP = 1,
    kTRKRegistersExtended1 = 2,
    kTRKRegistersExtended2 = 3
} TRKRegisterBlock;

/* kTRKCmdStep. */
typedef struct TRKStepCommand {
    u32 size;       /* 0x0 */
    u8 command;     /* 0x4 */
    u8 pad5[3];
    u8 options;     /* 0x8, TRKStepOption */
    u8 pad9[3];
    u8 count;       /* 0xC */
    u8 padD[3];
    u32 rangeStart; /* 0x10 */
    u32 rangeEnd;   /* 0x14 */
    u8 pad18[0x28];
} TRKStepCommand;

typedef enum TRKStepOption {
    kTRKStepIntoCount = 0x00,
    kTRKStepIntoRange = 0x01,
    kTRKStepOverCount = 0x10,
    kTRKStepOverRange = 0x11
} TRKStepOption;

/* kTRKCmdSetOption. */
typedef struct TRKOptionCommand {
    u32 size;       /* 0x0 */
    u8 command;     /* 0x4 */
    u8 pad5[3];
    u8 option;      /* 0x8 */
    u8 pad9[3];
    u8 value;       /* 0xC */
    u8 padD[0x33];
} TRKOptionCommand;

#define TRK_OPTION_SERIAL_IO 1

/* ---- requests the target makes of the host (support.c) ---- */

/* kTRKCmdWriteFile, kTRKCmdReadFile. */
typedef struct TRKFileDataCommand {
    u32 size;       /* 0x0 */
    u8 command;     /* 0x4 */
    u8 pad5[3];
    u32 handle;     /* 0x8 */
    u16 length;     /* 0xC */
    u8 padE[0x32];
} TRKFileDataCommand;

/* kTRKCmdOpenFile; the path follows the header. */
typedef struct TRKFileOpenCommand {
    u32 size;       /* 0x0 */
    u8 command;     /* 0x4 */
    u8 pad5[3];
    u8 mode;        /* 0x8 */
    u8 pad9[3];
    u16 pathLength; /* 0xC, with the terminator */
    u8 padE[0x32];
} TRKFileOpenCommand;

/* kTRKCmdCloseFile. */
typedef struct TRKFileCloseCommand {
    u32 size;       /* 0x0 */
    u8 command;     /* 0x4 */
    u8 pad5[3];
    u32 handle;     /* 0x8 */
    u8 padC[0x34];
} TRKFileCloseCommand;

/* kTRKCmdPositionFile. */
typedef struct TRKFilePositionCommand {
    u32 size;       /* 0x0 */
    u8 command;     /* 0x4 */
    u8 pad5[3];
    u32 handle;     /* 0x8 */
    u32 position;   /* 0xC */
    u8 mode;        /* 0x10 */
    u8 pad11[0x2F];
} TRKFilePositionCommand;

/* The host's reply to a file request. */
typedef struct TRKFileReply {
    u32 size;       /* 0x0 */
    u8 command;     /* 0x4 */
    u8 pad5[3];
    u32 handle;     /* 0x8, also the reply's error byte */
    u8 padC[4];
    u32 ioResult;   /* 0x10 */
    u16 length;     /* 0x14 */
    u8 pad16[2];
    u32 position;   /* 0x18 */
    u8 pad1C[0x24];
} TRKFileReply;

TRKResult TRKSuppAccessFile(u32 handle, u8 *data, TRKSize *count, int *ioResult, BOOL waitForReply,
                            BOOL read);
TRKResult TRKRequestSend(TRKBuffer *message, int *replyId, u32 replyMinimum, int retries,
                         BOOL timeout);
TRKResult HandleOpenFileSupportRequest(const char *path, u8 mode, u32 *handle, int *ioResult);
TRKResult HandleCloseFileSupportRequest(u32 handle, int *ioResult);
TRKResult HandlePositionFileSupportRequest(u32 handle, u32 *position, u8 mode, int *ioResult);

void OutputData(void *data, int length);
BOOL GetTRKConnected(void);
void SetTRKConnected(BOOL connected);

TRKResult TRKDoConnect(TRKBuffer *message);
TRKResult TRKDoDisconnect(TRKBuffer *message);
TRKResult TRKDoReset(TRKBuffer *message);
TRKResult TRKDoOverride(TRKBuffer *message);
TRKResult TRKDoVersions(TRKBuffer *message);
TRKResult TRKDoSupportMask(TRKBuffer *message);
TRKResult TRKDoReadMemory(TRKBuffer *message);
TRKResult TRKDoWriteMemory(TRKBuffer *message);
TRKResult TRKDoReadRegisters(TRKBuffer *message);
TRKResult TRKDoWriteRegisters(TRKBuffer *message);
TRKResult TRKDoContinue(TRKBuffer *message);
TRKResult TRKDoStep(TRKBuffer *message);
TRKResult TRKDoStop(TRKBuffer *message);
TRKResult TRKDoSetOption(TRKBuffer *message);

#ifdef __cplusplus
}
#endif

#endif

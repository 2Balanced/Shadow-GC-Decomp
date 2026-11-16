#ifndef TRK_MINNOW_DOLPHIN_TRK_H
#define TRK_MINNOW_DOLPHIN_TRK_H

/* MetroTRK, the Metrowerks debugger nub for the GameCube ("MetroTRK for GAMECUBE v2.6"), as linked
 * into the game. Shared declarations of the portable core; names the binary does not fix are this
 * project's own. */

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int TRKSize;

/* Result codes of the nub's functions. The functions return them as plain ints. */
typedef int TRKResult;

enum {
    kTRKOk = 0,
    kTRKEventQueueFull = 0x100,
    kTRKNoBufferFree = 0x300,
    kTRKBufferOverflow = 0x301,
    kTRKBufferReadPastEnd = 0x302,
    kTRKDispatchError = 0x500,
    kTRKInvalidMemory = 0x700,
    kTRKInvalidRegister = 0x701,
    kTRKCWDSException = 0x702,
    kTRKUnsupported = 0x703,
    kTRKInvalidProcessID = 0x704,
    kTRKInvalidThreadID = 0x705,
    kTRKOsError = 0x706,
    kTRKWaitAckError = 0x800
};

/* ---- events (nubevent.c) ---- */

typedef enum TRKEventType {
    kTRKEventNull = 0,
    kTRKEventShutdown = 1,
    kTRKEventRequest = 2,   /* a message from the debugger is in the event's buffer */
    kTRKEventBreakpoint = 3,
    kTRKEventException = 4,
    kTRKEventSupport = 5    /* the target asks the host for a service (file I/O) */
} TRKEventType;

typedef struct TRKEvent {
    int type;       /* 0x0 */
    u32 serial;     /* 0x4 */
    int bufferId;   /* 0x8 */
} TRKEvent;         /* 0xC */

BOOL TRKGetNextEvent(TRKEvent *event);
TRKResult TRKPostEvent(TRKEvent *event);
void TRKConstructEvent(TRKEvent *event, int type);
void TRKDestructEvent(TRKEvent *event);
TRKResult TRKInitializeEventQueue(void);

/* ---- message buffers (msgbuf.c) ---- */

#define TRK_BUFFER_DATA_SIZE 0x880
#define TRK_BUFFER_COUNT 3

typedef struct TRKBuffer {
    u32 mutex;                      /* 0x0 */
    BOOL inUse;                     /* 0x4 */
    u32 length;                     /* 0x8 */
    u32 position;                   /* 0xC */
    u8 data[TRK_BUFFER_DATA_SIZE];  /* 0x10 */
} TRKBuffer;                        /* 0x890 */

TRKResult TRKInitializeMessageBuffers(void);
TRKResult TRKGetFreeBuffer(int *id, TRKBuffer **buffer);
TRKBuffer *TRKGetBuffer(int id);
void TRKReleaseBuffer(int id);
void TRKResetBuffer(TRKBuffer *buffer, BOOL keepData);
TRKResult TRKSetBufferPosition(TRKBuffer *buffer, u32 position);
TRKResult TRKAppendBuffer(TRKBuffer *buffer, const void *data, TRKSize length);
TRKResult TRKReadBuffer(TRKBuffer *buffer, void *data, TRKSize length);
TRKResult TRKAppendBuffer1_ui64(TRKBuffer *buffer, const u64 data);
TRKResult TRKAppendBuffer_ui8(TRKBuffer *buffer, const u8 *data, int count);
TRKResult TRKAppendBuffer_ui32(TRKBuffer *buffer, const u32 *data, int count);
TRKResult TRKReadBuffer1_ui64(TRKBuffer *buffer, u64 *data);
TRKResult TRKReadBuffer_ui8(TRKBuffer *buffer, u8 *data, int count);
TRKResult TRKReadBuffer_ui32(TRKBuffer *buffer, u32 *data, int count);

/* ---- the rest of the portable core ---- */

extern BOOL gTRKBigEndian;
extern void *gTRKInputPendingPtr;

void TRKNubMainLoop(void);
TRKResult TRKInitializeNub(void);
TRKResult TRKTerminateNub(void);
void TRKNubWelcome(void);

TRKResult TRKMessageSend(TRKBuffer *message);

TRKResult TRKInitializeSerialHandler(void);
TRKResult TRKTerminateSerialHandler(void);
void TRKGetInput(void);
void TRKProcessInput(int bufferId);
int TRKTestForPacket(void);

void usr_put_initialize(void);
BOOL usr_puts_serial(const char *text);

TRKResult TRKInitializeDispatcher(void);
TRKResult TRKDispatchMessage(TRKBuffer *message);

TRKResult TRKInitializeMutex(void *mutex);
TRKResult TRKAcquireMutex(void *mutex);
TRKResult TRKReleaseMutex(void *mutex);

void *memset(void *dest, int value, TRKSize length);
void *memcpy(void *dest, const void *src, TRKSize length);
/* In .init, like the runtime's memset and memcpy. */
__declspec(section ".init") void *TRK_memset(void *dest, int value, TRKSize length);
__declspec(section ".init") void *TRK_memcpy(void *dest, const void *src, TRKSize length);

/* ---- target and board support ---- */

/* Reply codes sent back to the host in a reply's error byte. */
typedef enum TRKReplyError {
    kTRKReplyOk = 0x00,
    kTRKReplyError = 0x01,
    kTRKReplyPacketSize = 0x02,
    kTRKReplyCWDS = 0x03,
    kTRKReplyParameter = 0x11,
    kTRKReplyUnsupported = 0x12,
    kTRKReplyInvalidMemory = 0x13,
    kTRKReplyInvalidRegister = 0x14,
    kTRKReplyException = 0x15,
    kTRKReplyNotStopped = 0x16,
    kTRKReplyOsError = 0x20,
    kTRKReplyInvalidProcess = 0x21,
    kTRKReplyInvalidThread = 0x22
} TRKReplyError;

TRKResult TRKTargetStop(void);
u32 TRKTargetGetPC(void);
TRKResult TRKTargetSingleStep(u32 count, BOOL stepOver);
TRKResult TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, BOOL stepOver);
TRKResult TRKTargetAccessDefault(u32 first, u32 last, TRKBuffer *buffer, TRKSize *registerBytes, BOOL read);
TRKResult TRKTargetAccessFP(u32 first, u32 last, TRKBuffer *buffer, TRKSize *registerBytes, BOOL read);
TRKResult TRKTargetAccessExtended1(u32 first, u32 last, TRKBuffer *buffer, TRKSize *registerBytes, BOOL read);
TRKResult TRKTargetAccessExtended2(u32 first, u32 last, TRKBuffer *buffer, TRKSize *registerBytes, BOOL read);
TRKResult TRKTargetAccessMemory(void *data, u32 address, TRKSize *length, int validation, BOOL read);
TRKResult TRKTargetAccessARAM(void *data, u32 address, TRKSize *length, BOOL read);
void __TRK_copy_vectors(void);
void TRKTargetAddStopInfo(TRKBuffer *buffer);
void TRKTargetAddExceptionInfo(TRKBuffer *buffer);
TRKResult TRKDoNotifyStopped(int command);
void TRK_flush_cache(u32 address, u32 length);
void TRK_fill_mem(void *dest, int value, TRKSize length);
__declspec(section ".init") void __TRK_reset(void);
void SetUseSerialIO(u8 enable);
u8 GetUseSerialIO(void);

TRKResult TRKInitializeTarget(void);
void TRKTargetSetInputPendingPtr(void *ptr);
BOOL TRKTargetStopped(void);
void TRKTargetSetStopped(BOOL stopped);
TRKResult TRKTargetInterrupt(TRKEvent *event);
TRKResult TRKTargetSupportRequest(void);
TRKResult TRKTargetContinue(void);

void InitializeProgramEndTrap(void);
void TRK_board_display(char *text);
TRKResult TRKInitializeIntDrivenUART(u32 baudRate, u32 intDrivenInput, u32 intDrivenOutput, void *inputPendingPtr);
int TRKWriteUARTN(const void *data, u32 length);
int TRKReadUARTN(void *data, u32 length);
int TRKPollUART(void);
BOOL GetTRKConnected(void);
void SetTRKConnected(BOOL connected);

void MWTRACE(u8 level, char *format, ...);

#ifdef __cplusplus
}
#endif

#endif

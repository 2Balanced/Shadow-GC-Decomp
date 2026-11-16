#include <TRK_MINNOW_DOLPHIN/trk.h>

/* Polls the debugger connection for a message from the host and queues it as a request event. */

#define TRK_PACKET_HEADER_SIZE 0x40

/* Every message starts with a fixed 64-byte block; its first word is the size of the whole
 * message. */
typedef struct TRKPacketHeader {
    u32 size;
    u8 body[TRK_PACKET_HEADER_SIZE - 4];
} TRKPacketHeader;

typedef struct TRKReceiveState {
    int bufferId;       /* 0x0 */
    TRKBuffer *buffer;  /* 0x4 */
    int state;          /* 0x8 */
    BOOL escaped;       /* 0xC */
    u8 checksum;        /* 0x10 */
} TRKReceiveState;      /* 0x14 */

static TRKReceiveState gTRKReceiveState;

/* Retail has this pointer in .sbss although the nub is built without small data and addresses
 * it absolutely (it was probably a common symbol the linker placed). The section pragma gives
 * the same layout: a small-data section with absolute addressing. */
#pragma push
#pragma section ".sdata" ".sbss" data_mode = far_abs
__declspec(section ".sdata") void *gTRKInputPendingPtr;
#pragma pop

/* Reads one message into a free buffer; returns the buffer's id, or -1 when nothing arrived. */
int TRKTestForPacket(void) {
    u8 payload[TRK_BUFFER_DATA_SIZE];
    TRKPacketHeader header;
    int id;
    TRKBuffer *buffer;
    int result;

    if (TRKPollUART() <= 0) {
        return -1;
    }
    result = TRKGetFreeBuffer(&id, &buffer);
    MWTRACE(4, "TestForPacket : FreeBuffer is  %ld\n", result);
    TRKSetBufferPosition(buffer, 0);
    if (TRKReadUARTN(&header, sizeof(header)) == 0) {
        TRKAppendBuffer_ui8(buffer, (u8 *)&header, sizeof(header));
        result = id;
        if ((int)(header.size - sizeof(header)) > 0) {
            MWTRACE(1, "Reading payload %ld bytes\n", header.size - sizeof(header));
            if (TRKReadUARTN(payload, header.size - sizeof(header)) == 0) {
                TRKAppendBuffer_ui8(buffer, payload, header.size);
            } else {
                MWTRACE(8, "TestForPacket : Invalid size of packet hdr.size\n");
                TRKReleaseBuffer(result);
                result = -1;
            }
        }
    } else {
        MWTRACE(8, "TestForPacket : Invalid size of packet\n");
        TRKReleaseBuffer(result);
        result = -1;
    }
    MWTRACE(1, "TestForPacket returning %ld\n", result);
    return result;
}

void TRKGetInput(void) {
    TRKBuffer *message;
    int id;

    id = TRKTestForPacket();
    if (id != -1) {
        message = TRKGetBuffer(id);
        TRKProcessInput(id);
    }
}

void TRKProcessInput(int bufferId) {
    TRKEvent event;

    TRKConstructEvent(&event, kTRKEventRequest);
    event.bufferId = bufferId;
    gTRKReceiveState.bufferId = -1;
    TRKPostEvent(&event);
}

TRKResult TRKInitializeSerialHandler(void) {
    gTRKReceiveState.bufferId = -1;
    gTRKReceiveState.state = 0;
    gTRKReceiveState.escaped = FALSE;
    MWTRACE(1, "TRK_Packet_Header \t    %ld bytes\n", TRK_PACKET_HEADER_SIZE);
    MWTRACE(1, "TRK_CMD_ReadMemory     %ld bytes\n", TRK_PACKET_HEADER_SIZE);
    MWTRACE(1, "TRK_CMD_WriteMemory    %ld bytes\n", TRK_PACKET_HEADER_SIZE);
    MWTRACE(1, "TRK_CMD_Connect \t    %ld bytes\n", TRK_PACKET_HEADER_SIZE);
    MWTRACE(1, "TRK_CMD_ReplyAck\t    %ld bytes\n", TRK_PACKET_HEADER_SIZE);
    MWTRACE(1, "TRK_CMD_ReadRegisters\t%ld bytes\n", TRK_PACKET_HEADER_SIZE);
    return kTRKOk;
}

TRKResult TRKTerminateSerialHandler(void) {
    return kTRKOk;
}

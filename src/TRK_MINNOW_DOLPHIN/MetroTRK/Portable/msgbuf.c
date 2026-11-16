#include <TRK_MINNOW_DOLPHIN/trk.h>

/* The nub's message buffers: three fixed buffers that hold the messages exchanged with the host.
 * Multi-byte values travel big-endian; the helpers swap them when the target is not. */

TRKBuffer gTRKMsgBufs[TRK_BUFFER_COUNT];

static inline TRKResult TRKAppendBuffer1_ui8(TRKBuffer *buffer, const u8 data);
static inline TRKResult TRKAppendBuffer1_ui32(TRKBuffer *buffer, const u32 data);
static inline TRKResult TRKReadBuffer1_ui8(TRKBuffer *buffer, u8 *data);
static inline TRKResult TRKReadBuffer1_ui32(TRKBuffer *buffer, u32 *data);

TRKResult TRKInitializeMessageBuffers(void) {
    int i;

    for (i = 0; i < TRK_BUFFER_COUNT; i++) {
        TRKInitializeMutex(&gTRKMsgBufs[i]);
        TRKAcquireMutex(&gTRKMsgBufs[i]);
        gTRKMsgBufs[i].inUse = FALSE;
        TRKReleaseMutex(&gTRKMsgBufs[i]);
    }
    return kTRKOk;
}

TRKResult TRKGetFreeBuffer(int *id, TRKBuffer **buffer) {
    TRKBuffer *candidate;
    TRKResult result = kTRKNoBufferFree;
    int i;

    *buffer = NULL;
    for (i = 0; i < TRK_BUFFER_COUNT; i++) {
        candidate = TRKGetBuffer(i);
        TRKAcquireMutex(candidate);
        if (!candidate->inUse) {
            TRKResetBuffer(candidate, TRUE);
            candidate->inUse = TRUE;
            *buffer = candidate;
            *id = i;
            result = kTRKOk;
            i = TRK_BUFFER_COUNT;
        }
        TRKReleaseMutex(candidate);
    }
    if (result == kTRKNoBufferFree) {
        usr_puts_serial("ERROR : No buffer available\n");
    }
    return result;
}

TRKBuffer *TRKGetBuffer(int id) {
    TRKBuffer *buffer = NULL;

    if (id >= 0 && id < TRK_BUFFER_COUNT) {
        buffer = &gTRKMsgBufs[id];
    }
    return buffer;
}

void TRKReleaseBuffer(int id) {
    TRKBuffer *buffer;

    if (id != -1 && id >= 0 && id < TRK_BUFFER_COUNT) {
        buffer = &gTRKMsgBufs[id];
        TRKAcquireMutex(buffer);
        buffer->inUse = FALSE;
        TRKReleaseMutex(buffer);
    }
}

void TRKResetBuffer(TRKBuffer *buffer, BOOL keepData) {
    buffer->length = 0;
    buffer->position = 0;
    if (!keepData) {
        TRK_memset(buffer->data, 0, TRK_BUFFER_DATA_SIZE);
    }
}

TRKResult TRKSetBufferPosition(TRKBuffer *buffer, u32 position) {
    TRKResult result = kTRKOk;

    if (position > TRK_BUFFER_DATA_SIZE) {
        result = kTRKBufferOverflow;
    } else {
        buffer->position = position;
        if (position > buffer->length) {
            buffer->length = position;
        }
    }
    return result;
}

TRKResult TRKAppendBuffer(TRKBuffer *buffer, const void *data, TRKSize length) {
    TRKResult result = kTRKOk;
    u32 room;

    if (length == 0) {
        return kTRKOk;
    }
    room = TRK_BUFFER_DATA_SIZE - buffer->position;
    if (room < length) {
        result = kTRKBufferOverflow;
        length = room;
    }
    if (length == 1) {
        buffer->data[buffer->position] = *(const u8 *)data;
    } else {
        TRK_memcpy(buffer->data + buffer->position, data, length);
    }
    buffer->position += length;
    buffer->length = buffer->position;
    return result;
}

TRKResult TRKReadBuffer(TRKBuffer *buffer, void *data, TRKSize length) {
    TRKResult result = kTRKOk;
    u32 available;

    if (length == 0) {
        return kTRKOk;
    }
    available = buffer->length - buffer->position;
    if (length > available) {
        result = kTRKBufferReadPastEnd;
        length = available;
    }
    TRK_memcpy(data, buffer->data + buffer->position, length);
    buffer->position += length;
    return result;
}

static inline TRKResult TRKAppendBuffer1_ui8(TRKBuffer *buffer, const u8 data) {
    if (buffer->position >= TRK_BUFFER_DATA_SIZE) {
        return kTRKBufferOverflow;
    }
    buffer->data[buffer->position++] = data;
    buffer->length++;
    return kTRKOk;
}

static inline TRKResult TRKAppendBuffer1_ui32(TRKBuffer *buffer, const u32 data) {
    u8 *wire;
    u8 *native;
    u8 swapped[sizeof(data)];

    if (gTRKBigEndian) {
        wire = (u8 *)&data;
    } else {
        native = (u8 *)&data;
        wire = swapped;
        wire[0] = native[3];
        wire[1] = native[2];
        wire[2] = native[1];
        wire[3] = native[0];
    }
    return TRKAppendBuffer(buffer, wire, sizeof(data));
}

TRKResult TRKAppendBuffer1_ui64(TRKBuffer *buffer, const u64 data) {
    u8 *wire;
    u8 *native;
    u8 swapped[sizeof(data)];

    if (gTRKBigEndian) {
        wire = (u8 *)&data;
    } else {
        native = (u8 *)&data;
        wire = swapped;
        wire[0] = native[7];
        wire[1] = native[6];
        wire[2] = native[5];
        wire[3] = native[4];
        wire[4] = native[3];
        wire[5] = native[2];
        wire[6] = native[1];
        wire[7] = native[0];
    }
    return TRKAppendBuffer(buffer, wire, sizeof(data));
}

TRKResult TRKAppendBuffer_ui8(TRKBuffer *buffer, const u8 *data, int count) {
    TRKResult result;
    int i;

    for (i = 0, result = kTRKOk; result == kTRKOk && i < count; i++) {
        result = TRKAppendBuffer1_ui8(buffer, data[i]);
    }
    return result;
}

TRKResult TRKAppendBuffer_ui32(TRKBuffer *buffer, const u32 *data, int count) {
    TRKResult result;
    int i;

    for (i = 0, result = kTRKOk; result == kTRKOk && i < count; i++) {
        result = TRKAppendBuffer1_ui32(buffer, data[i]);
    }
    return result;
}

static inline TRKResult TRKReadBuffer1_ui8(TRKBuffer *buffer, u8 *data) {
    return TRKReadBuffer(buffer, data, 1);
}

static inline TRKResult TRKReadBuffer1_ui32(TRKBuffer *buffer, u32 *data) {
    TRKResult result;
    u8 *wire;
    u8 *native;
    u8 swapped[sizeof(*data)];

    if (gTRKBigEndian) {
        wire = (u8 *)data;
    } else {
        wire = swapped;
    }
    result = TRKReadBuffer(buffer, wire, sizeof(*data));
    if (!gTRKBigEndian && result == kTRKOk) {
        native = (u8 *)data;
        native[0] = wire[3];
        native[1] = wire[2];
        native[2] = wire[1];
        native[3] = wire[0];
    }
    return result;
}

TRKResult TRKReadBuffer1_ui64(TRKBuffer *buffer, u64 *data) {
    TRKResult result;
    u8 *wire;
    u8 *native;
    u8 swapped[sizeof(*data)];

    if (gTRKBigEndian) {
        wire = (u8 *)data;
    } else {
        wire = swapped;
    }
    result = TRKReadBuffer(buffer, wire, sizeof(*data));
    if (!gTRKBigEndian && result == kTRKOk) {
        native = (u8 *)data;
        native[0] = wire[7];
        native[1] = wire[6];
        native[2] = wire[5];
        native[3] = wire[4];
        native[4] = wire[3];
        native[5] = wire[2];
        native[6] = wire[1];
        native[7] = wire[0];
    }
    return result;
}

TRKResult TRKReadBuffer_ui8(TRKBuffer *buffer, u8 *data, int count) {
    TRKResult result;
    int i;

    for (i = 0, result = kTRKOk; result == kTRKOk && i < count; i++) {
        result = TRKReadBuffer1_ui8(buffer, &data[i]);
    }
    return result;
}

TRKResult TRKReadBuffer_ui32(TRKBuffer *buffer, u32 *data, int count) {
    TRKResult result;
    int i;

    for (i = 0, result = kTRKOk; result == kTRKOk && i < count; i++) {
        result = TRKReadBuffer1_ui32(buffer, &data[i]);
    }
    return result;
}

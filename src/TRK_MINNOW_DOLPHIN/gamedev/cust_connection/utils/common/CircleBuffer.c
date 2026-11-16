#include <TRK_MINNOW_DOLPHIN/CircleBuffer.h>

extern void *memcpy(void *dest, const void *src, unsigned long length);

u32 CBGetBytesAvailableForRead(CircleBuffer *buffer) {
    return buffer->readable;
}

void CircleBufferInitialize(CircleBuffer *buffer, u8 *storage, u32 size) {
    buffer->start = storage;
    buffer->size = size;
    buffer->read = buffer->start;
    buffer->write = buffer->start;
    buffer->readable = 0;
    buffer->writable = buffer->size;
    MWInitializeCriticalSection(&buffer->section);
}

int CircleBufferWriteBytes(CircleBuffer *buffer, u8 *data, u32 length) {
    u32 toEnd;

    if (length > buffer->writable) {
        return -1;
    }
    MWEnterCriticalSection(&buffer->section);
    toEnd = buffer->size - (buffer->write - buffer->start);
    if (toEnd >= length) {
        memcpy(buffer->write, data, length);
        buffer->write += length;
    } else {
        memcpy(buffer->write, data, toEnd);
        memcpy(buffer->start, data + toEnd, length - toEnd);
        buffer->write = buffer->start + length - toEnd;
    }
    if (buffer->size == buffer->write - buffer->start) {
        buffer->write = buffer->start;
    }
    buffer->writable -= length;
    buffer->readable += length;
    MWExitCriticalSection(&buffer->section);
    return 0;
}

int CircleBufferReadBytes(CircleBuffer *buffer, u8 *data, u32 length) {
    u32 toEnd;

    if (length > buffer->readable) {
        return -1;
    }
    MWEnterCriticalSection(&buffer->section);
    toEnd = buffer->size - (buffer->read - buffer->start);
    if (length < toEnd) {
        memcpy(data, buffer->read, length);
        buffer->read += length;
    } else {
        memcpy(data, buffer->read, toEnd);
        memcpy(data + toEnd, buffer->start, length - toEnd);
        buffer->read = buffer->start + length - toEnd;
    }
    if (buffer->size == buffer->read - buffer->start) {
        buffer->read = buffer->start;
    }
    buffer->writable += length;
    buffer->readable -= length;
    MWExitCriticalSection(&buffer->section);
    return 0;
}

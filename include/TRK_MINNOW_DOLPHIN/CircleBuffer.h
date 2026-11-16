#ifndef TRK_MINNOW_DOLPHIN_CIRCLEBUFFER_H
#define TRK_MINNOW_DOLPHIN_CIRCLEBUFFER_H

/* Helpers of the debugger-connection glue: a ring buffer for received bytes, a critical section
 * (interrupts off) and the trace output. */

#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Holds the interrupt state saved on entry. */
typedef struct MWCriticalSection {
    u32 interrupts;
} MWCriticalSection;

void MWInitializeCriticalSection(MWCriticalSection *section);
void MWEnterCriticalSection(MWCriticalSection *section);
void MWExitCriticalSection(MWCriticalSection *section);

typedef struct CircleBuffer {
    u8 *read;                   /* 0x00 */
    u8 *write;                  /* 0x04 */
    u8 *start;                  /* 0x08 */
    u32 size;                   /* 0x0C */
    u32 readable;               /* 0x10, bytes stored */
    u32 writable;               /* 0x14, bytes free */
    MWCriticalSection section;  /* 0x18 */
} CircleBuffer;                 /* 0x1C */

u32 CBGetBytesAvailableForRead(CircleBuffer *buffer);
void CircleBufferInitialize(CircleBuffer *buffer, u8 *storage, u32 size);
int CircleBufferWriteBytes(CircleBuffer *buffer, u8 *data, u32 length);
int CircleBufferReadBytes(CircleBuffer *buffer, u8 *data, u32 length);

void MWTRACE(u8 level, char *format, ...);

#ifdef __cplusplus
}
#endif

#endif

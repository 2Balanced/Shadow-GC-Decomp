#include <TRK_MINNOW_DOLPHIN/trk.h>

/* The nub's event queue: a ring of two events, filled by the exception handlers and the serial
 * poll and drained by TRKNubMainLoop. */

#define EVENT_QUEUE_SIZE 2
#define FIRST_EVENT_SERIAL 0x100

typedef struct TRKEventQueue {
    u32 mutex;                          /* 0x0 */
    int count;                          /* 0x4 */
    int first;                          /* 0x8 */
    TRKEvent events[EVENT_QUEUE_SIZE];  /* 0xC */
    u32 nextSerial;                     /* 0x24 */
} TRKEventQueue;                        /* 0x28 */

TRKEventQueue gTRKEventQueue;

TRKResult TRKInitializeEventQueue(void) {
    TRKInitializeMutex(&gTRKEventQueue);
    TRKAcquireMutex(&gTRKEventQueue);
    gTRKEventQueue.count = 0;
    gTRKEventQueue.first = 0;
    gTRKEventQueue.nextSerial = FIRST_EVENT_SERIAL;
    TRKReleaseMutex(&gTRKEventQueue);
    return kTRKOk;
}

BOOL TRKGetNextEvent(TRKEvent *event) {
    BOOL found = FALSE;

    TRKAcquireMutex(&gTRKEventQueue);
    if (gTRKEventQueue.count > 0) {
        TRK_memcpy(event, &gTRKEventQueue.events[gTRKEventQueue.first], sizeof(TRKEvent));
        gTRKEventQueue.count--;
        gTRKEventQueue.first++;
        if (gTRKEventQueue.first == EVENT_QUEUE_SIZE) {
            gTRKEventQueue.first = 0;
        }
        found = TRUE;
    }
    TRKReleaseMutex(&gTRKEventQueue);
    return found;
}

TRKResult TRKPostEvent(TRKEvent *event) {
    TRKResult result = kTRKOk;
    int slot;

    TRKAcquireMutex(&gTRKEventQueue);
    if (gTRKEventQueue.count == EVENT_QUEUE_SIZE) {
        result = kTRKEventQueueFull;
    } else {
        slot = (gTRKEventQueue.first + gTRKEventQueue.count) % EVENT_QUEUE_SIZE;
        TRK_memcpy(&gTRKEventQueue.events[slot], event, sizeof(TRKEvent));
        gTRKEventQueue.events[slot].serial = gTRKEventQueue.nextSerial;
        gTRKEventQueue.nextSerial++;
        if (gTRKEventQueue.nextSerial < FIRST_EVENT_SERIAL) {
            gTRKEventQueue.nextSerial = FIRST_EVENT_SERIAL;
        }
        gTRKEventQueue.count++;
    }
    TRKReleaseMutex(&gTRKEventQueue);
    return result;
}

void TRKConstructEvent(TRKEvent *event, int type) {
    event->type = type;
    event->serial = 0;
    event->bufferId = -1;
}

void TRKDestructEvent(TRKEvent *event) {
    TRKReleaseBuffer(event->bufferId);
}

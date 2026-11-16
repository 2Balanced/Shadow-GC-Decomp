/* Dolphin SDK DVD: the waiting queues. One circular doubly linked list per priority (0 is served
 * first); a DVDCommandBlock's next/prev fields are its links, and each list head is a bare pair
 * of links. */
#include <dolphin/dvd/__dvd.h>

#define MAX_QUEUES 4

typedef struct DVDQueue DVDQueue;
struct DVDQueue {
    DVDQueue *next;
    DVDQueue *prev;
};

static DVDQueue WaitingQueue[MAX_QUEUES];

void __DVDClearWaitingQueue(void) {
    u32 i;

    for (i = 0; i < MAX_QUEUES; i++) {
        DVDQueue *q = &WaitingQueue[i];

        q->next = q;
        q->prev = q;
    }
}

BOOL __DVDPushWaitingQueue(s32 prio, DVDCommandBlock *block) {
    BOOL enabled = OSDisableInterrupts();
    DVDQueue *q = &WaitingQueue[prio];

    q->prev->next = (DVDQueue *)block;
    block->prev = (DVDCommandBlock *)q->prev;
    block->next = (DVDCommandBlock *)q;
    q->prev = (DVDQueue *)block;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

DVDCommandBlock *__DVDPopWaitingQueue(void) {
    u32 i;
    BOOL enabled;
    DVDQueue *q;
    DVDCommandBlock *tmp;

    enabled = OSDisableInterrupts();
    for (i = 0; i < MAX_QUEUES; i++) {
        q = &WaitingQueue[i];
        if (q->next != q) {
            OSRestoreInterrupts(enabled);
            enabled = OSDisableInterrupts();
            q = &WaitingQueue[i];
            tmp = (DVDCommandBlock *)q->next;
            q->next = (DVDQueue *)tmp->next;
            tmp->next->prev = (DVDCommandBlock *)q;
            OSRestoreInterrupts(enabled);
            tmp->next = NULL;
            tmp->prev = NULL;
            return tmp;
        }
    }
    OSRestoreInterrupts(enabled);
    return NULL;
}

BOOL __DVDCheckWaitingQueue(void) {
    u32 i;
    BOOL enabled;
    DVDQueue *q;

    enabled = OSDisableInterrupts();
    for (i = 0; i < MAX_QUEUES; i++) {
        q = &WaitingQueue[i];
        if (q->next != q) {
            OSRestoreInterrupts(enabled);
            return TRUE;
        }
    }
    OSRestoreInterrupts(enabled);
    return FALSE;
}

BOOL fn_8037CDD8(DVDCommandBlock *block) {
    BOOL enabled;
    DVDCommandBlock *prev;
    DVDCommandBlock *next;

    enabled = OSDisableInterrupts();
    prev = block->prev;
    next = block->next;
    if (prev == NULL || next == NULL) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }
    prev->next = next;
    next->prev = prev;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

/* Dolphin SDK ARQ: a queue in front of the single ARAM DMA channel.
 *
 * Requests go into a high- or a low-priority FIFO. A high-priority request is moved in one DMA; a
 * low-priority one is cut into chunks of __ARQChunkSize bytes so a high-priority request never
 * waits for more than one chunk. Each DMA completion (the AR callback) reports the finished
 * request to its owner and starts the next transfer, high-priority work first. */
#include <dolphin/os.h>
#include <dolphin/os_interrupt.h>

/* The RenderWare units call this one by its placeholder symbol. */
#define ARQPostRequest fn_803836F4

typedef void (*ARCallback)(void);
ARCallback ARRegisterDMACallback(ARCallback callback);
void ARStartDMA(u32 type, u32 mainmem, u32 aram, u32 length);

typedef void (*ARQCallback)(u32 request);

typedef struct ARQRequest {
    struct ARQRequest *next;
    u32 owner;
    u32 type; /* 0: main memory to ARAM, 1: ARAM to main memory */
    u32 priority;
    u32 source;
    u32 dest;
    u32 length;
    ARQCallback callback;
} ARQRequest;

#define ARQ_TYPE_MRAM_TO_ARAM 0
#define ARQ_PRIORITY_LOW 0
#define ARQ_PRIORITY_HIGH 1

const char *__ARQVersion = "<< Dolphin SDK - ARQ\trelease build: Apr  5 2004 04:15:04 (0x2301) >>";

static ARQRequest *__ARQRequestQueueHi;
static ARQRequest *__ARQRequestTailHi;
static ARQRequest *__ARQRequestQueueLo;
static ARQRequest *__ARQRequestTailLo;
static ARQRequest *__ARQRequestPendingHi;
static ARQRequest *__ARQRequestPendingLo;
static ARQCallback __ARQCallbackHi;
static ARQCallback __ARQCallbackLo;
static u32 __ARQChunkSize;
static volatile BOOL __ARQ_init_flag;

/* Start the oldest high-priority request in one go. */
static inline void __ARQPopTaskQueueHi(void) {
    if (__ARQRequestQueueHi) {
        if (__ARQRequestQueueHi->type == ARQ_TYPE_MRAM_TO_ARAM) {
            ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->source,
                       __ARQRequestQueueHi->dest, __ARQRequestQueueHi->length);
        } else {
            ARStartDMA(__ARQRequestQueueHi->type, __ARQRequestQueueHi->dest,
                       __ARQRequestQueueHi->source, __ARQRequestQueueHi->length);
        }
        __ARQCallbackHi = __ARQRequestQueueHi->callback;
        __ARQRequestPendingHi = __ARQRequestQueueHi;
        __ARQRequestQueueHi = __ARQRequestQueueHi->next;
    }
}

/* Move the next chunk of the current low-priority request, taking a new request when idle. The
 * callback is only armed for the last chunk. */
static void __ARQServiceQueueLo(void) {
    if (__ARQRequestPendingLo == NULL && __ARQRequestQueueLo) {
        __ARQRequestPendingLo = __ARQRequestQueueLo;
        __ARQRequestQueueLo = __ARQRequestQueueLo->next;
    }
    if (__ARQRequestPendingLo) {
        if (__ARQRequestPendingLo->length <= __ARQChunkSize) {
            if (__ARQRequestPendingLo->type == ARQ_TYPE_MRAM_TO_ARAM) {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->source,
                           __ARQRequestPendingLo->dest, __ARQRequestPendingLo->length);
            } else {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->dest,
                           __ARQRequestPendingLo->source, __ARQRequestPendingLo->length);
            }
            __ARQCallbackLo = __ARQRequestPendingLo->callback;
        } else {
            if (__ARQRequestPendingLo->type == ARQ_TYPE_MRAM_TO_ARAM) {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->source,
                           __ARQRequestPendingLo->dest, __ARQChunkSize);
            } else {
                ARStartDMA(__ARQRequestPendingLo->type, __ARQRequestPendingLo->dest,
                           __ARQRequestPendingLo->source, __ARQChunkSize);
            }
        }
        __ARQRequestPendingLo->length -= __ARQChunkSize;
        __ARQRequestPendingLo->source += __ARQChunkSize;
        __ARQRequestPendingLo->dest += __ARQChunkSize;
    }
}

/* Stands in for a missing completion callback. */
static void __ARQCallbackHack(u32 request) {}

static void __ARQInterruptServiceRoutine(void) {
    if (__ARQCallbackHi) {
        __ARQCallbackHi((u32)__ARQRequestPendingHi);
        __ARQRequestPendingHi = NULL;
        __ARQCallbackHi = NULL;
    } else if (__ARQCallbackLo) {
        __ARQCallbackLo((u32)__ARQRequestPendingLo);
        __ARQRequestPendingLo = NULL;
        __ARQCallbackLo = NULL;
    }
    __ARQPopTaskQueueHi();
    if (__ARQRequestPendingHi == NULL) {
        __ARQServiceQueueLo();
    }
}

void ARQInit(void) {
    if (__ARQ_init_flag == TRUE) {
        return;
    }
    OSRegisterVersion(__ARQVersion);
    __ARQRequestQueueHi = __ARQRequestQueueLo = NULL;
    __ARQChunkSize = 0x1000;
    ARRegisterDMACallback(__ARQInterruptServiceRoutine);
    __ARQRequestPendingHi = NULL;
    __ARQRequestPendingLo = NULL;
    __ARQCallbackHi = NULL;
    __ARQCallbackLo = NULL;
    __ARQ_init_flag = TRUE;
}

void ARQPostRequest(ARQRequest *request, u32 owner, u32 type, u32 priority, u32 source, u32 dest,
                    u32 length, ARQCallback callback) {
    BOOL enabled;

    request->next = NULL;
    request->owner = owner;
    request->type = type;
    request->source = source;
    request->dest = dest;
    request->length = length;
    if (callback) {
        request->callback = callback;
    } else {
        request->callback = __ARQCallbackHack;
    }

    enabled = OSDisableInterrupts();
    switch (priority) {
    case ARQ_PRIORITY_LOW:
        if (__ARQRequestQueueLo) {
            __ARQRequestTailLo->next = request;
        } else {
            __ARQRequestQueueLo = request;
        }
        __ARQRequestTailLo = request;
        break;
    case ARQ_PRIORITY_HIGH:
        if (__ARQRequestQueueHi) {
            __ARQRequestTailHi->next = request;
        } else {
            __ARQRequestQueueHi = request;
        }
        __ARQRequestTailHi = request;
        break;
    }

    if (__ARQRequestPendingHi == NULL && __ARQRequestPendingLo == NULL) {
        __ARQPopTaskQueueHi();
        if (__ARQRequestPendingHi == NULL) {
            __ARQServiceQueueLo();
        }
    }
    OSRestoreInterrupts(enabled);
}

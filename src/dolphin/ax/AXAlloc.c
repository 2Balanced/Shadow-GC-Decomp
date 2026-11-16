/* Dolphin SDK AX: voice allocation.
 *
 * Voices live on 32 doubly linked stacks, one per priority; stack 0 holds the free voices. A new
 * voice goes on top of its priority's stack. When no voice is free, AXAcquireVoice steals the
 * oldest voice (the bottom of the stack) of the lowest priority below the one asked for, telling
 * its owner through the voice callback. Voices whose owner must be told from outside the DSP
 * interrupt are parked on a separate callback stack. */
#include <dolphin/os.h>
#include <dolphin/ax.h>

/* The RenderWare units call this one by its placeholder symbol. */
#define AXAcquireVoice fn_80383C48

#define AX_PRIORITY_STACKS 32
#define AX_PRIORITY_FREE 0

void __AXSetPBDefault(AXVPB *voice);

static AXVPB *__AXCallbackStack;
static AXVPB *__AXStackHead[AX_PRIORITY_STACKS];
static AXVPB *__AXStackTail[AX_PRIORITY_STACKS];

AXVPB *__AXGetStackHead(u32 priority) { return __AXStackHead[priority]; }

AXVPB *__AXPopCallbackStack(void);
void __AXRemoveFromStack(AXVPB *voice);
void __AXPushFreeStack(AXVPB *voice);

void __AXServiceCallbackStack(void) {
    AXVPB *voice;

    for (voice = __AXPopCallbackStack(); voice; voice = __AXPopCallbackStack()) {
        if (voice->priority) {
            if (voice->callback) {
                voice->callback(voice);
            }
            __AXRemoveFromStack(voice);
            __AXPushFreeStack(voice);
        }
    }
}

void __AXAllocInit(void) {
    u32 i;

    __AXCallbackStack = NULL;
    for (i = 0; i < AX_PRIORITY_STACKS; i++) {
        __AXStackHead[i] = __AXStackTail[i] = NULL;
    }
}

void __AXAllocQuit(void) {
    u32 i;

    __AXCallbackStack = NULL;
    for (i = 0; i < AX_PRIORITY_STACKS; i++) {
        __AXStackHead[i] = __AXStackTail[i] = NULL;
    }
}

void __AXPushFreeStack(AXVPB *voice) {
    voice->next = __AXStackHead[AX_PRIORITY_FREE];
    __AXStackHead[AX_PRIORITY_FREE] = voice;
    voice->priority = AX_PRIORITY_FREE;
}

void __AXPushCallbackStack(AXVPB *voice) {
    voice->next1 = __AXCallbackStack;
    __AXCallbackStack = voice;
}

AXVPB *__AXPopCallbackStack(void) {
    AXVPB *voice = __AXCallbackStack;

    if (voice) {
        __AXCallbackStack = voice->next1;
    }
    return voice;
}

void __AXRemoveFromStack(AXVPB *voice) {
    u32 priority = voice->priority;
    AXVPB *head = __AXStackHead[priority];
    AXVPB *tail = __AXStackTail[priority];

    if (head == tail) {
        __AXStackHead[priority] = __AXStackTail[priority] = NULL;
        return;
    }
    if (voice == head) {
        __AXStackHead[priority] = voice->next;
        __AXStackHead[priority]->prev = NULL;
        return;
    }
    if (voice == tail) {
        __AXStackTail[priority] = voice->prev;
        __AXStackTail[priority]->next = NULL;
        return;
    }
    head = voice->prev;
    tail = voice->next;
    head->next = tail;
    tail->prev = head;
}

static inline AXVPB *__AXPopFreeStack(void) {
    AXVPB *voice = __AXStackHead[AX_PRIORITY_FREE];

    if (voice) {
        __AXStackHead[AX_PRIORITY_FREE] = voice->next;
    }
    return voice;
}

static inline AXVPB *__AXPopStackFromBottom(u32 priority) {
    AXVPB *voice = NULL;

    if (__AXStackHead[priority]) {
        if (__AXStackHead[priority] == __AXStackTail[priority]) {
            voice = __AXStackHead[priority];
            __AXStackHead[priority] = __AXStackTail[priority] = NULL;
        } else if (__AXStackTail[priority]) {
            voice = __AXStackTail[priority];
            __AXStackTail[priority] = voice->prev;
            __AXStackTail[priority]->next = NULL;
        }
    }
    return voice;
}

static inline void __AXPushStackTop(AXVPB *voice, u32 priority) {
    voice->next = __AXStackHead[priority];
    voice->prev = NULL;
    if (voice->next) {
        __AXStackHead[priority]->prev = voice;
        __AXStackHead[priority] = voice;
    } else {
        __AXStackTail[priority] = voice;
        __AXStackHead[priority] = voice;
    }
}

void AXFreeVoice(AXVPB *voice) {
    BOOL enabled = OSDisableInterrupts();

    __AXRemoveFromStack(voice);
    if (voice->pb.state == 1) {
        voice->depop = TRUE;
    }
    __AXSetPBDefault(voice);
    __AXPushFreeStack(voice);
    OSRestoreInterrupts(enabled);
}

AXVPB *AXAcquireVoice(u32 priority, AXVoiceCallback callback, u32 userContext) {
    BOOL enabled = OSDisableInterrupts();
    AXVPB *voice = __AXPopFreeStack();
    u32 i;

    if (voice == NULL) {
        for (i = 1; i < priority; i++) {
            voice = __AXPopStackFromBottom(i);
            if (voice) {
                if (voice->pb.state == 1) {
                    voice->depop = TRUE;
                }
                if (voice->callback) {
                    voice->callback(voice);
                }
                break;
            }
        }
    }
    if (voice) {
        __AXPushStackTop(voice, priority);
        voice->priority = priority;
        voice->callback = callback;
        voice->userContext = userContext;
        __AXSetPBDefault(voice);
    }
    OSRestoreInterrupts(enabled);
    return voice;
}

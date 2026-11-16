/* Dolphin SDK AXFX: the delay effect. One delay line per bus (left, right, surround), each with
 * its own length, feedback and output level; the aux callback feeds the bus through its line.
 * Lines are sized in audio frames of 160 samples.
 *
 * Not linked yet: AXFXDelaySettings differs in register choice only. Retail keeps the six
 * feedback and output values of the range check in r9..r4 and loads the zero it stores later
 * (r31) in the middle of that check; this source uses r0 for all of them. */
#include <dolphin/os.h>

/* Placeholder symbols are kept: nothing in the binary names these functions, and other units
 * call them by these symbols. The names are the public AXFX API they correspond to. */
#define AXFXDelayCallback fn_80388D50
#define AXFXDelaySettings fn_80388F20
#define AXFXDelayInit fn_803891E8
#define AXFXDelayShutdown fn_80389230

#define AXFX_FRAME_SAMPLES 160

typedef void *(*AXFXAllocHook)(u32 size);
typedef void (*AXFXFreeHook)(void *block);

extern AXFXAllocHook __AXFXAlloc;
extern AXFXFreeHook __AXFXFree;

typedef struct AXFX_BUFFERUPDATE {
    s32 *left;
    s32 *right;
    s32 *surround;
} AXFX_BUFFERUPDATE;

typedef struct AXFX_DELAY {
    u32 currentSize[3];     /* 0x00: line length in frames */
    u32 currentPos[3];      /* 0x0C: frame the line is at */
    u32 currentFeedback[3]; /* 0x18: 0..128 */
    u32 currentOutput[3];   /* 0x24: 0..128 */
    s32 *left;              /* 0x30 */
    s32 *right;             /* 0x34 */
    s32 *sur;               /* 0x38 */
    /* Set by the caller before Init or Settings. */
    u32 delay[3];           /* 0x3C: milliseconds, 10..5000 */
    u32 feedback[3];        /* 0x48: percent */
    u32 output[3];          /* 0x54: percent */
} AXFX_DELAY;

int AXFXDelayShutdown(AXFX_DELAY *delay);

void AXFXDelayCallback(AXFX_BUFFERUPDATE *bufferUpdate, AXFX_DELAY *delay) {
    s32 *leftLine = delay->left + delay->currentPos[0] * AXFX_FRAME_SAMPLES;
    s32 *rightLine = delay->right + delay->currentPos[1] * AXFX_FRAME_SAMPLES;
    s32 *surLine = delay->sur + delay->currentPos[2] * AXFX_FRAME_SAMPLES;
    s32 *left = bufferUpdate->left;
    s32 *right = bufferUpdate->right;
    s32 *sur = bufferUpdate->surround;
    s32 l;
    s32 r;
    s32 s;
    u32 i;

    for (i = 0; i < AXFX_FRAME_SAMPLES; i++) {
        l = *leftLine;
        r = *rightLine;
        s = *surLine;
        *leftLine++ = *left + ((s32)(l * delay->currentFeedback[0]) >> 7);
        *rightLine++ = *right + ((s32)(r * delay->currentFeedback[1]) >> 7);
        *surLine++ = *sur + ((s32)(s * delay->currentFeedback[2]) >> 7);
        *left++ = (s32)(l * delay->currentOutput[0]) >> 7;
        *right++ = (s32)(r * delay->currentOutput[1]) >> 7;
        *sur++ = (s32)(s * delay->currentOutput[2]) >> 7;
    }

    delay->currentPos[0] = (delay->currentPos[0] + 1) % delay->currentSize[0];
    delay->currentPos[1] = (delay->currentPos[1] + 1) % delay->currentSize[1];
    delay->currentPos[2] = (delay->currentPos[2] + 1) % delay->currentSize[2];
}

int AXFXDelaySettings(AXFX_DELAY *delay) {
    u32 i;
    s32 *left;
    s32 *right;
    s32 *sur;
    BOOL enabled;

    if (delay->delay[0] < 10 || delay->delay[0] > 5000 || delay->delay[1] < 10 ||
        delay->delay[1] > 5000 || delay->delay[2] < 10 || delay->delay[2] > 5000 ||
        delay->feedback[0] > 100 || delay->feedback[1] > 100 || delay->feedback[2] > 100 ||
        delay->output[0] > 100 || delay->output[1] > 100 || delay->output[2] > 100) {
        return FALSE;
    }

    AXFXDelayShutdown(delay);

    enabled = OSDisableInterrupts();

    for (i = 0; i < 3; i++) {
        delay->currentSize[i] =
            ((delay->delay[i] - 5) * 32 + (AXFX_FRAME_SAMPLES - 1)) / AXFX_FRAME_SAMPLES;
        delay->currentPos[i] = 0;
        delay->currentFeedback[i] = (delay->feedback[i] << 7) / 100;
        delay->currentOutput[i] = (delay->output[i] << 7) / 100;
    }

    delay->left = __AXFXAlloc(delay->currentSize[0] * AXFX_FRAME_SAMPLES * sizeof(s32));
    delay->right = __AXFXAlloc(delay->currentSize[1] * AXFX_FRAME_SAMPLES * sizeof(s32));
    delay->sur = __AXFXAlloc(delay->currentSize[2] * AXFX_FRAME_SAMPLES * sizeof(s32));

    /* As shipped: this path leaves interrupts disabled. */
    if (delay->left == NULL || delay->right == NULL || delay->sur == NULL) {
        AXFXDelayShutdown(delay);
        return FALSE;
    }

    left = delay->left;
    right = delay->right;
    sur = delay->sur;

    for (i = 0; i < delay->currentSize[0] * AXFX_FRAME_SAMPLES; i++) {
        *left++ = 0;
    }
    for (i = 0; i < delay->currentSize[1] * AXFX_FRAME_SAMPLES; i++) {
        *right++ = 0;
    }
    for (i = 0; i < delay->currentSize[2] * AXFX_FRAME_SAMPLES; i++) {
        *sur++ = 0;
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

int AXFXDelayInit(AXFX_DELAY *delay) {
    BOOL enabled = OSDisableInterrupts();

    delay->left = NULL;
    delay->right = NULL;
    delay->sur = NULL;
    OSRestoreInterrupts(enabled);
    return AXFXDelaySettings(delay);
}

int AXFXDelayShutdown(AXFX_DELAY *delay) {
    BOOL enabled = OSDisableInterrupts();

    if (delay->left != NULL) {
        __AXFXFree(delay->left);
    }
    if (delay->right != NULL) {
        __AXFXFree(delay->right);
    }
    if (delay->sur != NULL) {
        __AXFXFree(delay->sur);
    }
    delay->left = NULL;
    delay->right = NULL;
    delay->sur = NULL;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

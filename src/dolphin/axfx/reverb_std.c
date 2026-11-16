/* Dolphin SDK AXFX: the standard reverb.
 *
 * Only the entry points that wrap the work-area routines are written so far; the routines
 * themselves (create at 0x80387898, the filter at 0x80387D14, free at 0x803880C8) are still to
 * do, so the unit is not linked.
 *
 * Built with the peephole pass off (configure.py: -opt nopeephole): register copies come out as
 * `mr`. */
#include <dolphin/os.h>

/* Placeholder symbols are kept: nothing in the binary names these functions. The names are the
 * public AXFX API they correspond to. */
#define ReverbSTDCreate fn_80387898
#define HandleReverb fn_80387D14
#define ReverbSTDFree fn_803880C8
#define AXFXReverbStdInit fn_803881EC
#define AXFXReverbStdShutdown fn_80388258
#define AXFXReverbStdCallback fn_803882A4

typedef struct AXFX_BUFFERUPDATE {
    s32 *left;
    s32 *right;
    s32 *surround;
} AXFX_BUFFERUPDATE;

typedef void (*AXFXFreeHook)(void *block);
extern AXFXFreeHook __AXFXFree;

typedef struct AXFX_REVSTD_DELAYLINE {
    s32 inPoint;     /* 0x00 */
    s32 outPoint;    /* 0x04 */
    s32 length;      /* 0x08 */
    f32 *inputs;     /* 0x0C */
    f32 lastOutput;  /* 0x10 */
} AXFX_REVSTD_DELAYLINE;

/* Only the parts the written functions touch are laid out. */
typedef struct AXFX_REVSTD_WORK {
    AXFX_REVSTD_DELAYLINE AP[6];  /* 0x000: all-pass lines */
    AXFX_REVSTD_DELAYLINE C[6];   /* 0x078: comb lines */
    u8 filterState[0x30]; /* 0x0F0 */
    s32 preDelayTime;     /* 0x120 */
    f32 *preDelayLine[3]; /* 0x124 */
    f32 *preDelayPtr[3];  /* 0x130 */
} AXFX_REVSTD_WORK;

typedef struct AXFX_REVERBSTD {
    AXFX_REVSTD_WORK rv; /* 0x000 */
    u8 tempDisableFX;    /* 0x13C */
    f32 coloration;      /* 0x140 */
    f32 mix;             /* 0x144 */
    f32 time;            /* 0x148 */
    f32 damping;         /* 0x14C */
    f32 preDelay;        /* 0x150 */
} AXFX_REVERBSTD;

int ReverbSTDCreate(AXFX_REVSTD_WORK *rv, f32 coloration, f32 time, f32 mix, f32 damping,
                    f32 preDelay);
void HandleReverb(s32 *samples, AXFX_REVSTD_WORK *rv);
void ReverbSTDFree(AXFX_REVSTD_WORK *rv);

void ReverbSTDFree(AXFX_REVSTD_WORK *rv) {
    u8 i;

    for (i = 0; i < 6; i++) {
        if (rv->AP[i].inputs != NULL) {
            __AXFXFree(rv->AP[i].inputs);
            rv->AP[i].inputs = NULL;
        }
    }
    for (i = 0; i < 6; i++) {
        if (rv->C[i].inputs != NULL) {
            __AXFXFree(rv->C[i].inputs);
            rv->C[i].inputs = NULL;
        }
    }
    if (rv->preDelayTime != 0) {
        for (i = 0; i < 3; i++) {
            if (rv->preDelayLine[i] != NULL) {
                __AXFXFree(rv->preDelayLine[i]);
                rv->preDelayLine[i] = NULL;
            }
        }
    }
}

int AXFXReverbStdInit(AXFX_REVERBSTD *reverb) {
    int result;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    reverb->tempDisableFX = FALSE;
    result = ReverbSTDCreate(&reverb->rv, reverb->coloration, reverb->time, reverb->mix,
                             reverb->damping, reverb->preDelay);
    OSRestoreInterrupts(enabled);
    return result;
}

int AXFXReverbStdShutdown(AXFX_REVERBSTD *reverb) {
    BOOL enabled = OSDisableInterrupts();

    ReverbSTDFree(&reverb->rv);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

void AXFXReverbStdCallback(AXFX_BUFFERUPDATE *bufferUpdate, AXFX_REVERBSTD *reverb) {
    if (!reverb->tempDisableFX) {
        HandleReverb(bufferUpdate->left, &reverb->rv);
    }
}

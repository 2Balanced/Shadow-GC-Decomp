/* Dolphin SDK AXFX: the high-quality reverb.
 *
 * Only the entry points that wrap the work-area routines are written so far; the routines
 * themselves (create at 0x80386990, crosstalk at 0x80386F18, the per-bus filter at 0x803870A8,
 * free at 0x803875B4) are still to do, so the unit is not
 * linked.
 *
 * Built with the peephole pass off (configure.py: -opt nopeephole): register copies come out as
 * `mr` and the callback keeps its branches to the next instruction. */
#include <dolphin/os.h>

/* Placeholder symbols are kept: nothing in the binary names these functions. The names are the
 * public AXFX API they correspond to. */
#define ReverbHICreate fn_80386990
#define ReverbHIFree fn_803875B4
#define DoCrossTalk fn_80386F18
#define HandleReverb fn_803870A8
#define AXFXReverbHiInit fn_803876D8
#define AXFXReverbHiShutdown fn_80387748
#define AXFXReverbHiCallback fn_80387794

typedef struct AXFX_BUFFERUPDATE {
    s32 *left;
    s32 *right;
    s32 *surround;
} AXFX_BUFFERUPDATE;

typedef void (*AXFXFreeHook)(void *block);
extern AXFXFreeHook __AXFXFree;

typedef struct AXFX_REVHI_DELAYLINE {
    s32 inPoint;     /* 0x00 */
    s32 outPoint;    /* 0x04 */
    s32 length;      /* 0x08 */
    f32 *inputs;     /* 0x0C */
    f32 lastOutput;  /* 0x10 */
} AXFX_REVHI_DELAYLINE;

/* Only the parts the written functions touch are laid out. */
typedef struct AXFX_REVHI_WORK {
    AXFX_REVHI_DELAYLINE AP[9];  /* 0x000: all-pass lines */
    AXFX_REVHI_DELAYLINE C[9];   /* 0x0B4: comb lines */
    u8 filterState[0x3C]; /* 0x168 */
    s32 preDelayTime;     /* 0x1A4 */
    f32 crosstalk;        /* 0x1A8 */
    f32 *preDelayLine[3]; /* 0x1AC */
    f32 *preDelayPtr[3];  /* 0x1B8 */
} AXFX_REVHI_WORK;

typedef struct AXFX_REVERBHI {
    AXFX_REVHI_WORK rv; /* 0x000 */
    u8 tempDisableFX;   /* 0x1C4 */
    f32 coloration;     /* 0x1C8 */
    f32 mix;            /* 0x1CC */
    f32 time;           /* 0x1D0 */
    f32 damping;        /* 0x1D4 */
    f32 preDelay;       /* 0x1D8 */
    f32 crosstalk;      /* 0x1DC */
} AXFX_REVERBHI;

int ReverbHICreate(AXFX_REVHI_WORK *rv, f32 coloration, f32 time, f32 mix, f32 damping,
                   f32 preDelay, f32 crosstalk);
void ReverbHIFree(AXFX_REVHI_WORK *rv);
void DoCrossTalk(s32 *left, s32 *right, f32 cross, f32 direct);
void HandleReverb(s32 *samples, AXFX_REVHI_WORK *rv, s32 bus);

void ReverbHIFree(AXFX_REVHI_WORK *rv) {
    u8 i;

    for (i = 0; i < 9; i++) {
        if (rv->AP[i].inputs != NULL) {
            __AXFXFree(rv->AP[i].inputs);
            rv->AP[i].inputs = NULL;
        }
    }
    for (i = 0; i < 9; i++) {
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

int AXFXReverbHiInit(AXFX_REVERBHI *reverb) {
    int result;
    BOOL enabled;

    enabled = OSDisableInterrupts();
    reverb->tempDisableFX = FALSE;
    result = ReverbHICreate(&reverb->rv, reverb->coloration, reverb->time, reverb->mix,
                            reverb->damping, reverb->preDelay, reverb->crosstalk);
    OSRestoreInterrupts(enabled);
    return result;
}

int AXFXReverbHiShutdown(AXFX_REVERBHI *reverb) {
    BOOL enabled = OSDisableInterrupts();

    ReverbHIFree(&reverb->rv);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

void AXFXReverbHiCallback(AXFX_BUFFERUPDATE *bufferUpdate, AXFX_REVERBHI *reverb) {
    u8 i;
    s32 *left;
    s32 *right;
    s32 *sur;

    if (!reverb->tempDisableFX) {
        sur = bufferUpdate->surround;
        right = bufferUpdate->right;
        left = bufferUpdate->left;
        for (i = 0; i < 3; i++) {
            switch (i) {
            case 0:
                if (reverb->rv.crosstalk != 0.0f) {
                    DoCrossTalk(left, right, 0.5f * reverb->rv.crosstalk,
                                1.0f - 0.5f * reverb->rv.crosstalk);
                }
                HandleReverb(left, &reverb->rv, 0);
                break;
            case 1:
                HandleReverb(right, &reverb->rv, 1);
                break;
            case 2:
                HandleReverb(sur, &reverb->rv, 2);
                break;
            }
        }
    }
}

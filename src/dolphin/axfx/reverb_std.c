/* Dolphin SDK AXFX: the standard reverb.
 *
 * Three buses (left, right, surround), each with two comb lines feeding two all-pass lines, a
 * one-pole low-pass for damping and an optional pre-delay line. The per-frame filter is
 * hand-written assembly in the original; everything after it in the file is compiled with the
 * peephole pass off, which the compiler does by itself after an asm function.
 *
 * Not linked yet: ReverbSTDCreate (0x80387898) differs in register choice only. Retail keeps the work
 * area pointer in r31 and everything else one register lower; this source gets the pointer in
 * the lowest saved register instead. Every other function and all data match. */
#include <dolphin/os.h>

#pragma fp_contract off

/* Placeholder symbols are kept: nothing in the binary names these functions. The names are the
 * public AXFX API they correspond to. */
#define powf fn_803B1F00
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

typedef void *(*AXFXAllocHook)(u32 size);
typedef void (*AXFXFreeHook)(void *block);
extern AXFXAllocHook __AXFXAlloc;
extern AXFXFreeHook __AXFXFree;

void *memset(void *dst, int value, unsigned long n);
f32 powf(f32 x, f32 y);

typedef struct AXFX_REVSTD_DELAYLINE {
    s32 inPoint;     /* 0x00 */
    s32 outPoint;    /* 0x04 */
    s32 length;      /* 0x08 */
    f32 *inputs;     /* 0x0C */
    f32 lastOutput;  /* 0x10 */
} AXFX_REVSTD_DELAYLINE;

typedef struct AXFX_REVSTD_WORK {
    AXFX_REVSTD_DELAYLINE AP[6]; /* 0x000: all-pass lines, two per bus */
    AXFX_REVSTD_DELAYLINE C[6];  /* 0x078: comb lines, two per bus */
    f32 allPassCoeff;            /* 0x0F0 */
    f32 combCoef[6];             /* 0x0F4 */
    f32 lpLastout[3];            /* 0x10C */
    f32 level;                   /* 0x118 */
    f32 damping;                 /* 0x11C */
    s32 preDelayTime;            /* 0x120: samples */
    f32 *preDelayLine[3];        /* 0x124 */
    f32 *preDelayPtr[3];         /* 0x130 */
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

void ReverbSTDFree(AXFX_REVSTD_WORK *rv);

/* Line lengths in samples: the two combs, then the two all-passes. */
static s32 lens[4] = { 1789, 1999, 433, 149 };

static inline void DLsetdelay(AXFX_REVSTD_DELAYLINE *dl, s32 lag) {
    dl->outPoint = dl->inPoint - lag * sizeof(f32);
    while (dl->outPoint < 0) {
        dl->outPoint += dl->length;
    }
}

static inline BOOL DLcreate(AXFX_REVSTD_DELAYLINE *dl, s32 length) {
    s32 size = length * sizeof(f32);

    dl->length = size;
    dl->inputs = __AXFXAlloc(size);
    if (dl->inputs == NULL) {
        return FALSE;
    }
    memset(dl->inputs, 0, size);
    dl->lastOutput = 0.0f;
    DLsetdelay(dl, length >> 1);
    dl->inPoint = 0;
    dl->outPoint = 0;
    return TRUE;
}

int ReverbSTDCreate(AXFX_REVSTD_WORK *rv, f32 coloration, f32 time, f32 mix, f32 damping,
                    f32 preDelay) {
    u8 i;
    u8 k;

    if (coloration < 0.0f || coloration > 1.0f || time < 0.01f || time > 10.0f || mix < 0.0f ||
        mix > 1.0f || damping < 0.0f || damping > 1.0f || preDelay < 0.0f || preDelay > 0.1f) {
        return FALSE;
    }

    memset(rv, 0, sizeof(AXFX_REVSTD_WORK));

    for (i = 0; i < 3; i++) {
        for (k = 0; k < 2; k++) {
            if (!DLcreate(&rv->C[k + i * 2], lens[k] + 2)) {
                ReverbSTDFree(rv);
                return FALSE;
            }
            DLsetdelay(&rv->C[k + i * 2], lens[k]);
            rv->combCoef[k + i * 2] = powf(10.0f, (lens[k] * -3) / (32000.0f * time));
        }
        for (k = 0; k < 2; k++) {
            if (!DLcreate(&rv->AP[k + i * 2], lens[k + 2] + 2)) {
                ReverbSTDFree(rv);
                return FALSE;
            }
            DLsetdelay(&rv->AP[k + i * 2], lens[k + 2]);
        }
        rv->lpLastout[i] = 0.0f;
    }

    rv->allPassCoeff = coloration;
    rv->level = mix;
    rv->damping = damping;
    if (rv->damping < 0.05f) {
        rv->damping = 0.05f;
    }
    rv->damping = 1.0f - (0.05f + 0.8f * rv->damping);

    if (preDelay != 0.0f) {
        rv->preDelayTime = 32000.0f * preDelay;
        for (i = 0; i < 3; i++) {
            rv->preDelayLine[i] = __AXFXAlloc(rv->preDelayTime * sizeof(f32));
            if (rv->preDelayLine[i] == NULL) {
                ReverbSTDFree(rv);
                return FALSE;
            }
            memset(rv->preDelayLine[i], 0, rv->preDelayTime * sizeof(f32));
            rv->preDelayPtr[i] = rv->preDelayLine[i];
        }
    } else {
        rv->preDelayTime = 0;
        for (i = 0; i < 3; i++) {
            rv->preDelayPtr[i] = NULL;
            rv->preDelayLine[i] = NULL;
        }
    }
    return TRUE;
}

static const f32 value0_3 = 0.3f;
static const f32 value0_6 = 0.6f;
static const double i2fMagic = 4503601774854144.0;

/* One frame of all three buses, in place. Hand-written assembly in the original. */
asm void HandleReverb(register s32 *samples, register AXFX_REVSTD_WORK *rv) {
    nofralloc
    stwu r1, -0x90(r1)
    stmw r17, 0x8(r1)
    stfd f14, 0x58(r1)
    stfd f15, 0x60(r1)
    stfd f16, 0x68(r1)
    stfd f17, 0x70(r1)
    stfd f18, 0x78(r1)
    stfd f19, 0x80(r1)
    stfd f20, 0x88(r1)
    lis r31, value0_3@ha
    lfs f6, value0_3@l(r31)
    lis r31, value0_6@ha
    lfs f9, value0_6@l(r31)
    lis r31, i2fMagic@ha
    lfd f5, i2fMagic@l(r31)
    lfs f2, 0xf0(r4)
    lfs f11, 0x11c(r4)
    lfs f8, 0x118(r4)
    fmuls f3, f8, f9
    fsubs f4, f9, f3
    lis r30, 0x4330
    stw r30, 0x50(r1)
    li r5, 0x0
L_80387D70:
    slwi r31, r5, 3
    add r31, r31, r4
    lfs f19, 0xf4(r31)
    lfs f20, 0xf8(r31)
    slwi r31, r5, 2
    add r31, r31, r4
    lfs f7, 0x10c(r31)
    lwz r27, 0x124(r31)
    lwz r28, 0x130(r31)
    lwz r31, 0x120(r4)
    subi r22, r31, 0x1
    slwi r22, r22, 2
    add r22, r22, r27
    cmpwi cr7, r31, 0x0
    mulli r31, r5, 0x28
    addi r29, r4, 0x78
    add r29, r29, r31
    addi r30, r4, 0x0
    add r30, r30, r31
    lwz r21, 0x0(r29)
    lwz r20, 0x4(r29)
    lwz r19, 0x14(r29)
    lwz r18, 0x18(r29)
    lfs f15, 0x10(r29)
    lfs f16, 0x24(r29)
    lwz r26, 0x8(r29)
    lwz r25, 0x1c(r29)
    lwz r7, 0xc(r29)
    lwz r8, 0x20(r29)
    lwz r12, 0x0(r30)
    lwz r11, 0x4(r30)
    lwz r10, 0x14(r30)
    lwz r9, 0x18(r30)
    lfs f17, 0x10(r30)
    lfs f18, 0x24(r30)
    lwz r24, 0x8(r30)
    lwz r23, 0x1c(r30)
    lwz r17, 0xc(r30)
    lwz r6, 0x20(r30)
    lwz r30, 0x0(r3)
    xoris r30, r30, 0x8000
    stw r30, 0x54(r1)
    lfd f12, 0x50(r1)
    fsubs f12, f12, f5
    li r31, 0x9f
    mtctr r31
L_80387E28:
    fmr f13, f12
    beq cr7, L_80387E48
    lfs f13, 0x0(r28)
    addi r28, r28, 0x4
    cmpw r28, r22
    stfs f12, -0x4(r28)
    bne+ L_80387E48
    mr r28, r27
L_80387E48:
    fmadds f8, f19, f15, f13
    lwzu r29, 0x4(r3)
    fmadds f9, f20, f16, f13
    stfsx f8, r7, r21
    addi r21, r21, 0x4
    stfsx f9, r8, r19
    lfsx f14, r7, r20
    addi r20, r20, 0x4
    lfsx f16, r8, r18
    cmpw r21, r26
    cmpw cr1, r20, r26
    addi r19, r19, 0x4
    addi r18, r18, 0x4
    fmr f15, f14
    cmpw cr5, r19, r25
    fadds f14, f14, f16
    cmpw cr6, r18, r25
    bne+ L_80387E94
    li r21, 0x0
L_80387E94:
    xoris r29, r29, 0x8000
    fmadds f9, f2, f17, f14
    bne+ cr1, L_80387EA4
    li r20, 0x0
L_80387EA4:
    stw r29, 0x54(r1)
    bne+ cr5, L_80387EB0
    li r19, 0x0
L_80387EB0:
    stfsx f9, r17, r12
    fnmsubs f14, f2, f9, f17
    addi r12, r12, 0x4
    bne+ cr6, L_80387EC4
    li r18, 0x0
L_80387EC4:
    lfsx f17, r17, r11
    cmpw cr5, r12, r24
    addi r11, r11, 0x4
    cmpw cr6, r11, r24
    bne+ cr5, L_80387EDC
    li r12, 0x0
L_80387EDC:
    bne+ cr6, L_80387EE4
    li r11, 0x0
L_80387EE4:
    fmuls f14, f14, f6
    lfd f10, 0x50(r1)
    fmadds f14, f11, f7, f14
    fmadds f9, f2, f18, f14
    fmr f7, f14
    stfsx f9, r6, r10
    fnmsubs f14, f2, f9, f18
    fmuls f8, f4, f12
    lfsx f18, r6, r9
    addi r10, r10, 0x4
    addi r9, r9, 0x4
    fmadds f14, f3, f14, f8
    cmpw cr5, r10, r23
    cmpw cr6, r9, r23
    fctiwz f14, f14
    bne+ cr5, L_80387F28
    li r10, 0x0
L_80387F28:
    bne+ cr6, L_80387F30
    li r9, 0x0
L_80387F30:
    li r31, -0x4
    fsubs f12, f10, f5
    stfiwx f14, r3, r31
    bdnz L_80387E28
    fmr f13, f12
    beq cr7, L_80387F60
    lfs f13, 0x0(r28)
    addi r28, r28, 0x4
    cmpw r28, r22
    stfs f12, -0x4(r28)
    bne+ L_80387F60
    mr r28, r27
L_80387F60:
    fmadds f8, f19, f15, f13
    fmadds f9, f20, f16, f13
    stfsx f8, r7, r21
    addi r21, r21, 0x4
    stfsx f9, r8, r19
    lfsx f14, r7, r20
    addi r20, r20, 0x4
    lfsx f16, r8, r18
    cmpw r21, r26
    cmpw cr1, r20, r26
    addi r19, r19, 0x4
    addi r18, r18, 0x4
    fmr f15, f14
    cmpw cr5, r19, r25
    fadds f14, f14, f16
    cmpw cr6, r18, r25
    bne+ L_80387FA8
    li r21, 0x0
L_80387FA8:
    fmadds f9, f2, f17, f14
    bne+ cr1, L_80387FB4
    li r20, 0x0
L_80387FB4:
    bne+ cr5, L_80387FBC
    li r19, 0x0
L_80387FBC:
    stfsx f9, r17, r12
    fnmsubs f14, f2, f9, f17
    addi r12, r12, 0x4
    bne+ cr6, L_80387FD0
    li r18, 0x0
L_80387FD0:
    lfsx f17, r17, r11
    cmpw cr5, r12, r24
    addi r11, r11, 0x4
    cmpw cr6, r11, r24
    bne+ cr5, L_80387FE8
    li r12, 0x0
L_80387FE8:
    bne+ cr6, L_80387FF0
    li r11, 0x0
L_80387FF0:
    fmuls f14, f14, f6
    fmadds f14, f11, f7, f14
    mulli r31, r5, 0x28
    fmadds f9, f2, f18, f14
    fmr f7, f14
    addi r29, r4, 0x78
    add r29, r29, r31
    stfsx f9, r6, r10
    fnmsubs f14, f2, f9, f18
    fmuls f8, f4, f12
    lfsx f18, r6, r9
    addi r10, r10, 0x4
    addi r9, r9, 0x4
    fmadds f14, f3, f14, f8
    cmpw cr5, r10, r23
    cmpw cr6, r9, r23
    fctiwz f14, f14
    bne+ cr5, L_8038803C
    li r10, 0x0
L_8038803C:
    bne+ cr6, L_80388044
    li r9, 0x0
L_80388044:
    addi r30, r4, 0x0
    add r30, r30, r31
    stfiwx f14, r0, r3
    stw r21, 0x0(r29)
    stw r20, 0x4(r29)
    stw r19, 0x14(r29)
    stw r18, 0x18(r29)
    addi r3, r3, 0x4
    stfs f15, 0x10(r29)
    stfs f16, 0x24(r29)
    slwi r31, r5, 2
    add r31, r31, r4
    addi r5, r5, 0x1
    stw r12, 0x0(r30)
    stw r11, 0x4(r30)
    stw r10, 0x14(r30)
    stw r9, 0x18(r30)
    cmpwi r5, 0x3
    stfs f17, 0x10(r30)
    stfs f18, 0x24(r30)
    stfs f7, 0x10c(r31)
    stw r28, 0x130(r31)
    bne L_80387D70
    lfd f14, 0x58(r1)
    lfd f15, 0x60(r1)
    lfd f16, 0x68(r1)
    lfd f17, 0x70(r1)
    lfd f18, 0x78(r1)
    lfd f19, 0x80(r1)
    lfd f20, 0x88(r1)
    lmw r17, 0x8(r1)
    addi r1, r1, 0x90
    blr
}

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

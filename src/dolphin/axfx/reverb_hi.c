/* Dolphin SDK AXFX: the high-quality reverb.
 *
 * Three buses (left, right, surround), each with three comb lines, two all-pass lines, a
 * low-pass all-pass line, a one-pole low-pass for damping and an optional pre-delay line; the
 * left and right inputs can be cross-fed. The crosstalk and the per-frame filter are hand-written
 * assembly in the original; everything after them in the file is compiled with the peephole pass
 * off, which the compiler does by itself after an asm function.
 *
 * Not linked yet: ReverbHICreate (0x80386990) differs in register choice only. Both keep the work
 * area pointer in r31, but retail colours the inner loop's lens pointer and delay-line pointer last
 * (r18, r17) and a line's length before its size, where this source gives the two pointers r28 and
 * r23 and colours the size first (111 rows). Every other function and all data match. The same
 * length/size swap is in ReverbSTDCreate, so a change to the shared DLcreate shape probably
 * settles both.
 *
 * Wave 41: the two asm functions here are assembly transcriptions, which CONTRIBUTING.md no longer
 * counts as source (routines that were hand-written assembly in the original stay unwritten), so
 * this unit cannot become Matching as it stands; copy propagation off for ReverbHICreate gives
 * more rows, not fewer. */
#include <dolphin/os.h>

#pragma fp_contract off

/* Placeholder symbols are kept: nothing in the binary names these functions. The names are the
 * public AXFX API they correspond to. */
#define powf fn_803B1F00
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

typedef void *(*AXFXAllocHook)(u32 size);
typedef void (*AXFXFreeHook)(void *block);
extern AXFXAllocHook __AXFXAlloc;
extern AXFXFreeHook __AXFXFree;

void *memset(void *dst, int value, unsigned long n);
f32 powf(f32 x, f32 y);

typedef struct AXFX_REVHI_DELAYLINE {
    s32 inPoint;     /* 0x00 */
    s32 outPoint;    /* 0x04 */
    s32 length;      /* 0x08 */
    f32 *inputs;     /* 0x0C */
    f32 lastOutput;  /* 0x10 */
} AXFX_REVHI_DELAYLINE;

typedef struct AXFX_REVHI_WORK {
    AXFX_REVHI_DELAYLINE AP[9]; /* 0x000: all-pass lines, three per bus */
    AXFX_REVHI_DELAYLINE C[9];  /* 0x0B4: comb lines, three per bus */
    f32 allPassCoeff;           /* 0x168 */
    f32 combCoef[9];            /* 0x16C */
    f32 lpLastout[3];           /* 0x190 */
    f32 level;                  /* 0x19C */
    f32 damping;                /* 0x1A0 */
    s32 preDelayTime;           /* 0x1A4: samples */
    f32 crosstalk;              /* 0x1A8 */
    f32 *preDelayLine[3];       /* 0x1AC */
    f32 *preDelayPtr[3];        /* 0x1B8 */
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

void ReverbHIFree(AXFX_REVHI_WORK *rv);

/* Line lengths in samples: three combs, two all-passes, then each bus's third all-pass. */
static s32 lens[8] = { 1789, 1999, 2333, 433, 149, 47, 73, 67 };

static inline void DLsetdelay(AXFX_REVHI_DELAYLINE *dl, s32 lag) {
    dl->outPoint = dl->inPoint - lag * sizeof(f32);
    while (dl->outPoint < 0) {
        dl->outPoint += dl->length;
    }
}

static inline BOOL DLcreate(AXFX_REVHI_DELAYLINE *dl, s32 length) {
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

int ReverbHICreate(AXFX_REVHI_WORK *rv, f32 coloration, f32 time, f32 mix, f32 damping,
                   f32 preDelay, f32 crosstalk) {
    u8 i;
    u8 k;

    if (coloration < 0.0f || coloration > 1.0f || time < 0.01f || time > 10.0f || mix < 0.0f ||
        mix > 1.0f || crosstalk < 0.0f || crosstalk > 1.0f || damping < 0.0f || damping > 1.0f ||
        preDelay < 0.0f || preDelay > 0.1f) {
        return FALSE;
    }

    memset(rv, 0, sizeof(AXFX_REVHI_WORK));

    for (i = 0; i < 3; i++) {
        for (k = 0; k < 3; k++) {
            if (!DLcreate(&rv->C[k + i * 3], lens[k] + 2)) {
                ReverbHIFree(rv);
                return FALSE;
            }
            DLsetdelay(&rv->C[k + i * 3], lens[k]);
            rv->combCoef[k + i * 3] = powf(10.0f, (lens[k] * -3) / (32000.0f * time));
        }
        for (k = 0; k < 2; k++) {
            if (!DLcreate(&rv->AP[k + i * 3], lens[k + 3] + 2)) {
                ReverbHIFree(rv);
                return FALSE;
            }
            DLsetdelay(&rv->AP[k + i * 3], lens[k + 3]);
        }
        if (!DLcreate(&rv->AP[i * 3 + 2], lens[i + 5] + 2)) {
            ReverbHIFree(rv);
            return FALSE;
        }
        DLsetdelay(&rv->AP[i * 3 + 2], lens[i + 5]);
        rv->lpLastout[i] = 0.0f;
    }

    rv->allPassCoeff = coloration;
    rv->level = mix;
    rv->crosstalk = crosstalk;
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
                ReverbHIFree(rv);
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

static const double i2fMagic = 4503601774854144.0;
static const f32 value1_0 = 1.0f;
static const f32 value0_3 = 0.3f;
static const f32 value0_6 = 0.6f;

/* Cross-feeds one frame of the left and right buses. Hand-written assembly in the original. */
asm void DoCrossTalk(register s32 *left, register s32 *right, register f32 cross,
                     register f32 direct) {
    nofralloc
    stwu r1, -0x30(r1)
    stfd f14, 0x28(r1)
    lis r5, i2fMagic@ha
    lfd f0, i2fMagic@l(r5)
    lis r5, 0x4330
    stw r5, 0x8(r1)
    stw r5, 0x10(r1)
    stw r5, 0x18(r1)
    stw r5, 0x20(r1)
    ps_merge00 f3, f2, f1
    ps_merge00 f4, f1, f2
    lis r5, value1_0@ha
    lfs f5, value1_0@l(r5)
    li r5, 0x4f
    mtctr r5
    li r10, -0x8
    li r11, -0x4
    ps_muls0 f4, f4, f5
    lwz r6, 0x0(r3)
    lwz r7, 0x0(r4)
    xoris r6, r6, 0x8000
    lwz r8, 0x4(r3)
    xoris r7, r7, 0x8000
    lwz r9, 0x4(r4)
    xoris r8, r8, 0x8000
    stw r6, 0xc(r1)
    xoris r9, r9, 0x8000
    stw r7, 0x14(r1)
    stw r8, 0x1c(r1)
    stw r9, 0x24(r1)
    lfd f5, 0x8(r1)
    lfd f6, 0x10(r1)
    fsubs f5, f5, f0
    lfd f7, 0x18(r1)
    fsubs f6, f6, f0
    lfd f8, 0x20(r1)
    fsubs f7, f7, f0
    fsubs f8, f8, f0
L_80386FB0:
    ps_merge00 f9, f5, f6
    lwzu r6, 0x8(r3)
    ps_merge00 f10, f7, f8
    lwzu r7, 0x8(r4)
    xoris r6, r6, 0x8000
    lwz r8, 0x4(r3)
    ps_mul f11, f9, f3
    xoris r7, r7, 0x8000
    ps_mul f12, f9, f4
    lwz r9, 0x4(r4)
    ps_mul f13, f10, f3
    xoris r8, r8, 0x8000
    ps_mul f14, f10, f4
    stw r6, 0xc(r1)
    ps_sum0 f11, f11, f11, f11
    xoris r9, r9, 0x8000
    ps_sum0 f12, f12, f12, f12
    stw r7, 0x14(r1)
    ps_sum0 f13, f13, f13, f13
    stw r8, 0x1c(r1)
    ps_sum0 f14, f14, f14, f14
    stw r9, 0x24(r1)
    fctiw f11, f11
    lfd f5, 0x8(r1)
    fctiw f12, f12
    lfd f6, 0x10(r1)
    fctiw f13, f13
    fsubs f5, f5, f0
    fctiw f14, f14
    lfd f7, 0x18(r1)
    stfiwx f11, r10, r3
    fsubs f6, f6, f0
    stfiwx f12, r10, r4
    lfd f8, 0x20(r1)
    stfiwx f13, r11, r3
    fsubs f7, f7, f0
    stfiwx f14, r11, r4
    fsubs f8, f8, f0
    bdnz L_80386FB0
    ps_merge00 f9, f5, f6
    addi r3, r3, 0x8
    ps_merge00 f10, f7, f8
    addi r4, r4, 0x8
    ps_mul f11, f9, f3
    ps_mul f12, f9, f4
    ps_mul f13, f10, f3
    ps_mul f14, f10, f4
    ps_sum0 f11, f11, f11, f11
    ps_sum0 f12, f12, f12, f12
    ps_sum0 f13, f13, f13, f13
    ps_sum0 f14, f14, f14, f14
    fctiw f11, f11
    fctiw f12, f12
    fctiw f13, f13
    fctiw f14, f14
    stfiwx f11, r10, r3
    stfiwx f12, r10, r4
    stfiwx f13, r11, r3
    stfiwx f14, r11, r4
    lfd f14, 0x28(r1)
    addi r1, r1, 0x30
    blr
}

/* One frame of one bus, in place. Hand-written assembly in the original. */
asm void HandleReverb(register s32 *samples, register AXFX_REVHI_WORK *rv, register s32 bus) {
    nofralloc
    stwu r1, -0xc0(r1)
    stmw r14, 0x8(r1)
    stfd f14, 0x60(r1)
    stfd f15, 0x68(r1)
    stfd f16, 0x70(r1)
    stfd f17, 0x78(r1)
    stfd f18, 0x80(r1)
    stfd f19, 0x88(r1)
    stfd f20, 0x90(r1)
    stfd f21, 0x98(r1)
    stfd f22, 0xa0(r1)
    stfd f23, 0xa8(r1)
    stfd f24, 0xb0(r1)
    stfd f25, 0xb8(r1)
    stw r5, 0x50(r1)
    stw r4, 0x54(r1)
    lis r31, value0_3@ha
    lfs f6, value0_3@l(r31)
    lis r31, value0_6@ha
    lfs f9, value0_6@l(r31)
    lis r31, i2fMagic@ha
    lfd f5, i2fMagic@l(r31)
    lfs f2, 0x168(r4)
    lfs f15, 0x1a0(r4)
    lfs f8, 0x19c(r4)
    fmuls f3, f8, f9
    fsubs f4, f9, f3
    slwi r30, r5, 1
    add r30, r30, r5
    mulli r31, r30, 0x14
    addi r29, r4, 0xb4
    add r29, r29, r31
    addi r27, r4, 0x0
    add r27, r27, r31
    slwi r31, r30, 2
    add r31, r31, r4
    lfs f22, 0x16c(r31)
    lfs f23, 0x170(r31)
    lfs f24, 0x174(r31)
    slwi r31, r5, 2
    add r31, r31, r4
    lfs f25, 0x190(r31)
    lwz r31, 0x1a4(r4)
    lis r30, 0x4330
    stw r30, 0x58(r1)
    subi r22, r31, 0x1
    slwi r22, r22, 2
    slwi r28, r5, 2
    add r28, r28, r4
    cmpwi cr7, r31, 0x0
    lwz r21, 0x0(r29)
    lwz r20, 0x4(r29)
    lwz r19, 0x14(r29)
    lwz r18, 0x18(r29)
    lwz r17, 0x28(r29)
    lwz r16, 0x2c(r29)
    lfs f16, 0x10(r29)
    lfs f17, 0x24(r29)
    lfs f18, 0x38(r29)
    lwz r25, 0x8(r29)
    lwz r24, 0x1c(r29)
    lwz r23, 0x30(r29)
    lwz r4, 0xc(r29)
    lwz r5, 0x20(r29)
    lwz r6, 0x34(r29)
    lwz r12, 0x0(r27)
    lwz r11, 0x4(r27)
    lwz r10, 0x14(r27)
    lwz r9, 0x18(r27)
    lwz r8, 0x28(r27)
    lwz r7, 0x2c(r27)
    lfs f19, 0x10(r27)
    lfs f20, 0x24(r27)
    lfs f21, 0x38(r27)
    lwz r15, 0x8(r27)
    lwz r14, 0x1c(r27)
    lwz r30, 0x0(r3)
    xoris r30, r30, 0x8000
    stw r30, 0x5c(r1)
    lfd f12, 0x58(r1)
    fsubs f12, f12, f5
    li r31, 0x9f
    mtctr r31
L_803871F4:
    fmr f13, f12
    beq cr7, L_80387224
    lwz r30, 0x1ac(r28)
    lwz r29, 0x1b8(r28)
    add r31, r22, r30
    addi r29, r29, 0x4
    lfs f13, -0x4(r29)
    cmpw r29, r31
    stfs f12, -0x4(r29)
    bne+ L_80387220
    mr r29, r30
L_80387220:
    stw r29, 0x1b8(r30)
L_80387224:
    fmadds f8, f22, f16, f13
    lwzu r29, 0x4(r3)
    fmadds f9, f23, f17, f13
    stfsx f8, r4, r21
    addi r21, r21, 0x4
    stfsx f9, r5, r19
    lfsx f14, r4, r20
    addi r20, r20, 0x4
    lfsx f17, r5, r18
    cmpw r21, r25
    cmpw cr1, r20, r25
    addi r19, r19, 0x4
    addi r18, r18, 0x4
    fmr f16, f14
    cmpw cr5, r19, r24
    fadds f14, f14, f17
    cmpw cr6, r18, r24
    bne+ L_80387270
    li r21, 0x0
L_80387270:
    fmadds f8, f24, f18, f13
    bne+ cr1, L_8038727C
    li r20, 0x0
L_8038727C:
    stfsx f8, r6, r17
    addi r17, r17, 0x4
    bne+ cr5, L_8038728C
    li r19, 0x0
L_8038728C:
    lfsx f18, r6, r16
    addi r16, r16, 0x4
    cmpw r17, r23
    bne+ cr6, L_803872A0
    li r18, 0x0
L_803872A0:
    fadds f14, f14, f18
    cmpw cr1, r16, r23
    lwz r26, 0xc(r27)
    fmadds f9, f2, f19, f14
    bne+ L_803872B8
    li r17, 0x0
L_803872B8:
    bne+ cr1, L_803872C0
    li r16, 0x0
L_803872C0:
    xoris r29, r29, 0x8000
    stfsx f9, r26, r12
    fnmsubs f14, f2, f9, f19
    addi r12, r12, 0x4
    lfsx f19, r26, r11
    cmpw cr5, r12, r15
    addi r11, r11, 0x4
    lwz r26, 0x20(r27)
    cmpw cr6, r11, r15
    fmadds f8, f2, f20, f14
    bne+ cr5, L_803872F0
    li r12, 0x0
L_803872F0:
    stw r29, 0x5c(r1)
    stfsx f8, r26, r10
    fnmsubs f14, f2, f8, f20
    addi r10, r10, 0x4
    bne+ cr6, L_80387308
    li r11, 0x0
L_80387308:
    lfsx f20, r26, r9
    cmpw r10, r14
    fmuls f14, f14, f6
    addi r9, r9, 0x4
    cmpw cr1, r9, r14
    lfd f10, 0x58(r1)
    fmadds f14, f15, f25, f14
    bne+ L_8038732C
    li r10, 0x0
L_8038732C:
    lwz r26, 0x34(r27)
    fmadds f9, f2, f21, f14
    fmr f25, f14
    bne+ cr1, L_80387340
    li r9, 0x0
L_80387340:
    stfsx f9, r26, r8
    fnmsubs f14, f2, f9, f21
    lwz r31, 0x30(r27)
    fmuls f8, f4, f12
    lfsx f21, r26, r7
    addi r8, r8, 0x4
    addi r7, r7, 0x4
    fmadds f14, f3, f14, f8
    cmpw cr5, r8, r31
    cmpw cr6, r7, r31
    fctiwz f14, f14
    bne+ cr5, L_80387374
    li r8, 0x0
L_80387374:
    bne+ cr6, L_8038737C
    li r7, 0x0
L_8038737C:
    li r31, -0x4
    fsubs f12, f10, f5
    stfiwx f14, r3, r31
    bdnz L_803871F4
    fmr f13, f12
    beq cr7, L_803873BC
    lwz r30, 0x1ac(r28)
    lwz r29, 0x1b8(r28)
    add r31, r22, r30
    addi r29, r29, 0x4
    lfs f13, -0x4(r29)
    cmpw r29, r31
    stfs f12, -0x4(r29)
    bne+ L_803873B8
    mr r29, r30
L_803873B8:
    stw r29, 0x1b8(r30)
L_803873BC:
    fmadds f8, f22, f16, f13
    fmadds f9, f23, f17, f13
    stfsx f8, r4, r21
    addi r21, r21, 0x4
    stfsx f9, r5, r19
    lfsx f14, r4, r20
    addi r20, r20, 0x4
    lfsx f17, r5, r18
    cmpw r21, r25
    cmpw cr1, r20, r25
    addi r19, r19, 0x4
    addi r18, r18, 0x4
    fmr f16, f14
    cmpw cr5, r19, r24
    fadds f14, f14, f17
    cmpw cr6, r18, r24
    bne+ L_80387404
    li r21, 0x0
L_80387404:
    fmadds f8, f24, f18, f13
    bne+ cr1, L_80387410
    li r20, 0x0
L_80387410:
    stfsx f8, r6, r17
    addi r17, r17, 0x4
    bne+ cr5, L_80387420
    li r19, 0x0
L_80387420:
    lfsx f18, r6, r16
    addi r16, r16, 0x4
    cmpw r17, r23
    bne+ cr6, L_80387434
    li r18, 0x0
L_80387434:
    fadds f14, f14, f18
    cmpw cr1, r16, r23
    lwz r26, 0xc(r27)
    fmadds f9, f2, f19, f14
    bne+ L_8038744C
    li r17, 0x0
L_8038744C:
    bne+ cr1, L_80387454
    li r16, 0x0
L_80387454:
    stfsx f9, r26, r12
    fnmsubs f14, f2, f9, f19
    addi r12, r12, 0x4
    lfsx f19, r26, r11
    cmpw cr5, r12, r15
    addi r11, r11, 0x4
    lwz r26, 0x20(r27)
    cmpw cr6, r11, r15
    fmadds f8, f2, f20, f14
    bne+ cr5, L_80387480
    li r12, 0x0
L_80387480:
    stfsx f8, r26, r10
    fnmsubs f14, f2, f8, f20
    addi r10, r10, 0x4
    bne+ cr6, L_80387494
    li r11, 0x0
L_80387494:
    lfsx f20, r26, r9
    cmpw r10, r14
    fmuls f14, f14, f6
    addi r9, r9, 0x4
    cmpw cr1, r9, r14
    fmadds f14, f15, f25, f14
    bne+ L_803874B4
    li r10, 0x0
L_803874B4:
    lwz r26, 0x34(r27)
    lwz r5, 0x50(r1)
    lwz r4, 0x54(r1)
    fmadds f9, f2, f21, f14
    fmr f25, f14
    bne+ cr1, L_803874D0
    li r9, 0x0
L_803874D0:
    stfsx f9, r26, r8
    fnmsubs f14, f2, f9, f21
    lwz r29, 0x30(r27)
    fmuls f8, f4, f12
    lfsx f21, r26, r7
    addi r8, r8, 0x4
    addi r7, r7, 0x4
    fmadds f14, f3, f14, f8
    cmpw cr5, r8, r29
    cmpw cr6, r7, r29
    fctiwz f14, f14
    bne+ cr5, L_80387504
    li r8, 0x0
L_80387504:
    bne+ cr6, L_8038750C
    li r7, 0x0
L_8038750C:
    slwi r30, r5, 1
    add r30, r30, r5
    mulli r31, r30, 0x14
    stfiwx f14, r0, r3
    addi r29, r4, 0xb4
    add r29, r29, r31
    stw r21, 0x0(r29)
    stw r20, 0x4(r29)
    stw r19, 0x14(r29)
    stw r18, 0x18(r29)
    stw r17, 0x28(r29)
    stw r16, 0x2c(r29)
    stfs f16, 0x10(r29)
    stfs f17, 0x24(r29)
    stfs f18, 0x38(r29)
    stw r12, 0x0(r27)
    stw r11, 0x4(r27)
    stw r10, 0x14(r27)
    stw r9, 0x18(r27)
    stw r8, 0x28(r27)
    stw r7, 0x2c(r27)
    stfs f19, 0x10(r27)
    stfs f20, 0x24(r27)
    stfs f21, 0x38(r27)
    slwi r31, r5, 2
    add r31, r31, r4
    stfs f25, 0x190(r31)
    lfd f14, 0x60(r1)
    lfd f15, 0x68(r1)
    lfd f16, 0x70(r1)
    lfd f17, 0x78(r1)
    lfd f18, 0x80(r1)
    lfd f19, 0x88(r1)
    lfd f20, 0x90(r1)
    lfd f21, 0x98(r1)
    lfd f22, 0xa0(r1)
    lfd f23, 0xa8(r1)
    lfd f24, 0xb0(r1)
    lfd f25, 0xb8(r1)
    lmw r14, 0x8(r1)
    addi r1, r1, 0xc0
    blr
}

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

/* Dolphin SDK AXFX: the chorus effect. Each bus keeps its last three frames; the output is read
 * back from them at a position that sweeps around a base delay, resampled with a 4-tap filter.
 *
 * The two resamplers are
 * hand-written assembly in the original; everything after them in the file is compiled with the
 * peephole pass off, which the compiler does by itself after an asm function. */
#include <dolphin/os.h>

/* Placeholder symbols are kept: nothing in the binary names these functions. The names are the
 * public AXFX API they correspond to. */
#define do_src1 fn_803882D4
#define do_src2 fn_8038846C
#define AXFXChorusInit fn_80388658
#define AXFXChorusShutdown fn_803888A0
#define AXFXChorusSettings fn_803888F0
#define AXFXChorusCallback fn_80388A00

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

typedef struct AXFX_CHORUS_SRCINFO {
    s32 *dest;    /* 0x00 */
    s32 *smpBase; /* 0x04 */
    s32 *old;     /* 0x08 */
    u32 posLo;    /* 0x0C */
    u32 posHi;    /* 0x10 */
    u32 pitchLo;  /* 0x14 */
    u32 pitchHi;  /* 0x18 */
    u32 trigger;  /* 0x1C */
    u32 target;   /* 0x20 */
} AXFX_CHORUS_SRCINFO;

typedef struct AXFX_CHORUS_WORK {
    s32 *lastLeft[3];           /* 0x00 */
    s32 *lastRight[3];          /* 0x0C */
    s32 *lastSur[3];            /* 0x18 */
    u8 currentLast;             /* 0x24 */
    s32 oldLeft[4];             /* 0x28 */
    s32 oldRight[4];            /* 0x38 */
    s32 oldSur[4];              /* 0x48 */
    u32 currentPosLo;           /* 0x58 */
    u32 currentPosHi;           /* 0x5C */
    s32 pitchOffset;            /* 0x60 */
    u32 pitchOffsetPeriodCount; /* 0x64 */
    u32 pitchOffsetPeriod;      /* 0x68 */
    AXFX_CHORUS_SRCINFO src;    /* 0x6C */
} AXFX_CHORUS_WORK;

typedef struct AXFX_CHORUS {
    AXFX_CHORUS_WORK work; /* 0x00 */
    /* Set by the caller before Init or Settings. */
    u32 baseDelay;         /* 0x90: milliseconds, 5..15 */
    u32 variation;         /* 0x94: milliseconds, 0..5 */
    u32 period;            /* 0x98: milliseconds, 500..10000 */
} AXFX_CHORUS;

int AXFXChorusSettings(AXFX_CHORUS *chorus);

/* The resampling filter: 128 phases of four coefficients (floats, kept as their bit patterns). */
static u32 rsmpTab12khz[512] = {
    0x3DC7B000, 0x3F4D5E00, 0x3DD01000, 0xBA800000, 0x3DBF8000, 0x3F4D5200, 0x3DD8B000, 0xBA880000,
    0x3DB78000, 0x3F4D3C00, 0x3DE18000, 0xBA980000, 0x3DAFB000, 0x3F4D1A00, 0x3DEA7000, 0xBAA80000,
    0x3DA80000, 0x3F4CEC00, 0x3DF39000, 0xBAB80000, 0x3DA08000, 0x3F4CB400, 0x3DFCE000, 0xBAC80000,
    0x3D993000, 0x3F4C7200, 0x3E033800, 0xBAD80000, 0x3D922000, 0x3F4C2400, 0x3E081000, 0xBAE80000,
    0x3D8B3000, 0x3F4BCA00, 0x3E0D0000, 0xBAFC0000, 0x3D848000, 0x3F4B6600, 0x3E120800, 0xBB060000,
    0x3D7BE000, 0x3F4AF800, 0x3E172800, 0xBB100000, 0x3D6F0000, 0x3F4A7E00, 0x3E1C6000, 0xBB180000,
    0x3D62A000, 0x3F49F800, 0x3E21B000, 0xBB220000, 0x3D568000, 0x3F496800, 0x3E271800, 0xBB2E0000,
    0x3D4AC000, 0x3F48D000, 0x3E2C9000, 0xBB380000, 0x3D3F6000, 0x3F482A00, 0x3E322800, 0xBB440000,
    0x3D344000, 0x3F477A00, 0x3E37D000, 0xBB4E0000, 0x3D29A000, 0x3F46C000, 0x3E3D9000, 0xBB5A0000,
    0x3D1F4000, 0x3F45FC00, 0x3E436800, 0xBB660000, 0x3D152000, 0x3F452E00, 0x3E495000, 0xBB720000,
    0x3D0B6000, 0x3F445600, 0x3E4F5800, 0xBB7E0000, 0x3D020000, 0x3F437400, 0x3E557000, 0xBB850000,
    0x3CF1C000, 0x3F428600, 0x3E5B9800, 0xBB8C0000, 0x3CE04000, 0x3F419000, 0x3E61D800, 0xBB920000,
    0x3CCF4000, 0x3F409000, 0x3E683000, 0xBB990000, 0x3CBF0000, 0x3F3F8800, 0x3E6E9800, 0xBB9F0000,
    0x3CAF4000, 0x3F3E7600, 0x3E751000, 0xBBA60000, 0x3CA04000, 0x3F3D5A00, 0x3E7BA000, 0xBBAE0000,
    0x3C91C000, 0x3F3C3600, 0x3E812000, 0xBBB50000, 0x3C838000, 0x3F3B0800, 0x3E847800, 0xBBBB0000,
    0x3C6C0000, 0x3F39D200, 0x3E87DC00, 0xBBC20000, 0x3C520000, 0x3F389400, 0x3E8B4800, 0xBBCA0000,
    0x3C398000, 0x3F374C00, 0x3E8EBC00, 0xBBD10000, 0x3C220000, 0x3F35FE00, 0x3E923800, 0xBBD70000,
    0x3C0B8000, 0x3F34A600, 0x3E95BC00, 0xBBDE0000, 0x3BEC0000, 0x3F334800, 0x3E994400, 0xBBE60000,
    0x3BC20000, 0x3F31E200, 0x3E9CD800, 0xBBED0000, 0x3B9A0000, 0x3F307400, 0x3EA07400, 0xBBF30000,
    0x3B6A0000, 0x3F2EFE00, 0x3EA41400, 0xBBFA0000, 0x3B220000, 0x3F2D8000, 0x3EA7BC00, 0xBC010000,
    0x3ABC0000, 0x3F2BFE00, 0x3EAB6800, 0xBC048000, 0x39F00000, 0x3F2A7400, 0x3EAF1800, 0xBC078000,
    0xBA000000, 0x3F28E400, 0x3EB2D000, 0xBC0A8000, 0xBAB00000, 0x3F274E00, 0x3EB69000, 0xBC0D8000,
    0xBB0E0000, 0x3F25B000, 0x3EBA5000, 0xBC110000, 0xBB420000, 0x3F240E00, 0x3EBE1800, 0xBC140000,
    0xBB720000, 0x3F226400, 0x3EC1E400, 0xBC170000, 0xBB8F0000, 0x3F20B600, 0x3EC5B400, 0xBC190000,
    0xBBA30000, 0x3F1F0400, 0x3EC98800, 0xBC1B8000, 0xBBB70000, 0x3F1D4C00, 0x3ECD5C00, 0xBC1E0000,
    0xBBC90000, 0x3F1B8E00, 0x3ED13400, 0xBC208000, 0xBBDA0000, 0x3F19CC00, 0x3ED50C00, 0xBC228000,
    0xBBEA0000, 0x3F180600, 0x3ED8E800, 0xBC240000, 0xBBF80000, 0x3F163C00, 0x3EDCC800, 0xBC260000,
    0xBC028000, 0x3F146C00, 0x3EE0A400, 0xBC278000, 0xBC088000, 0x3F129A00, 0x3EE48400, 0xBC288000,
    0xBC0E0000, 0x3F10C600, 0x3EE86400, 0xBC298000, 0xBC130000, 0x3F0EEE00, 0x3EEC4400, 0xBC2A8000,
    0xBC170000, 0x3F0D1200, 0x3EF02400, 0xBC2A8000, 0xBC1B0000, 0x3F0B3200, 0x3EF40400, 0xBC2A8000,
    0xBC1E8000, 0x3F095200, 0x3EF7E000, 0xBC2A0000, 0xBC218000, 0x3F076E00, 0x3EFBBC00, 0xBC2A0000,
    0xBC240000, 0x3F058800, 0x3EFF9400, 0xBC290000, 0xBC260000, 0x3F03A000, 0x3F01B600, 0xBC280000,
    0xBC280000, 0x3F01B600, 0x3F03A000, 0xBC260000, 0xBC290000, 0x3EFF9400, 0x3F058800, 0xBC240000,
    0xBC2A0000, 0x3EFBBC00, 0x3F076E00, 0xBC218000, 0xBC2A0000, 0x3EF7E000, 0x3F095200, 0xBC1E8000,
    0xBC2A8000, 0x3EF40400, 0x3F0B3200, 0xBC1B0000, 0xBC2A8000, 0x3EF02400, 0x3F0D1200, 0xBC170000,
    0xBC2A8000, 0x3EEC4400, 0x3F0EEE00, 0xBC130000, 0xBC298000, 0x3EE86400, 0x3F10C600, 0xBC0E0000,
    0xBC288000, 0x3EE48400, 0x3F129A00, 0xBC088000, 0xBC278000, 0x3EE0A400, 0x3F146C00, 0xBC028000,
    0xBC260000, 0x3EDCC800, 0x3F163C00, 0xBBF80000, 0xBC240000, 0x3ED8E800, 0x3F180600, 0xBBEA0000,
    0xBC228000, 0x3ED50C00, 0x3F19CC00, 0xBBDA0000, 0xBC208000, 0x3ED13400, 0x3F1B8E00, 0xBBC90000,
    0xBC1E0000, 0x3ECD5C00, 0x3F1D4C00, 0xBBB70000, 0xBC1B8000, 0x3EC98800, 0x3F1F0400, 0xBBA30000,
    0xBC190000, 0x3EC5B400, 0x3F20B600, 0xBB8F0000, 0xBC170000, 0x3EC1E400, 0x3F226400, 0xBB720000,
    0xBC140000, 0x3EBE1800, 0x3F240E00, 0xBB420000, 0xBC110000, 0x3EBA5000, 0x3F25B000, 0xBB0E0000,
    0xBC0D8000, 0x3EB69000, 0x3F274E00, 0xBAB00000, 0xBC0A8000, 0x3EB2D000, 0x3F28E400, 0xBA000000,
    0xBC078000, 0x3EAF1800, 0x3F2A7400, 0x39F00000, 0xBC048000, 0x3EAB6800, 0x3F2BFE00, 0x3ABC0000,
    0xBC010000, 0x3EA7BC00, 0x3F2D8000, 0x3B220000, 0xBBFA0000, 0x3EA41400, 0x3F2EFE00, 0x3B6A0000,
    0xBBF30000, 0x3EA07400, 0x3F307400, 0x3B9A0000, 0xBBED0000, 0x3E9CD800, 0x3F31E200, 0x3BC20000,
    0xBBE60000, 0x3E994400, 0x3F334800, 0x3BEC0000, 0xBBDE0000, 0x3E95BC00, 0x3F34A600, 0x3C0B8000,
    0xBBD70000, 0x3E923800, 0x3F35FE00, 0x3C220000, 0xBBD10000, 0x3E8EBC00, 0x3F374C00, 0x3C398000,
    0xBBCA0000, 0x3E8B4800, 0x3F389400, 0x3C520000, 0xBBC20000, 0x3E87DC00, 0x3F39D200, 0x3C6C0000,
    0xBBBB0000, 0x3E847800, 0x3F3B0800, 0x3C838000, 0xBBB50000, 0x3E812000, 0x3F3C3600, 0x3C91C000,
    0xBBAE0000, 0x3E7BA000, 0x3F3D5A00, 0x3CA04000, 0xBBA60000, 0x3E751000, 0x3F3E7600, 0x3CAF4000,
    0xBB9F0000, 0x3E6E9800, 0x3F3F8800, 0x3CBF0000, 0xBB990000, 0x3E683000, 0x3F409000, 0x3CCF4000,
    0xBB920000, 0x3E61D800, 0x3F419000, 0x3CE04000, 0xBB8C0000, 0x3E5B9800, 0x3F428600, 0x3CF1C000,
    0xBB850000, 0x3E557000, 0x3F437400, 0x3D020000, 0xBB7E0000, 0x3E4F5800, 0x3F445600, 0x3D0B6000,
    0xBB720000, 0x3E495000, 0x3F452E00, 0x3D152000, 0xBB660000, 0x3E436800, 0x3F45FC00, 0x3D1F4000,
    0xBB5A0000, 0x3E3D9000, 0x3F46C000, 0x3D29A000, 0xBB4E0000, 0x3E37D000, 0x3F477A00, 0x3D344000,
    0xBB440000, 0x3E322800, 0x3F482A00, 0x3D3F6000, 0xBB380000, 0x3E2C9000, 0x3F48D000, 0x3D4AC000,
    0xBB2E0000, 0x3E271800, 0x3F496800, 0x3D568000, 0xBB220000, 0x3E21B000, 0x3F49F800, 0x3D62A000,
    0xBB180000, 0x3E1C6000, 0x3F4A7E00, 0x3D6F0000, 0xBB100000, 0x3E172800, 0x3F4AF800, 0x3D7BE000,
    0xBB060000, 0x3E120800, 0x3F4B6600, 0x3D848000, 0xBAFC0000, 0x3E0D0000, 0x3F4BCA00, 0x3D8B3000,
    0xBAE80000, 0x3E081000, 0x3F4C2400, 0x3D922000, 0xBAD80000, 0x3E033800, 0x3F4C7200, 0x3D993000,
    0xBAC80000, 0x3DFCE000, 0x3F4CB400, 0x3DA08000, 0xBAB80000, 0x3DF39000, 0x3F4CEC00, 0x3DA80000,
    0xBAA80000, 0x3DEA7000, 0x3F4D1A00, 0x3DAFB000, 0xBA980000, 0x3DE18000, 0x3F4D3C00, 0x3DB78000,
    0xBA880000, 0x3DD8B000, 0x3F4D5200, 0x3DBF8000, 0xBA800000, 0x3DD01000, 0x3F4D5E00, 0x3DC7B000,
};

static const double i2fMagic = 4503601774854144.0;

/* Resamplers, hand-written assembly in the original. */
asm void do_src1(register AXFX_CHORUS_SRCINFO *src) {
    nofralloc
    stwu r1, -0x40(r1)
    stmw r26, 0x28(r1)
    lwz r4, 0xc(r3)
    lwz r5, 0x10(r3)
    lwz r6, 0x14(r3)
    lwz r8, 0x1c(r3)
    lwz r7, 0x20(r3)
    lwz r31, 0x4(r3)
    lwz r30, 0x0(r3)
    lwz r9, 0x8(r3)
    lis r10, 0x4330
    stw r10, 0x8(r1)
    stw r10, 0x10(r1)
    stw r10, 0x18(r1)
    stw r10, 0x20(r1)
    lis r10, i2fMagic@ha
    lfd f9, i2fMagic@l(r10)
    slwi r10, r5, 2
    lwz r11, 0x0(r9)
    lwz r29, 0x4(r9)
    lwz r28, 0x8(r9)
    lwzx r27, r31, r10
    xoris r11, r11, 0x8000
    xoris r29, r29, 0x8000
    stw r11, 0xc(r1)
    xoris r28, r28, 0x8000
    stw r29, 0x14(r1)
    xoris r27, r27, 0x8000
    stw r28, 0x1c(r1)
    lfd f1, 0x8(r1)
    stw r27, 0x24(r1)
    lfd f2, 0x10(r1)
    fsubs f1, f1, f9
    lfd f3, 0x18(r1)
    fsubs f2, f2, f9
    lfd f4, 0x20(r1)
    fsubs f3, f3, f9
    fsubs f4, f4, f9
    li r26, -0x4
    lis r12, rsmpTab12khz@ha
    addi r12, r12, rsmpTab12khz@l
    li r9, 0xa0
    mtctr r9
L_80388380:
    rlwinm r10, r4, 7, 21, 27
    addc r4, r4, r6
    add r10, r10, r12
    mcrxr cr0
    lfs f5, 0x0(r10)
    beq L_803883C8
    lfs f6, 0x4(r10)
    fmuls f10, f1, f5
    lfs f7, 0x8(r10)
    fmadds f10, f2, f6, f10
    lfs f8, 0xc(r10)
    fmadds f10, f3, f7, f10
    addi r30, r30, 0x4
    fmadds f10, f4, f8, f10
    fctiwz f10, f10
    stfiwx f10, r26, r30
    bdnz L_80388380
    b L_80388434
L_803883C8:
    addi r5, r5, 0x1
    lfs f6, 0x4(r10)
    fmuls f10, f1, f5
    cmpw r5, r8
    fmr f1, f2
    lfs f7, 0x8(r10)
    fmadds f10, f2, f6, f10
    fmr f2, f3
    lfs f8, 0xc(r10)
    fmadds f10, f3, f7, f10
    addi r30, r30, 0x4
    fmr f3, f4
    bne+ L_80388400
    mr r5, r7
L_80388400:
    fmadds f10, f4, f8, f10
    slwi r9, r5, 2
    bdz L_8038842C
    lwzx r10, r9, r31
    fctiwz f10, f10
    xoris r10, r10, 0x8000
    stw r10, 0xc(r1)
    stfiwx f10, r26, r30
    lfd f4, 0x8(r1)
    fsubs f4, f4, f9
    b L_80388380
L_8038842C:
    fctiwz f10, f10
    stfiwx f10, r26, r30
L_80388434:
    lwz r9, 0x8(r3)
    fctiwz f1, f1
    fctiwz f2, f2
    fctiwz f3, f3
    stfiwx f1, r0, r9
    addi r10, r9, 0x4
    stfiwx f2, r0, r10
    addi r10, r9, 0x8
    stfiwx f3, r0, r10
    stw r4, 0xc(r3)
    stw r5, 0x10(r3)
    lmw r26, 0x28(r1)
    addi r1, r1, 0x40
    blr
}

asm void do_src2(register AXFX_CHORUS_SRCINFO *src) {
    nofralloc
    stwu r1, -0x40(r1)
    stmw r26, 0x28(r1)
    lwz r4, 0xc(r3)
    lwz r5, 0x10(r3)
    lwz r6, 0x14(r3)
    lwz r8, 0x1c(r3)
    lwz r7, 0x20(r3)
    lwz r31, 0x4(r3)
    lwz r30, 0x0(r3)
    lwz r9, 0x8(r3)
    lis r10, 0x4330
    stw r10, 0x8(r1)
    stw r10, 0x10(r1)
    stw r10, 0x18(r1)
    stw r10, 0x20(r1)
    lis r10, i2fMagic@ha
    lfd f9, i2fMagic@l(r10)
    slwi r10, r5, 2
    lwz r11, 0x0(r9)
    lwz r29, 0x4(r9)
    lwz r28, 0x8(r9)
    lwzx r27, r31, r10
    xoris r11, r11, 0x8000
    xoris r29, r29, 0x8000
    stw r11, 0xc(r1)
    xoris r28, r28, 0x8000
    stw r29, 0x14(r1)
    xoris r27, r27, 0x8000
    stw r28, 0x1c(r1)
    lfd f1, 0x8(r1)
    stw r27, 0x24(r1)
    lfd f2, 0x10(r1)
    fsubs f1, f1, f9
    lfd f3, 0x18(r1)
    fsubs f2, f2, f9
    lfd f4, 0x20(r1)
    fsubs f3, f3, f9
    fsubs f4, f4, f9
    li r26, -0x4
    lis r12, rsmpTab12khz@ha
    addi r12, r12, rsmpTab12khz@l
    li r9, 0xa0
    mtctr r9
L_80388518:
    rlwinm r10, r4, 7, 21, 27
    addc r4, r4, r6
    add r10, r10, r12
    mcrxr cr0
    addi r5, r5, 0x1
    lfs f5, 0x0(r10)
    beq L_80388594
    lfs f6, 0x4(r10)
    fmuls f10, f1, f5
    cmpw r5, r8
    fmr f1, f2
    lfs f7, 0x8(r10)
    fmadds f10, f2, f6, f10
    fmr f2, f3
    lfs f8, 0xc(r10)
    fmadds f10, f3, f7, f10
    addi r30, r30, 0x4
    fmr f3, f4
    bne+ L_80388568
    mr r5, r7
L_80388568:
    fmadds f10, f4, f8, f10
    slwi r9, r5, 2
    bdz L_80388618
    lwzx r10, r9, r31
    fctiwz f10, f10
    xoris r10, r10, 0x8000
    stw r10, 0xc(r1)
    stfiwx f10, r26, r30
    lfd f4, 0x8(r1)
    fsubs f4, f4, f9
    b L_80388518
L_80388594:
    cmpw r5, r8
    lfs f6, 0x4(r10)
    bne+ L_803885A4
    mr r5, r7
L_803885A4:
    slwi r11, r5, 2
    addi r5, r5, 0x1
    lwzx r29, r11, r31
    fmuls f10, f1, f5
    cmpw r5, r8
    xoris r29, r29, 0x8000
    fmr f1, f3
    lfs f7, 0x8(r10)
    stw r29, 0xc(r1)
    fmadds f10, f2, f6, f10
    lfs f8, 0xc(r10)
    fmadds f10, f3, f7, f10
    lfd f3, 0x8(r1)
    fmr f2, f4
    addi r30, r30, 0x4
    fsubs f3, f3, f9
    bne+ L_803885EC
    mr r5, r7
L_803885EC:
    fmadds f10, f4, f8, f10
    slwi r9, r5, 2
    bdz L_80388618
    lwzx r10, r9, r31
    fctiwz f10, f10
    xoris r10, r10, 0x8000
    stw r10, 0xc(r1)
    stfiwx f10, r26, r30
    lfd f4, 0x8(r1)
    fsubs f4, f4, f9
    b L_80388518
L_80388618:
    fctiwz f10, f10
    stfiwx f10, r26, r30
    lwz r9, 0x8(r3)
    fctiwz f1, f1
    fctiwz f2, f2
    fctiwz f3, f3
    stfiwx f1, r0, r9
    addi r10, r9, 0x4
    stfiwx f2, r0, r10
    addi r10, r9, 0x8
    stfiwx f3, r0, r10
    stw r4, 0xc(r3)
    stw r5, 0x10(r3)
    lmw r26, 0x28(r1)
    addi r1, r1, 0x40
    blr
}

int AXFXChorusInit(AXFX_CHORUS *chorus) {
    BOOL enabled;
    s32 *left;
    s32 *right;
    s32 *sur;
    u32 i;

    if (chorus->baseDelay < 5 || chorus->baseDelay > 15 || chorus->variation < 0 ||
        chorus->variation > 5 || chorus->period < 500 || chorus->period > 10000) {
        return FALSE;
    }

    enabled = OSDisableInterrupts();

    chorus->work.lastLeft[0] = __AXFXAlloc(3 * 3 * AXFX_FRAME_SAMPLES * sizeof(s32));
    if (chorus->work.lastLeft[0] != NULL) {
        chorus->work.lastRight[0] = chorus->work.lastLeft[0] + 3 * AXFX_FRAME_SAMPLES;
        chorus->work.lastSur[0] = chorus->work.lastRight[0] + 3 * AXFX_FRAME_SAMPLES;
        for (i = 1; i < 3; i++) {
            chorus->work.lastLeft[i] = chorus->work.lastLeft[0] + i * AXFX_FRAME_SAMPLES;
            chorus->work.lastRight[i] = chorus->work.lastRight[0] + i * AXFX_FRAME_SAMPLES;
            chorus->work.lastSur[i] = chorus->work.lastSur[0] + i * AXFX_FRAME_SAMPLES;
        }

        left = chorus->work.lastLeft[0];
        right = chorus->work.lastRight[0];
        sur = chorus->work.lastSur[0];
        for (i = 0; i < 2 * AXFX_FRAME_SAMPLES; i++) {
            *left++ = 0;
            *right++ = 0;
            *sur++ = 0;
        }

        chorus->work.currentLast = 1;
        chorus->work.oldLeft[0] = chorus->work.oldLeft[1] = chorus->work.oldLeft[2] =
            chorus->work.oldLeft[3] = 0;
        chorus->work.oldRight[0] = chorus->work.oldRight[1] = chorus->work.oldRight[2] =
            chorus->work.oldRight[3] = 0;
        chorus->work.oldSur[0] = chorus->work.oldSur[1] = chorus->work.oldSur[2] =
            chorus->work.oldSur[3] = 0;
        chorus->work.src.trigger = 3 * AXFX_FRAME_SAMPLES;
        chorus->work.src.target = 0;

        OSRestoreInterrupts(enabled);
        return AXFXChorusSettings(chorus);
    }

    OSRestoreInterrupts(enabled);
    return FALSE;
}

int AXFXChorusShutdown(AXFX_CHORUS *chorus) {
    BOOL enabled = OSDisableInterrupts();

    __AXFXFree(chorus->work.lastLeft[0]);
    OSRestoreInterrupts(enabled);
    return TRUE;
}

int AXFXChorusSettings(AXFX_CHORUS *chorus) {
    BOOL enabled;

    if (chorus->baseDelay < 5 || chorus->baseDelay > 15 || chorus->variation < 0 ||
        chorus->variation > 5 || chorus->period < 500 || chorus->period > 10000) {
        return FALSE;
    }

    enabled = OSDisableInterrupts();

    chorus->work.currentPosHi = 2 * AXFX_FRAME_SAMPLES - ((chorus->baseDelay - 5) << 5);
    chorus->work.currentPosLo = 0;
    chorus->work.currentPosHi =
        (chorus->work.currentPosHi + (chorus->work.currentLast - 1) * AXFX_FRAME_SAMPLES) %
        (3 * AXFX_FRAME_SAMPLES);

    chorus->work.pitchOffsetPeriod = ((chorus->period / 5) + 1) & ~1;
    chorus->work.pitchOffsetPeriodCount = chorus->work.pitchOffsetPeriod >> 1;
    chorus->work.pitchOffset = (chorus->variation << 16) / (chorus->work.pitchOffsetPeriod * 5);

    OSRestoreInterrupts(enabled);
    return TRUE;
}

void AXFXChorusCallback(AXFX_BUFFERUPDATE *bufferUpdate, AXFX_CHORUS *chorus) {
    u8 nextCurrentLast;
    s32 *leftD;
    s32 *rightD;
    s32 *surD;
    s32 *leftS;
    s32 *rightS;
    s32 *surS;
    u32 i;

    /* Keep this frame's input as the newest of the three. */
    nextCurrentLast = (chorus->work.currentLast + 1) % 3;
    leftS = bufferUpdate->left;
    rightS = bufferUpdate->right;
    surS = bufferUpdate->surround;
    leftD = chorus->work.lastLeft[nextCurrentLast];
    rightD = chorus->work.lastRight[nextCurrentLast];
    surD = chorus->work.lastSur[nextCurrentLast];
    for (i = 0; i < AXFX_FRAME_SAMPLES; i++) {
        *leftD++ = *leftS++;
        *rightD++ = *rightS++;
        *surD++ = *surS++;
    }

    /* The read position moves at 1 +- pitchOffset (16.16); the sign flips every half period. */
    chorus->work.src.pitchHi = (chorus->work.pitchOffset >> 16) + 1;
    chorus->work.src.pitchLo = (chorus->work.pitchOffset & 0xFFFF) << 16;
    if (--chorus->work.pitchOffsetPeriodCount == 0) {
        chorus->work.pitchOffsetPeriodCount = chorus->work.pitchOffsetPeriod;
        chorus->work.pitchOffset = -chorus->work.pitchOffset;
    }

    for (i = 0; i < 3; i++) {
        chorus->work.src.posHi = chorus->work.currentPosHi;
        chorus->work.src.posLo = chorus->work.currentPosLo;
        switch (i) {
        case 0:
            chorus->work.src.smpBase = chorus->work.lastLeft[0];
            chorus->work.src.dest = bufferUpdate->left;
            chorus->work.src.old = chorus->work.oldLeft;
            break;
        case 1:
            chorus->work.src.smpBase = chorus->work.lastRight[0];
            chorus->work.src.dest = bufferUpdate->right;
            chorus->work.src.old = chorus->work.oldRight;
            break;
        case 2:
            chorus->work.src.smpBase = chorus->work.lastSur[0];
            chorus->work.src.dest = bufferUpdate->surround;
            chorus->work.src.old = chorus->work.oldSur;
            break;
        }
        switch (chorus->work.src.pitchHi) {
        case 0:
            do_src1(&chorus->work.src);
            break;
        case 1:
            do_src2(&chorus->work.src);
            break;
        }
    }

    chorus->work.currentPosHi = chorus->work.src.posHi % (3 * AXFX_FRAME_SAMPLES);
    chorus->work.currentPosLo = chorus->work.src.posLo;
    chorus->work.currentLast = nextCurrentLast;
}

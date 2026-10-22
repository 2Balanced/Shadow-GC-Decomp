/* Dolphin SDK OS: cache control. Range operations work a 32-byte cache line at a time; the
 * flush/store variants end with `sc` so the system-call handler (OSSync.c) syncs the bus.
 * __OSCacheInit turns on the L1 and L2 caches if the boot code didn't and installs the handler
 * for locked-cache DMA machine checks. Functions the game doesn't use (DCFreeze, the LC DMA
 * helpers, L2SetDataOnly, ...) were dead-stripped. */
#include <dolphin/base/PPCArch.h>
#include <dolphin/os.h>

#define LC_BASE_PREFIX 0xE000
#define LC_LINES 512

#define HID0_ICE 0x8000
#define HID0_DCE 0x4000

#define L2CR_L2E 0x80000000
#define L2CR_L2I 0x00200000
#define L2CR_L2IP 0x00000001

#define HID2_DCHERR 0x00800000 /* locked cache tag already in the cache */
#define HID2_DNCERR 0x00400000 /* DMA accessed the normal cache */
#define HID2_DCMERR 0x00200000 /* DMA missed in the data cache */
#define HID2_DQOERR 0x00100000 /* DMA queue overflow */

#define SRR1_DMA_BIT 0x00200000

#define MSR_IR 0x20
#define MSR_DR 0x10

asm void DCEnable(void) {
    nofralloc
    sync
    mfspr r3, HID0
    ori r3, r3, 0x4000
    mtspr HID0, r3
    blr
}

asm void DCInvalidateRange(register void *addr, register u32 nBytes) {
    nofralloc
    cmplwi nBytes, 0
    blelr
    clrlwi r5, addr, 27
    add nBytes, nBytes, r5
    addi nBytes, nBytes, 31
    srwi nBytes, nBytes, 5
    mtctr nBytes
@1
    dcbi r0, addr
    addi addr, addr, 32
    bdnz @1
    blr
}

asm void DCFlushRange(register void *addr, register u32 nBytes) {
    nofralloc
    cmplwi nBytes, 0
    blelr
    clrlwi r5, addr, 27
    add nBytes, nBytes, r5
    addi nBytes, nBytes, 31
    srwi nBytes, nBytes, 5
    mtctr nBytes
@1
    dcbf r0, addr
    addi addr, addr, 32
    bdnz @1
    sc
    blr
}

asm void DCStoreRange(register void *addr, register u32 nBytes) {
    nofralloc
    cmplwi nBytes, 0
    blelr
    clrlwi r5, addr, 27
    add nBytes, nBytes, r5
    addi nBytes, nBytes, 31
    srwi nBytes, nBytes, 5
    mtctr nBytes
@1
    dcbst r0, addr
    addi addr, addr, 32
    bdnz @1
    sc
    blr
}

asm void DCFlushRangeNoSync(register void *addr, register u32 nBytes) {
    nofralloc
    cmplwi nBytes, 0
    blelr
    clrlwi r5, addr, 27
    add nBytes, nBytes, r5
    addi nBytes, nBytes, 31
    srwi nBytes, nBytes, 5
    mtctr nBytes
@1
    dcbf r0, addr
    addi addr, addr, 32
    bdnz @1
    blr
}

asm void ICInvalidateRange(register void *addr, register u32 nBytes) {
    nofralloc
    cmplwi nBytes, 0
    blelr
    clrlwi r5, addr, 27
    add nBytes, nBytes, r5
    addi nBytes, nBytes, 31
    srwi nBytes, nBytes, 5
    mtctr nBytes
@1
    icbi r0, addr
    addi addr, addr, 32
    bdnz @1
    sync
    isync
    blr
}

asm void ICFlashInvalidate(void) {
    nofralloc
    mfspr r3, HID0
    ori r3, r3, 0x800
    mtspr HID0, r3
    blr
}

asm void ICEnable(void) {
    nofralloc
    isync
    mfspr r3, HID0
    ori r3, r3, 0x8000
    mtspr HID0, r3
    blr
}

/* Invalidates the 16 KB locked cache and clears HID2[LCE]. */
asm void LCDisable(void) {
    nofralloc
    lis r3, LC_BASE_PREFIX
    li r4, LC_LINES
    mtctr r4
@1
    dcbi r0, r3
    addi r3, r3, 32
    bdnz @1
    mfspr r4, HID2
    rlwinm r4, r4, 0, 4, 2
    mtspr HID2, r4
    blr
}

static void L2Disable(void) {
    __sync();
    PPCMtl2cr(PPCMfl2cr() & ~L2CR_L2E);
    __sync();
}

void L2GlobalInvalidate(void) {
    L2Disable();
    PPCMtl2cr(PPCMfl2cr() | L2CR_L2I);
    while (PPCMfl2cr() & L2CR_L2IP);

    PPCMtl2cr(PPCMfl2cr() & ~L2CR_L2I);
    while (PPCMfl2cr() & L2CR_L2IP) {
        DBPrintf(">>> L2 INVALIDATE : SHOULD NEVER HAPPEN\n");
    }
}

static void L2Init(void) {
    u32 oldMSR;

    oldMSR = PPCMfmsr();
    __sync();
    PPCMtmsr(MSR_IR | MSR_DR);
    __sync();
    L2Disable();
    L2GlobalInvalidate();
    PPCMtmsr(oldMSR);
}

static void L2Enable(void) {
    PPCMtl2cr((PPCMfl2cr() | L2CR_L2E) & ~L2CR_L2I);
}

void DMAErrorHandler(OSError error, OSContext *context, ...) {
    u32 hid2 = PPCMfhid2();

    OSReport("Machine check received\n");
    OSReport("HID2 = 0x%x   SRR1 = 0x%x\n", hid2, context->srr1);
    if (!(hid2 & (HID2_DCHERR | HID2_DNCERR | HID2_DCMERR | HID2_DQOERR)) ||
        !(context->srr1 & SRR1_DMA_BIT)) {
        OSReport("Machine check was not DMA/locked cache related\n");
        OSDumpContext(context);
        PPCHalt();
    }

    OSReport("DMAErrorHandler(): An error occurred while processing DMA.\n");
    OSReport("The following errors have been detected and cleared :\n");

    if (hid2 & HID2_DCHERR) {
        OSReport("\t- Requested a locked cache tag that was already in the cache\n");
    }
    if (hid2 & HID2_DNCERR) {
        OSReport("\t- DMA attempted to access normal cache\n");
    }
    if (hid2 & HID2_DCMERR) {
        OSReport("\t- DMA missed in data cache\n");
    }
    if (hid2 & HID2_DQOERR) {
        OSReport("\t- DMA queue overflowed\n");
    }

    /* Writing HID2 back clears the error bits. */
    PPCMthid2(hid2);
}

void __OSCacheInit(void) {
    if (!(PPCMfhid0() & HID0_ICE)) {
        ICEnable();
        DBPrintf("L1 i-caches initialized\n");
    }
    if (!(PPCMfhid0() & HID0_DCE)) {
        DCEnable();
        DBPrintf("L1 d-caches initialized\n");
    }
    if (!(PPCMfl2cr() & L2CR_L2E)) {
        L2Init();
        L2Enable();
        DBPrintf("L2 cache initialized\n");
    }

    OSSetErrorHandler(OS_ERROR_MACHINE_CHECK, DMAErrorHandler);
    DBPrintf("Locked cache machine check handler installed\n");
}

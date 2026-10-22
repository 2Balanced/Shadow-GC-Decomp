/* Dolphin SDK OS: L1/L2 cache management.
 *
 * The range primitives below walk the range one 32-byte block at a time. The flush and store
 * variants finish with `sc`, which the system-call handler (OSSync.c) turns into a bus sync.
 * __OSCacheInit switches on whichever of the instruction, data and L2 caches the boot code left
 * off, then hooks machine checks so locked-cache DMA errors are reported. The rest of the SDK's
 * cache API (DCFreeze, the locked-cache DMA helpers, L2SetDataOnly and so on) is not linked by
 * the game. */
#include <dolphin/base/PPCArch.h>
#include <dolphin/os.h>

/* Locked cache: mapped at 0xE0000000, 16 KB = 512 blocks. */
#define LOCKED_CACHE_HI    0xE000
#define LOCKED_CACHE_BLOCKS 512

/* HID0 cache enables. */
#define HID0_ICACHE_ON 0x8000
#define HID0_DCACHE_ON 0x4000

/* L2CR fields. */
#define L2CR_ENABLE      0x80000000
#define L2CR_INVALIDATE  0x00200000 /* start a global invalidate */
#define L2CR_INVAL_BUSY  0x00000001 /* global invalidate still running */

/* HID2 locked-cache DMA error flags; writing HID2 back acknowledges them. */
#define HID2_ERR_TAG_HIT    0x00800000 /* locked cache tag already present in the cache */
#define HID2_ERR_NORMAL_ACC 0x00400000 /* DMA touched the normal cache */
#define HID2_ERR_DC_MISS    0x00200000 /* DMA missed in the data cache */
#define HID2_ERR_QUEUE_OVF  0x00100000 /* DMA queue overflowed */
#define HID2_ERR_ANY (HID2_ERR_TAG_HIT | HID2_ERR_NORMAL_ACC | HID2_ERR_DC_MISS | HID2_ERR_QUEUE_OVF)

/* SRR1 bit set when the machine check came from the DMA engine. */
#define SRR1_FROM_DMA 0x00200000

/* MSR address translation enables. */
#define MSR_ITRANS 0x20
#define MSR_DTRANS 0x10

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

/* Throws away the locked cache contents and turns HID2[LCE] off again. */
asm void LCDisable(void) {
    nofralloc
    lis r3, LOCKED_CACHE_HI
    li r4, LOCKED_CACHE_BLOCKS
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
    PPCMtl2cr(PPCMfl2cr() & ~L2CR_ENABLE);
    __sync();
}

/* Disables the L2 and runs a hardware global invalidate, polling until the cache reports done. */
void L2GlobalInvalidate(void) {
    L2Disable();

    PPCMtl2cr(PPCMfl2cr() | L2CR_INVALIDATE);
    while ((PPCMfl2cr() & L2CR_INVAL_BUSY) != 0) {}

    PPCMtl2cr(PPCMfl2cr() & ~L2CR_INVALIDATE);
    while ((PPCMfl2cr() & L2CR_INVAL_BUSY) != 0) {
        /* Clearing the start bit should never restart the invalidate. */
        DBPrintf(">>> L2 INVALIDATE : SHOULD NEVER HAPPEN\n");
    }
}

/* The invalidate is done with translation on and interrupts masked; the old MSR is put back. */
static void L2Init(void) {
    u32 savedMsr = PPCMfmsr();

    __sync();
    PPCMtmsr(MSR_ITRANS | MSR_DTRANS);
    __sync();
    L2Disable();
    L2GlobalInvalidate();
    PPCMtmsr(savedMsr);
}

static void L2Enable(void) {
    /* Set the enable bit and make sure no new global invalidate gets started. */
    PPCMtl2cr((PPCMfl2cr() | L2CR_ENABLE) & ~L2CR_INVALIDATE);
}

#define REPORT_DMA_ERROR(flags, bit, text)     if ((flags) & (bit)) {                         OSReport(text);                        }

/* Machine check handler: anything that is not a locked-cache DMA error is fatal. */
void DMAErrorHandler(OSError error, OSContext *context, ...) {
    u32 errors = PPCMfhid2();

    OSReport("Machine check received\n");
    OSReport("HID2 = 0x%x   SRR1 = 0x%x\n", errors, context->srr1);

    if (!(errors & HID2_ERR_ANY) || !(context->srr1 & SRR1_FROM_DMA)) {
        OSReport("Machine check was not DMA/locked cache related\n");
        OSDumpContext(context);
        PPCHalt();
    }

    OSReport("DMAErrorHandler(): An error occurred while processing DMA.\n");
    OSReport("The following errors have been detected and cleared :\n");
    REPORT_DMA_ERROR(errors, HID2_ERR_TAG_HIT, "	- Requested a locked cache tag that was already in the cache\n");
    REPORT_DMA_ERROR(errors, HID2_ERR_NORMAL_ACC, "	- DMA attempted to access normal cache\n");
    REPORT_DMA_ERROR(errors, HID2_ERR_DC_MISS, "	- DMA missed in data cache\n");
    REPORT_DMA_ERROR(errors, HID2_ERR_QUEUE_OVF, "	- DMA queue overflowed\n");

    PPCMthid2(errors);
}

void __OSCacheInit(void) {
    if (!(PPCMfhid0() & HID0_ICACHE_ON)) {
        ICEnable();
        DBPrintf("L1 i-caches initialized\n");
    }
    if (!(PPCMfhid0() & HID0_DCACHE_ON)) {
        DCEnable();
        DBPrintf("L1 d-caches initialized\n");
    }
    if (!(PPCMfl2cr() & L2CR_ENABLE)) {
        L2Init();
        L2Enable();
        DBPrintf("L2 cache initialized\n");
    }

    OSSetErrorHandler(OS_ERROR_MACHINE_CHECK, DMAErrorHandler);
    DBPrintf("Locked cache machine check handler installed\n");
}

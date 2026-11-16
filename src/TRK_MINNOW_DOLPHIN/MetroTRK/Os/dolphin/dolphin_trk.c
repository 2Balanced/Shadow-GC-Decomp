#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>
#include <TRK_MINNOW_DOLPHIN/targimpl.h>
#include <dolphin/os.h>

extern u8 _db_stack_addr[];

extern u32 ARGetDMAStatus(void);
extern void ARStartDMA(u32 type, u32 mainAddress, u32 aramAddress, u32 length);
extern void __ARClearInterrupt(void);
extern u16 __ARGetInterruptStatus(void);

#define ARAM_BASE 0x4000
#define ARAM_END 0x08000000
#define ARAM_BLOCK 32

/* The locked cache, where the OS may have mapped the low-memory vectors. */
#define LOCKED_CACHE_SIZE 0x4000
static u32 lc_base;

/* Exception vectors the nub can take over, by bit of the mask the OS leaves at 0x80000044. */
static u32 TRK_ISR_OFFSETS[15] = {
    0x0100, 0x0200, 0x0300, 0x0400, 0x0500, 0x0600, 0x0700, 0x0800,
    0x0900, 0x0C00, 0x0D00, 0x0F00, 0x1300, 0x1400, 0x1700,
};

#pragma weak InitMetroTRK_BBA

void EnableMetroTRKInterrupts(void) {
    EnableEXI2Interrupts();
}

/* The address to use for a target address: itself when it is in the locked cache or in the
 * 0x7E000000 window, otherwise its cached mirror. */
u32 TRKTargetTranslate(u32 address) {
    if (address >= lc_base && address < lc_base + LOCKED_CACHE_SIZE &&
        (gTRKCPUState.extended1[TRK_EXT1_DBAT3U] & 3) != 0) {
        return address;
    }
    if (address >= 0x7E000000 && address <= 0x80000000) {
        return address;
    }
    return (address & 0x3FFFFFFF) | 0x80000000;
}

/* Installs the nub's vector code over the exception vectors the OS marked for the debugger. */
void __TRK_copy_vectors(void) {
    int i;
    u32 mask;
    u32 offset;
    void *vector;

    mask = *(u32 *)TRKTargetTranslate(0x44);
    for (i = 0; i <= 14; i++) {
        if ((mask & (1 << i)) != 0 && i != 4) {
            offset = TRK_ISR_OFFSETS[i];
            vector = (void *)TRKTargetTranslate(offset);
            TRK_memcpy(vector, gTRKInterruptVectorTable + offset, 0x100);
            TRK_flush_cache((u32)vector, 0x100);
        }
    }
}

TRKResult TRKInitializeTarget(void) {
    gTRKState.stopped = TRUE;
    gTRKState.msr = __TRK_get_MSR();
    lc_base = 0xE0000000;
    return kTRKOk;
}

/* Reads audio RAM into `data`, whole 32-byte blocks around the requested range. */
void __TRK_read_aram(register void *data, u32 address, TRKSize *length) {
    u32 blockAddress;
    u32 blockLength;
    register u32 i;
    u16 wasPending;

    if (address < ARAM_BASE || address + *length > ARAM_END) {
        return;
    }
    blockAddress = address & ~(ARAM_BLOCK - 1);
    blockLength = *length + (address & (ARAM_BLOCK - 1));
    blockLength = (blockLength + (ARAM_BLOCK - 1)) & ~(ARAM_BLOCK - 1);
    for (i = 0; i < blockLength; i += ARAM_BLOCK) {
        asm { dcbi i, data }
    }
    while (ARGetDMAStatus() != 0) {
    }
    wasPending = __ARGetInterruptStatus();
    __ARClearInterrupt();
    ARStartDMA(1, (u32)data, blockAddress, blockLength);
    while (__ARGetInterruptStatus() == 0) {
    }
    if (wasPending == 0) {
        __ARClearInterrupt();
    }
}

/* Writes `data` to audio RAM. The DMA moves whole 32-byte blocks, so a partial first or last
 * block is first read back and merged. */
void __TRK_write_aram(register void *data, u32 address, TRKSize *length) {
    u8 block[ARAM_BLOCK] __attribute__((aligned(32)));
    u16 wasPending;
    register u8 *blockPtr;
    u32 blockAddress;
    u32 blockLength;
    u32 fetched;
    register u32 i;

    if (address < ARAM_BASE || address + *length > ARAM_END) {
        return;
    }
    blockAddress = address & ~(ARAM_BLOCK - 1);
    blockLength = *length + (address & (ARAM_BLOCK - 1));
    blockLength = (blockLength + (ARAM_BLOCK - 1)) & ~(ARAM_BLOCK - 1);
    for (i = 0; i < blockLength; i += ARAM_BLOCK) {
        asm { dcbf i, data }
    }
    while (ARGetDMAStatus() != 0) {
    }
    wasPending = __ARGetInterruptStatus();
    fetched = ARAM_END;
    i = address & (ARAM_BLOCK - 1);
    if (i != 0) {
        blockPtr = block;
        fetched = blockAddress;
        asm { dcbi 0, blockPtr }
        __ARClearInterrupt();
        ARStartDMA(1, (u32)blockPtr, blockAddress, ARAM_BLOCK);
        while (__ARGetInterruptStatus() == 0) {
        }
        TRK_memcpy(data, block, i);
        asm { dcbf 0, data }
    }
    address += *length;
    i = address & (ARAM_BLOCK - 1);
    if (i != 0) {
        register u8 *tail;
        u32 lastBlock;

        lastBlock = address & ~(ARAM_BLOCK - 1);
        if (lastBlock != fetched) {
            blockPtr = block;
            asm { dcbi 0, blockPtr }
            __ARClearInterrupt();
            ARStartDMA(1, (u32)blockPtr, lastBlock, ARAM_BLOCK);
            while (__ARGetInterruptStatus() == 0) {
            }
        }
        tail = (u8 *)data + address;
        TRK_memcpy(tail, block + i, ARAM_BLOCK - i);
        asm { dcbf 0, tail }
    }
    __sync();
    __ARClearInterrupt();
    ARStartDMA(0, (u32)data, blockAddress, blockLength);
    if (wasPending == 0) {
        while (__ARGetInterruptStatus() == 0) {
        }
        __ARClearInterrupt();
    }
}

/* The nub's entry, called by the OS with the debugging hardware's id in r5: saves the caller's
 * registers as the target's state, moves to the debugger stack and starts the nub. */
asm void InitMetroTRK(void) {
    nofralloc
    addi r1, r1, -4
    stw r3, 0(r1)
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    stmw r0, 0(r3)
    lwz r4, 0(r1)
    addi r1, r1, 4
    stw r1, 4(r3)
    stw r4, 0xC(r3)
    mflr r4
    stw r4, 0x84(r3)
    stw r4, 0x80(r3)
    mfcr r4
    stw r4, 0x88(r3)
    mfmsr r4
    ori r3, r4, 0x8000
    xori r3, r3, 0x8000
    mtmsr r3
    mtsrr1 r4
    bl TRKSaveExtended1Block
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    lmw r0, 0(r3)
    li r0, 0
    mtspr 1010, r0
    mtspr 1013, r0
    lis r1, _db_stack_addr@h
    ori r1, r1, _db_stack_addr@l
    mr r3, r5
    bl InitMetroTRKCommTable
    cmpwi r3, 1
    bne start
    lwz r4, 0x84(r3)
    mtlr r4
    lmw r0, 0(r3)
    blr
start:
    b TRK_main
    blr
}

/* The nub's entry for the broadband adapter: as InitMetroTRK, but with external interrupts
 * left on and the hardware id fixed. */
asm void InitMetroTRK_BBA(void) {
    nofralloc
    addi r1, r1, -4
    stw r3, 0(r1)
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    stmw r0, 0(r3)
    lwz r4, 0(r1)
    addi r1, r1, 4
    stw r1, 4(r3)
    stw r4, 0xC(r3)
    mflr r4
    stw r4, 0x84(r3)
    stw r4, 0x80(r3)
    mfcr r4
    stw r4, 0x88(r3)
    mfmsr r4
    ori r3, r4, 0x8000
    mtmsr r3
    mtsrr1 r4
    bl TRKSaveExtended1Block
    lis r3, gTRKCPUState@h
    ori r3, r3, gTRKCPUState@l
    lmw r0, 0(r3)
    li r0, 0
    mtspr 1010, r0
    mtspr 1013, r0
    lis r1, _db_stack_addr@h
    ori r1, r1, _db_stack_addr@l
    li r3, 2
    bl InitMetroTRKCommTable
    cmpwi r3, 1
    bne start
    lwz r4, 0x84(r3)
    mtlr r4
    lmw r0, 0(r3)
    blr
start:
    b TRK_main
    blr
}

void __TRK_reset(void) {
    OSResetSystem(0, 0, 0);
}

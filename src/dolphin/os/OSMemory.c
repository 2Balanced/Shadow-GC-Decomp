/* Dolphin SDK OS: main memory size and protection.
 *
 * The memory interface can trap accesses to four protected ranges and to addresses past the
 * end of RAM; both arrive here as MEM interrupts and are reported as OS_ERROR_PROTECTION. On a
 * development console set to simulate a smaller retail memory size, the BATs are reprogrammed so
 * only the simulated amount is mapped. */
#include <dolphin/os.h>

volatile u16 __MEMRegs[] : 0xCC004000;

#define OS_ERROR_PROTECTION 15

/* MEM registers. */
#define MEM_PROT_ENABLE 8 /* per-range protection enables */
#define MEM_INT_ENABLE 14
#define MEM_INT_CAUSE 15
#define MEM_INT_ADDR_LO 17
#define MEM_INT_ADDR_HI 18
#define MEM_INT_CLEAR 16
#define MEM_REFRESH 20

#define OS_INTERRUPTMASK_MEM_0_3 0xF0000000
#define OS_INTERRUPTMASK_MEM_ADDRESS 0x08000000

#define OSGetPhysicalMemSize() (*(u32 *)OSPhysicalToCached(0x0028))
#define OSGetConsoleSimulatedMemSize() (*(u32 *)OSPhysicalToCached(0x00F0))

extern OSErrorHandler __OSErrorTable[];

void __OSUnhandledException(__OSException exception, OSContext *context, u32 dsisr, u32 dar);
u32 __OSMaskInterrupts(u32 mask);
u32 __OSUnmaskInterrupts(u32 mask);
void *__OSSetInterruptHandler(s16 interrupt, void *handler);

static BOOL OnReset(BOOL final);

static OSResetFunctionInfo ResetFunctionInfo = {OnReset, 127, NULL, NULL};

/* Protection is switched off before the next program starts. */
static BOOL OnReset(BOOL final) {
    if (final) {
        __MEMRegs[MEM_PROT_ENABLE] = 0xFF;
        __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0_3);
    }
    return TRUE;
}

static void MEMIntrruptHandler(s16 interrupt, OSContext *context) {
    u32 addr;
    u32 cause;

    addr = ((__MEMRegs[MEM_INT_ADDR_HI] & 0x3FF) << 16) | __MEMRegs[MEM_INT_ADDR_LO];
    cause = __MEMRegs[MEM_INT_CAUSE];
    __MEMRegs[MEM_INT_CLEAR] = 0;

    if (__OSErrorTable[OS_ERROR_PROTECTION] != NULL) {
        __OSErrorTable[OS_ERROR_PROTECTION](OS_ERROR_PROTECTION, context, cause, addr);
        return;
    }
    __OSUnhandledException(OS_ERROR_PROTECTION, context, cause, addr);
}

/* BAT setups for a 24 MB and a 48 MB machine: BAT0 maps the first 16 or 32 MB, BAT2 the rest.
 * They run in real mode (see RealMode) and return with rfi. */
static asm void Config24MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x0000
    addi r4, r4, 0x0002
    lis r3, 0x8000
    addi r3, r3, 0x01FF
    lis r6, 0x0100
    addi r6, r6, 0x0002
    lis r5, 0x8100
    addi r5, r5, 0x00FF
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

static asm void Config48MB(void) {
    nofralloc
    li r7, 0
    lis r4, 0x0000
    addi r4, r4, 0x0002
    lis r3, 0x8000
    addi r3, r3, 0x03FF
    lis r6, 0x0200
    addi r6, r6, 0x0002
    lis r5, 0x8200
    addi r5, r5, 0x01FF
    isync
    mtdbatu 0, r7
    mtdbatl 0, r4
    mtdbatu 0, r3
    isync
    mtibatu 0, r7
    mtibatl 0, r4
    mtibatu 0, r3
    isync
    mtdbatu 2, r7
    mtdbatl 2, r6
    mtdbatu 2, r5
    isync
    mtibatu 2, r7
    mtibatl 2, r6
    mtibatu 2, r5
    isync
    mfmsr r3
    ori r3, r3, 0x30
    mtsrr1 r3
    mflr r3
    mtsrr0 r3
    rfi
}

/* Calls `config` with address translation off; it returns to our caller. */
static asm void RealMode(register u32 config) {
    nofralloc
    clrlwi config, config, 2
    mtsrr0 config
    mfmsr config
    rlwinm config, config, 0, 28, 25
    mtsrr1 config
    rfi
}

void __OSInitMemoryProtection(void) {
    u32 simulatedSize;
    BOOL enabled;
    u32 unused[12]; /* stack the original reserves but never touches */

    simulatedSize = OSGetConsoleSimulatedMemSize();
    enabled = OSDisableInterrupts();

    __MEMRegs[MEM_INT_CLEAR] = 0;
    __MEMRegs[MEM_PROT_ENABLE] = 0xFF;
    __OSMaskInterrupts(OS_INTERRUPTMASK_MEM_0_3);
    __OSSetInterruptHandler(0, MEMIntrruptHandler);
    __OSSetInterruptHandler(1, MEMIntrruptHandler);
    __OSSetInterruptHandler(2, MEMIntrruptHandler);
    __OSSetInterruptHandler(3, MEMIntrruptHandler);
    __OSSetInterruptHandler(4, MEMIntrruptHandler);
    OSRegisterResetFunction(&ResetFunctionInfo);

    if (OSGetConsoleSimulatedMemSize() < OSGetPhysicalMemSize() &&
        OSGetConsoleSimulatedMemSize() == 0x1800000) {
        DCInvalidateRange((void *)0x81800000, 0x1800000);
        __MEMRegs[MEM_REFRESH] = 2;
    }

    if (simulatedSize <= 0x1800000) {
        RealMode((u32)Config24MB);
    } else if (simulatedSize <= 0x3000000) {
        RealMode((u32)Config48MB);
    }

    __OSUnmaskInterrupts(OS_INTERRUPTMASK_MEM_ADDRESS);
    OSRestoreInterrupts(enabled);
}

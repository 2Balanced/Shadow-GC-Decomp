#include <TRK_MINNOW_DOLPHIN/targimpl.h>
#include <TRK_MINNOW_DOLPHIN/dolphin_trk.h>
#include <TRK_MINNOW_DOLPHIN/msghndlr.h>

/* The target side of the nub on a PowerPC 750: the saved processor state, register and memory
 * access for the debugger's commands, single-stepping, and the exception entry and exit. */

/* An exception the nub took while it was running, and whether it is running at all. */
typedef struct TRKExceptionStatus {
    struct {
        u32 pc;             /* 0x0 */
        u32 instruction;    /* 0x4 */
        u16 exception;      /* 0x8 */
    } where;
    u8 inTRK;               /* 0xC */
    u8 exceptionDetected;   /* 0xD */
} TRKExceptionStatus;       /* 0x10 */

typedef struct TRKStepStatus {
    BOOL active;            /* 0x00 */
    int type;               /* 0x04, TRKStepOption */
    u32 count;              /* 0x08 */
    u32 rangeStart;         /* 0x0C */
    u32 rangeEnd;           /* 0x10 */
} TRKStepStatus;            /* 0x14 */

/* A range of memory and what the debugger may do with it. */
typedef struct TRKMemoryRange {
    u32 start;
    u32 end;
    BOOL readable;
    BOOL writeable;
} TRKMemoryRange;

/* The notification sent to the host when the target stops or the nub faults. */
typedef struct TRKNotifyMessage {
    u32 size;               /* 0x00 */
    u8 command;             /* 0x04 */
    u8 pad5[3];
    u32 pc;                 /* 0x08 */
    u32 instruction;        /* 0x0C */
    u32 exception;          /* 0x10 */
    u8 pad14[0x2C];
} TRKNotifyMessage;         /* 0x40 */

/* A 64-bit register as it travels in a message: two words, word-aligned. */
typedef struct TRKRegister64 {
    u32 high;
    u32 low;
} TRKRegister64;

/* A scratch value for the register-access stubs (128 bits, for an AltiVec register). */
typedef struct TRKValue128 {
    u32 words[4];
} TRKValue128;

#define PPC_NOP 0x60000000
#define PPC_BLR 0x4E800020
#define PPC_MFSPR_R4(spr) (0x7C8002A6 | (((spr) & 0x1F) << 16) | (((spr) & 0x3E0) << 6))
#define PPC_MTSPR_R4(spr) (0x7C8003A6 | (((spr) & 0x1F) << 16) | (((spr) & 0x3E0) << 6))
#define PPC_STW_R4_0_R3 0x90830000
#define PPC_LWZ_R4_0_R3 0x80830000
#define PPC_LFD_0_R3(reg) (0xC8030000 | ((reg) << 21))
#define PPC_STFD_0_R3(reg) (0xD8030000 | ((reg) << 21))
#define PPC_PSQ_L_0_R3(reg) (0xE0030000 | ((reg) << 21))
#define PPC_PSQ_ST_0_R3(reg) (0xF0030000 | ((reg) << 21))

#define SPR_GQR0 912
#define SPR_HID2 920
#define SPR_FPECR 1022

#define MSR_FP 0x2000
#define MSR_SE 0x0400
#define MSR_DR 0x0010

#define TRK_STUB_WORDS 10

/* Explicitly zero: that places it ahead of the uninitialised globals in .bss, as in retail. */
static u16 TRK_saved_exceptionID = 0;
TRKState gTRKState;
TRKCPUState gTRKCPUState;
struct {
    u32 gpr[32];
    u32 pc;
    u32 lr;
    u32 cr;
    u32 ctr;
    u32 xer;
} gTRKSaveState;
static TRKValue128 TRKvalue128_temp;

TRKRestoreFlags gTRKRestoreFlags = {FALSE, FALSE};
static TRKExceptionStatus gTRKExceptionStatus = {{0, 0, 0}, TRUE, FALSE};
static TRKStepStatus gTRKStepStatus = {FALSE, kTRKStepIntoCount, 0, 0, 0};

static const TRKMemoryRange gTRKMemMap[1] = {{0x00000000, 0xFFFFFFFF, TRUE, TRUE}};

static void TRKExceptionHandler(void);
void TRKInterruptHandlerEnableInterrupts(void);
void TRKPostInterruptEvent(void);
TRKResult TRKTargetReadInstruction(void *data, u32 address);

asm u32 __TRK_get_MSR(void) {
    nofralloc
    mfmsr r3
    blr
}

asm void __TRK_set_MSR(register u32 msr) {
    nofralloc
    mtmsr r3
    blr
}

/* Copies bytes with a different MSR for the reads and the writes, so that one side can be
 * accessed with the target's address translation and the other with the nub's. */
asm static void TRK_ppc_memcpy(register void *dest, register const void *src, register int length,
                               register u32 destMSR, register u32 srcMSR) {
    nofralloc
    mfmsr r8
    li r10, 0x0
at_C98C:
    cmpw r10, r5
    beq at_C9B4
    mtmsr r7
    sync
    lbzx r9, r10, r4
    mtmsr r6
    sync
    stbx r9, r10, r3
    addi r10, r10, 0x1
    b at_C98C
at_C9B4:
    mtmsr r8
    sync
    blr
}

/* Entered from the vector code with the exception number in r3 and the interrupted r2-r4 in
 * SPRG1-SPRG3. Connection interrupts taken while the target runs just flag the input; anything
 * else saves the target's registers, restores the nub's and returns into the nub
 * (TRKPostInterruptEvent). An exception inside the nub itself goes to TRKExceptionHandler. */
asm void TRKInterruptHandler(void) {
    nofralloc
    mtspr 26, r2
    mtspr 27, r4
    mfspr r4, 275
    mfcr r2
    mtspr 275, r2
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r2, 0x8c(r2)
    ori r2, r2, 0x8002
    xori r2, r2, 0x8002
    sync
    mtmsr r2
    sync
    lis r2, TRK_saved_exceptionID@h
    ori r2, r2, TRK_saved_exceptionID@l
    sth r3, 0x0(r2)
    cmpwi r3, 0x500
    bne at_CA88
    lis r2, gTRKCPUState@h
    ori r2, r2, gTRKCPUState@l
    mfspr r3, 8
    stw r3, 0x42c(r2)
    bl TRKUARTInterruptHandler
    lis r2, gTRKCPUState@h
    ori r2, r2, gTRKCPUState@l
    lwz r3, 0x42c(r2)
    mtspr 8, r3
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r2, 0xa0(r2)
    lbz r2, 0x0(r2)
    cmpwi r2, 0x0
    beq at_CA6C
    lis r2, gTRKExceptionStatus@h
    ori r2, r2, gTRKExceptionStatus@l
    lbz r2, 0xc(r2)
    cmpwi r2, 0x1
    beq at_CA6C
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    li r3, 0x1
    stb r3, 0x9c(r2)
    b at_CA88
at_CA6C:
    lis r2, gTRKSaveState@h
    ori r2, r2, gTRKSaveState@l
    lwz r3, 0x88(r2)
    mtcrf 0xFF, r3
    lwz r3, 0xc(r2)
    lwz r2, 0x8(r2)
    rfi
at_CA88:
    lis r2, TRK_saved_exceptionID@h
    ori r2, r2, TRK_saved_exceptionID@l
    lhz r3, 0x0(r2)
    lis r2, gTRKExceptionStatus@h
    ori r2, r2, gTRKExceptionStatus@l
    lbz r2, 0xc(r2)
    cmpwi r2, 0x0
    bne TRKExceptionHandler
    lis r2, gTRKCPUState@h
    ori r2, r2, gTRKCPUState@l
    stw r0, 0x0(r2)
    stw r1, 0x4(r2)
    mfspr r0, 273
    stw r0, 0x8(r2)
    sth r3, 0x2f8(r2)
    sth r3, 0x2fa(r2)
    mfspr r0, 274
    stw r0, 0xc(r2)
    stmw r4, 0x10(r2)
    mfspr r27, 26
    mfspr r28, 8
    mfspr r29, 275
    mfspr r30, 9
    mfspr r31, 1
    stmw r27, 0x80(r2)
    bl TRKSaveExtended1Block
    lis r2, gTRKExceptionStatus@h
    ori r2, r2, gTRKExceptionStatus@l
    li r3, 0x1
    stb r3, 0xc(r2)
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r0, 0x8c(r2)
    sync
    mtmsr r0
    sync
    lwz r0, 0x80(r2)
    mtspr 8, r0
    lwz r0, 0x84(r2)
    mtspr 9, r0
    lwz r0, 0x88(r2)
    mtspr 1, r0
    lwz r0, 0x94(r2)
    mtspr 18, r0
    lwz r0, 0x90(r2)
    mtspr 19, r0
    lmw r3, 0xc(r2)
    lwz r0, 0x0(r2)
    lwz r1, 0x4(r2)
    lwz r2, 0x8(r2)
    b TRKPostInterruptEvent
}

/* An exception while the nub was running (typically a bad address in a memory command): record
 * it, skip the faulting instruction where that is the right recovery, and resume the nub. */
asm static void TRKExceptionHandler(void) {
    nofralloc
    lis r2, gTRKExceptionStatus@h
    ori r2, r2, gTRKExceptionStatus@l
    sth r3, 0x8(r2)
    mfspr r3, 26
    stw r3, 0x0(r2)
    lhz r3, 0x8(r2)
    cmpwi r3, 0x200
    beq at_CBC0
    cmpwi r3, 0x300
    beq at_CBC0
    cmpwi r3, 0x400
    beq at_CBC0
    cmpwi r3, 0x600
    beq at_CBC0
    cmpwi r3, 0x700
    beq at_CBC0
    cmpwi r3, 0x800
    beq at_CBC0
    cmpwi r3, 0x1000
    beq at_CBC0
    cmpwi r3, 0x1100
    beq at_CBC0
    cmpwi r3, 0x1200
    beq at_CBC0
    cmpwi r3, 0x1300
    beq at_CBC0
    b at_CBCC
at_CBC0:
    mfspr r3, 26
    addi r3, r3, 0x4
    mtspr 26, r3
at_CBCC:
    lis r2, gTRKExceptionStatus@h
    ori r2, r2, gTRKExceptionStatus@l
    li r3, 0x1
    stb r3, 0xd(r2)
    mfspr r3, 275
    mtcrf 0xFF, r3
    mfspr r2, 273
    mfspr r3, 274
    rfi
}

/* Saves the nub's registers and resumes the target; "returns" when the target next stops. */
asm void TRKSwapAndGo(void) {
    nofralloc
    lis r3, gTRKState@h
    ori r3, r3, gTRKState@l
    stmw r0, 0x0(r3)
    mfmsr r0
    stw r0, 0x8c(r3)
    mfspr r0, 8
    stw r0, 0x80(r3)
    mfspr r0, 9
    stw r0, 0x84(r3)
    mfspr r0, 1
    stw r0, 0x88(r3)
    mfspr r0, 18
    stw r0, 0x94(r3)
    mfspr r0, 19
    stw r0, 0x90(r3)
    li r1, -0x7ffe
    nor r1, r1, r1
    mfmsr r3
    and r3, r3, r1
    mtmsr r3
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r2, 0xa0(r2)
    lbz r2, 0x0(r2)
    cmpwi r2, 0x0
    beq at_CC6C
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    li r3, 0x1
    stb r3, 0x9c(r2)
    b TRKInterruptHandlerEnableInterrupts
at_CC6C:
    lis r2, gTRKExceptionStatus@h
    ori r2, r2, gTRKExceptionStatus@l
    li r3, 0x0
    stb r3, 0xc(r2)
    bl TRKRestoreExtended1Block
    lis r2, gTRKCPUState@h
    ori r2, r2, gTRKCPUState@l
    lmw r27, 0x80(r2)
    mtspr 26, r27
    mtspr 8, r28
    mtcrf 0xFF, r29
    mtspr 9, r30
    mtspr 1, r31
    lmw r3, 0xc(r2)
    lwz r0, 0x0(r2)
    lwz r1, 0x4(r2)
    lwz r2, 0x8(r2)
    rfi
}

asm void TRKInterruptHandlerEnableInterrupts(void) {
    nofralloc
    lis r2, gTRKState@h
    ori r2, r2, gTRKState@l
    lwz r0, 0x8c(r2)
    sync
    mtmsr r0
    sync
    lwz r0, 0x80(r2)
    mtspr 8, r0
    lwz r0, 0x84(r2)
    mtspr 9, r0
    lwz r0, 0x88(r2)
    mtspr 1, r0
    lwz r0, 0x94(r2)
    mtspr 18, r0
    lwz r0, 0x90(r2)
    mtspr 19, r0
    lmw r3, 0xc(r2)
    lwz r0, 0x0(r2)
    lwz r1, 0x4(r2)
    lwz r2, 0x8(r2)
    b TRKPostInterruptEvent
}

/* The FPSCR goes through f31, which is saved and restored around it as both a double and a
 * paired single. Retail's save code is the kind a compiler writes for a function that uses f31;
 * the compilers at hand do not write the paired-single half, so the frame is spelled out. */
static asm void ReadFPSCR(register f64 *value) {
    nofralloc
    stwu r1, -0x40(r1)
    stfd fp31, 0x10(r1)
    psq_st fp31, 0x20(r1), 0, 0
    mffs fp31
    stfd fp31, 0(value)
    psq_l fp31, 0x20(r1), 0, 0
    lfd fp31, 0x10(r1)
    addi r1, r1, 0x40
    blr
}

static asm void WriteFPSCR(register f64 *value) {
    nofralloc
    stwu r1, -0x40(r1)
    stfd fp31, 0x10(r1)
    psq_st fp31, 0x20(r1), 0, 0
    lfd fp31, 0(value)
    mtfsf 0xFF, fp31
    psq_l fp31, 0x20(r1), 0, 0
    lfd fp31, 0x10(r1)
    addi r1, r1, 0x40
    blr
}

/* Checks a range against the memory map; ranges the map does not cover are checked piecewise. */
TRKResult TRKValidMemory32(const void *address, TRKSize length, int write) {
    TRKResult result = kTRKInvalidMemory;
    const u8 *start;
    const u8 *end;
    int i;

    start = (const u8 *)address;
    end = (const u8 *)address + (length - 1);
    if (end < start) {
        return kTRKInvalidMemory;
    }
    for (i = 0; i < (int)(sizeof(gTRKMemMap) / sizeof(gTRKMemMap[0])); i++) {
        if (start <= (const u8 *)gTRKMemMap[i].end && end >= (const u8 *)gTRKMemMap[i].start) {
            if ((write == 0 && !gTRKMemMap[i].readable) || (write == 1 && !gTRKMemMap[i].writeable)) {
                result = kTRKInvalidMemory;
            } else {
                result = kTRKOk;
                if (start < (const u8 *)gTRKMemMap[i].start) {
                    result = TRKValidMemory32(start, (const u8 *)gTRKMemMap[i].start - start, write);
                }
                if (result == kTRKOk && end > (const u8 *)gTRKMemMap[i].end) {
                    result = TRKValidMemory32((const u8 *)gTRKMemMap[i].end,
                                              end - (const u8 *)gTRKMemMap[i].end, write);
                }
            }
            break;
        }
    }
    return result;
}

/* Runs a few instructions built on the stack: the way to reach a register whose number is only
 * known at run time. The stub gets the value's address in r3 and a scratch area in r4. */
static inline TRKResult TRKPPCAccessSpecialReg(void *value, u32 *stub, BOOL read) {
    stub[TRK_STUB_WORDS - 1] = PPC_BLR;
    TRK_flush_cache((u32)stub, TRK_STUB_WORDS * sizeof(u32));
    ((void (*)(void *, void *))stub)(value, &TRKvalue128_temp);
    return kTRKOk;
}

static inline TRKResult TRKPPCAccessSPR(void *value, u32 spr, BOOL read) {
    u32 stub[TRK_STUB_WORDS] = {PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP,
                                PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP};

    if (read) {
        stub[0] = PPC_MFSPR_R4(spr);
        stub[1] = PPC_STW_R4_0_R3;
    } else {
        stub[0] = PPC_LWZ_R4_0_R3;
        stub[1] = PPC_MTSPR_R4(spr);
    }
    return TRKPPCAccessSpecialReg(value, stub, read);
}

static inline TRKResult TRKPPCAccessPairedSingleRegister(void *value, u32 reg, BOOL read) {
    u32 stub[TRK_STUB_WORDS] = {PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP,
                                PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP};

    if (read) {
        stub[0] = PPC_PSQ_ST_0_R3(reg);
    } else {
        stub[0] = PPC_PSQ_L_0_R3(reg);
    }
    return TRKPPCAccessSpecialReg(value, stub, read);
}

static inline TRKResult TRKPPCAccessFPRegister(void *value, u32 reg, BOOL read) {
    TRKResult result = kTRKOk;
    u32 stub[TRK_STUB_WORDS] = {PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP,
                                PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP, PPC_NOP};

    if (reg < 32) {
        if (read) {
            stub[0] = PPC_STFD_0_R3(reg);
        } else {
            stub[0] = PPC_LFD_0_R3(reg);
        }
        result = TRKPPCAccessSpecialReg(value, stub, read);
    } else if (reg == 32) {
        if (read) {
            ReadFPSCR((f64 *)value);
        } else {
            WriteFPSCR((f64 *)value);
        }
        *(u64 *)value &= 0xFFFFFFFF;
    } else if (reg == 33) {
        if (!read) {
            *(u32 *)value = *((u32 *)value + 1);
        }
        result = TRKPPCAccessSPR(value, SPR_FPECR, read);
        if (read) {
            *(u64 *)value = *(u32 *)value & 0xFFFFFFFFULL;
        }
    }
    return result;
}

TRKResult TRKTargetAccessMemory(void *data, u32 address, TRKSize *length, int validation, BOOL read) {
    TRKResult result;
    TRKExceptionStatus saved;
    void *translated;
    u32 nubMSR;
    u32 targetMSR;

    saved = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;
    translated = (void *)TRKTargetTranslate(address);
    result = TRKValidMemory32(translated, *length, read ? 0 : 1);
    if (result != kTRKOk) {
        *length = 0;
    } else {
        nubMSR = __TRK_get_MSR();
        targetMSR = nubMSR | (gTRKCPUState.extended1[TRK_EXT1_MSR] & MSR_DR);
        if (read) {
            TRK_ppc_memcpy(data, translated, *length, nubMSR, targetMSR);
        } else {
            TRK_ppc_memcpy(translated, data, *length, targetMSR, nubMSR);
            TRK_flush_cache((u32)translated, *length);
            if ((void *)address != translated) {
                TRK_flush_cache(address, *length);
            }
        }
    }
    if (gTRKExceptionStatus.exceptionDetected) {
        *length = 0;
        result = kTRKCWDSException;
    }
    gTRKExceptionStatus = saved;
    return result;
}

TRKResult TRKTargetReadInstruction(void *data, u32 address) {
    TRKResult result;
    TRKSize length = 4;

    result = TRKTargetAccessMemory(data, address, &length, 0, TRUE);
    if (result == kTRKOk && length != 4) {
        result = kTRKInvalidMemory;
    }
    return result;
}

TRKResult TRKTargetAccessDefault(u32 first, u32 last, TRKBuffer *buffer, TRKSize *registerBytes,
                                 BOOL read) {
    TRKResult result;
    u32 count;
    u32 *registers;
    TRKExceptionStatus saved;

    if (last > 0x24) {
        return kTRKInvalidRegister;
    }
    saved = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;
    registers = gTRKCPUState.general.gpr + first;
    count = last - first + 1;
    *registerBytes = count * sizeof(u32);
    if (read) {
        result = TRKAppendBuffer_ui32(buffer, registers, count);
    } else {
        result = TRKReadBuffer_ui32(buffer, registers, count);
    }
    if (gTRKExceptionStatus.exceptionDetected) {
        *registerBytes = 0;
        result = kTRKCWDSException;
    }
    gTRKExceptionStatus = saved;
    return result;
}

TRKResult TRKTargetAccessFP(u32 first, u32 last, TRKBuffer *buffer, TRKSize *registerBytes,
                            BOOL read) {
    TRKRegister64 value;
    TRKResult result;
    u32 reg;
    TRKExceptionStatus saved;

    if (last > 0x21) {
        return kTRKInvalidRegister;
    }
    saved = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;
    __TRK_set_MSR(__TRK_get_MSR() | MSR_FP);
    *registerBytes = 0;
    result = kTRKOk;
    for (reg = first; reg <= last && result == kTRKOk; reg++, *registerBytes += sizeof(u64)) {
        if (read) {
            TRKPPCAccessFPRegister(&value, reg, read);
            result = TRKAppendBuffer1_ui64(buffer, *(u64 *)&value);
        } else {
            TRKReadBuffer1_ui64(buffer, (u64 *)&value);
            result = TRKPPCAccessFPRegister(&value, reg, read);
        }
    }
    if (gTRKExceptionStatus.exceptionDetected) {
        *registerBytes = 0;
        result = kTRKCWDSException;
    }
    gTRKExceptionStatus = saved;
    return result;
}

TRKResult TRKTargetAccessExtended1(u32 first, u32 last, TRKBuffer *buffer, TRKSize *registerBytes,
                                   BOOL read) {
    TRKResult result;
    int count;
    u32 *registers;
    TRKExceptionStatus saved;

    if (last > 0x60) {
        return kTRKInvalidRegister;
    }
    saved = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;
    *registerBytes = 0;
    if (first <= last) {
        registers = (u32 *)&gTRKCPUState.extended1 + first;
        count = last - first + 1;
        *registerBytes += count * sizeof(u32);
        if (read) {
            result = TRKAppendBuffer_ui32(buffer, registers, count);
        } else {
            if (registers <= &gTRKCPUState.extended1[TRK_EXT1_TBU] &&
                registers + count - 1 >= &gTRKCPUState.extended1[TRK_EXT1_TBL]) {
                gTRKRestoreFlags.timeBase = TRUE;
            }
            if (registers <= &gTRKCPUState.extended1[TRK_EXT1_HID2] &&
                registers + count - 1 >= &gTRKCPUState.extended1[TRK_EXT1_HID2]) {
                gTRKRestoreFlags.hid2 = TRUE;
            }
            result = TRKReadBuffer_ui32(buffer, registers, count);
        }
    }
    if (gTRKExceptionStatus.exceptionDetected) {
        *registerBytes = 0;
        result = kTRKCWDSException;
    }
    gTRKExceptionStatus = saved;
    return result;
}

TRKResult TRKTargetAccessExtended2(u32 first, u32 last, TRKBuffer *buffer, TRKSize *registerBytes,
                                   BOOL read) {
    u32 reg;
    TRKExceptionStatus saved;
    TRKRegister64 value;
    u32 hid2;
    TRKResult result;

    if (last > 0x1F) {
        return kTRKInvalidRegister;
    }
    saved = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;
    /* Paired singles on, and a quantisation register that leaves the values alone. */
    TRKPPCAccessSPR(&hid2, SPR_HID2, TRUE);
    hid2 |= 0xA0000000;
    TRKPPCAccessSPR(&hid2, SPR_HID2, FALSE);
    hid2 = 0;
    TRKPPCAccessSPR(&hid2, SPR_GQR0, FALSE);
    *registerBytes = 0;
    result = kTRKOk;
    for (reg = first; reg <= last && result == kTRKOk; *registerBytes += sizeof(u64), reg++) {
        if (read) {
            TRKPPCAccessPairedSingleRegister(&value, reg, read);
            result = TRKAppendBuffer1_ui64(buffer, *(u64 *)&value);
        } else {
            TRKReadBuffer1_ui64(buffer, (u64 *)&value);
            result = TRKPPCAccessPairedSingleRegister(&value, reg, read);
        }
    }
    if (gTRKExceptionStatus.exceptionDetected) {
        *registerBytes = 0;
        result = kTRKCWDSException;
    }
    gTRKExceptionStatus = saved;
    return result;
}

static inline void TRKTargetDoStep(void) {
    gTRKStepStatus.active = TRUE;
    MWTRACE(1, "TargetDoStep()\n");
    gTRKCPUState.extended1[TRK_EXT1_MSR] |= MSR_SE;
    if (gTRKStepStatus.type == kTRKStepIntoCount || gTRKStepStatus.type == kTRKStepOverCount) {
        gTRKStepStatus.count--;
    }
    TRKTargetSetStopped(FALSE);
}

static inline BOOL TRKTargetStepDone(void) {
    BOOL done = TRUE;

    if (gTRKStepStatus.active && (u16)gTRKCPUState.extended1[TRK_EXT1_EXCEPTION] == 0xD00) {
        switch (gTRKStepStatus.type) {
        case kTRKStepIntoCount:
            if (gTRKStepStatus.count > 0) {
                done = FALSE;
            }
            break;
        case kTRKStepIntoRange:
            if (gTRKCPUState.general.pc >= gTRKStepStatus.rangeStart &&
                gTRKCPUState.general.pc <= gTRKStepStatus.rangeEnd) {
                done = FALSE;
            }
            break;
        }
    }
    return done;
}

static inline BOOL TRKTargetCheckStep(void) {
    if (gTRKStepStatus.active) {
        gTRKCPUState.extended1[TRK_EXT1_MSR] &= ~MSR_SE;
        if (TRKTargetStepDone()) {
            gTRKStepStatus.active = FALSE;
        } else {
            TRKTargetDoStep();
        }
    }
    return gTRKStepStatus.active;
}

/* Called in the nub, on its own stack, after the target stopped: queues the event for the main
 * loop. A trap that always fires is a request from the target (targsupp). */
void TRKPostInterruptEvent(void) {
    TRKEvent event;
    int type;
    u32 instruction;

    if (gTRKState.inputActivated) {
        gTRKState.inputActivated = FALSE;
    } else {
        switch ((u16)gTRKCPUState.extended1[TRK_EXT1_EXCEPTION]) {
        case 0x700:
        case 0xD00:
            TRKTargetReadInstruction(&instruction, gTRKCPUState.general.pc);
            if (instruction == 0x0FE00000) {
                type = kTRKEventSupport;
            } else {
                type = kTRKEventBreakpoint;
            }
            break;
        default:
            type = kTRKEventException;
            break;
        }
        TRKConstructEvent(&event, type);
        TRKPostEvent(&event);
    }
}

TRKResult TRKTargetInterrupt(TRKEvent *event) {
    TRKResult result = kTRKOk;

    switch (event->type) {
    case kTRKEventBreakpoint:
    case kTRKEventException:
        if (!TRKTargetCheckStep()) {
            TRKTargetSetStopped(TRUE);
            result = TRKDoNotifyStopped(kTRKCmdNotifyStopped);
        }
        break;
    }
    return result;
}

void TRKTargetAddStopInfo(TRKBuffer *buffer) {
    TRKNotifyMessage message;
    u32 instruction;

    memset(&message, 0, sizeof(message));
    message.size = sizeof(message);
    message.command = kTRKCmdNotifyStopped;
    message.pc = gTRKCPUState.general.pc;
    TRKTargetReadInstruction(&instruction, gTRKCPUState.general.pc);
    message.instruction = instruction;
    message.exception = (u16)gTRKCPUState.extended1[TRK_EXT1_EXCEPTION];
    TRKAppendBuffer_ui8(buffer, (u8 *)&message, sizeof(message));
}

void TRKTargetAddExceptionInfo(TRKBuffer *buffer) {
    TRKNotifyMessage message;
    u32 instruction;

    memset(&message, 0, sizeof(message));
    message.size = sizeof(message);
    message.command = kTRKCmdNotifyException;
    message.pc = gTRKExceptionStatus.where.pc;
    TRKTargetReadInstruction(&instruction, gTRKExceptionStatus.where.pc);
    message.instruction = instruction;
    message.exception = gTRKExceptionStatus.where.exception;
    TRKAppendBuffer_ui8(buffer, (u8 *)&message, sizeof(message));
}

TRKResult TRKTargetSingleStep(u32 count, BOOL stepOver) {
    if (stepOver) {
        return kTRKUnsupported;
    }
    gTRKStepStatus.count = count;
    gTRKStepStatus.type = kTRKStepIntoCount;
    TRKTargetDoStep();
    return kTRKOk;
}

TRKResult TRKTargetStepOutOfRange(u32 rangeStart, u32 rangeEnd, BOOL stepOver) {
    if (stepOver) {
        return kTRKUnsupported;
    }
    gTRKStepStatus.rangeStart = rangeStart;
    gTRKStepStatus.rangeEnd = rangeEnd;
    gTRKStepStatus.type = kTRKStepIntoRange;
    TRKTargetDoStep();
    return kTRKOk;
}

u32 TRKTargetGetPC(void) {
    return gTRKCPUState.general.pc;
}

/* Serves a request the target made by trapping: the command is in its r3, the arguments in
 * r4-r6, and the I/O result goes back in r3. */
TRKResult TRKTargetSupportRequest(void) {
    TRKResult result;
    TRKSize *length;
    int command;
    TRKEvent event;
    int ioResult;
    u32 position;

    command = gTRKCPUState.general.gpr[3];
    if (command != kTRKCmdReadFile && command != kTRKCmdWriteFile && command != kTRKCmdOpenFile &&
        command != kTRKCmdCloseFile && command != kTRKCmdPositionFile) {
        TRKConstructEvent(&event, kTRKEventException);
        TRKPostEvent(&event);
        return kTRKOk;
    }
    if (command == kTRKCmdOpenFile) {
        result = HandleOpenFileSupportRequest((const char *)gTRKCPUState.general.gpr[4],
                                              gTRKCPUState.general.gpr[5],
                                              (u32 *)gTRKCPUState.general.gpr[6], &ioResult);
        if (ioResult == 0 && result != kTRKOk) {
            ioResult = 1;
        }
        gTRKCPUState.general.gpr[3] = ioResult;
    } else if (command == kTRKCmdCloseFile) {
        result = HandleCloseFileSupportRequest(gTRKCPUState.general.gpr[4], &ioResult);
        if (ioResult == 0 && result != kTRKOk) {
            ioResult = 1;
        }
        gTRKCPUState.general.gpr[3] = ioResult;
    } else if (command == kTRKCmdPositionFile) {
        position = *(u32 *)gTRKCPUState.general.gpr[5];
        result = HandlePositionFileSupportRequest(gTRKCPUState.general.gpr[4], &position,
                                                  gTRKCPUState.general.gpr[6], &ioResult);
        if (ioResult == 0 && result != kTRKOk) {
            ioResult = 1;
        }
        gTRKCPUState.general.gpr[3] = ioResult;
        *(u32 *)gTRKCPUState.general.gpr[5] = position;
    } else {
        length = (TRKSize *)gTRKCPUState.general.gpr[5];
        result = TRKSuppAccessFile(gTRKCPUState.general.gpr[4], (u8 *)gTRKCPUState.general.gpr[6],
                                   length, &ioResult, TRUE, command == kTRKCmdReadFile);
        if (ioResult == 0 && result != kTRKOk) {
            ioResult = 1;
        }
        gTRKCPUState.general.gpr[3] = ioResult;
        if (command == kTRKCmdReadFile) {
            TRK_flush_cache(gTRKCPUState.general.gpr[6], *length);
        }
    }
    gTRKCPUState.general.pc += 4;
    return result;
}

BOOL TRKTargetStopped(void) {
    return gTRKState.stopped;
}

void TRKTargetSetStopped(BOOL stopped) {
    gTRKState.stopped = stopped;
}

TRKResult TRKTargetStop(void) {
    gTRKState.stopped = TRUE;
    return kTRKOk;
}

void TRKTargetSetInputPendingPtr(void *ptr) {
    gTRKState.inputPendingPtr = ptr;
}

TRKResult TRKTargetAccessARAM(void *data, u32 address, TRKSize *length, BOOL read) {
    TRKResult result = kTRKOk;
    TRKExceptionStatus saved;

    saved = gTRKExceptionStatus;
    gTRKExceptionStatus.exceptionDetected = FALSE;
    if (read) {
        __TRK_read_aram(data, address, length);
    } else {
        __TRK_write_aram(data, address, length);
    }
    if (gTRKExceptionStatus.exceptionDetected) {
        *length = 0;
        result = kTRKCWDSException;
    }
    gTRKExceptionStatus = saved;
    return result;
}

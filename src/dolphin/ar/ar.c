/* Dolphin SDK AR: the auxiliary RAM (ARAM) behind the DSP.
 *
 * ARAM is reached only by DMA through the DSP interface registers (0xCC005020-0xCC00502A). The
 * library keeps a simple stack allocator over it: ARAlloc hands out blocks from a moving head and
 * records each length on a caller-supplied stack, ARFree pops the newest one. ARInit also sizes the
 * memory: __ARChecksize probes for an expansion bank above the 16 MB internal ARAM by writing a
 * pattern at the internal size plus 2, 4, 8, 16 and 32 MB and checking where it aliases back to the
 * start of the expansion. */
#include <dolphin/os.h>
#include <dolphin/os_interrupt.h>
#include <dolphin/base/PPCArch.h>

void *memset(void *dst, int value, unsigned long n);

/* The RenderWare units call these two by their placeholder symbols. */
#define ARAlloc fn_80381A78
#define ARFree fn_80381AE0

volatile u16 __DSPRegs[] : 0xCC005000;

typedef void (*ARCallback)(void);

#define AR_DMA_BUSY       0x200
#define DSPCR_AIDINT      0x08
#define DSPCR_ARINT       0x20
#define DSPCR_DSPINT      0x80
#define AR_DMA_READ       0x8000 /* direction bit: ARAM to main memory */
#define AR_INTERNAL_SIZE  0x1000000
#define AR_USER_BASE      0x4000 /* the first 16 KB belong to the DSP */

#define ROUND_UP_32(x) (((u32)(x) + 31) & ~31)

/* The simulated-memory-size word the OS keeps in low memory (uncached). */
#define OS_ARAM_SIZE (*(u32 *)0xC00000D0)

const char *__ARVersion = "<< Dolphin SDK - AR\trelease build: Apr  5 2004 04:15:03 (0x2301) >>";

static ARCallback __AR_Callback;
static u32 __AR_Size;
static u32 __AR_InternalSize;
static u32 __AR_ExpansionSize;
static u32 __AR_StackPointer;
static u32 __AR_FreeBlocks;
static u32 *__AR_BlockLength;
static volatile BOOL __AR_init_flag;

static void __ARHandler(__OSInterrupt interrupt, OSContext *context);
static void __ARChecksize(void);

ARCallback ARRegisterDMACallback(ARCallback callback) {
    ARCallback previous = __AR_Callback;
    BOOL enabled = OSDisableInterrupts();
    __AR_Callback = callback;
    OSRestoreInterrupts(enabled);
    return previous;
}

u32 ARGetDMAStatus(void) {
    BOOL enabled = OSDisableInterrupts();
    u32 busy = __DSPRegs[5] & AR_DMA_BUSY;
    OSRestoreInterrupts(enabled);
    return busy;
}

/* `type` 0 copies main memory to ARAM, 1 the other way. Addresses and length are 32-byte units. */
void ARStartDMA(u32 type, u32 mainmem, u32 aram, u32 length) {
    BOOL enabled = OSDisableInterrupts();
    __DSPRegs[16] = (u16)((__DSPRegs[16] & ~0x3FF) | (u16)(mainmem >> 16));
    __DSPRegs[17] = (u16)((__DSPRegs[17] & ~0xFFE0) | (u16)(mainmem & 0xFFFF));
    __DSPRegs[18] = (u16)((__DSPRegs[18] & ~0x3FF) | (u16)(aram >> 16));
    __DSPRegs[19] = (u16)((__DSPRegs[19] & ~0xFFE0) | (u16)(aram & 0xFFFF));
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~AR_DMA_READ) | (type << 15));
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~0x3FF) | (u16)(length >> 16));
    __DSPRegs[21] = (u16)((__DSPRegs[21] & ~0xFFE0) | (u16)(length & 0xFFFF));
    OSRestoreInterrupts(enabled);
}

u32 ARAlloc(u32 length) {
    u32 block;
    BOOL enabled = OSDisableInterrupts();

    block = __AR_StackPointer;
    __AR_StackPointer += length;
    *__AR_BlockLength = length;
    __AR_BlockLength++;
    __AR_FreeBlocks--;
    OSRestoreInterrupts(enabled);
    return block;
}

/* Pops the newest block; its length goes to `length` when that is non-null. */
u32 ARFree(u32 *length) {
    BOOL enabled = OSDisableInterrupts();

    __AR_BlockLength--;
    if (length) {
        *length = *__AR_BlockLength;
    }
    __AR_StackPointer -= *__AR_BlockLength;
    __AR_FreeBlocks++;
    OSRestoreInterrupts(enabled);
    return __AR_StackPointer;
}

/* `stack` has room for `entries` block lengths. Returns the first ARAM address free for users. */
u32 ARInit(u32 *stack, u32 entries) {
    BOOL enabled;

    if (__AR_init_flag == TRUE) {
        return AR_USER_BASE;
    }
    OSRegisterVersion(__ARVersion);
    enabled = OSDisableInterrupts();
    __AR_Callback = NULL;
    __OSSetInterruptHandler(6, __ARHandler);
    __OSUnmaskInterrupts(0x02000000);
    __AR_FreeBlocks = entries;
    __AR_StackPointer = AR_USER_BASE;
    __AR_BlockLength = stack;
    __DSPRegs[13] = (u16)((__DSPRegs[13] & 0xFF) | (__DSPRegs[13] & ~0xFF));
    __ARChecksize();
    __AR_init_flag = TRUE;
    OSRestoreInterrupts(enabled);
    return __AR_StackPointer;
}

static void __ARHandler(__OSInterrupt interrupt, OSContext *context) {
    OSContext exceptionContext;
    u16 status = __DSPRegs[5];

    status = (u16)((status & ~(DSPCR_DSPINT | DSPCR_AIDINT)) | DSPCR_ARINT);
    __DSPRegs[5] = status;
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(&exceptionContext);
    if (__AR_Callback) {
        __AR_Callback();
    }
    OSClearContext(&exceptionContext);
    OSSetCurrentContext(context);
}

void __ARClearInterrupt(void) {
    u16 status = __DSPRegs[5];

    status = (u16)((status & ~(DSPCR_DSPINT | DSPCR_AIDINT)) | DSPCR_ARINT);
    __DSPRegs[5] = status;
}

u16 __ARGetInterruptStatus(void) { return (u16)(__DSPRegs[5] & DSPCR_ARINT); }

static inline void __ARWaitForDMA(void) {
    while (__DSPRegs[5] & AR_DMA_BUSY) {
    }
}

static inline void __ARWriteDMA(u32 mainmem, u32 aram, u32 length) {
    __DSPRegs[16] = (u16)((__DSPRegs[16] & ~0x3FF) | (u16)(mainmem >> 16));
    __DSPRegs[17] = (u16)((__DSPRegs[17] & ~0xFFE0) | (u16)(mainmem & 0xFFFF));
    __DSPRegs[18] = (u16)((__DSPRegs[18] & ~0x3FF) | (u16)(aram >> 16));
    __DSPRegs[19] = (u16)((__DSPRegs[19] & ~0xFFE0) | (u16)(aram & 0xFFFF));
    __DSPRegs[20] = (u16)(__DSPRegs[20] & ~AR_DMA_READ);
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~0x3FF) | (u16)(length >> 16));
    __DSPRegs[21] = (u16)((__DSPRegs[21] & ~0xFFE0) | (u16)(length & 0xFFFF));
    __ARWaitForDMA();
    __ARClearInterrupt();
}

static inline void __ARReadDMA(u32 mainmem, u32 aram, u32 length) {
    __DSPRegs[16] = (u16)((__DSPRegs[16] & ~0x3FF) | (u16)(mainmem >> 16));
    __DSPRegs[17] = (u16)((__DSPRegs[17] & ~0xFFE0) | (u16)(mainmem & 0xFFFF));
    __DSPRegs[18] = (u16)((__DSPRegs[18] & ~0x3FF) | (u16)(aram >> 16));
    __DSPRegs[19] = (u16)((__DSPRegs[19] & ~0xFFE0) | (u16)(aram & 0xFFFF));
    __DSPRegs[20] = (u16)(__DSPRegs[20] | AR_DMA_READ);
    __DSPRegs[20] = (u16)((__DSPRegs[20] & ~0x3FF) | (u16)(length >> 16));
    __DSPRegs[21] = (u16)((__DSPRegs[21] & ~0xFFE0) | (u16)(length & 0xFFFF));
    __ARWaitForDMA();
    __ARClearInterrupt();
}

/* Every probe saves the 32 bytes it is about to overwrite and puts them back afterwards, so the
 * DSP's area and anything already in ARAM survive. */
static void __ARChecksize(void) {
    u8 test_data_pad[0x20 + 31];
    u8 dummy_data_pad[0x20 + 31];
    u8 buffer_pad[0x20 + 31];
    u8 save_pad_1[0x20 + 31];
    u8 save_pad_2[0x20 + 31];
    u8 save_pad_3[0x20 + 31];
    u8 save_pad_4[0x20 + 31];
    u8 save_pad_5[0x20 + 31];
    u32 *test_data;
    u32 *dummy_data;
    u32 *buffer;
    u32 *save1;
    u32 *save2;
    u32 *save3;
    u32 *save4;
    u32 *save5;
    u16 aramMode;
    u32 aramSize;
    u32 i;

    while (!(__DSPRegs[11] & 1)) {
    }
    aramMode = 3;
    aramSize = __AR_InternalSize = AR_INTERNAL_SIZE;
    __DSPRegs[9] = (u16)((__DSPRegs[9] & ~0x3F) | 0x23);

    test_data = (u32 *)ROUND_UP_32(test_data_pad);
    dummy_data = (u32 *)ROUND_UP_32(dummy_data_pad);
    buffer = (u32 *)ROUND_UP_32(buffer_pad);
    save1 = (u32 *)ROUND_UP_32(save_pad_1);
    save2 = (u32 *)ROUND_UP_32(save_pad_2);
    save3 = (u32 *)ROUND_UP_32(save_pad_3);
    save4 = (u32 *)ROUND_UP_32(save_pad_4);
    save5 = (u32 *)ROUND_UP_32(save_pad_5);

    for (i = 0; i < 8; i++) {
        test_data[i] = 0xDEADBEEF;
        dummy_data[i] = 0xBAD0BAD0;
    }
    DCFlushRange(test_data, 0x20);
    DCFlushRange(dummy_data, 0x20);
    __AR_ExpansionSize = 0;

    DCInvalidateRange(save1, 0x20);
    __ARReadDMA((u32)save1, aramSize + 0, 0x20);
    PPCSync();
    __ARWriteDMA((u32)test_data, aramSize + 0, 0x20);
    memset(buffer, 0, 0x20);
    DCFlushRange(buffer, 0x20);
    __ARReadDMA((u32)buffer, aramSize + 0, 0x20);
    PPCSync();

    if (buffer[0] == test_data[0]) {
        DCInvalidateRange(save2, 0x20);
        __ARReadDMA((u32)save2, aramSize + 0x200000, 0x20);
        PPCSync();
        DCInvalidateRange(save3, 0x20);
        __ARReadDMA((u32)save3, aramSize + 0x1000000, 0x20);
        PPCSync();
        DCInvalidateRange(save4, 0x20);
        __ARReadDMA((u32)save4, aramSize + 0x200, 0x20);
        PPCSync();
        DCInvalidateRange(save5, 0x20);
        __ARReadDMA((u32)save5, aramSize + 0x400000, 0x20);
        PPCSync();

        __ARWriteDMA((u32)dummy_data, aramSize + 0x200000, 0x20);
        __ARWriteDMA((u32)test_data, aramSize + 0, 0x20);
        memset(buffer, 0, 0x20);
        DCFlushRange(buffer, 0x20);
        __ARReadDMA((u32)buffer, aramSize + 0x200000, 0x20);
        PPCSync();

        if (buffer[0] == test_data[0]) {
            __ARWriteDMA((u32)save1, aramSize + 0, 0x20);
            aramMode |= 0 << 3;
            aramSize += 0x200000;
            __AR_ExpansionSize = 0x200000;
        } else {
            __ARWriteDMA((u32)dummy_data, aramSize + 0x1000000, 0x20);
            __ARWriteDMA((u32)test_data, aramSize + 0, 0x20);
            memset(buffer, 0, 0x20);
            DCFlushRange(buffer, 0x20);
            __ARReadDMA((u32)buffer, aramSize + 0x1000000, 0x20);
            PPCSync();

            if (buffer[0] == test_data[0]) {
                __ARWriteDMA((u32)save1, aramSize + 0, 0x20);
                __ARWriteDMA((u32)save2, aramSize + 0x200000, 0x20);
                aramMode |= 1 << 3;
                aramSize += 0x400000;
                __AR_ExpansionSize = 0x400000;
            } else {
                __ARWriteDMA((u32)dummy_data, aramSize + 0x200, 0x20);
                __ARWriteDMA((u32)test_data, aramSize + 0, 0x20);
                memset(buffer, 0, 0x20);
                DCFlushRange(buffer, 0x20);
                __ARReadDMA((u32)buffer, aramSize + 0x200, 0x20);
                PPCSync();

                if (buffer[0] == test_data[0]) {
                    __ARWriteDMA((u32)save1, aramSize + 0, 0x20);
                    __ARWriteDMA((u32)save2, aramSize + 0x200000, 0x20);
                    __ARWriteDMA((u32)save3, aramSize + 0x1000000, 0x20);
                    aramMode |= 2 << 3;
                    aramSize += 0x800000;
                    __AR_ExpansionSize = 0x800000;
                } else {
                    __ARWriteDMA((u32)dummy_data, aramSize + 0x400000, 0x20);
                    __ARWriteDMA((u32)test_data, aramSize + 0, 0x20);
                    memset(buffer, 0, 0x20);
                    DCFlushRange(buffer, 0x20);
                    __ARReadDMA((u32)buffer, aramSize + 0x400000, 0x20);
                    PPCSync();

                    if (buffer[0] == test_data[0]) {
                        __ARWriteDMA((u32)save1, aramSize + 0, 0x20);
                        __ARWriteDMA((u32)save2, aramSize + 0x200000, 0x20);
                        __ARWriteDMA((u32)save3, aramSize + 0x1000000, 0x20);
                        __ARWriteDMA((u32)save4, aramSize + 0x200, 0x20);
                        aramMode |= 3 << 3;
                        aramSize += 0x1000000;
                        __AR_ExpansionSize = 0x1000000;
                    } else {
                        __ARWriteDMA((u32)save1, aramSize + 0, 0x20);
                        __ARWriteDMA((u32)save2, aramSize + 0x200000, 0x20);
                        __ARWriteDMA((u32)save3, aramSize + 0x1000000, 0x20);
                        __ARWriteDMA((u32)save4, aramSize + 0x200, 0x20);
                        __ARWriteDMA((u32)save5, aramSize + 0x400000, 0x20);
                        aramMode |= 4 << 3;
                        aramSize += 0x2000000;
                        __AR_ExpansionSize = 0x2000000;
                    }
                }
            }
        }
        __DSPRegs[9] = (u16)((__DSPRegs[9] & ~0x3F) | aramMode);
    }

    OS_ARAM_SIZE = aramSize;
    __AR_Size = aramSize;
}

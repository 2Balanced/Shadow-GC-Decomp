/* Dolphin SDK OS: external interrupts.
 *
 * Holds the MSR[EE] on/off helpers, the table of per-interrupt handlers (kept in low memory at
 * 0x80003040), and the two interrupt masks in low memory (global at 0x800000C4, local at
 * 0x800000C8). A set bit in either mask blocks that interrupt; changing the masks reprograms the
 * enable bits of the owning device (MEM, DSP, AI, the three EXI channels, PI). The external
 * interrupt exception lands in __OSDispatchInterrupt, which picks the most urgent pending,
 * unblocked interrupt and runs its handler. OSGetInterruptMask/OSSetInterruptMask were never
 * called by the game and are dead-stripped. */
#include <dolphin/os.h>

void *memset(void *dst, int value, unsigned long n);

volatile u32 __PIRegs[] : 0xCC003000;
volatile u16 __MEMRegs[] : 0xCC004000;
volatile u16 __DSPRegs[] : 0xCC005000;
volatile u32 __EXIRegs[] : 0xCC006800;
volatile u32 __AIRegs[] : 0xCC006C00;

typedef s16 __OSInterrupt;
typedef u32 OSInterruptMask;
typedef void (*__OSInterruptHandler)(__OSInterrupt interrupt, OSContext *context);

/* Interrupt numbers, grouped by the device that raises them. */
enum {
    IRQ_MEM0, IRQ_MEM1, IRQ_MEM2, IRQ_MEM3, IRQ_MEMADDR,
    IRQ_DSP_AI, IRQ_DSP_ARAM, IRQ_DSP_DSP,
    IRQ_AI,
    IRQ_EXI0, IRQ_EXI0_TC, IRQ_EXI0_EXT,
    IRQ_EXI1, IRQ_EXI1_TC, IRQ_EXI1_EXT,
    IRQ_EXI2, IRQ_EXI2_TC,
    IRQ_CP, IRQ_PE_TOKEN, IRQ_PE_FINISH, IRQ_SI, IRQ_DI, IRQ_RSW, IRQ_PI_ERR, IRQ_VI,
    IRQ_DEBUG, IRQ_HSP,
    IRQ_COUNT = 32
};

/* Masks number interrupts from the top bit down, so __cntlzw of a mask is its lowest-numbered
 * interrupt. */
#define BIT_OF(irq) (0x80000000u >> (irq))

#define GROUP_MEM                                                                                  \
    (BIT_OF(IRQ_MEM0) | BIT_OF(IRQ_MEM1) | BIT_OF(IRQ_MEM2) | BIT_OF(IRQ_MEM3) |                   \
     BIT_OF(IRQ_MEMADDR))
#define GROUP_DSP (BIT_OF(IRQ_DSP_AI) | BIT_OF(IRQ_DSP_ARAM) | BIT_OF(IRQ_DSP_DSP))
#define GROUP_EXI0 (BIT_OF(IRQ_EXI0) | BIT_OF(IRQ_EXI0_TC) | BIT_OF(IRQ_EXI0_EXT))
#define GROUP_EXI1 (BIT_OF(IRQ_EXI1) | BIT_OF(IRQ_EXI1_TC) | BIT_OF(IRQ_EXI1_EXT))
#define GROUP_EXI2 (BIT_OF(IRQ_EXI2) | BIT_OF(IRQ_EXI2_TC))
#define GROUP_EXI (GROUP_EXI0 | GROUP_EXI1 | GROUP_EXI2)
#define GROUP_PE (BIT_OF(IRQ_PE_TOKEN) | BIT_OF(IRQ_PE_FINISH))
#define GROUP_PI                                                                                   \
    (BIT_OF(IRQ_CP) | GROUP_PE | BIT_OF(IRQ_SI) | BIT_OF(IRQ_DI) | BIT_OF(IRQ_RSW) |               \
     BIT_OF(IRQ_PI_ERR) | BIT_OF(IRQ_VI) | BIT_OF(IRQ_DEBUG) | BIT_OF(IRQ_HSP))
#define GROUP_ALL (GROUP_MEM | GROUP_DSP | BIT_OF(IRQ_AI) | GROUP_EXI | GROUP_PI)

#define GLOBAL_MASK (*(OSInterruptMask *)OSPhysicalToCached(0x00C4))
#define LOCAL_MASK (*(OSInterruptMask *)OSPhysicalToCached(0x00C8))

/* PI interrupt cause (reg 0) and enable (reg 1) bits, one per source. */
#define PI_ERROR 0x0001
#define PI_RSW 0x0002
#define PI_DI 0x0004
#define PI_SI 0x0008
#define PI_EXI 0x0010
#define PI_AI 0x0020
#define PI_DSP 0x0040
#define PI_MEM 0x0080
#define PI_VI 0x0100
#define PI_PE_TOKEN 0x0200
#define PI_PE_FINISH 0x0400
#define PI_CP 0x0800
#define PI_DEBUG 0x1000
#define PI_HSP 0x2000
#define PI_RESET_LINE 0x00010000 /* reset switch state, not an interrupt */
#define PI_ALWAYS_ON 0xF0        /* EXI/AI/DSP/MEM lines are gated in their own devices */

/* Device register indices. */
#define MEM_INT_ENABLE 14
#define MEM_INT_STATUS 15
#define DSP_CSR 5
#define EXI_CHANNEL(n) ((n) * 5) /* each EXI channel has five 32-bit registers */

/* Set `enable` in `on` unless `blocked` holds interrupt irq. */
#define ALLOW(irq, enable)                                                                         \
    if ((blocked & BIT_OF(irq)) == 0)                                                              \
    on |= (enable)
/* Mark interrupt irq pending when `flag` is set in `status`. */
#define PENDING(status, flag, irq)                                                                 \
    if ((status) & (flag))                                                                         \
    pending |= BIT_OF(irq)

static __OSInterruptHandler *InterruptHandlerTable;

volatile OSTime __OSLastInterruptTime;
volatile __OSInterrupt __OSLastInterrupt;
volatile u32 __OSLastInterruptSrr0;

/* The dispatcher scans these groups in order and serves the first one with something pending;
 * the last entry catches anything left. */
static OSInterruptMask InterruptPrioTable[] = {
    BIT_OF(IRQ_PI_ERR),
    BIT_OF(IRQ_DEBUG),
    GROUP_MEM,
    BIT_OF(IRQ_RSW),
    BIT_OF(IRQ_VI),
    GROUP_PE,
    BIT_OF(IRQ_HSP),
    BIT_OF(IRQ_DSP_ARAM) | BIT_OF(IRQ_DSP_DSP) | BIT_OF(IRQ_AI) | GROUP_EXI | BIT_OF(IRQ_SI) |
        BIT_OF(IRQ_DI),
    BIT_OF(IRQ_DSP_AI),
    BIT_OF(IRQ_CP),
    0xFFFFFFFF,
};

static void ExternalInterruptHandler(register __OSException exception, register OSContext *context);

asm BOOL OSDisableInterrupts(void) {
    nofralloc
    mfmsr r3
    rlwinm r4, r3, 0, 17, 15
    mtmsr r4
    rlwinm r3, r3, 17, 31, 31
    blr
}

asm BOOL OSEnableInterrupts(void) {
    nofralloc
    mfmsr r3
    ori r4, r3, 0x8000
    mtmsr r4
    rlwinm r3, r3, 17, 31, 31
    blr
}

asm BOOL OSRestoreInterrupts(register BOOL level) {
    nofralloc
    cmpwi level, 0
    mfmsr r4
    beq _disable
    ori r5, r4, 0x8000
    b _restore
_disable:
    rlwinm r5, r4, 0, 17, 15
_restore:
    mtmsr r5
    rlwinm r3, r4, 17, 31, 31
    blr
}

__OSInterruptHandler __OSSetInterruptHandler(__OSInterrupt id, __OSInterruptHandler fn) {
    __OSInterruptHandler *slot = &InterruptHandlerTable[id];
    __OSInterruptHandler previous = *slot;

    *slot = fn;
    return previous;
}

__OSInterruptHandler __OSGetInterruptHandler(__OSInterrupt id) { return InterruptHandlerTable[id]; }

OSInterruptMask __OSMaskInterrupts(OSInterruptMask add);

void __OSInterruptInit(void) {
    InterruptHandlerTable = OSPhysicalToCached(0x3040);
    memset(InterruptHandlerTable, 0, sizeof(__OSInterruptHandler) * IRQ_COUNT);

    LOCAL_MASK = GLOBAL_MASK = 0;
    __PIRegs[1] = PI_ALWAYS_ON;

    /* Start with everything blocked; drivers unmask what they use. */
    __OSMaskInterrupts(GROUP_ALL);
    __OSSetExceptionHandler(4, ExternalInterruptHandler);
}

/* Writes the enable register of the device owning the lowest-numbered interrupt in `todo`, with
 * every source of that device enabled unless `blocked` has it, and returns `todo` minus that
 * device's interrupts. Called until `todo` is empty. */
static u32 SetInterruptMask(OSInterruptMask todo, OSInterruptMask blocked) {
    u32 on;

    switch (__cntlzw(todo)) {
    case IRQ_MEM0:
    case IRQ_MEM1:
    case IRQ_MEM2:
    case IRQ_MEM3:
    case IRQ_MEMADDR:
        on = 0;
        ALLOW(IRQ_MEM0, 1 << 0);
        ALLOW(IRQ_MEM1, 1 << 1);
        ALLOW(IRQ_MEM2, 1 << 2);
        ALLOW(IRQ_MEM3, 1 << 3);
        ALLOW(IRQ_MEMADDR, 1 << 4);
        __MEMRegs[MEM_INT_ENABLE] = (u16)on;
        todo &= ~GROUP_MEM;
        break;

    case IRQ_DSP_AI:
    case IRQ_DSP_ARAM:
    case IRQ_DSP_DSP:
        /* Clear the three enables and the write-1-to-clear status bits next to them. */
        on = __DSPRegs[DSP_CSR];
        on &= ~0x1F8;
        ALLOW(IRQ_DSP_AI, 0x10);
        ALLOW(IRQ_DSP_ARAM, 0x40);
        ALLOW(IRQ_DSP_DSP, 0x100);
        __DSPRegs[DSP_CSR] = (u16)on;
        todo &= ~GROUP_DSP;
        break;

    case IRQ_AI:
        on = __AIRegs[0];
        on &= ~0x2C;
        ALLOW(IRQ_AI, 0x4);
        __AIRegs[0] = on;
        todo &= ~BIT_OF(IRQ_AI);
        break;

    case IRQ_EXI0:
    case IRQ_EXI0_TC:
    case IRQ_EXI0_EXT:
        /* Channel 0 also has bit 13 (ROM descrambler), which is cleared too. */
        on = __EXIRegs[EXI_CHANNEL(0)];
        on &= ~0x2C0F;
        ALLOW(IRQ_EXI0, 0x1);
        ALLOW(IRQ_EXI0_TC, 0x4);
        ALLOW(IRQ_EXI0_EXT, 0x400);
        __EXIRegs[EXI_CHANNEL(0)] = on;
        todo &= ~GROUP_EXI0;
        break;

    case IRQ_EXI1:
    case IRQ_EXI1_TC:
    case IRQ_EXI1_EXT:
        on = __EXIRegs[EXI_CHANNEL(1)];
        on &= ~0xC0F;
        ALLOW(IRQ_EXI1, 0x1);
        ALLOW(IRQ_EXI1_TC, 0x4);
        ALLOW(IRQ_EXI1_EXT, 0x400);
        __EXIRegs[EXI_CHANNEL(1)] = on;
        todo &= ~GROUP_EXI1;
        break;

    case IRQ_EXI2:
    case IRQ_EXI2_TC:
        on = __EXIRegs[EXI_CHANNEL(2)];
        on &= ~0xF;
        ALLOW(IRQ_EXI2, 0x1);
        ALLOW(IRQ_EXI2_TC, 0x4);
        __EXIRegs[EXI_CHANNEL(2)] = on;
        todo &= ~GROUP_EXI2;
        break;

    case IRQ_CP:
    case IRQ_SI:
    case IRQ_DI:
    case IRQ_RSW:
    case IRQ_PI_ERR:
    case IRQ_VI:
    case IRQ_DEBUG:
    case IRQ_PE_TOKEN:
    case IRQ_PE_FINISH:
    case IRQ_HSP:
        on = PI_ALWAYS_ON;
        ALLOW(IRQ_CP, PI_CP);
        ALLOW(IRQ_SI, PI_SI);
        ALLOW(IRQ_DI, PI_DI);
        ALLOW(IRQ_RSW, PI_RSW);
        ALLOW(IRQ_PI_ERR, PI_ERROR);
        ALLOW(IRQ_VI, PI_VI);
        ALLOW(IRQ_DEBUG, PI_DEBUG);
        ALLOW(IRQ_PE_TOKEN, PI_PE_TOKEN);
        ALLOW(IRQ_PE_FINISH, PI_PE_FINISH);
        ALLOW(IRQ_HSP, PI_HSP);
        __PIRegs[1] = on;
        todo &= ~GROUP_PI;
        break;

    default:
        break;
    }
    return todo;
}

/* Adds `add` to the global mask and blocks whatever was not blocked before. Returns the old
 * global mask. */
OSInterruptMask __OSMaskInterrupts(OSInterruptMask add) {
    BOOL level = OSDisableInterrupts();
    OSInterruptMask old = GLOBAL_MASK;
    OSInterruptMask local = LOCAL_MASK;
    OSInterruptMask todo = ~(old | local) & add;

    add |= old;
    GLOBAL_MASK = add;
    while (todo != 0)
        todo = SetInterruptMask(todo, add | local);

    OSRestoreInterrupts(level);
    return old;
}

/* Removes `remove` from the global mask and re-enables those that were blocked (unless the local
 * mask still blocks them). Returns the old global mask. */
OSInterruptMask __OSUnmaskInterrupts(OSInterruptMask remove) {
    BOOL level = OSDisableInterrupts();
    OSInterruptMask old = GLOBAL_MASK;
    OSInterruptMask local = LOCAL_MASK;
    OSInterruptMask todo = (old | local) & remove;

    remove = old & ~remove;
    GLOBAL_MASK = remove;
    while (todo != 0)
        todo = SetInterruptMask(todo, remove | local);

    OSRestoreInterrupts(level);
    return old;
}

/* Builds the set of pending interrupts from the PI cause register and each signalling device's
 * status, then runs the handler of the most urgent unblocked one with thread switching held off.
 * Always ends by resuming `context`. */
void __OSDispatchInterrupt(__OSException exception, OSContext *context) {
    u32 cause;
    u32 status;
    OSInterruptMask pending;
    OSInterruptMask ready;
    OSInterruptMask *group;
    __OSInterrupt id;
    __OSInterruptHandler fn;

    cause = __PIRegs[0];
    cause &= ~PI_RESET_LINE;
    if (cause == 0 || (cause & __PIRegs[1]) == 0)
        OSLoadContext(context); /* spurious */

    pending = 0;

    if (cause & PI_MEM) {
        status = __MEMRegs[MEM_INT_STATUS];
        PENDING(status, 1 << 0, IRQ_MEM0);
        PENDING(status, 1 << 1, IRQ_MEM1);
        PENDING(status, 1 << 2, IRQ_MEM2);
        PENDING(status, 1 << 3, IRQ_MEM3);
        PENDING(status, 1 << 4, IRQ_MEMADDR);
    }

    if (cause & PI_DSP) {
        status = __DSPRegs[DSP_CSR];
        PENDING(status, 0x08, IRQ_DSP_AI);
        PENDING(status, 0x20, IRQ_DSP_ARAM);
        PENDING(status, 0x80, IRQ_DSP_DSP);
    }

    if (cause & PI_AI) {
        status = __AIRegs[0];
        PENDING(status, 0x8, IRQ_AI);
    }

    if (cause & PI_EXI) {
        /* Status bits sit one above their enables: 0x2 EXI, 0x8 transfer, 0x800 attach. */
        status = __EXIRegs[EXI_CHANNEL(0)];
        PENDING(status, 0x2, IRQ_EXI0);
        PENDING(status, 0x8, IRQ_EXI0_TC);
        PENDING(status, 0x800, IRQ_EXI0_EXT);
        status = __EXIRegs[EXI_CHANNEL(1)];
        PENDING(status, 0x2, IRQ_EXI1);
        PENDING(status, 0x8, IRQ_EXI1_TC);
        PENDING(status, 0x800, IRQ_EXI1_EXT);
        status = __EXIRegs[EXI_CHANNEL(2)];
        PENDING(status, 0x2, IRQ_EXI2);
        PENDING(status, 0x8, IRQ_EXI2_TC);
    }

    PENDING(cause, PI_HSP, IRQ_HSP);
    PENDING(cause, PI_DEBUG, IRQ_DEBUG);
    PENDING(cause, PI_PE_FINISH, IRQ_PE_FINISH);
    PENDING(cause, PI_PE_TOKEN, IRQ_PE_TOKEN);
    PENDING(cause, PI_VI, IRQ_VI);
    PENDING(cause, PI_SI, IRQ_SI);
    PENDING(cause, PI_DI, IRQ_DI);
    PENDING(cause, PI_RSW, IRQ_RSW);
    PENDING(cause, PI_CP, IRQ_CP);
    PENDING(cause, PI_ERROR, IRQ_PI_ERR);

    ready = pending & ~(GLOBAL_MASK | LOCAL_MASK);
    if (ready != 0) {
        /* The table ends in an all-ones entry, so this always stops. */
        group = InterruptPrioTable;
        while (1) {
            if ((ready & *group) != 0) {
                id = (__OSInterrupt)__cntlzw(ready & *group);
                break;
            }
            group++;
        }

        fn = __OSGetInterruptHandler(id);
        if (fn != NULL) {
            /* Memory interface errors are not recorded. */
            if (id > IRQ_MEMADDR) {
                __OSLastInterrupt = id;
                __OSLastInterruptTime = OSGetTime();
                __OSLastInterruptSrr0 = context->srr0;
            }
            OSDisableScheduler();
            fn(id, context);
            OSEnableScheduler();
            __OSReschedule();
            OSLoadContext(context);
        }
    }

    OSLoadContext(context);
}

/* Entry for the external interrupt exception: the first-level handler saved part of the context;
 * save the rest (r0-r2, r6-r31, GQRs) and tail-jump into the dispatcher on a new frame. */
static asm void ExternalInterruptHandler(register __OSException exception,
                                         register OSContext *context) {
    nofralloc
    OS_EXCEPTION_SAVE_GPRS(context)
    stwu r1, -8(r1)
    b __OSDispatchInterrupt
}

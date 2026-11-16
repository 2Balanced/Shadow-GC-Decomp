/* Dolphin SDK OS: the program's entry point (the CodeWarrior EABI start-up file as the SDK ships
 * it). __start sets up the registers and the hardware, copies the initialized data and clears
 * .bss, starts the debugger nub if the boot information asks for one, relocates the argument
 * vector the debugger hands over, initializes the OS and calls main. Everything here lives in
 * .init. __start and __init_registers run without a usable stack frame and are hand-written
 * assembly in the original; they also switch the peephole pass off for __init_data, which keeps
 * its unscheduled prologue and redundant branches. */
#include <dolphin/types.h>

/* Low-memory globals the boot code and the debugger fill in. */
#define BOOTINFO2_ADDR 0x800000F4   /* debugger's boot information, or null */
#define ARENAHI_ADDR 0x80000034     /* top of the arena */
#define DEBUGFLAG_ADDR 0x800030E8   /* which debugger nub to start, when there is no bootinfo2 */
#define DVD_DEVICECODE_ADDR 0x800030E6
#define PAD3_BUTTON_ADDR 0x800030E4 /* buttons held on controller 4 at boot */
#define OS_BI2_DEBUGFLAG_OFFSET 0xC
#define OS_BI2_ARGOFFSET_OFFSET 0x8

/* One entry of the linker-built tables that end with a zero size: where a section's initial data
 * is in ROM and where it goes, and a section to clear. */
typedef struct RomCopyInfo {
    char *rom;
    char *addr;
    unsigned int size;
} RomCopyInfo;

typedef struct BssInitInfo {
    char *addr;
    unsigned int size;
} BssInitInfo;

extern RomCopyInfo _rom_copy_info[];
extern BssInitInfo _bss_init_info[];
extern char _stack_addr[];
extern char _SDA_BASE_[];
extern char _SDA2_BASE_[];

extern int main(int argc, char *argv[]);
extern void exit(int status);
extern void __init_user(void);
extern void InitMetroTRK(void);
extern void InitMetroTRK_BBA(void);
extern void OSInit(void);
extern void DBInit(void);
extern void OSResetSystem(int reset, u32 resetCode, int forceMenu);
extern void *memcpy(void *dst, const void *src, unsigned long n);
extern void *memset(void *dst, int value, unsigned long n);

__declspec(section ".init") extern void __init_hardware(void);
__declspec(section ".init") extern void __flush_cache(void *address, unsigned int size);

__declspec(section ".init") static void __check_pad3(void);
__declspec(section ".init") static void __set_debug_bba(void);
__declspec(section ".init") static u8 __get_debug_bba(void);
__declspec(section ".init") extern void __start(void);
__declspec(section ".init") static void __init_registers(void);
__declspec(section ".init") static void __init_data(void);

/* Whether the debugger nub is to use the broadband adapter. */
static u8 Debug_BBA = 0;

/* Resets the console if the reset combination is held on controller 4. */
static void __check_pad3(void) {
    if ((*(u16 *)PAD3_BUTTON_ADDR & 0x0EEF) == 0x0EEF) {
        OSResetSystem(0, 0, 0);
    }
}

static void __set_debug_bba(void) { Debug_BBA = 1; }

static u8 __get_debug_bba(void) { return Debug_BBA; }

__declspec(weak) asm void __start(void) {
    nofralloc
    bl __init_registers
    bl __init_hardware
    /* a terminating stack frame */
    li r0, -1
    stwu r1, -8(r1)
    stw r0, 4(r1)
    stw r0, 0(r1)
    bl __init_data
    li r0, 0
    lis r6, 0x80000044@ha
    addi r6, r6, 0x80000044@l
    stw r0, 0(r6)
    /* the debugger kind: from bootinfo2 if there is one, else from low memory if the arena top
     * is set */
    lis r6, BOOTINFO2_ADDR@ha
    addi r6, r6, BOOTINFO2_ADDR@l
    lwz r6, 0(r6)
    cmplwi r6, 0
    beq no_bootinfo2
    lwz r7, OS_BI2_DEBUGFLAG_OFFSET(r6)
    b check_debugger
no_bootinfo2:
    lis r5, ARENAHI_ADDR@ha
    addi r5, r5, ARENAHI_ADDR@l
    lwz r5, 0(r5)
    cmplwi r5, 0
    beq no_debugger
    lis r7, DEBUGFLAG_ADDR@ha
    addi r7, r7, DEBUGFLAG_ADDR@l
    lwz r7, 0(r7)
check_debugger:
    li r5, 0
    cmplwi r7, 2
    beq start_debugger
    cmplwi r7, 3
    li r5, 1
    beq start_debugger
    cmplwi r7, 4
    bne no_debugger
    li r5, 2
    bl __set_debug_bba
    b no_debugger
start_debugger:
    lis r6, InitMetroTRK@ha
    addi r6, r6, InitMetroTRK@l
    mtlr r6
    blrl
no_debugger:
    /* the argument vector: bootinfo2 holds its offset; its entries are offsets to relocate */
    lis r6, BOOTINFO2_ADDR@ha
    addi r6, r6, BOOTINFO2_ADDR@l
    lwz r5, 0(r6)
    cmplwi r5, 0
    beq+ no_args
    lwz r6, OS_BI2_ARGOFFSET_OFFSET(r5)
    cmplwi r6, 0
    beq+ no_args
    add r6, r5, r6
    lwz r14, 0(r6)
    cmplwi r14, 0
    beq no_args
    addi r15, r6, 4
    mtctr r14
relocate_arg:
    addi r6, r6, 4
    lwz r7, 0(r6)
    add r7, r7, r5
    stw r7, 0(r6)
    bdnz relocate_arg
    /* the arguments sit at the top of memory: lower the arena top below them */
    lis r5, ARENAHI_ADDR@ha
    addi r5, r5, ARENAHI_ADDR@l
    clrrwi r7, r15, 5
    stw r7, 0(r5)
    b have_args
no_args:
    li r14, 0
    li r15, 0
have_args:
    bl DBInit
    bl OSInit
    /* a development unit (or a device code of 1) checks controller 4 for the reset request */
    lis r4, DVD_DEVICECODE_ADDR@ha
    addi r4, r4, DVD_DEVICECODE_ADDR@l
    lhz r3, 0(r4)
    andi. r5, r3, 0x8000
    beq check_pad3
    andi. r3, r3, 0x7FFF
    cmplwi r3, 1
    bne skip_pad3
check_pad3:
    bl __check_pad3
skip_pad3:
    bl __get_debug_bba
    cmplwi r3, 1
    bne skip_bba
    bl InitMetroTRK_BBA
skip_bba:
    bl __init_user
    mr r3, r14
    mr r4, r15
    bl main
    b exit
}

/* Clears every general-purpose register and loads the stack pointer and the two small-data
 * bases. */
static asm void __init_registers(void) {
    nofralloc
    li r0, 0
    li r3, 0
    li r4, 0
    li r5, 0
    li r6, 0
    li r7, 0
    li r8, 0
    li r9, 0
    li r10, 0
    li r11, 0
    li r12, 0
    li r14, 0
    li r15, 0
    li r16, 0
    li r17, 0
    li r18, 0
    li r19, 0
    li r20, 0
    li r21, 0
    li r22, 0
    li r23, 0
    li r24, 0
    li r25, 0
    li r26, 0
    li r27, 0
    li r28, 0
    li r29, 0
    li r30, 0
    li r31, 0
    lis r1, _stack_addr@h
    ori r1, r1, _stack_addr@l
    lis r2, _SDA2_BASE_@h
    ori r2, r2, _SDA2_BASE_@l
    lis r13, _SDA_BASE_@h
    ori r13, r13, _SDA_BASE_@l
    blr
}

/* Copies a section's initial data from ROM, unless it is already in place. */
inline static void __copy_rom_section(void *dst, const void *src, unsigned long size) {
    if (size && (dst != src)) {
        memcpy(dst, src, size);
        __flush_cache(dst, size);
    }
}

inline static void __init_bss_section(void *dst, unsigned long size) {
    if (size) {
        memset(dst, 0, size);
    }
}

static void __init_data(void) {
    RomCopyInfo *dci;
    BssInitInfo *bii;

    dci = _rom_copy_info;
    while (1) {
        if (dci->size == 0) {
            break;
        }
        __copy_rom_section(dci->addr, dci->rom, dci->size);
        dci++;
    }

    bii = _bss_init_info;
    while (1) {
        if (bii->size == 0) {
            break;
        }
        __init_bss_section(bii->addr, bii->size);
        bii++;
    }
}

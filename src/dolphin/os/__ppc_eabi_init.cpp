/* Dolphin SDK OS: hooks called by the CodeWarrior EABI start-up code. __init_hardware and
 * __flush_cache are called from __start and __init_data before any stack frame or data is set
 * up, and live in .init with them. __init_user runs every static constructor of the program
 * before main; _ExitProcess is the end of the road for exit(). */
#include <dolphin/base/PPCArch.h>

extern "C" {

/* One entry of the linker-built .ctors table, which ends with a null entry. */
typedef void (*StaticCtor)(void);
extern StaticCtor _ctors[];

/* Dolphin SDK OS (OS.c, OSCache.c): paired-single, floating-point register and cache set-up. */
void __OSPSInit(void);
void __OSFPRInit(void);
void __OSCacheInit(void);

__declspec(section ".init") void __init_hardware(void);
__declspec(section ".init") void __flush_cache(void *address, unsigned int size);

/* Enables the floating-point unit (MSR[FP]), then sets up paired singles, the floating-point
 * registers and the caches. Called from __start with no stack, so the return address is kept in
 * r31 across the calls. Hand-written assembly in the original. */
asm void __init_hardware(void) {
    nofralloc
    mfmsr r0
    ori r0, r0, 0x2000
    mtmsr r0
    mflr r31
    bl __OSPSInit
    bl __OSFPRInit
    bl __OSCacheInit
    mtlr r31
    blr
}

/* Writes back the data cache blocks of [address, address + size) and invalidates the same blocks
 * of the instruction cache, eight bytes at a time; __init_data calls it on code it has copied.
 * Hand-written assembly in the original. */
asm void __flush_cache(register void *address, register unsigned int size) {
    nofralloc
    lis r5, 0xFFFF
    ori r5, r5, 0xFFF1
    and r5, r5, address
    subf address, r5, address
    add size, size, address
loop:
    dcbst 0, r5
    sync
    icbi 0, r5
    addic r5, r5, 8
    subic. size, size, 8
    bge loop
    isync
    blr
}

static void __init_cpp(void);

void __init_user(void) { __init_cpp(); }

/* The two assembly functions above switch the peephole pass off for the rest of the file, as in
 * retail: this one keeps its unscheduled prologue and the redundant branches ahead of the
 * loop. */
static void __init_cpp(void) {
    StaticCtor *entry = _ctors;

    while (*entry != 0) {
        (*entry)();
        ++entry;
    }
}

/* Nothing to return to on a console: park the CPU. */
void _ExitProcess(void) { PPCHalt(); }
}

/* Dolphin SDK OS: hooks called by the CodeWarrior EABI start-up code. __init_user runs every
 * static constructor of the program before main; _ExitProcess is the end of the road for exit(). */
#include <dolphin/base/PPCArch.h>

extern "C" {

/* One entry of the linker-built .ctors table, which ends with a null entry. */
typedef void (*StaticCtor)(void);
extern StaticCtor _ctors[];

static void __init_cpp(void);

void __init_user(void) { __init_cpp(); }

/* Retail built this one without the peephole pass: it keeps the unscheduled prologue and the
 * redundant branches ahead of the loop. */
#pragma peephole off
static void __init_cpp(void) {
    StaticCtor *entry = _ctors;

    while (*entry != 0) {
        (*entry)();
        ++entry;
    }
}
#pragma peephole reset

/* Nothing to return to on a console: park the CPU. */
void _ExitProcess(void) { PPCHalt(); }
}

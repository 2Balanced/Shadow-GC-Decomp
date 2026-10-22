/* Dolphin SDK OS: start-up hooks the CodeWarrior EABI start code calls. __init_user runs the
 * static constructors listed in .ctors (_ctors, provided by the linker); _ExitProcess is where
 * exit() ends up. */
#include <dolphin/base/PPCArch.h>

extern "C" {

typedef void (*voidfunctionptr)(void);
extern voidfunctionptr _ctors[];

static void __init_cpp(void);

void __init_user(void) {
    __init_cpp();
}

#pragma peephole off
static void __init_cpp(void) {
    voidfunctionptr *constructor;

    for (constructor = _ctors; *constructor; constructor++) {
        (*constructor)();
    }
}
#pragma peephole reset

void _ExitProcess(void) {
    PPCHalt();
}
}

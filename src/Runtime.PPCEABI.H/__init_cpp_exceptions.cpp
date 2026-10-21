/* Start-up and shut-down hooks of the CodeWarrior C++ runtime: register this module's exception
 * tables before static constructors run, and unregister them (after the global destructors) at
 * exit. The three pointer constants at the bottom are what the start-up code walks: .ctors runs
 * __init_cpp_exceptions first, .dtors runs __destroy_global_chain and then
 * __fini_cpp_exceptions. */
#include "Runtime.PPCEABI.H/NMWException.h"

#pragma exceptions off
#pragma internal on

extern "C" {

extern void __init_cpp_exceptions(void);
extern void __fini_cpp_exceptions(void);

/* Handle from __register_fragment; -2 while nothing is registered. */
static int s_fragmentId = -2;

/* r2 holds the small-data (TOC) base the unwinder needs. */
static inline char *ReadSmallDataBase(void) {
    register char *base;
    asm { mr base, r2 }
    return base;
}

void __fini_cpp_exceptions(void) {
    if (s_fragmentId == -2) {
        return;
    }
    __unregister_fragment(s_fragmentId);
    s_fragmentId = -2;
}

void __init_cpp_exceptions(void) {
    if (s_fragmentId != -2) {
        return;
    }
    s_fragmentId = __register_fragment(_eti_init_info, ReadSmallDataBase());
}

#pragma section ".ctors$10"
__declspec(section ".ctors$10") extern void *const __init_cpp_exceptions_reference = __init_cpp_exceptions;

#pragma section ".dtors$10"
__declspec(section ".dtors$10") extern void *const __destroy_global_chain_reference = __destroy_global_chain;

#pragma section ".dtors$15"
__declspec(section ".dtors$15") extern void *const __fini_cpp_exceptions_reference = __fini_cpp_exceptions;

}

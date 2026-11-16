/* Dolphin SDK amcstubs: the do-nothing version of the AMC debugger link on EXI channel 2. The
 * debugger's target code is given either this set or the real driver (odemuexi2) at link time
 * and asks AMC_IsStub which one it got.
 *
 * Only AMC_IsStub has a name in the binary; the macros give the others readable ones. */
#include <dolphin/exi.h>

#define Exi2Init fn_803B1FAC
#define Exi2EnableInterrupts fn_803B1FB0
#define Exi2Poll fn_803B1FB4
#define Exi2Read fn_803B1FBC
#define Exi2Write fn_803B1FC4
#define Exi2Reserve fn_803B1FCC
#define Exi2Unreserve fn_803B1FD0

void Exi2Init(volatile u8 **inputPendingPtr, EXICallback monitorCallback) {
}

void Exi2EnableInterrupts(void) {
}

int Exi2Poll(void) {
    return 0;
}

int Exi2Read(void *bytes, u32 length) {
    return 0;
}

int Exi2Write(const void *bytes, u32 length) {
    return 0;
}

void Exi2Reserve(void) {
}

void Exi2Unreserve(void) {
}

BOOL AMC_IsStub(void) {
    return TRUE;
}

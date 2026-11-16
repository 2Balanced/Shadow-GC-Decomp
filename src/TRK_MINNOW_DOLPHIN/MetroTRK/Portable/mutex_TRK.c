#include <TRK_MINNOW_DOLPHIN/trk.h>

/* The nub runs with interrupts off and has nothing to lock against: its mutexes are stubs. */

TRKResult TRKInitializeMutex(void *mutex) {
    return kTRKOk;
}

TRKResult TRKAcquireMutex(void *mutex) {
    return kTRKOk;
}

TRKResult TRKReleaseMutex(void *mutex) {
    return kTRKOk;
}

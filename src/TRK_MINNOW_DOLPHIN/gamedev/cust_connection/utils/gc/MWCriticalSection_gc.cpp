#include <TRK_MINNOW_DOLPHIN/CircleBuffer.h>

extern "C" {
u32 OSDisableInterrupts(void);
u32 OSRestoreInterrupts(u32 level);
}

void MWInitializeCriticalSection(MWCriticalSection *section) {
}

void MWEnterCriticalSection(MWCriticalSection *section) {
    section->interrupts = OSDisableInterrupts();
}

void MWExitCriticalSection(MWCriticalSection *section) {
    OSRestoreInterrupts(section->interrupts);
}

/* Dolphin SDK OS: the system-call exception handler. __OSInitSystemCall copies SystemCallVector
 * to the system-call vector at 0xC00; `sc` (PPCSync) then runs it, which flushes the bus by
 * toggling HID0[ABE] around a sync. */
#include <dolphin/base/PPCArch.h>
#include <dolphin/os.h>

void *memcpy(void *dst, const void *src, unsigned long n);

void __OSSystemCallVectorStart(void);
void __OSSystemCallVectorEnd(void);

static asm void SystemCallVector(void) {
entry __OSSystemCallVectorStart
    nofralloc
    mfspr r9, HID0
    ori r10, r9, 0x8
    mtspr HID0, r10
    isync
    sync
    mtspr HID0, r9
    rfi
entry __OSSystemCallVectorEnd
    nop
}

void __OSInitSystemCall(void) {
    void *addr = OSPhysicalToCached(0xC00);

    memcpy(addr, __OSSystemCallVectorStart,
           (u32)&__OSSystemCallVectorEnd - (u32)&__OSSystemCallVectorStart);
    DCFlushRangeNoSync(addr, 0x100);
    __sync();
    ICInvalidateRange(addr, 0x100);
}

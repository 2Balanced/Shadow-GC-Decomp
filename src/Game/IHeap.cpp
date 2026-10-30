/* Sonicteam::System::IHeap (Game/IHeap.h): the interface's one non-pure virtual function, which
 * places its vtable and RTTI here, and the alignment helper the heap uses. The file name is
 * inferred. The unit's .rodata starts on an 8-byte boundary after TBGRead's, apart from the
 * LandManager.cpp unit that follows. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/IHeap.h"
#include <dolphin/types.h>

extern "C" u32 fn_80045FA0(u32 size, u32 align) {
    return (size + (align - 1)) & ~(align - 1);
}

void Sonicteam::System::IHeap::Virtual14() {}

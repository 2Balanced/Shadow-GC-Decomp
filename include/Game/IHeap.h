#ifndef GAME_IHEAP_H
#define GAME_IHEAP_H

#include <dolphin/types.h>

/* Sonicteam::System::IHeap (name from RTTI): the interface of the game's heaps. Its vtable
 * (0x8051D1E0) has eight slots; all but the fourth are pure. Sonicteam::System::Heap (constructor
 * 0x8040AFF0, vtable 0x80569E6C) implements it. The fourth slot's name is unknown, so it keeps
 * its placeholder name; Game/IHeap.cpp defines it. */
namespace Sonicteam {
namespace System {

class IHeap {
  public:
    /* Slot names and signatures follow Sonicteam::System::Heap's implementations
     * (Game/SonicteamHeap.cpp; names inferred). */
    virtual void *Allocate(u32 size) = 0;
    virtual void *Allocate(u32 alignment, u32 size) = 0;
    virtual void *AllocateZeroed(u32 size) = 0;
    virtual void Virtual14();
    virtual void Free(void *memory) = 0;
    virtual u32 GetLargestFreeSize() = 0;
    virtual void *Reallocate(void *memory, u32 size) = 0;
    virtual u32 GetFreeSize() = 0;
};

} // namespace System
} // namespace Sonicteam

extern "C" {
/* Rounds size up to a multiple of align (a power of two). */
u32 fn_80045FA0(u32 size, u32 align);
}

#endif

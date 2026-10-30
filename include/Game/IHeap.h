#ifndef GAME_IHEAP_H
#define GAME_IHEAP_H

#include <dolphin/types.h>

/* Sonicteam::System::IHeap (name from RTTI): the interface of the game's heaps. Its vtable
 * (0x8051D1E0) has eight slots; all but the fourth are pure. Sonicteam::System::Heap (constructor
 * 0x8040AFF0, vtable 0x80569E6C) implements it. The functions' names and signatures are unknown,
 * so they keep placeholder names; Game/IHeap.cpp defines the one with a body. */
namespace Sonicteam {
namespace System {

class IHeap {
  public:
    virtual void Virtual08() = 0;
    virtual void Virtual0C() = 0;
    virtual void Virtual10() = 0;
    virtual void Virtual14();
    virtual void Virtual18() = 0;
    virtual void Virtual1C() = 0;
    virtual void Virtual20() = 0;
    virtual void Virtual24() = 0;
};

} // namespace System
} // namespace Sonicteam

extern "C" {
/* Rounds size up to a multiple of align (a power of two). */
u32 fn_80045FA0(u32 size, u32 align);
}

#endif

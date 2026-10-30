#ifndef EFFECT_SYSTEM_EFFECTSYSTEM_H
#define EFFECT_SYSTEM_EFFECTSYSTEM_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The effect systems (class names and bases from RTTI): the platform-independent interface that
 * gives the effect library its memory, its RenderWare version, which adds the camera, and the
 * game's version (its methods are at 0x80028730, in the unit after Game/EffectRayUSA.cpp).
 * fn_8042A5D0 returns the current system and fn_8042A5E0 sets it (Game/GameSystem.cpp). Member
 * and function names are inferred. */
namespace Effect {
namespace System {

class EffectSystem {
  public:
    EffectSystem() {}
    /* Sets the system up on a block of memory, and releases it again. */
    virtual void *Initialize(void *heap, u32 size) = 0;
    virtual void *Finalize() = 0;
    /* Allocates and frees the effect library's memory. */
    virtual void *Allocate(u32 size) = 0;
    virtual void Free(void *memory) = 0;
    virtual void Fill(void *memory, int value, u32 size) = 0;
    virtual void Copy(void *to, const void *from, u32 size) = 0;
    virtual void Flush(void *memory) = 0;

    /* 0x4 */ u32 field_0x4;
};

extern "C" EffectSystem *fn_8042A5D0(void);

} // namespace System

namespace RW3 {

class EffectSystemRW3 : public System::EffectSystem {
  public:
    EffectSystemRW3() {}
    /* The RenderWare camera the effects are drawn with. */
    virtual void *GetCamera() = 0;
};

} // namespace RW3
} // namespace Effect

namespace SonicteamUSA {
namespace System {
namespace Effect {

class EffectSystemUSA : public ::Effect::RW3::EffectSystemRW3 {
  public:
    EffectSystemUSA() {}
    virtual void *Initialize(void *heap, u32 size);
    virtual void *Finalize();
    virtual void *Allocate(u32 size);
    virtual void Free(void *memory);
    virtual void Fill(void *memory, int value, u32 size);
    virtual void Copy(void *to, const void *from, u32 size);
    virtual void Flush(void *memory);
    virtual void *GetCamera();

    /* 0x8 */ void *m_heap;
    /* 0xC */ void *m_allocator;
};
SIZE_ASSERT(EffectSystemUSA, 0x10);

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

#endif

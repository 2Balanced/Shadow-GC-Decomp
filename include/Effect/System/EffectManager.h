#ifndef EFFECT_SYSTEM_EFFECTMANAGER_H
#define EFFECT_SYSTEM_EFFECTMANAGER_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The effect managers (class names and bases from RTTI): Effect::System::EffectManager, its
 * RenderWare implementation Effect::RW3::EffectManagerRW3 and the game's
 * SonicteamUSA::System::Effect::EffectManagerUSA. Only what other classes use is declared; the
 * classes are polymorphic (vtable pointer at 0), so they can be the target of a dynamic_cast. */
namespace Effect {
namespace System {

class EffectManager {
  public:
    virtual ~EffectManager();
};

} // namespace System

namespace RW3 {

class EffectManagerRW3 : public ::Effect::System::EffectManager {
  public:
    virtual ~EffectManagerRW3();
};

} // namespace RW3
} // namespace Effect

namespace SonicteamUSA {
namespace System {
namespace Effect {

class EffectManagerUSA : public ::Effect::RW3::EffectManagerRW3 {
  public:
    virtual ~EffectManagerUSA();
};

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

/* Four colour bytes (meaning inferred). */
struct UnkFogColor_8002893C {
    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkFogColor_8002893C, 0x4);

/* A fog setting of EffectManagerUSA (meaning inferred from its use by EffectGlobalParamUSA, which
 * blends its values): on/off, a second switch, a distance-like integer and a colour. The
 * constructor (fn_8002893C) clears both switches and sets 8 and a default colour. */
struct UnkFogParam_8002893C {
    UnkFogParam_8002893C();

    /* 0x0 */ bool enabled;
    /* 0x1 */ bool field_0x1;
    /* 0x4 */ int field_0x4;
    /* 0x8 */ UnkFogColor_8002893C color;
};
SIZE_ASSERT(UnkFogParam_8002893C, 0xC);

extern "C" {
/* Returns the manager's current fog setting, and sets it (behaviour inferred). */
UnkFogParam_8002893C fn_800288D8(SonicteamUSA::System::Effect::EffectManagerUSA *manager);
void fn_80028984(SonicteamUSA::System::Effect::EffectManagerUSA *manager,
                 const UnkFogParam_8002893C *param);
}

#endif

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

class EffectElement;
class EffectLaunchInfo;
struct UnkEffectDesc_801D0AFC;

class EffectManager {
  public:
    virtual ~EffectManager();
    /* Two virtual functions whose purpose is unknown (0x80425630 launches an effect; see
     * Game/EffectPJS.cpp). */
    virtual void fn_80425630();
    virtual void fn_804255F4();
    /* Creates the element desc describes, launched by info (name inferred; the PJS manager's
     * version is in Game/EffectManagerPJS.cpp). */
    virtual EffectElement *CreateElement(const UnkEffectDesc_801D0AFC *desc,
                                         EffectLaunchInfo *info);

    /* 0x04 */ u8 field_0x4[0x58 - 0x4]; /* split between the three manager classes unknown */
};

} // namespace System

namespace RW3 {

class EffectManagerRW3 : public ::Effect::System::EffectManager {
  public:
    virtual ~EffectManagerRW3();
    virtual ::Effect::System::EffectElement *CreateElement(
        const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
        ::Effect::System::EffectLaunchInfo *info);
    /* A virtual function the RenderWare manager adds (purpose unknown; 0x80437C14, and
     * 0x80028BA4 in EffectManagerUSA). */
    virtual void vf18();
};

} // namespace RW3
} // namespace Effect

namespace SonicteamUSA {
namespace System {
namespace Effect {

class EffectManagerUSA : public ::Effect::RW3::EffectManagerRW3 {
  public:
    /* Inline: the PJS manager's destructor (0x800A327C) inlines it. */
    virtual ~EffectManagerUSA() {}
    virtual ::Effect::System::EffectElement *CreateElement(
        const ::Effect::System::UnkEffectDesc_801D0AFC *desc,
        ::Effect::System::EffectLaunchInfo *info);
    virtual void vf18();
};
SIZE_ASSERT(EffectManagerUSA, 0x58);

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

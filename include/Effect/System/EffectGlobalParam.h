#ifndef EFFECT_SYSTEM_EFFECTGLOBALPARAM_H
#define EFFECT_SYSTEM_EFFECTGLOBALPARAM_H

/* The global-parameter effect (class names and bases from RTTI): an effect element that changes
 * scene-wide settings while it runs. Effect::System::EffectGlobalParam keeps the blend factor,
 * Effect::RW3::EffectGlobalParamRW3 applies it under RenderWare, and the game's
 * SonicteamUSA::System::Effect::EffectGlobalParamUSA also blends the effect manager's fog
 * (Game/EffectGlobalParamUSA.cpp). Members and method names other than the classes' are inferred. */
#ifdef EFFECT_SYSTEM_EFFECTELEMENT_H
#ifndef EFFECT_ELEMENT_POLYMORPHIC
#error "Effect/System/EffectGlobalParam.h needs the polymorphic EffectElement; include it first"
#endif
#endif
#define EFFECT_ELEMENT_POLYMORPHIC

#include "Effect/System/EffectElement.h"
#include "Effect/System/EffectManager.h"
#include "size_assert.h"

/* The data an EffectGlobalParam was launched with (layout from its uses). */
struct UnkGlobalParamData_80438A3C {
    /* 0x0 */ int type;   /* 4 changes the fog (inferred) */
    /* 0x4 */ u32 flags;  /* bit 0: the fog's second switch (inferred) */
    /* 0x8 */ int field_0x8;
    /* 0xC */ u8 r;
    /* 0xD */ u8 g;
    /* 0xE */ u8 b;
};

namespace Effect {
namespace System {

class EffectGlobalParam : public EffectElement {
  public:
    virtual ~EffectGlobalParam();
    /* Advances the blend factor by dt (0x8042A5EC). */
    virtual void Update(float dt);
    /* An empty slot (0x801D0FC0). */
    virtual void fn_801D0FC0() {}

    /* 0x054 */ u8 field_0x54[0xFC - 0x54];
    /* 0x0FC */ u32 field_0xFC; /* bit 16: turn the fog on (inferred) */
    /* 0x100 */ float m_blend;  /* 0 to 1 */
    /* 0x104 */ float field_0x104;
    /* 0x108 */ const UnkGlobalParamData_80438A3C *m_data;
};

} // namespace System

namespace RW3 {

class EffectGlobalParamRW3 : public ::Effect::System::EffectGlobalParam {
  public:
    EffectGlobalParamRW3();
    virtual ~EffectGlobalParamRW3();
    virtual void Update(float dt);

    /* 0x10C */ u8 field_0x10C[0x1F4 - 0x10C];
};
SIZE_ASSERT(EffectGlobalParamRW3, 0x1F4);

} // namespace RW3
} // namespace Effect

namespace SonicteamUSA {
namespace System {
namespace Effect {

class EffectGlobalParamUSA : public ::Effect::RW3::EffectGlobalParamRW3 {
  public:
    EffectGlobalParamUSA();
    virtual ~EffectGlobalParamUSA();
    virtual void Update(float dt);

    /* 0x1F4 */ UnkFogParam_8002893C m_savedFog;  /* the manager's fog when the effect started */
    /* 0x200 */ UnkFogParam_8002893C m_targetFog; /* the effect's fog */
};
SIZE_ASSERT(EffectGlobalParamUSA, 0x20C);

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

#endif

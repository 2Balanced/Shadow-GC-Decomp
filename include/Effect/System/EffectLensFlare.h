#ifndef EFFECT_SYSTEM_EFFECTLENSFLARE_H
#define EFFECT_SYSTEM_EFFECTLENSFLARE_H

/* Effect::System::EffectLensFlare (class name from RTTI): the lens flare element; the portable
 * class keeps a blend factor that runs from 0 to 1 over the description's duration, and the
 * RenderWare class (Game/EffectManagerPJS.h) draws it. Its members are seen only by
 * Game/EffectLensFlare.cpp, which defines EFFECT_ELEMENT_FULL; names other than the class's are
 * inferred. */
#include "Effect/System/EffectElement.h"

namespace Effect {
namespace System {

class EffectLensFlare : public EffectElement {
  public:
#ifdef EFFECT_ELEMENT_FULL
    EffectLensFlare(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                    EffectManager *manager);
#endif
    virtual ~EffectLensFlare();
    virtual void Update(float dt);

#ifdef EFFECT_ELEMENT_FULL
    /* 0x0FC */ u32 field_0xFC;
    /* 0x100 */ float m_blend;
    /* 0x104 */ float m_duration;
#endif
};

} // namespace System
} // namespace Effect

#endif

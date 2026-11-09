/* Effect::System::EffectLensFlare (class name from RTTI): the portable part of the lens flare
 * element, which only keeps a blend factor that runs from 0 to 1 over the description's duration
 * (as Game/EffectGlobalParam.cpp's does). File name from the class; member names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first).
 *
 * NonMatching: 2 of 3 functions match; the constructor (94%) has the load scheduling of
 * Game/EffectGlobalParam.cpp's. */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL

#include "Effect/System/EffectDesc.h"
#include "Effect/System/EffectLensFlare.h"

/* A lens flare element's description (layout from the constructor). */
struct UnkLensFlareDesc_8042A868 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u32 m_flags;
    /* 0x30 */ u8 field_0x30[0x34 - 0x30];
    /* 0x34 */ float m_duration;
};

namespace Effect {
namespace System {

EffectLensFlare::EffectLensFlare(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                 EffectManager *manager)
    : EffectElement(desc, info, manager), field_0xFC(0) {
    m_size -= sizeof(EffectElement);
    m_size += sizeof(EffectLensFlare);
    if (!(m_flags & 1)) {
        return;
    }
    const UnkLensFlareDesc_8042A868 *params =
        reinterpret_cast<const UnkLensFlareDesc_8042A868 *>(desc);
    field_0xFC = params->m_flags;
    m_duration = params->m_duration;
}

EffectLensFlare::~EffectLensFlare() {}

void EffectLensFlare::Update(float dt) {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 4) {
        return;
    }
    if (m_flags & 2) {
        m_time += dt;
    }
    if (m_time > 0.0001f + m_duration) {
        m_flags &= ~1;
        return;
    }
    if (m_duration > 0.0001f) {
        m_blend = m_time / m_duration;
    } else {
        m_blend = 1.0f;
    }
    if (m_blend > 1.0f) {
        m_blend = 1.0f;
    }
}

} // namespace System
} // namespace Effect

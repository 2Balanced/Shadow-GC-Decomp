/* Effect::System::EffectGlobalParam (class name from RTTI): the effect element that changes
 * scene-wide settings while it runs (Effect/System/EffectGlobalParam.h). The portable class only
 * keeps the blend factor, which runs from 0 to 1 over the description's duration. File name from
 * the class; member names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first).
 * The constructor takes the description as a pointer to non-const: retail keeps each field's load
 * after the store before it, and GC/2.7 moves the loads up when the parameter points to const
 * (wave 39; probes/Game_EffectGlobalParam*.py; Game/EffectLensFlare.cpp has the same). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define EFFECT_ELEMENT_FULL

#include "Effect/System/EffectDesc.h"
#include "Effect/System/EffectGlobalParam.h"

/* A global-parameter element's description (layout from the constructor). */
struct UnkGlobalParamDesc_8042A6E4 {
    /* 0x00 */ u8 field_0x0[0x2C];
    /* 0x2C */ u32 m_flags;
    /* 0x30 */ float m_duration;
    /* 0x34 */ const UnkGlobalParamData_80438A3C *m_data;
};

namespace Effect {
namespace System {

EffectGlobalParam::EffectGlobalParam(UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                     EffectManager *manager)
    : EffectElement(desc, info, manager), field_0xFC(0) {
    m_size -= sizeof(EffectElement);
    m_size += sizeof(EffectGlobalParam);
    if (!(m_flags & 1)) {
        return;
    }
    UnkGlobalParamDesc_8042A6E4 *params = reinterpret_cast<UnkGlobalParamDesc_8042A6E4 *>(desc);
    m_data = params->m_data;
    field_0x104 = params->m_duration;
    field_0xFC = params->m_flags;
}

EffectGlobalParam::~EffectGlobalParam() {}

void EffectGlobalParam::Update(float dt) {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_flags & 4) {
        return;
    }
    if (m_flags & 2) {
        m_time += dt;
    }
    if (m_time > 0.0001f + field_0x104) {
        m_flags &= ~1;
        return;
    }
    if (field_0x104 > 0.0001f) {
        m_blend = m_time / field_0x104;
    } else {
        m_blend = 1.0f;
    }
    if (m_blend > 1.0f) {
        m_blend = 1.0f;
    }
}

} // namespace System
} // namespace Effect


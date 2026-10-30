/* SonicteamUSA::System::Effect::EffectRayUSA (class name and bases from RTTI): the game's ray
 * effect element. On top of EffectRayRW3 it picks its draw pass from the description and moves to
 * draw pass 5 while hidden (the request bits at 0x104, as Game/EffectParticleUSA.cpp does). File
 * name from the class; member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The unit also emits the class's inline
 * destructor and vtable. */
#define EFFECT_ELEMENT_POLYMORPHIC

#include "Game/EffectManagerPJS.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

namespace SonicteamUSA {
namespace System {
namespace Effect {

EffectRayUSA::EffectRayUSA(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                           EffectManager *manager)
    : EffectRayRW3(desc, info, manager) {
    m_size -= sizeof(EffectRayRW3);
    m_size += sizeof(EffectRayUSA);
    if (!(m_flags & 1)) {
        return;
    }
    switch (desc->m_rayKind) {
    case 0:
        if (m_config & 4) {
            if (m_config & 8) {
                m_drawPass = 1;
            } else {
                m_drawPass = 2;
            }
        } else {
            m_drawPass = 0;
        }
        break;
    case 1:
        m_drawPass = 3;
        break;
    case 3:
        m_drawPass = 4;
        break;
    case 2:
        m_drawPass = 3;
        break;
    }
}

void EffectRayUSA::Update(float dt) {
    EffectRayRW3::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4) && (m_request & 2)) {
        if (m_request & 4) {
            if (m_drawPass != 5) {
                m_shownDrawPass = m_drawPass;
                m_drawPass = 5;
            }
        } else if (m_drawPass == 5) {
            m_drawPass = m_shownDrawPass;
        }
        m_request &= ~2;
    }
}

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

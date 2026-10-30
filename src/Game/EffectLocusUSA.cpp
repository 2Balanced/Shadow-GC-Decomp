/* SonicteamUSA::System::Effect::EffectLocusUSA (class name and bases from RTTI): the game's trail
 * ("locus") effect element. On top of EffectLocusRW3 it picks its draw pass from the description
 * and moves to draw pass 5 while hidden (the request bits at 0x108). File name from the class;
 * member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). The unit also emits the class's vtable; its inline destructor is
 * the PJS units' copy (0x801D0E1C). */
#define EFFECT_ELEMENT_POLYMORPHIC

#include "Game/EffectManagerPJS.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

/* The kind of trail in a locus description (the word at 0x3C, where an object's description
 * keeps its model; meaning inferred). */
inline s32 GetLocusKind(const UnkEffectDesc_801D0AFC *desc) {
    return *reinterpret_cast<const s32 *>(&desc->m_model);
}

namespace SonicteamUSA {
namespace System {
namespace Effect {

EffectLocusUSA::EffectLocusUSA(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                               EffectManager *manager)
    : EffectLocusRW3(desc, info, manager) {
    m_size -= sizeof(EffectLocusRW3);
    m_size += sizeof(EffectLocusUSA);
    if (!(m_flags & 1)) {
        return;
    }
    switch (GetLocusKind(desc)) {
    case 0:
        m_drawPass = 2;
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

void EffectLocusUSA::Update(float dt) {
    EffectLocusRW3::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4) && (field_0x108 & 2)) {
        if (field_0x108 & 4) {
            if (m_drawPass != 5) {
                m_shownDrawPass = m_drawPass;
                m_drawPass = 5;
            }
        } else if (m_drawPass == 5) {
            m_drawPass = m_shownDrawPass;
        }
        field_0x108 &= ~2;
    }
}

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

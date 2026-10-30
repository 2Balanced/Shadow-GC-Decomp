/* SonicteamUSA::System::Effect::EffectArrangerUSA (class name and bases from RTTI): the game's
 * arranger effect element. On top of EffectArrangerRW3 it keeps its own copy of the game launch
 * description it was launched with, and deletes it again. File name from the class; member names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). The unit also emits EffectArranger::Draw and the launch description
 * classes' inline members and vtables. */
#define EFFECT_ELEMENT_POLYMORPHIC

#include "Game/EffectManagerPJS.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

namespace SonicteamUSA {
namespace System {
namespace Effect {

EffectArrangerUSA::EffectArrangerUSA(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                     EffectManager *manager)
    : EffectArrangerRW3(desc, info, manager) {
    m_size -= sizeof(EffectArrangerRW3);
    m_size += sizeof(EffectArrangerUSA);
    if (!(m_flags & 1)) {
        return;
    }
    EffectLaunchInfoUSA *usaInfo = dynamic_cast<EffectLaunchInfoUSA *>(info);
    if (usaInfo != 0) {
        m_launchInfo = new EffectLaunchInfoUSA(usaInfo->m_data, usaInfo->m_file);
    }
}

EffectArrangerUSA::~EffectArrangerUSA() {
    if (m_launchInfo != 0 && dynamic_cast<EffectLaunchInfoUSA *>(m_launchInfo) != 0) {
        delete m_launchInfo;
        m_launchInfo = 0;
    }
}

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

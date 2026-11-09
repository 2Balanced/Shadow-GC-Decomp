/* Effect::RW3::EffectArrangerRW3 (class name and bases from RTTI): the RenderWare arranger effect
 * element. It keeps its own copy of the RenderWare launch description it was launched with, and
 * deletes it again. File name from the class; member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). The unit also
 * emits EffectLaunchInfoRW3's inline constructor and destructor, and its vtable. */
#include "Game/EffectRW3.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

namespace Effect {
namespace RW3 {

EffectArrangerRW3::EffectArrangerRW3(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                     EffectManager *manager)
    : EffectArranger(desc, info, manager) {
    m_size -= sizeof(EffectArranger);
    m_size += sizeof(EffectArrangerRW3);
    if (!(m_flags & 1)) {
        return;
    }
    EffectLaunchInfoRW3 *rw3Info = dynamic_cast<EffectLaunchInfoRW3 *>(info);
    if (rw3Info != 0) {
        m_launchInfo = new EffectLaunchInfoRW3(rw3Info->m_data, rw3Info->field_0x8,
                                               rw3Info->field_0xC, rw3Info->field_0x10);
    }
}

EffectArrangerRW3::~EffectArrangerRW3() {
    if (m_launchInfo != 0 && dynamic_cast<EffectLaunchInfoRW3 *>(m_launchInfo) != 0) {
        delete m_launchInfo;
        m_launchInfo = 0;
    }
}

} // namespace RW3
} // namespace Effect

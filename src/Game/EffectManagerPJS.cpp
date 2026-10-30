/* Effect::PJS::EffectManagerPJS (class name and bases from RTTI): the game's effect manager. Its
 * CreateElement builds the PJS version of each element type the effect files use and leaves the
 * rest to the game's manager. The unit also emits the PJS element classes' vtables and inline
 * destructors. File name from the class; the function name is inferred. */
#define EFFECT_ELEMENT_POLYMORPHIC

#include "Game/EffectManagerPJS.h"

using ::Effect::System::EffectElement;
using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;
using SonicteamUSA::System::Effect::EffectLaunchInfoUSA;
using SonicteamUSA::System::Effect::EffectManagerUSA;

namespace Effect {
namespace PJS {

EffectElement *EffectManagerPJS::CreateElement(const UnkEffectDesc_801D0AFC *desc,
                                               EffectLaunchInfo *info) {
    EffectLaunchInfoUSA *usaInfo = dynamic_cast<EffectLaunchInfoUSA *>(info);
    if (usaInfo != 0) {
        switch (desc->m_type) {
        case 2:
            return new EffectParticlePJS(desc, usaInfo, this);
        case 3:
            return new EffectArrangerPJS(desc, usaInfo, this);
        case 4:
            return new EffectObjectPJS(desc, usaInfo, this);
        case 5:
            return new EffectParticle3DPJS(desc, usaInfo, this);
        case 6:
            return new EffectRayPJS(desc, usaInfo, this);
        case 7:
            return new EffectLocusPJS(desc, usaInfo, this);
        case 8:
            return new EffectGlobalParamPJS(desc, usaInfo, this);
        case 9:
            return new EffectLensFlarePJS(desc, usaInfo, this);
        case 10:
            return new EffectShimmerPJS(desc, usaInfo, this);
        }
    }
    return EffectManagerUSA::CreateElement(desc, info);
}

} // namespace PJS
} // namespace Effect

/* SonicteamUSA::System::Effect::EffectManagerUSA (class name and bases from RTTI): the game's
 * effect manager. Its CreateElement builds the game's version of each element type the effect
 * files use and leaves the rest to the RenderWare manager; the functions before it draw the
 * effects of one draw pass each (called by the EffectDraw tasks, Game/EffectTask.cpp). The unit
 * also emits EffectShimmerUSA's inline destructor and vtable. File name from the class; the
 * function names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define EFFECT_ELEMENT_POLYMORPHIC

#include "Game/EffectManagerPJS.h"

using ::Effect::System::EffectElement;
using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;
using SonicteamUSA::System::Effect::EffectLaunchInfoUSA;
using SonicteamUSA::System::Effect::EffectManagerUSA;
using SonicteamUSA::System::Effect::EffectSystemUSA;

extern "C" {
/* Draws the manager's effects of one draw pass (Effect::System::EffectManager; name inferred). */
void fn_80425170(EffectManager *manager, int pass);
/* RenderWare: begins and ends drawing with a camera. */
void *fn_804854DC(void *camera);
void fn_804854B4(void *camera);
}

namespace SonicteamUSA {
namespace System {
namespace Effect {

EffectElement *EffectManagerUSA::CreateElement(const UnkEffectDesc_801D0AFC *desc,
                                               EffectLaunchInfo *info) {
    EffectLaunchInfoUSA *usaInfo = dynamic_cast<EffectLaunchInfoUSA *>(info);
    if (usaInfo != 0) {
        switch (desc->m_type) {
        case 2:
            return new EffectParticleUSA(desc, usaInfo, this);
        case 3:
            return new EffectArrangerUSA(desc, usaInfo, this);
        case 4:
            return new EffectObjectUSA(desc, usaInfo, this);
        case 5:
            return new EffectParticle3DUSA(desc, usaInfo, this);
        case 6:
            return new EffectRayUSA(desc, usaInfo, this);
        case 7:
            return new EffectLocusUSA(desc, usaInfo, this);
        case 8:
            return new EffectGlobalParamUSA(desc, usaInfo, this);
        case 10:
            return new EffectShimmerUSA(desc, usaInfo, this);
        }
    }
    return EffectManagerRW3::CreateElement(desc, info);
}

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

extern "C" void fn_8002774C(EffectManagerUSA *manager) {
    fn_80425170(manager, 0);
}

extern "C" void fn_80027728(EffectManagerUSA *manager) {
    fn_80425170(manager, 1);
}

extern "C" void fn_80027704(EffectManagerUSA *manager) {
    fn_80425170(manager, 2);
}

extern "C" void fn_800276E0(EffectManagerUSA *manager) {
    fn_80425170(manager, 3);
}

extern "C" void fn_800276BC(EffectManagerUSA *manager) {
    fn_80425170(manager, 4);
}

/* Pass 5 is drawn with the effect system's camera. */
extern "C" void fn_80027640(EffectManagerUSA *manager) {
    EffectSystemUSA *system = dynamic_cast<EffectSystemUSA *>(::Effect::System::fn_8042A5D0());
    if (system != 0) {
        void *camera = system->GetCamera();
        if (fn_804854DC(camera)) {
            fn_80425170(manager, 5);
            fn_804854B4(camera);
        }
    }
}

extern "C" void fn_8002761C(EffectManagerUSA *manager) {
    fn_80425170(manager, 6);
}

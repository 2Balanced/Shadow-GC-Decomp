/* Effect::PJS::EffectParticle3DPJS (class name and bases from RTTI): the PJS 3D particle effect.
 * Besides what the game's EffectParticle3DUSA does, it notes the world of the stage area its model
 * is in. File name from the class; member names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define EFFECT_ELEMENT_POLYMORPHIC

#include "Game/EffectManagerPJS.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;

/* The world of each stage area (as in Game/BlackDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

extern "C" {
/* The stage area a clump is in (see Game/BlackDoomCtrlUnit.cpp). */
int fn_80169364(void *clump);
int fn_80169350(int frame);
void fn_8003A840(int area);
}

namespace Effect {
namespace PJS {

EffectParticle3DPJS::~EffectParticle3DPJS() {}

EffectParticle3DPJS::EffectParticle3DPJS(const UnkEffectDesc_801D0AFC *desc,
                                         EffectLaunchInfo *info, EffectManager *manager)
    : EffectParticle3DUSA(desc, info, manager) {
    if (!(m_flags & 1)) {
        return;
    }
    if (m_clump != 0) {
        int frame = fn_80169364(m_clump);
        if (frame != 0) {
            int area = fn_80169350(frame);
            fn_8003A840(area);
            m_world = lbl_8051D208[area].m_world;
        }
    }
}

} // namespace PJS
} // namespace Effect

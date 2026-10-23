/* Effect::PJS::EffectObjectPJS (class name and bases from RTTI): the PJS model effect. Besides what
 * the game's EffectObjectUSA does, it puts the model into the world of the stage area it is in,
 * and when the effect file holds a UV animation for the model ("<MODEL>.uva"), registers it with
 * the UV animation manager under the model's address. File name from the class; member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define RESOURCEONEFILE_POLYMORPHIC
#define MSL_STRING_CTOR_EXTERN
#define MSL_STRING_DEFAULT_CTOR
#define MSL_STRING_COPY

#include <stdio.h>
#include <string.h>
#include <string>

#include "Game/EffectManagerPJS.h"
#include "Game/ResourceOneFile.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;
using SonicteamUSA::System::Effect::EffectLaunchInfoUSA;

/* The world of each stage area (as in Game/BlackDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The clump manager (see Game/HintRing.cpp). */
class UnkUVAnimManager_801EB994;
UnkUVAnimManager_801EB994 *GetUVAnimManager();

extern "C" {
/* RenderWare clumps and their worlds (see Game/BlackDoomCtrlUnit.cpp). */
void *fn_8046256C(void *clump);
void fn_804623EC(void *world, void *clump);
void fn_804622FC(void *world, void *clump);
int fn_80169364(void *clump);
int fn_80169350(int frame);
void fn_8003A840(int area);
/* Upper-cases a string in place (behaviour inferred). */
void fn_803B1F68(char *string);
/* Registers and removes a UV animation (see Game/EggMecaCtrlUnit.cpp). */
void fn_801EB660(UnkUVAnimManager_801EB994 *manager, std::string name, void *data);
void fn_801EB550(UnkUVAnimManager_801EB994 *manager, std::string name);
}

/* Puts clump in world (name inferred; as in Game/BlackDoomCtrlUnit.cpp). */
inline void SetClumpWorld(void *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

namespace Effect {
namespace PJS {

EffectObjectPJS::EffectObjectPJS(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                 EffectManager *manager)
    : EffectObjectUSA(desc, info, manager) {
    m_size -= sizeof(EffectObjectUSA);
    m_size += sizeof(EffectObjectPJS);
    if (!(m_flags & 1)) {
        return;
    }
    int frame = fn_80169364(m_clump);
    if (frame != 0) {
        int area = fn_80169350(frame);
        fn_8003A840(area);
        m_world = lbl_8051D208[area].m_world;
        void *world = m_world;
        if (world != 0) {
            SetClumpWorld(m_clump, world);
            m_inWorld = true;
        }
    }
    EffectLaunchInfoUSA *usaInfo = dynamic_cast<EffectLaunchInfoUSA *>(info);
    if (usaInfo != 0 && usaInfo->m_file != 0 && desc->m_model->m_hasUvAnim) {
        char path[0x24];
        strcpy(path, desc->m_model->m_name);
        strcat(path, ".uva");
        fn_803B1F68(path);
        m_uvAnim = usaInfo->m_file->GetFile(path);
        if (m_uvAnim != 0 && m_clump != 0) {
            sprintf(path, "%x", m_clump);
            m_uvAnimName = std::string(path);
            fn_801EB660(GetUVAnimManager(), m_uvAnimName, m_clump);
        }
    }
}

EffectObjectPJS::~EffectObjectPJS() {
    if (m_clump != 0 && m_inWorld) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
    }
    if (m_uvAnim != 0) {
        fn_801EB550(GetUVAnimManager(), m_uvAnimName);
    }
}

} // namespace PJS
} // namespace Effect

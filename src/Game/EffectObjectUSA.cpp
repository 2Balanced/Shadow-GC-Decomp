/* SonicteamUSA::System::Effect::EffectObjectUSA (class name and bases from RTTI): the game's model
 * effect element. It loads its model ("<MODEL>.dff") from the effect file it was launched with
 * and colours it through a clump fade, which Update keeps at the effect's colour. File name from
 * the class; member names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). The unit also emits the class's inline destructor
 * and vtable, and the clump fade's constructor fn_80027D5C, which other units call by that
 * name.
 *
 * NonMatching: 2 of 4 functions match (the destructor and fn_80027D5C).
 * - The constructor (95%) and Update (99%) convert the four colour bytes to floats through four
 *   stack temporaries. Every instruction matches except which temporary each byte gets: retail
 *   gives alpha the highest slot and red, green and blue the next ones down. Under GC/2.7 that
 *   order only comes with alpha converted after the colour is stored, which puts its load after
 *   the stores; retail loads all four bytes first. About 40 source shapes were tried (argument
 *   orders, locals, inline helpers, separate statements), and ten more in wave 40
 *   (probes/Game_EffectObjectUSA_R2.py: the colour read through pointers to const, inline
 *   functions for a byte or its float, a helper taking the four bytes); probably the
 *   conversion-slot difference of docs/research-notes.md. Game/EffectParticleUSA.cpp has the same
 *   block.
 * - fn_80027D5C is written as a C function because other units call it by its fn_ name;
 *   retail's is probably the struct's constructor (it is also passed to __construct_new_array,
 *   Game/EffectParticleUSA.cpp). Its loop stands in an inline function (see FillOnes). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define RESOURCEONEFILE_POLYMORPHIC

#include <string.h>

#include "Game/EffectManagerPJS.h"
#include "Game/ResourceOneFile.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;
using SonicteamUSA::System::Effect::EffectLaunchInfoUSA;

/* The colour a fade starts with (a constant of another unit). */
extern const UnkFadeColor_80027D5C lbl_804AB220;

extern "C" {
/* Upper-cases a string in place (behaviour inferred). */
void fn_803B1F68(char *string);
/* Makes a clump from model data, and applies a fade to a clump (see
 * Game/BlackDoomCtrlUnit.cpp). */
void *fn_80457090(void *data);
void fn_80038198(void *clump, UnkClumpFade_80027D5C *fade);
}

/* Sets a fade's colour from alpha, red, green and blue (name inferred). */
inline void SetFadeColor(UnkClumpFade_80027D5C *fade, float a, float r, float g, float b) {
    fade->m_color[0] = r;
    fade->m_color[1] = g;
    fade->m_color[2] = b;
    fade->m_color[3] = a;
}

/* Sets two values to one (name inferred). An inline function: GC/2.7 then makes the loop's
 * offset before the colour's copy and gives it r7, as retail has it; written in the constructor
 * the loop gets r4 (wave 40, probes/Game_EffectObjectUSA.py). */
inline void FillOnes(float *values) {
    for (int i = 0; i < 2; i++) {
        values[i] = 1.0f;
    }
}

extern "C" UnkClumpFade_80027D5C *fn_80027D5C(UnkClumpFade_80027D5C *fade) {
    *reinterpret_cast<UnkFadeColor_80027D5C *>(fade->m_color) = lbl_804AB220;
    fade->field_0x18[0] = 1.0f;
    fade->field_0x18[1] = 1.0f;
    fade->field_0x18[2] = 1.0f;
    FillOnes(fade->field_0x10);
    fade->m_mode = 0;
    fade->field_0x28 = -1.0f;
    return fade;
}


namespace SonicteamUSA {
namespace System {
namespace Effect {

EffectObjectUSA::EffectObjectUSA(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                 EffectManager *manager)
    : EffectObjectRW3(desc, info, manager) {
    m_size -= sizeof(EffectObjectRW3);
    m_size += sizeof(EffectObjectUSA);
    if (!(m_flags & 1)) {
        return;
    }
    EffectLaunchInfoUSA *usaInfo = dynamic_cast<EffectLaunchInfoUSA *>(info);
    if (usaInfo != 0) {
        if (usaInfo->m_file != 0) {
            char path[0x24];
            const ::Effect::System::UnkEffectModel_801D1238 *model = desc->m_model;
            m_drawPass = 0;
            strcpy(path, model->m_name);
            strcat(path, ".dff");
            fn_803B1F68(path);
            m_clump = fn_80457090(usaInfo->m_file->GetFile(path));
        }
        if (m_clump == 0) {
            m_flags &= ~1;
            return;
        }
    }
    if (m_clump != 0) {
        m_fade.m_mode = 4;
        float scale = 1.0f / 255.0f;
        SetFadeColor(&m_fade, ((m_color[3] >> 22) & 0xFF) * scale,
                     ((m_color[0] >> 22) & 0xFF) * scale, ((m_color[1] >> 22) & 0xFF) * scale,
                     ((m_color[2] >> 22) & 0xFF) * scale);
        m_fade.field_0x28 = 0.0f;
        fn_80038198(m_clump, &m_fade);
    }
}

void EffectObjectUSA::Update(float dt) {
    EffectObjectRW3::Update(dt);
    if ((m_flags & 1) && !(m_flags & 4) && m_clump != 0) {
        float scale = 1.0f / 255.0f;
        SetFadeColor(&m_fade, ((m_color[3] >> 22) & 0xFF) * scale,
                     ((m_color[0] >> 22) & 0xFF) * scale, ((m_color[1] >> 22) & 0xFF) * scale,
                     ((m_color[2] >> 22) & 0xFF) * scale);
        m_fade.field_0x28 = m_time / 60.0f;
    }
}

} // namespace Effect
} // namespace System
} // namespace SonicteamUSA

/* SonicteamUSA::System::Effect::EffectParticle3DUSA and EffectParticleUSA (class names and bases
 * from RTTI): the game's 3D and 2D particle effect elements. Both pick their draw pass from the
 * description and load their textures ("<NAME>.txd") from the effect file they were launched with;
 * the 3D one also loads its model ("<MODEL>.dff") and colours each particle's clump through a
 * clump fade. While hidden they move to draw pass 5 (the request bits at 0x104, as
 * Game/EffectLocusUSA.cpp does). File name inferred from the classes; member names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). The unit also emits EffectParticleUSA's inline destructor and both vtables.
 *
 * NonMatching: 5 of 6 functions match. EffectParticle3DUSA::Update (98.6%) has the colour
 * conversion of Game/EffectObjectUSA.cpp: the four stack temporaries come out in another order
 * (blocker described there). */
#define EFFECT_ELEMENT_POLYMORPHIC
#define RESOURCEONEFILE_POLYMORPHIC

#include <string.h>

#include "Game/EffectManagerPJS.h"
#include "Game/ResourceOneFile.h"

using ::Effect::System::EffectLaunchInfo;
using ::Effect::System::EffectManager;
using ::Effect::System::UnkEffectDesc_801D0AFC;
using ::Effect::System::EffectParticleExecUnit;
using SonicteamUSA::System::Effect::EffectLaunchInfoUSA;

extern "C" {
/* Upper-cases a string in place (behaviour inferred). */
void fn_803B1F68(char *string);
/* Applies a fade to a clump (see Game/BlackDoomCtrlUnit.cpp). */
void fn_80038198(void *clump, UnkClumpFade_80027D5C *fade);
/* RenderWare: reads a texture dictionary from file data, and sets a material's textures from
 * one (behaviour inferred). */
void *fn_8048CFFC(void *data, const ::Effect::System::UnkParticleTexture_80028034 *texture);
void fn_8045B270(void *material, void *texDict);
/* The runtime's array construction (what new[] of a class with a constructor calls). */
void *__construct_new_array(void *block, void *ctor, void *dtor, unsigned long size,
                            unsigned long count);
}

/* A 3D particle effect's model data: one clump per particle (layout from its use). */
struct UnkParticleModel_80027DC4 {
    /* 0x00 */ u8 field_0x0[0x90];
    /* 0x90 */ void *m_clump;
    /* 0x94 */ u8 field_0x94[0x98 - 0x94];
};

/* A 2D particle effect's model data, down to the material its textures go to (layout from its
 * use). */
struct UnkParticleMaterials_80028034 {
    /* 0x0 */ void *m_material;
};
struct UnkParticleGeometry_80028034 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ UnkParticleMaterials_80028034 *m_materials;
};
struct UnkParticleModel_80028034 {
    /* 0x00 */ u8 field_0x0[0x18];
    /* 0x18 */ UnkParticleGeometry_80028034 *m_geometry;
};

/* Sets the red, green and blue of a fade (name inferred). */
inline void SetFadeColor(UnkClumpFade_80027D5C *fade, float r, float g, float b) {
    fade->m_color[0] = r;
    fade->m_color[1] = g;
    fade->m_color[2] = b;
}

namespace SonicteamUSA {
namespace System {
namespace Effect {

EffectParticleUSA::EffectParticleUSA(const UnkEffectDesc_801D0AFC *desc, EffectLaunchInfo *info,
                                     EffectManager *manager)
    : EffectParticleRW3(desc, info, manager) {
    m_size -= sizeof(EffectParticleRW3);
    m_size += sizeof(EffectParticleUSA);
    if (!(m_flags & 1)) {
        return;
    }
    switch (desc->m_particleKind) {
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
    if ((m_config & 0x10) && desc->m_texture != 0) {
        EffectLaunchInfoUSA *usaInfo = dynamic_cast<EffectLaunchInfoUSA *>(info);
        if (usaInfo != 0 && usaInfo->m_file != 0) {
            char path[0x24];
            strcpy(path, desc->m_texture->m_name);
            strcat(path, ".txd");
            fn_803B1F68(path);
            void *data = usaInfo->m_file->GetFile(path);
            if (data != 0) {
                void *texDict = fn_8048CFFC(data, desc->m_texture);
                if (texDict != 0) {
                    UnkParticleModel_80028034 *model =
                        static_cast<UnkParticleModel_80028034 *>(m_model);
                    fn_8045B270(model->m_geometry->m_materials->m_material, texDict);
                }
            }
        }
    }
}

void EffectParticleUSA::Update(float dt) {
    EffectParticleRW3::Update(dt);
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

EffectParticle3DUSA::EffectParticle3DUSA(const UnkEffectDesc_801D0AFC *desc,
                                         EffectLaunchInfo *info, EffectManager *manager)
    : EffectParticle3DRW3(desc, info, manager), m_fades(0), m_fadeSet(0) {
    m_size -= sizeof(EffectParticle3DRW3);
    m_size += sizeof(EffectParticle3DUSA);
    if (!(m_flags & 1)) {
        return;
    }
    switch (desc->m_particleKind) {
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
    if ((m_config & 0x10) && desc->m_texture != 0) {
        EffectLaunchInfoUSA *usaInfo = dynamic_cast<EffectLaunchInfoUSA *>(info);
        if (usaInfo != 0 && usaInfo->m_file != 0) {
            char path[0x24];
            strcpy(path, desc->m_texture->m_name);
            strcat(path, ".txd");
            fn_803B1F68(path);
            void *data = usaInfo->m_file->GetFile(path);
            if (data != 0) {
                m_texDict = fn_8048CFFC(data, desc->m_texture);
            }
        }
    }
    if ((m_config & 0x8000) && desc->m_modelName != 0) {
        EffectLaunchInfoUSA *usaInfo = dynamic_cast<EffectLaunchInfoUSA *>(info);
        if (usaInfo != 0 && usaInfo->m_file != 0) {
            char path[0x24];
            strcpy(path, desc->m_modelName);
            strcat(path, ".dff");
            fn_803B1F68(path);
            m_clump = usaInfo->m_file->GetFile(path);
        }
    }
    s32 count = m_count;
    m_fades = static_cast<UnkClumpFade_80027D5C *>(__construct_new_array(
        operator new[](count * sizeof(UnkClumpFade_80027D5C) + 0x10), (void *)fn_80027D5C, 0,
        sizeof(UnkClumpFade_80027D5C), count));
    m_fadeSet = new bool[m_count];
    if (m_fadeSet != 0) {
        for (int i = 0; i < m_count; i++) {
            m_fadeSet[i] = false;
        }
    }
}

EffectParticle3DUSA::~EffectParticle3DUSA() {
    if (m_fades != 0) {
        delete[] m_fades;
    }
    if (m_fadeSet != 0) {
        delete[] m_fadeSet;
    }
}

void EffectParticle3DUSA::Update(float dt) {
    EffectParticle3DRW3::Update(dt);
    if (!(m_flags & 1) || (m_flags & 4)) {
    } else if (m_clump != 0) {
        EffectParticleExecUnit *particle = m_particles;
        for (int i = 0; i < m_count; i++, particle = particle->m_next) {
            if (particle == 0) {
                return;
            }
            UnkParticleModel_80027DC4 *model = static_cast<UnkParticleModel_80027DC4 *>(m_model);
            if (model[i].m_clump != 0 && m_fades != 0 && m_fadeSet != 0) {
                float scale = 1.0f / 255.0f;
                float alpha = ((particle->m_color[3] >> 22) & 0xFF) * scale;
                SetFadeColor(&m_fades[i], ((particle->m_color[0] >> 22) & 0xFF) * scale,
                             ((particle->m_color[1] >> 22) & 0xFF) * scale,
                             ((particle->m_color[2] >> 22) & 0xFF) * scale);
                m_fades[i].m_color[3] = alpha;
                m_fades[i].m_mode = 4;
                if (!m_fadeSet[i]) {
                    fn_80038198(static_cast<UnkParticleModel_80027DC4 *>(m_model)[i].m_clump,
                                &m_fades[i]);
                    m_fadeSet[i] = true;
                }
            }
        }
    } else if (m_request & 2) {
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

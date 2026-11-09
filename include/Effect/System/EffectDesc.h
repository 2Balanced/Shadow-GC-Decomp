#ifndef EFFECT_SYSTEM_EFFECTDESC_H
#define EFFECT_SYSTEM_EFFECTDESC_H

/* The effect files ("EFFD") the effect managers launch effects from, and the element descriptions
 * in them (layouts from their uses; names other than the classes' are inferred). The file holds
 * offsets from its start until EffectManager::Relocate (0x80425860) turns them into pointers. */
#include <dolphin/types.h>

namespace Effect {
namespace System {

/* The model an EffectObject shows (layout from Game/EffectObjectPJS.cpp). */
struct UnkEffectModel_801D1238 {
    /* 0x00 */ char m_name[0x40];
    /* 0x40 */ bool m_hasUvAnim; /* whether the effect file holds a UV animation for it */
};

/* A particle effect's textures (layout from Game/EffectParticleUSA.cpp). */
struct UnkParticleTexture_80028034 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ char m_name[0x20]; /* the texture dictionary's name */
};

/* The description an element is created from (layout from its uses). */
struct UnkEffectDesc_801D0AFC {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ s16 m_type; /* which element class to create */
    /* 0x22 */ u8 field_0x22[0x24 - 0x22];
    /* 0x24 */ s32 m_id;   /* the id Launch looks it up by */
    /* 0x28 */ UnkEffectDesc_801D0AFC *m_next; /* the next description in the file */
    /* 0x2C */ u8 field_0x2C[0x3C - 0x2C];
    /* 0x3C */ const UnkEffectModel_801D1238 *m_model; /* EffectObject's model */
    /* 0x40 */ u8 field_0x40[0x4C - 0x40];
    /* 0x4C */ s32 m_rayKind;      /* a ray effect's kind, which picks its draw pass */
    /* 0x50 */ s32 m_particleKind; /* a particle effect's kind, which picks its draw pass */
    /* 0x54 */ u8 field_0x54[0x58 - 0x54];
    /* 0x58 */ const UnkParticleTexture_80028034 *m_texture; /* a particle effect's textures */
    /* 0x5C */ const char *m_modelName; /* a 3D particle effect's model */
};

/* The header of an effect file. */
struct UnkEffectData_80425860 {
    /* 0x00 */ char m_magic[4]; /* "EFFD" */
    /* 0x04 */ s16 m_version;
    /* 0x06 */ s16 m_relocated; /* set once the offsets are pointers */
    /* 0x08 */ u8 field_0x8[0x2C - 0x8];
    /* 0x2C */ UnkEffectDesc_801D0AFC *m_descs; /* the first description */
};

} // namespace System
} // namespace Effect

#endif

#ifndef GAME_BOSSDEF_H
#define GAME_BOSSDEF_H

/* The tables a boss task gives its BossVarParam (BossVarParam::m_def): the boss's collision
 * spheres and its targets. The struct names are unknown; the layouts come from the tables of
 * Game/DevilDoom.cpp and Game/BlackBull0210.cpp. Member names and meanings are inferred. */

#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A sphere (class name unknown): a centre and a radius. The constructor is out of line at
 * 0x80035CEC; the bosses construct their spheres with it. */
struct UnkSphere_80035CEC {
    UnkSphere_80035CEC(const Vec3f &center, float radius);

    /* 0x0 */ Vec3f m_center;
    /* 0xC */ float m_radius;
};

/* A collision shape (struct name unknown). A collision's shapes form an array that an entry of
 * zeros ends. */
struct UnkBossColliShape {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ UnkSphere_80035CEC *m_sphere;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ u32 field_0xC;
};
SIZE_ASSERT(UnkBossColliShape, 0x10);

/* An entry of the boss's collision table (struct name unknown). */
struct UnkBossColli {
    /* 0x00 */ const UnkBossColliShape *m_shape;
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ float field_0xC;
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ s32 field_0x14;
    /* 0x18 */ u8 field_0x18;
    /* 0x1C */ s32 field_0x1C;
    /* 0x20 */ u8 field_0x20;
};
SIZE_ASSERT(UnkBossColli, 0x24);

/* An entry of the boss's target table (struct name unknown). */
struct UnkBossTargetEntry {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ Vec3f *m_target;
};

/* A point the boss's camera looks at, and a radius (struct name unknown). */
struct UnkBossFocus {
    /* 0x0 */ Vec3f *m_point;
    /* 0x4 */ float m_radius;
};

/* The tables a boss's BossVarParam points to (struct name unknown). */
struct UnkBossDef {
    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ UnkBossColli *m_collis;
    /* 0x18 */ UnkBossTargetEntry *m_targets;
    /* 0x1C */ UnkBossFocus *m_focus;
};
SIZE_ASSERT(UnkBossDef, 0x20);

#endif

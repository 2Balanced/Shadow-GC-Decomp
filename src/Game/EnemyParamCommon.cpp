/* EnemyParamCommon (class name from RTTI; Game/Enemy.h): the parameter part of an enemy, which
 * hands out the enemy's values from tables indexed by its type (Enemy::m_type): its kind, the files
 * of its resources (Game/EnemyTypeData.cpp), the bone its effects hang from, its body, movement and
 * jump values and the bones it can be hit at. Its motion sequences default to a one-step table. File
 * name inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 12 of 22 functions and all data match. Two compiler differences remain: in the
 * slots that call out (0x0C to 0x18 and 0x38) retail loads m_enemy's base pointer above the store
 * of the link register (as in Game/EnemyRender.cpp and Game/BkWorm.cpp), and in the float slots
 * that read past a record's first value (0x24 to 0x2C, 0x34 and 0x68) retail loads the base
 * pointer into r3 and the table into r4, where GC/2.7 swaps the two for every source form tried. */
#include "Game/Enemy.h"
#include "Game/MotionSequenceModule.h"
#include <dolphin/types.h>

/* The enemies' resource files and a count by enemy type (Game/EnemyTypeData.cpp). */
extern const char *const lbl_804CBF2C[15][4];
extern "C" s32 fn_801A04C0(int type);

extern "C" {
/* The resource loader and a value of a slot (see Game/BkLarva.cpp). */
void *fn_800122D4(void);
int fn_80012608(void *loader, int slot, int kind);
}

/* The values the tables hold (names inferred). */
struct EnemyBodyParam {
    /* 0x00 */ float m_radius;
    /* 0x04 */ float field_0x4;
    /* 0x08 */ float m_floorOffset;
    /* 0x0C */ float m_height;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ float field_0x14;
};

struct EnemyMoveParam {
    /* 0x00 */ float field_0x0;
    /* 0x04 */ float field_0x4;
    /* 0x08 */ float field_0x8;
    /* 0x0C */ float field_0xC;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ float field_0x14;
    /* 0x18 */ float m_gravity;
};

struct EnemyJumpParam {
    /* 0x00 */ float m_speed;
    /* 0x04 */ float m_height;
    /* 0x08 */ float field_0x8;
};

struct EnemyHitBone {
    /* 0x00 */ s32 m_bone;
    /* 0x04 */ u32 m_flags;
};

/* The motion sequences an enemy starts with: one sequence of one step (names inferred). */
static const MotionSequenceStep s_defaultSteps[] = {{1, -1, -1, 0, 0}};
static const MotionSequence s_defaultSequences[] = {{s_defaultSteps, 1, 0}};
static const MotionSequenceTable s_defaultSequenceTable = {s_defaultSequences, 1};

/* The enemies' kinds, by enemy type: 0 GUN, 2 Black Arms, 1 Eggman (name inferred). */
static const s32 s_kinds[15] = {0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1};

/* The bone the enemies' effects and target hang from, by enemy type (name inferred). */
static const s16 s_bones[15] = {40, 1000, 3201, 10, 10, 10, 3151, 4101, 3007, 2000, 5000, 10, 10, 10, 10};

/* The bodies' sizes, by enemy type (names inferred). */
static const EnemyBodyParam s_bodies[15] = {
    {7.0f, 22.0f, 0.0f, 10.0f, 5.0f, 4.0f}, /* GUN_SOLDIER */
    {15.0f, 22.0f, -8.0f, 0.0f, 9.0f, 8.0f}, /* GUN_BEETLE */
    {20.0f, 40.0f, 0.0f, 0.0f, 20.0f, 18.0f}, /* GUN_BIGFOOT */
    {18.0f, 30.0f, 0.0f, 0.0f, 15.0f, 12.0f}, /* GUN_ROBOT */
    {15.0f, 50.0f, 0.0f, 20.0f, 13.0f, 12.0f}, /* BK_GIANT */
    {10.0f, 30.0f, 0.0f, 20.0f, 7.0f, 6.0f}, /* BK_SOLDIER */
    {15.0f, 20.0f, -4.0f, 0.0f, 5.0f, 7.0f}, /* BK_WINGLARGE */
    {10.0f, 15.0f, -4.0f, 0.0f, 3.0f, 4.0f}, /* BK_WINGSMALL */
    {10.0f, 40.0f, 0.0f, 0.0f, 10.0f, 10.0f}, /* BK_WORM */
    {8.0f, 10.0f, 0.0f, 0.0f, 8.0f, 8.0f}, /* BK_LARVA */
    {8.0f, 10.0f, 0.0f, 0.0f, 12.0f, 12.0f}, /* BK_CHAOS */
    {8.0f, 40.0f, 0.0f, 15.0f, 8.0f, 7.0f}, /* BK_NINJA */
    {12.0f, 24.0f, 0.0f, 12.0f, 8.0f, 8.0f}, /* EGG_PAWN */
    {12.0f, 24.0f, 0.0f, 12.0f, 8.0f, 8.0f}, /* EGG_PIERROT */
    {7.0f, 20.0f, 0.0f, 5.0f, 3.0f, 3.0f}, /* EGG_SHADOWANDROID */
};

/* The movement values, by enemy type (names inferred; Game/BkLarva.cpp reads the table by its
 * address name). */
extern const EnemyMoveParam lbl_804CC424[15] = {
    {10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 6.0f, 120.0f}, /* GUN_SOLDIER */
    {50.0f, 50.0f, 50.0f, 50.0f, 50.0f, 3.0f, 0.0f}, /* GUN_BEETLE */
    {10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 1.0f, 120.0f}, /* GUN_BIGFOOT */
    {50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 3.0f, 120.0f}, /* GUN_ROBOT */
    {10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 3.0f, 120.0f}, /* BK_GIANT */
    {10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 6.0f, 120.0f}, /* BK_SOLDIER */
    {50.0f, 50.0f, 50.0f, 50.0f, 50.0f, 6.0f, 0.0f}, /* BK_WINGLARGE */
    {50.0f, 50.0f, 50.0f, 50.0f, 50.0f, 6.0f, 0.0f}, /* BK_WINGSMALL */
    {10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 6.0f, 120.0f}, /* BK_WORM */
    {10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 6.0f, 120.0f}, /* BK_LARVA */
    {10.0f, 10.0f, 10.0f, 0.0f, 0.0f, 8.0f, 0.0f}, /* BK_CHAOS */
    {50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 6.0f, 120.0f}, /* BK_NINJA */
    {50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 3.0f, 120.0f}, /* EGG_PAWN */
    {50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 3.0f, 120.0f}, /* EGG_PIERROT */
    {50.0f, 50.0f, 50.0f, 0.0f, 0.0f, 3.0f, 120.0f}, /* EGG_SHADOWANDROID */
};

/* A value by enemy type (meaning unknown; name inferred). */
static const float s_values[15] = {2.0f, 2.0f, 18.0f, 8.0f, 24.0f, 4.0f, 6.0f, 2.0f, 12.0f, 1.0f, 1.0f, 16.0f, 4.0f, 10.0f, 8.0f};

/* The jumps, by enemy type (names inferred; Game/BkLarva.cpp reads the table by its address
 * name). */
extern const EnemyJumpParam lbl_804CC604[15] = {
    {50.0f, 1.3f, 1.0f}, /* GUN_SOLDIER */
    {2.0f, 2.0f, 1.0f}, /* GUN_BEETLE */
    {0.0f, 0.0f, 0.0f}, /* GUN_BIGFOOT */
    {0.0f, 0.0f, 0.0f}, /* GUN_ROBOT */
    {0.0f, 0.0f, 0.0f}, /* BK_GIANT */
    {50.0f, 1.3f, 1.0f}, /* BK_SOLDIER */
    {0.0f, 0.0f, 1.0f}, /* BK_WINGLARGE */
    {0.0f, 0.0f, 1.0f}, /* BK_WINGSMALL */
    {0.0f, 0.0f, 0.0f}, /* BK_WORM */
    {50.0f, 1.2f, 1.0f}, /* BK_LARVA */
    {0.0f, 0.0f, 0.0f}, /* BK_CHAOS */
    {50.0f, 1.3f, 1.0f}, /* BK_NINJA */
    {40.0f, 1.2f, 1.0f}, /* EGG_PAWN */
    {0.0f, 0.0f, 0.0f}, /* EGG_PIERROT */
    {50.0f, 1.3f, 1.0f}, /* EGG_SHADOWANDROID */
};

/* The bones the enemies can be hit at, by enemy type: lists of a bone and the attack flags that
 * hit it, each ended by -1 (meanings and names inferred). The lists are defined in this order. */
static const EnemyHitBone s_hitBonesGunSoldier[] = {{40, 0xC80}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesBkSoldier[] = {{10, 0xC80}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesEggPawn[] = {{10, 0xC80}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesGunBeetle[] = {{1000, 0xC80}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesBkWingsmall[] = {{4101, 0xC80}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesBkWinglarge[] = {{3151, 0xC80}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesBkLarva[] = {{2000, 0xC80}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesGunRobot[] = {{30, 0x1400}, {10, 0x800}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesGunBigfoot[] = {{9000, 0x1400}, {3201, 0x800}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesBkWorm[] = {{3007, 0xC80}, {3007, 0x1000}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesBkGiant[] = {{30, 0x1400}, {10, 0x800}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesBkChaos[] = {{5000, 0xC80}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesBkNinja[] = {{30, 0x1400}, {10, 0x800}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesEggPierrot[] = {{10, 0x480}, {-1, 0x0}};
static const EnemyHitBone s_hitBonesEggShadowandroid[] = {{10, 0xC80}, {-1, 0x0}};

static const EnemyHitBone *s_hitBones[15] = {
    s_hitBonesGunSoldier,
    s_hitBonesGunBeetle,
    s_hitBonesGunBigfoot,
    s_hitBonesGunRobot,
    s_hitBonesBkGiant,
    s_hitBonesBkSoldier,
    s_hitBonesBkWinglarge,
    s_hitBonesBkWingsmall,
    s_hitBonesBkWorm,
    s_hitBonesBkLarva,
    s_hitBonesBkChaos,
    s_hitBonesBkNinja,
    s_hitBonesEggPawn,
    s_hitBonesEggPierrot,
    s_hitBonesEggShadowandroid,
};

/* A motion by enemy type (EnemyBaseAI plays it; name inferred). */
static const u16 s_motions[15] = {48, 3, 0, 12, 0, 22, 6, 5, 8, 0, 0, 5, 22, 8, 1};

EnemyParamCommon::EnemyParamCommon() {
    field_0x8 = reinterpret_cast<int>(&s_defaultSequenceTable);
}

EnemyParamCommon::~EnemyParamCommon() {}

int EnemyParamCommon::vfPara_0x08() {
    return s_kinds[m_enemy->m_type];
}

/* The enemy's type (name inferred). An empty inline call keeps it out of line, as retail has it. */
inline void UnkEnemyParamNothing() {}
inline s32 EnemyTypeOf(Enemy *enemy) {
    UnkEnemyParamNothing();
    return enemy->m_type;
}

/* The enemy's resource files (names inferred from Game/EnemyTypeData.cpp). */
int EnemyParamCommon::vfPara_0x0C() {
    return reinterpret_cast<int>(lbl_804CBF2C[EnemyTypeOf(m_enemy)][0]);
}

int EnemyParamCommon::vfPara_0x10() {
    return reinterpret_cast<int>(lbl_804CBF2C[EnemyTypeOf(m_enemy)][1]);
}

int EnemyParamCommon::vfPara_0x14() {
    return reinterpret_cast<int>(lbl_804CBF2C[EnemyTypeOf(m_enemy)][2]);
}

int EnemyParamCommon::vfPara_0x18() {
    return reinterpret_cast<int>(lbl_804CBF2C[EnemyTypeOf(m_enemy)][3]);
}

int EnemyParamCommon::vfPara_0x38() {
    return fn_801A04C0(m_enemy->m_type);
}

int EnemyParamCommon::vfPara_0x3C() {
    return s_bones[m_enemy->m_type];
}

int EnemyParamCommon::vfPara_0x1C() {
    return reinterpret_cast<int>(&s_bodies[m_enemy->m_type]);
}

float EnemyParamCommon::vfPara_0x20() {
    return lbl_804CC424[m_enemy->m_type].field_0x0;
}

float EnemyParamCommon::vfPara_0x24() {
    return lbl_804CC424[m_enemy->m_type].field_0x4;
}

float EnemyParamCommon::vfPara_0x28() {
    return lbl_804CC424[m_enemy->m_type].field_0x8;
}

float EnemyParamCommon::vfPara_0x2C() {
    return lbl_804CC424[m_enemy->m_type].field_0xC;
}

float EnemyParamCommon::vfPara_0x30() {
    return lbl_804CC424[m_enemy->m_type].field_0x0;
}

float EnemyParamCommon::vfPara_0x34() {
    return lbl_804CC424[m_enemy->m_type].field_0x14;
}

float EnemyParamCommon::vfPara_0x68() {
    return lbl_804CC424[m_enemy->m_type].m_gravity;
}

float EnemyParamCommon::vfPara_0x6C() {
    return s_values[m_enemy->m_type];
}

int EnemyParamCommon::vfPara_0x70() {
    return reinterpret_cast<int>(&lbl_804CC604[m_enemy->m_type]);
}

int EnemyParamCommon::vfPara_0x74() {
    return reinterpret_cast<int>(s_hitBones[m_enemy->m_type]);
}

/* The motion EnemyBaseAI plays for enemy type type; the Shadow android's comes from its resources
 * (name inferred). */
extern "C" u16 fn_801A61E4(int type) {
    if (type == 14) {
        return fn_80012608(fn_800122D4(), 0x15, 7);
    }
    return s_motions[type];
}

/* SoldierCommonBase (class name from RTTI; Game/Enemy.h): the soldiers' (BkSoldier, GunSoldier)
 * patrol point set-up, the unit after GunSoldier.cpp. The soldiers call these as functions of their
 * own (vfSet_0x20); they keep their address names. File name inferred from the class. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */

#include "Game/Enemy.h"
#include "Game/MathTypes.h"
#include "Game/Random.h"
#include "Game/SoldierParam.h"

/* pi. Declared const here: retail loads it ahead of the stores before it, which the compiler does
 * for const data only. */
extern const float lbl_805FAD38;

extern "C" {
/* v turned about y by angle, and vector += value (see GunRobot.cpp). */
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
}

/* The vector type's out-of-line assignment, called with a temporary the way the original's
 * operator is (name inferred; see JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The enemy's home position (name inferred; another unit calls it, 0x802B1C94). */
extern "C" Vec3f *fn_80198BD8(Enemy *enemy) {
    return &enemy->field_0x48;
}

/* Picks the random point of a soldier that moves at random (RANDOM_MOVE): a point within
 * field_0x284 of home (behaviour inferred; the soldiers' AI calls it too). */
extern "C" void fn_80198B5C(SoldierCommonBase *soldier) {
    Vec3f &point = soldier->m_points[3].m_position;
    point.x = fn_8040EC80(-soldier->field_0x284, soldier->field_0x284);
    point.y = 0.0f;
    point.z = 0.0f;
    AssignVector(point, fn_8040D0AC(&point, fn_8040EC80(0.0f, lbl_805FAD38)));
    fn_8040CFF8(&point, fn_80198BD8(soldier));
}

/* Sets a soldier up from its parameters: its patrol points around home, by AppearType (names
 * inferred; BkSoldier and GunSoldier call it from vfSet_0x20). The parameters and home are const:
 * retail loads them ahead of the stores before them, which the compiler does for const data
 * only. */
extern "C" void fn_801989D4(SoldierCommonBase *soldier, const SoldierParam *params,
                            const Vec3f *home) {
    soldier->field_0x270 = params->HaveShield != 0;
    soldier->m_appearType = params->AppearType;
    soldier->m_weaponType = params->WeaponType;
    soldier->field_0x27C = soldier->vf_0x23C();
    soldier->m_points[0].m_position = *home;
    soldier->m_points[0].field_0xC = params->Pos0_WaitSec;
    soldier->m_points[0].field_0x10 = params->Pos0_MoveSpeedRatio;
    soldier->m_points[0].field_0x14 = params->Pos0_WaitType;
    switch (soldier->field_0x27C) {
    case 0:
        break;
    case 2:
        fn_800091D0(&soldier->m_points[2].m_position, home);
        soldier->m_points[2].m_position.x += params->Pos2_X;
        soldier->m_points[2].m_position.z += params->Pos2_Z;
        soldier->m_points[2].field_0xC = params->Pos2_WaitSec;
        soldier->m_points[2].field_0x10 = params->Pos2_MoveSpeedRatio;
        soldier->m_points[2].field_0x14 = params->Pos2_WaitType;
    case 1:
        fn_800091D0(&soldier->m_points[1].m_position, home);
        soldier->m_points[1].m_position.x += params->Pos1_X;
        soldier->m_points[1].m_position.z += params->Pos1_Z;
        soldier->m_points[1].field_0xC = params->Pos1_WaitSec;
        soldier->m_points[1].field_0x10 = params->Pos1_MoveSpeedRatio;
        soldier->m_points[1].field_0x14 = params->Pos1_WaitType;
        break;
    case 3:
        soldier->field_0x284 = params->Pos1_WaitSec;
        if (soldier->field_0x284 > params->MoveRange) {
            soldier->field_0x284 = params->MoveRange;
        }
        fn_80198B5C(soldier);
        soldier->m_points[3].field_0xC = params->Pos0_WaitSec;
        soldier->m_points[3].field_0x10 = params->Pos0_MoveSpeedRatio;
        soldier->m_points[3].field_0x14 = params->Pos0_WaitType;
        break;
    }
}

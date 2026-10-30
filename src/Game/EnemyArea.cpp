/* The area an enemy stays in (UnkEnemyArea_8018A374, class name unknown; Game/Enemy.h, Enemy's
 * field_0xA8): a circle about the enemy's home that it moves in and a cylinder ahead of it that it
 * watches, set up from a larva generator's parameters (Game/BkLarva.h) and tested against points.
 * The other units call these functions by their address names. File name inferred; names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first).
 *
 * NonMatching: 3 of 5 functions and all data match. In Contains and fn_801A69A0 retail loads the
 * point's coordinates above the prologue's stores (stw r0 and the saved registers); GC/2.7 keeps
 * them after, for every source form tried. */
/* The two halves of the area (Game/Enemy.h). */
#define ENEMY_AREA_PARTS

#include "Game/BkLarva.h"
#include "Game/CharInfo.h"
#include "Game/Enemy.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"

/* MSL's std::fabs for floats, which the game calls out of line (as in Game/Enemy.cpp). */
namespace std {
float fabs(float x);
}

extern "C" {
/* The squared distance between two points (behaviour inferred), and vector helpers (see
 * Game/Enemy.cpp). */
float fn_8040D180(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
}

/* The area's values, by their place in its two halves (names inferred): the circle's radius and
 * its square, and the watched cylinder's radius, its square and its height. */
inline float &MoveRadius(UnkEnemyArea_8018A374 *area) {
    return *reinterpret_cast<float *>(&area->m_shape.field_0x18[0]);
}

inline float &MoveRadiusSquared(UnkEnemyArea_8018A374 *area) {
    return *reinterpret_cast<float *>(&area->m_shape.field_0x18[4]);
}

/* degrees in radians (name inferred; as in Game/Enemy.cpp). */
inline float Radians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* Sets the area up about position and rotation from a generator's parameters (name inferred). */
extern "C" void fn_801A69FC(UnkEnemyArea_8018A374 *area, const Vec3f *position, const Vec3f *rotation,
                            const BkLarvaParams *params) {
    fn_800091D0(&area->m_shape.field_0xC, position);
    area->m_shape.field_0x0.x = position->x;
    area->m_shape.field_0x0.z = position->z;
    MoveRadius(area) = params->MoveRange;
    MoveRadiusSquared(area) = params->MoveRange * params->MoveRange;
    fn_800091D0(&area->m_bounds.field_0xC,
                &(const Vec3f &)fn_8040CDD0(
                    &(const Vec3f &)fn_8040CDD0(
                        position, &(const Vec3f &)fn_8040D0AC(
                                      &Vec3f(0.0f, 0.0f, params->SearchRange),
                                      rotation->y + Radians(180.0f + params->SearchAngle))),
                    &Vec3f(0.0f, params->SearchHeightOffset, 0.0f)));
    area->m_bounds.field_0x0.x = area->m_bounds.field_0xC.x;
    area->m_bounds.field_0x0.z = area->m_bounds.field_0xC.z;
    area->m_bounds.field_0x18.x = params->SearchWidth;
    area->m_bounds.field_0x18.y = params->SearchWidth * params->SearchWidth;
    area->m_bounds.field_0x18.z = params->SearchHeight;
}

/* Whether a character is within the area's circle of its home (name inferred). */
extern "C" u8 fn_801A69A0(UnkEnemyArea_8018A374 *area, CharInfo *chara) {
    Vec3f position(chara->m_position.x, chara->m_position.y, chara->m_position.z);
    return fn_8040D180(&position, &area->m_shape.field_0xC) < MoveRadiusSquared(area);
}

/* Whether a point is within the area's circle, on the ground plane (name inferred). */
extern "C" u8 fn_801A6944(UnkEnemyArea_8018A374 *area, const Vec3f *point) {
    Vec3f flat(point->x, 0.0f, point->z);
    return fn_8040D180(&flat, &area->m_shape.field_0x0) < MoveRadiusSquared(area);
}

u8 UnkEnemyArea_8018A374::Contains(void *key) {
    const Vec3f *point = static_cast<const Vec3f *>(key);
    if (std::fabs(point->y - m_bounds.field_0xC.y) < m_bounds.field_0x18.z) {
        Vec3f flat(point->x, 0.0f, point->z);
        if (fn_8040D180(&flat, &m_bounds.field_0x0) < m_bounds.field_0x18.y) {
            return true;
        }
    }
    return false;
}

/* Centres the area's circle on point (name inferred). */
extern "C" void fn_801A687C(UnkEnemyArea_8018A374 *area, const Vec3f *point) {
    fn_800091D0(&area->m_shape.field_0xC, point);
    area->m_shape.field_0x0.x = point->x;
    area->m_shape.field_0x0.z = point->z;
}

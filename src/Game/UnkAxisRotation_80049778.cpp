/* Rotation matrices about an axis (file and function meanings inferred), which many units call by
 * their fn_ names: one returned by value and one written through the matrix's element accessor.
 * The unit is split by its .sdata2 (1.0 and 0.0, after the previous unit's own 0.0). Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching (1/2): fn_80049778 (86%) has retail's instructions, but GC/2.7 schedules y * z
 * earlier than retail does; about 12 source shapes (temporaries, order of the elements and of the
 * operands) were tried. */

#include "Game/MathTypes.h"
#include <cmath>
#include <dolphin/types.h>

extern "C" {
/* A matrix element by column and row. */
float &fn_8040C17C(Mtx34f *m, int column, int row);
}

/* Sets m to the rotation by angle about axis (a unit vector). */
extern "C" void fn_800498A0(Mtx34f *m, const Vec3f *axis, float angle) {
    float x = axis->x;
    float y = axis->y;
    float z = axis->z;
    float s = std::sin(angle);
    float c = std::cos(angle);
    float t = 1.0f - c;
    fn_8040C17C(m, 0, 0) = t * (x * x) + c;
    fn_8040C17C(m, 1, 0) = t * (x * y) + z * s;
    fn_8040C17C(m, 2, 0) = t * (z * x) - y * s;
    fn_8040C17C(m, 0, 1) = t * (x * y) - z * s;
    fn_8040C17C(m, 1, 1) = t * (y * y) + c;
    fn_8040C17C(m, 2, 1) = t * (y * z) + x * s;
    fn_8040C17C(m, 0, 2) = t * (z * x) + y * s;
    fn_8040C17C(m, 1, 2) = t * (y * z) - x * s;
    fn_8040C17C(m, 2, 2) = t * (z * z) + c;
    fn_8040C17C(m, 0, 3) = 0.0f;
    fn_8040C17C(m, 1, 3) = 0.0f;
    fn_8040C17C(m, 2, 3) = 0.0f;
}

/* The rotation by angle about axis (a unit vector; callers take it as a return value). */
extern "C" void fn_80049778(Mtx34f *m, const Vec3f *axis, float angle) {
    float x = axis->x;
    float y = axis->y;
    float z = axis->z;
    float s = std::sin(angle);
    float c = std::cos(angle);
    float t = 1.0f - c;
    m->values[0][0] = t * (x * x) + c;
    m->values[0][1] = t * (x * y) - z * s;
    m->values[0][2] = t * (z * x) + y * s;
    m->values[0][3] = 0.0f;
    m->values[1][0] = t * (x * y) + z * s;
    m->values[1][1] = t * (y * y) + c;
    m->values[1][2] = t * (y * z) - x * s;
    m->values[1][3] = 0.0f;
    m->values[2][0] = t * (z * x) - y * s;
    m->values[2][1] = t * (y * z) + x * s;
    m->values[2][2] = t * (z * z) + c;
    m->values[2][3] = 0.0f;
}

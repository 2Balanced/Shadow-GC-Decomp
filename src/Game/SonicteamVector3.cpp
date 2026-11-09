/* The game's three-component vector routines (Vec3f, Game/MathTypes.h): sums, differences,
 * scaling, length, normalization, dot and cross products, and the zero vector other units read
 * (lbl_805E07A0), built when the game starts. Other units call them by their fn_ names, so they keep
 * them; the file name is invented and everything here is named from what the code does. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#define fn_8040CD74 fn_8040CD74_declared
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#undef fn_8040CD74

#include <dolphin/types.h>

extern "C" {
float fn_8000FEE4(float x); /* square root */

Vec3f fn_8040CD74(const Vec3f *lhs, const Vec3f *rhs);
Vec3f fn_8040CDA8(const Vec3f *v, float scale);

Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CE04(Vec3f *v, const Vec3f *value);
void fn_8040CE38(Vec3f *v, float scale);
void fn_8040CE60(Vec3f *v);
float fn_8040CE74(Vec3f *v);
Fn8040CopyVector3 fn_8040CEE8(const Vec3f *v);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *v, const Vec3f *value);
}

const Vec3f lbl_805E07A0(0.0f, 0.0f, 0.0f);

void fn_8040CFF8(Vec3f *v, const Vec3f *value) {
    v->x += value->x;
    v->y += value->y;
    v->z += value->z;
}

Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b) {
    return Vec3f(a->y * b->z - a->z * b->y, a->z * b->x - a->x * b->z, a->x * b->y - a->y * b->x);
}

float fn_8040CF90(const Vec3f *a, const Vec3f *b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

float fn_8040CF50(const Vec3f *v) {
    return fn_8000FEE4(v->x * v->x + v->y * v->y + v->z * v->z);
}

float fn_8040CF2C(const Vec3f *v) {
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

Fn8040CopyVector3 fn_8040CEE8(const Vec3f *v) {
    Fn8040CopyVector3 result(*v);
    fn_8040CE74(reinterpret_cast<Vec3f *>(&result));
    return result;
}

float fn_8040CE74(Vec3f *v) {
    float length = fn_8040CF50(v);
    if (length < 0.000001f) {
        fn_800091D0(v, &lbl_805E07A0);
        return 0.0f;
    }
    v->x /= length;
    v->y /= length;
    v->z /= length;
    return length;
}

void fn_8040CE60(Vec3f *v) {
    v->x = v->y = v->z = 0.0f;
}

void fn_8040CE38(Vec3f *v, float scale) {
    v->x *= scale;
    v->y *= scale;
    v->z *= scale;
}

void fn_8040CE04(Vec3f *v, const Vec3f *value) {
    v->x -= value->x;
    v->y -= value->y;
    v->z -= value->z;
}

Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b) {
    return Vec3f(a->x + b->x, a->y + b->y, a->z + b->z);
}

Vec3f fn_8040CDA8(const Vec3f *v, float scale) {
    return Vec3f(v->x * scale, v->y * scale, v->z * scale);
}

Vec3f fn_8040CD74(const Vec3f *lhs, const Vec3f *rhs) {
    return Vec3f(lhs->x - rhs->x, lhs->y - rhs->y, lhs->z - rhs->z);
}

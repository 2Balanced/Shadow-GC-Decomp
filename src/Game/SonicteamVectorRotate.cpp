/* Vector helpers built on the vector routines of Game/SonicteamVector3.cpp: a vector turned about
 * the z, y or x axis, and the squared and plain distance between two points. Other units call them
 * by their fn_ names, so they keep them; the file name is invented and everything here is named
 * from what the code does. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/MathTypes.h"
#include <dolphin/types.h>

void SinCos(float *s, float *c, float angle);

extern "C" {
float fn_8040CF2C(const Vec3f *v);
float fn_8040CF50(const Vec3f *v);

Vec3f fn_8040D044(const Vec3f *v, float angle);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040D118(const Vec3f *v, float angle);
float fn_8040D180(const Vec3f *a, const Vec3f *b);
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
}

typedef Vec3f (*Vec3fBinaryFn)(const Vec3f *a, const Vec3f *b);

float fn_8040D1B4(const Vec3f *a, const Vec3f *b) {
    return fn_8040CF50(&(const Vec3f &)((Vec3fBinaryFn)fn_8040CD74)(a, b));
}

float fn_8040D180(const Vec3f *a, const Vec3f *b) {
    return fn_8040CF2C(&(const Vec3f &)((Vec3fBinaryFn)fn_8040CD74)(a, b));
}

Vec3f fn_8040D118(const Vec3f *v, float angle) {
    float s;
    float c;
    SinCos(&s, &c, angle);
    return Vec3f(v->x, v->y * c - v->z * s, v->y * s + v->z * c);
}

Vec3f fn_8040D0AC(const Vec3f *v, float angle) {
    float s;
    float c;
    SinCos(&s, &c, angle);
    return Vec3f(v->x * c + v->z * s, v->y, -v->x * s + v->z * c);
}

Vec3f fn_8040D044(const Vec3f *v, float angle) {
    float s;
    float c;
    SinCos(&s, &c, angle);
    return Vec3f(v->x * c - v->y * s, v->x * s + v->y * c, v->z);
}

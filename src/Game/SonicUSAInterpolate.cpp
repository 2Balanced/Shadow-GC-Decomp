/* Interpolation helpers of the SonicUSA library (built, like Game/SonicUSAAngle.cpp, without small
 * data): between two matrices (their rotations slerped as quaternions, their translations lerped),
 * two sets of Euler angles (through quaternions), two vectors and two angles (the short way round).
 * Each returns the first value for t <= 0 and the second for t >= 1. Other units call them by their
 * fn_ names, so they keep them; the file name is invented and everything here is named from what
 * the code does. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include "Game/Quaternion.h"
#include <dolphin/types.h>

/* A matrix returned by value and copied through fn_8040C148 (struct name invented). */
struct UnkLerpMatrix_8040EFA4 : public Mtx34f {
    UnkLerpMatrix_8040EFA4(const Mtx34f &m) {
        fn_8040C148(this, &m);
    }
    UnkLerpMatrix_8040EFA4(const UnkLerpMatrix_8040EFA4 &m) {
        fn_8040C148(this, &m);
    }
};

Fn8040CopyVector3 operator-(const Vec3f &a, const Vec3f &b);

extern "C" {
float fn_8040C168(const Mtx34f *m, int row, int column);
float *fn_8040C17C(Mtx34f *m, int row, int column);
Quatf fn_8040C3EC(const Quatf &from, const Quatf &to, float t);
Quatf fn_8040C55C(const Mtx34f *m);
UnkLerpMatrix_8040EFA4 fn_8040B228(const Quatf &q);
Fn8040CopyVector3 fn_800091A4(const Fn8040CopyVector3 &v, float scale);
Fn8040CopyVector3 fn_8040CDD0(const Vec3f &a, const Fn8040CopyVector3 &b);
Quatf fn_804102F4(const Vec3f *angles, int order);
Vec3f fn_80410204(const Quatf &q, int order);
float fn_8040E99C(float angle);

float fn_8040F314(float a, float b, float t);
Fn8040CopyVector3 fn_8040F268(const Vec3f *a, const Vec3f *b, float t);
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
UnkLerpMatrix_8040EFA4 fn_8040EFA4(const Mtx34f *a, const Mtx34f *b, float t);
}

/* An angle between a and b, the short way round. */
float fn_8040F314(float a, float b, float t) {
    float difference = fn_8040E99C(b - a);
    return a + difference * t;
}

Fn8040CopyVector3 fn_8040F268(const Vec3f *a, const Vec3f *b, float t) {
    if (t <= 0.0f) {
        return Fn8040CopyVector3(*a);
    }
    if (t >= 1.0f) {
        return Fn8040CopyVector3(*b);
    }
    return fn_8040CDD0(*a, fn_800091A4(*b - *a, t));
}

/* Euler angles between a and b, in the axis order order. */
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t) {
    if (t <= 0.0f) {
        return *a;
    }
    if (t >= 1.0f) {
        return *b;
    }
    Quatf from(fn_804102F4(a, order));
    Quatf to(fn_804102F4(b, order));
    return fn_80410204(fn_8040C3EC(from, to, t), order);
}

UnkLerpMatrix_8040EFA4 fn_8040EFA4(const Mtx34f *a, const Mtx34f *b, float t) {
    if (t <= 0.0f) {
        return *a;
    }
    if (t >= 1.0f) {
        return *b;
    }
    Quatf from(fn_8040C55C(a));
    Quatf to(fn_8040C55C(b));
    UnkLerpMatrix_8040EFA4 m(fn_8040B228(fn_8040C3EC(from, to, t)));
    float s = 1.0f - t;
    *fn_8040C17C(&m, 0, 3) = s * fn_8040C168(a, 0, 3) + t * fn_8040C168(b, 0, 3);
    *fn_8040C17C(&m, 1, 3) = s * fn_8040C168(a, 1, 3) + t * fn_8040C168(b, 1, 3);
    *fn_8040C17C(&m, 2, 3) = s * fn_8040C168(a, 2, 3) + t * fn_8040C168(b, 2, 3);
    return m;
}

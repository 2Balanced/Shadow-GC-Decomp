/* Euler-angle and vector geometry helpers of the SonicUSA library (built, like
 * Game/SonicUSAAngle.cpp, without small data): Euler angles from a direction and up vector, from
 * a matrix basis and from a quaternion; rotating a matrix or a vector by Euler angles in one of
 * three axis orders and building the quaternion of a set of angles; the angle between two vectors;
 * splitting a vector along a normal; a matrix's translation; the nearest point of a segment and
 * whether a point lies across it; and the rotation matrix about an axis. Other units call them by
 * their fn_ names, so they keep them; the file name is invented and everything here is named from
 * what the code does. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <cmath>

#define fn_8040CD74 fn_8040CD74_declared
#define fn_8040CF2C fn_8040CF2C_declared
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#undef fn_8040CD74
#undef fn_8040CF2C
#include "Game/MathConstants.h"
#include "Game/Quaternion.h"
#include <dolphin/types.h>

/* A matrix returned by value and copied through fn_8040C148 (struct name invented; as in
 * Game/SonicUSAInterpolate.cpp). */
struct UnkEulerMatrix_8040FEBC : public Mtx34f {
    UnkEulerMatrix_8040FEBC(const Mtx34f &m) {
        fn_8040C148(this, &m);
    }
    UnkEulerMatrix_8040FEBC(const UnkEulerMatrix_8040FEBC &m) {
        fn_8040C148(this, &m);
    }
};

/* A matrix built from its twelve values (as in Game/SonicteamMatrix.cpp; struct name invented). */
struct UnkMatrixValues_8040FD8C : public Mtx34f {
    UnkMatrixValues_8040FD8C(float m00, float m01, float m02, float m03, float m10, float m11, float m12,
                             float m13, float m20, float m21, float m22, float m23) {
        values[0][0] = m00;
        values[0][1] = m01;
        values[0][2] = m02;
        values[0][3] = m03;
        values[1][0] = m10;
        values[1][1] = m11;
        values[1][2] = m12;
        values[1][3] = m13;
        values[2][0] = m20;
        values[2][1] = m21;
        values[2][2] = m22;
        values[2][3] = m23;
    }
};

/* A line segment (struct name invented). */
struct UnkSegment_8040FB88 {
    /* 0x0 */ Vec3f m_start;
    /* 0xC */ Vec3f m_end;
};

void SinCos(float *s, float *c, float angle);

/* The float acos the game calls out of line (as in Game/SonicteamQuaternion.cpp). */
namespace std {
float acos(float x);
} // namespace std

extern "C" {
float fn_8000FEE4(float x);           /* square root */
float fn_80016260(float y, float x);  /* atan2 */
bool fn_8000A950(float x);            /* whether x is about zero */
Fn8040CopyVector3 fn_80016284(const Vec3f &v);
float fn_8040C168(const Mtx34f *m, int row, int column);
float *fn_8040C17C(Mtx34f *m, int row, int column);
Quatf fn_8040C55C(const Mtx34f *m);
UnkEulerMatrix_8040FEBC fn_8040B228(const Quatf &q);
Fn8040CopyVector3 fn_8040B67C(const Mtx34f *m, const Vec3f *v);
Fn8040CopyVector3 fn_8040CFB8(const Vec3f &a, const Vec3f &b);
Fn8040CopyVector3 fn_8040CEE8(const Vec3f &v);
float fn_8040CF90(const Vec3f &a, const Vec3f &b);
float fn_8040CF2C(const Vec3f &v);
Fn8040CopyVector3 fn_8040CD74(const Vec3f &a, const Vec3f &b);
Fn8040CopyVector3 fn_8040CDA8(const Vec3f &v, float scale);
Fn8040CopyVector3 fn_800091A4(const Fn8040CopyVector3 &v, float scale);
Fn8040CopyVector3 fn_8040CDD0(const Vec3f &a, const Fn8040CopyVector3 &b);
Fn8040CopyVector3 fn_8040D044(const Vec3f &v, float angle);
Fn8040CopyVector3 fn_8040D0AC(const Vec3f &v, float angle);
Fn8040CopyVector3 fn_8040D118(const Vec3f &v, float angle);

Quatf fn_804102F4(const Vec3f *angles, int order);
Vec3f fn_80410204(const Quatf &q, int order);
Fn8040CopyVector3 fn_80410098(const Vec3f *v, const Vec3f *angles, int order);
UnkEulerMatrix_8040FEBC fn_8040FFBC(const Mtx34f *m, const Vec3f *angles, int order);
UnkEulerMatrix_8040FEBC fn_8040FEBC(const Mtx34f *m, const Vec3f *angles, int order);
UnkMatrixValues_8040FD8C fn_8040FD8C(const Vec3f *axis, float angle);
Fn8040CopyVector3 fn_8040FC78(float *t, const UnkSegment_8040FB88 *segment, const Vec3f *point);
Fn8040CopyVector3 fn_8040FC3C(const UnkSegment_8040FB88 *segment, const Vec3f *point);
bool fn_8040FB88(const UnkSegment_8040FB88 *segment, const Vec3f *point);
Vec3f fn_8040FB08(const Mtx34f *m);
void fn_8040FAA0(Vec3f *out, const Mtx34f *m);
void fn_8040FA20(Mtx34f *m, const Vec3f *translation);
void fn_8040F990(Fn8040CopyVector3 *along, Fn8040CopyVector3 *across, const Vec3f *v, const Vec3f *normal);
float fn_8040F8FC(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040F7F8(const Vec3f *direction);
Vec2f fn_8040F760(const Vec3f *direction);
Vec2f fn_8040F664(const Vec3f *direction);
Vec3f fn_8040F5F4(const Vec3f *a, const Vec3f *b, int kind);
Vec3f fn_8040F450(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040F364(const Vec3f *direction, const Vec3f *up);
}

/* The quaternion of Euler angles in an axis order. */
Quatf fn_804102F4(const Vec3f *angles, int order) {
    UnkEulerMatrix_8040FEBC m(lbl_805E0720);
    switch (order) {
    case 0:
        fn_8040B80C(&m, angles->z);
        fn_8040B848(&m, angles->y);
        fn_8040B884(&m, angles->x);
        break;
    case 1:
        fn_8040B80C(&m, angles->z);
        fn_8040B884(&m, angles->x);
        fn_8040B848(&m, angles->y);
        break;
    case 2:
        fn_8040B848(&m, angles->y);
        fn_8040B884(&m, angles->x);
        fn_8040B80C(&m, angles->z);
        break;
    }
    return Quatf(fn_8040C55C(&m));
}

/* The Euler angles of a quaternion (only the order 2 is supported). */
Vec3f fn_80410204(const Quatf &q, int order) {
    UnkEulerMatrix_8040FEBC m(fn_8040B228(q));
    Vec3f up;
    Vec3f forward;
    switch (order) {
    case 0:
    case 1:
        break;
    case 2:
        fn_800091D0(&up, &(const Vec3f &)fn_8040B67C(&m, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f)));
        fn_800091D0(&forward, &(const Vec3f &)fn_8040B67C(&m, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f)));
        return fn_8040F364(&forward, &up);
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
}

/* v rotated by Euler angles in an axis order. */
Fn8040CopyVector3 fn_80410098(const Vec3f *v, const Vec3f *angles, int order) {
    Fn8040CopyVector3 result(*v);
    switch (order) {
    case 0:
        result = fn_8040D118((const Vec3f &)result, angles->x);
        result = fn_8040D0AC((const Vec3f &)result, angles->y);
        result = fn_8040D044((const Vec3f &)result, angles->z);
        break;
    case 1:
        result = fn_8040D0AC((const Vec3f &)result, angles->y);
        result = fn_8040D118((const Vec3f &)result, angles->x);
        result = fn_8040D044((const Vec3f &)result, angles->z);
        break;
    case 2:
        result = fn_8040D044((const Vec3f &)result, angles->z);
        result = fn_8040D118((const Vec3f &)result, angles->x);
        result = fn_8040D0AC((const Vec3f &)result, angles->y);
        break;
    }
    return result;
}

/* m rotated by Euler angles in an axis order. */
UnkEulerMatrix_8040FEBC fn_8040FFBC(const Mtx34f *m, const Vec3f *angles, int order) {
    UnkEulerMatrix_8040FEBC result(*m);
    switch (order) {
    case 0:
        fn_8040B80C(&result, angles->z);
        fn_8040B848(&result, angles->y);
        fn_8040B884(&result, angles->x);
        break;
    case 1:
        fn_8040B80C(&result, angles->z);
        fn_8040B884(&result, angles->x);
        fn_8040B848(&result, angles->y);
        break;
    case 2:
        fn_8040B848(&result, angles->y);
        fn_8040B884(&result, angles->x);
        fn_8040B80C(&result, angles->z);
        break;
    }
    return result;
}

/* m rotated back by Euler angles (the inverse of fn_8040FFBC). */
UnkEulerMatrix_8040FEBC fn_8040FEBC(const Mtx34f *m, const Vec3f *angles, int order) {
    UnkEulerMatrix_8040FEBC result(*m);
    switch (order) {
    case 0:
        fn_8040B884(&result, -angles->x);
        fn_8040B848(&result, -angles->y);
        fn_8040B80C(&result, -angles->z);
        break;
    case 1:
        fn_8040B848(&result, -angles->y);
        fn_8040B884(&result, -angles->x);
        fn_8040B80C(&result, -angles->z);
        break;
    case 2:
        fn_8040B80C(&result, -angles->z);
        fn_8040B884(&result, -angles->x);
        fn_8040B848(&result, -angles->y);
        break;
    }
    return result;
}

/* The rotation matrix by angle about a unit axis. */
UnkMatrixValues_8040FD8C fn_8040FD8C(const Vec3f *axis, float angle) {
    float x = axis->x;
    float y = axis->y;
    float z = axis->z;
    float s = std::sin(angle);
    float c = std::cos(angle);
    float t = 1.0f - c;
    return UnkMatrixValues_8040FD8C(t * (x * x) + c, t * (x * y) - z * s, t * (z * x) + y * s, 0.0f,
                                    t * (x * y) + z * s, t * (y * y) + c, t * (y * z) - x * s, 0.0f,
                                    t * (z * x) - y * s, t * (y * z) + x * s, t * (z * z) + c, 0.0f);
}

/* The point of a segment nearest to point, and where it lies along it (0 to 1). */
Fn8040CopyVector3 fn_8040FC78(float *t, const UnkSegment_8040FB88 *segment, const Vec3f *point) {
    Fn8040CopyVector3 direction(fn_8040CD74(segment->m_end, segment->m_start));
    Fn8040CopyVector3 offset(fn_8040CD74(*point, segment->m_start));
    float lengthSq = fn_8040CF2C((const Vec3f &)direction);
    float along = fn_8000A950(lengthSq) ? 1.0f : fn_8040CF90((const Vec3f &)direction, (const Vec3f &)offset) / lengthSq;
    if (t != 0) {
        *t = along;
    }
    if (along < 0.0f) {
        return Fn8040CopyVector3(segment->m_start);
    }
    if (along > 1.0f) {
        return Fn8040CopyVector3(segment->m_end);
    }
    return fn_8040CDD0(segment->m_start, fn_800091A4(direction, along));
}

/* The point of a segment nearest to point. */
Fn8040CopyVector3 fn_8040FC3C(const UnkSegment_8040FB88 *segment, const Vec3f *point) {
    return fn_8040FC78(0, segment, point);
}

/* Whether point lies between the planes through the segment's ends across it. */
bool fn_8040FB88(const UnkSegment_8040FB88 *segment, const Vec3f *point) {
    Fn8040CopyVector3 direction(fn_8040CD74(segment->m_end, segment->m_start));
    if (fn_8040CF90((const Vec3f &)fn_8040CD74(*point, segment->m_start), (const Vec3f &)direction) < 0.0f) {
        return false;
    }
    return !(fn_8040CF90((const Vec3f &)fn_8040CD74(*point, segment->m_end), (const Vec3f &)direction) > 0.0f);
}

/* A matrix's translation. */
Vec3f fn_8040FB08(const Mtx34f *m) {
    return Vec3f(fn_8040C168(m, 0, 3), fn_8040C168(m, 1, 3), fn_8040C168(m, 2, 3));
}

/* A matrix's translation. */
void fn_8040FAA0(Vec3f *out, const Mtx34f *m) {
    out->x = fn_8040C168(m, 0, 3);
    out->y = fn_8040C168(m, 1, 3);
    out->z = fn_8040C168(m, 2, 3);
}

/* Sets a matrix's translation. */
void fn_8040FA20(Mtx34f *m, const Vec3f *translation) {
    *fn_8040C17C(m, 0, 3) = translation->x;
    *fn_8040C17C(m, 1, 3) = translation->y;
    *fn_8040C17C(m, 2, 3) = translation->z;
}

/* Splits v into its part along normal and the rest. */
void fn_8040F990(Fn8040CopyVector3 *along, Fn8040CopyVector3 *across, const Vec3f *v, const Vec3f *normal) {
    Fn8040CopyVector3 projection(fn_8040CDA8(*normal, fn_8040CF90(*v, *normal)));
    if (across != 0) {
        *across = fn_8040CD74(*v, (const Vec3f &)projection);
    }
    if (along != 0) {
        *along = projection;
    }
}

/* The angle between two vectors. */
float fn_8040F8FC(const Vec3f *a, const Vec3f *b) {
    Fn8040CopyVector3 unitA(fn_8040CEE8(*a));
    Fn8040CopyVector3 unitB(fn_8040CEE8(*b));
    float cosine = fn_8040CF90((const Vec3f &)unitA, (const Vec3f &)unitB);
    if (cosine > 1.0f) {
        cosine = 1.0f;
    } else if (cosine < -1.0f) {
        cosine = -1.0f;
    }
    return std::acos(cosine);
}

/* The pitch and yaw of a direction, with no roll. */
Vec3f fn_8040F7F8(const Vec3f *direction) {
    Vec3f result;
    result.y = fn_80016260(direction->x, direction->z);
    float yaw = result.y;
    float t;
    float pitch;
    if (std::fabs(direction->x) > std::fabs(direction->z)) {
        t = std::sin(yaw);
        pitch = fn_80016260(-direction->y * t, direction->x);
    } else {
        t = std::cos(yaw);
        pitch = fn_80016260(-direction->y * t, direction->z);
    }
    if (t < 0.0f) {
        pitch += pitch < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
    }
    result.x = pitch;
    result.z = 0.0f;
    return result;
}

/* The pitch and yaw of a direction (looking down -z). */
Vec2f fn_8040F760(const Vec3f *direction) {
    float length = fn_8000FEE4(direction->x * direction->x + direction->z * direction->z);
    float yaw = fn_80016260(-direction->x, -direction->z);
    float pitch = fn_80016260(direction->y, length);
    return Vec2f(pitch, yaw);
}

/* The pitch and yaw of a direction. */
Vec2f fn_8040F664(const Vec3f *direction) {
    float yaw = fn_80016260(direction->x, direction->z);
    float t;
    float pitch;
    if (std::fabs(direction->x) > std::fabs(direction->z)) {
        t = std::sin(yaw);
        pitch = fn_80016260(-direction->y * t, direction->x);
    } else {
        t = std::cos(yaw);
        pitch = fn_80016260(-direction->y * t, direction->z);
    }
    if (t < 0.0f) {
        pitch += pitch < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
    }
    return Vec2f(pitch, yaw);
}

Vec3f fn_8040F5F4(const Vec3f *a, const Vec3f *b, int kind) {
    switch (kind) {
    case 0:
        return fn_8040F450(a, b);
    case 2:
        return fn_8040F364(a, b);
    case 1:
    default:
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
}

/* Euler angles of the basis whose axes are b and the cross product of a and b. */
Vec3f fn_8040F450(const Vec3f *a, const Vec3f *b) {
    Fn8040CopyVector3 axis;
    Fn8040CopyVector3 second;
    Fn8040CopyVector3 third;
    axis = fn_8040CFB8(*a, *b);
    second = *(const Fn8040CopyVector3 *)b;
    third = fn_80016284(*a);
    float yaw = fn_80016260(axis.y, axis.x);
    float s;
    float c;
    SinCos(&s, &c, yaw);
    float pitch;
    if (std::fabs(axis.x) > std::fabs(axis.y)) {
        pitch = fn_80016260(-axis.z * c, axis.x);
        if (c < 0.0f) {
            pitch += pitch < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
        }
    } else {
        pitch = fn_80016260(-axis.z * s, axis.y);
        if (s < 0.0f) {
            pitch += pitch < 0.0f ? lbl_805FAD38 : -lbl_805FAD38;
        }
    }
    float roll = fn_80016260(third.x * s - third.y * c, second.y * c - second.x * s);
    return Vec3f(roll, pitch, yaw);
}

/* Euler angles of a direction and the up vector that sets the roll. */
Vec3f fn_8040F364(const Vec3f *direction, const Vec3f *up) {
    float length = fn_8000FEE4(direction->x * direction->x + direction->z * direction->z);
    float yaw = fn_80016260(-direction->x, -direction->z);
    float pitch = fn_80016260(direction->y, length);
    Fn8040CopyVector3 rolled(fn_8040D0AC(*up, -yaw));
    rolled = fn_8040D118((const Vec3f &)rolled, -pitch);
    return Vec3f(pitch, yaw, fn_80016260(-rolled.x, rolled.y));
}

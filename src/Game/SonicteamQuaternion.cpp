/* The game's quaternion routines (Quatf, Game/Quaternion.h) - interpolation, conversion from a
 * matrix and from an axis and angle, products, normalization - with the zero and identity
 * quaternions built when the game starts, and the distance of a point from a plane and its
 * projection onto it. Other units call them by
 * their fn_ names, so they keep them; the library's own names are unknown and everything here is
 * named from what the code does. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#define fn_8040CA10 fn_8040CA10_declared
#define fn_8040C924 fn_8040C924_declared
#include "Game/Quaternion.h"
#undef fn_8040CA10
#undef fn_8040C924
#include <cmath>
#include <dolphin/types.h>
#include "size_assert.h"

/* A plane through a point (member names inferred). */
struct UnkPlane_8040C3AC {
    /* 0x0 */ Vec3f m_point;
    /* 0xC */ Vec3f m_normal;
};

void SinCos(float *s, float *c, float angle);

/* The float acos the game calls out of line (0x801B1DB0; the MSL stand-in has no float overload). */
namespace std {
float acos(float x);
}

extern "C" {
float fn_8000FEE4(float x); /* square root */
float fn_8040C168(const Mtx34f *m, int row, int column);
void fn_8040CDA8(Vec3f *out, const Vec3f *v, float scale);
void fn_8040CEE8(Vec3f *out, const Vec3f *v);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);

float fn_8040C340(const UnkPlane_8040C3AC *plane, const Vec3f *point, Vec3f *out);
float fn_8040C3AC(const UnkPlane_8040C3AC *plane, const Vec3f *point);
Quatf fn_8040C3EC(const Quatf *from, const Quatf *to, float t);
Quatf fn_8040C55C(const Mtx34f *m);
Quatf fn_8040C874(const Vec3f *axis, float angle);
Quatf fn_8040C908(void);
Quatf fn_8040C924(const Quatf *q, float scale);
Quatf fn_8040CA10(const Quatf *a, const Quatf *b);

float fn_8040CA84(const Quatf *q);
}

typedef Vec3f (*Vec3fBinaryFn)(const Vec3f *a, const Vec3f *b);
typedef Vec3f (*Vec3fScaleFn)(const Vec3f *v, float scale);
typedef Vec3f (*Vec3fUnaryFn)(const Vec3f *v);

#define V(v) (reinterpret_cast<Vec3f *>(&(v)))

Quatf lbl_805E0780(0.0f, 0.0f, 0.0f, 0.0f);
Quatf lbl_805E0790(0.0f, 0.0f, 0.0f, 1.0f);

/* The dot product of two quaternions (name inferred). */
inline float Dot(const Quatf &a, const Quatf &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

// FNS

void fn_8040CAB4(Quatf *q, const Quatf *value) {
    q->x += value->x;
    q->y += value->y;
    q->z += value->z;
    q->w += value->w;
}

float fn_8040CA84(const Quatf *q) {
    return q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w;
}

Quatf fn_8040CA10(const Quatf *a, const Quatf *b) {
    return Quatf(a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y,
                 a->w * b->y + a->y * b->w + a->z * b->x - a->x * b->z,
                 a->w * b->z + a->z * b->w + a->x * b->y - a->y * b->x,
                 a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z);
}

void fn_8040C958(Quatf *q) {
    float lengthSq = q->x * q->x + q->y * q->y + q->z * q->z + q->w * q->w;
    if (lengthSq >= 0.00001f) {
        float inv = 1.0f / fn_8000FEE4(lengthSq);
        q->x *= inv;
        q->y *= inv;
        q->z *= inv;
        q->w *= inv;
    } else {
        q->x = q->y = q->z = q->w = 0.0f;
    }
}

Quatf fn_8040C924(const Quatf *q, float scale) {
    return Quatf(q->x * scale, q->y * scale, q->z * scale, q->w * scale);
}

Quatf fn_8040C908(void) {
    return Quatf(0.0f, 0.0f, 0.0f, 1.0f);
}

Quatf fn_8040C874(const Vec3f *axis, float angle) {
    Fn8040CopyVector3 n(((Vec3fUnaryFn)fn_8040CEE8)(axis));
    float s;
    float c;
    SinCos(&s, &c, 0.5f * angle);
    return Quatf(s * n.x, s * n.y, s * n.z, c);
}

Quatf fn_8040C55C(const Mtx34f *m) {
    static const int next[3] = {1, 2, 0};
    float trace = fn_8040C168(m, 0, 0) + fn_8040C168(m, 1, 1) + fn_8040C168(m, 2, 2);
    if (trace > 0.0f) {
        float s = fn_8000FEE4(trace + 1.0f);
        float h = 0.5f / s;
        return Quatf(h * (fn_8040C168(m, 2, 1) - fn_8040C168(m, 1, 2)), h * (fn_8040C168(m, 0, 2) - fn_8040C168(m, 2, 0)),
                     h * (fn_8040C168(m, 1, 0) - fn_8040C168(m, 0, 1)), 0.5f * s);
    }
    float q[3];
    int i = 0;
    if (fn_8040C168(m, 1, 1) > fn_8040C168(m, 0, 0)) {
        i = 1;
    }
    if (fn_8040C168(m, 2, 2) > fn_8040C168(m, i, i)) {
        i = 2;
    }
    int j = next[i];
    int k = next[j];
    float s = fn_8000FEE4(fn_8040C168(m, i, i) - (fn_8040C168(m, j, j) + fn_8040C168(m, k, k)) + 1.0f);
    q[i] = 0.5f * s;
    if (s != 0.0f) {
        s = 0.5f / s;
    }
    q[j] = s * (fn_8040C168(m, i, j) + fn_8040C168(m, j, i));
    q[k] = s * (fn_8040C168(m, i, k) + fn_8040C168(m, k, i));
    return Quatf(q[0], q[1], q[2], s * (fn_8040C168(m, k, j) - fn_8040C168(m, j, k)));
}

Quatf fn_8040C3EC(const Quatf *from, const Quatf *to, float t) {
    float s0;
    float cosine = Dot(*from, *to);
    float s1 = 1.0f;
    if (cosine < 0.0f) {
        cosine = -cosine;
        s1 = -s1;
    }
    if (cosine <= 0.99999f) {
        float angle = std::acos(cosine);
        float sine = std::sin(angle);
        s0 = std::sin((1.0f - t) * angle) / sine;
        s1 *= std::sin(t * angle) / sine;
    } else {
        s1 *= t;
        s0 = 1.0f - t;
    }

    return Quatf(s0 * from->x + s1 * to->x, s0 * from->y + s1 * to->y, s0 * from->z + s1 * to->z,
                 s0 * from->w + s1 * to->w);
}

float fn_8040C3AC(const UnkPlane_8040C3AC *plane, const Vec3f *point) {
    return fn_8040CF90(&(const Vec3f &)((Vec3fBinaryFn)fn_8040CD74)(point, &plane->m_point), &plane->m_normal);
}

float fn_8040C340(const UnkPlane_8040C3AC *plane, const Vec3f *point, Vec3f *out) {
    float distance = fn_8040C3AC(plane, point);
    fn_800091D0(out, &(const Vec3f &)((Vec3fBinaryFn)fn_8040CD74)(
                         point, &(const Vec3f &)((Vec3fScaleFn)fn_8040CDA8)(&plane->m_normal, distance)));
    return distance;
}

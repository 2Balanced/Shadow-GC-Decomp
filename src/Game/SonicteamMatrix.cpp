/* The game's 3x4 matrix routines (Mtx34f, Game/MathTypes.h): building translation, scale and
 * rotation matrices, products, transforms, inverses and the look-at matrix, and the identity matrix
 * other units read (lbl_805E0720). Other units call them by their fn_ names, so they keep them; the
 * library's own class and function names are unknown and everything here is named from what the
 * code does. The functions that build a matrix return it by value (the twelve-value constructor
 * below); written with an output pointer instead, GC/2.7 schedules their epilogues differently.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 27 of 28 functions match. fn_8040BCD4 (the in-place inverse) does not: retail
 * computes it like fn_8040BEF8 but fuses the determinant's middle cofactor (fmsubs) and recomputes
 * the products of row 1, column 0, where every spelling tried here (locals for the twelve values,
 * the inline Determinant, the determinant written out, SetValues or the constructor, an inverted
 * test) shares them as fn_8040BEF8 does, or reloads the values after the test. */
#define lbl_805E0720 lbl_805E0720_declared
#include "Game/MathTypes.h"
#undef lbl_805E0720
#include "Game/Fn8040CopyVector3.h"
#include "Game/Quaternion.h"
#include <dolphin/types.h>

/* The game's matrix as constructed from twelve values (struct name invented): the identity and
 * zero matrices are built this way when the game starts. */
struct UnkMatrixValues_805E0720 : public Mtx34f {
    UnkMatrixValues_805E0720(float m00, float m01, float m02, float m03, float m10, float m11, float m12,
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
    UnkMatrixValues_805E0720(const UnkMatrixValues_805E0720 &other);
};

/* The matrix copy through fn_8040C148, inline (name inferred). */
inline void CopyMatrix(Mtx34f *m, const Mtx34f *from) {
    fn_8040C148(m, from);
}

/* A matrix assigned through fn_8040C148 (as in Game/DevilDoomCtrlUnitBit.cpp; struct name
 * inferred). Its assignment calls an inline function, so GC/2.7 keeps it out of line, as retail
 * does here; the one copy is DevilDoomCtrlUnitBit's (0x80325384). */
struct UnkMatrix_80325384 : public Mtx34f {
    UnkMatrix_80325384 &operator=(const Mtx34f &matrix) {
        CopyMatrix(this, &matrix);
        return *this;
    }
};

void SinCos(float *s, float *c, float angle);

extern "C" {
float fn_8040CA84(const Quatf *q);
void fn_8040CE74(Vec3f *v);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
void fn_8040CFB8(Vec3f *out, const Vec3f *a, const Vec3f *b);

void fn_8040B07C(Mtx34f *m, const Vec3f *scale);
UnkMatrixValues_805E0720 fn_8040B0C0(float radians);
UnkMatrixValues_805E0720 fn_8040B138(float radians);
UnkMatrixValues_805E0720 fn_8040B1B0(float radians);
UnkMatrixValues_805E0720 fn_8040B228(const Quatf *q);
UnkMatrixValues_805E0720 fn_8040B33C(const Vec3f *eye, const Vec3f *up, const Vec3f *target);
void fn_8040B4D0(Mtx34f *out, const Mtx34f *m);
void fn_8040B52C(Mtx34f *m, float x, float y, float z);
Vec3f fn_8040B618(const Mtx34f *m, const Vec3f *v);
Vec3f fn_8040B67C(const Mtx34f *m, const Vec3f *v);
void fn_8040B790(Mtx34f *m, const Vec3f *scale);
void fn_8040B8C0(Mtx34f *m, const Quatf *q);
UnkMatrixValues_805E0720 fn_8040B8FC(const Mtx34f *a, const Mtx34f *b);
void fn_8040BAC4(Mtx34f *m, const Mtx34f *other);
void fn_8040BCD4(Mtx34f *m);
UnkMatrixValues_805E0720 fn_8040BEF8(const Mtx34f *m);
float fn_8040C168(const Mtx34f *m, int row, int column);
float *fn_8040C17C(Mtx34f *m, int row, int column);
}

extern const UnkMatrixValues_805E0720 lbl_805E0720(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                                   1.0f, 0.0f);
extern const UnkMatrixValues_805E0720 lbl_805E0750(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                                   0.0f, 0.0f);

#define M(m, r, c) ((m)->values[r][c])

/* Vector difference and cross product (fn_8040CD74, fn_8040CFB8), called as returning their result by
 * value. */
typedef Vec3f (*Vec3fBinaryFn)(const Vec3f *a, const Vec3f *b);

#define V(v) (reinterpret_cast<Vec3f *>(&(v)))

/* Sets all twelve values (name inferred; the game's matrix class sets itself this way). */
inline void SetValues(Mtx34f *m, float m00, float m01, float m02, float m03, float m10, float m11, float m12,
                      float m13, float m20, float m21, float m22, float m23) {
    M(m, 0, 0) = m00;
    M(m, 0, 1) = m01;
    M(m, 0, 2) = m02;
    M(m, 0, 3) = m03;
    M(m, 1, 0) = m10;
    M(m, 1, 1) = m11;
    M(m, 1, 2) = m12;
    M(m, 1, 3) = m13;
    M(m, 2, 0) = m20;
    M(m, 2, 1) = m21;
    M(m, 2, 2) = m22;
    M(m, 2, 3) = m23;
}

// FNS

float *fn_8040C17C(Mtx34f *m, int row, int column) {
    return &(&M(m, 0, 0))[column + row * 4];
}

float fn_8040C168(const Mtx34f *m, int row, int column) {
    return (&M(m, 0, 0))[column + row * 4];
}

void fn_8040C148(Mtx34f *m, const Mtx34f *from) {
    for (int i = 0; i < 12; i++) {
        (&M(m, 0, 0))[i] = (&M(from, 0, 0))[i];
    }
}

#define m00 M(m, 0, 0)
#define m01 M(m, 0, 1)
#define m02 M(m, 0, 2)
#define m03 M(m, 0, 3)
#define m10 M(m, 1, 0)
#define m11 M(m, 1, 1)
#define m12 M(m, 1, 2)
#define m13 M(m, 1, 3)
#define m20 M(m, 2, 0)
#define m21 M(m, 2, 1)
#define m22 M(m, 2, 2)
#define m23 M(m, 2, 3)
/* The determinant of the rotation part (name inferred). */
inline float Determinant(const Mtx34f *m) {
    return m00 * (m11 * m22 - m12 * m21) + m01 * (m12 * m20 - m10 * m22) + m02 * (m10 * m21 - m11 * m20);
}

UnkMatrixValues_805E0720 fn_8040BEF8(const Mtx34f *m) {
    float det = Determinant(m);
    if (det == 0.0f) {
        return *static_cast<const UnkMatrixValues_805E0720 *>(m);
    }
    float inv = 1.0f / det;
    return UnkMatrixValues_805E0720(
        inv * (m11 * m22 - m12 * m21), inv * -(m01 * m22 - m02 * m21), inv * (m01 * m12 - m02 * m11),
        inv * -(m01 * m12 * m23 + m02 * m13 * m21 + m03 * m11 * m22 - m03 * m12 * m21 - m02 * m11 * m23 - m01 * m13 * m22),
        inv * -(m10 * m22 - m12 * m20), inv * (m00 * m22 - m02 * m20), inv * -(m00 * m12 - m02 * m10),
        inv * (m00 * m12 * m23 + m02 * m13 * m20 + m03 * m10 * m22 - m03 * m12 * m20 - m02 * m10 * m23 - m00 * m13 * m22),
        inv * (m10 * m21 - m11 * m20), inv * -(m00 * m21 - m01 * m20), inv * (m00 * m11 - m01 * m10),
        inv * -(m00 * m11 * m23 + m01 * m13 * m20 + m03 * m10 * m21 - m03 * m11 * m20 - m01 * m10 * m23 - m00 * m13 * m21));
}

void fn_8040BCD4(Mtx34f *m) {
    float det = Determinant(m);
    if (det == 0.0f) {
        return;
    }
    float inv = 1.0f / det;
    SetValues(m, inv * (m11 * m22 - m12 * m21), inv * -(m01 * m22 - m02 * m21), inv * (m01 * m12 - m02 * m11),
              inv * -(m01 * m12 * m23 + m02 * m13 * m21 + m03 * m11 * m22 - m03 * m12 * m21 - m02 * m11 * m23 - m01 * m13 * m22),
              inv * -(m10 * m22 - m12 * m20), inv * (m00 * m22 - m02 * m20), inv * -(m00 * m12 - m02 * m10),
              inv * (m00 * m12 * m23 + m02 * m13 * m20 + m03 * m10 * m22 - m03 * m12 * m20 - m02 * m10 * m23 - m00 * m13 * m22),
              inv * (m10 * m21 - m11 * m20), inv * -(m00 * m21 - m01 * m20), inv * (m00 * m11 - m01 * m10),
              inv * -(m00 * m11 * m23 + m01 * m13 * m20 + m03 * m10 * m21 - m03 * m11 * m20 - m01 * m10 * m23 - m00 * m13 * m21));
}
#undef m00
#undef m01
#undef m02
#undef m03
#undef m10
#undef m11
#undef m12
#undef m13
#undef m20
#undef m21
#undef m22
#undef m23

UnkMatrixValues_805E0720::UnkMatrixValues_805E0720(const UnkMatrixValues_805E0720 &other) {
    for (int i = 0; i < 12; i++) {
        (&values[0][0])[i] = (&other.values[0][0])[i];
    }
}

void fn_8040BAC4(Mtx34f *m, const Mtx34f *other) {
    UnkMatrixValues_805E0720 t(*static_cast<const UnkMatrixValues_805E0720 *>(m));
    M(m, 0, 0) = t.values[0][0] * M(other, 0, 0) + t.values[0][1] * M(other, 1, 0) + t.values[0][2] * M(other, 2, 0);
    M(m, 0, 1) = t.values[0][0] * M(other, 0, 1) + t.values[0][1] * M(other, 1, 1) + t.values[0][2] * M(other, 2, 1);
    M(m, 0, 2) = t.values[0][0] * M(other, 0, 2) + t.values[0][1] * M(other, 1, 2) + t.values[0][2] * M(other, 2, 2);
    M(m, 0, 3) = t.values[0][0] * M(other, 0, 3) + t.values[0][1] * M(other, 1, 3) + t.values[0][2] * M(other, 2, 3) +
                 t.values[0][3];
    M(m, 1, 0) = t.values[1][0] * M(other, 0, 0) + t.values[1][1] * M(other, 1, 0) + t.values[1][2] * M(other, 2, 0);
    M(m, 1, 1) = t.values[1][0] * M(other, 0, 1) + t.values[1][1] * M(other, 1, 1) + t.values[1][2] * M(other, 2, 1);
    M(m, 1, 2) = t.values[1][0] * M(other, 0, 2) + t.values[1][1] * M(other, 1, 2) + t.values[1][2] * M(other, 2, 2);
    M(m, 1, 3) = t.values[1][0] * M(other, 0, 3) + t.values[1][1] * M(other, 1, 3) + t.values[1][2] * M(other, 2, 3) +
                 t.values[1][3];
    M(m, 2, 0) = t.values[2][0] * M(other, 0, 0) + t.values[2][1] * M(other, 1, 0) + t.values[2][2] * M(other, 2, 0);
    M(m, 2, 1) = t.values[2][0] * M(other, 0, 1) + t.values[2][1] * M(other, 1, 1) + t.values[2][2] * M(other, 2, 1);
    M(m, 2, 2) = t.values[2][0] * M(other, 0, 2) + t.values[2][1] * M(other, 1, 2) + t.values[2][2] * M(other, 2, 2);
    M(m, 2, 3) = t.values[2][0] * M(other, 0, 3) + t.values[2][1] * M(other, 1, 3) + t.values[2][2] * M(other, 2, 3) +
                 t.values[2][3];
}

UnkMatrixValues_805E0720 fn_8040B8FC(const Mtx34f *a, const Mtx34f *b) {
    return UnkMatrixValues_805E0720(
        M(a, 0, 0) * M(b, 0, 0) + M(a, 0, 1) * M(b, 1, 0) + M(a, 0, 2) * M(b, 2, 0),
        M(a, 0, 0) * M(b, 0, 1) + M(a, 0, 1) * M(b, 1, 1) + M(a, 0, 2) * M(b, 2, 1),
        M(a, 0, 0) * M(b, 0, 2) + M(a, 0, 1) * M(b, 1, 2) + M(a, 0, 2) * M(b, 2, 2),
        M(a, 0, 0) * M(b, 0, 3) + M(a, 0, 1) * M(b, 1, 3) + M(a, 0, 2) * M(b, 2, 3) + M(a, 0, 3),
        M(a, 1, 0) * M(b, 0, 0) + M(a, 1, 1) * M(b, 1, 0) + M(a, 1, 2) * M(b, 2, 0),
        M(a, 1, 0) * M(b, 0, 1) + M(a, 1, 1) * M(b, 1, 1) + M(a, 1, 2) * M(b, 2, 1),
        M(a, 1, 0) * M(b, 0, 2) + M(a, 1, 1) * M(b, 1, 2) + M(a, 1, 2) * M(b, 2, 2),
        M(a, 1, 0) * M(b, 0, 3) + M(a, 1, 1) * M(b, 1, 3) + M(a, 1, 2) * M(b, 2, 3) + M(a, 1, 3),
        M(a, 2, 0) * M(b, 0, 0) + M(a, 2, 1) * M(b, 1, 0) + M(a, 2, 2) * M(b, 2, 0),
        M(a, 2, 0) * M(b, 0, 1) + M(a, 2, 1) * M(b, 1, 1) + M(a, 2, 2) * M(b, 2, 1),
        M(a, 2, 0) * M(b, 0, 2) + M(a, 2, 1) * M(b, 1, 2) + M(a, 2, 2) * M(b, 2, 2),
        M(a, 2, 0) * M(b, 0, 3) + M(a, 2, 1) * M(b, 1, 3) + M(a, 2, 2) * M(b, 2, 3) + M(a, 2, 3));
}

void fn_8040B8C0(Mtx34f *m, const Quatf *q) {
    fn_8040BAC4(m, &(const Mtx34f &)fn_8040B228(q));
}

void fn_8040B884(Mtx34f *m, float radians) {
    fn_8040BAC4(m, &(const Mtx34f &)fn_8040B1B0(radians));
}

void fn_8040B848(Mtx34f *m, float radians) {
    fn_8040BAC4(m, &(const Mtx34f &)fn_8040B138(radians));
}

void fn_8040B80C(Mtx34f *m, float radians) {
    fn_8040BAC4(m, &(const Mtx34f &)fn_8040B0C0(radians));
}

void fn_8040B790(Mtx34f *m, const Vec3f *scale) {
    M(m, 0, 0) *= scale->x;
    M(m, 0, 1) *= scale->y;
    M(m, 0, 2) *= scale->z;
    M(m, 1, 0) *= scale->x;
    M(m, 1, 1) *= scale->y;
    M(m, 1, 2) *= scale->z;
    M(m, 2, 0) *= scale->x;
    M(m, 2, 1) *= scale->y;
    M(m, 2, 2) *= scale->z;
}

void fn_8040B720(Mtx34f *m, float sx, float sy, float sz) {
    M(m, 0, 0) *= sx;
    M(m, 0, 1) *= sy;
    M(m, 0, 2) *= sz;
    M(m, 1, 0) *= sx;
    M(m, 1, 1) *= sy;
    M(m, 1, 2) *= sz;
    M(m, 2, 0) *= sx;
    M(m, 2, 1) *= sy;
    M(m, 2, 2) *= sz;
}

void fn_8040B6F8(Mtx34f *m) {
    *static_cast<UnkMatrix_80325384 *>(m) = lbl_805E0720;
}

Vec3f fn_8040B67C(const Mtx34f *m, const Vec3f *v) {
    return Vec3f(M(m, 0, 0) * v->x + M(m, 0, 1) * v->y + M(m, 0, 2) * v->z + M(m, 0, 3),
                 M(m, 1, 0) * v->x + M(m, 1, 1) * v->y + M(m, 1, 2) * v->z + M(m, 1, 3),
                 M(m, 2, 0) * v->x + M(m, 2, 1) * v->y + M(m, 2, 2) * v->z + M(m, 2, 3));
}

Vec3f fn_8040B618(const Mtx34f *m, const Vec3f *v) {
    return Vec3f(M(m, 0, 0) * v->x + M(m, 0, 1) * v->y + M(m, 0, 2) * v->z,
                 M(m, 1, 0) * v->x + M(m, 1, 1) * v->y + M(m, 1, 2) * v->z,
                 M(m, 2, 0) * v->x + M(m, 2, 1) * v->y + M(m, 2, 2) * v->z);
}

void fn_8040B59C(Mtx34f *m, const Vec3f *offset) {
    M(m, 0, 3) += M(m, 0, 0) * offset->x + M(m, 0, 1) * offset->y + M(m, 0, 2) * offset->z;
    M(m, 1, 3) += M(m, 1, 0) * offset->x + M(m, 1, 1) * offset->y + M(m, 1, 2) * offset->z;
    M(m, 2, 3) += M(m, 2, 0) * offset->x + M(m, 2, 1) * offset->y + M(m, 2, 2) * offset->z;
}

void fn_8040B52C(Mtx34f *m, float x, float y, float z) {
    M(m, 0, 3) += M(m, 0, 0) * x + M(m, 0, 1) * y + M(m, 0, 2) * z;
    M(m, 1, 3) += M(m, 1, 0) * x + M(m, 1, 1) * y + M(m, 1, 2) * z;
    M(m, 2, 3) += M(m, 2, 0) * x + M(m, 2, 1) * y + M(m, 2, 2) * z;
}

void fn_8040B4D0(Mtx34f *out, const Mtx34f *m) {
    SetValues(out, M(m, 0, 0), M(m, 1, 0), M(m, 2, 0), 0.0f, M(m, 0, 1), M(m, 1, 1), M(m, 2, 1), 0.0f, M(m, 0, 2),
              M(m, 1, 2), M(m, 2, 2), 0.0f);
}

UnkMatrixValues_805E0720 fn_8040B33C(const Vec3f *eye, const Vec3f *up, const Vec3f *target) {
    Fn8040CopyVector3 z(((Vec3fBinaryFn)fn_8040CD74)(eye, target));
    fn_8040CE74(V(z));
    Fn8040CopyVector3 x(((Vec3fBinaryFn)fn_8040CFB8)(up, V(z)));
    fn_8040CE74(V(x));
    Fn8040CopyVector3 y(((Vec3fBinaryFn)fn_8040CFB8)(V(z), V(x)));
    return UnkMatrixValues_805E0720(x.x, x.y, x.z, -fn_8040CF90(eye, V(x)), y.x, y.y, y.z, -fn_8040CF90(eye, V(y)), z.x, z.y,
                                    z.z, -fn_8040CF90(eye, V(z)));
}

UnkMatrixValues_805E0720 fn_8040B228(const Quatf *q) {
    float s = 2.0f / fn_8040CA84(q);
    float xs = q->x * s;
    float ys = q->y * s;
    float zs = q->z * s;
    float wx = q->w * xs;
    float wy = q->w * ys;
    float wz = q->w * zs;
    float xx = q->x * xs;
    float xy = q->x * ys;
    float xz = q->x * zs;
    float yy = q->y * ys;
    float yz = q->y * zs;
    float zz = q->z * zs;
    return UnkMatrixValues_805E0720(1.0f - (yy + zz), xy - wz, xz + wy, 0.0f, xy + wz, 1.0f - (xx + zz), yz - wx, 0.0f,
                                    xz - wy, yz + wx, 1.0f - (xx + yy), 0.0f);
}

UnkMatrixValues_805E0720 fn_8040B1B0(float radians) {
    float s;
    float c;
    SinCos(&s, &c, radians);
    return UnkMatrixValues_805E0720(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, c, -s, 0.0f, 0.0f, s, c, 0.0f);
}

UnkMatrixValues_805E0720 fn_8040B138(float radians) {
    float s;
    float c;
    SinCos(&s, &c, radians);
    return UnkMatrixValues_805E0720(c, 0.0f, s, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -s, 0.0f, c, 0.0f);
}

UnkMatrixValues_805E0720 fn_8040B0C0(float radians) {
    float s;
    float c;
    SinCos(&s, &c, radians);
    return UnkMatrixValues_805E0720(c, -s, 0.0f, 0.0f, s, c, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
}

void fn_8040B07C(Mtx34f *m, const Vec3f *scale) {
    SetValues(m, scale->x, 0.0f, 0.0f, 0.0f, 0.0f, scale->y, 0.0f, 0.0f, 0.0f, 0.0f, scale->z, 0.0f);
}

void fn_8040B034(Mtx34f *m, const Vec3f *offset) {
    SetValues(m, 1.0f, 0.0f, 0.0f, offset->x, 0.0f, 1.0f, 0.0f, offset->y, 0.0f, 0.0f, 1.0f, offset->z);
}

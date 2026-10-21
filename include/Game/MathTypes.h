#ifndef SHADOW_GAME_MATHTYPES_H
#define SHADOW_GAME_MATHTYPES_H

#include <dolphin/types.h>

/* Plain float vectors and the 3x4 matrix the game's math routines (most of them between
 * 0x80408000 and 0x80410000) work on. The game's own names for them are not known. Field order
 * and sizes follow the loads and stores in game code. */

/* Two floats. */
struct Vec2f {
    Vec2f() {}
    Vec2f(float x_, float y_) : x(x_), y(y_) {}

    float x;
    float y;
};

/* Three floats; positions, directions, Euler angles and scales all use it. */
struct Vec3f {
    Vec3f() {}
    Vec3f(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    float x;
    float y;
    float z;
};

/* Rows of a 3x4 matrix: a 3x3 rotation/scale part with the translation in column 3. */
struct Mtx34f {
    float values[3][4];
};

/* Read-only data: the 3x4 identity and a zero vector. */
extern const Mtx34f lbl_805E0720;
extern const Vec3f lbl_805E07A0;

extern "C" {
/* Vec3f helpers kept out of line by the game: construct from components, copy-construct and
 * assign. Each returns its first argument. */
Vec3f *fn_80008E1C(Vec3f *out, float x, float y, float z);
Vec3f *fn_800091D0(Vec3f *out, const Vec3f *from);
Vec3f *fn_800091EC(Vec3f *out, const Vec3f *from);

/* Vector routines. */
void fn_8040CD74(Vec3f *out, const Vec3f *lhs, const Vec3f *rhs); /* out = lhs - rhs */
float fn_8040CF2C(const Vec3f *v);                                /* |v|^2 */
float fn_8040CF50(const Vec3f *v);                                /* |v| */

/* Matrix routines. */
void fn_8040B6F8(Mtx34f *m);                              /* set to identity */
void fn_8040B034(Mtx34f *m, const Vec3f *offset);         /* set to a pure translation */
void fn_8040B59C(Mtx34f *m, const Vec3f *offset);         /* apply a translation */
void fn_8040B720(Mtx34f *m, float sx, float sy, float sz); /* apply a scale */
void fn_8040B884(Mtx34f *m, float radians);               /* apply a rotation about X */
void fn_8040B848(Mtx34f *m, float radians);               /* apply a rotation about Y */
void fn_8040B80C(Mtx34f *m, float radians);               /* apply a rotation about Z */
void fn_8040C148(Mtx34f *m, const Mtx34f *from);          /* copy */

/* Rotation matrix from three Euler angles; order selects the axis order. */
void fn_8001E070(Mtx34f *m, const Vec3f *eulerAngles, int order);
}

/* m becomes a translation by at. */
inline void SetTranslationMtx(Mtx34f *m, const Vec3f &at) {
    fn_8040B034(m, &at);
}

#endif

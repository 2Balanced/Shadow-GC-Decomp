#ifndef GAME_QUATERNION_H
#define GAME_QUATERNION_H

#include <dolphin/types.h>

/* A quaternion of four floats (x, y, z, w; name invented, as Vec3f's is). Its copy constructor is
 * out of line (0x801B1DD4). The operators are inline functions in the original that the game
 * calls out of line: their one copy is ThrowBomb's (0x8020B9C8 and 0x8020B9F4), and another unit
 * (0x8028B5AC) calls the same copies. */
struct Quatf {
    Quatf() {}
    Quatf(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
    /* 0x801B1DD4 */
    Quatf(const Quatf &q);

    float x;
    float y;
    float z;
    float w;
};

/* q * scale, a * b (the product of two quaternions), q += value and q made unit length
 * (behaviour inferred). The first two store their result through their first argument. */
extern "C" {
void fn_8040C924(Quatf *result, const Quatf *q, float scale);
void fn_8040CA10(Quatf *result, const Quatf *a, const Quatf *b);
void fn_8040CAB4(Quatf *q, const Quatf *value);
void fn_8040C958(Quatf *q);
}

/* fn_8040C924 and fn_8040CA10 called as returning their result by value. */
typedef Quatf (*QuatScaleFn)(const Quatf *q, float scale);
typedef Quatf (*QuatMultiplyFn)(const Quatf *a, const Quatf *b);

/* The calls behind the operators (names inferred). The operators call them as inline functions,
 * which keeps the operators themselves out of line, as in retail. */
inline Quatf ScaleQuat(const Quatf &q, float scale) {
    return ((QuatScaleFn)fn_8040C924)(&q, scale);
}

inline Quatf MultiplyQuat(const Quatf &a, const Quatf &b) {
    return ((QuatMultiplyFn)fn_8040CA10)(&a, &b);
}

inline Quatf operator*(const Quatf &q, float scale) {
    return ScaleQuat(q, scale);
}

inline Quatf operator*(const Quatf &a, const Quatf &b) {
    return MultiplyQuat(a, b);
}

#endif

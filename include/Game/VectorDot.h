#ifndef GAME_VECTORDOT_H
#define GAME_VECTORDOT_H

#include "Game/MathTypes.h"

extern "C" {
/* a . b (behaviour inferred from its body). */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
}

/* a . b through fn_8040CF90 (name inferred); inlined where it is called. */
inline float Fn8040Dot(const Vec3f &a, const Vec3f &b) {
    return fn_8040CF90(&a, &b);
}

/* a . b (name inferred). Its body calls an inline function, so GC/2.7 keeps it out of line: the game
 * has one shared copy (0x80221D94, first emitted by Game/ScatteringWeapon.cpp) that other units
 * call too (0x8023AE0C). */
inline float DotProduct(const Vec3f &a, const Vec3f &b) {
    return Fn8040Dot(a, b);
}

#endif

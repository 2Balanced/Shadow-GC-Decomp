#ifndef GAME_FN8040COPYVECTOR3_H
#define GAME_FN8040COPYVECTOR3_H

#include "Game/MathTypes.h"
#include "size_assert.h"

/* A vector member copied and assigned through the out-of-line vector copy constructor and
 * assignment (fn_800091EC, fn_800091D0). The game's vector class has those out of line;
 * Vec3f stands in without them, so members that the game copies through them use this
 * type (name inferred). It has no base class, so that its inline copy constructor calls no other
 * inline function and is inlined. */
struct Fn8040CopyVector3 {
    Fn8040CopyVector3() {}
    Fn8040CopyVector3(const Fn8040CopyVector3 &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
    }
    /* A copy of a plain vector (explicit, so that a Vec3f never converts to it implicitly). */
    explicit Fn8040CopyVector3(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }
    Fn8040CopyVector3 &operator=(const Fn8040CopyVector3 &v) {
        fn_800091D0(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
        return *this;
    }

    float x;
    float y;
    float z;
};
SIZE_ASSERT(Fn8040CopyVector3, 0xC);

#endif

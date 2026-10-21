#ifndef GAME_UNKZEROVECTOR3_80147830_H
#define GAME_UNKZEROVECTOR3_80147830_H

#include "Game/MathTypes.h"

/* A vector whose default constructor zeroes it (type name unknown). The constructor is inline:
 * EggBalloon.cpp builds an array of them through __construct_array and emits the copy the linker
 * keeps (0x80147830), which the unit at 0x8029DA9C also uses. */
struct UnkZeroVector3_80147830 : public Vec3f {
    UnkZeroVector3_80147830() : Vec3f(0.0f, 0.0f, 0.0f) {}
};

#endif

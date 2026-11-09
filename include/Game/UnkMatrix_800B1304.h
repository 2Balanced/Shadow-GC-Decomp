#ifndef GAME_UNKMATRIX_800B1304_H
#define GAME_UNKMATRIX_800B1304_H

#include "Game/MathTypes.h"

/* A matrix whose empty default constructor is out of line at 0x800B1304 (the class name is
 * unknown; matrix arrays use it as their element type: Game/DevilDoomCtrlUnit.h,
 * Game/VehicleAirBolt.cpp). */
struct UnkMatrix_800B1304 : public Mtx34f {
#ifdef UNK_MATRIX_DEFAULT_CTOR_EXTERN
    UnkMatrix_800B1304();
#else
    UnkMatrix_800B1304() {}
#endif
};

#endif

#ifndef GAME_GADGETDEBRIS_H
#define GAME_GADGETDEBRIS_H

#include "boost/shared_ptr.hpp"
#include "dolphin/types.h"
#include "size_assert.h"

class CharInfo;

/* The parameters of a debris spawn: the constructor at 0x8015F0B8 sets them up at a character
 * info's place from a template, and fn_8015D93C launches the pieces (layout from the constructor
 * and the gadgets that set fields; names inferred). BreakObj, CaptureCage, ElecSmallCube and Stick
 * still declare the same struct locally. */
struct UnkDebrisParam_8015F0B8 {
    UnkDebrisParam_8015F0B8(const boost::shared_ptr<CharInfo> &info, int index);

    /* 0x00 */ u8 field_0x0[0x34];
    /* 0x34 */ float scale;
    /* 0x38 */ u8 field_0x38[0x44 - 0x38];
    /* 0x44 */ s32 model; /* from fn_80012700 */
    /* 0x48 */ u32 field_0x48;
    /* 0x4C */ s32 field_0x4C; /* a piece count or lifetime (meaning unknown) */
    /* 0x50 */ u32 field_0x50;
    /* 0x54 */ u8 field_0x54;
    /* 0x55 */ u8 field_0x55[0x5C - 0x55];
};
SIZE_ASSERT(UnkDebrisParam_8015F0B8, 0x5C);

extern "C" {
/* Launches the debris described by param (behaviour inferred). */
void fn_8015D93C(UnkDebrisParam_8015F0B8 *param);
}

#endif

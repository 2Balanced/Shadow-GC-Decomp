#ifndef SHADOW_GAME_DEBUGDRAW_H
#define SHADOW_GAME_DEBUGDRAW_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>

/* RGBA packed into one word, as the debug shape routines take it (by pointer). */
typedef u32 UnkDrawColor_802E0E84;

/* The color nearly every gadget's debug view uses. */
extern UnkDrawColor_802E0E84 lbl_805F06D8;

extern "C" {
/* Debug view: a wire sphere around `at`. */
void fn_802E0E84(const Vec3f *at, const UnkDrawColor_802E0E84 *rgba, float size);
}

#endif

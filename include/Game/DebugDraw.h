#ifndef SHADOW_GAME_DEBUGDRAW_H
#define SHADOW_GAME_DEBUGDRAW_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>

/* RGBA packed into one word, as the debug shape routines take it (by pointer). */
typedef u32 UnkDrawColor_802E0E84;

/* The color nearly every gadget's debug view uses. */
extern UnkDrawColor_802E0E84 lbl_805F06D8;

/* A sphere for the debug view: centre and radius (struct name unknown). Its implicit default
 * constructor is out of line (0x8004E6CC, empty: Vec3f's constructor does nothing). */
struct UnkSphere_8004E6CC {
    /* 0x0 */ Vec3f m_center;
    /* 0xC */ float m_radius;
};

/* Both are defined in Game/DebugDisp.cpp, which queues the shapes for SonicteamUSA::System::DebugDisp
 * (the colour there is UnkColor_8040D2D8, see Game/DebugDisp.h). */
extern "C" {
/* Debug view: a wire sphere around `at`. */
void fn_802E0E84(const Vec3f *at, const UnkDrawColor_802E0E84 *rgba, float size);
/* Debug view: the same for a sphere (0x802E0F3C). */
void fn_802E0F3C(const UnkSphere_8004E6CC *sphere, const UnkDrawColor_802E0E84 *rgba);
}

#endif

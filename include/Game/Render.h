#ifndef GAME_RENDER_H
#define GAME_RENDER_H

#include <boost/function.hpp>

#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The argument a Render passes to its draw callbacks (struct name from the game's RTTI, which
 * spells it "Redner"). */
struct RednerDrawArgs;
class Task;

/* Render (class name from the RTTI of its shared_ptr control block): a 0x28-byte drawing hook. Only
 * the list of draw callbacks at 0x14 is known (name inferred). ElecKyuu.cpp keeps its own copy of
 * this declaration. */
class Render {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u8 m_drawCallbacks[0x28 - 0x14];
};
SIZE_ASSERT(Render, 0x28);

/* The object fn_80046178 returns; the word at 0x8 is passed when creating a Render (meaning
 * unknown). */
struct UnkRenderOwner_80046178 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ void *field_0x8;
    /* 0xC */ u8 field_0xC[0x10 - 0xC];
    /* 0x10 */ Task *field_0x10; /* the parent of Effect::PJS::EffectDrawPunchTask */
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
    /* 0x18 */ void *field_0x18; /* passed by TPjsEventActionFade (meaning unknown) */
    /* 0x1C */ u8 field_0x1C[0x20 - 0x1C];
    /* 0x20 */ Task *field_0x20; /* the parent of Test::EffBelt and Test::EffLocusMultiVtx */
    /* 0x24 */ Task *field_0x24; /* the parent of the PJS effects' glare and lens flare tasks */
    /* 0x28 */ void *field_0x28; /* passed instead by ElecCoreWall (meaning unknown) */
    /* 0x2C */ Task *field_0x2C; /* the parent of the PJS effects' shimmer and blur tasks */
    /* 0x30 */ void *field_0x30; /* passed by VehicleArrow (meaning unknown) */
    /* 0x34 */ u8 field_0x34[0x38 - 0x34];
    /* 0x38 */ void *field_0x38; /* passed by SavePointSelect (meaning unknown) */
    /* 0x3C */ void *field_0x3C; /* passed by GiGameOver (meaning unknown) */
    /* 0x40 */ void *field_0x40; /* passed by StageSequence::MovieTest (meaning unknown) */
    /* 0x44 */ u8 field_0x44[0x4C - 0x44];
    /* 0x4C */ void *field_0x4C; /* passed by StageMovie (meaning unknown) */
    /* 0x50 */ u8 field_0x50[0x54 - 0x50];
    /* 0x54 */ void *field_0x54; /* passed by EggLastMechSlotDisp2D (meaning unknown) */
    /* 0x58 */ Task *field_0x58; /* the parent of the PJS effects' screen tasks */
};

extern "C" {
UnkRenderOwner_80046178 *fn_80046178(void);
/* Creates a Render named name (behaviour inferred). */
boost::shared_ptr<Render> fn_8004DFB0(void *arg0, const char *name);
#ifndef RENDER_DRAW_CALLBACK_DEFINED /* Game/WeaponLockonMark.cpp defines it, returning callbacks */
/* Adds a draw callback to a Render's list (behaviour inferred). */
void fn_80062CD0(void *callbacks, const boost::function1<void, RednerDrawArgs &> &callback);
#endif
}

#endif

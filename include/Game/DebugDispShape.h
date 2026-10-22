#ifndef GAME_DEBUGDISPSHAPE_H
#define GAME_DEBUGDISPSHAPE_H

/* What the debug shape units (Game/DebugDispCapsule.cpp and the others, see Game/DebugDisp.h) draw
 * with: the parts of RenderWare they touch (rw/rwcore.h, rp/rpworld.h; only the members used are
 * laid out) and the colour conversion they share. */

#include "Game/DebugDisp.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

struct RwV3d {
    float x;
    float y;
    float z;
};

typedef struct RwRGBA {
    u8 red;
    u8 green;
    u8 blue;
    u8 alpha;
} RwRGBA;

struct RwMatrix {
    /* 0x00 */ RwV3d right;
    /* 0x0C */ u32 flags;
    /* 0x10 */ RwV3d up;
    /* 0x1C */ u32 pad1;
    /* 0x20 */ RwV3d at;
    /* 0x2C */ u32 pad2;
    /* 0x30 */ RwV3d pos;
    /* 0x3C */ u32 pad3;
};
SIZE_ASSERT(RwMatrix, 0x40);

struct RwFrame {
    /* 0x00 */ u8 object[0x10];
    /* 0x10 */ RwMatrix modelling;
    /* 0x50 */ RwMatrix ltm;
    /* 0x90 */ u8 objectList[0x8];
    /* 0x98 */ RwFrame *child;
    /* 0x9C */ RwFrame *next;
    /* 0xA0 */ RwFrame *root;
};

struct RpClump {
    /* 0x0 */ u8 object[0x4];
    /* 0x4 */ RwFrame *frame;
};

extern "C" {
/* The RenderWare matrix of matrix (behaviour inferred). */
RwMatrix fn_800591F0(const Mtx34f *matrix);
/* RwFrameUpdateObjects, RwFrameTranslate and RwFrameScale. */
RwFrame *fn_8048722C(RwFrame *frame);
RwFrame *fn_804875B0(RwFrame *frame, const RwV3d *translation, int combine);
RwFrame *fn_8048764C(RwFrame *frame, const RwV3d *scale, int combine);
/* Sets the colour of clump's materials, and renders clump (behaviour inferred). */
void fn_80037B84(RpClump *clump, const RwRGBA &color);
RpClump *fn_80456BC4(RpClump *clump);
}

/* The RenderWare colour of a shape's colour (bytes 1, 2, 3 are red, green, blue, byte 0 the alpha;
 * name inferred). */
inline RwRGBA ToRwRGBA(const UnkColor_8040D2D8 &color) {
    RwRGBA rgba = {color.g, color.b, color.a, color.r};
    return rgba;
}

#endif

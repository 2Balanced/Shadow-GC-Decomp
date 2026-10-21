#ifndef SHADOW_GAME_GADGETCLASSDESC_H
#define SHADOW_GAME_GADGETCLASSDESC_H

#include <dolphin/types.h>
#include "size_assert.h"

class UnkPlacement_800C9CD0;

/* Passed to a parameter's label function; units that look inside it describe its layout
 * themselves. */
class UnkLabelQuery_800CBC20;

/* A row of a gadget type's parameter table in .data, describing one value the stage editor can
 * set. A row with kind 0 ends the table. The meaning of the plain fields is open. */
struct GadgetParamDesc {
    /* 0x00 */ s32 kind; /* 1 an integer, 4 a float, ... */
    /* 0x04 */ const char *label;
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ s32 field_0x10;
    /* 0x14 */ float initial;
    /* 0x18 */ float lowest;
    /* 0x1C */ float highest;
    /* 0x20 */ const char *(*labelFn)(UnkLabelQuery_800CBC20 *query);
    /* 0x24 */ u32 field_0x24;
};
SIZE_ASSERT(GadgetParamDesc, 0x28);

/* What the game knows about one gadget type: its name, hooks to load and drop its files, the
 * function that spawns an instance from a placement record, and its parameter table. The game
 * keeps all of them in one big table. Layout from those records; names are ours. */
struct GadgetClassDesc {
    /* 0x00 */ const char *typeName;
    /* 0x04 */ void (*loadFiles)(void);
    /* 0x08 */ void (*dropFiles)(void);
    /* 0x0C */ void (*spawn)(UnkPlacement_800C9CD0 *owner);
    /* 0x10 */ u32 field_0x10;
    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ u16 typeId;
    /* 0x1A */ u8 field_0x1A;
    /* 0x1B */ u8 field_0x1B;
    /* 0x1C */ u8 field_0x1C;
    /* 0x20 */ GadgetParamDesc *paramTable;
};
SIZE_ASSERT(GadgetClassDesc, 0x24);

extern "C" {
/* The label function most parameters use; it supplies no label. */
const char *fn_800CBC20(UnkLabelQuery_800CBC20 *query);
}

#endif

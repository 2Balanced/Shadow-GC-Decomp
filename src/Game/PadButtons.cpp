/* The game's button table (file name inferred; the game does not name this code): each of ten
 * logical buttons maps to up to three pad button masks, and a logical button is held or pressed
 * on a pad when one of its masks is (all its bits, or all of its bits in 0x3C0 or in 0x3C0000).
 * fn_80356260 also remembers the pad that pressed logical button 1. The functions other units call
 * keep their fn_ names; the behaviour and the names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

/* The state of one pad as the pad manager keeps it (struct name unknown). */
struct UnkPadState_80051734 {
    /* 0x00 */ u32 m_held;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 m_pressed;
    /* 0x0C */ u8 field_0xC[0x28 - 0xC];
};
SIZE_ASSERT(UnkPadState_80051734, 0x28);

/* The pad manager fn_80051734 returns (struct name unknown; see Game/AdvTitle.cpp). */
struct UnkPadInput_80051734 {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ UnkPadState_80051734 m_pads[4];
};

extern "C" {
UnkPadInput_80051734 *fn_80051734(void);
/* Whether mask was held on pad (behaviour inferred). */
bool fn_80349800(UnkPadInput_80051734 *input, int pad, u32 mask, int arg3, int arg4);
}

/* The pad that last pressed logical button 1 (name inferred). */
static s32 s_pad;
/* The table in use (name inferred). */
static u32 (*s_buttons)[3];

/* The default table (name inferred). */
static u32 s_defaultButtons[10][3] = {
    {0x400, 0, 0},
    {0x400, 0x8, 0x1},
    {0x10, 0x2, 0},
    {0x40040, 0, 0},
    {0x80080, 0, 0},
    {0x100100, 0, 0},
    {0x200200, 0, 0},
    {0x800, 0, 0},
    {0x2000, 0, 0},
    {0x1000, 0, 0},
};

/* Whether buttons hold mask (name inferred). */
inline bool Matches(u32 mask, u32 buttons) {
    if ((mask & buttons) == mask) {
        return true;
    }
    u32 part = mask & 0x3C0;
    if (part != 0 && (part & buttons) == part) {
        return true;
    }
    part = mask & 0x3C0000;
    if (part != 0 && (part & buttons) == part) {
        return true;
    }
    return false;
}

/* Selects the default table. */
extern "C" void fn_803563C0(void) {
    s_buttons = s_defaultButtons;
}

/* Whether logical button button was pressed on pad pad; remembers the pad that pressed button 1.
 *
 * NonMatching (99%): the 88 instructions are retail's instruction for instruction, only the saved
 * registers are a permutation of retail's set: retail keeps the counters in r27, the button row
 * offset in r28, the pad offset in r29 and the mask in r30, while GC/2.7 puts the mask in the
 * first saved slot r27 and shifts everything else up. Declaring the mask at function scope moves
 * 3 rows of the 18 (28 shapes probed in probes/Game_PadButtons.py and probes/Game_PadButtons2.py,
 * floor 15). */
extern "C" bool fn_80356260(int button, int pad) {
    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_pressed)) {
            if (button == 1) {
                for (u32 j = 0; j < 2; j++) {
                    u32 other = s_buttons[button][i];
                    if (Matches(other, fn_80051734()->m_pads[j].m_pressed)) {
                        s_pad = j;
                    }
                }
            }
            return true;
        }
    }
    return false;
}

/* Whether mask is held on pad, through the pad manager (the object is not used). */
extern "C" bool fn_80356214(UnkPadInput_80051734 *input, int pad, u32 mask, int arg3, int arg4) {
    return fn_80349800(fn_80051734(), pad, mask, arg3, arg4);
}

/* Whether logical button button is held on pad pad (through fn_80349800).
 *
 * NonMatching (93%): same as fn_80356260, 33 instructions on both sides and one register swap
 * left. fn_803560C8 below shows the allocation retail wants (counter r27, mask r28, pad r29,
 * row offset r30, byte offset r31); there the pad parameter never needs a register of its own
 * because the prologue folds it into `mulli r29,r4,40`. Declaring the mask with its initialiser
 * inside the loop gets the mask to r28 and leaves 5 rows (16 shapes probed, floor 5): the counter
 * and the pad parameter still trade places. */
extern "C" bool fn_80356190(int button, int pad) {
    for (int i = 0; i < 3; i++) {
        u32 mask = s_buttons[button][i];
        if (mask != 0 && fn_80356214(fn_80051734(), pad, mask, 12, 1)) {
            return true;
        }
    }
    return false;
}

/* Whether logical button button is held on pad pad. */
extern "C" bool fn_803560C8(int button, int pad) {
    u32 mask;
    for (int i = 0; i < 3; i++) {
        mask = s_buttons[button][i];
        if (mask != 0 && Matches(mask, fn_80051734()->m_pads[pad].m_held)) {
            return true;
        }
    }
    return false;
}

/* The pad that last pressed logical button 1. */
extern "C" s32 fn_803560C0(void) {
    return s_pad;
}

extern "C" bool fn_803560B8(void) {
    return false;
}

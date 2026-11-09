/* The game's four-byte colour: its constructors (from four bytes, and from red, green and blue with
 * the first byte, alpha, set to 0xFF), a saturating difference, and the ten named colours other
 * units read (black, blue, cyan, grey, green, light grey, magenta, red, white and yellow), built
 * when the game starts. The colour is known to the project under two names, one per constructor
 * (Game/DebugDisp.h, Game/EffLocusMultiVtx.h); both are declared here as there. The file and
 * function names are invented. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <dolphin/types.h>
#include "size_assert.h"

struct UnkColor_8040D2D8 {
    UnkColor_8040D2D8(u8 r, u8 g, u8 b, u8 a);

    /* 0x0 */ u8 r;
    /* 0x1 */ u8 g;
    /* 0x2 */ u8 b;
    /* 0x3 */ u8 a;
};
SIZE_ASSERT(UnkColor_8040D2D8, 0x4);

struct UnkColor_8040D2EC {
    UnkColor_8040D2EC(u8 r, u8 g, u8 b);

    /* 0x0 */ u8 m_a;
    /* 0x1 */ u8 m_r;
    /* 0x2 */ u8 m_g;
    /* 0x3 */ u8 m_b;
};
SIZE_ASSERT(UnkColor_8040D2EC, 0x4);

extern "C" void fn_8040D254(UnkColor_8040D2EC *color, const UnkColor_8040D2EC *other);

#pragma auto_inline on

UnkColor_8040D2EC::UnkColor_8040D2EC(u8 r, u8 g, u8 b) {
    m_a = 0xFF;
    m_r = r;
    m_g = g;
    m_b = b;
}

UnkColor_8040D2D8::UnkColor_8040D2D8(u8 r_, u8 g_, u8 b_, u8 a_) {
    r = r_;
    g = g_;
    b = b_;
    a = a_;
}

#pragma auto_inline reset

/* Each byte of other taken from color's, not going below zero (name inferred). */
inline u8 SaturatingSub(u8 a, u8 b) {
    int difference = a - b;
    return difference <= 0 ? 0 : difference;

}

void fn_8040D254(UnkColor_8040D2EC *color, const UnkColor_8040D2EC *other) {
    color->m_a = SaturatingSub(color->m_a, other->m_a);
    color->m_r = SaturatingSub(color->m_r, other->m_r);
    color->m_g = SaturatingSub(color->m_g, other->m_g);
    color->m_b = SaturatingSub(color->m_b, other->m_b);
}

UnkColor_8040D2EC lbl_805F1360(0x00, 0x00, 0x00);
UnkColor_8040D2EC lbl_805F1364(0x00, 0x00, 0xFF);
UnkColor_8040D2EC lbl_805F1368(0x00, 0xFF, 0xFF);
UnkColor_8040D2EC lbl_805F136C(0x80, 0x80, 0x80);
UnkColor_8040D2EC lbl_805F1370(0x00, 0xFF, 0x00);
UnkColor_8040D2EC lbl_805F1374(0xC8, 0xC8, 0xC8);
UnkColor_8040D2EC lbl_805F1378(0xFF, 0x00, 0xFF);
UnkColor_8040D2EC lbl_805F137C(0xFF, 0x00, 0x00);
UnkColor_8040D2EC lbl_805F1380(0xFF, 0xFF, 0xFF);
UnkColor_8040D2EC lbl_805F1384(0xFF, 0xFF, 0x00);

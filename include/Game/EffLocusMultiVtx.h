#ifndef GAME_EFFLOCUSMULTIVTX_H
#define GAME_EFFLOCUSMULTIVTX_H

#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "size_assert.h"

/* A colour of four bytes whose first byte the constructor (0x8040D2EC) sets to 0xFF (struct name
 * unknown; field names inferred). */
struct UnkColor_8040D2EC {
    UnkColor_8040D2EC(u8 r, u8 g, u8 b);

    /* 0x0 */ u8 m_a;
    /* 0x1 */ u8 m_r;
    /* 0x2 */ u8 m_g;
    /* 0x3 */ u8 m_b;
};
SIZE_ASSERT(UnkColor_8040D2EC, 0x4);

namespace Test {

/* EffLocusMultiVtx (class name from RTTI; vtable 0x80558F14): a task that draws a trail through a
 * set of moving points. The constructor is at 0x802DEB40; the methods keep their address names
 * and what is known of them is inferred from Game/WeaponSpecialKatana.cpp. */
class EffLocusMultiVtx : public Task {
  public:
    /* length: the trail's history (inferred); points: count points that the owner keeps up to
     * date. */
    EffLocusMultiVtx(int length, int count, Vec3f *points, int arg,
                     const UnkColor_8040D2EC *color);

    /* Shows the trail (0x802DE9BC). */
    void fn_802DE9BC();
    /* Hides the trail (0x802DE9CC). */
    void fn_802DE9CC();
    /* Lets the trail fade out and end (0x802DE9DC). */
    void fn_802DE9DC();

    /* 0x28 */ void *m_locus; /* a 0x30-byte object built by 0x802DF0E0 */
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(EffLocusMultiVtx, 0x30);

} // namespace Test

#endif

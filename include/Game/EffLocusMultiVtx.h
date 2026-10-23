#ifndef GAME_EFFLOCUSMULTIVTX_H
#define GAME_EFFLOCUSMULTIVTX_H

#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
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

/* EffLocusMultiVtx (class name from RTTI; vtable 0x80558F14; Game/EffLocusMultiVtx.cpp): a task
 * named "Eff::LocusMultiVtx" that draws a trail through a set of moving points. The methods keep
 * their address names. */
class EffLocusMultiVtx : public Task {
  public:
    /* The trail (class name unknown). */
    class Impl;

    /* length: the trail's history; points: count points that the owner keeps up to date; arg: a
     * matrix the points are taken through when not 0 (inferred). */
    EffLocusMultiVtx(int length, int count, Vec3f *points, int arg,
                     const UnkColor_8040D2EC *color);
    virtual ~EffLocusMultiVtx();
    virtual void Tick(float dt);

    /* Shows the trail (0x802DE9BC). */
    void fn_802DE9BC();
    /* Hides the trail (0x802DE9CC). */
    void fn_802DE9CC();
    /* Lets the trail fade out and end (0x802DE9DC). */
    void fn_802DE9DC();

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(EffLocusMultiVtx, 0x30);

} // namespace Test

#endif

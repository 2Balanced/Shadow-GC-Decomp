#ifndef GAME_EFFBELT_H
#define GAME_EFFBELT_H

#include "Game/EffLocusMultiVtx.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Player {
class PlayerBase;
}

namespace Test {

/* EffBelt (class name from RTTI; vtable 0x80558EF4, Game/EffBelt.cpp): a task named "Eff::Belt"
 * that draws a ribbon through the last positions it was given, each with a ring of 11 points
 * turned about the direction of travel. Given a player, it hides while the player's weapon is
 * hidden. Finish fades it out and ends it. Member names are inferred. */
class EffBelt : public Task {
  public:
    /* The ribbon (class name unknown). */
    class Impl;

    /* count: how many positions the ribbon keeps; width: the ring's radius (inferred). */
    EffBelt(int count, const UnkColor_8040D2EC &color, float width);
    EffBelt(const boost::weak_ptr<Player::PlayerBase> &player, int count,
            const UnkColor_8040D2EC &color, float width);
    virtual ~EffBelt();
    virtual void Tick(float dt);

    /* Adds the next position (0x802DDCC8), and fades the ribbon out (0x802DDC94). */
    void AddPosition(const Vec3f &position);
    void Finish();

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 pad_0x2C; /* the class size is a multiple of 8 */
};
SIZE_ASSERT(EffBelt, 0x30);

} // namespace Test

#endif

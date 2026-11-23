#ifndef GAME_PLAYERSHADOWELECTRIC_H
#define GAME_PLAYERSHADOWELECTRIC_H

#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

namespace Player {
namespace Shadow {

/* The electric aura task on Shadow (class name from RTTI; Game/PlayerShadowElectric.cpp). Player
 * Effect starts it through the constructor and ends it through Stop. */
class Electric : public Task {
  public:
    class Impl;

    Electric(Task *parent, const boost::shared_ptr<PlayerBase> &player, Electric **slot);
    virtual ~Electric();
    virtual void Tick(float dt);

    /* Ends the aura: it fades out, and the task ends with it. */
    void Stop();

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 field_0x2C; /* (the constructor leaves it alone; meaning unknown) */
};
SIZE_ASSERT(Electric, 0x30);

} // namespace Shadow
} // namespace Player

#endif

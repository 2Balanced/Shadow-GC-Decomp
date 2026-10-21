#ifndef GAME_PLAYERSOUNDBASE_H
#define GAME_PLAYERSOUNDBASE_H

#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Player {

/* SoundBase (class name from RTTI; vtable at 0x80558D6C): the task that plays a character's
 * voices and sounds. The constructor (0x802DBDF0) keeps the player and registers itself at the
 * player's 0x240; the destructor (0x802DBD7C) clears that. Player::Shadow::Sound and
 * Player::Npc::Sound (Game/PlayerNpcSound.cpp) derive from it. Member names are inferred. */
class SoundBase : public Task {
  public:
    SoundBase(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~SoundBase();

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x30 */ u32 field_0x30;
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(SoundBase, 0x38);

} // namespace Player

#endif

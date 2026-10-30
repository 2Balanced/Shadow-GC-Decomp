#ifndef GAME_PLAYEREXECUTER_H
#define GAME_PLAYEREXECUTER_H

#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharCommand;

namespace Player {

namespace Behavior {
class Behavior;
class Factory;
} // namespace Behavior

/* Executer (class name from RTTI; vtable 0x80521A04): the task that runs a player character's
 * behaviors (Player::Npc::SonicType::SonicTypeExecuter and BossSonic's executer derive from it).
 * Constructor 0x800A81D8, destructor 0x800A80DC. Only the members its users reach are named;
 * names are inferred. The other functions (command handling 0x800A78FC, the frame update
 * 0x800A7D78, the behavior switch 0x800A7F14) are called by their addresses. */
class Executer : public Task {
  public:
    Executer(Task *parent, const boost::shared_ptr<PlayerBase> &player);
    virtual ~Executer();

    /* 0x28 */ boost::shared_ptr<PlayerBase> m_player;
#ifdef PLAYER_EXECUTER_MEMBERS
    /* The members as Game/PlayerExecuter.cpp keeps them (names inferred): the running behavior,
     * the factory that makes them, and the time and count of the talking commands it sends. */
    /* 0x30 */ boost::shared_ptr<Behavior::Behavior> m_behavior;
    /* 0x38 */ boost::shared_ptr<Behavior::Factory> m_factory;
    /* 0x40 */ u8 field_0x40[0x48 - 0x40];
    /* 0x48 */ float m_waitTime;
    /* 0x4C */ s32 m_waitCount;
#else
    /* 0x30 */ Behavior::Behavior *m_behavior; /* the running behavior */
    /* 0x34 */ u8 field_0x34[0x50 - 0x34];
#endif
};
SIZE_ASSERT(Executer, 0x50);

} // namespace Player

#endif

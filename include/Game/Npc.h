#ifndef GAME_NPC_H
#define GAME_NPC_H

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Player.h"
#include "size_assert.h"

/* The non-player characters (class names and bases from RTTI: Player::Npc::Npc derives from
 * Player::PlayerBase, declared with its bases in Game/Player.h). Only what gadget code
 * reaches is declared; layouts beyond CharInfo are unknown. */
namespace Player {

namespace Npc {

class Npc : public PlayerBase {};

/* NpcCommand: the base of the commands only NPCs accept (class name from RTTI). */
class NpcCommand : public CharCommand {};

/* ShowCommand (class name from RTTI; constructor at 0x800732D0, which stores arg0 at 0x14): the
 * argument meanings are unknown. */
class ShowCommand : public NpcCommand {
  public:
    ShowCommand(int arg0, int arg1);

    /* 0x14 */ u32 field_0x14;
};

} // namespace Npc
} // namespace Player

SIZE_ASSERT(Player::Npc::ShowCommand, 0x18);

#endif

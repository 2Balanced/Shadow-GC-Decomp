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

/* Npc (vtable 0x8051FF2C): constructor 0x800742FC, destructor 0x80074224. The constructor's
 * arguments go to PlayerBase's constructor (0x80079F04) except arg3, which goes to the object at
 * 0x290 (Boss Sonic passes 3, 0xD, "BossSonic", 2; meanings unknown). Slot names are as in
 * CharInfo and PlayerBase; slots 0x18 to 0x20 return 1 and 0x24 and 0x28 return 0. */
class Npc : public PlayerBase {
  public:
    Npc(int arg0, int arg1, const char *name, int arg3);
    virtual ~Npc();
    virtual bool vf_0x18();
    virtual bool vf_0x1C();
    virtual bool vf_0x20();
    virtual bool vf_0x24();
    virtual bool vf_0x28();
    virtual void vf_0x34();
    virtual void vf_0x38();
    virtual bool IsWeaponHidden();

    /* 0x290 */ u32 field_0x290; /* an object of 0x24 bytes (0x800745C8) */
};

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

SIZE_ASSERT(Player::Npc::Npc, 0x294);
SIZE_ASSERT(Player::Npc::ShowCommand, 0x18);

#endif

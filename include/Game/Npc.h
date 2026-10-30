#ifndef GAME_NPC_H
#define GAME_NPC_H

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/Player.h"
#include "size_assert.h"

/* The non-player characters (class names and bases from RTTI: Player::Npc::Npc derives from
 * Player::PlayerBase, declared with its bases in Game/Player.h). Only what gadget code
 * reaches is declared; layouts beyond CharInfo are unknown. */
#ifdef NPC_IMPLEMENTATION
#include "boost/scoped_ptr.hpp"
class UnkNpcState_800745C8;
#endif

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
#ifdef NPC_IMPLEMENTATION
    /* Inline in Game/Npc.cpp, whose vtable emits their out-of-line copies. */
    virtual bool vf_0x18() {
        return true;
    }
    virtual bool vf_0x1C() {
        return true;
    }
    virtual bool vf_0x20() {
        return true;
    }
    virtual bool vf_0x24() {
        return false;
    }
    virtual bool vf_0x28() {
        return false;
    }
#else
    virtual bool vf_0x18();
    virtual bool vf_0x1C();
    virtual bool vf_0x20();
    virtual bool vf_0x24();
    virtual bool vf_0x28();
#endif
    virtual void vf_0x34();
    virtual void vf_0x38();
    virtual bool IsWeaponHidden();

#ifdef NPC_IMPLEMENTATION
    /* The NPC's state (Game/Npc.cpp; class name unknown). */
    /* 0x290 */ boost::scoped_ptr<UnkNpcState_800745C8> m_state;
#else
    /* 0x290 */ u32 field_0x290; /* an object of 0x24 bytes (0x800745C8) */
#endif
};

/* NpcCommand: the base of the commands only NPCs accept (class name from RTTI; vtable 0x8051FE98;
 * constructor 0x800734E4 and destructor 0x80073488, Game/NpcCommand.cpp). */
class NpcCommand : public CharCommand {
  public:
    NpcCommand(u16 type, int priority);
    virtual ~NpcCommand();
};

/* ShowCommand (class name from RTTI; vtable 0x8051FE54; constructor at 0x800732D0, which stores
 * arg0 at 0x14): shows the NPC; arg0 picks how (0: back to control 2, 1: greet the player first, 2:
 * control 4; meanings inferred from its Reject in Game/NpcCommand.cpp), arg1 is the priority. */
class ShowCommand : public NpcCommand {
  public:
    ShowCommand(int arg0, int arg1);
    virtual ~ShowCommand();
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
    virtual const char *GetName();
#endif

    /* 0x14 */ u32 field_0x14;
};

/* HideCommand (class name from RTTI; vtable 0x8051FE80; constructor 0x80073444): hides the NPC
 * (Game/NpcCommand.cpp). */
class HideCommand : public NpcCommand {
  public:
    HideCommand();
    virtual ~HideCommand();
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
    virtual const char *GetName();
#endif
};

/* EnemyDeadByPlayerCommand (class name from RTTI; vtable 0x8051FE28; constructor 0x80072FF4, which
 * passes 0x403 and 4 to its base): tells the partner that the player destroyed an enemy of a kind,
 * with the number of that kind the player has destroyed (Game/EnemyEntry.cpp; names inferred). */
class EnemyDeadByPlayerCommand : public NpcCommand {
  public:
    EnemyDeadByPlayerCommand(int kind, int count);
    virtual ~EnemyDeadByPlayerCommand();
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
    virtual std::string vf_0x10() const;
    virtual const char *GetName();
#endif

    /* 0x14 */ s32 m_kind;
    /* 0x18 */ s32 m_count;
};

} // namespace Npc
} // namespace Player

SIZE_ASSERT(Player::Npc::Npc, 0x294);
SIZE_ASSERT(Player::Npc::ShowCommand, 0x18);
SIZE_ASSERT(Player::Npc::HideCommand, 0x14);
SIZE_ASSERT(Player::Npc::EnemyDeadByPlayerCommand, 0x1C);

#endif

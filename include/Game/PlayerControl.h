#ifndef GAME_PLAYERCONTROL_H
#define GAME_PLAYERCONTROL_H

#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* Target (class name from RTTI): units that use it declare its members. */
class Target;

namespace Player {
namespace Control {

class InitializeArgs;

/* ControlBase (class name from RTTI; vtable 0x8051FC40): what steers a character, standing in for
 * the pad (Player::Control::UserInput) or an NPC's AI (Approach, Bukuu, the Npc and BossSonic
 * controls). Constructor 0x8009DCC0, destructor 0x8006D1A8. The three middle slots are pure
 * virtual and their roles unknown (Game/BossSonicControlRestart.cpp overrides them). Member names
 * are inferred; arg's meaning is unknown (ControlRestart passes 4). */
class ControlBase {
  public:
    ControlBase(const boost::shared_ptr<PlayerBase> &player, const char *name, int arg);
    virtual ~ControlBase();
#ifdef PLAYER_CONTROL_SLOT_ARGS
    /* The slots with the arguments Player::Controller passes (0x8009F398): entering the control
     * (after control prevId, with its InitializeArgs) and leaving it (for control nextId; meanings
     * inferred). The other units keep the declarations without arguments, which name the symbols
     * of the Boss Sonic controls. */
    virtual void Enter(int prevId, const InitializeArgs &args) = 0;
    virtual void Leave(int nextId) = 0;
#else
    virtual void vf_0x0C() = 0;
    virtual void vf_0x10() = 0;
#endif
    /* Slot 0x14: the per-frame update; Boss Sonic's ControlAI passes dt on to its desires
     * (Game/BossSonicControlAI.cpp). */
    virtual void vf_0x14(float dt) = 0;
    /* Slot 0x18: empty in ControlBase (0x8006D14C). */
    virtual void vf_0x18();

    /* 0x04 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x0C */ const char *m_name;
    /* 0x10 */ s32 field_0x10;
};
SIZE_ASSERT(ControlBase, 0x14);

/* InitializeArgs (class name from RTTI; vtable 0x8051FC94): the control to switch to (as in
 * Game/BossSonicExecuter.cpp, which declares it itself). */
class InitializeArgs {
  public:
    InitializeArgs(int id) : m_id(id) {}
    virtual ~InitializeArgs() {}

    /* 0x4 */ s32 m_id;
};
SIZE_ASSERT(InitializeArgs, 0x8);

/* InitializeArgsApproach (class name and base from RTTI; vtable 0x80520D4C): control 1,
 * Player::Control::Approach, towards a target (Player::ApproachControlCommand makes it). Its
 * constructor and destructor are out of line in Game/PlayerCommands' unit (0x8009B470,
 * 0x8009B400). Member names are inferred: kind 1 picks the target's owner up, kind 2 turns it
 * over. */
class InitializeArgsApproach : public InitializeArgs {
  public:
    InitializeArgsApproach(int kind, const boost::weak_ptr< ::Target> &target)
        : InitializeArgs(1), m_kind(kind), m_target(target) {}

    /* 0x08 */ s32 m_kind;
    /* 0x0C */ boost::weak_ptr< ::Target> m_target;
};
SIZE_ASSERT(InitializeArgsApproach, 0x14);

} // namespace Control
} // namespace Player

#endif

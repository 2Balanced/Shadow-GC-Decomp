#ifndef GAME_PLAYERCONTROL_H
#define GAME_PLAYERCONTROL_H

#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Player {
namespace Control {

/* ControlBase (class name from RTTI; vtable 0x8051FC40): what steers a character, standing in for
 * the pad (Player::Control::UserInput) or an NPC's AI (Approach, Bukuu, the Npc and BossSonic
 * controls). Constructor 0x8009DCC0, destructor 0x8006D1A8. The three middle slots are pure
 * virtual and their roles unknown (Game/BossSonicControlRestart.cpp overrides them). Member names
 * are inferred; arg's meaning is unknown (ControlRestart passes 4). */
class ControlBase {
  public:
    ControlBase(const boost::shared_ptr<PlayerBase> &player, const char *name, int arg);
    virtual ~ControlBase();
    virtual void vf_0x0C() = 0;
    virtual void vf_0x10() = 0;
    virtual void vf_0x14() = 0;
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

} // namespace Control
} // namespace Player

#endif

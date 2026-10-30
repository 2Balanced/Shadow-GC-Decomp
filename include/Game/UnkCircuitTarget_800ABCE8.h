#ifndef GAME_UNKCIRCUITTARGET_800ABCE8_H
#define GAME_UNKCIRCUITTARGET_800ABCE8_H

#include "Game/CharInfo.h"
#include "Game/PathCtrl.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* The object an ElectricCircuitCommand carries: a shared pointer to the path the character
 * follows (Player::PathImpl, class name from the RTTI of its shared_ptr control block; defined in
 * Game/PlayerPath.cpp). */
namespace Player {
class PathImpl;
}

/* (Moved here from Game/ElecCircuitEntrance.cpp: Player::Behavior::ChaosControl keeps one as a
 * member, Game/PlayerBehaviorChaosControl.cpp.) */
class UnkCircuitTarget_800ABCE8 {
  public:
    /* An empty target (0x800ABD9C; the destructor is 0x800ABC9C). */
    UnkCircuitTarget_800ABCE8();
    UnkCircuitTarget_800ABCE8(const boost::weak_ptr<CharInfo> &character,
                              SonicteamUSA::System::PathCtrl *path, int arg2);
    ~UnkCircuitTarget_800ABCE8();

    /* 0x0 */ boost::shared_ptr<Player::PathImpl> m_object;
};

#endif

#ifndef GAME_UNKCIRCUITTARGET_800ABCE8_H
#define GAME_UNKCIRCUITTARGET_800ABCE8_H

#include "Game/CharInfo.h"
#include "Game/PathCtrl.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

/* The object an ElectricCircuitCommand carries (class name unknown): a shared pointer to the
 * object fn_800ABCE8 finds for the character. */
class UnkCircuitObject_800ABCE8;

/* (Moved here from Game/ElecCircuitEntrance.cpp: Player::Behavior::ChaosControl keeps one as a
 * member, Game/PlayerBehaviorChaosControl.cpp.) */
class UnkCircuitTarget_800ABCE8 {
  public:
    /* An empty target (0x800ABD9C; the destructor is 0x800ABC9C). */
    UnkCircuitTarget_800ABCE8();
    UnkCircuitTarget_800ABCE8(const boost::weak_ptr<CharInfo> &character,
                              SonicteamUSA::System::PathCtrl *path, int arg2);

    /* 0x0 */ boost::shared_ptr<UnkCircuitObject_800ABCE8> m_object;
};

#endif

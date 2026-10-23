#ifndef GAME_BOSSSONICTARGET_H
#define GAME_BOSSSONICTARGET_H

#include "Game/Npc.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace Player {
namespace Npc {

/* BossSonicTarget (class name from RTTI; vtable 0x8055276C): the aim of Boss Sonic, built like
 * NpcTarget (Game/NpcTarget.h): its state lives in an Impl behind a boost::scoped_ptr, and slot
 * 0x28 returns the target it picked. Boss Sonic's setup (Game/BossSonic.cpp) constructs it
 * (0x80295970); Game/BossSonicTarget.cpp defines both. */
class BossSonicTarget : public PlayerTargetBase {
  public:
    class Impl;

    BossSonicTarget(const boost::shared_ptr<Npc> &npc);
    virtual ~BossSonicTarget();
    virtual void vf_0x0C();
    virtual boost::weak_ptr<Target> vf_0x28();

    /* 0x18 */ boost::scoped_ptr<Impl> m_impl;
};
SIZE_ASSERT(BossSonicTarget, 0x1C);

} // namespace Npc
} // namespace Player

#endif

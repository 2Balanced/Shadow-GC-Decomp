#ifndef GAME_NPCTARGET_H
#define GAME_NPCTARGET_H

#include "Game/Npc.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace Player {
namespace Npc {

/* NpcTarget (class name from RTTI; vtable 0x80543E34): the aim of an NPC partner. Each frame it
 * picks the target nearest the NPC among those of the kinds its character type allows; slot
 * 0x28 returns it. The NPC character units construct it (0x8020705C, called from 0x8020F3FC to
 * 0x80219AA4). Its state lives in an Impl behind a boost::scoped_ptr; Game/PlayerNpcTarget.cpp
 * defines both. */
class NpcTarget : public PlayerTargetBase {
  public:
    class Impl;

    NpcTarget(const boost::shared_ptr<Npc> &npc);
    virtual ~NpcTarget();
    virtual void vf_0x0C();
    virtual boost::weak_ptr<Target> vf_0x28();

    /* 0x18 */ boost::scoped_ptr<Impl> m_impl;
};
SIZE_ASSERT(NpcTarget, 0x1C);

} // namespace Npc
} // namespace Player

#endif

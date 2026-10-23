/* Player::Npc::SonicType::SonicTypeControlFactory (class name from RTTI): the control factory of
 * the Sonic-type NPCs (fn_802970D8 makes one for an NPC). It is FlyTypeControlFactory
 * (Game/FlyTypeControlFactory.cpp) with the Sonic-type NPCs' own control as 2
 * (@unnamed@NpcControlSonicType_cpp@::NpcSonicType, 0x8006DE34). File name from the class;
 * function names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "Game/PlayerFactory.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using Player::PlayerBase;
using Player::Control::ControlBase;

extern "C" {
/* The controls this factory makes (see the file comment; names unknown). */
ControlBase *fn_8009F9FC(const shared_ptr<PlayerBase> &player);
ControlBase *fn_8006DE34(const shared_ptr<PlayerBase> &player);
ControlBase *fn_8006D150(const shared_ptr<PlayerBase> &player);
ControlBase *fn_8006D268(const shared_ptr<PlayerBase> &player);
ControlBase *fn_8009E324(const shared_ptr<PlayerBase> &player);
}

namespace Player {
namespace Npc {
namespace SonicType {

class SonicTypeControlFactory : public Control::Factory {
  public:
    SonicTypeControlFactory(const shared_ptr<Npc> &npc);
    virtual ~SonicTypeControlFactory();
    virtual shared_ptr<ControlBase> Create(int id);

    /* 0x4 */ shared_ptr<Npc> m_npc;
};
SIZE_ASSERT(SonicTypeControlFactory, 0xC);

SonicTypeControlFactory::SonicTypeControlFactory(const shared_ptr<Npc> &npc) : m_npc(npc) {}

SonicTypeControlFactory::~SonicTypeControlFactory() {}

shared_ptr<ControlBase> SonicTypeControlFactory::Create(int id) {
    shared_ptr<ControlBase> control;
    switch (id) {
    case 0:
        control = shared_ptr<ControlBase>(fn_8009F9FC(m_npc));
        break;
    case 2:
        control = shared_ptr<ControlBase>(fn_8006DE34(m_npc));
        break;
    case 3:
        control = shared_ptr<ControlBase>(fn_8006D150(m_npc));
        break;
    case 4:
        control = shared_ptr<ControlBase>(fn_8006D268(m_npc));
        break;
    case 5:
        control = shared_ptr<ControlBase>(fn_8009E324(m_npc));
        break;
    }
    return control;
}

} // namespace SonicType
} // namespace Npc
} // namespace Player

/* Creates the control factory of a Sonic-type NPC (called by the NPC units; name inferred). */
extern "C" shared_ptr<Player::Control::Factory> fn_802970D8(const shared_ptr<Player::Npc::Npc> &npc) {
    return shared_ptr<Player::Control::Factory>(new Player::Npc::SonicType::SonicTypeControlFactory(npc));
}

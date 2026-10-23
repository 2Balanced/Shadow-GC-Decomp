/* Player::Npc::FlyType::FlyTypeControlFactory (class name from RTTI): the control factory of the
 * flying NPCs (fn_8029642C makes one for an NPC). Control 0 is 0x8009F9FC, 2 the flying NPCs' own
 * (@unnamed@NpcControlFlyType_cpp@::NpcFlyType, 0x801F326C), 3 the NPCs' no-control (0x8006D150),
 * 4 0x8006D268 and 5 0x8009E324; other ids make none. File name from the class; function names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
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
ControlBase *fn_801F326C(const shared_ptr<PlayerBase> &player);
ControlBase *fn_8006D150(const shared_ptr<PlayerBase> &player);
ControlBase *fn_8006D268(const shared_ptr<PlayerBase> &player);
ControlBase *fn_8009E324(const shared_ptr<PlayerBase> &player);
}

namespace Player {
namespace Npc {
namespace FlyType {

class FlyTypeControlFactory : public Control::Factory {
  public:
    FlyTypeControlFactory(const shared_ptr<Npc> &npc);
    virtual ~FlyTypeControlFactory();
    virtual shared_ptr<ControlBase> Create(int id);

    /* 0x4 */ shared_ptr<Npc> m_npc;
};
SIZE_ASSERT(FlyTypeControlFactory, 0xC);

FlyTypeControlFactory::FlyTypeControlFactory(const shared_ptr<Npc> &npc) : m_npc(npc) {}

FlyTypeControlFactory::~FlyTypeControlFactory() {}

shared_ptr<ControlBase> FlyTypeControlFactory::Create(int id) {
    shared_ptr<ControlBase> control;
    switch (id) {
    case 0:
        control = shared_ptr<ControlBase>(fn_8009F9FC(m_npc));
        break;
    case 2:
        control = shared_ptr<ControlBase>(fn_801F326C(m_npc));
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

} // namespace FlyType
} // namespace Npc
} // namespace Player

/* Creates the control factory of a flying NPC (called by the NPC units; name inferred). */
extern "C" shared_ptr<Player::Control::Factory> fn_8029642C(const shared_ptr<Player::Npc::Npc> &npc) {
    return shared_ptr<Player::Control::Factory>(new Player::Npc::FlyType::FlyTypeControlFactory(npc));
}

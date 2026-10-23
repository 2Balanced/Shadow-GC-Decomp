/* Player::Npc::BossSonic::SonicTypeControl (class name from RTTI): the control factory of Boss
 * Sonic (Game/BossSonic.cpp hands it to the NPC's controller). Controls 0 and 2 are the AI
 * (Game/BossSonicControlAI.cpp), 3 the NPCs' no-control (0x8006D150), 4 the restart
 * (Game/BossSonicControlRestart.cpp) and 5 another of the game's controls (0x8009E324); other ids
 * make none. The file name is the class name with its owner's in front, to keep it apart from the
 * NPCs' SonicType factories. Function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/BossSonic.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "Game/PlayerFactory.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using Player::PlayerBase;
using Player::Control::ControlBase;
using Player::Npc::Npc;
using Player::Npc::BossSonic::BossSonicWork;

extern "C" {
/* The controls this factory makes (see the file comment; names unknown). */
ControlBase *fn_8029499C(const shared_ptr<Npc> &npc, const shared_ptr<BossSonicWork> &work);
ControlBase *fn_8006D150(const shared_ptr<PlayerBase> &player);
ControlBase *fn_8033D79C(const shared_ptr<BossSonicWork> &work);
ControlBase *fn_8009E324(const shared_ptr<PlayerBase> &player);
}

namespace Player {
namespace Npc {
namespace BossSonic {

class SonicTypeControl : public Control::Factory {
  public:
    SonicTypeControl(const shared_ptr<Npc> &npc, const shared_ptr<BossSonicWork> &work);
    virtual ~SonicTypeControl();
    virtual shared_ptr<ControlBase> Create(int id);

    /* 0x04 */ shared_ptr<Npc> m_npc;
    /* 0x0C */ shared_ptr<BossSonicWork> m_work;
};
SIZE_ASSERT(SonicTypeControl, 0x14);

SonicTypeControl::SonicTypeControl(const shared_ptr<Npc> &npc, const shared_ptr<BossSonicWork> &work)
    : m_npc(npc), m_work(work) {}

SonicTypeControl::~SonicTypeControl() {}

shared_ptr<ControlBase> SonicTypeControl::Create(int id) {
    shared_ptr<ControlBase> control;
    switch (id) {
    case 0:
        control = shared_ptr<ControlBase>(fn_8029499C(m_npc, m_work));
        break;
    case 2:
        control = shared_ptr<ControlBase>(fn_8029499C(m_npc, m_work));
        break;
    case 3:
        control = shared_ptr<ControlBase>(fn_8006D150(m_npc));
        break;
    case 4:
        control = shared_ptr<ControlBase>(fn_8033D79C(m_work));
        break;
    case 5:
        control = shared_ptr<ControlBase>(fn_8009E324(m_npc));
        break;
    }
    return control;
}

} // namespace BossSonic
} // namespace Npc
} // namespace Player

/* Creates Boss Sonic's control factory (called by Game/BossSonic.cpp; name inferred). */
extern "C" shared_ptr<Player::Control::Factory> fn_802951EC(const shared_ptr<Npc> &npc,
                                                            const shared_ptr<BossSonicWork> &work) {
    return shared_ptr<Player::Control::Factory>(new Player::Npc::BossSonic::SonicTypeControl(npc, work));
}

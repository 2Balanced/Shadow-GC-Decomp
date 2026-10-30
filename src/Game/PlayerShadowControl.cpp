/* Player::Shadow::ShadowControl (class name from RTTI): Shadow's control factory
 * (fn_800AEB10 makes one for him, Game/PlayerShadow.cpp). Controls 0 and 1 are two of the game's
 * player controls (0x8009F9FC and 0x8009DD10) and 5 another (0x8009E324, as for Boss Sonic, see
 * Game/BossSonicSonicTypeControl.cpp); other ids make none. File name from the class; function
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
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
ControlBase *fn_8009DD10(const shared_ptr<PlayerBase> &player);
ControlBase *fn_8009E324(const shared_ptr<PlayerBase> &player);
}

namespace Player {
namespace Shadow {

class ShadowControl : public Control::Factory {
  public:
    ShadowControl(const shared_ptr<PlayerBase> &player);
    virtual ~ShadowControl();
    virtual shared_ptr<ControlBase> Create(int id);

    /* 0x4 */ shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(ShadowControl, 0xC);

ShadowControl::ShadowControl(const shared_ptr<PlayerBase> &player) : m_player(player) {}

ShadowControl::~ShadowControl() {}

shared_ptr<ControlBase> ShadowControl::Create(int id) {
    shared_ptr<ControlBase> control;
    switch (id) {
    case 0:
        control = shared_ptr<ControlBase>(fn_8009F9FC(m_player));
        break;
    case 1:
        control = shared_ptr<ControlBase>(fn_8009DD10(m_player));
        break;
    case 5:
        control = shared_ptr<ControlBase>(fn_8009E324(m_player));
        break;
    }
    return control;
}

} // namespace Shadow
} // namespace Player

/* Creates Shadow's control factory (called by Game/PlayerShadow.cpp; name inferred). */
extern "C" shared_ptr<Player::Control::Factory> fn_800AEB10(const shared_ptr<PlayerBase> &player) {
    return shared_ptr<Player::Control::Factory>(new Player::Shadow::ShadowControl(player));
}

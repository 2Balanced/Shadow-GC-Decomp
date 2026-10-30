/* Player::Behavior::Hide (class name from RTTI): behavior 0x23 of the flying NPCs
 * (Game/FlyTypeBehaviorFactory.cpp): while it runs, it holds the player's state bits 0x27 and 0x43
 * (meanings unknown). It cannot follow itself. File name inferred from the class; function names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* Sets and clears a bit of the player's state bits (see Game/Player.h). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
}

namespace Player {
namespace Behavior {

class Hide : public Behavior {
  public:
    Hide(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Hide();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's function (name inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);

    static const char *s_name;
};
SIZE_ASSERT(Hide, 0x1C);

const char *Hide::s_name = "Hide";

Hide::Hide(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0x23) {}

Hide::~Hide() {}

bool Hide::vf_0x0C(int prevId, const InitializeArgs &args) {
    return prevId != 0x23;
}

void Hide::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CA8(m_player->m_stateBits, 0x27);
    fn_80076CA8(m_player->m_stateBits, 0x43);
}

void Hide::Leave(int nextId) {
    fn_80076CC8(m_player->m_stateBits, 0x27);
    fn_80076CC8(m_player->m_stateBits, 0x43);
}

void Hide::vf_0x18(CharCommand *command) {}

void Hide::Update(float dt) {}

Behavior *Hide::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Hide(player);
}

} // namespace Behavior
} // namespace Player

/* The factory record (Game/FlyTypeBehaviorFactory.cpp). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804D30E8 = {Player::Behavior::Hide::Create, 0, 0};

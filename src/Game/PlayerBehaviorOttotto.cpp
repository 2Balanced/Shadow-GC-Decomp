/* Player::Behavior::Ottotto (class name from RTTI; "ottotto" is a Japanese "whoops"): behavior 4 of
 * Shadow, teetering on an edge. It plays the teeter motion and its voice once, and stops (behavior
 * 0) when fn_800A9E1C says so. Its factory record has no second function. File name inferred from
 * the class (as Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/CharCommand.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
}

namespace Player {
namespace Behavior {

class Ottotto : public Behavior {
  public:
    Ottotto(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Ottotto();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void Update(float dt);

    /* The factory record's function (name inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);

    /* 0x1C */ s32 m_step; /* 0 until the voice has played */
};
SIZE_ASSERT(Ottotto, 0x20);

Ottotto::Ottotto(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, "Ottotto", 4) {}

Ottotto::~Ottotto() {}

bool Ottotto::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void Ottotto::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80076CA8(m_player.get()->m_stateBits, 2);
    m_player.get()->m_motion->vf_0x10(0xB, 0.0f);
    m_step = 0;
}

void Ottotto::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
}

void Ottotto::Update(float dt) {
    switch (m_step) {
    case 0:
        m_player.get()->m_sound->StopVoice(0x15);
        m_step++;
        break;
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (fn_800A9E1C(&m_player)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
    }
}

Behavior *Ottotto::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Ottotto(player);
}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Ottotto by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0988 = {
    Player::Behavior::Ottotto::Create, 0, 0};

/* Player::Behavior::Landing (class name from RTTI): behavior 3 of Shadow, landing. Touching the
 * ground stops the player (unless it slides, state bit 0x1B); after a short pause it runs on
 * (behavior 1, Player::Behavior::InitializeArgsGround) if it still moves, and it stops (behavior 0)
 * when fn_800A9E1C says so. Its factory record has no second function. File name inferred from the
 * class (as Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
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

void fn_8040CE60(Vec3f *vector);   /* vector = 0 */
float fn_8040CF2C(const Vec3f *v); /* |v|^2 */
}

namespace Player {
namespace Behavior {

class Landing : public Behavior {
  public:
    Landing(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Landing();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void Update(float dt);

    /* The factory record's function (name inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);

    /* 0x1C */ float m_time; /* the pause before running on */
    /* 0x20 */ bool m_landed;
};
SIZE_ASSERT(Landing, 0x24);

Landing::Landing(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, "Landing", 3) {}

Landing::~Landing() {}

bool Landing::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void Landing::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80076CA8(m_player.get()->m_stateBits, 0x3E);
    fn_8006127C(&m_player.get()->m_flags, 0);
    fn_80014E1C(&m_player.get()->m_flags, 5);
    fn_80014E1C(&m_player.get()->m_flags, 7);
    m_player.get()->m_motion->vf_0x10(4, 5.0f);
    m_time = 0.1f;
    m_landed = false;
}

void Landing::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 0x3E);
    fn_8006127C(&m_player.get()->m_flags, 1);
    fn_8006127C(&m_player.get()->m_flags, 5);
    fn_8006127C(&m_player.get()->m_flags, 6);
    fn_8006127C(&m_player.get()->m_flags, 7);
}

void Landing::Update(float dt) {
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (fn_800A9E1C(&m_player)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
        return;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        if (!m_landed) {
            m_landed = true;
            if (!fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
                fn_8040CE60(&m_player.get()->field_0x18);
            }
        }
        m_time -= dt;
        fn_80014E1C(&m_player.get()->m_flags, 6);
        fn_80014E1C(&m_player.get()->m_flags, 1);
        if ((m_time <= 0.0f && fn_8040CF2C(&m_player.get()->field_0x18) > 2500.0f) ||
            fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
            fn_800A7F14(m_player.get()->m_behaviors,
                        &(const InitializeArgs &)InitializeArgsGround(false));
        }
    }
}

Behavior *Landing::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Landing(player);
}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Landing by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0858 = {
    Player::Behavior::Landing::Create, 0, 0};

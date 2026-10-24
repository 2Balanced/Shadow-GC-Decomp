/* Player::Behavior::Idle (class name from RTTI): behavior 0 of Shadow, standing still. While it
 * runs (state bit 0x47) it marks the player as standing (state bit 1) when it stands still on flat
 * ground; it falls (behavior 2) off the ground and runs (behavior 1,
 * Player::Behavior::InitializeArgsGround) once the player moves. Command 0x132 plays the idle
 * motion backwards, and every fifth one takes the command. File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
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
/* Whether the player stands on ground flat enough to stand still on: not state bit 0x1B, and its
 * jump direction (0xE8) close enough to the ground's normal (0x1C8; behaviour inferred). */
bool fn_8009330C(boost::shared_ptr<Player::PlayerBase> *player);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Calls on the player's motion (meanings unknown; fn_800AA7B8 sets the motion's word at 0x438, as
 * in AirAttack). */
void fn_800AA700(Player::PlayerMotion *motion, int arg);
void fn_800AA7B8(Player::PlayerMotion *motion, float value);

float fn_8040CF2C(const Vec3f *v); /* |v|^2 */
}

/* Whether a vector is about zero (an inline function in the game: the player's operator-> in its
 * argument stays out of line; name inferred). */
inline bool IsAboutZero(const Vec3f &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z < 0.0001f;
}

namespace Player {
namespace Behavior {

class Idle : public Behavior {
  public:
    Idle(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Idle();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    static const char *s_name;

    /* 0x1C */ s32 m_count; /* commands 0x132 since the last one taken */
};
SIZE_ASSERT(Idle, 0x20);

const char *Idle::s_name = "Idle";

Idle::Idle(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0) {}

Idle::~Idle() {}

void Idle::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x132:
        m_count++;
        if (m_count >= 5 && !fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
            m_count = 0;
            fn_800AA7B8(m_player.get()->m_motion, -2.0f);
            command->Flags() |= 2;
        } else {
            fn_800AA7B8(m_player.get()->m_motion, -1.0f);
        }
        break;
    }
}

bool Idle::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void Idle::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CA8(m_player.get()->m_stateBits, 0x3E);
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_8006127C(&m_player.get()->m_flags, 0);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    fn_80014E1C(&m_player.get()->m_flags, 3);
    fn_80014E1C(&m_player.get()->m_flags, 5);
    fn_80014E1C(&m_player.get()->m_flags, 6);
    fn_80014E1C(&m_player.get()->m_flags, 7);
    fn_80014E1C(&m_player.get()->m_flags, 9);
    fn_800AA700(m_player.get()->m_motion, 1);
    m_player.get()->m_motion->vf_0x10(0, 5.0f);
    m_count = 0;
}

void Idle::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 0x3E);
    fn_80076CC8(m_player.get()->m_stateBits, 1);
    fn_8006127C(&m_player.get()->m_flags, 1);
    fn_8006127C(&m_player.get()->m_flags, 3);
    fn_8006127C(&m_player.get()->m_flags, 5);
    fn_8006127C(&m_player.get()->m_flags, 6);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_8006127C(&m_player.get()->m_flags, 9);
    fn_800AA700(m_player.get()->m_motion, 0);
}

void Idle::Update(float dt) {
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x47)) {
        return;
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (fn_8009330C(&m_player) && !fn_8006CD78(m_player.get()->m_stateBits, 0x1B) &&
        IsAboutZero(m_player->field_0x18) &&
        fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        fn_80076CA8(m_player.get()->m_stateBits, 1);
    } else {
        fn_80076CC8(m_player.get()->m_stateBits, 1);
    }
    if (!fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    }
    if (fn_8040CF2C(&m_player.get()->field_0x18) > 25.0f ||
        fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
        fn_800A7F14(m_player.get()->m_behaviors,
                    &(const InitializeArgs &)InitializeArgsGround(false));
    }
}

Behavior *Idle::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Idle(player);
}

void Idle::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Idle by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0518 = {
    Player::Behavior::Idle::Create, Player::Behavior::Idle::Release, 0};

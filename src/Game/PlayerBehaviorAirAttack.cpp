/* Player::Behavior::AirAttack (class name from RTTI): behavior 0x1A of Shadow, using his weapon in
 * the air. Entering it marks the player's flags; with a weapon in hand (state bit 0x1F) it plays the
 * attack motion of the weapon's kind and starts using the weapon, without one it plays motion 0x21.
 * Each frame it turns the player and switches to behavior 2 once the shot has run out, or as soon
 * as the weapon is gone. File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
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
/* Player updates run before the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* The direction the player faces, and turning it (behaviour inferred; as in Super). */
Vec3f fn_80093FC0(boost::shared_ptr<Player::PlayerBase> *player);
void fn_80093BA0(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *direction, float rate);
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Sets the motion's word at 0x438 (meaning unknown). */
void fn_800AA7B8(Player::PlayerMotion *motion, float value);
/* Called with the player's object at 0x23C when the behavior starts and ends (behaviour
 * unknown). */
void fn_800A2D08(void *object);
void fn_800A2CE4(void *object);
}

namespace Player {
namespace Behavior {

class AirAttack : public Behavior {
  public:
    AirAttack(const boost::shared_ptr<PlayerBase> &player);
    virtual ~AirAttack();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void Turn(float dt);

    static const char *s_name;

    /* 0x1C */ s32 m_state;
    /* 0x20 */ float m_timer;
    /* 0x24 */ u8 m_armed; /* entered with a weapon */
};
SIZE_ASSERT(AirAttack, 0x28);

const char *AirAttack::s_name = "AirAttack";

AirAttack::AirAttack(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0x1A) {}

AirAttack::~AirAttack() {}

bool AirAttack::vf_0x0C(int prevId, const InitializeArgs &args) {
    return prevId != 0x1A;
}

void AirAttack::Enter(int prevId, const InitializeArgs &args) {
    m_state = 0;
    fn_80076CA8(m_player.get()->m_stateBits, 0x1D);
    fn_80014E1C(&m_player.get()->m_flags, 6);
    fn_80014E1C(&m_player.get()->m_flags, 7);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1F)) {
        switch (m_player.get()->m_weapon->GetWeaponKind()) {
        case 1:
        case 2:
        case 6:
            m_player.get()->m_motion->vf_0x10(0x23, 0.0f);
            m_player.get()->m_weapon->BeginUse();
            m_timer = 0.3f;
            m_state = 2;
            break;
        case 3:
            m_player.get()->m_motion->vf_0x10(0x23, 0.0f);
            m_player.get()->m_weapon->BeginUse();
            m_state = 3;
            break;
        case 5:
            m_player.get()->m_motion->vf_0x10(0x22, 0.0f);
            break;
        case 4:
            m_player.get()->m_motion->vf_0x10(0x23, 0.0f);
            m_player.get()->m_weapon->BeginUse();
            m_state = 6;
            break;
        }
        m_armed = 1;
    } else {
        m_player.get()->m_motion->vf_0x10(0x21, 0.0f);
        m_armed = 0;
    }
    fn_800A2D08(reinterpret_cast<void *>(m_player.get()->field_0x23C));
}

void AirAttack::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0x1D);
    fn_8006127C(&m_player.get()->m_flags, 6);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_800A2CE4(reinterpret_cast<void *>(m_player.get()->field_0x23C));
}

void AirAttack::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x102:
        command->Flags() |= 2;
        break;
    case 0x103:
        command->Flags() |= 2;
        break;
    }
}

void AirAttack::Update(float dt) {
    switch (m_state) {
    case 0:
        if (fn_800A9E1C(&m_player)) {
            InitializeArgs args(2);
            fn_800A7F14(m_player.get()->m_behaviors, &args);
            return;
        }
        break;
    case 2:
        Turn(dt);
        if (m_player.get()->m_weapon->IsFiring()) {
            fn_800AA7B8(m_player.get()->m_motion, 1.0f);
            m_timer = 0.3f;
        }
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            InitializeArgs args(2);
            fn_800A7F14(m_player.get()->m_behaviors, &args);
            return;
        }
        break;
    case 3:
        m_state = 4;
        break;
    case 4:
        fn_80093AF8(&m_player, 2.0f * dt);
        if (m_player.get()->m_weapon->IsFiring()) {
            fn_800AA7B8(m_player.get()->m_motion, 1.0f);
        }
        if (!m_player.get()->m_weapon->IsFlag2()) {
            m_timer = 0.125f;
            m_state = 5;
        }
        break;
    case 5:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            InitializeArgs args(2);
            fn_800A7F14(m_player.get()->m_behaviors, &args);
            return;
        }
        break;
    case 6:
        m_state = 7;
        break;
    case 7:
        fn_80093AF8(&m_player, 2.0f * dt);
        if (!m_player.get()->m_weapon->IsFlag2()) {
            m_timer = 0.125f;
            m_state = 5;
        }
        break;
    }
    if (m_armed && !fn_8006CD78(m_player.get()->m_stateBits, 0x1F)) {
        InitializeArgs args(2);
        fn_800A7F14(m_player.get()->m_behaviors, &args);
        return;
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
}

/* Turns the player towards its aim while it locks on (state bit 0x3C), along its own heading
 * otherwise (behaviour inferred). */
void AirAttack::Turn(float dt) {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x3C)) {
        fn_80093BA0(&m_player, &(Vec3f &)fn_80093FC0(&m_player), 2.0f * dt);
    } else {
        fn_80093AF8(&m_player, 2.0f * dt);
    }
}

void AirAttack::vf_0x20() {}

Behavior *AirAttack::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new AirAttack(player);
}

void AirAttack::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds AirAttack by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AF784 = {
    Player::Behavior::AirAttack::Create, Player::Behavior::AirAttack::Release, 0};

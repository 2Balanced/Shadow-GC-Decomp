/* Player::Behavior::Dead (class name from RTTI): behavior 0x1F of Shadow, dying. Entering it drops
 * the player's weapon, plays the death motion and voice and, in two stages, an effect; once the
 * event state allows it, it switches the event to the game over (mode 9). It refuses to start when
 * the player is already dead or the game state forbids it. File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stage; /* the stage's number (meaning inferred) */
    /* 0x0AC */ u8 field_0xAC[0xB5 - 0xAC];
    /* 0x0B5 */ u8 field_0xB5; /* set: the player cannot die (meaning inferred) */
};

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The event state fn_80074470 returns (struct name unknown; as in
 * Game/PlayerBehaviorFirstMeet.cpp). */
struct UnkObject_80074470 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ s32 m_mode;
    /* 0xC */ s32 field_0xC;
};

extern "C" {
/* The event state, and setting its mode (behaviour inferred; as in FirstMeet). */
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int mode, int arg2);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Called on the player when the behavior starts and ends (behaviour unknown). */
void fn_800794C8(Player::PlayerBase *player);
void fn_8007922C(Player::PlayerBase *player);
void fn_800791BC(Player::PlayerBase *player);
/* Player updates run before the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Called with the player's effects at 0x23C, and starting effect id of them (behaviour
 * inferred). */
void fn_800A2B44(void *effects);
boost::weak_ptr<EffectElement> fn_800A2754(void *effects, int id);
/* Moves an effect to the player (behaviour inferred). */
void fn_80425E4C(EffectElement *effect, Player::PlayerBase *player);
}

namespace Player {
namespace Behavior {

class Dead : public Behavior {
  public:
    Dead(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Dead();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's function (name inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);

    /* 0x1C */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x24 */ u8 m_gameOver; /* the event was switched to the game over */
};
SIZE_ASSERT(Dead, 0x28);

Dead::Dead(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, "Dead", 0x1F) {}

Dead::~Dead() {
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
    }
}

bool Dead::vf_0x0C(int prevId, const InitializeArgs &args) {
    if (prevId == 0x1F) {
        return false;
    }
    return lbl_8057D6A0.field_0xB5 == 0;
}

void Dead::Enter(int prevId, const InitializeArgs &args) {
    m_player.get()->vf_0x48();
    if (m_player.get()->m_weapon != 0) {
        m_player.get()->m_weapon->DropWeapon();
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x24)) {
        fn_800A2B44(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    }
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80076CA8(m_player.get()->m_stateBits, 0x12);
    m_player.get()->m_motion->vf_0x10(0x2A, 5.0f);
    fn_800794C8(m_player.get());
    fn_8007922C(m_player.get());
    bool inAir = fn_8006CD78(m_player.get()->m_stateBits, 2);
    int voice = 0x11;
    if (inAir) {
        voice = 0x10;
    }
    m_player.get()->m_sound->StopVoice(voice);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x13)) {
        int id = -1;
        switch (lbl_8057D6A0.m_stage) {
        case 0x1F4:
            id = 0x272;
            break;
        case 0x25B:
            id = 0x276;
            break;
        }
        if (id > 0) {
            m_effect = fn_800A2754(reinterpret_cast<void *>(m_player.get()->field_0x23C), id);
        }
    }
    m_gameOver = 0;
}

void Dead::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 0x12);
    fn_80076CC8(m_player.get()->m_stateBits, 0x13);
    fn_800791BC(m_player.get());
}

void Dead::vf_0x18(CharCommand *command) {
    int type = command->TypeId();
    if ((u32)type >= 0x100) {
        switch (type) {
        case 0x15:
        case 0x101:
        case 0x117:
        case 0x127:
        case 0x137:
            break;
        default:
            command->Flags() |= 2;
            break;
        }
    }
}

void Dead::Update(float dt) {
    if (!m_gameOver) {
        UnkObject_80074470 *state = fn_80074470();
        if (state->m_mode == 3 && state->field_0xC == 0) {
            m_gameOver = 1;
            fn_80175EEC(state, 9, 0);
        }
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (!m_effect.expired()) {
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        fn_80425E4C(effect.get(), m_player.get());
    }
}

Behavior *Dead::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Dead(player);
}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Dead by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AFB10 = {
    Player::Behavior::Dead::Create, 0, 0};

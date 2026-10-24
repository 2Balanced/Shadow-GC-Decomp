/* Player::Behavior::ExternalControl (class name from RTTI): behavior 0x20 of Shadow, handing the
 * player to an outside controller (Player::Behavior::InitializeArgsExternalControl). Each frame
 * it copies the position and rotation the controller exposes; when asked it rides the controller
 * (RideCommand on entry, LeaveCommand on exit). Command 0x12C, or the controller going away, ends
 * it (behavior 2). File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_80080120 {
    /* 0x000 */ u8 field_0x0[0xF4];
    /* 0x0F4 */ Vec3f m_up;
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Sets the player's up vector (see Game/PlayerBehaviorChaosControl.cpp). */
void fn_8009339C(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *up);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_80080120 *>(player)

namespace Player {
namespace Behavior {

/* The behavior's flag word (struct name unknown): bit 0 asks to end. */
struct ExternalControlFlags {
    ExternalControlFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }
    void Reset() {
        fn_80014E10(&m_bits);
    }

    u32 m_bits;
};

class ExternalControl : public Behavior {
  public:
    ExternalControl(const boost::shared_ptr<PlayerBase> &player);
    virtual ~ExternalControl();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's function (name inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);

    /* 0x1C */ boost::weak_ptr<CharInfo> m_controller;
    /* 0x24 */ const Vec3f *m_position;
    /* 0x28 */ const Vec3f *m_rotation;
    /* 0x2C */ u32 m_flags; /* as in InitializeArgsExternalControl */
    /* 0x30 */ ExternalControlFlags m_state;
};
SIZE_ASSERT(ExternalControl, 0x34);

ExternalControl::ExternalControl(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, "ExternalControl", 0x20) {}

ExternalControl::~ExternalControl() {}

void ExternalControl::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x12C:
        m_state.Set(0);
        command->Flags() |= 2;
        break;
    }
}

bool ExternalControl::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void ExternalControl::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsExternalControl *control =
        dynamic_cast<const InitializeArgsExternalControl *>(&args);
    m_controller = control->m_controller;
    m_position = control->m_position;
    m_rotation = control->m_rotation;
    m_flags = control->m_flags;
    m_state.Reset();
    fn_80076CA8(m_player.get()->m_stateBits, 0x29);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    if (m_flags & 1) {
        fn_80014E1C(&m_player.get()->m_flags, 1);
    }
    if ((m_flags & 2) && !m_controller.expired()) {
        boost::shared_ptr<CharInfo> controller = m_controller.lock();
        controller->PushCommand(new RideCommand(boost::weak_ptr<CharInfo>(m_player.get()->m_self),
                                                m_player.get()->m_position));
    }
}

void ExternalControl::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0x29);
    fn_8006127C(&m_player.get()->m_flags, 1);
    if ((m_flags & 2) && !m_controller.expired()) {
        boost::shared_ptr<CharInfo> controller = m_controller.lock();
        controller->PushCommand(new LeaveCommand(boost::weak_ptr<CharInfo>(m_player.get()->m_self),
                                                 m_player.get()->m_position));
    }
}

void ExternalControl::Update(float dt) {
    if ((m_flags & 2) && m_controller.expired()) {
        m_state.Set(0);
    }
    if (m_flags & 4) {
        fn_800091D0(&m_player.get()->m_position, m_position);
    }
    if (m_flags & 8) {
        m_player->m_rotation = *m_rotation;
    }
    fn_8009339C(&m_player, &FieldsOf(m_player.get())->m_up);
    if (m_state.Test(0)) {
        InitializeArgs args(2);
        fn_800A7F14(m_player.get()->m_behaviors, &args);
    }
}

void ExternalControl::vf_0x20() {}

Behavior *ExternalControl::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new ExternalControl(player);
}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds ExternalControl by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AFD74 = {
    Player::Behavior::ExternalControl::Create, 0, 0};

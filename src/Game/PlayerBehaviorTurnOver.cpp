/* Player::Behavior::TurnOver (class name from RTTI): behavior 0xB of Shadow, working an object (a
 * switch or a lid; Player::Behavior::InitializeArgsTurnOver names the target the object publishes).
 * Entering it plays the motion, listens to the motion's events and tells the object the player is
 * ready (ReadyCommand). Commands 0x106 and 0x11C mark the action; at the motion's step the player
 * uses the object (UseCommand) and the motion runs on at full speed. Command 0xF from the object,
 * losing it, or fn_800A9E1C at the end stops it (behavior 0); stopping before the use cancels it
 * (CancelCommand). File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* Target (class name from RTTI; see Game/CharCommand.h): what a character aims at to work an
 * object. Only what this unit uses is declared (as in Game/PlayerBehaviorHomingAttack.cpp). */
class Target {
  public:
    /* The point to aim at, as seen by viewer (vtable slot 0x8). */
    virtual Vec3f GetPosition(CharInfo *viewer);

    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner; /* the character the target belongs to */
    /* 0x30 */ u32 m_flags;                       /* bit 0: it can be locked on to */
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit and fn_80014E34 tests bit (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The player's flag word (see Game/PlayerShadowSuper.cpp). */
void fn_8006127C(u32 *flags, int bit);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Sets the motion's speed (see Game/PlayerBehaviorSliding.cpp). */
void fn_800AA7B8(Player::PlayerMotion *motion, float speed);
}

namespace Player {
namespace Behavior {

/* The behavior's flag word (struct name unknown): bit 0 the motion's step (event 0x5000's value
 * 1), bit 1 the object used, bit 2 command 0xF, bit 3 the action (commands 0x106 and 0x11C), bit 4
 * the end. */
struct TurnOverFlags {
    TurnOverFlags() {
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

class TurnOver : public Behavior {
  public:
    TurnOver(const boost::shared_ptr<PlayerBase> &player);
    virtual ~TurnOver();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void OnMotion(MotionEventArgs &args);
    void SendReady();
    void SendUse();
    void SendCancel();

    static const char *s_name;

    /* 0x1C */ boost::weak_ptr<CharInfo> m_object; /* the object worked */
    /* 0x24 */ TurnOverFlags m_flags;
    /* 0x28 */ s32 m_step;
};
SIZE_ASSERT(TurnOver, 0x2C);

const char *TurnOver::s_name = "TurnOver";

TurnOver::TurnOver(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0xB) {}

TurnOver::~TurnOver() {}

bool TurnOver::vf_0x0C(int prevId, const InitializeArgs &args) {
    const InitializeArgsTurnOver &turn = dynamic_cast<const InitializeArgsTurnOver &>(args);
    if (turn.m_target.expired()) {
        return false;
    }
    boost::shared_ptr<Target> target = turn.m_target.lock();
    if (target->m_owner.expired()) {
        return false;
    }
    return true;
}

void TurnOver::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsTurnOver &turn = dynamic_cast<const InitializeArgsTurnOver &>(args);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    m_player.get()->m_motion->vf_0x10(0x1C, 5.0f);
    PlayerMotionEventCallback(m_player.get()) =
        MotionEventCallback(boost::bind(&TurnOver::OnMotion, this, _1));
    boost::shared_ptr<Target> target = turn.m_target.lock();
    m_object = target->m_owner.lock();
    SendReady();
    m_flags.Reset();
    m_step = 0;
}

void TurnOver::Leave(int nextId) {
    fn_8006127C(&m_player.get()->m_flags, 1);
    PlayerMotionEventCallback(m_player.get()).Clear();
    if (!m_flags.Test(1)) {
        SendCancel();
    }
    m_object.reset();
}

void TurnOver::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x104:
        if (m_flags.Test(3)) {
            command->Flags() |= 2;
        }
        break;
    case 0x106:
    case 0x11C:
        m_flags.Set(3);
        command->Flags() |= 2;
        break;
    }
}

void TurnOver::Update(float dt) {
    if (m_flags.Test(1)) {
        if (fn_800A9E1C(&m_player)) {
            m_flags.Set(4);
        }
    } else if (m_object.expired()) {
        m_flags.Set(4);
    } else {
        boost::shared_ptr<CharInfo> object = m_object.lock();
        switch (m_step) {
        case 0:
            if (fn_800A9E1C(&m_player)) {
                m_step++;
            }
            break;
        case 1:
            if (m_flags.Test(3)) {
                SendUse();
                fn_800AA7B8(m_player.get()->m_motion, 1.0f);
                m_step++;
            }
            break;
        case 2:
            if (fn_800A9E1C(&m_player)) {
                m_flags.Set(4);
            }
            break;
        }
        if (m_flags.Test(2)) {
            SendCancel();
            m_flags.Set(4);
        }
    }
    if (m_flags.Test(4)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
    }
}

void TurnOver::OnMotion(MotionEventArgs &args) {
    switch (args.m_event) {
    case 0x5000:
        switch (*args.m_value) {
        case 1:
            m_flags.Set(0);
            break;
        }
        break;
    }
}

void TurnOver::SendReady() {
    boost::shared_ptr<CharInfo> object = m_object.lock();
    if (object) {
        object->PushCommand(new ReadyCommand(m_player.get()->m_self));
    }
}

void TurnOver::SendUse() {
    boost::shared_ptr<CharInfo> object = m_object.lock();
    if (object) {
        object->PushCommand(new UseCommand(m_player.get()->m_self));
    }
    m_flags.Set(1);
}

void TurnOver::SendCancel() {
    boost::shared_ptr<CharInfo> object = m_object.lock();
    if (object) {
        object->PushCommand(new CancelCommand(m_player.get()->m_self));
    }
}

void TurnOver::vf_0x20() {}

Behavior *TurnOver::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new TurnOver(player);
}

void TurnOver::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds TurnOver by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B10B8 = {
    Player::Behavior::TurnOver::Create, Player::Behavior::TurnOver::Release, 0};

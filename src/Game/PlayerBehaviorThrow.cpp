/* Player::Behavior::Throw (class name from RTTI): behavior 0xA of Shadow, throwing the object the
 * player carries (Player::Behavior::InitializeArgsThrow may hand the player the object first). It
 * plays the throw motion and listens to the motion's events; at the release (event 0x5000's value
 * 3) the player lets go of the object (fn_800B7BCC), and when fn_800A9E1C says so it lets go and
 * stops (behavior 0). File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

/* Declares Player::PlayerBase's slots 0x40 and 0x4C with their types (see Game/Player.h). */
#define PLAYER_PICKUP_SLOT

#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/PickupCharInfo.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit and fn_80014E34 tests bit (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80076CC8(u32 *bits, int bit);
/* Counted holds on the player (see Game/PlayerBehaviorPickupObject.cpp). */
void fn_8007922C(Player::PlayerBase *player);
void fn_800791BC(Player::PlayerBase *player);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Lets go of the object the player carries (behaviour inferred: it sends the object a command; see
 * Game/PlayerBehaviorGrind.cpp). */
void fn_800B7BCC(boost::shared_ptr<Player::PlayerBase> *player);
}

namespace Player {
namespace Behavior {

/* The behavior's flag word (struct name unknown): bit 0 the release (event 0x5000's value 3). */
struct ThrowFlags {
    ThrowFlags() {
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

class Throw : public Behavior {
  public:
    Throw(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Throw();
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

    static const char *s_name;

    /* 0x1C */ s32 m_step; /* 0 until the release */
    /* 0x20 */ ThrowFlags m_flags;
};
SIZE_ASSERT(Throw, 0x24);

const char *Throw::s_name = "Throw";

Throw::Throw(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0xA) {}

Throw::~Throw() {}

bool Throw::vf_0x0C(int prevId, const InitializeArgs &args) {
    const InitializeArgsThrow &object = dynamic_cast<const InitializeArgsThrow &>(args);
    if (object.m_object.expired() && m_player.get()->vf_0x4C().expired()) {
        return false;
    }
    return true;
}

void Throw::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsThrow &object = dynamic_cast<const InitializeArgsThrow &>(args);
    if (!object.m_object.expired()) {
        m_player.get()->vf_0x40(object.m_object);
    }
    fn_8007922C(m_player.get());
    m_player.get()->m_motion->vf_0x10(0x18, 0.0f);
    PlayerMotionEventCallback(m_player.get()) =
        MotionEventCallback(boost::bind(&Throw::OnMotion, this, _1));
    fn_80076CC8(m_player.get()->m_stateBits, 0x3D);
    m_step = 0;
    m_flags.Reset();
}

void Throw::Leave(int nextId) {
    m_player.get()->vf_0x44();
    PlayerMotionEventCallback(m_player.get()).Clear();
    fn_800791BC(m_player.get());
}

void Throw::vf_0x18(CharCommand *command) {}

void Throw::Update(float dt) {
    switch (m_step) {
    case 0:
        if (m_flags.Test(0)) {
            fn_800B7BCC(&m_player);
            m_step++;
        }
        break;
    }
    if (fn_800A9E1C(&m_player)) {
        fn_800B7BCC(&m_player);
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
    }
}

void Throw::OnMotion(MotionEventArgs &args) {
    switch (args.m_event) {
    case 0x5000:
        switch (*args.m_value) {
        case 3:
            m_flags.Set(0);
            break;
        }
        break;
    }
}

void Throw::vf_0x20() {}

Behavior *Throw::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Throw(player);
}

void Throw::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Throw by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0F44 = {
    Player::Behavior::Throw::Create, Player::Behavior::Throw::Release, 0};

/* Player::Behavior::PickupWeapon (class name from RTTI): behavior 0x16 of Shadow, picking up a
 * weapon (Player::Behavior::InitializeArgsPickupWeapon), or dropping the one the player holds when
 * there is none to pick up. Entering it stops the player and plays the pickup motion; at the
 * motion's event (0x5000) the player sends the weapon a UseCommand or drops its weapon, and it
 * stops (behavior 0) when fn_800A9E1C says so. Its factory record has no second function. File name
 * inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
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

namespace boost {
/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * emits the copy for characters (boost 1.32's definition). */
template <> inline void weak_ptr<CharInfo>::reset() {
    this_type().swap(*this);
}
} // namespace boost

extern "C" {
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Sets or clears the motion's flag 0, which raises the weapon (see Game/PlayerShadowWeapon.cpp). */
void fn_800AA6C0(Player::PlayerMotion *motion, bool raise);

void fn_8040CE60(Vec3f *vector); /* vector = 0 */
}

namespace Player {
namespace Behavior {

class PickupWeapon : public Behavior {
  public:
    PickupWeapon(const boost::shared_ptr<PlayerBase> &player);
    virtual ~PickupWeapon();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's function (name inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);

    void OnMotion(MotionEventArgs &args);

    /* 0x1C */ s32 m_step; /* 0 picking up, 1 dropping, 3 picking up again, 4 done */
    /* 0x20 */ bool m_motionEvent; /* the motion reached its event (0x5000) */
    /* 0x24 */ boost::weak_ptr<CharInfo> m_weapon;
};
SIZE_ASSERT(PickupWeapon, 0x2C);

PickupWeapon::PickupWeapon(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, "PickupWeapon", 0x16) {}

PickupWeapon::~PickupWeapon() {}

bool PickupWeapon::vf_0x0C(int prevId, const InitializeArgs &args) {
    const InitializeArgsPickupWeapon *pickup =
        dynamic_cast<const InitializeArgsPickupWeapon *>(&args);
    if (prevId == 0x16) {
        return false;
    }
    if (pickup->m_weapon.expired() && m_player.get()->m_weapon->GetWeaponType() == 0) {
        return false;
    }
    return true;
}

void PickupWeapon::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsPickupWeapon *pickup =
        dynamic_cast<const InitializeArgsPickupWeapon *>(&args);
    if (pickup->m_weapon.expired()) {
        m_player.get()->m_motion->vf_0x10(0x1B, 0.0f);
        fn_800AA6C0(m_player.get()->m_motion, false);
        m_step = 1;
        m_weapon.reset();
    } else {
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1F)) {
            m_player.get()->m_motion->vf_0x10(0x1A, 0.0f);
            fn_800AA6C0(m_player.get()->m_motion, false);
            m_player.get()->m_weapon->DropWeapon();
            m_step = 0;
        } else {
            m_player.get()->m_motion->vf_0x10(0x1A, 0.0f);
            m_step = 0;
        }
        m_weapon = pickup->m_weapon;
    }
    PlayerMotionEventCallback(m_player.get()) =
        MotionEventCallback(boost::bind(&PickupWeapon::OnMotion, this, _1));
    m_motionEvent = false;
    fn_8040CE60(&m_player.get()->field_0x18);
}

void PickupWeapon::Leave(int nextId) {
    PlayerMotionEventCallback(m_player.get()).Clear();
}

void PickupWeapon::Update(float dt) {
    if (m_motionEvent) {
        switch (m_step) {
        case 0: {
            {
                boost::shared_ptr<CharInfo> weapon = m_weapon.lock();
                if (weapon) {
                    weapon->PushCommand(new UseCommand(m_player.get()->m_self));
                }
            }
            m_step = 4;
            break;
        }
        case 1:
            m_player.get()->m_weapon->DropWeapon();
            m_step = 4;
            break;
        case 2:
            m_player.get()->m_weapon->DropWeapon();
            m_step = 3;
            break;
        }
        m_motionEvent = false;
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (fn_800A9E1C(&m_player)) {
        switch (m_step) {
        case 3:
            m_player.get()->m_motion->vf_0x10(0x1A, 0.0f);
            m_step = 0;
            break;
        case 4:
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
            break;
        }
    }
}

void PickupWeapon::OnMotion(MotionEventArgs &args) {
    switch (args.m_event) {
    case 0x5000:
        m_motionEvent = true;
        break;
    }
}

void PickupWeapon::vf_0x20() {}

Behavior *PickupWeapon::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new PickupWeapon(player);
}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds PickupWeapon by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0C90 = {
    Player::Behavior::PickupWeapon::Create, 0, 0};

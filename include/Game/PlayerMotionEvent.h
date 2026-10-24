#ifndef GAME_PLAYERMOTIONEVENT_H
#define GAME_PLAYERMOTIONEVENT_H

#include <boost/function.hpp>

#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

class CharInfo;

/* A contact of a collision (see Game/BirdBase.cpp). */
struct UnkColliContactNode_800587A0;

/* The game has one shared copy of std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls; an overload declared before the callback's assignment below stands in for
 * it (see Game/CatchStick.cpp). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
}

namespace Player {

/* MotionEventArgs (class name from the RTTI of the behaviors' bound callbacks): what a player's
 * motion passes to its event callback. Only the event number is known (Player::Behavior::ChaosBlast
 * waits for 0x5000; meaning inferred), and the value event 0x5000 carries
 * (Player::Behavior::Grind reads 1 and 3; meaning unknown). */
struct MotionEventArgs {
    /* 0x0 */ u16 m_event;
    /* 0x4 */ s32 *m_value;
};

/* The function a player's motion calls back with its events. */
typedef boost::function1<void, MotionEventArgs &> MotionEventCallback;

/* CollisionEventArgs (class name from the RTTI of Player::Behavior::HomingAttack's bound callback):
 * what a player's collision passes to its event callback: the contact and the character it
 * belongs to (meanings inferred from Player::Behavior::HomingAttack, which asks whether the contact
 * is an attack target, and Player::Behavior::JumpDash, which hangs on walls). */
struct CollisionEventArgs {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ UnkColliContactNode_800587A0 *m_contact;
    /* 0xC */ boost::weak_ptr<CharInfo> m_chara;
};

/* The function a player's collision calls back with its contacts. */
typedef boost::function1<void, CollisionEventArgs &> CollisionEventCallback;

} // namespace Player

/* The motion event callback of a player, at 0x74 of Player::PlayerBase (class name unknown): it
 * holds a MotionEventCallback (a member: the destructor 0x80079D6C that the player's destructor
 * calls destroys it as one). Its assignment from a MotionEventCallback is a function of its own,
 * first emitted by Game/PlayerBehaviorChaosBlast.cpp (0x8007B470) and called by the behaviors that
 * listen to the motion, as is its Clear (0x8007AFE8). */
class UnkMotionEventCallback_8007B470 {
  public:
    UnkMotionEventCallback_8007B470 &operator=(const Player::MotionEventCallback &f) {
        m_function = f;
        return *this;
    }
    /* Unsets the function (name inferred). */
    UnkMotionEventCallback_8007B470 &Clear() {
        m_function.clear();
        return *this;
    }

    /* 0x0 */ Player::MotionEventCallback m_function;
};

/* The callback in a player (a macro: an inline function taking the player would change how the
 * callers reach it). */
#define PlayerMotionEventCallback(player)                                                          \
    (*reinterpret_cast<UnkMotionEventCallback_8007B470 *>(reinterpret_cast<u8 *>(player) + 0x74))

/* The collision event callback of a player, at 0x68 of Player::PlayerBase (class name unknown),
 * which holds a CollisionEventCallback as UnkMotionEventCallback_8007B470 holds its function. Its
 * assignment (0x80086228) and Clear (0x800858B0) are first emitted by
 * Game/PlayerBehaviorHomingAttack.cpp. */
class UnkCollisionEventCallback_80086228 {
  public:
    UnkCollisionEventCallback_80086228 &operator=(const Player::CollisionEventCallback &f) {
        m_function = f;
        return *this;
    }
    /* Unsets the function (name inferred). */
    UnkCollisionEventCallback_80086228 &Clear() {
        m_function.clear();
        return *this;
    }

    /* 0x0 */ Player::CollisionEventCallback m_function;
};

/* The callback in a player (a macro, as PlayerMotionEventCallback). */
#define PlayerCollisionEventCallback(player)                                                       \
    (*reinterpret_cast<UnkCollisionEventCallback_80086228 *>(reinterpret_cast<u8 *>(player) + 0x68))

#endif

#ifndef GAME_PLAYERBEHAVIOR_H
#define GAME_PLAYERBEHAVIOR_H

#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Player {
namespace Behavior {

/* InitializeArgs (class name from RTTI; vtable 0x8051FDF8): the behavior to switch to, passed to
 * fn_800A7F14. id is the behavior's number (meaning inferred from Player::Behavior::FirstMeet,
 * which switches to 0 when its camera path ends). */
class InitializeArgs {
  public:
    InitializeArgs(int id) : m_id(id) {}
    virtual ~InitializeArgs() {}

    /* 0x4 */ s32 m_id;
};
SIZE_ASSERT(InitializeArgs, 0x8);

/* Behavior (class name from RTTI; vtable 0x80520030): the base of a player character's behaviors
 * (Idle, Jump, Grind, ...). The constructor (0x80076C3C) stores the player, the behavior's name and
 * its number; the destructor is at 0x80076B38. Slots 0x0C, 0x10, 0x14 and 0x1C are pure; slot
 * names are inferred from Player::Behavior::FirstMeet. */
class Behavior {
  public:
    Behavior(const boost::shared_ptr<PlayerBase> &player, const char *name, int id);
    virtual ~Behavior();
    virtual bool vf_0x0C() = 0;
    /* Called when the behavior is entered and left (meanings inferred). */
    virtual void Enter() = 0;
    virtual void Leave() = 0;
    virtual void vf_0x18();
    /* The frame update. */
    virtual void Update(float dt) = 0;
    virtual void vf_0x20();

    /* 0x04 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x0C */ const char *m_name;
    /* 0x10 */ s32 m_id;
    /* 0x14 */ u8 field_0x14[0x1C - 0x14]; /* a weak_ptr (to a class not known) */
};
SIZE_ASSERT(Behavior, 0x1C);

/* The record a behavior factory table (0x804FA558) keeps for each behavior number (struct name
 * unknown): the function that creates the behavior for a player, and a second function (empty for
 * FirstMeet; meaning unknown). */
struct UnkBehaviorDesc_804EC024 {
    Behavior *(*create)(const boost::shared_ptr<PlayerBase> &player);
    void (*fn_0x4)(void);
    u32 field_0x8;
};

} // namespace Behavior
} // namespace Player

#endif

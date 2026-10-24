#ifndef GAME_PLAYERBEHAVIOR_H
#define GAME_PLAYERBEHAVIOR_H

#include "Game/Player.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharCommand;
class Target;

namespace Sonicteam {
namespace System {
namespace Geometry {
class Line;
} // namespace Geometry
} // namespace System
} // namespace Sonicteam

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

/* InitializeArgsDamage (class name from RTTI; vtable 0x8051FDEC): switches to the damage
 * behavior (0x1E), knocked along direction. (Moved here from Game/BossSonicExecuter.cpp:
 * Player::Behavior::Damage reads it, Game/PlayerBehaviorDamage.cpp.) */
class InitializeArgsDamage : public InitializeArgs {
  public:
    InitializeArgsDamage(const Vec3f &direction);
    virtual ~InitializeArgsDamage();

    /* 0x8 */ Vec3f m_direction;
};
SIZE_ASSERT(InitializeArgsDamage, 0x14);

/* InitializeArgsJump (class name from RTTI; vtable 0x80520060): switches to the jump behavior (5)
 * along a direction (constructor 0x8007660C, destructor 0x800765B0; the meaning of the vector is
 * inferred from Player::Behavior::DarkSpin, which passes the up vector of its path). */
class InitializeArgsJump : public InitializeArgs {
  public:
    InitializeArgsJump(const Vec3f &direction);
    virtual ~InitializeArgsJump();

    /* 0x8 */ Vec3f m_direction;
};
SIZE_ASSERT(InitializeArgsJump, 0x14);

/* A vector of an InitializeArgs, copy-constructed through the game's out-of-line copy constructor
 * (fn_800091EC); readers take its address as a Vec3f (name invented; as AimPosition in
 * Game/PlayerWeaponBase.h). It has no base class, so that its constructor is inlined. Its inline
 * functions are only defined where needed (PLAYER_BEHAVIOR_ARGS_INLINE): elsewhere they would
 * renumber the units' local labels. */
struct InitializeArgsVector {
#ifdef PLAYER_BEHAVIOR_ARGS_INLINE
    InitializeArgsVector(const Vec3f &from) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &from);
    }
#endif

    float x;
    float y;
    float z;
};

/* InitializeArgsEdgeHang (class name from RTTI): switches to the edge hang (behavior 0xC) at an
 * edge: where to hang, the edge's normal and the character the edge belongs to (names inferred from
 * Player::Behavior::EdgeHang). Player::Behavior::Fall makes one: its vtable (0x8052061C),
 * constructor (0x80080BA0) and destructor (0x80080B30) are out-of-line copies in that unit. */
class InitializeArgsEdgeHang : public InitializeArgs {
  public:
#ifdef PLAYER_BEHAVIOR_ARGS_INLINE
    InitializeArgsEdgeHang(const Vec3f &position, const Vec3f &normal,
                           const boost::weak_ptr<CharInfo> &chara)
        : InitializeArgs(0xC), m_position(position), m_normal(normal), m_chara(chara) {}
    virtual ~InitializeArgsEdgeHang() {}
#else
    virtual ~InitializeArgsEdgeHang();
#endif

    /* 0x08 */ InitializeArgsVector m_position;
    /* 0x14 */ InitializeArgsVector m_normal;
    /* 0x20 */ boost::weak_ptr<CharInfo> m_chara;
};
SIZE_ASSERT(InitializeArgsEdgeHang, 0x28);

/* InitializeArgsGround (class name from RTTI; vtable 0x80520708): switches to the ground behavior
 * (1); m_fullSpeed starts the run at its top speed (meaning inferred from Player::Behavior::Ground).
 * Player::Behavior::GroundAttack makes one: its constructor (0x80084504) and destructor
 * (0x800844A8) are out-of-line copies in that unit. */
class InitializeArgsGround : public InitializeArgs {
  public:
#ifdef PLAYER_BEHAVIOR_ARGS_INLINE
    InitializeArgsGround(bool fullSpeed) : InitializeArgs(1), m_fullSpeed(fullSpeed) {}
    virtual ~InitializeArgsGround() {}
#else
    InitializeArgsGround(bool fullSpeed);
    virtual ~InitializeArgsGround();
#endif

    /* 0x8 */ u8 m_fullSpeed;
};
SIZE_ASSERT(InitializeArgsGround, 0xC);

/* InitializeArgsHomingAttack (class name from RTTI; vtable 0x80520CBC, destructor 0x8009C650):
 * switches to the homing attack (behavior 8) on a target (name inferred from
 * Player::Behavior::HomingAttack, which locks on to it). */
class InitializeArgsHomingAttack : public InitializeArgs {
  public:
    virtual ~InitializeArgsHomingAttack();

    /* 0x08 */ boost::weak_ptr<Target> m_target;
};
SIZE_ASSERT(InitializeArgsHomingAttack, 0x10);

/* InitializeArgsHomingJump (class name from RTTI): switches to the jump that follows a homing
 * attack (behavior 9) along a direction; m_hit says the attack met its target, which makes the
 * jump higher (names and meanings inferred from Player::Behavior::HomingJump).
 * Player::Behavior::HomingAttack makes one: its vtable (0x80520778), constructor (0x800857B0) and
 * destructor (0x80085754) are out-of-line copies in that unit. */
class InitializeArgsHomingJump : public InitializeArgs {
  public:
#ifdef PLAYER_BEHAVIOR_ARGS_INLINE
    InitializeArgsHomingJump(const Vec3f &direction, bool hit)
        : InitializeArgs(9), m_direction(direction), m_hit(hit) {}
    virtual ~InitializeArgsHomingJump() {}
#else
    virtual ~InitializeArgsHomingJump();
#endif

    /* 0x08 */ InitializeArgsVector m_direction;
    /* 0x14 */ u8 m_hit;
};
SIZE_ASSERT(InitializeArgsHomingJump, 0x18);

/* InitializeArgsWallHang (class name from RTTI): switches to hanging on a wall (behavior 0xD): the
 * wall's normal and the character the wall belongs to (names inferred from
 * Player::Behavior::JumpDash, which takes them from the contact it meets).
 * Player::Behavior::JumpDash makes one: its vtable (0x80520880), constructor (0x80088240) and
 * destructor (0x800881D0) are out-of-line copies in that unit. */
class InitializeArgsWallHang : public InitializeArgs {
  public:
#ifdef PLAYER_BEHAVIOR_ARGS_INLINE
    InitializeArgsWallHang(const Vec3f &normal, const boost::weak_ptr<CharInfo> &wall)
        : InitializeArgs(0xD), m_normal(normal), m_wall(wall) {}
    virtual ~InitializeArgsWallHang() {}
#else
    InitializeArgsWallHang(const Vec3f &normal, const boost::weak_ptr<CharInfo> &wall);
    virtual ~InitializeArgsWallHang();
#endif

    /* 0x08 */ InitializeArgsVector m_normal;
    /* 0x14 */ boost::weak_ptr<CharInfo> m_wall;
};
SIZE_ASSERT(InitializeArgsWallHang, 0x1C);

/* InitializeArgsWallJump (class name from RTTI): switches to the jump off a wall (behavior 0x10):
 * the wall's normal and the direction to jump in (names inferred from Player::Behavior::WallHang,
 * which makes one: its vtable (0x80520BC4), constructor (0x80092068) and destructor (0x8009200C)
 * are out-of-line copies in that unit). */
class InitializeArgsWallJump : public InitializeArgs {
  public:
#ifdef PLAYER_BEHAVIOR_ARGS_INLINE
    InitializeArgsWallJump(const Vec3f &normal, const Vec3f &direction)
        : InitializeArgs(0x10), m_normal(normal), m_direction(direction) {}
    virtual ~InitializeArgsWallJump() {}
#else
    InitializeArgsWallJump(const Vec3f &normal, const Vec3f &direction);
    virtual ~InitializeArgsWallJump();
#endif

    /* 0x08 */ InitializeArgsVector m_normal;
    /* 0x14 */ InitializeArgsVector m_direction;
};
SIZE_ASSERT(InitializeArgsWallJump, 0x20);

/* InitializeArgsLaunch (class name from RTTI; vtable 0x80520DAC, destructor 0x8009AE74): switches
 * to being launched (behavior 0x17); flags bit 0 curls the player up, bit 1 sets the player's state
 * bit 0x40 (meanings inferred from Player::Behavior::Launch). */
class InitializeArgsLaunch : public InitializeArgs {
  public:
    virtual ~InitializeArgsLaunch();

    /* 0x8 */ u32 m_flags;
};
SIZE_ASSERT(InitializeArgsLaunch, 0xC);

/* InitializeArgsLineHang (class name from RTTI; vtable 0x80520D04, destructor 0x8009BFBC):
 * switches to hanging on a line (behavior 0xE): the character the line belongs to and the line
 * (names inferred from Player::Behavior::LineHang). */
class InitializeArgsLineHang : public InitializeArgs {
  public:
    virtual ~InitializeArgsLineHang();

    /* 0x08 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x10 */ boost::weak_ptr<Sonicteam::System::Geometry::Line> m_line;
};
SIZE_ASSERT(InitializeArgsLineHang, 0x18);

/* InitializeArgsPickupObject (class name from RTTI; vtable 0x80520D64, destructor 0x8009B29C):
 * switches to picking up an object (behavior 0x15) through the target it publishes (name inferred
 * from Player::Behavior::PickupObject, which picks up the target's owner). */
class InitializeArgsPickupObject : public InitializeArgs {
  public:
    virtual ~InitializeArgsPickupObject();

    /* 0x08 */ boost::weak_ptr<Target> m_target;
};
SIZE_ASSERT(InitializeArgsPickupObject, 0x10);

/* InitializeArgsPickupWeapon (class name from RTTI; vtable 0x80520D34, destructor 0x8009B908):
 * switches to picking up a weapon (behavior 0x16); without one the player drops the weapon it holds
 * (names and meanings inferred from Player::Behavior::PickupWeapon). */
class InitializeArgsPickupWeapon : public InitializeArgs {
  public:
    virtual ~InitializeArgsPickupWeapon();

    /* 0x08 */ boost::weak_ptr<CharInfo> m_weapon;
};
SIZE_ASSERT(InitializeArgsPickupWeapon, 0x10);

/* InitializeArgsSomethingHang (class name from RTTI; vtable 0x80520D1C, destructor 0x8009BE28):
 * switches to hanging on a character (behavior 0xF): the character, where the player's hands and
 * rotation are taken from, and flags (bit 0: set the player's flag 1, bit 1: its flag 7, bit 2:
 * StartActionCommand does not let go; names and meanings inferred from
 * Player::Behavior::SomethingHang, which takes the same from TouchHangCommand). */
class InitializeArgsSomethingHang : public InitializeArgs {
  public:
    virtual ~InitializeArgsSomethingHang();

    /* 0x08 */ boost::weak_ptr<CharInfo> m_target;
    /* 0x10 */ const Vec3f *m_position;
    /* 0x14 */ const Vec3f *m_rotation;
    /* 0x18 */ u32 m_flags;
};
SIZE_ASSERT(InitializeArgsSomethingHang, 0x1C);

/* InitializeArgsThrow (class name from RTTI; vtable 0x80520D7C, destructor 0x8009B13C): switches
 * to throwing the object the player carries (behavior 0xA), handing the player m_object first when
 * it is set (names inferred from Player::Behavior::Throw). */
class InitializeArgsThrow : public InitializeArgs {
  public:
    virtual ~InitializeArgsThrow();

    /* 0x08 */ boost::weak_ptr<PickupCharInfo> m_object;
};
SIZE_ASSERT(InitializeArgsThrow, 0x10);

/* InitializeArgsTurnOver (class name from RTTI; vtable 0x80520D94, destructor 0x8009AFDC):
 * switches to turning over an object (behavior 0xB) through the target it publishes (name inferred
 * from Player::Behavior::TurnOver, which works on the target's owner). */
class InitializeArgsTurnOver : public InitializeArgs {
  public:
    virtual ~InitializeArgsTurnOver();

    /* 0x08 */ boost::weak_ptr<Target> m_target;
};
SIZE_ASSERT(InitializeArgsTurnOver, 0x10);

/* InitializeArgsDriveVehicle (class name from RTTI; vtable 0x80520DC4, destructor 0x8009AB10):
 * switches to riding a vehicle (behavior 0x1B; name inferred from Player::Behavior::DriveVehicle). */
class InitializeArgsDriveVehicle : public InitializeArgs {
  public:
    virtual ~InitializeArgsDriveVehicle();

    /* 0x08 */ boost::weak_ptr<Vehicle::TVehicleBase> m_vehicle;
};
SIZE_ASSERT(InitializeArgsDriveVehicle, 0x10);

/* InitializeArgsExternalControl (class name from RTTI; vtable 0x80520DDC, destructor 0x8009A508):
 * hands the player to an outside controller (behavior 0x20): the controller, where the player's
 * position and rotation are taken from, and flags (bit 0: set the player's flag 1, bit 1: ride the
 * controller, bit 2: follow the position, bit 3: follow the rotation; names and meanings inferred
 * from Player::Behavior::ExternalControl). */
class InitializeArgsExternalControl : public InitializeArgs {
  public:
    virtual ~InitializeArgsExternalControl();

    /* 0x08 */ boost::weak_ptr<CharInfo> m_controller;
    /* 0x10 */ const Vec3f *m_position;
    /* 0x14 */ const Vec3f *m_rotation;
    /* 0x18 */ u32 m_flags;
};
SIZE_ASSERT(InitializeArgsExternalControl, 0x1C);

/* Behavior (class name from RTTI; vtable 0x80520030): the base of a player character's behaviors
 * (Idle, Jump, Grind, ...). The constructor (0x80076C3C) stores the player, the behavior's name and
 * its number; the destructor is at 0x80076B38. Slots 0x0C, 0x10, 0x14 and 0x1C are pure; slot
 * names are inferred from Player::Behavior::FirstMeet. The switch (fn_800A7F14) asks the new
 * behavior's slot 0x0C, then calls the old one's Leave with the new number and the new one's Enter
 * with the old number (0x25 when there was none) and the switch's arguments. */
class Behavior {
  public:
    Behavior(const boost::shared_ptr<PlayerBase> &player, const char *name, int id);
    virtual ~Behavior();
#ifdef PLAYER_BEHAVIOR_SLOT_ARGS
    /* The slots with the arguments the behavior executer passes (the behaviors of
     * Game/PlayerBehavior*.cpp read them): whether the behavior may start after behavior prevId
     * (Player::Behavior::AirAttack refuses itself; meaning inferred), and entering it (from
     * behavior prevId, with its InitializeArgs) and leaving it (for behavior nextId). The other
     * units keep the declarations without arguments, which name the symbols of
     * Player::Behavior::FirstMeet and Player::Behavior::Super. */
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args) = 0;
    virtual void Enter(int prevId, const InitializeArgs &args) = 0;
    virtual void Leave(int nextId) = 0;
#else
    virtual bool vf_0x0C() = 0;
    /* Called when the behavior is entered and left (meanings inferred). */
    virtual void Enter() = 0;
    virtual void Leave() = 0;
#endif
    /* Offered each command the player receives; a behavior marks the ones it takes as handled
     * (CharCommand::Flags() bit 1; argument from Player::Behavior::Super and the behaviors whose
     * slot reads the command type at 0x4, e.g. 0x80083EC0). */
    virtual void vf_0x18(CharCommand *command);
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

/* An entry of a behavior factory's table (struct name unknown; 0x804FA558 and 0x804FA888, in
 * Game/SonicTypeBehaviorFactory.cpp and Game/FlyTypeBehaviorFactory.cpp): a behavior number and its
 * record. The factory's constructor calls each record's fn_0x4 with the player and its destructor
 * the function at field_0x8; an entry with id 0x25 and no record ends the table. */
struct UnkBehaviorEntry_804FA558 {
    s32 id;
    const UnkBehaviorDesc_804EC024 *desc;
};

} // namespace Behavior
} // namespace Player

#endif

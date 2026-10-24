#ifndef GAME_CHARCOMMAND_H
#define GAME_CHARCOMMAND_H

#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

#ifdef CHARCOMMAND_SET_LOOP_PATH
#include "Game/UnkCircuitTarget_800ABCE8.h"
#endif

class CharInfo;
/* Target (class name from RTTI): units that use it declare its members. */
class Target;
class CharCommandUnit;

namespace Weapon {
class WeaponVacuumBase;
} // namespace Weapon

/* CharCommand (class name from RTTI): a command queued on a character through CharInfo's
 * PushCommand. The commands below (class names from RTTI) all allocate through CharCommand's
 * operator new, which uses heap 2 (inferred from its body). The integer arguments' meanings are
 * unknown; the sizes come from the operator new calls.
 *
 * The units that define CharCommand's and the commands' functions (Game/CharCommandManagerTask.cpp,
 * Game/CharCommands.cpp) define CHARCOMMAND_IMPLEMENTATION (after including <string>): they then
 * see the destructor inline, as the game has it (the copy linked is 0x8006E3A4; Reject is then the
 * first out-of-line virtual function, which puts the vtable with it), slot 0x10's real signature,
 * and the named members. Elsewhere the class keeps the declarations below, so that no unit
 * instantiates more templates than before. */
class CharCommand {
  public:
    /* Stores the command type at 0x4 and arg at 0x8 (0x8006E454; arg is the priority level, 0..6,
     * whose queue Game/CharCommandManagerTask.cpp files the command in). */
    CharCommand(u16 type, int arg);
    static void *operator new(size_t size);
    /* 0x801D9C64 (takes the size; behaviour inferred). */
    static void operator delete(void *p, size_t size);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual ~CharCommand() {}
#else
    virtual ~CharCommand();
#endif
    /* The default handling of a command its receiver does not take (slot 0x0C; empty in CharCommand,
     * 0x801D9CFC; name inferred from EggLastMech, which passes itself as receiver). */
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
#ifdef CHARCOMMAND_IMPLEMENTATION
    /* The command described for debug output: its name, then the values of the commands that
     * override it (slot 0x10; name unknown). */
    virtual std::string vf_0x10() const;
#else
    virtual void vf_0x10() const;
#endif
    /* The command's name, for debug messages (slot 0x14; name inferred from EggLastMech). */
    virtual const char *GetName();

#ifdef CHARCOMMAND_IMPLEMENTATION
    u8 &Flags() {
        return m_flags;
    }
    u16 TypeId() const {
        return m_type;
    }

    /* 0x04 */ u16 m_type;
    /* 0x06 */ u8 field_0x6; /* passed to fn_800CB560 when no command unit takes the command */
    /* 0x07 */ u8 m_flags; /* 0x1 handled, 0x4 for a command unit (meanings inferred) */
    /* 0x08 */ s32 m_priority;
    /* 0x0C */ boost::weak_ptr<CharCommandUnit> m_receiver;
#else
    /* The command's flag byte (at 0x7): bit 1 marks it as handled (names inferred from
     * EggLastMech). */
    u8 &Flags() {
        return field_0x4[0x7 - 0x4];
    }

    /* The command's type, which the base constructor (0x8006E454) stores at 0x4: 0xB RideCommand,
     * 0xC LeaveCommand, 0xD UseCommand, ... (name inferred). */
    u16 TypeId() const {
        return *reinterpret_cast<const u16 *>(field_0x4);
    }

    /* 0x04 */ u8 field_0x4[0x10];
#endif
};
SIZE_ASSERT(CharCommand, 0x14);

/* The commands of Game/CharCommands.cpp (class names from RTTI), in the order of their command
 * types (1 to 0x1D). Each has its name (GetName) and, where noted, a description of its values for
 * debug output (slot 0x10) and a Reject that applies the command to a character no command unit
 * takes it for (marking it handled, flag 0x2). The constructors marked inline are inline in the
 * game (each user emits a copy); the others are in Game/CharCommands.cpp. Argument meanings are
 * unknown unless noted; arg1 or priority is the queue level CharCommand stores (see
 * Game/CharCommandManagerTask.cpp). This is the one declaration of each command: the debug
 * descriptions (slot 0x10) are declared for Game/CharCommands.cpp alone
 * (CHARCOMMAND_IMPLEMENTATION), as CharCommand's slot 0x10 is. */

#ifdef CHARCOMMAND_IMPLEMENTATION
/* A vector member copied by the game's out-of-line copy (fn_800091EC): PickupCommand's
 * constructor copies its vectors in its initializer list, around the weak_ptr's. Only
 * Game/CharCommands.cpp sees the members with this type; it has Vec3f's layout (type name
 * unknown). */
struct CommandVec3f {
    CommandVec3f(const Vec3f &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &v);
    }

    float x;
    float y;
    float z;
};
#endif

/* Type 1: steers the character, or a vehicle, along m_direction, as an input would (sent by
 * Game/BkSoldier.cpp and the Boss Sonic controls; taken by Game/Coaster.cpp and
 * Game/UpDownReel.cpp). */
class AccelerateInputCommand : public CharCommand {
  public:
    AccelerateInputCommand(const Vec3f &direction, int arg1);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ Vec3f m_direction;
};
SIZE_ASSERT(AccelerateInputCommand, 0x20);

/* Type 2: moves the character to m_position (constructor inline in the game, 0x8009E51C). */
class SetPositionCommand : public CharCommand {
  public:
    SetPositionCommand(const Vec3f &position, int arg1);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ Vec3f m_position;
};
SIZE_ASSERT(SetPositionCommand, 0x20);

/* Type 3: turns the character to m_angles, given in rotation order field_0x20 (constructor inline
 * in the game, 0x800B81F8). */
class SetAngleCommand : public CharCommand {
  public:
    SetAngleCommand(const Vec3f &angles, int arg1, int arg2);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ Vec3f m_angles;
    /* 0x20 */ s32 field_0x20; /* the rotation order, compared with CharInfo::field_0x30 */
};
SIZE_ASSERT(SetAngleCommand, 0x24);

/* Type 4: clears the character's velocity (constructor inline in the game, 0x8009E934). */
class ClrVelocityCommand : public CharCommand {
  public:
    ClrVelocityCommand(int arg0);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
    virtual const char *GetName();
};
SIZE_ASSERT(ClrVelocityCommand, 0x14);

/* Type 5: adds m_velocity to the character's velocity (CharInfo::field_0x18; constructor inline in
 * the game, 0x800DE6B4). */
class SetVelocityCommand : public CharCommand {
  public:
    SetVelocityCommand(const Vec3f &velocity, int arg1);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ Vec3f m_velocity;
};
SIZE_ASSERT(SetVelocityCommand, 0x20);

/* Type 6: pushes the character by m_velocity (added to CharInfo::field_0x24; constructor inline in
 * the game, 0x8006E354, vtable 0x8053D470). */
class AccelerateCommand : public CharCommand {
  public:
    AccelerateCommand(const Vec3f &velocity, int arg1);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ Vec3f m_velocity;
};
SIZE_ASSERT(AccelerateCommand, 0x20);

/* DamageCommand (vtable at 0x8053D44C): the command of type 7, which hurts a character
 * (constructor 0x801DB804; the direction and amount are inferred from Game/WeaponVacuumPod.cpp,
 * arg1 unknown). */
class DamageCommand : public CharCommand {
  public:
    DamageCommand(const Vec3f &direction, int arg1, float amount);
    /* The same with a zero direction (constructor 0x801DB864; used by WEIGHT). */
    DamageCommand(int arg1, float amount);
    virtual const char *GetName();

    /* 0x14 */ float m_amount;
    /* 0x18 */ Vec3f m_direction;
};
SIZE_ASSERT(DamageCommand, 0x24);

/* Type 8: tells a character that something it owns has died (meaning inferred from
 * Game/BalloonGenerator.cpp; constructor inline in the game, 0x8009EE8C). */
class DeadCommand : public CharCommand {
  public:
    DeadCommand(int arg0);
    virtual const char *GetName();
};
SIZE_ASSERT(DeadCommand, 0x14);

/* Type 9 (vtable at 0x8053D404): switches path m_index of a path-following object to its
 * alternative (sent by Game/RailSwitch.cpp; taken by Game/BKTankController.cpp and
 * Game/EggBalloonController.cpp). */
class SwitchCommand : public CharCommand {
  public:
    SwitchCommand(int index) : CharCommand(CommandType(9), 4), m_index(index) {}
    virtual const char *GetName();
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif

    /* The command type as CharCommand stores it. Calling this inline function keeps the
     * constructor out of line, as the game has it (each user emits a copy; as
     * Game/GadgetCommand.h does). */
    static u16 CommandType(int type) {
        return type;
    }

    /* 0x14 */ s32 m_index;
};
SIZE_ASSERT(SwitchCommand, 0x18);

/* Type 0xA: a jump off a wall, with the character's position (meanings inferred). */
class WallJumpCommand : public CharCommand {
  public:
    WallJumpCommand(const boost::weak_ptr<CharInfo> &character, const Vec3f &position);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x1C */ Vec3f m_position;
};
SIZE_ASSERT(WallJumpCommand, 0x28);

/* Types 0xB and 0xC: getting on and off, at m_position. m_partner is the other side: the vehicle
 * a character gets on (Game/VehicleCharaColliMove.cpp sends them to the character), or the
 * character that gets on a gadget (taken by Game/ElecBar.cpp, Game/Helicopter.cpp and
 * Game/UpDownReel.cpp). Meanings inferred. */
class RideCommand : public CharCommand {
  public:
    RideCommand(const boost::weak_ptr<CharInfo> &partner, const Vec3f &position);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_partner;
    /* 0x1C */ Vec3f m_position;
};
SIZE_ASSERT(RideCommand, 0x28);

class LeaveCommand : public CharCommand {
  public:
    LeaveCommand(const boost::weak_ptr<CharInfo> &partner, const Vec3f &position);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_partner;
    /* 0x1C */ Vec3f m_position;
};
SIZE_ASSERT(LeaveCommand, 0x28);

/* UseCommand: the command of type 0xD, which a character sends to the gadget it uses. Only the
 * user is known (name inferred). */
class UseCommand : public CharCommand {
  public:
    /* The command without a target (0x801DB480). */
    UseCommand(const boost::weak_ptr<CharInfo> &user);
    UseCommand(const boost::weak_ptr<CharInfo> &user, const boost::weak_ptr<Target> &target);
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_user;
    /* 0x1C */ boost::weak_ptr<Target> m_target; /* the target used (from RegularRing) */
};
SIZE_ASSERT(UseCommand, 0x24);

/* ReadyCommand (vtable at 0x8053D350): the command of type 0xE, which a picked-up object sends its
 * carrier (constructor 0x801DB3C0; meaning of the character inferred). */
class ReadyCommand : public CharCommand {
  public:
    ReadyCommand(const boost::weak_ptr<CharInfo> &character);
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_character;
};
SIZE_ASSERT(ReadyCommand, 0x1C);

/* Type 0xF: cancels what the character does with m_character (meaning inferred). */
class CancelCommand : public CharCommand {
  public:
    CancelCommand(const boost::weak_ptr<CharInfo> &character);
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_character;
};
SIZE_ASSERT(CancelCommand, 0x1C);

/* RecoverCommand (vtable at 0x8053D308): the command of type 0x10 that heals a character
 * (constructor 0x801DB2F8; meanings inferred from the HEALING_UNIT gadget). */
class RecoverCommand : public CharCommand {
  public:
    RecoverCommand(const boost::weak_ptr<CharInfo> &source, float amount);
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_source;
    /* 0x1C */ float m_amount;
};
SIZE_ASSERT(RecoverCommand, 0x20);

/* PickupCommand (vtable at 0x8053D2E4): the command of type 0x11, which a character sends to the
 * object it picks up. Only the character is known (name inferred). */
class PickupCommand : public CharCommand {
  public:
    PickupCommand(const boost::weak_ptr<CharInfo> &character, const Vec3f &arg1, float arg2,
                  const Vec3f &arg3);
    virtual const char *GetName();

#ifdef CHARCOMMAND_IMPLEMENTATION
    /* 0x14 */ CommandVec3f field_0x14;
    /* 0x20 */ float field_0x20;
    /* 0x24 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x2C */ CommandVec3f field_0x2C;
#else
    /* 0x14 */ Vec3f field_0x14;
    /* 0x20 */ float field_0x20;
    /* 0x24 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x2C */ Vec3f field_0x2C;
#endif
};
SIZE_ASSERT(PickupCommand, 0x38);

/* CastCommand and ThrowCommand (vtables at 0x8053D2C0 and 0x8053D29C): the commands of types 0x12
 * and 0x13, which a carrier sends to the object it drops or throws: the carrier, then the
 * direction (names inferred). */
class CastCommand : public CharCommand {
  public:
    CastCommand(const boost::weak_ptr<CharInfo> &character, const Vec3f &direction);
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x1C */ Vec3f m_direction;
};
SIZE_ASSERT(CastCommand, 0x28);

class ThrowCommand : public CharCommand {
  public:
    ThrowCommand(const boost::weak_ptr<CharInfo> &character, const Vec3f &direction);
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x1C */ Vec3f m_direction;
};
SIZE_ASSERT(ThrowCommand, 0x28);

/* VacuumCommand (vtable at 0x8053D278): the command of type 0x14, which pulls a target in. Only the
 * target is known (name inferred). fn_801DADFC gives the character the pull is for: the weapon's
 * owner, or field_0x24. */
class VacuumCommand : public CharCommand {
  public:
    /* Pulls target in with weapon (0x801DB0D8; from Game/WeaponVacuumPod.cpp, arg2 unknown: its
     * callers pass 0 or 1). */
    VacuumCommand(const boost::weak_ptr<Target> &target,
                  const boost::weak_ptr<Weapon::WeaponVacuumBase> &weapon, int arg2);
    /* Pulls target in for owner, with no weapon (0x801DB004; from
     * Game/PlayerBarrierMagnetEffect.cpp, where owner is the player). */
    VacuumCommand(const boost::weak_ptr<Target> &target, const boost::weak_ptr<CharInfo> &owner);
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<Target> m_target;
    /* 0x1C */ boost::weak_ptr<Weapon::WeaponVacuumBase> m_weapon;
    /* 0x24 */ boost::weak_ptr<CharInfo> field_0x24;
    /* 0x2C */ s32 field_0x2C;
};
SIZE_ASSERT(VacuumCommand, 0x30);

/* Type 0x15: an action for the vehicle a character controls (m_action: 1 and 4 get off, 2 and 3
 * set and clear a flag, 6 and 7 start and stop firing, going by Game/BirdBase.cpp, Game/GunLift.cpp
 * and Game/VehicleAirBolt.cpp; meanings inferred). */
class ControlVehicleCommand : public CharCommand {
  public:
    ControlVehicleCommand(int action, int arg1);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ s32 m_action;
};
SIZE_ASSERT(ControlVehicleCommand, 0x18);

/* AnnounceAttackCommand (vtable at 0x8053D230): the command of type 0x16, which tells a character
 * it is being attacked (constructor 0x801DACE4; the attacker and the meaning of the arguments are
 * inferred from Player::Shadow's gun control, which passes 4, 4). */
class AnnounceAttackCommand : public CharCommand {
  public:
    AnnounceAttackCommand(const boost::weak_ptr<CharInfo> &attacker, int arg1, int arg2);
    virtual ~AnnounceAttackCommand();
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_attacker;
    /* 0x1C */ s32 field_0x1C;
};
SIZE_ASSERT(AnnounceAttackCommand, 0x20);

/* Type 0x17: fades the character (m_transparent) at the rate m_alpha (meanings inferred from the
 * debug description and from the containers, Game/Door.cpp and Game/FootingMovable.cpp, which
 * fade their models with it). */
class SetTransparentCommand : public CharCommand {
  public:
    SetTransparentCommand(bool transparent, float rate, int target);
#ifdef CHARCOMMAND_IMPLEMENTATION
    virtual std::string vf_0x10() const;
#endif
    virtual const char *GetName();

    /* 0x14 */ u8 m_transparent;
    /* 0x18 */ float m_alpha;
    /* 0x1C */ u32 field_0x1C;
};
SIZE_ASSERT(SetTransparentCommand, 0x20);

/* Type 0x18: a light setting (meaning unknown). */
class SetLightCommand : public CharCommand {
  public:
    SetLightCommand(int light);
    virtual const char *GetName();

    /* 0x14 */ s32 m_light;
};
SIZE_ASSERT(SetLightCommand, 0x18);

/* GenerateCommand (vtable at 0x8053D1C4): the command of type 0x19 that a set generator
 * (Game/SetGenerator.cpp) sends to the object it releases, with the object's placed position and
 * rotation (constructor 0x801DA9E4; names inferred). */
class GenerateCommand : public CharCommand {
  public:
    /* rotation is not const: the constructor reloads each angle after the previous store. */
    GenerateCommand(const Vec3f &position, Vec3f &rotation);
    virtual const char *GetName();

    /* 0x14 */ Vec3f m_position;
    /* 0x20 */ Vec3f m_rotation;
};
SIZE_ASSERT(GenerateCommand, 0x2C);

/* Types 0x1A and 0x1B: hold a character at, and let it go from, the object whose character info
 * they keep (behaviour inferred from the cages, Game/Cage.cpp and Game/CaptureCage.cpp). */
class BindCommand : public CharCommand {
  public:
    BindCommand(const boost::weak_ptr<CharInfo> &binder);
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_binder;
};
SIZE_ASSERT(BindCommand, 0x1C);

class ReleaseCommand : public CharCommand {
  public:
    ReleaseCommand(const boost::weak_ptr<CharInfo> &binder);
    virtual const char *GetName();

    /* 0x14 */ boost::weak_ptr<CharInfo> m_binder;
};
SIZE_ASSERT(ReleaseCommand, 0x1C);

/* Type 0x1C: five values for the score, passed on unchanged by Game/TurnOver.cpp (meanings
 * unknown; sent by Game/ScoreData.cpp). */
class ScoreCommand : public CharCommand {
  public:
    ScoreCommand(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
    virtual const char *GetName();

    /* 0x14 */ s32 m_values[5];
};
SIZE_ASSERT(ScoreCommand, 0x28);

/* Type 0x1D: sent by springs (destructor and constructor out of line). */
class SpringCommand : public CharCommand {
  public:
    SpringCommand();
    virtual ~SpringCommand();
    virtual const char *GetName();
};
SIZE_ASSERT(SpringCommand, 0x14);

namespace Player {

/* PlayerCommand (class name from RTTI): the base of the player-only commands. */
class PlayerCommand : public CharCommand {
  public:
    /* id identifies the command (inferred from the constants its subclasses pass); arg1's meaning
     * is unknown. */
    PlayerCommand(int id, int arg1);
    virtual ~PlayerCommand();
    /* The command's name, looked up by its id (0x8009CCE4). */
    virtual const char *GetName();
};

class LaunchCommand : public PlayerCommand {
  public:
    LaunchCommand(int arg0, int arg1);

    /* 0x14 */ u32 field_0x14;
};

class IgnoreInputCommand : public PlayerCommand {
  public:
    IgnoreInputCommand(int arg0, float time);
    /* 0x800774C4: stores the two integers at 0x18 and 0x1C (meanings unknown). */
    IgnoreInputCommand(int arg0, int arg1);

    /* 0x14 */ u32 field_0x14;
    /* 0x18 */ float m_time;
    /* 0x1C */ u32 field_0x1C;
};

class ActingAccelerateCommand : public PlayerCommand {
  public:
    ActingAccelerateCommand(const Vec3f &velocity, float time);

    /* 0x14 */ Vec3f m_velocity;
    /* 0x20 */ float m_time;
};

/* StartJumpCommand (class name from RTTI; vtable at 0x805214F4): constructor 0x8009C994, which
 * zeroes the vector at 0x14 (meanings of arg0 and the vector unknown; Vehicle::GunLift sends it
 * with 4 to the rider it lets off). */
class StartJumpCommand : public PlayerCommand {
  public:
    StartJumpCommand(int arg0);

    /* 0x14 */ Vec3f field_0x14;
};

/* SetGravityCommand (class name from RTTI; vtable 0x80520F48): turns a character towards a new
 * gravity direction. Constructor 0x8009A368 (command id 0x12D) stores the direction normalized
 * (arg1's meaning unknown; the stage's gravity, Game/GravityDirection.cpp, passes 4). */
class SetGravityCommand : public PlayerCommand {
  public:
    SetGravityCommand(const Vec3f &direction, int arg1);

    /* 0x14 */ Vec3f m_direction;
};

#ifdef CHARCOMMAND_SET_LOOP_PATH
/* SetLoopPathCommand (class name from RTTI; vtable 0x805213EC): the command of type 0x10C, which
 * puts a running player on a loop's path (Player::Behavior::Ground takes it). Only the path is
 * known (name inferred). Declared where CHARCOMMAND_SET_LOOP_PATH is defined
 * (Game/PlayerBehaviorGround.cpp), as the path's type needs Game/UnkCircuitTarget_800ABCE8.h. */
class SetLoopPathCommand : public PlayerCommand {
  public:
    /* 0x14 */ UnkCircuitTarget_800ABCE8 m_path;
};
#endif

} // namespace Player

SIZE_ASSERT(Player::StartJumpCommand, 0x20);
SIZE_ASSERT(Player::SetGravityCommand, 0x20);
SIZE_ASSERT(Player::LaunchCommand, 0x18);
SIZE_ASSERT(Player::IgnoreInputCommand, 0x20);
SIZE_ASSERT(Player::ActingAccelerateCommand, 0x24);

#endif

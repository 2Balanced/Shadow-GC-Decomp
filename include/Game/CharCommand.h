#ifndef GAME_CHARCOMMAND_H
#define GAME_CHARCOMMAND_H

#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

#if defined(CHARCOMMAND_SET_LOOP_PATH) || defined(CHARCOMMAND_PLAYER_COMMANDS) || defined(CHARCOMMAND_PLAYER_CONTROLLER) || \
    defined(CHARCOMMAND_PLAYER_USER_INPUT)
#include "Game/UnkCircuitTarget_800ABCE8.h"
#endif

class CharInfo;
/* Target (class name from RTTI): units that use it declare its members. */
class Target;
class CharCommandUnit;

namespace Weapon {
class WeaponVacuumBase;
} // namespace Weapon

#if defined(CHARCOMMAND_PLAYER_COMMANDS) || defined(CHARCOMMAND_PLAYER_USER_INPUT)
class PickupCharInfo;
namespace Vehicle {
class TVehicleBase;
} // namespace Vehicle
#endif

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
#if defined(CHARCOMMAND_INLINE_BASE) && defined(CHARCOMMAND_IMPLEMENTATION)
    /* Inline in the game, where CHARCOMMAND_INLINE_BASE is defined (Game/NpcControlSonicType.cpp,
     * the unit that has the copy linked). */
    CharCommand(u16 type, int arg) : m_type(type), m_priority(arg) {
        m_flags = 0;
        field_0x6 = 0;
    }
#else
    CharCommand(u16 type, int arg);
#endif
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
#ifdef CHARCOMMAND_INLINE_CONSTRUCTORS
    /* The inline constructor, where CHARCOMMAND_INLINE_CONSTRUCTORS is defined
     * (Game/PlayerControlBukuu.cpp, which has the copy the game links). The inline type accessor
     * keeps it out of line, as with Player::CancelBehaviorCommand (Game/GadgetCharCommands.h). */
    static int Id() {
        return 2;
    }

    SetPositionCommand(const Vec3f &position, int arg1) : CharCommand(Id(), arg1) {
        fn_800091EC(&m_position, &position);
    }
#else
    SetPositionCommand(const Vec3f &position, int arg1);
#endif
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
#ifdef CHARCOMMAND_SET_ANGLE_INLINE
    /* The inline constructor, where CHARCOMMAND_SET_ANGLE_INLINE is defined
     * (Game/PlayerHelpers.cpp, which has the copy the game links). The inline type accessor
     * keeps it out of line, as with SetPositionCommand. */
    static int Id() {
        return 3;
    }

    SetAngleCommand(const Vec3f &angles, int arg1, int arg2)
        : CharCommand(Id(), arg2), m_angles(angles), field_0x20(arg1) {}
#else
    SetAngleCommand(const Vec3f &angles, int arg1, int arg2);
#endif
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
#ifdef CHARCOMMAND_INLINE_CONSTRUCTORS
    /* As SetPositionCommand's (Game/PlayerControlBukuu.cpp). */
    static int Id() {
        return 4;
    }

    ClrVelocityCommand(int arg0) : CharCommand(Id(), arg0) {}
#else
    ClrVelocityCommand(int arg0);
#endif
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
#ifdef CHARCOMMAND_INLINE_BASE
    /* Inline, where CHARCOMMAND_INLINE_BASE is defined (Game/NpcControlSonicType.cpp has the copy
     * linked). */
    AccelerateCommand(const Vec3f &velocity, int arg1) : CharCommand(6, arg1) {
        fn_800091EC(&m_velocity, &velocity);
    }
#else
    AccelerateCommand(const Vec3f &velocity, int arg1);
#endif
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
#ifdef CHARCOMMAND_PLAYER_CONTROLLER
    /* Inline, as Game/PlayerController.cpp (the first unit to emit it) sees it; the inline type
     * accessor keeps it out of line. */
    DeadCommand(int arg0) : CharCommand(CommandType(8), arg0) {}
    static u16 CommandType(int type) {
        return type;
    }
#else
    DeadCommand(int arg0);
#endif
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

#ifdef CHARCOMMAND_PLAYER_COMMANDS
class PlayerBase;
#endif

/* PlayerCommand (class name from RTTI): the base of the player-only commands. */
class PlayerCommand : public CharCommand {
  public:
    /* id identifies the command (inferred from the constants its subclasses pass); arg1's meaning
     * is unknown. */
#ifdef CHARCOMMAND_PLAYER_COMMANDS
    /* Inline in the game, as Game/PlayerCommands.cpp sees them (the copies linked are 0x8006EE30
     * and 0x8006EDD4); the inline type accessor keeps the constructor out of line. The vtable is
     * then emitted with GetName, in Game/PlayerCommands.cpp. */
    static u16 CommandType(int id) {
        return id;
    }
    PlayerCommand(int id, int arg1) : CharCommand(CommandType(id), arg1) {}
    virtual ~PlayerCommand() {}
#else
    PlayerCommand(int id, int arg1);
    virtual ~PlayerCommand();
#endif
    /* The command's name, looked up by its id (0x8009CCE4). */
    virtual const char *GetName();
#ifdef CHARCOMMAND_PLAYER_COMMANDS
    /* The receiver as a player, or null (0x8009CC30; name inferred). */
    boost::shared_ptr<PlayerBase> GetPlayer(const boost::weak_ptr<CharInfo> &receiver);
#endif
};

class LaunchCommand : public PlayerCommand {
  public:
    LaunchCommand(int arg0, int arg1);
#ifdef CHARCOMMAND_PLAYER_COMMANDS
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
#endif

    /* 0x14 */ u32 field_0x14;
};

class IgnoreInputCommand : public PlayerCommand {
  public:
    IgnoreInputCommand(int arg0, float time);
    /* 0x800774C4: stores the two integers at 0x18 and 0x1C (meanings unknown). */
    IgnoreInputCommand(int arg0, int arg1);
#ifdef CHARCOMMAND_PLAYER_COMMANDS
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
#endif

    /* The layout as both constructors fill it: (arg0, time) stores time, 0 and arg0; (arg0, arg1)
     * stores arg0 and arg1 at 0x18 and 0x1C (meanings unknown). */
    /* 0x14 */ float m_time;
    /* 0x18 */ s32 m_kind;
    /* 0x1C */ s32 m_mode;
};

class ActingAccelerateCommand : public PlayerCommand {
  public:
    ActingAccelerateCommand(const Vec3f &velocity, float time);
#ifdef CHARCOMMAND_PLAYER_COMMANDS
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
#endif

    /* 0x14 */ float m_time;
    /* 0x18 */ Vec3f m_velocity;
};

/* StartJumpCommand (class name from RTTI; vtable at 0x805214F4): constructor 0x8009C994, which
 * zeroes the vector at 0x14 (meanings of arg0 and the vector unknown; Vehicle::GunLift sends it
 * with 4 to the rider it lets off). */
class StartJumpCommand : public PlayerCommand {
  public:
    StartJumpCommand(int arg0);
#if defined(CHARCOMMAND_START_JUMP_DIRECTION) && !defined(CHARCOMMAND_PLAYER_COMMANDS)
    /* A jump in direction (0x8009C900; see below). Declared for Game/TVehicleBase.cpp. */
    StartJumpCommand(const Vec3f &direction, int arg1);
#endif
#ifdef CHARCOMMAND_PLAYER_COMMANDS
    /* A jump in direction (normalized; zero when it is too short; 0x8009C900). */
    StartJumpCommand(const Vec3f &direction, int arg1);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
    virtual std::string vf_0x10() const;
#endif

    /* 0x14 */ Vec3f field_0x14; /* the jump's direction */
};

/* SetGravityCommand (class name from RTTI; vtable 0x80520F48): turns a character towards a new
 * gravity direction. Constructor 0x8009A368 (command id 0x12D) stores the direction normalized
 * (arg1's meaning unknown; the stage's gravity, Game/GravityDirection.cpp, passes 4). */
class SetGravityCommand : public PlayerCommand {
  public:
    SetGravityCommand(const Vec3f &direction, int arg1);
#ifdef CHARCOMMAND_PLAYER_COMMANDS
    virtual ~SetGravityCommand();
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
    virtual std::string vf_0x10() const;
#endif

    /* 0x14 */ Vec3f m_direction;
};

/* TurnOverCommand and PickupObjectCommand (class names from RTTI; vtables 0x8052112C and
 * 0x80521184): commands 0x11F and 0x11D, which make a player turn a target's owner over or pick
 * it up (Player::Control::Approach sends them; arg1's meaning unknown). Their constructors are
 * inline (Game/PlayerControlApproach.cpp has the copies); Reject and the destructors are in the
 * player commands' unit. */
class TurnOverCommand : public PlayerCommand {
  public:
    TurnOverCommand(const boost::weak_ptr<Target> &target, int arg1)
        : PlayerCommand(0x11F, arg1), m_target(target) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<Target> m_target;
};

class PickupObjectCommand : public PlayerCommand {
  public:
    PickupObjectCommand(const boost::weak_ptr<Target> &target, int arg1)
        : PlayerCommand(0x11D, arg1), m_target(target) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<Target> m_target;
};

#if defined(CHARCOMMAND_SET_LOOP_PATH) || defined(CHARCOMMAND_PLAYER_CONTROLLER)
/* SetLoopPathCommand (class name from RTTI; vtable 0x805213EC): the command of type 0x10C, which
 * puts a running player on a loop's path (Player::Behavior::Ground takes it). Only the path is
 * known (name inferred). Declared where CHARCOMMAND_SET_LOOP_PATH is defined
 * (Game/PlayerBehaviorGround.cpp), as the path's type needs Game/UnkCircuitTarget_800ABCE8.h. */
class SetLoopPathCommand : public PlayerCommand {
  public:
#ifdef CHARCOMMAND_PLAYER_CONTROLLER
    /* Inline: Game/PlayerController.cpp, which sends it, emits the copy the game keeps. */
    SetLoopPathCommand(const UnkCircuitTarget_800ABCE8 &path, int arg1)
        : PlayerCommand(0x10C, arg1), m_path(path) {}
#endif
#if defined(CHARCOMMAND_PLAYER_COMMANDS) || defined(CHARCOMMAND_PLAYER_CONTROLLER)
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
#endif

    /* 0x14 */ UnkCircuitTarget_800ABCE8 m_path;
};
#endif

#ifdef CHARCOMMAND_PLAYER_COMMANDS
/* The other player commands (class names from RTTI), declared where CHARCOMMAND_PLAYER_COMMANDS is
 * defined: Game/PlayerCommands.cpp, which has their Reject functions (each applies the command to
 * the player, through its behaviors, controls, motion or weapon, and marks it handled), their
 * vtables and the constructors that are not inline. The commands of Game/GadgetCharCommands.h
 * take their Reject declarations there. Field meanings are inferred from the Reject functions. */

/* Command 0x137: puts the player back in its starting state and drops every queued command. */
class ResetCommand : public PlayerCommand {
  public:
    ResetCommand();
    virtual ~ResetCommand();
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* Resets the stage camera of the player's number. */
class ResetCameraCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* Command 0x135: ends what the player's controller acts out. */
class CancelActingCommand : public PlayerCommand {
  public:
    CancelActingCommand(int arg1);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* Command 0x133: turns the player's aim to m_position for m_time. */
class SetLookAtPositionCommand : public PlayerCommand {
  public:
    SetLookAtPositionCommand(const Vec3f &position, float time);
    virtual ~SetLookAtPositionCommand();
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ Vec3f m_position;
    /* 0x20 */ float m_time;
};

/* Command 0x131: sends the player into an electric circuit (Game/ElecCircuitEntrance.cpp sends
 * it, with its own declaration). */
class ElectricCircuitCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ UnkCircuitTarget_800ABCE8 m_target;
    /* 0x1C */ u8 field_0x1C;
};

/* Command 0x12F: turns the player super, or back (m_on). */
class ToSuperCommand : public PlayerCommand {
  public:
    ToSuperCommand(bool on);
    virtual ~ToSuperCommand();
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ s32 m_on;
};

/* Plays motions m_motions on the player's external motion for m_time. */
class SetExternalMotionCommand : public PlayerCommand {
  public:
#ifdef CHARCOMMAND_EXTERNAL_MOTION_INLINE
    /* Inline in the game where CHARCOMMAND_EXTERNAL_MOTION_INLINE is defined (Game/TVehicleCarMove.cpp,
     * which defines the body; its weak copy is 0x801BC8A0). */
    SetExternalMotionCommand(const s32 *motions, int arg1, float time);
#endif
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
    virtual std::string vf_0x10() const;

    /* 0x14 */ s32 m_motions[3];
    /* 0x20 */ float m_time;
};

/* Makes the player invincible, or not (m_on). */
class InvincibleCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ bool m_on;
};

/* Clear and set the player's parallel-move state (bit 0x3C). */
class EndParallelMoveCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class StartParallelMoveCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* Command 0x123: puts the player in m_vehicle (behavior 0x1B). */
class GetOnVehicleCommand : public PlayerCommand {
  public:
    GetOnVehicleCommand(const boost::weak_ptr<Vehicle::TVehicleBase> &vehicle, int arg1);
    virtual ~GetOnVehicleCommand();
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<Vehicle::TVehicleBase> m_vehicle;
};

/* Makes the player throw the object it carries (behavior 0xA). */
class ThrowCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<PickupCharInfo> m_object;
};

/* Hands the player to its approach control (control 1) towards m_target. */
class ApproachControlCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ s32 m_kind;
    /* 0x18 */ boost::weak_ptr<Target> m_target;
};

/* Command 0x11B: gives the player item m_item (0 to 9) worth m_value, showing its icon when
 * m_showIcon is set (Game/GadgetModelInfo.cpp builds it). */
class GetItemCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ s32 m_item;
    /* 0x18 */ s32 m_value;
    /* 0x1C */ u8 m_showIcon;
};

/* Makes the player drop its weapon. */
class CastWeaponCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* Makes the player pick up m_weapon. */
class PickupWeaponCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_weapon;
};

/* The chaos control and chaos blast (behaviors 0x1D and 0x1C), the light dash and the spin dash
 * (0x14 and 0x12) and the slide (7): each switches the player's behavior. */
class ChaosControlCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class ChaosBlastCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class LightDashCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* The dark spin and the grind along a circuit target (behaviors 0x13 and 0x11). */
class DarkSpinCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ UnkCircuitTarget_800ABCE8 m_target;
};

class SpinDashCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class GrindCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ UnkCircuitTarget_800ABCE8 m_rail;
};

/* Sends the player to attack m_target (Game/BossSonicSubControlJump.cpp sends it, with its own
 * declaration). */
class HomingAttackCommand : public PlayerCommand {
  public:
#ifdef CHARCOMMAND_INLINE_BASE
    /* Inline (Game/NpcControlSonicType.cpp has the copy linked). */
    HomingAttackCommand(const boost::weak_ptr<Target> &target, int arg1)
        : PlayerCommand(0x10A, arg1), m_target(target) {}
#endif
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<Target> m_target;
};

class SlidingCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* Commands the behaviors take; Reject only marks them handled. */
class EndActionCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class EndAttackCommand : public PlayerCommand {
  public:
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* Starts the attack of the player's weapon. */
class StartAttackCommand : public PlayerCommand {
  public:
#ifdef CHARCOMMAND_INLINE_BASE
    /* Inline (Game/NpcControlSonicType.cpp has the copy linked). */
    StartAttackCommand(int arg1) : PlayerCommand(0x102, arg1) {}
#endif
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

#ifdef CHARCOMMAND_INLINE_BASE
/* EndJumpCommand (class name from RTTI; vtable 0x8051FD28, emitted with its inline functions in
 * Game/NpcControlSonicType.cpp): command 0x105, which ends a jump early (meaning inferred from its
 * senders). It overrides nothing: its receivers take it by its id. */
class EndJumpCommand : public PlayerCommand {
  public:
    EndJumpCommand(int arg1) : PlayerCommand(0x105, arg1) {}
};
#endif
#endif

#if defined(CHARCOMMAND_PLAYER_CONTROLLER) && !defined(CHARCOMMAND_PLAYER_COMMANDS)
/* GrindCommand (see above) as Game/PlayerController.cpp sends it: its inline constructor (the copy
 * the game keeps is that unit's). */
class GrindCommand : public PlayerCommand {
  public:
    GrindCommand(const UnkCircuitTarget_800ABCE8 &rail, int arg1) : PlayerCommand(0x10D, arg1), m_rail(rail) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ UnkCircuitTarget_800ABCE8 m_rail;
};
#endif

#if defined(CHARCOMMAND_PLAYER_USER_INPUT) && !defined(CHARCOMMAND_PLAYER_COMMANDS)
/* The player commands Player::Control::UserInput sends (Game/PlayerControlUserInput.cpp; see
 * above), as that unit sees them. Most constructors are inline: the inline id accessor keeps them
 * out of line, so the unit emits the copies the game keeps (as CancelUseWeaponCommand in
 * Game/PlayerController.cpp). Reject keeps the vtables in Game/PlayerCommands.cpp. */
inline int UserInputCommandId(int id) {
    return id;
}

class EndActionCommand : public PlayerCommand {
  public:
    EndActionCommand(int arg1) : PlayerCommand(UserInputCommandId(0x107), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class EndAttackCommand : public PlayerCommand {
  public:
    EndAttackCommand(int arg1) : PlayerCommand(UserInputCommandId(0x103), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class EndParallelMoveCommand : public PlayerCommand {
  public:
    EndParallelMoveCommand(int arg1) : PlayerCommand(UserInputCommandId(0x125), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class StartParallelMoveCommand : public PlayerCommand {
  public:
    StartParallelMoveCommand(int arg1) : PlayerCommand(UserInputCommandId(0x124), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class PickupWeaponCommand : public PlayerCommand {
  public:
    PickupWeaponCommand(const boost::weak_ptr<CharInfo> &weapon, int arg1)
        : PlayerCommand(UserInputCommandId(0x118), arg1), m_weapon(weapon) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_weapon;
};

class ApproachControlCommand : public PlayerCommand {
  public:
    ApproachControlCommand(int kind, const boost::weak_ptr<Target> &target, int arg1)
        : PlayerCommand(UserInputCommandId(0x11C), arg1), m_kind(kind), m_target(target) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ s32 m_kind;
    /* 0x18 */ boost::weak_ptr<Target> m_target;
};

class SpinDashCommand : public PlayerCommand {
  public:
    SpinDashCommand(int arg1) : PlayerCommand(UserInputCommandId(0x10E), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class SlidingCommand : public PlayerCommand {
  public:
    SlidingCommand(int arg1) : PlayerCommand(UserInputCommandId(0x109), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class LightDashCommand : public PlayerCommand {
  public:
    LightDashCommand(int arg1) : PlayerCommand(UserInputCommandId(0x110), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class DarkSpinCommand : public PlayerCommand {
  public:
    DarkSpinCommand(const UnkCircuitTarget_800ABCE8 &target, int arg1)
        : PlayerCommand(UserInputCommandId(0x10F), arg1), m_target(target) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ UnkCircuitTarget_800ABCE8 m_target;
};

class ThrowCommand : public PlayerCommand {
  public:
    ThrowCommand(const boost::weak_ptr<PickupCharInfo> &object, int arg1)
        : PlayerCommand(UserInputCommandId(0x11E), arg1), m_object(object) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<PickupCharInfo> m_object;
};

class CastWeaponCommand : public PlayerCommand {
  public:
    CastWeaponCommand(int arg1) : PlayerCommand(UserInputCommandId(0x119), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class ChaosControlCommand : public PlayerCommand {
  public:
    ChaosControlCommand(int arg1) : PlayerCommand(UserInputCommandId(0x114), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

class ChaosBlastCommand : public PlayerCommand {
  public:
    ChaosBlastCommand(int arg1) : PlayerCommand(UserInputCommandId(0x113), arg1) {}
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* Command 0x123 (constructor 0x8009AC80, in Game/PlayerCommands.cpp). */
class GetOnVehicleCommand : public PlayerCommand {
  public:
    GetOnVehicleCommand(const boost::weak_ptr<Vehicle::TVehicleBase> &vehicle, int arg1);
    virtual ~GetOnVehicleCommand();
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<Vehicle::TVehicleBase> m_vehicle;
};

/* (Constructor 0x8006FE88, in Game/NpcControlSonicType.cpp.) */
class HomingAttackCommand : public PlayerCommand {
  public:
    HomingAttackCommand(const boost::weak_ptr<Target> &target, int arg1);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);

    /* 0x14 */ boost::weak_ptr<Target> m_target;
};

/* (Constructor 0x8006ED90, in Game/NpcControlSonicType.cpp.) */
class StartAttackCommand : public PlayerCommand {
  public:
    StartAttackCommand(int arg1);
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
};

/* EndJumpCommand (see above; constructor 0x8006FED8, in Game/NpcControlSonicType.cpp). */
class EndJumpCommand : public PlayerCommand {
  public:
    EndJumpCommand(int arg1);
};
#endif

#if defined(CHARCOMMAND_SET_EXTERNAL_MOTION) && !defined(CHARCOMMAND_PLAYER_COMMANDS)
/* SetExternalMotionCommand (see above) as Game/UnkMotion_8022F058.cpp sends it: its constructor
 * (0x801BC8A0, command 0x12A; argument names inferred). */
class SetExternalMotionCommand : public PlayerCommand {
  public:
    SetExternalMotionCommand(const s32 *motions, int arg1, float time);

    /* 0x14 */ s32 m_motions[3];
    /* 0x20 */ float m_time;
};
#endif

} // namespace Player

SIZE_ASSERT(Player::StartJumpCommand, 0x20);
SIZE_ASSERT(Player::SetGravityCommand, 0x20);
SIZE_ASSERT(Player::LaunchCommand, 0x18);
SIZE_ASSERT(Player::IgnoreInputCommand, 0x20);
SIZE_ASSERT(Player::ActingAccelerateCommand, 0x24);
SIZE_ASSERT(Player::TurnOverCommand, 0x1C);
SIZE_ASSERT(Player::PickupObjectCommand, 0x1C);
#ifdef CHARCOMMAND_PLAYER_COMMANDS
SIZE_ASSERT(Player::ResetCommand, 0x14);
SIZE_ASSERT(Player::SetLookAtPositionCommand, 0x24);
SIZE_ASSERT(Player::ElectricCircuitCommand, 0x20);
SIZE_ASSERT(Player::ToSuperCommand, 0x18);
SIZE_ASSERT(Player::SetExternalMotionCommand, 0x24);
SIZE_ASSERT(Player::InvincibleCommand, 0x18);
SIZE_ASSERT(Player::GetOnVehicleCommand, 0x1C);
SIZE_ASSERT(Player::ThrowCommand, 0x1C);
SIZE_ASSERT(Player::ApproachControlCommand, 0x20);
SIZE_ASSERT(Player::GetItemCommand, 0x20);
SIZE_ASSERT(Player::PickupWeaponCommand, 0x1C);
SIZE_ASSERT(Player::DarkSpinCommand, 0x1C);
SIZE_ASSERT(Player::GrindCommand, 0x1C);
SIZE_ASSERT(Player::HomingAttackCommand, 0x1C);
#endif

#endif

#ifndef GAME_CHARCOMMAND_H
#define GAME_CHARCOMMAND_H

#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;
/* Target (class name from RTTI): units that use it declare its members. */
class Target;

namespace Weapon {
class WeaponVacuumBase;
} // namespace Weapon

/* CharCommand (class name from RTTI): a command queued on a character through CharInfo's
 * PushCommand. The commands below (class names from RTTI) all allocate through CharCommand's
 * operator new, which uses heap 2 (inferred from its body). The integer arguments' meanings are
 * unknown; the sizes come from the operator new calls. */
class CharCommand {
  public:
    /* Stores the command type at 0x4 and arg at 0x8 (0x8006E454; arg's meaning unknown). */
    CharCommand(u16 type, int arg);
    static void *operator new(size_t size);
    /* 0x801D9C64 (takes the size; behaviour inferred). */
    static void operator delete(void *p, size_t size);
    virtual ~CharCommand();
    /* The default handling of a command its receiver does not take (slot 0x0C; empty in CharCommand,
     * 0x801D9CFC; name inferred from EggLastMech, which passes itself as receiver). */
    virtual void Reject(const boost::weak_ptr<CharInfo> &receiver);
    virtual void vf_0x10();
    /* The command's name, for debug messages (slot 0x14; name inferred from EggLastMech). */
    virtual const char *GetName();

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
};
SIZE_ASSERT(CharCommand, 0x14);

class ClrVelocityCommand : public CharCommand {
  public:
    ClrVelocityCommand(int arg0);
};
SIZE_ASSERT(ClrVelocityCommand, 0x14);

class SpringCommand : public CharCommand {
  public:
    SpringCommand();
};
SIZE_ASSERT(SpringCommand, 0x14);

class SetPositionCommand : public CharCommand {
  public:
    SetPositionCommand(const Vec3f &position, int arg1);

    /* 0x14 */ Vec3f m_position;
};
SIZE_ASSERT(SetPositionCommand, 0x20);

class SetVelocityCommand : public CharCommand {
  public:
    SetVelocityCommand(const Vec3f &velocity, int arg1);

    /* 0x14 */ Vec3f m_velocity;
};
SIZE_ASSERT(SetVelocityCommand, 0x20);

class SetAngleCommand : public CharCommand {
  public:
    SetAngleCommand(const Vec3f &angles, int arg1, int arg2);

    /* 0x14 */ Vec3f m_angles;
    /* 0x20 */ u32 field_0x20;
};
SIZE_ASSERT(SetAngleCommand, 0x24);

/* UseCommand (class name from RTTI): the command of type 0xD, which a character sends to the gadget
 * it uses. Only the user is known (name inferred). */
class UseCommand : public CharCommand {
  public:
    /* The command without a target (0x801DB480). */
    UseCommand(const boost::weak_ptr<CharInfo> &user);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_user;
    /* 0x1C */ boost::weak_ptr<Target> m_target; /* the target used (from RegularRing) */
};
SIZE_ASSERT(UseCommand, 0x24);

/* VacuumCommand (class name from RTTI; vtable at 0x8053D278): the command of type 0x14, which
 * pulls a target in. Only the target is known (name inferred). */
class VacuumCommand : public CharCommand {
  public:
    /* Pulls target in with weapon (0x801DB0D8; from Game/WeaponVacuumPod.cpp, arg2 unknown). */
    VacuumCommand(const boost::weak_ptr<Target> &target,
                  const boost::weak_ptr<Weapon::WeaponVacuumBase> &weapon, bool arg2);

    /* 0x14 */ boost::weak_ptr<Target> m_target;
    /* 0x1C */ boost::weak_ptr<Weapon::WeaponVacuumBase> m_weapon;
    /* 0x24 */ boost::weak_ptr<CharInfo> field_0x24;
    /* 0x2C */ s32 field_0x2C;
};
SIZE_ASSERT(VacuumCommand, 0x30);

/* DamageCommand (class name from RTTI; vtable at 0x8053D44C): the command of type 7, which hurts a
 * character (constructor 0x801DB804; the direction and amount are inferred from
 * Game/WeaponVacuumPod.cpp, arg1 unknown). */
class DamageCommand : public CharCommand {
  public:
    DamageCommand(const Vec3f &direction, int arg1, float amount);
    /* The same with a zero direction (constructor 0x801DB864; used by WEIGHT). */
    DamageCommand(int arg1, float amount);

    /* 0x14 */ float m_amount;
    /* 0x18 */ Vec3f m_direction;
};
SIZE_ASSERT(DamageCommand, 0x24);

/* ReadyCommand (class name from RTTI; vtable at 0x8053D350): the command of type 0xE, which a
 * picked-up object sends its carrier (constructor 0x801DB3C0; meaning of the character
 * inferred). */
class ReadyCommand : public CharCommand {
  public:
    ReadyCommand(const boost::weak_ptr<CharInfo> &character);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_character;
};
SIZE_ASSERT(ReadyCommand, 0x1C);

/* PickupCommand (class name from RTTI; vtable at 0x8053D2E4): the command of type 0x11, which a
 * character sends to the object it picks up. Only the character is known (name inferred). */
class PickupCommand : public CharCommand {
  public:
    /* 0x14 */ Vec3f field_0x14;
    /* 0x20 */ float field_0x20;
    /* 0x24 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x2C */ Vec3f field_0x2C;
};
SIZE_ASSERT(PickupCommand, 0x38);

/* CastCommand and ThrowCommand (class names from RTTI; vtables at 0x8053D2C0 and 0x8053D29C):
 * the commands of types 0x12 and 0x13, which a carrier sends to the object it drops or throws:
 * the carrier, then the direction (names inferred). */
class CastCommand : public CharCommand {
  public:
    /* 0x14 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x1C */ Vec3f m_direction;
};
SIZE_ASSERT(CastCommand, 0x28);

class ThrowCommand : public CharCommand {
  public:
    /* 0x14 */ boost::weak_ptr<CharInfo> m_character;
    /* 0x1C */ Vec3f m_direction;
};
SIZE_ASSERT(ThrowCommand, 0x28);

/* RecoverCommand (class name from RTTI; vtable at 0x8053D308): the command of type 0x10 that heals
 * a character (constructor 0x801DB2F8; meanings inferred from the HEALING_UNIT gadget). */
class RecoverCommand : public CharCommand {
  public:
    RecoverCommand(const boost::weak_ptr<CharInfo> &source, float amount);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_source;
    /* 0x1C */ float m_amount;
};
SIZE_ASSERT(RecoverCommand, 0x20);

/* AnnounceAttackCommand (class name from RTTI; vtable at 0x8053D230): the command of type 0x16,
 * which tells a character it is being attacked (constructor 0x801DACE4; the attacker and the
 * meaning of the arguments are inferred from Player::Shadow's gun control, which passes 4, 4). */
class AnnounceAttackCommand : public CharCommand {
  public:
    AnnounceAttackCommand(const boost::weak_ptr<CharInfo> &attacker, int arg1, int arg2);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_attacker;
    /* 0x1C */ s32 field_0x1C;
};
SIZE_ASSERT(AnnounceAttackCommand, 0x20);

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

} // namespace Player

SIZE_ASSERT(Player::StartJumpCommand, 0x20);
SIZE_ASSERT(Player::LaunchCommand, 0x18);
SIZE_ASSERT(Player::IgnoreInputCommand, 0x20);
SIZE_ASSERT(Player::ActingAccelerateCommand, 0x24);

#endif

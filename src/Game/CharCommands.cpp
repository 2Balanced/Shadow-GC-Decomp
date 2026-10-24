/* The character commands (Game/CharCommand.h; class names from RTTI), in the order of their command
 * types: their names, their debug descriptions (slot 0x10: the base description, CharCommand's
 * name, followed by the values), their Reject functions, which apply the command directly to a
 * character no command unit takes it for, and the constructors that are not inline. The
 * destructors the classes leave implicit come out at the end of the unit with their vtables. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <stdio.h>
#include <string>

#define CHARCOMMAND_IMPLEMENTATION
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Quaternion.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace Weapon {
/* WeaponVacuumBase (class name from RTTI): only its owner is needed here. */
class WeaponVacuumBase;
} // namespace Weapon

extern "C" {
/* a + b for the debug descriptions (MSL's operator+ for a string and a C string, out of line in
 * the game). */
std::string fn_8004F5DC(const std::string &a, const char *b);
/* The character a vacuum weapon belongs to (see Game/VacuumObject.cpp). */
boost::weak_ptr<CharInfo> fn_80065034(Weapon::WeaponVacuumBase *weapon);
/* a += b, and v set to zero (behaviour inferred). */
void fn_8040CFF8(Vec3f *a, const Vec3f *b);
void fn_8040CE60(Vec3f *v);
/* The quaternion of Euler angles in a rotation order, and back (see Game/ThrowBomb.cpp). */
Quatf fn_804102F4(const Vec3f *angles, int order);
Vec3f fn_80410204(const Quatf *q, int order);
}

/* MSL's strncpy (see Game/MemoryCardCtrl.cpp). */
extern "C" char *fn_803ACABC(char *dst, const char *src, unsigned long size);

/* pi (see Game/BkSoldier.cpp). */
extern const float lbl_805FAD38;

/* Adds value to vector (name inferred). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* The vector type's out-of-line assignment (fn_800091EC; name inferred). GenerateCommand's
 * constructor calls it through this inline function: each inline call takes a number from the
 * compiler's count of anonymous objects, and retail's numbering (the RTTI records' @ names) has
 * room for this one. */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091EC(&vector, &value);
}

/* Copies a rotation one angle at a time (as in Game/ThrowBomb.cpp; name inferred). */
inline void CopyRotation(Vec3f &rotation, const Vec3f &value) {
    rotation.x = value.x;
    rotation.y = value.y;
    rotation.z = value.z;
}

/* Radians to degrees (name inferred). */
inline float RadToDeg(float radians) {
    return radians * (180.0f / lbl_805FAD38);
}

AccelerateInputCommand::AccelerateInputCommand(const Vec3f &direction, int arg1)
    : CharCommand(1, arg1) {
    fn_800091EC(&m_direction, &direction);
}

std::string AccelerateInputCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " % 8.4f % 8.4f % 8.4f", m_direction.x, m_direction.y, m_direction.z);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *AccelerateInputCommand::GetName() {
    return "AccelerateInput";
}

void SetPositionCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    if (boost::shared_ptr<CharInfo> character = receiver.lock()) {
        fn_800091D0(&character->m_position, &m_position);
        Flags() |= 2;
    }
}

std::string SetPositionCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " % 8.4f % 8.4f % 8.4f", m_position.x, m_position.y, m_position.z);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *SetPositionCommand::GetName() {
    return "SetPosition";
}

/* The rotation order as the conversion takes it (name unknown; out of line in the game, and
 * called from Game/WeaponChaosSpear.cpp and Game/WeaponSpecialKatana.cpp too). */
extern "C" int fn_801DBD00(int order) {
    return order;
}

void SetAngleCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    if (boost::shared_ptr<CharInfo> character = receiver.lock()) {
        if (field_0x20 == character->field_0x30) {
            CopyRotation(character->m_rotation, m_angles);
        } else {
            Quatf rotation(fn_804102F4(&m_angles, field_0x20));
            CopyRotation(character->m_rotation,
                         fn_80410204(&rotation, fn_801DBD00(character->field_0x30)));
        }
        Flags() |= 2;
    }
}

std::string SetAngleCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " % 8.4f % 8.4f % 8.4f", RadToDeg(m_angles.x), RadToDeg(m_angles.y),
            RadToDeg(m_angles.z));
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *SetAngleCommand::GetName() {
    return "SetAngle";
}

void ClrVelocityCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    if (boost::shared_ptr<CharInfo> character = receiver.lock()) {
        fn_8040CE60(&character->field_0x18);
        Flags() |= 2;
    }
}

const char *ClrVelocityCommand::GetName() {
    return "ClrVelocity";
}

void SetVelocityCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    if (boost::shared_ptr<CharInfo> character = receiver.lock()) {
        AddVector(character->field_0x18, m_velocity);
        Flags() |= 2;
    }
}

std::string SetVelocityCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " % 8.4f % 8.4f % 8.4f", m_velocity.x, m_velocity.y, m_velocity.z);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *SetVelocityCommand::GetName() {
    return "SetVelocity";
}

void AccelerateCommand::Reject(const boost::weak_ptr<CharInfo> &receiver) {
    if (boost::shared_ptr<CharInfo> character = receiver.lock()) {
        AddVector(character->field_0x24, m_velocity);
        Flags() |= 2;
    }
}

std::string AccelerateCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " % 8.4f % 8.4f % 8.4f", m_velocity.x, m_velocity.y, m_velocity.z);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *AccelerateCommand::GetName() {
    return "Accelerate";
}

DamageCommand::DamageCommand(int arg1, float amount) : CharCommand(7, arg1), m_amount(amount) {
    fn_800091EC(&m_direction, &lbl_805E07A0);
}

DamageCommand::DamageCommand(const Vec3f &direction, int arg1, float amount)
    : CharCommand(7, arg1), m_amount(amount) {
    fn_800091EC(&m_direction, &direction);
}

const char *DamageCommand::GetName() {
    return "Damage";
}

const char *DeadCommand::GetName() {
    return "Dead";
}

const char *SwitchCommand::GetName() {
    return "Switch";
}

std::string SwitchCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " UserData : %d", m_index);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

WallJumpCommand::WallJumpCommand(const boost::weak_ptr<CharInfo> &character,
                                 const Vec3f &position)
    : CharCommand(0xA, 4), m_character(character) {
    fn_800091EC(&m_position, &position);
}

std::string WallJumpCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " % 8.4f % 8.4f % 8.4f", m_position.x, m_position.y, m_position.z);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *WallJumpCommand::GetName() {
    return "WallJump";
}

RideCommand::RideCommand(const boost::weak_ptr<CharInfo> &partner, const Vec3f &position)
    : CharCommand(0xB, 4), m_partner(partner) {
    fn_800091EC(&m_position, &position);
}

std::string RideCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " % 8.4f % 8.4f % 8.4f", m_position.x, m_position.y, m_position.z);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *RideCommand::GetName() {
    return "Ride";
}

LeaveCommand::LeaveCommand(const boost::weak_ptr<CharInfo> &partner, const Vec3f &position)
    : CharCommand(0xC, 4), m_partner(partner) {
    fn_800091EC(&m_position, &position);
}

std::string LeaveCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " % 8.4f % 8.4f % 8.4f", m_position.x, m_position.y, m_position.z);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *LeaveCommand::GetName() {
    return "Leave";
}

UseCommand::UseCommand(const boost::weak_ptr<CharInfo> &user) : CharCommand(0xD, 4), m_user(user) {}

UseCommand::UseCommand(const boost::weak_ptr<CharInfo> &user, const boost::weak_ptr<Target> &target)
    : CharCommand(0xD, 4), m_user(user), m_target(target) {}

const char *UseCommand::GetName() {
    return "Use";
}

ReadyCommand::ReadyCommand(const boost::weak_ptr<CharInfo> &character)
    : CharCommand(0xE, 4), m_character(character) {}

const char *ReadyCommand::GetName() {
    return "Ready";
}

CancelCommand::CancelCommand(const boost::weak_ptr<CharInfo> &character)
    : CharCommand(0xF, 4), m_character(character) {}

const char *CancelCommand::GetName() {
    return "Cancel";
}

RecoverCommand::RecoverCommand(const boost::weak_ptr<CharInfo> &source, float amount)
    : CharCommand(0x10, 4), m_source(source), m_amount(amount) {}

const char *RecoverCommand::GetName() {
    return "Recover";
}

PickupCommand::PickupCommand(const boost::weak_ptr<CharInfo> &character, const Vec3f &arg1,
                             float arg2, const Vec3f &arg3)
    : CharCommand(0x11, 4), field_0x14(arg1), field_0x20(arg2), m_character(character),
      field_0x2C(arg3) {}

const char *PickupCommand::GetName() {
    return "Pickup";
}

CastCommand::CastCommand(const boost::weak_ptr<CharInfo> &character, const Vec3f &direction)
    : CharCommand(0x12, 4), m_character(character) {
    fn_800091EC(&m_direction, &direction);
}

const char *CastCommand::GetName() {
    return "Cast";
}

ThrowCommand::ThrowCommand(const boost::weak_ptr<CharInfo> &character, const Vec3f &direction)
    : CharCommand(0x13, 4), m_character(character) {
    fn_800091EC(&m_direction, &direction);
}

const char *ThrowCommand::GetName() {
    return "Throw";
}

VacuumCommand::VacuumCommand(const boost::weak_ptr<Target> &target,
                             const boost::weak_ptr<Weapon::WeaponVacuumBase> &weapon, int arg2)
    : CharCommand(0x14, 4), m_target(target), m_weapon(weapon), field_0x2C(arg2) {}

VacuumCommand::VacuumCommand(const boost::weak_ptr<Target> &target,
                             const boost::weak_ptr<CharInfo> &owner)
    : CharCommand(0x14, 4), m_target(target), field_0x24(owner), field_0x2C(1) {}

const char *VacuumCommand::GetName() {
    return "Vacuum";
}

/* The character command pulls for: the weapon's owner, else the one it was given, else none (name
 * inferred). */
extern "C" boost::weak_ptr<CharInfo> fn_801DADFC(VacuumCommand *command) {
    if (!command->m_weapon.expired()) {
        return fn_80065034(command->m_weapon.lock().get());
    }
    if (!command->field_0x24.expired()) {
        return command->field_0x24;
    }
    return boost::shared_ptr<CharInfo>();
}

ControlVehicleCommand::ControlVehicleCommand(int action, int arg1)
    : CharCommand(0x15, arg1), m_action(action) {}

std::string ControlVehicleCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " %4d", m_action);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *ControlVehicleCommand::GetName() {
    return "ControlVehicle";
}

AnnounceAttackCommand::AnnounceAttackCommand(const boost::weak_ptr<CharInfo> &attacker, int arg1,
                                             int arg2)
    : CharCommand(0x16, arg2), m_attacker(attacker), field_0x1C(arg1) {}

AnnounceAttackCommand::~AnnounceAttackCommand() {}

std::string AnnounceAttackCommand::vf_0x10() const {
    char text[0x44];
    char name[0x10];
    name[0] = '\0';
    boost::shared_ptr<CharInfo> attacker = m_attacker.lock();
    if (attacker.get() != 0) {
        fn_803ACABC(name, reinterpret_cast<const char *>(attacker.get()->field_0x3C), sizeof(name));
    }
    name[sizeof(name) - 1] = '\0';
    sprintf(text, " %4d from %s", field_0x1C, name);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *AnnounceAttackCommand::GetName() {
    return "AnnounceAttack";
}

SetTransparentCommand::SetTransparentCommand(bool transparent, float rate, int target)
    : CharCommand(0x17, 4), m_transparent(transparent), m_alpha(rate), field_0x1C(target) {}

std::string SetTransparentCommand::vf_0x10() const {
    char text[0x44];
    sprintf(text, " %s Rate %5.2f Target %d", m_transparent ? "On" : "Off", m_alpha, field_0x1C);
    return fn_8004F5DC(CharCommand::vf_0x10(), text);
}

const char *SetTransparentCommand::GetName() {
    return "SetTransparent";
}

SetLightCommand::SetLightCommand(int light) : CharCommand(0x18, 4), m_light(light) {}

const char *SetLightCommand::GetName() {
    return "SetLight";
}

GenerateCommand::GenerateCommand(const Vec3f &position, Vec3f &rotation)
    : CharCommand(0x19, 4) {
    AssignVector(m_position, position);
    CopyRotation(m_rotation, rotation);
}

const char *GenerateCommand::GetName() {
    return "Generate";
}

BindCommand::BindCommand(const boost::weak_ptr<CharInfo> &binder)
    : CharCommand(0x1A, 4), m_binder(binder) {}

const char *BindCommand::GetName() {
    return "Bind";
}

ReleaseCommand::ReleaseCommand(const boost::weak_ptr<CharInfo> &binder)
    : CharCommand(0x1B, 4), m_binder(binder) {}

const char *ReleaseCommand::GetName() {
    return "Release";
}

ScoreCommand::ScoreCommand(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
    : CharCommand(0x1C, 4) {
    m_values[0] = arg0;
    m_values[1] = arg1;
    m_values[2] = arg2;
    m_values[3] = arg3;
    m_values[4] = arg4;
}

const char *ScoreCommand::GetName() {
    return "Score";
}

SpringCommand::SpringCommand() : CharCommand(0x1D, 4) {}

SpringCommand::~SpringCommand() {}

const char *SpringCommand::GetName() {
    return "Spring";
}

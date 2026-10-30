/* Vehicle::TVehicleBase (class name from RTTI; declared in Game/TVehicleBase.h): the base of the
 * vehicles, a CharInfo with a rider, a second character (the shooter), its own weak pointer and a
 * state object, Vehicle::VehicleBaseImpl (class name from the RTTI of its bound callback): the
 * vehicle's kind, its life, its two colliders and the command unit that passes commands on to the
 * vehicle. Also Vehicle::VehicleTask (class name from RTTI), the task that runs a vehicle. Many
 * functions keep their placeholder names, as free functions over the vehicle, because other units
 * call them by those names; member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define VEHICLE_BASE_IMPL
#define CHARCOMMAND_START_JUMP_DIRECTION
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliMove.h"
#include "Game/MathTypes.h"
#include "Game/TVehicleBase.h"
#include "Game/Task.h"
#include "Game/VehicleEffect.h"
#include "Game/VehicleTask.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;
using Vehicle::TVehicleBase;
using Vehicle::VehicleTask;

namespace Player {
class PlayerBase;
} // namespace Player

class UnkCameraManager_80009548;

/* The game state; the frame time is its float at 0x20 (see Game/Enemy.h). */
extern u8 lbl_8057D6A0[0x150];

/* A player's frame time (0x80087F84; see Game/PlayerBehaviorJump.cpp). */
const float &FrameTime(Player::PlayerBase *player);

extern "C" {
/* The 32-bit flag word (see Game/BAMine.cpp): fn_80014E04 initializes it, fn_80014E10 clears it,
 * fn_80014E1C sets a bit, fn_8006127C clears one and fn_80014E34 tests one. */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);

/* The camera manager and a screen's camera position, and the squared distance of two points (see
 * Game/AudioChannel.cpp). */
UnkCameraManager_80009548 *fn_80009548(void);
const Vec3f *fn_80010244(UnkCameraManager_80009548 *manager, int index);
float fn_8040D180(const Vec3f *a, const Vec3f *b);

/* A value clamped to a range (see Game/BirdBase.cpp). */
const float &fn_80070460(const float &value, const float &low, const float &high);

/* The collider manager: a collider made from a model clump, and a collider's removal (see
 * Game/AttackObject.cpp and Game/GadgetModelInfo.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C81F4(void *manager,
                                              const boost::weak_ptr<CharInfo> &charInfo,
                                              void *clump, int group, int arg4);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);

/* Sends a command to a command unit (see Game/BlackBull0210.cpp). */
void fn_801D9C0C(boost::shared_ptr<CharCommandUnit> unit, CharCommand *command);

/* The vehicle table's kind names (Game/VehicleTable.cpp). */
const char *fn_801B4B34(int kind);

/* The zero vector (see Game/GunLift.cpp). */
extern const Vec3f lbl_805E07A0;
}

/* A vector returned through the out-of-line copy constructor (fn_800091EC; as in
 * Game/VehicleCharaColliMove.cpp, whose CharaColliMove::GetVelocity returns one). */
struct UnkCopyVector3_800091EC {
    UnkCopyVector3_800091EC(const UnkCopyVector3_800091EC &v) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
    }

    float x;
    float y;
    float z;
};

inline const UnkCopyVector3_800091EC &AsCopy(const Vec3f &v) {
    return reinterpret_cast<const UnkCopyVector3_800091EC &>(v);
}

/* CharInfo's virtual slots 0x0C and 0x14 as the vehicle calls them, with an argument
 * (Game/CharInfo.h declares them without one; names inferred): the side the character is on, and
 * a command passed on from the vehicle. Only the layout matters; the class is never defined. */
class UnkCharInfoSlots_8053B87C {
  public:
    /* 0x00 */ u8 field_0x0[0x38];

    virtual ~UnkCharInfoSlots_8053B87C();
    virtual void SetSide(int side);
    virtual void PushCommand(CharCommand *command);
    virtual void TakeCommand(CharCommand *command);
};

inline UnkCharInfoSlots_8053B87C *Slots(CharInfo *chara) {
    return reinterpret_cast<UnkCharInfoSlots_8053B87C *>(chara);
}

/* Sets the frame time a character follows (name inferred). */
inline void SetFrameTime(CharInfo *chara, const float *time) {
    *reinterpret_cast<const float **>(chara->field_0x60) = time;
}

/* The frame time a character follows (CharInfo's word at 0x60; name inferred). */
inline const float *&FrameTimeOf(CharInfo *chara) {
    return *reinterpret_cast<const float **>(chara->field_0x60);
}

/* How long a vehicle may stand without a rider: it starts to warn, then it is deleted (seconds;
 * names inferred). */
static float s_warnTime = 25.0f;
static float s_deleteTime = 30.0f;

namespace Vehicle {

class VehicleBaseImpl {
  public:
    VehicleBaseImpl(void *kind);
    ~VehicleBaseImpl();

    void PushCommand(CharCommand *command);
    boost::shared_ptr<CharaColliBase> GetCollider2() const;
    void SetCollider2(boost::shared_ptr<CharaColliBase> collider);
    boost::shared_ptr<CharaColliBase> GetCollider() const;
    void SetCollider(boost::shared_ptr<CharaColliBase> collider);
    void OnCommand(CharCommand &command);
    void CreateCollider(TVehicleBase *owner, void *clump);
    void ReleaseCollider();
    void CreateCollider2(TVehicleBase *owner, void *clump);
    void ReleaseCollider2();

    void SetOwner(TVehicleBase *owner) {
        m_owner = owner;
    }

    /* 0x00 */ s32 m_kind;
    /* 0x04 */ float m_maxLife;
    /* 0x08 */ float m_life;
    /* 0x0C */ TVehicleBase *m_owner;
    /* 0x10 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x18 */ boost::shared_ptr<CharaColliBase> m_collider2;
    /* 0x20 */ boost::shared_ptr<CharCommandUnit> m_commandUnit;
    /* 0x28 */ float m_idleTime; /* how long nobody has ridden the vehicle */
};
SIZE_ASSERT(VehicleBaseImpl, 0x2C);

} // namespace Vehicle

using Vehicle::VehicleBaseImpl;

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteVehicleBaseImpl(VehicleBaseImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<VehicleBaseImpl> out of line (0x801B6C28) and the destructor
 * at its definition; see Game/Rocket.cpp and docs/research-notes.md. */
namespace boost {
template <> inline void checked_delete<VehicleBaseImpl>(VehicleBaseImpl *x) {
    DeleteVehicleBaseImpl(x);
}
} // namespace boost

namespace Vehicle {

VehicleBaseImpl::VehicleBaseImpl(void *kind) : m_kind((s32)kind), m_maxLife(0.0f), m_life(0.0f) {
    m_commandUnit = boost::shared_ptr<CharCommandUnit>(new CharCommandUnit);
    fn_800729B0(&m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&VehicleBaseImpl::OnCommand, this, _1)));
}

VehicleBaseImpl::~VehicleBaseImpl() {}

void VehicleBaseImpl::PushCommand(CharCommand *command) {
    fn_801D9C0C(m_commandUnit, command);
}

boost::shared_ptr<CharaColliBase> VehicleBaseImpl::GetCollider2() const {
    return m_collider2;
}

void VehicleBaseImpl::SetCollider2(boost::shared_ptr<CharaColliBase> collider) {
    m_collider2 = collider;
}

boost::shared_ptr<CharaColliBase> VehicleBaseImpl::GetCollider() const {
    return m_collider;
}

void VehicleBaseImpl::SetCollider(boost::shared_ptr<CharaColliBase> collider) {
    m_collider = collider;
}

void VehicleBaseImpl::OnCommand(CharCommand &command) {
    m_owner->OnCommand(&command);
}

void VehicleBaseImpl::CreateCollider(TVehicleBase *owner, void *clump) {
    void *manager = fn_80021B08();
    m_collider = fn_800C81F4(manager, boost::weak_ptr<CharInfo>(owner->m_self), clump, 2, 0);
}

void VehicleBaseImpl::ReleaseCollider() {
    if (m_collider) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
}

void VehicleBaseImpl::CreateCollider2(TVehicleBase *owner, void *clump) {
    void *manager = fn_80021B08();
    m_collider2 = fn_800C81F4(manager, boost::weak_ptr<CharInfo>(owner->m_self), clump, 10, 0);
}

void VehicleBaseImpl::ReleaseCollider2() {
    if (m_collider2) {
        fn_800C78B0(fn_80021B08(), &m_collider2);
    }
}

TVehicleBase::TVehicleBase(void *arg) : CharInfo(0x33), field_0x98(new VehicleBaseImpl(arg)) {
    field_0x5C = 6;
    field_0x74 = 0;
    field_0x78 = 0;
    field_0x7C = 0;
    field_0x30 = 2;
}

TVehicleBase::~TVehicleBase() {}

void TVehicleBase::PushCommand(CharCommand *command) {
    field_0x98->PushCommand(command);
}

bool TVehicleBase::vf_0x18() {
    if (m_rider.expired()) {
        return false;
    }
    return m_rider.lock()->vf_0x18();
}

bool TVehicleBase::vf_0x1C() {
    if (m_rider.expired()) {
        return false;
    }
    return m_rider.lock()->vf_0x1C();
}

bool TVehicleBase::vf_0x20() {
    return false;
}

bool TVehicleBase::vf_0x24() {
    return false;
}

bool TVehicleBase::vf_0x28() {
    if (m_rider.expired()) {
        return false;
    }
    return m_rider.lock()->vf_0x28();
}

} // namespace Vehicle

extern "C" {

/* Takes the vehicle's own weak pointer and sets the vehicle up (name inferred). */
void fn_801B6958(TVehicleBase *vehicle, const boost::weak_ptr<TVehicleBase> &self) {
    fn_80014E10(&vehicle->m_vehicleFlags.m_bits);
    vehicle->m_self = self;
    vehicle->field_0x98->SetOwner(vehicle);
    vehicle->Setup();
}

/* Deletes the vehicle's task (name inferred). */
void fn_801B6944(TVehicleBase *vehicle) {
    reinterpret_cast<Task *>(vehicle->field_0x78)->m_flags |= 1;
}

/* The vehicle's kind (name inferred). */
int fn_801B6938(TVehicleBase *vehicle) {
    return vehicle->field_0x98->m_kind;
}

/* Marks the commands a vehicle takes itself as handled and passes the others on to its rider
 * (name inferred). */
void fn_801B6860(TVehicleBase *vehicle, CharCommand *command) {
    switch (command->TypeId()) {
    case 0xB:
    case 0xC:
    case 0x101:
    case 0x121:
    case 0x122:
    case 0x12B:
    case 0x12C:
        command->Flags() |= 2;
        break;
    }
    if (!(command->Flags() & 2) && !vehicle->m_rider.expired()) {
        Slots(vehicle->m_rider.lock().get())->TakeCommand(command);
        command->Flags() |= 2;
    }
}

/* A rider gets on, unless the vehicle has one (name inferred). */
bool fn_801B67CC(TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> &rider) {
    if (vehicle->m_rider.expired()) {
        vehicle->m_rider = rider;
        vehicle->m_shooter.reset();
        if (!vehicle->m_rider.expired()) {
            SetFrameTime(vehicle, &FrameTime(reinterpret_cast<Player::PlayerBase *>(
                                         vehicle->m_rider.lock().operator->())));
        }
        return true;
    }
    return false;
}

} // extern "C"

namespace Vehicle {

boost::weak_ptr<CharInfo> &TVehicleBase::GetRider() {
    return m_rider;
}

} // namespace Vehicle

extern "C" {

/* The rider gets off, if it is the vehicle's (name inferred). */
void fn_801B6718(TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> &rider) {
    if (vehicle->m_rider.lock() == rider.lock()) {
        vehicle->m_rider.reset();
        vehicle->m_shooter.reset();
        SetFrameTime(vehicle, reinterpret_cast<const float *>(&lbl_8057D6A0[0x20]));
    }
}

void fn_801B66F4(TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> &shooter) {
    vehicle->m_shooter = shooter;
}

} // extern "C"

namespace Vehicle {

boost::weak_ptr<CharInfo> &TVehicleBase::GetShooter() {
    return m_shooter;
}

} // namespace Vehicle

extern "C" {

/* Sets and tests the vehicle's flag 4 (meaning unknown). */
void fn_801B66AC(TVehicleBase *vehicle, bool on) {
    if (on) {
        fn_80014E1C(&vehicle->m_vehicleFlags.m_bits, 4);
    } else {
        fn_8006127C(&vehicle->m_vehicleFlags.m_bits, 4);
    }
}

bool fn_801B6684(TVehicleBase *vehicle) {
    return fn_80014E34(&vehicle->m_vehicleFlags.m_bits, 4);
}

} // extern "C"

namespace Vehicle {

void TVehicleBase::SetPlacement(const Vec3f *position, const Vec3f *rotation) {
    fn_800091D0(&m_position, position);
    m_rotation.x = rotation->x;
    m_rotation.y = rotation->y;
    m_rotation.z = rotation->z;
}

} // namespace Vehicle

extern "C" {

/* The vehicle's position and rotation; either may be null (name inferred). */
void fn_801B65DC(const TVehicleBase *vehicle, Vec3f *position, Vec3f *rotation) {
    if (position != 0) {
        fn_800091D0(position, &vehicle->m_position);
    }
    if (rotation != 0) {
        *rotation = vehicle->m_rotation;
    }
}

} // extern "C"

namespace Vehicle {

void TVehicleBase::GetSeat(Vec3f *position, Vec3f *rotation) {
    if (position != 0) {
        fn_800091D0(position, &m_position);
    }
    if (rotation != 0) {
        *rotation = m_rotation;
    }
}

} // namespace Vehicle

extern "C" {

/* The vehicle's life: its maximum, the damage it takes (clamped to 0 and the maximum), its value
 * and its reset (names inferred). */
void fn_801B6570(TVehicleBase *vehicle, float value) {
    vehicle->field_0x98->m_maxLife = value;
}

float fn_801B6564(TVehicleBase *vehicle) {
    return vehicle->field_0x98->m_maxLife;
}

/* The life never goes below it (name inferred). Retail keeps the zero in .sdata. */
#pragma explicit_zero_data on
static float s_noLife = 0.0f;
#pragma explicit_zero_data reset

void fn_801B6510(TVehicleBase *vehicle, float delta) {
    vehicle->field_0x98->m_life += delta;
    float &life = vehicle->field_0x98->m_life;
    life = fn_80070460(life, s_noLife, vehicle->field_0x98->m_maxLife);
}

} // extern "C"

namespace Vehicle {

void TVehicleBase::SetField98Float(float value) {
    field_0x98->m_life = value;
}

} // namespace Vehicle

extern "C" {

float fn_801B64F8(TVehicleBase *vehicle) {
    return vehicle->field_0x98->m_life;
}

void fn_801B64E8(TVehicleBase *vehicle) {
    vehicle->field_0x98->m_life = vehicle->field_0x98->m_maxLife;
}

/* The second collider: made from a model clump, set, released (names inferred). */
void fn_801B64B8(TVehicleBase *vehicle, void *clump) {
    vehicle->field_0x98->CreateCollider2(vehicle, clump);
}

void fn_801B6470(TVehicleBase *vehicle, boost::shared_ptr<CharaColliBase> collider) {
    vehicle->field_0x98->SetCollider2(collider);
}

void fn_801B644C(TVehicleBase *vehicle) {
    vehicle->field_0x98->ReleaseCollider2();
}

} // extern "C"

namespace Vehicle {

void TVehicleBase::SetClump(void *clump) {
    field_0x98->CreateCollider(this, clump);
}

} // namespace Vehicle

extern "C" {

/* The body collider: set, released (names inferred). */
void fn_801B63D4(TVehicleBase *vehicle, boost::shared_ptr<CharaColliBase> collider) {
    vehicle->field_0x98->SetCollider(collider);
}

void fn_801B63B0(TVehicleBase *vehicle) {
    vehicle->field_0x98->ReleaseCollider();
}

boost::shared_ptr<CharaColliBase> fn_801B6380(const TVehicleBase *vehicle) {
    return vehicle->field_0x98->GetCollider2();
}

boost::shared_ptr<CharaColliBase> fn_801B6350(const TVehicleBase *vehicle) {
    return vehicle->field_0x98->GetCollider();
}

} // extern "C"

namespace Vehicle {

void TVehicleBase::Setup() {}

void TVehicleBase::vf_0x40(UnkColliContactNode_800587A0 *const &hit) {}

void TVehicleBase::OnGetOn() {
    fn_80014E1C(&m_vehicleFlags.m_bits, 0);
    fn_8006127C(&m_vehicleFlags.m_bits, 3);
    if (!m_rider.expired()) {
        Slots(this)->SetSide(m_rider.lock()->field_0x54);
    }
}

void TVehicleBase::OnGetOff() {
    fn_8006127C(&m_vehicleFlags.m_bits, 0);
    fn_80014E1C(&m_vehicleFlags.m_bits, 2);
    fn_80014E1C(&m_vehicleFlags.m_bits, 3);
    field_0x98->m_idleTime = 0.0f;
    Slots(this)->SetSide(-1);
}

bool TVehicleBase::vf_0x50() {
    return false;
}

bool TVehicleBase::vf_0x54() {
    return false;
}

bool TVehicleBase::vf_0x58() {
    return true;
}

bool TVehicleBase::vf_0x5C() {
    return false;
}

bool TVehicleBase::vf_0x60() {
    return false;
}

float TVehicleBase::vf_0x64() {
    return 0.0f;
}

void TVehicleBase::vf_0x68() {}

void TVehicleBase::vf_0x6C() {}

bool TVehicleBase::vf_0x70() {
    return false;
}

void TVehicleBase::vf_0x74() {}

void TVehicleBase::vf_0x78() {}

void TVehicleBase::vf_0x7C(void *arg) {}

} // namespace Vehicle

/* Marks a task as done (name inferred). */
inline void KillTask(Task *task) {
    task->m_flags |= 1;
}

/* The vehicle's collision move (TVehicleBase::field_0x7C; name inferred). */
inline Vehicle::CharaColliMove *ColliMoveOf(const TVehicleBase *vehicle) {
    return reinterpret_cast<Vehicle::CharaColliMove *>(vehicle->field_0x7C);
}

extern "C" {

/* The vehicle's collision move: its callback, its contact and its velocity (names inferred). */
void fn_801B61DC(TVehicleBase *vehicle,
                 const boost::function1<void, CharaColliMoveArgs &> &callback) {
    if (ColliMoveOf(vehicle) != 0) {
        ColliMoveOf(vehicle)->SetCallback(callback);
    }
}

void fn_801B61B0(TVehicleBase *vehicle, const UnkColliMoveContact_803334A4 &contact) {
    if (ColliMoveOf(vehicle) != 0) {
        ColliMoveOf(vehicle)->SetContact(contact);
    }
}

void fn_801B6184(TVehicleBase *vehicle) {
    if (ColliMoveOf(vehicle) != 0) {
        ColliMoveOf(vehicle)->ClearContact();
    }
}

const UnkColliMoveContact_803334A4 *fn_801B6150(const TVehicleBase *vehicle) {
    if (ColliMoveOf(vehicle) != 0) {
        return ColliMoveOf(vehicle)->GetContact();
    }
    return 0;
}

UnkCopyVector3_800091EC fn_801B6108(const TVehicleBase *vehicle) {
    if (ColliMoveOf(vehicle) != 0) {
        return ColliMoveOf(vehicle)->GetVelocity();
    }
    return AsCopy(lbl_805E07A0);
}

/* The rider jumps off, in direction if one is given (name inferred). */
void fn_801B6030(TVehicleBase *vehicle, const Vec3f *direction) {
    boost::shared_ptr<CharInfo> rider = vehicle->m_rider.lock();
    if (rider) {
        if (direction != 0) {
            rider->PushCommand(new Player::StartJumpCommand(*direction, 4));
        } else {
            rider->PushCommand(new Player::StartJumpCommand(4));
        }
    }
}

/* The rider dies (name inferred). */
void fn_801B5FA4(TVehicleBase *vehicle) {
    boost::shared_ptr<CharInfo> rider = vehicle->m_rider.lock();
    if (rider) {
        rider->PushCommand(new DeadCommand(4));
    }
}

} // extern "C"

/* Whether the vehicle is farther than range from the cameras (name inferred). */
static bool IsOutOfRange(TVehicleBase *vehicle, float range) {
    Vec3f camera;
    fn_800091D0(&camera, fn_80010244(fn_80009548(), 0));
    range *= range;
    if (fn_8040D180(&camera, &vehicle->m_position) < range) {
        return false;
    }
    if (lbl_8057D6A0[0xB2] != 0) {
        fn_800091D0(&camera, fn_80010244(fn_80009548(), 1));
        if (fn_8040D180(&camera, &vehicle->m_position) < range) {
            return false;
        }
    }
    return true;
}

namespace Vehicle {

VehicleTask::VehicleTask(Task *parent, const boost::shared_ptr<TVehicleBase> &vehicle)
    : Task(parent), m_vehicle(vehicle) {
    m_range = -1.0f;
    m_name = fn_801B4B34(fn_801B6938(m_vehicle.get()));
    m_vehicle->field_0x78 = (u32)this;
    new Effect(this, m_vehicle);
    fn_801B6958(m_vehicle.get(), boost::weak_ptr<TVehicleBase>(m_vehicle));
}

VehicleTask::~VehicleTask() {
    m_vehicle->field_0x78 = 0;
    if (m_vehicle->field_0x7C != 0) {
        KillTask(reinterpret_cast<Task *>(m_vehicle->field_0x7C));
    }
}

} // namespace Vehicle

extern "C" {

void fn_801B5D24(VehicleTask *task, const Vec3f *position, const Vec3f *rotation) {
    task->m_vehicle->SetPlacement(position, rotation);
}

void fn_801B5D1C(VehicleTask *task, float range) {
    task->m_range = range;
}

/* Lets the vehicle go (name inferred). */
void fn_801B5CE8(VehicleTask *task) {
    if (task->m_vehicle) {
        fn_80014E1C(&task->m_vehicle->m_vehicleFlags.m_bits, 3);
    }
}

} // extern "C"

namespace Vehicle {

void VehicleTask::Tick(float dt) {
    m_vehicle->Tick(*FrameTimeOf(m_vehicle.get()));
    if (fn_80014E34(&m_vehicle->m_vehicleFlags.m_bits, 3) && m_vehicle->m_rider.expired() &&
        m_range > 0.0f && IsOutOfRange(m_vehicle.get(), m_range)) {
        m_flags |= 1;
        return;
    }
    if (fn_80014E34(&m_vehicle->m_vehicleFlags.m_bits, 2)) {
        if (m_vehicle->m_rider.expired()) {
            if (m_vehicle->vf_0x5C()) {
                m_vehicle->field_0x98->m_idleTime += dt;
                if (m_vehicle->field_0x98->m_idleTime >= s_deleteTime) {
                    m_flags |= 1;
                } else if (m_vehicle->field_0x98->m_idleTime >= s_warnTime) {
                    if (!m_vehicle->vf_0x70()) {
                        m_vehicle->vf_0x68();
                    }
                }
            }
        } else {
            if (m_vehicle->vf_0x70()) {
                m_vehicle->vf_0x6C();
            }
            m_vehicle->field_0x98->m_idleTime = 0.0f;
        }
    }
}

} // namespace Vehicle

extern "C" {

/* The vehicle's kind, whether someone rides it, and its slot 0x58 (names inferred). */
int fn_801B5B08(VehicleTask *task) {
    return fn_801B6938(task->m_vehicle.get());
}

bool fn_801B5AD4(VehicleTask *task) {
    return !task->m_vehicle->m_rider.expired();
}

bool fn_801B5AA4(VehicleTask *task) {
    return task->m_vehicle->vf_0x58();
}

} // extern "C"

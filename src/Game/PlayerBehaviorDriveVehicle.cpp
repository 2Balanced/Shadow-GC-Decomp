/* Player::Behavior::DriveVehicle (class name from RTTI): behavior 0x1B of Shadow, riding a vehicle
 * (Player::Behavior::InitializeArgsDriveVehicle). Entering it checks that the vehicle takes the
 * player, drops a kind 5 weapon and plays the riding motion; the player then swings onto the seat
 * along an arc for half a second, tells the vehicle it has got on and follows the seat (on a
 * vehicle of type 7 the hands are placed on its controls), passing inputs and vehicle commands on
 * and keeping the vehicle's gauge in the player's record. Losing the vehicle gets off and jumps
 * (behavior 0x10's Player::Behavior::InitializeArgsJump). File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). Where the game reloads the player
 * pointer in a call's arguments after another argument's call, the code reads m_player.px (as in
 * Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
/* Declares Vehicle::TVehicleBase's slot 0x48 and Player::PlayerMotion's slot 0x24 with their
 * parameters (see Game/TVehicleBase.h and Game/PlayerWeaponBase.h). */
#define VEHICLE_GET_ON_RIDER
#define PLAYER_MOTION_HAND_MATRIX

#include "Game/CharCommand.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/TGi.h"
#include "Game/TVehicleBase.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Vehicle::TVehicleBase;

namespace boost {
/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * emits the copy for vehicles (boost 1.32's definition). */
template <> inline void weak_ptr<TVehicleBase>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8007E58C {
    /* 0x000 */ u8 field_0x0[0xD0];
    /* 0x0D0 */ Vec3f m_prevPosition; /* (as in Game/PlayerBehaviorLightDash.cpp) */
    /* 0x0DC */ u8 field_0xDC[0xF4 - 0xDC];
    /* 0x0F4 */ Vec3f m_up;           /* (as in Game/PlayerBehaviorEdgeHang.cpp) */
};

/* The pair of floats at 0x44 of a player's record (see Game/SaveRecord.cpp). */
struct UnkFloatPair;

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Whether a value is about zero (see Game/PlayerBehaviorSomethingHang.cpp). */
bool fn_8000A950(float value);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* The player's up axis (see Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_80079698(Player::PlayerBase *player);
/* Point from to to bowed along up by t (behaviour inferred). */
Vec3f fn_800930AC(const Vec3f *from, const Vec3f *to, const Vec3f *up, float t);
/* Turns the player upright along up (behaviour inferred). */
void fn_8009339C(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *up);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Calls on the player's motion: a value handed to a vehicle, its flag 0, and its speed (meanings
 * inferred; see Game/PlayerBehaviorGrind.cpp and Game/PlayerBehaviorSliding.cpp). */
void *fn_800AA584(Player::PlayerMotion *motion);
void fn_800AA700(Player::PlayerMotion *motion, int arg);
void fn_800AA7C0(Player::PlayerMotion *motion, float value);
/* Entry index of the player's character slots at 0x254 (see Game/Player.h). */
boost::weak_ptr<CharInfo> fn_800AADA4(Player::UnkCharaSlots_800AADA4 *slots, int index);
/* Rumbles player's controller with pattern (see Game/PlayerBehaviorHomingJump.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);
/* A player's record: its vehicle and its gauge and the gauge's maximum (see
 * Game/SaveRecord.cpp). */
void fn_8016EED8(UnkPlayerInfo_80178B5C *info, s32 value);
void fn_8016F66C(UnkFloatPair *pair, float value);
void fn_8016F674(UnkFloatPair *pair, float value);
/* The vehicle's gauge, its maximum, its type, a flag, and taking or releasing a rider (behaviour
 * inferred; functions of Vehicle::TVehicleBase). */
float fn_801B64F8(TVehicleBase *vehicle);
float fn_801B6564(TVehicleBase *vehicle);
void fn_801B66AC(TVehicleBase *vehicle, bool on);
void fn_801B66F4(TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> *shooter);
void fn_801B6718(TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> *rider);
bool fn_801B67CC(TVehicleBase *vehicle, const boost::weak_ptr<CharInfo> *rider);
s32 fn_801B6938(TVehicleBase *vehicle);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
/* The point transformed (the other matrix routines are in Game/MathTypes.h). */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CE60(Vec3f *vector);                     /* vector = 0 */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
/* a turned towards b by t (Euler angles in axis order order; behaviour inferred; as in
 * Game/PlayerBehaviorLaunch.cpp). */
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8007E58C *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* fn_8040B034 (Game/MathTypes.h) builds a translation matrix through its first argument, as a
 * function returning a Mtx34f does; called as one, the matrix is a temporary, as in the game (as
 * in Game/PlayerBehaviorFall.cpp). */
typedef Mtx34f (*TranslationMtxFn)(const Vec3f *offset);
#define TranslationMtx(offset) (reinterpret_cast<TranslationMtxFn>(fn_8040B034)(offset))

/* Degrees to radians (name inferred; as in Game/PlayerBehaviorGrind.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* vector * scale (an inline wrapper, which orders the statement's work as the game does). */
inline Vec3f Scale(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* Uninitialized storage for a vector, built through the out-of-line vector constructor
 * fn_80008E1C (as in Game/BirdBase.cpp: it has no constructor, and its assignment operator makes it
 * non-POD so it is not zeroed). */
struct UnkVectorStorage {
    UnkVectorStorage &operator=(const UnkVectorStorage &other);

    float m_values[3];
};

#define NewVector(x, y, z) \
    fn_80008E1C((Vec3f *)&(const UnkVectorStorage &)UnkVectorStorage(), x, y, z)

/* A position copied through the game's out-of-line vector copy constructor (fn_800091EC; as
 * AimPosition in Game/PlayerWeaponBase.h; name invented). */
struct HandPosition {
    HandPosition(const Vec3f &position) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), &position);
    }

    float x;
    float y;
    float z;
};

/* Where a hand goes on a vehicle of type 7's controls, from the seat (struct name invented; its
 * constructor is an out-of-line copy at the end of the unit). */
struct HandPlacement {
    HandPlacement(const Vec3f &position, const Vec3f &rotation)
        : m_position(position), m_rotation(rotation) {}

    /* 0x00 */ HandPosition m_position;
    /* 0x0C */ Vec3f m_rotation;
};

namespace Player {
namespace Behavior {

/* The behavior's flag word (struct name unknown): bit 0 the vehicle lost, bit 1 the player got
 * on, bit 2 the vehicle drives the motion's speed (its slot 0x60). */
struct DriveVehicleFlags {
    DriveVehicleFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    void Clear(int bit) {
        fn_8006127C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }
    void Reset() {
        fn_80014E10(&m_bits);
    }

    u32 m_bits;
};

class DriveVehicle : public Behavior {
  public:
    DriveVehicle(const boost::shared_ptr<PlayerBase> &player);
    virtual ~DriveVehicle();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void UpdateRide();
    void GetSeat(Vec3f *position, Vec3f *rotation);
    void GetOn();
    void GetOff();
    void UpdateGetOn();

    static const char *s_name;

    /* 0x1C */ s32 m_mode;              /* 0 getting on, 1 riding */
    /* 0x20 */ DriveVehicleFlags m_flags;
    /* 0x24 */ float m_time;            /* since the move onto the seat began */
    /* 0x28 */ Vec3f m_startPosition;   /* where the move onto the seat began */
    /* 0x34 */ Vec3f m_startRotation;
    /* 0x40 */ boost::weak_ptr<TVehicleBase> m_vehicle;
};
SIZE_ASSERT(DriveVehicle, 0x48);

/* The two hands' places (names inferred). */
static HandPlacement s_rightHand(Vec3f(2.0f, -4.15f, 2.0f),
                                 Vec3f(DegToRad(180.0f), DegToRad(-35.0f), DegToRad(0.0f)));
static HandPlacement s_leftHand(Vec3f(-1.7f, -4.15f, -1.0f),
                                Vec3f(DegToRad(180.0f), DegToRad(10.0f), DegToRad(0.0f)));
static HandPlacement *s_hands[2] = {&s_rightHand, &s_leftHand};

const char *DriveVehicle::s_name = "DriveVehicle";

DriveVehicle::DriveVehicle(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0x1B), m_startRotation(0.0f, 0.0f, 0.0f) {}

DriveVehicle::~DriveVehicle() {}

bool DriveVehicle::vf_0x0C(int prevId, const InitializeArgs &args) {
    const InitializeArgsDriveVehicle &drive = static_cast<const InitializeArgsDriveVehicle &>(args);
    boost::shared_ptr<TVehicleBase> vehicle = drive.m_vehicle.lock();
    if (vehicle && vehicle->GetRider().expired()) {
        return true;
    }
    return false;
}

void DriveVehicle::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsDriveVehicle &drive = static_cast<const InitializeArgsDriveVehicle &>(args);
    m_flags.Reset();
    boost::shared_ptr<TVehicleBase> vehicle = drive.m_vehicle.lock();
    if (!vehicle) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
        return;
    }
    boost::weak_ptr<CharInfo> rider = fn_800AADA4(m_player.get()->m_charaSlots, 0);
    if (!fn_801B67CC(vehicle.get(), &rider)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
        return;
    }
    boost::weak_ptr<CharInfo> shooter = fn_800AADA4(m_player.get()->m_charaSlots, 1);
    fn_801B66F4(vehicle.get(), &shooter);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1F) &&
        m_player.get()->m_weapon->GetWeaponKind() == 5) {
        m_player.get()->m_weapon->DropWeapon();
    }
    m_vehicle = drive.m_vehicle;
    m_player.get()->m_vehicle = m_vehicle;
    fn_80076CA8(m_player.get()->m_stateBits, 0x10);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CA8(m_player.get()->m_stateBits, 0x31);
    fn_80076CA8(m_player.get()->m_stateBits, 0x38);
    fn_80076CA8(m_player.get()->m_stateBits, 0x43);
    fn_80076CA8(m_player.get()->m_stateBits, 0x40);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    fn_80014E1C(&m_player.get()->m_flags, 7);
    if (vehicle->vf_0x50()) {
        fn_80014E1C(&m_player.get()->m_flags, 8);
        fn_8006127C(&m_player.get()->m_flags, 7);
    }
    if (vehicle->vf_0x54()) {
        fn_80076CA8(m_player.get()->m_stateBits, 0x3A);
    }
    m_player.get()->m_motion->vf_0x10(0xC, 5.0f);
    fn_800AA700(m_player.get()->m_motion, 1);
    fn_8040CE60(&m_player.get()->field_0x18);
    m_time = 0.0f;
    fn_800091D0(&m_startPosition, &m_player.get()->m_position);
    m_startRotation = m_player->m_rotation;
    m_mode = 0;
    UnkPlayerInfo_80178B5C *info = fn_80178B5C(fn_80079808(), m_player.get()->field_0x54);
    if (info != 0) {
        fn_8016EED8(info, fn_801B6938(vehicle.get()));
        fn_8016F66C(reinterpret_cast<UnkFloatPair *>(&info->m_meter), fn_801B6564(vehicle.get()));
    }
}

void DriveVehicle::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0x10);
    fn_80076CC8(m_player.get()->m_stateBits, 0x31);
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 0x43);
    fn_80076CC8(m_player.get()->m_stateBits, 0x40);
    fn_8006127C(&m_player.get()->m_flags, 1);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_8006127C(&m_player.get()->m_flags, 8);
    fn_80076CC8(m_player.get()->m_stateBits, 0x3A);
    if (m_player.get()->m_motion != 0) {
        fn_800AA700(m_player.get()->m_motion, 1);
    }
    GetOff();
    fn_8016EED8(fn_80178B5C(fn_80079808(), m_player.get()->field_0x54), 0);
}

void DriveVehicle::vf_0x18(CharCommand *command) {
    boost::shared_ptr<TVehicleBase> vehicle = m_vehicle.lock();
    if (!vehicle) {
        return;
    }
    switch (command->TypeId()) {
    case 0x111:
    case 0x112:
        command->Flags() |= 2;
        break;
    case 1:
        if (m_flags.Test(1)) {
            vehicle->PushCommand(new AccelerateInputCommand(
                static_cast<AccelerateInputCommand *>(command)->m_direction, 4));
            command->Flags() |= 2;
        }
        break;
    case 0x15:
        if (m_flags.Test(1)) {
            vehicle->PushCommand(new ControlVehicleCommand(
                static_cast<ControlVehicleCommand *>(command)->m_action, 4));
        }
        command->Flags() |= 2;
        break;
    }
}

void DriveVehicle::Update(float dt) {
    m_time += dt;
    if (m_vehicle.expired()) {
        m_flags.Set(0);
    } else {
        boost::shared_ptr<TVehicleBase> vehicle = m_vehicle.lock();
        switch (m_mode) {
        case 0:
            UpdateGetOn();
            break;
        case 1:
            UpdateRide();
            break;
        }
        UnkPlayerInfo_80178B5C *info = fn_80178B5C(fn_80079808(), m_player.get()->field_0x54);
        if (info != 0) {
            fn_8016F674(reinterpret_cast<UnkFloatPair *>(&info->m_meter),
                        fn_801B64F8(vehicle.get()));
        }
        if (m_flags.Test(2)) {
            fn_800AA7C0(m_player.get()->m_motion, vehicle->vf_0x64());
        }
        fn_801B66AC(vehicle.get(), fn_8006CD78(m_player.get()->m_stateBits, 0x2A));
    }
    if (!fn_8000A950(dt)) {
        fn_800091D0(&m_player.px->field_0x18,
                    &(const Vec3f &)fn_8040CDA8(
                        &(const Vec3f &)Subtract(&m_player.get()->m_position,
                                                 &FieldsOf(m_player.get())->m_prevPosition),
                        1.0f / dt));
    }
    fn_8009339C(&m_player, &FieldsOf(m_player.get())->m_up);
    if (m_flags.Test(0)) {
        GetOff();
        fn_800A7F14(m_player.get()->m_behaviors,
                    &(const InitializeArgs &)InitializeArgsJump(fn_80079698(m_player.get())));
    }
}

void DriveVehicle::UpdateRide() {
    Vec3f position;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    GetSeat(&position, &rotation);
    fn_800091D0(&m_player.get()->m_position, &position);
    m_player->m_rotation = rotation;
}

} // namespace Behavior
} // namespace Player

/* The player's height (the word at 0x274; Game/PlayerBehaviorFall.cpp and
 * Game/PlayerBehaviorGrind.cpp call it too, so it keeps its fn_ name). */
extern "C" float fn_8007E514(Player::PlayerBase *player) {
    return player->field_0x274;
}

/* point transformed by matrix (Game/BirdBase.cpp calls it too, so it keeps its fn_ name). */
extern "C" Vec3f fn_8007E4E8(const Mtx34f *matrix, const Vec3f *point) {
    return fn_8040B67C(matrix, point);
}

namespace Player {
namespace Behavior {

/* The seat's position and rotation: the vehicle's seat raised by the player's height, or the
 * player's own place without a vehicle. On a vehicle of type 7 the hands are placed on the
 * controls and only the yaw is kept (name inferred). */
void DriveVehicle::GetSeat(Vec3f *position, Vec3f *rotation) {
    boost::shared_ptr<TVehicleBase> vehicle = m_vehicle.lock();
    if (vehicle) {
        vehicle->GetSeat(position, rotation);
        Mtx34f matrix;
        fn_8040C148(&matrix, &lbl_805E0720);
        fn_8040B848(&matrix, rotation->y);
        fn_8040B884(&matrix, rotation->x);
        fn_8040B80C(&matrix, rotation->z);
        AddVector(*position,
                  fn_8007E4E8(&matrix, NewVector(0.0f, fn_8007E514(m_player.operator->()), 0.0f)));
        switch (fn_801B6938(vehicle.get())) {
        case 7:
            if (m_flags.Test(1)) {
                Mtx34f seat;
                fn_8040C148(&seat, &(const Mtx34f &)TranslationMtx(position));
                fn_8040B848(&seat, rotation->y);
                fn_8040B884(&seat, rotation->x);
                fn_8040B80C(&seat, rotation->z);
                Mtx34f hand;
                fn_8040C148(&hand, &seat);
                fn_8040B59C(&seat, reinterpret_cast<const Vec3f *>(&s_hands[0]->m_position));
                fn_8040B848(&seat, s_hands[0]->m_rotation.y);
                fn_8040B884(&seat, s_hands[0]->m_rotation.x);
                fn_8040B80C(&seat, s_hands[0]->m_rotation.z);
                m_player.get()->m_motion->vf_0x24(2, &seat);
                fn_8040B59C(&hand, reinterpret_cast<const Vec3f *>(&s_hands[1]->m_position));
                fn_8040B848(&hand, s_hands[1]->m_rotation.y);
                fn_8040B884(&hand, s_hands[1]->m_rotation.x);
                fn_8040B80C(&hand, s_hands[1]->m_rotation.z);
                m_player.get()->m_motion->vf_0x24(3, &hand);
            }
            rotation->x = 0.0f;
            rotation->z = 0.0f;
            break;
        }
    } else {
        fn_800091D0(position, &m_player.get()->m_position);
        *rotation = m_player->m_rotation;
    }
}

void DriveVehicle::GetOn() {
    boost::shared_ptr<TVehicleBase> vehicle = m_vehicle.lock();
    if (vehicle) {
        vehicle->OnGetOn(m_player.get()->m_self);
        m_flags.Set(1);
        if (vehicle->vf_0x60()) {
            m_flags.Set(2);
        }
        if (m_player.get()->m_motion != 0) {
            vehicle->vf_0x7C(fn_800AA584(m_player.get()->m_motion));
        }
    }
    fn_80076CA8(m_player.get()->m_stateBits, 0x28);
}

void DriveVehicle::GetOff() {
    boost::shared_ptr<TVehicleBase> vehicle = m_vehicle.lock();
    if (vehicle) {
        vehicle->OnGetOff();
        boost::weak_ptr<CharInfo> rider = fn_800AADA4(m_player.get()->m_charaSlots, 0);
        fn_801B6718(vehicle.get(), &rider);
        vehicle->vf_0x7C(0);
        m_vehicle.reset();
    }
    m_player.get()->m_vehicle.reset();
    m_flags.Clear(1);
    m_flags.Clear(2);
    m_player.get()->m_motion->vf_0x20(2);
    m_player.get()->m_motion->vf_0x20(3);
    UnkPlayerInfo_80178B5C *info = fn_80178B5C(fn_80079808(), m_player.get()->field_0x54);
    if (info != 0) {
        fn_8016F674(reinterpret_cast<UnkFloatPair *>(&info->m_meter), 0.0f);
    }
    fn_80076CC8(m_player.get()->m_stateBits, 0x28);
}

/* Swings the player onto the seat over half a second, then rides (name inferred). */
void DriveVehicle::UpdateGetOn() {
    Vec3f position;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    GetSeat(&position, &rotation);
    if (m_time >= 0.5f) {
        m_time = 0.0f;
        UpdateRide();
        m_player.get()->m_motion->vf_0x10(0, 5.0f);
        GetOn();
        fn_8034DF48(6, m_player.get()->field_0x54);
        m_mode = 1;
    } else {
        float t = m_time / 0.5f;
        fn_800091D0(&m_player.get()->m_position,
                    &(const Vec3f &)fn_800930AC(
                        &m_startPosition, &position,
                        &(const Vec3f &)Scale(FieldsOf(m_player.px)->m_up, -100.0f), t));
        m_player->m_rotation = fn_8040F16C(&m_startRotation, &rotation, 2, t);
    }
}

Behavior *DriveVehicle::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new DriveVehicle(player);
}

void DriveVehicle::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds DriveVehicle by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AFB60 = {
    Player::Behavior::DriveVehicle::Create, Player::Behavior::DriveVehicle::Release, 0};

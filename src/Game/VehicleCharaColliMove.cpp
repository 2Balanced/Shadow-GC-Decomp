/* Vehicle::CharaColliMove (class name from RTTI): the task that keeps a vehicle on what it stands
 * on. Its state (a separately allocated Impl; class name unknown) records the contact the
 * vehicle's collision move reports, tells the character it rides on and the one it leaves with
 * RideCommand and LeaveCommand, and every frame carries the vehicle along with the moving ground
 * through a callback that takes a CharaColliMoveArgs (the argument type of the player's collision
 * move, named by the RTTI of the bound callback's type). It also measures how far the vehicle
 * moved. The vehicle points back at the task (TVehicleBase::field_0x7C). File, member and function
 * names are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define BOOST_SP_STATIC_POINTER_CAST
#define BOOST_SP_CONVERTING_ASSIGNMENT
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/MathTypes.h"
#include "Game/TVehicleBase.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::CharaColliBase;
using Vehicle::TVehicleBase;

extern "C" {
void fn_8040CE60(Vec3f *vector); /* sets the vector to zero */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
bool fn_8000A950(float value); /* whether the value is zero */
Vec3f fn_80016284(const Vec3f *vector); /* the negated vector */
Mtx34f fn_8040FFBC(const Mtx34f *matrix, const Vec3f *angles, int order);
Mtx34f fn_8040FEBC(const Mtx34f *matrix, const Vec3f *angles, int order);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
}

/* A collision object of the world (see Game/VehicleColliMove.cpp). */
class UnkColliObject_80028CA4;

/* What a mover stands on (see Game/VehicleColliMove.cpp, whose unit holds its functions; the
 * assignment is a weak function of the player's collision move, 0x800943D4). */
struct UnkColliMoveContact_803334A4 {
    UnkColliMoveContact_803334A4();
    ~UnkColliMoveContact_803334A4();

    bool IsValid() const;

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x0C */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x14 */ UnkColliObject_80028CA4 *m_object;
};
SIZE_ASSERT(UnkColliMoveContact_803334A4, 0x18);

bool operator!=(const UnkColliMoveContact_803334A4 &a, const UnkColliMoveContact_803334A4 &b);

/* The placement taken from a contact (see Game/VehicleColliMove.cpp). Its constructor and
 * assignment are functions of the player's collision move (0x80094F50, 0x80094CC0). */
struct UnkColliMovePlacement_80333098 {
    UnkColliMovePlacement_80333098();
    UnkColliMovePlacement_80333098 &operator=(const UnkColliMovePlacement_80333098 &from);

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ u32 m_kind;
};
SIZE_ASSERT(UnkColliMovePlacement_80333098, 0x1C);

extern "C" void fn_80333098(UnkColliMovePlacement_80333098 *placement,
                            const UnkColliMoveContact_803334A4 *contact);

/* CharaColliMoveArgs (class name from RTTI): what the collision move hands its callback: the
 * contact, the moved character, and where the ground is now and was before (names inferred). */
struct CharaColliMoveArgs {
    CharaColliMoveArgs();
    ~CharaColliMoveArgs();

    /* 0x00 */ UnkColliMoveContact_803334A4 *m_contact;
    /* 0x04 */ boost::shared_ptr<CharInfo> m_chara;
    /* 0x0C */ UnkColliMovePlacement_80333098 m_current;
    /* 0x28 */ UnkColliMovePlacement_80333098 m_previous;
};
SIZE_ASSERT(CharaColliMoveArgs, 0x44);

typedef boost::function1<void, CharaColliMoveArgs &> CharaColliMoveCallback;

/* The callback holder of the collision move (class name unknown; its functions belong to the
 * player's collision move: constructor 0x80094F64, destructor 0x80094DBC, assignment 0x80094738,
 * test 0x80094D14 and call 0x80094C94; names inferred). */
struct UnkColliMoveCallback_80094F64 {
    UnkColliMoveCallback_80094F64();
    ~UnkColliMoveCallback_80094F64();
    UnkColliMoveCallback_80094F64 &operator=(const CharaColliMoveCallback &function);
    bool IsSet() const;
    void Call(CharaColliMoveArgs &args);

    /* 0x00 */ CharaColliMoveCallback m_function;
};
SIZE_ASSERT(UnkColliMoveCallback_80094F64, 0xC);

/* The commands a vehicle sends to the character it gets on and the one it leaves (class names
 * from RTTI; types 0xB and 0xC). The vehicle passes itself and its position. */
class RideCommand : public CharCommand {
  public:
    RideCommand(const boost::weak_ptr<CharInfo> &vehicle, const Vec3f &position);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_vehicle;
    /* 0x1C */ Vec3f m_position;
};
SIZE_ASSERT(RideCommand, 0x28);

class LeaveCommand : public CharCommand {
  public:
    LeaveCommand(const boost::weak_ptr<CharInfo> &vehicle, const Vec3f &position);

    /* 0x14 */ boost::weak_ptr<CharInfo> m_vehicle;
    /* 0x1C */ Vec3f m_position;
};
SIZE_ASSERT(LeaveCommand, 0x28);

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A vector returned through the out-of-line copy constructor (fn_800091EC; as in
 * Game/VehicleColliMove.cpp). */
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

namespace Vehicle {

class CharaColliMove : public Task {
  public:
    class Impl;

    CharaColliMove(Task *parent, const boost::shared_ptr<TVehicleBase> &vehicle);
    virtual ~CharaColliMove();
    virtual void Tick(float dt);

    void Init();
    void SetCallback(const CharaColliMoveCallback &callback);
    void SetContact(const UnkColliMoveContact_803334A4 &contact);
    void ClearContact();
    const UnkColliMoveContact_803334A4 *GetContact() const;
    UnkCopyVector3_800091EC GetVelocity() const;

    /* 0x28 */ boost::scoped_ptr<Impl> m_impl;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(CharaColliMove, 0x30);

class CharaColliMove::Impl {
  public:
    Impl(const boost::shared_ptr<TVehicleBase> &vehicle);
    ~Impl();
    void Update();

    /* 0x00 */ boost::shared_ptr<TVehicleBase> m_vehicle;
    /* 0x08 */ UnkColliMoveCallback_80094F64 m_callback;
    /* 0x14 */ UnkColliMovePlacement_80333098 m_placement;
    /* 0x30 */ CharaColliMoveArgs m_args;
    /* 0x74 */ UnkColliMoveContact_803334A4 m_contact;
    /* 0x8C */ Vec3f m_moved;    /* how far the vehicle moved this frame */
    /* 0x98 */ Vec3f m_velocity; /* m_moved over the frame time */
};
SIZE_ASSERT(CharaColliMove::Impl, 0xA4);

} // namespace Vehicle

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(Vehicle::CharaColliMove::Impl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<Impl> out of line (0x80335374) and ~Impl at its own place
 * (0x80335514): the specialization calls an inline helper (as in Game/BossCtrl.cpp; see
 * docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<Vehicle::CharaColliMove::Impl>(Vehicle::CharaColliMove::Impl *x) {
    DeleteImpl(x);
}
} // namespace boost

namespace Vehicle {

/* Carries the vehicle along with the ground it stands on: moves it by the ground's change of
 * position and rotation, turns it by the ground's turn about Y, and records its new position
 * (name inferred). */
static void FollowGround(CharaColliMoveArgs &args) {
    Mtx34f matrix;
    Mtx34f translation;
    fn_8040B034(&translation, &args.m_current.m_position);
    fn_8040C148(&matrix, &translation);
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FFBC(&matrix, &args.m_current.m_rotation, args.m_current.m_kind));
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FEBC(&matrix, &args.m_previous.m_rotation, args.m_previous.m_kind));
    fn_8040B59C(&matrix, &(const Vec3f &)fn_80016284(&args.m_previous.m_position));
    const Vec3f &position = fn_8040B67C(&matrix, &args.m_chara->m_position);
    fn_800091D0(&args.m_chara->m_position, &position);
    args.m_chara->m_rotation.y += args.m_current.m_rotation.y - args.m_previous.m_rotation.y;
    boost::shared_ptr<TVehicleBase> vehicle = boost::static_pointer_cast<TVehicleBase>(args.m_chara);
    fn_800091D0(reinterpret_cast<Vec3f *>(vehicle->field_0x64), &args.m_chara->m_position);
}

CharaColliMove::Impl::Impl(const boost::shared_ptr<TVehicleBase> &vehicle) : m_vehicle(vehicle) {
    m_args.m_chara = m_vehicle;
    fn_8040CE60(&m_moved);
    fn_8040CE60(&m_velocity);
}

CharaColliMove::Impl::~Impl() {}

void CharaColliMove::Impl::Update() {
    fn_8040CE60(&m_moved);
    fn_8040CE60(&m_velocity);
    if (m_contact.IsValid()) {
        Vec3f previous;
        fn_800091EC(&previous, &m_vehicle->m_position);
        if (m_callback.IsSet()) {
            m_args.m_contact = &m_contact;
            m_args.m_previous = m_placement;
            fn_80333098(&m_args.m_current, &m_contact);
            m_placement = m_args.m_current;
            m_callback.Call(m_args);
        }
        Vec3f moved;
        fn_8040CD74(&moved, &m_vehicle->m_position, &previous);
        AssignVector(m_moved, moved);
        /* The word at CharInfo 0x60 points to the frame time (meaning inferred). */
        if (!fn_8000A950(**reinterpret_cast<float **>(m_vehicle->field_0x60))) {
            fn_800091D0(&m_velocity,
                        &(const Vec3f &)fn_8040CDA8(&m_moved, 1.0f / **reinterpret_cast<float **>(m_vehicle->field_0x60)));
        }
    }
}

CharaColliMove::CharaColliMove(Task *parent, const boost::shared_ptr<TVehicleBase> &vehicle)
    : Task(parent), m_impl(new Impl(vehicle)) {
    m_name = "CharaColliMove";
    m_impl->m_vehicle->field_0x7C = reinterpret_cast<u32>(this);
    Init();
}

CharaColliMove::~CharaColliMove() {
    m_impl->m_vehicle->field_0x7C = 0;
}

void CharaColliMove::Tick(float dt) {
    m_impl->Update();
}

void CharaColliMove::SetCallback(const CharaColliMoveCallback &callback) {
    m_impl->m_callback = callback;
}

void CharaColliMove::Init() {
    m_impl->m_callback = boost::bind(&FollowGround, _1);
}

void CharaColliMove::SetContact(const UnkColliMoveContact_803334A4 &contact) {
    if (m_impl->m_contact.m_chara.lock() == contact.m_chara.lock()) {
        if (m_impl->m_contact != contact) {
            m_impl->m_contact = contact;
            if (!m_impl->m_contact.m_chara.expired()) {
                fn_80333098(&m_impl->m_placement, &m_impl->m_contact);
            }
        }
    } else {
        if (!m_impl->m_contact.m_chara.expired()) {
            boost::shared_ptr<CharInfo> chara = m_impl->m_contact.m_chara.lock();
            chara->PushCommand(new LeaveCommand(m_impl->m_vehicle, m_impl->m_vehicle->m_position));
        }
        m_impl->m_contact = contact;
        if (!m_impl->m_contact.m_chara.expired()) {
            boost::shared_ptr<CharInfo> chara = m_impl->m_contact.m_chara.lock();
            chara->PushCommand(new RideCommand(m_impl->m_vehicle, m_impl->m_vehicle->m_position));
            fn_80333098(&m_impl->m_placement, &m_impl->m_contact);
        }
    }
}

void CharaColliMove::ClearContact() {
    SetContact(UnkColliMoveContact_803334A4());
}

const UnkColliMoveContact_803334A4 *CharaColliMove::GetContact() const {
    return &m_impl->m_contact;
}

UnkCopyVector3_800091EC CharaColliMove::GetVelocity() const {
    return AsCopy(m_impl->m_velocity);
}

} // namespace Vehicle

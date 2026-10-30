/* Player::CharaColliMove (class name from RTTI): the task that keeps a player on what it stands
 * on, the model of Vehicle::CharaColliMove (Game/VehicleCharaColliMove.cpp). Its state (a
 * separately allocated Impl; class name unknown) records the contact the player's collision
 * reports, tells the character it rides on and the one it leaves with RideCommand and
 * LeaveCommand, and every frame carries the player along with the moving ground through a callback
 * that takes a CharaColliMoveArgs (class name from the RTTI of the bound callback's type); the
 * default callback then lets the player's collision (fn_8009728C) push it out of the ground,
 * ignoring the collider it stands on. It also measures how far the player moved. The player
 * points back at the task (word 0x228). File, member and function names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#define BOOST_SP_STATIC_POINTER_CAST
#define BOOST_SP_CONVERTING_ASSIGNMENT
/* boost::function's operator() puts a bad_function_call on the stack (see the MSL's stdexcept). */
#define MSL_RUNTIME_ERROR_STRING
#include <list>

#include "Game/UnkWords_8007A4D4.h"
#include "boost/shared_ptr.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/Task.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* The game's one shared std::swap for boost::function's any_pointer (0x80019164), which
 * function1::swap calls (an overload stands in for it, as in Game/CatchStick.cpp). */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);
} // namespace std

using Player::PlayerBase;
using SonicteamUSA::System::CharaColli::CharaColliBase;

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
namespace SonicteamUSA {
namespace System {
namespace Range {
class RangeBase;
} // namespace Range
} // namespace System
} // namespace SonicteamUSA

/* What a mover stands on (see Game/VehicleColliMove.cpp, whose unit holds its constructor,
 * destructor and IsValid; the assignment the compiler writes is emitted here). */
struct UnkColliMoveContact_803334A4 {
    UnkColliMoveContact_803334A4();
    ~UnkColliMoveContact_803334A4();

    bool IsValid() const;

    /* 0x00 */ u8 m_isCollider;
    /* 0x04 */ boost::weak_ptr<CharInfo> m_chara;
    /* 0x0C */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x14 */ SonicteamUSA::System::Range::RangeBase *m_object;
};
SIZE_ASSERT(UnkColliMoveContact_803334A4, 0x18);

bool operator!=(const UnkColliMoveContact_803334A4 &a, const UnkColliMoveContact_803334A4 &b);

/* vector = value through the out-of-line assignment (see Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A vector assigned through the out-of-line assignment (fn_800091D0); the compiler's assignment
 * of a struct holding one calls it (struct name unknown). */
struct UnkAssignVector3_800091D0 {
    UnkAssignVector3_800091D0 &operator=(const UnkAssignVector3_800091D0 &v) {
        fn_800091D0(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
        return *this;
    }

    Vec3f *AsVector() {
        return reinterpret_cast<Vec3f *>(this);
    }
    const Vec3f *AsVector() const {
        return reinterpret_cast<const Vec3f *>(this);
    }

    float x;
    float y;
    float z;
};

/* The placement taken from a contact (see Game/VehicleColliMove.cpp). */
struct UnkColliMovePlacement_80333098 {
    UnkColliMovePlacement_80333098() : m_rotation(0.0f, 0.0f, 0.0f) {}

    /* 0x00 */ UnkAssignVector3_800091D0 m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ u32 m_kind;
};
SIZE_ASSERT(UnkColliMovePlacement_80333098, 0x1C);

extern "C" void fn_80333098(UnkColliMovePlacement_80333098 *placement,
                            const UnkColliMoveContact_803334A4 *contact);

/* CharaColliMoveArgs (class name from RTTI): what the collision move hands its callback: the
 * contact, the moved character, and where the ground is now and was before (names inferred). */
struct CharaColliMoveArgs {
    CharaColliMoveArgs() : m_contact(0) {}

    /* 0x00 */ UnkColliMoveContact_803334A4 *m_contact;
    /* 0x04 */ boost::shared_ptr<CharInfo> m_chara;
    /* 0x0C */ UnkColliMovePlacement_80333098 m_current;
    /* 0x28 */ UnkColliMovePlacement_80333098 m_previous;
};
SIZE_ASSERT(CharaColliMoveArgs, 0x44);

typedef boost::function1<void, CharaColliMoveArgs &> CharaColliMoveCallback;

/* The callback holder of the collision move (class name unknown; names inferred). */
struct UnkColliMoveCallback_80094F64 {
    UnkColliMoveCallback_80094F64 &operator=(const CharaColliMoveCallback &function) {
        m_function = function;
        return *this;
    }
    bool IsSet() const {
        return HasFunction();
    }
    bool HasFunction() const {
        return !m_function.empty();
    }
    void Call(CharaColliMoveArgs &args) {
        if (!m_function.empty()) {
            m_function(args);
        }
    }

    /* 0x00 */ CharaColliMoveCallback m_function;
};
SIZE_ASSERT(UnkColliMoveCallback_80094F64, 0xC);

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_80094088 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ UnkWords_8007A4D4 m_stateBits;
    /* 0x0B4 */ u8 field_0xB4[0xD0 - 0xB4];
    /* 0x0D0 */ Vec3f m_lastPosition;
    /* 0x0DC */ u8 field_0xDC[0x228 - 0xDC];
    /* 0x228 */ u32 m_colliMove;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_80094088 *>(player)

typedef std::list<boost::shared_ptr<CharaColliBase> > ColliderList;

/* The player's collision against the world, skipping the colliders in ignore (behaviour
 * inferred; see Game/PlayerBehaviorWallJump.cpp). */
extern "C" void fn_8009728C(boost::shared_ptr<PlayerBase> *player, ColliderList *ignore);

/* A vector returned through the out-of-line copy constructor (fn_800091EC; as in
 * Game/VehicleCharaColliMove.cpp). */
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

namespace Player {

class CharaColliMove : public Task {
  public:
    class Impl;

    CharaColliMove(Task *parent, const boost::shared_ptr<PlayerBase> &player);
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
    Impl(const boost::shared_ptr<PlayerBase> &player);
    ~Impl();
    void Update();

    /* 0x00 */ boost::shared_ptr<PlayerBase> m_player;
    /* 0x08 */ UnkColliMoveCallback_80094F64 m_callback;
    /* 0x14 */ UnkColliMovePlacement_80333098 m_placement;
    /* 0x30 */ CharaColliMoveArgs m_args;
    /* 0x74 */ UnkColliMoveContact_803334A4 m_contact;
    /* 0x8C */ Vec3f m_moved;    /* how far the player moved this frame */
    /* 0x98 */ Vec3f m_velocity; /* m_moved over the frame time */
};
SIZE_ASSERT(CharaColliMove::Impl, 0xA4);

} // namespace Player

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(Player::CharaColliMove::Impl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<Impl> out of line and ~Impl at its own place: the
 * specialization calls an inline helper (as in Game/VehicleCharaColliMove.cpp; see
 * docs/research-notes.md). */
namespace boost {
template <> inline void checked_delete<Player::CharaColliMove::Impl>(Player::CharaColliMove::Impl *x) {
    DeleteImpl(x);
}
} // namespace boost

namespace Player {

/* Carries the player along with the ground it stands on: moves it by the ground's change of
 * position and rotation and turns it by the ground's turn about Y, then lets its collision push it
 * out of the ground (ignoring the collider it stands on) and records its new position (name
 * inferred). */
static void FollowGround(CharaColliMoveArgs &args) {
    Mtx34f matrix;
    Mtx34f translation;
    fn_8040B034(&translation, args.m_current.m_position.AsVector());
    fn_8040C148(&matrix, &translation);
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FFBC(&matrix, &args.m_current.m_rotation, args.m_current.m_kind));
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040FEBC(&matrix, &args.m_previous.m_rotation, args.m_previous.m_kind));
    fn_8040B59C(&matrix, &(const Vec3f &)fn_80016284(args.m_previous.m_position.AsVector()));
    const Vec3f &position = fn_8040B67C(&matrix, &args.m_chara->m_position);
    fn_800091D0(&args.m_chara->m_position, &position);
    args.m_chara->m_rotation.y += args.m_current.m_rotation.y - args.m_previous.m_rotation.y;
    boost::shared_ptr<PlayerBase> player = boost::static_pointer_cast<PlayerBase>(args.m_chara);
    /* Never runs (no code): it names the state bits' assignment before their copy constructor,
     * which makes the compiler emit the two in retail's order (copy constructor first; inferred
     * from the order of the weak functions, as the unused functions of docs/research-notes.md fix
     * the order of constants). */
    if (0) {
        Fields(player.get())->m_stateBits = Fields(player.get())->m_stateBits;
    }
    UnkWords_8007A4D4 bits = Fields(player.get())->m_stateBits;
    ColliderList ignore;
    if (args.m_contact != 0 && args.m_contact->IsValid() && !args.m_contact->m_collider.expired()) {
        ignore.push_back(args.m_contact->m_collider.lock());
    }
    fn_8009728C(&player, &ignore);
    fn_800091D0(&Fields(player.get())->m_lastPosition, &args.m_chara->m_position);
    Fields(player.get())->m_stateBits = bits;
}

CharaColliMove::Impl::Impl(const boost::shared_ptr<PlayerBase> &player) : m_player(player) {
    m_args.m_chara = m_player;
    fn_8040CE60(&m_moved);
    fn_8040CE60(&m_velocity);
}

CharaColliMove::Impl::~Impl() {}

void CharaColliMove::Impl::Update() {
    fn_8040CE60(&m_moved);
    fn_8040CE60(&m_velocity);
    if (m_contact.IsValid()) {
        Vec3f previous;
        fn_800091EC(&previous, &m_player->m_position);
        if (m_callback.IsSet()) {
            m_args.m_contact = &m_contact;
            m_args.m_previous = m_placement;
            fn_80333098(&m_args.m_current, &m_contact);
            m_placement = m_args.m_current;
            m_callback.Call(m_args);
        }
        Vec3f moved;
        fn_8040CD74(&moved, &m_player->m_position, &previous);
        fn_800091D0(&m_moved, &moved);
        /* The word at CharInfo 0x60 points to the frame time (meaning inferred). */
        if (!fn_8000A950(**reinterpret_cast<float **>(m_player->field_0x60))) {
            fn_800091D0(&m_velocity,
                        &(const Vec3f &)fn_8040CDA8(&m_moved, 1.0f / **reinterpret_cast<float **>(m_player->field_0x60)));
        }
    }
}

CharaColliMove::CharaColliMove(Task *parent, const boost::shared_ptr<PlayerBase> &player)
    : Task(parent), m_impl(new Impl(player)) {
    m_name = "CharaColliMove";
    Fields(m_impl->m_player.get())->m_colliMove = reinterpret_cast<u32>(this);
    Init();
}

CharaColliMove::~CharaColliMove() {
    Fields(m_impl->m_player.get())->m_colliMove = 0;
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
            chara->PushCommand(new LeaveCommand(m_impl->m_player, m_impl->m_player->m_position));
        }
        m_impl->m_contact = contact;
        if (!m_impl->m_contact.m_chara.expired()) {
            boost::shared_ptr<CharInfo> chara = m_impl->m_contact.m_chara.lock();
            chara->PushCommand(new RideCommand(m_impl->m_player, m_impl->m_player->m_position));
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

} // namespace Player

/* Gives a player its collision move task (name inferred). */
extern "C" void fn_80094088(Task *parent, const boost::shared_ptr<PlayerBase> &player) {
    new Player::CharaColliMove(parent, player);
}

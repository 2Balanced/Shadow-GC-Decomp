/* Player::Shadow::PlayerTarget (class name from RTTI): Shadow's aim. Each frame its state (class
 * name unknown) turns the aim direction towards the stick's turn, searches the target manager for
 * the target in front of Shadow (and, with a lock-on weapon, for the target the weapon locks on
 * to), and eases the aim towards it. fn_800B66A0 creates it for Game/PlayerShadow.cpp. The unit
 * also emits the stage table's accessor fn_800B74B4, which many units call. File name from the
 * class; other names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
/* Enables shared_ptr::reset(Y *) (see boost/shared_ptr.hpp). */
#define BOOST_SP_RESET_POINTER
#define PLAYER_TARGET_BOOL_SLOT_10
#define PLAYER_TARGET_SLOT_18_ARGS
#include "Game/MathConstants.h"
#include "Game/Player.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/StageTable.h"
#include "Game/TargetSearchList.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::PlayerBase;

/* Target (class name from RTTI): a point other objects can aim at (as in
 * Game/PlayerShadowWeaponGun.cpp); the word at 0x30 holds its flags. */
class Target {
  public:
    virtual Vec3f GetPosition(const Vec3f &from);
    virtual ~Target();

    void SetFlags(u32 flags) {
        m_flags = flags;
    }

    /* 0x04 */ u8 field_0x4[0x30 - 0x4];
    /* 0x30 */ u32 m_flags;
};

/* The object fn_800B74B4 returns (class unknown; see Game/StageTable.h). */
class UnkObject_800B74B4 {
    /* 0x0 */ u32 field_0x0;
};

/* The game state (struct name unknown; as far as this unit reads it). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ s32 m_stageId; /* (name inferred) */
};
extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The pointer to the stage table's object (in another unit's .sbss). */
extern UnkObject_800B74B4 *lbl_805EE01C;

/* A constant vector (as in Game/BlackBull0412ModeCtrlUnit.cpp). */
extern const Vec3f lbl_805E07B8;

namespace std {
float cos(float x);
}

extern "C" {
/* A bit array: test bit. */
bool fn_8006CD78(const u32 *bits, int bit);
/* A 32-bit flag word: construct cleared, clear, set bit, clear bit, test bit (see
 * Game/ScatteringRing.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(const u32 *flags, int bit);
/* The larger of two values (behaviour inferred). */
const float &fn_80070494(const float &a, const float &b);
/* The time (as in Game/PlayerNpcTarget.cpp; read around the search, results unused). */
u64 fn_80051A94(void);
/* Whether a motion is in a pose that holds the aim (meaning unknown). */
bool fn_800AA640(Player::PlayerMotion *motion);
/* The player's own target (Game/PlayerTargetBase.cpp). */
weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *aim);
/* Narrowing a search list: drops the targets of a character (behaviour inferred). */
void fn_801E81B0(TargetSearchList *list, CharInfo *character);
/* A direction turned into a player's frame and back (behaviour inferred). */
Vec3f fn_80079628(PlayerBase *player, const Vec3f *direction);
Vec3f fn_80079508(PlayerBase *player, const Vec3f *point);
/* Vector helpers (behaviour inferred): turned about x and y, scaled, sum, dot and cross
 * products, normalized, normalizing in place, a point transformed. */
Vec3f fn_8040D118(const Vec3f *vector, float angle);
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_800091A4(const Vec3f *vector, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *vector);
float fn_8040CE74(Vec3f *vector);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* A rotation about axis, and a direction's angles (behaviour inferred). */
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
Vec2f fn_8040F760(const Vec3f &direction);
Vec3f fn_800576CC(const Vec2f &angles);
}

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does
 * (as in Game/NpcControlFlyType.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* The player fields this unit reads (names inferred). */
struct UnkPlayerFields_800B66A0 {
    /* 0x000 */ Vec3f m_position;
    /* 0x00C */ Vec3f m_rotation;
    /* 0x018 */ Vec3f m_front;
    /* 0x024 */ u8 field_0x24[0x198 - 0x24];
    /* 0x198 */ Mtx34f m_matrix;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_800B66A0 *>(player)

/* The frame time a player points at (the word at 0x60). */
#define FrameTime(player) (**reinterpret_cast<float **>(reinterpret_cast<u8 *>(player) + 0x60))

/* Vector assignment and copy construction through the game's out-of-line copies. Macros rather
 * than inline wrappers: an inline call in an inline call's argument is called out of line. */
#define Assign(to, from) fn_800091D0(&(to), &(const Vec3f &)(from))
#define Construct(to, from) fn_800091EC(&(to), &(const Vec3f &)(from))

/* A point transformed by a matrix, and a sum (names inferred): an operator-> in their arguments
 * is called out of line, as retail does. */
inline Vec3f Transform(const Mtx34f &matrix, const Vec3f &point) {
    return fn_8040B67C(&matrix, &point);
}

inline Vec3f Add(const Vec3f &a, const Vec3f &b) {
    return fn_8040CDD0(&a, &b);
}

/* The stage table's object (name inferred). */
static UnkObject_800B74B4 s_stageTable;

namespace Player {
namespace Shadow {

/* PlayerTarget (class name from RTTI; vtable 0x80522354): Shadow's aim. Its state lives in an
 * Impl behind a boost::scoped_ptr (as Player::Npc::NpcTarget's, Game/NpcTarget.h). */
class PlayerTarget : public PlayerTargetBase {
  public:
    class Impl;

    PlayerTarget(const shared_ptr<PlayerBase> &player);
    virtual ~PlayerTarget();
    virtual void vf_0x0C();
    virtual bool vf_0x10();
    virtual bool HasTarget();
    virtual void vf_0x18(const Vec3f &position, float time);
    virtual AimPosition vf_0x1C();
    virtual AimPosition GetTargetPosition();
    virtual weak_ptr<Target> GetTarget();
    virtual weak_ptr<Target> vf_0x28();
    virtual Vec3f ToAimSpace(const Vec3f &vector);

    /* 0x18 */ boost::scoped_ptr<Impl> m_impl;
};
SIZE_ASSERT(PlayerTarget, 0x1C);

/* The aim's flag word (struct name unknown; bit 0: a target is locked on, bit 1: the aim is on
 * a target). */
struct TargetFlags {
    TargetFlags() {
        fn_80014E04(&m_bits);
    }
    void Clear() {
        fn_80014E10(&m_bits);
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

    u32 m_bits;
};

/* The aim's state (class name unknown; size from its allocation). */
class PlayerTarget::Impl {
  public:
    Impl();
    ~Impl();
    void Init(PlayerTargetBase *owner, const shared_ptr<PlayerBase> &player);
    void Update();
    void LookAt(const Vec3f &position, float time);
    AimPosition GetAimPosition();
    AimPosition GetTargetPosition();
    Vec3f ToWorld(const Vec3f &vector);
    Vec3f TurnTo(const Vec3f &position);
    void Search();

    /* 0x00 */ TargetFlags m_flags;
    /* 0x04 */ float m_lookTime;   /* how long the aim stays on m_lookAt */
    /* 0x08 */ Vec3f m_lookAt;     /* the position vf_0x18 gives */
    /* 0x14 */ Vec3f m_aim;        /* the point aimed at */
    /* 0x20 */ Vec3f m_target;     /* the point the aim turns to */
    /* 0x2C */ Vec3f m_direction;  /* the aim direction in the player's frame */
    /* 0x38 */ Vec3f m_angles;     /* its angles */
    /* 0x44 */ weak_ptr<Target> m_lockOn;
    /* 0x4C */ weak_ptr<Target> m_nearest;
    /* 0x54 */ float m_holdTime;   /* how long the aim stays on m_target */
    /* 0x58 */ PlayerTargetBase *m_owner;
    /* 0x5C */ shared_ptr<PlayerBase> m_player;
};
SIZE_ASSERT(PlayerTarget::Impl, 0x64);

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteImpl(PlayerTarget::Impl *impl) {
    delete impl;
}

} // namespace Shadow
} // namespace Player

/* Retail keeps boost::checked_delete<PlayerTarget::Impl> out of line (0x800B69C0) and ~Impl at
 * its own place (0x800B74D0). As in Game/PlayerNpcTarget.cpp, this specialization calls an inline
 * helper, which gives retail's code and order under GC/2.7 (see docs/research-notes.md). */
namespace boost {
template<> inline void checked_delete<Player::Shadow::PlayerTarget::Impl>(
    Player::Shadow::PlayerTarget::Impl *x) {
    Player::Shadow::DeleteImpl(x);
}
} // namespace boost

namespace Player {
namespace Shadow {

PlayerTarget::Impl::Impl() : m_angles(0.0f, 0.0f, 0.0f) {
    m_flags.Clear();
    m_lookTime = 0.0f;
    m_holdTime = 0.0f;
    Assign(m_direction, Vec3f(0.0f, 0.0f, -1.0f));
    m_angles = lbl_805E07B8;
}

PlayerTarget::Impl::~Impl() {}

} // namespace Shadow
} // namespace Player

extern "C" UnkObject_800B74B4 *fn_800B74B4(void) {
    if (lbl_805EE01C == 0) {
        lbl_805EE01C = &s_stageTable;
    }
    return lbl_805EE01C;
}

namespace Player {
namespace Shadow {

void PlayerTarget::Impl::Init(PlayerTargetBase *owner, const shared_ptr<PlayerBase> &player) {
    m_owner = owner;
    m_player = player;
    shared_ptr<Target> target = fn_80206574(m_owner).lock();
    if (target) {
        u32 flags = target->m_flags;
        flags |= 0x90080;
        UnkStageInfo_804C4A18 *stage = fn_80176C3C(fn_800B74B4(), lbl_8057D6A0.m_stageId);
        u32 stageFlags = stage->m_flags;
        if (stageFlags & 1) {
            flags |= 0x2C00;
        }
        if (stageFlags & 4) {
            flags |= 0x2C00;
        }
        target->SetFlags(flags);
    }
}

void PlayerTarget::Impl::Update() {
    float dt = FrameTime(m_player.get());
    if (m_lookTime > 0.0f) {
        m_lookTime = fn_80070494(m_lookTime - dt, 0.0f);
    }
    if (m_holdTime > 0.0f) {
        m_holdTime = fn_80070494(m_holdTime - dt, 0.0f);
    }
    fn_80051A94();
    Search();
    fn_80051A94();
    if (fn_800AA640(m_player.get()->m_motion)) {
        m_holdTime = 1.0f;
    }
    if (m_holdTime > 0.0f) {
        Assign(m_aim, m_target);
        m_flags.Set(1);
    } else if (m_lookTime > 0.0f) {
        Assign(m_aim, m_lookAt);
        m_flags.Set(1);
    }
}

void PlayerTarget::Impl::LookAt(const Vec3f &position, float time) {
    Assign(m_lookAt, position);
    m_lookTime = time;
}

AimPosition PlayerTarget::Impl::GetAimPosition() {
    return m_aim;
}

AimPosition PlayerTarget::Impl::GetTargetPosition() {
    return m_target;
}

Vec3f PlayerTarget::Impl::ToWorld(const Vec3f &vector) {
    if (fn_8006CD78(m_player->m_stateBits, 0x40)) {
        return fn_80079628(m_player.get(), &vector);
    }
    return Transform(Fields(m_player.operator->())->m_matrix,
                     fn_8040D0AC(&vector, Fields(m_player.operator->())->m_rotation.y));
}

Vec3f PlayerTarget::Impl::TurnTo(const Vec3f &position) {
    Vec3f current;
    Construct(current, fn_80079628(m_player.get(), &m_direction));
    Vec3f to;
    Construct(to, Subtract(&position, &Fields(m_player.get())->m_position));
    float step;
    float distance = fn_8040CF50(&to);
    fn_8040CE74(&to);
    float frameTime = FrameTime(m_player.get());
    step = 360.0f * frameTime * (lbl_805FAD38 / 180.0f);
    float cosStep = std::cos(step);
    if (fn_8040CF90(&current, &to) < cosStep) {
        Vec3f axis;
        Construct(axis, fn_8040CEE8(&fn_8040CFB8(&current, &to)));
        Mtx34f rotation;
        fn_8040C148(&rotation, &(const Mtx34f &)fn_8040FD8C(&axis, step));
        Assign(to, fn_8040B67C(&rotation, &current));
    }
    Assign(m_direction, fn_80079508(m_player.px, &fn_8040CDD0(&to, &m_player->m_position)));
    m_angles = fn_800576CC(fn_8040F760(m_direction));
    return Add(Fields(m_player.operator->())->m_position, fn_800091A4(&to, distance));
}

void PlayerTarget::Impl::Search() {
    Vec3f direction;
    Construct(direction, Vec3f(0.0f, 0.0f, -1.0f));
    Assign(direction, fn_8040D0AC(&direction, m_owner->GetTurn()));
    Assign(direction, ToWorld(direction));
    m_lockOn.reset();
    shared_ptr<TargetSearchList> list;
    list.reset(fn_801E7930(fn_80060058()));
    fn_801E7FB4(list.get(), &m_player->m_position, 0.0f, &direction);
    fn_801E7E48(list.get(), fn_80206574(m_owner));
    fn_801E81B0(list.get(), m_player.get());
    m_nearest = fn_801E848C(list.get(), &m_player->m_position);
    m_flags.Clear(1);
    m_flags.Clear(0);
    Assign(m_aim, fn_8040CDD0(&m_player.px->m_position, &fn_8040CDA8(&direction, 1000.0f)));
    Vec3f aim;
    Assign(aim, m_aim);
    {
        TargetSearchList nearList(*list.get());
        fn_801E80C0(&nearList, 0x2000);
        weak_ptr<Target> nearest = fn_801E848C(&nearList, &m_player->m_position);
        shared_ptr<Target> target = nearest.lock();
        if (target) {
            Assign(m_aim, target->GetPosition(m_player->m_position));
            m_flags.Set(1);
        }
    }
    if (m_player->m_weapon != 0) {
        m_player->m_weapon->GetWeaponKind();
        if (fn_8006CD78(m_player->m_stateBits, 0x41)) {
            fn_801E80C0(list.get(), 0x800);
            fn_801E7FB4(list.get(), &m_player->m_position, 0.7071f, &direction);
            Vec3f right;
            Construct(right, fn_80079628(m_player.get(), &Vec3f(0.9397f, 0.0f, -0.34202f)));
            fn_801E7FB4(list.get(), &m_player->m_position, 0.0f, &right);
            Vec3f left;
            Construct(left, fn_80079628(m_player.get(), &Vec3f(-0.9397f, 0.0f, -0.34202f)));
            fn_801E7FB4(list.get(), &m_player->m_position, 0.0f, &left);
            fn_801E8218(list.get(), &m_player->m_position, m_player->m_weapon->GetWeaponValue());
            weak_ptr<Target> lockOn = fn_801E848C(list.get(), &m_player->m_position);
            shared_ptr<Target> target = lockOn.lock();
            if (target) {
                m_lockOn = target;
                Assign(aim, target->GetPosition(m_player->m_position));
                m_flags.Set(0);
            }
        }
    }
    Assign(m_target, TurnTo(aim));
}

PlayerTarget::PlayerTarget(const shared_ptr<PlayerBase> &player)
    : PlayerTargetBase(player), m_impl(new Impl) {
    m_impl->Init(this, player);
}

PlayerTarget::~PlayerTarget() {}

void PlayerTarget::vf_0x0C() {
    PlayerTargetBase::vf_0x0C();
    m_impl->Update();
}

bool PlayerTarget::vf_0x10() {
    return fn_80014E34(&m_impl->m_flags.m_bits, 1);
}

bool PlayerTarget::HasTarget() {
    return fn_80014E34(&m_impl->m_flags.m_bits, 0);
}

void PlayerTarget::vf_0x18(const Vec3f &position, float time) {
    m_impl->LookAt(position, time);
}

AimPosition PlayerTarget::vf_0x1C() {
    return m_impl->GetAimPosition();
}

AimPosition PlayerTarget::GetTargetPosition() {
    return m_impl->GetTargetPosition();
}

weak_ptr<Target> PlayerTarget::GetTarget() {
    return m_impl->m_lockOn;
}

weak_ptr<Target> PlayerTarget::vf_0x28() {
    return m_impl->m_nearest;
}

Vec3f PlayerTarget::ToAimSpace(const Vec3f &vector) {
    Vec3f turned;
    Assign(turned, fn_8040D118(&vector, m_impl->m_angles.x));
    Assign(turned, fn_8040D0AC(&turned, m_impl->m_angles.y));
    if (fn_8006CD78(m_player->m_stateBits, 0x40)) {
        return fn_80079628(m_player.get(), &turned);
    }
    return Transform(Fields(m_player.operator->())->m_matrix,
                     fn_8040D0AC(&turned, Fields(m_player.operator->())->m_rotation.y));
}

} // namespace Shadow
} // namespace Player

extern "C" Player::PlayerTargetBase *fn_800B66A0(const shared_ptr<PlayerBase> &player) {
    return new Player::Shadow::PlayerTarget(player);
}

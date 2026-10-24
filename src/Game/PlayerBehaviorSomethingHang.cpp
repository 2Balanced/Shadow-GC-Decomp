/* Player::Behavior::SomethingHang (class name from RTTI): behavior 0xF of Shadow, hanging from a
 * character (Player::Behavior::InitializeArgsSomethingHang; TouchHangCommand moves the hang). The
 * player tells the character it rides it, eases its hands onto the hang position (blending its
 * rotation over 0.1 seconds, then telling the character it is used), follows the position and
 * rotation every frame and passes inputs (command 1) on to the character. StartActionCommand lets
 * go (behavior 2, unless flag bit 2 holds on), as does losing the character; leaving tells the
 * character, and blocks hanging again for a quarter of a second. File name inferred from the class
 * (as Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
/* Compiles boost's operator!= for shared_ptr (see boost/shared_ptr.hpp). */
#define BOOST_SP_NOT_EQUAL

#include "Game/CharCommand.h"
#include "Game/GadgetCharCommands.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8008E360 {
    /* 0x000 */ u8 field_0x0[0x9C];
    /* 0x09C */ float m_hangWait; /* (as in Game/PlayerBehaviorLineHang.cpp) */
    /* 0x0A0 */ u8 field_0xA0[0xD0 - 0xA0];
    /* 0x0D0 */ Vec3f m_prevPosition; /* (as in Game/PlayerBehaviorLightDash.cpp) */
};

/* a - b (0x8000AD64; as in Game/PlayerBehaviorEdgeHang.cpp). */
Vec3f operator-(const Vec3f &a, const Vec3f &b);

extern "C" {
/* Whether a value is about zero (|value| below a small bound; behaviour from its body). */
bool fn_8000A950(float value);
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
/* Matrix routines: the point transformed, a copy, and turns about z, y and x (as in
 * Game/PlayerBehaviorFirstMeet.cpp). */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
void fn_8040B80C(Mtx34f *matrix, float angle);
void fn_8040B848(Mtx34f *matrix, float angle);
void fn_8040B884(Mtx34f *matrix, float angle);
void fn_8040C148(Mtx34f *matrix, const Mtx34f *value);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CE60(Vec3f *vector);                     /* vector = 0 */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
/* a turned towards b by t (Euler angles in axis order order; behaviour inferred; as in
 * Game/PlayerBehaviorLaunch.cpp). */
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8008E360 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

class SomethingHang : public Behavior {
  public:
    SomethingHang(const boost::shared_ptr<PlayerBase> &player);
    virtual ~SomethingHang();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void Follow(float dt);
    Vec3f HandsOffset();

    static const char *s_name;

    /* 0x1C */ boost::weak_ptr<CharInfo> m_target; /* the character hung from */
    /* 0x24 */ const Vec3f *m_position;            /* where the hands go */
    /* 0x28 */ const Vec3f *m_rotation;            /* the rotation the player takes */
    /* 0x2C */ float m_blendTime;                  /* left of the move onto the hang */
    /* 0x30 */ u32 m_flags;
};
SIZE_ASSERT(SomethingHang, 0x34);

const char *SomethingHang::s_name = "SomethingHang";

/* Where the hands are above the player and behind it (names inferred; the values of
 * Game/PlayerBehaviorLineHang.cpp's level hang). The game sets them in __sinit rather than in
 * .sbss, as an initializer that is not a constant expression does; the inline call reproduces that
 * (as in Game/PlayerBehaviorFall.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_handsUp = DynamicFloat(6.0f);
static float s_handsBack = DynamicFloat(0.5f);

SomethingHang::SomethingHang(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0xF) {}

SomethingHang::~SomethingHang() {}

bool SomethingHang::vf_0x0C(int prevId, const InitializeArgs &args) {
    if (FieldsOf(m_player.get())->m_hangWait > 0.0f) {
        return false;
    }
    return !fn_8006CD78(m_player.get()->m_stateBits, 0x1E);
}

void SomethingHang::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsSomethingHang &hang = static_cast<const InitializeArgsSomethingHang &>(args);
    m_target = hang.m_target;
    m_position = hang.m_position;
    m_rotation = hang.m_rotation;
    m_blendTime = 0.1f;
    m_flags = hang.m_flags;
    fn_80076CA8(m_player.get()->m_stateBits, 0x34);
    fn_80076CA8(m_player.get()->m_stateBits, 0xB);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CA8(m_player.get()->m_stateBits, 0x38);
    m_player.get()->m_motion->vf_0x10(0xE, 2.0f);
    fn_8040CE60(&m_player.get()->field_0x18);
    if (!m_target.expired()) {
        boost::shared_ptr<CharInfo> target = m_target.lock();
        target->PushCommand(new RideCommand(m_player, m_player.get()->m_position));
    }
}

void SomethingHang::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0x34);
    fn_80076CC8(m_player.get()->m_stateBits, 0xB);
    fn_8006127C(&m_player.get()->m_flags, 1);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    if (!m_target.expired()) {
        boost::shared_ptr<CharInfo> target = m_target.lock();
        switch (nextId) {
        case 0x1E:
        case 0x1F:
            target->PushCommand(new CancelCommand(m_player.get()->m_self));
            break;
        }
        target->PushCommand(new LeaveCommand(m_player, m_player.get()->m_position));
    }
    FieldsOf(m_player.get())->m_hangWait = 0.25f;
}

void SomethingHang::vf_0x18(CharCommand *command) {
    if (m_target.expired()) {
        return;
    }
    switch (command->TypeId()) {
    case 1: {
        boost::shared_ptr<CharInfo> target = m_target.lock();
        if (target) {
            target->PushCommand(new AccelerateInputCommand(
                static_cast<AccelerateInputCommand *>(command)->m_direction, 4));
        }
        break;
    }
    case 0x106:
        if (!(m_flags & 4)) {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
            command->Flags() |= 2;
        }
        break;
    case 0x112: {
        TouchHangCommand *hang = static_cast<TouchHangCommand *>(command);
        if (m_target.lock() != hang->m_target.lock()) {
            m_target = hang->m_target;
        }
        m_position = hang->m_position;
        m_rotation = hang->m_rotation;
        m_flags = hang->field_0x24;
        command->Flags() |= 2;
        break;
    }
    }
}

void SomethingHang::Update(float dt) {
    if (m_target.expired()) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    }
    if (m_flags & 1) {
        fn_80014E1C(&m_player.get()->m_flags, 1);
    }
    if (m_flags & 2) {
        fn_80014E1C(&m_player.get()->m_flags, 7);
    }
    if (m_blendTime > 0.0f) {
        m_blendTime -= dt;
        if (m_blendTime <= 0.0f) {
            m_blendTime = 0.0f;
            boost::shared_ptr<CharInfo> target = m_target.lock();
            target->PushCommand(new UseCommand(m_player.get()->m_self));
        }
    }
    Follow(dt);
    if (!fn_8000A950(dt)) {
        fn_800091D0(&m_player.px->field_0x18,
                    &(const Vec3f &)fn_8040CDA8(
                        &(const Vec3f &)Subtract(&m_player.get()->m_position,
                                                 &FieldsOf(m_player.get())->m_prevPosition),
                        1.0f / dt));
    }
}

Vec3f SomethingHang::HandsOffset() {
    Mtx34f matrix;
    fn_8040C148(&matrix, &lbl_805E0720);
    fn_8040B848(&matrix, m_rotation->y);
    fn_8040B884(&matrix, m_rotation->x);
    fn_8040B80C(&matrix, m_rotation->z);
    Vec3f offset;
    fn_80008E1C(&offset, 0.0f, s_handsUp, -s_handsBack);
    return fn_8040B67C(&matrix, &offset);
}

void SomethingHang::Follow(float dt) {
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)Subtract(m_position, &(const Vec3f &)HandsOffset()));
    if (0.0f == m_blendTime) {
        fn_800091D0(&m_player.get()->m_position, &position);
        m_player->m_rotation = *m_rotation;
    } else {
        float t = dt / m_blendTime;
        Vec3f rotation = m_player->m_rotation;
        float rate = t < 1.0f ? t : 1.0f;
        AddVector(m_player->m_position,
                  fn_800091A4(&(const Vec3f &)(position - m_player->m_position), rate));
        m_player->m_rotation = fn_8040F16C(&rotation, m_rotation, 2, rate);
    }
}

Behavior *SomethingHang::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new SomethingHang(player);
}

void SomethingHang::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds SomethingHang by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0D60 = {
    Player::Behavior::SomethingHang::Create, Player::Behavior::SomethingHang::Release, 0};

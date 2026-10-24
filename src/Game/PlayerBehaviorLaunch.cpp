/* Player::Behavior::Launch (class name from RTTI): behavior 0x17 of Shadow, being thrown through the
 * air (by a spring or a cannon; Player::Behavior::InitializeArgsLaunch). The player flies on along
 * its velocity, curled up or turned to face where it goes, and steers a little; it falls (behavior
 * 2) once its course bends more than 45 degrees and lands (behavior 3) on the ground. File name
 * inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <cmath>

#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/TGi.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_80089100 {
    /* 0x000 */ u8 field_0x0[0xB8];
    /* 0x0B8 */ Vec3f field_0xB8; /* the input's direction (as in Game/PlayerBehaviorEdgeHang.cpp) */
};

extern "C" {
/* The player's flag words and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* -v (the game's out-of-line unary minus; behaviour from its body). */
Vec3f fn_80016284(const Vec3f *v);
/* Called with the player's object at 0x234: stopping and starting its effect (behaviour inferred;
 * see Game/PlayerBehaviorFall.cpp). */
void fn_8007792C(void *object);
void fn_80077950(void *object, int arg);
/* The player's up and front axes (see Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_80079698(Player::PlayerBase *player);
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player by its turn rate (behaviour inferred; as in Super), and the player's Euler
 * angles for a frame (see Game/PlayerBehaviorHomingAttack.cpp). */
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
Vec3f fn_80093D34(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *forward,
                  const Vec3f *up);
/* Starts effect id of the player's effects at 0x23C (behaviour inferred; see
 * Game/PlayerBarrier.cpp). */
boost::weak_ptr<EffectElement> fn_800A2854(void *effects, int arg, int id);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Calls on the player's motion: whether it has ended, and fn_800AA700 (meanings inferred; as in
 * Game/PlayerBehaviorGrind.cpp). */
bool fn_800AA640(Player::PlayerMotion *motion);
void fn_800AA700(Player::PlayerMotion *motion, int arg);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
/* a turned towards b by t (Euler angles in axis order order; behaviour inferred). */
Vec3f fn_8040F16C(const Vec3f *a, const Vec3f *b, int order, float t);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_80089100 *>(player)

/* Degrees to radians (name inferred; as in Game/PlayerBehaviorGrind.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

/* The launch's flag word (struct name unknown): bit 0 the player is curled up. */
struct LaunchFlags {
    LaunchFlags() {
        fn_80014E04(&m_bits);
    }
    void Set(int bit) {
        fn_80014E1C(&m_bits, bit);
    }
    bool Test(int bit) {
        return fn_80014E34(&m_bits, bit);
    }
    void Reset() {
        fn_80014E10(&m_bits);
    }

    u32 m_bits;
};

class Launch : public Behavior {
  public:
    Launch(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Launch();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    static const char *s_name;

    /* 0x1C */ LaunchFlags m_flags;
    /* 0x20 */ Vec3f m_direction; /* where the launch started to go */
};
SIZE_ASSERT(Launch, 0x2C);

const char *Launch::s_name = "Launch";

Launch::Launch(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0x17) {}

Launch::~Launch() {}

bool Launch::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void Launch::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsLaunch &launch = static_cast<const InitializeArgsLaunch &>(args);
    m_flags.Reset();
    fn_800091D0(&m_direction, &(const Vec3f &)fn_8040CEE8(&m_player.get()->field_0x18));
    if (launch.m_flags & 1) {
        m_flags.Set(0);
    }
    if (launch.m_flags & 2) {
        fn_80076CA8(m_player.get()->m_stateBits, 0x40);
    }
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80076CA8(m_player.get()->m_stateBits, 0x38);
    if (m_flags.Test(0)) {
        m_player.get()->m_motion->vf_0x10(0xC, 0.0f);
        fn_80014E1C(&m_player.get()->m_flags, 2);
        fn_80014E1C(&m_player.get()->m_flags, 6);
        fn_80076CA8(m_player.get()->m_stateBits, 0x35);
        fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 4);
    } else {
        fn_80014E1C(&m_player.get()->m_flags, 7);
        m_player.get()->m_motion->vf_0x10(0x26, 0.0f);
        if (m_player.get()->field_0x54 != -1 && !lbl_8057D6A0.m_twoPlayer) {
            fn_800A2854(reinterpret_cast<void *>(m_player.get()->field_0x23C), 1, 0x5C);
        }
    }
}

void Launch::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 0x40);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_800AA700(m_player.get()->m_motion, 0);
    if (m_flags.Test(0)) {
        if (nextId != 2) {
            fn_8006127C(&m_player.get()->m_flags, 2);
        }
        fn_80076CC8(m_player.get()->m_stateBits, 0x35);
        fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
    }
}

void Launch::vf_0x18(CharCommand *command) {}

void Launch::Update(float dt) {
    AddVector(m_player->field_0x24,
              fn_800091A4(&FieldsOf(m_player.operator->())->field_0xB8, 220.0f));
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (fn_80014E34(&m_player.get()->field_0xA4, 8)) {
        fn_80076CA8(m_player.get()->m_stateBits, 0x40);
    } else {
        float course =
            fn_8040CF90(&(const Vec3f &)fn_8040CEE8(&m_player.get()->field_0x18), &m_direction);
        if (course <= std::cos(DegToRad(45.0f))) {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
            return;
        }
        if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(3));
            return;
        }
    }
    if (fn_800AA640(m_player.get()->m_motion)) {
        fn_800AA700(m_player.get()->m_motion, 1);
    }
    if (m_flags.Test(0)) {
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
            fn_80093AF8(&m_player, 12.0f * dt);
        }
    } else {
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&m_player.get()->field_0x18));
        Vec3f up;
        fn_800091EC(&up, &(const Vec3f &)fn_80079698(m_player.get()));
        float dot = fn_8040CF90(&up, &direction);
        if (dot < -0.998f) {
            fn_800091D0(&up, &(const Vec3f &)fn_800796DC(m_player.get()));
        } else if (dot > 0.998f) {
            fn_800091D0(&up, &(const Vec3f &)fn_80016284(
                                 &(const Vec3f &)fn_800796DC(m_player.get())));
        }
        Vec3f angles = fn_80093D34(&m_player, &direction, &up);
        m_player->m_rotation = fn_8040F16C(&m_player->m_rotation, &angles, 2, 12.0f * dt);
    }
}

Behavior *Launch::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Launch(player);
}

void Launch::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Launch by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B08A0 = {
    Player::Behavior::Launch::Create, Player::Behavior::Launch::Release, 0};

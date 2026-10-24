/* Player::Behavior::Jump (class name from RTTI): behavior 5 of Shadow, the jump
 * (Player::Behavior::InitializeArgsJump gives its direction; without one it jumps along the
 * player's up axis). Entering it throws the player along the direction at the character's jump
 * speed; holding the button for up to 0.75 seconds pushes it higher. A jump off a wall, a pole
 * swing or a spring (behaviors 7, 0x13 and 0x1B) and a jump while sliding start differently. In
 * the air the player steers along the input and may curl up (the character's jump effect); it
 * falls (behavior 2) once it moves down after a jump into a curl and lands (behavior 3) on the
 * ground. File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). Where the game reloads the player pointer in a call's arguments after
 * another argument's call, the code reads m_player.px (as in Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <cmath>

#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerCharaParam.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_800871D8 {
    /* 0x000 */ u8 field_0x0[0x60];
    /* 0x060 */ const float *m_frameTime; /* (see Game/DiablonController.cpp) */
    /* 0x064 */ s32 m_kind;               /* the character (as in Game/PlayerBehaviorFall.cpp) */
    /* 0x068 */ u8 field_0x68[0xB8 - 0x68];
    /* 0x0B8 */ Vec3f field_0xB8; /* the input's direction (as in Game/PlayerBehaviorEdgeHang.cpp) */
    /* 0x0C4 */ u8 field_0xC4[0xE8 - 0xC4];
    /* 0x0E8 */ Vec3f m_jumpUp; /* (as in Game/PlayerBehaviorHomingAttack.cpp) */
    /* 0x0F4 */ Vec3f m_up;
    /* 0x100 */ u8 field_0x100[0x11C - 0x100];
    /* 0x11C */ Mtx34f m_matrix; /* the player's frame (meaning inferred) */
};

/* A character's jump (struct name unknown): one record per character kind (names inferred). */
struct UnkJumpParam_804B0558 {
    /* 0x0 */ float m_speed; /* the jump's starting speed */
    /* 0x4 */ float m_push;  /* the push while the button is held */
    /* 0x8 */ bool m_curl;   /* the jump may curl up (the jump's flag 0) */
};

extern "C" {
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
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
/* The player's gravity (see Game/PlayerBehaviorDamage.cpp), up and front axes (see
 * Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_80078D58(Player::PlayerBase *player);
Vec3f fn_80079698(Player::PlayerBase *player);
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player: by its turn rate, towards a direction, and the direction it faces; the
 * input's direction relative to a direction (behaviour inferred; as in Super). */
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
void fn_80093BA0(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *direction, float rate);
int fn_80093C60(boost::shared_ptr<Player::PlayerBase> *player, const Vec3f *direction);
Vec3f fn_80093FC0(boost::shared_ptr<Player::PlayerBase> *player);
/* Starts effect id of the player's effects at 0x23C (behaviour inferred; see
 * Game/PlayerBarrier.cpp). */
boost::weak_ptr<EffectElement> fn_800A2854(void *effects, int arg, int id);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Whether the player falls (see Game/PlayerBehaviorHomingJump.cpp). */
bool fn_800B7B58(boost::shared_ptr<Player::PlayerBase> *player);

Vec3f fn_800091A4(const Vec3f *vector, float scale);        /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);        /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);          /* a + b */
void fn_8040CE60(Vec3f *vector);                            /* vector = 0 */
float fn_8040CE74(Vec3f *vector);                           /* normalises it */
Vec3f fn_8040CEE8(const Vec3f *vector);                     /* normalized */
float fn_8040CF2C(const Vec3f *v);                          /* |v|^2 */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);          /* dot product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);        /* vector += value */
Mtx34f fn_8040BEF8(const Mtx34f *matrix);                   /* inverse */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point); /* the point transformed */
Vec2f fn_8040F664(const Vec3f &rotation);
/* Splits vector along normal (see Game/AdjustCosRot.cpp). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_800871D8 *>(player)

/* The effect of a player's motion, the weak pointer at 0x28 (a macro, as FieldsOf). */
#define MotionEffectOf(motion) (*reinterpret_cast<boost::weak_ptr<EffectElement> *>((motion)->field_0x28))

/* The player's character kind and frame time: inline functions in the game, whose out-of-line
 * copies this unit emits first (names inferred). */
inline s32 CharaKind(Player::PlayerBase *player) {
    return FieldsOf(player)->m_kind;
}
inline const float &FrameTime(Player::PlayerBase *player) {
    return *FieldsOf(player)->m_frameTime;
}

/* The larger of a and b (name inferred). Retail calls it out of line, so it is not declared inline
 * like std::max (as in Game/ThreatObjHolder.cpp). */
template <class T> const T &Max(const T &a, const T &b) {
    return a < b ? b : a;
}

/* Whether a vector is about zero (as in Game/PlayerBehaviorIdle.cpp). */
inline bool IsAboutZero(const Vec3f &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z < 0.0001f;
}

/* direction . the player's jump direction (an inline helper: the game loads the player before
 * working out the direction; name inferred). */
inline float DotJumpUp(const Vec3f &direction, Player::PlayerBase *player) {
    return fn_8040CF90(&direction, &FieldsOf(player)->m_jumpUp);
}

/* vector = value, through the game's out-of-line copy (see Game/JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

/* The jump's flag word (struct name unknown): bit 0 the jump may still curl up, bit 1 steering
 * along the input, bit 2 turning towards m_turnTo, bit 3 a jump that falls once it moves down,
 * bit 4 the button held (cleared by command 0x105), bit 5 a jump off a spring. */
struct JumpFlags {
    JumpFlags() {
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

class Jump : public Behavior {
  public:
    Jump(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Jump();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void Curl();

    static const char *s_name;

    /* 0x1C */ s32 m_step;
    /* 0x20 */ Vec3f m_direction;
    /* 0x2C */ float m_holdTime; /* how much longer holding the button pushes */
    /* 0x30 */ JumpFlags m_flags;
    /* 0x34 */ Vec3f m_turnTo;
    /* 0x40 */ int m_wait; /* frames before a landing counts */
};
SIZE_ASSERT(Jump, 0x44);

/* The characters' jumps (the record of kind 9 jumps lower, without a push or a curl). */
static const UnkJumpParam_804B0558 s_params[] = {
    {100.0f, 270.0f, true}, {100.0f, 270.0f, true}, {100.0f, 270.0f, true},
    {100.0f, 270.0f, true}, {100.0f, 270.0f, true}, {100.0f, 270.0f, true},
    {100.0f, 270.0f, true}, {100.0f, 270.0f, true}, {100.0f, 270.0f, true},
    {75.0f, 0.0f, false},   {100.0f, 270.0f, true}, {100.0f, 270.0f, true},
    {100.0f, 270.0f, true}, {100.0f, 270.0f, true}, {100.0f, 270.0f, true},
    {100.0f, 270.0f, true}, {100.0f, 270.0f, true}, {100.0f, 270.0f, true},
    {100.0f, 270.0f, true}, {100.0f, 270.0f, true},
};

const char *Jump::s_name = "Jump";

Jump::Jump(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 5) {}

Jump::~Jump() {}

void Jump::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x105:
        m_flags.Clear(4);
        m_holdTime = 0.0f;
        command->Flags() |= 2;
        break;
    }
}

bool Jump::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

/* NonMatching (99.7%): two spots. Retail reads the frame time through the pointer after setting up
 * the scaled gravity's result (mr r4,r3 before the lfs), and keeps the jump table's address in r3
 * and the player in r4 at the curl check (as Player::Behavior::Ground::SetSpeedRange). */
void Jump::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsJump *jump = dynamic_cast<const InitializeArgsJump *>(&args);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CA8(m_player.get()->m_stateBits, 4);
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80014E1C(&m_player.get()->m_flags, 3);
    fn_80014E1C(&m_player.get()->m_flags, 6);
    m_step = 0;
    if (IsAboutZero(jump->m_direction)) {
        fn_800091D0(&m_direction, &(const Vec3f &)fn_80079698(m_player.get()));
    } else {
        AssignVector(m_direction, jump->m_direction);
    }
    m_flags.Reset();
    m_flags.Set(1);
    m_flags.Set(0);
    m_holdTime = 0.75f;
    m_flags.Set(4);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B) &&
        fn_8006CD78(m_player.get()->m_stateBits, 0x3C)) {
        m_flags.Clear(1);
        m_flags.Clear(0);
        fn_80076CA8(m_player.get()->m_stateBits, 0x45);
        m_holdTime = 0.0f;
        m_flags.Clear(4);
        Vec3f across;
        fn_8040F990(0, &across, &m_player.get()->field_0x18, &FieldsOf(m_player.get())->m_up);
        fn_8040CE74(&across);
        switch (fn_80093C60(&m_player, &across)) {
        case 0:
            m_player.get()->m_motion->vf_0x10(5, 2.0f);
            m_flags.Set(0);
            break;
        case 3:
            m_player.get()->m_motion->vf_0x10(7, 2.0f);
            break;
        case 1:
            m_player.get()->m_motion->vf_0x10(7, 2.0f);
            break;
        case 2:
            m_player.get()->m_motion->vf_0x10(7, 2.0f);
            break;
        }
        fn_800091D0(&m_direction,
                    &(const Vec3f &)fn_8040CDD0(&across, &(const Vec3f &)fn_8040CDA8(
                                                             &FieldsOf(m_player.get())->m_up, 1.0f)));
        fn_8040CE74(&m_direction);
        m_flags.Set(2);
        AssignVector(m_turnTo, fn_80093FC0(&m_player));
        fn_800091D0(&m_turnTo, &(const Vec3f &)fn_800796DC(m_player.get()));
        m_player.get()->m_sound->StopVoice(2);
        fn_8040F990(0, &m_player.get()->field_0x18, &m_player.get()->field_0x18, &m_direction);
        AddVector(m_player->field_0x18, fn_800091A4(&m_direction, 200.0f));
    } else if (prevId == 0x13) {
        m_flags.Clear(1);
        m_flags.Set(0);
        m_holdTime = 0.0f;
        m_flags.Clear(4);
        fn_800091D0(&m_player.px->field_0x18,
                    &(const Vec3f &)fn_8040CDA8(&m_direction,
                                                s_params[CharaKind(m_player.get())].m_speed));
    } else if (prevId == 7) {
        m_player.get()->m_motion->vf_0x10(7, 2.0f);
        fn_8040CE60(&m_player.get()->field_0x18);
        AddVector(m_player->field_0x18, fn_800091A4(&m_direction, 200.0f));
        m_flags.Clear(1);
        m_flags.Clear(0);
        m_holdTime = 0.0f;
        m_flags.Clear(4);
        m_player.get()->m_sound->StopVoice(2);
    } else if (prevId == 0x1B) {
        m_flags.Clear(1);
        m_flags.Set(0);
        m_holdTime = 0.0f;
        m_flags.Clear(4);
        m_flags.Set(5);
        fn_80076CA8(m_player.get()->m_stateBits, 0x43);
        if (IsAboutZero(jump->m_direction)) {
            fn_800091D0(&m_direction, &FieldsOf(m_player.get())->m_jumpUp);
        }
        fn_800091D0(&m_player.px->field_0x18,
                    &(const Vec3f &)fn_8040CDA8(&m_direction,
                                                s_params[CharaKind(m_player.get())].m_speed));
        m_player.get()->m_motion->vf_0x10(5, 2.0f);
    } else {
        Vec3f along;
        Vec3f across;
        fn_8040F990(&along, &across, &(const Vec3f &)fn_8040CEE8(&m_direction),
                    &FieldsOf(m_player.px)->m_up);
        fn_80014E1C(&m_player.get()->m_flags, 2);
        float acrossSq = fn_8040CF2C(&across);
        if (fn_8040CF2C(&along) > acrossSq) {
            m_player.get()->m_motion->vf_0x10(5, 2.0f);
        } else {
            m_player.get()->m_motion->vf_0x10(6, 2.0f);
            m_flags.Clear(0);
            m_flags.Set(3);
            fn_80076CA8(m_player.get()->m_stateBits, 0x38);
        }
        m_player.get()->m_sound->StopVoice(1);
        fn_8040F990(0, &m_player.get()->field_0x18, &m_player.get()->field_0x18, &m_direction);
        AddVector(m_player->field_0x18,
                  fn_800091A4(&m_direction, s_params[CharaKind(m_player.operator->())].m_speed));
    }
    Vec3f gravity;
    fn_800091EC(&gravity, &(const Vec3f &)fn_80078D58(m_player.get()));
    if (fn_8040CF90(&gravity, &m_player.get()->field_0x18) >= 0.0f) {
        AddVector(m_player->m_position,
                  fn_800091A4(&gravity, FrameTime(m_player.operator->())));
        AddVector(m_player->field_0x18, gravity);
    }
    m_wait = 10;
    if (!s_params[CharaKind(m_player.get())].m_curl) {
        m_flags.Clear(0);
    }
    fn_80076CA8(m_player.get()->m_stateBits, 0x35);
    fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 4);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
        m_holdTime = 0.0f;
        m_flags.Clear(4);
        m_flags.Clear(0);
        fn_8006127C(&m_player.get()->m_flags, 2);
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x19)) {
        fn_80014E1C(&m_player.get()->m_flags, 7);
    }
}

void Jump::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 4);
    fn_8006127C(&m_player.get()->m_flags, 3);
    fn_8006127C(&m_player.get()->m_flags, 2);
    fn_8006127C(&m_player.get()->m_flags, 6);
    fn_8006127C(&m_player.get()->m_flags, 7);
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 0x43);
    fn_80076CC8(m_player.get()->m_stateBits, 0x45);
    fn_80076CC8(m_player.get()->m_stateBits, 0x35);
    fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
    boost::weak_ptr<EffectElement> &effect = MotionEffectOf(m_player.get()->m_motion);
    if (!effect.expired()) {
        effect.lock()->Kill();
    }
}

void Jump::Update(float dt) {
    if (fn_80014E34(&m_player.get()->m_flags, 7) &&
        !fn_8006CD78(m_player.get()->m_stateBits, 0x19)) {
        fn_8006127C(&m_player.get()->m_flags, 7);
    }
    if (m_flags.Test(4) && m_holdTime >= 0.0f) {
        m_holdTime -= dt;
        AddVector(m_player->field_0x24,
                  fn_800091A4(&m_direction, s_params[CharaKind(m_player.operator->())].m_push));
    }
    switch (m_step) {
    case 0:
        m_step = 1;
        break;
    case 1:
        if (m_flags.Test(0) && fn_800A9E1C(&m_player)) {
            Curl();
            m_flags.Clear(0);
            m_step = 2;
        }
    case 2:
        if (m_holdTime <= 0.0f) {
            m_flags.Clear(4);
            if (m_flags.Test(0)) {
                Curl();
            }
            m_step = 3;
        }
        break;
    case 3:
        if (fn_800B7B58(&m_player)) {
            m_step++;
            if (m_flags.Test(3)) {
                fn_800A7F14(m_player.get()->m_behaviors,
                            &(const InitializeArgs &)InitializeArgs(2));
                return;
            }
        }
        break;
    case 4:
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    }
    if (m_flags.Test(5) && fn_800B7B58(&m_player)) {
        m_flags.Clear(5);
        fn_80076CC8(m_player.get()->m_stateBits, 0x43);
    }
    if (m_flags.Test(1)) {
        float push;
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
            push = 110.0f;
        } else {
            push = 220.0f;
        }
        AddVector(m_player->field_0x24,
                  fn_800091A4(&FieldsOf(m_player.operator->())->field_0xB8, push));
        if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
            fn_80093AF8(&m_player, 6.5f * dt);
        }
    }
    if (m_flags.Test(2)) {
        fn_80093BA0(&m_player, &m_turnTo, 6.5f * dt);
    }
    fn_80079720(m_player.get());
    if (!m_flags.Test(5)) {
        fn_8009728C(&m_player, 0);
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 2) && m_wait == 0) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(3));
        return;
    }
    m_wait--;
    m_wait = Max(m_wait, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    if (m_flags.Test(3)) {
        Vec3f velocity;
        fn_800091EC(&velocity,
                    &(const Vec3f &)fn_8040B67C(
                        &(const Mtx34f &)fn_8040BEF8(&FieldsOf(m_player.px)->m_matrix),
                        &(const Vec3f &)fn_80016284(&m_player.get()->field_0x18)));
        Vec2f angles = fn_8040F664(velocity);
        m_player.get()->m_rotation.x = angles.x;
        if (std::fabs(DotJumpUp(fn_8040CEE8(&velocity), m_player.px)) < 0.998f) {
            m_player.get()->m_rotation.y = angles.y;
        }
        m_player.get()->m_rotation.z = 0.0f;
    }
}

/* Curls the player up: the curl motion and the character's jump effect (name inferred). */
void Jump::Curl() {
    if (m_player.get()->m_motion != 0) {
        m_player.get()->m_motion->vf_0x10(0xC, 2.0f);
        boost::weak_ptr<EffectElement> &effect = MotionEffectOf(m_player.get()->m_motion);
        if (effect.expired()) {
            effect = fn_800A2854(reinterpret_cast<void *>(m_player.get()->field_0x23C), 0,
                                 lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_jumpEffect);
        }
    }
}

void Jump::vf_0x20() {}

Behavior *Jump::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Jump(player);
}

void Jump::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Jump by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0694 = {
    Player::Behavior::Jump::Create, Player::Behavior::Jump::Release, 0};

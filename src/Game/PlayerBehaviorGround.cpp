/* Player::Behavior::Ground (class name from RTTI): behavior 1 of Shadow, running on the ground.
 * Each frame it pushes the player along the input, faster the straighter the input points ahead
 * (up to a top speed from the character's parameters), turns the player and sets the run motion's
 * rate from the speed. A sharp turn at speed plays a skid (modes 2 and 3), a slope makes the player
 * slide (mode 1), and a loop's path (Player::SetLoopPathCommand) steers the input along the path.
 * It stops (behavior 0) when slow, falls (behavior 2) or lands on the ground found below (behavior
 * 4) when in the air. File name inferred from the class (as Game/PlayerBehaviorSuper.cpp); all
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). Where the game reloads the player pointer in a call's arguments
 * after another argument's call, the code reads m_player.px: an inline get() there is expanded
 * ahead of the calls. */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <cmath>

/* Declares Player::SetLoopPathCommand (Game/CharCommand.h). */
#define CHARCOMMAND_SET_LOOP_PATH

#include "Game/CharCommand.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerCharaParam.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/UnkCircuitTarget_800ABCE8.h"
#include "Game/WeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_800831D8 {
    /* 0x000 */ u8 field_0x0[0x64];
    /* 0x064 */ s32 m_kind; /* the character (as in Game/PlayerBehaviorFall.cpp) */
    /* 0x068 */ u8 field_0x68[0xB8 - 0x68];
    /* 0x0B8 */ Vec3f field_0xB8; /* the input's direction (as in Game/PlayerBehaviorEdgeHang.cpp) */
    /* 0x0C4 */ u8 field_0xC4[0xF4 - 0xC4];
    /* 0x0F4 */ Vec3f m_up;
    /* 0x100 */ u8 field_0x100[0x1C8 - 0x100];
    /* 0x1C8 */ Vec3f field_0x1C8; /* the ground's normal (meaning inferred) */
};

extern "C" {
/* A 32-bit flag word: fn_80014E1C sets bit, fn_8006127C clears bit and fn_80014E34 tests bit (see
 * Game/BAMine.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* std::min for floats (an out-of-line copy; see Game/BirdBase.cpp). */
const float &fn_800704AC(const float &a, const float &b);
/* The player's front and up axes (behaviour inferred). */
Vec3f fn_800796DC(Player::PlayerBase *player);
Vec3f fn_80079698(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player: towards a direction, by its turn rate, and the direction the input asks for
 * (behaviour inferred; as in Super). */
void fn_80093BA0(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *direction, float rate);
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
Vec3f fn_80093FC0(boost::shared_ptr<Player::PlayerBase> *player);
/* Looks for the ground below the player: where it is (behaviour inferred). */
bool fn_80096E20(Vec3f *position, boost::shared_ptr<Player::PlayerBase> *player);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* The run's effect of the player's effects at 0x23C, on and off (behaviour inferred). */
void fn_800A2CBC(void *effects);
void fn_800A2C98(void *effects);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Calls on the player's motion (meanings unknown; fn_800AA7B8 sets the motion's word at 0x438, as
 * in AirAttack). */
void fn_800AA700(Player::PlayerMotion *motion, int arg);
void fn_800AA7B8(Player::PlayerMotion *motion, float value);
/* The circuit target: where it is (position and the two directions of its frame), whether it has
 * a path, a flag, clearing it, and whether the player is on it (behaviour inferred; see
 * Game/PlayerBehaviorChaosControl.cpp). */
void fn_800ABAE8(const UnkCircuitTarget_800ABCE8 *target, Vec3f *position, Vec3f *forward,
                 Vec3f *up);
bool fn_8007646C(UnkCircuitTarget_800ABCE8 *target);
void fn_800ABAA4(UnkCircuitTarget_800ABCE8 *target, bool on);
void fn_8007684C(UnkCircuitTarget_800ABCE8 *target);
bool fn_800ABBA4(UnkCircuitTarget_800ABCE8 *target, boost::shared_ptr<Player::PlayerBase> *player);

float fn_8000FEE4(float value);                       /* square root */
float fn_80015D68(float value);                       /* arc cosine */
void fn_8040CE60(Vec3f *vector);                      /* vector = 0 */
void fn_8040CE38(Vec3f *vector, float scale);         /* vector *= scale */
Vec3f fn_8040CEE8(const Vec3f *vector);               /* normalized */
float fn_8040CF2C(const Vec3f *v);                    /* |v|^2 */
float fn_8040CF50(const Vec3f *v);                    /* |v| */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);    /* dot product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);  /* vector += value */
Vec3f fn_800091A4(const Vec3f *vector, float scale);  /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);  /* vector * scale */
/* Splits vector along normal (see Game/AdjustCosRot.cpp). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_800831D8 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

/* vector *= scale (the game's out-of-line scaling, fn_8040CE38). */
inline Vec3f &operator*=(Vec3f &vector, float scale) {
    fn_8040CE38(&vector, scale);
    return vector;
}

/* The blend from a to b (name inferred; as in Game/PlayerBehaviorChaosBlast.cpp). */
inline float Lerp(float a, float b, float t) {
    return t * (b - a) + a;
}

/* Degrees to radians (name inferred; as in Game/BalloonGenerator.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace Player {
namespace Behavior {

class Ground : public Behavior {
  public:
    Ground(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Ground();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void ResetSpeed();
    void SetSpeedRange();
    void ReleasePath();
    void SetPath(const UnkCircuitTarget_800ABCE8 &path);
    int ChooseMode(const Vec3f *input);
    void Skid(float dt);
    void Slide(float dt);
    void Run(float dt);

    /* velocity scaled down to the slide's top speed from speed (an inline helper: the game reads
     * the top speed again after the out-of-line operator-> call). */
    void LimitSlideSpeed(Vec3f &velocity, float speed) {
        fn_8040CE38(&velocity, s_slideSpeed / speed);
    }

    static const char *s_name;
    static float s_stopSpeed;
    static float s_fastSpeed;
    static float s_slideSpeed;

    /* 0x1C */ s32 m_mode; /* 0 running, 1 sliding, 2 and 3 skidding */
    /* 0x20 */ s32 m_step; /* the step of the mode */
    /* 0x24 */ float m_minSpeed;
    /* 0x28 */ float m_maxSpeed;
    /* 0x2C */ float m_speed;
    /* 0x30 */ float m_turnRate;
    /* 0x34 */ float m_lean; /* the angle between the input and the player's front */
    /* 0x38 */ UnkCircuitTarget_800ABCE8 m_path;
};
SIZE_ASSERT(Ground, 0x40);

const char *Ground::s_name = "Ground";

/* The speeds below which the player stops, above which it counts as fast, and the top speed of a
 * slide (names inferred). The game sets them in __sinit rather than in .sdata, as an initializer
 * that is not a constant expression does; the inline call reproduces that (as in
 * Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
float Ground::s_stopSpeed = DynamicFloat(1.0f - 0.8f);
float Ground::s_fastSpeed = DynamicFloat(200.0f);
float Ground::s_slideSpeed = DynamicFloat(30.0f);

Ground::Ground(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 1) {
    SetSpeedRange();
    m_speed = m_minSpeed;
    m_turnRate = 2.0f;
}

Ground::~Ground() {}

void Ground::vf_0x18(CharCommand *command) {
    if (m_mode == 0) {
        switch (command->TypeId()) {
        case 0x10C: {
            SetLoopPathCommand *path = dynamic_cast<SetLoopPathCommand *>(command);
            if (path != 0 && fn_80014E34(&m_player.get()->field_0xA4, 0) &&
                fn_80014E34(&m_player.get()->m_flags, 0)) {
                SetPath(path->m_path);
            }
            command->Flags() |= 2;
            break;
        }
        case 0x124:
            m_player.get()->m_motion->vf_0x10(2, 15.0f);
            break;
        case 0x125:
            m_player.get()->m_motion->vf_0x10(1, 15.0f);
            break;
        }
    }
}

bool Ground::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void Ground::Enter(int prevId, const InitializeArgs &args) {
    SetSpeedRange();
    if (static_cast<const InitializeArgsGround &>(args).m_fullSpeed) {
        m_speed = m_maxSpeed;
    } else {
        ResetSpeed();
    }
    m_mode = 0;
    m_step = 0;
    fn_80076CC8(m_player.get()->m_stateBits, 0x1A);
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80076CA8(m_player.get()->m_stateBits, 0x3E);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    fn_80014E1C(&m_player.get()->m_flags, 3);
    fn_80014E1C(&m_player.get()->m_flags, 5);
    fn_80014E1C(&m_player.get()->m_flags, 6);
    fn_80014E1C(&m_player.get()->m_flags, 7);
    fn_800AA700(m_player.get()->m_motion, 1);
    m_player.get()->m_motion->vf_0x10(1, 15.0f);
    fn_8007684C(&m_path);
}

void Ground::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 3);
    fn_80076CC8(m_player.get()->m_stateBits, 0x1A);
    fn_80076CC8(m_player.get()->m_stateBits, 0x3B);
    fn_80076CC8(m_player.get()->m_stateBits, 0x3E);
    fn_8006127C(&m_player.get()->m_flags, 1);
    fn_8006127C(&m_player.get()->m_flags, 3);
    fn_8006127C(&m_player.get()->m_flags, 4);
    fn_8006127C(&m_player.get()->m_flags, 5);
    fn_8006127C(&m_player.get()->m_flags, 6);
    fn_8006127C(&m_player.get()->m_flags, 7);
    if (m_player.get()->m_motion != 0) {
        fn_800AA700(m_player.get()->m_motion, 0);
    }
    ReleasePath();
    fn_800A2C98(reinterpret_cast<void *>(m_player.get()->field_0x23C));
}

void Ground::Update(float dt) {
    SetSpeedRange();
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1F) && m_player.get()->m_weapon->IsFiring()) {
        boost::shared_ptr<Weapon::WeaponBase> weapon = m_player.get()->m_weapon->GetWeapon();
        float recoil = 0.0f;
        if (weapon.get() != 0) {
            recoil = weapon->vf_0x28();
        }
        m_player->field_0x18 *= 1.0f - recoil;
        m_speed *= 1.0f - recoil;
    }
    switch (m_mode) {
    case 0:
        Run(dt);
        break;
    case 1:
        Slide(dt);
        break;
    case 2:
    case 3:
        Skid(dt);
        break;
    }
    float speedSq = fn_8040CF2C(&m_player.get()->field_0x18);
    if (speedSq > 62500.0f) {
        fn_80014E1C(&m_player.get()->m_flags, 0);
    } else {
        fn_8006127C(&m_player.get()->m_flags, 0);
    }
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x3C) && speedSq > 22500.0f) {
        fn_80076CA8(m_player.get()->m_stateBits, 0x3B);
    } else {
        fn_80076CC8(m_player.get()->m_stateBits, 0x3B);
    }
    if (!fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        Vec3f position;
        if (fn_80096E20(&position, &m_player)) {
            fn_800091D0(&m_player.get()->m_position, &position);
            fn_8040CE60(&m_player.get()->field_0x18);
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(4));
        } else {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        }
    } else {
        speedSq = fn_8040CF2C(&m_player.get()->field_0x18);
        if (speedSq > s_fastSpeed * s_fastSpeed) {
            fn_80014E1C(&m_player.get()->m_flags, 4);
        } else {
            fn_8006127C(&m_player.get()->m_flags, 4);
        }
        if (!fn_80014E34(&m_player.get()->m_flags, 0) &&
            fn_8040CF90(&FieldsOf(m_player.get())->field_0x1C8, &FieldsOf(m_player.get())->m_up) <
                0.42261826f) {
            fn_800091D0(&m_player.px->field_0x18,
                        &(const Vec3f &)fn_8040CDA8(&FieldsOf(m_player.get())->field_0x1C8, 100.0f));
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        } else if (speedSq < s_stopSpeed * s_stopSpeed) {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
        }
    }
}

/* Running: the input pushes the player, faster the straighter it points ahead. */
void Ground::Run(float dt) {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x3C)) {
        m_mode = 1;
        m_step = 0;
        Slide(dt);
        return;
    }
    switch (m_step) {
    case 0:
        m_player.get()->m_motion->vf_0x10(1, 15.0f);
        m_step++;
        break;
    }
    Vec3f input;
    Vec3f pathPosition;
    Vec3f pathForward;
    Vec3f pathUp;
    fn_800091EC(&input, &FieldsOf(m_player.get())->field_0xB8);
    if (fn_8007646C(&m_path)) {
        if (fn_80014E34(&m_player.get()->m_flags, 0) && fn_800ABBA4(&m_path, &m_player)) {
            fn_800ABAE8(&m_path, &pathPosition, &pathForward, &pathUp);
            fn_800091D0(&input, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&pathForward),
                                                            fn_8040CF50(&input)));
        } else {
            ReleasePath();
        }
    }
    int mode = ChooseMode(&input);
    if (mode != m_mode) {
        m_mode = mode;
        m_step = 0;
        Skid(dt);
        return;
    }
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
        ResetSpeed();
    } else if (fn_8040CF2C(&input) < 0.9f * 0.9f) {
        m_speed *= 0.9f;
        if (m_speed < m_minSpeed) {
            m_speed = m_minSpeed;
        }
        m_lean = 360.0f;
    } else {
        Vec3f direction;
        Vec3f front;
        Vec3f up;
        fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&input));
        fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_player.get()));
        fn_800091EC(&up, &(const Vec3f &)fn_80079698(m_player.get()));
        float cosine;
        if (fn_8040CF90(&direction, &up) > 0.0f) {
            Vec3f across;
            fn_8040F990(0, &across, &direction, &up);
            cosine = std::fabs(fn_8040CF90(&front, &(const Vec3f &)fn_8040CEE8(&across)));
        } else {
            cosine = std::fabs(fn_8040CF90(&front, &direction));
        }
        float speed = fn_8040CF50(&m_player.get()->field_0x18);
        float t;
        if (speed < 30.0f || cosine < 0.766f) {
            t = 0.0f;
        } else {
            t = (cosine - 0.766f) / (1.0f - 0.766f);
        }
        float limit = t * (m_maxSpeed - m_minSpeed) + m_minSpeed;
        m_lean = fn_80015D68(cosine);
        cosine *= cosine;
        cosine *= cosine;
        cosine *= cosine;
        float target = cosine * (dt * ((m_maxSpeed - m_minSpeed) / 4.0f)) + m_speed;
        if (target > limit) {
            target = limit;
        }
        if (fn_8007646C(&m_path)) {
            fn_800091D0(&m_player.px->field_0x24,
                        &(const Vec3f &)fn_8040CDA8(&input, fn_8040CF50(&m_player.get()->field_0x24)));
            fn_800091D0(&m_player.px->field_0x18, &(const Vec3f &)fn_8040CDA8(&input, speed));
            if (m_speed < target) {
                m_speed = target;
            }
        } else if (fn_8006CD78(m_player.get()->m_stateBits, 0x3B)) {
            fn_800091D0(&input, &(const Vec3f &)fn_8040CDA8(&front, fn_8040CF50(&input)));
            if (m_speed < target) {
                m_speed = target;
            }
        } else {
            m_speed = target;
        }
    }
    AddVector(m_player->field_0x24, fn_800091A4(&input, m_speed));
    if (fn_8040CF50(&m_player.get()->field_0x24) > m_maxSpeed) {
        fn_800091D0(&m_player.px->field_0x24,
                    &(const Vec3f &)fn_8040CDA8(
                        &(const Vec3f &)fn_8040CEE8(&m_player.get()->field_0x24), m_maxSpeed));
    }
    fn_80079720(m_player.get());
    if (fn_8007646C(&m_path)) {
        Vec3f toPath;
        fn_800091EC(&toPath, &(const Vec3f &)Subtract(&pathPosition, &m_player.get()->m_position));
        fn_800091D0(&toPath,
                    &(const Vec3f &)fn_8040CDA8(
                        &(const Vec3f &)fn_8040CDA8(
                            &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&toPath),
                                                        fn_8040CF50(&m_player.get()->field_0x18)),
                            dt),
                        0.1f));
        AddVector(m_player->m_position, toPath);
    }
    fn_8009728C(&m_player, 0);
    if (fn_8007646C(&m_path)) {
        if (!(input.x * input.x + input.y * input.y + input.z * input.z < 0.0001f)) {
            fn_80093BA0(&m_player, &input, 15.0f * dt);
        }
    } else {
        float speedSq = fn_8040CF2C(&m_player.get()->field_0x18);
        if (speedSq <= 22500.0f) {
            m_turnRate = 15.0f;
        } else if (speedSq > 90000.0f) {
            m_turnRate = 2.0f;
        } else {
            float speed = fn_8000FEE4(speedSq);
            m_turnRate = 13.0f * ((speed - 300.0f) / -150.0f) + 2.0f;
        }
        fn_80093AF8(&m_player, m_turnRate * dt);
    }
    float speed = fn_8040CF50(&m_player.get()->field_0x18);
    bool fast = false;
    bool sparks = false;
    float rate;
    if (speed < 12.5f) {
        rate = speed / 12.5f;
    } else if (speed < 30.0f) {
        rate = (speed - 12.5f) / 17.5f;
        rate += 1.0f;
    } else if (speed < 100.0f) {
        rate = (speed - 30.0f) / 70.0f;
        rate += 2.0f;
    } else if (speed < 720.0f) {
        rate = (speed - 100.0f) / 620.0f;
        rate += 3.0f;
        sparks = true;
        fast = true;
    } else {
        rate = 4.0f;
        sparks = true;
        fast = true;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
        sparks = false;
    }
    fn_800AA7B8(m_player.get()->m_motion, rate);
    if (fast) {
        fn_80076CA8(m_player.get()->m_stateBits, 3);
    } else {
        fn_80076CC8(m_player.get()->m_stateBits, 3);
    }
    if (FieldsOf(m_player.get())->m_kind >= 0xE) {
        if (sparks) {
            fn_800A2CBC(reinterpret_cast<void *>(m_player.get()->field_0x23C));
        } else {
            fn_800A2C98(reinterpret_cast<void *>(m_player.get()->field_0x23C));
        }
    }
}

/* Sliding down a slope: the input pushes the player at a fixed speed, up to a top speed. */
void Ground::Slide(float dt) {
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x3C)) {
        m_mode = 0;
        m_step = 0;
        Run(dt);
        return;
    }
    switch (m_step) {
    case 0:
        m_player.get()->m_motion->vf_0x10(2, 15.0f);
        m_step++;
        break;
    }
    Vec3f input;
    fn_800091EC(&input, &FieldsOf(m_player.get())->field_0xB8);
    m_speed = 2200.0f;
    m_turnRate = 15.0f;
    AddVector(m_player->field_0x24, fn_800091A4(&input, m_speed));
    fn_80079720(m_player.get());
    fn_80093BA0(&m_player, &(Vec3f &)fn_80093FC0(&m_player), m_turnRate * dt);
    fn_8009728C(&m_player, 0);
    float speed = fn_8040CF50(&m_player.get()->field_0x18);
    if (speed >= s_slideSpeed) {
        LimitSlideSpeed(m_player->field_0x18, speed);
    }
    float rate = speed / s_slideSpeed;
    if (rate > 1.0f) {
        rate = 1.0f;
    }
    fn_800AA7B8(m_player.get()->m_motion, rate);
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x1B) &&
        fn_8040CF2C(&m_player.get()->field_0x18) < s_stopSpeed * s_stopSpeed) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
    }
}

/* A skid on a sharp turn at speed (mode 2, or 3). */
void Ground::Skid(float dt) {
    switch (m_step) {
    case 0: {
        m_player.get()->m_sound->StopVoice(7);
        fn_80076CA8(m_player.get()->m_stateBits, 0x1A);
        int motion = 0xA;
        PlayerMotion *playerMotion = m_player.get()->m_motion;
        if (m_mode == 2) {
            motion = 9;
        }
        playerMotion->vf_0x10(motion, 0.0f);
        m_step++;
        break;
    }
    case 1:
        if (fn_800A9E1C(&m_player)) {
            fn_80076CC8(m_player.get()->m_stateBits, 0x1A);
            m_mode = 0;
            m_step = 0;
        }
        break;
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
}

/* The mode the input asks for: a skid (2) when it turns the running player sharply round. */
int Ground::ChooseMode(const Vec3f *input) {
    if (fn_8007646C(&m_path)) {
        return 0;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x2E)) {
        return 0;
    }
    if (!fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
        return 0;
    }
    if (fn_8040CF2C(&m_player.get()->field_0x18) < 40000.0f) {
        return 0;
    }
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_player.get()));
    float cosine = fn_8040CF90(&front, &(const Vec3f &)fn_8040CEE8(input));
    if (cosine > std::cos(DegToRad(90.0f))) {
        return 0;
    }
    return 2;
}

void Ground::SetPath(const UnkCircuitTarget_800ABCE8 &path) {
    fn_80076CA8(m_player.get()->m_stateBits, 0x31);
    m_path = path;
    fn_800ABAA4(&m_path, 0);
}

void Ground::ReleasePath() {
    if (fn_8007646C(&m_path)) {
        fn_80076CC8(m_player.get()->m_stateBits, 0x31);
        fn_8007684C(&m_path);
    }
}

/* The speeds of the run: a top speed from the character's parameters, lower while carrying
 * something (state bit 0x1E).
 * NonMatching (95%): retail computes the table's address into r3 (lis and addi in one register)
 * and loads the player into r4; GC/2.7 puts the address in r4 and the player in r3. Retail has the
 * same allocation at about 25 such table reads. Not reproduced by: a local for the player, the
 * kind or the address; pointer arithmetic or `+=` on a pointer; a reference; inline accessors for
 * the table, the entry, the kind or the flag; m_player.px or operator->; a swapped subscript; an
 * enum or unsigned kind; a bounded array or a struct of arrays; other GC compiler versions. */
void Ground::SetSpeedRange() {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
        m_minSpeed = 1000.0f;
        m_maxSpeed = 3000.0f;
    } else {
        m_minSpeed = 1000.0f;
        m_maxSpeed = 15000.0f * lbl_804F1834[FieldsOf(m_player.get())->m_kind].m_runSpeed;
    }
}

/* The largest share of the top speed the velocity sets (name inferred). */
static float s_maxRatio = 1.0f;

/* The speed of the run from the player's velocity. */
void Ground::ResetSpeed() {
    float speed = fn_8040CF50(&m_player.get()->field_0x18);
    if (speed < 30.0f) {
        m_speed = m_minSpeed;
    } else {
        float t = speed / 750.0f;
        t = fn_800704AC(t, s_maxRatio);
        m_speed = Lerp(m_minSpeed, m_maxSpeed, t);
    }
}

Behavior *Ground::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Ground(player);
}

void Ground::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Ground by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0000 = {
    Player::Behavior::Ground::Create, Player::Behavior::Ground::Release, 0};

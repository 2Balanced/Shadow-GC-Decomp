/* Player::Behavior::DarkSpin (class name from RTTI): behavior 0x13 of Shadow, spinning along a
 * circuit path (the target comes with Player::Behavior::InitializeArgsDarkSpin). Entering it takes
 * the player's speed and up axis, plays the spin motion and voice; each frame it eases the speed
 * towards its maximum and moves the player along the path, with a spin effect that hides with the
 * player's weapon. At the path's end it jumps off along the path's up vector (behavior 5,
 * Player::Behavior::InitializeArgsJump). File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/GadgetSound.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/UnkCircuitTarget_800ABCE8.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8007D1DC {
    /* 0x000 */ u8 field_0x0[0xD0];
    /* 0x0D0 */ Vec3f m_prevPosition; /* where the player was before this frame (inferred) */
};

extern "C" {
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Called on the player when the spin starts and ends (behaviour unknown). */
void fn_8007922C(Player::PlayerBase *player);
void fn_800791BC(Player::PlayerBase *player);
/* The player's up axis (behaviour inferred). */
Vec3f fn_80079698(Player::PlayerBase *player);
/* Starts effect id of the player's effects at 0x23C (see Game/PlayerBarrier.cpp). */
boost::weak_ptr<EffectElement> fn_800A2854(void *effects, int arg, int id);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* The circuit target: where it is (position and the two directions of its frame; any may be 0),
 * whether it has a path, moving it on by a distance (false at the path's end), a flag, and
 * clearing it (behaviour inferred; see Game/PlayerBehaviorChaosControl.cpp). */
void fn_800ABAE8(UnkCircuitTarget_800ABCE8 *target, Vec3f *position, Vec3f *forward, Vec3f *up);
bool fn_8007646C(UnkCircuitTarget_800ABCE8 *target);
bool fn_800ABB14(UnkCircuitTarget_800ABCE8 *target, float distance);
void fn_800ABAA4(UnkCircuitTarget_800ABCE8 *target, bool on);
void fn_8007684C(UnkCircuitTarget_800ABCE8 *target);
/* Whether a value is about zero (see Game/ArkCannon.cpp). */
bool fn_8000A950(float value);

float fn_8040CF50(const Vec3f *v);                        /* |v| */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);      /* vector * scale */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8007D1DC *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

/* Degrees to radians (name inferred; as in Game/BalloonGenerator.cpp). */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* vector = value, through the game's out-of-line copy (see Game/JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The sound handle's constructor and assignment (Game/GadgetSound.h), whose out-of-line copies the
 * game emits in this unit first (0x8007D770 and 0x8007D624; they call other inline functions, so
 * they are not inlined). */
inline UnkSoundHandle_801D3DC8::UnkSoundHandle_801D3DC8()
    : m_id(0), field_0x4(0), field_0x6(0), field_0x8(0), field_0xA(0), field_0xB(0),
      field_0x18(0) {}

inline UnkSoundHandle_801D3DC8 &UnkSoundHandle_801D3DC8::operator=(
    const UnkSoundHandle_801D3DC8 &other) {
    m_id = other.m_id;
    field_0x4 = other.field_0x4;
    field_0x6 = other.field_0x6;
    field_0x8 = other.field_0x8;
    field_0xA = other.field_0xA;
    field_0xB = other.field_0xB;
    AssignVector(m_position, other.m_position);
    field_0x18 = other.field_0x18;
    return *this;
}

namespace Player {
namespace Behavior {

/* InitializeArgsDarkSpin (class name from RTTI; vtable 0x80520CEC, destructor 0x8009C220): switches
 * to the dark spin along a circuit target. */
class InitializeArgsDarkSpin : public InitializeArgs {
  public:
    virtual ~InitializeArgsDarkSpin();

    /* 0x8 */ UnkCircuitTarget_800ABCE8 m_target;
};

class DarkSpin : public Behavior {
  public:
    DarkSpin(const boost::shared_ptr<PlayerBase> &player);
    virtual ~DarkSpin();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void StopEffect();
    void StartEffect();
    void Move(float dt);
    bool Advance(float dt);

    static const char *s_name;

    /* 0x1C */ UnkSoundHandle_801D3DC8 m_voice;
    /* 0x38 */ Vec3f m_up;
    /* 0x44 */ Vec3f m_forward;
    /* 0x50 */ UnkCircuitTarget_800ABCE8 m_target;
    /* 0x58 */ float m_speed;
    /* 0x5C */ s32 m_state; /* 0 start, 1 spinning, 2 at the path's end */
    /* 0x60 */ boost::weak_ptr<EffectElement> m_effect;
};
SIZE_ASSERT(DarkSpin, 0x68);

const char *DarkSpin::s_name = "DarkSpin";

/* The speed the spin eases towards (name inferred). The game sets it in __sinit rather than in
 * .sdata, as an initializer that is not a constant expression does; the inline call reproduces
 * that (as in Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_maxSpeed = DynamicFloat(340.0f);

DarkSpin::DarkSpin(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0x13) {}

DarkSpin::~DarkSpin() {}

bool DarkSpin::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void DarkSpin::Enter(int prevId, const InitializeArgs &args) {
    m_target = static_cast<const InitializeArgsDarkSpin &>(args).m_target;
    fn_800ABAA4(&m_target, 1);
    m_speed = fn_8040CF50(&m_player.get()->field_0x18);
    fn_800091D0(&m_up, &(const Vec3f &)fn_80079698(m_player.get()));
    fn_8007922C(m_player.get());
    fn_80076CA8(m_player.get()->m_stateBits, 0x31);
    fn_80076CA8(m_player.get()->m_stateBits, 0x34);
    fn_80076CA8(m_player.get()->m_stateBits, 0x38);
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    m_player.get()->m_motion->vf_0x10(0xC, 0.0f);
    m_player.get()->m_sound->StopVoice(0x1D);
    m_voice = m_player.get()->m_sound->PlayVoice(0x1E, reinterpret_cast<s32>(this));
    m_state = 0;
}

void DarkSpin::Leave(int nextId) {
    fn_800791BC(m_player.get());
    fn_80076CC8(m_player.get()->m_stateBits, 0x31);
    fn_80076CC8(m_player.get()->m_stateBits, 0x34);
    fn_80076CA8(m_player.get()->m_stateBits, 0x32);
    fn_80076CC8(m_player.get()->m_stateBits, 0x38);
    fn_8007684C(&m_target);
    m_player.get()->m_rotation.x = DegToRad(0.0f);
    m_player.get()->m_rotation.z = DegToRad(0.0f);
    m_voice.fn_801D455C();
    StopEffect();
}

void DarkSpin::vf_0x18(CharCommand *command) {}

void DarkSpin::Update(float dt) {
    switch (m_state) {
    case 0:
        StartEffect();
        m_state = 1;
        break;
    case 1:
        if (!Advance(dt)) {
            m_state = 2;
        }
        break;
    case 2: {
        Vec3f direction;
        if (fn_8007646C(&m_target)) {
            fn_800ABAE8(&m_target, 0, 0, &direction);
        } else {
            fn_800091D0(&direction, &(const Vec3f &)fn_80079698(m_player.get()));
        }
        m_player.get()->m_sound->StopVoice(0x1F);
        m_voice.fn_801D455C();
        StopEffect();
        InitializeArgsJump args(direction);
        fn_800A7F14(m_player.get()->m_behaviors, &args);
        return;
    }
    }
    if (!m_effect.expired()) {
        if (m_player.get()->IsWeaponHidden()) {
            m_effect.lock()->Hide();
        } else {
            m_effect.lock()->Show();
        }
    }
}

/* Moves the speed towards its maximum and the player along the path; false at the path's end. */
bool DarkSpin::Advance(float dt) {
    m_speed += 0.05f * (s_maxSpeed - m_speed);
    if (fn_8007646C(&m_target) && fn_800ABB14(&m_target, dt * m_speed)) {
        Move(dt);
    } else {
        return false;
    }
    if (!fn_8000A950(dt)) {
        const Vec3f &velocity =
            fn_8040CDA8(&(const Vec3f &)Subtract(&m_player.get()->m_position,
                                                 &FieldsOf(m_player.get())->m_prevPosition),
                        1.0f / dt);
        fn_800091D0(&m_player.get()->field_0x18, &velocity);
    }
    return true;
}

/* Places the player where the target is on its path, turned along it. */
void DarkSpin::Move(float dt) {
    Vec3f position;
    fn_800ABAE8(&m_target, &position, &m_forward, &m_up);
    fn_800091D0(&m_player.get()->m_position, &position);
    m_player->m_rotation = fn_8040F364(&m_forward, &m_up);
}

void DarkSpin::StartEffect() {
    if (m_effect.expired()) {
        m_effect = fn_800A2854(reinterpret_cast<void *>(m_player.get()->field_0x23C), 0, 0x5E);
    }
}

void DarkSpin::StopEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
        m_effect.reset();
    }
}

void DarkSpin::vf_0x20() {}

Behavior *DarkSpin::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new DarkSpin(player);
}

void DarkSpin::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds DarkSpin by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AFACC = {
    Player::Behavior::DarkSpin::Create, Player::Behavior::DarkSpin::Release, 0};

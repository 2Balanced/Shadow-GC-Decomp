/* Player::Behavior::SpinDash (class name from RTTI): behavior 0x12 of Shadow, the spin dash.
 * Entering it stops the player, plays the curl motion and its voice and starts the spin effect,
 * which follows the motion's root node; the player crouches (lowering its height, the float at
 * 0x274, as Player::Behavior::Sliding does), charges while turning, and command 0x107 releases it:
 * the player shoots forwards, faster the longer it charged, and rolls (on a slope the velocity
 * turns with the player) until the roll time runs out or it slows down, standing up where there is
 * room (behavior 1) and rolling on otherwise. A roll that leaves the ground falls (behavior 2); a
 * charge longer than 10 seconds stops (behavior 0). File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). Where the game reloads the player
 * pointer in a call's arguments after another argument's call, the code reads m_player.px (as in
 * Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Effect/System/EffectElement.h"
#include "Game/CharCommand.h"
#include "Game/GadgetSound.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Effect::System::EffectElement;

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8008ED8C {
    /* 0x000 */ u8 field_0x0[0x1C8];
    /* 0x1C8 */ Vec3f m_groundNormal; /* (as in Game/PlayerBehaviorHomingAttack.cpp) */
};

/* A node of the player's skeleton (as in Game/EmbeddedWeapon.cpp). */
struct UnkMotionNode_800AA760 {
    /* 0x00 */ const Mtx34f *m_matrix;
};

extern "C" {
/* The player's flag word and state bits (see Game/PlayerShadowSuper.cpp). */
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Called with the player's object at 0x234: stopping and starting its effect (behaviour inferred;
 * see Game/PlayerBehaviorFall.cpp). */
void fn_8007792C(void *object);
void fn_80077950(void *object, int arg);
/* The player's front axis (see Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player by its turn rate (behaviour inferred; as in Super), and towards a direction
 * (see Game/PlayerBehaviorAirAttack.cpp). */
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
void fn_80093BA0(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *direction, float rate);
/* Whether nothing is above the player within height (see Game/PlayerBehaviorSliding.cpp). */
bool fn_80096D3C(boost::shared_ptr<Player::PlayerBase> *player, float height);
/* Starts effect id of the player's effects at 0x23C at a position and rotation (behaviour
 * inferred; a sibling of fn_800A2854, see Game/PlayerBarrier.cpp). */
boost::weak_ptr<EffectElement> fn_800A2794(void *effects, const Vec3f *position,
                                           const Vec3f *rotation, int id);
/* Stopping and starting the slide effect of the player's effects at 0x23C (as in
 * Game/PlayerBehaviorSliding.cpp). */
void fn_800A2D30(void *effects);
void fn_800A2D54(void *effects, float length);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* The motion's skeleton node index (as in Game/EmbeddedWeapon.cpp). */
UnkMotionNode_800AA760 *fn_800AA760(Player::PlayerMotion *motion, int index);
/* Rumbles player's controller with pattern (see Game/PlayerBehaviorHomingJump.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);
/* Makes the effect follow matrix (see Game/PlayerBarrier.cpp). */
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
void fn_8040CE04(Vec3f *vector, const Vec3f *value); /* vector -= value */
void fn_8040CE60(Vec3f *vector);                     /* vector = 0 */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
float fn_8040CF2C(const Vec3f *v);                   /* |v|^2 */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
Vec3f fn_8040D044(const Vec3f *vector, float angle); /* turned about z */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about y */
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* turned about x */
/* Splits vector into its parts along normal and across it (see Game/PlayerBehaviorGrind.cpp). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8008ED8C *>(player)

/* Sets the player's height (an inline function in the game, as in Game/PlayerBehaviorSliding.cpp:
 * the shared_ptr's operator-> before it stays out of line). */
inline void SetHeight(Player::PlayerBase *player, float height) {
    player->field_0x274 = height;
}

/* vector -= value (an inline wrapper; the player's operator-> in its arguments stays out of
 * line). */
inline void SubtractVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CE04(&vector, &value);
}

/* An inline function with an empty body in this build (name and purpose unknown): the game still
 * calls the shared_ptr's operator-> for its argument after turning a roll's velocity. */
inline void Unused(const Vec3f &vector) {}

namespace Player {
namespace Behavior {

class SpinDash : public Behavior {
  public:
    SpinDash(const boost::shared_ptr<PlayerBase> &player);
    virtual ~SpinDash();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    bool CanStand();
    bool ChangeHeight(bool lower);
    void StopEffect();
    void StartEffect();
    void UpdateRoll(float dt);
    void UpdateCharge(float dt);
    void UpdateCrouch();

    static const char *s_name;

    /* 0x1C */ s32 m_mode;      /* 0 crouching, 1 charging, 2 rolling */
    /* 0x20 */ float m_height;  /* the player's height before the spin dash */
    /* 0x24 */ float m_charge;  /* how long the player charged */
    /* 0x28 */ float m_rollTime; /* left of the roll */
    /* 0x2C */ bool m_released; /* command 0x107 came */
    /* 0x30 */ UnkSoundHandle_801D3DC8 m_voice;
    /* 0x4C */ boost::weak_ptr<EffectElement> m_effect; /* the spin effect */
};
SIZE_ASSERT(SpinDash, 0x54);

const char *SpinDash::s_name = "SpinDash";

/* The fastest a roll leaves the ground (name inferred). */
static float s_maxRollSpeed = 270.0f;

/* The height the spin dash lowers the player to and the change per frame (names inferred; const,
 * as the game hoists their loads above stack stores). The game sets them in __sinit rather than in
 * .sbss, as an initializer that is not a constant expression does; the inline call reproduces that
 * (as in Game/PlayerBehaviorFall.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static const float s_lowHeight = DynamicFloat(2.0f);
static const float s_heightStep = DynamicFloat(0.25f);

SpinDash::SpinDash(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0x12) {}

SpinDash::~SpinDash() {}

bool SpinDash::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void SpinDash::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CA8(m_player.get()->m_stateBits, 0x3E);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    m_player.get()->m_motion->vf_0x10(0xC, 5.0f);
    fn_8040CE60(&m_player.get()->field_0x18);
    m_height = m_player.get()->field_0x274;
    m_charge = 0.0f;
    m_mode = 0;
    m_released = false;
    m_voice = m_player.get()->m_sound->PlayVoice(0xC, reinterpret_cast<s32>(this));
    m_effect = fn_800A2794(reinterpret_cast<void *>(m_player.get()->field_0x23C),
                           &m_player.get()->m_position, &m_player.get()->m_rotation, 0x31F);
}

void SpinDash::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0x3E);
    fn_8006127C(&m_player.get()->m_flags, 1);
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 9);
    SetHeight(m_player.operator->(), m_height);
    StopEffect();
    m_voice.fn_801D455C();
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
    }
}

void SpinDash::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x107:
        m_released = true;
        m_voice.fn_801D455C();
        command->Flags() |= 2;
        break;
    case 0x104:
        if (!CanStand()) {
            command->Flags() |= 2;
        }
        break;
    case 0x113:
        if (!CanStand()) {
            m_player.get()->m_sound->StopVoice(0);
            command->Flags() |= 2;
        }
        break;
    }
}

void SpinDash::Update(float dt) {
    switch (m_mode) {
    case 0:
        UpdateCrouch();
        break;
    case 1:
        UpdateCharge(dt);
        break;
    case 2:
        UpdateRoll(dt);
        break;
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (!m_effect.expired()) {
        boost::shared_ptr<EffectElement> effect = m_effect.lock();
        fn_80425E68(effect.px, fn_800AA760(m_player.get()->m_motion, 0)->m_matrix);
        if (m_player.get()->IsWeaponHidden()) {
            effect->Hide();
        } else {
            effect->Show();
        }
    }
    if (m_mode == 2) {
        Vec3f across;
        Vec3f along;
        fn_8040F990(&along, &across, &m_player.get()->field_0x18,
                    &FieldsOf(m_player.get())->m_groundNormal);
        float acrossSpeed = fn_8040CF2C(&across);
        if (fn_8040CF2C(&along) > acrossSpeed && CanStand()) {
            if (fn_8040CF2C(&along) > s_maxRollSpeed * s_maxRollSpeed) {
                fn_800091D0(&along, &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_8040CEE8(&along),
                                                                s_maxRollSpeed));
                fn_800091D0(&m_player.px->field_0x18, &(const Vec3f &)fn_8040CDD0(&along, &across));
            }
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
            return;
        }
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        fn_80014E1C(&m_player.get()->m_flags, 1);
    } else {
        fn_8006127C(&m_player.get()->m_flags, 1);
    }
}

void SpinDash::UpdateCrouch() {
    if (ChangeHeight(true)) {
        m_mode = 1;
    }
}

void SpinDash::UpdateCharge(float dt) {
    m_charge += dt;
    if (m_charge > 10.0f) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
    }
    fn_80093AF8(&m_player, 2.0f * dt);
    if (m_released) {
        float charge = m_charge > 1.0f ? 1.0f : m_charge;
        fn_80076CA8(m_player.get()->m_stateBits, 9);
        StartEffect();
        m_rollTime = 2.0f * charge;
        fn_800091D0(&m_player.px->field_0x18,
                    &(const Vec3f &)fn_8040CDA8(&(const Vec3f &)fn_800796DC(m_player.get()),
                                                850.0f * charge + 150.0f));
        m_player.get()->m_sound->StopVoice(0xD);
        fn_8034DF48(1, m_player.get()->field_0x54);
        fn_80076CC8(m_player.get()->m_stateBits, 0x3E);
        m_mode = 2;
    }
}

void SpinDash::UpdateRoll(float dt) {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
        /* On a slope the velocity turns with the player: into the old rotation's frame, then out
         * of the new one's (as in Game/PlayerBehaviorSliding.cpp). */
        Vec3f rotation = m_player->m_rotation;
        fn_80093AF8(&m_player, 2.0f * dt);
        fn_800091D0(&m_player.get()->field_0x18,
                    &(const Vec3f &)fn_8040D0AC(&m_player.get()->field_0x18, -rotation.y));
        fn_800091D0(&m_player.get()->field_0x18,
                    &(const Vec3f &)fn_8040D118(&m_player.get()->field_0x18, -rotation.x));
        fn_800091D0(&m_player.get()->field_0x18,
                    &(const Vec3f &)fn_8040D044(&m_player.get()->field_0x18, -rotation.z));
        {
            Vec3f &velocity = m_player.px->field_0x18;
            fn_800091D0(&velocity,
                        &(const Vec3f &)fn_8040D044(&velocity, m_player.px->m_rotation.z));
        }
        {
            Vec3f &velocity = m_player.px->field_0x18;
            fn_800091D0(&velocity,
                        &(const Vec3f &)fn_8040D118(&velocity, m_player.px->m_rotation.x));
        }
        {
            Vec3f &velocity = m_player.px->field_0x18;
            fn_800091D0(&velocity,
                        &(const Vec3f &)fn_8040D0AC(&velocity, m_player.px->m_rotation.y));
        }
        Unused(m_player->field_0x18);
    } else {
        fn_80093BA0(&m_player, &m_player.get()->field_0x18, 2.0f * dt);
    }
    if (fn_8040CF90(&m_player.get()->field_0x18, &FieldsOf(m_player.get())->m_groundNormal) >=
        0.0f) {
        Vec3f push;
        fn_800091EC(&push,
                    &(const Vec3f &)fn_8040CDA8(
                        &FieldsOf(m_player.get())->m_groundNormal,
                        fn_8040CF90(&m_player.get()->field_0x18,
                                    &FieldsOf(m_player.get())->m_groundNormal)));
        SubtractVector(m_player->field_0x18, push);
    }
    float speed = fn_8040CF2C(&m_player.get()->field_0x18);
    m_rollTime -= dt;
    if (m_rollTime <= 0.0f || speed < 22500.0f) {
        if (fn_800A9E1C(&m_player) && CanStand()) {
            StopEffect();
            fn_800A7F14(m_player.get()->m_behaviors,
                        &(const InitializeArgs &)InitializeArgsGround(false));
        } else {
            fn_800091D0(&m_player.px->field_0x18,
                        &(const Vec3f &)fn_8040CDA8(
                            &(const Vec3f &)fn_8040CEE8(&m_player.get()->field_0x18), 150.0f));
            m_rollTime = 0.0f;
        }
    }
}

void SpinDash::StartEffect() {
    fn_800A2D54(reinterpret_cast<void *>(m_player.get()->field_0x23C),
                s_lowHeight - 0.099999994f);
    fn_80076CA8(m_player.get()->m_stateBits, 0x35);
    fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 5);
}

void SpinDash::StopEffect() {
    fn_800A2D30(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    fn_80076CC8(m_player.get()->m_stateBits, 0x35);
    fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
}

bool SpinDash::ChangeHeight(bool lower) {
    bool reached = false;
    float height = m_player.get()->field_0x274;
    if (lower) {
        height -= s_heightStep;
        if (height <= s_lowHeight) {
            height = s_lowHeight;
            reached = true;
        }
    } else {
        height += s_heightStep;
        if (height >= m_height) {
            height = m_height;
            reached = true;
        }
    }
    SetHeight(m_player.operator->(), height);
    return reached;
}

bool SpinDash::CanStand() {
    return fn_80096D3C(&m_player, 2.0f * m_height - m_player.get()->field_0x274);
}

void SpinDash::vf_0x20() {}

Behavior *SpinDash::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new SpinDash(player);
}

void SpinDash::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds SpinDash by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0DB4 = {
    Player::Behavior::SpinDash::Create, Player::Behavior::SpinDash::Release, 0};

/* Player::Behavior::Sliding (class name from RTTI): behavior 7 of Shadow, sliding on the ground.
 * Entering it plays the slide motion, starts the slide effect and lowers the player's height (the
 * float at 0x274) step by step; on a slope (state bit 0x1B) the velocity turns with the player,
 * elsewhere the player turns towards it. Once fn_800A9E1C has fired and the player is slow, it
 * stands up where there is room above (raising the height again, then running on: behavior 1) or
 * keeps sliding at a set speed. Command 0x104 jumps out of the slide when there is room, and the
 * slide ends when the velocity leaves the ground. File name inferred from the class (as
 * Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerSoundBase.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8008DCD4 {
    /* 0x000 */ u8 field_0x0[0x1C8];
    /* 0x1C8 */ Vec3f m_groundNormal; /* (as in Game/PlayerBehaviorHomingAttack.cpp) */
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
/* The player's up and front axes (see Game/PlayerBehaviorGrind.cpp). */
Vec3f fn_80079698(Player::PlayerBase *player);
Vec3f fn_800796DC(Player::PlayerBase *player);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player by its turn rate (behaviour inferred; as in Super), and towards a direction
 * (see Game/PlayerBehaviorAirAttack.cpp). */
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
void fn_80093BA0(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *direction, float rate);
/* Whether nothing is above the player within height along its up axis (behaviour from its body:
 * it casts a line upwards). */
bool fn_80096D3C(boost::shared_ptr<Player::PlayerBase> *player, float height);
/* Stopping and starting the slide effect of the player's effects at 0x23C (behaviour inferred). */
void fn_800A2D30(void *effects);
void fn_800A2D54(void *effects, float length);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Sets the motion's float at 0x438 (its speed, going by the value 1; behaviour from its body). */
void fn_800AA7B8(Player::PlayerMotion *motion, float speed);

Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalized */
float fn_8040CF2C(const Vec3f *v);                   /* |v|^2 */
void fn_8040CE74(Vec3f *vector);                     /* normalises it */
Vec3f fn_8040D044(const Vec3f *vector, float angle); /* turned about z */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about y */
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* turned about x */
/* Splits vector into its parts along normal and across it (see Game/PlayerBehaviorGrind.cpp). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8008DCD4 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). The
 * inline wrapper orders the arguments as the game does. */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
inline Vec3f Subtract(const Vec3f &lhs, const Vec3f &rhs) {
    return reinterpret_cast<VectorSubtract>(fn_8040CD74)(&lhs, &rhs);
}

/* Sets the player's height (an inline function in the game: the shared_ptr's operator-> before it
 * stays out of line). */
inline void SetHeight(Player::PlayerBase *player, float height) {
    player->field_0x274 = height;
}

namespace Player {
namespace Behavior {

class Sliding : public Behavior {
  public:
    Sliding(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Sliding();
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
    void UpdateStand();
    void UpdateSlide(float dt);

    static const char *s_name;

    /* 0x1C */ float m_height; /* the player's height before the slide */
    /* 0x20 */ s32 m_mode;     /* 0 sliding, 1 standing up */
    /* 0x24 */ s32 m_endCount; /* how often fn_800A9E1C fired */
};
SIZE_ASSERT(Sliding, 0x28);

const char *Sliding::s_name = "Sliding";

/* The height the slide lowers the player to and the change per frame (names inferred). The game
 * sets them in __sinit rather than in .sbss, as an initializer that is not a constant expression
 * does; the inline call reproduces that (as in Game/PlayerBehaviorFall.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static const float s_lowHeight = DynamicFloat(2.0f);
static const float s_heightStep = DynamicFloat(10.0f);

Sliding::Sliding(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 7) {}

Sliding::~Sliding() {}

bool Sliding::vf_0x0C(int prevId, const InitializeArgs &args) {
    return prevId != 7;
}

void Sliding::Enter(int prevId, const InitializeArgs &args) {
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80076CA8(m_player.get()->m_stateBits, 7);
    fn_80014E1C(&m_player.get()->m_flags, 1);
    m_player.get()->m_motion->vf_0x10(8, 10.0f);
    fn_800AA7B8(m_player.get()->m_motion, 1.0f);
    m_height = m_player.get()->field_0x274;
    m_mode = 0;
    m_endCount = 0;
    m_player.get()->m_sound->StopVoice(9);
    StartEffect();
}

void Sliding::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 7);
    fn_8006127C(&m_player.get()->m_flags, 1);
    SetHeight(m_player.operator->(), m_height);
    StopEffect();
}

void Sliding::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x104:
        if (CanStand()) {
            Vec3f direction;
            fn_800091EC(&direction,
                        &(const Vec3f &)Subtract(
                            fn_8040CDA8(&(const Vec3f &)fn_80079698(m_player.get()), 2.0f),
                            fn_800796DC(m_player.get())));
            fn_8040CE74(&direction);
            fn_800A7F14(m_player.get()->m_behaviors,
                        &(const InitializeArgs &)InitializeArgsJump(direction));
        }
        command->Flags() |= 2;
        break;
    case 0x113:
        if (!CanStand()) {
            m_player.get()->m_sound->StopVoice(0);
            command->Flags() |= 2;
        }
        break;
    }
}

void Sliding::Update(float dt) {
    switch (m_mode) {
    case 0:
        UpdateSlide(dt);
        break;
    case 1:
        UpdateStand();
        break;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        fn_80014E1C(&m_player.get()->m_flags, 1);
    } else {
        fn_8006127C(&m_player.get()->m_flags, 1);
    }
    Vec3f across;
    Vec3f along;
    fn_8040F990(&along, &across, &m_player.get()->field_0x18,
                &FieldsOf(m_player.get())->m_groundNormal);
    float acrossSpeed = fn_8040CF2C(&across);
    if (fn_8040CF2C(&along) > acrossSpeed) {
        if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
            fn_800A7F14(m_player.get()->m_behaviors,
                        &(const InitializeArgs &)InitializeArgsGround(false));
        } else {
            fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        }
    }
}

void Sliding::UpdateSlide(float dt) {
    ChangeHeight(true);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1B)) {
        /* On a slope the velocity turns with the player: into the old rotation's frame, then out
         * of the new one's. */
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
    } else {
        fn_80093BA0(&m_player, &m_player.get()->field_0x18, 2.0f * dt);
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (fn_800A9E1C(&m_player)) {
        m_endCount++;
    }
    if (m_endCount > 0 && fn_8040CF2C(&m_player.get()->field_0x18) < 22500.0f) {
        if (CanStand()) {
            StopEffect();
            m_player.get()->m_motion->vf_0x10(0, 5.0f);
            m_mode = 1;
        } else {
            fn_800091D0(&m_player.px->field_0x18,
                        &(const Vec3f &)fn_8040CDA8(
                            &(const Vec3f &)fn_8040CEE8(&m_player.get()->field_0x18), 150.0f));
        }
    }
}

void Sliding::UpdateStand() {
    if (ChangeHeight(false)) {
        fn_800A7F14(m_player.get()->m_behaviors,
                    &(const InitializeArgs &)InitializeArgsGround(false));
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
}

void Sliding::StartEffect() {
    fn_800A2D54(reinterpret_cast<void *>(m_player.get()->field_0x23C),
                s_lowHeight - 0.099999994f);
    fn_80076CA8(m_player.get()->m_stateBits, 0x35);
    fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 6);
}

void Sliding::StopEffect() {
    fn_800A2D30(reinterpret_cast<void *>(m_player.get()->field_0x23C));
    fn_80076CC8(m_player.get()->m_stateBits, 0x35);
    fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
}

bool Sliding::ChangeHeight(bool lower) {
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

bool Sliding::CanStand() {
    return fn_80096D3C(&m_player, 2.0f * m_height - m_player.get()->field_0x274);
}

void Sliding::vf_0x20() {}

Behavior *Sliding::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Sliding(player);
}

void Sliding::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Sliding by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B0D08 = {
    Player::Behavior::Sliding::Create, Player::Behavior::Sliding::Release, 0};

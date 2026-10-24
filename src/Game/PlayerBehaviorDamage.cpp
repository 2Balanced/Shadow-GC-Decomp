/* Player::Behavior::Damage (class name from RTTI): behavior 0x1E of Shadow, being hurt. Entering it
 * turns the player against the hit's direction (Player::Behavior::InitializeArgsDamage), plays the
 * damage motion and knocks the player back and up; in the air (state bit 0x25) it knocks the player
 * back only, and on landing it pushes off once more and switches to behavior 0x22 or 2. File name
 * inferred from the class (as Game/PlayerBehaviorSuper.cpp); all other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/CharCommand.h"
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

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8007C9F4 {
    /* 0x000 */ u8 field_0x0[0xF4];
    /* 0x0F4 */ Vec3f m_up;
    /* 0x100 */ u8 field_0x100[0x248 - 0x100];
    /* 0x248 */ void *field_0x248; /* passed to fn_80095AD0 (meaning unknown) */
};

extern "C" {
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Keeps the player's word at 0x27C at least time (behaviour from its body; meaning unknown). */
void fn_80079028(Player::PlayerBase *player, float time);
/* The player's gravity direction, front and up axes (behaviour inferred). */
Vec3f fn_80078D58(Player::PlayerBase *player);
Vec3f fn_800796DC(Player::PlayerBase *player);
Vec3f fn_80079698(Player::PlayerBase *player);
/* Player updates run before the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player towards a direction (behaviour inferred; as in Super). */
void fn_80093BA0(boost::shared_ptr<Player::PlayerBase> *player, Vec3f *direction, float rate);
/* Called with the player's object at 0x248 (behaviour unknown). */
void fn_80095AD0(void *object);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Whether the player has landed, and the rings it drops (behaviour inferred). */
bool fn_800B7B58(boost::shared_ptr<Player::PlayerBase> *player);
void fn_800B7D48(boost::shared_ptr<Player::PlayerBase> *player, int count);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Rumbles the controller (see Game/VibrationRequest.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int pad);

Vec3f fn_80016284(const Vec3f *v);                   /* -v */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);   /* dot product */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8007C9F4 *>(player)

/* fn_8040CD74 returns lhs - rhs through its first argument, as a function returning a Vec3f does;
 * called this way the result is a temporary, as in the game (see Game/PlayerBehaviorSuper.cpp). */
typedef Vec3f (*VectorSubtract)(const Vec3f *lhs, const Vec3f *rhs);
#define Subtract(lhs, rhs) (reinterpret_cast<VectorSubtract>(fn_8040CD74)(lhs, rhs))

inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

inline Vec3f Scale(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

namespace Player {
namespace Behavior {

class Damage : public Behavior {
  public:
    Damage(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Damage();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void Land();

    static const char *s_name;

    /* 0x1C */ s32 m_state; /* 0 start, 1 knocked, 2 landed */
    /* 0x20 */ s32 m_step;
    /* 0x24 */ float m_time;
};
SIZE_ASSERT(Damage, 0x28);

const char *Damage::s_name = "Damage";

/* The speed the player is knocked back with in the air (name inferred). The game sets it in
 * __sinit rather than in .sdata, as an initializer that is not a constant expression does; the
 * inline call reproduces that (as in Game/PlayerBehaviorSuper.cpp). */
inline float DynamicFloat(float value) {
    return value;
}
static float s_airKnockSpeed = DynamicFloat(700.0f);

Damage::Damage(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 0x1E) {}

Damage::~Damage() {}

void Damage::vf_0x18(CharCommand *command) {}

bool Damage::vf_0x0C(int prevId, const InitializeArgs &args) {
    return true;
}

void Damage::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsDamage &damage = dynamic_cast<const InitializeArgsDamage &>(args);
    m_player.get()->vf_0x48();
    fn_80076CA8(m_player.get()->m_stateBits, 0x11);
    fn_80076CA8(m_player.get()->m_stateBits, 0x44);
    fn_80095AD0(FieldsOf(m_player.get())->field_0x248);
    if (!(damage.m_direction.x * damage.m_direction.x + damage.m_direction.y * damage.m_direction.y +
              damage.m_direction.z * damage.m_direction.z <
          0.0001f)) {
        fn_80093BA0(&m_player, &(Vec3f &)fn_80016284(&damage.m_direction), 1.0f);
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x25)) {
        m_player.get()->m_motion->vf_0x10(0x2D, 5.0f);
        fn_80076CC8(m_player.get()->m_stateBits, 2);
        const Vec3f &velocity =
            fn_8040CDA8(&(const Vec3f &)fn_800796DC(m_player.get()), -s_airKnockSpeed);
        fn_800091D0(&m_player.get()->field_0x18, &velocity);
    } else {
        fn_80076CA8(m_player.get()->m_stateBits, 0);
        m_player.get()->m_motion->vf_0x10(0x29, 5.0f);
        fn_80076CC8(m_player.get()->m_stateBits, 2);
        const Vec3f &velocity =
            Subtract(&(const Vec3f &)Scale(fn_80079698(m_player.get()), 100.0f),
                     &(const Vec3f &)Scale(fn_800796DC(m_player.get()), 100.0f));
        fn_800091D0(&m_player.get()->field_0x18, &velocity);
        fn_800B7D48(&m_player, 10);
    }
    m_player.get()->m_sound->StopVoice(0xF);
    fn_8034DF48(4, m_player.get()->field_0x54);
    m_state = 0;
    m_step = 0;
    m_time = 0.0f;
}

void Damage::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 0x11);
    fn_80076CC8(m_player.get()->m_stateBits, 0x44);
    fn_80079028(m_player.get(), 2.0f);
}

void Damage::Update(float dt) {
    m_time += dt;
    switch (m_state) {
    case 0:
        m_state = 1;
        m_step = 0;
        break;
    case 1:
        if (!fn_8006CD78(m_player.get()->m_stateBits, 0x25) &&
            !fn_8006CD78(m_player.get()->m_stateBits, 2) && m_time >= 1.5f) {
            InitializeArgs args(2);
            fn_800A7F14(m_player.get()->m_behaviors, &args);
        } else if (fn_800A9E1C(&m_player)) {
            m_state = 2;
            m_step = 0;
        }
        break;
    case 2:
        Land();
        break;
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
}

/* After the landing: pushes off along the player's up axis and away from gravity, then waits until
 * the player is down to switch (behaviour inferred). */
void Damage::Land() {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x25)) {
        InitializeArgs args(0x22);
        fn_800A7F14(m_player.get()->m_behaviors, &args);
    } else {
        fn_80079028(m_player.get(), 2.0f);
        switch (m_step) {
        case 0: {
            fn_80076CC8(m_player.get()->m_stateBits, 2);
            AddVector(m_player->field_0x18,
                      fn_800091A4(&FieldsOf(m_player.operator->())->m_up, 130.0f));
            Vec3f gravity;
            fn_800091EC(&gravity, &(const Vec3f &)fn_80078D58(m_player.get()));
            if (fn_8040CF90(&gravity, &m_player.get()->field_0x18) >= 0.0f) {
                AddVector(m_player->field_0x18, gravity);
            }
            m_player.get()->m_motion->vf_0x10(0xC, 15.0f);
            m_step++;
            break;
        }
        case 1:
            if (fn_800B7B58(&m_player) || fn_8006CD78(m_player.get()->m_stateBits, 2)) {
                fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
                m_step++;
            }
            break;
        }
    }
}

Behavior *Damage::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new Damage(player);
}

void Damage::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds Damage by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804AFA7C = {
    Player::Behavior::Damage::Create, Player::Behavior::Damage::Release, 0};

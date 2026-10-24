/* Player::Behavior::HomingJump (class name from RTTI): behavior 9 of Shadow, the jump that follows a
 * homing attack (Player::Behavior::InitializeArgsHomingJump). Entering it throws the player along
 * the given direction (or the player's own jump direction), higher after a hit, plays the jump
 * motion, rumbles the player's controller and keeps pushing the player for 0.75 seconds; it falls
 * (behavior 2) once the player moves down and lands (behavior 3) on the ground. File name inferred
 * from the class (as Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). Where the game
 * reloads the player pointer in a call's arguments after another argument's call, the code reads
 * m_player.px (as in Game/PlayerBehaviorGrind.cpp). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerWeaponBase.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8008697C {
    /* 0x000 */ u8 field_0x0[0xE8];
    /* 0x0E8 */ Vec3f m_jumpUp; /* the direction jumps go (as in Game/PlayerBehaviorHomingAttack.cpp) */
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
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);
/* Whether the player falls: in the air and moving against its jump direction (behaviour
 * inferred from its body). */
bool fn_800B7B58(boost::shared_ptr<Player::PlayerBase> *player);
/* Starts vibration pattern on the controller of player (see Game/BirdBase.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int player);

Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_8008697C *>(player)

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

class HomingJump : public Behavior {
  public:
    HomingJump(const boost::shared_ptr<PlayerBase> &player);
    virtual ~HomingJump();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    static const char *s_name;

    /* 0x1C */ s32 m_step;   /* 0 while the push lasts, then 1 */
    /* 0x20 */ float m_time; /* the push's time left */
    /* 0x24 */ Vec3f m_push; /* added to the player's force each frame of the push */
};
SIZE_ASSERT(HomingJump, 0x30);

const char *HomingJump::s_name = "HomingJump";

HomingJump::HomingJump(const boost::shared_ptr<PlayerBase> &player) : Behavior(player, s_name, 9) {}

HomingJump::~HomingJump() {}

bool HomingJump::vf_0x0C(int prevId, const InitializeArgs &args) {
    return prevId != 9;
}

void HomingJump::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsHomingJump &jump = dynamic_cast<const InitializeArgsHomingJump &>(args);
    Vec3f direction;
    fn_800091EC(&direction, reinterpret_cast<const Vec3f *>(&jump.m_direction));
    if (direction.x * direction.x + direction.y * direction.y + direction.z * direction.z <
        0.0001f) {
        fn_800091D0(&direction, &FieldsOf(m_player.get())->m_jumpUp);
    }
    if (jump.m_hit) {
        m_time = 0.75f;
        fn_800091D0(&m_player.px->field_0x18, &(const Vec3f &)fn_8040CDA8(&direction, 200.0f));
        AssignVector(m_push, fn_8040CDA8(&direction, 180.0f));
    } else {
        m_time = 0.75f;
        fn_800091D0(&m_player.px->field_0x18, &(const Vec3f &)fn_8040CDA8(&direction, 150.0f));
        AssignVector(m_push, fn_8040CDA8(&direction, 100.0f));
    }
    fn_80076CC8(m_player.get()->m_stateBits, 2);
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80014E1C(&m_player.get()->m_flags, 2);
    fn_80014E1C(&m_player.get()->m_flags, 6);
    fn_80076CA8(m_player.get()->m_stateBits, 0x35);
    fn_80077950(reinterpret_cast<void *>(m_player.get()->field_0x234), 4);
    m_player.get()->m_motion->vf_0x10(0x1D, 2.0f);
    m_step = 0;
    fn_8034DF48(1, m_player.get()->field_0x54);
}

void HomingJump::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_8006127C(&m_player.get()->m_flags, 2);
    fn_8006127C(&m_player.get()->m_flags, 6);
    fn_80076CC8(m_player.get()->m_stateBits, 0x35);
    fn_8007792C(reinterpret_cast<void *>(m_player.get()->field_0x234));
}

void HomingJump::vf_0x18(CharCommand *command) {}

void HomingJump::Update(float dt) {
    switch (m_step) {
    case 0:
        m_time -= dt;
        if (m_time < 0.0f) {
            m_step++;
        } else {
            AddVector(m_player->field_0x24, m_push);
        }
        break;
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (fn_800B7B58(&m_player)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    }
    if (fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(3));
    }
}

Behavior *HomingJump::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new HomingJump(player);
}

void HomingJump::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds HomingJump by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B04CC = {
    Player::Behavior::HomingJump::Create, Player::Behavior::HomingJump::Release, 0};

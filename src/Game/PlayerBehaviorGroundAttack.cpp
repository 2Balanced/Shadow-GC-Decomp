/* Player::Behavior::GroundAttack (class name from RTTI): behavior 0x19 of Shadow, the attack combo
 * on the ground. Entering it plays the attack motion (or the weapon's when the player holds a kind
 * 5 weapon) and listens to the motion's events; while the first steps play the player keeps moving
 * along the input. A command chains the next step, another (with event 0x5000's value 1) moves on;
 * at the end it runs on (behavior 1, Player::Behavior::InitializeArgsGround), falls (behavior 2),
 * lands (behavior 3), stops (behavior 0) or follows up (behavior 0x12). File name inferred from the
 * class (as Game/PlayerBehaviorSuper.cpp); all other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
/* Declares Player::Behavior::Behavior's slots with their arguments (see Game/PlayerBehavior.h). */
#define PLAYER_BEHAVIOR_SLOT_ARGS
#include <boost/bind.hpp>
#include <boost/function.hpp>

/* Defines InitializeArgsGround's inline functions, whose out-of-line copies this unit emits. */
#define PLAYER_BEHAVIOR_ARGS_INLINE

#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerBehavior.h"
#include "Game/PlayerMotionEvent.h"
#include "Game/PlayerWeaponBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* The player's members read here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_80084170 {
    /* 0x000 */ u8 field_0x0[0xB8];
    /* 0x0B8 */ Vec3f field_0xB8; /* the input's direction (as in Game/PlayerBehaviorEdgeHang.cpp) */
    /* 0x0C4 */ Vec3f field_0xC4; /* (as in Game/PlayerBehaviorGrind.cpp) */
};

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it cleared, fn_80014E10 clears it, fn_80014E1C sets
 * bit, fn_8006127C clears bit and fn_80014E34 tests bit (see Game/BAMine.cpp). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* The player's state bits (see Game/PlayerShadowSuper.cpp). */
bool fn_8006CD78(u32 *bits, int bit);
void fn_80076CA8(u32 *bits, int bit);
void fn_80076CC8(u32 *bits, int bit);
/* Player updates run before and after the behavior's own (behaviour unknown; as in FirstMeet). */
void fn_80079720(Player::PlayerBase *player);
void fn_8009728C(boost::shared_ptr<Player::PlayerBase> *player, int arg1);
/* Turning the player by its turn rate (behaviour inferred; as in Super). */
void fn_80093AF8(boost::shared_ptr<Player::PlayerBase> *player, float rate);
/* Tests bit 3 of the player's word at 0xB4 (meaning unknown; as in AirAttack). */
bool fn_800A9E1C(boost::shared_ptr<Player::PlayerBase> *player);
/* Switches a player's behavior (behaviour inferred). */
void fn_800A7F14(Player::Behavior::UnkBehaviorExecuter_800A7F14 *executer,
                 const Player::Behavior::InitializeArgs *args);

Vec3f fn_800091A4(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
}

/* The player's members above (a macro, as in Game/PlayerBehaviorSuper.cpp). */
#define FieldsOf(player) reinterpret_cast<UnkPlayerFields_80084170 *>(player)

/* vector += value (an inline wrapper, which orders the call's arguments as the game does). */
inline void AddVector(Vec3f &vector, const Vec3f &value) {
    fn_8040CFF8(&vector, &value);
}

namespace Player {
namespace Behavior {

/* The behavior's flag word (struct name unknown): bit 0 the motion's end (event 0x5000's value 5),
 * bit 1 the step's end (value 1), bit 2 the next step asked for (command 0x102), bit 3 the attack
 * button held (cleared by command 0x103), bit 4 an attack with a weapon. */
struct GroundAttackFlags {
    GroundAttackFlags() {
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

class GroundAttack : public Behavior {
  public:
    GroundAttack(const boost::shared_ptr<PlayerBase> &player);
    virtual ~GroundAttack();
    virtual bool vf_0x0C(int prevId, const InitializeArgs &args);
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x18(CharCommand *command);
    virtual void Update(float dt);
    virtual void vf_0x20();

    /* The factory record's functions (names inferred). */
    static Behavior *Create(const boost::shared_ptr<PlayerBase> &player);
    static void Release();

    void OnMotion(MotionEventArgs &args);

    static const char *s_name;

    /* 0x1C */ s32 m_step; /* the combo's step, from 1 */
    /* 0x20 */ GroundAttackFlags m_flags;
};
SIZE_ASSERT(GroundAttack, 0x24);

const char *GroundAttack::s_name = "GroundAttack";

GroundAttack::GroundAttack(const boost::shared_ptr<PlayerBase> &player)
    : Behavior(player, s_name, 0x19) {}

GroundAttack::~GroundAttack() {}

bool GroundAttack::vf_0x0C(int prevId, const InitializeArgs &args) {
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1F) &&
        m_player.get()->m_weapon->GetWeaponKind() != 5) {
        return false;
    }
    return true;
}

void GroundAttack::Enter(int prevId, const InitializeArgs &args) {
    m_step = 1;
    m_flags.Reset();
    fn_80076CA8(m_player.get()->m_stateBits, 0x3E);
    fn_80076CA8(m_player.get()->m_stateBits, 0);
    fn_80014E1C(&m_player.get()->m_flags, 6);
    if (fn_8006CD78(m_player.get()->m_stateBits, 0x1F)) {
        switch (m_player.get()->m_weapon->GetWeaponKind()) {
        case 5:
            m_player.get()->m_motion->vf_0x10(0x1F, 0.0f);
            break;
        }
        m_flags.Set(4);
    } else {
        m_player.get()->m_motion->vf_0x10(0x1E, 0.0f);
    }
    PlayerMotionEventCallback(m_player.get()) =
        MotionEventCallback(boost::bind(&GroundAttack::OnMotion, this, _1));
}

void GroundAttack::Leave(int nextId) {
    fn_80076CC8(m_player.get()->m_stateBits, 0);
    fn_80076CC8(m_player.get()->m_stateBits, 0x3E);
    fn_8006127C(&m_player.get()->m_flags, 6);
    PlayerMotionEventCallback(m_player.get()).Clear();
    AddVector(m_player->field_0x18,
              fn_800091A4(&FieldsOf(m_player.operator->())->field_0xC4, 60.0f));
}

void GroundAttack::vf_0x18(CharCommand *command) {
    switch (command->TypeId()) {
    case 0x102:
        if (!fn_8006CD78(m_player.get()->m_stateBits, 0x1E)) {
            m_flags.Set(2);
        }
        command->Flags() |= 2;
        break;
    case 0x103:
        m_flags.Clear(3);
        command->Flags() |= 2;
        break;
    }
}

void GroundAttack::Update(float dt) {
    if (m_step <= 2) {
        AddVector(m_player->field_0x24,
                  fn_800091A4(&FieldsOf(m_player.operator->())->field_0xB8, 400.0f));
        fn_80093AF8(&m_player, 2.0f * dt);
    }
    fn_80079720(m_player.get());
    fn_8009728C(&m_player, 0);
    if (m_flags.Test(1)) {
        if (!m_flags.Test(2)) {
            if (m_step == 1 && m_flags.Test(3) && !m_flags.Test(4)) {
                InitializeArgs args(0x12);
                fn_800A7F14(m_player.get()->m_behaviors, &args);
            } else {
                fn_800A7F14(m_player.get()->m_behaviors,
                            &(const InitializeArgs &)InitializeArgsGround(false));
            }
            return;
        }
        m_step++;
        m_flags.Clear(2);
        m_flags.Clear(1);
    }
    if (m_flags.Test(4) && !fn_8006CD78(m_player.get()->m_stateBits, 0x1F)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
        return;
    }
    if (m_step > 2 && !fn_8006CD78(m_player.get()->m_stateBits, 0x1F)) {
        fn_80076CC8(m_player.get()->m_stateBits, 2);
        fn_80076CC8(m_player.get()->m_stateBits, 0);
    } else if (!fn_8006CD78(m_player.get()->m_stateBits, 2)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(2));
        return;
    }
    if (m_flags.Test(0)) {
        fn_80076CC8(m_player.get()->m_stateBits, 2);
        fn_80076CC8(m_player.get()->m_stateBits, 0);
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(3));
        return;
    }
    if (fn_800A9E1C(&m_player)) {
        fn_800A7F14(m_player.get()->m_behaviors, &(const InitializeArgs &)InitializeArgs(0));
        return;
    }
}

void GroundAttack::OnMotion(MotionEventArgs &args) {
    switch (args.m_event) {
    case 0x5000:
        switch (*args.m_value) {
        case 1:
            m_flags.Set(1);
            break;
        case 5:
            m_flags.Set(0);
            break;
        }
        break;
    }
}

void GroundAttack::vf_0x20() {}

Behavior *GroundAttack::Create(const boost::shared_ptr<PlayerBase> &player) {
    return new GroundAttack(player);
}

void GroundAttack::Release() {}

} // namespace Behavior
} // namespace Player

/* The record the behavior factory table finds GroundAttack by (keeps its lbl_ name). */
extern const Player::Behavior::UnkBehaviorDesc_804EC024 lbl_804B011C = {
    Player::Behavior::GroundAttack::Create, Player::Behavior::GroundAttack::Release, 0};

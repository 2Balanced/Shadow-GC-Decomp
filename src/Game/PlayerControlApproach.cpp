/* Player::Control::Approach (class name from RTTI): control 1 of a player, which walks the player
 * up to a target (Player::Control::InitializeArgsApproach, sent by Player::ApproachControlCommand)
 * until it faces the target's owner and touches it, then sends the player the command to pick the
 * owner up or turn it over and hands control back (control 0). It gives up after a second, or when
 * the target goes away or the player leaves the ground. Also here: Player::Control::ControlBase's
 * constructor. Member and function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define PLAYER_CONTROL_SLOT_ARGS
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Player::PlayerBase;

/* Target (class name from RTTI; as in Game/PlayerBehaviorLightDash.cpp). */
class Target {
  public:
    /* The point to aim at, as seen from a position (vtable slot 0x8). */
    virtual Vec3f GetPosition(const Vec3f *from);

    /* 0x04 */ u8 field_0x4[0x28 - 0x4];
    /* 0x28 */ boost::weak_ptr<CharInfo> m_owner; /* the character the target belongs to */
};

/* The player's collision state (Game/PlayerCharaColli's unit): what the player touches, and the
 * character it should take note of (behaviour inferred). */
class UnkPlayerCharaColli_80095BD4;

extern "C" {
Vec3f fn_800796DC(PlayerBase *player); /* the player's front */
boost::weak_ptr<CharInfo> fn_80095A10(UnkPlayerCharaColli_80095BD4 *colli);
void fn_80095A40(UnkPlayerCharaColli_80095BD4 *colli);
void fn_80095A64(UnkPlayerCharaColli_80095BD4 *colli, const boost::weak_ptr<CharInfo> *chara);
/* Switches the player's control (Player::Controller; see its unit). */
void fn_8009EBA4(Task *controller, const Player::Control::InitializeArgs *args);
bool fn_8006CD78(u32 *bits, int bit);
float fn_8040CE74(Vec3f *vector); /* normalizes */
void fn_8040CE38(Vec3f *vector, float scale);
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* dot product */
}

/* fn_8040CD74 (a - b) called as returning its result by value. */
typedef Vec3f (*SubtractFn)(const Vec3f *a, const Vec3f *b);

/* The player's members used here (struct name unknown; Player::PlayerBase leaves them
 * unmapped). */
struct UnkPlayerFields_8009DCC0 {
    /* 0x000 */ u8 field_0x0[0xA8];
    /* 0x0A8 */ u32 m_stateBits[2];
    /* 0x0B0 */ u8 field_0xB0[0x224 - 0xB0];
    /* 0x224 */ Task *m_controller;
    /* 0x228 */ u8 field_0x228[0x248 - 0x228];
    /* 0x248 */ UnkPlayerCharaColli_80095BD4 *m_colli;
};

#define Fields(player) reinterpret_cast<UnkPlayerFields_8009DCC0 *>(player)

namespace Player {
namespace Control {

class Approach : public ControlBase {
  public:
    Approach(const boost::shared_ptr<PlayerBase> &player);
    virtual ~Approach();
    virtual void Enter(int prevId, const InitializeArgs &args);
    virtual void Leave(int nextId);
    virtual void vf_0x14(float dt);

    static const char *s_name;

    /* 0x14 */ s32 m_kind;
    /* 0x18 */ boost::weak_ptr<Target> m_target;
    /* 0x20 */ boost::weak_ptr<CharInfo> m_owner;
    /* 0x28 */ float m_time;
};
SIZE_ASSERT(Approach, 0x2C);

const char *Approach::s_name = "Approach";

Approach::Approach(const boost::shared_ptr<PlayerBase> &player) : ControlBase(player, s_name, 1) {}

Approach::~Approach() {}

void Approach::Enter(int prevId, const InitializeArgs &args) {
    const InitializeArgsApproach &approach = dynamic_cast<const InitializeArgsApproach &>(args);
    m_kind = approach.m_kind;
    m_target = approach.m_target;
    boost::shared_ptr<Target> target = m_target.lock();
    if (target) {
        m_owner = target->m_owner;
        fn_80095A64(Fields(m_player.get())->m_colli, &m_owner);
    }
    m_time = 0.0f;
}

void Approach::Leave(int nextId) {
    fn_80095A40(Fields(m_player.get())->m_colli);
}

void Approach::vf_0x14(float dt) {
    if (m_target.expired() || m_owner.expired() ||
        !fn_8006CD78(Fields(m_player.get())->m_stateBits, 2)) {
        fn_8009EBA4(Fields(m_player.get())->m_controller, &InitializeArgs(0));
        return;
    }
    m_time += dt;
    if (m_time >= 1.0f) {
        fn_8009EBA4(Fields(m_player.get())->m_controller, &InitializeArgs(0));
        return;
    }
    boost::shared_ptr<Target> target = m_target.lock();
    boost::shared_ptr<CharInfo> owner = m_owner.lock();
    PlayerBase *player = m_player.get();
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((SubtractFn)fn_8040CD74)(
                                &(const Vec3f &)target->GetPosition(&player->m_position),
                                &player->m_position));
    direction.y = 0.0f;
    fn_8040CE74(&direction);
    float facing = fn_8040CF90(&direction, &(const Vec3f &)fn_800796DC(m_player.get()));
    fn_8040CE38(&direction, 0.5f);
    m_player->PushCommand(new AccelerateInputCommand(direction, 1));
    if (facing >= 0.966f) {
        boost::shared_ptr<CharInfo> touched =
            fn_80095A10(Fields(m_player.get())->m_colli).lock();
        if (touched == owner) {
            switch (m_kind) {
            case 1:
                m_player->PushCommand(new PickupObjectCommand(m_target, 2));
                break;
            case 2:
                m_player->PushCommand(new TurnOverCommand(m_target, 2));
                break;
            }
            fn_8009EBA4(Fields(m_player.get())->m_controller, &InitializeArgs(0));
            return;
        }
    }
}

} // namespace Control
} // namespace Player

/* Makes a player's Approach control (name inferred). */
extern "C" Player::Control::ControlBase *fn_8009DD10(const boost::shared_ptr<PlayerBase> &player) {
    return new Player::Control::Approach(player);
}

namespace Player {
namespace Control {

ControlBase::ControlBase(const boost::shared_ptr<PlayerBase> &player, const char *name, int arg)
    : m_player(player), m_name(name), field_0x10(arg) {}

} // namespace Control
} // namespace Player

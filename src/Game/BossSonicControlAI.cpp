/* @unnamed@BossSonicControlAI_cpp@::ControlAI (class name from RTTI; its name is "BossSonicAI"):
 * Boss Sonic's AI, the control the boss runs normally (made by its SonicTypeControl,
 * Game/BossSonicSonicTypeControl.cpp, for controls 0 and 1). Each frame, unless the game is
 * paused, it updates the target, lets the desires (Move, Attack, CounterAttack, Dodge) pick the
 * next mode, and runs the mode: 1 turns the boss towards its target, 2 moves it to the move
 * desire's destination, 3 and 4 start a jump (at the target's own target with 3), 5 a dodge. When
 * the boss falls below -100 it switches the NPC to control 4 (Game/BossSonicControlRestart.cpp).
 * The file name comes from the RTTI name. Function and member names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/BossSonic.h"
#include "Game/CharCommand.h"
#include "Game/MathTypes.h"
#include "Game/Npc.h"
#include "Game/Player.h"
#include "Game/PlayerControl.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class Target;

/* The game state (struct name unknown; see Game/StageTimer.cpp): the byte at 0xC8 is set while
 * the game is paused (meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xC8];
    /* 0x0C8 */ u8 field_0xC8;
    /* 0x0C9 */ u8 field_0xC9[0x150 - 0xC9];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* AccelerateInputCommand (class name from RTTI; constructor 0x801DBE84): steers a character along
 * a direction (as declared in Game/BkSoldier.cpp). */
class AccelerateInputCommand : public CharCommand {
  public:
    AccelerateInputCommand(const Vec3f &direction, int arg1);

    /* 0x14 */ Vec3f m_direction;
};
SIZE_ASSERT(AccelerateInputCommand, 0x20);

namespace Player {
namespace Npc {
namespace BossSonic {

/* The sub controls the AI starts (Game/BossSonicSubControlJump.cpp,
 * Game/BossSonicSubControlDodge.cpp). */
class SubControlJump {
  public:
    static void Create(Task **slot, const boost::shared_ptr<BossSonicWork> &work,
                       const boost::weak_ptr<Target> &target);
    static void Create(Task **slot, const boost::shared_ptr<BossSonicWork> &work);
};

class SubControlDodge {
  public:
    static void Create(Task **slot, const boost::shared_ptr<BossSonicWork> &work);
};

namespace Desire {

/* The desires' factory functions (Game/BossSonicDesire*.cpp). */
class Move {
  public:
    static boost::shared_ptr<BossSonicDesireBase> Instantiate(const boost::shared_ptr<BossSonicWork> &work);
};

class Attack {
  public:
    static boost::shared_ptr<BossSonicDesireBase> Instantiate(const boost::shared_ptr<BossSonicWork> &work);
};

class CounterAttack {
  public:
    static boost::shared_ptr<BossSonicDesireBase> Instantiate(const boost::shared_ptr<BossSonicWork> &work);
};

class Dodge {
  public:
    static boost::shared_ptr<BossSonicDesireBase> Instantiate(const boost::shared_ptr<BossSonicWork> &work);
};

} // namespace Desire
} // namespace BossSonic
} // namespace Npc
} // namespace Player

extern "C" {
/* Vector helpers (behaviour from their bodies; see Game/BossSonicDesireMove.cpp). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
void fn_8040CE38(Vec3f *vector, float scale); /* *= */
void fn_8040CE74(Vec3f *vector);              /* normalizes */
float fn_8040CF50(const Vec3f *vector);       /* the length */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* the dot product */
/* The direction the NPC faces (behaviour inferred). */
Vec3f fn_800796DC(Player::Npc::Npc *npc);
/* The target a player's aim holds (see Game/ArkCannon.cpp). */
boost::weak_ptr<Target> fn_80206574(Player::PlayerTargetBase *target);
/* Switches the control of the character's controller (see Game/BossSonicControlRestart.cpp). */
void fn_8009EBA4(Task *controller, const Player::Control::InitializeArgs *args);
}

/* fn_8040CD74 called as returning a - b by value (see Game/BossSonicDesireMove.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

using boost::shared_ptr;
using Player::PlayerBase;
using Player::Npc::Npc;
using Player::Npc::BossSonic::BossSonicWork;
using namespace Player::Npc::BossSonic;

static const char *s_name = "BossSonicAI";

namespace {

class ControlAI : public Player::Control::ControlBase {
  public:
    ControlAI(const shared_ptr<Npc> &npc, const shared_ptr<BossSonicWork> &work);
    virtual ~ControlAI();
    virtual void vf_0x0C();
    virtual void vf_0x10();
    virtual void vf_0x14(float dt);

    /* Sets up the four desires. */
    void InitDesires();
    void TickDesires(float dt);
    /* Mode 1: turns the boss towards its target. */
    void TurnToTarget(float dt);
    /* Mode 2: moves the boss to the move desire's destination. */
    void MoveToDestination();

    /* 0x14 */ shared_ptr<Npc> m_npc;
    /* 0x1C */ u32 field_0x1C; /* cleared when a new mode starts (meaning unknown) */
    /* 0x20 */ shared_ptr<BossSonicWork> m_work;
    /* 0x28 */ Task *m_jump;  /* the running SubControlJump */
    /* 0x2C */ Task *m_dodge; /* the running SubControlDodge */
};
SIZE_ASSERT(ControlAI, 0x30);

ControlAI::ControlAI(const shared_ptr<Npc> &npc, const shared_ptr<BossSonicWork> &work)
    : ControlBase(npc, s_name, 2), m_npc(npc), m_work(work) {
    InitDesires();
    field_0x1C = 0;
    m_jump = 0;
}

ControlAI::~ControlAI() {}

void ControlAI::vf_0x0C() {}

void ControlAI::vf_0x10() {}

void ControlAI::vf_0x14(float dt) {
    if (lbl_8057D6A0.field_0xC8) {
        return;
    }
    m_work->UpdateTarget();
    if (m_work->m_target.expired()) {
        return;
    }
    if (m_work->m_player.expired()) {
        return;
    }
    TickDesires(dt);
    if (m_work->m_mode.HasRequest()) {
        m_work->m_mode.Apply();
        field_0x1C = 0;
    }
    switch (m_work->m_mode.Get()) {
    case 1:
        TurnToTarget(dt);
        break;
    case 2:
        MoveToDestination();
        break;
    case 3: {
        shared_ptr<PlayerBase> target = boost::dynamic_pointer_cast<PlayerBase>(m_work->m_target.lock());
        if (target) {
            SubControlJump::Create(&m_jump, m_work, fn_80206574(target->m_target));
            m_work->m_mode.Set(1);
        }
        break;
    }
    case 4:
        SubControlJump::Create(&m_jump, m_work);
        m_work->m_mode.Set(1);
        break;
    case 5:
        SubControlDodge::Create(&m_dodge, m_work);
        m_work->m_mode.Set(1);
        break;
    }
    if (m_npc->m_position.y <= -100.0f) {
        fn_8009EBA4(m_npc->m_task, &Player::Control::InitializeArgs(4));
    }
}

void ControlAI::InitDesires() {
    m_work->m_desires.Init(4);
    m_work->m_desires.Set(0, Desire::Move::Instantiate(m_work));
    m_work->m_desires.Set(1, Desire::Attack::Instantiate(m_work));
    m_work->m_desires.Set(2, Desire::CounterAttack::Instantiate(m_work));
    m_work->m_desires.Set(3, Desire::Dodge::Instantiate(m_work));
}

void ControlAI::TickDesires(float dt) {
    m_work->m_desires.Tick(dt);
}

void ControlAI::TurnToTarget(float dt) {
    if (m_work->m_target.expired()) {
        return;
    }
    shared_ptr<CharInfo> target = m_work->m_target.lock();
    Vec3f toTarget;
    fn_800091EC(&toTarget, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&target->m_position, &m_npc->m_position));
    Vec3f front;
    fn_800091EC(&front, &(const Vec3f &)fn_800796DC(m_npc.get()));
    toTarget.y = 0.0f;
    fn_8040CE74(&toTarget);
    if (fn_8040CF90(&front, &toTarget) < 0.94f) {
        m_npc->PushCommand(new AccelerateInputCommand(fn_8040CDA8(&toTarget, 0.1f), 4));
    }
}

void ControlAI::MoveToDestination() {
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_work->m_destination, &m_npc->m_position));
    direction.y = 0.0f;
    float distance = fn_8040CF50(&direction);
    fn_8040CE74(&direction);
    if (distance < 40.0f) {
        fn_8040CE38(&direction, distance / 40.0f);
    }
    if (distance < m_work->m_params->field_0xD0) {
        m_work->m_mode.Set(1);
    }
    m_npc->PushCommand(new AccelerateInputCommand(direction, 4));
}

} // namespace

/* Creates the AI control for npc (called by Boss Sonic's SonicTypeControl; name inferred). */
extern "C" Player::Control::ControlBase *fn_8029499C(const shared_ptr<Npc> &npc,
                                                    const shared_ptr<BossSonicWork> &work) {
    return new ControlAI(npc, work);
}

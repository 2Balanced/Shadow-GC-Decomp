/* Boss::Diablon::Behavior::Idle (class name from RTTI): the Diablon behavior between attacks. It
 * keeps flag 8 set until its timer runs out (flag 5 extends the timer), turns the parts towards
 * the side the player is on (ActionMove 2 and 3), and turns the boss to face the player, slowing
 * down once it does. File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace std {
float cos(float x);
}

extern "C" {
/* The larger of a and b, and value clamped to [min, max] (behaviour from their bodies). */
const float &fn_80070494(const float &a, const float &b);
const float &fn_80070460(const float &value, const float &min, const float &max);
/* Vector helpers (see Game/DiablonBehaviorMove.cpp). */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* dot product */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
}

/* Converts degrees to radians (name inferred; see Game/DiablonBehaviorEvent.cpp). */
inline float ToRadians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

namespace Boss {
namespace Diablon {
namespace Behavior {

class Idle : public BehaviorBase {
  public:
    Idle(const boost::shared_ptr<Diablon> &owner) : BehaviorBase(owner, "Idle", 1) {}
    virtual ~Idle();
    virtual void Begin(int previous, const InitializeArgs &args);
    virtual void End(int next);
    virtual void Tick(float dt);

    /* Whether the player is on the side the boss's x axis points to (name inferred). */
    bool IsTargetOnRight();

    static boost::shared_ptr<BehaviorBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
    static void Setup(void *executer);
    static void Teardown(void *executer);

    /* 0x14 */ float m_timer;
    /* 0x18 */ float m_side; /* 1 or -1 once the parts were turned (meaning inferred) */
    /* 0x1C */ float m_turnSpeed;
};
SIZE_ASSERT(Idle, 0x20);

Idle::~Idle() {}

void Idle::Begin(int previous, const InitializeArgs &args) {
    fn_802D9E4C(m_owner->m_executer, Parts::Action(0));
    m_owner->m_flags.Set(1);
    m_owner->m_flags.Set(3);
    m_owner->m_flags.Set(8);
    m_timer = m_owner->m_params->m_idleTime;
    m_side = 0.0f;
    m_turnSpeed = 0.0f;
}

void Idle::End(int next) {
    m_owner->m_flags.Reset(1);
    m_owner->m_flags.Reset(3);
    m_owner->m_flags.Reset(8);
}

void Idle::Tick(float dt) {
    if (m_timer > 0.0f) {
        m_timer -= dt;
        if (m_owner->m_flags.Test(5)) {
            m_owner->m_flags.Reset(5);
            m_timer = fn_80070494(m_timer, m_owner->m_params->m_idleMinTime);
        }
        if (m_timer <= 0.0f) {
            m_owner->m_flags.Reset(8);
        }
    }
    if (IsTargetOnRight()) {
        if (m_side >= 0.0f) {
            fn_802D9E4C(m_owner->m_executer, Parts::ActionMove(2));
            m_side = -1.0f;
        }
    } else if (m_side <= 0.0f) {
        fn_802D9E4C(m_owner->m_executer, Parts::ActionMove(3));
        m_side = 1.0f;
    }
    float facing = std::cos(ToRadians(m_owner->m_params->m_idleFacingAngle));
    if (fn_802D9570(m_owner.get()) > facing) {
        m_turnSpeed *= 0.5f;
    } else {
        DiablonParams *params = m_owner->m_params;
        float accel = ToRadians(params->m_idleTurnAccel);
        float max = ToRadians(params->m_idleMaxTurnSpeed);
        m_turnSpeed += dt * (m_side * accel);
        float min = -max;
        m_turnSpeed = fn_80070460(m_turnSpeed, min, max);
    }
    m_owner->m_rotation.y += m_turnSpeed * dt;
}

bool Idle::IsTargetOnRight() {
    boost::shared_ptr<CharInfo> target = m_owner->m_target.lock();
    if (target.get() == 0) {
        return true;
    }
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&target->m_position,
                                                                          &m_owner->m_position));
    Vec3f right(1.0f, 0.0f, 0.0f);
    fn_800091D0(&right, &(const Vec3f &)fn_8040D0AC(&right, m_owner->m_rotation.y));
    float dot = fn_8040CF90(&offset, &right);
    return dot >= 0.0f;
}

boost::shared_ptr<BehaviorBase> Idle::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<BehaviorBase>(new Idle(owner));
}

void Idle::Setup(void *executer) {}

void Idle::Teardown(void *executer) {}

/* The behavior table's entry (name inferred). */
extern const Info g_idleInfo;
const Info g_idleInfo = {Idle::Instantiate, Idle::Setup, Idle::Teardown};

} // namespace Behavior
} // namespace Diablon
} // namespace Boss

/* Boss::Diablon::Behavior::Move (class name from RTTI): the Diablon behavior that moves the boss
 * to where the player stood when it began. It turns the parts forward or backward depending on
 * how far the target lies along the boss's facing, accelerates until it is close enough, then
 * waits for the boss to slow down and switches to the idle behavior. File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace std {
float fabs(float x);
}

extern "C" {
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);  /* dot product */
void fn_8040CE60(Vec3f *vector);                            /* zero */
/* vector turned by angle about the y axis (behaviour inferred). */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
/* Moves position on by velocity and velocity by acceleration (see Bomb.cpp). */
void fn_804103E8(Vec3f *position, Vec3f *velocity,
                 const Vec3f *acceleration, float scale, float dt);
}

/* Assigns value to vector through the out-of-line vector assignment (name inferred). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

namespace Boss {
namespace Diablon {
namespace Behavior {

class Move : public BehaviorBase {
  public:
    Move(const boost::shared_ptr<Diablon> &owner);
    virtual ~Move();
    virtual void Begin();
    virtual void End();
    virtual void Tick(float dt);

    /* How far the target lies along the boss's facing (name inferred). */
    float GetFrontDistance();

    static boost::shared_ptr<BehaviorBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
    static void Setup(void *executer);
    static void Teardown(void *executer);

    /* 0x14 */ Vec3f m_target;
    /* 0x20 */ float m_direction; /* 1 forward, -1 backward */
    /* 0x24 */ s32 m_state;
};
SIZE_ASSERT(Move, 0x28);

Move::Move(const boost::shared_ptr<Diablon> &owner) : BehaviorBase(owner, "Move", 2) {}

Move::~Move() {}

void Move::Begin() {
    fn_802D4ED4(&m_owner->m_flags, 1);
    fn_802D4ED4(&m_owner->m_flags, 3);
    fn_802D4ED4(&m_owner->m_flags, 8);
    fn_800091D0(&m_target, &m_owner->m_target.lock()->m_position);
    if (GetFrontDistance() > m_owner->m_params->m_moveBackLimit) {
        m_direction = 1.0f;
        fn_802D9E4C(m_owner->m_executer, Parts::ActionMove(0));
    } else {
        m_direction = -1.0f;
        fn_802D9E4C(m_owner->m_executer, Parts::ActionMove(1));
    }
    m_state = 0;
}

void Move::End() {
    fn_802D4EF4(&m_owner->m_flags, 1);
    fn_802D4EF4(&m_owner->m_flags, 3);
    fn_802D4EF4(&m_owner->m_flags, 8);
}

void Move::Tick(float dt) {
    Vec3f forward(0.0f, 0.0f, -1.0f * m_direction);
    fn_800091D0(&forward, &(const Vec3f &)fn_8040D0AC(&forward, m_owner->m_rotation.y));
    switch (m_state) {
    case 0:
        /* Through px: an inline operator-> here would be called out of line (see
         * docs/matching-gadgets.md), and fn_800091D0 called directly evaluates the target first. */
        AssignVector(m_owner.px->field_0x24, fn_8040CDA8(&forward, m_owner.px->m_params->m_moveAccel));
        {
            float distance = GetFrontDistance();
            if (m_direction > 0.0f) {
                if (distance < m_owner->m_params->m_moveStopFront) {
                    m_state++;
                }
            } else if (distance > m_owner->m_params->m_moveStopBack) {
                m_state++;
            }
        }
        break;
    case 1:
        if (fn_8040CF2C(&m_owner->field_0x18) < 9.0f) {
            fn_8040CE60(&m_owner->field_0x18);
            fn_802D9ED8(m_owner->m_executer, InitializeArgs(1));
            m_state++;
        }
        break;
    }
    fn_804103E8(&m_owner->m_position, &m_owner->field_0x18, &m_owner->field_0x24,
                m_owner->m_params->m_moveFriction, dt);
    fn_8040CE60(&m_owner->field_0x24);
}

float Move::GetFrontDistance() {
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                             &m_target, &m_owner->m_position));
    offset.y = 0.0f;
    Vec3f forward(0.0f, 0.0f, -1.0f);
    fn_800091D0(&forward, &(const Vec3f &)fn_8040D0AC(&forward, m_owner->m_rotation.y));
    return std::fabs(fn_8040CF90(&forward, &offset));
}

boost::shared_ptr<BehaviorBase> Move::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<BehaviorBase>(new Move(owner));
}

void Move::Setup(void *executer) {}

void Move::Teardown(void *executer) {}

/* The behavior table's entry (0x80558CA8; name inferred). */
extern const Info g_moveInfo;
const Info g_moveInfo = {Move::Instantiate, Move::Setup, Move::Teardown};

} // namespace Behavior
} // namespace Diablon
} // namespace Boss

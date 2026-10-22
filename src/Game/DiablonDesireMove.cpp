/* Boss::Diablon::Desire::Move (class name from RTTI): the Diablon boss's desire to move. While the
 * boss may move (flag 0) and faces the player, the desire grows as long as the player is too near
 * or too far; when the boss stops facing the player, a full desire falls back just below full.
 * Starting it switches the boss to the move behavior (Behavior::Move). File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/BossDesire.h"
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace std {
float cos(float x);
}

extern const float lbl_805FAD38; /* pi */

namespace Boss {
namespace Diablon {
namespace Desire {

class Move : public DiablonDesireBase {
  public:
    Move(const boost::shared_ptr<Diablon> &owner) : DiablonDesireBase(owner, "Move", 0) {}
    virtual ~Move();
    virtual void Tick(float dt);
    virtual void Begin();

    /* Whether the boss faces the player, and whether the player is out of the boss's range
     * (names inferred). */
    bool IsFacingTarget();
    bool IsTargetOutOfRange();

    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
};
SIZE_ASSERT(Move, 0x1C);

Move::~Move() {}

void Move::Tick(float dt) {
    if (m_owner->m_flags.Test(0)) {
        if (IsFacingTarget()) {
            if (IsTargetOutOfRange()) {
                AddDesire(dt / m_owner->m_params->m_moveDesireTime);
            }
        } else if (m_desire >= 1.0f) {
            m_desire = 0.98f;
        }
    }
}

void Move::Begin() {
    DiablonDesireBase::Begin();
    fn_802D9ED8(m_owner->m_executer, Behavior::InitializeArgs(2));
    m_desire = 0.0f;
}

bool Move::IsFacingTarget() {
    DiablonParams *params = m_owner->m_params;
    float facing = fn_802D9570(m_owner.get());
    return !(facing < std::cos(params->m_moveFacingAngle * (lbl_805FAD38 / 180.0f)));
}

bool Move::IsTargetOutOfRange() {
    boost::shared_ptr<CharInfo> target = m_owner->m_target.lock();
    DiablonParams *params = m_owner->m_params;
    if (target.get() == 0) {
        return false;
    }
    float distance = fn_802D94B8(m_owner.get());
    if (params->m_moveMaxDistance < distance) {
        return true;
    }
    if (params->m_moveMinDistance > distance) {
        return true;
    }
    return false;
}

boost::shared_ptr<DiablonDesireBase> Move::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<DiablonDesireBase>(new Move(owner));
}

} // namespace Desire
} // namespace Diablon
} // namespace Boss

/* Boss::Diablon::Desire::Wipe (class name from RTTI): the Diablon boss's desire to sweep the arena.
 * While the boss may move (flag 0) and the player is in reach and in front of it, the desire grows;
 * otherwise it is cleared. Starting it switches the boss to the wipe behavior (Behavior::Wipe).
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
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

class Wipe : public DiablonDesireBase {
  public:
    Wipe(const boost::shared_ptr<Diablon> &owner) : DiablonDesireBase(owner, "Wipe", 1) {}
    virtual ~Wipe();
    virtual void Tick(float dt);
    virtual void Begin();

    /* Whether the player is in the sweep's reach and in front of the boss (name inferred). */
    bool IsTargetInRange();

    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
};
SIZE_ASSERT(Wipe, 0x1C);

Wipe::~Wipe() {}

void Wipe::Tick(float dt) {
    if (!m_owner->m_flags.Test(0)) {
        m_desire = 0.0f;
    } else if (IsTargetInRange()) {
        AddDesire(dt / m_owner->m_params->m_wipeDesireTime);
    } else {
        m_desire = 0.0f;
    }
}

void Wipe::Begin() {
    DiablonDesireBase::Begin();
    fn_802D9ED8(m_owner->m_executer, Behavior::InitializeArgs(5));
    m_desire = 0.0f;
}

bool Wipe::IsTargetInRange() {
    boost::shared_ptr<CharInfo> target = m_owner->m_target.lock();
    DiablonParams *params = m_owner->m_params;
    if (target.get() == 0) {
        return false;
    }
    float distance = fn_802D94B8(m_owner.get());
    if (params->m_wipeMinDistance > distance || params->m_wipeMaxDistance < distance) {
        return false;
    }
    float facing = fn_802D9570(m_owner.get());
    if (facing < std::cos(params->m_wipeFacingAngle * (lbl_805FAD38 / 180.0f))) {
        return false;
    }
    return true;
}

boost::shared_ptr<DiablonDesireBase> Wipe::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<DiablonDesireBase>(new Wipe(owner));
}

} // namespace Desire
} // namespace Diablon
} // namespace Boss

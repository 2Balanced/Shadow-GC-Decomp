/* Boss::Diablon::Desire::Bomb (class name from RTTI): the Diablon boss's desire to drop the bomb.
 * Once field 0xD0 has reached 5, the desire grows while the player is in the bomb's reach.
 * Starting it switches the boss to the bomb behavior (Behavior::Bomb). File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/BossDesire.h"
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

namespace Boss {
namespace Diablon {
namespace Desire {

class Bomb : public DiablonDesireBase {
  public:
    Bomb(const boost::shared_ptr<Diablon> &owner) : DiablonDesireBase(owner, "Bomb", 3) {}
    virtual ~Bomb();
    virtual void Tick(float dt);
    virtual void Begin();

    /* Whether the player is in the bomb's reach (name inferred). */
    bool IsTargetInRange();

    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
};
SIZE_ASSERT(Bomb, 0x1C);

Bomb::~Bomb() {}

void Bomb::Tick(float dt) {
    if (m_owner->field_0xD0 >= 5 && IsTargetInRange()) {
        boost::shared_ptr<CharInfo> target = m_owner->m_target.lock();
        if (target.get() != 0) {
            AddDesire(dt / m_owner->m_params->m_bombDesireTime);
        }
    }
}

void Bomb::Begin() {
    DiablonDesireBase::Begin();
    fn_802D9ED8(m_owner->m_executer, Behavior::InitializeArgs(3));
    m_desire = 0.0f;
}

bool Bomb::IsTargetInRange() {
    boost::shared_ptr<CharInfo> target = m_owner->m_target.lock();
    DiablonParams *params = m_owner->m_params;
    if (target.get() == 0) {
        return false;
    }
    float distance = fn_802D94B8(m_owner.get());
    if (params->m_bombMinDistance > distance || params->m_bombMaxDistance < distance) {
        return false;
    }
    return true;
}

boost::shared_ptr<DiablonDesireBase> Bomb::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<DiablonDesireBase>(new Bomb(owner));
}

} // namespace Desire
} // namespace Diablon
} // namespace Boss

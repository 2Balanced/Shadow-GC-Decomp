/* Player::Npc::BossSonic::Desire::CounterAttack (class name from RTTI): Boss Sonic's desire to
 * counter attack. While the AI's flag bit 1 is set it fills over the stage's counter attack time;
 * starting it clears the flag and asks for mode 4. This unit also holds BossSonicDesireBase's
 * functions (see Game/BossSonic.h). File name inferred from the class. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: every function matches, but retail's data holds two RTTI chains for
 * BossSonicDesireBase and Boss::Desire::DesireBase, one for BossSonicDesireBase's vtable and one
 * for CounterAttack's base list (with their name strings), where GC/2.7 shares one; the same
 * difference keeps Game/Mission.cpp NonMatching. */
#include "Game/BossSonic.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

namespace Player {
namespace Npc {
namespace BossSonic {
namespace Desire {

class CounterAttack : public BossSonicDesireBase {
  public:
    CounterAttack(const boost::shared_ptr<BossSonicWork> &work) : BossSonicDesireBase(work, "CounterAttack", 2) {}
    virtual ~CounterAttack();
    virtual void Tick(float dt);
    virtual void Begin();

    static boost::shared_ptr<BossSonicDesireBase> Instantiate(const boost::shared_ptr<BossSonicWork> &work);
};
SIZE_ASSERT(CounterAttack, 0x1C);

CounterAttack::~CounterAttack() {}

void CounterAttack::Tick(float dt) {
    if (m_work->m_flags.Test(1)) {
        float time = m_work->m_params->m_counterAttackTime;
        m_desire += time <= 0.0f ? 1.0f : dt / time;
    }
}

void CounterAttack::Begin() {
    BossSonicDesireBase::Begin();
    if (m_work->m_flags.Test(1)) {
        m_work->m_flags.Clear(1);
        m_work->m_mode.Request(4);
    }
    m_desire = 0.0f;
}

boost::shared_ptr<BossSonicDesireBase> CounterAttack::Instantiate(const boost::shared_ptr<BossSonicWork> &work) {
    return boost::shared_ptr<BossSonicDesireBase>(new CounterAttack(work));
}

BossSonicDesireBase::BossSonicDesireBase(const boost::shared_ptr<BossSonicWork> &work, const char *name, int arg)
    : Boss::Desire::DesireBase(name, arg), m_work(work) {}

BossSonicDesireBase::~BossSonicDesireBase() {}

void BossSonicDesireBase::Begin() {}

} // namespace Desire
} // namespace BossSonic
} // namespace Npc
} // namespace Player

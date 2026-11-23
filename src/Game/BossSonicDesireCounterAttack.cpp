/* Player::Npc::BossSonic::Desire::CounterAttack (class name from RTTI): Boss Sonic's desire to
 * counter attack. While the AI's flag bit 1 is set it fills over the stage's counter attack time;
 * starting it clears the flag and asks for mode 4. File name inferred from the class. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first).
 * BossSonicDesireBase's functions, which retail has at the start of this range, are a translation
 * unit of their own (Game/BossSonicDesireBase.cpp, split in wave 41). */
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

} // namespace Desire
} // namespace BossSonic
} // namespace Npc
} // namespace Player

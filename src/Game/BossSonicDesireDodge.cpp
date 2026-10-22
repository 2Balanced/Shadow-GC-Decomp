/* Player::Npc::BossSonic::Desire::Dodge (class name from RTTI): Boss Sonic's desire to dodge.
 * While the AI's flag bit 2 is set it becomes full at once; starting it clears the flag and asks
 * for mode 5 (SubControlDodge). File name inferred from the class. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#include "Game/BossSonic.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

namespace Player {
namespace Npc {
namespace BossSonic {
namespace Desire {

class Dodge : public BossSonicDesireBase {
  public:
    Dodge(const boost::shared_ptr<BossSonicWork> &work) : BossSonicDesireBase(work, "Dodge", 3) {}
    virtual ~Dodge();
    virtual void Tick(float dt);
    virtual void Begin();

    static boost::shared_ptr<BossSonicDesireBase> Instantiate(const boost::shared_ptr<BossSonicWork> &work);
};
SIZE_ASSERT(Dodge, 0x1C);

Dodge::~Dodge() {}

void Dodge::Tick(float dt) {
    if (m_work->m_flags.Test(2)) {
        m_desire += 1.0f;
    }
}

void Dodge::Begin() {
    BossSonicDesireBase::Begin();
    if (m_work->m_flags.Test(2)) {
        m_work->m_flags.Clear(2);
        m_work->m_mode.Request(5);
    }
    m_desire = 0.0f;
}

boost::shared_ptr<BossSonicDesireBase> Dodge::Instantiate(const boost::shared_ptr<BossSonicWork> &work) {
    return boost::shared_ptr<BossSonicDesireBase>(new Dodge(work));
}

} // namespace Desire
} // namespace BossSonic
} // namespace Npc
} // namespace Player

/* Boss::Diablon::Parts::HPGauge (class name from RTTI): the Diablon boss's health gauge, a part of
 * the boss. It makes the boss health gauge task (TGiHPGaugeBoss, class name from RTTI) below the
 * executer, shows the gauge, and each tick sets it to the boss's health ratio until the gauge task
 * ends. File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/Diablon.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* TGiHPGaugeBoss (class name from RTTI; vtable 0x80522628, constructor 0x800BE3F8): the boss
 * health gauge task. */
class TGiHPGaugeBoss : public Task {
  public:
    TGiHPGaugeBoss(Task *parent);

    /* 0x28 */ void *m_gauge;
    /* 0x2C */ u32 field_0x2C;
};
SIZE_ASSERT(TGiHPGaugeBoss, 0x30);

extern "C" {
/* Whether task is still alive (see Game/SetVehicle.cpp). */
bool fn_8004EAC0(Task *task);
/* The boss health gauge's ratio and visibility (see Game/BossCtrlUnit.cpp). */
void fn_800BE2DC(float ratio);
void fn_800BE2EC(int show);
/* The boss's health ratio (Game/Diablon.cpp). */
float fn_802D96FC(Boss::Diablon::Diablon *diablon);
}

namespace Boss {
namespace Diablon {
namespace Parts {

class HPGauge : public PartsBase {
  public:
    HPGauge(PartsBase *parent, const boost::shared_ptr<Diablon> &owner);
    virtual ~HPGauge();
    virtual void Tick(float dt);

    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner);

    /* 0x24 */ TGiHPGaugeBoss *m_gauge;
};
SIZE_ASSERT(HPGauge, 0x28);

HPGauge::HPGauge(PartsBase *parent, const boost::shared_ptr<Diablon> &owner)
    : PartsBase(parent, "HPGauge", owner) {
    m_gauge = new TGiHPGaugeBoss(owner->m_executer);
    fn_800BE2EC(1);
}

HPGauge::~HPGauge() {}

void HPGauge::Tick(float dt) {
    if (m_gauge != 0) {
        if (!fn_8004EAC0(m_gauge)) {
            m_gauge = 0;
        } else {
            fn_800BE2DC(fn_802D96FC(m_owner.get()));
        }
    }
}

PartsBase *HPGauge::Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner) {
    return new HPGauge(parent, owner);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss

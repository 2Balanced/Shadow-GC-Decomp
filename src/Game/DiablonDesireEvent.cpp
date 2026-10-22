/* Boss::Diablon::Desire::Event (class name from RTTI): the Diablon boss's desire to play its events.
 * It wants the first event at once, the second once fn_802D96FC falls to 0.8 and the third once
 * field 0xCC reaches 0, each after the previous event behavior has ended. Starting it switches the
 * boss to the event behavior with the stage's event (Behavior::InitializeArgsEvent). File name
 * inferred from the class. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/BossDesire.h"
#include "Game/Diablon.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

extern "C" {
/* Sets bit of a flag word (see EggLastMech.h). */
void fn_80014E1C(u32 *flags, int bit);
/* The id of the executer's current behavior, and a ratio of the boss's state compared against 0.8
 * (behaviour inferred). */
int fn_802D9E78(Boss::Diablon::Executer *executer);
float fn_802D96FC(Boss::Diablon::Diablon *diablon);
}

namespace Boss {
namespace Diablon {
namespace Desire {

class Event : public DiablonDesireBase {
  public:
    Event(const boost::shared_ptr<Diablon> &owner) : DiablonDesireBase(owner, "Event", 4) {
        m_state = 0;
        fn_80014E1C(&field_0x4, 2);
        fn_80014E1C(&field_0x4, 3);
    }
    virtual ~Event();
    virtual void Tick(float dt);
    virtual void Begin();

    static boost::shared_ptr<DiablonDesireBase> Instantiate(const boost::shared_ptr<Diablon> &owner);

    /* 0x1C */ s32 m_state;
};
SIZE_ASSERT(Event, 0x20);

Event::~Event() {}

void Event::Tick(float dt) {
    switch (m_state) {
    case 0:
        AddDesire(1.0f);
        break;
    case 1:
        if (fn_802D9E78(m_owner->m_executer) != 6) {
            m_state = 100;
        }
        break;
    case 100:
        if (fn_802D96FC(m_owner.get()) <= 0.8f) {
            AddDesire(1.0f);
        }
        break;
    case 101:
        if (fn_802D9E78(m_owner->m_executer) != 6) {
            m_state = 200;
        }
        break;
    case 200:
        if (m_owner->field_0xCC <= 0) {
            AddDesire(1.0f);
        }
        break;
    case 201:
        if (fn_802D9E78(m_owner->m_executer) != 6) {
            m_state = 300;
        }
        break;
    case 300:
        break;
    }
}

void Event::Begin() {
    DiablonDesireBase::Begin();
    switch (m_state) {
    case 0:
        fn_802D9ED8(m_owner->m_executer, Behavior::InitializeArgsEvent(0));
        m_state++;
        break;
    case 100:
        fn_802D9ED8(m_owner->m_executer, Behavior::InitializeArgsEvent(1));
        m_state++;
        break;
    case 200:
        fn_802D9ED8(m_owner->m_executer, Behavior::InitializeArgsEvent(2));
        m_state++;
        break;
    }
    m_desire = 0.0f;
}

boost::shared_ptr<DiablonDesireBase> Event::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<DiablonDesireBase>(new Event(owner));
}

} // namespace Desire
} // namespace Diablon
} // namespace Boss

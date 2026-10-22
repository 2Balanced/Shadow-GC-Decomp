/* Boss::Diablon::Behavior::Bomb (class name from RTTI): the Diablon behavior that drops the bomb.
 * It has the parts charge and drop the bomb (with a hint message), waits for the bomb time,
 * then has the parts stop and switches to the idle behavior once the parts are done (flag 4).
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/Diablon.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* The hint manager (class name unknown; see HintRing.cpp and EnemyHint.cpp). */
class UnkHintManager_801FB180;

/* The out-of-line copy of the inline accessor HintRing.cpp defines (0x80109D64). */
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* The hint manager showing message id of branch (see HintRing.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
}

namespace Boss {
namespace Diablon {
namespace Behavior {

class Bomb : public BehaviorBase {
  public:
    Bomb(const boost::shared_ptr<Diablon> &owner) : BehaviorBase(owner, "Bomb", 3) {}
    virtual ~Bomb();
    virtual void Begin();
    virtual void End();
    virtual void Tick(float dt);

    static boost::shared_ptr<BehaviorBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
    static void Setup(void *executer);
    static void Teardown(void *executer);

    /* 0x14 */ bool m_firing; /* the parts were told to fire and not yet to stop (name inferred) */
    /* 0x18 */ float m_timer;
    /* 0x1C */ s32 m_state;
};
SIZE_ASSERT(Bomb, 0x20);

Bomb::~Bomb() {}

void Bomb::Begin() {
    fn_802D4ED4(&m_owner->m_flags, 7);
    m_firing = false;
    m_state = 0;
}

void Bomb::End() {
    fn_802D4EF4(&m_owner->m_flags, 7);
    if (m_firing && m_owner->m_executer != 0) {
        fn_802D9E4C(m_owner->m_executer, Parts::Action(10));
    }
}

void Bomb::Tick(float dt) {
    switch (m_state) {
    case 0:
        fn_802D9E4C(m_owner->m_executer, Parts::Action(8));
        m_firing = true;
        fn_801FAF00(GetHintManager(), m_owner->m_params->m_bombHintId, 0, 1, 0);
        m_state++;
        break;
    case 1:
        if (fn_802D58FC(&m_owner->m_flags, 4)) {
            fn_802D9E4C(m_owner->m_executer, Parts::Action(9));
            fn_802D9E4C(m_owner->m_executer, Parts::Action(14));
            m_timer = 0.0f;
            m_state++;
        }
        break;
    case 2:
        m_timer += dt;
        if (m_timer >= m_owner->m_params->m_bombGrowTime && fn_802D58FC(&m_owner->m_flags, 4)) {
            m_state = 100;
        }
        break;
    case 100:
        fn_802D9E4C(m_owner->m_executer, Parts::Action(10));
        m_firing = false;
        m_state++;
        break;
    case 101:
        fn_802D9ED8(m_owner->m_executer, InitializeArgs(1));
        m_state++;
        break;
    }
}

boost::shared_ptr<BehaviorBase> Bomb::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<BehaviorBase>(new Bomb(owner));
}

void Bomb::Setup(void *executer) {}

void Bomb::Teardown(void *executer) {}

/* The behavior table's entry (name inferred). */
extern const Info g_bombInfo;
const Info g_bombInfo = {Bomb::Instantiate, Bomb::Setup, Bomb::Teardown};

} // namespace Behavior
} // namespace Diablon
} // namespace Boss

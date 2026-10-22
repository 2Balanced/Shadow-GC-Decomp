/* Boss::Diablon::Behavior::Wipe (class name from RTTI): the Diablon behavior that sweeps the arena.
 * It has the parts start the sweep (with a hint message), run it and end it, each step waiting for
 * the parts to be done (flag 4), then switches to the idle behavior. File name inferred from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
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

class Wipe : public BehaviorBase {
  public:
    Wipe(const boost::shared_ptr<Diablon> &owner) : BehaviorBase(owner, "Wipe", 5) {}
    virtual ~Wipe();
    virtual void Begin(int previous, const InitializeArgs &args);
    virtual void End(int next);
    virtual void Tick(float dt);

    static boost::shared_ptr<BehaviorBase> Instantiate(const boost::shared_ptr<Diablon> &owner);
    static void Setup(void *executer);
    static void Teardown(void *executer);

    /* 0x14 */ s32 m_state;
};
SIZE_ASSERT(Wipe, 0x18);

Wipe::~Wipe() {}

void Wipe::Begin(int previous, const InitializeArgs &args) {
    m_owner->m_flags.Set(7);
    m_state = 0;
}

void Wipe::End(int next) {
    m_owner->m_flags.Reset(7);
}

void Wipe::Tick(float dt) {
    switch (m_state) {
    case 0:
        fn_802D9E4C(m_owner->m_executer, Parts::Action(2));
        fn_801FAF00(GetHintManager(), m_owner->m_params->m_wipeHintId, 0, 1, 0);
        m_state++;
        break;
    case 1:
        if (m_owner->m_flags.Test(4)) {
            fn_802D9E4C(m_owner->m_executer, Parts::Action(3));
            m_state++;
        }
        break;
    case 2:
        if (m_owner->m_flags.Test(4)) {
            m_state = 100;
        }
        break;
    case 100:
        fn_802D9E4C(m_owner->m_executer, Parts::Action(4));
        m_state++;
        break;
    case 101:
        if (m_owner->m_flags.Test(4)) {
            fn_802D9ED8(m_owner->m_executer, InitializeArgs(1));
            m_state++;
        }
        break;
    }
}

boost::shared_ptr<BehaviorBase> Wipe::Instantiate(const boost::shared_ptr<Diablon> &owner) {
    return boost::shared_ptr<BehaviorBase>(new Wipe(owner));
}

void Wipe::Setup(void *executer) {}

void Wipe::Teardown(void *executer) {}

/* The behavior table's entry (name inferred). */
extern const Info g_wipeInfo;
const Info g_wipeInfo = {Wipe::Instantiate, Wipe::Setup, Wipe::Teardown};

} // namespace Behavior
} // namespace Diablon
} // namespace Boss

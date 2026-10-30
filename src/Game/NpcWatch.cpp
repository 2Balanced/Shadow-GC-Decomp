/* NpcWatch::Watch (class name from RTTI; base Task; its name is "Npc::Watch"): the task each NPC
 * keeps beside it. It ends once the NPC is gone, and when it is destroyed it takes the NPC out of
 * the character manager. Its maker keeps its address as its name: other units call it by that.
 * File name from the RTTI name; function names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/CharInfo.h"
#include "Game/Npc.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using boost::shared_ptr;
using boost::weak_ptr;
using Player::Npc::Npc;

/* The character manager (class name unknown; fn_8006F754 returns it). */
class UnkCharManager_8006F754;

extern "C" {
UnkCharManager_8006F754 *fn_8006F754(void);
/* Takes an NPC out of the character manager (behaviour inferred). */
void fn_80074CE8(UnkCharManager_8006F754 *manager, const shared_ptr<Npc> &npc);
}

namespace NpcWatch {

class Watch : public Task {
  public:
    Watch(Task *parent, const shared_ptr<Npc> &npc);
    virtual ~Watch();
    virtual void Tick(float dt);

    /* 0x28 */ weak_ptr<Npc> m_npc;
};
SIZE_ASSERT(Watch, 0x30);

Watch::Watch(Task *parent, const shared_ptr<Npc> &npc) : Task(parent), m_npc(npc) {
    m_name = "Npc::Watch";
}

Watch::~Watch() {
    shared_ptr<Npc> npc = boost::dynamic_pointer_cast<Npc>(m_npc.lock());
    if (npc) {
        fn_80074CE8(fn_8006F754(), npc);
    }
}

void Watch::Tick(float dt) {
    if (m_npc.expired()) {
        m_flags |= 1;
    }
}

} // namespace NpcWatch

/* Makes the watch of an NPC (called by the NPC classes by its address). */
extern "C" Task *fn_80075E30(Task *parent, const shared_ptr<Npc> &npc) {
    return new NpcWatch::Watch(parent, npc);
}

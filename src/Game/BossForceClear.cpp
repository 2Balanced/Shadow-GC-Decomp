/* BossForceClear (class name from RTTI; vtable 0x8055925C): a task named "ForceClear" that BossCtrl
 * creates under itself for every boss (Game/BossCtrl.cpp). In this build it does nothing: its tick
 * is empty and the check the bosses make (fn_802DFA00, see Game/EggLastMech.cpp) always answers
 * false; presumably a debug feature to clear a boss at once. File name inferred from the class.
 * Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"

class BossForceClear : public Task {
  public:
    BossForceClear(Task *parent);
    virtual ~BossForceClear();
    virtual void Tick(float dt);

    /* 0x28 */ Task *m_parent; /* (name inferred) */
    /* 0x2C */ u32 pad_0x2C;   /* the class size is a multiple of 8 */
};
SIZE_ASSERT(BossForceClear, 0x30);

BossForceClear::BossForceClear(Task *parent) : Task(parent) {
    m_parent = parent;
    m_name = "ForceClear";
}

BossForceClear::~BossForceClear() {}

void BossForceClear::Tick(float dt) {}

/* Whether a boss should be cleared at once (name of the function unknown; always false here). */
extern "C" bool fn_802DFA00(void) {
    return false;
}

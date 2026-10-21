/* Mission::EnemyMission (class name from RTTI): the mission of destroying a number of enemies of one
 * kind. Each frame it copies the count of destroyed enemies of its kind into the mission's count.
 * The mission factory at 0x8016B384 builds it for mission type 3. Member names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/Mission.h"

/* The object that counts destroyed enemies (class name unknown; see Game/UnkSingletons.h). */
class UnkObject_801A2AF4;

extern "C" {
/* Returns the object, creating it on first use (see Game/SavePoint.cpp). */
UnkObject_801A2AF4 *fn_80119178(void);
/* The number of destroyed enemies of a kind (behaviour inferred). */
s32 fn_801A2140(UnkObject_801A2AF4 *counter, int kind);
}

namespace Mission {

class EnemyMission : public CountMission {
  public:
    EnemyMission(int id, int player, int kind, int target, int count);
    virtual ~EnemyMission();
    virtual int GetType();
    virtual void OnUpdate();

    /* 0x58 */ int m_kind; /* the kind of enemy counted */
};
SIZE_ASSERT(EnemyMission, 0xA0);

EnemyMission::EnemyMission(int id, int player, int kind, int target, int count)
    : Mission(id, player), CountMission(id, player, target, 0, count, 0), m_kind(kind) {}

EnemyMission::~EnemyMission() {}

void EnemyMission::OnUpdate() {
    SetCount(fn_801A2140(fn_80119178(), m_kind), 0);
}

int EnemyMission::GetType() {
    return 3;
}

} // namespace Mission

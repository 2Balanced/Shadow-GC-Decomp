/* Mission::GoalMission (class name from RTTI): the mission of reaching the goal. It adds nothing to
 * Mission but its type; the goal clears it from outside. The mission factory at 0x8016B384 builds
 * it for mission type 1. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Game/Mission.h"

namespace Mission {

class GoalMission : public Mission {
  public:
    GoalMission(int id, int player);
    virtual ~GoalMission();
    virtual int GetType();
};
SIZE_ASSERT(GoalMission, 0x40);

GoalMission::GoalMission(int id, int player) : Mission(id, player) {}

GoalMission::~GoalMission() {}

int GoalMission::GetType() {
    return 1;
}

} // namespace Mission

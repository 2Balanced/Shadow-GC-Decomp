/* Mission::TimerGoalMission (class name from RTTI; declared in Game/TimerMission.h): reaching the
 * goal within a time limit. It adds only its type to Mission::TimerMission; the goal clears it from
 * outside. The mission factory at 0x8016B384 builds it for mission type 5. This unit also holds the
 * linked copies of TimerMission's inline GetTimeLimit and IsTimed and of the thunks both timer
 * missions share. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/TimerMission.h"

namespace Mission {

TimerGoalMission::TimerGoalMission(int id, int player, float time)
    : Mission(id, player), TimerMission(id, player, time) {}

TimerGoalMission::~TimerGoalMission() {}

int TimerGoalMission::GetType() {
    return 5;
}

} // namespace Mission

#ifndef GAME_TIMERMISSION_H
#define GAME_TIMERMISSION_H

#include "Game/Mission.h"
#include "Game/TimerMessage.h"

namespace Mission {

/* Mission::TimerMission (class name from RTTI; Game/TimerMission.cpp): a mission with a time
 * limit, which fails when the stage timer runs out. The mission factory at 0x8016B384 builds it for
 * mission type 4 (GetType). Member names are inferred. */
class TimerMission : public virtual Mission {
  public:
    TimerMission(int id, int player, float time);
    virtual ~TimerMission();
    virtual int GetType();
    virtual bool IsTimed() {
        return true;
    }
    virtual float GetTimeLimit() {
        return m_time;
    }
    virtual void OnSave();
    virtual void OnRestore();
    virtual void OnReset();
    virtual void OnUpdate();

    /* 0x08 */ float m_time;
    /* 0x0C */ TimerMessageManage m_message; /* the timer display */
};
SIZE_ASSERT(TimerMission, 0x80);

/* Mission::TimerGoalMission (class name from RTTI; Game/TimerGoalMission.cpp): reaching the goal
 * within the time limit. The goal clears it from outside. */
class TimerGoalMission : public TimerMission {
  public:
    TimerGoalMission(int id, int player, float time);
    virtual ~TimerGoalMission();
    virtual int GetType();
};
SIZE_ASSERT(TimerGoalMission, 0x80);

} // namespace Mission

#endif

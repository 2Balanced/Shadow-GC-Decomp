/* Mission::TimerCountMission (class name from RTTI): a counting mission with a time limit. It is a
 * CountMission and a TimerMission at once (Mission is their virtual base), forwards the counting
 * to CountMission and saves, restores and resets both halves. The mission factory at 0x8016B384
 * builds it for mission type 6. Member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/Mission.h"

namespace Mission {

/* Mission::TimerMission (class name from RTTI; code at 0x802C8AF4, another unit): a mission with a
 * time limit, which fails when the stage timer runs out. Only what this unit uses is declared. */
class TimerMission : public virtual Mission {
  public:
    TimerMission(int id, int player, float time);
    virtual ~TimerMission();
    virtual int GetType();
    virtual bool IsTimed();
    virtual float GetTimeLimit();
    virtual void OnSave();
    virtual void OnRestore();
    virtual void OnReset();
    virtual void OnUpdate();

    /* 0x08 */ float m_time;
    /* 0x0C */ u8 m_message[0x30]; /* the timer display (Mission::TimerMessageManage) */
};

class TimerCountMission : public CountMission, public TimerMission {
  public:
    TimerCountMission(int id, int player, float time, int target);
    virtual ~TimerCountMission();
    virtual void OnSave();
    virtual void OnRestore();
    virtual void OnReset();
    virtual int GetType();
    virtual void OnUpdate();
    virtual int GetCount(int index);
    virtual int GetTarget();
    virtual int GetSubTarget();
    virtual void AddCount(int delta, int index);
    virtual void SetCount(int value, int index);
};
SIZE_ASSERT(TimerCountMission, 0xD8);

TimerCountMission::TimerCountMission(int id, int player, float time, int target)
    : Mission(id, player), CountMission(id, player, target, 0, 0, 0),
      TimerMission(id, player, time) {}

TimerCountMission::~TimerCountMission() {}

void TimerCountMission::OnSave() {
    CountMission::OnSave();
    TimerMission::OnSave();
}

void TimerCountMission::OnRestore() {
    CountMission::OnRestore();
    TimerMission::OnRestore();
}

void TimerCountMission::OnReset() {
    CountMission::OnReset();
    TimerMission::OnReset();
}

int TimerCountMission::GetType() {
    return 6;
}

void TimerCountMission::OnUpdate() {
    TimerMission::OnUpdate();
}

int TimerCountMission::GetCount(int index) {
    return CountMission::GetCount(index);
}

int TimerCountMission::GetTarget() {
    return CountMission::GetTarget();
}

int TimerCountMission::GetSubTarget() {
    return CountMission::GetSubTarget();
}

void TimerCountMission::AddCount(int delta, int index) {
    CountMission::AddCount(delta, index);
}

void TimerCountMission::SetCount(int value, int index) {
    CountMission::SetCount(value, index);
}

} // namespace Mission

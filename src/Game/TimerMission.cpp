/* Mission::TimerMission (class name from RTTI; declared in Game/TimerMission.h): a mission with a
 * time limit. Each frame it passes the time left to its timer display and fails when the time runs
 * out. Saving, restoring and resetting go to the display. Its inline GetTimeLimit and IsTimed, and
 * the thunks the subclasses share, are linked from Game/TimerGoalMission.cpp. Member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/TimerMission.h"

/* The game state (see Game/TGi.h); the float at 0xA0 is the time left (meaning inferred). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0xA0];
    /* 0x0A0 */ float m_timeLeft;
    /* 0x0A4 */ u8 field_0xA4[0x150 - 0xA4];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern "C" {
extern UnkGameState_8057D6A0 lbl_8057D6A0;
/* Whether a value is (nearly) zero (see Game/TimerMessage.cpp). */
bool fn_8000A950(float value);
/* Fails a mission (Game/Mission.cpp). */
void fn_8016B270(Mission::Mission *mission);
}

namespace Mission {

TimerMission::TimerMission(int id, int player, float time)
    : Mission(id, player), m_time(time), m_message(id, player, time) {}

TimerMission::~TimerMission() {}

void TimerMission::OnUpdate() {
    float timeLeft = lbl_8057D6A0.m_timeLeft;
    m_message.Update(timeLeft);
    if (fn_8000A950(timeLeft)) {
        Mission &mission = *this;
        fn_8016B270(&mission);
    }
}

void TimerMission::OnSave() {
    m_message.Save();
}

void TimerMission::OnRestore() {
    m_message.Restore();
}

void TimerMission::OnReset() {
    m_message.Reset();
}

int TimerMission::GetType() {
    return 4;
}

} // namespace Mission

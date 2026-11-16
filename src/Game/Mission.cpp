/* Mission::Mission (class name from RTTI): a stage's mission for one player, its state and the
 * message manager that shows it. The subclasses live in their own units (Game/CountMission.cpp,
 * Game/GoalMission.cpp, Game/EnemyMission.cpp, Game/RingMission.cpp; the timer missions at
 * 0x802C8AF4 and 0x802EB7F0), the table of mission kinds in Game/MissionTable.cpp and the manager
 * in Game/MissionManagerTask.cpp (they were one unit until wave 39: see Game/CountMission.cpp).
 * File name from the namespace; all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). Functions other units call under
 * their addresses (fn_8016B270, fn_8016B2A4) keep C names here.
 *
 * The eight stage-event callbacks at 0x8016A9E0..0x8016AA90 before Game/CountMission.cpp are not
 * part of these units: only the stage table at 0x804C4A18 (another unit's .rodata) refers to
 * them, so their owner is not clear.
 *
 * NonMatching: OnFailed (see there); the other 22 functions and every data section match. */
#include "Game/MissionManager.h"
#include <stdio.h>
#include <string.h>

namespace Mission {

Mission::Mission(int id, int player)
    : m_id(id), m_player(player), m_state(0), m_savedState(0), m_flag(0), m_savedFlag(0),
      m_message(id, player) {}

Mission::~Mission() {}

} // namespace Mission

extern "C" void fn_8016B2A4(Mission::Mission *mission) {
    mission->m_state = 1;
    mission->OnCleared();
}

extern "C" void fn_8016B270(Mission::Mission *mission) {
    if (mission->m_state != 1) {
        mission->m_state = 2;
        mission->OnFailed();
    }
}

namespace Mission {

int Mission::GetCount(int index) {
    if (index == 0) {
        return IsState(1);
    }
    return IsState(2);
}

int Mission::GetTarget() {
    return 1;
}

int Mission::GetSubTarget() {
    return 1;
}

float Mission::GetTimeLimit() {
    return 0.0f;
}

void Mission::AddCount(int delta, int index) {}

void Mission::SetCount(int value, int index) {}

void Mission::SetTarget(int value, int index) {}

int Mission::vf_0x30() {
    return 0;
}

void Mission::vf_0x34() {}

void Mission::Update(float dt) {
    if (!IsState(2) && !IsState(1)) {
        OnUpdate();
    }
}

void Mission::Save() {
    m_savedState = m_state;
    m_savedFlag = m_flag;
    MessageOf(m_message).Save();
    OnSave();
}

void Mission::Restore() {
    m_state = m_savedState;
    m_flag = m_savedFlag;
    MessageOf(m_message).Restore();
    OnRestore();
}

void Mission::Reset() {
    m_state = m_savedState = 0;
    m_flag = m_savedFlag = 0;
    MessageOf(m_message).Reset();
    OnReset();
}

void Mission::OnCleared() {
    if (fn_80074470()->m_mode != 9) {
        m_message.Show(kMissionCleared);
        fn_80175EEC(fn_80074470(), 6, 0);
    }
}

/* NonMatching (93.6%), the only open function of the unit (its data matches): retail forms the
 * game state's address in r5 and loads the format string's address last, into r4; GC/2.7 forms
 * the state's address in r4 and the string's in r5 first. About ten source shapes were tried
 * before wave 39 and twenty more in it (probes/Game_Mission.py, probes/Game_Mission_R2.py): the
 * language or the suffix through inline functions or locals, the format as a named array or a
 * local pointer, const and non-const declarations of the table and of the game state. All give
 * the same six instructions. */
void Mission::OnFailed() {
    if (fn_80074470()->m_mode != 9) {
        MissionManager *manager = fn_8006D6A8();
        if (m_player == manager->m_currentPlayer) {
            manager->SetPlayer(1);
        }
        m_flag = 0;
        m_message.Show(kMissionFailed);
        if (!manager->m_allTimed) {
            char path[0x40];
            memset(path, 0, sizeof(path));
            sprintf(path, "CsdFiles/MissionFailed%s", lbl_8051D528[lbl_8057D6A0.m_language]);
            if (m_player == 2) {
                fn_8035367C(path, "mf", "Hero");
            } else if (m_player == 0) {
                fn_8035367C(path, "mf", "Dark");
            }
            fn_801D560C(fn_80053164(), 0x1028, 0, 0);
        }
        manager->CheckAllFailed();
    }
}

void Mission::OnReset() {}

void Mission::OnRestore() {}

void Mission::OnSave() {}

int Mission::GetType() {
    return 0;
}

} // namespace Mission

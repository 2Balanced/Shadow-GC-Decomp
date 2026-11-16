/* Mission::MissionManagerTask (class name from RTTI) and the manager it runs: the missions of
 * the three players and the timer they share. The manager is the object fn_8006D6A8 returns,
 * which other units call UnkPlayerManager_8006D6A8. Part of the stage missions (see
 * Game/Mission.cpp; split out of it in wave 39, see Game/CountMission.cpp). File name from the
 * class; all other names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). Functions other units call under their addresses
 * (fn_8016DA34, fn_8016DFF8) keep C names here. */
#define MSL_VECTOR_MIDDLE
/* destroy_all comes out after reserve, among the vector's last members (see <vector>). */
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_VECTOR_DESTROY_ALL_FIRST
#include "Game/MissionManager.h"
#include <vector>

namespace Mission {

MissionManagerTask::~MissionManagerTask() {
    fn_8006D6A8()->Release();
}

void MissionManagerTask::Tick(float dt) {
    fn_8006D6A8()->Update(dt);
}

MissionManager::MissionManager() : m_task(0), m_stage(-1), m_currentPlayer(1) {}

MissionManager::~MissionManager() {}

void MissionManager::Init(Task *parent, int stage) {
    m_stage = stage;
    m_currentPlayer = m_savedPlayer = 1;
    SetPlayer(m_currentPlayer);
    if (lbl_8057D6A0.field_0x125) {
        fn_800B74B4();
        /* Byte arithmetic: indexing the array (lbl_804C4A18[stage].m_missionId) swaps two
         * registers. */
        m_missions[1] = m_table.Create(
            *reinterpret_cast<int *>(reinterpret_cast<u8 *>(lbl_804C4A18) + stage * sizeof(UnkStageInfo_804C4A18) + 0x40),
            1);
    } else {
        for (int i = 0; i < 3; i++) {
            int id = fn_80176BE4(fn_800B74B4(), m_stage, i);
            m_missions[i] = m_table.Create(id, i);
        }
    }
    m_hasTimer = false;
    m_timeLimit = -1.0f;
    m_allTimed = false;
    int count = 0;
    int timed = 0;
    for (int i = 0; i < 3; i++) {
        if (m_missions[i].get() != 0) {
            count++;
            if (m_missions[i]->IsTimed()) {
                float limit = m_missions[i]->GetTimeLimit();
                m_timeLimit = *fn_80070494(&m_timeLimit, &limit);
                timed++;
            }
        }
    }
    if (timed > 0) {
        m_hasTimer = true;
        if (timed == count) {
            m_allTimed = true;
        }
    }
    fn_80336FE4(lbl_8057D6A0.m_timer);
    if (m_hasTimer) {
        m_timeLimit -= 0.001f;
        fn_80336FC0(lbl_8057D6A0.m_timer, m_timeLimit);
        if (m_allTimed) {
            lbl_8057D6A0.m_timerShown = 1;
        } else {
            lbl_8057D6A0.m_timerShown = 0;
        }
    } else {
        fn_80336FD0(lbl_8057D6A0.m_timer);
        lbl_8057D6A0.m_timerShown = 0;
    }
    MarkStartMission();
    m_task = new MissionManagerTask(parent);
}

/* Mission 0x1C (meaning unknown) starts flagged and makes its player the current one. */
void MissionManager::MarkStartMission() {
    for (int i = 0; i < 3; i++) {
        if (m_missions[i].get() != 0) {
            switch (m_missions[i]->GetId()) {
            case 0x1C:
                m_missions[i]->SetFlag();
                m_missions[i]->m_savedFlag = 1;
                m_currentPlayer = m_savedPlayer = i;
                SetPlayer(m_currentPlayer);
                break;
            }
        }
    }
}

} // namespace Mission

extern "C" void fn_8016DFF8(Mission::MissionManager *manager) {
    for (int i = 0; i < 3; i++) {
        if (manager->m_missions[i].get() != 0) {
            manager->m_missions[i]->Save();
        }
    }
    if (manager->m_hasTimer) {
        fn_80336FB4(lbl_8057D6A0.m_timer);
    }
    manager->m_savedPlayer = manager->m_currentPlayer;
}

namespace Mission {

void MissionManager::RestoreAll() {
    for (int i = 0; i < 3; i++) {
        if (m_missions[i].get() != 0) {
            m_missions[i]->Restore();
        }
    }
    if (m_hasTimer) {
        fn_80336F98(lbl_8057D6A0.m_timer);
    }
    SetPlayer(m_savedPlayer);
}

void MissionManager::ResetAll() {
    for (int i = 0; i < 3; i++) {
        if (m_missions[i].get() != 0) {
            m_missions[i]->Reset();
        }
    }
    if (m_hasTimer) {
        fn_80336FC0(lbl_8057D6A0.m_timer, m_timeLimit);
    }
    SetPlayer(1);
    MarkStartMission();
}

void MissionManager::SetPlayer(int player) {
    m_currentPlayer = player;
    fn_800749FC(fn_8006F754(), m_currentPlayer);
    if (m_hasTimer && !m_allTimed) {
        if (m_missions[player].get() != 0) {
            if (m_missions[player]->IsTimed()) {
                lbl_8057D6A0.m_timerShown = 1;
            } else {
                lbl_8057D6A0.m_timerShown = 0;
            }
        } else {
            lbl_8057D6A0.m_timerShown = 0;
        }
    }
}

u8 MissionManager::IsFlagSet(int player) {
    if (lbl_8057D6A0.field_0xB1) {
        return false;
    }
    if (player == 1) {
        return true;
    }
    if (m_missions[player].get() != 0) {
        return m_missions[player]->GetFlag();
    }
    return false;
}

} // namespace Mission

/* Clears the first goal mission still running (a timer goal mission first); returns whether one
 * was cleared. */
extern "C" bool fn_8016DA34(Mission::MissionManager *manager) {
    std::vector<boost::shared_ptr<Mission::Mission> > goals;
    goals.clear();
    goals.reserve(3);
    for (int i = 0; i < 3; i++) {
        if (manager->m_missions[i].get() != 0 &&
            (manager->m_missions[i]->GetType() == 5 || manager->m_missions[i]->GetType() == 1)) {
            goals.push_back(manager->m_missions[i]);
        }
    }
    typedef std::vector<boost::shared_ptr<Mission::Mission> >::iterator Iterator;
    for (Iterator it = goals.begin(); it != goals.end(); ++it) {
        if ((*it)->GetType() == 5 && !(*it)->IsState(2)) {
            fn_8016B2A4(it->get());
            return true;
        }
    }
    for (Iterator it2 = goals.begin(); it2 != goals.end(); ++it2) {
        if ((*it2)->GetType() == 1) {
            fn_8016B2A4(it2->get());
            return true;
        }
    }
    return false;
}

namespace Mission {

void MissionManager::CheckAllFailed() {
    for (int i = 0; i < 3; i++) {
        if (m_missions[i].get() != 0 && m_missions[i]->m_state != 2) {
            return;
        }
    }
    fn_80175EEC(fn_80074470(), 0xF, 0);
}

void MissionManager::Update(float dt) {
    for (int i = 0; i < 3; i++) {
        if (m_missions[i].get() != 0) {
            m_missions[i]->Update(dt);
        }
    }
}

void MissionManager::Release() {
    m_stage = -1;
    for (int i = 0; i < 3; i++) {
        m_missions[i].reset();
    }
    fn_80336FD0(lbl_8057D6A0.m_timer);
    m_task = 0;
}

} // namespace Mission

#include <vector_impl>

const char *Mission::MissionManagerTask::s_taskName = "MissionManagerTask";

/* Not called anywhere: the linker strips it from the game. It creates the task's RTTI records
 * where retail has them, the name "Task" first in .sdata2 (see Game/TrigerCounter.cpp). */
const std::type_info &TaskTypes(int i) {
    if (i == 0) {
        return typeid(Task);
    }
    return typeid(Mission::MissionManagerTask);
}

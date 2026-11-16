/* Mission::CountMission (class name from RTTI): a mission that counts towards a target (its
 * subclasses EnemyMission, RingMission and TimerCountMission live in their own units). The unit
 * also defines Mission's OnUpdate and IsTimed, which retail emits here. File name from the class;
 * member names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * Split out of Game/Mission.cpp in wave 39: retail's sections show four translation units in the
 * old unit's range (each has its own copy of Mission's RTTI name or of "Memory allocation
 * failure", and its sections are padded to 8 bytes): this one, Game/Mission.cpp,
 * Game/MissionTable.cpp and Game/MissionManagerTask.cpp. */
#include "Game/MissionManager.h"

namespace Mission {

CountMission::CountMission(int id, int player, int target, int subTarget, int count, int subCount)
    : Mission(id, player), m_target(target), m_subTarget(subTarget), m_initCount(count),
      m_initSubCount(subCount), m_message(id, player, target) {
    OnReset();
}

CountMission::~CountMission() {}

int CountMission::GetCount(int index) {
    if (index == 0) {
        return m_count;
    }
    return m_subCount;
}

int CountMission::GetTarget() {
    return m_target;
}

int CountMission::GetSubTarget() {
    return m_subTarget;
}

void CountMission::AddCount(int delta, int index) {
    if (index == 0) {
        if (delta < 0) {
            m_count += delta;
            if (m_count < 0) {
                m_count = 0;
            }
        } else {
            m_count += delta;
            m_message.SetCount(m_count);
            if (m_count >= m_target) {
                fn_8016B2A4(&static_cast<Mission &>(*this));
            }
        }
    } else if (m_subTarget != 0) {
        m_subCount += delta;
        if (m_subCount >= m_subTarget) {
            fn_8016B270(&static_cast<Mission &>(*this));
        }
    }
}

void CountMission::SetCount(int value, int index) {
    if (index == 0) {
        if (value < 0) {
            m_count = 0;
        } else {
            m_count = value;
            m_message.SetCount(m_count);
            if (m_count >= m_target) {
                fn_8016B2A4(&static_cast<Mission &>(*this));
            }
        }
    } else if (m_subTarget != 0) {
        m_subCount = value;
        if (m_subCount >= m_subTarget) {
            fn_8016B270(&static_cast<Mission &>(*this));
        }
    }
}

void CountMission::SetTarget(int value, int index) {
    if (index == 0) {
        m_target = value;
    } else {
        m_subTarget = value;
    }
}

void CountMission::OnSave() {
    m_savedCount = m_count;
    m_savedSubCount = m_subCount;
    MessageOf(m_message).Save();
}

void CountMission::OnRestore() {
    m_count = m_savedCount;
    m_subCount = m_savedSubCount;
    MessageOf(m_message).Restore();
}

void CountMission::OnReset() {
    m_count = m_savedCount = m_initCount;
    m_subCount = m_savedSubCount = m_initSubCount;
    MessageOf(m_message).Reset();
}

int CountMission::GetType() {
    return 2;
}

void Mission::OnUpdate() {}

bool Mission::IsTimed() {
    return false;
}

} // namespace Mission

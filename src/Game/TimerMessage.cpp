/* Mission::TimerMessage and Mission::TimerMessageManage (class names from RTTI): the hints a
 * timed mission shows as its time runs down. The manager keeps a message for each of the
 * mission's hints of kind 2 (see Game/MissionMessage.h), each with the time left that shows it,
 * sorted so that the next one is last; the timer missions (code at 0x802C8AF4) pass it the time
 * left. File name from the class; all other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_BACK
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_ALGORITHM_SORT
#define BOOST_SP_FREE_SWAP
#include "Game/TimerMessage.h"

/* The mission manager (see Game/Mission.cpp): the current player at 0x20. */
struct UnkMissionManager_8006D6A8 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ int m_currentPlayer;
};

extern "C" {
UnkMissionManager_8006D6A8 *fn_8006D6A8(void);
/* Whether a value is (nearly) zero, and whether two values are nearly equal (see
 * Game/ArkCannon.cpp and Game/CameraScreenSet.cpp). */
bool fn_8000A950(float value);
bool fn_801C455C(float a, float b);
}

namespace Mission {

TimerMessage::~TimerMessage() {}

/* Sorts the messages with the least time left first. */
static bool CompareTime(boost::shared_ptr<TimerMessage> a, boost::shared_ptr<TimerMessage> b) {
    return a->m_time < b->m_time;
}

TimerMessageManage::TimerMessageManage(int id, int player, float time)
    : MessageManage(id, player), m_time(time), m_next(-1.0f) {
    m_messages.reserve(m_infoCount);
    m_saved.reserve(m_infoCount);
    Reset();
}

TimerMessageManage::~TimerMessageManage() {}

void TimerMessageManage::Update(float time) {
    while (m_next >= 0.0f || fn_8000A950(m_next)) {
        if (!(time <= m_next || fn_801C455C(time, m_next))) {
            break;
        }
        int current = fn_8006D6A8()->m_currentPlayer;
        {
            boost::shared_ptr<TimerMessage> message = m_messages.back();
            const MessageInfo *info = message->m_info;
            if (info->m_player == -1 && m_player == current) {
                message->Show(m_player);
            } else if (info->m_player == current) {
                message->Show(info->m_player);
            }
        }
        m_messages.pop_back();
        UpdateNext();
    }
}

void TimerMessageManage::Save() {
    m_saved.clear();
    for (std::vector<boost::shared_ptr<TimerMessage> >::iterator it = m_messages.begin(); it != m_messages.end(); ++it) {
        m_saved.push_back(boost::shared_ptr<TimerMessage>(*it));
    }
}

void TimerMessageManage::Restore() {
    m_messages.clear();
    for (std::vector<boost::shared_ptr<TimerMessage> >::iterator it = m_saved.begin(); it != m_saved.end(); ++it) {
        m_messages.push_back(boost::shared_ptr<TimerMessage>(*it));
    }
    UpdateNext();
}

void TimerMessageManage::Reset() {
    m_messages.clear();
    m_saved.clear();
    if (m_infos != 0) {
        for (u32 i = 0; i < m_infoCount; i++) {
            const MessageInfo *info = &m_infos[i];
            if (info->m_kind == 2) {
                m_messages.push_back(boost::shared_ptr<TimerMessage>(new TimerMessage(info, m_time)));
            }
        }
        std::sort(m_messages.begin(), m_messages.end(), &CompareTime);
        UpdateNext();
    }
}

void TimerMessageManage::UpdateNext() {
    if (m_messages.empty()) {
        m_next = -1.0f;
    } else {
        m_next = m_messages.back()->m_time;
    }
}

} // namespace Mission

#include <vector_impl>

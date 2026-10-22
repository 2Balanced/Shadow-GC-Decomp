/* Mission::CountMessage and Mission::CountMessageManage (class names from RTTI): the hints a
 * counting mission shows as its count grows. The manager keeps a message for each of the mission's
 * hints of kind 1 (see Game/MissionMessage.h), each with the count that shows it, sorted so that
 * the next one is last; Mission::CountMission (Game/Mission.cpp) passes it each new count. File
 * name from the class; all other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_BACK
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_ALGORITHM_SORT
#define BOOST_SP_FREE_SWAP
#include <vector>
#include "Game/MissionMessage.h"
#include "boost/shared_ptr.hpp"
#include <cmath>

/* The mission manager (see Game/Mission.cpp): the current player at 0x20. */
struct UnkMissionManager_8006D6A8 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ int m_currentPlayer;
};

extern "C" UnkMissionManager_8006D6A8 *fn_8006D6A8(void);

namespace Mission {

class CountMessage : public Message {
  public:
    CountMessage(const MessageInfo *info, int target);
    virtual ~CountMessage();

    /* 0x8 */ int m_count; /* the count that shows it */
};
SIZE_ASSERT(CountMessage, 0xC);

class CountMessageManage : public MessageManage {
  public:
    CountMessageManage(int id, int player, int target);
    ~CountMessageManage();
    virtual void Save();
    virtual void Restore();
    virtual void Reset();

    void SetCount(s32 count);
    void UpdateNext();

    /* 0x10 */ int m_target;
    /* 0x14 */ s32 m_next; /* the count that shows the next message; -1: none left */
    /* 0x18 */ std::vector<boost::shared_ptr<CountMessage> > m_messages;
    /* 0x24 */ std::vector<boost::shared_ptr<CountMessage> > m_saved;
};
SIZE_ASSERT(CountMessageManage, 0x30);

/* Trigger 0 counts up to m_count, 1 to that fraction of the target, 2 to m_count below it. */
CountMessage::CountMessage(const MessageInfo *info, int target) : Message(info) {
    switch (info->m_trigger) {
    case 0:
        m_count = m_info->m_count;
        break;
    case 1:
        m_count = std::ceil(target * m_info->m_ratio);
        break;
    case 2:
        m_count = target - m_info->m_count;
        break;
    default:
        m_count = 0;
        break;
    }
}

CountMessage::~CountMessage() {}

/* Sorts the messages with the highest count first. */
static bool CompareCount(boost::shared_ptr<CountMessage> a, boost::shared_ptr<CountMessage> b) {
    return a->m_count > b->m_count;
}

CountMessageManage::CountMessageManage(int id, int player, int target)
    : MessageManage(id, player), m_target(target), m_next(-1) {
    m_messages.reserve(m_infoCount);
    m_saved.reserve(m_infoCount);
    Reset();
}

CountMessageManage::~CountMessageManage() {}

void CountMessageManage::SetCount(s32 count) {
    while (m_next != -1) {
        if (m_next > count) {
            break;
        }
        int current = fn_8006D6A8()->m_currentPlayer;
        {
            boost::shared_ptr<CountMessage> message = m_messages.back();
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

void CountMessageManage::Save() {
    m_saved.clear();
    for (std::vector<boost::shared_ptr<CountMessage> >::iterator it = m_messages.begin(); it != m_messages.end(); ++it) {
        m_saved.push_back(boost::shared_ptr<CountMessage>(*it));
    }
}

void CountMessageManage::Restore() {
    m_messages.clear();
    for (std::vector<boost::shared_ptr<CountMessage> >::iterator it = m_saved.begin(); it != m_saved.end(); ++it) {
        m_messages.push_back(boost::shared_ptr<CountMessage>(*it));
    }
    UpdateNext();
}

void CountMessageManage::Reset() {
    m_messages.clear();
    m_saved.clear();
    if (m_infos != 0) {
        for (u32 i = 0; i < m_infoCount; i++) {
            const MessageInfo *info = &m_infos[i];
            if (info->m_kind == 1) {
                m_messages.push_back(boost::shared_ptr<CountMessage>(new CountMessage(info, m_target)));
            }
        }
        std::sort(m_messages.begin(), m_messages.end(), &CompareCount);
        UpdateNext();
    }
}

void CountMessageManage::UpdateNext() {
    if (m_messages.empty()) {
        m_next = -1;
    } else {
        m_next = m_messages.back()->m_count;
    }
}

} // namespace Mission

#include <vector_impl>

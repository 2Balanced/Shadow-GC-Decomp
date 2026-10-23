#ifndef GAME_TIMERMESSAGE_H
#define GAME_TIMERMESSAGE_H

#include <vector>
#include "Game/MissionMessage.h"
#include "boost/shared_ptr.hpp"

/* Mission::TimerMessage and Mission::TimerMessageManage (class names from RTTI; code in
 * Game/TimerMessage.cpp): the hints a timed mission shows as its time runs down.
 * Mission::TimerMission (Game/TimerMission.h) holds a manager. Member names are inferred. */
namespace Mission {

class TimerMessage : public Message {
  public:
    /* Trigger 0 shows it m_ratio seconds after the start, 1 at that fraction of the time, 2 with
     * m_ratio seconds left. */
    TimerMessage(const MessageInfo *info, float time) : Message(info) {
        switch (info->m_trigger) {
        case 0:
            m_time = time - m_info->m_ratio;
            break;
        case 1:
            m_time = time * m_info->m_ratio;
            break;
        case 2:
            m_time = m_info->m_ratio;
            break;
        default:
            m_time = -1.0f;
            break;
        }
    }
    virtual ~TimerMessage();

    /* 0x8 */ float m_time; /* the time left that shows it */
};
SIZE_ASSERT(TimerMessage, 0xC);

class TimerMessageManage : public MessageManage {
  public:
    TimerMessageManage(int id, int player, float time);
    ~TimerMessageManage();
    virtual void Save();
    virtual void Restore();
    virtual void Reset();

    void Update(float time);
    void UpdateNext();

    /* 0x10 */ float m_time;
    /* 0x14 */ float m_next; /* the time left that shows the next message; -1: none left */
    /* 0x18 */ std::vector<boost::shared_ptr<TimerMessage> > m_messages;
    /* 0x24 */ std::vector<boost::shared_ptr<TimerMessage> > m_saved;
};
SIZE_ASSERT(TimerMessageManage, 0x30);

} // namespace Mission

#endif

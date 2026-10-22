/* Mission::BasicMessage and Mission::BasicMessageManage (class names from RTTI): the hints a
 * mission shows when it is cleared or failed. The manager keeps a message for each of the
 * mission's hints of kind 0 (see Game/MissionMessage.h) and, when the mission's state changes,
 * shows and drops the ones whose trigger matches. Mission::Mission owns one (Game/Mission.cpp).
 * File name from the class; all other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_ERASE
#define MSL_VECTOR_DESTROY_ALL_LATE
#define MSL_VECTOR_DESTROY_ALL_FIRST
#define MSL_ALGORITHM_COPY
#define MSL_ALGORITHM_REMOVE_IF
#include <vector>
#include <boost/bind.hpp>
#include "Game/MissionMessage.h"
#include "boost/shared_ptr.hpp"

namespace Mission {
class MissionManager;
}

/* The mission manager (see Game/Mission.cpp): the current player at 0x20. */
struct UnkMissionManager_8006D6A8 {
    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ int m_currentPlayer;
};

extern "C" UnkMissionManager_8006D6A8 *fn_8006D6A8(void);

namespace Mission {

/* A mission's state as the manager is told it (Mission::Mission keeps it as an int). The type is
 * inferred: retail binds the player and the state as two different types (two value<T> instances
 * of list1::operator[]). */
enum MissionState {
    kMissionRunning,
    kMissionCleared,
    kMissionFailed,
};

class BasicMessage : public Message {
  public:
    BasicMessage(const MessageInfo *info) : Message(info) {}
    virtual ~BasicMessage();
};
SIZE_ASSERT(BasicMessage, 0x8);

class BasicMessageManage : public MessageManage {
  public:
    BasicMessageManage(int id, int player);
    ~BasicMessageManage();
    virtual void Save();
    virtual void Restore();
    virtual void Reset();

    void Show(MissionState state);

    /* 0x10 */ std::vector<boost::shared_ptr<BasicMessage> > m_messages;
    /* 0x1C */ std::vector<boost::shared_ptr<BasicMessage> > m_saved;
};
SIZE_ASSERT(BasicMessageManage, 0x28);

BasicMessage::~BasicMessage() {}

/* Shows a message whose trigger is the new state (1 cleared, 2 failed; a hint for one player only
 * when that player is the current one); returns whether it was shown, to drop it. */
static bool ShowOnState(boost::shared_ptr<BasicMessage> message, int player, MissionState state) {
    const MessageInfo *info = message->m_info;
    if (state == kMissionCleared && info->m_trigger == 0) {
        message->Show(player);
        return true;
    }
    if (state == kMissionFailed && info->m_trigger == 1) {
        if (info->m_player == -1) {
            message->Show(player);
            return true;
        }
        if (info->m_player == fn_8006D6A8()->m_currentPlayer) {
            message->Show(player);
        }
        return true;
    }
    return false;
}

BasicMessageManage::BasicMessageManage(int id, int player) : MessageManage(id, player) {
    m_messages.reserve(4);
    m_saved.reserve(4);
    Reset();
}

BasicMessageManage::~BasicMessageManage() {}

void BasicMessageManage::Show(MissionState state) {
    std::vector<boost::shared_ptr<BasicMessage> >::iterator last = m_messages.end();
    m_messages.erase(std::remove_if(m_messages.begin(), m_messages.end(), boost::bind(&ShowOnState, _1, m_player, state)),
                     last);
}

void BasicMessageManage::Save() {
    m_saved.clear();
    for (std::vector<boost::shared_ptr<BasicMessage> >::iterator it = m_messages.begin(); it != m_messages.end(); ++it) {
        m_saved.push_back(boost::shared_ptr<BasicMessage>(*it));
    }
}

void BasicMessageManage::Restore() {
    m_messages.clear();
    for (std::vector<boost::shared_ptr<BasicMessage> >::iterator it = m_saved.begin(); it != m_saved.end(); ++it) {
        m_messages.push_back(boost::shared_ptr<BasicMessage>(*it));
    }
}

void BasicMessageManage::Reset() {
    m_messages.clear();
    m_saved.clear();
    if (m_infos != 0) {
        for (u32 i = 0; i < m_infoCount; i++) {
            const MessageInfo *info = &m_infos[i];
            if (info->m_kind == 0) {
                m_messages.push_back(boost::shared_ptr<BasicMessage>(new BasicMessage(info)));
            }
        }
    }
}

} // namespace Mission

#include <vector_impl>

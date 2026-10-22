#ifndef GAME_MISSIONMESSAGE_H
#define GAME_MISSIONMESSAGE_H

#include <dolphin/types.h>
#include "size_assert.h"

/* The hint messages a mission shows while it runs (class names from RTTI; the code is at
 * 0x802EB9E0..0x802F0730: Game/BasicMessage.cpp, Game/CountMessage.cpp, Game/MissionMessage.cpp,
 * Game/TimerMessage.cpp). Each mission number has a list of hints in a table (two tables, one for
 * game mode 5); a message manager keeps one Message per hint of its kind and shows it when the
 * mission reaches its trigger. Member names are inferred from what the functions do. */
namespace Mission {

/* One hint of a mission (0x18 bytes in the tables). */
struct MessageInfo {
    /* 0x00 */ int m_hintId;  /* the hint the hint manager plays */
    /* 0x04 */ int m_kind;    /* 0 BasicMessage, 1 CountMessage, 2 TimerMessage */
    /* 0x08 */ int m_trigger; /* BasicMessage: the mission state that shows it */
    /* 0x0C */ int m_count;
    /* 0x10 */ float m_ratio;
    /* 0x14 */ s32 m_player; /* -1: any player. s32 (long), not int: the managers compare it with
                              * an int and pass it on as one, and the game loads it twice. */
};
SIZE_ASSERT(MessageInfo, 0x18);

/* A mission number's hints. */
struct MessageTableEntry {
    /* 0x0 */ const MessageInfo *m_infos;
    /* 0x4 */ int m_count;
};
SIZE_ASSERT(MessageTableEntry, 0x8);

/* One hint on screen. */
class Message {
  public:
    Message(const MessageInfo *info);
    virtual ~Message();
    virtual void Show(int player);

    /* 0x4 */ const MessageInfo *m_info;
};
SIZE_ASSERT(Message, 0x8);

/* Keeps a mission's messages (the subclasses hold them). */
class MessageManage {
  public:
    MessageManage(int id, int player);
    ~MessageManage() {}

    virtual void Save() = 0;
    virtual void Restore() = 0;
    virtual void Reset() = 0;

    /* 0x4 */ const MessageInfo *m_infos;
    /* 0x8 */ int m_infoCount;
    /* 0xC */ int m_player;
};
SIZE_ASSERT(MessageManage, 0x10);

} // namespace Mission

#endif

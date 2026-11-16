#ifndef GAME_MISSIONMANAGER_H
#define GAME_MISSIONMANAGER_H

/* What the four units of the stage missions share (Game/CountMission.cpp, Game/Mission.cpp,
 * Game/MissionTable.cpp and Game/MissionManagerTask.cpp): the reduced layouts of the mission
 * subclasses that live in other units, the table of mission kinds, the manager of the players'
 * missions and its task (Mission::MissionManagerTask, class name from RTTI), and the functions
 * and globals of other units they use. All names but the RTTI ones are inferred. */

/* See Game/Mission.h: the missions' message managers as members. */
#define MISSION_MESSAGE_MEMBERS
#include "Game/Mission.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"
#include <map>

namespace Mission {

/* The timer missions (code at 0x802C8AF4 and 0x802EB7F0; layouts reduced to their sizes). */
class TimerMission : public virtual Mission {
  public:
    TimerMission(int id, int player, float time);
    virtual ~TimerMission();

    u8 field_0xC[0x40 - 0xC];
};
SIZE_ASSERT(TimerMission, 0x80);

class TimerGoalMission : public TimerMission {
  public:
    TimerGoalMission(int id, int player, float time);
    virtual ~TimerGoalMission();
};
SIZE_ASSERT(TimerGoalMission, 0x80);

class TimerCountMission : public TimerMission, public CountMission {
  public:
    TimerCountMission(int id, int player, float time, int target);
    virtual ~TimerCountMission();
};
SIZE_ASSERT(TimerCountMission, 0xD8);

class GoalMission : public Mission {
  public:
    GoalMission(int id, int player);
    virtual ~GoalMission();
};
SIZE_ASSERT(GoalMission, 0x40);

class EnemyMission : public CountMission {
  public:
    EnemyMission(int id, int player, int kind, int target, int count);
    virtual ~EnemyMission();

    /* 0x58 */ int m_kind;
};
SIZE_ASSERT(EnemyMission, 0xA0);

class RingMission : public CountMission {
  public:
    RingMission(int id, int player, int target, int count);
    virtual ~RingMission();
};
SIZE_ASSERT(RingMission, 0x9C);

/* What a mission number stands for: the kind of mission (the factory's switch) and its
 * parameters. */
struct MissionInfo {
    MissionInfo(int type, int param, int target, int subTarget)
        : m_type(type), m_param(param), m_target(target), m_subTarget(subTarget) {}

    /* 0x0 */ int m_type; /* 0 Mission, 1 Goal, 2 Count, 3 Enemy, 4 Timer, 5 TimerGoal, 6 TimerCount,
                            7 Ring */
    /* 0x4 */ int m_param; /* the enemy kind */
    /* 0x8 */ int m_target;
    /* 0xC */ int m_subTarget; /* a TimerCountMission's time */
};
SIZE_ASSERT(MissionInfo, 0x10);

/* The mission kinds by mission number. */
class MissionTable {
  public:
    MissionTable();
    ~MissionTable();

    void Register(int id, int type, int param);
    const MissionInfo *Find(int id) const;
    void Set(int id, const MissionInfo &info);
    boost::shared_ptr<Mission> Create(int id, int player);

    /* 0x0 */ std::map<int, MissionInfo> m_infos;
};
SIZE_ASSERT(MissionTable, 0x10);

class MissionManagerTask : public Task {
  public:
    MissionManagerTask(Task *parent) : Task(parent) {
        m_name = s_taskName;
    }
    virtual ~MissionManagerTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(MissionManagerTask, 0x28);

/* The missions of the three players (by player index) and the timer they share. */
class MissionManager {
  public:
    MissionManager();
    ~MissionManager();

    void Init(Task *parent, int stage);
    void MarkStartMission();
    void ResetAll();
    void RestoreAll();
    void SetPlayer(int player);
    u8 IsFlagSet(int player);
    void CheckAllFailed();
    void Update(float dt);
    void Release();

    /* 0x00 */ MissionManagerTask *m_task;
    /* 0x04 */ int m_stage;
    /* 0x08 */ boost::shared_ptr<Mission> m_missions[3];
    /* 0x20 */ int m_currentPlayer;
    /* 0x24 */ int m_savedPlayer;
    /* 0x28 */ bool m_hasTimer;
    /* 0x29 */ bool m_allTimed;
    /* 0x2C */ float m_timeLimit;
    /* 0x30 */ MissionTable m_table;
};
SIZE_ASSERT(MissionManager, 0x40);

/* The message manager a mission shows itself with (Mission::BasicMessageManage and
 * Mission::CountMessageManage, at 0x802EB9E0 and 0x802ED02C; only the virtual functions used
 * here). */
class MessageManage {
  public:
    virtual void Save();
    virtual void Restore();
    virtual void Reset();
};

template <class T>
inline MessageManage &MessageOf(T &message) {
    return *reinterpret_cast<MessageManage *>(&message);
}

} // namespace Mission

/* What fn_80074470 returns (class unknown): the game mode at 0x8. */
struct UnkObject_80074470 {
    /* 0x0 */ u8 field_0x0[0x8];
    /* 0x8 */ s32 m_mode;
};

/* The game state (struct name unknown; see Game/TFogSet.cpp). */
struct UnkGameState_8057D6A0 {
    /* 0x000 */ u8 field_0x0[0x58];
    /* 0x058 */ s32 m_language; /* indexes lbl_8051D528 (meaning inferred) */
    /* 0x05C */ u8 field_0x5C[0x88 - 0x5C];
    /* 0x088 */ u8 m_timer[0xB1 - 0x88]; /* the stage timer */
    /* 0x0B1 */ u8 field_0xB1;
    /* 0x0B2 */ u8 field_0xB2[0x125 - 0xB2];
    /* 0x125 */ u8 field_0x125;
    /* 0x126 */ u8 field_0x126[0x14A - 0x126];
    /* 0x14A */ u8 m_timerShown;
    /* 0x14B */ u8 field_0x14B[0x150 - 0x14B];
};
SIZE_ASSERT(UnkGameState_8057D6A0, 0x150);

extern UnkGameState_8057D6A0 lbl_8057D6A0;

/* The stages' table (0x50 bytes per stage; the mission number of a stage played alone at 0x40). */
struct UnkStageInfo_804C4A18 {
    /* 0x00 */ u8 field_0x0[0x40];
    /* 0x40 */ int m_missionId;
    /* 0x44 */ u8 field_0x44[0x50 - 0x44];
};
SIZE_ASSERT(UnkStageInfo_804C4A18, 0x50);

extern UnkStageInfo_804C4A18 lbl_804C4A18[];
extern const char *const lbl_8051D528[];

class UnkObject_800B74B4;
class UnkCharManager_8006F754;

extern "C" {
Mission::MissionManager *fn_8006D6A8(void);
UnkObject_80074470 *fn_80074470(void);
void fn_80175EEC(UnkObject_80074470 *object, int arg1, int arg2);
void fn_8035367C(const char *file, const char *layout, const char *animation);
void *fn_80053164(void);
void fn_801D560C(void *sound, int id, int arg2, int arg3);
UnkObject_800B74B4 *fn_800B74B4(void);
int fn_80176BE4(UnkObject_800B74B4 *object, int stage, int player);
const float *fn_80070494(const float *a, const float *b);
UnkCharManager_8006F754 *fn_8006F754(void);
void fn_800749FC(UnkCharManager_8006F754 *manager, int player);
/* The stage timer (behaviour inferred). */
void fn_80336F98(u8 *timer);
void fn_80336FB4(u8 *timer);
void fn_80336FC0(u8 *timer, float time);
void fn_80336FD0(u8 *timer);
void fn_80336FE4(u8 *timer);

void fn_8016B270(Mission::Mission *mission);
void fn_8016B2A4(Mission::Mission *mission);
bool fn_8016DA34(Mission::MissionManager *manager);
void fn_8016DFF8(Mission::MissionManager *manager);
}

namespace boost {

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line (boost
 * 1.32's definition, as in Game/GadgetUtility.cpp). */
template<class T> inline void shared_ptr<T>::reset() {
    this_type().swap(*this);
}

} // namespace boost

#endif

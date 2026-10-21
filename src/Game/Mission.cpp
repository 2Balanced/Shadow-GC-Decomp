/* A stage's missions: Mission::Mission and Mission::CountMission (class names from RTTI), the
 * table of mission kinds by mission number, the mission factory, and the manager that keeps each
 * player's mission and its task, Mission::MissionManagerTask (RTTI). The subclasses live in their
 * own units (Game/GoalMission.cpp, Game/EnemyMission.cpp, Game/RingMission.cpp; the timer
 * missions at 0x802C8AF4 and 0x802EB7F0). File name from the namespace; all other names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first).
 *
 * The manager is the object fn_8006D6A8 returns, which other units call UnkPlayerManager_8006D6A8.
 * Functions other units call under their addresses (fn_8016B270, fn_8016B2A4, fn_8016DA34,
 * fn_8016DFF8) keep C names here.
 *
 * The unit stays NonMatching: OnFailed differs in register allocation, and retail's .sdata has a
 * second RTTI record named "Mission::Mission" (at 0x805E6740, the one Mission's vtable points to)
 * and a zero word before s_taskName that GC/2.7 does not produce. The eight stage-event callbacks
 * at 0x8016A9E0..0x8016AA90 before this unit are not part of it here: only the stage table at
 * 0x804C4A18 (another unit's .rodata) refers to them, so their owner is not clear. */
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_TREE_BOOL_OBJECTS
#define MSL_TREE_ITERATOR_ASSIGN_CALL
#define MSL_VECTOR_MIDDLE
#include "Game/Mission.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"
#include <map>
#include <vector>
#include <stdio.h>
#include <string.h>

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

inline MessageManage &MessageOf(u8 *message) {
    return *reinterpret_cast<MessageManage *>(message);
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
/* The message managers (Mission::BasicMessageManage, Mission::CountMessageManage). */
void fn_802EBE68(u8 *message, int state);
void fn_802EBF14(u8 *message, int flag);
void fn_802EBFE0(u8 *message);
void fn_802ED4E0(u8 *message, int count);
void fn_802ED644(u8 *message, int flag);
void fn_802ED710(u8 *message, int id, int player, int target);

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

/* Not called anywhere: the linker strips them from the game. Retail's .sdata holds the RTTI
 * records of the missions and of the shared-pointer control blocks, then insert_unique's result
 * flags, then the deleters' records, then the task's name and records; GC/2.7 creates the records
 * after the source is read, so these typeids create them in retail's order (see
 * Game/TrigerCounter.cpp). */
const std::type_info &MissionTypes(int i) {
    if (i == 0) {
        return typeid(Mission::Mission);
    }
    if (i == 1) {
        return typeid(Mission::CountMission);
    }
    if (i == 2) {
        return typeid(boost::detail::sp_counted_base);
    }
    if (i == 3) {
        return typeid(boost::detail::sp_counted_base_impl<Mission::Mission *, boost::checked_deleter<Mission::Mission> >);
    }
    if (i == 4) {
        return typeid(boost::detail::sp_counted_base_impl<Mission::GoalMission *, boost::checked_deleter<Mission::GoalMission> >);
    }
    if (i == 5) {
        return typeid(boost::detail::sp_counted_base_impl<Mission::CountMission *, boost::checked_deleter<Mission::CountMission> >);
    }
    if (i == 6) {
        return typeid(boost::detail::sp_counted_base_impl<Mission::EnemyMission *, boost::checked_deleter<Mission::EnemyMission> >);
    }
    if (i == 7) {
        return typeid(boost::detail::sp_counted_base_impl<Mission::TimerMission *, boost::checked_deleter<Mission::TimerMission> >);
    }
    if (i == 8) {
        return typeid(boost::detail::sp_counted_base_impl<Mission::TimerGoalMission *, boost::checked_deleter<Mission::TimerGoalMission> >);
    }
    if (i == 9) {
        return typeid(boost::detail::sp_counted_base_impl<Mission::TimerCountMission *, boost::checked_deleter<Mission::TimerCountMission> >);
    }
    if (i == 10) {
        return typeid(boost::detail::sp_counted_base_impl<Mission::RingMission *, boost::checked_deleter<Mission::RingMission> >);
    }
    return typeid(Mission::Mission);
}

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

MissionTable::MissionTable() {
    m_infos.clear();
    Register(0, 1, 0);
    Register(1, 3, 0);
    Register(2, 3, 2);
    Register(3, 2, 0);
    Register(4, 2, 0);
    Register(5, 3, 2);
    Register(6, 2, 0);
    Register(7, 2, 0);
    Register(8, 2, 0);
    Register(9, 3, 0);
    Register(10, 2, 0);
    Register(11, 3, 0);
    Register(12, 7, 0);
    Register(13, 6, 0);
    Register(14, 6, 0);
    Register(15, 3, 0);
    Register(16, 2, 0);
    Register(17, 2, 0);
    Register(18, 2, 0);
    Register(19, 2, 0);
    Register(20, 2, 0);
    Register(21, 3, 2);
    Register(22, 2, 0);
    Register(23, 2, 0);
    Register(24, 3, 2);
    Register(25, 3, 0);
    Register(26, 2, 0);
    Register(27, 2, 0);
    Register(28, 5, 0);
    Register(29, 3, 2);
    Register(30, 2, 0);
    Register(31, 3, 0);
    Register(32, 2, 0);
    Register(33, 5, 0);
    Register(34, 4, 0);
    Register(35, 2, 0);
    Register(36, 5, 0);
    Register(37, 2, 0);
    Register(38, 2, 0);
    Register(39, 2, 0);
    Register(40, 4, 0);
}

void MissionTable::Register(int id, int type, int param) {
    std::pair<std::map<int, MissionInfo>::iterator, bool> result;
    result = m_infos.insert(std::pair<int, MissionInfo>(id, MissionInfo(type, param, 1, 0)));
}

const MissionInfo *MissionTable::Find(int id) const {
    std::map<int, MissionInfo>::const_iterator it = m_infos.find(id);
    if (it == m_infos.end()) {
        return 0;
    }
    return &(*it).second;
}

void MissionTable::Set(int id, const MissionInfo &info) {
    std::map<int, MissionInfo>::iterator it = m_infos.find(id);
    if (it != m_infos.end()) {
        it->second = info;
    }
}

boost::shared_ptr<Mission> MissionTable::Create(int id, int player) {
    boost::shared_ptr<Mission> mission;
    const MissionInfo *info = Find(id);
    if (info != 0) {
        switch (info->m_type) {
        case 0:
            mission = boost::shared_ptr<Mission>(new Mission(id, player));
            break;
        case 1:
            mission = boost::shared_ptr<Mission>(new GoalMission(id, player));
            break;
        case 2:
            mission = boost::shared_ptr<Mission>(new CountMission(id, player, info->m_target, info->m_subTarget, 0, 0));
            break;
        case 3:
            mission = boost::shared_ptr<Mission>(new EnemyMission(id, player, info->m_param, info->m_target, 0));
            break;
        case 4:
            mission = boost::shared_ptr<Mission>(new TimerMission(id, player, info->m_target));
            break;
        case 5:
            mission = boost::shared_ptr<Mission>(new TimerGoalMission(id, player, info->m_target));
            break;
        case 6:
            mission = boost::shared_ptr<Mission>(
                new TimerCountMission(id, player, info->m_subTarget, info->m_target));
            break;
        case 7:
            mission = boost::shared_ptr<Mission>(new RingMission(id, player, info->m_target, 0));
            break;
        }
    }
    return mission;
}

Mission::Mission(int id, int player)
    : m_id(id), m_player(player), m_state(0), m_savedState(0), m_flag(0), m_savedFlag(0) {
    fn_802EBFE0(m_message);
}

Mission::~Mission() {
    fn_802EBF14(m_message, -1);
}

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
        fn_802EBE68(m_message, 1);
        fn_80175EEC(fn_80074470(), 6, 0);
    }
}

/* NonMatching (93.6%): retail gives the game state's address and the suffix table other
 * registers around the sprintf; about ten source shapes were tried. */
void Mission::OnFailed() {
    if (fn_80074470()->m_mode != 9) {
        MissionManager *manager = fn_8006D6A8();
        if (m_player == manager->m_currentPlayer) {
            manager->SetPlayer(1);
        }
        m_flag = 0;
        fn_802EBE68(m_message, 2);
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

CountMission::CountMission(int id, int player, int target, int subTarget, int count, int subCount)
    : Mission(id, player), m_target(target), m_subTarget(subTarget), m_initCount(count),
      m_initSubCount(subCount) {
    fn_802ED710(m_message, id, player, target);
    OnReset();
}

CountMission::~CountMission() {
    fn_802ED644(m_message, -1);
}

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
            fn_802ED4E0(m_message, m_count);
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
            fn_802ED4E0(m_message, m_count);
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

#include <map_impl>
#include <vector_impl>

/* Not called anywhere (see MissionTypes). */
const std::type_info &DeleterTypes(int i) {
    if (i == 0) {
        return typeid(boost::checked_deleter<Mission::RingMission>);
    }
    if (i == 1) {
        return typeid(boost::checked_deleter<Mission::TimerCountMission>);
    }
    if (i == 2) {
        return typeid(boost::checked_deleter<Mission::TimerGoalMission>);
    }
    if (i == 3) {
        return typeid(boost::checked_deleter<Mission::TimerMission>);
    }
    if (i == 4) {
        return typeid(boost::checked_deleter<Mission::EnemyMission>);
    }
    if (i == 5) {
        return typeid(boost::checked_deleter<Mission::CountMission>);
    }
    if (i == 6) {
        return typeid(boost::checked_deleter<Mission::GoalMission>);
    }
    if (i == 7) {
        return typeid(boost::checked_deleter<Mission::Mission>);
    }
    return typeid(boost::checked_deleter<Mission::Mission>);
}

const char *Mission::MissionManagerTask::s_taskName = "MissionManagerTask";

/* Not called anywhere (see MissionTypes). */
const std::type_info &TaskTypes(int i) {
    if (i == 0) {
        return typeid(Task);
    }
    return typeid(Mission::MissionManagerTask);
}

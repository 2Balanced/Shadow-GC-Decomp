/* Mission::MissionTable (name inferred): the kind of mission each mission number stands for, and
 * the factory that creates a player's mission from it. Part of the stage missions (see
 * Game/Mission.cpp; split out of it in wave 39, see Game/CountMission.cpp). Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * As a unit of its own it needs none of the typeid helpers the combined unit had: GC/2.7 writes
 * the control blocks' RTTI records, insert_unique's two flags and the deleters' records to .sdata
 * in retail's order, and each control block's base list ahead of its vtable behind Create's jump
 * table in .data. The flags are the plain bool temporaries of <map_impl> (no
 * MSL_TREE_BOOL_OBJECTS), and the table's destructor is defined out of line behind the
 * constructor, where retail has it. */
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_TREE_ITERATOR_ASSIGN_CALL
/* The unit keeps its own copy of the tree end node's constructor (0x8016D660): see <map>. */
#define MSL_TREE_END_NODE_NAME __tree_end_node_MissionTable
#include "Game/MissionManager.h"

namespace Mission {

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

MissionTable::~MissionTable() {}

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

} // namespace Mission

#include <map_impl>

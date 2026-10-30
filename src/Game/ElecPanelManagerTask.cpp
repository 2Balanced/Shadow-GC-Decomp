/* ElecPanelManagerTask (class name from RTTI) and the ELEC_PANEL gadgets' tables (class name
 * unknown; Game/ElecPanel.h): every frame the task goes through the circuits whose panels changed
 * colour and turns a circuit's switch on once all its panels show the same colour. The tables keep
 * the owners of the stage's ELEC_PANEL records by circuit id (a multimap), the live panels by owner
 * (a map) and the changed circuits (a set). File name from the task's class. Method names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). The maps and the set are the stand-ins of <map> and <set>. */
#define MSL_MULTIMAP_TREE
#define MSL_MULTIMAP_MEMBERS
#define MSL_MAKE_PAIR_BY_VALUE
#define MSL_PAIR_CONST_FIRST
#define MSL_SET_CONST_ITERATOR
#define MSL_TREE_END_NODE_PER_TREE
#define MSL_TREE_ITERATOR_ASSIGN_CALL

#include "Game/ElecPanel.h"
#include "Game/Gadget.h"
#include "Game/GadgetUtil.h"
#include "Game/Task.h"
#include "Game/UnkSwitchBank_800CB69C.h"
#include "size_assert.h"
#include <map>
#include <set>

extern "C" {
/* The stage's placement records: the manager, the first record of a layer, and a record's gadget
 * type (behaviour inferred; see Game/CharCommandManagerTask.cpp). */
void *fn_800C9B84(void);
UnkPlacement_800C9CD0 *fn_800CB560(void *manager, int layer);
u16 fn_800C9D24(UnkPlacement_800C9CD0 *owner);
}

/* The next record of a layer (layout inferred). */
inline UnkPlacement_800C9CD0 *NextOwner(UnkPlacement_800C9CD0 *owner) {
    return *reinterpret_cast<UnkPlacement_800C9CD0 **>(reinterpret_cast<u8 *>(owner) + 0x8);
}

UnkElecPanelTables_801E6114::UnkElecPanelTables_801E6114() {
    m_task = 0;
}

void UnkElecPanelTables_801E6114::Load() {
    for (int layer = 1; layer < 0x100; layer++) {
        for (UnkPlacement_800C9CD0 *owner = fn_800CB560(fn_800C9B84(), layer); owner != 0;
             owner = NextOwner(owner)) {
            if (fn_800C9D24(owner) == 0x7D2) {
                m_owners.insert(std::make_pair((unsigned char)layer, owner));
            }
        }
    }
}

void UnkElecPanelTables_801E6114::AddPanel(ElecPanel *panel) {
    m_panels.insert(std::make_pair(panel->m_owner, panel));
}

void UnkElecPanelTables_801E6114::RemovePanel(ElecPanel *panel) {
    std::map<UnkPlacement_800C9CD0 *, ElecPanel *>::iterator it;
    if ((it = m_panels.find(panel->m_owner)) != m_panels.end()) {
        m_panels.erase(it);
    }
}

/* Not called anywhere: the linker strips it from the game. Retail's unit has the instances it
 * makes (make_pair and the pair conversion of the owners' values) between RemoveOwner and
 * RemovePanel (name inferred; see AddOwner in Game/UnkGadgetRegistry_80260E48.cpp). */
void UnkElecPanelTables_801E6114::AddOwner(UnkPlacement_800C9CD0 *owner) {
    u8 id;
    fn_800C9E60(owner, &id);
    m_owners.insert(std::make_pair(id, owner));
}

void UnkElecPanelTables_801E6114::RemoveOwner(UnkPlacement_800C9CD0 *owner) {
    std::multimap<unsigned char, UnkPlacement_800C9CD0 *>::iterator it;
    for (it = m_owners.begin(); it != m_owners.end(); it++) {
        if ((*it).second == owner) {
            m_owners.erase(it);
            return;
        }
    }
}

void UnkElecPanelTables_801E6114::SetChanged(u8 id) {
    m_changed.insert(id);
}

void UnkElecPanelTables_801E6114::Clear() {
    m_owners.clear();
    m_panels.clear();
    m_changed.clear();
}

typedef std::map<UnkPlacement_800C9CD0 *, ElecPanel *> PanelMap;

void UnkElecPanelTables_801E6114::Update() {
    std::set<int>::const_iterator it;
    for (it = m_changed.begin(); it != m_changed.end(); it++) {
        int color = -1;
        std::pair<std::multimap<unsigned char, UnkPlacement_800C9CD0 *>::iterator,
                  std::multimap<unsigned char, UnkPlacement_800C9CD0 *>::iterator>
            range = m_owners.equal_range(*it);
        std::multimap<unsigned char, UnkPlacement_800C9CD0 *>::const_iterator owner;
        for (owner = range.first; owner != range.second; owner++) {
            PanelMap::iterator panel = m_panels.find(owner->second);
            if (panel != PanelMap::const_iterator(m_panels.end())) {
                if (color == -1) {
                    color = (*panel).second->m_color;
                } else if (color != (*panel).second->m_color) {
                    break;
                }
            }
        }
        if (owner == range.second) {
            fn_801690D0(fn_800CB69C(), *it, 1);
            fn_801D560C(fn_80053164(), 0x7236, 0, 0);
        }
    }
    m_changed.clear();
}

ElecPanelManagerTask::ElecPanelManagerTask(Task *parent) : Task(parent) {}

ElecPanelManagerTask::~ElecPanelManagerTask() {
    fn_801E6114()->m_task = 0;
}

void ElecPanelManagerTask::Tick(float dt) {
    fn_801E6114()->Update();
}

#include <map_impl>
#include <set_impl>

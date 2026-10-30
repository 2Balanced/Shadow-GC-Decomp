/* The registry of a gadget type's placement records (class name unknown; see
 * Game/UnkGadgetRegistry_80260E48.h): the ELEC_SECURITY gadgets' (Game/ElecSecurityCodeGenerator.cpp).
 * No RTTI names this unit; the file is named after the class. Its multimap's members are the weak
 * copies Game/ElecPanelManagerTask.cpp emits; only make_pair and the multimap's size are this
 * unit's own. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define MSL_MULTIMAP_TREE
#define MSL_MULTIMAP_MEMBERS
#define MSL_MAKE_PAIR_BY_VALUE
#define MSL_PAIR_CONST_FIRST
#define MSL_TREE_END_NODE_PER_TREE
#define MSL_TREE_ITERATOR_ASSIGN_CALL

#include "Game/Gadget.h"
#include "Game/UnkGadgetRegistry_80260E48.h"
#include "size_assert.h"
#include <map>

extern "C" {
/* The stage's placement records: the manager, the first record of a layer, and a record's gadget
 * type (see Game/ElecPanelManagerTask.cpp). */
void *fn_800C9B84(void);
UnkPlacement_800C9CD0 *fn_800CB560(void *manager, int layer);
u16 fn_800C9D24(UnkPlacement_800C9CD0 *owner);
}

/* The next record of a layer (layout inferred). */
inline UnkPlacement_800C9CD0 *NextOwner(UnkPlacement_800C9CD0 *owner) {
    return *reinterpret_cast<UnkPlacement_800C9CD0 **>(reinterpret_cast<u8 *>(owner) + 0x8);
}

UnkGadgetRegistry_80260E48::UnkGadgetRegistry_80260E48(u16 typeId) {
    Load(typeId);
}

UnkGadgetRegistry_80260E48::~UnkGadgetRegistry_80260E48() {
    m_owners.clear();
}

void UnkGadgetRegistry_80260E48::Load(u16 typeId) {
    for (int layer = 1; layer < 0x100; layer++) {
        for (UnkPlacement_800C9CD0 *owner = fn_800CB560(fn_800C9B84(), layer); owner != 0;
             owner = NextOwner(owner)) {
            if (fn_800C9D24(owner) == typeId) {
                m_owners.insert(std::make_pair((unsigned char)layer, owner));
            }
        }
    }
}

extern "C" void fn_80260CE8(UnkGadgetRegistry_80260E48 *registry, UnkPlacement_800C9CD0 *owner) {
    u8 id;
    fn_800C9E60(owner, &id);
    registry->m_owners.insert(std::make_pair(id, owner));
}

extern "C" bool fn_80260C18(UnkGadgetRegistry_80260E48 *registry, UnkPlacement_800C9CD0 *owner) {
    std::multimap<unsigned char, UnkPlacement_800C9CD0 *>::iterator it;
    for (it = registry->m_owners.begin(); it != registry->m_owners.end(); it++) {
        if ((*it).second == owner) {
            registry->m_owners.erase(it);
            if (registry->m_owners.size() == 0) {
                return true;
            }
            break;
        }
    }
    return false;
}

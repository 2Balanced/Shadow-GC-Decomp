#ifndef GAME_UNKGADGETREGISTRY_80260E48_H
#define GAME_UNKGADGETREGISTRY_80260E48_H

#include <dolphin/types.h>
#include "size_assert.h"
#include <map>

/* A registry of the stage's placement records of one gadget type (class name unknown;
 * Game/UnkGadgetRegistry_80260E48.cpp): its constructor collects the owners whose gadget type is
 * typeId, keyed by their circuit id (the layer they were found in). Game/ElecSecurityCodeGenerator.cpp
 * keeps one for the ELEC_SECURITY gadgets. The multimap is the one of the ELEC_PANEL tables
 * (Game/ElecPanel.h). A unit that includes this header defines MSL_MULTIMAP_TREE before it. */
class UnkPlacement_800C9CD0;

class UnkGadgetRegistry_80260E48 {
  public:
    UnkGadgetRegistry_80260E48(u16 typeId);
    ~UnkGadgetRegistry_80260E48();

    /* Collects the owners of gadget type typeId (name inferred). */
    void Load(u16 typeId);

    /* 0x0 */ std::multimap<unsigned char, UnkPlacement_800C9CD0 *> m_owners;
};
SIZE_ASSERT(UnkGadgetRegistry_80260E48, 0x10);

extern "C" {
/* Adds owner to the registry, keyed by its circuit id, and removes it; the latter is true when the
 * registry is then empty (names of the original methods unknown). */
void fn_80260CE8(UnkGadgetRegistry_80260E48 *registry, UnkPlacement_800C9CD0 *owner);
bool fn_80260C18(UnkGadgetRegistry_80260E48 *registry, UnkPlacement_800C9CD0 *owner);
}

#endif

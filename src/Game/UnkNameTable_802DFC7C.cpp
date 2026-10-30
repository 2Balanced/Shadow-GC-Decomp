/* The name table of the event scenes (class and file names invented; no class of the unit has
 * RTTI): the table Game/PjsEventActionScene.cpp gets from fn_801702E4, a vector of 8-byte entries
 * that keep the data the scenes load (released with fn_804107EC when the table is cleared), and the
 * lookup that strips the "EBN" prefix from a name. Game/PjsEventActionManager.cpp clears it. Names
 * marked "(name inferred)" are not original. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first); the vector's members defined out of line come out
 * at the end of the unit. */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_ERASE_ONE
#define MSL_VECTOR_ERASE_ONE_DATA
#define MSL_ALGORITHM_COPY
#include <dolphin/types.h>
#include "size_assert.h"
#include <string.h>
#include <vector>

/* An entry (layout as in Game/PjsEventActionScene.cpp; field_0x4 is the data). */
struct UnkNameTableEntry_802DFC7C {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
};

/* The table (layout as in Game/PjsEventActionScene.cpp). */
class UnkNameTable_802DFC7C {
  public:
    UnkNameTable_802DFC7C();

    /* 0x0 */ std::vector<UnkNameTableEntry_802DFC7C> m_entries;
    /* 0xC */ u32 field_0xC;
};
SIZE_ASSERT(UnkNameTable_802DFC7C, 0x10);

extern "C" {
/* Takes data on, with its release callback (behaviour inferred), and releases it. */
u32 fn_804109C4(void *data, void (*release)(void *));
void fn_804107EC(u32 handle);
void fn_80049B34(void *data);
/* strstr (as in Game/MemoryCardCtrl.cpp). */
char *fn_803AC6C0(const char *s, const char *sub);
}

/* The prefix the lookup strips (name inferred). */
static const char *s_prefix = "EBN";

UnkNameTable_802DFC7C::UnkNameTable_802DFC7C() {
    m_entries.reserve(0x40);
}

/* Does nothing (name inferred; Game/PjsEventActionManager.cpp calls it before it looks for the
 * scenes). */
extern "C" void fn_802DFC78(UnkNameTable_802DFC7C *table) {}

/* Releases every entry's data and empties the table (name inferred). */
extern "C" void fn_802DFBCC(UnkNameTable_802DFC7C *table) {
    UnkNameTableEntry_802DFC7C *it = table->m_entries.begin();
    while (it != table->m_entries.end()) {
        s32 handle = it->field_0x4;
        if (handle != 0) {
            fn_804107EC(handle);
        }
        it = table->m_entries.erase(it);
    }
}

/* Takes data on and keeps it in a new entry (name inferred). */
extern "C" u32 fn_802DFB04(UnkNameTable_802DFC7C *table, void *data) {
    UnkNameTableEntry_802DFC7C entry;
    entry.field_0x4 = fn_804109C4(data, fn_80049B34);
    table->m_entries.push_back(entry);
    return entry.field_0x4;
}

/* name without what comes up to and including the prefix (name inferred). */
extern "C" const char *fn_802DFAB4(UnkNameTable_802DFC7C *table, const char *name) {
    const char *p = fn_803AC6C0(name, s_prefix);
    if (p == 0) {
        return name;
    }
    return p + strlen(s_prefix);
}

/* GadgetBuilder (namespace from the RTTI of GadgetBuilder::CrashInfoCtrl's shared_ptr control
 * block): lookups in the table of named entries that a gadget's resource file holds (models,
 * parts, debris and sounds). Gadgets such as BreakWall, Elevator, Stick and CircusBall call these
 * functions by their address names. File name inferred from the namespace; names other than
 * GadgetBuilder::CrashInfoCtrl are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include <cstring>

#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* A sound entry of the table (layout as in BreakWall, Stick and CircusBall). */
struct UnkSoundEntry_80208888 {
    /* 0x00 */ const char *name;
    /* 0x04 */ s32 index;
    /* 0x08 */ u32 soundId;
    /* 0x0C */ u32 field_0xC;
    /* 0x10 */ Vec3f offset;
};
SIZE_ASSERT(UnkSoundEntry_80208888, 0x1C);

/* A part entry of the table, found by name (only the size is known here). */
struct UnkModelPartEntry_80208900 {
    /* 0x0 */ const char *name;
    /* 0x4 */ u8 field_0x4[0x10 - 0x4];
};
SIZE_ASSERT(UnkModelPartEntry_80208900, 0x10);

/* A debris entry of the table, found by name (see BreakWall.cpp). */
struct UnkDebrisEntry_802086C0 {
    /* 0x00 */ const char *name;
    /* 0x04 */ u8 field_0x4[0x40 - 0x4];
};
SIZE_ASSERT(UnkDebrisEntry_802086C0, 0x40);

/* The record of one model of the table, found by its id (the "model" parameter); it names the
 * model's files, its debris and its sound (names inferred from the users). */
struct UnkModelRecord_802087F4 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ s32 id;
    /* 0x08 */ const char *model;
    /* 0x0C */ const char *collision;
    /* 0x10 */ const char *brokenModel;
    /* 0x14 */ const char *brokenCollision;
    /* 0x18 */ const char *debris;
    /* 0x1C */ const char *sound;
    /* 0x20 */ float scale;
    /* 0x24 */ u8 field_0x24[0x48 - 0x24];
};
SIZE_ASSERT(UnkModelRecord_802087F4, 0x48);

/* A model's table of named entries in its resource file (name as in Elevator). */
class UnkModelTable_80208888 {
  public:
    /* 0x00 */ UnkDebrisEntry_802086C0 *m_debris;
    /* 0x04 */ s32 m_debrisCount;
    /* 0x08 */ UnkModelPartEntry_80208900 *m_parts;
    /* 0x0C */ s32 m_partCount;
    /* 0x10 */ u8 field_0x10[0x28 - 0x10];
    /* 0x28 */ UnkModelRecord_802087F4 *m_records;
    /* 0x2C */ s32 m_recordCount;
    /* 0x30 */ UnkSoundEntry_80208888 *m_sounds;
    /* 0x34 */ s32 m_soundCount;
};

namespace GadgetBuilder {

/* CrashInfoCtrl (class name from RTTI): the record of one model of a table, which gadgets keep
 * through a shared_ptr (fn_80208978 makes one). */
class CrashInfoCtrl {
  public:
    CrashInfoCtrl(UnkModelTable_80208888 *table, int id);
    /* Empty, but user-declared: retail's checked_delete<CrashInfoCtrl> (0x80208B5C) stays out of
     * line and tests the pointer before deleting it, as a class with a destructor makes it. */
    ~CrashInfoCtrl() {}

    /* 0x0 */ UnkModelTable_80208888 *m_table;
    /* 0x4 */ UnkModelRecord_802087F4 *m_record; /* 0 when the table has no such model */
};
SIZE_ASSERT(CrashInfoCtrl, 0x8);

} // namespace GadgetBuilder

using GadgetBuilder::CrashInfoCtrl;

/* Whether the table has a model of id. */
extern "C" bool fn_80208BB8(UnkModelTable_80208888 *table, int id) {
    if (table != 0) {
        UnkModelRecord_802087F4 *records = table->m_records;
        for (int i = 0; i < table->m_recordCount; i++) {
            if (id == records[i].id) {
                return true;
            }
        }
    }
    return false;
}

/* A new record holder for model id of the table. */
extern "C" boost::shared_ptr<CrashInfoCtrl> fn_80208978(UnkModelTable_80208888 *table, int id) {
    boost::shared_ptr<CrashInfoCtrl> entry = boost::shared_ptr<CrashInfoCtrl>(new CrashInfoCtrl(table, id));
    return entry;
}

/* The part entry called name, or 0. */
extern "C" UnkModelPartEntry_80208900 *fn_80208900(UnkModelTable_80208888 *table,
                                                   const char *name) {
    if (table != 0) {
        for (int i = 0; i < table->m_partCount; i++) {
            if (strcmp(table->m_parts[i].name, name) == 0) {
                return &table->m_parts[i];
            }
        }
    }
    return 0;
}

/* The sound entry called name, or 0. */
extern "C" UnkSoundEntry_80208888 *fn_80208888(UnkModelTable_80208888 *table, const char *name) {
    if (table != 0) {
        for (int i = 0; i < table->m_soundCount; i++) {
            if (strcmp(table->m_sounds[i].name, name) == 0) {
                return &table->m_sounds[i];
            }
        }
    }
    return 0;
}

/* The sound entry of index, or 0. */
extern "C" UnkSoundEntry_80208888 *fn_80208848(UnkModelTable_80208888 *table, int index) {
    if (table != 0) {
        UnkSoundEntry_80208888 *sounds = table->m_sounds;
        for (int i = 0; i < table->m_soundCount; i++) {
            if (index == sounds[i].index) {
                return &sounds[i];
            }
        }
    }
    return 0;
}

GadgetBuilder::CrashInfoCtrl::CrashInfoCtrl(UnkModelTable_80208888 *table, int id) {
    m_table = table;
    m_record = 0;
    if (m_table != 0) {
        for (int i = 0; i < m_table->m_recordCount; i++) {
            if (id == m_table->m_records[i].id) {
                m_record = &m_table->m_records[i];
            }
        }
    }
}

/* The names a model record keeps, or 0 without a record. */
extern "C" const char *fn_802087D8(CrashInfoCtrl *entry) {
    if (entry->m_record != 0) {
        return entry->m_record->model;
    }
    return 0;
}

extern "C" const char *fn_802087BC(CrashInfoCtrl *entry) {
    if (entry->m_record != 0) {
        return entry->m_record->collision;
    }
    return 0;
}

extern "C" const char *fn_802087A0(CrashInfoCtrl *entry) {
    if (entry->m_record != 0) {
        return entry->m_record->brokenModel;
    }
    return 0;
}

extern "C" const char *fn_80208784(CrashInfoCtrl *entry) {
    if (entry->m_record != 0) {
        return entry->m_record->brokenCollision;
    }
    return 0;
}

extern "C" const char *fn_80208768(CrashInfoCtrl *entry) {
    if (entry->m_record != 0) {
        return entry->m_record->sound;
    }
    return 0;
}

/* The debris entry the model record names, or 0. */
extern "C" UnkDebrisEntry_802086C0 *fn_802086C0(CrashInfoCtrl *entry) {
    if (entry->m_table == 0) {
        return 0;
    }
    if (entry->m_record != 0 && entry->m_record->debris != 0) {
        for (int i = 0; i < entry->m_table->m_debrisCount; i++) {
            if (strcmp(entry->m_table->m_debris[i].name, entry->m_record->debris) == 0) {
                return &entry->m_table->m_debris[i];
            }
        }
    }
    return 0;
}

/* The sound entry the model record names, or 0. */
extern "C" UnkSoundEntry_80208888 *fn_80208618(CrashInfoCtrl *entry) {
    if (entry->m_table == 0) {
        return 0;
    }
    if (entry->m_record != 0 && entry->m_record->sound != 0) {
        for (int i = 0; i < entry->m_table->m_soundCount; i++) {
            if (strcmp(entry->m_table->m_sounds[i].name, entry->m_record->sound) == 0) {
                return &entry->m_table->m_sounds[i];
            }
        }
    }
    return 0;
}

/* The model record's scale, 1 without a record. */
extern "C" float fn_802085FC(CrashInfoCtrl *entry) {
    if (entry->m_record != 0) {
        return entry->m_record->scale;
    }
    return 1.0f;
}

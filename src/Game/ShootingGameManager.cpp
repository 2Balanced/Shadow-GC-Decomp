/* ShootingGame::ManagerTask (class name from RTTI) and the shooting game's balloon table: the
 * state shared by the Circus Park shooting game. PARK_BALLOON (Game/BalloonGenerator.cpp) creates
 * the table and the task when its type is loaded and releases them when it is unloaded; the
 * balloons and the SHOOTING_PANEL scores read and update one 0x1C-byte entry per balloon id. The
 * task clears the table every frame while the stage's current action reports that it is done.
 * File name inferred from the namespace (the unit follows PARK_BALLOON's, with sections of its
 * own). Names marked "(name inferred)" are not original, and so are the extern "C" fn_ names,
 * which keep the callers in other units linking. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include <string.h>

#include "Game/Gadget.h"
#include "Game/GadgetUtil.h"
/* The manager defines and uses the table's constructors and destructor. */
#define SHOOTINGGAME_TABLE_CTORS
#include "Game/ShootingGameTable.h"
#include "Game/StageAction.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

class CharInfo;

extern "C" {
/* A 32-bit flag word: fn_80014E04 constructs it clear, fn_80014E10 clears it, fn_80014E1C sets
 * bit and fn_80014E34 tests bit (behaviour from their bodies). */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
/* Rewards the character that popped a balloon (behaviour inferred; the meanings of the integer
 * arguments are unknown). */
void fn_8015F6EC(boost::shared_ptr<CharInfo> character, int arg1, int arg2, int arg3);
/* Creates a task of the Gong unit for kind unless kind is 5 (behaviour inferred). */
void fn_80136B5C(int kind);
}

namespace ShootingGame {

class ManagerTask : public Task {
  public:
    ManagerTask(Task *parent) : Task(parent) {
        m_name = s_taskName;
    }
    virtual ~ManagerTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(ManagerTask, 0x28);

const char *ManagerTask::s_taskName = "ShootingGame::ManagerTask";

} // namespace ShootingGame

using ShootingGame::BalloonEntry;
using ShootingGame::BalloonPosition;
using ShootingGame::BalloonTable;
using ShootingGame::ManagerTask;

/* The table and the task (names inferred). */
BalloonTable *lbl_805EEFC0;
ManagerTask *lbl_805EEFC4;

BalloonEntry::BalloonEntry() : x(0.0f), y(0.0f), z(0.0f), w(0.0f), m_count(0), m_state(0) {
    fn_80014E04(&m_flags);
}

BalloonTable::BalloonTable() {
    memset(this, 0, sizeof(BalloonTable));
}

extern "C" {

/* Adds delta to the balloon's count; a non-positive delta plays a sound (name inferred). */
void fn_8013D7BC(BalloonTable *table, int id, int delta) {
    if (id >= 0 && id < 0x100) {
        table->m_entries[id].m_count += delta;
        if (delta <= 0) {
            fn_801D560C(fn_80053164(), 0x743C, 0, 0);
        }
    }
}

/* Returns the balloon's count (name inferred). */
s32 fn_8013D794(BalloonTable *table, int id) {
    if (id >= 0 && id < 0x100) {
        return table->m_entries[id].m_count;
    }
    return 0;
}

/* Resets the balloon and plays a sound (name inferred). */
void fn_8013D72C(BalloonTable *table, int id) {
    if (id >= 0 && id < 0x100) {
        table->m_entries[id].m_count = 0;
        table->m_entries[id].m_state = 1;
        fn_80014E10(&table->m_entries[id].m_flags);
        fn_801D560C(fn_80053164(), 0x7436, 0, 0);
    }
}

/* Records a hit on the balloon by character: the first hit rewards the character by the
 * balloon's count and spawns the matching Gong task (name inferred). */
void fn_8013D5BC(BalloonTable *table, int id, boost::weak_ptr<CharInfo> &character) {
    if (id >= 0 && id < 0x100) {
        table->m_entries[id].m_state = 3;
        if (!fn_80014E34(&table->m_entries[id].m_flags, 0)) {
            if (!character.expired()) {
                if (boost::shared_ptr<CharInfo> hitter = character.lock()) {
                    int kind = 5;
                    int reward;
                    s32 count = table->m_entries[id].m_count;
                    if (count < 0) {
                        reward = -50;
                        kind = 0;
                    } else if (count <= 3) {
                        reward = 0;
                    } else if (count <= 7) {
                        reward = 5;
                        kind = 1;
                    } else if (count <= 11) {
                        reward = 10;
                        kind = 2;
                    } else if (count <= 15) {
                        reward = 15;
                        kind = 3;
                    } else {
                        reward = 50;
                        kind = 4;
                    }
                    fn_8015F6EC(hitter, 0, reward, 0);
                    fn_80136B5C(kind);
                }
            }
            fn_80014E1C(&table->m_entries[id].m_flags, 0);
        }
    }
}

/* Clears the balloon's count and flags (name inferred). */
void fn_8013D568(BalloonTable *table, int id) {
    if (id >= 0 && id < 0x100) {
        table->m_entries[id].m_count = 0;
        table->m_entries[id].m_state = 2;
        fn_80014E10(&table->m_entries[id].m_flags);
    }
}

/* Sets flag bit 1 of the balloon (name inferred). */
void fn_8013D524(BalloonTable *table, int id) {
    if (id >= 0 && id < 0x100) {
        fn_80014E1C(&table->m_entries[id].m_flags, 1);
    }
}

/* Tests flag bit 1 of the balloon (name inferred). */
bool fn_8013D4D8(BalloonTable *table, int id) {
    if (id >= 0 && id < 0x100) {
        return fn_80014E34(&table->m_entries[id].m_flags, 1);
    }
    return false;
}

/* Records the balloon's position (name inferred). The pointer is not const: with a const one
 * GC/2.7 loads all four floats before the first store, where retail interleaves them. */
void fn_8013D49C(BalloonTable *table, int id, BalloonPosition *position) {
    if (id >= 0 && id < 0x100) {
        table->m_entries[id].x = position->x;
        table->m_entries[id].y = position->y;
        table->m_entries[id].w = position->w;
        table->m_entries[id].z = position->z;
    }
}

/* Returns the word at 0xC of the balloon's position (name inferred). */
float fn_8013D474(BalloonTable *table, int id) {
    if (id >= 0 && id < 0x100) {
        return table->m_entries[id].w;
    }
    return 0.0f;
}

ManagerTask *fn_8013D280(void);

/* Creates the table and the task (name inferred; PARK_BALLOON's load callback). */
void fn_8013D434(void) {
    lbl_805EEFC0 = new BalloonTable;
    lbl_805EEFC4 = fn_8013D280();
}

/* Finishes the task and releases the table (name inferred; PARK_BALLOON's unload callback). */
void fn_8013D3DC(void) {
    if (lbl_805EEFC4 != 0) {
        lbl_805EEFC4->m_flags |= 1;
        lbl_805EEFC4 = 0;
    }
    if (lbl_805EEFC0 != 0) {
        delete lbl_805EEFC0;
        lbl_805EEFC0 = 0;
    }
}

} // extern "C"

ManagerTask::~ManagerTask() {}

bool StageActionManager::IsActionDone() {
    if (m_action) {
        return m_action->IsDone();
    }
    return true;
}

void ManagerTask::Tick(float dt) {
    if (fn_80074470()->m_prevState != 3 && fn_80074470()->IsActionDone()) {
        if (lbl_805EEFC0 != 0) {
            memset(lbl_805EEFC0, 0, sizeof(BalloonTable));
        }
    }
}

/* Creates the task under task layer 0 (name inferred). */
extern "C" ManagerTask *fn_8013D280(void) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 0);
    return new ManagerTask(parent);
}

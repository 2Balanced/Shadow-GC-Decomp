/* DeltaMorphAnimManagerTask (class name from RTTI): the task that plays the delta morph animations
 * of the clumps registered with the manager behind GetDeltaMorphAnimManager (class name unknown).
 * The manager keeps the clumps by name in a std::map (stand-in in MSL_C++), counting how often each
 * name was added. Before it come the collision table set up for the game (fn_801DDDD4) and the
 * constructor of an unknown object that clears a block of globals (fn_801DDE9C). File name inferred
 * from the class. Names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: every function matches, but one call does not. Retail calls the pair's destructor
 * out of line from the tree (0x801DE608, 0x801DE730) and inlines it for the temporary pair in
 * fn_801DE0EC, which calls the string's destructor directly. GC/2.7 either inlines it in all three
 * places (with a null check the tree's calls do not have) or in none: with the clump's empty
 * destructor below the tree's calls match and fn_801DE0EC calls the pair's destructor (the only
 * difference in the linked DOL, at 0x801DE188). No source shape tried gives both; it looks like
 * another difference of the game's compiler (docs/research-notes.md). */
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_PAIR_COPY_CALL
#define MSL_STRING_COPY
#define MSL_STRING_LESS
#define MSL_MAP_ERASE_KEY
#define MSL_TREE_CONST_ITERATOR_MIXED
#define MSL_TREE_END_NODE_NAME __tree_end_node_DeltaMorph
#include "Game/BitTable.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <map>
#include <string>
#include <string.h>

extern "C" {
typedef struct RpClump RpClump;

/* Where new tasks go (see Game/Gadget.h). */
void *fn_8001C4F8(void);
Task *fn_801EB304(void *root, int layer);
/* Plays a clump's delta morph animation on by dt (behaviour inferred). */
void fn_80037A48(RpClump *clump, float dt);

/* The collider manager (see Game/ArkLaser.cpp) and setting its number of kinds (inferred). */
void *fn_80021B08(void);
void fn_800C76C0(void *manager, int kinds);
}

/* A clump the manager animates and how often its name was added (struct name unknown). */
struct UnkDeltaMorphClump {
    /* 0x0 */ RpClump *m_clump;
    /* 0x4 */ s32 m_count;

    /* An empty destructor: written out, the pair's destructor is called out of line from the tree
     * (retail calls it there). */
    ~UnkDeltaMorphClump() {}
};

/* The manager of the clumps whose delta morph animations play (class name unknown; see
 * Game/CircusPole.cpp). */
class UnkDeltaMorphAnimManager_801DE414 {
  public:
    typedef std::map<std::string, UnkDeltaMorphClump> ClumpMap;

    UnkDeltaMorphAnimManager_801DE414();
    ~UnkDeltaMorphAnimManager_801DE414();

    static UnkDeltaMorphAnimManager_801DE414 *s_cached;

    /* 0x00 */ ClumpMap m_clumps;
    /* 0x10 */ u32 field_0x10;
};
SIZE_ASSERT(UnkDeltaMorphAnimManager_801DE414, 0x14);

/* The manager: a function-local static object, whose address the accessor caches (see
 * Game/CircusPole.cpp). */
inline UnkDeltaMorphAnimManager_801DE414 &DeltaMorphAnimManagerObject() {
    static UnkDeltaMorphAnimManager_801DE414 s_object;
    return s_object;
}

inline UnkDeltaMorphAnimManager_801DE414 *GetDeltaMorphAnimManager() {
    if (UnkDeltaMorphAnimManager_801DE414::s_cached == 0) {
        UnkDeltaMorphAnimManager_801DE414::s_cached = &DeltaMorphAnimManagerObject();
    }
    return UnkDeltaMorphAnimManager_801DE414::s_cached;
}

extern "C" {
/* Adds clump under name, or counts name once more if it is there; true if the clump was added
 * (names inferred). */
bool fn_801DE0EC(UnkDeltaMorphAnimManager_801DE414 *manager, const std::string &name, RpClump *clump);
/* Counts name once less and drops it at zero (name inferred). */
void fn_801DDFDC(UnkDeltaMorphAnimManager_801DE414 *manager, const std::string &name);
/* Plays every clump's animation (name inferred). */
void fn_801DE1F0(UnkDeltaMorphAnimManager_801DE414 *manager, float dt);
}

class DeltaMorphAnimManagerTask : public Task {
  public:
    DeltaMorphAnimManagerTask(Task *parent) : Task(parent) {
        m_name = s_taskName;
    }
    virtual ~DeltaMorphAnimManagerTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(DeltaMorphAnimManagerTask, 0x28);

/* The block of globals the object below clears (struct name unknown). */
struct UnkGlobals_80583860 {
    /* 0x00 */ s32 field_0x0;
    /* 0x04 */ s32 field_0x4;
    /* 0x08 */ u8 field_0x8[0x34 - 0x8];
};

UnkGlobals_80583860 lbl_80583860;

/* An object with no data (class unknown) whose constructor clears the globals above. */
class UnkReset_801DDE9C {
  public:
    UnkReset_801DDE9C();
};

/* Which collision kinds meet which (inferred), followed by four bytes of zeroes in the game (their
 * meaning is unknown). */
static const struct {
    bool kinds[18][18];
    u32 field_0x144;
} s_collisionTable = {{
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0},
    {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0},
    {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
    {1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0},
    {1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 1, 0, 0},
    {1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0},
    {1, 1, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0},
    {1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
    {1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
}, 0};

const char *DeltaMorphAnimManagerTask::s_taskName = "DeltaMorphAnimManagerTask";

/* Not called anywhere: the linker strips it from the game. Retail's .rodata has the allocation
 * failure message before the tree's length error, the order of a unit that uses it before the
 * tree's instances. */
void UnusedAllocationFailure() {
    std::__msl_error("Memory allocation failure");
}

UnkDeltaMorphAnimManager_801DE414::UnkDeltaMorphAnimManager_801DE414() {
    m_clumps.clear();
}

UnkDeltaMorphAnimManager_801DE414::~UnkDeltaMorphAnimManager_801DE414() {
    m_clumps.clear();
}

void fn_801DE1F0(UnkDeltaMorphAnimManager_801DE414 *manager, float dt) {
    UnkDeltaMorphAnimManager_801DE414::ClumpMap::const_iterator it;
    for (it = manager->m_clumps.begin(); it != manager->m_clumps.end(); ++it) {
        fn_80037A48((*it).second.m_clump, dt);
    }
}

bool fn_801DE0EC(UnkDeltaMorphAnimManager_801DE414 *manager, const std::string &name, RpClump *clump) {
    UnkDeltaMorphAnimManager_801DE414::ClumpMap::iterator it = manager->m_clumps.find(name);
    if (it != manager->m_clumps.end()) {
        (*it).second.m_count++;
        return false;
    }
    UnkDeltaMorphClump entry;
    entry.m_clump = clump;
    entry.m_count = 1;
    manager->m_clumps.insert(UnkDeltaMorphAnimManager_801DE414::ClumpMap::value_type(name, entry));
    return true;
}

void fn_801DDFDC(UnkDeltaMorphAnimManager_801DE414 *manager, const std::string &name) {
    UnkDeltaMorphAnimManager_801DE414::ClumpMap::iterator it = manager->m_clumps.find(name);
    if (it != manager->m_clumps.end()) {
        (*it).second.m_count--;
        if ((*it).second.m_count <= 0) {
            manager->m_clumps.erase(name);
        }
    }
}

DeltaMorphAnimManagerTask::~DeltaMorphAnimManagerTask() {}

void DeltaMorphAnimManagerTask::Tick(float dt) {
    fn_801DE1F0(GetDeltaMorphAnimManager(), dt);
}

/* Starts the task (name inferred). */
extern "C" void fn_801DDEF0(void) {
    new DeltaMorphAnimManagerTask(fn_801EB304(fn_8001C4F8(), 0));
}

UnkReset_801DDE9C::UnkReset_801DDE9C() {
    memset(&lbl_80583860, 0, sizeof(lbl_80583860));
    lbl_80583860.field_0x0 = -1;
    lbl_80583860.field_0x4 = -1;
}

/* Sets up the collider manager's table of which collision kinds meet (name inferred). */
extern "C" void fn_801DDDD4(void) {
    SonicteamUSA::System::BitTable matrix(18, 18, false);
    for (s32 i = 0; i < 18; i++) {
        for (s32 j = 0; j < 18; j++) {
            if (s_collisionTable.kinds[i][j]) {
                matrix.Set(i, j);
            } else {
                matrix.Clear(i, j);
            }
        }
    }
    fn_800C76C0(fn_80021B08(), 18);
    *reinterpret_cast<SonicteamUSA::System::BitTable *>((u8 *)fn_80021B08() + 8) = matrix;
}

#include <map_impl>

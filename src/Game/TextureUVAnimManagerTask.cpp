/* TextureUVAnimManagerTask (class name from RTTI): the task that plays the texture UV animations of
 * the clumps registered with the manager behind GetUVAnimManager (class name unknown). The manager
 * keeps the clumps by name in a std::map (stand-in in MSL_C++), counting how often each name was
 * added; Game/DeltaMorphAnimManagerTask.cpp is its twin for delta morph animations. File name from
 * the class. Names marked "(name inferred)" are not original. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: objdiff matches every function and section, but the linked DOL differs in one call,
 * the blocker of Game/DeltaMorphAnimManagerTask.cpp: retail calls the pair's destructor out of line
 * from the tree (0x801EB718) and inlines it for the temporary pair in fn_801EB660 (0x801EB6FC calls
 * the string's destructor directly); GC/2.7 does one or the other everywhere. */
#define MSL_PAIR_CONVERTING_CONSTRUCTOR
#define MSL_PAIR_COPY_CALL
#define MSL_STRING_COPY
#define MSL_STRING_LESS
#define MSL_MAP_ERASE_KEY
#define MSL_TREE_CONST_ITERATOR_MIXED
#define MSL_TREE_END_NODE_NAME __tree_end_node_TextureUVAnim
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <map>
#include <string>

extern "C" {
typedef struct RpClump RpClump;

/* Plays a clump's UV animation on by dt (behaviour inferred; the last argument is -1). */
void fn_80039B94(RpClump *clump, float dt, float frame);
}

/* A clump the manager animates and how often its name was added (struct name unknown). */
struct UnkUVAnimClump {
    /* 0x0 */ RpClump *m_clump;
    /* 0x4 */ s32 m_count;

    /* An empty destructor: written out, the pair's destructor is called out of line from the tree
     * (retail calls it there). */
    ~UnkUVAnimClump() {}
};

/* The manager of the clumps whose UV animations play (class name unknown; see Game/HintRing.cpp). */
class UnkUVAnimManager_801EB994 {
  public:
    typedef std::map<std::string, UnkUVAnimClump> ClumpMap;

    UnkUVAnimManager_801EB994();
    ~UnkUVAnimManager_801EB994();

    /* 0x00 */ ClumpMap m_clumps;
};
SIZE_ASSERT(UnkUVAnimManager_801EB994, 0x10);

/* The manager (defined in another unit; see Game/HintRing.cpp). */
UnkUVAnimManager_801EB994 *GetUVAnimManager();

extern "C" {
/* Adds clump under name, or counts name once more if it is there; true if the clump was added
 * (names inferred). */
bool fn_801EB660(UnkUVAnimManager_801EB994 *manager, const std::string &name, RpClump *clump);
/* Counts name once less and drops it at zero (name inferred). */
void fn_801EB550(UnkUVAnimManager_801EB994 *manager, const std::string &name);
/* Plays every clump's animation (name inferred). */
void fn_801EB764(UnkUVAnimManager_801EB994 *manager, float dt);
}

class TextureUVAnimManagerTask : public Task {
  public:
    /* Out of line: Game/GameSystem.cpp constructs the task (it calls it by its address,
     * fn_801EB50C). */
    TextureUVAnimManagerTask(Task *parent);
    virtual ~TextureUVAnimManagerTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(TextureUVAnimManagerTask, 0x28);

const char *TextureUVAnimManagerTask::s_taskName = "TextureUVAnimManagerTask";

/* Not called anywhere: the linker strips it from the game. Retail's .rodata has the allocation
 * failure message before the tree's length error, the order of a unit that uses it before the
 * tree's instances. */
void UnusedAllocationFailure() {
    std::__msl_error("Memory allocation failure");
}

UnkUVAnimManager_801EB994::UnkUVAnimManager_801EB994() {
    m_clumps.clear();
}

UnkUVAnimManager_801EB994::~UnkUVAnimManager_801EB994() {
    m_clumps.clear();
}

void fn_801EB764(UnkUVAnimManager_801EB994 *manager, float dt) {
    UnkUVAnimManager_801EB994::ClumpMap::const_iterator it;
    for (it = manager->m_clumps.begin(); it != manager->m_clumps.end(); ++it) {
        fn_80039B94((*it).second.m_clump, dt, -1.0f);
    }
}

bool fn_801EB660(UnkUVAnimManager_801EB994 *manager, const std::string &name, RpClump *clump) {
    UnkUVAnimManager_801EB994::ClumpMap::iterator it = manager->m_clumps.find(name);
    if (it != manager->m_clumps.end()) {
        (*it).second.m_count++;
        return false;
    }
    UnkUVAnimClump entry;
    entry.m_clump = clump;
    entry.m_count = 1;
    manager->m_clumps.insert(UnkUVAnimManager_801EB994::ClumpMap::value_type(name, entry));
    return true;
}

void fn_801EB550(UnkUVAnimManager_801EB994 *manager, const std::string &name) {
    UnkUVAnimManager_801EB994::ClumpMap::iterator it = manager->m_clumps.find(name);
    if (it != manager->m_clumps.end()) {
        (*it).second.m_count--;
        if ((*it).second.m_count <= 0) {
            manager->m_clumps.erase(name);
        }
    }
}

TextureUVAnimManagerTask::TextureUVAnimManagerTask(Task *parent) : Task(parent) {
    m_name = s_taskName;
}

TextureUVAnimManagerTask::~TextureUVAnimManagerTask() {}

void TextureUVAnimManagerTask::Tick(float dt) {
    fn_801EB764(GetUVAnimManager(), dt);
}

#include <map_impl>

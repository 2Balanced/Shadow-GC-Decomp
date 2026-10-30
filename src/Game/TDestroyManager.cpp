/* TDestroyManager (class name from RTTI): the task that destroys RenderWare objects a few at a time.
 * Each frame it destroys the first object of its list; fn_800436FC (called by the game system)
 * empties the whole list at once. Names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first); the vector's
 * members defined out of line come out at the end of the unit. */
#define MSL_VECTOR_MIDDLE
#define MSL_VECTOR_BACK
#define MSL_VECTOR_ERASE_ONE
#define MSL_VECTOR_ERASE_ONE_DATA
#define MSL_ALGORITHM_COPY
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <vector>

struct RpClump;

extern "C" {
/* RenderWare: destroy an object, the world a clump is in, take a clump out of its world and destroy
 * a clump (behaviours inferred). */
void fn_8045FFA0(void *object);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_80457468(RpClump *clump);
void fn_802E0600(void *object);
bool fn_800436FC(void);
}

/* An object waiting to be destroyed (struct name unknown): one of the three is set. */
struct UnkDestroyEntry_80043740 {
    /* 0x0 */ void *m_object;  /* destroyed by fn_8045FFA0 */
    /* 0x4 */ RpClump *m_clump; /* taken out of its world, then destroyed */
    /* 0x8 */ void *m_other;   /* destroyed by fn_802E0600 */
};
SIZE_ASSERT(UnkDestroyEntry_80043740, 0xC);

class TDestroyManager : public Task {
  public:
    TDestroyManager(Task *parent);
    virtual ~TDestroyManager();
    virtual void Tick(float dt);

    /* Destroys the first object of the list; false if it is empty (name inferred). */
    bool DestroyOne();

    static TDestroyManager *s_instance;

    /* 0x28 */ std::vector<UnkDestroyEntry_80043740> m_list;
    /* 0x34 */ u32 field_0x34;
};
SIZE_ASSERT(TDestroyManager, 0x38);

TDestroyManager *TDestroyManager::s_instance;

/* Destroys a clump's world object at once (name inferred; the list is not used). */
extern "C" bool fn_80043A64(void *object) {
    fn_8045FFA0(object);
    return true;
}

/* Destroys an object at once (name inferred; the list is not used). */
extern "C" bool fn_80043A40(void *object) {
    fn_802E0600(object);
    return true;
}

TDestroyManager::TDestroyManager(Task *parent) : Task(parent) {
    m_name = "TDestoryManager"; /* sic */
    s_instance = this;
    m_flags |= 0x100;
}

TDestroyManager::~TDestroyManager() {
    fn_800436FC();
    s_instance = 0;
}

void TDestroyManager::Tick(float dt) {
    DestroyOne();
}

bool TDestroyManager::DestroyOne() {
    if (m_list.empty()) {
        return false;
    }
    UnkDestroyEntry_80043740 *entry = m_list.begin();
    if (entry->m_object != 0) {
        fn_8045FFA0(entry->m_object);
    } else if (entry->m_clump != 0) {
        void *world = fn_8046256C(entry->m_clump);
        if (world != 0) {
            fn_804623EC(world, entry->m_clump);
            fn_80457468(entry->m_clump);
        }
    } else if (entry->m_other != 0) {
        fn_802E0600(entry->m_other);
    }
    m_list.erase(entry);
    return true;
}

/* Destroys every object of the list (name inferred); false if there is no manager. */
extern "C" bool fn_800436FC(void) {
    if (TDestroyManager::s_instance != 0) {
        while (TDestroyManager::s_instance->DestroyOne()) {
        }
        return true;
    }
    return false;
}

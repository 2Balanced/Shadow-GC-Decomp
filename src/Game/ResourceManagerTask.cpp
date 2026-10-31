/* ResourceManagerTask (class name from RTTI): the task that loads the resources registered with
 * the resource manager behind fn_800122D4 (class name unknown). The manager keeps the resources
 * (Game/Resource.h) by number in a std::map (stand-in in MSL_C++); other units reach it and the
 * data of its resources through the extern "C" functions below. The unit also holds the game's
 * first copies of the map's shared tree code: stepping an iterator (fn_80012E90) and the
 * rebalancing before a node is unlinked (fn_80013398). File name from the class; other names are
 * inferred.
 *
 * NonMatching: the unit has retail's 64 functions, with its size for every one of them and for
 * .text as a whole, and 62 of the 64 stand in retail's order. The two shared helpers do not:
 * retail emits them inside the tree's instance run (fn_80012E90 after value_compare's operator(),
 * fn_80013398 right after erase), as if they were instances of a template, while GC/2.7 emits
 * them here as a pair in front of erase_one. Their place does not follow the place their
 * definitions have in the unit: with the two blocks later in the unit, one of them is emitted
 * before everything else; as friends of a class template (the shape that fits fn_8000CD64's group
 * in Game/CameraFactory.cpp) they come out separately but at 24 and 66 of 68, four functions too
 * many. Retail's data also needs three tails we do not write: 2 B in .rodata after the last string
 * pool entry, 4 B in .bss after the manager and 3 B in .sdata2 after its pool constant. Its .sdata is
 * the right size but not the right order: the three RTTI records come before the two tree flags,
 * where our trailing #include <map_impl> emits the flags first. */
#define MSL_MAP_ERASE_KEY
#define MSL_TREE_ERASE_VALUE_INLINE
#define MSL_TREE_ITERATOR_ASSIGN_CALL
#define MSL_TREE_BOOL_OBJECTS
#define MSL_TREE_END_NODE_NAME __tree_end_node_ResourceManager
#include "Game/Resource.h"
#include "Game/Task.h"
#include <dolphin/types.h>
#include "size_assert.h"
#include <map>
#include <typeinfo>

namespace std {

inline __tree_node_base *__tree_parent(const __tree_node_base *node) {
    return (__tree_node_base *)(node->parent_ & ~1);
}

extern "C" {
/* A node's colour and its setting, and the two rotations (Game/CameraFactory.cpp). */
bool fn_8000CF60(__tree_node_base *node);
void fn_8000CF6C(__tree_node_base *node, bool red);
void fn_8000CF94(__tree_node_base *node, __tree_node_base *&root);
void fn_8000D014(__tree_node_base *node, __tree_node_base *&root);
/* Steps an iterator to the next node; declared here so that <map_impl>'s erase and increment can
 * call it, and defined below along with the rebalancing it uses. */
void fn_80012E90(__tree_node_base **iterator);
}

/* Rebalances the tree under root before z is unlinked. */
extern "C" void fn_80013398(__tree_node_base *z, __tree_node_base *root) {
    __tree_node_base *y = z;
    if (z->left_ != 0 && z->right_ != 0) {
        fn_80012E90(&y);
    }
    __tree_node_base *x = y->left_ != 0 ? y->left_ : y->right_;
    __tree_node_base *xParent = __tree_parent(y);
    if (x != 0) {
        x->set_parent(xParent);
    }
    bool xLeft;
    if (y == __tree_parent(y)->left_) {
        __tree_parent(y)->left_ = x;
        xLeft = true;
    } else {
        __tree_parent(y)->right_ = x;
        xLeft = false;
    }
    bool rebalance = fn_8000CF60(y) == false;
    if (y != z) {
        y->parent_ = (z->parent_ & ~1) | (y->parent_ & 1);
        if (z == __tree_parent(y)->left_) {
            __tree_parent(y)->left_ = y;
        } else {
            __tree_parent(y)->right_ = y;
        }
        y->left_ = z->left_;
        if (y->left_ != 0) {
            y->left_->set_parent(y);
        } else {
            xParent = y;
        }
        y->right_ = z->right_;
        if (y->right_ != 0) {
            y->right_->set_parent(y);
        } else {
            xParent = y;
        }
        fn_8000CF6C(y, fn_8000CF60(z));
        if (root == z) {
            root = y;
        }
    } else if (x == 0 && z == root) {
        rebalance = false;
    }
    if (rebalance) {
        while (x != root && (x == 0 || fn_8000CF60(x) == false)) {
            if (xLeft) {
                __tree_node_base *w = xParent->right_;
                if (fn_8000CF60(w) == true) {
                    fn_8000CF6C(w, false);
                    fn_8000CF6C(xParent, true);
                    fn_8000D014(xParent, root);
                    w = xParent->right_;
                }
                if ((w->left_ == 0 || fn_8000CF60(w->left_) == false) &&
                    (w->right_ == 0 || fn_8000CF60(w->right_) == false)) {
                    fn_8000CF6C(w, true);
                    x = xParent;
                    xParent = __tree_parent(xParent);
                    xLeft = x == xParent->left_;
                } else {
                    if (w->right_ == 0 || fn_8000CF60(w->right_) == false) {
                        fn_8000CF6C(w->left_, false);
                        fn_8000CF6C(w, true);
                        fn_8000CF94(w, root);
                        w = xParent->right_;
                    }
                    fn_8000CF6C(w, fn_8000CF60(xParent));
                    fn_8000CF6C(xParent, false);
                    fn_8000CF6C(w->right_, false);
                    fn_8000D014(xParent, root);
                    x = root;
                }
            } else {
                __tree_node_base *w = xParent->left_;
                if (fn_8000CF60(w) == true) {
                    fn_8000CF6C(w, false);
                    fn_8000CF6C(xParent, true);
                    fn_8000CF94(xParent, root);
                    w = xParent->left_;
                }
                if ((w->left_ == 0 || fn_8000CF60(w->left_) == false) &&
                    (w->right_ == 0 || fn_8000CF60(w->right_) == false)) {
                    fn_8000CF6C(w, true);
                    x = xParent;
                    xParent = __tree_parent(xParent);
                    xLeft = x == xParent->left_;
                } else {
                    if (w->left_ == 0 || fn_8000CF60(w->left_) == false) {
                        fn_8000CF6C(w->right_, false);
                        fn_8000CF6C(w, true);
                        fn_8000D014(w, root);
                        w = xParent->left_;
                    }
                    fn_8000CF6C(w, fn_8000CF60(xParent));
                    fn_8000CF6C(xParent, false);
                    fn_8000CF6C(w->left_, false);
                    fn_8000CF94(xParent, root);
                    x = root;
                }
            }
        }
        if (x != 0) {
            fn_8000CF6C(x, false);
        }
    }
}

/* Steps an iterator to the next node. */
extern "C" void fn_80012E90(__tree_node_base **iterator) {
    __tree_node_base *node = (*iterator)->right_;
    if (node != 0) {
        while (node->left_ != 0) {
            node = node->left_;
        }
        *iterator = node;
        return;
    }
    while (*iterator != __tree_parent(*iterator)->left_) {
        *iterator = __tree_parent(*iterator);
    }
    *iterator = __tree_parent(*iterator);
}

} // namespace std

/* The resource manager (class name unknown): the resources by number, how many were loading at
 * the last update, and the first number Add tries. */
class UnkResourceManager_800122D4 {
  public:
    typedef std::map<int, Resource *> ResourceMap;

    UnkResourceManager_800122D4();
    ~UnkResourceManager_800122D4();

    /* 0x00 */ ResourceMap m_resources;
    /* 0x10 */ s32 m_loading;
    /* 0x14 */ s32 m_nextId;
};

class ResourceManagerTask : public Task {
  public:
    ResourceManagerTask(Task *parent);
    virtual ~ResourceManagerTask();
    virtual void Tick(float dt);

    static const char *s_taskName;
};
SIZE_ASSERT(ResourceManagerTask, 0x28);

typedef UnkResourceManager_800122D4 ResourceManager;

extern "C" {
ResourceManager *fn_800122D4(void);
ResourceManager *fn_80012308(void);
void fn_80012A30(ResourceManager *manager, float dt);
}

const char *ResourceManagerTask::s_taskName = "ResourceManagerTask";
UnkResourceManager_800122D4::UnkResourceManager_800122D4() {
    m_resources.clear();
    m_loading = 0;
    m_nextId = 0;
}

UnkResourceManager_800122D4::~UnkResourceManager_800122D4() {
    m_resources.clear();
    m_loading = 0;
    m_nextId = 0;
}

/* Loads every resource that is not loaded yet, counting them. */
extern "C" void fn_80012A30(ResourceManager *manager, float dt) {
    ResourceManager::ResourceMap::iterator it;
    manager->m_loading = 0;
    for (it = manager->m_resources.begin(); it != manager->m_resources.end(); ++it) {
        if ((*it).second->GetState() != 3) {
            (*it).second->Load();
            manager->m_loading++;
        }
    }
}

/* Registers resource under number id, unless the number is taken. */
extern "C" bool fn_8001299C(ResourceManager *manager, int id, Resource *resource) {
    if (manager->m_resources.find(id) != manager->m_resources.end()) {
        return false;
    }
    manager->m_resources.insert(ResourceManager::ResourceMap::value_type(id, resource));
    manager->m_loading++;
    return true;
}

/* Registers resource under the first free number from the manager's first one (of 100 tried);
 * the number, or -1. */
extern "C" int fn_800128D8(ResourceManager *manager, Resource *resource) {
    int base = manager->m_nextId;
    for (int i = 0; i < 100; i++) {
        s32 id = base + i;
        if (manager->m_resources.find(id) == manager->m_resources.end()) {
            manager->m_resources.insert(ResourceManager::ResourceMap::value_type(id, resource));
            manager->m_loading++;
            return id;
        }
    }
    return -1;
}

/* Releases and deletes the resource numbered id. */
extern "C" void fn_800127F8(ResourceManager *manager, int id) {
    ResourceManager::ResourceMap::iterator it = manager->m_resources.find(id);
    if (it != manager->m_resources.end()) {
        (*it).second->Release();
        delete (*it).second;
        manager->m_resources.erase(id);
    }
}

/* The data of an entry of the resource numbered id, by index or name (0 without the resource). */
extern "C" void *fn_8001277C(ResourceManager *manager, int id, int index) {
    ResourceManager::ResourceMap::iterator it = manager->m_resources.find(id);
    if (it != manager->m_resources.end()) {
        return (*it).second->GetData(index);
    }
    return 0;
}

extern "C" void *fn_80012700(ResourceManager *manager, int id, const char *name) {
    ResourceManager::ResourceMap::iterator it = manager->m_resources.find(id);
    if (it != manager->m_resources.end()) {
        return (*it).second->GetData(name);
    }
    return 0;
}

/* The hierarchy of a bone entry of the resource numbered id. */
extern "C" void *fn_80012684(ResourceManager *manager, int id, int index) {
    ResourceManager::ResourceMap::iterator it = manager->m_resources.find(id);
    if (it != manager->m_resources.end()) {
        return (*it).second->GetHierarchy(index);
    }
    return 0;
}

/* The motion of a motion entry of the resource numbered id, by index or name. */
extern "C" u32 fn_80012608(ResourceManager *manager, int id, int index) {
    ResourceManager::ResourceMap::iterator it = manager->m_resources.find(id);
    if (it != manager->m_resources.end()) {
        return (*it).second->GetMotion(index);
    }
    return 0;
}

extern "C" u32 fn_8001258C(ResourceManager *manager, int id, const char *name) {
    ResourceManager::ResourceMap::iterator it = manager->m_resources.find(id);
    if (it != manager->m_resources.end()) {
        return (*it).second->GetMotion(name);
    }
    return 0;
}

extern "C" void *fn_80012510(ResourceManager *manager, int id, int index) {
    ResourceManager::ResourceMap::iterator it = manager->m_resources.find(id);
    if (it != manager->m_resources.end()) {
        return (*it).second->vf_0x30(index);
    }
    return 0;
}

/* The load state of the resource numbered id (4 without the resource). */
extern "C" int fn_80012408(ResourceManager *manager, int id) {
    ResourceManager::ResourceMap::iterator it = manager->m_resources.find(id);
    if (it != manager->m_resources.end()) {
        return (*it).second->GetState();
    }
    return 4;
}

ResourceManagerTask::ResourceManagerTask(Task *parent) : Task(parent) {
    m_name = s_taskName;
    m_flags |= 0x100;
}

ResourceManagerTask::~ResourceManagerTask() {}

/* The manager itself. */
extern "C" ResourceManager *fn_80012308(void) {
    static ResourceManager s_manager;
    return &s_manager;
}

/* The resource manager. */
extern "C" ResourceManager *fn_800122D4(void) {
    static ResourceManager *s_instance;
    if (s_instance == 0) {
        s_instance = fn_80012308();
    }
    return s_instance;
}

void ResourceManagerTask::Tick(float dt) {
    fn_80012A30(fn_800122D4(), dt);
}
/* Retail's .sdata holds the RTTI records of the task and of Resource before insert_unique's result
 * flags; GC/2.7 emits those records with the two vtables, and the flags are named objects defined
 * where <map_impl> is included (see MSL_TREE_BOOL_OBJECTS there). */
#include <map_impl>

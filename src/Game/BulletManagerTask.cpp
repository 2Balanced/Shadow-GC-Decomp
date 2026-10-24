/* BulletManagerTask (class name from RTTI: Weapon::Bullet::BulletManagerTask): the task that ticks
 * the bullets in flight, each with its owner's frame time, and deletes the bullets left when it
 * ends. The bullets are kept by the bullet manager (Game/BulletManager.h), whose functions are
 * here too, in a list of pointers, the game's list of pointers (see Game/ElecSecurityCode.h). This
 * unit is the first to emit two of that list's shared functions (fn_80058F00 and fn_80058F48),
 * which other units call by their addresses. It also has the model a homing shot carries
 * (Game/BulletHoming.cpp). File name from the class name; other names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#define BULLET_MANAGER_MEMBERS
#include "Game/BulletManager.h"
#include "Game/CharInfo.h"
#include "Game/DebugDispShape.h"
#include "Game/MathTypes.h"
#include "Game/Task.h"
#include "Game/WeaponBullet.h"
#include "boost/checked_delete.hpp"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Weapon::Bullet::BulletBase;
using Weapon::Bullet::BulletManager;
using Weapon::Bullet::BulletPosition;

/* The links of a node of a list of pointers; the node keeps its value after them (see
 * Game/ElecSecurityCode.h). */
struct UnkPtrListLink_8001B5B8 {
    /* 0x0 */ UnkPtrListLink_8001B5B8 *prev;
    /* 0x4 */ UnkPtrListLink_8001B5B8 *next;
};

/* A position in the list implementation below. */
struct UnkPtrListPosition_8001B674 {
    UnkPtrListPosition_8001B674(const UnkPtrListPosition_8001B674 &other) : node(other.node) {}

    /* 0x0 */ UnkPtrListLink_8001B5B8 *node;
};

/* The list implementation the game's lists of pointers share (see Game/ElecSecurityCode.h). */
struct UnkPtrList_8001B5B8 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkPtrListLink_8001B5B8 m_end;
};
SIZE_ASSERT(UnkPtrList_8001B5B8, 0xC);

extern "C" {
/* The list implementation's constructor and destructor, its first and its end position, its end
 * node, removing and appending a value (see Game/ElecSecurityCode.h and Game/DebugDisp.cpp). */
void fn_8001BE04(UnkPtrList_8001B5B8 *list);
void fn_8001B5B8(UnkPtrList_8001B5B8 *list, short flag);
UnkPtrListPosition_8001B674 fn_8001B6B0(UnkPtrList_8001B5B8 *list);
UnkPtrListPosition_8001B674 fn_8001B674(UnkPtrList_8001B5B8 *list);
void fn_8001B42C(UnkPtrList_8001B5B8 *list, void *const &value);
void fn_8001D820(UnkPtrList_8001B5B8 *list, void *const &value);
/* The bullet manager (Game/BulletBase.cpp). */
BulletManager *fn_800522A8(void);
/* The resource loader and a clump of one of its resources (see Game/NpcMotion.cpp). */
void *fn_800122D4(void);
RpClump *fn_8001277C(void *loader, int group, int index);
/* RenderWare: a copy of a clump, destroying one, the world a clump is in, removing it from and
 * adding it to a world (see Game/NpcMotion.cpp), and the frame of a clump's frame (behaviour
 * inferred). */
RpClump *fn_80457090(RpClump *clump);
void fn_80457468(RpClump *clump);
void *fn_8046256C(RpClump *clump);
void fn_804623EC(void *world, RpClump *clump);
void fn_804622FC(void *world, RpClump *clump);
RwFrame *fn_804872D8(RwFrame *frame);
/* The area a clump is in (see Game/NpcMotion.cpp). */
void *fn_80169364(RpClump *clump);
int fn_80169350(void *frame);
}

/* Keeps the inline function that calls it out of line, as the list's members are. */
inline void BulletListKeep() {}

/* The number of values of a list of pointers (other units call it by its address). It calls
 * SizeOf, so both stay out of line. */
inline u32 &SizeOf(UnkPtrList_8001B5B8 *list) {
    BulletListKeep();
    return list->m_size;
}

extern "C" inline u32 fn_80058F00(UnkPtrList_8001B5B8 *list) {
    return SizeOf(list);
}

/* The end node of a list of pointers (0x8001B6A8, which an earlier unit is the first to emit). */
extern "C" inline UnkPtrListLink_8001B5B8 *fn_8001B6A8(UnkPtrList_8001B5B8 *list) {
    BulletListKeep();
    return &list->m_end;
}

/* The first value of a list of pointers (other units call it by its address). */
extern "C" inline void *&fn_80058F48(UnkPtrList_8001B5B8 *list) {
    return *reinterpret_cast<void **>(fn_8001B6A8(list)->next + 1);
}

namespace Weapon {
namespace Bullet {

/* The bullets: the game's list of pointers holding BulletBase pointers. Its members are inline and
 * emitted after their first user, as the game's list template's are (names inferred). */
class BulletList : public UnkPtrList_8001B5B8 {
  public:
    struct Node : public UnkPtrListLink_8001B5B8 {
        /* 0x8 */ BulletBase *value;
    };

    class iterator {
      public:
        iterator(const iterator &other) : m_node(other.m_node) {
            BulletListKeep();
        }
        iterator(const UnkPtrListPosition_8001B674 &position) : m_node(position.node) {
            BulletListKeep();
        }

        BulletBase *&operator*() const {
            BulletListKeep();
            return static_cast<Node *>(m_node)->value;
        }
        iterator operator++(int) {
            iterator old(*this);
            m_node = m_node->next;
            return old;
        }
        bool operator!=(const iterator &other) const {
            BulletListKeep();
            return m_node != other.m_node;
        }

        /* 0x0 */ UnkPtrListLink_8001B5B8 *m_node;
    };

    BulletList() {
        BulletListKeep();
        fn_8001BE04(this);
    }
    ~BulletList() {
        BulletListKeep();
        fn_8001B5B8(this, 0);
    }

    iterator begin() {
        return iterator(fn_8001B6B0(this));
    }
    iterator end() {
        return iterator(fn_8001B674(this));
    }
    BulletBase *&front() {
        return reinterpret_cast<BulletBase *&>(fn_80058F48(this));
    }
    void push_back(BulletBase *const &value) {
        BulletListKeep();
        fn_8001D820(this, reinterpret_cast<void *const &>(value));
    }
    void remove(BulletBase *const &value) {
        BulletListKeep();
        fn_8001B42C(this, reinterpret_cast<void *const &>(value));
    }
};
SIZE_ASSERT(BulletList, 0xC);

/* What the manager keeps behind its pointer (class name unknown). */
class BulletManager::Impl {
  public:
    Impl();
    ~Impl();

    void Add(BulletBase *bullet);
    void Remove(BulletBase *bullet);
    /* Deletes every bullet. */
    void DeleteAll();
    /* Ticks every bullet with its owner's frame time. */
    void Tick(float dt);

    /* 0x0 */ BulletList m_bullets;
};

/* Deletes the list (name inferred; see checked_delete below). */
inline void DeleteBulletManagerImpl(BulletManager::Impl *impl) {
    delete impl;
}

} // namespace Bullet
} // namespace Weapon

/* Retail keeps boost::checked_delete of the list out of line and its destructor at its own place:
 * this specialization calls an inline helper, which gives that (see Game/EnergyCore.cpp). */
namespace boost {
template <> inline void checked_delete<BulletManager::Impl>(BulletManager::Impl *x) {
    Weapon::Bullet::DeleteBulletManagerImpl(x);
}
} // namespace boost

/* The world of each stage area (see Game/BlackDoomCtrlUnit.cpp). */
struct UnkWorldEntry_8051D208 {
    /* 0x00 */ u8 field_0x0[0x10];
    /* 0x10 */ void *m_world;
    /* 0x14 */ u8 field_0x14[0x18 - 0x14];
};
extern UnkWorldEntry_8051D208 lbl_8051D208[];

/* The stage (class name unknown; fn_8003A840 returns it). GetWorld, which calls GetAreas, stays
 * out of line; this unit is the first to emit it. */
class UnkStage_8003A840 {
  public:
    UnkWorldEntry_8051D208 *GetAreas() {
        return lbl_8051D208;
    }
    void *GetWorld(int area) {
        return GetAreas()[area].m_world;
    }
};

extern "C" UnkStage_8003A840 *fn_8003A840(void);

/* Puts clump in world (name inferred; as in Game/NpcMotion.cpp). */
inline void SetClumpWorld(RpClump *clump, void *world) {
    void *current = fn_8046256C(clump);
    if (current != world) {
        if (current != 0) {
            fn_804623EC(current, clump);
        }
        fn_804622FC(world, clump);
    }
}

/* The model a homing shot carries (class name unknown): it follows the shot's position and
 * rotation. */
class UnkBulletModel_8005930C {
  public:
    UnkBulletModel_8005930C(int group, int id);
    ~UnkBulletModel_8005930C();
    void Update(const BulletPosition *state);

    /* 0x0 */ RpClump *m_clump;
    /* 0x4 */ RwFrame *m_frame;
};
SIZE_ASSERT(UnkBulletModel_8005930C, 0x8);

UnkBulletModel_8005930C::UnkBulletModel_8005930C(int group, int id) : m_frame(0) {
    m_clump = fn_8001277C(fn_800122D4(), group, id);
    if (m_clump != 0) {
        m_clump = fn_80457090(m_clump);
        m_frame = fn_804872D8(m_clump->frame);
        int area = fn_80169350(fn_80169364(m_clump));
        RpClump *clump = m_clump;
        SetClumpWorld(clump, fn_8003A840()->GetWorld(area));
    }
}

UnkBulletModel_8005930C::~UnkBulletModel_8005930C() {
    if (m_clump != 0) {
        void *world = fn_8046256C(m_clump);
        if (world != 0) {
            fn_804623EC(world, m_clump);
        }
        fn_80457468(m_clump);
        m_clump = 0;
        m_frame = 0;
    }
}

/* The RenderWare matrix of matrix (other units call it by its address). */
extern "C" inline RwMatrix fn_800591F0(const Mtx34f *matrix) {
    RwMatrix result;
    result.right.x = matrix->values[0][0];
    result.right.y = matrix->values[1][0];
    result.right.z = matrix->values[2][0];
    result.up.x = matrix->values[0][1];
    result.up.y = matrix->values[1][1];
    result.up.z = matrix->values[2][1];
    result.at.x = matrix->values[0][2];
    result.at.y = matrix->values[1][2];
    result.at.z = matrix->values[2][2];
    result.pos.x = matrix->values[0][3];
    result.pos.y = matrix->values[1][3];
    result.pos.z = matrix->values[2][3];
    result.flags = result.pad1 = result.pad2 = result.pad3 = 0;
    return result;
}

void UnkBulletModel_8005930C::Update(const BulletPosition *state) {
    RwFrame *frame = m_frame;
    if (frame != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        fn_8040B034(&translation, &state->m_position);
        fn_8040C148(&matrix, &translation);
        fn_8040B848(&matrix, state->m_rotation.y);
        fn_8040B884(&matrix, state->m_rotation.x);
        frame->modelling = fn_800591F0(&matrix);
        fn_8048722C(m_frame);
    }
}

namespace Weapon {
namespace Bullet {

BulletManager::Impl::Impl() {}

BulletManager::Impl::~Impl() {}

void BulletManager::Impl::Add(BulletBase *bullet) {
    m_bullets.push_back(bullet);
}

void BulletManager::Impl::Remove(BulletBase *bullet) {
    m_bullets.remove(bullet);
}

void BulletManager::Impl::DeleteAll() {
    /* Each bullet removes itself when it is deleted. */
    while (fn_80058F00(&m_bullets) != 0) {
        delete m_bullets.front();
    }
}

void BulletManager::Impl::Tick(float dt) {
    for (BulletList::iterator it = m_bullets.begin(); it != m_bullets.end();) {
        BulletBase *bullet = *it++;
        float time = dt;
        if (!bullet->m_owner.expired()) {
            /* The owner's frame time, through the pointer at 0x60 (meaning inferred). */
            time = **reinterpret_cast<float *const *>(bullet->m_owner.lock()->field_0x60);
        }
        bullet->Tick(time);
    }
}

BulletManager::BulletManager() : m_impl(new Impl) {}

BulletManager::~BulletManager() {}

void BulletManager::Add(BulletBase *bullet) {
    m_impl->Add(bullet);
}

void BulletManager::Remove(BulletBase *bullet) {
    m_impl->Remove(bullet);
}

} // namespace Bullet
} // namespace Weapon

/* Deletes every bullet (name inferred; other units call it by its address). */
extern "C" void fn_80058B38(BulletManager *manager) {
    manager->m_impl->DeleteAll();
}

namespace Weapon {
namespace Bullet {

class BulletManagerTask : public Task {
  public:
    BulletManagerTask(Task *parent);
    virtual ~BulletManagerTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(BulletManagerTask, 0x28);

BulletManagerTask::BulletManagerTask(Task *parent) : Task(parent) {
    m_name = "BulletManager";
    fn_800522A8();
}

BulletManagerTask::~BulletManagerTask() {
    fn_800522A8()->m_impl->DeleteAll();
}

void BulletManagerTask::Tick(float dt) {
    fn_800522A8()->m_impl->Tick(dt);
}

} // namespace Bullet
} // namespace Weapon

/* Creates the bullet manager's task under parent (name inferred; Game/StageManager.cpp calls it by
 * its address). */
extern "C" Task *fn_80058A04(Task *parent) {
    return new Weapon::Bullet::BulletManagerTask(parent);
}

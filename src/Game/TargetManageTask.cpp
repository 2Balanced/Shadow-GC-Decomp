/* TargetManageTask, TargetSearchList and TargetList (class names from RTTI): the targets weapons and
 * homing attacks aim at (Game/Target.h) are registered with the target manager behind fn_80060058
 * (class name unknown), which keeps them in a TargetList. A TargetSearchList is a copy of that list
 * that a search narrows down with predicates bound through boost::bind; the targets still selected
 * run from the start of the list to m_end. TargetManageTask draws the targets in the debug view.
 * File name from the task. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching (188 of 197 functions match):
 * - fn_801E7FB4, fn_801E81B0, fn_801E8218: retail copies the placeholder _1 before the vectors it
 *   binds by value. GC/2.7 evaluates inline calls in an argument list first, and the vector's copy
 *   constructor here is inline (it calls fn_800091EC). With an out-of-line copy constructor (a
 *   label at 0x800091EC) these three match, but then boost's value<T> constructor is expanded in the
 *   bind instances, which retail keeps out of line; both cannot hold under GC/2.7.
 * - IsNotOfKind (0x801E8E94), IsFacing (0x801E9014): retail loads the compared values, runs the
 *   shared_ptr destructors and compares afterwards; GC/2.7 compares before the destructors.
 * - StoreDistance (0x801E91D8): retail reads the first lock's pointer between the subtraction and
 *   fn_8040CF2C; GC/2.7 calls operator-> out of line or reads it earlier.
 * - HasFlags (0x801E8FBC), LacksFlags (0x801E8F5C): the operands of one and are swapped.
 * - fn_801E848C: retail loads 999999.9 straight into f31 on the null branch.
 * Forms tried for each are in the history of this unit; they look like the compiler differences of
 * docs/research-notes.md. */
#define MSL_ALGORITHM_FOR_EACH
#define MSL_ALGORITHM_REMOVE_IF
#define MSL_LIST_ACCESSORS
#define MSL_LIST_DEREF_DIRECT
#define MSL_LIST_RANGE_INSERT
#define PTRVECTOR_ASSIGN
#define TARGETSEARCHLIST_LAYOUT
#include <algorithm>
#include <list>

#include "Game/CharInfo.h"
#include "Game/DebugDisp.h"
#include "Game/DebugDraw.h"
#include "Game/MathTypes.h"
#include "Game/PtrVector.h"
#include "Game/Target.h"
#include "Game/TargetSearchList.h"
#include "Game/Task.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <boost/bind.hpp>
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" {
/* The normal of a vector, by value, and the dot product (see Game/WeaponLockonBase.cpp). */
Vec3f fn_8040CEE8(const Vec3f *vector);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The game's vector class as a bound argument (class name unknown): copied through the out-of-line
 * fn_800091EC. */
struct TargetVector {
    TargetVector(const TargetVector &from) {
        fn_800091EC(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&from));
    }

    float x;
    float y;
    float z;
};

/* A bound vector as the math routines take it. */
#define VEC(v) reinterpret_cast<const Vec3f *>(&(v))

/* The distance squared that fn_801E8218 stores in a target (field 0x4). */
#define TARGET_DISTANCE(target) (*reinterpret_cast<float *>(&(target)->field_0x4))

/* TargetList (class name from RTTI; vtable at 0x8053DC08): the targets the manager knows. */
class TargetList {
  public:
    typedef std::list<boost::weak_ptr<Target> > Targets;

    TargetList();
    virtual ~TargetList();

    /* 0x4 */ Targets m_targets;
};
SIZE_ASSERT(TargetList, 0x10);

/* The target manager (class name unknown; a function-local static, see Game/WeaponLockonBase.cpp). */
class UnkTargetManager_80060058 {
  public:
    UnkTargetManager_80060058();
    ~UnkTargetManager_80060058();

    /* 0x0 */ bool m_debugDraw;
    /* 0x4 */ TargetList m_list;
};
SIZE_ASSERT(UnkTargetManager_80060058, 0x14);

/* TargetManageTask (class name from RTTI): draws the targets in the debug view. */
class TargetManageTask : public Task {
  public:
    TargetManageTask(Task *parent) : Task(parent) {
        m_name = "TargetManager";
    }
    virtual ~TargetManageTask();
    virtual void Tick(float dt);
};
SIZE_ASSERT(TargetManageTask, 0x28);

typedef const boost::weak_ptr<Target> *TargetEntry;

extern "C" {
UnkTargetManager_80060058 *fn_80060058(void);
}

/* The predicates the searches bind (names inferred). */

/* Stores the distance squared from position in the target. */
static inline void SetDistance(const boost::shared_ptr<Target> &t, float distance) {
    TARGET_DISTANCE(t) = distance;
}

static void StoreDistance(TargetEntry target, const TargetVector &position) {
    SetDistance(target->lock(), fn_8040CF2C(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
        &(const Vec3f &)target->lock()->GetPosition(VEC(position)), VEC(position))));
}

/* Whether the target is not active. */
static bool IsInactive(TargetEntry target) {
    return !target->lock()->IsActive();
}

/* Whether the stored distance squared is beyond rangeSq. */
static bool IsBeyond(TargetEntry target, float rangeSq) {
    return TARGET_DISTANCE(target->lock()) > rangeSq;
}

/* Whether the target is set and position lies inside its cone (the set direction and the radius as
 * the cosine of the angle). */
static bool IsFacing(TargetEntry target, const TargetVector &position) {
    boost::shared_ptr<Target> locked(target->lock());
    if (!locked->IsPositionSet()) {
        return false;
    }
    Vec3f direction;
    fn_800091EC(&direction, &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                &(const Vec3f &)locked->GetPosition(VEC(position)), VEC(position))));
    return fn_8040CF90(&direction, &locked->m_setPosition) < locked->m_radius;
}

/* Whether the target has any of flags. */
static bool HasFlags(TargetEntry target, u32 flags) {
    return (target->lock()->m_flags & flags) != 0;
}

/* Whether the target has none of flags. */
static bool LacksFlags(TargetEntry target, u32 flags) {
    bool has = (target->lock()->m_flags & flags) != 0;
    return !has;
}

/* Whether the target's owner is not of kind (CharInfo field 0x5C). */
static bool IsNotOfKind(TargetEntry target, int kind) {
    boost::shared_ptr<Target> locked(target->lock());
    boost::shared_ptr<CharInfo> owner(locked->m_owner.lock());
    if (owner.get() != 0) {
        int ownerKind = owner->field_0x5C;
        return ownerKind != kind;
    }
    return true;
}

/* Whether the target lies outside the cone of cosine cosAngle about direction from position. */
static bool IsOutsideCone(TargetEntry target, const TargetVector &position, const TargetVector &direction,
                          float cosAngle) {
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                             &(const Vec3f &)target->lock()->GetPosition(VEC(position)), VEC(position)));
    return !(fn_8040CF90(VEC(direction), &(const Vec3f &)fn_8040CEE8(&offset)) > cosAngle);
}

/* Whether the target is other. */
static bool IsTarget(TargetEntry target, const boost::weak_ptr<Target> &other) {
    return target->lock() == other.lock();
}

/* Whether the target belongs to owner. */
static bool IsOwnedBy(TargetEntry target, const boost::shared_ptr<CharInfo> &owner) {
    boost::shared_ptr<Target> locked(target->lock());
    boost::shared_ptr<CharInfo> targetOwner(locked->m_owner.lock());
    if (targetOwner.get() != 0 && targetOwner == owner) {
        return true;
    }
    return false;
}

TargetList::TargetList() {}

TargetList::~TargetList() {}

TargetSearchList::TargetSearchList(const TargetList &list) {
    m_targets.reserve(list.m_targets.size());
    for (TargetList::Targets::const_iterator it = list.m_targets.begin(); it != list.m_targets.end(); it++) {
        m_targets.push_back(&*it);
    }
    m_end = m_targets.end();
}

TargetSearchList::TargetSearchList(const TargetSearchList &other) {
    m_targets.assign(other.Begin(), other.m_end);
    m_end = m_targets.end();
}

TargetSearchList::~TargetSearchList() {}

extern "C" {

/* The selected target nearest to position (name inferred). */
boost::weak_ptr<Target> fn_801E848C(TargetSearchList *list, const Vec3f *position) {
    boost::shared_ptr<Target> locked;
    if (list->m_targets.begin() == list->m_end) {
        return boost::weak_ptr<Target>();
    }
    TargetSearchList::iterator it = list->m_targets.begin();
    TargetEntry nearest = *it;
    locked = nearest->lock();
    float nearestDistance;
    if (locked.get() == 0) {
        nearestDistance = 999999.9f;
    } else {
        nearestDistance = fn_8040CF2C(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
            &(const Vec3f &)locked->GetPosition(position), position));
    }
    for (++it; it != list->m_end; ++it) {
        locked = (*it)->lock();
        if (locked.get() != 0) {
            float distance = fn_8040CF2C(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                &(const Vec3f &)locked->GetPosition(position), position));
            if (nearestDistance > distance) {
                nearestDistance = distance;
                nearest = *it;
            }
        }
    }
    return *nearest;
}

/* Keeps the targets within range of position (name inferred). */
void fn_801E8218(TargetSearchList *list, const Vec3f *position, float range) {
    std::for_each(list->m_targets.begin(), list->m_end, boost::bind(StoreDistance, _1, *reinterpret_cast<const TargetVector *>(position)));
    list->m_end = std::remove_if(list->m_targets.begin(), list->m_end, boost::bind(IsBeyond, _1, range * range));
}

/* Keeps the targets that face position (name inferred). */
void fn_801E81B0(TargetSearchList *list, const Vec3f *position) {
    list->m_end = std::remove_if(list->m_targets.begin(), list->m_end,
                                 boost::bind(IsFacing, _1, *reinterpret_cast<const TargetVector *>(position)));
}

/* Drops the targets with any of flags (name inferred). */
void fn_801E8138(TargetSearchList *list, int flags) {
    list->m_end = std::remove_if(list->m_targets.begin(), list->m_end, boost::bind(HasFlags, _1, (u32)flags));
}

/* Keeps the targets with any of flags (name inferred). */
void fn_801E80C0(TargetSearchList *list, int flags) {
    list->m_end = std::remove_if(list->m_targets.begin(), list->m_end, boost::bind(LacksFlags, _1, (u32)flags));
}

/* Keeps the targets whose owner is of kind (name inferred). */
void fn_801E8048(TargetSearchList *list, int kind) {
    list->m_end = std::remove_if(list->m_targets.begin(), list->m_end, boost::bind(IsNotOfKind, _1, kind));
}

/* Keeps the targets inside the cone of cosine cosAngle about direction from position (name
 * inferred). */
void fn_801E7FB4(TargetSearchList *list, const Vec3f *position, float cosAngle, const Vec3f *direction) {
    list->m_end = std::remove_if(list->m_targets.begin(), list->m_end,
                                 boost::bind(IsOutsideCone, _1, *reinterpret_cast<const TargetVector *>(position),
                                             *reinterpret_cast<const TargetVector *>(direction), cosAngle));
}

/* Drops target from the list (name inferred). */
void fn_801E7E48(TargetSearchList *list, const boost::weak_ptr<Target> &target) {
    list->m_end = std::remove_if(list->m_targets.begin(), list->m_end, boost::bind(IsTarget, _1, target));
}

/* Drops the targets of owner (name inferred). */
void fn_801E7C94(TargetSearchList *list, const boost::weak_ptr<CharInfo> &owner) {
    if (owner.expired()) {
        return;
    }
    boost::shared_ptr<CharInfo> locked(owner.lock());
    list->m_end = std::remove_if(list->m_targets.begin(), list->m_end, boost::bind(IsOwnedBy, _1, locked));
}

}

UnkTargetManager_80060058::UnkTargetManager_80060058() : m_debugDraw(false) {}

UnkTargetManager_80060058::~UnkTargetManager_80060058() {}

extern "C" {

/* Registers a target (name inferred; see Game/Target.cpp). */
void fn_801E7AF4(UnkTargetManager_80060058 *manager, boost::shared_ptr<Target> target) {
    if (!(target->m_flags & 0x800000)) {
        target->m_flags |= 0x800000;
        manager->m_list.m_targets.push_back(boost::weak_ptr<Target>(target));
    }
}

/* Drops the targets that no longer exist (name inferred). */
void fn_801E7A20(UnkTargetManager_80060058 *manager) {
    TargetList::Targets::iterator it = manager->m_list.m_targets.begin();
    while (it != manager->m_list.m_targets.end()) {
        TargetList::Targets::iterator current = it++;
        boost::shared_ptr<Target> locked(current->lock());
        if (locked.get() == 0) {
            manager->m_list.m_targets.erase(current);
        }
    }
}

/* A new list of the manager's active targets (name inferred). */
TargetSearchList *fn_801E7930(UnkTargetManager_80060058 *manager) {
    TargetSearchList *list = new TargetSearchList(manager->m_list);
    list->m_end = std::remove_if(list->m_targets.begin(), list->m_targets.end(), IsInactive);
    return list;
}

}

TargetManageTask::~TargetManageTask() {}

/* Draws a target as a sphere (name inferred). */
static void DrawTarget(boost::weak_ptr<Target> target) {
    UnkSphere_8004E6CC sphere;
    UnkColor_8040D2D8 color(0x80, 0xFF, 0xFF, 0xFF);
    fn_800091D0(&sphere.m_center, &(const Vec3f &)target.lock()->Position());
    sphere.m_radius = 11.0f;
    fn_802E0F3C(&sphere, (const UnkDrawColor_802E0E84 *)&color);
}

void TargetManageTask::Tick(float dt) {
    UnkTargetManager_80060058 *manager = fn_80060058();
    if (manager->m_debugDraw) {
        std::for_each(manager->m_list.m_targets.begin(), manager->m_list.m_targets.end(), DrawTarget);
    }
}

/* Creates the task (name inferred). */
extern "C" Task *fn_801E769C(Task *parent) {
    return new TargetManageTask(parent);
}

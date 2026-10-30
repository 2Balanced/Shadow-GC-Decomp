/* SonicteamUSA::System::PathData (class name from the RTTI of its boost::shared_ptr control block;
 * Game/PathData.h): a path's data, built from the path as loaded, and the links that join paths
 * into one (UnkPathLink_80017DBC, class name unknown), which the path manager keeps. This unit also
 * holds PathCtrl's constructor and assignment, the Euler-angle rotation matrix, and the game's
 * first copies of vector<void *> (constructor, destructor, fill-assignment), of the list of
 * pointers the game's managers use (list<void *>, written here as UnkPtrList_8001B5B8 with the C
 * names other units call it by), of list<shared_ptr<PathData> >, and of boost::function1's
 * assignment for the branch callback. File name from the class; other names are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first); the list and vector functions the game emits at the end of the unit are defined first. */
#define PATHDATA_CODE
#define PTRVECTOR_CORE
#define MSL_LIST_MERGE
#define MSL_LESS_CAST
#define MSL_FUNCTIONAL_BINDERS
#define MSL_BINDER_OUTLINE
#define BOOST_SP_LESS
#include <functional>
#include "Game/PathData.h"
#include "Game/PathCtrl.h"

#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::BranchCallBackParam;
using SonicteamUSA::System::JointPathData;
using SonicteamUSA::System::PathCtrl;
using SonicteamUSA::System::PathData;
using SonicteamUSA::System::PathList;
using SonicteamUSA::System::PathPoint;
using SonicteamUSA::System::PathPoints;
using SonicteamUSA::System::UnkJointList_80016BB4;
using SonicteamUSA::System::UnkPathLink_80017DBC;

typedef std::list<JointPathData> JointList;
typedef std::__list_iterator<JointPathData> JointIterator;
typedef boost::shared_ptr<PathData> PathPtr;

extern const float lbl_805FAD38; /* pi */
extern const Vec3f lbl_805E07A0;  /* the zero vector */
extern const Mtx34f lbl_805E0720; /* the identity */

extern "C" {
/* strncpy. */
char *fn_803ACABC(char *dst, const char *src, unsigned long size);
/* Allocation from a heap, and its release. */
void *fn_80049B90(u32 size, int heap);
void fn_80049B34(void *data);
/* Square root, dot product, squared length and squared distance. */
float fn_8000FEE4(float value);
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040CF2C(const Vec3f *v);
float fn_8040D180(const Vec3f *a, const Vec3f *b);
/* Rotations about z, y and x, a matrix copy and product, and the Euler angles of a matrix. */
Mtx34f fn_8040B0C0(float angle);
Mtx34f fn_8040B138(float angle);
Mtx34f fn_8040B1B0(float angle);
void fn_8040C148(Mtx34f *m, const Mtx34f *from);
void fn_8040BAC4(Mtx34f *m, const Mtx34f *other);
void fn_800493C4(Vec3f *rotation, const Mtx34f *matrix);

/* Game/PathCtrl.cpp: the shared copies of weak_ptr<PathData> and list<JointPathData> code, and
 * the PathCtrl functions this unit calls. */
bool fn_80016980(const boost::weak_ptr<PathData> *path);
PathPtr fn_80016718(const boost::weak_ptr<PathData> *path);
u32 fn_80016A34(const JointList *list);
JointIterator fn_80016D84(JointList *list);
JointIterator fn_80016DB8(JointIterator *it, int);
JointIterator fn_80016E18(JointList *list);
void fn_8001725C(PathCtrl *path, u32 index, bool stay);
UnkPathLink_80017DBC *fn_80017DBC(PathCtrl *path);
Vec3f fn_80018434(PathCtrl *path, bool interpolate, bool unused);
/* The link's destructor (below). */
UnkPathLink_80017DBC *fn_8001B280(UnkPathLink_80017DBC *link, short flag);
/* The box's constructor (below). */
struct UnkVecPair_8001C4B8 *fn_8001C4B8(struct UnkVecPair_8001C4B8 *pair, const Vec3f *a, const Vec3f *b);
/* A point of the path (below). */
PathPoint *fn_8001974C(PathData *data, int index);
}

namespace ret {
/* Game/PathCtrl.cpp's fn_800169F8 as a function returning the vector. */
extern "C" Fn8040CopyVector3 fn_800169F8(const Vec3f *v);
} // namespace ret

/* The game's one shared copy of std::swap for boost::function's any_pointer, which other units
 * declare (see Game/CatchStick.cpp) and function1::swap calls: an overload, defined as a friend of
 * a class template so that it comes out where the template instance would. */
namespace std {
void swap(boost::detail::function::any_pointer &a, boost::detail::function::any_pointer &b);

template <class T> struct __any_pointer_swap {
    friend void swap(T &a, T &b) {
        T tmp = a;
        a = b;
        b = tmp;
    }
};
template struct __any_pointer_swap<boost::detail::function::any_pointer>;
} // namespace std

/* list<shared_ptr<PathData> >'s members that the game emits at the end of the unit: written as
 * explicit specializations (bodies as in <list>), so that they come out in the game's order after
 * the other functions defined after the class. */
namespace std {
template <> void __list_imp<boost::shared_ptr<PathData> >::do_splice(iterator position, __list_imp &x);
template <> __list_imp<boost::shared_ptr<PathData> >::__list_imp();
template <>
__list_imp<boost::shared_ptr<PathData> >::iterator
__list_imp<boost::shared_ptr<PathData> >::insert(iterator position, const boost::shared_ptr<PathData> &x);
template <>
__list_imp<boost::shared_ptr<PathData> >::iterator
__list_imp<boost::shared_ptr<PathData> >::erase(iterator first, iterator last);
} // namespace std

/* An empty inline function: calling it keeps the caller out of line (deferred inlining is one
 * level deep). */
inline void PathKeep() {}

/* ---- The game's list of pointers (list<void *>; its functions keep their C names, which other
 * units call them by: see Game/BulletManagerTask.cpp, Game/ElecSecurityCode.h). ---- */

struct UnkPtrListLink_8001B5B8 {
    /* 0x0 */ UnkPtrListLink_8001B5B8 *prev;
    /* 0x4 */ UnkPtrListLink_8001B5B8 *next;
};

struct UnkPtrListNode_8001B5B8 : public UnkPtrListLink_8001B5B8 {
    /* 0x8 */ void *value;
};

/* A position in the list. */
struct UnkPtrListPosition_8001B674 {
    UnkPtrListPosition_8001B674() {}
    explicit UnkPtrListPosition_8001B674(UnkPtrListLink_8001B5B8 *n) : node(n) {}
    UnkPtrListPosition_8001B674(const UnkPtrListPosition_8001B674 &other) : node(other.node) {}

    void **operator->() const {
        PathKeep();
        return &static_cast<UnkPtrListNode_8001B5B8 *>(node)->value;
    }

    /* 0x0 */ UnkPtrListLink_8001B5B8 *node;
};

/* The (empty) allocator of the nodes. */
struct UnkPtrListNodeAlloc_8001B5B8 {};

struct UnkPtrList_8001B5B8 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkPtrListLink_8001B5B8 m_end;
};
SIZE_ASSERT(UnkPtrList_8001B5B8, 0xC);

typedef UnkPtrList_8001B5B8 PtrList;
typedef UnkPtrListPosition_8001B674 PtrListPos;
typedef std::binder2nd<std::equal_to<void *> > PtrListMatch;

extern "C" {
PtrListPos fn_8001BD24(PtrList *list, PtrListPos first, PtrListPos last);
PtrList *fn_8001BE04(PtrList *list);
void fn_8001B83C(PtrList *list, PtrListMatch pred);
void fn_8001B960(PathList *list, PathList *other, std::less<PathPtr> comp);
}

/* The list's storage: the size, and the end node (which links the first and the last node). */
extern "C" inline u32 &fn_8001BDE8(PtrList *list) {
    PathKeep();
    return list->m_size;
}

extern "C" inline UnkPtrListNodeAlloc_8001B5B8 &fn_8001BDEC(PtrList *list) {
    PathKeep();
    return *reinterpret_cast<UnkPtrListNodeAlloc_8001B5B8 *>(&list->m_end);
}

/* The value allocator: list<void *>'s own alloc(), placed here as an explicit specialization. */
namespace std {
template <> inline __list_imp<void *>::value_allocator &__list_imp<void *>::alloc() {
    PathKeep();
    return *reinterpret_cast<value_allocator *>(this);
}
} // namespace std

extern "C" inline UnkPtrListLink_8001B5B8 *fn_8001B6A8(PtrList *list) {
    PathKeep();
    return &list->m_end;
}

extern "C" inline void *fn_8001BEC0(u32 *size, u32 n) {
    PathKeep();
    *size = n;
    return size;
}

extern "C" inline void *fn_8001BE90(u32 *size, u32 n) {
    fn_8001BEC0(size, n);
    return size;
}

extern "C" inline void *fn_8001BE8C(void *alloc, const UnkPtrListNodeAlloc_8001B5B8 *from) {
    PathKeep();
    return alloc;
}

extern "C" inline void *fn_8001BE5C(void *end, const UnkPtrListNodeAlloc_8001B5B8 *alloc) {
    fn_8001BE8C(end, alloc);
    return end;
}

/* The first and the end position. */
extern "C" inline PtrListPos fn_8001B674(PtrList *list) {
    return PtrListPos(fn_8001B6A8(list));
}

extern "C" inline PtrListPos fn_8001B6B0(PtrList *list) {
    return PtrListPos(fn_8001B6A8(list)->next);
}

/* The destructors of the list's layers. */
extern "C" inline PtrList *fn_8001B604(PtrList *list, short flag) {
    if (list != 0) {
        fn_8001BD24(list, fn_8001B6B0(list), fn_8001B674(list));
        if (flag > 0) {
            operator delete(list);
        }
    }
    return list;
}

extern "C" inline PtrList *fn_8001B5B8(PtrList *list, short flag) {
    if (list != 0) {
        fn_8001B604(list, 0);
        if (flag > 0) {
            operator delete(list);
        }
    }
    return list;
}

/* Removes value (the list of the game's pointers, and that of links). NonMatching: the game's binder
 * constructor (fn_8001B480) loads the operation into r0 and the value into r4, GC/2.7 the other way
 * round; and GC/2.7 also emits an unused local copy of bind2nd after this function, which the
 * linker keeps. */
extern "C" inline void fn_8001B42C(PtrList *list, void *const &value) {
    fn_8001B83C(list, std::bind2nd(std::equal_to<void *>(), value));
}

extern "C" inline void fn_8001B40C(PtrList *list, UnkPathLink_80017DBC *const &link) {
    fn_8001B42C(list, reinterpret_cast<void *const &>(link));
}

/* ---- Statics ---- */

/* A box of two corners (struct name unknown; Game/EnemyPath.cpp). Its constructor is
 * fn_8001C4B8 (below), which other units call by its address: the box at the origin is
 * initialized through it, with a constructor that takes the box built in place. */
struct UnkVecPair_8001C4B8 {
    explicit UnkVecPair_8001C4B8(UnkVecPair_8001C4B8 *) {}

    /* 0x00 */ float a[3];
    /* 0x0C */ float b[3];
};

/* The box at the origin (Game/EnemyPath.cpp). */
UnkVecPair_8001C4B8 lbl_80570B50(fn_8001C4B8(&lbl_80570B50, &lbl_805E07A0, &lbl_805E07A0));

/* The sound manager's instance (Game/BulletCannon.cpp, Game/SoundManager.h): this unit emits the
 * inline function's weak statics first. Only its constructor is seen here, so the static gets a
 * guard and no destructor record, as in the game. */
class UnkSound_80053164 {
  public:
    UnkSound_80053164();

    /* 0x00 */ u8 field_0x0[0x1C];
};

inline UnkSound_80053164 &SoundManagerObject() {
    static UnkSound_80053164 s_object;
    return s_object;
}

/* The links of the path manager: the list of pointers with a layer of its own (names inferred). */
class UnkPathLinkList_8001B56C : public PtrList {
  public:
    UnkPathLinkList_8001B56C() {
        PathKeep();
        fn_8001BE04(this);
    }
    ~UnkPathLinkList_8001B56C() {
        fn_8001B5B8(this, 0);
    }
};

/* The path manager (class name unknown): the links. */
class UnkPathManager_8001B494 {
  public:
    /* 0x0 */ UnkPathLinkList_8001B56C m_links;
};

/* The path manager made on first use (Game/PathCtrl.cpp holds the pointer). */
extern UnkPathManager_8001B494 *g_pathManager;

extern "C" inline UnkPathManager_8001B494 *fn_8001B4C8(void) {
    static UnkPathManager_8001B494 s_manager;
    return &s_manager;
}

extern "C" inline UnkPathManager_8001B494 *fn_8001B494(void) {
    if (g_pathManager == 0) {
        g_pathManager = fn_8001B4C8();
    }
    return g_pathManager;
}

/* Instance pointers of objects made on first use (types unknown; see Game/PlayerBase.cpp and
 * Game/BulletCannon.cpp). */
void *lbl_805EE014;
void *lbl_805EE018;
void *lbl_805EE01C;

struct UnkObject_80079808;

template <class T> struct Instance {
    static T *s_instance;
};

template <class T> T *Instance<T>::s_instance;
template struct Instance<UnkObject_80079808>;

/* The box's constructor (the game's C++ constructor of the box, which calls the vector's
 * out-of-line copy constructor), written as a friend of a class template: GC/2.7 keeps it out of
 * line after its first caller, as the game does. */
template <class T> struct UnkVecPairCtor_8001C4B8 {
    friend T *fn_8001C4B8(T *pair, const Vec3f *a, const Vec3f *b) {
        PathKeep();
        fn_800091EC(reinterpret_cast<Vec3f *>(pair->a), a);
        fn_800091EC(reinterpret_cast<Vec3f *>(pair->b), b);
        return pair;
    }
};
template struct UnkVecPairCtor_8001C4B8<UnkVecPair_8001C4B8>;

/* ---- Functions the game emits at the end of the unit ---- */

namespace std {

template <> void __list_imp<boost::shared_ptr<PathData> >::do_splice(iterator position, __list_imp &x) {
    if (x.sz() != 0) {
        if (alloc() == x.alloc()) {
        }
        __list_node_base *p;
        __list_node_base *t = x.tail();
        __list_node_base *f = t->next_;
        __list_node_base *l = t->prev_;
        t->prev_ = t->next_ = t;
        p = position.node_;
        p->prev_->next_ = f;
        f->prev_ = p->prev_;
        p->prev_ = l;
        l->next_ = p;
        sz() += x.sz();
        x.sz() = 0;
    }
}

template <>
__list_imp<boost::shared_ptr<PathData> >::__list_imp() : size_(0), tail_(node_allocator(alloc())) {
    __list_node_base *t = tail();
    t->prev_ = t->next_ = t;
}

template <>
__list_imp<boost::shared_ptr<PathData> >::iterator
__list_imp<boost::shared_ptr<PathData> >::insert(iterator position, const boost::shared_ptr<PathData> &x) {
    __list_holder<node, node_allocator, __list_node_deleter<node> > h(
        node_alloc().allocate(1), node_alloc(), __list_node_deleter<node>());
    alloc().construct(&h->data_, x);
    node *p = h.release();
    __list_node_base *n = position.node_;
    n->prev_->next_ = p;
    p->prev_ = n->prev_;
    n->prev_ = p;
    p->next_ = n;
    ++sz();
    return iterator(p);
}

template <>
__list_imp<boost::shared_ptr<PathData> >::iterator
__list_imp<boost::shared_ptr<PathData> >::erase(iterator first, iterator last) {
    if (first == last) {
        return last;
    }
    __list_node_base *lp = last.node_->prev_;
    __list_node_base *fp = first.node_;
    fp->prev_->next_ = lp->next_;
    lp->next_->prev_ = fp->prev_;
    while (first != last) {
        alloc().destroy(&static_cast<node *>(first.node_)->data_);
        __list_node_base *p = first.node_;
        ++first;
        node_alloc().deallocate(static_cast<node *>(p), 1);
        --sz();
    }
    return last;
}

} // namespace std

/* The list of pointers' storage constructor. */
extern "C" PtrList *fn_8001BE04(PtrList *list) {
    UnkPtrListNodeAlloc_8001B5B8 alloc;
    fn_8001BE90(&list->m_size, 0);
    reinterpret_cast<std::__list_imp<void *> *>(list)->alloc();
    fn_8001BE5C(&list->m_end, &alloc);
    UnkPtrListLink_8001B5B8 *t = fn_8001B6A8(list);
    t->prev = t->next = t;
    return list;
}

/* Erases [first, last). */
extern "C" PtrListPos fn_8001BD24(PtrList *list, PtrListPos first, PtrListPos last) {
    if (first.node == last.node) {
        return last;
    }
    UnkPtrListLink_8001B5B8 *lp = last.node->prev;
    UnkPtrListLink_8001B5B8 *fp = first.node;
    fp->prev->next = lp->next;
    lp->next->prev = fp->prev;
    while (first.node != last.node) {
        first.operator->();
        reinterpret_cast<std::__list_imp<void *> *>(list)->alloc();
        UnkPtrListLink_8001B5B8 *p = first.node;
        first.node = first.node->next;
        fn_8001BDEC(list);
        operator delete(p);
        --fn_8001BDE8(list);
    }
    return last;
}

/* Fills the vector with n copies of x. */
void std::vector<void *, std::allocator<void *> >::assign(size_t n, void *const &x) {
    if (n > capacity()) {
        if (n > max_size()) {
            __msl_error("vector::assign length error\n");
        }
        m_size = 0;
        if (m_data != 0) {
            m_cap.first().deallocate(data(), m_cap.second());
            m_data = 0;
            m_cap.second() = 0;
        }
        m_data = m_cap.first().allocate(n, 0);
        m_cap.second() = n;
    }
    fill_n(data(), n, x);
    m_size = n;
}

/* Merges other into the list (both in comp's order); what is left of other goes to the end. */
extern "C" void fn_8001B960(PathList *list, PathList *other, std::less<PathPtr> comp) {
    if (list != other) {
        if (list->alloc() == other->alloc()) {
        }
        PathList::iterator first1 = list->begin();
        PathList::iterator last1 = list->end();
        PathList::iterator first2 = other->begin();
        PathList::iterator last2 = other->end();
        while (first1 != last1 && first2 != last2) {
            if (comp(first2.node_->data_, first1.node_->data_)) {
                PathList::iterator j = first2;
                size_t n = 1;
                for (++j; j != last2; ++j, ++n) {
                    if (!comp(j.node_->data_, first1.node_->data_)) {
                        break;
                    }
                }
                std::__list_node_base *p = first1.node_;
                std::__list_node_base *f = first2.node_;
                std::__list_node_base *l = j.node_->prev_;
                f->prev_->next_ = l->next_;
                l->next_->prev_ = f->prev_;
                p->prev_->next_ = f;
                f->prev_ = p->prev_;
                p->prev_ = l;
                l->next_ = p;
                other->sz() -= n;
                list->sz() += n;
                first2 = j;
            }
            ++first1;
        }
        if (first2 != last2) {
            list->do_splice(first1, *other);
        }
    }
}

/* Removes the values pred holds for. */
extern "C" void fn_8001B83C(PtrList *list, PtrListMatch pred) {
    PtrListPos e = fn_8001B674(list);
    for (PtrListPos i = fn_8001B6B0(list); i.node != e.node; i.node = i.node->next) {
        if (pred(static_cast<UnkPtrListNode_8001B5B8 *>(i.node)->value)) {
            PtrListPos j = i;
            for (j.node = j.node->next; j.node != e.node; j.node = j.node->next) {
                if (!pred(static_cast<UnkPtrListNode_8001B5B8 *>(j.node)->value)) {
                    break;
                }
            }
            i = fn_8001BD24(list, i, j);
            if (i.node == e.node) {
                break;
            }
        }
    }
}

/* ---- The links ---- */

/* Merges other's paths into the link's (by their control blocks). */
extern "C" inline void fn_8001AE14(PathList *list, PathList *other) {
    std::less<PathPtr> comp;
    PathKeep();
    fn_8001B960(list, other, comp);
}

/* Adds a joint to path in a list of joints. */
inline void AddJoint(UnkJointList_80016BB4 *list, const boost::weak_ptr<PathData> &path, bool atStart,
                     bool flip) {
    JointPathData joint;
    joint.m_path = path;
    joint.m_atStart = atStart;
    joint.m_flip = flip;
    list->push_back(joint);
}

/* NonMatching: the game copies the bounds through the vector's out-of-line copy constructor
 * (fn_800091EC), reading the path through an inlined operator-> for each; with the inline copy
 * constructor of Fn8040CopyVector3, GC/2.7 calls operator-> out of line instead (its result feeds an
 * inline call), and no source form found reloads the path pointer as the game does. */
UnkPathLink_80017DBC::UnkPathLink_80017DBC(const PathPtr &path)
    : m_length(path->m_points->m_length), m_max(path->m_max), m_min(path->m_min), m_first(path),
      m_last(path) {
    m_branched = false;
    path->m_link = this;
    m_paths.push_back(path);
    if (path->m_points->m_name != 0) {
        fn_803ACABC(m_name, path->m_points->m_name, 0x13);
        m_name[0x13] = 0;
    }
}

/* The link's destructor (the game's frees it with fn_80049B34, which a C++ destructor cannot
 * call as the class's operator delete under GC/2.7, so it is written as a C function). */
extern "C" UnkPathLink_80017DBC *fn_8001B280(UnkPathLink_80017DBC *link, short flag) {
    if (link != 0) {
        fn_8001B40C(&fn_8001B494()->m_links, link);
        link->m_paths.~PathList();
        link->m_last.~weak_ptr();
        link->m_first.~weak_ptr();
        if (flag > 0) {
            fn_80049B34(link);
        }
    }
    return link;
}

/* The squared length of (x, y, z) (the squares are not fused into multiply-adds). */
inline float DistanceSq(float x, float y, float z) {
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    return zz + (xx + yy);
}

/* Joins path to the paths of the link whose ends meet it (within 1); true if it was joined. */
bool UnkPathLink_80017DBC::Connect(PathPtr path) {
    PathPoints *points = path->m_points;
    PathPoint *first = fn_8001974C(path.get(), 0);
    PathPoint *last = fn_8001974C(path.get(), points->m_count - 1);
    for (PathList::iterator it = m_paths.begin(); it != m_paths.end(); it++) {
        if (path.get() != it.operator->()->get()) {
            PathPtr other(*it.operator->());
            points = other->m_points;
            if (!other->field_0x51) {
                PathPoint *p = fn_8001974C(other.get(), 0);
                float dx = p->m_position.x - first->m_position.x;
                float dy = p->m_position.y - first->m_position.y;
                float dz = p->m_position.z - first->m_position.z;
                if (DistanceSq(dx, dy, dz) <= 1.0f) {
                    Join(other, path, true, true);
                }
                dx = p->m_position.x - last->m_position.x;
                dy = p->m_position.y - last->m_position.y;
                dz = p->m_position.z - last->m_position.z;
                if (DistanceSq(dx, dy, dz) <= 1.0f) {
                    Join(other, path, true, false);
                }
            }
            if (!other->field_0x52) {
                PathPoint *p = fn_8001974C(other.get(), points->m_count - 1);
                float dx = p->m_position.x - first->m_position.x;
                float dy = p->m_position.y - first->m_position.y;
                float dz = p->m_position.z - first->m_position.z;
                if (DistanceSq(dx, dy, dz) <= 1.0f) {
                    Join(other, path, false, false);
                }
                dx = p->m_position.x - last->m_position.x;
                dy = p->m_position.y - last->m_position.y;
                dz = p->m_position.z - last->m_position.z;
                if (DistanceSq(dx, dy, dz) <= 1.0f) {
                    Join(other, path, false, true);
                }
            }
        }
    }
    return fn_80016A34(&path->m_startJoints) != 0 || fn_80016A34(&path->m_endJoints) != 0;
}

/* Joins path to other (at other's start or end; flip when the paths run against each other):
 * adds the joints, then takes in path's link, or path itself, and widens the bounds.
 * NonMatching: in the three tests of the largest coordinates the game holds the link's value in
 * f1 and the path's in f0; GC/2.7 swaps them (every other instruction matches). */
void UnkPathLink_80017DBC::Join(PathPtr other, PathPtr path, bool atStart, bool flip) {
    if (atStart) {
        if (fn_80016A34(&other->m_startJoints) != 0) {
            m_branched = true;
        }
        bool linked = true;
        if (fn_80016A34(&path->m_startJoints) == 0 && fn_80016A34(&path->m_endJoints) == 0) {
            linked = false;
        }
        if (flip) {
            AddJoint(&path->m_startJoints, other, true, flip);
            AddJoint(&other->m_startJoints, path, true, flip);
        } else {
            AddJoint(&path->m_endJoints, other, true, false);
            AddJoint(&other->m_startJoints, path, false, false);
        }
        if (linked) {
            UnkPathLink_80017DBC *link = path->m_link;
            if (link != this) {
                bool first = fn_80016718(&m_first).get() == other.get();
                if (first) {
                    m_first = link->m_first;
                }
                m_length += link->m_length;
                fn_8001AE14(&m_paths, &link->m_paths);
                for (PathList::iterator it = m_paths.begin(); it != m_paths.end(); it++) {
                    it.operator->()->get()->m_link = this;
                }
                link->m_first.reset();
                link->m_last.reset();
                fn_8001B280(link, 1);
            }
        } else {
            m_length += path->m_points->m_length;
            path->m_link = this;
            m_paths.push_back(path);
            if (fn_80016718(&m_first).get() == other.get()) {
                m_first = path;
            }
        }
    } else {
        if (fn_80016A34(&other->m_endJoints) != 0) {
            m_branched = true;
        }
        bool linked = true;
        if (fn_80016A34(&path->m_startJoints) == 0 && fn_80016A34(&path->m_endJoints) == 0) {
            linked = false;
        }
        if (flip) {
            AddJoint(&path->m_endJoints, other, false, flip);
            AddJoint(&other->m_endJoints, path, false, flip);
        } else {
            AddJoint(&path->m_startJoints, other, false, false);
            AddJoint(&other->m_endJoints, path, true, false);
        }
        if (linked) {
            UnkPathLink_80017DBC *link = path->m_link;
            if (link != this) {
                bool last = fn_80016718(&m_last).get() == other.get();
                if (last) {
                    m_last = link->m_last;
                }
                m_length += link->m_length;
                fn_8001AE14(&m_paths, &link->m_paths);
                for (PathList::iterator it = m_paths.begin(); it != m_paths.end(); it++) {
                    it.operator->()->get()->m_link = this;
                }
                link->m_first.reset();
                link->m_last.reset();
                fn_8001B280(link, 1);
            }
        } else {
            m_length += path->m_points->m_length;
            path->m_link = this;
            m_paths.push_back(path);
            if (fn_80016718(&m_last).get() == other.get()) {
                m_last = path;
            }
        }
    }
    if (path->m_min.x < m_min.x) {
        m_min.x = path->m_min.x;
    }
    if (path->m_min.y < m_min.y) {
        m_min.y = path->m_min.y;
    }
    if (path->m_min.z < m_min.z) {
        m_min.z = path->m_min.z;
    }
    if (m_max.x < path->m_max.x) {
        m_max.x = path->m_max.x;
    }
    if (m_max.y < path->m_max.y) {
        m_max.y = path->m_max.y;
    }
    if (m_max.z < path->m_max.z) {
        m_max.z = path->m_max.z;
    }
}

/* Vectors differ. */
inline bool Differ(const Vec3f &a, const Vec3f &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

/* Puts path on the point of the link's paths nearest position (within radius when it is not
 * zero); outDirection tells whether direction runs against the path there. */
extern "C" bool fn_8001A0EC(UnkPathLink_80017DBC *link, PathCtrl *path, const Vec3f *position,
                            const Vec3f *direction, int *outDirection, float radius) {
    PathPtr best;
    float bestT = 0.0f;
    u32 bestIndex = 0;
    UnkPathLink_80017DBC *pathLink = fn_80017DBC(path);
    float range = radius * radius;
    if (0.0f == range) {
        range = -1.0f;
    }
    if (pathLink != 0) {
        if (-1.0f == range ||
            (pathLink->m_min.x - radius <= position->x && pathLink->m_min.y - radius <= position->y &&
             pathLink->m_min.z - radius <= position->z && position->x <= pathLink->m_max.x + radius &&
             position->y <= pathLink->m_max.y + radius && position->z <= pathLink->m_max.z + radius)) {
            for (PathList::iterator it = link->m_paths.begin(); it != link->m_paths.end(); it++) {
                PathPtr data(*it.operator->());
                if (-1.0f == range ||
                    (data->m_min.x - radius <= position->x && data->m_min.y - radius <= position->y &&
                     data->m_min.z - radius <= position->z && position->x <= radius + data->m_max.x &&
                     position->y <= radius + data->m_max.y && position->z <= radius + data->m_max.z)) {
                    PathPoints *points = data->m_points;
                    for (int i = 0; i < (int)points->m_count - 1; i++) {
                        PathPoint *a = &points->m_points[i];
                        PathPoint *b = &points->m_points[i + 1];
                        if (-1.0f != range) {
                            u32 flags = a->field_0x1C;
                            if (flags & 1) {
                                if (!(b->m_position.x - radius <= position->x &&
                                      position->x <= a->m_position.x + radius)) {
                                    continue;
                                }
                            } else if (!(a->m_position.x - radius <= position->x &&
                                         position->x <= b->m_position.x + radius)) {
                                continue;
                            }
                            if (flags & 2) {
                                if (!(b->m_position.y - radius <= position->y &&
                                      position->y <= a->m_position.y + radius)) {
                                    continue;
                                }
                            } else if (!(a->m_position.y - radius <= position->y &&
                                         position->y <= b->m_position.y + radius)) {
                                continue;
                            }
                            if (flags & 4) {
                                if (!(b->m_position.z - radius <= position->z &&
                                      position->z <= a->m_position.z + radius)) {
                                    continue;
                                }
                            } else if (!(a->m_position.z - radius <= position->z &&
                                         position->z <= b->m_position.z + radius)) {
                                continue;
                            }
                        }
                        Vec3f segment(b->m_position.x - a->m_position.x, b->m_position.y - a->m_position.y,
                                 b->m_position.z - a->m_position.z);
                        Vec3f offset(position->x - a->m_position.x, position->y - a->m_position.y,
                                 position->z - a->m_position.z);
                        float dot = fn_8040CF90(&offset, &segment);
                        float length = fn_8040CF90(&segment, &segment);
                        if (0.0f <= dot && dot <= length) {
                            float t = dot / length;
                            Vec3f point(segment.x * t + a->m_position.x, segment.y * t + a->m_position.y,
                                     segment.z * t + a->m_position.z);
                            float distance = fn_8040D180(position, &point);
                            if (-1.0f == range || distance <= range) {
                                best = data;
                                bestT = t;
                                bestIndex = i;
                                if (-1.0f == range) {
                                    radius = fn_8000FEE4(distance);
                                }
                                range = distance;
                            }
                        } else if (dot < 0.0f) {
                            Vec3f d(a->m_position.x - position->x, a->m_position.y - position->y,
                                     a->m_position.z - position->z);
                            float distance = fn_8040CF2C(&d);
                            if (-1.0f == range || distance <= range) {
                                best = data;
                                bestIndex = i;
                                bestT = 0.0f;
                                if (-1.0f == range) {
                                    radius = fn_8000FEE4(distance);
                                }
                                range = distance;
                            }
                        } else if (i == (int)points->m_count - 2) {
                            Vec3f d(b->m_position.x - position->x, b->m_position.y - position->y,
                                     b->m_position.z - position->z);
                            float distance = fn_8040CF2C(&d);
                            if (-1.0f == range || distance <= range) {
                                best = data;
                                bestIndex = i;
                                bestT = 1.0f;
                                if (-1.0f == range) {
                                    radius = fn_8000FEE4(distance);
                                }
                                range = distance;
                            }
                        }
                    }
                }
            }
        }
        if (best) {
            path->m_path = best;
            path->m_index = bestIndex;
            path->m_t = bestT;
            if (Differ(*direction, lbl_805E07A0) && outDirection != 0) {
                PathPoint *a = &best->m_points->m_points[bestIndex];
                PathPoint *b = &best->m_points->m_points[bestIndex + 1];
                Vec3f segment(b->m_position.x - a->m_position.x, b->m_position.y - a->m_position.y,
                              b->m_position.z - a->m_position.z);
                if (fn_8040CF90(&segment, direction) >= 0.0f) {
                    *outDirection = 0;
                } else {
                    *outDirection = 1;
                }
            }
            return true;
        }
        return false;
    }
    return false;
}

/* Sets each path's offset in the link. */
void UnkPathLink_80017DBC::UpdateOffsets() {
    float offset = 0.0f;
    for (PathList::iterator it = m_paths.begin(); it != m_paths.end(); it++) {
        PathData *data = it.operator->()->get();
        data->m_linkOffset = offset;
        offset += data->m_points->m_length;
    }
}

/* Sets the rotation of every joint of the link's paths. */
void UnkPathLink_80017DBC::UpdateJoints() {
    for (PathList::iterator it = m_paths.begin(); it != m_paths.end(); it++) {
        PathPtr path(*it.operator->());
        JointIterator j;
        for (j = fn_80016E18(&path->m_startJoints); j != fn_80016D84(&path->m_startJoints);
             fn_80016DB8(&j, 0)) {
            UpdateJoint(path, j.operator->(), true);
        }
        for (j = fn_80016E18(&path->m_endJoints); j != fn_80016D84(&path->m_endJoints);
             fn_80016DB8(&j, 0)) {
            UpdateJoint(path, j.operator->(), false);
        }
    }
}

/* Sets a joint's rotation: the turn from path's direction at the joint to that of the path the
 * joint leads to. */
void UnkPathLink_80017DBC::UpdateJoint(PathPtr path, JointPathData *joint, bool atStart) {
    PathCtrl ctrl;
    ctrl.m_path = path;
    if (atStart) {
        fn_8001725C(&ctrl, 0, true);
    } else {
        fn_8001725C(&ctrl, path->m_points->m_count - 1, true);
    }
    Vec3f from = fn_80018434(&ctrl, false, false);
    if (!atStart) {
        from.y += lbl_805FAD38;
        if (from.y > lbl_805FAD38) {
            from.y -= 2.0f * lbl_805FAD38;
        }
    }
    ctrl.m_path = joint->m_path;
    ctrl.m_t = 0.0f;
    bool away = !atStart;
    if (away ^ joint->m_flip) {
        fn_8001725C(&ctrl, 0, true);
    } else {
        fn_8001725C(&ctrl, fn_80016718(&ctrl.m_path)->m_points->m_count - 1, true);
    }
    Vec3f to = fn_80018434(&ctrl, false, false);
    if (away ^ joint->m_flip) {
        to.y += lbl_805FAD38;
        if (to.y > lbl_805FAD38) {
            to.y -= 2.0f * lbl_805FAD38;
        }
    }
    Mtx34f matrix;
    fn_8040C148(&matrix, &(const Mtx34f &)fn_8040B138(-from.y));
    fn_8040BAC4(&matrix, &(const Mtx34f &)fn_8040B1B0(-from.x));
    fn_8040BAC4(&matrix, &(const Mtx34f &)fn_8040B0C0(-from.z));
    fn_8040BAC4(&matrix, &(const Mtx34f &)fn_8040B0C0(to.z));
    fn_8040BAC4(&matrix, &(const Mtx34f &)fn_8040B1B0(to.x));
    fn_8040BAC4(&matrix, &(const Mtx34f &)fn_8040B138(to.y));
    fn_800493C4(&joint->m_rotation, &matrix);
}

void *UnkPathLink_80017DBC::operator new(size_t size) {
    return fn_80049B90(size, 2);
}

/* ---- Paths ---- */

/* NonMatching: the game loads both flag words before storing them (into r4 and r0); GC/2.7 loads
 * and stores them one after the other. */
PathData::PathData(PathPoints *points, bool ownsPoints)
    : m_flags(points->m_flags), field_0x4(points->field_0x1C),
      m_max(ret::fn_800169F8(&points->m_max)), m_min(ret::fn_800169F8(&points->m_min)),
      m_points(points), m_link(0), m_ownsPoints(ownsPoints), field_0x51(0), field_0x52(0) {}

/* The path's destructor (a C function for the reason given at fn_8001B280). */
extern "C" PathData *fn_8001977C(PathData *data, short flag) {
    if (data != 0) {
        if (data->m_ownsPoints) {
            fn_80049B34(data->m_points);
        }
        data->m_endJoints.~UnkJointList_80016BB4();
        data->m_startJoints.~UnkJointList_80016BB4();
        data->m_distances.~__podvec<float>();
        if (flag > 0) {
            fn_80049B34(data);
        }
    }
    return data;
}

/* A point of the path (null out of range; Game/PathCtrl.cpp calls it by its address). */
extern "C" PathPoint *fn_8001974C(PathData *data, int index) {
    PathPoints *points = data->m_points;
    int count = points->m_count;
    if (index >= 0 && index < count) {
        return &points->m_points[index];
    }
    return 0;
}

/* Sets the distance of each point from the path's start. */
void PathData::UpdateDistances() {
    m_distances.assign(m_points->m_count, 0.0f);
    float distance = 0.0f;
    for (int i = 0; i < (int)m_points->m_count; i++) {
        m_distances[i] = distance;
        if (i == (int)m_points->m_count - 1) {
            break;
        }
        distance += m_points->m_points[i].m_length;
    }
}

/* The rotation matrix of Euler angles, in one of three orders (other units call it by its
 * address). */
extern "C" void fn_800193B8(Mtx34f *m, const Vec3f *rotation, int order) {
    bool set = false;
    switch (order) {
    case 0:
        if (0.0f != rotation->z) {
            fn_8040C148(m, &(const Mtx34f &)fn_8040B0C0(rotation->z));
            set = true;
        }
        if (0.0f != rotation->y) {
            if (set) {
                fn_8040BAC4(m, &(const Mtx34f &)fn_8040B138(rotation->y));
            } else {
                fn_8040C148(m, &(const Mtx34f &)fn_8040B138(rotation->y));
                set = true;
            }
        }
        if (0.0f != rotation->x) {
            if (set) {
                fn_8040BAC4(m, &(const Mtx34f &)fn_8040B1B0(rotation->x));
            } else {
                fn_8040C148(m, &(const Mtx34f &)fn_8040B1B0(rotation->x));
            }
        } else if (!set) {
            fn_8040C148(m, &lbl_805E0720);
        }
        break;
    case 1:
        if (0.0f != rotation->z) {
            fn_8040C148(m, &(const Mtx34f &)fn_8040B0C0(rotation->z));
            set = true;
        }
        if (0.0f != rotation->x) {
            if (set) {
                fn_8040BAC4(m, &(const Mtx34f &)fn_8040B1B0(rotation->x));
            } else {
                fn_8040C148(m, &(const Mtx34f &)fn_8040B1B0(rotation->x));
                set = true;
            }
        }
        if (0.0f != rotation->y) {
            if (set) {
                fn_8040BAC4(m, &(const Mtx34f &)fn_8040B138(rotation->y));
            } else {
                fn_8040C148(m, &(const Mtx34f &)fn_8040B138(rotation->y));
            }
        } else if (!set) {
            fn_8040C148(m, &lbl_805E0720);
        }
        break;
    case 2:
        if (0.0f != rotation->y) {
            fn_8040C148(m, &(const Mtx34f &)fn_8040B138(rotation->y));
            set = true;
        }
        if (0.0f != rotation->x) {
            if (set) {
                fn_8040BAC4(m, &(const Mtx34f &)fn_8040B1B0(rotation->x));
            } else {
                fn_8040C148(m, &(const Mtx34f &)fn_8040B1B0(rotation->x));
                set = true;
            }
        }
        if (0.0f != rotation->z) {
            if (set) {
                fn_8040BAC4(m, &(const Mtx34f &)fn_8040B0C0(rotation->z));
            } else {
                fn_8040C148(m, &(const Mtx34f &)fn_8040B0C0(rotation->z));
            }
        } else if (!set) {
            fn_8040C148(m, &lbl_805E0720);
        }
        break;
    }
}

void *PathData::operator new(size_t size) {
    return fn_80049B90(size, 2);
}

/* The flags of the path a joint leads to (0 when it is gone). */
extern "C" u32 fn_80019320(JointPathData *joint) {
    return fn_80016980(&joint->m_path) ? 0 : fn_80016718(&joint->m_path)->m_flags;
}

/* ---- PathCtrl ---- */

PathCtrl::PathCtrl() : m_index(0), m_t(0.0f), m_jointsOf(0), m_branchArg(0), m_reverse(false) {}

/* PathCtrl's assignment (Game/PlayerPath.cpp calls it by its address). */
extern "C" inline PathCtrl *fn_80018FFC(PathCtrl *path, const PathCtrl *from) {
    path->m_path = from->m_path;
    path->m_index = from->m_index;
    path->m_t = from->m_t;
    path->m_jointsOf = from->m_jointsOf;
    path->m_startJoint = from->m_startJoint;
    path->m_endJoint = from->m_endJoint;
    path->m_branch = from->m_branch;
    path->m_branchArg = from->m_branchArg;
    path->m_reverse = from->m_reverse;
    return path;
}

/* Puts path at the start or the end of the path a joint leads to; direction tells which way it
 * then runs (Game/PlayerPath.cpp calls it by its address). */
extern "C" void fn_80018F1C(JointPathData *joint, PathCtrl *path, int *direction) {
    PathCtrl ctrl;
    ctrl.m_path = joint->m_path;
    if (joint->m_atStart) {
        fn_8001725C(&ctrl, 0, true);
        *direction = 0;
    } else {
        if (fn_80016980(&joint->m_path)) {
            return;
        }
        fn_8001725C(&ctrl, fn_80016718(&joint->m_path)->m_points->m_count - 1, true);
        *direction = 1;
    }
    fn_80018FFC(path, &ctrl);
}

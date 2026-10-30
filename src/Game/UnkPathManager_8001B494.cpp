/* The path manager (class name unknown; Game/PathData.cpp makes it on first use, fn_8001B494): the
 * links of paths it keeps, and what adds paths to it (from the stage's path data, or placed by a
 * model), removes them, finds the path nearest a position and moves a PathCtrl along its path. This
 * unit makes the paths (sp_counted_base_impl<PathData *, checked_deleter<PathData> > is its RTTI),
 * and holds the game's first copies of the task tree's instance (fn_8001C4F8), of list<void *>'s
 * push_back, insert and clear (other units call them by these C names) and of the rotation matrix
 * from Euler angles in a given axis order (fn_8001E070). File name from the manager's placeholder;
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * NonMatching (53 of 54 functions match; every data section matches in objdiff):
 * - list<void *>::insert (0x8001E528) is the list's real template instance, so it and its deleter
 *   temporary, string and RTTI come out in the game's order. Its storage accessors are the list's
 *   members, which the game calls out of line: Game/PathData.cpp has sz and node_alloc under C
 *   names that other units call (fn_8001BDE8, fn_8001BDEC), so those two are written here as
 *   inline specializations that call them; alloc is the real member (0x8001BDF4).
 * - fn_8001DB08: the game loads source->m_count ahead of the register saves and copies the path's
 *   header through two registers; GC/2.7 does neither for any source shape tried (95.5%).
 * Two functions the game's linker dropped are written to reproduce the order of the shared code
 * that follows them (UnkPathManagerUnused_8001C4F8, UpdatePathLinks). */
#define PATHDATA_CODE
#define MSL_LIST_DEREF_DIRECT
#include "boost/shared_ptr.hpp"

namespace SonicteamUSA {
namespace System {
class PathData;
}
} // namespace SonicteamUSA

/* The path's destructor (Game/PathData.cpp). */
extern "C" SonicteamUSA::System::PathData *fn_8001977C(SonicteamUSA::System::PathData *data, short flag);

namespace boost {
/* The path's destructor is a C function (see Game/PathData.cpp). */
template <> inline void checked_delete(SonicteamUSA::System::PathData *x) {
    fn_8001977C(x, 1);
}

namespace detail {
/* The path's control block, as boost writes it, with its virtual functions defined out of line
 * below, where the game emits them. */
template <>
class sp_counted_base_impl<SonicteamUSA::System::PathData *, checked_deleter<SonicteamUSA::System::PathData> > : public sp_counted_base {
  private:
    typedef SonicteamUSA::System::PathData PathData;

    PathData *ptr;
    checked_deleter<PathData> del;

    sp_counted_base_impl(sp_counted_base_impl const &);
    sp_counted_base_impl &operator=(sp_counted_base_impl const &);

  public:
    sp_counted_base_impl(PathData *p, checked_deleter<PathData> d) : ptr(p), del(d) {}

    virtual ~sp_counted_base_impl();
    virtual void dispose();
    virtual void *get_deleter(std::type_info const &ti);
};
} // namespace detail
} // namespace boost

#include "Game/PathData.h"
#include "Game/PathCtrl.h"

#include <list>
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

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

extern const float lbl_805FAD38;  /* pi */
extern const Vec3f lbl_805E07B8;  /* the default rotation */

/* An empty inline function: calling it keeps the caller out of line (deferred inlining is one
 * level deep). */
inline void PathManagerKeep() {}

/* ---- The task tree's instance ---- */

/* The root of the task tree (Game/UnkTaskLayers_801EB424.cpp). */
class UnkTaskLayers_801EB424 {
  public:
    UnkTaskLayers_801EB424();
    ~UnkTaskLayers_801EB424();

    /* 0x00 */ u8 field_0x0[0x50];
};

/* Its instance (below). */
extern UnkTaskLayers_801EB424 *lbl_805EE030;

/* The task tree, made on first use (an inline function other units call by this name; this unit
 * has the game's first copy). */
extern "C" inline UnkTaskLayers_801EB424 *fn_8001C4F8(void) {
    PathManagerKeep();
    if (lbl_805EE030 == 0) {
        static UnkTaskLayers_801EB424 s_layers;
        lbl_805EE030 = &s_layers;
    }
    return lbl_805EE030;
}

/* Instance pointers (the first is not used in the game; the last is the next unit's, see
 * fn_8001FD38). */
void *lbl_805EE02C;
UnkTaskLayers_801EB424 *lbl_805EE030;
void *lbl_805EE034;

/* ---- The game's list of pointers (list<void *>; see Game/PathData.cpp) ---- */

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

    void *const &Value() const {
        return static_cast<UnkPtrListNode_8001B5B8 *>(node)->value;
    }
    void Next() {
        node = node->next;
    }
    bool operator!=(const UnkPtrListPosition_8001B674 &other) const {
        return node != other.node;
    }

    /* 0x0 */ UnkPtrListLink_8001B5B8 *node;
};

struct UnkPtrList_8001B5B8 {
    /* 0x0 */ u32 m_size;
    /* 0x4 */ UnkPtrListLink_8001B5B8 m_end;
};
SIZE_ASSERT(UnkPtrList_8001B5B8, 0xC);

typedef UnkPtrList_8001B5B8 PtrList;
typedef UnkPtrListPosition_8001B674 PtrListPos;
typedef std::__list_node<void *> PtrListNode;
typedef std::__list_node_allocator<PtrListNode> PtrListNodeAlloc;

extern "C" {
/* Game/PathData.cpp: the list's size and node allocator, its first and end position, and erasing
 * a range. */
u32 &fn_8001BDE8(PtrList *list);
PtrListNodeAlloc &fn_8001BDEC(PtrList *list);
PtrListPos fn_8001B674(PtrList *list);
PtrListPos fn_8001B6B0(PtrList *list);
PtrListPos fn_8001BD24(PtrList *list, PtrListPos first, PtrListPos last);
}

/* list<void *>'s storage, as the list's members (Game/PathData.cpp has their copies, which other
 * units call by these C names). */
namespace std {
template <> inline __list_imp<void *>::size_type &__list_imp<void *>::sz() {
    return fn_8001BDE8(reinterpret_cast<PtrList *>(this));
}
template <> inline __list_imp<void *>::node_allocator &__list_imp<void *>::node_alloc() {
    return fn_8001BDEC(reinterpret_cast<PtrList *>(this));
}
} // namespace std

typedef std::__list_imp<void *> PtrListImp;
typedef std::__list_iterator<void *> PtrListIterator;

/* The list's end position, as the list's iterator (the same function as fn_8001B674 above). */
namespace PtrListEnd {
extern "C" PtrListIterator fn_8001B674(PtrList *list);
}

/* The path's control block's virtual functions (see above). */
void *boost::detail::sp_counted_base_impl<PathData *, boost::checked_deleter<PathData> >::get_deleter(
    std::type_info const &ti) {
    return ti == typeid(boost::checked_deleter<PathData>) ? &del : 0;
}

void boost::detail::sp_counted_base_impl<PathData *, boost::checked_deleter<PathData> >::dispose() {
    del(ptr);
}

boost::detail::sp_counted_base_impl<PathData *, boost::checked_deleter<PathData> >::~sp_counted_base_impl() {}

/* ---- The links' list (list<UnkPathLink_80017DBC *>, built on the list of pointers) ---- */

/* list<void *>::push_back and clear (other units call them by these names; see
 * Game/CharaColliGeneral.h, Game/CsdScene.h). */
extern "C" inline void fn_8001D820(PtrList *list, void *const &x) {
    PathManagerKeep();
    reinterpret_cast<PtrListImp *>(list)->insert(PtrListEnd::fn_8001B674(list), x);
}

extern "C" inline void fn_8001E404(PtrList *list) {
    PathManagerKeep();
    fn_8001BD24(list, fn_8001B6B0(list), fn_8001B674(list));
}

/* A list of pointers to T (template name invented). */
template <class T> class UnkPtrListOf_8001C6B0 : public PtrList {
  public:
    class const_iterator {
      public:
        const_iterator() : m_pos() {}
        const_iterator(const PtrListPos &pos) : m_pos(pos) {}
        const_iterator(const const_iterator &other) : m_pos(other.m_pos) {}

        T *const &operator*() const {
            return reinterpret_cast<T *const &>(m_pos.Value());
        }
        const_iterator operator++(int) {
            const_iterator tmp(*this);
            m_pos.Next();
            return tmp;
        }
        bool operator!=(const const_iterator &other) const {
            return m_pos != other.m_pos;
        }

        /* 0x0 */ PtrListPos m_pos;
    };

    const_iterator begin() const {
        return const_iterator(fn_8001B6B0(const_cast<UnkPtrListOf_8001C6B0 *>(this)));
    }
    const_iterator end() const {
        return const_iterator(fn_8001B674(const_cast<UnkPtrListOf_8001C6B0 *>(this)));
    }
    void push_back(T *const &x) {
        fn_8001D820(this, reinterpret_cast<void *const &>(x));
    }
};

typedef UnkPtrListOf_8001C6B0<UnkPathLink_80017DBC> PathLinkList;
typedef PathLinkList::const_iterator PathLinkIterator;

/* The path manager: its links. */
class UnkPathManager_8001B494 {
  public:
    /* 0x0 */ PathLinkList m_links;
    /* 0xC */ u32 field_0xC;
};

/* What a path filter looks at: the path's flags and the caller's value. */
struct UnkPathFilterParam_8001CDA0 {
    /* 0x0 */ u32 m_flags;
    /* 0x4 */ int m_arg;
};

typedef bool (*UnkPathFilter_8001CDA0)(UnkPathFilterParam_8001CDA0 *param);

/* Axes a placed path is turned to (Game/PathData.cpp, fn_8001C4B8). */
struct UnkVecPair_8001C4B8 {
    /* 0x00 */ Vec3f a;
    /* 0x0C */ Vec3f b;
};

/* Three floats (the translation of a matrix, as fn_800496F8 returns it; struct name unknown). */
struct UnkVec3_800496F8 {
    float x;
    float y;
    float z;
};

extern "C" {
/* Allocation from a heap. */
void *fn_80049B90(u32 size, int heap);
/* Dot product, squared length and squared distance. */
float fn_8040CF90(const Vec3f *a, const Vec3f *b);
float fn_8040D180(const Vec3f *a, const Vec3f *b);
/* Matrix copy and product (in place), Euler angles of a matrix, its translation, and two angles
 * of a direction. */
void fn_8040BAC4(Mtx34f *m, const Mtx34f *other);
void fn_800493C4(Vec3f *rotation, const Mtx34f *matrix);
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);

/* Game/PathCtrl.cpp and Game/PathData.cpp. */
PathPtr fn_800182D8(PathCtrl *path);
PathCtrl *fn_80018FFC(PathCtrl *path, const PathCtrl *from);
PathData *fn_80016B48(PathCtrl *path);
u32 fn_80016AAC(PathCtrl *path);
float fn_80016FB8(PathCtrl *path);
boost::weak_ptr<PathData> fn_80017B48(PathCtrl *path, bool *flip);
void fn_8001725C(PathCtrl *path, u32 index, bool stay);
Fn8040CopyVector3 fn_80018634(PathCtrl *path);
Vec3f fn_80018434(PathCtrl *path, bool interpolate, int direction);
bool fn_800174C4(PathCtrl *path);
bool fn_800170B8(PathCtrl *path);
UnkPathLink_80017DBC *fn_80017DBC(PathCtrl *path);
u32 fn_80016A34(const JointList *list);
JointIterator fn_80016D84(JointList *list);
JointIterator fn_80016DB8(JointIterator *it, int);
JointIterator fn_80016E18(JointList *list);
u32 fn_80019320(JointPathData *joint);
UnkPathLink_80017DBC *fn_8001B280(UnkPathLink_80017DBC *link, short flag);
void fn_800193B8(Mtx34f *m, const Vec3f *rotation, int order);
}

/* Adds a path (below). */
extern "C" UnkPathLink_80017DBC *fn_8001D514(UnkPathManager_8001B494 *manager, PathPoints *points,
                                             PathCtrl *path, bool ownsPoints);

namespace ret {
/* The same functions, returning what they build. */
extern "C" Mtx34f fn_8040B034(const Vec3f *offset);
extern "C" Mtx34f fn_8040B0C0(float angle);
extern "C" Mtx34f fn_8040B138(float angle);
extern "C" Mtx34f fn_8040B1B0(float angle);
extern "C" Fn8040CopyVector3 fn_800169F8(const Vec3f *v);
extern "C" Vec3f fn_80016284(const UnkVecPair_8001C4B8 *axes);
extern "C" UnkVec3_800496F8 fn_800496F8(const Mtx34f *m);
extern "C" Mtx34f fn_8040B8FC(const Mtx34f *a, const Mtx34f *b);
} // namespace ret

/* Adds a joint to path in a list of joints (Game/PathData.cpp). */
void AddJoint(UnkJointList_80016BB4 *list, const boost::weak_ptr<PathData> &path, bool atStart,
              bool flip);

inline bool operator!=(const Vec3f &a, const Vec3f &b) {
    return a.x != b.x || a.y != b.y || a.z != b.z;
}

/* Integer constants (the flags of a segment that runs backwards along x, y and z). */
const u32 kSegmentBackX = 1 << 0;
const u32 kSegmentBackY = 1 << 1;
const u32 kSegmentBackZ = 1 << 2;

/* ---- The path manager ---- */

/* Removes every link. */
extern "C" void fn_8001E358(UnkPathManager_8001B494 *manager) {
    PathLinkIterator it;
    PathLinkIterator cur;
    for (it = manager->m_links.begin(); it != manager->m_links.end();) {
        cur = it++;
        fn_8001B280(*cur, 1);
    }
    fn_8001E404(&manager->m_links);
    manager->field_0xC = 0;
}

/* Removes the path a PathCtrl is on: its link, or the path itself. */
extern "C" void fn_8001E2DC(UnkPathManager_8001B494 *manager, PathCtrl *path) {
    UnkPathLink_80017DBC *link = fn_80017DBC(path);
    if (link != 0) {
        fn_8001B280(link, 1);
    } else {
        /* The game reads both joint counts and drops the result (an assertion, presumably). */
        fn_80016A34(&fn_80016B48(path)->m_startJoints) == 0 &&
            fn_80016A34(&fn_80016B48(path)->m_endJoints) == 0;
        fn_8001977C(fn_80016B48(path), 1);
    }
}

/* Adds the paths of a null-terminated table, then links them. */
extern "C" void fn_8001E20C(UnkPathManager_8001B494 *manager, PathPoints **paths) {
    for (PathPoints *points = *paths++; points != 0; points = *paths++) {
        fn_8001D514(manager, points, 0, false);
    }
    PathLinkIterator it;
    for (it = manager->m_links.begin(); it != manager->m_links.end(); it++) {
        UnkPathLink_80017DBC *link = *it;
        if (!link->m_branched) {
            link->UpdateOffsets();
        }
        link->UpdateJoints();
    }
}

/* The rotation matrix of Euler angles, applied in the axis order given. */
extern "C" void fn_8001E070(Mtx34f *m, const Vec3f *rotation, int order) {
    switch (order) {
    case 0:
        if (0.0f != rotation->z) {
            fn_8040BAC4(m, &(const Mtx34f &)ret::fn_8040B0C0(rotation->z));
        }
        if (0.0f != rotation->y) {
            fn_8040BAC4(m, &(const Mtx34f &)ret::fn_8040B138(rotation->y));
        }
        if (0.0f != rotation->x) {
            fn_8040BAC4(m, &(const Mtx34f &)ret::fn_8040B1B0(rotation->x));
        }
        break;
    case 1:
        if (0.0f != rotation->z) {
            fn_8040BAC4(m, &(const Mtx34f &)ret::fn_8040B0C0(rotation->z));
        }
        if (0.0f != rotation->x) {
            fn_8040BAC4(m, &(const Mtx34f &)ret::fn_8040B1B0(rotation->x));
        }
        if (0.0f != rotation->y) {
            fn_8040BAC4(m, &(const Mtx34f &)ret::fn_8040B138(rotation->y));
        }
        break;
    case 2:
        if (0.0f != rotation->y) {
            fn_8040BAC4(m, &(const Mtx34f &)ret::fn_8040B138(rotation->y));
        }
        if (0.0f != rotation->x) {
            fn_8040BAC4(m, &(const Mtx34f &)ret::fn_8040B1B0(rotation->x));
        }
        if (0.0f != rotation->z) {
            fn_8040BAC4(m, &(const Mtx34f &)ret::fn_8040B0C0(rotation->z));
        }
        break;
    }
}

/* Matrix product (other units call it by this name). */
extern "C" inline Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b) {
    PathManagerKeep();
    return ret::fn_8040B8FC(a, b);
}

/* The vector of three floats (other units call it by this name). */
extern "C" inline UnkVec3_800496F8 fn_8001DFE8(const UnkVec3_800496F8 *v) {
    PathManagerKeep();
    UnkVec3_800496F8 r = {v->x, v->y, v->z};
    return r;
}

/* A copy of a vector. */
extern "C" inline Vec3f *fn_8001DFCC(Vec3f *v, const Vec3f *from) {
    PathManagerKeep();
    v->x = from->x;
    v->y = from->y;
    v->z = from->z;
    return v;
}

/* Adds a copy of a path placed at position, turned by rotation (in the axis order given) and to the
 * axes given. NonMatching: see the top of the file. */
extern "C" void fn_8001DB08(UnkPathManager_8001B494 *manager, PathPoints *source, const Vec3f *position,
                            Vec3f rotation, int order, PathCtrl *path, const UnkVecPair_8001C4B8 *axes) {
    Mtx34f turn;
    Mtx34f flip;
    Mtx34f matrix;
    Vec3f min;
    PathPoints *points = (PathPoints *)fn_80049B90(source->m_count * sizeof(PathPoint) + sizeof(PathPoints), 2);
    PathPoint *to = (PathPoint *)(points + 1);
    PathPoint *from;
    bool turned = false;
    *points = *source;
    from = source->m_points;
    points->m_points = to;

    fn_8040C148(&matrix, &(const Mtx34f &)ret::fn_8040B034(position));
    fn_8001E070(&matrix, &rotation, order);

    if (axes->b != lbl_805E07A0) {
        float pitch;
        float yaw;
        turned = true;
        fn_8003B9E8(&axes->b, &pitch, &yaw);
        fn_8040C148(&turn, &(const Mtx34f &)ret::fn_8040B034(&(const Vec3f &)ret::fn_80016284(axes)));
        fn_8040C148(&turn, &(const Mtx34f &)fn_8001E044(&(const Mtx34f &)ret::fn_8040B138(-yaw), &turn));
        fn_8040C148(&turn, &(const Mtx34f &)fn_8001E044(&(const Mtx34f &)ret::fn_8040B1B0(-pitch), &turn));
        fn_8040C148(&flip, &lbl_805E0720);
        flip.values[2][2] = -1.0f;
        fn_8040C148(&turn, &(const Mtx34f &)fn_8001E044(&flip, &turn));
        fn_8040C148(&turn, &(const Mtx34f &)fn_8001E044(&(const Mtx34f &)ret::fn_8040B1B0(pitch), &turn));
        fn_8040C148(&turn, &(const Mtx34f &)fn_8001E044(&(const Mtx34f &)ret::fn_8040B138(yaw), &turn));
        fn_8040C148(&turn, &(const Mtx34f &)fn_8001E044(
                               &(const Mtx34f &)ret::fn_8040B034(fn_80008E1C(
                                   const_cast<Vec3f *>(&(const Vec3f &)Vec3f()), axes->a.x, axes->a.y, axes->a.z)),
                               &turn));
        fn_8040BAC4(&matrix, &turn);
    }

    for (int i = 0; i < (int)points->m_count; i++, from++, to++) {
        Mtx34f pointMatrix;
        fn_800193B8(&pointMatrix, &from->m_rotation, 1);
        fn_8040C148(&pointMatrix,
                    &(const Mtx34f &)fn_8001E044(
                        &(const Mtx34f &)fn_8001E044(&matrix, &(const Mtx34f &)ret::fn_8040B034(
                                                                  &(const Vec3f &)ret::fn_800169F8(&from->m_position))),
                        &pointMatrix));
        *reinterpret_cast<UnkVec3_800496F8 *>(&to->m_position) = fn_8001DFE8(&(const UnkVec3_800496F8 &)ret::fn_800496F8(&pointMatrix));
        if (turned) {
            fn_8040BAC4(&pointMatrix, &flip);
        }
        fn_800493C4(&to->m_rotation, &pointMatrix);
        if (turned) {
            to->m_rotation.y += lbl_805FAD38;
            if (to->m_rotation.y > lbl_805FAD38) {
                to->m_rotation.y -= 2.0f * lbl_805FAD38;
            }
        }
        to->m_length = from->m_length;
    }

    PathPoint *point = points->m_points;
    Vec3f max = *fn_8001DFCC(&min, &point->m_position);
    for (int j = 0; j < (int)points->m_count - 1; j++, point++) {
        u32 flags = 0;
        if (point[0].m_position.x > point[1].m_position.x) {
            flags |= kSegmentBackX;
        }
        if (point[0].m_position.y > point[1].m_position.y) {
            flags |= kSegmentBackY;
        }
        if (point[0].m_position.z > point[1].m_position.z) {
            flags |= kSegmentBackZ;
        }
        point->field_0x1C = flags;
        if (point[1].m_position.x < min.x) {
            min.x = point[1].m_position.x;
        }
        if (point[1].m_position.y < min.y) {
            min.y = point[1].m_position.y;
        }
        if (point[1].m_position.z < min.z) {
            min.z = point[1].m_position.z;
        }
        if (max.x < point[1].m_position.x) {
            max.x = point[1].m_position.x;
        }
        if (max.y < point[1].m_position.y) {
            max.y = point[1].m_position.y;
        }
        if (max.z < point[1].m_position.z) {
            max.z = point[1].m_position.z;
        }
    }
    point->field_0x1C = 0;
    points->m_max = max;
    points->m_min = min;
    fn_8001D514(manager, points, path, true);
}

/* Adds the paths of a null-terminated table that the filter accepts, placed at position, turned
 * and to the axes given, then links them. */
extern "C" void fn_8001D9C0(UnkPathManager_8001B494 *manager, PathPoints **paths, const Vec3f *position,
                            Vec3f rotation, int order, PathCtrl *path, const UnkVecPair_8001C4B8 *axes,
                            UnkPathFilter_8001CDA0 filter, int arg) {
    PathPoints *points;
    PathPoints **next = paths;
    while ((points = *next++) != 0) {
        UnkPathFilterParam_8001CDA0 param = {points->m_flags, arg};
        if (filter == 0 || filter(&param)) {
            fn_8001DB08(manager, points, position, rotation, order, path, axes);
        }
    }
    PathLinkIterator it;
    for (it = manager->m_links.begin(); it != manager->m_links.end(); it++) {
        UnkPathLink_80017DBC *link = *it;
        if (!link->m_branched) {
            link->UpdateOffsets();
        }
        link->UpdateJoints();
    }
}

/* Adds a path: links it to the links it meets, or makes it a link of its own; path (when given)
 * is put at the link's start. Returns the link. */
extern "C" UnkPathLink_80017DBC *fn_8001D514(UnkPathManager_8001B494 *manager, PathPoints *points, PathCtrl *path,
                                 bool ownsPoints) {
    PathPtr data;
    UnkPathLink_80017DBC *link = 0;
    if (data = PathPtr(new PathData(points, ownsPoints))) {
        if (data->m_flags & 1) {
            PathLinkIterator it;
            for (it = manager->m_links.begin(); it != manager->m_links.end(); it++) {
                if ((*it)->Connect(data)) {
                    link = *it;
                }
                if (fn_80016A34(&data->m_startJoints) != 0 && fn_80016A34(&data->m_endJoints) != 0) {
                    break;
                }
            }
        }
        if (fn_80016A34(&data->m_startJoints) == 0 && fn_80016A34(&data->m_endJoints) == 0) {
            if ((link = new UnkPathLink_80017DBC(PathPtr(data))) != 0) {
                manager->m_links.push_back(link);
            }
            if (data->m_flags & 2) {
                AddJoint(&data->m_startJoints, boost::weak_ptr<PathData>(data), false, false);
                AddJoint(&data->m_endJoints, boost::weak_ptr<PathData>(data), true, false);
            }
        }
        data->UpdateDistances();
        if (path != 0) {
            path->m_path = link->m_first;
            path->m_index = 0;
            path->m_t = 0.0f;
        }
    }
    return link;
}

/* Finds the path nearest position (within radius; any distance when it is 0) among those the
 * filter accepts, and puts path there; direction (when given) tells which way the path runs
 * along *outDirection. */
extern "C" bool fn_8001CDA0(UnkPathManager_8001B494 *manager, PathCtrl *path, const Vec3f *position,
                            const Vec3f *direction, int *outDirection, UnkPathFilter_8001CDA0 filter,
                            int arg, float radius) {
    PathPtr best;
    float bestT;
    float range = radius * radius;
    bestT = 0.0f;
    int bestIndex = 0;
    bool anywhere = false;
    if (0.0f == range) {
        range = -1.0f;
        anywhere = true;
    }
    PathLinkIterator it;
    for (it = manager->m_links.begin(); it != manager->m_links.end(); it++) {
        UnkPathLink_80017DBC *link = *it;
        if (!anywhere) {
            if (!(link->m_min.x - radius <= position->x && link->m_min.y - radius <= position->y &&
                  link->m_min.z - radius <= position->z && position->x <= link->m_max.x + radius &&
                  position->y <= link->m_max.y + radius && position->z <= link->m_max.z + radius)) {
                continue;
            }
        }
        for (PathList::iterator j = link->m_paths.begin(); j != link->m_paths.end(); j++) {
            PathPtr data = *j;
            u32 flags = data->m_flags;
            UnkPathFilterParam_8001CDA0 param = {flags, arg};
            if (filter != 0 && !filter(&param)) {
                continue;
            }
            if (anywhere ||
                (data->m_min.x - radius <= position->x && data->m_min.y - radius <= position->y &&
                 data->m_min.z - radius <= position->z && position->x <= radius + data->m_max.x &&
                 position->y <= radius + data->m_max.y && position->z <= radius + data->m_max.z)) {
                PathPoints *points = data->m_points;
                for (int i = 0; i < (int)points->m_count - 1; i++) {
                    PathPoint *a = &points->m_points[i];
                    PathPoint *b = &points->m_points[i + 1];
                    if (!anywhere) {
                        u32 flags = a->field_0x1C;
                        if (flags & kSegmentBackX) {
                            if (!(b->m_position.x - radius <= position->x &&
                                  position->x <= a->m_position.x + radius)) {
                                continue;
                            }
                        } else if (!(a->m_position.x - radius <= position->x &&
                                     position->x <= b->m_position.x + radius)) {
                            continue;
                        }
                        if (flags & kSegmentBackY) {
                            if (!(b->m_position.y - radius <= position->y &&
                                  position->y <= a->m_position.y + radius)) {
                                continue;
                            }
                        } else if (!(a->m_position.y - radius <= position->y &&
                                     position->y <= b->m_position.y + radius)) {
                            continue;
                        }
                        if (flags & kSegmentBackZ) {
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
                        Vec3f at(segment.x * t + a->m_position.x, segment.y * t + a->m_position.y,
                                 segment.z * t + a->m_position.z);
                        float distance = fn_8040D180(position, &at);
                        if (-1.0f == range || distance <= range) {
                            best = data;
                            bestT = t;
                            bestIndex = i;
                            range = distance;
                        }
                    } else if (dot < 0.0f) {
                        Vec3f to(a->m_position.x - position->x, a->m_position.y - position->y,
                                 a->m_position.z - position->z);
                        float distance = fn_8040CF2C(&to);
                        if (-1.0f == range || distance <= range) {
                            best = data;
                            range = distance;
                            bestT = 0.0f;
                            bestIndex = i;
                        }
                    } else if (i == (int)points->m_count - 2) {
                        Vec3f to(b->m_position.x - position->x, b->m_position.y - position->y,
                                 b->m_position.z - position->z);
                        float distance = fn_8040CF2C(&to);
                        if (-1.0f == range || distance <= range) {
                            best = data;
                            range = distance;
                            bestT = 1.0f;
                            bestIndex = i;
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
        path->m_jointsOf = 0;
        path->m_reverse = false;
        if (*direction != lbl_805E07A0 && outDirection != 0) {
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

/* Moves path by distance (backwards when direction is set or distance is negative); position and
 * rotation (when given) receive where it is then. True at the end of the path. */
extern "C" bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int direction, Vec3f *position,
                            Vec3f *rotation, bool interpolate, float distance) {
    bool none = !fn_800182D8(path);
    if (none) {
        fn_800091D0(position, &lbl_805E07A0);
        *rotation = lbl_805E07B8;
        return true;
    }
    PathCtrl ctrl;
    fn_80018FFC(&ctrl, path);
    if (distance < 0.0f) {
        distance = -distance;
        if (direction == 0) {
            direction = 1;
        } else {
            direction = 0;
        }
    }
    if (direction == 0) {
        do {
            PathData *data = fn_80016B48(&ctrl);
            u32 index = fn_80016AAC(&ctrl);
            float t = ctrl.m_reverse ? 1.0f - ctrl.m_t : ctrl.m_t;
            float length = fn_80016FB8(&ctrl);
            float done = length * t;
            float rest = length - done;
            if (rest < 0.0f) {
                rest = 0.0f;
            }
            if (0.0f == length || distance > rest) {
                distance -= rest;
            } else {
                float to = 1.0f - (rest - distance) / length;
                if (to < 0.0f) {
                    to = 0.0f;
                }
                if (index == data->m_points->m_count - 1 && to > 0.0f) {
                    bool flip;
                    ctrl.m_path = fn_80017B48(&ctrl, &flip);
                    ctrl.m_reverse ^= flip;
                    fn_8001725C(&ctrl, 0, false);
                }
                fn_80018FFC(path, &ctrl);
                path->m_t = path->m_reverse ? 1.0f - to : to;
                *reinterpret_cast<Fn8040CopyVector3 *>(position) = fn_80018634(path);
                if (rotation != 0) {
                    *rotation = fn_80018434(path, interpolate, direction);
                }
                return false;
            }
        } while (!fn_800174C4(&ctrl));
        *reinterpret_cast<Fn8040CopyVector3 *>(position) = fn_80018634(&ctrl);
        if (rotation != 0) {
            *rotation = fn_80018434(&ctrl, interpolate, direction);
        }
        fn_80018FFC(path, &ctrl);
        return true;
    } else {
        do {
            PathData *data = fn_80016B48(&ctrl);
            u32 index = fn_80016AAC(&ctrl);
            float t = ctrl.m_reverse ? 1.0f - ctrl.m_t : ctrl.m_t;
            float length = fn_80016FB8(&ctrl);
            float done = length * t;
            if (length - done < 0.0f) {
                done = length;
            }
            if (0.0f == length || distance > done) {
                distance -= done;
            } else {
                done -= distance;
                float to = done / length;
                if (1.0f - to < 0.0f) {
                    to = 1.0f;
                }
                fn_80018FFC(path, &ctrl);
                if (path->m_reverse) {
                    to = 1.0f - to;
                }
                path->m_t = to;
                *reinterpret_cast<Fn8040CopyVector3 *>(position) = fn_80018634(path);
                if (rotation != 0) {
                    *rotation = fn_80018434(path, interpolate, direction);
                }
                return false;
            }
        } while (!fn_800170B8(&ctrl));
        *reinterpret_cast<Fn8040CopyVector3 *>(position) = fn_80018634(&ctrl);
        if (rotation != 0) {
            *rotation = fn_80018434(&ctrl, interpolate, direction);
        }
        fn_80018FFC(path, &ctrl);
        return true;
    }
}

/* Puts path on the path with the id given; false when there is none. */
extern "C" bool fn_8001C7B4(UnkPathManager_8001B494 *manager, PathCtrl *path, u32 id) {
    PathLinkIterator it;
    for (it = manager->m_links.begin(); it != manager->m_links.end(); it++) {
        for (PathList::iterator j = (*it)->m_paths.begin(); j != (*it)->m_paths.end(); j++) {
            PathPtr data = *j;
            if (id == data->field_0x4) {
                path->m_path = data;
                return true;
            }
        }
    }
    return false;
}

/* Sets up the joints of every link (not called in the game; the linker drops it, but the list's
 * functions it uses come out after it, as in the game). */
void UpdatePathLinks(UnkPathManager_8001B494 *manager) {
    PathLinkIterator it;
    for (it = manager->m_links.begin(); it != manager->m_links.end(); it++) {
        UnkPathLink_80017DBC *link = *it;
        if (!link->m_branched) {
            link->UpdateOffsets();
        }
        link->UpdateJoints();
    }
}

/* The joint to take among a path's joints (the branch callback's default): the first that goes the
 * way flip tells and whose path has one of the flags in mask; else the first such joint whose path
 * has flag 4; else the first one. */
extern "C" JointPathData *fn_8001C5D8(JointList *joints, u32 mask, bool flip) {
    JointPathData *joint = 0;
    for (JointIterator it = fn_80016E18(joints); it != fn_80016D84(joints); fn_80016DB8(&it, 0)) {
        if (!(flip ^ it->m_flip)) {
            if (mask & fn_80019320(it.operator->())) {
                return it.operator->();
            }
            if (fn_80019320(it.operator->()) & 4) {
                joint = it.operator->();
            } else if (joint == 0) {
                joint = it.operator->();
            }
        }
    }
    return joint;
}

/* An object's setup (struct name unknown; Game/UnkSystemManager_801E2128.cpp). */
struct UnkObject_8001C5C8 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ u32 field_0x4;
    /* 0x8 */ bool field_0x8;
};

extern "C" UnkObject_8001C5C8 *fn_8001C5C8(UnkObject_8001C5C8 *object) {
    object->field_0x4 = 0;
    object->field_0x8 = false;
    return object;
}

/* The default branch callback: the joint fn_8001C5D8 takes. */
struct UnkDefaultBranch_8001C5D8 {
    JointPathData *operator()(SonicteamUSA::System::BranchCallBackParam &param) const {
        return fn_8001C5D8(param.m_joints, param.m_arg, false);
    }
};

/* Not called in the game (the linker drops it; what it did is unknown): written so that the task
 * tree's instance and boost::function's helpers that it uses come out first in this unit, as they
 * do in the game. */
void UnkPathManagerUnused_8001C4F8(PathCtrl *path) {
    path->m_branch.m_callback = UnkDefaultBranch_8001C5D8();
    fn_8001C4F8();
}

/* SonicteamUSA::System::PathCtrl (class name from the RTTI of its boost::shared_ptr control block;
 * Game/PathCtrl.h): a position on a path (Game/PathData.h) that steps from point to point and goes
 * on into the next path through the joints at the path's ends. This unit holds its destructor and
 * the functions other units call by address (C functions taking the PathCtrl); its constructor
 * belongs to the next unit (0x80018F1C, which starts with fn_80018F1C and the PathCtrl assignment).
 * It also holds the game's first copies of boost::bad_weak_ptr, std::swap for boost::function's
 * any_pointer, and the weak_ptr<PathData> and list<JointPathData> code. File name from the class;
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first).
 *
 * Template instances that other units call by fn_ name are written as friends of class templates
 * (see UnkWeakPtrCopies_80016718): GC/2.7 then keeps them out of line and emits them after their
 * first caller, as it does the instances. fn_800169F8 and fn_80018634 construct their vector
 * through the out-of-line copy constructor fn_800091EC, which returns its object: callers here see
 * them through declarations that return the vector (namespace ret), the definitions take it as
 * their first argument. */
#define MSL_LIST_ACCESSORS
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
using SonicteamUSA::System::PathPoint;
using SonicteamUSA::System::UnkJointList_80016BB4;
using SonicteamUSA::System::UnkPathLink_80017DBC;

typedef std::list<JointPathData> JointList;
typedef std::__list_iterator<JointPathData> JointIterator;

namespace std {
float fabs(float x);
}

extern const float lbl_805FAD38; /* pi */
extern const Vec3f lbl_805E07B8;

extern "C" {
/* The next unit's PathData helpers: a point (null out of range) and the link between two paths
 * (behaviour inferred). */
PathPoint *fn_8001974C(PathData *data, int index);
bool fn_8001A0EC(UnkPathLink_80017DBC *link, PathCtrl *path, const Vec3f *position,
                 const Vec3f *direction, int *outDirection, float radius);
/* The angle between a and b at t (1 - t given too; behaviour inferred). */
float fn_8004924C(float a, float b, float t, float u);
/* v * scale and a + b. */
Fn8040CopyVector3 fn_800091A4(const Fn8040CopyVector3 *v, float scale);
Fn8040CopyVector3 fn_8040CDD0(const Fn8040CopyVector3 *a, const Fn8040CopyVector3 *b);

/* The game's shared copies of weak_ptr<PathData> and list<JointPathData> code, which other units
 * call by these names. */
bool fn_80016980(const boost::weak_ptr<PathData> *path);
boost::shared_ptr<PathData> fn_80016718(const boost::weak_ptr<PathData> *path);
JointPathData &fn_800169C8(JointList *list);
u32 fn_80016A34(const JointList *list);
JointIterator fn_80016D84(JointList *list);
JointIterator fn_80016DB8(JointIterator *it, int);
JointIterator fn_80016E18(JointList *list);

PathData *fn_80016B48(PathCtrl *path);
u32 fn_80016AAC(PathCtrl *path);
JointList *fn_80016478(PathCtrl *path, int direction, Fn8040CopyVector3 *position, float range);
float fn_80016FB8(PathCtrl *path);
void fn_8001725C(PathCtrl *path, u32 index, bool stay);
boost::weak_ptr<PathData> fn_800176FC(PathCtrl *path, bool *flip);
boost::weak_ptr<PathData> fn_80017B88(PathCtrl *path, bool *flip);
boost::weak_ptr<PathData> fn_80017B48(PathCtrl *path, bool *flip);
boost::weak_ptr<PathData> fn_80017D7C(PathCtrl *path, bool *flip);
PathData *fn_80017A9C(PathCtrl *path);
UnkPathLink_80017DBC *fn_80017DBC(PathCtrl *path);
boost::shared_ptr<PathData> fn_800182D8(PathCtrl *path);
}

extern "C" void fn_800169F8(Fn8040CopyVector3 *out, const Vec3f *v);

namespace ret {
/* fn_800169F8 as a function returning the vector. */
extern "C" Fn8040CopyVector3 fn_800169F8(const Vec3f *v);
} // namespace ret

/* The index of the path's last point. */
inline u32 LastIndex(PathData *data) {
    return data->m_points->m_count - 1;
}

/* A temporary as a const reference (no copy). */
inline const Fn8040CopyVector3 &AsConst(const Fn8040CopyVector3 &v) {
    return v;
}

/* The game's copies of weak_ptr<PathData> and list<JointPathData> members that other units call by
 * these names (expired, lock, front, size, end, postfix ++ and begin). Defined as friends of class
 * templates, so that GC/2.7 treats them as it treats the template instances they stand for: kept
 * out of line and emitted after their first caller. */
template <class T> struct UnkWeakPtrCopies_80016718 {
    friend bool fn_80016980(const boost::weak_ptr<T> *path) {
        return path->pn.use_count() == 0;
    }
    friend boost::shared_ptr<T> fn_80016718(const boost::weak_ptr<T> *path) {
        return fn_80016980(path) ? boost::shared_ptr<T>() : boost::shared_ptr<T>(*path);
    }
};
template struct UnkWeakPtrCopies_80016718<PathData>;

template <class T> struct UnkListCopies_80016A34 {
    friend T &fn_800169C8(std::list<T> *list) {
        return static_cast<std::__list_node<T> *>(list->tail()->next_)->data_;
    }
    friend u32 fn_80016A34(const std::list<T> *list) {
        return list->sz();
    }
    friend std::__list_iterator<T> fn_80016D84(std::list<T> *list) {
        return std::__list_iterator<T>(list->tail());
    }
    friend std::__list_iterator<T> fn_80016DB8(std::__list_iterator<T> *it, int) {
        std::__list_iterator<T> tmp(*it);
        ++*it;
        return tmp;
    }
    friend std::__list_iterator<T> fn_80016E18(std::list<T> *list) {
        return std::__list_iterator<T>(list->tail()->next_);
    }
};
template struct UnkListCopies_80016A34<JointPathData>;

/* A point's position as a vector, constructed in out (callers see it as a function returning the
 * vector; the game's copy is the vector's constructor from a point, through its out-of-line copy
 * constructor). */
extern "C" inline void fn_800169F8(Fn8040CopyVector3 *out, const Vec3f *v) {
    Vec3f position(v->x, v->y, v->z);
    fn_800091EC(reinterpret_cast<Vec3f *>(out), &position);
}

/* The game's one shared copy of std::swap for boost::function's any_pointer, which other units
 * declare (see Game/CatchStick.cpp) and function1::swap calls: an overload, defined as a friend
 * of a class template so that it comes out where the template instance would. */
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

/* boost/weak_ptr.hpp only declares weak_ptr::reset, which the game has out of line; this unit
 * holds the copy for PathData (boost 1.32's definition). */
namespace boost {
template <> inline void weak_ptr<PathData>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The path manager (class name unknown; Game/PlayerPath.cpp), which the next unit's fn_8001B494
 * creates the first time it is called. The game may have it as the weak static of an inline
 * accessor (this unit references it nowhere); it is the first object of the unit's .sbss. */
class UnkPathManager_8001B494;
UnkPathManager_8001B494 *g_pathManager;

/* Index conversions between the path's order and the direction followed. */
extern "C" u32 fn_80017094(PathData *data, u32 index, bool reverse);
extern "C" u32 fn_800176B4(PathData *data, u32 index, bool reverse);
extern "C" u32 fn_800176D8(PathData *data, u32 index, bool reverse);

PathCtrl::~PathCtrl() {
    fn_80017DBC(this);
}

/* The position on the path. */
extern "C" Fn8040CopyVector3 fn_80018634(PathCtrl *path) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return Fn8040CopyVector3(lbl_805E07A0);
    }
    if (0.0f == path->m_t) {
        return ret::fn_800169F8(&data->m_points->m_points[path->m_index].m_position);
    }
    if (1.0f == path->m_t) {
        return ret::fn_800169F8(&data->m_points->m_points[path->m_index + 1].m_position);
    }
    Fn8040CopyVector3 a = ret::fn_800169F8(&data->m_points->m_points[path->m_index].m_position);
    Fn8040CopyVector3 b = ret::fn_800169F8(&data->m_points->m_points[path->m_index + 1].m_position);
    return fn_8040CDD0(&AsConst(fn_800091A4(&a, 1.0f - path->m_t)),
                       &AsConst(fn_800091A4(&b, path->m_t)));
}

/* The rotation on the path (Euler angles), interpolated between the points or not; turned round
 * backwards. */
extern "C" Vec3f fn_80018434(PathCtrl *path, bool interpolate) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return lbl_805E07B8;
    }
    Vec3f rot;
    if (0.0f == path->m_t) {
        rot = data->m_points->m_points[path->m_index].m_rotation;
    } else if (1.0f == path->m_t) {
        rot = data->m_points->m_points[path->m_index + 1].m_rotation;
    } else if (interpolate) {
        Vec3f a = data->m_points->m_points[path->m_index].m_rotation;
        Vec3f b = data->m_points->m_points[path->m_index + 1].m_rotation;
        rot.x = fn_8004924C(a.x, b.x, path->m_t, 1.0f - path->m_t);
        rot.y = fn_8004924C(a.y, b.y, path->m_t, 1.0f - path->m_t);
        rot.z = fn_8004924C(a.z, b.z, path->m_t, 1.0f - path->m_t);
    } else {
        rot = data->m_points->m_points[path->m_index].m_rotation;
    }
    if (path->m_reverse) {
        rot.y += lbl_805FAD38;
        if (rot.y > lbl_805FAD38) {
            rot.y -= 2.0f * lbl_805FAD38;
        }
    }
    return rot;
}

/* The length of the path (or of the link it is on). */
extern "C" float fn_800183D8(PathCtrl *path) {
    UnkPathLink_80017DBC *link = fn_80017DBC(path);
    if (link != 0) {
        return link->m_length;
    }
    PathData *data = fn_80016B48(path);
    if (data != 0) {
        return data->m_points->m_length;
    }
    return 0.0f;
}

/* Goes to the path's start when it has a path (and its link a first path). */
extern "C" void fn_80018304(PathCtrl *path) {
    boost::shared_ptr<PathData> data = fn_800182D8(path);
    if (!data) {
        return;
    }
    UnkPathLink_80017DBC *link = fn_80017DBC(path);
    if (link != 0) {
        data = fn_80016718(&link->m_first);
        if (!data) {
            return;
        }
        path->m_index = 0;
        path->m_t = 0.0f;
    }
}

extern "C" boost::shared_ptr<PathData> fn_800182D8(PathCtrl *path) {
    return fn_80016718(&path->m_path);
}

/* Goes to the end of the link's last path when it has a path (and its link a last path). */
extern "C" void fn_800181BC(PathCtrl *path) {
    boost::shared_ptr<PathData> data = fn_800182D8(path);
    if (!data) {
        return;
    }
    UnkPathLink_80017DBC *link = fn_80017DBC(path);
    if (link != 0) {
        data = fn_80016718(&link->m_last);
        if (!data) {
            return;
        }
        path->m_index = data->m_points->m_count - 1;
        path->m_t = 0.0f;
    }
}

/* Looks for a path of the link near position (fn_8001A0EC). */
extern "C" bool fn_80018150(PathCtrl *path, const Vec3f *position, const Vec3f *direction,
                            int *outDirection, float radius) {
    UnkPathLink_80017DBC *link = fn_80017DBC(path);
    if (link != 0) {
        return fn_8001A0EC(link, path, position, direction, outDirection, radius);
    }
    return false;
}

/* True at the start of the path (its end backwards) when it is its link's first path. */
extern "C" bool fn_80018030(PathCtrl *path) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return false;
    }
    if (0.0f == path->m_t) {
        UnkPathLink_80017DBC *link = fn_80017DBC(path);
        if (link != 0) {
            if (fn_80016980(&link->m_first)) {
                return false;
            }
            if (data == fn_80016718(&link->m_first).get()) {
                if (path->m_reverse) {
                    return path->m_index == data->m_points->m_count - 1;
                }
                return path->m_index == 0;
            }
        } else {
            if (path->m_reverse) {
                return path->m_index == data->m_points->m_count - 1;
            }
            return path->m_index == 0;
        }
    }
    return false;
}

/* True at the end of the path (its start backwards) when it is its link's last path. */
extern "C" bool fn_80017E2C(PathCtrl *path) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return false;
    }
    if (0.0f == path->m_t) {
        UnkPathLink_80017DBC *link = fn_80017DBC(path);
        if (link != 0) {
            if (fn_80016980(&link->m_last)) {
                return false;
            }
            if (data == fn_80016718(&link->m_last).get()) {
                if (path->m_reverse) {
                    return path->m_index == 0;
                }
                return path->m_index == data->m_points->m_count - 1;
            }
        } else {
            if (path->m_reverse) {
                return path->m_index == 0;
            }
            return path->m_index == data->m_points->m_count - 1;
        }
    } else if (1.0f == path->m_t) {
        UnkPathLink_80017DBC *link = fn_80017DBC(path);
        if (link != 0) {
            if (fn_80016980(&link->m_last)) {
                return false;
            }
            if (data == fn_80016718(&link->m_last).get()) {
                if (path->m_reverse) {
                    return path->m_index == 1;
                }
                return path->m_index == data->m_points->m_count - 2;
            }
        } else {
            if (path->m_reverse) {
                return path->m_index == 1;
            }
            return path->m_index == data->m_points->m_count - 2;
        }
    }
    return false;
}

/* The link the path is on. */
extern "C" UnkPathLink_80017DBC *fn_80017DBC(PathCtrl *path) {
    return fn_80016980(&path->m_path) ? 0 : fn_80016718(&path->m_path)->m_link;
}

/* The path before (going forwards) through the joint at the path's start. */
extern "C" boost::weak_ptr<PathData> fn_80017D7C(PathCtrl *path, bool *flip) {
    if (path->m_reverse) {
        return fn_800176FC(path, flip);
    }
    return fn_80017B88(path, flip);
}

/* The path the start joint leads to (refreshing the joints when the path changed); flip tells
 * whether the direction turns round there. */
extern "C" boost::weak_ptr<PathData> fn_80017B88(PathCtrl *path, bool *flip) {
    if (path->m_jointsOf != fn_80016B48(path)) {
        path->m_jointsOf = fn_80016B48(path);
        if (fn_80016A34(&path->m_jointsOf->m_startJoints) > 1) {
            path->m_startJoint.m_path.reset();
        } else {
            path->m_startJoint = fn_800169C8(&path->m_jointsOf->m_startJoints);
        }
        if (fn_80016A34(&path->m_jointsOf->m_endJoints) > 1 ||
            fn_80016A34(&path->m_jointsOf->m_endJoints) == 0) {
            path->m_endJoint.m_path.reset();
        } else {
            path->m_endJoint = fn_800169C8(&path->m_jointsOf->m_endJoints);
        }
    }
    if (fn_80016980(&path->m_startJoint.m_path)) {
        if (path->m_branch.IsSet()) {
            BranchCallBackParam param;
            param.m_joints = &path->m_jointsOf->m_startJoints;
            param.m_arg = path->m_branchArg;
            JointPathData *joint = path->m_branch.Call(param);
            if (joint != 0) {
                path->m_startJoint = *joint;
            }
        } else {
            for (JointIterator it = fn_80016E18(&path->m_jointsOf->m_startJoints);
                 it != fn_80016D84(&path->m_jointsOf->m_startJoints); fn_80016DB8(&it, 0)) {
                if ((fn_80016718(&it->m_path)->m_flags >> 2) & 1) {
                    path->m_startJoint.m_path = it->m_path;
                    break;
                }
            }
            if (fn_80016980(&path->m_startJoint.m_path)) {
                path->m_startJoint = fn_800169C8(&path->m_jointsOf->m_startJoints);
            }
        }
    }
    *flip = path->m_startJoint.m_flip;
    return path->m_startJoint.m_path;
}

/* The path after (going forwards). */
extern "C" boost::weak_ptr<PathData> fn_80017B48(PathCtrl *path, bool *flip) {
    if (path->m_reverse) {
        return fn_80017B88(path, flip);
    }
    return fn_800176FC(path, flip);
}

/* The next path's data. */
extern "C" PathData *fn_80017A9C(PathCtrl *path) {
    bool flip;
    if (path->m_reverse) {
        boost::shared_ptr<PathData> next(fn_80017B88(path, &flip));
        return next.get();
    } else {
        boost::shared_ptr<PathData> next(fn_800176FC(path, &flip));
        return next.get();
    }
}

/* The path the end joint leads to (see fn_80017B88). */
extern "C" boost::weak_ptr<PathData> fn_800176FC(PathCtrl *path, bool *flip) {
    if (path->m_jointsOf != fn_80016B48(path)) {
        path->m_jointsOf = fn_80016B48(path);
        if (fn_80016A34(&path->m_jointsOf->m_startJoints) > 1 ||
            fn_80016A34(&path->m_jointsOf->m_startJoints) == 0) {
            path->m_startJoint.m_path.reset();
        } else {
            path->m_startJoint = fn_800169C8(&path->m_jointsOf->m_startJoints);
        }
        if (fn_80016A34(&path->m_jointsOf->m_endJoints) > 1) {
            path->m_endJoint.m_path.reset();
        } else {
            path->m_endJoint = fn_800169C8(&path->m_jointsOf->m_endJoints);
        }
    }
    if (fn_80016980(&path->m_endJoint.m_path)) {
        if (path->m_branch.IsSet()) {
            BranchCallBackParam param;
            param.m_joints = &path->m_jointsOf->m_endJoints;
            param.m_arg = path->m_branchArg;
            JointPathData *joint = path->m_branch.Call(param);
            if (joint != 0) {
                path->m_endJoint = *joint;
            }
        } else {
            for (JointIterator it = fn_80016E18(&path->m_jointsOf->m_endJoints);
                 it != fn_80016D84(&path->m_jointsOf->m_endJoints); fn_80016DB8(&it, 0)) {
                if ((fn_80016718(&it->m_path)->m_flags >> 2) & 1) {
                    path->m_endJoint.m_path = it->m_path;
                    break;
                }
            }
            if (fn_80016980(&path->m_endJoint.m_path)) {
                path->m_endJoint = fn_800169C8(&path->m_jointsOf->m_endJoints);
            }
        }
    }
    *flip = path->m_endJoint.m_flip;
    return path->m_endJoint.m_path;
}

extern "C" u32 fn_800176D8(PathData *data, u32 index, bool reverse) {
    if (reverse) {
        return data->m_points->m_count - 1 - index;
    }
    return index;
}

extern "C" u32 fn_800176B4(PathData *data, u32 index, bool reverse) {
    if (reverse) {
        return data->m_points->m_count - 1 - index;
    }
    return index;
}

/* Steps to the next segment; true at the path's end. */
extern "C" bool fn_800174C4(PathCtrl *path) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return true;
    }
    u32 index = fn_80016AAC(path);
    if (index == fn_80016B48(path)->m_points->m_count - 2 ||
        index == fn_80016B48(path)->m_points->m_count - 1) {
        if ((path->m_reverse && fn_80016A34(&fn_80016B48(path)->m_startJoints) != 0) ||
            (!path->m_reverse && fn_80016A34(&fn_80016B48(path)->m_endJoints) != 0)) {
            bool flip;
            path->m_path = fn_80017B48(path, &flip);
            if (fn_80016980(&path->m_path)) {
                return true;
            }
            path->m_reverse ^= flip;
            path->m_t = path->m_reverse ? 1.0f : 0.0f;
            path->m_index = fn_800176D8(fn_80016B48(path), 0, path->m_reverse);
            return false;
        }
        path->m_t = path->m_reverse ? 1.0f : 0.0f;
        if (path->m_reverse) {
            path->m_index = 0;
        } else {
            path->m_index = fn_80016B48(path)->m_points->m_count - 1;
        }
        return true;
    }
    path->m_t = path->m_reverse ? 1.0f : 0.0f;
    u32 i = fn_800176B4(data, path->m_index, path->m_reverse);
    path->m_index = fn_800176D8(data, i + 1, path->m_reverse);
    return false;
}

/* Sets the segment (in the direction followed); at the path's end, goes on into the next path
 * unless stay is set. */
extern "C" inline void fn_8001725C(PathCtrl *path, u32 index, bool stay) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return;
    }
    if (path->m_reverse && index == data->m_points->m_count - 1) {
        if (fn_80016A34(&data->m_endJoints) != 0 && !stay) {
            bool flip;
            path->m_path = fn_80017B48(path, &flip);
            path->m_reverse ^= flip;
            fn_8001725C(path, 0, false);
            return;
        }
        index--;
        path->m_t = 0.0f;
    }
    path->m_index = path->m_reverse ? data->m_points->m_count - 2 - index : index;
}

/* Steps to the previous segment; true at the path's start. */
extern "C" bool fn_800170B8(PathCtrl *path) {
    if (fn_80016B48(path) == 0) {
        return true;
    }
    if (fn_80016AAC(path) == 0) {
        if ((!path->m_reverse && fn_80016A34(&fn_80016B48(path)->m_startJoints) != 0) ||
            (path->m_reverse && fn_80016A34(&fn_80016B48(path)->m_endJoints) != 0)) {
            bool flip;
            path->m_path = fn_80017D7C(path, &flip);
            path->m_reverse ^= flip;
            path->m_t = path->m_reverse ? 0.0f : 1.0f;
            fn_8001725C(path, LastIndex(fn_80016B48(path)) - 1, false);
            return false;
        }
        path->m_t = path->m_reverse ? 1.0f : 0.0f;
        if (path->m_reverse) {
            path->m_index = fn_80016B48(path)->m_points->m_count - 1;
        } else {
            path->m_index = 0;
        }
        return true;
    }
    path->m_t = path->m_reverse ? 0.0f : 1.0f;
    fn_8001725C(path, fn_80016AAC(path) - 1, false);
    return false;
}

extern "C" u32 fn_80017094(PathData *data, u32 index, bool reverse) {
    if (reverse) {
        return data->m_points->m_count - 2 - index;
    }
    return index;
}

/* The length of the current segment (that of the next path's first one at the path's end). */
extern "C" float fn_80016FB8(PathCtrl *path) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return 0.0f;
    }
    u32 index = fn_80016AAC(path);
    if (fn_80016AAC(path) == LastIndex(fn_80016B48(path))) {
        if (fn_80016A34(&fn_80016B48(path)->m_endJoints) != 0) {
            index = 0;
            data = fn_80017A9C(path);
        } else {
            return 0.0f;
        }
    }
    if (data == 0) {
        return 0.0f;
    }
    u32 i = fn_80017094(data, index, path->m_reverse);
    return data->m_points->m_points[i].m_length;
}

/* The joints within range (behind it when negative) of the position whose paths lead back to this
 * one (new list; null when there are none). */
extern "C" UnkJointList_80016BB4 *fn_80016BB4(PathCtrl *path, int direction,
                                             Fn8040CopyVector3 *position, float range) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return 0;
    }
    JointList *joints = fn_80016478(path, direction, position, range);
    if (joints == 0) {
        return 0;
    }
    UnkJointList_80016BB4 *result = new UnkJointList_80016BB4;
    JointIterator it;
    for (it = fn_80016E18(joints); it != fn_80016D84(joints); fn_80016DB8(&it, 0)) {
        result->push_back(*it.operator->());
    }
    JointPathData *joint = &fn_800169C8(joints);
    if (fn_80016980(&joint->m_path)) {
        return 0;
    }
    PathData *other = fn_80016718(&joint->m_path).get();
    JointList *back = joint->m_atStart ? &other->m_startJoints : &other->m_endJoints;
    for (it = fn_80016E18(back); it != fn_80016D84(back); fn_80016DB8(&it, 0)) {
        if (!fn_80016980(&it->m_path)) {
            if (fn_80016718(&it->m_path).get() == data) {
                result->push_back(*it.operator->());
            }
        }
    }
    return result;
}

extern "C" inline PathData *fn_80016B48(PathCtrl *path) {
    return fn_80016980(&path->m_path) ? 0 : fn_80016718(&path->m_path).get();
}

/* The segment in the path's order. */
extern "C" inline u32 fn_80016AAC(PathCtrl *path) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return 0;
    }
    if (path->m_reverse && path->m_index == data->m_points->m_count - 1) {
        path->m_index = path->m_index - 1;
        path->m_t = 1.0f;
    }
    if (path->m_reverse) {
        return LastIndex(data) - path->m_index - 1;
    }
    return path->m_index;
}

/* The joints within range of the position (see fn_80016BB4); fills position with the path's end
 * there. */
extern "C" JointList *fn_80016478(PathCtrl *path, int direction, Fn8040CopyVector3 *position,
                                  float range) {
    PathData *data = fn_80016B48(path);
    if (data == 0) {
        return 0;
    }
    float distance = data->m_distances[fn_80016AAC(path)];
    float t = path->m_reverse ? 1.0f - path->m_t : path->m_t;
    float length = fn_80016FB8(path);
    distance += length * t;
    bool flip = (direction == 1) ^ (range < 0.0f);
    bool forward = flip ^ path->m_reverse;
    if (!forward) {
        distance = data->m_points->m_length - distance;
    }
    range = std::fabs(range);
    while (distance <= range) {
        JointList *joints = forward ? &data->m_startJoints : &data->m_endJoints;
        if (fn_80016A34(joints) > 1) {
            if (forward) {
                if (position != 0) {
                    Fn8040CopyVector3 point;
                    fn_800169F8(&point, &fn_8001974C(data, 0)->m_position);
                    *position = point;
                }
                return joints;
            }
            if (position != 0) {
                Fn8040CopyVector3 point;
                fn_800169F8(&point, &fn_8001974C(data, data->m_points->m_count - 1)->m_position);
                *position = point;
            }
            return joints;
        }
        if (fn_80016A34(joints) == 1) {
            JointPathData *joint = &fn_800169C8(joints);
            if (fn_80016980(&joint->m_path)) {
                return 0;
            }
            data = fn_80016718(&joint->m_path).get();
            distance += data->m_points->m_length;
            forward = forward ^ joint->m_flip;
        } else {
            return 0;
        }
    }
    return 0;
}

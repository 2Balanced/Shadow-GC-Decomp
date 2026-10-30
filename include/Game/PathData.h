#ifndef GAME_PATHDATA_H
#define GAME_PATHDATA_H

#include <list>

#define PTRVECTOR_INDEX
#include "Game/PtrVector.h"

#include <boost/function.hpp>
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

/* A path's data and its joints (class names from RTTI: sp_counted_base_impl<PathData *> and the
 * bound branch callbacks, function1<JointPathData *, BranchCallBackParam &>). Layouts follow the
 * loads and stores of Game/PathCtrl.cpp; member names are inferred. Game/PlayerPath.cpp declares
 * JointPathData and BranchCallBackParam on its own with the members it uses, so it does not include
 * this header. */

namespace std {

/* The path's distance table: MSL builds it on vector<void *> (Game/PtrVector.h), through two layers
 * whose operator[] the game has out of line (layer names invented). */
template <class T> class __podvec_imp : public vector<void *, allocator<void *> > {
  public:
    typedef vector<void *, allocator<void *> > base;

    T &operator[](size_t n) {
        return (T &)base::operator[](n);
    }
};

template <class T> class __podvec : public __podvec_imp<T> {
  public:
    typedef __podvec_imp<T> imp;

    T &operator[](size_t n) {
        return imp::operator[](n);
    }
};

} // namespace std

namespace SonicteamUSA {
namespace System {
class JointPathData;
}
} // namespace SonicteamUSA

/* list<JointPathData>'s begin and end are the game's copies that other units call by these names
 * (defined in Game/PathCtrl.cpp); the list's members call them too. */
extern "C" {
std::__list_iterator<SonicteamUSA::System::JointPathData>
fn_80016D84(std::list<SonicteamUSA::System::JointPathData> *list);
std::__list_iterator<SonicteamUSA::System::JointPathData>
fn_80016E18(std::list<SonicteamUSA::System::JointPathData> *list);
}

namespace std {
template <> inline __list_imp<SonicteamUSA::System::JointPathData>::~__list_imp() {
    erase(fn_80016E18(static_cast<list<SonicteamUSA::System::JointPathData> *>(this)),
          fn_80016D84(static_cast<list<SonicteamUSA::System::JointPathData> *>(this)));
}

template <>
inline void
list<SonicteamUSA::System::JointPathData>::push_back(const SonicteamUSA::System::JointPathData &x) {
    this->insert(fn_80016D84(this), x);
}
} // namespace std

namespace SonicteamUSA {
namespace System {

class PathData;

/* A point of a path (0x20 bytes): its position, its rotation (Euler angles) and the length of
 * the segment to the next point. */
struct PathPoint {
    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_rotation;
    /* 0x18 */ float m_length;
    /* 0x1C */ u32 field_0x1C;
};
SIZE_ASSERT(PathPoint, 0x20);

/* A path as loaded: its point count, length and points, then the flags and bounds PathData
 * copies. */
struct PathPoints {
    /* 0x00 */ u32 m_count;
    /* 0x04 */ float m_length;
    /* 0x08 */ PathPoint *m_points;
    /* 0x0C */ u32 m_flags;
    /* 0x10 */ Vec3f m_min;
    /* 0x1C */ u32 field_0x1C;
    /* 0x20 */ Vec3f m_max;
};

/* Where a path meets another one: the direction of the branch, the other path, whether the joint
 * is at the other path's start and whether going on reverses the direction. */
class JointPathData {
  public:
    /* 0x00 */ Vec3f m_rotation;
    /* 0x0C */ boost::weak_ptr<PathData> m_path;
    /* 0x14 */ bool m_atStart;
    /* 0x15 */ bool m_flip;
};
SIZE_ASSERT(JointPathData, 0x18);

/* What a path passes to its branch callback: the joints to choose from and the callback's own
 * value. */
class BranchCallBackParam {
  public:
    /* 0x0 */ std::list<JointPathData> *m_joints;
    /* 0x4 */ u32 m_arg;
};

/* A list of joints (class name unknown): those at a path's start and end, and those a search
 * returns (Game/PathCtrl.cpp's fn_80016BB4; deleted with fn_80019910). */
class UnkJointList_80016BB4 : public std::list<JointPathData> {
  public:
    UnkJointList_80016BB4() {}
};

/* Paths linked into one (class name unknown; the next unit builds and searches them): the length
 * of them all, their bounds, the first and the last path, and the paths (a std::list of
 * shared_ptr<PathData> at 0x2C, not yet modelled). */
struct UnkPathLink_80017DBC {
    /* 0x00 */ float m_length;
    /* 0x04 */ Vec3f m_min;
    /* 0x10 */ Vec3f m_max;
    /* 0x1C */ boost::weak_ptr<PathData> m_first;
    /* 0x24 */ boost::weak_ptr<PathData> m_last;
};

/* A path (0x54 bytes; the next unit's constructor at 0x8001995C sets it up from the file data):
 * its flags and bounds from the file, its offset in its link, its points, the distance of each
 * point from the start, the joints at its start and end, its link, and whether it owns the
 * points. */
class PathData {
  public:
    /* 0x00 */ u32 m_flags;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ float m_linkOffset;
    /* 0x0C */ Fn8040CopyVector3 m_min;
    /* 0x18 */ Fn8040CopyVector3 m_max;
    /* 0x24 */ PathPoints *m_points;
    /* 0x28 */ std::__podvec<float> m_distances;
    /* 0x34 */ UnkJointList_80016BB4 m_startJoints;
    /* 0x40 */ UnkJointList_80016BB4 m_endJoints;
    /* 0x4C */ UnkPathLink_80017DBC *m_link;
    /* 0x50 */ bool m_ownsPoints;
    /* 0x51 */ u8 field_0x51;
    /* 0x52 */ u8 field_0x52;
};
SIZE_ASSERT(PathData, 0x54);

typedef boost::function1<JointPathData *, BranchCallBackParam &> BranchCallBack;

} // namespace System
} // namespace SonicteamUSA

/* The branch callback slot of a PathCtrl (at 0x44; name as in Game/PlayerPath.cpp). */
struct UnkBranchSlot_800AC16C {
    bool IsSet() {
        return HasTarget();
    }
    bool HasTarget() const {
        return !m_callback.empty();
    }
    SonicteamUSA::System::JointPathData *Call(SonicteamUSA::System::BranchCallBackParam &param) {
        if (m_callback.empty()) {
            return 0;
        }
        return m_callback(param);
    }

    SonicteamUSA::System::BranchCallBack m_callback;
};

#endif

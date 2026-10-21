/* A path follower of the vehicles (file and class names unknown): a pimpl wrapper
 * (UnkPathMover_803345C4, whose methods the vehicle code at 0x801AF644 calls by their fn_ names)
 * around a PathCtrl that the path manager moves along a path, keeping the follower's position and
 * its up and forward vectors. Names marked "(name inferred)" are not original. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first).
 *
 * NonMatching: 12 of 13 functions match. fn_80334480 differs by two swapped instructions: retail
 * loads mover->m_impl above the store of the link register (lwz r3, 0(r3) before stw r0, 0x14(r1));
 * GC/2.7 does not, for every source form tried, while it matches the two-argument forwarders
 * beside it. */
#include <boost/scoped_ptr.hpp>

#include "Game/PathCtrl.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::PathCtrl;

/* The path manager fn_8001B494 returns (class name unknown; see BKTankController.cpp). */
class UnkPathManager_8001B494;

class UnkPathMoverImpl_803348C8;

extern "C" {
/* The path manager, setting a path up from a start and a direction with a filter on the path
 * flags, and moving a placement along it by distance (behaviour inferred; see
 * BKTankController.cpp and ElecCircuitEntrance.cpp). */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001CDA0(UnkPathManager_8001B494 *manager, PathCtrl *path, const Vec3f *start,
                 const Vec3f *direction, int *mode, bool (*filter)(const u32 *flags), int arg6,
                 float arg7);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int mode, UnkPathMoverImpl_803348C8 *mover,
                 Vec3f *rotation, int arg5, float distance);
/* Whether the path is set up (behaviour inferred; see Coaster.cpp). */
bool fn_800ABDCC(PathCtrl *path);
/* vector turned by angle about the y, x and z axes (behaviour inferred). */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);
Vec3f fn_8040D118(const Vec3f *vector, float angle);
Vec3f fn_8040D044(const Vec3f *vector, float angle);
}

/* The path follower (class name unknown; constructor 0x803348C8). The path manager writes the
 * position (it takes the follower as a placement); m_mode is the direction along the path that
 * fn_8001CDA0 chooses (1: forward; names inferred). */
class UnkPathMoverImpl_803348C8 {
  public:
    UnkPathMoverImpl_803348C8();

    bool Start(const Vec3f *start, const Vec3f *direction);
    bool Propagate(float distance);
    void GetPlacement(Vec3f *position, Vec3f *forward, Vec3f *up);
    void SetRotation(const Vec3f *rotation);

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_forward;
    /* 0x18 */ Vec3f m_up;
    /* 0x24 */ PathCtrl m_path;
    /* 0x7C */ int m_mode;
};
SIZE_ASSERT(UnkPathMoverImpl_803348C8, 0x80);

/* The pimpl wrapper (class name unknown; constructor 0x803345C4). */
class UnkPathMover_803345C4 {
  public:
    UnkPathMover_803345C4();
    ~UnkPathMover_803345C4();

    /* 0x0 */ boost::scoped_ptr<UnkPathMoverImpl_803348C8> m_impl;
};
SIZE_ASSERT(UnkPathMover_803345C4, 0x4);

/* The paths a follower may take: those with flag 0x200 or 0x80000 (name and meanings inferred). */
static bool IsVehiclePath(const u32 *flags) {
    return (*flags & 0x80200) != 0;
}

UnkPathMoverImpl_803348C8::UnkPathMoverImpl_803348C8() {}

bool UnkPathMoverImpl_803348C8::Start(const Vec3f *start, const Vec3f *direction) {
    if (fn_8001CDA0(fn_8001B494(), &m_path, start, direction, &m_mode, IsVehiclePath, 0, 0.0f)) {
        return Propagate(0.0f);
    }
    return false;
}

bool UnkPathMoverImpl_803348C8::Propagate(float distance) {
    bool ended = false;
    if (fn_800ABDCC(&m_path)) {
        UnkPathManager_8001B494 *manager = fn_8001B494();
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        ended = !fn_8001C91C(manager, &m_path, m_mode, this, &rotation, 1, distance);
        SetRotation(&rotation);
    }
    return ended;
}

void UnkPathMoverImpl_803348C8::GetPlacement(Vec3f *position, Vec3f *forward, Vec3f *up) {
    if (fn_800ABDCC(&m_path)) {
        if (position != 0) {
            fn_800091D0(position, &m_position);
        }
        if (up != 0) {
            fn_800091D0(up, &m_up);
        }
        if (forward != 0) {
            fn_800091D0(forward, &m_forward);
        }
    }
}

void UnkPathMoverImpl_803348C8::SetRotation(const Vec3f *rotation) {
    fn_800091D0(&m_up, &Vec3f(0.0f, 1.0f, 0.0f));
    fn_800091D0(&m_up, &fn_8040D0AC(&m_up, rotation->y));
    fn_800091D0(&m_up, &fn_8040D118(&m_up, rotation->x));
    fn_800091D0(&m_up, &fn_8040D044(&m_up, rotation->z));
    fn_800091D0(&m_forward, &Vec3f(0.0f, 0.0f, m_mode == 1 ? 1.0f : -1.0f));
    fn_800091D0(&m_forward, &fn_8040D0AC(&m_forward, rotation->y));
    fn_800091D0(&m_forward, &fn_8040D118(&m_forward, rotation->x));
    fn_800091D0(&m_forward, &fn_8040D044(&m_forward, rotation->z));
}

UnkPathMover_803345C4::UnkPathMover_803345C4() : m_impl(new UnkPathMoverImpl_803348C8) {}

UnkPathMover_803345C4::~UnkPathMover_803345C4() {}

/* Starts following the path from start along direction (name inferred). */
extern "C" bool fn_803344C8(UnkPathMover_803345C4 *mover, const Vec3f *start,
                            const Vec3f *direction) {
    return mover->m_impl->Start(start, direction);
}

/* Moves along the path by distance; returns true at its end (name inferred). */
extern "C" bool fn_803344A4(UnkPathMover_803345C4 *mover, float distance) {
    return mover->m_impl->Propagate(distance);
}

/* Reads the position and the forward and up vectors; any may be null (name inferred). */
extern "C" void fn_80334480(UnkPathMover_803345C4 *mover, Vec3f *position, Vec3f *forward,
                            Vec3f *up) {
    mover->m_impl->GetPlacement(position, forward, up);
}

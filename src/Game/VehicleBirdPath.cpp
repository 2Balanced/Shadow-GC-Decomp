/* The path a bird (Vehicle::BirdBase::BirdBase) follows: the pimpl wrapper
 * Vehicle::BirdBase::UnkBirdPath_801B446C (Game/BirdBase.h) and its state, a PathCtrl with the
 * point the bird has reached on it and that point's frame. Class names are unknown (no RTTI or
 * vtable), so the file is named after the wrapper's role; member names are inferred. The
 * wrapper's methods keep their placeholder names, as free functions over the wrapper, because
 * Game/BirdBase.cpp and Game/VehicleAirWingCamera.cpp call them by those names. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#define VEHICLE_BIRD_PATH_IMPL
#include <boost/scoped_ptr.hpp>

#include "Game/BirdBase.h"
#include "Game/MathTypes.h"
#include "Game/PathCtrl.h"
#include "Game/TVehicleBase.h"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::PathCtrl;
using Vehicle::TVehicleBase;
using Vehicle::BirdBase::UnkBirdPath_801B446C;
using Vehicle::BirdBase::UnkBirdPathImpl;

class UnkPathManager_8001B494;

/* A path's record as the path search's filter sees it (see Game/GunLift.cpp). */
struct UnkPathRecord_8001CDA0 {
    /* 0x0 */ u32 flags;
};

extern "C" {
/* The path manager and its searches (see Game/GunLift.cpp and Game/Coaster.cpp): moves along a
 * path by a distance (true at its end), finds the path nearest a point, and whether a path is set
 * and has ended (behaviour inferred). */
UnkPathManager_8001B494 *fn_8001B494(void);
bool fn_8001C91C(UnkPathManager_8001B494 *manager, PathCtrl *path, int backward, Vec3f *position,
                 Vec3f *rotation, int arg5, float distance);
bool fn_8001CDA0(UnkPathManager_8001B494 *manager, PathCtrl *path, const TVehicleBase *vehicle,
                 const Vec3f *direction, int arg4, bool (*filter)(const UnkPathRecord_8001CDA0 *),
                 int arg6, float arg7);
bool fn_80018150(PathCtrl *path, const TVehicleBase *vehicle, const Vec3f *direction,
                 int *outDirection, float arg4);
bool fn_80018030(PathCtrl *path);
bool fn_800ABDCC(PathCtrl *path);

/* The zero vector (see Game/GunLift.cpp). */
extern const Vec3f lbl_805E07A0;

/* Vectors turned about an axis (see Game/VehicleAirWingCamera.cpp). */
Vec3f fn_8040D044(const Vec3f *vector, float angle); /* about z */
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* about x */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* about y */
}

/* The vector type's out-of-line assignment (see Game/JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

namespace Vehicle {
namespace BirdBase {

class UnkBirdPathImpl {
  public:
    UnkBirdPathImpl();

    bool IsSet();
    void GetFrame(Vec3f *position, Vec3f *forward, Vec3f *up);
    bool Move(float distance);
    bool Attach(const TVehicleBase *vehicle);
    bool Find(const TVehicleBase *vehicle);

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f m_forward;
    /* 0x18 */ Vec3f m_up;
    /* 0x24 */ PathCtrl m_path;
};
SIZE_ASSERT(UnkBirdPathImpl, 0x7C);

/* The path search's filter (name inferred). */
static bool IsBirdPath(const UnkPathRecord_8001CDA0 *record) {
    return (record->flags & 0x410000) != 0;
}

/* Turns the frame by the path's rotation (name inferred). */
static void RotateFrame(Vec3f *forward, Vec3f *up, const Vec3f *rotation) {
    fn_800091D0(up, &(const Vec3f &)Vec3f(0.0f, 1.0f, 0.0f));
    AssignVector(*up, fn_8040D0AC(up, rotation->y));
    AssignVector(*up, fn_8040D118(up, rotation->x));
    AssignVector(*up, fn_8040D044(up, rotation->z));
    fn_800091D0(forward, &(const Vec3f &)Vec3f(0.0f, 0.0f, -1.0f));
    AssignVector(*forward, fn_8040D0AC(forward, rotation->y));
    AssignVector(*forward, fn_8040D118(forward, rotation->x));
    AssignVector(*forward, fn_8040D044(forward, rotation->z));
}

UnkBirdPathImpl::UnkBirdPathImpl() {}

bool UnkBirdPathImpl::Find(const TVehicleBase *vehicle) {
    if (fn_8001CDA0(fn_8001B494(), &m_path, vehicle, &lbl_805E07A0, 0, IsBirdPath, 0, 0.0f)) {
        return Move(0.0f);
    }
    return false;
}

bool UnkBirdPathImpl::Attach(const TVehicleBase *vehicle) {
    if (fn_80018150(&m_path, vehicle, &lbl_805E07A0, 0, 0.0f)) {
        return Move(0.0f);
    }
    return false;
}

bool UnkBirdPathImpl::Move(float distance) {
    UnkPathManager_8001B494 *manager = fn_8001B494();
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    bool ended = false;
    if (fn_800ABDCC(&m_path)) {
        ended = !fn_8001C91C(manager, &m_path, 0, &m_position, &rotation, 1, distance);
        if (!ended && fn_80018030(&m_path)) {
            ended = true;
        }
        RotateFrame(&m_forward, &m_up, &rotation);
    }
    return ended;
}

void UnkBirdPathImpl::GetFrame(Vec3f *position, Vec3f *forward, Vec3f *up) {
    if (fn_800ABDCC(&m_path)) {
        if (position != 0) {
            AssignVector(*position, m_position);
        }
        if (up != 0) {
            AssignVector(*up, m_up);
        }
        if (forward != 0) {
            AssignVector(*forward, m_forward);
        }
    }
}

bool UnkBirdPathImpl::IsSet() {
    return fn_800ABDCC(&m_path);
}

UnkBirdPath_801B446C::UnkBirdPath_801B446C() : m_impl(new UnkBirdPathImpl) {}

UnkBirdPath_801B446C::~UnkBirdPath_801B446C() {}

} // namespace BirdBase
} // namespace Vehicle

/* The wrapper's methods (names keep their placeholders; see above). The first two are const
 * (retail loads the state before saving the link register). */
extern "C" bool fn_801B4370(UnkBirdPath_801B446C *path, const TVehicleBase *vehicle) {
    return path->m_impl->Find(vehicle);
}

extern "C" bool fn_801B434C(UnkBirdPath_801B446C *path, const TVehicleBase *vehicle) {
    return path->m_impl->Attach(vehicle);
}

extern "C" bool fn_801B4328(UnkBirdPath_801B446C *path, float distance) {
    return path->m_impl->Move(distance);
}

extern "C" void fn_801B4304(const UnkBirdPath_801B446C *path, Vec3f *position, Vec3f *forward,
                            Vec3f *up) {
    path->m_impl->GetFrame(position, forward, up);
}

extern "C" bool fn_801B42E0(const UnkBirdPath_801B446C *path) {
    return path->m_impl->IsSet();
}

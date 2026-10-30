#ifndef GAME_TVEHICLECAMERA_H
#define GAME_TVEHICLECAMERA_H

#include "Game/CameraModeUser.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace PJSCamera {
class ExternalCameraRequest;
namespace Collision {
class ProcessBase;
} // namespace Collision
} // namespace PJSCamera

namespace Vehicle {

/* Vehicle::TVehicleCamera (class name from the RTTI of its bound callbacks;
 * Game/TVehicleCamera.cpp): the camera of a car (Vehicle::TVehicleCar), which owns it through a
 * shared_ptr. Constructor 0x801B7FC4, destructor 0x801B794C; member names are inferred. */
class TVehicleCamera {
  public:
    /* The followed point and its rotation (pointers kept), and the eye's offset from it. */
    TVehicleCamera(const Vec3f *target, const Vec3f *rotation, const Vec3f *offset);
    ~TVehicleCamera();

    /* The camera mode's events (names inferred). */
    void OnSwitchIn(SonicteamUSA::System::Camera::CameraModeUser::SwitchEventArgs &args);
    void OnSwitchOut(SonicteamUSA::System::Camera::CameraModeUser::SwitchEventArgs &args);
    void Update(SonicteamUSA::System::Camera::CameraModeUser::UpdateEventArgs &args);

    /* The distance of the eye (name inferred). An inline function in the original: retail loads it
     * in its place among the arguments. */
    float GetDistance() const {
        return m_state.m_distance;
    }

    /* The camera's state (struct name unknown): its implicit constructor is out of line
     * (0x801B85BC), as the Vec3f members' constructors make it non-trivial. */
    struct State {
        /* 0x00 */ const Vec3f *m_target;
        /* 0x04 */ const Vec3f *m_rotation;
        /* 0x08 */ Vec3f m_offset;
        /* 0x14 */ float m_distance;
        /* 0x18 */ bool m_first;
        /* 0x1C */ Vec3f m_eye;
    };

    /* 0x00 */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_request;
    /* 0x08 */ boost::shared_ptr<SonicteamUSA::System::Camera::CameraModeUser> m_mode;
    /* 0x10 */ boost::shared_ptr<PJSCamera::Collision::ProcessBase> m_collision;
    /* 0x18 */ State m_state;
};
SIZE_ASSERT(TVehicleCamera, 0x40);

} // namespace Vehicle

#endif

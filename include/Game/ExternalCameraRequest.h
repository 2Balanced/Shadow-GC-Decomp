#ifndef GAME_EXTERNALCAMERAREQUEST_H
#define GAME_EXTERNALCAMERAREQUEST_H

#include "Game/CameraModeUser.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

namespace PJSCamera {

/* ExternalCameraRequest (class name from the RTTI of its shared_ptr control block, 0x8053C334):
 * a request that puts a camera mode on the stage camera; fn_801CED24 makes one (0x70 bytes,
 * constructed at 0x801C8B9C) and returns a weak pointer to it, fn_801CEAD4 withdraws it (behaviour inferred from Vehicle::GunLift). Its
 * contents are unknown. weak_ptr<ExternalCameraRequest>'s constructor, destructor, assignment and
 * expired() are shared weak instances (0x800A7120, 0x800A5E28, 0x800A5DE8, 0x801CECF8). */
class ExternalCameraRequest;

} // namespace PJSCamera

extern "C" {
/* Requests mode for the stage camera (the meanings of the integer arguments are unknown). */
boost::weak_ptr<PJSCamera::ExternalCameraRequest>
fn_801CED24(const boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> &mode, int arg1,
            int arg2, int arg3, int arg4, int arg5, int arg6);
/* Withdraws request. */
void fn_801CEAD4(boost::weak_ptr<PJSCamera::ExternalCameraRequest> *request);
}

#endif

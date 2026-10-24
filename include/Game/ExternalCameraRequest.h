#ifndef GAME_EXTERNALCAMERAREQUEST_H
#define GAME_EXTERNALCAMERAREQUEST_H

#include "Game/CameraModeUser.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

namespace PJSCamera {

/* ExternalCameraRequest (class name from the RTTI of its shared_ptr control block, 0x8053C334):
 * a request that puts a camera mode on the stage camera; fn_801CED24 makes one (0x70 bytes) and
 * returns a weak pointer to it, fn_801CEAD4 withdraws it (behaviour inferred from Vehicle::GunLift).
 * weak_ptr<ExternalCameraRequest>'s constructor, destructor, assignment and expired() are shared
 * weak instances (0x800A7120, 0x800A5E28, 0x800A5DE8, 0x801CECF8). Its constructor (which takes
 * the mode by value: fn_801CED24 copies it) and destructor are in Game/StageCameraManager.cpp; the
 * stage camera blends into the mode with the first blend and out of it with the second (member
 * names inferred from that use). */
class ExternalCameraRequest {
  public:
    ExternalCameraRequest(boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> mode,
                          u32 flags, int blendKindIn, const UnkCameraBlend_8000B2C0 *blendIn,
                          int blendKindOut, const UnkCameraBlend_8000B2C0 *blendOut,
                          u32 requestFlags);
    ~ExternalCameraRequest();

    boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> GetMode() {
        return m_mode;
    }

    /* 0x00 */ boost::shared_ptr<SonicteamUSA::System::Camera::CameraMode> m_mode;
    /* 0x08 */ u32 m_flags;
    /* 0x0C */ s32 m_blendKindIn;
    /* 0x10 */ UnkCameraBlend_8000B2C0 m_blendIn;
    /* 0x3C */ s32 m_blendKindOut;
    /* 0x40 */ UnkCameraBlend_8000B2C0 m_blendOut;
    /* 0x6C */ u32 m_requestFlags;
};

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

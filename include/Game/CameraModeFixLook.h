#ifndef GAME_CAMERAMODEFIXLOOK_H
#define GAME_CAMERAMODEFIXLOOK_H

#include "Game/PJSCameraMode.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

namespace PJSCamera {

/* CameraModeFixLook (class name and bases from RTTI; vtable 0x8053BFF8; mode 8 of
 * PJSCamera::PJSCameraFactory): keeps the point looked at of the arguments it was made with and
 * puts the eye beyond the position the screen's camera follows, "distance" away and raised by
 * "height" (Game/CameraModeFixLook.cpp). A collision process keeps the eye clear of the stage.
 * Member names are inferred. */
class CameraModeFixLook : public PJSCameraMode {
  public:
    CameraModeFixLook(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeFixLook();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    /* 0xAC */ float m_distance;
    /* 0xB0 */ float m_height;
    /* 0xB4 */ boost::shared_ptr<Collision::ProcessBase> m_process;
};
SIZE_ASSERT(CameraModeFixLook, 0xBC);

} // namespace PJSCamera

#endif

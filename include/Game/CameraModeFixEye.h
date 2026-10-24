#ifndef GAME_CAMERAMODEFIXEYE_H
#define GAME_CAMERAMODEFIXEYE_H

#include "Game/PJSCameraMode.h"
#include "size_assert.h"

namespace PJSCamera {

/* CameraModeFixEye (class name and bases from RTTI; vtable 0x8053BF78; mode 7 of
 * PJSCamera::PJSCameraFactory): keeps the eye of the placement it was made with and looks at the
 * position the screen's camera follows (Game/CameraModeFixEye.cpp). It adds no members. */
class CameraModeFixEye : public PJSCameraMode {
  public:
    CameraModeFixEye(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeFixEye();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);
};
SIZE_ASSERT(CameraModeFixEye, 0xAC);

} // namespace PJSCamera

#endif

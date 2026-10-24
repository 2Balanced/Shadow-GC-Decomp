#ifndef GAME_CAMERAMODETESTPLAYERNOROT_H
#define GAME_CAMERAMODETESTPLAYERNOROT_H

#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "size_assert.h"

namespace PJSCamera {

/* CameraModeTestPlayerNoRot (class name and bases from RTTI; mode 3 of
 * PJSCamera::PJSCameraFactory): CameraModeTestPlayer that keeps the eye's offset from the player
 * instead of turning with it (Game/CameraModeTestPlayerNoRot.cpp). Member names are inferred. */
class CameraModeTestPlayerNoRot : public PJSCameraMode {
  public:
    CameraModeTestPlayerNoRot(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeTestPlayerNoRot();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    /* 0xAC */ Vec3f m_offset; /* the eye from the player */
};
SIZE_ASSERT(CameraModeTestPlayerNoRot, 0xB8);

} // namespace PJSCamera

#endif

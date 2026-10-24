#ifndef GAME_CAMERAMODETESTPLAYER_H
#define GAME_CAMERAMODETESTPLAYER_H

#include "Game/PJSCameraMode.h"
#include "size_assert.h"

namespace PJSCamera {

/* CameraModeTestPlayer (class name and bases from RTTI; mode 2 of PJSCamera::PJSCameraFactory): a
 * test camera that follows the player within distance and height limits and orbits it with the
 * right stick (Game/CameraModeTestPlayer.cpp). It adds no members. */
class CameraModeTestPlayer : public PJSCameraMode {
  public:
    CameraModeTestPlayer(UnkCameraOwner_8000E9BC *owner, int number)
        : PJSCameraMode(owner, number) {}
    virtual ~CameraModeTestPlayer();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);
};
SIZE_ASSERT(CameraModeTestPlayer, 0xAC);

} // namespace PJSCamera

#endif

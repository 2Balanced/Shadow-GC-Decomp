#ifndef GAME_CAMERAMODELOOKFRONT_H
#define GAME_CAMERAMODELOOKFRONT_H

#include "Game/PJSCameraMode.h"
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

/* The work of CameraModeLookFront (class name unknown; 0x28 bytes, constructor 0x801C557C):
 * Game/CameraModeLookFront.cpp defines it. */
class UnkLookFrontCamera_801C557C;

namespace PJSCamera {

/* CameraModeLookFront (class name and bases from RTTI; mode 11 of PJSCamera::PJSCameraFactory): a
 * camera that looks the way the player runs, from behind and above (Game/CameraModeLookFront.cpp).
 * Its state lives in a separate object the mode owns. */
class CameraModeLookFront : public PJSCameraMode {
  public:
    CameraModeLookFront(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeLookFront();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    /* 0xAC */ boost::scoped_ptr<UnkLookFrontCamera_801C557C> m_impl;
};
SIZE_ASSERT(CameraModeLookFront, 0xB0);

} // namespace PJSCamera

#endif

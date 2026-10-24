#ifndef GAME_CAMERAMODEGENERAL_H
#define GAME_CAMERAMODEGENERAL_H

#include "Game/PJSCameraMode.h"
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

/* The work of CameraModeGeneral (class name unknown; 0x150 bytes, constructor 0x801C4594):
 * Game/CameraModeGeneral.cpp defines it. */
class UnkGeneralCamera_801C4594;

namespace PJSCamera {

/* CameraModeGeneral (class name and bases from RTTI; mode 10 of PJSCamera::PJSCameraFactory): the
 * camera that follows the player around the stage. All its state lives in a separate object the
 * mode owns (Game/CameraModeGeneral.cpp); the mode copies that object's placement each frame. */
class CameraModeGeneral : public PJSCameraMode {
  public:
    CameraModeGeneral(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeGeneral();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);
    virtual void vf_0x18(int flags);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    /* 0xAC */ boost::scoped_ptr<UnkGeneralCamera_801C4594> m_impl;
};
SIZE_ASSERT(CameraModeGeneral, 0xB0);

} // namespace PJSCamera

#endif

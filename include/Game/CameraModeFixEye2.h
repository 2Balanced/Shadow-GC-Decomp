#ifndef GAME_CAMERAMODEFIXEYE2_H
#define GAME_CAMERAMODEFIXEYE2_H

#include "Game/PJSCameraMode.h"
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

/* The rail of CameraModeFixEye2 (struct name unknown; 0x1C bytes; Game/CameraModeFixEye2.cpp
 * defines it). */
struct UnkRail_801C0F90;

namespace PJSCamera {

/* CameraModeFixEye2 (class name and bases from RTTI; vtable 0x8053BFB8; mode 12 of
 * PJSCamera::PJSCameraFactory): puts the eye on the point of a rail nearest to the position the
 * screen's camera follows and looks at that position (Game/CameraModeFixEye2.cpp). */
class CameraModeFixEye2 : public PJSCameraMode {
  public:
    CameraModeFixEye2(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeFixEye2();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    /* 0xAC */ boost::scoped_ptr<UnkRail_801C0F90> m_rail;
};
SIZE_ASSERT(CameraModeFixEye2, 0xB0);

} // namespace PJSCamera

#endif

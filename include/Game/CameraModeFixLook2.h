#ifndef GAME_CAMERAMODEFIXLOOK2_H
#define GAME_CAMERAMODEFIXLOOK2_H

#include "Game/PJSCameraMode.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

/* The rail of CameraModeFixLook2 (struct name unknown; 0x2C bytes; Game/CameraModeFixLook2.cpp
 * defines it). */
struct UnkLookRail_801C1804;

namespace PJSCamera {

/* CameraModeFixLook2 (class name and bases from RTTI; vtable 0x8053C038; mode 13 of
 * PJSCamera::PJSCameraFactory): finds how far the position the screen's camera follows is along a
 * rail from the eye of its arguments, and blends the point looked at, the distance and the height
 * of the eye by that (Game/CameraModeFixLook2.cpp). A collision process keeps the eye clear of the
 * stage. Member names are inferred. */
class CameraModeFixLook2 : public PJSCameraMode {
  public:
    CameraModeFixLook2(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeFixLook2();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    /* 0xAC */ boost::shared_ptr<Collision::ProcessBase> m_process;
    /* 0xB4 */ boost::scoped_ptr<UnkLookRail_801C1804> m_rail;
};
SIZE_ASSERT(CameraModeFixLook2, 0xB8);

} // namespace PJSCamera

#endif

#ifndef GAME_CAMERAMODEFREE_H
#define GAME_CAMERAMODEFREE_H

#include "Game/PJSCameraMode.h"
#include "size_assert.h"

namespace PJSCamera {

/* CameraModeFree (class name and bases from RTTI; vtable 0x8053C078; mode 1 of
 * PJSCamera::PJSCameraFactory): a debug camera flown with the pad while the player ignores input
 * (Game/CameraModeFree.cpp). Member names are inferred. */
class CameraModeFree : public PJSCameraMode {
  public:
    CameraModeFree(UnkCameraOwner_8000E9BC *owner, int number)
        : PJSCameraMode(owner, number), m_speed(2.5f) {}
    virtual ~CameraModeFree();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    /* 0xAC */ float m_speed;
};
SIZE_ASSERT(CameraModeFree, 0xB0);

} // namespace PJSCamera

#endif

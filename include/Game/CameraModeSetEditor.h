#ifndef GAME_CAMERAMODESETEDITOR_H
#define GAME_CAMERAMODESETEDITOR_H

#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "size_assert.h"

namespace PJSCamera {

/* CameraModeSetEditor (class name and bases from RTTI; mode 5 of PJSCamera::PJSCameraFactory): a
 * debug camera moved with the pad, relative to the direction the screen's camera faces
 * (Game/CameraModeSetEditor.cpp). Member names are inferred. */
class CameraModeSetEditor : public PJSCameraMode {
  public:
    CameraModeSetEditor(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeSetEditor();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    /* 0xAC */ Vec3f m_angles; /* x: pitch, y: yaw of the eye about the point looked at */
    /* 0xB8 */ float m_speed;
    /* 0xBC */ float m_distance;
};
SIZE_ASSERT(CameraModeSetEditor, 0xC0);

} // namespace PJSCamera

#endif

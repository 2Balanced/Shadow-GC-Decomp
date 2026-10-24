#ifndef GAME_CAMERAMODESETCAMEDIT_H
#define GAME_CAMERAMODESETCAMEDIT_H

#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "size_assert.h"

namespace PJSCamera {

/* CameraModeSetCamEdit (class name and bases from RTTI; mode 6 of PJSCamera::PJSCameraFactory): a
 * debug camera moved with the pad (Game/CameraModeSetCamEdit.cpp). Member names are inferred. */
class CameraModeSetCamEdit : public PJSCameraMode {
  public:
    CameraModeSetCamEdit(UnkCameraOwner_8000E9BC *owner, int number);
    virtual ~CameraModeSetCamEdit();
    virtual int GetModeId();
    virtual void Update(float dt, UnkCameraUpdateArg_800099B8 *arg);
    virtual void OnSwitch(UnkCameraSwitchArg_8000996C *arg);
    virtual void vf_0x14(UnkCameraArg_800098E4 *arg);

    /* The two ways of moving: with the enabled controls, and freely. */
    void MoveEdit();
    void MoveFree();

    /* Creates the mode for the camera factory (name inferred). */
    static SonicteamUSA::System::Camera::CameraMode *Create(UnkCameraOwner_8000E9BC *owner,
                                                            int number);

    /* 0xAC */ Vec3f m_angles; /* x: pitch, y: yaw of the eye about the point looked at */
    /* 0xB8 */ float m_speed;
    /* 0xBC */ float m_distance;
    /* 0xC0 */ s32 m_moveTarget;
    /* 0xC4 */ s32 m_orbit;
    /* 0xC8 */ s32 m_turn;
    /* 0xCC */ s32 m_zoom;
    /* 0xD0 */ s32 m_editMode;
};
SIZE_ASSERT(CameraModeSetCamEdit, 0xD4);

} // namespace PJSCamera

#endif

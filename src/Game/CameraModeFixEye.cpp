/* PJSCamera::CameraModeFixEye (class name and bases from RTTI; mode 7 of the camera factory;
 * Game/CameraModeFixEye.h): a camera that keeps the eye of the placement it was made with and looks
 * at the position the screen's camera follows. The unit also emits the out-of-line lookup of a
 * screen's camera target, which the stage camera units call. File name from the class; other names
 * are inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/CameraModeFixEye.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CameraTargetInfo.h"
#include "Game/PJSCameraMode.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using PJSCamera::CameraModeFixEye;
using PJSCamera::PJSCameraMode;
using SonicteamUSA::System::Camera::CameraManagerUnit;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;
using SonicteamUSA::System::Camera::CameraTargetInfo;

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
/* The camera unit of a screen (see Game/StageCameraManager.cpp). */
boost::shared_ptr<CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
/* The placement in a mode's arguments (behaviour inferred). */
UnkCameraState_8000AA70 fn_8000B3B4(const UnkCameraModeArgs_8000EBB0 *args);

/* The target a screen's camera follows (out of line here; the stage camera units call this
 * copy). */
inline CameraTargetInfo *fn_801C0B98(UnkCameraManager_80009548 *manager, int index) {
    return fn_8000D0F0(manager, index)->m_target;
}
}

/* A camera mode starts with a placement and keeps its arguments at 0x34. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

inline const UnkCameraModeArgs_8000EBB0 *ModeArgs(CameraMode *mode) {
    return reinterpret_cast<const UnkCameraModeArgs_8000EBB0 *>(mode->field_0x34);
}

namespace PJSCamera {

CameraModeFixEye::CameraModeFixEye(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number) {
    *ModeState(this) = fn_8000B3B4(ModeArgs(this));
}

CameraModeFixEye::~CameraModeFixEye() {}

void CameraModeFixEye::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    CameraTargetInfo *target = fn_801C0B98(fn_80009548(), field_0xA0);
    fn_8000A5D4(this, &m_eye, &target->m_position);
    fn_8000ACE4(reinterpret_cast<CameraModeUser *>(this), &target->field_0x30);
}

void CameraModeFixEye::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {}

void CameraModeFixEye::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeFixEye::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeFixEye(owner, number);
}

int CameraModeFixEye::GetModeId() {
    return 7;
}

} // namespace PJSCamera

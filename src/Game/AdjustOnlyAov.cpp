/* PJSCamera::AdjustOnlyAov (class name from RTTI; Game/PJSCameraAdjust.h; adjust type 3 of
 * PJSCamera::PJSCameraFactory): a camera move between modes that takes the new mode's placement at
 * once. File name from the class; other names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/PJSCameraAdjust.h"
#include "Game/CameraState.h"
#include "size_assert.h"

using PJSCamera::AdjustOnlyAov;
using SonicteamUSA::System::Camera::CameraAdjust;
using SonicteamUSA::System::Camera::CameraMode;

extern "C" {
/* Copies the eye, the point looked at and the field of view of a placement (a camera mode starts
 * with one; behaviour inferred). */
void fn_8000A544(UnkCameraState_8000AA70 *state, const void *placement);
}

AdjustOnlyAov::~AdjustOnlyAov() {}

void AdjustOnlyAov::Start(const UnkCameraState_8000AA70 *from, CameraMode *mode) {}

void AdjustOnlyAov::UpdateEye(CameraMode *mode, float dt) {
    fn_8000A544(&m_state, mode);
}

CameraAdjust *AdjustOnlyAov::Create(const UnkCameraBlend_8000B2C0 &blend) {
    return new AdjustOnlyAov(blend);
}

s32 AdjustOnlyAov::GetType() {
    return 3;
}

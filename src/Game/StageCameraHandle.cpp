/* The general camera mode the stage camera requests over the stage's modes while the camera stick
 * is used (UnkCameraHandle_801CF664, class name unknown; Game/StageCameraManager.h). No class of
 * the unit has RTTI; the file name is inferred from its contents, as are the other names.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/StageCameraManager.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraRequest;

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for the camera requests (boost 1.32's definition). */
namespace boost {
template <> inline void shared_ptr<CameraRequest>::reset() {
    this_type().swap(*this);
}
} // namespace boost

extern "C" {
/* Camera modes and requests (see Game/StageCameraManager.cpp): a mode of a kind with arguments
 * for a screen, a request of a mode with a priority, a blend and flags, and releasing either. */
boost::shared_ptr<CameraMode> fn_8026F6A4(int kind, const UnkCameraModeArgs_8000EBB0 *args,
                                          int player);
void fn_8026F684(boost::shared_ptr<CameraMode> *mode);
boost::shared_ptr<CameraRequest> fn_8026F464(boost::shared_ptr<CameraMode> mode, int priority,
                                             int blendKind, const UnkCameraBlend_8000B2C0 *blend,
                                             u32 flags);
void fn_8026F444(boost::shared_ptr<CameraRequest> *request);
}

/* The time of the blend into the mode (name unknown). */
float lbl_805E87E0 = 0.25f;

UnkCameraHandle_801CF664::UnkCameraHandle_801CF664(int player) : m_active(false) {
    m_request.reset();
    m_mode = fn_8026F6A4(lbl_805F5D18, &lbl_805812E4, player);
}

UnkCameraHandle_801CF664::~UnkCameraHandle_801CF664() {
    fn_8026F444(&m_request);
    fn_8026F684(&m_mode);
}

void UnkCameraHandle_801CF664::Request() {
    if (!IsRequested()) {
        UnkCameraBlend_8000B2C0 blend(lbl_805E87E0);
        m_request = fn_8026F464(m_mode, 0x14, lbl_805E87D8, &blend, 0);
        m_active = true;
    }
}

void UnkCameraHandle_801CF664::Release() {
    fn_8026F444(&m_request);
    m_active = false;
}

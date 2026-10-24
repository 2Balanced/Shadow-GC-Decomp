/* The general camera mode the stage camera keeps ready for a player (UnkCameraModeHolder_801CF3C0,
 * class name unknown; Game/StageCameraManager.h) and the parameters of that mode: its kind, its
 * arguments and its blend, and the blend kind the stage camera uses with it. No class of the unit
 * has RTTI; the file name is inferred from its contents, as are the other names. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/StageCameraManager.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;

/* The vectors, field of view and window the mode starts from (see Game/StageCameraManager.cpp;
 * meanings inferred). */
extern Vec3f lbl_8056EE28;
extern Vec3f lbl_8056EE34;
extern const float lbl_805F1708;
extern Vec2f lbl_805EDFA8;

extern "C" {
/* A camera mode of a kind with arguments for a screen, and releasing one (see
 * Game/StageCameraManager.cpp). */
boost::shared_ptr<CameraMode> fn_8026F6A4(int kind, const UnkCameraModeArgs_8000EBB0 *args,
                                          int player);
void fn_8026F684(boost::shared_ptr<CameraMode> *mode);
}

const int lbl_805F5D18 = 10;
int lbl_805E87D8 = 2;
UnkCameraModeArgs_8000EBB0 lbl_805812E4 =
    UnkCameraModeArgs_8000EBB0(&lbl_8056EE28, &lbl_8056EE34, lbl_805F1708, &lbl_805EDFA8, 1);
UnkCameraBlend_8000B2C0 lbl_8058135C = UnkCameraBlend_8000B2C0(0.2f);

UnkCameraModeHolder_801CF3C0::UnkCameraModeHolder_801CF3C0(int player) {
    m_mode = fn_8026F6A4(lbl_805F5D18, &lbl_805812E4, player);
}

UnkCameraModeHolder_801CF3C0::~UnkCameraModeHolder_801CF3C0() {
    fn_8026F684(&m_mode);
}

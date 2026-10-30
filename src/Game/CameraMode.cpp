/* SonicteamUSA::System::Camera::CameraMode (class name from RTTI; Game/CameraModeUser.h): the base
 * of the camera modes. The unit holds its constructor and destructor, the routines the camera unit
 * calls when a request on a mode starts, ends and runs a frame (fn_8000E8D8, fn_8000E88C,
 * fn_8000E928; the mode counts the requests on it at 0xA4 and is switched to by the first), and
 * the out-of-line members of the parameters a mode is made with (UnkCameraModeArgs_8000EBB0,
 * Game/CameraState.h). It is split from the camera manager before it by the padding at the end of
 * that unit's .data, and from CameraRequest after it by its own .sdata2. File name from the class;
 * other names are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define CAMERAMODE_MEMBERS
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"
#include <string.h>

using SonicteamUSA::System::Camera::CameraMode;

/* A request on a camera mode (Game/CameraManagerUnit.h). */
namespace SonicteamUSA {
namespace System {
namespace Camera {
class CameraRequest;
} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Camera::CameraRequest;

UnkCameraModeArgs_8000EBB0::UnkCameraModeArgs_8000EBB0()
    : m_flags(0), field_0x28(0), field_0x34(0.0f), field_0x38(0.0f), field_0x3C(0.0f), field_0x40(0.0f),
      field_0x44(0.0f), field_0x48(0.0f) {
    memset(&field_0x2C, 0, sizeof(field_0x2C));
    memset(&m_distance, 0, sizeof(UnkCameraModeParams_8000EBB0));
}

UnkCameraModeArgs_8000EBB0::UnkCameraModeArgs_8000EBB0(const Vec3f *eye, const Vec3f *target, float fov,
                                                       const Vec2f *window, u32 flags)
    : m_view(eye, target, fov, window), m_flags(flags), field_0x28(0), field_0x34(0.0f), field_0x38(0.0f),
      field_0x3C(0.0f), field_0x40(0.0f), field_0x44(0.0f), field_0x48(0.0f) {
    memset(&field_0x2C, 0, sizeof(field_0x2C));
    memset(&m_distance, 0, sizeof(UnkCameraModeParams_8000EBB0));
}

UnkCameraModeArgs_8000EBB0::~UnkCameraModeArgs_8000EBB0() {}

/* Blocks of two and three words the assignment copies whole. */
struct UnkCameraWords2_8000EA28 {
    u32 m_words[2];
};
struct UnkCameraWords3_8000EA28 {
    u32 m_words[3];
};
#define BLOCK(T, x) (*reinterpret_cast<T *>(&(x)))
#define CONST_BLOCK(T, x) (*reinterpret_cast<const T *>(&(x)))

/* The parameters' assignment (implicit in the game: the placement member by member, the rest as
 * words and the eight parameters from 0x4C as one block). */
extern "C" UnkCameraModeArgs_8000EBB0 *fn_8000EA28(UnkCameraModeArgs_8000EBB0 *args,
                                                   const UnkCameraModeArgs_8000EBB0 *from) {
    args->m_view.m_eyeX = from->m_view.m_eyeX;
    args->m_view.m_eyeY = from->m_view.m_eyeY;
    args->m_view.m_eyeZ = from->m_view.m_eyeZ;
    args->m_view.m_targetX = from->m_view.m_targetX;
    args->m_view.m_targetY = from->m_view.m_targetY;
    args->m_view.m_targetZ = from->m_view.m_targetZ;
    args->m_view.m_fov = from->m_view.m_fov;
    args->m_view.m_windowX = from->m_view.m_windowX;
    args->m_view.m_windowY = from->m_view.m_windowY;
    args->m_flags = from->m_flags;
    args->field_0x28 = from->field_0x28;
    BLOCK(UnkCameraWords2_8000EA28, args->field_0x2C) = CONST_BLOCK(UnkCameraWords2_8000EA28, from->field_0x2C);
    BLOCK(UnkCameraWords3_8000EA28, args->field_0x34) = CONST_BLOCK(UnkCameraWords3_8000EA28, from->field_0x34);
    BLOCK(UnkCameraWords3_8000EA28, args->field_0x40) = CONST_BLOCK(UnkCameraWords3_8000EA28, from->field_0x40);
    *reinterpret_cast<UnkCameraModeParams_8000EBB0 *>(&args->m_distance) =
        *reinterpret_cast<const UnkCameraModeParams_8000EBB0 *>(&from->m_distance);
    return args;
}

namespace SonicteamUSA {
namespace System {
namespace Camera {

CameraMode::CameraMode(UnkCameraOwner_8000E9BC *owner, int screen) : field_0xA0(screen), field_0xA4(0) {
    if (owner != 0) {
        fn_8000EA28(&m_args, reinterpret_cast<const UnkCameraModeArgs_8000EBB0 *>(owner));
    }
}

CameraMode::~CameraMode() {}

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

/* A request's frame, start and end on a mode (the first request switches to the mode, the last
 * one's end calls slot 0x14). */
extern "C" void fn_8000E928(CameraMode *mode, CameraRequest *request, float dt) {
    mode->Update(dt, reinterpret_cast<UnkCameraUpdateArg_800099B8 *>(request));
}

extern "C" void fn_8000E8D8(CameraMode *mode, CameraRequest *request) {
    if ((s32)mode->field_0xA4 == 0) {
        mode->OnSwitch(reinterpret_cast<UnkCameraSwitchArg_8000996C *>(request));
    }
    mode->field_0xA4++;
}

extern "C" void fn_8000E88C(CameraMode *mode, CameraRequest *request) {
    if ((s32)mode->field_0xA4 > 0) {
        mode->field_0xA4--;
        if ((s32)mode->field_0xA4 == 0) {
            mode->vf_0x14(reinterpret_cast<UnkCameraArg_800098E4 *>(request));
        }
    }
}

/* The out-of-line members of a camera blend (UnkCameraBlend_8000B2C0, Game/CameraModeUser.h) and of
 * the placement a camera mode is made with (UnkCameraView_8000B4AC and the readers of
 * UnkCameraModeArgs_8000EBB0, Game/CameraState.h), which the camera units call. The unit has no
 * class with RTTI; it is split from CameraAdjust by its own 0.0f in .sdata2 (a unit pools equal
 * constants, and CameraAdjust has one) and from CameraFactory by the padding before that unit's
 * strings. File name and function names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"
#include <string.h>

/* The default placement (Game/CameraState.cpp). */
extern const Vec3f lbl_8056EE28;
extern const Vec3f lbl_8056EE34;
extern const float lbl_805F1708;
extern const Vec2f lbl_805EDFA8;

UnkCameraView_8000B4AC::UnkCameraView_8000B4AC() {
    m_eyeX = lbl_8056EE28.x;
    m_eyeY = lbl_8056EE28.y;
    m_eyeZ = lbl_8056EE28.z;
    m_targetX = lbl_8056EE34.x;
    m_targetY = lbl_8056EE34.y;
    m_targetZ = lbl_8056EE34.z;
    m_fov = lbl_805F1708;
    m_windowX = lbl_805EDFA8.x;
    m_windowY = lbl_805EDFA8.y;
}

UnkCameraView_8000B4AC::UnkCameraView_8000B4AC(const Vec3f *eye, const Vec3f *target, float fov,
                                               const Vec2f *window) {
    m_eyeX = eye->x;
    m_eyeY = eye->y;
    m_eyeZ = eye->z;
    m_targetX = target->x;
    m_targetY = target->y;
    m_targetZ = target->z;
    m_fov = fov;
    m_windowX = window->x;
    m_windowY = window->y;
}

UnkCameraView_8000B4AC::~UnkCameraView_8000B4AC() {}

/* A mode's placement from the parameters it is made with, and their eye and point looked at. */
extern "C" UnkCameraState_8000AA70 fn_8000B3B4(const UnkCameraModeArgs_8000EBB0 *args) {
    return UnkCameraState_8000AA70(
        &(const Vec3f &)Vec3f(args->m_view.m_eyeX, args->m_view.m_eyeY, args->m_view.m_eyeZ),
        &(const Vec3f &)Vec3f(args->m_view.m_targetX, args->m_view.m_targetY, args->m_view.m_targetZ),
        args->m_view.m_fov, &(const Vec2f &)Vec2f(args->m_view.m_windowX, args->m_view.m_windowY));
}

extern "C" Vec3f fn_8000B398(const UnkCameraModeArgs_8000EBB0 *args) {
    return Vec3f(args->m_view.m_eyeX, args->m_view.m_eyeY, args->m_view.m_eyeZ);
}

extern "C" Vec3f fn_8000B37C(const UnkCameraModeArgs_8000EBB0 *args) {
    return Vec3f(args->m_view.m_targetX, args->m_view.m_targetY, args->m_view.m_targetZ);
}

UnkCameraBlend_8000B2C0::UnkCameraBlend_8000B2C0() {
    m_time = 0.0f;
    field_0x4 = 0.0f;
    field_0x8 = 0;
    memset(field_0xC, 0, sizeof(field_0xC));
    memset(field_0x1C, 0, sizeof(field_0x1C));
}

UnkCameraBlend_8000B2C0::UnkCameraBlend_8000B2C0(float time) {
    m_time = time;
    field_0x4 = time;
    field_0x8 = 0;
    memset(field_0xC, 0, sizeof(field_0xC));
    memset(field_0x1C, 0, sizeof(field_0x1C));
}

UnkCameraBlend_8000B2C0::UnkCameraBlend_8000B2C0(float time, float value) {
    m_time = time;
    field_0x4 = value;
    field_0x8 = 0;
    memset(field_0xC, 0, sizeof(field_0xC));
    memset(field_0x1C, 0, sizeof(field_0x1C));
}

UnkCameraBlend_8000B2C0::~UnkCameraBlend_8000B2C0() {}

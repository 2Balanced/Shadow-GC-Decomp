/* PJSCamera::AdjustLinerWorld (class name from RTTI; Game/PJSCameraAdjust.h; adjust type 1 of
 * PJSCamera::PJSCameraFactory): a camera move between modes that moves the eye and the point
 * looked at from the old placement to the new one in straight lines and blends the field of view.
 * File name from the class; other names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#include "Game/PJSCameraAdjust.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"
#include "size_assert.h"

using PJSCamera::AdjustLinerWorld;
using SonicteamUSA::System::Camera::CameraAdjust;
using SonicteamUSA::System::Camera::CameraMode;

extern "C" {
/* The directions of a placement's look and its field of view (behaviour inferred; a camera mode
 * starts with a placement). */
Vec3f fn_8000A610(const void *placement);
/* Sets a placement's eye, point looked at and field of view (see Game/AdjustCosRot.cpp). */
void fn_8000A588(UnkCameraState_8000AA70 *state, const Vec3f *eye, const Vec3f *target, float fov);
/* a + b, v *= scale (see Game/AdjustCosRot.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CE38(Vec3f *v, float scale);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

AdjustLinerWorld::~AdjustLinerWorld() {}

void AdjustLinerWorld::Start(const UnkCameraState_8000AA70 *from, CameraMode *mode) {}

void AdjustLinerWorld::UpdateEye(CameraMode *mode, float dt) {
    const Vec3f *fromEye = &m_from.m_eye;
    const Vec3f *fromTarget = &m_from.m_target;
    float to = fn_8000A610(mode).z;
    float from = fn_8000A610(&m_from).z;
    float fov = m_eyeRate * (to - from) + from;
    Vec3f eye;
    fn_800091EC(&eye, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&mode->m_eye, fromEye));
    Vec3f target;
    fn_800091EC(&target,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&mode->m_target, fromTarget));
    fn_8040CE38(&eye, m_eyeRate);
    fn_8040CE38(&target, m_eyeRate);
    fn_8000A588(&m_state, &(const Vec3f &)fn_8040CDD0(&eye, fromEye),
                &(const Vec3f &)fn_8040CDD0(&target, fromTarget), fov);
}

CameraAdjust *AdjustLinerWorld::Create(const UnkCameraBlend_8000B2C0 &blend) {
    return new AdjustLinerWorld(blend);
}

s32 AdjustLinerWorld::GetType() {
    return 1;
}

/* PJSCamera::AdjustLinerLocal (class name from RTTI; Game/PJSCameraAdjust.h; adjust type 2 of
 * PJSCamera::PJSCameraFactory): a camera move between modes that blends the eye's offset from the
 * player, the direction looked in and the distance to the point looked at linearly. File name from
 * the class; other names are inferred. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/PJSCameraAdjust.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraState.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using PJSCamera::AdjustLinerLocal;
using SonicteamUSA::System::Camera::CameraAdjust;
using SonicteamUSA::System::Camera::CameraManagerUnit;
using SonicteamUSA::System::Camera::CameraMode;

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
/* The camera unit of a screen and the position its camera follows (see
 * Game/StageCameraManager.cpp). */
boost::shared_ptr<CameraManagerUnit> fn_8000D0F0(UnkCameraManager_80009548 *manager, int index);
const Vec3f *fn_801C0B98(UnkCameraManager_80009548 *manager, int index);
/* The player of a screen (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int index);
/* Sets a placement's eye, point looked at and field of view (see Game/AdjustCosRot.cpp). */
void fn_8000A588(UnkCameraState_8000AA70 *state, const Vec3f *eye, const Vec3f *target, float fov);
/* v * scale, a + b, v normalised in place (returns the former length), v *= scale (see
 * Game/AdjustCosRot.cpp). */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *v, float scale);

/* The last placement of a screen's camera unit, with the position it followed (out of line here;
 * Game/AdjustCosRot.cpp calls this copy). */
inline UnkCameraState_80359164 *fn_801CFA4C(UnkCameraManager_80009548 *manager, int index) {
    CameraManagerUnit *unit = fn_8000D0F0(manager, index).get();
    return &unit->m_lastState;
}
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* Copies value into vector through the game's out-of-line assignment (as in Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A camera mode starts with a placement. */
inline float &ModeFov(CameraMode *mode) {
    return *reinterpret_cast<float *>(mode->field_0x18);
}

AdjustLinerLocal::AdjustLinerLocal(const UnkCameraBlend_8000B2C0 &blend) : CameraAdjust(blend) {}

AdjustLinerLocal::~AdjustLinerLocal() {}

void AdjustLinerLocal::Start(const UnkCameraState_8000AA70 *from, CameraMode *mode) {
    UnkCameraState_80359164 *last = fn_801CFA4C(fn_80009548(), mode->field_0xA0);
    AssignVector(m_startOffset,
                 ((VectorSubtractFn)fn_8040CD74)(&m_from.m_eye, &last->m_targetPosition));
    AssignVector(m_startDirection, ((VectorSubtractFn)fn_8040CD74)(&m_from.m_target, &m_from.m_eye));
    m_startDistance = fn_8040CF50(&m_startDirection);
    fn_8040CE74(&m_startDirection);
}

void AdjustLinerLocal::UpdateEye(CameraMode *mode, float dt) {
    float fov = m_eyeRate * (ModeFov(mode) - m_from.m_fov) + m_from.m_fov;
    const Vec3f *target = &mode->m_target;
    boost::shared_ptr<CharInfo> player(fn_800A8C54(fn_80074194(), mode->field_0xA0).lock());
    if (player.get() != 0) {
        Vec3f eye;
        Vec3f look;
        const Vec3f *center = fn_801C0B98(fn_80009548(), mode->field_0xA0);
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&mode->m_eye, center));
        AssignVector(eye, fn_8040CDD0(&(const Vec3f &)fn_8040CDD0(
                                          &(const Vec3f &)fn_8040CDA8(&m_startOffset,
                                                                      1.0f - m_eyeRate),
                                          &(const Vec3f &)fn_8040CDA8(&offset, m_eyeRate)),
                                      center));
        Vec3f direction;
        fn_800091EC(&direction,
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(target, &mode->m_eye));
        float length = fn_8040CF50(&direction);
        float distance = m_startDistance * (1.0f - m_eyeRate) + length * m_eyeRate;
        fn_8040CE74(&direction);
        AssignVector(look, fn_8040CDD0(&(const Vec3f &)fn_8040CDA8(&m_startDirection,
                                                                   1.0f - m_eyeRate),
                                       &(const Vec3f &)fn_8040CDA8(&direction, m_eyeRate)));
        fn_8040CE74(&look);
        fn_8040CE38(&look, distance);
        fn_8000A588(&m_state, &eye, &(const Vec3f &)fn_8040CDD0(&eye, &look), fov);
        return;
    }
    fn_8000A588(&m_state, &mode->m_eye, target, fov);
}

CameraAdjust *AdjustLinerLocal::Create(const UnkCameraBlend_8000B2C0 &blend) {
    return new AdjustLinerLocal(blend);
}

s32 AdjustLinerLocal::GetType() {
    return 2;
}

/* PJSCamera::AdjustCosRot (class name from RTTI; adjust type 4 of PJSCamera::PJSCameraFactory): a
 * camera move between modes that eases in and out along a cosine and swings the eye about the
 * player instead of moving it in a straight line. The eye's offset from the player and the
 * direction looked in are each split along the camera's up vector; the parts along it are blended
 * linearly, the parts across it are turned from the old direction to the new one and their
 * lengths blended. The unit also holds the out-of-line constructor and destructor of
 * UnkCameraState_80359164 (Game/CameraState.h), which follow it in the binary. File name from
 * the class; other names are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CameraAdjust.h"
#include "Game/CameraState.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraAdjust;
using SonicteamUSA::System::Camera::CameraMode;

/* The float overload of std::cos, out of line in the game (as in Game/BAMine.cpp). */
namespace std {
float cos(float x);
} // namespace std

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* What fn_801CFA4C returns for a screen (class name unknown): the player's position at 0x40. */
struct UnkCameraTarget_801CFA4C {
    /* 0x00 */ u8 field_0x0[0x40];
    /* 0x40 */ Vec3f m_position;
};

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
/* The position the camera of a screen follows, and the object behind it (behaviour inferred). */
const Vec3f *fn_801C0B98(UnkCameraManager_80009548 *manager, int index);
UnkCameraTarget_801CFA4C *fn_801CFA4C(UnkCameraManager_80009548 *manager, int index);
/* The player of a screen (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int index);
/* Sets a placement's eye, point looked at and field of view (behaviour inferred). */
void fn_8000A588(UnkCameraState_8000AA70 *state, const Vec3f *eye, const Vec3f *target, float fov);
/* Splits vector into its parts along and across normal (see Game/ArkCannon.cpp). */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *normal);
/* The linear blend from a to b at t, the angle between a and b, the rotation by angle about axis
 * (behaviour inferred). */
Vec3f fn_8040F268(const Vec3f *a, const Vec3f *b, float t);
float fn_8040F8FC(const Vec3f *a, const Vec3f *b);
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);
/* matrix * point, a + b, a x b, |v|, v normalised in place (returns the former length),
 * v *= scale. */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);
float fn_8040CF50(const Vec3f *v);
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *v, float scale);
}

extern const float lbl_805FAD38; /* pi */

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

/* Whether v is (nearly) the zero vector. */
inline bool IsZero(const Vec3f &v) {
    return v.x * v.x + v.y * v.y + v.z * v.z < 1e-4f;
}

UnkCameraState_80359164::UnkCameraState_80359164() {
    fn_800091EC(&m_velocity, &lbl_805E07A0);
    fn_800091EC(&m_targetPosition, &lbl_805E07A0);
}

UnkCameraState_80359164::~UnkCameraState_80359164() {}

/* from turned towards to by t about up (see the file comment). */
static Vec3f Swing(const Vec3f &from, const Vec3f &to, const Vec3f &up, float t) {
    Vec3f fromAlong;
    Vec3f fromAcross;
    fn_8040F990(&fromAlong, &fromAcross, &from, &up);
    Vec3f toAlong;
    Vec3f toAcross;
    fn_8040F990(&toAlong, &toAcross, &to, &up);
    Vec3f along;
    AssignVector(along, fn_8040F268(&fromAlong, &toAlong, t));
    float fromLength = fn_8040CF50(&fromAcross);
    float length = t * (fn_8040CF50(&toAcross) - fromLength) + fromLength;
    Vec3f across;
    if (!IsZero(fromAcross) && !IsZero(toAcross)) {
        fn_8040CE74(&fromAcross);
        fn_8040CE74(&toAcross);
        Vec3f axis;
        fn_800091EC(&axis, &(const Vec3f &)fn_8040CFB8(&fromAcross, &toAcross));
        if (IsZero(axis)) {
            AssignVector(axis, up);
        }
        fn_8040CE74(&axis);
        float angle = fn_8040F8FC(&fromAcross, &toAcross);
        AssignVector(across, fn_8040B67C(&(const Mtx34f &)fn_8040FD8C(&axis, angle * t), &fromAcross));
        fn_8040CE38(&across, length);
    } else {
        AssignVector(across, toAcross);
    }
    return fn_8040CDD0(&across, &along);
}

namespace PJSCamera {

class AdjustCosRot : public CameraAdjust {
  public:
    AdjustCosRot(const UnkCameraBlend_8000B2C0 &blend);
    virtual s32 GetType();
    virtual void Start(const UnkCameraState_8000AA70 *from, CameraMode *mode);
    virtual void UpdateEye(CameraMode *mode, float dt);
    virtual float GetEyeRate(float time, float duration);
    virtual ~AdjustCosRot();

    /* The camera factory's maker of this adjust. */
    static CameraAdjust *Create(const UnkCameraBlend_8000B2C0 &blend);

    /* 0xB0 */ Vec3f m_startOffset;   /* the eye from the player */
    /* 0xBC */ Vec3f m_startDirection; /* looked in, normalised */
    /* 0xC8 */ float m_startDistance;  /* to the point looked at */
};
SIZE_ASSERT(AdjustCosRot, 0xCC);

AdjustCosRot::AdjustCosRot(const UnkCameraBlend_8000B2C0 &blend) : CameraAdjust(blend) {}

AdjustCosRot::~AdjustCosRot() {}

void AdjustCosRot::Start(const UnkCameraState_8000AA70 *from, CameraMode *mode) {
    UnkCameraTarget_801CFA4C *target = fn_801CFA4C(fn_80009548(), mode->field_0xA0);
    AssignVector(m_startOffset, ((VectorSubtractFn)fn_8040CD74)(&m_from.m_eye, &target->m_position));
    AssignVector(m_startDirection, ((VectorSubtractFn)fn_8040CD74)(&m_from.m_target, &m_from.m_eye));
    m_startDistance = fn_8040CF50(&m_startDirection);
    fn_8040CE74(&m_startDirection);
}

void AdjustCosRot::UpdateEye(CameraMode *mode, float dt) {
    float fov = m_eyeRate * (ModeFov(mode) - m_from.m_fov) + m_from.m_fov;
    boost::shared_ptr<CharInfo> player(fn_800A8C54(fn_80074194(), mode->field_0xA0).lock());
    if (player.get() != 0) {
        Vec3f eye;
        Vec3f look;
        const Vec3f *up;
        const Vec3f *center = fn_801C0B98(fn_80009548(), mode->field_0xA0);
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&mode->m_eye, center));
        up = &m_state.m_up;
        AssignVector(eye, fn_8040CDD0(&(const Vec3f &)Swing(m_startOffset, offset, *up, m_eyeRate),
                                      center));
        Vec3f direction;
        fn_800091EC(&direction,
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&mode->m_target, &mode->m_eye));
        float length = fn_8040CF50(&direction);
        float distance = m_eyeRate * (length - m_startDistance) + m_startDistance;
        fn_8040CE74(&direction);
        AssignVector(look, Swing(m_startDirection, direction, *up, m_eyeRate));
        fn_8040CE74(&look);
        fn_8040CE38(&look, distance);
        fn_8000A588(&m_state, &eye, &(const Vec3f &)fn_8040CDD0(&eye, &look), fov);
    } else {
        fn_8000A588(&m_state, &mode->m_eye, &mode->m_target, fov);
    }
}

float AdjustCosRot::GetEyeRate(float time, float duration) {
    float rate = std::cos(lbl_805FAD38 * (time / duration));
    return 1.0f - (rate + 1.0f) * 0.5f;
}

CameraAdjust *AdjustCosRot::Create(const UnkCameraBlend_8000B2C0 &blend) {
    return new AdjustCosRot(blend);
}

s32 AdjustCosRot::GetType() {
    return 4;
}

} // namespace PJSCamera

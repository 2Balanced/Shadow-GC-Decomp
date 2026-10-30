/* Vehicle::TVehicleCamera (class name from the RTTI of its bound callbacks): the camera of a car
 * (Vehicle::TVehicleCar). It pushes a user camera mode (CameraModeUser) whose eye sits at an offset
 * from the car, turned with it, and follows the car with a lag, at a fixed distance; a collision
 * process keeps the eye out of the stage. The car owns it through a shared_ptr. Member names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). The constructor reaches .data through a pooled base address, so the linker
 * keeps that section whole: Game/CharaColliBaseCharInfo.h supplies the RTTI base list it starts
 * with. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Game/CameraModeUser.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "Game/TVehicleCamera.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;
using Vehicle::TVehicleCamera;

extern "C" {
/* A camera mode's eye and target (see Game/CameraModeGeneral.cpp). */
void fn_8000A5D4(CameraMode *mode, const Vec3f *eye, const Vec3f *target);
/* Releases a user camera mode (see Game/GunLift.cpp). */
void fn_80009618(boost::shared_ptr<CameraModeUser> *mode);

/* Vector helpers (see Game/VehicleAirWingCamera.cpp). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040CEE8(const Vec3f *vector); /* normalized */
float fn_8040CF50(const Vec3f *v);      /* |v| */
Vec3f fn_8040D044(const Vec3f *vector, float angle); /* turned about z */
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* turned about x */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about y */
}

/* fn_8040CD74 called as returning a - b by value (see Game/GunLift.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The vector type's out-of-line assignment (see Game/GunLift.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Sets a camera mode's eye and target, and a vector scaled (names inferred). Inline functions in
 * the original: retail calls the shared_ptr's operator-> out of line before the first and loads
 * the distance ahead of the normalization in the second's arguments. */
inline void SetEye(CameraMode *mode, const Vec3f *eye, const Vec3f *target) {
    fn_8000A5D4(mode, eye, target);
}

inline Vec3f Scaled(const Vec3f &vector, float scale) {
    return fn_8040CDA8(&vector, scale);
}

namespace Vehicle {

TVehicleCamera::TVehicleCamera(const Vec3f *target, const Vec3f *rotation, const Vec3f *offset) {
    m_mode = fn_80009704(0, 0);
    fn_800A6A24(&m_mode.get()->m_onSwitch,
                &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                    boost::bind(&TVehicleCamera::OnSwitchIn, this, _1)));
    fn_800A673C(&m_mode.get()->m_onUpdate,
                &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                    boost::bind(&TVehicleCamera::Update, this, _1)));
    fn_800A6A24(reinterpret_cast<CameraModeUser::SwitchCallback *>(m_mode.get()->field_0xB8),
                &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                    boost::bind(&TVehicleCamera::OnSwitchOut, this, _1)));
    m_request = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 0, 0, -1, 0, 0);
    m_state.m_target = target;
    m_state.m_rotation = rotation;
    AssignVector(m_state.m_offset, *offset);
    m_state.m_distance = fn_8040CF50(offset);
    m_state.m_first = true;
    m_collision = fn_801C6EB0();
}

/* How fast the eye follows the car, per frame (name inferred). Defined after the constructor:
 * retail keeps it after the bound callbacks' type records in .sdata. */
static float s_follow = 0.025f;

TVehicleCamera::~TVehicleCamera() {
    fn_801CEAD4(&m_request);
    fn_80009618(&m_mode);
}

void TVehicleCamera::OnSwitchIn(CameraModeUser::SwitchEventArgs &args) {}

void TVehicleCamera::OnSwitchOut(CameraModeUser::SwitchEventArgs &args) {}

void TVehicleCamera::Update(CameraModeUser::UpdateEventArgs &args) {
    Vec3f offset;
    fn_800091EC(&offset, &(const Vec3f &)fn_8040D0AC(&m_state.m_offset, m_state.m_rotation->y));
    fn_800091D0(&offset, &(const Vec3f &)fn_8040D118(&offset, m_state.m_rotation->x));
    fn_800091D0(&offset, &(const Vec3f &)fn_8040D044(&offset, m_state.m_rotation->z));
    Vec3f eye;
    fn_800091EC(&eye, &(const Vec3f &)fn_8040CDD0(&offset, m_state.m_target));
    if (m_state.m_first) {
        fn_800091D0(&m_state.m_eye, &eye);
        m_state.m_first = false;
    } else {
        Vec3f follow;
        fn_800091D0(&follow, &(const Vec3f &)fn_8040CDD0(
                                 &(const Vec3f &)fn_8040CDA8(&eye, s_follow),
                                 &(const Vec3f &)fn_8040CDA8(&m_state.m_eye, 1.0f - s_follow)));
        Vec3f direction;
        fn_800091EC(&direction, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                                    &follow, m_state.m_target));
        fn_800091D0(&m_state.m_eye,
                    &(const Vec3f &)fn_8040CDD0(
                        m_state.m_target,
                        &(const Vec3f &)Scaled(fn_8040CEE8(&direction), GetDistance())));
    }
    SetEye(m_mode.operator->(), &m_state.m_eye, m_state.m_target);
    if (m_collision) {
        /* The process gets the target and the eye (the declaration's parameter names follow
         * Game/CameraModeFixLook.cpp). */
        m_collision->Process(m_mode.get(), const_cast<Vec3f *>(m_state.m_target),
                             reinterpret_cast<CharInfo *>(&m_state.m_eye));
    }
}

} // namespace Vehicle

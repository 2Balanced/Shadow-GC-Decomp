/* Vehicle::AirWingCameraImpl (class name from the RTTI of its bound callbacks): the camera of a
 * bird (Vehicle::BirdBase::BirdBase, the AirBolt and the AirWing) on its path. It pushes a user
 * camera mode (CameraModeUser) whose eye sits behind the bird's path point, turned with the path,
 * and which leans toward the bird as it strays from the path, within the screen's extent. Its owner
 * is the pimpl wrapper Vehicle::BirdBase::UnkBirdTrail_801B3148 (Game/AirWingCamera.h), which
 * BirdBase creates when the bird takes off. File name from the class (prefixed like
 * Game/VehicleAirWing.cpp); member names are inferred. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). The constructor reaches .data through a
 * pooled base address, so the linker keeps that section whole: Game/CharaColliBaseCharInfo.h
 * supplies the RTTI base list it starts with. */
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include "Game/AirWingCamera.h"
#include "Game/BirdBase.h"
#include "Game/CameraModeUser.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/ExternalCameraRequest.h"
#include "Game/MathTypes.h"
#include "Game/TVehicleBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;
using Vehicle::TVehicleBase;
using Vehicle::BirdBase::UnkBirdPath_801B446C;
using Vehicle::BirdBase::UnkBirdTrail_801B3148;

/* The float fabs of <cmath> (out of line; the copy linked is another unit's). */
namespace std {
float fabs(float x);
} // namespace std

extern "C" {
/* The bird's path (see Game/BirdBase.cpp): whether it is set, and the point nearest the bird and
 * its frame (any output may be null). */
bool fn_801B42E0(UnkBirdPath_801B446C *path);
void fn_801B4304(UnkBirdPath_801B446C *path, Vec3f *position, Vec3f *forward, Vec3f *up);

/* A user camera mode's eye, its angles and the extent of the screen at a point (behaviour
 * inferred; see Game/GunLift.cpp and Game/CameraModeFree.cpp). */
void fn_8000DB78(CameraModeUser *mode, const Vec3f *eye);
void fn_8000A304(CameraMode *mode, const Vec3f *angles);
Vec3f fn_8000F57C(const CameraModeUser *mode, const Vec3f *point, int viewport);
/* Releases a user camera mode (see Game/GunLift.cpp). */
void fn_80009618(boost::shared_ptr<CameraModeUser> *mode);

/* Vector helpers (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
float fn_8040CF50(const Vec3f *v);                 /* |v| */
void fn_8040CE38(Vec3f *vector, float scale);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
Vec3f fn_8040D044(const Vec3f *vector, float angle); /* turned about z */
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* turned about x */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about y */
Vec3f fn_8040F364(const Vec3f *forward, const Vec3f *up); /* Euler angles */
void fn_8040F990(Vec3f *along, Vec3f *across, const Vec3f *vector, const Vec3f *axis);
Vec3f fn_80093204(const Vec3f *a, const Vec3f *b);
}

/* fn_8040CD74 called as returning a - b by value (see Game/GunLift.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* The vector type's out-of-line copy constructor and assignment (see Game/GunLift.cpp). */
inline void ConstructVector(Vec3f *vector, const Vec3f &value) {
    fn_800091EC(vector, &value);
}

inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Sets a camera mode's angles (name inferred). An inline function in the original: retail calls
 * the shared_ptr's operator-> out of line before it. */
inline void SetAngles(CameraMode *mode, const Vec3f *angles) {
    fn_8000A304(mode, angles);
}

namespace Vehicle {

class AirWingCameraImpl {
  public:
    AirWingCameraImpl(TVehicleBase *vehicle, UnkBirdPath_801B446C *path);
    ~AirWingCameraImpl();

    /* Places the camera (name inferred). */
    void Update();

    /* The camera mode's events (names inferred). */
    void OnSwitchIn(CameraModeUser::SwitchEventArgs &args);
    void OnSwitchOut(CameraModeUser::SwitchEventArgs &args);
    void OnUpdate(CameraModeUser::UpdateEventArgs &args);

    /* 0x00 */ TVehicleBase *m_vehicle;
    /* 0x04 */ UnkBirdPath_801B446C *m_path;
    /* 0x08 */ boost::weak_ptr<PJSCamera::ExternalCameraRequest> m_request;
    /* 0x10 */ boost::shared_ptr<CameraModeUser> m_mode;
};
SIZE_ASSERT(AirWingCameraImpl, 0x18);

} // namespace Vehicle

using Vehicle::AirWingCameraImpl;

/* Deletes the camera (name inferred; see checked_delete below). */
inline void DeleteAirWingCamera(AirWingCameraImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<AirWingCameraImpl> out of line (0x801B3124) and the
 * destructor at its definition; see Game/Rocket.cpp and docs/research-notes.md. */
namespace boost {
template <> inline void checked_delete<AirWingCameraImpl>(AirWingCameraImpl *x) {
    DeleteAirWingCamera(x);
}
} // namespace boost

namespace Vehicle {

AirWingCameraImpl::AirWingCameraImpl(TVehicleBase *vehicle, UnkBirdPath_801B446C *path)
    : m_vehicle(vehicle), m_path(path) {
    m_mode = fn_80009704(0, 0);
    fn_800A6A24(&m_mode.get()->m_onSwitch,
                &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                    boost::bind(&AirWingCameraImpl::OnSwitchIn, this, _1)));
    fn_800A673C(&m_mode.get()->m_onUpdate,
                &(const CameraModeUser::UpdateCallback &)CameraModeUser::UpdateCallback(
                    boost::bind(&AirWingCameraImpl::OnUpdate, this, _1)));
    fn_800A6A24(reinterpret_cast<CameraModeUser::SwitchCallback *>(m_mode.get()->field_0xB8),
                &(const CameraModeUser::SwitchCallback &)CameraModeUser::SwitchCallback(
                    boost::bind(&AirWingCameraImpl::OnSwitchOut, this, _1)));
    UnkCameraBlend_8000B2C0 blend(0.5f);
    m_request = fn_801CED24(boost::shared_ptr<CameraMode>(m_mode), 0, 2, (int)&blend, -1, 0, 0);
}

AirWingCameraImpl::~AirWingCameraImpl() {
    fn_801CEAD4(&m_request);
    if (m_mode.get() != 0) {
        fn_80009618(&m_mode);
    }
}

void AirWingCameraImpl::Update() {
    if (m_mode.get() != 0 && fn_801B42E0(m_path)) {
        Vec3f position;
        Vec3f forward;
        Vec3f up;
        Vec3f angles(0.0f, 0.0f, 0.0f);
        fn_801B4304(m_path, &position, &forward, &up);
        angles = fn_8040F364(&forward, &up);
        Vec3f offset(0.0f, 0.0f, 75.0f);
        AssignVector(offset, fn_8040D044(&offset, angles.z));
        AssignVector(offset, fn_8040D118(&offset, angles.x));
        AssignVector(offset, fn_8040D0AC(&offset, angles.y));
        Vec3f eye;
        fn_800091EC(&eye, &(const Vec3f &)fn_8040CDD0(&position, &offset));
        fn_8000DB78(m_mode.get(), &eye);
        SetAngles(m_mode.operator->(), &angles);
        Vec3f extent;
        fn_800091EC(&extent, &(const Vec3f &)fn_8000F57C(m_mode.get(), &m_vehicle->m_position, 0));
        Vec3f local;
        AssignVector(local, ((VectorSubtractFn)fn_8040CD74)(&m_vehicle->m_position, &position));
        AssignVector(local, fn_8040D0AC(&local, -angles.y));
        AssignVector(local, fn_8040D118(&local, -angles.x));
        AssignVector(local, fn_8040D044(&local, -angles.z));
        Vec3f stray;
        ConstructVector(&stray,
                        ((VectorSubtractFn)fn_8040CD74)(&m_vehicle->m_position, &position));
        Vec3f along;
        Vec3f across;
        fn_8040F990(&along, &across, &stray, &up);
        float acrossLength = fn_8040CF50(&across) / 250.0f;
        float alongLength = fn_8040CF50(&along) / 250.0f;
        float acrossRatio = 1.0f - std::fabs(acrossLength / extent.x);
        float alongRatio = 1.0f - std::fabs(alongLength / extent.y);
        if (acrossRatio > 0.0f) {
            fn_8040CE38(&across, acrossRatio);
        }
        if (alongRatio > 0.0f) {
            fn_8040CE38(&along, alongRatio);
        }
        fn_8040CFF8(&eye, &(const Vec3f &)fn_80093204(&across, &along));
        fn_8000DB78(m_mode.get(), &eye);
        SetAngles(m_mode.operator->(), &angles);
    }
}

void AirWingCameraImpl::OnSwitchIn(CameraModeUser::SwitchEventArgs &args) {
    Update();
}

void AirWingCameraImpl::OnSwitchOut(CameraModeUser::SwitchEventArgs &args) {}

void AirWingCameraImpl::OnUpdate(CameraModeUser::UpdateEventArgs &args) {
    Update();
}

} // namespace Vehicle

namespace Vehicle {
namespace BirdBase {

UnkBirdTrail_801B3148::UnkBirdTrail_801B3148(TVehicleBase *vehicle, UnkBirdPath_801B446C *path)
    : m_impl(new AirWingCameraImpl(vehicle, path)) {}

UnkBirdTrail_801B3148::~UnkBirdTrail_801B3148() {}

} // namespace BirdBase
} // namespace Vehicle

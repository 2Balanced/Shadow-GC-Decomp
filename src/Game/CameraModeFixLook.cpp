/* PJSCamera::CameraModeFixLook (class name and bases from RTTI; mode 8 of the camera factory;
 * Game/CameraModeFixLook.h): a camera that keeps the point looked at of the arguments it was made
 * with and puts the eye on the line from that point through the position the screen's camera
 * follows (raised along its up vector), "distance" beyond it; a collision process keeps the eye
 * clear of the stage. The unit also emits shared_ptr<ProcessBase>::reset, which the other camera
 * modes call. File name from the class; other names are inferred. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/CameraModeFixLook.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CameraTargetInfo.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "boost/shared_ptr.hpp"
#include "size_assert.h"

using PJSCamera::CameraModeFixLook;
using PJSCamera::PJSCameraMode;
using PJSCamera::Collision::ProcessBase;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;
using SonicteamUSA::System::Camera::CameraTargetInfo;

/* boost/shared_ptr.hpp only declares shared_ptr::reset, which the game has out of line; this unit
 * emits the copy for the collision processes (boost 1.32's definition). */
namespace boost {
template <> inline void shared_ptr<ProcessBase>::reset() {
    this_type().swap(*this);
}
} // namespace boost

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* The switch argument; only the flags are read (see Game/CameraModeFixLookChar.cpp). */
class UnkCameraSwitchArg_8000996C {
  public:
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u32 m_flags; /* bit 2: no collision process; bit 3: the 0x14-byte process */
};

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
/* The target a screen's camera follows (Game/CameraModeFixEye.cpp). */
CameraTargetInfo *fn_801C0B98(UnkCameraManager_80009548 *manager, int index);
/* The placement in a mode's arguments and its point looked at (behaviour inferred). */
UnkCameraState_8000AA70 fn_8000B3B4(const UnkCameraModeArgs_8000EBB0 *args);
Vec3f fn_8000B37C(const UnkCameraModeArgs_8000EBB0 *args);
/* v * scale, a + b, v normalised in place (returns the former length), v *= scale, and the vector
 * routine Game/CameraModeFixLookChar.cpp calls the same way (behaviour inferred). */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
float fn_8040CE74(Vec3f *v);
void fn_8040CE38(Vec3f *v, float scale);
Vec3f fn_80093204(const Vec3f *a, const Vec3f *b);
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A camera mode starts with a placement and keeps its arguments at 0x34. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

inline const UnkCameraModeArgs_8000EBB0 *ModeArgs(CameraMode *mode) {
    return reinterpret_cast<const UnkCameraModeArgs_8000EBB0 *>(mode->field_0x34);
}

namespace PJSCamera {

CameraModeFixLook::CameraModeFixLook(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number), m_distance(60.0f), m_height(0.0f) {
    *ModeState(this) = fn_8000B3B4(ModeArgs(this));
    m_distance = field_0x80;
    m_height = field_0x84;
}

CameraModeFixLook::~CameraModeFixLook() {}

void CameraModeFixLook::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    fn_8000A5D4(this, &m_eye, &(const Vec3f &)fn_8000B37C(ModeArgs(this)));
    CameraTargetInfo *target = fn_801C0B98(fn_80009548(), field_0xA0);
    Vec3f look;
    fn_800091EC(&look, &(const Vec3f &)fn_8040CDD0(&target->m_position,
                                                   &(const Vec3f &)fn_8040CDA8(&target->field_0x30,
                                                                               m_height)));
    Vec3f direction;
    fn_800091EC(&direction,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&look, &m_target));
    fn_8040CE74(&direction);
    fn_8040CE38(&direction, m_distance);
    fn_8000A5D4(this, &(const Vec3f &)fn_80093204(&direction, &look), &m_target);
    fn_8000ACE4(reinterpret_cast<CameraModeUser *>(this), &target->field_0x30);
    if (m_process.get() != 0) {
        m_process->Process(this, &look, reinterpret_cast<CharInfo *>(target));
    }
}

void CameraModeFixLook::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    u32 flags = arg->m_flags;
    if (flags & 4) {
        m_process.reset();
    } else if (flags & 8) {
        m_process = fn_801C68E8();
    } else {
        m_process = fn_801C6EB0();
    }
}

void CameraModeFixLook::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeFixLook::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeFixLook(owner, number);
}

int CameraModeFixLook::GetModeId() {
    return 8;
}

} // namespace PJSCamera

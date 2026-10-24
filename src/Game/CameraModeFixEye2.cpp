/* PJSCamera::CameraModeFixEye2 (class name and bases from RTTI; mode 12 of the camera factory;
 * Game/CameraModeFixEye2.h): a camera whose eye moves on a rail between two points of the
 * arguments it was made with, to the point nearest to the position the screen's camera follows,
 * and looks at that position. File name from the class; other names are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#define MSL_LIST_ACCESSORS
#include "Game/CameraModeFixEye2.h"
#include "Game/CameraManagerUnit.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CameraTargetInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "boost/scoped_ptr.hpp"
#include "size_assert.h"

using PJSCamera::CameraModeFixEye2;
using PJSCamera::PJSCameraMode;
using SonicteamUSA::System::Camera::CameraMode;
using SonicteamUSA::System::Camera::CameraModeUser;
using SonicteamUSA::System::Camera::CameraTargetInfo;

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* A line between two points (layout unknown; see Game/StageCameraPostProcess.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* A zero vector (see Game/StageCameraModeHolder.cpp). */
extern Vec3f lbl_8056EE28;

extern "C" {
UnkCameraManager_80009548 *fn_80009548(void);
/* The target a screen's camera follows (Game/CameraModeFixEye.cpp). */
CameraTargetInfo *fn_801C0B98(UnkCameraManager_80009548 *manager, int index);
/* The placement in a mode's arguments, and the eye of it (behaviour inferred). */
UnkCameraState_8000AA70 fn_8000B3B4(const UnkCameraModeArgs_8000EBB0 *args);
Vec3f fn_8000B398(const UnkCameraModeArgs_8000EBB0 *args);
/* A line between two points, and the point of a line nearest to point with its parameter (see
 * Game/SetSeLoop.cpp). */
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
Vec3f fn_8040FC78(float *t, const UnkLine_80020A20 *line, const Vec3f *point);

/* Three floats as a vector, and the vector of a mode's arguments at 0x34 (out of line here;
 * Game/CameraModeFixLook2.cpp calls these copies; behaviour from their bodies). */
inline Vec3f fn_801C0F74(const float *xyz) {
    return Vec3f(xyz[0], xyz[1], xyz[2]);
}

inline Vec3f fn_801C0F44(const UnkCameraModeArgs_8000EBB0 *args) {
    return fn_801C0F74(&args->field_0x34);
}
}

/* The rail the eye moves on (struct name unknown; 0x1C bytes): its two ends and where on it the
 * eye was last (names inferred). */
struct UnkRail_801C0F90 {
    UnkRail_801C0F90() {
        CopyVector(m_start, lbl_8056EE28);
        CopyVector(m_end, lbl_8056EE28);
        m_t = 0.0f;
    }
    ~UnkRail_801C0F90() {}
    static void CopyVector(Vec3f &vector, const Vec3f &value) {
        fn_800091EC(&vector, &value);
    }

    /* 0x00 */ Vec3f m_start;
    /* 0x0C */ Vec3f m_end;
    /* 0x18 */ float m_t;
};

/* Copies value into vector through the game's out-of-line assignment (as in Game/GadgetUtil.h). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A camera mode starts with a placement and keeps its arguments at 0x34. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

inline const UnkCameraModeArgs_8000EBB0 *ModeArgs(CameraMode *mode) {
    return reinterpret_cast<const UnkCameraModeArgs_8000EBB0 *>(mode->field_0x34);
}

namespace PJSCamera {

CameraModeFixEye2::CameraModeFixEye2(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number), m_rail(new UnkRail_801C0F90) {
    *ModeState(this) = fn_8000B3B4(ModeArgs(this));
    fn_800091D0(&m_rail->m_start, &(const Vec3f &)fn_8000B398(ModeArgs(this)));
    fn_800091D0(&m_rail->m_end, &(const Vec3f &)fn_801C0F44(ModeArgs(this)));
}

CameraModeFixEye2::~CameraModeFixEye2() {}

void CameraModeFixEye2::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    CameraTargetInfo *target = fn_801C0B98(fn_80009548(), field_0xA0);
    UnkLine_80020A20 line;
    fn_80020A20(&line, &m_rail->m_start, &m_rail->m_end);
    Vec3f eye;
    float t;
    AssignVector(eye, fn_8040FC78(&t, &line, &target->m_position));
    if (t < 0.0f) {
        t = 0.0f;
    } else if (t > 1.0f) {
        t = 1.0f;
    }
    fn_8000A5D4(this, &eye, &target->m_position);
    fn_8000ACE4(reinterpret_cast<CameraModeUser *>(this), &target->field_0x30);
    m_rail->m_t = t;
}

void CameraModeFixEye2::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {}

void CameraModeFixEye2::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeFixEye2::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeFixEye2(owner, number);
}

int CameraModeFixEye2::GetModeId() {
    return 12;
}

} // namespace PJSCamera

/* PJSCamera::CameraModeTestPlayerNoRot (class name and bases from RTTI; mode 3 of the camera
 * factory): a test camera like CameraModeTestPlayer that keeps the eye's offset from the player,
 * so it moves with the player without turning. The right stick turns the offset; its horizontal
 * length is kept and its height limited to between 0 and 60. Switched to, it starts from the
 * first screen's camera. File name from the class; other names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/CameraModeTestPlayerNoRot.h"
#include "Game/CameraState.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using PJSCamera::CameraModeTestPlayerNoRot;
using PJSCamera::PJSCameraMode;
using SonicteamUSA::System::Camera::CameraMode;

/* The pad state (struct name unknown; a reduced view as in Game/AdvStaffRoll.cpp). */
struct UnkPads_80051734 {
    /* 0x00 */ u8 field_0x0[0x44];
    /* 0x44 */ float m_rightX;
    /* 0x48 */ float m_rightY;
};

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* The number of the player character (meaning inferred). */
extern s32 lbl_805EE588;

extern "C" {
UnkPads_80051734 *fn_80051734(void);
UnkCameraManager_80009548 *fn_80009548(void);
const UnkCameraState_8000AA70 *fn_80009370(UnkCameraManager_80009548 *manager, int index);
/* The character of a screen, and a character by number (behaviour inferred). */
boost::weak_ptr<CharInfo> fn_800A8C54(UnkCharaList_80074194 *list, int index);
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int number);
/* Copies a camera placement's eye, point looked at and field of view (behaviour from its body). */
void fn_8000A544(UnkCameraState_8000AA70 *state, const UnkCameraState_8000AA70 *from);
/* The pitch and yaw of a direction (behaviour inferred). */
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
/* Vector routines (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* turned about x */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about y */
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

extern const float lbl_805FAD38; /* pi */

/* Degrees to radians. */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* A camera mode starts with its placement. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

namespace PJSCamera {

CameraModeTestPlayerNoRot::CameraModeTestPlayerNoRot(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number) {}

CameraModeTestPlayerNoRot::~CameraModeTestPlayerNoRot() {}

void CameraModeTestPlayerNoRot::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    boost::shared_ptr<CharInfo> chara;
    chara = fn_800A8C54(fn_80074194(), field_0xA0).lock();
    if (chara.get() != 0) {
        Vec3f eye;
        fn_800091EC(&eye, &m_eye);
        Vec3f center;
        fn_800091EC(&center, &chara->m_position);
        fn_800091D0(&eye, &(const Vec3f &)fn_8040CDD0(&center, &m_offset));
        float turnX = fn_80051734()->m_rightX;
        float turnY = fn_80051734()->m_rightY;
        Mtx34f rotation;
        fn_8040C148(&rotation, &lbl_805E0720);
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&eye, &center));
        Vec3f angles(0.0f, 0.0f, 0.0f);
        fn_8003B9E8(&offset, &angles.x, &angles.y);
        float pitch = angles.x + DegToRad(2.0f * turnY);
        float yaw = angles.y + DegToRad(2.0f * turnX);
        if (pitch < DegToRad(-89.9f)) {
            pitch = DegToRad(-89.9f);
        }
        Vec3f arm(0.0f, 0.0f, fn_8040CF50(&offset));
        fn_800091D0(&arm, &(const Vec3f &)fn_8040D118(&arm, pitch));
        fn_800091D0(&arm, &(const Vec3f &)fn_8040D0AC(&arm, yaw));
        fn_800091D0(&eye, &(const Vec3f &)fn_8040CDD0(&arm, &center));
        Vec3f flat;
        fn_800091EC(&flat, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&eye, &center));
        flat.y = 0.0f;
        fn_8040CF50(&flat);
        float height = eye.y - center.y;
        if (height > 60.0f) {
            height = 60.0f;
        } else if (height < 0.0f) {
            height = 0.0f;
        }
        flat.y = height;
        eye.y = center.y + flat.y;
        fn_8000A5D4(this, &eye, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&eye, &flat));
        fn_800091D0(&m_offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&eye, &center));
    }
}

void CameraModeTestPlayerNoRot::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    boost::shared_ptr<CharInfo> chara(fn_800A8B74(fn_80074194(), lbl_805EE588).lock());
    if (chara.get() != 0) {
        fn_8000A544(ModeState(this), fn_80009370(fn_80009548(), 0));
        fn_800091D0(&m_offset,
                    &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_eye, &chara->m_position));
    }
}

void CameraModeTestPlayerNoRot::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeTestPlayerNoRot::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeTestPlayerNoRot(owner, number);
}

int CameraModeTestPlayerNoRot::GetModeId() {
    return 3;
}

} // namespace PJSCamera

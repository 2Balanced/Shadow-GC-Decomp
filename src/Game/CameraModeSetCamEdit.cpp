/* PJSCamera::CameraModeSetCamEdit (class name and bases from RTTI; mode 6 of the camera factory): a
 * debug camera moved with the first pad. With the right stick (or a held button) it orbits the
 * point it looks at at a set distance; otherwise the left stick moves that point and the camera
 * keeps its eye. The second way of moving checks which controls are enabled (the words from
 * 0xC0). File name from the class; member names and the meanings of the pad fields are inferred.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include "Game/CameraModeSetCamEdit.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "size_assert.h"

using PJSCamera::CameraModeSetCamEdit;
using PJSCamera::PJSCameraMode;
using SonicteamUSA::System::Camera::CameraMode;

/* The pad state (struct name unknown; a reduced view as in Game/AdvStaffRoll.cpp). */
struct UnkPads_80051734 {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ u32 m_held;
    /* 0x2C */ u32 m_buttons;
    /* 0x30 */ u8 field_0x30[0x38 - 0x30];
    /* 0x38 */ float m_leftX;
    /* 0x3C */ float m_leftY;
    /* 0x40 */ u8 field_0x40[0x44 - 0x40];
    /* 0x44 */ float m_rightX;
    /* 0x48 */ float m_rightY;
};

extern "C" {
UnkPads_80051734 *fn_80051734(void);
/* A camera placement's look vector (target - eye; behaviour from its body). */
Vec3f fn_8000A3BC(const UnkCameraState_8000AA70 *state);
/* The angle of (x, z), the pitch and yaw of a direction (behaviour inferred). */
float fn_80016260(float x, float z);
void fn_8003B9E8(const Vec3f *direction, float *pitch, float *yaw);
/* Sets the angles to zero (see Game/BirdBase.cpp). */
void fn_8040E970(Vec3f *angles);
/* The length of a two-float vector (behaviour from its body). */
float fn_8040CCEC(const Vec2f *vector);
/* Vector routines (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
float fn_8040CE74(Vec3f *vector);                  /* normalises */
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* turned about x */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about y */
}

/* fn_8040CD74 called as returning a - b by value (see Game/ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

extern const float lbl_805FAD38; /* pi */

/* A camera mode starts with its placement. */
inline UnkCameraState_8000AA70 *ModeState(CameraMode *mode) {
    return reinterpret_cast<UnkCameraState_8000AA70 *>(mode);
}

/* Degrees to radians. */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace PJSCamera {

CameraModeSetCamEdit::CameraModeSetCamEdit(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number), m_angles(0.0f, 0.0f, 0.0f), m_speed(2.5f), m_distance(100.0f),
      m_moveTarget(1), m_orbit(1), m_turn(1), m_zoom(1), m_editMode(1) {
    fn_8040E970(&m_angles);
}

CameraModeSetCamEdit::~CameraModeSetCamEdit() {}

void CameraModeSetCamEdit::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    if (m_editMode != 0) {
        MoveEdit();
    } else {
        MoveFree();
    }
}

void CameraModeSetCamEdit::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {}

void CameraModeSetCamEdit::vf_0x14(UnkCameraArg_800098E4 *arg) {}

void CameraModeSetCamEdit::MoveEdit() {
    Vec3f target;
    fn_800091EC(&target, &m_target);
    if (m_moveTarget != 0 && (fn_80051734()->m_buttons & 0x10) == 0x10 && (fn_80051734()->m_buttons & 0x8) == 0x8 &&
        (fn_80051734()->m_buttons & 0x8000) == 0x8000) {
        float moveX = fn_80051734()->m_leftX;
        Vec3f move(moveX, 0.0f, fn_80051734()->m_leftY);
        Vec3f look;
        fn_800091EC(&look, &(const Vec3f &)fn_8000A3BC(ModeState(this)));
        fn_800091D0(&move, &(const Vec3f &)fn_8040D0AC(
                               &move, fn_80016260(look.x, look.z) - DegToRad(180.0f)));
        target.z += move.z * m_speed;
        target.x += move.x * m_speed;
    }
    if (m_orbit != 0 && m_turn != 0 && (fn_80051734()->m_buttons & 0x10) == 0x10 && (fn_80051734()->m_buttons & 0x8) == 0x8 &&
        (fn_80051734()->m_buttons & 0x8000) == 0x8000) {
        float turnX = fn_80051734()->m_rightX;
        float turnY = fn_80051734()->m_rightY;
        m_angles.y += DegToRad(turnX);
        m_angles.x += DegToRad(turnY);
    }
    if (m_orbit != 0 && m_zoom != 0 && (fn_80051734()->m_held & 0x10) == 0x10) {
        UnkPads_80051734 *pad = fn_80051734();
        m_distance += 0.4f * (m_speed * pad->m_leftY);

    }
    if (m_moveTarget != 0 && (fn_80051734()->m_held & 0x8) == 0x8) {
        UnkPads_80051734 *pad = fn_80051734();
        target.y += -m_speed * pad->m_leftY;
    }
    if (m_orbit != 0) {
        Vec3f offset(0.0f, 0.0f, m_distance);
        fn_800091D0(&offset, &(const Vec3f &)fn_8040D118(&offset, m_angles.x));
        fn_800091D0(&offset, &(const Vec3f &)fn_8040D0AC(&offset, m_angles.y));
        fn_8000A5D4(this, &(const Vec3f &)fn_8040CDD0(&offset, &target), &target);
    } else {
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_eye, &target));
        m_distance = fn_8040CF50(&offset);
        fn_8040CE74(&offset);
        fn_8003B9E8(&offset, &m_angles.x, &m_angles.y);
        fn_8000A5D4(this, &m_eye, &target);
    }
}

void CameraModeSetCamEdit::MoveFree() {
    Vec3f target;
    fn_800091EC(&target, &m_target);
    int orbit = 0;
    float leftX = fn_80051734()->m_leftX;
    float leftY = fn_80051734()->m_leftY;
    float rightX = fn_80051734()->m_rightX;
    float rightY = fn_80051734()->m_rightY;
    Vec2f right(rightX, rightY);
    if (fn_8040CCEC(&right) > 0.25f || (fn_80051734()->m_held & 0x10) == 0x10) {
        orbit = 1;
    }
    if (orbit != 0) {
        if ((fn_80051734()->m_buttons & 0x10) == 0x10 && (fn_80051734()->m_buttons & 0x8) == 0x8 &&
            (fn_80051734()->m_buttons & 0x8000) == 0x8000) {
            m_angles.y += DegToRad(rightX);
            m_angles.x += DegToRad(rightY);
        }
        if ((fn_80051734()->m_held & 0x10) == 0x10) {
            m_distance += 0.4f * (leftY * m_speed);
        }
        Vec3f offset(0.0f, 0.0f, m_distance);
        fn_800091D0(&offset, &(const Vec3f &)fn_8040D118(&offset, m_angles.x));
        fn_800091D0(&offset, &(const Vec3f &)fn_8040D0AC(&offset, m_angles.y));
        fn_8000A5D4(this, &(const Vec3f &)fn_8040CDD0(&offset, &target), &target);
    } else {
        if ((fn_80051734()->m_buttons & 0x10) == 0x10 && (fn_80051734()->m_buttons & 0x8) == 0x8 &&
            (fn_80051734()->m_buttons & 0x8000) == 0x8000) {
            Vec3f move(leftX, 0.0f, leftY);
            Vec3f look;
            fn_800091EC(&look, &(const Vec3f &)fn_8000A3BC(ModeState(this)));
            fn_800091D0(&move, &(const Vec3f &)fn_8040D0AC(
                                   &move, fn_80016260(look.x, look.z) - DegToRad(180.0f)));
            target.z += move.z * m_speed;
            target.x += move.x * m_speed;
        }
        if ((fn_80051734()->m_held & 0x8) == 0x8) {
            target.y += leftY * -m_speed;
        }
        Vec3f offset;
        fn_800091EC(&offset, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_eye, &target));
        m_distance = fn_8040CF50(&offset);
        fn_8040CE74(&offset);
        fn_8003B9E8(&offset, &m_angles.x, &m_angles.y);
        fn_8000A5D4(this, &m_eye, &target);
    }
}

CameraMode *CameraModeSetCamEdit::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeSetCamEdit(owner, number);
}

int CameraModeSetCamEdit::GetModeId() {
    return 6;
}

} // namespace PJSCamera

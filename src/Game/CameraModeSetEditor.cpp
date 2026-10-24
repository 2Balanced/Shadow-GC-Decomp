/* PJSCamera::CameraModeSetEditor (class name and bases from RTTI; mode 5 of the camera factory): a
 * debug camera for the set editor. The left stick moves the point looked at, turned to the
 * direction the screen's RenderWare camera faces, the right stick orbits the eye about it and two
 * held buttons change the distance and the height. File name from the class; member names and
 * the meanings of the pad fields are inferred. Definitions are in reverse address order (deferred
 * inlining emits functions last-defined first). */
#include "Game/CameraModeSetEditor.h"
#include "Game/CameraState.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "size_assert.h"

using PJSCamera::CameraModeSetEditor;
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

/* A RenderWare frame: its modelling matrix's at vector (RwFrame; a reduced view). */
struct UnkFrame_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x30];
    /* 0x30 */ Vec3f m_at;
};

/* The RenderWare camera and the game's screen (see Game/ScreenHolder.h; reduced views). */
struct UnkScreen_8000DCC0 {
    /* 0x00 */ u8 field_0x0[0x4];
    /* 0x04 */ UnkFrame_8000DCC0 *m_frame;
};

class UnkScreenHolder_8000DCC0 {
  public:
    /* 0x00 */ UnkScreen_8000DCC0 *m_camera;
};

extern "C" {
UnkPads_80051734 *fn_80051734(void);
UnkScreenHolder_8000DCC0 *fn_8000DCC0(void);
/* The angle of (x, z) (behaviour inferred). */
float fn_80016260(float x, float z);
/* Sets the angles to zero (see Game/BirdBase.cpp). */
void fn_8040E970(Vec3f *angles);
/* Vector routines (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
Vec3f fn_8040D118(const Vec3f *vector, float angle); /* turned about x */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle); /* turned about y */
}

extern const float lbl_805FAD38; /* pi */

/* Degrees to radians. */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace PJSCamera {

CameraModeSetEditor::CameraModeSetEditor(UnkCameraOwner_8000E9BC *owner, int number)
    : PJSCameraMode(owner, number), m_angles(0.0f, 0.0f, 0.0f), m_speed(10.0f),
      m_distance(100.0f) {
    fn_8040E970(&m_angles);
}

CameraModeSetEditor::~CameraModeSetEditor() {}

void CameraModeSetEditor::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    Vec3f target;
    fn_800091EC(&target, &m_target);
    if ((fn_80051734()->m_buttons & 0x10) == 0x10 && (fn_80051734()->m_buttons & 0x8) == 0x8) {
        float moveX = fn_80051734()->m_leftX;
        Vec3f move(moveX, 0.0f, fn_80051734()->m_leftY);
        UnkScreen_8000DCC0 *camera = fn_8000DCC0()->m_camera;
        if (camera != 0) {
            UnkFrame_8000DCC0 *frame = camera->m_frame;
            fn_800091D0(&move, &(const Vec3f &)fn_8040D0AC(
                                   &move, fn_80016260(frame->m_at.x, frame->m_at.z) -
                                              DegToRad(180.0f)));
        }
        target.z += move.z * m_speed;
        target.x += move.x * m_speed;
    }
    float turnX = fn_80051734()->m_rightX;
    float turnY = fn_80051734()->m_rightY;
    m_angles.y += DegToRad(turnX);
    m_angles.x += DegToRad(turnY);
    if ((fn_80051734()->m_held & 0x10) == 0x10) {
        UnkPads_80051734 *pad = fn_80051734();
        m_distance += 2.0f * pad->m_leftY;
    }
    if ((fn_80051734()->m_held & 0x8) == 0x8) {
        UnkPads_80051734 *pad = fn_80051734();
        target.y += -10.0f * pad->m_leftY;
    }
    Vec3f offset(0.0f, 0.0f, m_distance);
    fn_800091D0(&offset, &(const Vec3f &)fn_8040D118(&offset, m_angles.x));
    fn_800091D0(&offset, &(const Vec3f &)fn_8040D0AC(&offset, m_angles.y));
    fn_8000A5D4(this, &(const Vec3f &)fn_8040CDD0(&offset, &target), &target);
}

void CameraModeSetEditor::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {}

void CameraModeSetEditor::vf_0x14(UnkCameraArg_800098E4 *arg) {}

CameraMode *CameraModeSetEditor::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeSetEditor(owner, number);
}

int CameraModeSetEditor::GetModeId() {
    return 5;
}

} // namespace PJSCamera

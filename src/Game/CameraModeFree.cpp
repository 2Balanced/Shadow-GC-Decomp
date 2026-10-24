/* PJSCamera::CameraModeFree (class name and bases from RTTI; mode 1 of the camera factory;
 * Game/CameraModeFree.h): a debug camera flown with the first pad. The left stick moves it in the
 * direction it faces, two held buttons raise and lower it, the right stick turns it, two more held
 * buttons roll it (both: level), and a pressed button cycles the speed; the player ignores input
 * while it is on. File name from the class; member names and the meanings of the pad fields are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <cmath>

#include "Game/CameraModeFree.h"
#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include "Game/CharCommand.h"
#include "Game/CharInfo.h"
#include "Game/MathTypes.h"
#include "Game/PJSCameraMode.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using PJSCamera::CameraModeFree;
using PJSCamera::PJSCameraMode;
using SonicteamUSA::System::Camera::CameraMode;

/* The camera manager (class name unknown; see Game/CameraManagerUnit.h). */
class UnkCameraManager_80009548;

/* The pad state (struct name unknown; a reduced view as in Game/CameraModeSetEditor.cpp). */
struct UnkPads_80051734 {
    /* 0x00 */ u8 field_0x0[0x28];
    /* 0x28 */ u32 m_held;
    /* 0x2C */ u32 m_buttons;
    /* 0x30 */ u32 m_pressed;
    /* 0x34 */ u8 field_0x34[0x38 - 0x34];
    /* 0x38 */ float m_leftX;
    /* 0x3C */ float m_leftY;
    /* 0x40 */ u8 field_0x40[0x44 - 0x40];
    /* 0x44 */ float m_rightX;
    /* 0x48 */ float m_rightY;
};

/* The player's character id (see Game/BAGunShip.cpp). */
extern int lbl_805EE588;

extern "C" {
UnkPads_80051734 *fn_80051734(void);
UnkCameraManager_80009548 *fn_80009548(void);
/* The placement of a screen's camera unit (behaviour inferred). */
const UnkCameraState_8000AA70 *fn_80009370(UnkCameraManager_80009548 *manager, int index);
/* A character by id (see Game/BAGunShip.cpp). */
boost::weak_ptr<CharInfo> fn_800A8B74(UnkCharaList_80074194 *list, int id);
/* The angles of a placement's look (pitch, yaw) and its field of view, and setting the angles
 * (behaviour inferred). */
Vec3f fn_8000A610(const void *placement);
void fn_8000A304(CameraMode *mode, const Vec3f *angles);
/* Copies the eye, the point looked at and the field of view of a placement, and moves both points
 * of a mode by a vector (behaviour inferred). */
void fn_8000A544(CameraMode *mode, const UnkCameraState_8000AA70 *placement);
void fn_8000A508(CameraMode *mode, const Vec3f *move);
/* Sets a vector to zero (behaviour inferred). */
void fn_8040CE60(Vec3f *v);
}

extern const float lbl_805FAD38; /* pi */

/* Degrees to radians. */
inline float DegToRad(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

namespace PJSCamera {

CameraModeFree::~CameraModeFree() {}

void CameraModeFree::Update(float dt, UnkCameraUpdateArg_800099B8 *arg) {
    float leftX = fn_80051734()->m_leftX;
    float leftY = fn_80051734()->m_leftY;
    float rightX = fn_80051734()->m_rightX;
    float rightY = fn_80051734()->m_rightY;
    if ((fn_80051734()->m_pressed & 8) == 8) {
        m_speed *= 5.0f;
        if (m_speed > 62.5f) {
            m_speed = 0.1f;
        }
    }
    float yaw = fn_8000A610(this).y;
    Vec3f move;
    fn_8040CE60(&move);
    if (leftX != 0.0f || leftY != 0.0f) {
        float c;
        float s;
        float angle = -yaw;
        c = cos(angle);
        s = sin(angle);
        move.x += m_speed * (c * leftX - s * leftY);
        move.z += m_speed * (s * leftX + c * leftY);
    }
    if ((fn_80051734()->m_held & 0x1000) == 0x1000) {
        move.y += m_speed;
    } else if ((fn_80051734()->m_held & 0x2000) == 0x2000) {
        move.y -= m_speed;
    }
    fn_8000A508(this, &move);
    Vec3f angles = fn_8000A610(this);
    if ((fn_80051734()->m_held & 0x10000) == 0x10000 &&
        (fn_80051734()->m_held & 0x20000) == 0x20000) {
        angles.z = DegToRad(0.0f);
    } else if ((fn_80051734()->m_held & 0x10000) == 0x10000) {
        angles.z -= DegToRad(0.25f);
    } else if ((fn_80051734()->m_held & 0x20000) == 0x20000) {
        angles.z += DegToRad(0.25f);
    }
    angles.x -= DegToRad(2.0f * rightY);
    angles.y -= DegToRad(2.0f * rightX);
    fn_8000A304(this, &angles);
}

void CameraModeFree::OnSwitch(UnkCameraSwitchArg_8000996C *arg) {
    boost::shared_ptr<CharInfo> player(fn_800A8B74(fn_80074194(), lbl_805EE588).lock());
    if (player.get() != 0) {
        player->PushCommand(new Player::IgnoreInputCommand(2, 0));
    }
    fn_8000A544(this, fn_80009370(fn_80009548(), 0));
}

void CameraModeFree::vf_0x14(UnkCameraArg_800098E4 *arg) {
    boost::shared_ptr<CharInfo> player(fn_800A8B74(fn_80074194(), lbl_805EE588).lock());
    if (player.get() != 0) {
        player->PushCommand(new Player::IgnoreInputCommand(3, 0));
    }
}

CameraMode *CameraModeFree::Create(UnkCameraOwner_8000E9BC *owner, int number) {
    return new CameraModeFree(owner, number);
}

int CameraModeFree::GetModeId() {
    return 1;
}

} // namespace PJSCamera

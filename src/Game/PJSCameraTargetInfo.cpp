/* PJSCamera::PJSCameraTargetInfo (class name and base from RTTI): the game's camera target. Its
 * defaults put both vectors at the origin with up and -z directions; Assign copies another target.
 * File name inferred from the class; method and member names are inferred. Definitions are in
 * reverse address order (deferred inlining emits functions last-defined first). */
#include "Game/PJSCameraTargetInfo.h"
#include "Game/MathTypes.h"
#include "size_assert.h"

/* A pair of floats the defaults copy (meaning unknown). */
extern Vec2f lbl_805F1358;

void UnkWords_8007A4D4::Reset() {
    for (int i = 0; i < 3; i++) {
        m_words[i] = 0;
    }
}

namespace PJSCamera {

PJSCameraTargetInfo::PJSCameraTargetInfo() {
    Reset();
}

PJSCameraTargetInfo::~PJSCameraTargetInfo() {}

void PJSCameraTargetInfo::Assign(const SonicteamUSA::System::Camera::CameraTargetInfo &other) {
    *this = static_cast<const PJSCameraTargetInfo &>(other);
}

void PJSCameraTargetInfo::Reset() {
    fn_800091D0(&m_position, fn_800091D0(&field_0xC, &lbl_805E07A0));
    Vec3f up(0.0f, 1.0f, 0.0f);
    fn_800091D0(&field_0x18, &up);
    Vec3f front(0.0f, 0.0f, -1.0f);
    fn_800091D0(&field_0x24, &front);
    Vec3f up2(0.0f, 1.0f, 0.0f);
    fn_800091D0(&field_0x30, &up2);
    field_0x40.Reset();
    field_0x4C = lbl_805F1358;
}

} // namespace PJSCamera

#ifndef GAME_CAMERAADJUST_H
#define GAME_CAMERAADJUST_H

#include "Game/CameraModeUser.h"
#include "Game/CameraState.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace Camera {

/* CameraAdjust (class name from RTTI; vtable at 0x8051C4F8): how the camera moves from one mode
 * to the next (PJSCamera::AdjustCosRot, AdjustLinerLocal, AdjustLinerWorld, AdjustOnlyAov derive
 * from it). It is made from a blend's parameters (constructor 0x8000B158, destructor 0x8000B0E4),
 * starts from the placement the camera had (0x8000AF74 calls Start) and each frame moves its own
 * placement towards the new mode's (0x8000ADD0): the eye through UpdateEye at the rate
 * GetEyeRate gives, the point looked at at the rate of GetLookRate. The data members come before
 * the virtual functions, so the vtable pointer sits at 0xAC. Names other than the class's are
 * inferred. */
class CameraAdjust {
  public:
    CameraAdjust(const UnkCameraBlend_8000B2C0 &blend);

    /* 0x00 */ s32 m_status; /* 1 while moving, 2 once both parts have arrived */
    /* 0x04 */ UnkCameraBlend_8000B2C0 m_blend; /* the durations of the two parts at 0x0 and 0x4 */
    /* 0x30 */ UnkCameraState_8000AA70 m_from;
    /* 0x64 */ UnkCameraState_8000AA70 m_state;
    /* 0x98 */ float m_time;
    /* 0x9C */ float m_eyeRate;  /* 0 to 1 */
    /* 0xA0 */ float m_lookRate; /* 0 to 1 */
    /* 0xA4 */ s32 m_eyeStatus;
    /* 0xA8 */ s32 m_lookStatus;

    /* 0xAC holds the vtable pointer. */
    virtual s32 GetType() = 0;
    virtual void Start(const UnkCameraState_8000AA70 *from, CameraMode *mode) = 0;
    virtual void UpdateEye(CameraMode *mode, float dt) = 0;
    /* The rates at time of duration (time / duration in the base class). */
    virtual float GetEyeRate(float time, float duration);
    virtual float GetLookRate(float time, float duration);
    virtual ~CameraAdjust();
    virtual void vf_0x20();
};
SIZE_ASSERT(CameraAdjust, 0xB0);

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

#endif

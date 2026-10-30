#ifndef GAME_PJSCAMERATARGETINFO_H
#define GAME_PJSCAMERATARGETINFO_H

#include "Game/CameraTargetInfo.h"
#include "Game/MathTypes.h"
#include "Game/UnkWords_8007A4D4.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace PJSCamera {

/* PJSCameraTargetInfo (class name and base from RTTI; vtable at 0x8055241C): the game's camera
 * target. Members past the base are known only by offset. */
class PJSCameraTargetInfo : public SonicteamUSA::System::Camera::CameraTargetInfo {
  public:
    PJSCameraTargetInfo();
    virtual void Reset();
    virtual void Assign(const SonicteamUSA::System::Camera::CameraTargetInfo &other);
    virtual ~PJSCameraTargetInfo();

    /* 0x40 */ UnkWords_8007A4D4 field_0x40;
    /* 0x4C */ Vec2f field_0x4C;
};
SIZE_ASSERT(PJSCameraTargetInfo, 0x54);

} // namespace PJSCamera

#endif

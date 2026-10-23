#ifndef GAME_CAMERATARGETINFO_H
#define GAME_CAMERATARGETINFO_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace Camera {

/* CameraTargetInfo (class name from RTTI; vtable at 0x8054E678, constructor 0x80286F1C, destructor
 * 0x80286ED4): what a camera follows. Its five vectors come first and the vtable pointer follows
 * them at 0x3C. Reset and Assign are pure (names inferred from PJSCamera::PJSCameraTargetInfo,
 * which sets the vectors to defaults and copies another target). The camera factory makes one for
 * each camera unit (CameraManagerUnit::m_target, Game/CameraManagerUnit.h), which follows its
 * position. The other vectors' meanings are unknown; PJSCameraTargetInfo's defaults are zero,
 * zero, up, -z and up. */
class CameraTargetInfo {
  public:
    CameraTargetInfo();

    /* 0x00 */ Vec3f m_position;
    /* 0x0C */ Vec3f field_0xC;
    /* 0x18 */ Vec3f field_0x18;
    /* 0x24 */ Vec3f field_0x24;
    /* 0x30 */ Vec3f field_0x30;

    /* 0x3C holds the vtable pointer. */
    virtual void Reset() = 0;
    virtual void Assign(const CameraTargetInfo &other) = 0;
    virtual ~CameraTargetInfo();
};

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

#endif

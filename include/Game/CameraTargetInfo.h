#ifndef GAME_CAMERATARGETINFO_H
#define GAME_CAMERATARGETINFO_H

#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A word array cleared by its constructor (struct name unknown; constructor 0x8007A4D4, Reset
 * 0x80290374 in Game/PJSCameraTargetInfo.cpp). The constructor calls the inline Clear, so it stays
 * out of line where it is used (the compiler's assignment, a loop over the words, too). Names
 * and meanings are inferred. */
struct UnkWords_8007A4D4 {
    UnkWords_8007A4D4() {
        Clear();
    }

    void Reset();

    void Clear() {
        for (int i = 0; i < 3; i++) {
            m_words[i] = 0;
        }
    }

    /* 0x0 */ u32 m_words[3];
};

namespace SonicteamUSA {
namespace System {
namespace Camera {

/* CameraTargetInfo (class name from RTTI; vtable at 0x8054E678, constructor 0x80286F1C, destructor
 * 0x80286ED4): what a camera follows. Its five vectors come first and the vtable pointer follows
 * them at 0x3C. Reset and Assign are pure (names inferred from PJSCamera::PJSCameraTargetInfo,
 * which sets the vectors to defaults and copies another target). The vectors' meanings are
 * unknown; PJSCameraTargetInfo's defaults are zero, zero, up, -z and up. */
class CameraTargetInfo {
  public:
    CameraTargetInfo();

    /* 0x00 */ Vec3f field_0x0;
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

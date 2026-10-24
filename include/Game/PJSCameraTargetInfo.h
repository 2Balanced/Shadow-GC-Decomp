#ifndef GAME_PJSCAMERATARGETINFO_H
#define GAME_PJSCAMERATARGETINFO_H

#include "Game/CameraTargetInfo.h"
#include "Game/MathTypes.h"
#include <dolphin/types.h>
#include "size_assert.h"

extern "C" void fn_80095364(u32 *to, const u32 *from);

/* A word array cleared by its constructor (struct name unknown; constructor 0x8007A4D4, Reset
 * 0x80290374 in Game/PJSCameraTargetInfo.cpp). The constructor calls the inline Clear, so it stays
 * out of line where it is used (the compiler's assignment, a loop over the words, too). Names
 * and meanings are inferred. The assignment is the out-of-line word copy (fn_80095364), which
 * PJSCameraTargetInfo's assignment calls (Game/CameraModeGeneral.cpp). */
struct UnkWords_8007A4D4 {
    UnkWords_8007A4D4() {
        Clear();
    }
    UnkWords_8007A4D4 &operator=(const UnkWords_8007A4D4 &other) {
        fn_80095364(m_words, other.m_words);
        return *this;
    }

    void Reset();

    void Clear() {
        for (int i = 0; i < 3; i++) {
            m_words[i] = 0;
        }
    }

    /* 0x0 */ u32 m_words[3];
};

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

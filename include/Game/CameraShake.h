#ifndef GAME_CAMERASHAKE_H
#define GAME_CAMERASHAKE_H

#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace Camera {

/* CameraShake (class name from the RTTI of its shared_ptr control block): a camera unit's shake,
 * which moves the camera along its up vector (Game/CameraShake.cpp). A negative duration shakes
 * until the shake is replaced. Member names are inferred. */
class CameraShake {
  public:
    /* The amplitude and frequency of one of the three kinds of shake (struct name unknown). */
    struct Kind {
        /* 0x0 */ float m_amplitude;
        /* 0x4 */ float m_frequency;
    };

    CameraShake(float duration, float amplitude, float frequency);
    CameraShake(float duration, const Kind &kind)
        : m_duration(duration), m_amplitude(kind.m_amplitude), m_frequency(kind.m_frequency), m_time(0.0f),
          m_mode(0), m_step(0) {}
    ~CameraShake();

    /* 0x00 */ float m_duration;
    /* 0x04 */ float m_amplitude;
    /* 0x08 */ float m_frequency; /* shakes per second */
    /* 0x0C */ float m_time;
    /* 0x10 */ s32 m_mode; /* 0 smooth, 1 every frame, 2 every other step (from the frame time) */
    /* 0x14 */ s32 m_step; /* quarter of the period */
};
SIZE_ASSERT(CameraShake, 0x18);

} // namespace Camera
} // namespace System
} // namespace SonicteamUSA

#endif

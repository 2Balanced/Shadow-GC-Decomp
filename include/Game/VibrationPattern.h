#ifndef GAME_VIBRATIONPATTERN_H
#define GAME_VIBRATIONPATTERN_H

#include <dolphin/types.h>
#include "size_assert.h"

namespace Vibration {

/* A rumble pattern of the game's table (Game/VibrationPattern.cpp; struct and member names
 * inferred): two strengths and how much each fades every frame, and how long the pattern lasts
 * in seconds. Vibration::Request (Game/VibrationRequest.h) plays them. */
struct Pattern {
    /* 0x00 */ float m_strength;
    /* 0x04 */ float m_fade;
    /* 0x08 */ float m_strength2;
    /* 0x0C */ float m_fade2;
    /* 0x10 */ float m_time;
};
SIZE_ASSERT(Pattern, 0x14);

/* The pattern of a kind (0 to 12; name inferred). */
Pattern *GetPattern(int kind);

} // namespace Vibration

#endif

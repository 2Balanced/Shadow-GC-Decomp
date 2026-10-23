/* The rumble patterns the vibration requests play (Vibration::Request, Game/VibrationRequest.cpp):
 * thirteen kinds of strength, fade and length (no RTTI; the file and all names are inferred). The
 * lengths are given in frames at 60 per second; computed through an inline function, they are
 * stored by the static initializer, as in retail. */
#include "Game/VibrationPattern.h"

namespace Vibration {

/* A length of frames at 60 frames per second, in seconds (name inferred). */
inline float Frames(int frames) {
    return frames / 60.0f;
}

static Pattern s_patterns[13] = {
    {0.4f, 0.04f, 0.0f, 0.0f, Frames(10)},
    {0.5f, 0.08f, 0.0f, 0.0f, Frames(5)},
    {0.6f, 0.04f, 0.0f, 0.0f, Frames(10)},
    {0.7f, 0.04f, 0.0f, 0.0f, Frames(10)},
    {0.7f, 0.01f, 0.0f, 0.0f, Frames(60)},
    {0.4f, 0.08f, 0.0f, 0.0f, Frames(5)},
    {0.2f, 0.04f, 0.0f, 0.0f, Frames(5)},
    {0.5f, 0.005f, 0.0f, 0.0f, Frames(100)},
    {0.4f, 0.005f, 0.0f, 0.0f, Frames(80)},
    {0.3f, 0.002f, 0.0f, 0.0f, Frames(50)},
    {0.1f, 0.08f, 0.0f, 0.0f, Frames(2)},
    {0.2f, 0.005f, 0.0f, 0.0f, Frames(10)},
    {0.3f, 0.005f, 0.0f, 0.0f, Frames(60)},
};

Pattern *GetPattern(int kind) {
    return &s_patterns[kind];
}

} // namespace Vibration

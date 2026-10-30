/* A unit with no functions of its own (file name inferred): the frame time Game/GameSystem.cpp sets,
 * the two frame timers Game/CameraRender.cpp fills, and a screen position, a size and two colours
 * with a static initializer (0x8004DEDC) that nothing reads. It starts on an 8-byte boundary in
 * .sbss after Game/TOneFileAsync.cpp's statics, and its static initializer is the last function
 * before Game/RenderUnit.cpp. The weak copies of the constructors the static initializer calls come
 * after it (their copies in Game/ScreenRect.cpp are the ones linked). */
#include "Game/EffLocusMultiVtx.h"
#include "Game/ScreenRect.h"
#include <dolphin/types.h>
#include "size_assert.h"

/* A frame timer (as in Game/CameraRender.cpp). */
struct UnkTimer_80573B78 {
    /* 0x00 */ u64 m_elapsed;
    /* 0x08 */ u64 m_start;
    /* 0x10 */ u8 field_0x10[0x20 - 0x10];
};
SIZE_ASSERT(UnkTimer_80573B78, 0x20);

float lbl_805EE1A8;
UnkTimer_80573B78 lbl_80573B78[2];

static ScreenPoint s_position = ScreenPoint(366, 0);
static ScreenSize s_size = ScreenSize(268, 48);
static UnkColor_8040D2EC s_colors[2] = {UnkColor_8040D2EC(255, 255, 0), UnkColor_8040D2EC(255, 0, 255)};

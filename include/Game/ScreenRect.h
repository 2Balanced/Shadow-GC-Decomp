#ifndef GAME_SCREENRECT_H
#define GAME_SCREENRECT_H

#include <dolphin/types.h>

/* A point on the screen, in pixels. Game/ScreenViewport.cpp and Game/CameraScreenSet.cpp declare
 * the same struct locally. The constructors are inline; static objects copy-initialized from them
 * call weak copies (0x80045F90 for ScreenPoint, 0x80045F84 for ScreenSize, both first emitted by
 * Game/ScreenRect.cpp). */
struct ScreenPoint {
    ScreenPoint(s32 x, s32 y) : x(x), y(y) {}

    s32 x;
    s32 y;
};

/* A size on the screen, in pixels (class name inferred: a second class with ScreenPoint's layout,
 * whose constructor has its own weak copy). */
struct ScreenSize {
    ScreenSize(s32 w, s32 h) : w(w), h(h) {}

    s32 w;
    s32 h;
};

/* A rectangle on the screen. Its constructor is out of line (0x80045EFC, Game/ScreenRect.cpp). */
struct ScreenRect {
    ScreenRect(ScreenPoint position, ScreenPoint size);

    ScreenPoint position;
    ScreenPoint size;
};

#endif

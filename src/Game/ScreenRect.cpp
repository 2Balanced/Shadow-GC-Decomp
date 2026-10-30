/* ScreenRect's constructor (the mangled name is the game's), and two screen positions with static
 * initializers that nothing reads. The file name is inferred. The weak copies of ScreenPoint's and
 * ScreenSize's constructors that the static initializer calls come after it (they are generated
 * last); the static initializer at 0x8004DEDC calls these copies too. */
#include "Game/ScreenRect.h"
#include <dolphin/types.h>

static ScreenPoint s_screenPosition = ScreenPoint(0, 340);
static ScreenSize s_screenSize = ScreenSize(640, 120);

ScreenRect::ScreenRect(ScreenPoint position, ScreenPoint size) : position(position), size(size) {}

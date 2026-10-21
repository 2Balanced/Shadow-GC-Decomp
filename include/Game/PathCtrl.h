#ifndef GAME_PATHCTRL_H
#define GAME_PATHCTRL_H

#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {

/* PathCtrl (class name from the RTTI of its boost::shared_ptr control block at 0x80528A1C): a path
 * a character or object follows (layout unknown). Its constructor (0x80019224) sets it up and its
 * destructor (0x80018770) releases it; ElecCircuitEntrance has the path manager fill one for the
 * circuit command, MOVE_FOOTING keeps one as a member, and Coaster and Monster hold one through a
 * shared_ptr. */
class PathCtrl {
  public:
    PathCtrl();
    ~PathCtrl();

    /* 0x00 */ u8 field_0x0[0x58];
};
SIZE_ASSERT(PathCtrl, 0x58);

} // namespace System
} // namespace SonicteamUSA

#endif

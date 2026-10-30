#ifndef GAME_PATHCTRL_H
#define GAME_PATHCTRL_H

#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {

/* PathCtrl (class name from the RTTI of its boost::shared_ptr control block at 0x80528A1C): a path
 * a character or object follows (Game/PathCtrl.cpp). Its constructor (0x80019224) sets it up and its
 * destructor (0x80018770) releases it; ElecCircuitEntrance has the path manager fill one for the
 * circuit command, MOVE_FOOTING keeps one as a member, and Coaster and Monster hold one through a
 * shared_ptr. */
class PathCtrl {
  public:
    PathCtrl();
    ~PathCtrl();

#ifdef GAME_PATHDATA_H
    /* The members, where Game/PathData.h is included first (other units see the bytes only, since
     * Game/PlayerPath.cpp declares JointPathData on its own; names inferred): the path, the
     * segment (the index of its first point) and the position in it (0 to 1), the path whose
     * joints were last looked up, the joints at its start and end, the branch callback and its
     * value, and whether the path is followed backwards. */
    /* 0x00 */ boost::weak_ptr<PathData> m_path;
    /* 0x08 */ u32 m_index;
    /* 0x0C */ float m_t;
    /* 0x10 */ PathData *m_jointsOf;
    /* 0x14 */ JointPathData m_startJoint;
    /* 0x2C */ JointPathData m_endJoint;
    /* 0x44 */ UnkBranchSlot_800AC16C m_branch;
    /* 0x50 */ u32 m_branchArg;
    /* 0x54 */ bool m_reverse;
#else
    /* 0x00 */ u8 field_0x0[0x58];
#endif
};
SIZE_ASSERT(PathCtrl, 0x58);

} // namespace System
} // namespace SonicteamUSA

#endif

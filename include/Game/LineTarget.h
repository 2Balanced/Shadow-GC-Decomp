#ifndef GAME_LINETARGET_H
#define GAME_LINETARGET_H

#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;

/* LineTarget (class name from RTTI; its base is Target): a target that other objects can aim at
 * along a line segment. fn_801E29D8 creates one and returns it through a
 * boost::shared_ptr<LineTarget>, whose weak instances (default constructor, destructor, assignment
 * and operator->) were first used by Game/TurnOver.cpp. Only the members gadget code touches are
 * mapped; their names are inferred. */
class LineTarget {
  public:
    /* Sets the radius around the segment (name inferred). */
    void SetRadius(const float &radius) {
        m_radius = radius;
    }

    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_radius;
};

extern "C" {
/* Creates a line target for charInfo with the given flags and registers it with the target
 * manager (behaviour inferred). */
boost::shared_ptr<LineTarget> fn_801E29D8(const boost::weak_ptr<CharInfo> &charInfo, int flags);
/* Target's position setter: copies position to 0x14 and sets flag bit 1 at 0x30 (behaviour
 * inferred from its body). */
void fn_801E73B0(LineTarget *target, const Vec3f *position);
/* Copies a 0x18-byte line segment (two points, built by fn_80020A20) to 0x38 (behaviour inferred
 * from its body). */
void fn_801E2C9C(LineTarget *target, const void *segment);
}

#endif

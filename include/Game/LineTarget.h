#ifndef GAME_LINETARGET_H
#define GAME_LINETARGET_H

#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/Target.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

class CharInfo;

/* LineTarget (class name from RTTI; vtable 0x8053D86C): a Target that other objects aim at along a
 * line segment; the position it reports is the point of the segment nearest to the one asking.
 * fn_801E29D8 (Game/LineTarget.cpp) creates one and returns it through a
 * boost::shared_ptr<LineTarget>, whose weak instances (default constructor, destructor, assignment
 * and operator->) were first used by Game/TurnOver.cpp. Member names are inferred. */
class LineTarget : public Target {
  public:
    LineTarget(const boost::weak_ptr<CharInfo> &owner, u32 flags);
    virtual TargetPosition GetPosition(const Vec3f *from);
    virtual ~LineTarget();

    /* Sets the radius around the segment (name inferred). */
    void SetRadius(const float &radius) {
        m_radius = radius;
    }

    /* 0x38 */ Sonicteam::System::Geometry::Line m_line;
};
SIZE_ASSERT(LineTarget, 0x50);

extern "C" {
/* Creates a line target for charInfo with the given flags and registers it with the target
 * manager (behaviour inferred). */
boost::shared_ptr<LineTarget> fn_801E29D8(const boost::weak_ptr<CharInfo> &charInfo, u32 flags);
/* Target's position setter (Game/Target.cpp): copies position to 0x14 and sets flag bit 1 at
 * 0x30. */
void fn_801E73B0(LineTarget *target, const Vec3f *position);
/* Copies a 0x18-byte line segment (two points, built by fn_80020A20) to the target's segment. */
void fn_801E2C9C(LineTarget *target, const void *segment);
}

#endif

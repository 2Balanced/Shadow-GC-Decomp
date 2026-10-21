#ifndef GAME_GEOMETRYLINE_H
#define GAME_GEOMETRYLINE_H

#include "Game/MathTypes.h"
#include "size_assert.h"

namespace Sonicteam {
namespace System {
namespace Geometry {

/* Line (class name from the RTTI of its shared_ptr control block, vtable 0x80525328): a line
 * segment between two points. Its constructor (0x80029DA4) does nothing; its boost::shared_ptr
 * and boost::weak_ptr instances (constructors 0x8008AB8C and 0x8008BC28, destructors 0x8008AA5C and
 * 0x8008BB7C, assignment 0x800F57F0) are shared. Member names are inferred. */
class Line {
  public:
    Line();

    /* 0x00 */ Vec3f m_start;
    /* 0x0C */ Vec3f m_end;
};
SIZE_ASSERT(Line, 0x18);

} // namespace Geometry
} // namespace System
} // namespace Sonicteam

#endif

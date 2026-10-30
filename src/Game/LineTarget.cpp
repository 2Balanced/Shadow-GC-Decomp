/* LineTarget (class name from RTTI; Game/LineTarget.h): a Target along a line segment. This unit has
 * the class and fn_801E29D8, which creates one in a shared_ptr whose deleter (DeleteLineTarget) also
 * has the target manager drop it, as Game/Target.cpp does for plain targets. File name from the
 * class. Names marked "(name inferred)" are not original. Definitions are in reverse address order
 * (deferred inlining emits functions last-defined first). */
#define BOOST_SP_DELETER_CONSTRUCTOR
#include "Game/LineTarget.h"
#include "Game/CharInfo.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/Target.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

using Sonicteam::System::Geometry::Line;

extern "C" {
/* Copies a line segment (name inferred). */
void fn_8002CCF0(Line *line, const void *from);
/* The point of a segment nearest to point (see Game/ArkLaser.cpp). */
Vec3f fn_8040FC3C(const void *segment, const Vec3f *point);
}

/* The vector type's out-of-line assignment, called with a temporary the way the original's
 * operator is (name inferred; see Game/JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

LineTarget::LineTarget(const boost::weak_ptr<CharInfo> &owner, u32 flags) : Target(owner, flags) {}

LineTarget::~LineTarget() {}

/* Sets the segment (name inferred; other units call it by its address). */
extern "C" void fn_801E2C9C(LineTarget *target, const void *segment) {
    fn_8002CCF0(&target->m_line, segment);
}

TargetPosition LineTarget::GetPosition(const Vec3f *from) {
    AssignVector(m_position, fn_8040FC3C(&m_line, from));
    return m_position;
}

/* The deleter of the shared_ptrs line targets are created in (name inferred). */
static void DeleteLineTarget(LineTarget *target) {
    delete target;
    fn_801E7A20(fn_80060058());
}

/* Creates a line target for owner (name inferred). */
extern "C" boost::shared_ptr<LineTarget> fn_801E29D8(const boost::weak_ptr<CharInfo> &owner, u32 flags) {
    boost::shared_ptr<LineTarget> target(new LineTarget(owner, flags), DeleteLineTarget);
    fn_801E7AF4(fn_80060058(), target);
    return target;
}

/* SonicteamUSA::System::Range::RangeCapsule (class name and base from RTTI): a capsule (shape kind 2,
 * Game/RangeCapsule.h). Its tests against spheres work on its axis segment (m_line); against lines
 * and segments it splits the query into the two round ends and the straight side, which it tests in
 * its own space (m_inverse). A capsule of no height tests as a sphere (Game/RangeSphere.cpp). Member
 * names and meanings are inferred. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#define RANGE_UNIT
#include "Game/CharaColliGeneral.h"
#include "Game/RangeCapsule.h"
#include "Game/CharaColli.h"
#include "Game/DebugDisp.h"
#include "Game/DebugDraw.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "Game/RangeBox.h"
#include "Game/RangeSphere.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using Sonicteam::System::Geometry::Line;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::Range::RangeBase;
using SonicteamUSA::System::Range::RangeBox;
using SonicteamUSA::System::Range::RangeCapsule;
using SonicteamUSA::System::Range::RangeSphere;

namespace std {
float fabs(float x);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
typedef Fn8040CopyVector3 (*VectorCopyFn)(const Vec3f *v);
typedef Mtx34f (*TranslationFn)(const Vec3f *offset);

extern "C" {
float fn_8000FEE4(float x); /* square root */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* a . b */
float fn_8040D180(const Vec3f *a, const Vec3f *b); /* |a - b|^2 */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b); /* |a - b| */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *v); /* v normalized */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* a x b */
void fn_8040CE04(Vec3f *v, const Vec3f *other);
void fn_8040CE38(Vec3f *v, float scale);
void fn_8040CE60(Vec3f *v); /* sets v to zero */
void fn_8040CC6C(Vec2f *v); /* normalizes v */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *direction);
Mtx34f fn_8040B0C0(float angle);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
Mtx34f fn_8040BEF8(const Mtx34f *matrix); /* inverse */
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
float *fn_8040C17C(Mtx34f *matrix, int row, int column);
Vec3f fn_800091A4(const Vec3f *v, float scale);
Vec3f fn_8001DFE8(const Vec3f *v);
Vec3f fn_80016284(const Vec3f *v); /* -v */
float fn_80016260(float y, float x);
UnkSphere_8004E6CC *fn_80021C04(UnkSphere_8004E6CC *sphere, const Vec3f *center, float radius);
float fn_8003C660(const Vec3f *point, const UnkDebugLine_80020A20 *line, Vec3f *closest);
float fn_8003BC14(const UnkDebugLine_80020A20 *a, const UnkDebugLine_80020A20 *b, const Vec3f *directionA,
                  const Vec3f *directionB, Vec3f *closestA, Vec3f *closestB);
bool fn_8003B7AC(float *t0, float *t1, float a, float b, float c);
bool fn_800259C0(const Vec3f *point, const Vec3f *move, const Vec3f *axis, Vec3f *hit);
bool fn_80028F64(const Vec3f *point, const UnkDebugLine_80020A20 *segment, float radius, Vec3f *closest,
                 float *distance);
bool fn_80029118(RangeBase *a, RangeBase *b);
void fn_80020A20(UnkDebugLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
}

/* The sphere's tests (Game/RangeSphere.cpp). */
bool TestSphere(const Vec3f *position, const UnkSphere_8004E6CC *sphere, float radius,
                UnkColliContact_8001FF00 *result);
bool SweepSphere(const Vec3f *position, const UnkSphere_8004E6CC *sphere, Vec3f *move,
                 UnkColliContact_8001FF00 *result, float radius);
bool TestLine(const Vec3f *position, const UnkDebugLine_80020A20 *line, float radius,
              UnkColliContact_8001FF00 *result);
bool TestSegment(const Vec3f *position, const UnkDebugLine_80020A20 *segment, float radius,
                 UnkColliContact_8001FF00 *result);

Vec3f operator-(const Vec3f &a, const Vec3f &b);

extern const Vec3f lbl_80570C00;

/* Assigns through the out-of-line vector assignment (fn_800091D0). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* A vector built from three floats and assigned through the out-of-line assignment. */
struct RangeAxis {
    RangeAxis(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    void operator=(const Vec3f &v) {
        fn_800091D0(reinterpret_cast<Vec3f *>(this), &v);
    }

    float x;
    float y;
    float z;
};

/* A local matrix of the game's matrix class (copied through fn_8040C148). */
struct RangeMatrixCopy {
    RangeMatrixCopy(const Mtx34f &from) {
        fn_8040C148(&m, &from);
    }

    Mtx34f m;
};

/* Plain data the size of a line (lines are built in it through fn_80020A20). */
struct RangeLineData {
    float v[6];
};

/* Plain data the size of a sphere description. */
struct RangeFloat4 {
    float v[4];
};

/* A sphere description built in place by fn_80021C04, as a temporary. */
struct RangeSphereTemp {
    RangeSphereTemp() {}

    float v[4];
};

/* Three floats (the description's centre is built through one). */
struct RangeFloat3 {
    float v[3];
};

extern "C" Vec3f fn_80029418(const Mtx34f *matrix, s32 order);

bool TestCapsule(const Line *segment, const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result, float radius,
                 float halfHeight);
bool SweepCapsule(const Line *segment, const UnkSphere_8004E6CC *sphere, Vec3f *move, UnkColliContact_8001FF00 *result,
                  float radius, float halfHeight);

inline const UnkDebugLine_80020A20 *AsLine(const Line *line) {
    return reinterpret_cast<const UnkDebugLine_80020A20 *>(line);
}

typedef bool (*HitFn)(RangeBase *self, RangeBase *other);

extern "C" bool fn_8002F2C8(RangeBase *self, RangeBase *other);
extern "C" bool fn_8002EC10(RangeBase *self, RangeBase *other);
extern "C" bool fn_8002EAC0(RangeBase *self, RangeBase *other);

/* The tests by the other shape's kind (sphere, box, capsule). */
static HitFn s_hitTable[] = {fn_8002F2C8, fn_8002EC10, fn_8002EAC0};

UnkCapsule_80030A5C::UnkCapsule_80030A5C(const Vec3f &center, float halfHeight, float radius, const Vec3f &rotation,
                                         int arg4) {
    {
        RangeFloat3 c = {0.0f, 0.0f, 0.0f};
        c.v[0] = center.x;
        c.v[1] = center.y;
        c.v[2] = center.z;
        *reinterpret_cast<RangeFloat3 *>(&m_center) = c;
    }
    m_halfHeight = halfHeight;
    m_radius = radius;
    m_rotation.x = rotation.x;
    m_rotation.y = rotation.y;
    m_rotation.z = rotation.z;
    field_0x20 = arg4;
}

UnkCapsule_80030A5C::UnkCapsule_80030A5C(const UnkCapsule_80030A5C &other) {
    *reinterpret_cast<RangeFloat3 *>(&m_center) = *reinterpret_cast<const RangeFloat3 *>(&other.m_center);
    m_halfHeight = other.m_halfHeight;
    m_radius = other.m_radius;
    m_rotation.x = other.m_rotation.x;
    m_rotation.y = other.m_rotation.y;
    m_rotation.z = other.m_rotation.z;
    field_0x20 = other.field_0x20;
}

RangeCapsule::RangeCapsule(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position, const Vec3f *rotation,
                           s32 order, boost::weak_ptr<CharaColliBase> owner)
    : RangeBase(2, *position, desc->field_0x8, desc->field_0xC, owner),
      m_capsule(*reinterpret_cast<const UnkCapsule_80030A5C *>(desc->field_0x4)) {
    UpdateRotationFlags();
    Update(position, rotation, order);
    UpdateBounds();
}

RangeCapsule::~RangeCapsule() {}

bool RangeCapsule::vf_0x48(RangeBase *other) {
    return s_hitTable[other->m_priority](this, other);
}

bool RangeCapsule::vf_0x4C(RangeBase *other, UnkColliContact_8001FF00 *mine, UnkColliContact_8001FF00 *theirs) {
    switch (other->m_priority) {
    case 0: {
        bool hit = false;
        if (theirs != 0) {
            float radius;
            other->GetSize(&radius, 0, 0);
            RangeFloat4 sphere;
            hit = vf_0x38(fn_80021C04(reinterpret_cast<UnkSphere_8004E6CC *>(&sphere),
                                      reinterpret_cast<const Vec3f *>(&other->m_position), radius),
                          theirs);
        }
        return hit;
    }
    }
    return false;
}

bool RangeCapsule::vf_0x30(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result) {
    if (GetHalfHeight() < 0.0001f) {
        return TestLine(reinterpret_cast<const Vec3f *>(&m_position), line, m_capsule.m_radius, result);
    }
    float radius = 0.001f + m_margin;
    if (fn_8003C660(reinterpret_cast<const Vec3f *>(&m_position), line, 0) > radius * radius) {
        return false;
    }
    Fn8040CopyVector3 direction(((VectorSubtractFn)fn_8040CD74)(&line->m_end, &line->m_start));
    Fn8040CopyVector3 axis(((VectorSubtractFn)fn_8040CD74)(&m_line.m_end, &m_line.m_start));
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(&line->m_start, &m_line.m_start));
    float along = fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&offset));
    radius = m_capsule.m_radius;
    int startSide;
    if (along < 0.0f) {
        if (fn_8040D180(&m_line.m_start, &line->m_start) < radius * radius) {
            if (result != 0) {
                result->SetPosition(line->m_start);
                result->m_distance = 0.0f;
                result->field_0x10 = 0.0f;
                result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&line->m_start, &m_line.m_start)));
            }
            return true;
        }
        startSide = 0;
    } else if (along > fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&axis))) {
        if (fn_8040D180(&m_line.m_end, &line->m_start) < radius * radius) {
            if (result != 0) {
                result->SetPosition(line->m_start);
                result->m_distance = 0.0f;
                result->field_0x10 = 0.0f;
                result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&line->m_end, &m_line.m_start)));
                result->field_0x2C = 3;
            }
            return true;
        }
        startSide = 2;
    } else {
        startSide = 1;
    }
    Fn8040CopyVector3 endOffset(((VectorSubtractFn)fn_8040CD74)(&line->m_end, &m_line.m_start));
    float endAlong = fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&endOffset));
    int endSide;
    if (endAlong < 0.0f) {
        endSide = 0;
    } else if (endAlong > fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&axis))) {
        endSide = 2;
    } else {
        endSide = 1;
    }
    bool outside = false;
    if (startSide == 0) {
        if (HitEndLine(&m_line.m_start, &fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)), line, result)) {
            return true;
        }
        if (endSide == 0) {
            return false;
        }
        if (HitSide(line, &outside, result)) {
            return true;
        }
        if (outside) {
            return false;
        }
        if (endSide == 1) {
            return false;
        }
        return HitEndLine(&m_line.m_end, reinterpret_cast<const Vec3f *>(&m_axis), line, result);
    } else if (startSide == 2) {
        if (HitEndLine(&m_line.m_end, reinterpret_cast<const Vec3f *>(&m_axis), line, result)) {
            return true;
        }
        if (endSide == 2) {
            return false;
        }
        if (HitSide(line, &outside, result)) {
            return true;
        }
        if (outside) {
            return false;
        }
        if (endSide == 1) {
            return false;
        }
        return HitEndLine(&m_line.m_start, &fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)), line, result);
    } else {
        if (HitSide(line, &outside, result)) {
            return true;
        }
        if (outside) {
            return false;
        }
        if (endSide == 0) {
            return HitEndLine(&m_line.m_start, &fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)), line, result);
        }
        if (endSide == 2) {
            return HitEndLine(&m_line.m_end, reinterpret_cast<const Vec3f *>(&m_axis), line, result);
        }
        return false;
    }
}

bool RangeCapsule::HitEndLine(const Vec3f *point, const Vec3f *direction, const UnkDebugLine_80020A20 *line,
                              UnkColliContact_8001FF00 *result) {
    float radiusSq = m_capsule.m_radius * m_capsule.m_radius;
    Vec3f closest;
    bool hit = fn_8003C660(point, line, &closest) < radiusSq;
    if (hit) {
        float distance = fn_8040D180(point, &closest);
        float t = fn_8000FEE4((radiusSq - distance) / fn_8040D180(&line->m_start, &line->m_end));
        fn_8040CE04(&closest, &fn_800091A4(&(line->m_end - line->m_start), t));
        if (fn_8040CF90(&((VectorSubtractFn)fn_8040CD74)(&closest, point), direction) < -0.0001f) {
            return false;
        }
        if (result != 0) {
            result->SetPosition(closest);
            result->m_distance = fn_8040D1B4(&closest, &line->m_start);
            result->field_0x10 = result->GetDistance();
            result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&closest, point)));
            result->field_0x2C = 0;
        }
    }
    return hit;
}

bool RangeCapsule::HitSide(const UnkDebugLine_80020A20 *segment, bool *outside, UnkColliContact_8001FF00 *result) {
    *outside = false;
    Fn8040CopyVector3 direction(((VectorSubtractFn)fn_8040CD74)(&segment->m_end, &segment->m_start));
    Fn8040CopyVector3 start(fn_8040B67C(&m_inverse, &segment->m_start));
    Fn8040CopyVector3 move(fn_8040B618(&m_inverse, reinterpret_cast<const Vec3f *>(&direction)));
    float radius = m_capsule.m_radius;
    if (start.x * start.x + start.y * start.y <= 1.0001f * (radius * radius)) {
        if (std::fabs(start.z) <= HalfHeight()) {
            if (result != 0) {
                result->SetPosition(segment->m_start);
                result->m_distance = 0.0f;
                result->field_0x10 = 0.0f;
                Vec2f flat(start.x, start.y);
                fn_8040CC6C(&flat);
                Vec3f normal(flat.x, flat.y, 0.0f);
                result->SetNormal(fn_8040CEE8(&fn_8040B618(&m_matrix, &normal)));
                result->field_0x2C = 3;
            }
            return true;
        }
        *outside = true;
        return false;
    }
    float along = start.x * move.x + start.y * move.y;
    if (along >= -0.0001f) {
        return false;
    }
    float t0;
    float t1;
    if (fn_8003B7AC(&t0, &t1, move.x * move.x + move.y * move.y, 2.0f * along, start.x * start.x + start.y * start.y - radius * radius)) {
        if (t0 > 0.0f) {
            float t = t0 < t1 ? t0 : t1;
            if (t > 1.0f) {
                return false;
            }
            Fn8040CopyVector3 hit(fn_8040CDD0(reinterpret_cast<const Vec3f *>(&start),
                                              &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&move), t)));
            if (std::fabs(hit.z) <= HalfHeight()) {
                if (result != 0) {
                    result->SetPosition(fn_8040B67C(&m_matrix, reinterpret_cast<const Vec3f *>(&hit)));
                    result->m_distance = t * fn_8040CF50(reinterpret_cast<const Vec3f *>(&move));
                    Vec2f flat(hit.x, hit.y);
                    fn_8040CC6C(&flat);
                    hit.x = flat.x;
                    hit.y = flat.y;
                    hit.z = 0.0f;
                    result->SetNormal(fn_8040B618(&m_matrix, reinterpret_cast<const Vec3f *>(&hit)));
                    result->field_0x2C = 0;
                }
                return true;
            }
        } else {
            return false;
        }
    }
    return false;
}

bool RangeCapsule::vf_0x34(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result) {
    if (GetHalfHeight() < 0.0001f) {
        return TestSegment(reinterpret_cast<const Vec3f *>(&m_position), segment, m_capsule.m_radius, result);
    }
    float bound = 0.001f + m_margin;
    if (!fn_80028F64(reinterpret_cast<const Vec3f *>(&m_position), segment, bound * bound, 0, 0)) {
        return false;
    }
    Fn8040CopyVector3 direction(((VectorSubtractFn)fn_8040CD74)(&segment->m_end, &segment->m_start));
    Fn8040CopyVector3 axis(((VectorSubtractFn)fn_8040CD74)(&m_line.m_end, &m_line.m_start));
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(&segment->m_start, &m_line.m_start));
    float along = fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&offset));
    float radius = m_capsule.m_radius;
    int startSide;
    if (along < 0.0f) {
        if (fn_8040D180(&m_line.m_start, &segment->m_start) < radius * radius) {
            if (result != 0) {
                result->SetPosition(segment->m_start);
                result->m_distance = 0.0f;
                result->field_0x10 = 0.0f;
                result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&segment->m_start, &m_line.m_start)));
            }
            return true;
        }
        startSide = 0;
    } else if (along > fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&axis))) {
        if (fn_8040D180(&m_line.m_end, &segment->m_start) < radius * radius) {
            if (result != 0) {
                result->SetPosition(segment->m_start);
                result->m_distance = 0.0f;
                result->field_0x10 = 0.0f;
                result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&segment->m_end, &m_line.m_start)));
                result->field_0x2C = 3;
            }
            return true;
        }
        startSide = 2;
    } else {
        startSide = 1;
    }
    Fn8040CopyVector3 endOffset(((VectorSubtractFn)fn_8040CD74)(&segment->m_end, &m_line.m_start));
    float endAlong = fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&endOffset));
    int endSide;
    if (endAlong < 0.0f) {
        endSide = 0;
    } else if (endAlong > fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&axis))) {
        endSide = 2;
    } else {
        endSide = 1;
    }
    bool outside = false;
    if (startSide == 0) {
        if (HitEndSegment(&m_line.m_start, &fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)), segment, result)) {
            return true;
        }
        if (endSide == 0) {
            return false;
        }
        if (HitSide(segment, &outside, result)) {
            return true;
        }
        if (outside) {
            return false;
        }
        if (endSide == 1) {
            return false;
        }
        return HitEndSegment(&m_line.m_end, reinterpret_cast<const Vec3f *>(&m_axis), segment, result);
    } else if (startSide == 2) {
        if (HitEndSegment(&m_line.m_end, reinterpret_cast<const Vec3f *>(&m_axis), segment, result)) {
            return true;
        }
        if (endSide == 2) {
            return false;
        }
        if (HitSide(segment, &outside, result)) {
            return true;
        }
        if (outside) {
            return false;
        }
        if (endSide == 1) {
            return false;
        }
        return HitEndSegment(&m_line.m_start, &fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)), segment,
                             result);
    } else {
        if (HitSide(segment, &outside, result)) {
            return true;
        }
        if (outside) {
            return false;
        }
        if (endSide == 0) {
            return HitEndSegment(&m_line.m_start, &fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)), segment,
                                 result);
        }
        if (endSide == 2) {
            return HitEndSegment(&m_line.m_end, reinterpret_cast<const Vec3f *>(&m_axis), segment, result);
        }
        return false;
    }
}

bool RangeCapsule::HitEndSegment(const Vec3f *point, const Vec3f *direction, const UnkDebugLine_80020A20 *segment,
                                 UnkColliContact_8001FF00 *result) {
    float radiusSq = m_capsule.m_radius * m_capsule.m_radius;
    Vec3f closest;
    float distance;
    bool hit = fn_80028F64(point, segment, radiusSq, &closest, &distance);
    if (hit) {
        float t = fn_8000FEE4((radiusSq - distance) /
                              ((segment->m_start.y - segment->m_end.y) * (segment->m_start.y - segment->m_end.y)));
        closest.y -= t * (segment->m_end.y - segment->m_start.y);
        if (fn_8040CF90(&((VectorSubtractFn)fn_8040CD74)(&closest, point), direction) < -0.0001f) {
            return false;
        }
        if (result != 0) {
            result->SetPosition(closest);
            result->m_distance = std::fabs(closest.y - segment->m_start.y);
            result->field_0x10 = result->GetDistance();
            result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&closest, point)));
            result->field_0x2C = 0;
        }
    }
    return hit;
}

bool RangeCapsule::vf_0x38(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result) {
    return TestCapsule(&m_line, sphere, result, m_capsule.m_radius, m_capsule.m_halfHeight);
}

bool RangeCapsule::vf_0x3C(const UnkSphere_8004E6CC *sphere, const Vec3f *move, UnkColliContact_8001FF00 *result) {
    float radius = 0.001f + (sphere->m_radius + m_margin);
    RangeLineData path;
    fn_80020A20(reinterpret_cast<UnkDebugLine_80020A20 *>(&path), &sphere->m_center,
                &fn_8040CDD0(&sphere->m_center, move));
    if (fn_8003C660(reinterpret_cast<const Vec3f *>(&m_position), reinterpret_cast<const UnkDebugLine_80020A20 *>(&path),
                    0) > radius * radius) {
        return false;
    }
    return SweepCapsule(&m_line, sphere, const_cast<Vec3f *>(move), result, m_capsule.m_radius,
                        m_capsule.m_halfHeight);
}

void RangeCapsule::Update(const Vec3f *position, const Vec3f *rotation, s32 order) {
    fn_8040C148(&m_matrix, &((TranslationFn)fn_8040B034)(position));
    fn_8001E070(&m_matrix, rotation, order);
    fn_8040BAC4(&m_matrix, &((TranslationFn)fn_8040B034)(reinterpret_cast<const Vec3f *>(&GetExtent())));
    fn_8001E070(&m_matrix, GetBaseRotation(), GetRotationOrder());
    m_position.x = *fn_8040C17C(&m_matrix, 0, 3);
    m_position.y = *fn_8040C17C(&m_matrix, 1, 3);
    m_position.z = *fn_8040C17C(&m_matrix, 2, 3);
    fn_8040C148(&m_inverse, &fn_8040BEF8(&m_matrix));
    UpdateLine();
}

void RangeCapsule::SetMatrix(Mtx34f *matrix) {
    fn_8040C148(&m_matrix, &fn_8001E044(matrix, &((TranslationFn)fn_8040B034)(
                                                    reinterpret_cast<const Vec3f *>(&GetExtent()))));
    fn_8001E070(&m_matrix, GetBaseRotation(), GetRotationOrder());
    m_position.x = *fn_8040C17C(&m_matrix, 0, 3);
    m_position.y = *fn_8040C17C(&m_matrix, 1, 3);
    m_position.z = *fn_8040C17C(&m_matrix, 2, 3);
    fn_8040C148(&m_inverse, &fn_8040BEF8(&m_matrix));
    UpdateLine();
}

void RangeCapsule::UpdateLine() {
    RangeAxis axis(0.0f, 0.0f, GetHalfHeight());
    axis = fn_8040B618(&m_matrix, reinterpret_cast<const Vec3f *>(&axis));
    AssignVector(m_line.m_start, ((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&m_position),
                                                                  reinterpret_cast<const Vec3f *>(&axis)));
    AssignVector(m_line.m_end, fn_8040CDD0(reinterpret_cast<const Vec3f *>(&m_position),
                                           reinterpret_cast<const Vec3f *>(&axis)));
    AssignVector(*reinterpret_cast<Vec3f *>(&m_axis), fn_8040CEE8(reinterpret_cast<const Vec3f *>(&axis)));
}

void RangeCapsule::UpdateBounds() {
    m_margin = m_capsule.m_halfHeight + m_capsule.m_radius;
}

void RangeCapsule::SetScale(const Vec3f *scale) {
    Fn8040CopyVector3 offset(GetExtent());
    offset.x = offset.x * scale->x;
    offset.y = offset.y * scale->y;
    offset.z = offset.z * scale->z;
    SetOffset(reinterpret_cast<const Vec3f *>(&offset));
    if (m_owner.lock()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), owner->vf_0x18(), owner->vf_0x24());
    } else {
        UpdateLine();
    }
    SetSize(scale->x * m_capsule.m_radius, 0.0f, scale->z * m_capsule.m_halfHeight);
}

void RangeCapsule::GetSize(float *x, float *y, float *z) {
    if (x != 0) {
        *x = m_capsule.m_radius;
    }
    if (z != 0) {
        *z = m_capsule.m_halfHeight;
    }
}

void RangeCapsule::SetSize(float x, float y, float z) {
    m_capsule.m_radius = x;
    m_capsule.m_halfHeight = z;
    UpdateBounds();
}

/* a . b */
inline float Dot(const Fn8040CopyVector3 &a, const Vec3f &b) {
    return fn_8040CF90(reinterpret_cast<const Vec3f *>(&a), &b);
}

/* A sphere description of the sphere shape s, built in tmp. */
inline UnkSphere_8004E6CC *SphereOf(RangeFloat4 *tmp, RangeSphere *s) {
    return fn_80021C04(reinterpret_cast<UnkSphere_8004E6CC *>(tmp), reinterpret_cast<const Vec3f *>(&s->m_position),
                       s->GetRadius());
}

extern "C" bool fn_8002F2C8(RangeBase *self, RangeBase *other) {
    RangeCapsule *capsule = static_cast<RangeCapsule *>(self);
    RangeSphere *sphere = static_cast<RangeSphere *>(other);
    if (0.0f == (capsule->m_capsule.m_halfHeight < 0.0001f)) {
        RangeFloat4 local;
        return TestSphere(reinterpret_cast<const Vec3f *>(&capsule->m_position), SphereOf(&local, sphere),
                          capsule->GetRadius(), 0);
    }
    if (fn_80029118(capsule, sphere)) {
        float radius = capsule->m_capsule.m_radius;
        radius = 0.001f + (radius + sphere->GetRadius());
        return fn_8003C660(reinterpret_cast<const Vec3f *>(&sphere->m_position), AsLine(&capsule->m_line), 0) <
               radius * radius;
    }
    return false;
}

extern "C" bool fn_8002EC10(RangeBase *self, RangeBase *other) {
    RangeCapsule *capsule = static_cast<RangeCapsule *>(self);
    if (capsule->GetHalfHeight() < 0.0001f) {
        return other->vf_0x38(fn_80021C04(reinterpret_cast<UnkSphere_8004E6CC *>(&RangeSphereTemp()),
                                        reinterpret_cast<const Vec3f *>(&capsule->m_position),
                                        capsule->m_capsule.m_radius),
                            0);
    }
    RangeBox *box = static_cast<RangeBox *>(other);
    if (fn_80029118(capsule, box)) {
        Fn8040CopyVector3 t(fn_8040B618(&capsule->m_inverse,
                                        &((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&box->m_position),
                                                                         reinterpret_cast<const Vec3f *>(&capsule->m_position))));
        float angle;
        if (std::fabs(t.x) > 0.0001f) {
            angle = fn_80016260(t.y, t.x);
        } else {
            angle = 0.0f;
        }
        RangeMatrixCopy rotation(fn_8001E044(&fn_8040B0C0(-angle), &capsule->m_inverse));
        AssignVector(*reinterpret_cast<Vec3f *>(&t),
                     fn_8040B618(&rotation.m, &((VectorSubtractFn)fn_8040CD74)(
                                                  reinterpret_cast<const Vec3f *>(&box->m_position),
                                                  reinterpret_cast<const Vec3f *>(&capsule->m_position))));
        fn_8040BAC4(&rotation.m, box->GetMatrix());
        Vec3f c0(rotation.m.values[0][0], rotation.m.values[1][0], rotation.m.values[2][0]);
        Vec3f c1(rotation.m.values[0][1], rotation.m.values[1][1], rotation.m.values[2][1]);
        Vec3f c2(rotation.m.values[0][2], rotation.m.values[1][2], rotation.m.values[2][2]);
        Fn8040CopyVector3 hb(box->GetHalfSize());
        float h = capsule->m_capsule.m_halfHeight;
        float r = capsule->m_capsule.m_radius;

        {
            float lim = 0.001f + (r + hb.x * std::fabs(c0.x) + hb.y * std::fabs(c1.x) + hb.z * std::fabs(c2.x));
            if (std::fabs(t.x) > lim) {
                return false;
            }
        }
        {
            float lim = 0.001f + ((r + h) + hb.x * std::fabs(c0.z) + hb.y * std::fabs(c1.z) + hb.z * std::fabs(c2.z));
            if (std::fabs(t.z) > lim) {
                return false;
            }
        }

        {
            if (std::fabs(Dot(t, c0)) > 0.001f + (hb.x + r + h * std::fabs(c0.z))) {
                return false;
            }
        }
        {
            if (std::fabs(Dot(t, c1)) > 0.001f + (hb.y + r + h * std::fabs(c1.z))) {
                return false;
            }
        }
        {
            if (std::fabs(Dot(t, c2)) > 0.001f + (hb.z + r + h * std::fabs(c2.z))) {
                return false;
            }
        }

        {
            if (std::fabs(-t.x * c0.y + t.y * c0.x) > 0.001f + (r + hb.y * std::fabs(c2.z) + hb.z * std::fabs(c1.z))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.x * c1.y + t.y * c1.x) > 0.001f + (r + hb.z * std::fabs(c0.z) + hb.x * std::fabs(c2.z))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.x * c2.y + t.y * c2.x) > 0.001f + (r + hb.x * std::fabs(c1.z) + hb.y * std::fabs(c0.z))) {
                return false;
            }
        }

        {
            if (std::fabs(-t.y * c0.z + t.z * c0.y) > 0.001f + (r + h * std::fabs(c0.y) + hb.y * std::fabs(c2.x) + hb.z * std::fabs(c1.x))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.y * c1.z + t.z * c1.y) > 0.001f + (r + h * std::fabs(c1.y) + hb.z * std::fabs(c0.x) + hb.x * std::fabs(c2.x))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.y * c2.z + t.z * c2.y) > 0.001f + (r + h * std::fabs(c2.y) + hb.x * std::fabs(c1.x) + hb.y * std::fabs(c0.x))) {
                return false;
            }
        }

        {
            if (std::fabs(-t.z * c0.x + t.x * c0.z) > 0.001f + (r + h * std::fabs(c0.x) + hb.y * std::fabs(c2.y) + hb.z * std::fabs(c1.y))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.z * c1.x + t.x * c1.z) > 0.001f + (r + h * std::fabs(c1.x) + hb.z * std::fabs(c0.y) + hb.x * std::fabs(c2.y))) {
                return false;
            }
        }
        return !(std::fabs(-t.z * c2.x + t.x * c2.z) >
                 0.001f + (r + h * std::fabs(c2.x) + hb.x * std::fabs(c1.y) + hb.y * std::fabs(c0.y)));
    }
    return false;
}

extern "C" bool fn_8002EAC0(RangeBase *self, RangeBase *other) {
    RangeCapsule *a = static_cast<RangeCapsule *>(self);
    RangeCapsule *b = static_cast<RangeCapsule *>(other);
    if (fn_80029118(a, b)) {
        float radius = 0.001f + (a->m_capsule.m_radius + b->m_capsule.m_radius);
        bool aFlat = a->m_capsule.m_halfHeight < 0.0001f;
        bool bFlat = b->m_capsule.m_halfHeight < 0.0001f;
        if (aFlat || bFlat) {
            if (aFlat && bFlat) {
                return fn_8040D180(reinterpret_cast<const Vec3f *>(&a->m_position),
                                   reinterpret_cast<const Vec3f *>(&b->m_position)) < radius * radius;
            }
            if (aFlat) {
                return fn_8003C660(reinterpret_cast<const Vec3f *>(&a->m_position), AsLine(&b->m_line), 0) <
                       radius * radius;
            }
            if (bFlat) {
                return fn_8003C660(reinterpret_cast<const Vec3f *>(&b->m_position), AsLine(&a->m_line), 0) <
                       radius * radius;
            }
        }
        return fn_8003BC14(AsLine(&a->m_line), AsLine(&b->m_line), reinterpret_cast<const Vec3f *>(&a->m_axis),
                           reinterpret_cast<const Vec3f *>(&b->m_axis), 0, 0) <= radius * radius;
    }
    return false;
}

/* A sphere against the capsule of the given axis segment, radius and half height. */
bool TestCapsule(const Line *segment, const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result, float radius,
                 float halfHeight) {
    if (halfHeight < 0.0001f) {
        return TestSphere(&fn_8040CDA8(&fn_8040CDD0(&segment->m_start, &segment->m_end), 0.5f), sphere, radius,
                          result);
    }
    float bound = 0.001f + (sphere->m_radius + (halfHeight + radius));
    if (fn_8003C660(&sphere->m_center, AsLine(segment), 0) > bound * bound) {
        return false;
    }
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, &segment->m_start));
    Fn8040CopyVector3 axis(((VectorSubtractFn)fn_8040CD74)(&segment->m_end, &segment->m_start));
    float along = fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&offset));
    if (along < 0.0f) {
        float reach = radius + sphere->m_radius;
        float distanceSq = fn_8040D180(&segment->m_start, &sphere->m_center);
        if (distanceSq < radius * radius) {
            if (result != 0) {
                result->SetPosition(sphere->m_center);
                result->m_distance = 0.0f;
                result->field_0x10 = -sphere->m_radius - (radius - fn_8000FEE4(distanceSq));
                if (0.0f == distanceSq) {
                    result->field_0x2C = 4;
                    result->SetNormal(lbl_80570C00);
                } else {
                    result->field_0x2C = 3;
                    result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, &segment->m_start)));
                }
            }
            return true;
        }
        if (distanceSq < reach * reach) {
            if (result != 0) {
                Fn8040CopyVector3 normal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, &segment->m_start)));
                result->SetPosition(fn_8040CDD0(&segment->m_start, &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&normal), radius)));
                result->SetNormal(*reinterpret_cast<const Vec3f *>(&normal));
                result->m_distance = fn_8000FEE4(distanceSq) - radius;
                result->field_0x10 = fn_8000FEE4(distanceSq) - radius - sphere->m_radius;
                result->field_0x2C = 0;
            }
            return true;
        }
        return false;
    } else if (along > fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&axis))) {
        float reach = radius + sphere->m_radius;
        float distanceSq = fn_8040D180(&segment->m_end, &sphere->m_center);
        if (distanceSq < radius * radius) {
            if (result != 0) {
                result->SetPosition(sphere->m_center);
                result->m_distance = 0.0f;
                result->field_0x10 = -sphere->m_radius - (radius - fn_8000FEE4(distanceSq));
                if (0.0f == distanceSq) {
                    result->field_0x2C = 4;
                    result->SetNormal(lbl_80570C00);
                } else {
                    result->field_0x2C = 3;
                    result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, &segment->m_end)));
                }
            }
            return true;
        }
        if (distanceSq < reach * reach) {
            if (result != 0) {
                Fn8040CopyVector3 normal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, &segment->m_end)));
                result->SetPosition(fn_8040CDD0(&segment->m_end, &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&normal), radius)));
                result->SetNormal(*reinterpret_cast<const Vec3f *>(&normal));
                result->m_distance = fn_8000FEE4(distanceSq) - radius;
                result->field_0x10 = fn_8000FEE4(distanceSq) - radius - sphere->m_radius;
                result->field_0x2C = 0;
            }
            return true;
        }
        return false;
    } else {
        Fn8040CopyVector3 closest(fn_8040CDD0(&segment->m_start,
                                              &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&axis),
                                                           along / fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis),
                                                                               reinterpret_cast<const Vec3f *>(&axis)))));
        float distanceSq = fn_8040D180(&sphere->m_center, reinterpret_cast<const Vec3f *>(&closest));
        float reach = radius + sphere->m_radius;
        if (distanceSq < radius * radius) {
            if (result != 0) {
                result->SetPosition(sphere->m_center);
                result->m_distance = 0.0f;
                result->field_0x10 = -sphere->m_radius - (radius - fn_8000FEE4(distanceSq));
                if (0.0f == distanceSq) {
                    result->field_0x2C = 4;
                    result->SetNormal(lbl_80570C00);
                } else {
                    result->field_0x2C = 3;
                    result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(
                        &sphere->m_center, reinterpret_cast<const Vec3f *>(&closest))));
                }
            }
            return true;
        }
        if (distanceSq < reach * reach) {
            if (result != 0) {
                Fn8040CopyVector3 normal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(
                    &sphere->m_center, reinterpret_cast<const Vec3f *>(&closest))));
                result->SetPosition(fn_8040CDD0(reinterpret_cast<const Vec3f *>(&closest),
                                                &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&normal), radius)));
                result->SetNormal(*reinterpret_cast<const Vec3f *>(&normal));
                result->m_distance = fn_8000FEE4(distanceSq) - radius;
                result->field_0x10 = fn_8000FEE4(distanceSq) - radius - sphere->m_radius;
                result->field_0x2C = 0;
            }
            return true;
        }
        return false;
    }
}

/* A sphere moving by move against the capsule; move is cut back to the contact. */
bool SweepCapsule(const Line *segment, const UnkSphere_8004E6CC *sphere, Vec3f *move, UnkColliContact_8001FF00 *result,
                  float radius, float halfHeight) {
    if (halfHeight < 0.0001f) {
        return SweepSphere(&fn_8040CDA8(&fn_8040CDD0(&segment->m_start, &segment->m_end), 0.5f), sphere, move, result,
                           radius);
    }
    if (TestCapsule(segment, sphere, result, radius, halfHeight)) {
        fn_8040CE60(move);
        return true;
    }
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, &segment->m_start));
    bool inside = false;
    Fn8040CopyVector3 axis(((VectorSubtractFn)fn_8040CD74)(&segment->m_end, &segment->m_start));
    Fn8040CopyVector3 normal(fn_8040CFB8(move, reinterpret_cast<const Vec3f *>(&axis)));
    float along = fn_8040CF90(reinterpret_cast<const Vec3f *>(&normal), reinterpret_cast<const Vec3f *>(&offset));
    float normalSq = fn_8040CF2C(reinterpret_cast<const Vec3f *>(&normal));
    float reach = sphere->m_radius + radius;
    if (normalSq < 0.0001f) {
        float t = fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&offset)) /
                  (4.0f * (halfHeight * halfHeight));
        Fn8040CopyVector3 onAxis(fn_8040CDA8(reinterpret_cast<const Vec3f *>(&axis), t));
        Fn8040CopyVector3 away(((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&offset),
                                                               reinterpret_cast<const Vec3f *>(&onAxis)));
        if (fn_8040CF2C(reinterpret_cast<const Vec3f *>(&away)) > reach * reach) {
            return false;
        }
        if (0.0f <= t && t <= 1.0f) {
            return false;
        }
        if (t < 0.0f) {
            return SweepSphere(&segment->m_start, sphere, move, result, radius);
        }
        if (t > 0.0f) {
            return SweepSphere(&segment->m_end, sphere, move, result, radius);
        }
    }
    float scale = along / normalSq;
    float distanceSq = along * scale;
    if (distanceSq > reach * reach) {
        return false;
    }
    Fn8040CopyVector3 plane(((VectorSubtractFn)fn_8040CD74)(
        reinterpret_cast<const Vec3f *>(&offset), &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&normal), scale)));
    if (distanceSq == reach * reach) {
        Vec3f hit;
        if (fn_800259C0(reinterpret_cast<const Vec3f *>(&plane), move, reinterpret_cast<const Vec3f *>(&axis), &hit)) {
            AssignVector(*move, ((VectorSubtractFn)fn_8040CD74)(&hit, reinterpret_cast<const Vec3f *>(&plane)));
            if (result != 0) {
                result->SetNormal(fn_8040CEE8(reinterpret_cast<const Vec3f *>(&normal)));
                result->SetPosition(fn_8040CDD0(&fn_8040CDD0(&hit, &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&result->m_normal), radius)),
                                                &segment->m_start));
                result->m_distance = sphere->m_radius;
                result->field_0x10 = 0.0f;
                result->field_0x34 = 0;
                result->field_0x2C = 0;
            }
            return true;
        }
        return false;
    }
    float rest = reach * reach - distanceSq;
    Fn8040CopyVector3 onAxis(fn_8040CDA8(reinterpret_cast<const Vec3f *>(&axis),
                                         fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis),
                                                     reinterpret_cast<const Vec3f *>(&plane)) /
                                             (4.0f * (halfHeight * halfHeight))));
    Fn8040CopyVector3 side(((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&onAxis),
                                                           reinterpret_cast<const Vec3f *>(&plane)));
    float sideSq = fn_8040CF2C(reinterpret_cast<const Vec3f *>(&side));
    if (rest >= sideSq) {
        inside = true;
    }
    fn_8040CE38(reinterpret_cast<Vec3f *>(&side), fn_8000FEE4(rest / sideSq));
    Fn8040CopyVector3 from(fn_8040CDD0(reinterpret_cast<const Vec3f *>(&plane), reinterpret_cast<const Vec3f *>(&side)));
    if (!inside) {
        Vec3f hit;
        if (fn_800259C0(reinterpret_cast<const Vec3f *>(&from), move, reinterpret_cast<const Vec3f *>(&axis), &hit)) {
            AssignVector(*move, ((VectorSubtractFn)fn_8040CD74)(&hit, reinterpret_cast<const Vec3f *>(&from)));
            if (result != 0) {
                result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(
                    &fn_8040CDD0(reinterpret_cast<const Vec3f *>(&offset), move), &hit)));
                result->SetPosition(fn_8040CDD0(&fn_8040CDD0(&hit, &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&result->m_normal), radius)),
                                                &segment->m_start));
                result->m_distance = sphere->m_radius;
                result->field_0x10 = 0.0f;
                result->field_0x34 = 0;
                result->field_0x2C = 0;
            }
            return true;
        }
    }
    bool startHit = SweepSphere(&segment->m_start, sphere, move, result, radius);
    bool endHit = SweepSphere(&segment->m_end, sphere, move, result, radius);
    return startHit || endHit;
}

void RangeCapsule::Reset() {}

void RangeCapsule::SetBaseRotation(const Vec3f *rotation) {
    m_capsule.m_rotation.x = rotation->x;
    m_capsule.m_rotation.y = rotation->y;
    m_capsule.m_rotation.z = rotation->z;
    UpdateRotationFlags();
    if (m_owner.lock()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), owner->vf_0x18(), owner->vf_0x24());
    }
}

void RangeCapsule::SetOffset(const Vec3f *offset) {
    m_capsule.m_center = fn_8001DFE8(offset);
    UpdateRotationFlags();
    if (m_owner.lock()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), owner->vf_0x18(), owner->vf_0x24());
    }
}

s32 RangeCapsule::GetRotationOrder() {
    return m_capsule.field_0x20;
}

const Vec3f *RangeCapsule::GetBaseRotation() {
    return &m_capsule.m_rotation;
}

Fn8040CopyVector3 RangeCapsule::GetExtent() {
    return ((VectorCopyFn)fn_800169F8)(&m_capsule.m_center);
}

Vec3f RangeCapsule::GetRotation() {
    if (!m_owner.expired()) {
        boost::shared_ptr<CharaColliBase> owner(m_owner);
        owner->vf_0x7C();
        return fn_80029418(&m_matrix, owner->vf_0x24());
    }
    return fn_80029418(&m_matrix, m_capsule.field_0x20);
}

/* Not called anywhere: the linker strips it from the game. The original pools 0.0f ahead of the
 * other float literals of the unit, which compiling a function that uses it here (emitted first)
 * reproduces (inferred from the order of the literals). */
float RangeCapsuleZero() {
    return 0.0f;
}

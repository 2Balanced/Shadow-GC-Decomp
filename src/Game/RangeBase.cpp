/* SonicteamUSA::System::Range::RangeBase (class name from RTTI): the base class of the collision
 * shapes (Game/RangeBase.h). The unit holds the base's constructor, destructor and default
 * virtuals, the calls the shapes make on their owner collider, two tests the shapes share
 * (fn_80028F64, fn_80029118), and CharaColliBase's default vf_0x24. Member names and meanings are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#define RANGE_UNIT
#include "Game/RangeBase.h"
#include "Game/CharaColli.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using SonicteamUSA::System::CharaColli::CharaColliBase;
using Sonicteam::System::Geometry::Line;
using SonicteamUSA::System::Range::RangeBase;

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

extern "C" {
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* a . b */
float fn_8040D180(const Vec3f *a, const Vec3f *b); /* |a - b|^2 */
}

/* The rotation flags of field_0x18 (bit 0: the shape has an extent). */
const u8 kRotatedX = 2;
const u8 kRotatedY = 4;
const u8 kRotatedZ = 8;

RangeBase::RangeBase(s32 priority, const Vec3f &position, u32 attributes, u32 id,
                     boost::weak_ptr<CharaColliBase> owner)
    : m_position(position), m_priority(priority), m_enabled(1), m_attributes(attributes), m_id(id),
      m_owner(owner) {}

RangeBase::~RangeBase() {}

void RangeBase::UpdateRotationFlags() {
    const Fn8040CopyVector3 &extent = GetExtent();
    if (extent.x == 0.0f && extent.y == 0.0f && extent.z == 0.0f) {
        field_0x18 = 0;
    } else {
        field_0x18 = 1;
    }
    const Vec3f *rotation = GetBaseRotation();
    if (rotation->x != 0.0f) {
        field_0x18 |= kRotatedX;
    }
    if (rotation->y != 0.0f) {
        field_0x18 |= kRotatedY;
    }
    if (rotation->z != 0.0f) {
        field_0x18 |= kRotatedZ;
    }
}

extern "C" {

/* Whether the bounding spheres of a and b touch. */
bool fn_80029118(RangeBase *a, RangeBase *b) {
    float radius = 0.001f + (a->m_margin + b->m_margin);
    return fn_8040D180(reinterpret_cast<const Vec3f *>(&a->m_position),
                       reinterpret_cast<const Vec3f *>(&b->m_position)) <= radius * radius;
}

/* Whether point lies within radius of the vertical segment (tested in XZ first); the closest
 * point and the squared distance are returned through closest and distance when given. */
bool fn_80028F64(const Vec3f *point, const Line *segment, float radius, Vec3f *closest, float *distance) {
    float radiusSq = radius * radius;
    float dx = point->x - segment->m_start.x;
    float dz = point->z - segment->m_start.z;
    float dxSq = dx * dx;
    float dzSq = dz * dz;
    float flat = dxSq + dzSq;
    if (flat > radiusSq) {
        return false;
    }
    Fn8040CopyVector3 direction(((VectorSubtractFn)fn_8040CD74)(&segment->m_end, &segment->m_start));
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(point, &segment->m_start));
    float t = fn_8040CF90(reinterpret_cast<const Vec3f *>(&offset), reinterpret_cast<const Vec3f *>(&direction));
    if (t > 0.0f) {
        if (t >= fn_8040CF90(reinterpret_cast<const Vec3f *>(&direction), reinterpret_cast<const Vec3f *>(&direction))) {
            if (closest != 0) {
                fn_800091D0(closest, &segment->m_end);
            }
            float d = fn_8040D180(point, &segment->m_end);
            if (distance != 0) {
                *distance = d;
            }
            return d <= radiusSq;
        }
        if (closest != 0) {
            closest->x = segment->m_start.x;
            closest->y = point->y;
            closest->z = segment->m_start.z;
        }
        if (distance != 0) {
            *distance = flat;
        }
        return true;
    }
    if (closest != 0) {
        fn_800091D0(closest, &segment->m_start);
    }
    float d = fn_8040CF2C(reinterpret_cast<const Vec3f *>(&offset));
    if (distance != 0) {
        *distance = d;
    }
    return d <= radiusSq;
}

} // extern "C"

s32 CharaColliBase::vf_0x24() {
    return 0;
}

extern "C" {

/* The owner's vf_0x24 (0 without an owner). */
s32 fn_80028ECC(RangeBase *shape) {
    boost::shared_ptr<CharaColliBase> owner = shape->m_owner.lock();
    if (owner) {
        return owner->vf_0x24();
    }
    return 0;
}

/* Calls the owner's vf_0x7C (when there is one). */
void fn_80028CA4(RangeBase *shape) {
    boost::shared_ptr<CharaColliBase> owner = shape->m_owner.lock();
    if (owner) {
        owner->vf_0x7C();
        return;
    }
}

} // extern "C"

void RangeBase::DebugDraw() {}

void RangeBase::Reset() {}

s32 RangeBase::GetRotationOrder() {
    return 0;
}

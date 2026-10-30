/* SonicteamUSA::System::CharaColli::CharaColliBody (class name from RTTI): a collider of one shape
 * (UnkColliObject_80028CA4, RTTI's Range::RangeBase) with a position and rotation of its own. A
 * move only marks the collider (m_moved); slot 0x8C hands it to the shape. The queries (slots
 * 0x60 to 0x6C) first ask an optional filter, then reject by the collider's bounding sphere before
 * they ask the shape. Member and function names are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#include "Game/CharaColliBody.h"
#include "Game/DebugDraw.h"
#include "Game/MathTypes.h"
#include "Game/UnkColliObject_80028CA4.h"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliBody;

/* A line segment: start and end (Game/DebugDisp.h's UnkDebugLine_80020A20; fn_80020A20 builds
 * one in place, so only its size is given here). */
struct UnkSegment_80020A20 {
    /* 0x00 */ u8 field_0x0[0x18];
};

extern "C" {
/* Makes the shape the description asks for (behaviour inferred). */
UnkColliObject_80028CA4 *fn_80037668(CharaColliBase *owner, const UnkColliShapeDesc_80037668 *shape,
                                     const Vec3f *position, const Vec3f *rotation, s32 arg);
/* The squared distance from point to the segment (t: where along it; may be 0). */
float fn_8003C660(const Vec3f *point, const UnkDebugLine_80020A20 *segment, float *t);
float fn_8040D180(const Vec3f *a, const Vec3f *b); /* |a - b|^2 */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b); /* a + b */
void fn_80020A20(UnkSegment_80020A20 *segment, const Vec3f *start, const Vec3f *end);
}

/* Whether a collider's shape takes part in contacts (meaning inferred). */
inline bool IsSolid(CharaColliBase *collider, s32 priority) {
    return collider->field_0x2C && priority == 0;
}

CharaColliBody::CharaColliBody(const UnkColliShapeDesc_80037668 *shape, const Vec3f *position,
                               const Vec3f *rotation, s32 arg, u16 type, u16 typeArg,
                               boost::weak_ptr<CharInfo> info)
    : CharaColliBaseCharInfo(type, typeArg, info) {
    fn_800091EC(&m_position, position);
    m_rotation.x = rotation->x;
    m_rotation.y = rotation->y;
    m_rotation.z = rotation->z;
    field_0x58 = arg;
    m_shape = fn_80037668(this, shape, position, rotation, 0);
    vf_0x7C();
    vf_0x84();
}

CharaColliBody::~CharaColliBody() {
    delete m_shape;
}

void CharaColliBody::vf_0x54(const boost::weak_ptr<CharaColliBase> &owner) {
    m_shape->SetOwner(owner);
}

void CharaColliBody::SetScale(const Vec3f *scale) {
    vf_0x28(0)->SetScale(scale);
    vf_0x84();
    m_changed = 1;
}

void CharaColliBody::vf_0x7C() {
    vf_0x88();
    vf_0x8C();
}

void CharaColliBody::vf_0x8C() {
    if (m_moved) {
        vf_0x28(0)->Update(&m_position, &m_rotation, field_0x58);
        m_changed = 1;
        m_moved = 0;
    }
}

UnkColliObject_80028CA4 *CharaColliBody::vf_0x60(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result,
                                                 Filter filter, u32 arg) {
    vf_0x88();
    UnkColliObject_80028CA4 *shape = vf_0x28(0);
    float radius = m_radius;
    if (filter != 0) {
        UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
        if (!filter(&query)) {
            return 0;
        }
    }
    if (fn_8003C660(vf_0x14(), line, 0) > radius * radius) {
        return 0;
    }
    vf_0x8C();
    if (shape->vf_0x30(line, result)) {
        return vf_0x28(0);
    }
    return 0;
}

UnkColliObject_80028CA4 *CharaColliBody::vf_0x64(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result,
                                                 Filter filter, u32 arg) {
    vf_0x88();
    UnkColliObject_80028CA4 *shape = vf_0x28(0);
    float radius = m_radius;
    if (filter != 0) {
        UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
        if (!filter(&query)) {
            return 0;
        }
    }
    if (fn_8003C660(vf_0x14(), segment, 0) > radius * radius) {
        return 0;
    }
    vf_0x8C();
    if (shape->vf_0x34(segment, result)) {
        return vf_0x28(0);
    }
    return 0;
}

UnkColliObject_80028CA4 *CharaColliBody::vf_0x68(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result,
                                                 Filter filter, u32 arg) {
    vf_0x88();
    UnkColliObject_80028CA4 *shape = vf_0x28(0);
    float radius = m_radius + sphere->m_radius;
    if (filter != 0) {
        UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
        if (!filter(&query)) {
            return 0;
        }
    }
    if (fn_8040D180(vf_0x14(), &sphere->m_center) > radius * radius) {
        return 0;
    }
    vf_0x8C();
    if (shape->vf_0x38(sphere, result)) {
        return vf_0x28(0);
    }
    return 0;
}

UnkColliObject_80028CA4 *CharaColliBody::vf_0x6C(const UnkSphere_8004E6CC *sphere, const Vec3f *move,
                                                 UnkColliContact_8001FF00 *result, Filter filter, u32 arg) {
    vf_0x88();
    UnkColliObject_80028CA4 *shape = vf_0x28(0);
    float radius = m_radius + sphere->m_radius;
    if (filter != 0) {
        UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
        if (!filter(&query)) {
            return 0;
        }
    }
    UnkSegment_80020A20 path;
    fn_80020A20(&path, &sphere->m_center, &(const Vec3f &)fn_8040CDD0(&sphere->m_center, move));
    if (fn_8003C660(vf_0x14(), reinterpret_cast<UnkDebugLine_80020A20 *>(&path), 0) >
        (radius + sphere->m_radius) * (radius + sphere->m_radius)) {
        return 0;
    }
    vf_0x8C();
    if (shape->vf_0x3C(sphere, move, result)) {
        return vf_0x28(0);
    }
    return 0;
}

bool CharaColliBody::vf_0x80(CharaColliBase *other, UnkColliObject_80028CA4 **mine,
                             UnkColliObject_80028CA4 **theirs, UnkColliHit_8002686C **mineHit,
                             UnkColliHit_8002686C **theirHit) {
    UnkColliObject_80028CA4 *shape = vf_0x28(0);
    UnkColliObject_80028CA4 *own = m_shape;
    for (int i = 0; i < other->GetShapeCount(); i++) {
        UnkColliObject_80028CA4 *otherShape = other->vf_0x28(i);
        UnkColliObject_80028CA4 *first;
        UnkColliObject_80028CA4 *second;
        bool firstSolid;
        bool secondSolid;
        s32 priority = shape->m_priority;
        s32 otherPriority = otherShape->m_priority;
        if (priority >= otherPriority) {
            first = shape;
            second = otherShape;
            firstSolid = IsSolid(this, priority);
            secondSolid = IsSolid(other, otherPriority);
        } else {
            first = otherShape;
            second = shape;
            firstSolid = IsSolid(other, otherPriority);
            secondSolid = IsSolid(this, priority);
        }
        if (firstSolid || secondSolid) {
            UnkColliContact_8001FF00 firstContact;
            UnkColliContact_8001FF00 secondContact;
            fn_8001FF00(&firstContact);
            fn_8001FF00(&secondContact);
            UnkColliContact_8001FF00 *firstOut = 0;
            UnkColliContact_8001FF00 *secondOut = 0;
            if (firstSolid) {
                firstOut = &firstContact;
            }
            if (secondSolid) {
                secondOut = &secondContact;
            }
            if (first->vf_0x4C(second, firstOut, secondOut)) {
                *mine = own;
                *theirs = otherShape;
                if (firstSolid) {
                    if (shape->m_priority >= otherShape->m_priority) {
                        UnkColliHit_8002686C *hit = (UnkColliHit_8002686C *)fn_8001FC88(0x48);
                        if (hit != 0) {
                            hit = fn_8002686C(hit, otherShape, &firstContact);
                        }
                        *mineHit = hit;
                    } else {
                        UnkColliHit_8002686C *hit = (UnkColliHit_8002686C *)fn_8001FC88(0x48);
                        if (hit != 0) {
                            hit = fn_8002686C(hit, own, &firstContact);
                        }
                        *theirHit = hit;
                    }
                }
                if (secondSolid) {
                    if (shape->m_priority >= otherShape->m_priority) {
                        UnkColliHit_8002686C *hit = (UnkColliHit_8002686C *)fn_8001FC88(0x48);
                        if (hit != 0) {
                            hit = fn_8002686C(hit, own, &secondContact);
                        }
                        *theirHit = hit;
                    } else {
                        UnkColliHit_8002686C *hit = (UnkColliHit_8002686C *)fn_8001FC88(0x48);
                        if (hit != 0) {
                            hit = fn_8002686C(hit, otherShape, &secondContact);
                        }
                        *mineHit = hit;
                    }
                }
                return true;
            }
        } else if (first->vf_0x48(second)) {
            *mine = own;
            *theirs = otherShape;
            if (mineHit != 0) {
                *mineHit = 0;
            }
            if (theirHit != 0) {
                *theirHit = 0;
            }
            return true;
        }
    }
    return false;
}

void CharaColliBody::SetDebugColor(const UnkColor_8040D2D8 *color) {
    vf_0x28(0)->SetDebugColor(color);
}

void CharaColliBody::Reset() {
    vf_0x7C();
    vf_0x28(0)->Reset();
}

void CharaColliBody::vf_0x74() {
    vf_0x7C();
    vf_0x28(0)->DebugDraw();
    float radius = m_radius;
    if (radius != m_shape->m_margin) {
        fn_802E0E84(vf_0x14(), &lbl_805F06D8, radius);
    }
}

void CharaColliBody::vf_0x84() {
    m_radius = fn_8040CF50(&(const Vec3f &)m_shape->GetExtent()) + m_shape->GetMargin();
}

void CharaColliBody::vf_0x88() {}

void CharaColliBody::vf_0x5C(u32 attributes, u32 id) {
    UnkColliObject_80028CA4 *shape = m_shape;
    if (id == shape->m_id) {
        shape->m_attributes = attributes;
    }
}

void CharaColliBody::SetAttributes(u32 attributes, int arg1) {
    m_shape->m_attributes = attributes;
}

void CharaColliBody::SetRotation(const Vec3f *rotation) {
    m_rotation.x = rotation->x;
    m_rotation.y = rotation->y;
    m_rotation.z = rotation->z;
    m_moved = 1;
}

void CharaColliBody::SetCharInfo(CharInfo *info) {
    fn_800091D0(&m_position, reinterpret_cast<const Vec3f *>(info));
    m_moved = 1;
}

u32 CharaColliBody::vf_0x34(u32 id) const {
    UnkColliObject_80028CA4 *shape = m_shape;
    if (id == shape->m_id) {
        return shape->m_attributes;
    }
    return 0;
}

u32 CharaColliBody::GetAttributes(int arg) const {
    return m_shape->m_attributes;
}

UnkColliObject_80028CA4 *CharaColliBody::vf_0x2C(u32 id) const {
    UnkColliObject_80028CA4 *shape = m_shape;
    if (id == shape->m_id) {
        return shape;
    }
    return 0;
}

UnkColliObject_80028CA4 *CharaColliBody::vf_0x28(int index) const {
    return m_shape;
}

s32 CharaColliBody::vf_0x24() {
    return field_0x58;
}

Vec3f *CharaColliBody::vf_0x18() {
    return &m_rotation;
}

Vec3f *CharaColliBody::vf_0x14() {
    return &m_position;
}

u16 CharaColliBody::GetShapeCount() {
    return 1;
}

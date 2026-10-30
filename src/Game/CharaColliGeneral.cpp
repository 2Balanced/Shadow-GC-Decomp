/* SonicteamUSA::System::CharaColli::CharaColliGeneral (class name from RTTI): a collider of several
 * shapes (UnkColliObject_80028CA4, RTTI's Range::RangeBase) kept in a list of pointers, sharing the
 * collider's position and rotation. Shapes can be turned on and off one by one (byte 0x19 of a
 * shape). A query without a result returns the first shape hit; with a result it returns the
 * nearest one and leaves that one's contact in the result. Member and function names are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Game/CharaColliGeneral.h"
#include "Game/DebugDraw.h"
#include "Game/MathTypes.h"
#include "Game/UnkColliObject_80028CA4.h"
#include "boost/weak_ptr.hpp"

using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::CharaColli::CharaColliGeneral;
using SonicteamUSA::System::CharaColli::ColliShapeList;

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

CharaColliGeneral::CharaColliGeneral(const UnkColliShapeDesc_80037668 *shapes, const Vec3f *position,
                                     const Vec3f *rotation, s32 arg, u16 type, u16 typeArg,
                                     boost::weak_ptr<CharInfo> info)
    : CharaColliBaseCharInfo(type, typeArg, info), m_position(*position),
      m_rotation(rotation->x, rotation->y, rotation->z), field_0x58(arg), m_count(0) {
    while (shapes->field_0x4 != 0) {
        m_shapes.push_back(fn_80037668(this, shapes, position, rotation, arg));
        shapes++;
        m_count++;
    }
    vf_0x7C();
    vf_0x84();
}

CharaColliGeneral::~CharaColliGeneral() {
    ColliShapeList::iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        delete *it;
    }
}

void CharaColliGeneral::vf_0x54(const boost::weak_ptr<CharaColliBase> &owner) {
    ColliShapeList::iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        (*it)->SetOwner(owner);
    }
}

void CharaColliGeneral::SetAttributes(u32 attributes, int arg1) {
    ColliShapeList::iterator it;
    if (arg1 == -1) {
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            (*it)->m_attributes = attributes;
        }
    } else {
        UnkColliObject_80028CA4 *shape = vf_0x28(arg1);
        if (shape != 0) {
            shape->m_attributes = attributes;
        }
    }
}

void CharaColliGeneral::vf_0x5C(u32 attributes, u32 id) {
    UnkColliObject_80028CA4 *shape = vf_0x2C(id);
    if (shape != 0) {
        shape->m_attributes = attributes;
    }
}

void CharaColliGeneral::TurnOn(u32 arg) {
    if (arg == -1) {
        if (!field_0x2A) {
            field_0x2B = 0;
            field_0x2A = 1;
            m_moved = 1;
        }
    } else {
        UnkColliObject_80028CA4 *shape = vf_0x2C(arg);
        if (shape != 0 && arg == shape->m_id && !shape->m_enabled) {
            field_0x2B = 0;
            shape->m_enabled = 1;
            vf_0x84();
        }
    }
}

void CharaColliGeneral::TurnOff(u32 arg) {
    if (arg == -1) {
        field_0x2B = field_0x2A;
        field_0x2A = 0;
    } else {
        UnkColliObject_80028CA4 *shape = vf_0x2C(arg);
        if (shape != 0 && arg == shape->m_id && shape->m_enabled) {
            field_0x2B = 0;
            shape->m_enabled = 0;
            vf_0x84();
        }
    }
}

void CharaColliGeneral::SetScale(const Vec3f *scale) {
    ColliShapeList::iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        (*it)->SetScale(scale);
    }
    vf_0x84();
    m_changed = 1;
}

void CharaColliGeneral::vf_0x7C() {
    vf_0x88();
    vf_0x8C();
}

void CharaColliGeneral::vf_0x8C() {
    if (m_moved) {
        ColliShapeList::iterator it;
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            (*it)->Update(&m_position, &m_rotation, field_0x58);
        }
        m_changed = 1;
        m_moved = 0;
    }
}

UnkColliObject_80028CA4 *CharaColliGeneral::vf_0x60(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result,
                                                    Filter filter, u32 arg) {
    vf_0x88();
    if (fn_8003C660(vf_0x14(), line, 0) > m_radius * m_radius) {
        return 0;
    }
    vf_0x8C();
    if (result == 0) {
        ColliShapeList::iterator it;
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            UnkColliObject_80028CA4 *shape = *it;
            if (filter != 0) {
                UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
                if (!filter(&query)) {
                    continue;
                }
            }
            if (shape->vf_0x30(line, result)) {
                return shape;
            }
        }
        return 0;
    } else {
        bool restore = false;
        UnkColliObject_80028CA4 *nearest = 0;
        UnkColliContact_8001FF00 best;
        fn_8001FF00(&best);
        ColliShapeList::iterator it;
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            UnkColliObject_80028CA4 *shape = *it;
            if (filter != 0) {
                UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
                if (!filter(&query)) {
                    continue;
                }
            }
            if (shape->vf_0x30(line, result)) {
                if (nearest != 0 && result->m_distance > best.m_distance) {
                    restore = true;
                }
                if (nearest == 0 || result->m_distance <= best.m_distance) {
                    nearest = shape;
                    best = *result;
                }
            }
        }
        if (restore) {
            *result = best;
        }
        return nearest;
    }
}

UnkColliObject_80028CA4 *CharaColliGeneral::vf_0x64(const UnkDebugLine_80020A20 *segment,
                                                    UnkColliContact_8001FF00 *result, Filter filter, u32 arg) {
    vf_0x88();
    if (fn_8003C660(vf_0x14(), segment, 0) > m_radius * m_radius) {
        return 0;
    }
    vf_0x8C();
    if (result == 0) {
        ColliShapeList::iterator it;
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            UnkColliObject_80028CA4 *shape = *it;
            if (filter != 0) {
                UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
                if (!filter(&query)) {
                    continue;
                }
            }
            if (shape->vf_0x34(segment, result)) {
                return shape;
            }
        }
        return 0;
    } else {
        bool restore = false;
        UnkColliObject_80028CA4 *nearest = 0;
        UnkColliContact_8001FF00 best;
        fn_8001FF00(&best);
        ColliShapeList::iterator it;
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            UnkColliObject_80028CA4 *shape = *it;
            if (filter != 0) {
                UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
                if (!filter(&query)) {
                    continue;
                }
            }
            if (shape->vf_0x34(segment, result)) {
                if (nearest != 0 && result->m_distance > best.m_distance) {
                    restore = true;
                }
                if (nearest == 0 || result->m_distance <= best.m_distance) {
                    nearest = shape;
                    best = *result;
                }
            }
        }
        if (restore) {
            *result = best;
        }
        return nearest;
    }
}

UnkColliObject_80028CA4 *CharaColliGeneral::vf_0x68(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result,
                                                    Filter filter, u32 arg) {
    vf_0x88();
    float radius = m_radius + sphere->m_radius;
    if (fn_8040D180(vf_0x14(), &sphere->m_center) > radius * radius) {
        return 0;
    }
    vf_0x8C();
    if (result == 0) {
        ColliShapeList::iterator it;
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            UnkColliObject_80028CA4 *shape = *it;
            if (filter != 0) {
                UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
                if (!filter(&query)) {
                    continue;
                }
            }
            if (shape->vf_0x38(sphere, result)) {
                return shape;
            }
        }
        return 0;
    } else {
        bool restore = false;
        UnkColliObject_80028CA4 *nearest = 0;
        UnkColliContact_8001FF00 best;
        fn_8001FF00(&best);
        ColliShapeList::iterator it;
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            UnkColliObject_80028CA4 *shape = *it;
            if (filter != 0) {
                UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
                if (!filter(&query)) {
                    continue;
                }
            }
            if (shape->vf_0x38(sphere, result)) {
                if (nearest != 0 && result->m_distance > best.m_distance) {
                    restore = true;
                }
                if (nearest == 0 || result->m_distance <= best.m_distance) {
                    nearest = shape;
                    best = *result;
                }
            }
        }
        if (restore) {
            *result = best;
        }
        return nearest;
    }
}

UnkColliObject_80028CA4 *CharaColliGeneral::vf_0x6C(const UnkSphere_8004E6CC *sphere, const Vec3f *move,
                                                    UnkColliContact_8001FF00 *result, Filter filter, u32 arg) {
    vf_0x88();
    float radius = m_radius + sphere->m_radius;
    UnkSegment_80020A20 path;
    fn_80020A20(&path, &sphere->m_center, &(const Vec3f &)fn_8040CDD0(&sphere->m_center, move));
    if (fn_8003C660(vf_0x14(), reinterpret_cast<UnkDebugLine_80020A20 *>(&path), 0) >
        (radius + sphere->m_radius) * (radius + sphere->m_radius)) {
        return 0;
    }
    vf_0x8C();
    if (result == 0) {
        ColliShapeList::iterator it;
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            UnkColliObject_80028CA4 *shape = *it;
            if (filter != 0) {
                UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
                if (!filter(&query)) {
                    continue;
                }
            }
            if (shape->vf_0x3C(sphere, move, result)) {
                return shape;
            }
        }
        return 0;
    } else {
        bool restore = false;
        UnkColliObject_80028CA4 *nearest = 0;
        UnkColliContact_8001FF00 best;
        fn_8001FF00(&best);
        ColliShapeList::iterator it;
        for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
            UnkColliObject_80028CA4 *shape = *it;
            if (filter != 0) {
                UnkColliQuery_800C0D20 query = {0, shape, this, shape->m_attributes, arg};
                if (!filter(&query)) {
                    continue;
                }
            }
            if (shape->vf_0x3C(sphere, move, result)) {
                if (nearest != 0 && result->m_distance > best.m_distance) {
                    restore = true;
                }
                if (nearest == 0 || result->m_distance <= best.m_distance) {
                    nearest = shape;
                    best = *result;
                }
            }
        }
        if (restore) {
            *result = best;
        }
        return nearest;
    }
}

bool CharaColliGeneral::vf_0x80(CharaColliBase *other, UnkColliObject_80028CA4 **mine,
                                UnkColliObject_80028CA4 **theirs, UnkColliHit_8002686C **mineHit,
                                UnkColliHit_8002686C **theirHit) {
    for (int i = 0; i < GetShapeCount(); i++) {
        UnkColliObject_80028CA4 *shape = vf_0x28(i);
        if (!shape->m_enabled) {
            continue;
        }
        for (int j = 0; j < other->GetShapeCount(); j++) {
            UnkColliObject_80028CA4 *otherShape = other->vf_0x28(j);
            if (!otherShape->m_enabled) {
                continue;
            }
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
                    *mine = shape;
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
                                hit = fn_8002686C(hit, shape, &firstContact);
                            }
                            *theirHit = hit;
                        }
                    }
                    if (secondSolid) {
                        if (shape->m_priority >= otherShape->m_priority) {
                            UnkColliHit_8002686C *hit = (UnkColliHit_8002686C *)fn_8001FC88(0x48);
                            if (hit != 0) {
                                hit = fn_8002686C(hit, shape, &secondContact);
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
                *mine = shape;
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
    }
    return false;
}

UnkColliObject_80028CA4 *CharaColliGeneral::vf_0x28(int index) const {
    ColliShapeList::const_iterator it;
    u32 i = 0;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        if (i == index) {
            return *it;
        }
        i++;
    }
    return 0;
}

UnkColliObject_80028CA4 *CharaColliGeneral::vf_0x2C(u32 id) const {
    ColliShapeList::const_iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        if (id == (*it)->m_id) {
            return *it;
        }
    }
    return 0;
}

u32 CharaColliGeneral::GetAttributes(int arg) const {
    ColliShapeList::const_iterator it;
    u32 i = 0;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        if (i == arg) {
            return (*it)->m_attributes;
        }
        i++;
    }
    return 0;
}

u32 CharaColliGeneral::vf_0x34(u32 id) const {
    ColliShapeList::const_iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        if (id == (*it)->m_id) {
            return (*it)->m_attributes;
        }
    }
    return 0;
}

void CharaColliGeneral::SetDebugColor(const UnkColor_8040D2D8 *color) {
    ColliShapeList::iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        (*it)->SetDebugColor(color);
    }
}

void CharaColliGeneral::Reset() {
    vf_0x7C();
    ColliShapeList::iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        (*it)->Reset();
    }
}

void CharaColliGeneral::vf_0x74() {
    vf_0x7C();
    ColliShapeList::iterator it;
    for (it = m_shapes.begin(); it != m_shapes.end(); it++) {
        (*it)->DebugDraw();
    }
    fn_802E0E84(vf_0x14(), &lbl_805F06D8, m_radius);
}

void CharaColliGeneral::vf_0x84() {
    ColliShapeList::iterator it = m_shapes.begin();
    UnkColliObject_80028CA4 *first = *it;
    float radius = fn_8040CF50(&(const Vec3f &)first->GetExtent()) + first->GetMargin();
    for (; it != m_shapes.end(); it++) {
        UnkColliObject_80028CA4 *shape = *it;
        if (shape->m_enabled) {
            float reach = fn_8040CF50(&(const Vec3f &)shape->GetExtent()) + shape->GetMargin();
            if (radius < reach) {
                radius = reach;
            }
        }
    }
    m_radius = radius;
}

void CharaColliGeneral::vf_0x88() {}

void CharaColliGeneral::SetRotation(const Vec3f *rotation) {
    m_rotation.x = rotation->x;
    m_rotation.y = rotation->y;
    m_rotation.z = rotation->z;
    m_moved = 1;
}

void CharaColliGeneral::SetCharInfo(CharInfo *info) {
    fn_800091D0(&m_position, reinterpret_cast<const Vec3f *>(info));
    m_moved = 1;
}

s32 CharaColliGeneral::vf_0x24() {
    return field_0x58;
}

Vec3f *CharaColliGeneral::vf_0x18() {
    return &m_rotation;
}

Vec3f *CharaColliGeneral::vf_0x14() {
    return &m_position;
}

u16 CharaColliGeneral::GetShapeCount() {
    return m_count;
}

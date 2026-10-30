/* SonicteamUSA::System::Range::RangeSphere (class name and base from RTTI): a sphere (shape kind 0,
 * Game/RangeSphere.h). Its tests against lines, segments and spheres are static functions of the
 * unit; against another shape it asks that shape to test a sphere. The unit also defines the
 * collider-less owner the shapes are made with (a null shared_ptr<CharaColliBase>) and the
 * constructor of the sphere description. Member names and meanings are inferred. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#define RANGE_UNIT
#include "Game/CharaColliGeneral.h"
#include "Game/RangeSphere.h"
#include "Game/BossDef.h"
#include "Game/CharaColli.h"
#include "Game/DebugDisp.h"
#include "Game/DebugDraw.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::Range::RangeBase;
using SonicteamUSA::System::Range::RangeSphere;

namespace std {
float fabs(float x);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A matrix as a value (the game's matrix class copies through fn_8040C148). */
struct RangeMatrix {
    RangeMatrix(const Mtx34f &m) {
        fn_8040C148(&this->m, &m);
    }

    Mtx34f m;
};

typedef Mtx34f (*TranslationFn)(const Vec3f *offset);

extern "C" {
float fn_8000FEE4(float x); /* square root */
float fn_8040CF90(const Vec3f *a, const Vec3f *b); /* a . b */
float fn_8040D180(const Vec3f *a, const Vec3f *b); /* |a - b|^2 */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b); /* |a - b| */
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CEE8(const Vec3f *v); /* v normalized */
void fn_8040CE04(Vec3f *v, const Vec3f *other);
void fn_8040CE38(Vec3f *v, float scale);
void fn_8040CE60(Vec3f *v);
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_800091A4(const Vec3f *v, float scale);
Vec3f fn_8001DFE8(const Vec3f *v);
UnkSphere_8004E6CC *fn_80021C04(UnkSphere_8004E6CC *sphere, const Vec3f *center, float radius);
Vec3f fn_800169F8(const Vec3f *v);
}

typedef Fn8040CopyVector3 (*VectorCopyFn)(const Vec3f *v);

extern "C" {
bool fn_80028F64(const Vec3f *point, const UnkDebugLine_80020A20 *segment, float radius, Vec3f *closest,
                 float *distance);
float fn_8003C660(const Vec3f *point, const UnkDebugLine_80020A20 *line, Vec3f *closest);
float fn_8003C55C(const Vec3f *point, const UnkDebugLine_80020A20 *line, Vec3f *closest);
bool fn_8003B7AC(float *t0, float *t1, float a, float b, float c);
}

Vec3f operator-(const Vec3f &a, const Vec3f &b);

extern const Vec3f lbl_805E07B8;
extern const Vec3f lbl_80570C00;

/* The owner shapes are made with: no collider (name inferred). */
boost::shared_ptr<CharaColliBase> lbl_805EE0A0;

inline bool SamePoint(const Vec3f &a, const Vec3f &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

/* Three floats (the description's centre is built through one). */
struct RangeFloat3 {
    float v[3];
};

/* The sphere description as plain data (it is copied member by member). */
struct RangeFloat4 {
    RangeFloat3 center;
    float radius;
};

typedef bool (*HitFn)(RangeBase *self, RangeBase *other);

static bool HitSphere(RangeBase *self, RangeBase *other);
bool TestSphere(const Vec3f *position, const UnkSphere_8004E6CC *sphere, float radius,
                       UnkColliContact_8001FF00 *result);
bool SweepSphere(const Vec3f *position, const UnkSphere_8004E6CC *sphere, Vec3f *move,
                        UnkColliContact_8001FF00 *result, float radius);
bool TestLine(const Vec3f *position, const UnkDebugLine_80020A20 *line, float radius,
                     UnkColliContact_8001FF00 *result);
bool TestSegment(const Vec3f *position, const UnkDebugLine_80020A20 *segment, float radius,
                        UnkColliContact_8001FF00 *result);

/* The tests by the other shape's kind (sphere). */
static HitFn s_hitTable[] = {HitSphere};

UnkSphere_80035CEC::UnkSphere_80035CEC(const Vec3f &center, float radius) {
    RangeFloat3 c = {0.0f, 0.0f, 0.0f};
    c.v[0] = center.x;
    c.v[1] = center.y;
    c.v[2] = center.z;
    *reinterpret_cast<RangeFloat3 *>(&m_center) = c;
    m_radius = radius;
}

RangeSphere::RangeSphere(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position,
                         boost::weak_ptr<CharaColliBase> owner)
    : RangeBase(0, *position, desc->field_0x8, desc->field_0xC, owner) {
    const RangeFloat4 *from = reinterpret_cast<const RangeFloat4 *>(desc->field_0x4);
    reinterpret_cast<RangeFloat4 *>(&m_sphere)->center = from->center;
    m_sphere.m_radius = from->radius;
    UpdateRotationFlags();
    Update(position, &lbl_805E07B8, 0);
    UpdateBounds();
}

RangeSphere::~RangeSphere() {}

bool RangeSphere::vf_0x48(RangeBase *other) {
    return s_hitTable[other->m_priority](this, other);
}

bool RangeSphere::vf_0x4C(RangeBase *other, UnkColliContact_8001FF00 *mine, UnkColliContact_8001FF00 *theirs) {
    switch (other->m_priority) {
    case 0: {
        bool hit = false;
        if (mine != 0) {
            RangeFloat4 sphere;
            hit = other->vf_0x38(fn_80021C04(reinterpret_cast<UnkSphere_8004E6CC *>(&sphere),
                                             reinterpret_cast<const Vec3f *>(&m_position), GetRadius()),
                                 mine);
        }
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

bool RangeSphere::vf_0x30(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result) {
    return TestLine(reinterpret_cast<const Vec3f *>(&m_position), line, GetRadius(), result);
}

bool RangeSphere::vf_0x34(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result) {
    return TestSegment(reinterpret_cast<const Vec3f *>(&m_position), segment, GetRadius(), result);
}

bool TestSegment(const Vec3f *position, const UnkDebugLine_80020A20 *segment, float radius,
                        UnkColliContact_8001FF00 *result) {
    float radiusSq = radius * radius;
    Vec3f closest;
    float distance;
    bool hit = fn_80028F64(position, segment, radiusSq, &closest, &distance);
    if (hit && result != 0) {
        if (fn_8040D180(position, &segment->m_start) < radiusSq) {
            result->SetPosition(segment->m_start);
            result->m_distance = 0.0f;
            result->field_0x10 = 0.0f;
            result->SetNormal(segment->m_start.y > segment->m_end.y ? Vec3f(0.0f, 1.0f, 0.0f)
                                                                    : Vec3f(0.0f, -1.0f, 0.0f));
            result->field_0x2C = 3;
            return true;
        }
        if (distance < radiusSq) {
            float dx = position->x - segment->m_start.x;
            float dz = position->z - segment->m_start.z;
            distance = dx * dx + dz * dz;
            float height = segment->m_start.y - segment->m_end.y;
            float t = fn_8000FEE4((radiusSq - distance) / (height * height));
            closest.y -= t * (segment->m_end.y - segment->m_start.y);
            result->SetPosition(closest);
            result->m_distance = std::fabs(closest.y - segment->m_start.y);
            result->field_0x10 = result->GetDistance();
            result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&closest, position)));
            result->field_0x2C = 0;
        }
    }
    return hit;
}

bool TestLine(const Vec3f *position, const UnkDebugLine_80020A20 *line, float radius,
                     UnkColliContact_8001FF00 *result) {
    float radiusSq = radius * radius;
    Vec3f closest;
    float distance = fn_8003C660(position, line, &closest);
    bool hit = distance < radiusSq;
    if (hit && result != 0) {
        if (fn_8040D180(position, &line->m_start) < radiusSq) {
            result->SetPosition(line->m_start);
            result->m_distance = 0.0f;
            result->field_0x10 = 0.0f;
            result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&line->m_start, &line->m_end)));
            result->field_0x2C = 3;
            return true;
        }
        if (distance < radiusSq) {
            float along = fn_8003C55C(position, line, &closest);
            float t = fn_8000FEE4((radiusSq - along) / fn_8040D180(&line->m_start, &line->m_end));
            fn_8040CE04(&closest, &fn_800091A4(&(line->m_end - line->m_start), t));
            result->SetPosition(closest);
            result->m_distance = fn_8040D1B4(&closest, &line->m_start);
            result->field_0x10 = result->GetDistance();
            result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&closest, position)));
            result->field_0x2C = 0;
        }
    }
    return hit;
}

bool RangeSphere::vf_0x38(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result) {
    return TestSphere(reinterpret_cast<const Vec3f *>(&m_position), sphere, GetRadius(), result);
}

bool RangeSphere::vf_0x3C(const UnkSphere_8004E6CC *sphere, const Vec3f *move, UnkColliContact_8001FF00 *result) {
    return SweepSphere(reinterpret_cast<const Vec3f *>(&m_position), sphere, const_cast<Vec3f *>(move), result,
                       GetRadius());
}

void RangeSphere::Update(const Vec3f *position, const Vec3f *rotation, s32 arg) {
    if (!(field_0x18 & 1) || (rotation->x == 0.0f && rotation->y == 0.0f && rotation->z == 0.0f)) {
        fn_800091D0(reinterpret_cast<Vec3f *>(&m_position), &fn_8040CDD0(position, reinterpret_cast<const Vec3f *>(&GetExtent())));
    } else {
        RangeMatrix matrix(((TranslationFn)fn_8040B034)(position));
        fn_8001E070(&matrix.m, rotation, arg);
        fn_800091D0(reinterpret_cast<Vec3f *>(&m_position), &fn_8040B67C(&matrix.m, reinterpret_cast<const Vec3f *>(&GetExtent())));
    }
}

void RangeSphere::SetMatrix(Mtx34f *matrix) {
    fn_800091D0(reinterpret_cast<Vec3f *>(&m_position), &fn_8040B67C(matrix, reinterpret_cast<const Vec3f *>(&GetExtent())));
}

void RangeSphere::UpdateBounds() {
    m_margin = GetRadius();
}

void RangeSphere::SetScale(const Vec3f *scale) {
    Fn8040CopyVector3 offset(GetExtent());
    offset.x = offset.x * scale->x;
    offset.y = offset.y * scale->y;
    offset.z = offset.z * scale->z;
    SetOffset(reinterpret_cast<const Vec3f *>(&offset));
    if (m_owner.lock()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), &lbl_805E07B8, 0);
    }
    SetSize(scale->x * GetRadius(), 0.0f, 0.0f);
}

void RangeSphere::GetSize(float *x, float *y, float *z) {
    *x = GetRadius();
}

void RangeSphere::SetSize(float x, float y, float z) {
    SetRadius(x);
    UpdateBounds();
}

static bool HitSphere(RangeBase *self, RangeBase *other) {
    RangeSphere *a = static_cast<RangeSphere *>(self);
    RangeSphere *b = static_cast<RangeSphere *>(other);
    float radius = 0.001f + (a->GetRadius() + b->GetRadius());
    return fn_8040D180(reinterpret_cast<const Vec3f *>(&a->m_position),
                       reinterpret_cast<const Vec3f *>(&b->m_position)) < radius * radius;
}

bool TestSphere(const Vec3f *position, const UnkSphere_8004E6CC *sphere, float radius,
                       UnkColliContact_8001FF00 *result) {
    float distanceSq = fn_8040D180(position, &sphere->m_center);
    if (distanceSq < radius * radius) {
        if (result != 0) {
            result->SetPosition(sphere->m_center);
            result->m_distance = 0.0f;
            result->field_0x10 = -sphere->m_radius - (radius - fn_8000FEE4(distanceSq));
            if (SamePoint(*position, sphere->m_center)) {
                result->field_0x2C = 4;
                result->SetNormal(lbl_80570C00);
            } else {
                result->field_0x2C = 3;
                result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, position)));
            }
        }
        return true;
    }
    if (distanceSq < (radius + sphere->m_radius) * (radius + sphere->m_radius)) {
        Fn8040CopyVector3 normal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, position)));
        if (result != 0) {
            result->SetPosition(fn_8040CDD0(position, &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&normal), radius)));
            result->m_distance = fn_8000FEE4(distanceSq) - radius;
            result->field_0x10 = result->GetDistance() - sphere->m_radius;
            result->field_0x2C = 0;
            result->SetNormal(*reinterpret_cast<const Vec3f *>(&normal));
        }
        return true;
    }
    return false;
}

bool SweepSphere(const Vec3f *position, const UnkSphere_8004E6CC *sphere, Vec3f *move,
                        UnkColliContact_8001FF00 *result, float radius) {
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, position));
    float distanceSq = fn_8040CF90(reinterpret_cast<const Vec3f *>(&offset), reinterpret_cast<const Vec3f *>(&offset));
    float reach = radius + sphere->m_radius;
    if (distanceSq <= radius * radius) {
        fn_8040CE60(move);
        if (result != 0) {
            result->SetPosition(sphere->m_center);
            result->m_distance = 0.0f;
            result->field_0x10 = -sphere->m_radius - (radius - fn_8000FEE4(distanceSq));
            result->field_0x34 = 1;
            if (SamePoint(*position, sphere->m_center)) {
                result->field_0x2C = 4;
                result->SetNormal(lbl_80570C00);
            } else {
                result->field_0x2C = 3;
                result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, position)));
            }
        }
        return true;
    }
    float reachSq = reach * reach;
    if (distanceSq < 0.001f + reachSq) {
        Fn8040CopyVector3 normal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, position)));
        fn_8040CE60(move);
        if (result != 0) {
            result->SetPosition(fn_8040CDD0(position, &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&normal), radius)));
            result->m_distance = fn_8000FEE4(distanceSq) - radius;
            result->field_0x10 = result->GetDistance() - sphere->m_radius;
            result->field_0x2C = 0;
            result->field_0x34 = 1;
            result->SetNormal(*reinterpret_cast<const Vec3f *>(&normal));
        }
        return true;
    }
    int hits = 0;
    float along = fn_8040CF90(reinterpret_cast<const Vec3f *>(&offset), move);
    float moveSq = fn_8040CF90(move, move);
    if (along >= 0.0f) {
        return false;
    }
    float t0;
    float t1;
    if (fn_8003B7AC(&t0, &t1, moveSq, 2.0f * along, distanceSq - reachSq)) {
        if (0.0f <= t0 && t0 <= 1.0f) {
            hits = 1;
        }
        if (0.0f <= t1 && t1 <= 1.0f) {
            hits |= 2;
        }
        if (hits == 0) {
            return false;
        }
        switch (hits) {
        case 1:
            fn_8040CE38(move, t0);
            break;
        case 2:
            fn_8040CE38(move, t1);
            break;
        case 3:
            fn_8040CE38(move, t0 < t1 ? t0 : t1);
            break;
        }
        if (result != 0) {
            Fn8040CopyVector3 normal(fn_8040CEE8(&fn_8040CDD0(reinterpret_cast<const Vec3f *>(&offset), move)));
            result->SetPosition(fn_8040CDD0(position, &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&normal), radius)));
            result->m_distance = sphere->m_radius;
            result->field_0x10 = 0.0f;
            result->field_0x2C = 0;
            result->field_0x34 = 0;
            result->SetNormal(*reinterpret_cast<const Vec3f *>(&normal));
        }
        return true;
    }
    return false;
}

void RangeSphere::Reset() {}

void RangeSphere::SetBaseRotation(const Vec3f *rotation) {}

void RangeSphere::SetOffset(const Vec3f *offset) {
    *reinterpret_cast<Vec3f *>(m_sphere.m_center) = fn_8001DFE8(offset);
    UpdateRotationFlags();
    if (m_owner.lock()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), &lbl_805E07B8, 0);
    }
}

void RangeSphere::SetRadius(float radius) {
    m_sphere.m_radius = radius;
}

const Vec3f *RangeSphere::GetBaseRotation() {
    return &lbl_805E07B8;
}

Fn8040CopyVector3 RangeSphere::GetExtent() {
    return ((VectorCopyFn)fn_800169F8)(reinterpret_cast<const Vec3f *>(m_sphere.m_center));
}

Vec3f RangeSphere::GetRotation() {
    return lbl_805E07B8;
}

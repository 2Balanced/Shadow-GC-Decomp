/* SonicteamUSA::System::Range::RangeBox (class name and base from RTTI): an oriented box (shape kind
 * 1, Game/RangeBox.h). Its tests work in the box's own space (m_inverse) and bring the contact back
 * with m_matrix. The unit also holds the box-against-box and box-against-sphere tests the other
 * shapes call, the first copies of the contact's setters, of Geometry::Line's constructor and
 * assignment, and of RangeSphere::GetRadius. Member names and meanings are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first).
 *
 * Not yet written: the box-against-sphere test (0x8002A704), the box's moving-sphere test
 * (vf_0x3C, 0x8002B830) and its segment test in box space (0x8002CFAC). */
#define RANGE_UNIT
#include "Game/CharaColliGeneral.h"
#include "Game/RangeBox.h"
#include "Game/CharaColli.h"
#include "Game/DebugDisp.h"
#include "Game/DebugDraw.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "Game/RangeSphere.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using Sonicteam::System::Geometry::Line;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::Range::RangeBase;
using SonicteamUSA::System::Range::RangeBox;
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
Vec3f fn_8040CDA8(const Vec3f *v, float scale);
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
void fn_8040CFF8(Vec3f *v, const Vec3f *other); /* v += other */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
Vec3f fn_8040B618(const Mtx34f *matrix, const Vec3f *direction);
Mtx34f fn_8001E044(const Mtx34f *a, const Mtx34f *b);
Mtx34f fn_8040BEF8(const Mtx34f *matrix); /* inverse */
void fn_8040BAC4(Mtx34f *matrix, const Mtx34f *other);
float *fn_8040C17C(Mtx34f *matrix, int row, int column);
Vec3f fn_800091A4(const Vec3f *v, float scale);
Vec3f fn_8001DFE8(const Vec3f *v);
UnkSphere_8004E6CC *fn_80021C04(UnkSphere_8004E6CC *sphere, const Vec3f *center, float radius);
Vec3f fn_800169F8(const Vec3f *v);
float fn_8003C660(const Vec3f *point, const UnkDebugLine_80020A20 *line, Vec3f *closest);
bool fn_80028F64(const Vec3f *point, const UnkDebugLine_80020A20 *segment, float radius, Vec3f *closest,
                 float *distance);
bool fn_80029118(RangeBase *a, RangeBase *b);
void fn_80020A20(UnkDebugLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void fn_80049290(Vec3f *angles, const Mtx34f *matrix);
void fn_800493C4(Vec3f *angles, const Mtx34f *matrix);
void fn_800494F8(Vec3f *angles, const Mtx34f *matrix);
}

/* Assigns through the out-of-line vector assignment (fn_800091D0). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* The contact's edge normal where it has one (an edge or corner contact), else its normal. */
inline const Vec3f *EdgeNormal(UnkColliContact_8001FF00 *contact) {
    if (contact->field_0x2C == 1 || contact->field_0x2C == 2) {
        return reinterpret_cast<const Vec3f *>(&contact->field_0x20);
    }
    return reinterpret_cast<const Vec3f *>(&contact->m_normal);
}

/* Three floats (the box description's centre and size are built through one). */
struct RangeFloat3 {
    float v[3];
};

/* A local matrix of the game's matrix class (copied through fn_8040C148). */
struct RangeMatrixCopy {
    RangeMatrixCopy(const Mtx34f &from) {
        fn_8040C148(&m, &from);
    }

    Mtx34f m;
};

/* Plain data the size of a line (the box builds lines in it through fn_80020A20). */
struct RangeLineData {
    float v[6];
};

/* Plain data the size of a sphere description (the box builds spheres in it). */
struct RangeFloat4 {
    float v[4];
};

/* The rotation as Euler angles of a matrix, in the given order. */
extern "C" Vec3f fn_80029418(const Mtx34f *matrix, s32 order) {
    Vec3f angles(0.0f, 0.0f, 0.0f);
    switch (order) {
    case 0:
        fn_80049290(&angles, matrix);
        break;
    case 1:
        fn_800493C4(&angles, matrix);
        break;
    case 2:
        fn_800494F8(&angles, matrix);
        break;
    }
    return angles;
}

/* a . b */
inline float Dot(const Fn8040CopyVector3 &a, const Vec3f &b) {
    return fn_8040CF90(reinterpret_cast<const Vec3f *>(&a), &b);
}

/* Copies from into to through a temporary (two out-of-line copies, fn_800091EC; the second copies
 * from the temporary the first returns): retail copies the box's half sizes this way. */
#define RANGE_COPY_TWICE(to, from)                                                                             fn_800091EC(reinterpret_cast<Vec3f *>(&(to)),                                                                           fn_800091EC(reinterpret_cast<Vec3f *>(&Fn8040CopyVector3()), reinterpret_cast<const Vec3f *>(&(from))))

typedef bool (*HitFn)(RangeBase *self, RangeBase *other);

extern "C" bool fn_8002A67C(RangeBase *self, RangeBase *other);
extern "C" bool fn_80029ED0(RangeBase *self, RangeBase *other);

/* The tests by the other shape's kind (sphere, box). */
static HitFn s_hitTable[] = {fn_8002A67C, fn_80029ED0};

UnkBox_8002DBC8::UnkBox_8002DBC8(const Vec3f &center, const Vec3f &size, const Vec3f &rotation, int arg3) {
    {
        RangeFloat3 c = {0.0f, 0.0f, 0.0f};
        c.v[0] = center.x;
        c.v[1] = center.y;
        c.v[2] = center.z;
        *reinterpret_cast<RangeFloat3 *>(&m_center) = c;
    }
    {
        RangeFloat3 c = {0.0f, 0.0f, 0.0f};
        c.v[0] = size.x;
        c.v[1] = size.y;
        c.v[2] = size.z;
        *reinterpret_cast<RangeFloat3 *>(&m_size) = c;
    }
    m_rotation.x = rotation.x;
    m_rotation.y = rotation.y;
    m_rotation.z = rotation.z;
    field_0x24 = arg3;
}

UnkBox_8002DBC8::UnkBox_8002DBC8(const UnkBox_8002DBC8 &other) {
    *reinterpret_cast<RangeFloat3 *>(&m_center) = *reinterpret_cast<const RangeFloat3 *>(&other.m_center);
    *reinterpret_cast<RangeFloat3 *>(&m_size) = *reinterpret_cast<const RangeFloat3 *>(&other.m_size);
    m_rotation.x = other.m_rotation.x;
    m_rotation.y = other.m_rotation.y;
    m_rotation.z = other.m_rotation.z;
    field_0x24 = other.field_0x24;
}

RangeBox::RangeBox(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position, const Vec3f *rotation, s32 order,
                   boost::weak_ptr<CharaColliBase> owner)
    : RangeBase(1, *position, desc->field_0x8, desc->field_0xC, owner),
      m_box(*reinterpret_cast<const UnkBox_8002DBC8 *>(desc->field_0x4)) {
    UpdateRotationFlags();
    Update(position, rotation, order);
    UpdateBounds();
}

RangeBox::~RangeBox() {}

bool RangeBox::vf_0x48(RangeBase *other) {
    return s_hitTable[other->m_priority](this, other);
}

bool RangeBox::vf_0x4C(RangeBase *other, UnkColliContact_8001FF00 *mine, UnkColliContact_8001FF00 *theirs) {
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

bool RangeBox::vf_0x30(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result) {
    float radius = 0.001f + m_margin;
    if (fn_8003C660(reinterpret_cast<const Vec3f *>(&m_position), line, 0) > radius * radius) {
        return false;
    }
    RangeLineData local;
    fn_80020A20(reinterpret_cast<UnkDebugLine_80020A20 *>(&local), &fn_8040B67C(&m_inverse, &line->m_start), &fn_8040B67C(&m_inverse, &line->m_end));
    bool hit = HitSegment(reinterpret_cast<const UnkDebugLine_80020A20 *>(&local), result);
    if (hit && result != 0) {
        result->SetPosition(fn_8040B67C(&m_matrix, reinterpret_cast<const Vec3f *>(&result->m_position)));
        result->SetNormal(fn_8040B618(&m_matrix, reinterpret_cast<const Vec3f *>(&result->m_normal)));
        result->SetEdgeNormal(fn_8040B618(&m_matrix, EdgeNormal(result)));
    }
    return hit;
}

bool RangeBox::vf_0x34(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result) {
    float radius = 0.001f + m_margin;
    if (!fn_80028F64(reinterpret_cast<const Vec3f *>(&m_position), segment, radius * radius, 0, 0)) {
        return false;
    }
    RangeLineData local;
    fn_80020A20(reinterpret_cast<UnkDebugLine_80020A20 *>(&local), &fn_8040B67C(&m_inverse, &segment->m_start), &fn_8040B67C(&m_inverse, &segment->m_end));
    bool hit = HitSegment(reinterpret_cast<const UnkDebugLine_80020A20 *>(&local), result);
    if (hit && result != 0) {
        result->SetPosition(fn_8040B67C(&m_matrix, reinterpret_cast<const Vec3f *>(&result->m_position)));
        result->SetNormal(fn_8040B618(&m_matrix, reinterpret_cast<const Vec3f *>(&result->m_normal)));
        result->SetEdgeNormal(fn_8040B618(&m_matrix, EdgeNormal(result)));
    }
    return hit;
}

bool RangeBox::vf_0x38(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result) {
    float radius = 0.001f + (sphere->m_radius + m_margin);
    if (fn_8040D180(reinterpret_cast<const Vec3f *>(&m_position), &sphere->m_center) > radius * radius) {
        return false;
    }
    Fn8040CopyVector3 center(fn_8040B67C(&m_inverse, &sphere->m_center));
    bool hit = HitSphere(reinterpret_cast<const Vec3f *>(&center), sphere->m_radius, result);
    if (hit && result != 0) {
        result->SetPosition(fn_8040B67C(&m_matrix, reinterpret_cast<const Vec3f *>(&result->m_position)));
        result->SetNormal(fn_8040B618(&m_matrix, reinterpret_cast<const Vec3f *>(&result->m_normal)));
        result->SetEdgeNormal(fn_8040B618(&m_matrix, EdgeNormal(result)));
    }
    return hit;
}

Vec3f RangeBox::FaceNormal(const Vec3f *point, const Vec3f *half) {
    float dx = std::fabs(point->x) - half->x;
    float dy = std::fabs(point->y) - half->y;
    float dz = std::fabs(point->z) - half->z;
    if (dx < dy) {
        if (dz < dy) {
            return Vec3f(0.0f, point->y < 0.0f ? -1.0f : 1.0f, 0.0f);
        }
        return Vec3f(0.0f, 0.0f, point->z < 0.0f ? -1.0f : 1.0f);
    }
    if (dz < dx) {
        return Vec3f(point->x < 0.0f ? -1.0f : 1.0f, 0.0f, 0.0f);
    }
    return Vec3f(0.0f, 0.0f, point->z < 0.0f ? -1.0f : 1.0f);
}

extern "C" inline Line *fn_8002CCF0(Line *line, const Line *other) {
    reinterpret_cast<Fn8040CopyVector3 &>(line->m_start) = reinterpret_cast<const Fn8040CopyVector3 &>(other->m_start);
    reinterpret_cast<Fn8040CopyVector3 &>(line->m_end) = reinterpret_cast<const Fn8040CopyVector3 &>(other->m_end);
    return line;
}

void RangeBox::Update(const Vec3f *position, const Vec3f *rotation, s32 order) {
    fn_8040C148(&m_matrix, &((TranslationFn)fn_8040B034)(position));
    fn_8001E070(&m_matrix, rotation, order);
    fn_8040BAC4(&m_matrix, &((TranslationFn)fn_8040B034)(reinterpret_cast<const Vec3f *>(&GetExtent())));
    fn_8001E070(&m_matrix, GetBaseRotation(), GetRotationOrder());
    m_position.x = *fn_8040C17C(&m_matrix, 0, 3);
    m_position.y = *fn_8040C17C(&m_matrix, 1, 3);
    m_position.z = *fn_8040C17C(&m_matrix, 2, 3);
    fn_8040C148(&m_inverse, &fn_8040BEF8(&m_matrix));
}

void RangeBox::SetMatrix(Mtx34f *matrix) {
    fn_8040C148(&m_matrix, &fn_8001E044(matrix, &((TranslationFn)fn_8040B034)(
                                                    reinterpret_cast<const Vec3f *>(&GetExtent()))));
    fn_8001E070(&m_matrix, GetBaseRotation(), GetRotationOrder());
    m_position.x = *fn_8040C17C(&m_matrix, 0, 3);
    m_position.y = *fn_8040C17C(&m_matrix, 1, 3);
    m_position.z = *fn_8040C17C(&m_matrix, 2, 3);
    fn_8040C148(&m_inverse, &fn_8040BEF8(&m_matrix));
}

void RangeBox::UpdateBounds() {
    Fn8040CopyVector3 half(GetHalfSize());
    float x = half.x * half.x;
    float y = half.y * half.y;
    float z = half.z * half.z;
    m_margin = fn_8000FEE4(z + (x + y));
}

void RangeBox::SetScale(const Vec3f *scale) {
    Fn8040CopyVector3 offset(GetExtent());
    offset.x = offset.x * scale->x;
    offset.y = offset.y * scale->y;
    offset.z = offset.z * scale->z;
    SetOffset(reinterpret_cast<const Vec3f *>(&offset));
    if (m_owner.lock()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), owner->vf_0x18(), owner->vf_0x24());
    }
    Fn8040CopyVector3 half(GetHalfSize());
    SetSize(half.x * scale->x, half.y * scale->y, half.z * scale->z);
}

void RangeBox::GetSize(float *x, float *y, float *z) {
    Fn8040CopyVector3 half(GetHalfSize());
    if (x != 0) {
        *x = half.x;
    }
    if (y != 0) {
        *y = half.y;
    }
    if (z != 0) {
        *z = half.z;
    }
}

void RangeBox::SetHalfSize(const Vec3f *half) {
    *reinterpret_cast<Vec3f *>(&m_box.m_size) = fn_8001DFE8(half);
}

void RangeBox::SetSize(float x, float y, float z) {
    Vec3f half(x, y, z);
    SetHalfSize(&half);
    UpdateBounds();
}

float RangeBox::Clamp(float value, float half, s32 *side) {
    if (half < value) {
        *side = 2;
        return half;
    }
    if (value < -half) {
        *side = 0;
        return -half;
    }
    *side = 1;
    return value;
}

extern "C" bool fn_8002A67C(RangeBase *self, RangeBase *other) {
    RangeBox *box = static_cast<RangeBox *>(self);
    RangeSphere *sphere = static_cast<RangeSphere *>(other);
    if (fn_80029118(box, sphere)) {
        Fn8040CopyVector3 center(fn_8040B67C(&box->m_inverse, reinterpret_cast<const Vec3f *>(&sphere->m_position)));
        return box->HitSphere(reinterpret_cast<const Vec3f *>(&center), sphere->GetRadius(), 0);
    }
    return false;
}

extern "C" bool fn_80029ED0(RangeBase *self, RangeBase *other) {
    RangeBox *a = static_cast<RangeBox *>(self);
    RangeBox *b = static_cast<RangeBox *>(other);
    if (fn_80029118(a, b)) {
        RangeMatrixCopy rotation(fn_8001E044(&a->m_inverse, b->GetMatrix()));
        Vec3f c0(rotation.m.values[0][0], rotation.m.values[1][0], rotation.m.values[2][0]);
        Vec3f c1(rotation.m.values[0][1], rotation.m.values[1][1], rotation.m.values[2][1]);
        Vec3f c2(rotation.m.values[0][2], rotation.m.values[1][2], rotation.m.values[2][2]);
        Fn8040CopyVector3 t(fn_8040B618(&a->m_inverse, &((VectorSubtractFn)fn_8040CD74)(
                                                           reinterpret_cast<const Vec3f *>(&b->m_position),
                                                           reinterpret_cast<const Vec3f *>(&a->m_position))));
        Fn8040CopyVector3 ha;
        RANGE_COPY_TWICE(ha, a->GetHalfSize());
        Fn8040CopyVector3 hb;
        RANGE_COPY_TWICE(hb, b->GetHalfSize());

        {
            if (std::fabs(t.x) > 0.001f + (ha.x + hb.x * std::fabs(c0.x) + hb.y * std::fabs(c1.x) + hb.z * std::fabs(c2.x))) {
                return false;
            }
        }
        {
            if (std::fabs(t.y) > 0.001f + (ha.y + hb.x * std::fabs(c0.y) + hb.y * std::fabs(c1.y) + hb.z * std::fabs(c2.y))) {
                return false;
            }
        }
        {
            if (std::fabs(t.z) > 0.001f + (ha.z + hb.x * std::fabs(c0.z) + hb.y * std::fabs(c1.z) + hb.z * std::fabs(c2.z))) {
                return false;
            }
        }

        {
            if (std::fabs(Dot(t, c0)) > 0.001f + (hb.x + ha.x * std::fabs(c0.x) + ha.y * std::fabs(c0.y) + ha.z * std::fabs(c0.z))) {
                return false;
            }
        }
        {
            if (std::fabs(Dot(t, c1)) > 0.001f + (hb.y + ha.x * std::fabs(c1.x) + ha.y * std::fabs(c1.y) + ha.z * std::fabs(c1.z))) {
                return false;
            }
        }
        {
            if (std::fabs(Dot(t, c2)) > 0.001f + (hb.z + ha.x * std::fabs(c2.x) + ha.y * std::fabs(c2.y) + ha.z * std::fabs(c2.z))) {
                return false;
            }
        }

        {
            if (std::fabs(-t.y * c0.z + t.z * c0.y) > 0.001f + (ha.y * std::fabs(c0.z) + ha.z * std::fabs(c0.y) + hb.y * std::fabs(c2.x) + hb.z * std::fabs(c1.x))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.y * c1.z + t.z * c1.y) > 0.001f + (ha.y * std::fabs(c1.z) + ha.z * std::fabs(c1.y) + hb.z * std::fabs(c0.x) + hb.x * std::fabs(c2.x))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.y * c2.z + t.z * c2.y) > 0.001f + (ha.y * std::fabs(c2.z) + ha.z * std::fabs(c2.y) + hb.x * std::fabs(c1.x) + hb.y * std::fabs(c0.x))) {
                return false;
            }
        }

        {
            if (std::fabs(-t.z * c0.x + t.x * c0.z) > 0.001f + (ha.z * std::fabs(c0.x) + ha.x * std::fabs(c0.z) + hb.y * std::fabs(c2.y) + hb.z * std::fabs(c1.y))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.z * c1.x + t.x * c1.z) > 0.001f + (ha.z * std::fabs(c1.x) + ha.x * std::fabs(c1.z) + hb.z * std::fabs(c0.y) + hb.x * std::fabs(c2.y))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.z * c2.x + t.x * c2.z) > 0.001f + (ha.z * std::fabs(c2.x) + ha.x * std::fabs(c2.z) + hb.x * std::fabs(c1.y) + hb.y * std::fabs(c0.y))) {
                return false;
            }
        }

        {
            if (std::fabs(-t.x * c0.y + t.y * c0.x) > 0.001f + (ha.x * std::fabs(c0.y) + ha.y * std::fabs(c0.x) + hb.y * std::fabs(c2.z) + hb.z * std::fabs(c1.z))) {
                return false;
            }
        }
        {
            if (std::fabs(-t.x * c1.y + t.y * c1.x) > 0.001f + (ha.x * std::fabs(c1.y) + ha.y * std::fabs(c1.x) + hb.z * std::fabs(c0.z) + hb.x * std::fabs(c2.z))) {
                return false;
            }
        }
        return !(std::fabs(-t.x * c2.y + t.y * c2.x) > 0.001f + (ha.x * std::fabs(c2.y) + ha.y * std::fabs(c2.x) + hb.x * std::fabs(c1.z) + hb.y * std::fabs(c0.z)));
    }
    return false;
}

/* Where point lies against the box's half sizes along each axis (0 below, 1 within, 2 above), and
 * the region those make (x + 3 y + 9 z). */
extern "C" void fn_80029DA8(RangeBox *box, const Vec3f *point, s32 *x, s32 *y, s32 *z, s32 *region) {
    const Fn8040CopyVector3 &half = box->GetHalfSize();
    if (point->x < -half.x) {
        *x = 0;
    } else if (half.x < point->x) {
        *x = 2;
    } else {
        *x = 1;
    }
    *region = *x;
    if (point->y < -half.y) {
        *y = 0;
    } else if (half.y < point->y) {
        *y = 2;
    } else {
        *y = 1;
    }
    *region += *y * 3;
    if (point->z < -half.z) {
        *z = 0;
    } else if (half.z < point->z) {
        *z = 2;
    } else {
        *z = 1;
    }
    *region += *z * 9;
}

Line::Line() {}

UnkDebugLine_80020A20 RangeBox::GetEdge(int index) {
    Line line;
    Vec3f center(m_matrix.values[0][3], m_matrix.values[1][3], m_matrix.values[2][3]);
    if (index >= 0 && index <= 3) {
        float s = (index & 1) ? GetHalfSize().x : -GetHalfSize().x;
        Vec3f axis0;
        fn_8040CFF8(&center, &fn_800091A4(fn_80008E1C(&axis0, m_matrix.values[0][0], m_matrix.values[1][0],
                                                      m_matrix.values[2][0]),
                                         s));
        s = (index & 2) ? GetHalfSize().y : -GetHalfSize().y;
        Vec3f axis1;
        fn_8040CFF8(&center, &fn_800091A4(fn_80008E1C(&axis1, m_matrix.values[0][1], m_matrix.values[1][1],
                                                      m_matrix.values[2][1]),
                                         s));
        AssignVector(line.m_start,
                     fn_8040CDD0(&center, &fn_8040CDA8(&Vec3f(m_matrix.values[0][2], m_matrix.values[1][2],
                                                              m_matrix.values[2][2]),
                                                        -GetHalfSize().z)));
        AssignVector(line.m_end,
                     fn_8040CDD0(&center, &fn_8040CDA8(&Vec3f(m_matrix.values[0][2], m_matrix.values[1][2],
                                                              m_matrix.values[2][2]),
                                                        GetHalfSize().z)));
        return reinterpret_cast<const UnkDebugLine_80020A20 &>(line);
    } else if (index >= 4 && index <= 7) {
        float s = ((index - 4) & 1) ? GetHalfSize().x : -GetHalfSize().x;
        Vec3f axis0;
        fn_8040CFF8(&center, &fn_800091A4(fn_80008E1C(&axis0, m_matrix.values[0][0], m_matrix.values[1][0],
                                                      m_matrix.values[2][0]),
                                         s));
        s = ((index - 4) & 2) ? GetHalfSize().z : -GetHalfSize().z;
        Vec3f axis2;
        fn_8040CFF8(&center, &fn_800091A4(fn_80008E1C(&axis2, m_matrix.values[0][2], m_matrix.values[1][2],
                                                      m_matrix.values[2][2]),
                                         s));
        AssignVector(line.m_start,
                     fn_8040CDD0(&center, &fn_8040CDA8(&Vec3f(m_matrix.values[0][1], m_matrix.values[1][1],
                                                              m_matrix.values[2][1]),
                                                        -GetHalfSize().y)));
        AssignVector(line.m_end,
                     fn_8040CDD0(&center, &fn_8040CDA8(&Vec3f(m_matrix.values[0][1], m_matrix.values[1][1],
                                                              m_matrix.values[2][1]),
                                                        GetHalfSize().y)));
        return reinterpret_cast<const UnkDebugLine_80020A20 &>(line);
    } else {
        float s = ((index - 8) & 1) ? GetHalfSize().y : -GetHalfSize().y;
        Vec3f axis1;
        fn_8040CFF8(&center, &fn_800091A4(fn_80008E1C(&axis1, m_matrix.values[0][1], m_matrix.values[1][1],
                                                      m_matrix.values[2][1]),
                                         s));
        s = ((index - 8) & 2) ? GetHalfSize().z : -GetHalfSize().z;
        Vec3f axis2;
        fn_8040CFF8(&center, &fn_800091A4(fn_80008E1C(&axis2, m_matrix.values[0][2], m_matrix.values[1][2],
                                                      m_matrix.values[2][2]),
                                         s));
        AssignVector(line.m_start,
                     fn_8040CDD0(&center, &fn_8040CDA8(&Vec3f(m_matrix.values[0][0], m_matrix.values[1][0],
                                                              m_matrix.values[2][0]),
                                                        -GetHalfSize().x)));
        AssignVector(line.m_end,
                     fn_8040CDD0(&center, &fn_8040CDA8(&Vec3f(m_matrix.values[0][0], m_matrix.values[1][0],
                                                              m_matrix.values[2][0]),
                                                        GetHalfSize().x)));
        return reinterpret_cast<const UnkDebugLine_80020A20 &>(line);
    }
}

Fn8040CopyVector3 RangeBox::GetCorner(int index) {
    Fn8040CopyVector3 corner;
    fn_800091EC(reinterpret_cast<Vec3f *>(&corner),
                &fn_8040CDD0(&Vec3f(m_matrix.values[0][3], m_matrix.values[1][3], m_matrix.values[2][3]),
                             &fn_8040CDA8(&Vec3f(m_matrix.values[0][0], m_matrix.values[1][0], m_matrix.values[2][0]),
                                          (index & 1) ? GetHalfSize().x : -GetHalfSize().x)));
    Vec3f axis1;
    fn_8040CFF8(reinterpret_cast<Vec3f *>(&corner),
                &fn_800091A4(fn_80008E1C(&axis1, m_matrix.values[0][1], m_matrix.values[1][1], m_matrix.values[2][1]),
                             (index & 2) ? GetHalfSize().y : -GetHalfSize().y));
    Vec3f axis2;
    fn_8040CFF8(reinterpret_cast<Vec3f *>(&corner),
                &fn_800091A4(fn_80008E1C(&axis2, m_matrix.values[0][2], m_matrix.values[1][2], m_matrix.values[2][2]),
                             (index & 4) ? GetHalfSize().z : -GetHalfSize().z));
    return corner;
}

Fn8040CopyVector3 RangeBox::GetHalfSize() {
    return ((VectorCopyFn)fn_800169F8)(reinterpret_cast<const Vec3f *>(&m_box.m_size));
}

void RangeBox::Reset() {}

void RangeBox::SetBaseRotation(const Vec3f *rotation) {
    m_box.m_rotation.x = rotation->x;
    m_box.m_rotation.y = rotation->y;
    m_box.m_rotation.z = rotation->z;
    UpdateRotationFlags();
    if (!m_owner.expired()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), owner->vf_0x18(), owner->vf_0x24());
    }
}

void RangeBox::SetOffset(const Vec3f *offset) {
    *reinterpret_cast<Vec3f *>(&m_box.m_center) = fn_8001DFE8(offset);
    UpdateRotationFlags();
    if (!m_owner.expired()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), owner->vf_0x18(), owner->vf_0x24());
    }
}

s32 RangeBox::GetRotationOrder() {
    return m_box.field_0x24;
}

const Vec3f *RangeBox::GetBaseRotation() {
    return &m_box.m_rotation;
}

Fn8040CopyVector3 RangeBox::GetExtent() {
    return ((VectorCopyFn)fn_800169F8)(reinterpret_cast<const Vec3f *>(&m_box.m_center));
}

Vec3f RangeBox::GetRotation() {
    if (!m_owner.expired()) {
        boost::shared_ptr<CharaColliBase> owner(m_owner);
        owner->vf_0x7C();
        return fn_80029418(&m_matrix, owner->vf_0x24());
    }
    return fn_80029418(&m_matrix, m_box.field_0x24);
}

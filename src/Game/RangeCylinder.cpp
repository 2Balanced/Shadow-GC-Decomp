/* SonicteamUSA::System::Range::RangeCylinder (class name and base from RTTI): a cylinder (shape kind 3,
 * Game/RangeCylinder.h). It tests most queries in its own space (m_inverse): the flat caps as planes,
 * the round side as a circle in its XY plane, and a moving sphere against the rim by solving a quartic.
 * Against boxes, capsules and other cylinders it runs a separating-axis test, its radial extent along
 * an axis taken from the axis's angle in its XY plane (SinCos2). Member names and meanings are
 * inferred. Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first).
 *
 * Not matching yet (unit NonMatching, 39/43): HitRim (the quartic's coefficients are scheduled
 * differently), vf_0x3C (GC/2.7 calls the Vec3f constructor of the normal's temporary out of line),
 * and the tests against a capsule (fn_800315C4) and a cylinder (fn_80030E08), whose registers are
 * allocated in a different order. */
#define RANGE_UNIT
#include "Game/CharaColliGeneral.h"
#include "Game/RangeCylinder.h"
#include "Game/CharaColli.h"
#include "Game/DebugDisp.h"
#include "Game/DebugDraw.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "Game/RangeBox.h"
#include "Game/RangeCapsule.h"
#include "Game/RangeSphere.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using Sonicteam::System::Geometry::Line;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::Range::RangeBase;
using SonicteamUSA::System::Range::RangeBox;
using SonicteamUSA::System::Range::RangeCapsule;
using SonicteamUSA::System::Range::RangeCylinder;
using SonicteamUSA::System::Range::RangeSphere;

extern "C" {
double sin(double x);
double cos(double x);
}

namespace std {
float fabs(float x);

inline float sin(float x) {
    return ::sin(x);
}

inline float cos(float x) {
    return ::cos(x);
}
} // namespace std

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
float fn_8040CE74(Vec3f *v); /* normalizes v */
Vec3f fn_8040D044(const Vec3f *v, float angle);
void fn_8003B8F4(const Vec3f *direction, float *a, float *b);
bool fn_8003B03C(float *root0, float *root1, float *root2, float *root3, float a, float b, float c, float d);
float fn_8040CD20(const Vec2f *a, const Vec2f *b);  /* a . b */
Vec2f fn_8040CC38(const Vec2f *a, const Vec2f *b);  /* a + b */
Vec2f fn_8040CC1C(const Vec2f *v, float scale);
float fn_8040CCD4(const Vec2f *v);                  /* |v|^2 */
}

/* A point and a normal (a plane), built by the out-of-line fn_8001C4B8. */
struct RangeVecPair {
    float v[6];
};

extern "C" {
RangeVecPair *fn_8001C4B8(RangeVecPair *pair, const Vec3f *point, const Vec3f *normal);
float fn_8040C340(const RangeVecPair *plane, const Vec3f *point, Vec3f *foot); /* signed distance */
bool fn_8003BADC(const UnkDebugLine_80020A20 *line, const RangeVecPair *plane, Vec3f *hit);
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

/* A vector of three floats, as a value. */
inline Vec3f MakeVector(float x, float y, float z) {
    Vec3f v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

/* Three floats (the description's centre is built through one). */
struct RangeFloat3 {
    float v[3];
};

extern "C" Vec3f fn_80029418(const Mtx34f *matrix, s32 order);

inline const UnkDebugLine_80020A20 *AsLine(const Line *line) {
    return reinterpret_cast<const UnkDebugLine_80020A20 *>(line);
}

typedef bool (*HitFn)(RangeBase *self, RangeBase *other);

extern "C" bool fn_800325D8(RangeBase *self, RangeBase *other);
extern "C" bool fn_80031EEC(RangeBase *self, RangeBase *other);
extern "C" bool fn_800315C4(RangeBase *self, RangeBase *other);
extern "C" bool fn_80030E08(RangeBase *self, RangeBase *other);

/* a . b */
inline float Dot(const Fn8040CopyVector3 &a, const Vec3f &b) {
    return fn_8040CF90(reinterpret_cast<const Vec3f *>(&a), &b);
}

/* The sine and cosine of angle, and of the angle of (x, y) (an axis along x, or along nothing, when x is
 * zero). Each calls another inline function, so GC/2.7 keeps them out of line. */
inline void SinCos(float *s, float *c, float angle) {
    *s = std::sin(angle);
    *c = std::cos(angle);
}

inline void SinCos2(float *s, float *c, float y, float x) {
    if (x == 0.0f) {
        *c = 0.0f;
        *s = y == 0.0f ? 0.0f : 1.0f;
    } else {
        SinCos(s, c, fn_80016260(y, x));
    }
}

/* Vec2f sums and products (each calls another inline function, so GC/2.7 keeps them out of line). */
inline Vec2f Vec2Add(const Vec2f &a, const Vec2f &b) {
    return fn_8040CC38(&a, &b);
}

inline Vec2f Vec2Scale(const Vec2f &v, float scale) {
    return fn_8040CC1C(&v, scale);
}

inline Vec2f operator+(const Vec2f &a, const Vec2f &b) {
    return Vec2Add(a, b);
}

/* v * scale (other units call it by this name). */
extern "C" inline Vec2f fn_80033934(const Vec2f *v, float scale) {
    return Vec2Scale(*v, scale);
}

/* The tests by the other shape's kind (sphere, box, capsule, cylinder). */
static HitFn s_hitTable[] = {fn_800325D8, fn_80031EEC, fn_800315C4, fn_80030E08};

UnkCylinder_80034854::UnkCylinder_80034854(const Vec3f &center, float halfHeight, float radius,
                                           const Vec3f &rotation, int arg4) {
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

UnkCylinder_80034854::UnkCylinder_80034854(const UnkCylinder_80034854 &other) {
    *reinterpret_cast<RangeFloat3 *>(&m_center) = *reinterpret_cast<const RangeFloat3 *>(&other.m_center);
    m_halfHeight = other.m_halfHeight;
    m_radius = other.m_radius;
    m_rotation.x = other.m_rotation.x;
    m_rotation.y = other.m_rotation.y;
    m_rotation.z = other.m_rotation.z;
    field_0x20 = other.field_0x20;
}

RangeCylinder::RangeCylinder(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position, const Vec3f *rotation,
                             s32 order, boost::weak_ptr<CharaColliBase> owner)
    : RangeBase(3, *position, desc->field_0x8, desc->field_0xC, owner),
      m_cylinder(*reinterpret_cast<const UnkCylinder_80034854 *>(desc->field_0x4)) {
    UpdateRotationFlags();
    Update(position, rotation, order);
    UpdateBounds();
}

RangeCylinder::~RangeCylinder() {}

bool RangeCylinder::vf_0x48(RangeBase *other) {
    return s_hitTable[other->m_priority](this, other);
}

bool RangeCylinder::vf_0x4C(RangeBase *other, UnkColliContact_8001FF00 *mine, UnkColliContact_8001FF00 *theirs) {
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

bool RangeCylinder::vf_0x30(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result) {
    float bound = 0.001f + m_margin;
    if (fn_8003C660(reinterpret_cast<const Vec3f *>(&m_position), line, 0) > bound * bound) {
        return false;
    }
    return HitLine(line, result);
}

bool RangeCylinder::vf_0x34(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result) {
    float bound = 0.001f + m_margin;
    if (!fn_80028F64(reinterpret_cast<const Vec3f *>(&m_position), segment, bound * bound, 0, 0)) {
        return false;
    }
    return HitLine(segment, result);
}

bool RangeCylinder::HitLine(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result) {
    Fn8040CopyVector3 direction(((VectorSubtractFn)fn_8040CD74)(&line->m_end, &line->m_start));
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(&line->m_start, &m_line.m_start));
    Fn8040CopyVector3 axis(((VectorSubtractFn)fn_8040CD74)(&m_line.m_end, &m_line.m_start));
    float along = fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&offset));
    float radius = m_cylinder.m_radius;
    if (along < 0.0f) {
        RangeVecPair plane;
        fn_8001C4B8(&plane, &m_line.m_start, &fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)));
        Vec3f hit;
        if (fn_8003BADC(line, &plane, &hit)) {
            if (fn_8040D180(&m_line.m_start, &hit) < radius * radius) {
                if (result != 0) {
                    result->SetPosition(hit);
                    result->m_distance = fn_8040D1B4(&line->m_start, &hit);
                    result->field_0x10 = result->GetDistance();
                    result->SetNormal(fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)));
                    result->field_0x2C = 0;
                }
                return true;
            }
            return HitSide(line, result);
        }
        return false;
    } else if (along > fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&axis))) {
        RangeVecPair plane;
        fn_8001C4B8(&plane, &m_line.m_end, reinterpret_cast<const Vec3f *>(&m_axis));
        Vec3f hit;
        if (fn_8003BADC(line, &plane, &hit)) {
            if (fn_8040D180(&m_line.m_end, &hit) < radius * radius) {
                if (result != 0) {
                    result->SetPosition(hit);
                    result->m_distance = fn_8040D1B4(&line->m_start, &hit);
                    result->field_0x10 = result->GetDistance();
                    result->SetNormal(*reinterpret_cast<const Vec3f *>(&m_axis));
                    result->field_0x2C = 0;
                }
                return true;
            }
            return HitSide(line, result);
        }
        return false;
    }
    return HitSide(line, result);
}

bool RangeCylinder::vf_0x38(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result) {
    float along = 0.001f + (sphere->m_radius + m_margin);
    if (fn_8040D180(reinterpret_cast<const Vec3f *>(&m_position), &sphere->m_center) > along * along) {
        return false;
    }
    Fn8040CopyVector3 offset(((VectorSubtractFn)fn_8040CD74)(&sphere->m_center, &m_line.m_start));
    Fn8040CopyVector3 axis(((VectorSubtractFn)fn_8040CD74)(&m_line.m_end, &m_line.m_start));
    along = fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&offset));
    float radius = m_cylinder.m_radius;
    if (along < 0.0f) {
        RangeVecPair plane;
        fn_8001C4B8(&plane, &m_line.m_start, &fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)));
        Vec3f foot;
        float height = fn_8040C340(&plane, &sphere->m_center, &foot);
        if (height < sphere->m_radius) {
            float across = fn_8040D1B4(&m_line.m_start, &foot);
            if (across < radius) {
                if (result != 0) {
                    result->SetPosition(foot);
                    result->m_distance = height;
                    result->field_0x10 = height - sphere->m_radius;
                    result->field_0x2C = 0;
                    result->SetNormal(fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)));
                }
                return true;
            }
            if (across < radius + sphere->m_radius) {
                Fn8040CopyVector3 rim(fn_8040CDD0(&m_line.m_start,
                                                  &fn_8040CDA8(&((VectorSubtractFn)fn_8040CD74)(&foot, &m_line.m_start),
                                                               radius / across)));
                radius = fn_8040D180(&sphere->m_center, reinterpret_cast<const Vec3f *>(&rim));
                if (radius < sphere->m_radius * sphere->m_radius) {
                    if (result != 0) {
                        result->SetPosition(*reinterpret_cast<const Vec3f *>(&rim));
                        result->m_distance = fn_8000FEE4(radius);
                        result->field_0x10 = result->GetDistance() - sphere->m_radius;
                        result->field_0x2C = 1;
                        result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(
                            &sphere->m_center, reinterpret_cast<const Vec3f *>(&rim))));
                        if (height < across) {
                            result->SetEdgeNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&foot, &m_line.m_start)));
                        } else {
                            result->SetEdgeNormal(fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)));
                        }
                    }
                    return true;
                }
                return false;
            }
            return false;
        }
        return false;
    } else if (along > fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&axis))) {
        RangeVecPair plane;
        fn_8001C4B8(&plane, &m_line.m_end, reinterpret_cast<const Vec3f *>(&m_axis));
        Vec3f foot;
        float height = fn_8040C340(&plane, &sphere->m_center, &foot);
        if (height < sphere->m_radius) {
            float across = fn_8040D1B4(&m_line.m_end, &foot);
            if (across < radius) {
                if (result != 0) {
                    result->SetPosition(foot);
                    result->m_distance = height;
                    result->field_0x10 = height - sphere->m_radius;
                    result->field_0x2C = 0;
                    result->SetNormal(*reinterpret_cast<const Vec3f *>(&m_axis));
                }
                return true;
            }
            if (across < radius + sphere->m_radius) {
                Fn8040CopyVector3 rim(fn_8040CDD0(&m_line.m_end,
                                                  &fn_8040CDA8(&((VectorSubtractFn)fn_8040CD74)(&foot, &m_line.m_end),
                                                               radius / across)));
                radius = fn_8040D180(&sphere->m_center, reinterpret_cast<const Vec3f *>(&rim));
                if (radius < sphere->m_radius * sphere->m_radius) {
                    if (result != 0) {
                        result->SetPosition(*reinterpret_cast<const Vec3f *>(&rim));
                        result->m_distance = fn_8000FEE4(radius);
                        result->field_0x10 = result->GetDistance() - sphere->m_radius;
                        result->field_0x2C = 1;
                        result->SetNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(
                            &sphere->m_center, reinterpret_cast<const Vec3f *>(&rim))));
                        if (height < across) {
                            result->SetEdgeNormal(fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(&foot, &m_line.m_end)));
                        } else {
                            result->SetEdgeNormal(*reinterpret_cast<const Vec3f *>(&m_axis));
                        }
                    }
                    return true;
                }
                return false;
            }
            return false;
        }
        return false;
    } else {
        Fn8040CopyVector3 closest(fn_8040CDD0(&m_line.m_start,
                                              &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&axis),
                                                           along / fn_8040CF90(reinterpret_cast<const Vec3f *>(&axis), reinterpret_cast<const Vec3f *>(&axis)))));
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

bool RangeCylinder::HitSide(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result) {
    Fn8040CopyVector3 direction(((VectorSubtractFn)fn_8040CD74)(&line->m_end, &line->m_start));
    Fn8040CopyVector3 start(fn_8040B67C(&m_inverse, &line->m_start));
    Fn8040CopyVector3 move(fn_8040B618(&m_inverse, reinterpret_cast<const Vec3f *>(&direction)));
    float radius = m_cylinder.m_radius;
    if (start.x * start.x + start.y * start.y <= 1.0001f * (radius * radius)) {
        if (std::fabs(start.z) <= HalfHeight()) {
            if (result != 0) {
                result->SetPosition(line->m_start);
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
        return false;
    }
    float along = start.x * move.x + start.y * move.y;
    if (along >= -0.0001f) {
        return false;
    }
    float t0;
    float t1;
    if (fn_8003B7AC(&t0, &t1, move.x * move.x + move.y * move.y, 2.0f * along,
                    start.x * start.x + start.y * start.y - radius * radius)) {
        if (t0 > 0.0f || t1 > 0.0f) {
            float t;
            if (t0 > 0.0f && t1 > 0.0f) {
                t = t0 < t1 ? t0 : t1;
            } else if (t0 > 0.0f) {
                t = t0;
            } else {
                t = t1;
            }
            if (t > 1.0f) {
                return false;
            }
            Fn8040CopyVector3 hit(fn_8040CDD0(reinterpret_cast<const Vec3f *>(&start),
                                              &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&move), t)));
            if (std::fabs(hit.z) <= HalfHeight()) {
                if (result != 0) {
                    result->SetPosition(fn_8040B67C(&m_matrix, reinterpret_cast<const Vec3f *>(&hit)));
                    result->m_distance = t * fn_8040CF50(reinterpret_cast<const Vec3f *>(&move));
                    result->field_0x10 = result->GetDistance();
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

/* The squared distance from the origin to the segment from point along direction (2D). */
float DistanceSq2D(const Vec2f *point, const Vec2f *direction) {
    float along = -fn_8040CD20(point, direction);
    if (along > 0.0f) {
        float lengthSq = fn_8040CD20(direction, direction);
        if (along >= lengthSq) {
            return fn_8040CCD4(&Vec2Add(*point, *direction));
        }
        Vec2f closest = *point + fn_80033934(direction, along / lengthSq);
        return fn_8040CCD4(&closest);
    }
    return fn_8040CCD4(point);
}

bool RangeCylinder::HitRim(const UnkSphere_8004E6CC *sphere, Vec3f *move, const Vec3f *start, const Vec3f *direction,
                           UnkColliContact_8001FF00 *result, float height) {
    float angleA;
    float angleB;
    fn_8003B8F4(direction, &angleA, &angleB);
    Fn8040CopyVector3 d(fn_8040D044(direction, -angleB));
    Fn8040CopyVector3 p(fn_8040D044(start, -angleB));
    float k = d.y / d.z;
    p.z -= height;
    float k2 = k * k + 1.0f;
    float py = p.y - k * p.z;
    float inv = 1.0f / (k2 * k2);
    float q = py * py + p.x * p.x;
    float r2 = m_cylinder.m_radius * m_cylinder.m_radius;
    float e = r2 + q - sphere->m_radius * sphere->m_radius;
    float r4 = 4.0f * r2;
    float b = 2.0f * k * py;
    float roots[4];
    if (fn_8003B03C(&roots[0], &roots[1], &roots[2], &roots[3], 2.0f * k2 * b * inv,
                    (b * b + 2.0f * e * k2 - k * (r4 * k)) * inv, b * (2.0f * e - r4) * inv,
                    (e * e - r4 * q) * inv)) {
        float best = 100.0f;
        for (int i = 0; i < 4; i++) {
            float t = (roots[i] - p.z) / d.z;
            if (0.0f <= t && t <= 1.0f && t < best) {
                best = t;
            }
        }
        if (0.0f <= best && best <= 1.0f) {
            fn_8040CE38(move, best);
            if (result != 0) {
                Fn8040CopyVector3 hit(fn_8040CDD0(start, &fn_8040CDA8(direction, best)));
                RangeAxis rim(hit.x, hit.y, 0.0f);
                rim = fn_8040CDA8(&fn_8040CEE8(reinterpret_cast<const Vec3f *>(&rim)), m_cylinder.m_radius);
                rim.z = height;
                result->SetPosition(fn_8040B67C(&m_matrix, reinterpret_cast<const Vec3f *>(&rim)));
                result->SetNormal(fn_8040B618(&m_matrix, &fn_8040CEE8(&((VectorSubtractFn)fn_8040CD74)(
                                                             reinterpret_cast<const Vec3f *>(&hit),
                                                             reinterpret_cast<const Vec3f *>(&rim)))));
                result->m_distance = sphere->m_radius;
                result->field_0x10 = 0.0f;
                result->field_0x34 = 0;
                result->field_0x2C = 0;
            }
            return true;
        }
        return false;
    }
    return false;
}

bool RangeCylinder::HitTop(const UnkSphere_8004E6CC *sphere, Vec3f *move, const Vec3f *start, const Vec3f *end,
                           const Vec3f *direction, UnkColliContact_8001FF00 *result) {
    float height = m_cylinder.m_halfHeight;
    float gap = height - (start->z - sphere->m_radius - 0.001f);
    if (gap < 0.0f) {
        if (end->z - sphere->m_radius - 0.001f < height) {
            float t;
            if (direction->z != 0.0f) {
                t = gap / direction->z;
            } else {
                t = 0.0f;
            }
            Fn8040CopyVector3 hit(fn_8040CDD0(start, &fn_8040CDA8(direction, t)));
            float bound = 0.001f + m_cylinder.m_radius;
            bound = bound * bound;
            if (hit.x * hit.x + hit.y * hit.y > bound) {
                return false;
            }
            fn_8040CE38(move, t);
            if (result != 0) {
                result->SetNormal(*reinterpret_cast<const Vec3f *>(&m_axis));
                hit.z -= sphere->m_radius;
                result->SetPosition(fn_8040B67C(&m_matrix, reinterpret_cast<const Vec3f *>(&hit)));
                result->m_distance = sphere->m_radius;
                result->field_0x10 = 0.0f;
                result->field_0x34 = 0;
                result->field_0x2C = 0;
            }
            return true;
        }
    }
    return false;
}

bool RangeCylinder::HitBottom(const UnkSphere_8004E6CC *sphere, Vec3f *move, const Vec3f *start, const Vec3f *end,
                              const Vec3f *direction, UnkColliContact_8001FF00 *result) {
    float gap = -m_cylinder.m_halfHeight - (0.001f + (start->z + sphere->m_radius));
    float height = -m_cylinder.m_halfHeight;
    if (gap > 0.0f) {
        if (height < 0.001f + (end->z + sphere->m_radius)) {
            float t;
            if (direction->z != 0.0f) {
                t = gap / direction->z;
            } else {
                t = 0.0f;
            }
            Fn8040CopyVector3 hit(fn_8040CDD0(start, &fn_8040CDA8(direction, t)));
            float bound = 0.001f + m_cylinder.m_radius;
            bound = bound * bound;
            if (hit.x * hit.x + hit.y * hit.y > bound) {
                return false;
            }
            fn_8040CE38(move, t);
            if (result != 0) {
                result->SetNormal(fn_80016284(reinterpret_cast<const Vec3f *>(&m_axis)));
                hit.z += sphere->m_radius;
                result->SetPosition(fn_8040B67C(&m_matrix, reinterpret_cast<const Vec3f *>(&hit)));
                result->m_distance = sphere->m_radius;
                result->field_0x10 = 0.0f;
                result->field_0x34 = 0;
                result->field_0x2C = 0;
            }
            return true;
        }
    }
    return false;
}

bool RangeCylinder::vf_0x3C(const UnkSphere_8004E6CC *sphere, const Vec3f *move, UnkColliContact_8001FF00 *result) {
    float bound = 0.001f + (sphere->m_radius + m_margin);
    RangeLineData path;
    fn_80020A20(reinterpret_cast<UnkDebugLine_80020A20 *>(&path), &sphere->m_center,
                &fn_8040CDD0(&sphere->m_center, move));
    if (fn_8003C660(reinterpret_cast<const Vec3f *>(&m_position), reinterpret_cast<const UnkDebugLine_80020A20 *>(&path),
                    0) > bound * bound) {
        return false;
    }
    Fn8040CopyVector3 start(fn_8040B67C(&m_inverse, &sphere->m_center));
    Fn8040CopyVector3 direction(fn_8040B618(&m_inverse, move));
    Fn8040CopyVector3 end(fn_8040CDD0(reinterpret_cast<const Vec3f *>(&start), reinterpret_cast<const Vec3f *>(&direction)));
    float height = m_cylinder.m_halfHeight;
    if ((0.001f + (start.z + sphere->m_radius) < -height && 0.001f + (end.z + sphere->m_radius) < -height) ||
        (height < start.z - sphere->m_radius - 0.001f && height < end.z - sphere->m_radius - 0.001f)) {
        return false;
    }
    Vec2f start2(start.x, start.y);
    bound = 0.001f + (sphere->m_radius + m_cylinder.m_radius);
    Vec2f direction2(direction.x, direction.y);
    if (DistanceSq2D(&start2, &direction2) > bound * bound) {
        return false;
    }
    fn_8040CCD4(&start2);
    if (vf_0x38(sphere, result)) {
        fn_8040CE60(const_cast<Vec3f *>(move));
        return true;
    }
    bound = m_cylinder.m_radius * m_cylinder.m_radius;
    Vec2f from(start.x, start.y);
    if (fn_8040CCD4(&from) <= bound) {
        Vec2f to(end.x, end.y);
        if (fn_8040CCD4(&to) <= bound) {
            if (direction.z < 0.0f) {
                return HitTop(sphere, const_cast<Vec3f *>(move), reinterpret_cast<const Vec3f *>(&start),
                              reinterpret_cast<const Vec3f *>(&end), reinterpret_cast<const Vec3f *>(&direction), result);
            }
            return HitBottom(sphere, const_cast<Vec3f *>(move), reinterpret_cast<const Vec3f *>(&start),
                             reinterpret_cast<const Vec3f *>(&end), reinterpret_cast<const Vec3f *>(&direction), result);
        }
    }
    if (std::fabs(direction.x) < 0.0001f && std::fabs(direction.y) < 0.0001f) {
        if (start.z > 0.0f && direction.z < 0.0f) {
            return HitRim(sphere, const_cast<Vec3f *>(move), reinterpret_cast<const Vec3f *>(&start),
                          reinterpret_cast<const Vec3f *>(&direction), result, m_cylinder.m_halfHeight);
        }
        if (start.z < 0.0f && direction.z > 0.0f) {
            return HitRim(sphere, const_cast<Vec3f *>(move), reinterpret_cast<const Vec3f *>(&start),
                          reinterpret_cast<const Vec3f *>(&direction), result, -m_cylinder.m_halfHeight);
        }
    } else {
    bound = 0.001f + (sphere->m_radius + m_cylinder.m_radius);
    bound = bound * bound;
    Vec2f flat(start.x, start.y);
    if (fn_8040CCD4(&flat) >= bound) {
        float t0;
        float t1;
        if (fn_8003B7AC(&t0, &t1, direction.x * direction.x + direction.y * direction.y,
                        2.0f * (start.x * direction.x + start.y * direction.y),
                        start.x * start.x + start.y * start.y - bound)) {
            if (t0 > 0.0f || t1 > 0.0f) {
                float t;
                if (t0 > 0.0f && t1 > 0.0f) {
                    t = t0 < t1 ? t0 : t1;
                } else if (t0 > 0.0f) {
                    t = t0;
                } else {
                    t = t1;
                }
                if (t > 1.0f) {
                    return false;
                }
                Fn8040CopyVector3 hit(fn_8040CDD0(reinterpret_cast<const Vec3f *>(&start),
                                                  &fn_8040CDA8(reinterpret_cast<const Vec3f *>(&direction), t)));
                if (std::fabs(hit.z) <= HalfHeight()) {
                    fn_8040CE38(const_cast<Vec3f *>(move), t);
                    if (result != 0) {
                        Fn8040CopyVector3 normal(fn_8040CEE8(&MakeVector(hit.x, hit.y, 0.0f)));
                        Fn8040CopyVector3 rim(fn_8040CDA8(reinterpret_cast<const Vec3f *>(&normal), m_cylinder.m_radius));
                        rim.z = hit.z;
                        result->SetPosition(fn_8040B67C(&m_matrix, reinterpret_cast<const Vec3f *>(&rim)));
                        result->m_distance = sphere->m_radius;
                        result->field_0x10 = 0.0f;
                        result->SetNormal(fn_8040B618(&m_matrix, reinterpret_cast<const Vec3f *>(&normal)));
                        result->field_0x34 = 0;
                        result->field_0x2C = 0;
                    }
                    return true;
                }
                return HitCaps(sphere, const_cast<Vec3f *>(move), reinterpret_cast<const Vec3f *>(&start),
                               reinterpret_cast<const Vec3f *>(&end), reinterpret_cast<const Vec3f *>(&direction),
                               result);
            }
            return false;
        }
        return false;
    }
        return HitCaps(sphere, const_cast<Vec3f *>(move), reinterpret_cast<const Vec3f *>(&start),
                       reinterpret_cast<const Vec3f *>(&end), reinterpret_cast<const Vec3f *>(&direction), result);
    }
    return false;
}

bool RangeCylinder::HitCaps(const UnkSphere_8004E6CC *sphere, Vec3f *move, const Vec3f *start, const Vec3f *end,
                            const Vec3f *direction, UnkColliContact_8001FF00 *result) {
    float height = m_cylinder.m_halfHeight;
    float z = start->z;
    float low = 0.001f + (z + sphere->m_radius);
    if (height < low) {
        if (end->z - sphere->m_radius - 0.001f < height) {
            if (height < z - sphere->m_radius - 0.001f && direction->z < 0.0f) {
                if (HitTop(sphere, move, start, end, direction, result)) {
                    return true;
                }
            }
            return HitRim(sphere, move, start, direction, result, m_cylinder.m_halfHeight);
        }
    }
    height = -height;
    if (height > z - sphere->m_radius - 0.001f) {
        if (height < 0.001f + (end->z + sphere->m_radius)) {
            if (height > low && direction->z > 0.0f) {
                if (HitBottom(sphere, move, start, end, direction, result)) {
                    return true;
                }
            }
            return HitRim(sphere, move, start, direction, result, -m_cylinder.m_halfHeight);
        }
    }
    return false;
}

void RangeCylinder::Update(const Vec3f *position, const Vec3f *rotation, s32 order) {
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

void RangeCylinder::SetMatrix(Mtx34f *matrix) {
    fn_8040C148(&m_matrix, &fn_8001E044(matrix, &((TranslationFn)fn_8040B034)(
                                                    reinterpret_cast<const Vec3f *>(&GetExtent()))));
    fn_8001E070(&m_matrix, GetBaseRotation(), GetRotationOrder());
    m_position.x = *fn_8040C17C(&m_matrix, 0, 3);
    m_position.y = *fn_8040C17C(&m_matrix, 1, 3);
    m_position.z = *fn_8040C17C(&m_matrix, 2, 3);
    fn_8040C148(&m_inverse, &fn_8040BEF8(&m_matrix));
    UpdateLine();
}

void RangeCylinder::UpdateLine() {
    RangeAxis axis(0.0f, 0.0f, GetHalfHeight());
    axis = fn_8040B618(&m_matrix, reinterpret_cast<const Vec3f *>(&axis));
    AssignVector(m_line.m_start, ((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&m_position),
                                                                  reinterpret_cast<const Vec3f *>(&axis)));
    AssignVector(m_line.m_end, fn_8040CDD0(reinterpret_cast<const Vec3f *>(&m_position),
                                           reinterpret_cast<const Vec3f *>(&axis)));
    AssignVector(*reinterpret_cast<Vec3f *>(&m_axis), fn_8040CEE8(reinterpret_cast<const Vec3f *>(&axis)));
}

void RangeCylinder::UpdateBounds() {
    float r = m_cylinder.m_radius;
    float h = m_cylinder.m_halfHeight;
    m_margin = fn_8000FEE4(h * h + r * r);
}

void RangeCylinder::SetScale(const Vec3f *scale) {
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
    SetSize(scale->x * m_cylinder.m_radius, 0.0f, scale->z * m_cylinder.m_halfHeight);
}

void RangeCylinder::GetSize(float *x, float *y, float *z) {
    if (x != 0) {
        *x = m_cylinder.m_radius;
    }
    if (z != 0) {
        *z = m_cylinder.m_halfHeight;
    }
}

void RangeCylinder::SetSize(float x, float y, float z) {
    m_cylinder.m_radius = x;
    m_cylinder.m_halfHeight = z;
    UpdateBounds();
}

extern "C" bool fn_800325D8(RangeBase *self, RangeBase *other) {
    RangeCylinder *cylinder = static_cast<RangeCylinder *>(self);
    RangeSphere *sphere = static_cast<RangeSphere *>(other);
    if (fn_80029118(cylinder, sphere)) {
        RangeFloat4 local;
        fn_80021C04(reinterpret_cast<UnkSphere_8004E6CC *>(&local), reinterpret_cast<const Vec3f *>(&sphere->m_position),
                    sphere->GetRadius());
        return cylinder->vf_0x38(reinterpret_cast<const UnkSphere_8004E6CC *>(&local), 0);
    }
    return false;
}

extern "C" bool fn_80031EEC(RangeBase *self, RangeBase *other) {
    RangeCylinder *cylinder = static_cast<RangeCylinder *>(self);
    RangeBox *box = static_cast<RangeBox *>(other);
    if (fn_80029118(cylinder, box)) {
        Fn8040CopyVector3 t(fn_8040B618(&cylinder->m_inverse,
                                        &((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&box->m_position),
                                                                         reinterpret_cast<const Vec3f *>(&cylinder->m_position))));
        float angle;
        if (std::fabs(t.x) > 0.0001f) {
            angle = fn_80016260(t.y, t.x);
        } else {
            angle = 0.0f;
        }
        RangeMatrixCopy rotation(fn_8001E044(&fn_8040B0C0(-angle), &cylinder->m_inverse));
        AssignVector(*reinterpret_cast<Vec3f *>(&t),
                     fn_8040B618(&rotation.m, &((VectorSubtractFn)fn_8040CD74)(
                                                  reinterpret_cast<const Vec3f *>(&box->m_position),
                                                  reinterpret_cast<const Vec3f *>(&cylinder->m_position))));
        fn_8040BAC4(&rotation.m, box->GetMatrix());
        Vec3f c0(rotation.m.values[0][0], rotation.m.values[1][0], rotation.m.values[2][0]);
        Vec3f c1(rotation.m.values[0][1], rotation.m.values[1][1], rotation.m.values[2][1]);
        Vec3f c2(rotation.m.values[0][2], rotation.m.values[1][2], rotation.m.values[2][2]);
        Fn8040CopyVector3 hb(box->GetHalfSize());
        float h = cylinder->m_cylinder.m_halfHeight;
        float r = cylinder->m_cylinder.m_radius;
        float s;
        float c;

        if (std::fabs(t.x) > 0.001f + (r + hb.x * std::fabs(c0.x) + hb.y * std::fabs(c1.x) + hb.z * std::fabs(c2.x))) {
            return false;
        }
        if (std::fabs(t.z) > 0.001f + (h + hb.x * std::fabs(c0.z) + hb.y * std::fabs(c1.z) + hb.z * std::fabs(c2.z))) {
            return false;
        }

        SinCos2(&s, &c, c0.y, c0.x);
        if (std::fabs(Dot(t, c0)) >
            0.001f + (hb.x + r * (std::fabs(c * c0.x) + std::fabs(s * c0.y)) + h * std::fabs(c0.z))) {
            return false;
        }
        SinCos2(&s, &c, c1.y, c1.x);
        if (std::fabs(Dot(t, c1)) >
            0.001f + (hb.y + r * (std::fabs(c * c1.x) + std::fabs(s * c1.y)) + h * std::fabs(c1.z))) {
            return false;
        }
        SinCos2(&s, &c, c2.y, c2.x);
        if (std::fabs(Dot(t, c2)) >
            0.001f + (hb.z + r * (std::fabs(c * c2.x) + std::fabs(s * c2.y)) + h * std::fabs(c2.z))) {
            return false;
        }

        if (std::fabs(-t.z * c0.x + t.x * c0.z) >
            0.001f + (r * std::fabs(c0.z) + h * std::fabs(c0.x) + hb.y * std::fabs(c2.y) + hb.z * std::fabs(c1.y))) {
            return false;
        }
        if (std::fabs(-t.z * c1.x + t.x * c1.z) >
            0.001f + (r * std::fabs(c1.z) + h * std::fabs(c1.x) + hb.z * std::fabs(c0.y) + hb.x * std::fabs(c2.y))) {
            return false;
        }
        if (std::fabs(-t.z * c2.x + t.x * c2.z) >
            0.001f + (r * std::fabs(c2.z) + h * std::fabs(c2.x) + hb.x * std::fabs(c1.y) + hb.y * std::fabs(c0.y))) {
            return false;
        }

        SinCos2(&s, &c, c0.x, -c0.y);
        if (std::fabs(-t.x * c0.y + t.y * c0.x) >
            0.001f + (r * (std::fabs(c * c0.y) + std::fabs(s * c0.x)) + hb.y * std::fabs(c2.z) + hb.z * std::fabs(c1.z))) {
            return false;
        }
        SinCos2(&s, &c, c1.x, -c1.y);
        if (std::fabs(-t.x * c1.y + t.y * c1.x) >
            0.001f + (r * (std::fabs(c * c1.y) + std::fabs(s * c1.x)) + hb.z * std::fabs(c0.z) + hb.x * std::fabs(c2.z))) {
            return false;
        }
        SinCos2(&s, &c, c2.x, -c2.y);
        return !(std::fabs(-t.x * c2.y + t.y * c2.x) >
                 0.001f + (r * (std::fabs(c * c2.y) + std::fabs(s * c2.x)) + hb.x * std::fabs(c1.z) +
                           hb.y * std::fabs(c0.z)));
    }
    return false;
}

extern "C" bool fn_800315C4(RangeBase *self, RangeBase *other) {
    RangeCylinder *a = static_cast<RangeCylinder *>(self);
    RangeCapsule *b = static_cast<RangeCapsule *>(other);
    if (fn_80029118(a, b)) {
        Fn8040CopyVector3 t(fn_8040B618(&a->m_inverse,
                                        &((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&b->m_position),
                                                                         reinterpret_cast<const Vec3f *>(&a->m_position))));
        float angleA;
        if (std::fabs(t.x) > 0.0001f) {
            angleA = fn_80016260(t.y, t.x);
        } else {
            angleA = 0.0f;
        }
        Fn8040CopyVector3 u(fn_8040B618(&b->m_inverse,
                                        &((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&a->m_position),
                                                                         reinterpret_cast<const Vec3f *>(&b->m_position))));
        float angleB;
        if (std::fabs(u.x) > 0.0001f) {
            angleB = fn_80016260(u.y, u.x);
        } else {
            angleB = 0.0f;
        }
        RangeMatrixCopy rotation(fn_8001E044(&fn_8040B0C0(-angleA), &a->m_inverse));
        AssignVector(*reinterpret_cast<Vec3f *>(&t),
                     fn_8040B618(&rotation.m, &((VectorSubtractFn)fn_8040CD74)(
                                                  reinterpret_cast<const Vec3f *>(&b->m_position),
                                                  reinterpret_cast<const Vec3f *>(&a->m_position))));
        fn_8040BAC4(&rotation.m, &fn_8001E044(b->GetMatrix(), &fn_8040B0C0(angleB)));
        Vec3f c0(rotation.m.values[0][0], rotation.m.values[1][0], rotation.m.values[2][0]);
        Vec3f c1(rotation.m.values[0][1], rotation.m.values[1][1], rotation.m.values[2][1]);
        Vec3f c2(rotation.m.values[0][2], rotation.m.values[1][2], rotation.m.values[2][2]);
        float ha = a->m_cylinder.m_halfHeight;
        float ra = a->m_cylinder.m_radius;
        float hb = b->m_capsule.m_halfHeight;
        float rb = b->m_capsule.m_radius;
        float s;
        float c;

        if (std::fabs(t.x) > 0.001f + (ra + rb + hb * std::fabs(c2.x))) {
            return false;
        }
        if (std::fabs(t.z) > 0.001f + (ha + rb + hb * std::fabs(c2.z))) {
            return false;
        }
        SinCos2(&s, &c, c0.y, c0.x);
        if (std::fabs(Dot(t, c0)) > 0.001f + (rb + ra * (std::fabs(c * c0.x) + std::fabs(s * c0.y)) + ha * std::fabs(c0.z))) {
            return false;
        }
        SinCos2(&s, &c, c2.y, c2.x);
        if (std::fabs(Dot(t, c2)) >
            0.001f + (hb + rb + ra * (std::fabs(c * c2.x) + std::fabs(s * c2.y)) + ha * std::fabs(c2.z))) {
            return false;
        }
        {
            float k = fn_8000FEE4(c1.z * c1.z + c1.x * c1.x);
            if (std::fabs(-t.z * c1.x + t.x * c1.z) >
                0.001f + (ra * std::fabs(c1.z) + ha * std::fabs(c1.x) + hb * std::fabs(c0.y) + rb * k)) {
                return false;
            }
        }
        {
            float k = fn_8000FEE4(c2.z * c2.z + c2.x * c2.x);
            if (std::fabs(-t.z * c2.x + t.x * c2.z) > 0.001f + (ra * std::fabs(c2.z) + ha * std::fabs(c2.x) + rb * k)) {
                return false;
            }
        }
        {
            float k = fn_8000FEE4(c1.y * c1.y + c1.x * c1.x);
            SinCos2(&s, &c, c1.x, -c1.y);
            if (std::fabs(-t.x * c1.y + t.y * c1.x) >
                0.001f + (ra * (std::fabs(c * c1.y) + std::fabs(s * c1.x)) + hb * std::fabs(c0.z) + rb * k)) {
                return false;
            }
        }
        {
            float k = fn_8000FEE4(c2.y * c2.y + c2.x * c2.x);
            SinCos2(&s, &c, c2.x, -c2.y);
            if (std::fabs(-t.x * c2.y + t.y * c2.x) > 0.001f + (ra * (std::fabs(c * c2.y) + std::fabs(s * c2.x)) + rb * k)) {
                return false;
            }
        }

        AssignVector(*reinterpret_cast<Vec3f *>(&t),
                     ((VectorSubtractFn)fn_8040CD74)(&b->m_line.m_start, reinterpret_cast<const Vec3f *>(&a->m_position)));
        Fn8040CopyVector3 cap(fn_8040CDA8(reinterpret_cast<const Vec3f *>(&a->m_axis),
                                          fn_8040CF90(reinterpret_cast<const Vec3f *>(&t), reinterpret_cast<const Vec3f *>(&a->m_axis)) > 0.0f ? ha : -ha));
        Fn8040CopyVector3 side(fn_8040CFB8(reinterpret_cast<const Vec3f *>(&cap), reinterpret_cast<const Vec3f *>(&t)));
        if (std::fabs(side.x) < 0.001f && std::fabs(side.y) < 0.001f && std::fabs(side.z) < 0.001f) {
        }
        AssignVector(*reinterpret_cast<Vec3f *>(&side),
                     fn_8040CDA8(&fn_8040CEE8(&fn_8040CFB8(reinterpret_cast<const Vec3f *>(&side),
                                                           reinterpret_cast<const Vec3f *>(&cap))),
                                 a->m_cylinder.m_radius));
        Fn8040CopyVector3 rim(fn_8040CDD0(&fn_8040CDD0(reinterpret_cast<const Vec3f *>(&a->m_position),
                                                       reinterpret_cast<const Vec3f *>(&cap)),
                                          reinterpret_cast<const Vec3f *>(&side)));
        Fn8040CopyVector3 away(((VectorSubtractFn)fn_8040CD74)(&b->m_line.m_start, reinterpret_cast<const Vec3f *>(&rim)));
        if (fn_8040CF90(reinterpret_cast<const Vec3f *>(&away), reinterpret_cast<const Vec3f *>(&b->m_axis)) > -0.001f) {
            fn_8040CE74(reinterpret_cast<Vec3f *>(&away));
            float d0 = fn_8040CF90(reinterpret_cast<const Vec3f *>(&away), reinterpret_cast<const Vec3f *>(&cap));
            float d1 = fn_8040CF90(reinterpret_cast<const Vec3f *>(&away), reinterpret_cast<const Vec3f *>(&side));
            float d2 = fn_8040CF90(reinterpret_cast<const Vec3f *>(&away), reinterpret_cast<const Vec3f *>(&t));
            if (std::fabs(d2) > 0.001f + (rb + (std::fabs(d1) + std::fabs(d0)))) {
                return false;
            }
        }

        const Vec3f *end = &b->m_line.m_end;
        AssignVector(*reinterpret_cast<Vec3f *>(&t),
                     ((VectorSubtractFn)fn_8040CD74)(end, reinterpret_cast<const Vec3f *>(&a->m_position)));
        AssignVector(*reinterpret_cast<Vec3f *>(&cap),
                     fn_8040CDA8(reinterpret_cast<const Vec3f *>(&a->m_axis),
                                 fn_8040CF90(reinterpret_cast<const Vec3f *>(&t), reinterpret_cast<const Vec3f *>(&a->m_axis)) > 0.0f ? ha : -ha));
        AssignVector(*reinterpret_cast<Vec3f *>(&side),
                     fn_8040CFB8(reinterpret_cast<const Vec3f *>(&cap), reinterpret_cast<const Vec3f *>(&t)));
        if (std::fabs(side.x) < 0.001f && std::fabs(side.y) < 0.001f && std::fabs(side.z) < 0.001f) {
        }
        AssignVector(*reinterpret_cast<Vec3f *>(&side),
                     fn_8040CDA8(&fn_8040CEE8(&fn_8040CFB8(reinterpret_cast<const Vec3f *>(&side),
                                                           reinterpret_cast<const Vec3f *>(&cap))),
                                 ra));
        AssignVector(*reinterpret_cast<Vec3f *>(&rim),
                     fn_8040CDD0(&fn_8040CDD0(reinterpret_cast<const Vec3f *>(&a->m_position),
                                              reinterpret_cast<const Vec3f *>(&cap)),
                                 reinterpret_cast<const Vec3f *>(&side)));
        AssignVector(*reinterpret_cast<Vec3f *>(&away),
                     ((VectorSubtractFn)fn_8040CD74)(end, reinterpret_cast<const Vec3f *>(&rim)));
        if (fn_8040CF90(reinterpret_cast<const Vec3f *>(&away), reinterpret_cast<const Vec3f *>(&b->m_axis)) < 0.001f) {
            fn_8040CE74(reinterpret_cast<Vec3f *>(&away));
            float d0 = fn_8040CF90(reinterpret_cast<const Vec3f *>(&away), reinterpret_cast<const Vec3f *>(&cap));
            float d1 = fn_8040CF90(reinterpret_cast<const Vec3f *>(&away), reinterpret_cast<const Vec3f *>(&side));
            float d2 = fn_8040CF90(reinterpret_cast<const Vec3f *>(&away), reinterpret_cast<const Vec3f *>(&t));
            if (std::fabs(d2) > 0.001f + (rb + (std::fabs(d1) + std::fabs(d0)))) {
                return false;
            }
        }
        return true;
    }
    return false;
}

extern "C" bool fn_80030E08(RangeBase *self, RangeBase *other) {
    RangeCylinder *a = static_cast<RangeCylinder *>(self);
    RangeCylinder *b = static_cast<RangeCylinder *>(other);
    if (fn_80029118(a, b)) {
        Fn8040CopyVector3 t(fn_8040B618(&a->m_inverse,
                                        &((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&b->m_position),
                                                                         reinterpret_cast<const Vec3f *>(&a->m_position))));
        float angleA;
        if (std::fabs(t.x) > 0.0001f) {
            angleA = fn_80016260(t.y, t.x);
        } else {
            angleA = 0.0f;
        }
        Fn8040CopyVector3 u(fn_8040B618(&b->m_inverse,
                                        &((VectorSubtractFn)fn_8040CD74)(reinterpret_cast<const Vec3f *>(&a->m_position),
                                                                         reinterpret_cast<const Vec3f *>(&b->m_position))));
        float angleB;
        if (std::fabs(u.x) > 0.0001f) {
            angleB = fn_80016260(u.y, u.x);
        } else {
            angleB = 0.0f;
        }
        RangeMatrixCopy rotation(fn_8001E044(&fn_8040B0C0(-angleA), &a->m_inverse));
        AssignVector(*reinterpret_cast<Vec3f *>(&t),
                     fn_8040B618(&rotation.m, &((VectorSubtractFn)fn_8040CD74)(
                                                  reinterpret_cast<const Vec3f *>(&b->m_position),
                                                  reinterpret_cast<const Vec3f *>(&a->m_position))));
        fn_8040BAC4(&rotation.m, &fn_8001E044(b->GetMatrix(), &fn_8040B0C0(angleB)));
        Vec3f c0(rotation.m.values[0][0], rotation.m.values[1][0], rotation.m.values[2][0]);
        Vec3f c1(rotation.m.values[0][1], rotation.m.values[1][1], rotation.m.values[2][1]);
        Vec3f c2(rotation.m.values[0][2], rotation.m.values[1][2], rotation.m.values[2][2]);
        float ha = a->m_cylinder.m_halfHeight;
        float ra = a->m_cylinder.m_radius;
        float hb = b->m_cylinder.m_halfHeight;
        float rb = b->m_cylinder.m_radius;
        float s;
        float c;
        float s2;
        float c2v;

        SinCos2(&s, &c, c1.x, c0.x);
        if (std::fabs(t.x) > 0.001f + (ra + rb * (std::fabs(c * c0.x) + std::fabs(s * c1.x)) + hb * std::fabs(c2.x))) {
            return false;
        }
        SinCos2(&s, &c, c1.z, c0.z);
        if (std::fabs(t.z) > 0.001f + (ha + rb * (std::fabs(c * c0.z) + std::fabs(s * c1.z)) + hb * std::fabs(c2.z))) {
            return false;
        }
        SinCos2(&s2, &c2v, c0.y, c0.x);
        if (std::fabs(Dot(t, c0)) >
            0.001f + (rb + ra * (std::fabs(c2v * c0.x) + std::fabs(s2 * c0.y)) + ha * std::fabs(c0.z))) {
            return false;
        }
        SinCos2(&s2, &c2v, c2.y, c2.x);
        if (std::fabs(Dot(t, c2)) >
            0.001f + (hb + ra * (std::fabs(c2v * c2.x) + std::fabs(s2 * c2.y)) + ha * std::fabs(c2.z))) {
            return false;
        }
        c = 0.0f == c0.x * c1.z + c0.z * -c1.x ? 0.0f : 1.0f;
        if (std::fabs(-t.z * c1.x + t.x * c1.z) >
            0.001f + (ra * std::fabs(c1.z) + ha * std::fabs(c1.x) + hb * std::fabs(c0.y) + rb * std::fabs(c * c2.y))) {
            return false;
        }
        SinCos2(&s, &c, c1.x * c2.z + c1.z * -c2.x, c0.x * c2.z + c0.z * -c2.x);
        if (std::fabs(-t.z * c2.x + t.x * c2.z) >
            0.001f + (ra * std::fabs(c2.z) + ha * std::fabs(c2.x) + rb * (std::fabs(c * c1.y) + std::fabs(s * c0.y)))) {
            return false;
        }
        {
            float ny = -c1.y;
            SinCos2(&s2, &c2v, c1.x, ny);
            c = 0.0f == c0.x * ny + c0.y * c1.x ? 0.0f : 1.0f;
            if (std::fabs(-t.x * c1.y + t.y * c1.x) >
                0.001f + (ra * (std::fabs(c2v) * std::fabs(c1.y) + std::fabs(s2) * std::fabs(c1.x)) +
                          hb * std::fabs(c0.z) + rb * c * std::fabs(c2.z))) {
                return false;
            }
        }
        SinCos2(&s2, &c2v, c2.x, -c2.y);
        SinCos2(&s, &c, c1.x * -c2.y + c1.y * c2.x, c0.x * -c2.y + c0.y * c2.x);
        return !(std::fabs(-t.x * c2.y + t.y * c2.x) >
                 0.001f + (ra * (std::fabs(c2v) * std::fabs(c2.y) + std::fabs(s2) * std::fabs(c2.x)) +
                           rb * (std::fabs(c * c1.z) + std::fabs(s * c0.z))));
    }
    return false;
}

void RangeCylinder::Reset() {}

void RangeCylinder::SetBaseRotation(const Vec3f *rotation) {
    m_cylinder.m_rotation.x = rotation->x;
    m_cylinder.m_rotation.y = rotation->y;
    m_cylinder.m_rotation.z = rotation->z;
    UpdateRotationFlags();
    if (m_owner.lock()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), owner->vf_0x18(), owner->vf_0x24());
    }
}

void RangeCylinder::SetOffset(const Vec3f *offset) {
    m_cylinder.m_center = fn_8001DFE8(offset);
    UpdateRotationFlags();
    if (m_owner.lock()) {
        CharaColliBase *owner = m_owner.lock().get();
        Update(owner->vf_0x14(), owner->vf_0x18(), owner->vf_0x24());
    }
}

s32 RangeCylinder::GetRotationOrder() {
    return m_cylinder.field_0x20;
}

const Vec3f *RangeCylinder::GetBaseRotation() {
    return &m_cylinder.m_rotation;
}

Fn8040CopyVector3 RangeCylinder::GetExtent() {
    return ((VectorCopyFn)fn_800169F8)(&m_cylinder.m_center);
}

Vec3f RangeCylinder::GetRotation() {
    if (!m_owner.expired()) {
        boost::shared_ptr<CharaColliBase> owner(m_owner);
        owner->vf_0x7C();
        return fn_80029418(&m_matrix, owner->vf_0x24());
    }
    return fn_80029418(&m_matrix, m_cylinder.field_0x20);
}

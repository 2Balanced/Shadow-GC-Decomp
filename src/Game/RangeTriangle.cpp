/* SonicteamUSA::System::Range::RangeTriangle (class name and base from RTTI): a triangle of the
 * collision mesh (Game/RangeTriangle.h). Every virtual is empty or returns false; the unit holds
 * its constructors, its normal and corners, and the operator delete that gives a pooled triangle
 * back to its pool. Member names and meanings are inferred. Definitions are in reverse address
 * order (deferred inlining emits functions last-defined first). */
#define RANGE_UNIT
#include "Game/RangeTriangle.h"
#include "Game/CharaColli.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

using SonicteamUSA::System::CharaColli::CharaColliBase;
using SonicteamUSA::System::Range::RangeBase;
using SonicteamUSA::System::Range::RangeTriangle;

/* Assigns through the out-of-line vector assignment (fn_800091D0). */
inline void AssignVector(Fn8040CopyVector3 &vector, const Vec3f &value) {
    fn_800091D0(reinterpret_cast<Vec3f *>(&vector), &value);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);
typedef Vec3f (*VectorCopyFn)(const Vec3f *v);

/* The pool pooled triangles come from (struct name unknown): its free list and the number of
 * triangles in use (names inferred). */
struct UnkTrianglePool_80020054 {
    /* 0x0 */ u32 field_0x0;
    /* 0x4 */ RangeTriangle *m_free;
    /* 0x8 */ u32 field_0x8;
    /* 0xC */ s32 m_used;
};

extern "C" {
UnkTrianglePool_80020054 *fn_80020054(void);
void fn_80049B34(void *memory);
Vec3f fn_800169F8(const Vec3f *v);
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b); /* a x b */
Vec3f fn_8040CEE8(const Vec3f *v);                 /* v normalized */
}

extern const Vec3f lbl_805E07B8;
extern boost::shared_ptr<CharaColliBase> lbl_805EE0A0;

RangeTriangle::RangeTriangle() : RangeBase(-1, lbl_805E07A0, 0, 0, lbl_805EE0A0), m_prev(0), m_next(0) {
    m_hasNormal = 0;
}

RangeTriangle::RangeTriangle(const UnkColliPolygon_80036038 *polygon, u32 attributes)
    : RangeBase(-1, fn_800169F8(&polygon->m_position), attributes, 0, lbl_805EE0A0),
      m_normal(fn_800169F8(&polygon->m_normal)) {
    m_hasNormal = 1;
    for (int i = 0; i < 3; i++) {
        RangeVertex *vertex = m_vertices;
        vertex[i].x = polygon->m_vertices[i]->x;
        vertex[i].y = polygon->m_vertices[i]->y;
        vertex[i].z = polygon->m_vertices[i]->z;
    }
}

void RangeTriangle::operator delete(void *memory) {
    RangeTriangle *triangle = static_cast<RangeTriangle *>(memory);
    if (triangle != 0) {
        if (triangle->m_prev != 0) {
            triangle->m_prev->m_next = triangle->m_next;
        }
        if (triangle->m_next != 0) {
            triangle->m_next->m_prev = triangle->m_prev;
        }
        if (!triangle->m_unpooled) {
            UnkTrianglePool_80020054 *pool = fn_80020054();
            pool->m_free->m_prev = triangle;
            triangle->m_next = pool->m_free;
            triangle->m_prev = 0;
            pool->m_free = triangle;
            pool->m_used--;
        } else {
            fn_80049B34(triangle);
        }
    }
}

RangeTriangle::~RangeTriangle() {}

Vec3f RangeTriangle::GetVertex(int index) {
    return fn_800169F8(reinterpret_cast<const Vec3f *>(&m_vertices[index]));
}

const Vec3f *RangeTriangle::GetNormal() {
    if (!m_hasNormal) {
        Fn8040CopyVector3 edge1(((VectorSubtractFn)fn_8040CD74)(&GetVertex(1), &GetVertex(0)));
        Fn8040CopyVector3 edge2(((VectorSubtractFn)fn_8040CD74)(&GetVertex(2), &GetVertex(0)));
        AssignVector(m_normal, fn_8040CEE8(&fn_8040CFB8(reinterpret_cast<const Vec3f *>(&edge1),
                                                         reinterpret_cast<const Vec3f *>(&edge2))));
        m_hasNormal = 1;
    }
    return reinterpret_cast<const Vec3f *>(&m_normal);
}

bool RangeTriangle::vf_0x3C(const UnkSphere_8004E6CC *sphere, const Vec3f *move, UnkColliContact_8001FF00 *result) {
    return false;
}

bool RangeTriangle::vf_0x38(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result) {
    return false;
}

bool RangeTriangle::vf_0x34(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result) {
    return false;
}

bool RangeTriangle::vf_0x30(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result) {
    return false;
}

void RangeTriangle::vf_0x60() {}

bool RangeTriangle::vf_0x4C(RangeBase *other, UnkColliContact_8001FF00 *mine, UnkColliContact_8001FF00 *theirs) {
    return false;
}

bool RangeTriangle::vf_0x48(RangeBase *other) {
    return false;
}

void RangeTriangle::UpdateBounds() {}

void RangeTriangle::Update(const Vec3f *position, const Vec3f *rotation, s32 arg) {}

void RangeTriangle::SetMatrix(Mtx34f *matrix) {}

void RangeTriangle::SetSize(float x, float y, float z) {}

void RangeTriangle::SetScale(const Vec3f *scale) {}

void RangeTriangle::SetBaseRotation(const Vec3f *rotation) {}

void RangeTriangle::SetOffset(const Vec3f *offset) {}

void RangeTriangle::GetSize(float *x, float *y, float *z) {}

const Vec3f *RangeTriangle::GetBaseRotation() {
    return &lbl_805E07B8;
}

Fn8040CopyVector3 RangeTriangle::GetExtent() {
    return reinterpret_cast<const Fn8040CopyVector3 &>(lbl_805E07A0);
}

Vec3f RangeTriangle::GetRotation() {
    return lbl_805E07B8;
}

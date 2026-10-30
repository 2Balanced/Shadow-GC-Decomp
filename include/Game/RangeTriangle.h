#ifndef GAME_RANGETRIANGLE_H
#define GAME_RANGETRIANGLE_H

#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include <dolphin/types.h>

/* A polygon of the collision mesh a triangle is made from (struct name unknown): its normal, a
 * point on it, and its three corners. */
struct UnkColliPolygon_80036038 {
    /* 0x00 */ Vec3f m_normal;
    /* 0x0C */ Vec3f m_position;
    /* 0x18 */ u32 field_0x18;
    /* 0x1C */ const Vec3f *m_vertices[3];
};

/* A corner of a triangle: three floats with no constructor (the triangle constructs none). */
struct RangeVertex {
    float x;
    float y;
    float z;
};

namespace SonicteamUSA {
namespace System {
namespace Range {

/* RangeTriangle (class name from RTTI; Game/RangeTriangle.cpp): a triangle of the collision mesh,
 * shape kind -1. It takes part in no test of its own (every test returns false). Triangles live in
 * a pool's lists (m_prev, m_next); the class's operator delete gives a pooled one back. Member
 * names and meanings are inferred. */
class RangeTriangle : public RangeBase {
  public:
    RangeTriangle(const UnkColliPolygon_80036038 *polygon, u32 attributes);
    RangeTriangle();

    static void operator delete(void *memory);

    /* The triangle's normal, computed from its corners on first use, and corner index. */
    const Vec3f *GetNormal();
    Vec3f GetVertex(int index);

    virtual Vec3f GetRotation();
    virtual RANGE_EXTENT GetExtent();
    virtual const Vec3f *GetBaseRotation();
    virtual void GetSize(float *x, float *y, float *z);
    virtual void SetOffset(const Vec3f *offset);
    virtual void SetBaseRotation(const Vec3f *rotation);
    virtual void SetScale(const Vec3f *scale);
    virtual void SetSize(float x, float y, float z);
    virtual ~RangeTriangle();
    virtual bool vf_0x30(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x34(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x38(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x3C(const UnkSphere_8004E6CC *sphere, const Vec3f *move, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x48(RangeBase *other);
    virtual bool vf_0x4C(RangeBase *other, UnkColliContact_8001FF00 *mine, UnkColliContact_8001FF00 *theirs);
    virtual void Update(const Vec3f *position, const Vec3f *rotation, s32 arg);
    virtual void SetMatrix(Mtx34f *matrix);
    virtual void UpdateBounds();
    virtual void vf_0x60();

    /* 0x2C */ RangeTriangle *m_prev;
    /* 0x30 */ RangeTriangle *m_next;
    /* 0x34 */ u8 m_unpooled;
    /* 0x38 */ RangeVertex m_vertices[3];
    /* 0x5C */ Fn8040CopyVector3 m_normal;
    /* 0x68 */ u8 m_hasNormal;
};

} // namespace Range
} // namespace System
} // namespace SonicteamUSA

#endif

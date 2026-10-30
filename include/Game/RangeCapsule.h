#ifndef GAME_RANGECAPSULE_H
#define GAME_RANGECAPSULE_H

#include "Game/DebugDisp.h"
#include "Game/Fn8040CopyVector3.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

struct UnkColliShapeDesc_80037668;

namespace SonicteamUSA {
namespace System {
namespace Range {

/* RangeCapsule (class name from RTTI; Game/RangeCapsule.cpp): a capsule, its shape kind 2. m_capsule
 * is its description: centre offset, half height (along the capsule's own Z axis), radius, rotation
 * and the order of the rotation's Euler angles. m_matrix places it in the world and m_inverse is its
 * inverse; m_line is its axis segment in the world and m_axis that segment's direction. Member names
 * and meanings are inferred. */
class RangeCapsule : public RangeBase {
  public:
    RangeCapsule(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position, const Vec3f *rotation, s32 order,
                 boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> owner);

    virtual Vec3f GetRotation();
    virtual RANGE_EXTENT GetExtent();
    virtual const Vec3f *GetBaseRotation();
    virtual s32 GetRotationOrder();
    virtual void GetSize(float *x, float *y, float *z);
    virtual void SetOffset(const Vec3f *offset);
    virtual void SetBaseRotation(const Vec3f *rotation);
    virtual void SetScale(const Vec3f *scale);
    virtual void SetSize(float x, float y, float z);
    virtual ~RangeCapsule();
    virtual bool vf_0x30(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x34(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x38(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x3C(const UnkSphere_8004E6CC *sphere, const Vec3f *move, UnkColliContact_8001FF00 *result);
    virtual void Reset();
    virtual bool vf_0x48(RangeBase *other);
    virtual bool vf_0x4C(RangeBase *other, UnkColliContact_8001FF00 *mine, UnkColliContact_8001FF00 *theirs);
    virtual void Update(const Vec3f *position, const Vec3f *rotation, s32 arg);
    virtual void SetMatrix(Mtx34f *matrix);
    virtual void UpdateBounds();

#ifdef RANGE_UNIT
    /* The capsule's matrix (inline; kept out of line, it reads through another inline call). */
    Mtx34f *GetMatrix() {
        return MatrixOf(m_matrix);
    }
    static Mtx34f *MatrixOf(Mtx34f &matrix) {
        return &matrix;
    }
    /* The half height (kept out of line: it reads the description through another inline call). */
    float GetHalfHeight() {
        return HalfHeightOf(m_capsule);
    }
    static float HalfHeightOf(const UnkCapsule_80030A5C &capsule) {
        return capsule.m_halfHeight;
    }
    float GetRadius() const {
        return m_capsule.m_radius;
    }
    float HalfHeight() const {
        return m_capsule.m_halfHeight;
    }
    /* Recomputes m_line and m_axis from the matrix. */
    void UpdateLine();
    /* The tests against the round side: a segment, and a line (point is an end of the axis, direction
     * points out of that end). */
    bool HitEndSegment(const Vec3f *point, const Vec3f *direction, const UnkDebugLine_80020A20 *segment,
                       UnkColliContact_8001FF00 *result);
    bool HitEndLine(const Vec3f *point, const Vec3f *direction, const UnkDebugLine_80020A20 *line,
                    UnkColliContact_8001FF00 *result);
    /* The test against the straight side; outside is set when the segment starts inside the radius
     * but beyond the ends. */
    bool HitSide(const UnkDebugLine_80020A20 *segment, bool *outside, UnkColliContact_8001FF00 *result);
#endif

    /* 0x2C */ UnkCapsule_80030A5C m_capsule;
    /* 0x50 */ Mtx34f m_matrix;
    /* 0x80 */ Mtx34f m_inverse;
    /* 0xB0 */ Sonicteam::System::Geometry::Line m_line;
    /* 0xC8 */ Fn8040CopyVector3 m_axis;
};

} // namespace Range
} // namespace System
} // namespace SonicteamUSA

#endif

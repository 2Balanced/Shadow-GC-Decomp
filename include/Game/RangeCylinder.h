#ifndef GAME_RANGECYLINDER_H
#define GAME_RANGECYLINDER_H

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

/* RangeCylinder (class name from RTTI; Game/RangeCylinder.cpp): a cylinder, its shape kind 3. m_cylinder
 * is its description: centre offset, half height (along the cylinder's own Z axis), radius, rotation
 * and the order of the rotation's Euler angles. m_matrix places it in the world and m_inverse is its
 * inverse; m_line is its axis segment in the world and m_axis that segment's direction. Member names
 * and meanings are inferred. */
class RangeCylinder : public RangeBase {
  public:
    RangeCylinder(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position, const Vec3f *rotation, s32 order,
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
    virtual ~RangeCylinder();
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
    /* The cylinder's matrix and half height (inline; kept out of line, they read through another
     * inline call). */
    Mtx34f *GetMatrix() {
        return MatrixOf(m_matrix);
    }
    static Mtx34f *MatrixOf(Mtx34f &matrix) {
        return &matrix;
    }
    float GetHalfHeight() {
        return HalfHeightOf(m_cylinder);
    }
    static float HalfHeightOf(const UnkCylinder_80034854 &cylinder) {
        return cylinder.m_halfHeight;
    }
    float HalfHeight() const {
        return m_cylinder.m_halfHeight;
    }
    /* Recomputes m_line and m_axis from the matrix. */
    void UpdateLine();
    /* A line or segment against the cylinder (the ends are its flat caps), and against its round
     * side in its own space. */
    bool HitLine(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result);
    bool HitSide(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result);
    /* A moving sphere (start and end of its path and the path's direction, in the cylinder's space)
     * against the cylinder's top cap, bottom cap, the rim at height, and the whole cylinder once the
     * sphere is known to pass within reach. */
    bool HitTop(const UnkSphere_8004E6CC *sphere, Vec3f *move, const Vec3f *start, const Vec3f *end,
                const Vec3f *direction, UnkColliContact_8001FF00 *result);
    bool HitBottom(const UnkSphere_8004E6CC *sphere, Vec3f *move, const Vec3f *start, const Vec3f *end,
                   const Vec3f *direction, UnkColliContact_8001FF00 *result);
    bool HitRim(const UnkSphere_8004E6CC *sphere, Vec3f *move, const Vec3f *start, const Vec3f *direction,
                UnkColliContact_8001FF00 *result, float height);
    bool HitCaps(const UnkSphere_8004E6CC *sphere, Vec3f *move, const Vec3f *start, const Vec3f *end,
                 const Vec3f *direction, UnkColliContact_8001FF00 *result);
#endif

    /* 0x2C */ UnkCylinder_80034854 m_cylinder;
    /* 0x50 */ Mtx34f m_matrix;
    /* 0x80 */ Mtx34f m_inverse;
    /* 0xB0 */ Sonicteam::System::Geometry::Line m_line;
    /* 0xC8 */ Fn8040CopyVector3 m_axis;
};

} // namespace Range
} // namespace System
} // namespace SonicteamUSA

#endif

#ifndef GAME_RANGEBOX_H
#define GAME_RANGEBOX_H

#include "Game/DebugDisp.h"
#include "Game/GeometryLine.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

struct UnkColliShapeDesc_80037668;

namespace SonicteamUSA {
namespace System {
namespace Range {

/* RangeBox (class name from RTTI; Game/RangeBox.cpp): an oriented box, its shape kind 1. m_box is
 * its description: centre offset, half size, rotation and the order of the rotation's Euler
 * angles; m_matrix places the box in the world and m_inverse is its inverse. Member names and
 * meanings are inferred. */
class RangeBox : public RangeBase {
  public:
    RangeBox(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position, const Vec3f *rotation, s32 order,
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
    virtual ~RangeBox();
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
    /* One of the box's twelve edges, and one of its eight corners (slots 0x60, 0x64). */
    virtual UnkDebugLine_80020A20 GetEdge(int index);
    virtual RANGE_EXTENT GetCorner(int index);

#ifdef RANGE_UNIT
    /* The box's matrix (inline; the game's copy is out of line, first emitted by Game/RangeBox.cpp). */
    Mtx34f *GetMatrix() {
        return MatrixOf(m_matrix);
    }
    static Mtx34f *MatrixOf(Mtx34f &matrix) {
        return &matrix;
    }
    /* Half the box's size, and setting it. */
    Fn8040CopyVector3 GetHalfSize();
    void SetHalfSize(const Vec3f *half);
    /* value clamped to [-half, half]; side says where it was (0 below, 1 inside, 2 above). */
    float Clamp(float value, float half, s32 *side);
    /* The normal of the face nearest point (box space). */
    Vec3f FaceNormal(const Vec3f *point, const Vec3f *half);
    /* The tests in box space: a sphere, and a segment. */
    bool HitSphere(const Vec3f *center, float radius, UnkColliContact_8001FF00 *result);
    bool HitSegment(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result);
#endif

    /* 0x2C */ UnkBox_8002DBC8 m_box;
    /* 0x54 */ Mtx34f m_matrix;
    /* 0x84 */ Mtx34f m_inverse;
};

} // namespace Range
} // namespace System
} // namespace SonicteamUSA

#endif

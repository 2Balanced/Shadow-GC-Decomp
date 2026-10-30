#ifndef GAME_RANGESPHERE_H
#define GAME_RANGESPHERE_H

#include "Game/BossDef.h"
#include "Game/MathTypes.h"
#include "Game/RangeBase.h"
#include "boost/weak_ptr.hpp"
#include <dolphin/types.h>

struct UnkColliShapeDesc_80037668;

/* The sphere's description as the shape keeps it: UnkSphere_80035CEC's layout as plain data (the
 * game copies its centre word by word, its radius as a float). */
struct RangeSphereData {
    /* 0x0 */ float m_center[3];
    /* 0xC */ float m_radius;
};

namespace SonicteamUSA {
namespace System {
namespace Range {

/* RangeSphere (class name from RTTI; Game/RangeSphere.cpp): a sphere, its shape kind 0. m_sphere is
 * its description: centre offset and radius. GetRadius is inline: Game/RangeBox.cpp holds the
 * game's copy (0x8002A6FC). Member names and meanings are inferred. */
class RangeSphere : public RangeBase {
  public:
    RangeSphere(const UnkColliShapeDesc_80037668 *desc, const Vec3f *position,
                boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> owner);

    virtual Vec3f GetRotation();
    virtual RANGE_EXTENT GetExtent();
    virtual const Vec3f *GetBaseRotation();
    virtual void GetSize(float *x, float *y, float *z);
    virtual void SetOffset(const Vec3f *offset);
    virtual void SetBaseRotation(const Vec3f *rotation);
    virtual void SetScale(const Vec3f *scale);
    virtual void SetSize(float x, float y, float z);
    virtual ~RangeSphere();
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
    /* The sphere's radius (slots 0x60, 0x64). */
    virtual float GetRadius() {
        return m_sphere.m_radius;
    }
    virtual void SetRadius(float radius);

    /* 0x2C */ RangeSphereData m_sphere;
};

} // namespace Range
} // namespace System
} // namespace SonicteamUSA

#endif

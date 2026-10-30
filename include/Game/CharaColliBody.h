#ifndef GAME_CHARACOLLIBODY_H
#define GAME_CHARACOLLIBODY_H

#ifndef CHARACOLLI_BASE_MEMBERS
#define CHARACOLLI_BASE_MEMBERS
#endif

#include "Game/CharaColli.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include "dolphin/types.h"
#include "size_assert.h"

class CharInfo;
class UnkColliObject_80028CA4;

/* The description of a shape that fn_80037668 makes (struct name unknown; its first word selects
 * the kind). */
struct UnkColliShapeDesc_80037668;

namespace SonicteamUSA {
namespace System {
namespace CharaColli {

/* CharaColliBody (class name from RTTI): a collider of one shape at a position and rotation of
 * its own. Moves are applied lazily (m_moved; slot 0x8C). Names inferred. */
class CharaColliBody : public CharaColliBaseCharInfo {
  public:
    CharaColliBody(const UnkColliShapeDesc_80037668 *shape, const Vec3f *position, const Vec3f *rotation,
                   s32 arg, u16 type, u16 typeArg, boost::weak_ptr<CharInfo> info);

    virtual u16 GetShapeCount();
    virtual Vec3f *vf_0x14();
    virtual Vec3f *vf_0x18();
    virtual s32 vf_0x24();
    virtual UnkColliObject_80028CA4 *vf_0x28(int index) const;
    virtual UnkColliObject_80028CA4 *vf_0x2C(u32 id) const;
    virtual u32 GetAttributes(int arg) const;
    virtual u32 vf_0x34(u32 id) const;
    virtual void SetCharInfo(CharInfo *info);
    virtual void SetRotation(const Vec3f *rotation);
    virtual void SetScale(const Vec3f *scale);
    virtual void SetDebugColor(const UnkColor_8040D2D8 *color);
    virtual void vf_0x54(const boost::weak_ptr<CharaColliBase> &owner);
    virtual void SetAttributes(u32 attributes, int arg1);
    virtual void vf_0x5C(u32 attributes, u32 id);
    virtual UnkColliObject_80028CA4 *vf_0x60(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result,
                                             Filter filter, u32 arg);
    virtual UnkColliObject_80028CA4 *vf_0x64(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result,
                                             Filter filter, u32 arg);
    virtual UnkColliObject_80028CA4 *vf_0x68(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result,
                                             Filter filter, u32 arg);
    virtual UnkColliObject_80028CA4 *vf_0x6C(const UnkSphere_8004E6CC *sphere, const Vec3f *move,
                                             UnkColliContact_8001FF00 *result, Filter filter, u32 arg);
    virtual void Reset();
    virtual void vf_0x74();
    virtual ~CharaColliBody();
    virtual void vf_0x7C();
    virtual bool vf_0x80(CharaColliBase *other, UnkColliObject_80028CA4 **mine, UnkColliObject_80028CA4 **theirs,
                         UnkColliHit_8002686C **mineHit, UnkColliHit_8002686C **theirHit);
    virtual void vf_0x84();
    virtual void vf_0x88();
    virtual void vf_0x8C();

    /* 0x40 */ Vec3f m_position;
    /* 0x4C */ Vec3f m_rotation;
    /* 0x58 */ s32 field_0x58;
    /* 0x5C */ UnkColliObject_80028CA4 *m_shape;
};
SIZE_ASSERT(CharaColliBody, 0x60);

/* CharaColliBodyCharInfo (class name from RTTI): a CharaColliBody that follows its character's
 * position, rotation and word 0x30 (slot 0x88). Names inferred. */
class CharaColliBodyCharInfo : public CharaColliBody {
  public:
    CharaColliBodyCharInfo(boost::weak_ptr<CharInfo> info, const UnkColliShapeDesc_80037668 *shape, u16 type,
                           u16 typeArg);

    virtual void SetCharInfo(CharInfo *info);
    virtual void SetRotation(const Vec3f *rotation);
    virtual ~CharaColliBodyCharInfo();
    virtual void vf_0x88();
};

} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

#endif

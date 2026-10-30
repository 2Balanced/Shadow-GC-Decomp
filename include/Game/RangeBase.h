#ifndef GAME_RANGEBASE_H
#define GAME_RANGEBASE_H

#include "Game/CharaColli.h"
#ifdef RANGE_UNIT
#include "Game/Fn8040CopyVector3.h"
/* The shapes' own units see the vector GetExtent returns as the game's vector class, whose copy
 * constructor is out of line (fn_800091EC); other units see a Vec3f (same layout and calls). */
#define RANGE_EXTENT Fn8040CopyVector3
#else
#define RANGE_EXTENT Vec3f
#endif
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include "dolphin/types.h"

/* SonicteamUSA::System::Range::RangeBase (class name from RTTI): a collision shape of a collider,
 * the base of RangeBox, RangeCapsule, RangeCylinder, RangePlane, RangeSphere and RangeTriangle
 * (each its own unit, Game/RangeBase.cpp, Game/RangeBox.cpp, ...). A collider owns its shapes;
 * fn_80037668 makes one. Names and meanings are inferred. */
namespace SonicteamUSA {
namespace System {
namespace Range {

class RangeBase {
  public:
#ifdef RANGE_UNIT
    RangeBase(s32 priority, const Vec3f &position, u32 attributes, u32 id,
              boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> owner);
#endif

    /* Retail keeps it out of line: the weak_ptr assignment is an inline call of its own. */
    void SetOwner(boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> owner) {
        m_owner = owner;
    }

    float GetMargin() const {
        return m_margin;
    }

    /* Sets the shape's colour in the debug view (empty in this build: the colliders' callers keep
     * only their own calls). */
    void SetDebugColor(const UnkColor_8040D2D8 *color) {}

    /* The shape's rotation as Euler angles (slot 0x08). */
    virtual Vec3f GetRotation() = 0;
    /* Half the shape's size along each axis (slot 0x0C). */
    virtual RANGE_EXTENT GetExtent() = 0;
    /* The rotation the shape was given, and the order of its Euler angles (slots 0x10, 0x14). */
    virtual const Vec3f *GetBaseRotation() = 0;
    virtual s32 GetRotationOrder();
    /* The shape's size through the three pointers (slot 0x18; CharaColliBase::vf_0x1C). */
    virtual void GetSize(float *x, float *y, float *z) = 0;
    /* Sets the shape's offset from its owner and moves it there (slot 0x1C), and sets its
     * rotation (slot 0x20). */
    virtual void SetOffset(const Vec3f *offset) = 0;
    virtual void SetBaseRotation(const Vec3f *rotation) = 0;
    virtual void SetScale(const Vec3f *scale) = 0;
    virtual void SetSize(float x, float y, float z) = 0;
    virtual ~RangeBase() = 0;
    /* The tests behind CharaColliBase's slots 0x60 to 0x6C. */
    virtual bool vf_0x30(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result) = 0;
    virtual bool vf_0x34(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result) = 0;
    virtual bool vf_0x38(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result) = 0;
    virtual bool vf_0x3C(const UnkSphere_8004E6CC *sphere, const Vec3f *move, UnkColliContact_8001FF00 *result) = 0;
    virtual void Reset();
    virtual void DebugDraw();
    /* Whether the shape touches other, and the same with the contacts on either side. */
    virtual bool vf_0x48(RangeBase *other) = 0;
    virtual bool vf_0x4C(RangeBase *other, UnkColliContact_8001FF00 *mine,
                         UnkColliContact_8001FF00 *theirs) = 0;
    /* Moves the shape to position and rotation. */
    virtual void Update(const Vec3f *position, const Vec3f *rotation, s32 arg) = 0;
    /* Makes the shape follow a bone's matrix (slot 0x54). */
    virtual void SetMatrix(Mtx34f *matrix) = 0;
    /* Recomputes the shape's bounds (slot 0x58), and its rotation flags (slot 0x5C). */
    virtual void UpdateBounds() = 0;
    virtual void UpdateRotationFlags();

#ifdef RANGE_UNIT
    /* Retail copies the position through the out-of-line vector copy (same layout as Vec3f). */
    /* 0x04 */ Fn8040CopyVector3 m_position;
#else
    /* 0x04 */ Vec3f m_position;
#endif
    /* 0x10 */ float m_margin;
    /* 0x14 */ s32 m_priority;
    /* 0x18 */ u8 field_0x18;
    /* 0x19 */ u8 m_enabled;
    /* 0x1C */ u32 m_attributes;
    /* 0x20 */ u32 m_id;
    /* 0x24 */ boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_owner;
};

} // namespace Range
} // namespace System
} // namespace SonicteamUSA

/* What a collider's filter is asked with (struct name unknown): the shape and collider about to be
 * tested, the shape's attributes and the query's argument. */
struct UnkColliQuery_800C0D20 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ SonicteamUSA::System::Range::RangeBase *m_shape;
    /* 0x08 */ SonicteamUSA::System::CharaColli::CharaColliBase *m_collider;
    /* 0x0C */ u32 m_attributes;
    /* 0x10 */ u32 m_arg;
};

/* A vector assigned through the out-of-line assignment (fn_800091D0); the compiler's assignment
 * of a struct holding one calls it. */
struct UnkAssignVector3_800091D0 {
    UnkAssignVector3_800091D0 &operator=(const UnkAssignVector3_800091D0 &v) {
        fn_800091D0(reinterpret_cast<Vec3f *>(this), reinterpret_cast<const Vec3f *>(&v));
        return *this;
    }

    float x;
    float y;
    float z;
};

/* A contact two shapes report (0x38 bytes; struct name unknown): fn_8001FF00 constructs it; the
 * word at 0xC is how far along the query the contact is (meanings inferred). */
struct UnkColliContact_8001FF00 {
    /* The shapes' setters and getter (names inferred). Each calls another inline function, so GC/2.7
     * keeps them out of line: Game/RangeBox.cpp emits the game's copies. */
    void SetPosition(const Vec3f &position) {
        m_position = reinterpret_cast<const UnkAssignVector3_800091D0 &>(position);
    }
    void SetNormal(const Vec3f &normal) {
        m_normal = reinterpret_cast<const UnkAssignVector3_800091D0 &>(normal);
    }
    void SetEdgeNormal(const Vec3f &normal) {
        field_0x20 = reinterpret_cast<const UnkAssignVector3_800091D0 &>(normal);
    }
    const float &Distance() const {
        return m_distance;
    }
    float GetDistance() const {
        return Distance();
    }

    /* 0x00 */ UnkAssignVector3_800091D0 m_position;
    /* 0x0C */ float m_distance;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ UnkAssignVector3_800091D0 m_normal;
    /* 0x20 */ UnkAssignVector3_800091D0 field_0x20;
    /* 0x2C */ s32 field_0x2C;
    /* 0x30 */ s32 field_0x30;
    /* 0x34 */ s32 field_0x34;
};

/* A contact as a collider hands it out (0x48 bytes; struct name unknown): allocated from a pool
 * by fn_8001FC88 and built by fn_8002686C from the shape hit and its contact. */
struct UnkColliHit_8002686C {
    /* 0x00 */ u8 field_0x0[0x48];
};

extern "C" {
void fn_8001FF00(UnkColliContact_8001FF00 *contact);
void *fn_8001FC88(u32 size);
UnkColliHit_8002686C *fn_8002686C(UnkColliHit_8002686C *hit, SonicteamUSA::System::Range::RangeBase *shape,
                                  const UnkColliContact_8001FF00 *contact);
}

#endif

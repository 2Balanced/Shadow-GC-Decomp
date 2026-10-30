#ifndef GAME_UNKCOLLIOBJECT_80028CA4_H
#define GAME_UNKCOLLIOBJECT_80028CA4_H

#include "Game/CharaColli.h"
#include "Game/MathTypes.h"
#include "boost/weak_ptr.hpp"
#include "dolphin/types.h"

/* A collision shape of a collider (Game/VehicleColliMove.cpp and Game/PlayerCharaColliMove.cpp
 * know it by this placeholder; its vtables' RTTI name the base class
 * SonicteamUSA::System::Range::RangeBase, with RangeBox, RangeCapsule, RangeCylinder, RangePlane,
 * RangeSphere and RangeTriangle deriving from it). A collider owns its shapes; fn_80037668 makes
 * one. Only the members and slots the colliders use are mapped (names and meanings inferred). */
class UnkColliObject_80028CA4 {
  public:
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

    virtual Vec3f GetRotation();
    /* Half the shape's size along each axis (slot 0x0C). */
    virtual Vec3f GetExtent();
    virtual void vf_0x10();
    virtual void vf_0x14();
    virtual void vf_0x18();
    virtual void vf_0x1C();
    virtual void vf_0x20();
    virtual void SetScale(const Vec3f *scale);
    virtual void vf_0x28();
    virtual ~UnkColliObject_80028CA4();
    /* The tests behind CharaColliBase's slots 0x60 to 0x6C. */
    virtual bool vf_0x30(const UnkDebugLine_80020A20 *line, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x34(const UnkDebugLine_80020A20 *segment, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x38(const UnkSphere_8004E6CC *sphere, UnkColliContact_8001FF00 *result);
    virtual bool vf_0x3C(const UnkSphere_8004E6CC *sphere, const Vec3f *move, UnkColliContact_8001FF00 *result);
    virtual void Reset();
    virtual void DebugDraw();
    /* Whether the shape touches other, and the same with the contacts on either side. */
    virtual bool vf_0x48(UnkColliObject_80028CA4 *other);
    virtual bool vf_0x4C(UnkColliObject_80028CA4 *other, UnkColliContact_8001FF00 *mine,
                         UnkColliContact_8001FF00 *theirs);
    /* Moves the shape to position and rotation. */
    virtual void Update(const Vec3f *position, const Vec3f *rotation, s32 arg);
    /* Makes the shape follow a bone's matrix (slot 0x54). */
    virtual void SetMatrix(Mtx34f *matrix);

    /* 0x04 */ Vec3f m_position;
    /* 0x10 */ float m_margin;
    /* 0x14 */ s32 m_priority;
    /* 0x18 */ u8 field_0x18;
    /* 0x19 */ u8 m_enabled;
    /* 0x1C */ u32 m_attributes;
    /* 0x20 */ u32 m_id;
    /* 0x24 */ boost::weak_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_owner;
};

/* What a collider's filter is asked with (struct name unknown): the shape and collider about to be
 * tested, the shape's attributes and the query's argument. */
struct UnkColliQuery_800C0D20 {
    /* 0x00 */ u32 field_0x0;
    /* 0x04 */ UnkColliObject_80028CA4 *m_shape;
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
UnkColliHit_8002686C *fn_8002686C(UnkColliHit_8002686C *hit, UnkColliObject_80028CA4 *shape,
                                  const UnkColliContact_8001FF00 *contact);
}

#endif

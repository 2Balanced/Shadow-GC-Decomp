#ifndef GAME_FLYINGOBJECT_H
#define GAME_FLYINGOBJECT_H

/* AttackObject and FlyingObject (class names from RTTI): an object that hurts what it touches
 * (AttackObject, vtable 0x80539F54, code around 0x80198D58..0x801997F0) and one that also flies
 * (FlyingObject, vtable 0x80539ECC). Enemies fire them, e.g. the Black Arms wing's air cutter
 * (Game/BkWingLarge.cpp). Slot names are vf_0x<offset> after their offset in the vtable; member
 * names are inferred. Their code is in Game/AttackObject.cpp. */

#include "Game/Fn8040CopyVector3.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include <dolphin/types.h>
#include "size_assert.h"

namespace SonicteamUSA {
namespace System {
namespace CharaColli {
class CharaColliBase;
struct AttackCallbackParam;
} // namespace CharaColli
} // namespace System
} // namespace SonicteamUSA

/* A rotation (struct name unknown): unlike Vec3f it is copied word by word. */
struct UnkRotation {
    UnkRotation() : x(0.0f), y(0.0f), z(0.0f) {}
    UnkRotation(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    float x;
    float y;
    float z;
};

/* What an attack object is made from (struct name unknown; AttackObject keeps a copy at 0x34).
 * The meanings of the numbers are unknown. */
struct AttackObjectParam {
    AttackObjectParam(float radius, s32 arg1, s32 arg2, s32 arg3, float arg4, s32 arg5, s32 arg6,
                      s32 arg7, const Vec3f &position, UnkRotation rotation) {
        m_radius = radius;
        field_0x4 = arg1;
        field_0x8 = arg2;
        field_0xC = arg3;
        field_0x10 = arg4;
        field_0x14 = arg5;
        field_0x18 = arg6;
        field_0x1C = arg7;
        fn_800091D0(reinterpret_cast<Vec3f *>(&m_position), &position);
        m_rotation = rotation;
    }

    /* 0x00 */ float m_radius;
    /* 0x04 */ u32 field_0x4; /* a resource table (see BkWingLargeAirCutter::Create) */
    /* 0x08 */ s32 field_0x8;
    /* 0x0C */ s32 field_0xC;
    /* 0x10 */ float field_0x10;
    /* 0x14 */ s32 field_0x14;
    /* 0x18 */ s32 field_0x18;
    /* 0x1C */ s32 field_0x1C;
    /* 0x20 */ Fn8040CopyVector3 m_position; /* copied out of line by the implicit copy (0x801997F0) */
    /* 0x2C */ UnkRotation m_rotation;
};
SIZE_ASSERT(AttackObjectParam, 0x38);

/* AttackObject (class name from RTTI): it lives until m_time passes the parameters' m_radius
 * (a time limit, in seconds, by its use), and its collider's attack collision calls back slots
 * 0x14, 0x18 and 0x1C (meanings inferred). */
class AttackObject {
  public:
    AttackObject(const AttackObjectParam *param); /* 0x801992A8 */

    virtual ~AttackObject(); /* 0x80198F64 */
    /* Ends the object (name inferred). */
    virtual void vf_0x0C() {
        m_ended = 1;
    }
    /* Whether the object has ended (name inferred). */
    virtual u8 vf_0x10() {
        return m_ended;
    }
    /* The attack collision's callbacks (bound in the constructor). */
    virtual void vf_0x14(SonicteamUSA::System::CharaColli::AttackCallbackParam &param); /* 0x80198EB8 */
    virtual void vf_0x18(SonicteamUSA::System::CharaColli::AttackCallbackParam &param); /* 0x80198EB4 */
    virtual void vf_0x1C(SonicteamUSA::System::CharaColli::AttackCallbackParam &param); /* 0x80198E80 */
    /* The hit check (behaviour inferred). */
    virtual bool vf_0x20(); /* 0x80198DD8 */
    /* Counts the time and ends the object when it is up (behaviour inferred). */
    virtual void vf_0x24(); /* 0x80198D84 */
    virtual void vf_0x28(); /* 0x80198D58 */
    virtual void vf_0x2C(SonicteamUSA::System::CharaColli::AttackCallbackParam &param) {
        vf_0x28();
    }
    virtual void vf_0x30(SonicteamUSA::System::CharaColli::AttackCallbackParam &param) {
        vf_0x28();
    }
    /* Called by vf_0x20 with what the hit check found (argument types unknown). */
    virtual bool vf_0x34(u32 arg0, void *arg1) {
        vf_0x28();
        return true;
    }

    /* Removes the collider from the collision manager (fn_80198F24; name inferred). */
    void ReleaseCollider();

    /* 0x04 */ Vec3f m_position;
    /* 0x10 */ Vec3f field_0x10; /* the position before the last move */
    /* 0x1C */ UnkRotation m_rotation;
    /* 0x28 */ UnkRotation field_0x28;
    /* 0x34 */ AttackObjectParam m_param;
    /* 0x6C */ float m_time;
    /* 0x70 */ u8 m_ended;
    /* 0x71 */ u8 field_0x71;
    /* 0x72 */ u8 field_0x72;
    /* 0x74 */ s32 field_0x74;
    /* 0x78 */ boost::shared_ptr<SonicteamUSA::System::CharaColli::CharaColliBase> m_collider;
};
SIZE_ASSERT(AttackObject, 0x80);

/* How a flying object moves (struct name unknown; the meanings of the fields are unknown). */
struct FlyingObjectMotion {
    /* 0x00 */ float m_speed;
    /* 0x04 */ u32 field_0x4;
    /* 0x08 */ u32 field_0x8;
    /* 0x0C */ float field_0xC; /* a fall speed */
    /* 0x10 */ s32 field_0x10; /* bit 0: check for hits while flying */
    /* 0x14 */ s32 field_0x14;
};

/* FlyingObject (class name from RTTI). Its constructor and destructor are inline (first emitted
 * with Game/BkWingLarge.cpp). */
class FlyingObject : public AttackObject {
  public:
    FlyingObject(const FlyingObjectMotion *motion, const AttackObjectParam *param)
        : AttackObject(param), field_0x80(0.0f, 0.0f, 0.0f), m_motion(*motion) {}

    virtual ~FlyingObject() {}
    /* Moves the object (behaviour inferred). */
    virtual void vf_0x38(float dt); /* 0x80198BE0 */
    virtual void vf_0x3C() {}
    virtual float vf_0x40() {
        return m_motion.m_speed;
    }

    /* 0x80 */ Vec3f field_0x80;
    /* 0x8C */ FlyingObjectMotion m_motion;
    /* 0xA4 */ u32 field_0xA4;
};
SIZE_ASSERT(FlyingObject, 0xA8);

#endif

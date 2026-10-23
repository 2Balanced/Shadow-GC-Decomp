/* AttackObject and FlyingObject (class names from RTTI; Game/FlyingObject.h): the objects enemies
 * fire (BkWingLarge's air cutter). An AttackObject owns a collider made from its parameters' attack
 * table, whose attack collision calls back its slots 0x14, 0x18 and 0x1C; a FlyingObject also moves.
 * File name inferred from the class. Names marked "(name inferred)" are not original. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). The
 * constructor reaches the unit's .data through a pooled base address (for the callbacks' member
 * pointers), so the linker keeps that section whole: Game/CharaColliBaseCharInfo.h supplies the
 * RTTI base list it starts with. */

#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharaColliAttack.h"
#include "Game/FlyingObject.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include <boost/bind.hpp>
#include <boost/function.hpp>

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* The character info the attack objects' colliders belong to (none; see BlackBullCtrlUnit.cpp). */
extern boost::shared_ptr<CharInfo> lbl_805EE698;

/* The frame data (its float at 0x20 is the frame time; see Game/Enemy.h). */
extern u8 lbl_8057D6A0[0x150];

/* A line segment between two points (layout unknown; built by fn_80020A20). */
struct UnkSegment_80020A20 {
    /* 0x00 */ u8 field_0x0[0x18];
};

/* A hit the collision world reports (layout unknown but for the word at 0x14). */
struct UnkHit_80021E24 {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ u32 field_0x14;
};

extern "C" {
/* The collider manager, a collider made from an attack table, its attack collision, and the
 * collider's removal (see ArkLaser.cpp and BlockChange.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C86A8(void *manager, u32 table, const Vec3f *position,
                                              const UnkRotation *rotation, s32 arg4, s32 arg5,
                                              int arg6, const boost::weak_ptr<CharInfo> &owner);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
void fn_800C78B0(void *manager, boost::shared_ptr<CharaColliBase> *collider);
/* Sets the attack collision's callbacks (see BKTank.cpp, which declares the float last: the
 * arguments' order follows from the order retail evaluates them in). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, float arg3, int arg2, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
/* The collision world, a segment, the first hit along it and its release (see
 * ContainerBehavior.cpp). */
void *fn_80020D40(void);
void *fn_80020A20(UnkSegment_80020A20 *segment, const Vec3f *start, const Vec3f *end);
UnkHit_80021E24 *fn_80021E24(void *world, UnkSegment_80020A20 *segment, int arg2, int arg3, int arg4,
                             int arg5);
void *fn_80026588(void);
void fn_800265A4(UnkHit_80021E24 *hit, int arg1);
/* Vector helpers returning by value: v turned about z, x and y by angle (see BkWingLarge.cpp),
 * and v normalized; and vector += value. */
Vec3f fn_8040D044(const Vec3f *v, float angle);
Vec3f fn_8040D118(const Vec3f *v, float angle);
Vec3f fn_8040D0AC(const Vec3f *v, float angle);
Vec3f fn_8040CEE8(const Vec3f *v);
void fn_8040CFF8(Vec3f *vector, const Vec3f *value);
}

/* fn_8040CD74 as a function that returns its result (see Game/EnemyRender.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *lhs, const Vec3f *rhs);

/* The vector type's out-of-line assignment, called with a temporary the way the original's
 * operator is (name inferred; see JumpBoard.cpp). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

AttackObject::AttackObject(const AttackObjectParam *param) : m_param(*param) {
    if (m_param.m_radius < 0.0f) {
        m_param.m_radius = 1.0f / 60.0f;
    }
    m_time = 0.0f;
    m_ended = 0;
    fn_800091D0(&m_position, fn_800091D0(&field_0x10, reinterpret_cast<Vec3f *>(&m_param.m_position)));
    m_rotation = field_0x28 = m_param.m_rotation;
    field_0x71 = 0;
    field_0x72 = 0;
    field_0x74 = 0;
    if (m_param.field_0x4 != 0) {
        m_collider = fn_800C86A8(fn_80021B08(), m_param.field_0x4, &m_position, &m_rotation,
                                 m_param.field_0x8, m_param.field_0xC, 0,
                                 boost::weak_ptr<CharInfo>(lbl_805EE698));
        if (m_collider.get() != 0) {
            boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), m_collider);
            if (boost::shared_ptr<CharaColliAttack> collision = attack.lock()) {
                fn_80054DD0(collision.get(), m_param.field_0x14, m_param.field_0x10,
                            m_param.field_0x18,
                            AttackCallback(boost::bind(&AttackObject::vf_0x14, this, _1)),
                            AttackCallback(boost::bind(&AttackObject::vf_0x18, this, _1)));
                if (m_param.field_0x1C != 6) {
                    fn_80096B64(collision.get(), m_param.field_0x1C,
                                AttackCallback(boost::bind(&AttackObject::vf_0x1C, this, _1)));
                }
            }
        }
    }
}

AttackObject::~AttackObject() {
    ReleaseCollider();
}

void AttackObject::ReleaseCollider() {
    if (m_collider.get() != 0) {
        fn_800C78B0(fn_80021B08(), &m_collider);
    }
}

void AttackObject::vf_0x14(AttackCallbackParam &param) {
    fn_800091D0(reinterpret_cast<Vec3f *>(&param.field_0x8[0x34 - 0x8]),
                &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                    reinterpret_cast<Vec3f *>(&param.field_0x8[0x18 - 0x8]), &m_position)));
    vf_0x2C(param);
}

void AttackObject::vf_0x18(AttackCallbackParam &param) {}

void AttackObject::vf_0x1C(AttackCallbackParam &param) {
    param.m_result = 5;
    vf_0x30(param);
}

bool AttackObject::vf_0x20() {
    bool hit = false;
    void *world = fn_80020D40();
    UnkSegment_80020A20 segment;
    fn_80020A20(&segment, &field_0x10, &m_position);
    UnkHit_80021E24 *found = fn_80021E24(world, &segment, 1, 6, 0, 0);
    if (found != 0) {
        u32 kind = found->field_0x14;
        hit = vf_0x34(kind, fn_80026588());
        fn_800265A4(found, 1);
    }
    return hit;
}

void AttackObject::vf_0x24() {
    m_time += *reinterpret_cast<float *>(&lbl_8057D6A0[0x20]);
    if (m_time > m_param.m_radius) {
        vf_0x28();
    }
}

void AttackObject::vf_0x28() {
    vf_0x0C();
}

void FlyingObject::vf_0x38(float dt) {
    vf_0x24();
    if (m_motion.field_0x14 >= 0) {
        vf_0x3C();
    }
    fn_800091D0(&field_0x10, &m_position);
    Vec3f velocity(0.0f, 0.0f, dt * -vf_0x40());
    AssignVector(velocity, fn_8040D044(&velocity, m_rotation.z));
    AssignVector(velocity, fn_8040D118(&velocity, m_rotation.x));
    AssignVector(velocity, fn_8040D0AC(&velocity, m_rotation.y));
    fn_8040CFF8(&m_position, &velocity);
    if (m_motion.field_0xC != 0.0f) {
        m_position.y -= m_motion.field_0xC * dt;
    }
    if (m_motion.field_0x10 & 1) {
        vf_0x20();
    }
    if (m_collider.get() != 0) {
        m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&m_position));
        m_collider->SetRotation(reinterpret_cast<Vec3f *>(&m_rotation));
    }
}


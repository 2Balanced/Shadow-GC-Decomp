/* ThrowObject (class name from RTTI): a task for something thrown (see Game/ThrowObject.h). Its
 * state is a ThrowObjectImpl (class name from the RTTI of its bound callbacks): the object moves
 * by its velocity and acceleration, and a sphere of the given radius is swept along each step
 * through the collision world; when the sweep hits something the object stops there and the task
 * calls OnHit, and once the object has flown its range the task calls OnRangeEnd. Either way the
 * task ends. It carries an attack collision of its thrower (or of the thrower's owner) and can show
 * an effect along its flight. File name inferred from the class. Names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/MathTypes.h"
#include "Game/ThrowObject.h"
#include "boost/scoped_ptr.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

namespace SonicteamUSA {
namespace System {
namespace Collision {
/* CollisionResultList (class name from the RTTI of its shared_ptr control block; see
 * MeteorsHolder.cpp): the hits of a sweep. */
class CollisionResultList;
} // namespace Collision
} // namespace System
} // namespace SonicteamUSA

using SonicteamUSA::System::Collision::CollisionResultList;

/* Set up at startup and never read (name and meaning unknown). Its value is the float just above
 * 0.02 (0x3CA3D70B), not the nearest one, as a product such as 0.2f * 0.1f would give. */
inline float Plain(float value) {
    return value;
}
static float s_unused = Plain(0.020000001f);

/* What fn_80021934 passes its filter for each candidate hit (layout as in MeteorsHolder.cpp). */
struct UnkCollisionCandidate_80021934 {
    /* 0x00 */ u8 field_0x0[0x8];
    /* 0x08 */ CharaColliBase *m_collider;
    /* 0x0C */ u32 m_attributes;
    /* 0x10 */ void *m_data; /* what the caller passed with the filter */
};

/* A sphere: centre and radius (built by fn_80021C04; see MeteorsHolder.cpp). */
struct UnkSphere_80021C04 {
    /* 0x00 */ float center[3];
    /* 0x0C */ float radius;
};

extern "C" {
/* Starts effect id at position with rotation (behaviour inferred; the other arguments are
 * unknown). */
boost::weak_ptr<Effect::System::EffectElement> fn_801D2BEC(int arg0, const Vec3f *position,
                                                           const Vec3f *rotation, int id, int arg4);
/* The character a character belongs to, such as a vehicle's driver (behaviour inferred; see
 * EggLastMech.cpp). */
boost::weak_ptr<CharInfo> fn_800B75DC(const boost::weak_ptr<CharInfo> &character);
/* Creates the attack collision of a bullet (see BulletLaser.cpp). */
boost::shared_ptr<CharaColliAttack> fn_8005A300(int side, boost::weak_ptr<CharInfo> owner, float damage,
                                                const AttackCallback &onHit, const AttackCallback &onEnd);
/* Makes an attack collision ignore another (see BulletLaser.cpp). */
void fn_800532F8(CharaColliAttack *attack, boost::weak_ptr<CharaColliAttack> other);
/* Releases an attack collision (behaviour inferred). */
void fn_80053B40(boost::shared_ptr<CharaColliAttack> *attack);
/* Moves position by velocity, which acceleration changes, over dt (see Bomb.cpp). */
void fn_804103E8(Vec3f *position, Vec3f *velocity, const Vec3f *acceleration, float scale, float dt);
/* The distance between two points (see BulletLaser.cpp). */
float fn_8040D1B4(const Vec3f *a, const Vec3f *b);
/* Vector helpers (behaviour inferred from their bodies; see BulletLaser.cpp). */
Vec3f fn_8040CEE8(const Vec3f *vector);              /* normalised */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale); /* vector * scale */
void fn_8040CFF8(Vec3f *vector, const Vec3f *value); /* vector += value */
/* A direction turned into the angles of a rotation (see BulletLaser.cpp). */
Vec2f fn_8040F760(const Vec3f &direction);

/* The collision world, a sphere from a centre and a radius, and the sweep of a sphere through the
 * world along velocity; it returns the hits that filter accepts, or null (see MeteorsHolder.cpp).
 * Then the collision manager, handing it the hits of an attack, and releasing the hits (behaviour
 * inferred). */
void *fn_80020D40(void);
void fn_80021C04(UnkSphere_80021C04 *sphere, const Vec3f *center, float radius);
CollisionResultList *fn_80021934(void *world, const UnkSphere_80021C04 *sphere, const Vec3f *velocity,
                                 int arg3, int arg4, int arg5,
                                 bool (*filter)(UnkCollisionCandidate_80021934 *candidate), void *data,
                                 int arg8);
void *fn_80021B08(void);
void fn_800C4190(void *manager, boost::weak_ptr<CharaColliAttack> attack, CollisionResultList *hits,
                 int arg3);
void fn_80026FB4(CollisionResultList *hits, int arg1);
}

/* fn_8040CD74 called as returning a - b by value (see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* What the sweep's filter compares the candidates with (struct name invented; names inferred). */
struct SweepFilter {
    /* 0x0 */ u32 m_mask;                              /* the attributes that stop the sweep */
    /* 0x4 */ boost::shared_ptr<CharaColliBase> m_self; /* the collider the sweep ignores */
};

/* Accepts the candidates of the sweep: not the object's own collider, and of the others only those
 * with an attribute in the mask (name inferred). */
static bool AcceptHit(UnkCollisionCandidate_80021934 *candidate) {
    SweepFilter *filter = (SweepFilter *)candidate->m_data;
    u32 mask = filter->m_mask;
    if (filter->m_self.get() != 0 && candidate->m_collider != 0 &&
        filter->m_self.get() == candidate->m_collider) {
        return false;
    }
    if (candidate->m_attributes != 0) {
        return (candidate->m_attributes & mask) ? true : false;
    }
    return true;
}

/* Sweeps a sphere of radius from position along move, and moves position to where it stopped;
 * when it hit something, the hits go to the attack collision (name inferred). */
static bool Sweep(Vec3f *position, const Vec3f *move, u32 mask, int arg,
                  const boost::shared_ptr<CharaColliBase> &self,
                  const boost::shared_ptr<CharaColliAttack> &attack, float radius) {
    void *world = fn_80020D40();
    UnkSphere_80021C04 sphere;
    fn_80021C04(&sphere, position, radius);
    Vec3f velocity;
    fn_800091EC(&velocity, move);
    SweepFilter filter;
    filter.m_mask = mask;
    filter.m_self = self;
    CollisionResultList *hits;
    bool hit = false;
    hits = fn_80021934(world, &sphere, &velocity, 3, arg, 0, AcceptHit, &filter, 0);
    fn_8040CFF8(reinterpret_cast<Vec3f *>(sphere.center), &velocity);
    if (hits != 0) {
        hit = true;
        void *manager = fn_80021B08();
        fn_800C4190(manager, attack, hits, 0);
        fn_80026FB4(hits, 1);
    }
    fn_800091D0(position, reinterpret_cast<Vec3f *>(sphere.center));
    return hit;
}

class ThrowObjectImpl {
  public:
    ThrowObjectImpl();
    ~ThrowObjectImpl();

    /* Moves the object by dt (name inferred). */
    void Move(float dt);
    /* Turns the rotation towards direction (name inferred). */
    void UpdateRotation(const Vec3f &direction);
    /* Sweeps the last step; true when it hit something (name inferred). */
    bool CheckHit(float dt);
    /* Creates the attack collision for owner (name inferred). */
    void SetupAttack(const boost::weak_ptr<CharInfo> &owner);
    /* The attack collision's callbacks (names inferred). */
    void OnAttackEnd(AttackCallbackParam &param);
    void OnHit(AttackCallbackParam &param);

    /* Member names are inferred. */
    /* 0x00 */ boost::weak_ptr<CharaColliBase> m_collider;
    /* 0x08 */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0x10 */ Vec3f m_position;
    /* 0x1C */ Vec3f m_prevPosition;
    /* 0x28 */ Vec3f m_velocity;
    /* 0x34 */ Vec3f m_acceleration;
    /* 0x40 */ Vec3f m_rotation;
    /* 0x4C */ float m_radius;
    /* 0x50 */ float m_range;
    /* 0x54 */ float m_distance;
    /* 0x58 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x60 */ boost::weak_ptr<CharInfo> m_thrower;
};
SIZE_ASSERT(ThrowObjectImpl, 0x68);

ThrowObjectImpl::ThrowObjectImpl() {
    m_distance = 0.0f;
}

ThrowObjectImpl::~ThrowObjectImpl() {
    if (m_attack.get() != 0) {
        fn_80053B40(&m_attack);
    }
}

void ThrowObjectImpl::Move(float dt) {
    fn_804103E8(&m_position, &m_velocity, &m_acceleration, 0.0f, dt);
    UpdateRotation(m_velocity);
    m_distance += fn_8040D1B4(&m_position, &m_prevPosition);
}

void ThrowObjectImpl::UpdateRotation(const Vec3f &direction) {
    if (!(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z < 0.0001f)) {
        Vec2f angles = fn_8040F760(direction);
        m_rotation.x = angles.x;
        m_rotation.y = angles.y;
    }
}

bool ThrowObjectImpl::CheckHit(float dt) {
    Vec3f move;
    fn_800091EC(&move, &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_position, &m_prevPosition));
    Vec3f position;
    fn_800091EC(&position, &m_prevPosition);
    if (Sweep(&position, &move, 0xFFF3FFE7, 3, m_collider.lock(), m_attack, m_radius)) {
        fn_800091D0(&m_position, &position);
        fn_800091D0(&m_velocity, &(const Vec3f &)fn_8040CDA8(&move, 1.0f / dt));
        return true;
    }
    return false;
}

void ThrowObjectImpl::SetupAttack(const boost::weak_ptr<CharInfo> &owner) {
    m_attack = fn_8005A300(0, owner, 6.0f, boost::bind(&ThrowObjectImpl::OnHit, this, _1),
                           boost::bind(&ThrowObjectImpl::OnAttackEnd, this, _1));
}

void ThrowObjectImpl::OnHit(AttackCallbackParam &param) {
    /* The direction and speed of the hit at 0x34 and 0x4C (meanings inferred; see BulletLaser.cpp). */
    fn_800091D0(reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)),
                &(const Vec3f &)fn_8040CEE8(&m_velocity));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = fn_8040CF50(&m_velocity);
}

void ThrowObjectImpl::OnAttackEnd(AttackCallbackParam &param) {
    if (m_attack.get() != 0) {
        /* What the attack hit at 0x8 (meaning inferred; see BulletLaser.cpp). */
        fn_800532F8(m_attack.get(), *reinterpret_cast<boost::weak_ptr<CharaColliAttack> *>(param.field_0x8));
    }
}

/* Deletes the state (name inferred; see checked_delete below). */
inline void DeleteThrowObjectImpl(ThrowObjectImpl *impl) {
    delete impl;
}

/* Retail keeps boost::checked_delete<ThrowObjectImpl> out of line (0x8027D040) and the destructor
 * at its definition; see Rocket.cpp and docs/research-notes.md. */
namespace boost {
template<> inline void checked_delete<ThrowObjectImpl>(ThrowObjectImpl *x) {
    DeleteThrowObjectImpl(x);
}
} // namespace boost

ThrowObject::ThrowObject(const boost::weak_ptr<CharInfo> &thrower, const Vec3f &position,
                         const Vec3f &velocity, const Vec3f &acceleration, float radius, float range)
    : Task(fn_801EB304(fn_8001C4F8(), 11)), m_impl(new ThrowObjectImpl) {
    m_name = "ThrowObject";
    fn_800091D0(&m_impl->m_position, fn_800091D0(&m_impl->m_prevPosition, &position));
    fn_800091D0(&m_impl->m_velocity, &velocity);
    fn_800091D0(&m_impl->m_acceleration, &acceleration);
    m_impl->m_radius = radius;
    m_impl->m_range = range;
    m_impl->UpdateRotation(velocity);
    if (!thrower.expired() && thrower.lock()->vf_0x24()) {
        m_impl->SetupAttack(fn_800B75DC(thrower));
    } else {
        m_impl->SetupAttack(thrower);
    }
    m_impl->m_thrower = thrower;
}

ThrowObject::~ThrowObject() {
    if (!m_impl->m_effect.expired()) {
        m_impl->m_effect.lock()->Stop();
        m_impl->m_effect.reset();
    }
}

void ThrowObject::Tick(float dt) {
    m_impl->Move(dt);
    if (m_impl->CheckHit(dt)) {
        OnHit();
        m_flags |= 1;
    }
    if (m_impl->m_distance > m_impl->m_range) {
        OnRangeEnd();
        m_flags |= 1;
    }
    fn_800091D0(&m_impl->m_prevPosition, &m_impl->m_position);
}

ThrowObjectPosition ThrowObject::GetPosition() {
    return m_impl->m_position;
}

void ThrowObject::SetEffect(int id) {
    if (!m_impl->m_effect.expired()) {
        m_impl->m_effect.lock()->Stop();
        m_impl->m_effect.reset();
    }
    m_impl->m_effect = fn_801D2BEC(0, &m_impl->m_position, &m_impl->m_rotation, id, 0);
}

void ThrowObject::OnRangeEnd() {}

void ThrowObject::OnHit() {}

boost::weak_ptr<CharInfo> ThrowObject::GetThrower() {
    return m_impl->m_thrower;
}

/* Boss::Diablon::Parts::Barrier (class name from RTTI): the Diablon boss's barrier, a part that
 * follows a node of the boss. It has a sphere collision whose attack callback turns every hit
 * away and shows a barrier effect facing the attacker. Once flag 3 is set the barrier drops (its
 * collision off, flag 2 clear) for the boss's barrier down time, then grows back to full size and
 * stays up until flag 3 clears again. File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Diablon.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

extern "C" {
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CEE8(const Vec3f *vector);  /* normalised */
Vec3f fn_8040FB08(const Mtx34f *matrix); /* its translation */
Vec2f fn_8040F760(const Vec3f *direction); /* the angles turning the y axis onto it (inferred) */

/* The collider manager, a sphere collider for owner, and the attack collision of a collider
 * (see Bomb.cpp and BlockChange.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position,
                                              u32 flags, int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Sets the attack callback of an attack collision (see BKTank.cpp). */
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);

/* Starts effect id of the boss's effect set (Diablon+0x6C) following matrix (behaviour inferred;
 * see DiablonPartsBeam.cpp). */
boost::weak_ptr<EffectElement> fn_802DC998(u32 effects, const Mtx34f *matrix, int id);

/* The sound manager, and plays sound id at the character info (see BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, CharInfo *source, int arg3, int arg4);
}

extern const Vec3f lbl_805E07A0; /* the zero vector */

/* Assigns value to vector through the out-of-line vector assignment (name inferred). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail (as in SecretDisk.cpp). */
inline float Span(float length) {
    return length;
}

/* The barrier's full radius, and its height above the node (names inferred). */
static float s_radius = Span(40.0f);
static float s_height = Span(15.0f);

namespace Boss {
namespace Diablon {
namespace Parts {

class Barrier : public PartsBase {
  public:
    Barrier(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *node);
    virtual ~Barrier();
    virtual void Tick(float dt);

    /* Names inferred. */
    void UpdateEffect(float dt);
    void UpdateMatrix();
    void UpdateCollider();
    void CreateCollider();
    void ReleaseCollider();
    void OnAttack(AttackCallbackParam &param);

    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node);

    /* 0x24 */ const Mtx34f *m_node;
    /* 0x28 */ Vec3f m_position;
    /* 0x34 */ Mtx34f m_matrix; /* the barrier effect's */
    /* 0x64 */ Vec3f m_direction; /* from the last attacker to the barrier */
    /* 0x70 */ float m_effectTime; /* how long the effect still shows */
    /* 0x74 */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x7C */ boost::weak_ptr<CharaColliAttack> m_attack;
    /* 0x84 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x8C */ float m_scale;
    /* 0x90 */ s32 m_state;
    /* 0x94 */ float m_timer;
};
SIZE_ASSERT(Barrier, 0x98);

Barrier::Barrier(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *node)
    : PartsBase(parent, "Barrier", owner) {
    m_node = node;
    CreateCollider();
    fn_800091D0(&m_direction, &Vec3f(0.0f, 1.0f, 0.0f));
    m_effectTime = 0.0f;
    m_state = 0;
    m_scale = 0.0f;
}

Barrier::~Barrier() {
    ReleaseCollider();
    if (!m_effect.expired()) {
        m_effect.lock()->Retire();
    }
}

void Barrier::Tick(float dt) {
    AssignVector(m_position, fn_8040FB08(m_node));
    m_position.y += s_height;
    switch (m_state) {
    case 0:
        fn_802D4EF4(&m_owner->m_flags, 2);
        m_collider->TurnOff(-1);
        m_scale = 0.0f;
        m_state++;
        break;
    case 1:
        if (fn_802D58FC(&m_owner->m_flags, 3)) {
            m_state = 100;
            m_timer = m_owner->m_params->m_barrierDownTime;
        }
        break;
    case 100:
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_state++;
        }
        break;
    case 101:
        fn_802D4ED4(&m_owner->m_flags, 2);
        m_collider->TurnOn(-1);
        m_scale = 0.01f;
        m_state++;
        break;
    case 102:
        m_scale += dt / 0.5f;
        if (m_scale >= 1.0f) {
            m_scale = 1.0f;
            m_state++;
        }
    case 103:
        if (!fn_802D58FC(&m_owner->m_flags, 3)) {
            m_state = 0;
        }
        break;
    }
    if (fn_802D58FC(&m_owner->m_flags, 2)) {
        UpdateCollider();
    }
    UpdateEffect(dt);
}

void Barrier::UpdateEffect(float dt) {
    if (!m_effect.expired()) {
        UpdateMatrix();
        m_effectTime -= dt;
        if (m_effectTime <= 0.0f) {
            m_effectTime = 0.0f;
            m_effect.lock()->Stop();
            m_effect.reset();
        }
    }
}

void Barrier::UpdateMatrix() {
    Mtx34f translation;
    fn_8040B034(&translation, &m_position);
    fn_8040C148(&m_matrix, &translation);
    Vec2f angles = fn_8040F760(&m_direction);
    fn_8040B848(&m_matrix, angles.y);
    fn_8040B884(&m_matrix, angles.x);
    fn_8040B720(&m_matrix, m_scale, m_scale, m_scale);
}

void Barrier::UpdateCollider() {
    if (m_collider.get() != 0) {
        /* Slot 0x3C reads a position from the start of what it is given. */
        m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&m_position));
        m_collider->SetSize(m_scale * s_radius, 0.0f, 0.0f, -2);
    }
}

void Barrier::CreateCollider() {
    void *manager = fn_80021B08();
    if (m_collider.get() != 0) {
        ReleaseCollider();
    }
    Vec3f position;
    m_collider = fn_800C7EA8(manager, &position, 0x202, 4, 0, m_owner, &lbl_805E07A0, s_radius);
    m_collider->SetField2C(1);
    m_attack = fn_800C4374(manager, m_collider);
    boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
    if (attack.get() != 0) {
        fn_80096B64(attack.get(), 1, AttackCallback(boost::bind(&Barrier::OnAttack, this, _1)));
    }
    UpdateCollider();
}

void Barrier::ReleaseCollider() {
    if (m_collider.get() != 0) {
        m_collider.reset();
    }
}

void Barrier::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    m_effectTime = 0.25f;
    AssignVector(m_direction,
                 fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                     &m_position, reinterpret_cast<Vec3f *>(param.field_0x8 + (0x18 - 0x8)))));
    UpdateMatrix();
    if (m_effect.expired()) {
        m_effect = fn_802DC998(m_owner->field_0x6C, &m_matrix, 10);
    }
    fn_802D4ED4(&m_owner->m_flags, 5);
    fn_801D54F4(fn_80053164(), 0x7E40, m_owner.px, 0, 1);
}

PartsBase *Barrier::Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                const Mtx34f *node) {
    return new Barrier(parent, owner, node);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss

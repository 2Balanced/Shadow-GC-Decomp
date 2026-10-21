/* Boss::Diablon::Parts::Bomb (class name from RTTI): the Diablon boss's bomb attack, a part
 * created at a node of the boss. It starts a charging effect and sound there, and an effect on the
 * ground below. Action 9 sets it off: it gets a sphere collision with an attack, plays the blast
 * sound and shakes the controller, and the blast then grows from its starting radius to the
 * boss's bomb radius over the bomb grow time. Action 10 stops it, and the part then finishes.
 * File name inferred from the class. Names marked "(name inferred)" are not original.
 * Definitions are in reverse address order (deferred inlining emits functions last-defined
 * first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Diablon.h"
#include "Game/GadgetSound.h"
#include "Game/Vibration.h"
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
Vec3f fn_8040CEE8(const Vec3f *vector);                   /* normalised */
Vec3f fn_8040FB08(const Mtx34f *matrix);                  /* its translation */
/* The lesser of two values (behaviour inferred). */
const float &fn_800704AC(const float &a, const float &b);

/* The collider manager, a sphere collider for owner, and the attack collision of a collider
 * (see Bomb.cpp and BlockChange.cpp). */
void *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position,
                                              u32 flags, int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Sets up an attack collision and its two callbacks (see Meteor.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);

/* Starts effect id of the boss's effect set (Diablon+0x6C), following matrix or not, and places
 * an effect at a matrix (behaviour inferred; see DiablonPartsBeam.cpp). */
boost::weak_ptr<EffectElement> fn_802DC998(u32 effects, const Mtx34f *matrix, int id);
boost::weak_ptr<EffectElement> fn_802DC95C(u32 effects, int id);
void fn_80425E68(EffectElement *effect, const Mtx34f *matrix);

/* The sound manager, and plays sound id at the character info (see BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, CharInfo *source, int arg3, int arg4);
/* Shakes the controller with pattern near position (see EggLastMech.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position,
                                                      int arg2);
}

extern const Vec3f lbl_805E07A0; /* the zero vector */

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail (as in SecretDisk.cpp). */
inline float Span(float length) {
    return length;
}

/* The radius the blast model is made for, and the blast's starting radius (names inferred). */
static float s_modelRadius = Span(60.0f);
static float s_startRadius = Span(60.0f);

namespace Boss {
namespace Diablon {
namespace Parts {

class Bomb : public PartsBase {
  public:
    Bomb(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *matrix);
    virtual ~Bomb();
    virtual void Tick(float dt);
    virtual void OnAction(const Action &action);

    /* Names inferred. */
    void StartCharge();
    void Stop();
    void UpdateMatrix();
    void StartGroundEffect();
    void Explode();
    void UpdateCollider();
    void OnAttackHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);

    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                             const Mtx34f *matrix);

    /* 0x24 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x2C */ boost::shared_ptr<CharaColliBase> m_collider;
    /* 0x34 */ boost::weak_ptr<CharaColliAttack> m_attack;
    /* 0x3C */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x58 */ Mtx34f m_matrix;
    /* 0x88 */ Vec3f m_position;
    /* 0x94 */ float m_radius;
    /* 0x98 */ s32 m_state;
};
SIZE_ASSERT(Bomb, 0x9C);

Bomb::Bomb(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *matrix)
    : PartsBase(parent, "Bomb", owner) {
    fn_800091D0(&m_position, &(const Vec3f &)fn_8040FB08(matrix));
    m_radius = s_startRadius;
    m_state = 0;
}

Bomb::~Bomb() {
    Stop();
}

void Bomb::Tick(float dt) {
    switch (m_state) {
    case 0:
        StartCharge();
        StartGroundEffect();
        m_state++;
        break;
    case 100:
        Explode();
        m_state++;
        break;
    case 101: {
        DiablonParams *params = m_owner->m_params;
        m_radius += dt * (params->m_bombMaxRadius - s_startRadius) / params->m_bombGrowTime;
        m_radius = fn_800704AC(m_radius, params->m_bombMaxRadius);
        break;
    }
    case 200:
        Stop();
        m_state++;
        break;
    case 201:
        m_finished = true;
        return;
    }
    UpdateMatrix();
    UpdateCollider();
}

void Bomb::OnAction(const Action &action) {
    switch (action.m_type) {
    case 9:
        if (m_state < 100) {
            m_state = 100;
        }
        break;
    case 10:
        if (m_state < 200) {
            m_state = 200;
        }
        break;
    }
}

void Bomb::StartCharge() {
    m_effect = fn_802DC998(m_owner->field_0x6C, &m_matrix, 0);
    m_sound = fn_801D3DC8(0x7E33, reinterpret_cast<GadgetCharInfo *>(m_owner.px), this);
}

void Bomb::Stop() {
    if (!m_effect.expired()) {
        m_effect.lock()->Stop();
        m_effect.reset();
    }
    m_sound.fn_801D455C();
}

void Bomb::UpdateMatrix() {
    float scale = m_radius / s_modelRadius;
    Mtx34f translation;
    fn_8040B034(&translation, &m_position);
    fn_8040C148(&m_matrix, &translation);
    fn_8040B720(&m_matrix, scale, scale, scale);
}

void Bomb::StartGroundEffect() {
    boost::weak_ptr<EffectElement> weak;
    boost::shared_ptr<EffectElement> effect;
    weak = fn_802DC95C(m_owner->field_0x6C, 0x32);
    effect = weak.lock();
    if (effect.get() != 0) {
        Mtx34f matrix;
        Mtx34f translation;
        Vec3f position;
        fn_8040B034(&translation, fn_80008E1C(&position, m_position.x, 0.0f, m_position.z));
        fn_8040C148(&matrix, &translation);
        fn_80425E68(effect.get(), &matrix);
    }
}

void Bomb::Explode() {
    void *manager = fn_80021B08();
    m_collider = fn_800C7EA8(manager, &lbl_805E07A0, 0x40000, 6, 0, m_owner, &lbl_805E07A0, 0.0f);
    if (m_collider.get() != 0) {
        m_collider->SetField2C(1);
        m_attack = fn_800C4374(manager, m_collider);
        boost::shared_ptr<CharaColliAttack> attack = m_attack.lock();
        if (attack.get() != 0) {
            fn_80054DD0(attack.get(), 4, 0x100, 1.0f, boost::bind(&Bomb::OnAttackHit, this, _1),
                        boost::bind(&Bomb::OnAttackEnd, this, _1));
            attack->m_owner = m_owner;
        }
    }
    m_sound.fn_801D455C();
    fn_801D54F4(fn_80053164(), 0x7E34, m_owner.px, 0, 1);
    fn_8034DE08(7, &m_owner->m_position, -2);
}

void Bomb::UpdateCollider() {
    if (m_collider.get() != 0) {
        /* Slot 0x3C reads a position from the start of what it is given. */
        m_collider->SetCharInfo(reinterpret_cast<CharInfo *>(&m_position));
        m_collider->SetSize(m_radius, 0.0f, 0.0f, -2);
    }
}

void Bomb::OnAttackHit(AttackCallbackParam &param) {
    fn_800091D0(reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)),
                &(const Vec3f &)fn_8040CEE8(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
                    reinterpret_cast<Vec3f *>(param.field_0x8 + (0x18 - 0x8)),
                    &m_owner->m_position)));
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = 1.0f;
    param.m_damage = 1.0f;
}

void Bomb::OnAttackEnd(AttackCallbackParam &param) {}

PartsBase *Bomb::Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                        const Mtx34f *matrix) {
    return new Bomb(parent, owner, matrix);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss

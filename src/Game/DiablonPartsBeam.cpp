/* Boss::Diablon::Parts::Beam (class name from RTTI): the Diablon boss's beam attack, a part
 * created on a node matrix of the boss. It charges (an effect and a sound) until the boss sends
 * action 6; it then aims at the player (or straight ahead without one), pitched no higher than
 * the boss's beam angle limit, and fires: the beam grows to its full length while sweeping
 * sideways for the beam time, is cut short where it meets the ground (with a hit effect there),
 * and attacks what it passes through. Action 7, or the end of the beam time, stops it, and the
 * part then finishes. File name inferred from the class. Names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
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

namespace std {
float cos(float x);
float sin(float x);
} // namespace std

/* Degrees to radians: an out-of-line function of the game (0x8000AB90). */
float DegToRad(float degrees);

extern const float lbl_805FAD38; /* pi */

/* A line segment set up by fn_80020A20 (0x18 bytes; see ScatteringWeapon.cpp). */
struct UnkLine_80020A20 {
    u8 field_0x0[0x18];
};

/* The hits of a collision query (see ScatteringWeapon.cpp); fn_8002735C returns the first. */
struct UnkLineHits_80021FE4;
struct UnkLineHit_8002735C {
    /* 0x00 */ u8 field_0x0[0x14];
    /* 0x14 */ void *m_object;
};

/* An iterator over the hits (layout from fn_8001EDFC; see BombServer.cpp). */
struct UnkColliContactNode_800587A0;
struct UnkColliContactIter_8001EDFC {
    UnkColliContactIter_8001EDFC(UnkColliContactNode_800587A0 *node) : m_node(node) {}

    UnkColliContactNode_800587A0 *m_node;
};

extern "C" {
/* Vector helpers (behaviour inferred from their bodies). */
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);         /* vector * scale */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);   /* a + b */
Vec3f fn_8040CFB8(const Vec3f *a, const Vec3f *b);   /* a x b */
float fn_8040CE74(Vec3f *vector); /* normalises; returns the former length */
Vec3f fn_8040D0AC(const Vec3f *vector, float angle);         /* turned about y */
Vec3f fn_8040FB08(const Mtx34f *matrix);                     /* its translation */
Vec3f fn_8040F7F8(const Vec3f *direction);                   /* Euler angles */
Mtx34f fn_8040FD8C(const Vec3f *axis, float angle);          /* rotation */
Vec3f fn_8040B67C(const Mtx34f *matrix, const Vec3f *point);
/* The lesser of two values (behaviour inferred). */
const float &fn_800704AC(const float &a, const float &b);

/* The collision world, the line test against it, its first hit, the hit object's position and
 * normal, and the release of the result (see ScatteringWeapon.cpp). */
void *fn_80020A20(UnkLine_80020A20 *line, const Vec3f *start, const Vec3f *end);
void *fn_80020D40(void);
UnkLineHits_80021FE4 *fn_80021FE4(void *world, const UnkLine_80020A20 *line, int arg2, int arg3,
                                  void *filter, int arg5);
UnkLineHit_8002735C *fn_8002735C(UnkLineHits_80021FE4 *hits);
const Vec3f *fn_8001E9BC(void *object);
const Vec3f *fn_800266D4(void *object);
void fn_80026FB4(UnkLineHits_80021FE4 *hits, int arg1);
/* The hits as a list, and the step to the next one (see BombServer.cpp). */
UnkColliContactNode_800587A0 *fn_8001EE04(UnkLineHits_80021FE4 *hits);
UnkColliContactNode_800587A0 *fn_8001EDEC(UnkLineHits_80021FE4 *hits);
UnkColliContactIter_8001EDFC fn_80027374(UnkColliContactIter_8001EDFC *it, int);
/* The collider manager, and an attack on what a hit touched (behaviour inferred). */
void *fn_80021B08(void);
void fn_800C40C4(void *manager, boost::weak_ptr<CharaColliAttack> attack,
                 UnkColliContactNode_800587A0 *node, int arg3);
/* Sets up an attack collision and its two callbacks (see Meteor.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);

/* Starts effect id of the boss's effect set (Diablon+0x6C), following matrix or not (behaviour
 * inferred). */
boost::weak_ptr<EffectElement> fn_802DC998(u32 effects, const Mtx34f *matrix, int id);
boost::weak_ptr<EffectElement> fn_802DC95C(u32 effects, int id);
/* Moves an effect to position (behaviour inferred). */
void fn_80425E4C(EffectElement *effect, const Vec3f *position);

/* The sound manager, and plays sound id at the character info (see BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, CharInfo *source, int arg3, int arg4);
}

/* Assigns value to vector through the out-of-line vector assignment (name inferred). */
inline void AssignVector(Vec3f &vector, const Vec3f &value) {
    fn_800091D0(&vector, &value);
}

/* Assigns value to matrix through the out-of-line matrix copy (name inferred). */
inline void AssignMatrix(Mtx34f &matrix, const Mtx34f &value) {
    fn_8040C148(&matrix, &value);
}

typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

/* A length (name inferred). As a call it is not a constant expression, so the constants below are
 * stored by a static initializer, as in retail (as in SecretDisk.cpp). */
inline float Span(float length) {
    return length;
}

/* The length unit of the beam model, and the beam's full length (names inferred). */
static float s_modelLength = Span(100.0f);
static float s_maxLength = Span(3000.0f);

namespace Boss {
namespace Diablon {
namespace Parts {

class Beam : public PartsBase {
  public:
    Beam(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *matrix);
    virtual ~Beam();
    virtual void Tick(float dt);
    virtual void OnAction(const Action &action);

    /* Names inferred. */
    void StartBeam();
    void UpdateMatrix();
    void UpdateTarget();
    void Aim();
    bool FindGround(Vec3f *position, Vec3f *normal);
    void CreateAttack();
    void UpdateAttack();
    void OnAttackHit(AttackCallbackParam &param);
    void OnAttackEnd(AttackCallbackParam &param);
    void StartCharge();
    void StopCharge();
    void UpdateHitEffect(const Vec3f *position, const Vec3f *normal);
    void StopHitEffect();

    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                             const Mtx34f *matrix);

    /* 0x24 */ float m_length;
    /* 0x28 */ Vec3f m_direction;
    /* 0x34 */ Vec3f m_position;
    /* 0x40 */ Vec3f m_sweepAxis;
    /* 0x4C */ Vec3f m_target;
    /* 0x58 */ boost::weak_ptr<EffectElement> m_beamEffect;
    /* 0x60 */ boost::weak_ptr<EffectElement> m_chargeEffect;
    /* 0x68 */ boost::weak_ptr<EffectElement> m_hitEffect;
    /* 0x70 */ boost::shared_ptr<CharaColliAttack> m_attack;
    /* 0x78 */ const Mtx34f *m_matrix; /* the node the beam starts from */
    /* 0x7C */ Mtx34f m_beamMatrix;
    /* 0xAC */ float m_time;
    /* 0xB0 */ float m_delay;
    /* 0xB4 */ s32 m_state;
};
SIZE_ASSERT(Beam, 0xB8);

Beam::Beam(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *matrix)
    : PartsBase(parent, "Beam", owner), m_matrix(matrix) {
    AssignVector(m_target, fn_8040FB08(m_matrix));
    m_length = 0.0f;
    m_time = 0.0f;
    m_delay = m_owner->m_params->m_beamDelay;
    m_state = 0;
}

Beam::~Beam() {
    if (!m_beamEffect.expired()) {
        m_beamEffect.lock()->Retire();
    }
    if (!m_chargeEffect.expired()) {
        m_chargeEffect.lock()->Retire();
    }
    if (!m_hitEffect.expired()) {
        m_hitEffect.lock()->Retire();
    }
}

void Beam::Tick(float dt) {
    AssignVector(m_position, fn_8040FB08(m_matrix));
    if (m_delay > 0.0f) {
        m_delay -= dt;
        if (m_delay <= 0.0f) {
            UpdateTarget();
        }
    }
    switch (m_state) {
    case 0:
        StartCharge();
        m_state++;
        break;
    case 1:
        break;
    case 100:
        if (m_delay > 0.0f) {
            UpdateTarget();
            m_delay = 0.0f;
        }
        Aim();
        m_state++;
        break;
    case 101:
        StartBeam();
        m_state++;
    case 102: {
        m_length += s_maxLength * dt / 0.35f;
        m_length = fn_800704AC(m_length, s_maxLength);
        Mtx34f rotation;
        float angle = dt * m_owner->m_params->m_beamSweepAngle / m_owner->m_params->m_beamTime;
        fn_8040C148(&rotation, &(const Mtx34f &)fn_8040FD8C(
                                   &m_sweepAxis, angle * (lbl_805FAD38 / 180.0f)));
        AssignVector(m_direction, fn_8040B67C(&rotation, &m_direction));
        Vec3f position;
        Vec3f normal;
        if (FindGround(&position, &normal)) {
            UpdateHitEffect(&position, &normal);
        } else {
            StopHitEffect();
        }
        m_time += dt;
        if (m_time >= m_owner->m_params->m_beamTime) {
            m_state = 200;
        }
        break;
    }
    case 200:
        StopCharge();
        StopHitEffect();
        if (!m_beamEffect.expired()) {
            m_beamEffect.lock()->Stop();
            m_beamEffect.reset();
        }
        m_state++;
        break;
    case 201:
        m_finished = true;
        return;
    }
    UpdateMatrix();
    UpdateAttack();
}

void Beam::OnAction(const Action &action) {
    switch (action.m_type) {
    case 6:
        if (m_state < 100) {
            m_state = 100;
        }
        break;
    case 7:
        if (m_state < 200) {
            m_state = 200;
        }
        break;
    }
}

void Beam::StartBeam() {
    m_beamEffect = fn_802DC998(m_owner->field_0x6C, &m_beamMatrix, 0x14);
    CreateAttack();
    fn_801D54F4(fn_80053164(), 0x7E31, m_owner.px, 0, 1);
}

void Beam::UpdateMatrix() {
    Mtx34f translation;
    fn_8040B034(&translation, &m_position);
    fn_8040C148(&m_beamMatrix, &translation);
    Vec3f angles = fn_8040F7F8(&m_direction);
    fn_8040B848(&m_beamMatrix, angles.y);
    fn_8040B884(&m_beamMatrix, angles.x);
    fn_8040B720(&m_beamMatrix, 1.0f, 1.0f, m_length / s_modelLength);
}

void Beam::UpdateTarget() {
    boost::shared_ptr<CharInfo> target = m_owner->m_target.lock();
    if (target.get() != 0) {
        fn_800091D0(&m_target, &target->m_position);
    } else {
        fn_800091D0(&m_target, &(const Vec3f &)fn_8040B67C(
                                   m_matrix, &Vec3f(0.0f, 0.0f, 1.0f)));
    }
}

void Beam::Aim() {
    Mtx34f rotation;
    Vec3f offset;
    fn_800091EC(&offset,
                &(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(&m_target, &m_position));
    m_direction.x = 0.0f;
    m_direction.y = offset.y;
    offset.y = 0.0f;
    m_direction.z = -fn_8040CF50(&offset);
    fn_8040CE74(&m_direction);
    DiablonParams *params = m_owner->m_params;
    float limit = params->m_beamAngleLimit * (lbl_805FAD38 / 180.0f);
    float c = std::cos(limit);
    if (-m_direction.z < c) {
        m_direction.z = -c;
        float sign;
        if (m_direction.y > 0.0f) {
            sign = 1.0f;
        } else {
            sign = -1.0f;
        }
        m_direction.y = sign * std::sin(limit);
        fn_8040CE74(&m_direction);
    }
    fn_800091D0(&m_direction, &(const Vec3f &)fn_8040D0AC(&m_direction, m_owner->m_rotation.y));
    Vec3f side;
    fn_800091EC(&side, &(const Vec3f &)fn_8040CFB8(&m_direction,
                                                            &Vec3f(0.0f, 1.0f, 0.0f)));
    fn_800091D0(&m_sweepAxis, &(const Vec3f &)fn_8040CFB8(&side, &m_direction));
    fn_8040CE74(&m_sweepAxis);
    AssignMatrix(rotation,
                 fn_8040FD8C(&m_sweepAxis, DegToRad(-m_owner->m_params->m_beamSweepAngle / 2.0f)));
    fn_800091D0(&m_direction, &(const Vec3f &)fn_8040B67C(&rotation, &m_direction));
}

bool Beam::FindGround(Vec3f *position, Vec3f *normal) {
    void *object;
    UnkLine_80020A20 line;
    fn_80020A20(&line, &m_position,
                &(const Vec3f &)fn_8040CDD0(
                    &m_position, &(const Vec3f &)fn_8040CDA8(&m_direction, m_length)));
    UnkLineHits_80021FE4 *hits = fn_80021FE4(fn_80020D40(), &line, 1, 6, 0, 0);
    if (hits != 0) {
        object = fn_8002735C(hits)->m_object;
        AssignVector(*position, *fn_8001E9BC(object));
        AssignVector(*normal, *fn_800266D4(object));
        float distance = fn_8040CF50(&(const Vec3f &)((VectorSubtractFn)fn_8040CD74)(
            position, &m_position));
        if (m_length > distance) {
            m_length = distance;
        }
        fn_80026FB4(hits, 1);
        return true;
    }
    return false;
}

void Beam::CreateAttack() {
    m_attack = boost::shared_ptr<CharaColliAttack>(new CharaColliAttack);
    if (m_attack.get() != 0) {
        fn_80054DD0(m_attack.get(), 4, 0x100, 1.0f, boost::bind(&Beam::OnAttackHit, this, _1),
                    boost::bind(&Beam::OnAttackEnd, this, _1));
        m_attack->m_owner = m_owner;
    }
}

void Beam::UpdateAttack() {
    if (m_attack.get() != 0) {
        UnkColliContactNode_800587A0 *node;
        UnkLine_80020A20 line;
        fn_80020A20(&line, &m_position,
                    &(const Vec3f &)fn_8040CDD0(
                        &m_position, &(const Vec3f &)fn_8040CDA8(&m_direction, m_length)));
        void *world = fn_80020D40();
        void *manager = fn_80021B08();
        UnkLineHits_80021FE4 *hits = fn_80021FE4(world, &line, 3, 6, 0, 0);
        if (hits != 0) {
            for (UnkColliContactIter_8001EDFC it(fn_8001EE04(hits));
                 (node = it.m_node) != fn_8001EDEC(hits); fn_80027374(&it, 0)) {
                fn_800C40C4(manager, m_attack, node, 0);
            }
            fn_80026FB4(hits, 1);
        }
    }
}

void Beam::OnAttackHit(AttackCallbackParam &param) {
    AssignVector(*reinterpret_cast<Vec3f *>(param.field_0x8 + (0x34 - 0x8)), m_direction);
    *reinterpret_cast<float *>(param.field_0x8 + (0x4C - 0x8)) = 1.0f;
    param.m_damage = 1.0f;
}

void Beam::OnAttackEnd(AttackCallbackParam &param) {}

void Beam::StartCharge() {
    m_chargeEffect = fn_802DC998(m_owner->field_0x6C, m_matrix, 2);
    fn_801D54F4(fn_80053164(), 0x7E30, m_owner.px, 0, 1);
}

void Beam::StopCharge() {
    if (!m_chargeEffect.expired()) {
        m_chargeEffect.lock()->Stop();
        m_chargeEffect.reset();
    }
}

void Beam::UpdateHitEffect(const Vec3f *position, const Vec3f *normal) {
    if (m_hitEffect.expired()) {
        m_hitEffect = fn_802DC95C(m_owner->field_0x6C, 0x17);
    }
    if (!m_hitEffect.expired()) {
        boost::shared_ptr<EffectElement> effect = m_hitEffect.lock();
        fn_80425E4C(effect.get(), position);
    }
}

void Beam::StopHitEffect() {
    if (!m_hitEffect.expired()) {
        m_hitEffect.lock()->Stop();
        m_hitEffect.reset();
    }
}

PartsBase *Beam::Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                        const Mtx34f *matrix) {
    return new Beam(parent, owner, matrix);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss

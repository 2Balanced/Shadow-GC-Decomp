/* Boss::Diablon::Parts::Body (class name from RTTI): the Diablon boss's body, the root part with a
 * model. It builds the boss's other parts, puts eight colliders on nodes of its model whose attack
 * callbacks pass damage to the parts (ActionDamage), plays the motions the parts' actions ask for,
 * turns its upper body towards the player, handles its motions' sound and vibration events and
 * keeps the boss on the ground. File name inferred from the class. Names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/Diablon.h"
#include "Game/GadgetSound.h"
#include "Game/GeometryLine.h"
#include "Game/MathConstants.h"
#include "Game/MathTypes.h"
#include "Game/Vibration.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;
using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

class UnkHintManager_801FB180;
UnkHintManager_801FB180 *GetHintManager();

/* What CharInfo's word at 0x60 points to (struct name unknown; see Game/DiablonController.cpp). */
struct UnkCharTime {
    /* 0x0 */ float m_frameTime;
    /* 0x4 */ u32 field_0x4;
};

extern "C" {
/* The collider manager, a sphere and a capsule collider for owner, and the attack collision of a
 * collider (see Game/DiablonPartsBarrier.cpp and Game/DevilDoomCtrlUnit.cpp). */
void *fn_80021B08(void);
/* The resource loader (see Game/ResourceOneFile.h). */
void *fn_800122D4(void);
boost::shared_ptr<CharaColliBase> fn_800C7EA8(void *manager, const Vec3f *position, u32 flags,
                                              int arg3, int arg4,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, float radius);
boost::shared_ptr<CharaColliBase> fn_800C799C(void *manager, const Vec3f *position,
                                              const Vec3f *arg2, int arg3, int arg4, int arg5,
                                              int arg6, const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, const Vec3f *rotation,
                                              int arg10, float arg11, float arg12);
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
void fn_80096B64(CharaColliAttack *attack, int arg1, const AttackCallback &callback);
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
/* Whether the hit is to be ignored (see Game/EggLastMech.cpp). */
bool fn_80053538(AttackCallbackParam *param);
/* Rewards the attacker (see Game/ArkCannon.cpp). */
void fn_8020642C(const boost::shared_ptr<CharInfo> &attacker, int reward);

/* Effects: one at a position facing a direction, one following a matrix, and one of the boss's
 * effect set following a matrix (behaviour inferred). */
boost::weak_ptr<EffectElement> fn_801D2AFC(int arg0, const Vec3f *position, const Vec3f *direction,
                                           int id, u32 *arg4);
boost::weak_ptr<EffectElement> fn_801D2C28(int arg0, const Mtx34f *matrix, int id, u32 *arg3);
boost::weak_ptr<EffectElement> fn_802DC998(u32 effects, const Mtx34f *matrix, int id);

/* The sound manager, and plays sound id at the character info (see Game/BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, CharInfo *source, int arg3, int arg4);

/* Controller vibrations (see Game/BKTank.cpp). */
boost::weak_ptr<Vibration::RequestHandle> fn_8034DF48(int pattern, int arg1);
boost::weak_ptr<Vibration::RequestHandle> fn_8034DE08(int pattern, const Vec3f *position, int arg2);

/* The next event of a motion (behaviour inferred; pass the motion first, then 0). */
int fn_80415854(void *motion, int arg1, u16 *type, s32 **data, int arg4);

/* A 32-bit flag word (see Game/EggLastMech.h); fn_8006127C clears bit. */
void fn_80014E04(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* value clamped to [min, max], and the smaller of a and b (behaviour from their bodies). */
const float &fn_80070460(const float &value, const float &min, const float &max);
const float &fn_800704AC(const float &a, const float &b);
/* The arc cosine (behaviour inferred). */
float fn_801B1DB0(float value);

/* The ground line test (see Game/BlackDoomCtrlUnit.cpp) and the hit's result (behaviour
 * inferred). */
void *fn_80020D40(void);
void *fn_80021E24(void *manager, const Sonicteam::System::Geometry::Line *line, int arg2, int arg3,
                  int arg4, int arg5);
void fn_80356400(const Vec3f *position, const Vec3f *normal, float size);
void fn_800265A4(void *hits, int arg1);

/* The hint manager showing message id of branch (see Game/DiablonBehaviorBeam.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
/* The boss's damage (behaviour inferred). */
void fn_80336888(void *gauge, UnkClumpFade_80027D5C *fade, float arg2, float arg3, float arg4, float arg5);

/* Vector helpers. */
Vec3f fn_8040FB08(const Mtx34f *matrix); /* its translation */
}

extern const Vec3f lbl_805E07A0; /* the zero vector */
extern const Vec3f lbl_805E07B8;

/* Converts degrees to radians (name inferred; see Game/DiablonBehaviorEvent.cpp). */
inline float ToRadians(float degrees) {
    return degrees * (lbl_805FAD38 / 180.0f);
}

/* A length (name inferred). As a call it is not a constant expression, so the constant below is
 * stored by a static initializer, as in retail (as in Game/DiablonPartsBarrier.cpp). */
inline float Span(float length) {
    return length;
}

/* Unused (name inferred). */
static float s_height = Span(10.0f);

namespace Boss {
namespace Diablon {

extern "C" {
/* Changes the boss's health by amount (behaviour inferred). */
void fn_802D96B0(Diablon *diablon, int amount);
}

namespace Parts {

/* A collider of the body (struct name inferred): its radius, whether it is rebuilt as a capsule,
 * whether hits on it hurt the boss, and the motion hit id that strengthens it. */
struct ColliderInfo {
    /* 0x0 */ float m_radius;
    /* 0x4 */ u8 m_capsule;
    /* 0x5 */ u8 m_damage;
    /* 0x8 */ s32 m_hitId;
};

static const ColliderInfo s_colliders[8] = {
    {15.0f, 0, 1, -99}, {18.0f, 0, 0, -99}, {18.0f, 0, 0, -99}, {15.0f, 1, 0, -99},
    {14.0f, 1, 0, 1},   {17.0f, 1, 0, 1},   {14.0f, 1, 0, 0},   {17.0f, 1, 0, 0},
};

static const ModelNode s_nodes[23] = {
    {10, 0}, {9, 0},  {21, 0}, {20, 0}, {25, 0}, {23, 0}, {24, 0}, {22, 0},
    {13, 0}, {14, 0}, {15, 0}, {2, 0},  {1, 0},  {19, 0}, {18, 0}, {7, 0},
    {5, 0},  {8, 0},  {6, 0},  {12, 0}, {3, 0},  {4, 0},  {11, 0},
};

static const s32 s_motions[21] = {
    4032, 4060, 4070, 4046, 4056, 6000, 8000, 7000, 4010, 5001, 5002,
    5003, 5004, 4032, 4056, 4046, 4020, 4052, 4053, 4042, 4043,
};

static const ModelDesc s_modelDesc = {2, 10, s_nodes, 23, s_motions, 21};

class Body : public ModelBase {
  public:
    Body(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *parentMatrix);
    virtual ~Body();
    virtual void Tick(float dt);
    virtual void OnAction(const Action &action);

    /* Names inferred. */
    void HandleMotionEvents();
    void StartEffects();
    void StopEffects();
    void UpdateColliders();
    void CreateColliders();
    void CreateCapsules();
    boost::weak_ptr<CharaColliAttack> CreateAttack(const boost::shared_ptr<CharaColliBase> &collider,
                                                   const ColliderInfo *info);
    void SetAttackKinds(int hitId);
    void OnHitEnd(AttackCallbackParam &param);
    void OnHit(AttackCallbackParam &param);
    void OnDamage(AttackCallbackParam &param);

    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *parentMatrix);

    /* 0x28 */ boost::weak_ptr<EffectElement> m_effects[4];
    /* 0x48 */ boost::weak_ptr<EffectElement> m_hitEffect;
    /* 0x50 */ boost::shared_ptr<CharaColliBase> m_colliders[8];
    /* 0x90 */ boost::weak_ptr<CharaColliAttack> m_attacks[8];
    /* 0xD0 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0xEC */ u32 m_flags; /* 0 turning towards the player */
    /* 0xF0 */ float m_turn;
};
SIZE_ASSERT(Body, 0xF4);

/* The other parts (defined in their own units); only their factories are used here. */
class Charge {
  public:
    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node);
};
class ChargeMeter {
  public:
    /* The meters on the left (node 1) and the right (node 2) (names inferred). */
    static PartsBase *InstantiateLeft(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                      const Mtx34f *node);
    static PartsBase *InstantiateRight(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                       const Mtx34f *node);
};
class Head {
  public:
    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node, const Mtx34f *node2);
};
class Shoulder {
  public:
    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node);
};
class Barrier {
  public:
    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node);
};
class Beam {
  public:
    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node);
};
class Bomb {
  public:
    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node);
};

Body::Body(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *parentMatrix)
    : ModelBase(parent, "Body", owner, &s_modelDesc, parentMatrix) {
    fn_80014E04(&m_flags);
    field_0x1C = 1;
    Head::Instantiate(this, owner, GetNodeMatrix(0), GetNodeMatrix(6));
    ChargeMeter::InstantiateLeft(this, owner, GetNodeMatrix(1));
    ChargeMeter::InstantiateRight(this, owner, GetNodeMatrix(2));
    Shoulder::Instantiate(this, owner, GetNodeMatrix(3));
    Shoulder::Instantiate(this, owner, GetNodeMatrix(4));
    Charge::Instantiate(this, owner, GetNodeMatrix(7));
    Barrier::Instantiate(this, owner, GetNodeMatrix(7));
    StartEffects();
    CreateColliders();
}

Body::~Body() {
    StopEffects();
    if (!m_hitEffect.expired()) {
        m_hitEffect.lock()->Retire();
    }
}

void Body::Tick(float dt) {
    if (fn_80014E34(&m_flags, 0)) {
        float angle = fn_801B1DB0(fn_802D9570(m_owner.get()));
        float turn = angle / (60.0f * (lbl_805FAD38 / 180.0f));
        float min;
        float max = 0.05f + m_turn;
        min = m_turn - 0.05f;
        m_turn = fn_80070460(turn, min, max);
        m_turn = fn_800704AC(m_turn, 1.0f);
        SetMotionSpeed(m_turn);
    }
    ModelBase::Tick(dt);
    if (fn_80014E34(GetModelFlags(), 3)) {
        m_owner->m_flags.Set(4);
    } else {
        m_owner->m_flags.Reset(4);
    }
    UpdateColliders();
    HandleMotionEvents();
    void *manager = fn_80020D40();
    Sonicteam::System::Geometry::Line line;
    fn_800091D0(&line.m_start, &Vec3f(m_owner->m_position.x, 10.0f, m_owner->m_position.z));
    fn_800091D0(&line.m_end, &Vec3f(m_owner->m_position.x, -10.0f, m_owner->m_position.z));
    void *hits = fn_80021E24(manager, &line, 1, 12, 0, 0);
    if (hits != 0) {
        Vec3f position(m_owner->m_position.x, 0.2f, m_owner->m_position.z);
        Vec3f up(0.0f, 1.0f, 0.0f);
        fn_80356400(&position, &up, 10.0f);
        fn_800265A4(hits, 1);
    }
}

void Body::OnAction(const Action &action) {
    int open = m_owner->m_flags.Test(0) ? 0 : 1;
    fn_8006127C(&m_flags, 0);
    switch (action.m_type) {
    case 0:
        PlayMotion(open, 20.0f);
        break;
    case 1:
        switch (static_cast<const ActionMove &>(action).m_direction) {
        case 0:
            PlayMotion(2, 30.0f);
            fn_801D54F4(fn_80053164(), 0x7E3A, m_owner.get(), 0, 1);
            break;
        case 1:
            PlayMotion(3, 30.0f);
            fn_801D54F4(fn_80053164(), 0x7E3B, m_owner.get(), 0, 1);
            break;
        case 2:
            PlayMotions(open, open + 4, 10.0f);
            fn_80014E1C(&m_flags, 0);
            m_turn = 0.0f;
            break;
        default:
            PlayMotions(open, open + 6, 10.0f);
            fn_80014E1C(&m_flags, 0);
            m_turn = 0.0f;
            break;
        }
        break;
    case 2:
        PlayMotion(8, 10.0f);
        break;
    case 3:
        PlayMotion(9, 0.0f);
        break;
    case 4:
        PlayMotion(10, 0.0f);
        break;
    case 5:
        PlayMotion(open + 11, 10.0f);
        Beam::Instantiate(this, m_owner, GetNodeMatrix(5));
        break;
    case 6:
        PlayMotion(open + 13, 0.0f);
        break;
    case 7:
        PlayMotion(open, 32.0f);
        break;
    case 8:
        PlayMotion(open + 15, 10.0f);
        Bomb::Instantiate(this, m_owner, GetNodeMatrix(8));
        break;
    case 9:
        PlayMotion(open + 17, 0.0f);
        break;
    case 10:
        PlayMotion(open, 32.0f);
        break;
    case 11:
        SetMotionNode(22, 4030);
        fn_802D96B0(m_owner.get(), -static_cast<const ActionDamage &>(action).m_damage);
        if (!m_owner->m_flags.Test(6)) {
            m_owner->m_flags.Set(6);
            UnkHintManager_801FB180 *hint = GetHintManager();
            fn_801FAF00(hint, m_owner->m_params->field_0x94, 0, 1, 0);
            fn_801FAF00(hint, m_owner->m_params->field_0x98, 0, 0, 0);
        }
        fn_80336888(m_owner->m_controller, &m_owner->m_fade, 1.0f, 0.0f, 0.0f, 1.0f);
        break;
    case 12:
        m_hitEffect = fn_801D2C28(0, GetNodeMatrix(8), 0x228,
                                  &reinterpret_cast<UnkCharTime *&>(m_owner->field_0x60)->field_0x4);
        fn_801D54F4(fn_80053164(), 0x7E42, m_owner.get(), 0, 1);
        break;
    case 15:
        PlayMotion(19, 0.0f);
        break;
    case 16:
        PlayMotion(20, 0.0f);
        CreateCapsules();
        break;
    case 17:
        PlayMotion(21, 0.0f);
        break;
    }
}

void Body::HandleMotionEvents() {
    void *motion = GetMotion();
    int hitId = -1;
    u16 type;
    s32 *data;
    while (fn_80415854(motion, 0, &type, &data, 0)) {
        motion = 0;
        switch (type) {
        case 0xD000:
            hitId = *data;
            break;
        case 0x5003:
            switch (*data) {
            case 0x1D:
                fn_801D54F4(fn_80053164(), 0x7E41, m_owner.get(), 0, 1);
                fn_8034DF48(12, -2);
                break;
            case 0x1F:
                fn_801D54F4(fn_80053164(), 0x7E36, m_owner.get(), 0, 1);
                fn_8034DE08(8, &m_owner->m_position, -2);
                break;
            case 0x20:
                fn_801D54F4(fn_80053164(), 0x7E37, m_owner.get(), 0, 1);
                break;
            case 0x21:
                fn_8034DF48(12, -2);
                break;
            }
            break;
        }
    }
    SetAttackKinds(hitId);
}

void Body::StartEffects() {
    for (int i = 0; i < 4; i++) {
        const Mtx34f *node = GetNodeMatrix(i + 9);
        m_effects[i] = fn_802DC998(m_owner->field_0x6C, node, 16);
    }
    m_sound = fn_801D3DC8(0x7E38, reinterpret_cast<GadgetCharInfo *>(m_owner.get()), this);
}

void Body::StopEffects() {
    for (int i = 0; i < 4; i++) {
        if (!m_effects[i].expired()) {
            m_effects[i].lock()->Stop();
            m_effects[i].reset();
        }
    }
    m_sound.fn_801D455C();
}

void Body::UpdateColliders() {
    fn_80021B08();
    for (int i = 0; i < 8; i++) {
        if (m_colliders[i].get() != 0) {
            const Mtx34f *node = GetNodeMatrix(i + 13);
            Vec3f position;
            fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(node));
            m_colliders[i].get()->SetCharInfo(reinterpret_cast<CharInfo *>(&position));
        }
    }
}

void Body::CreateColliders() {
    fn_800122D4();
    void *manager = fn_80021B08();
    for (int i = 0; i < 8; i++) {
        m_colliders[i] = fn_800C7EA8(manager, &lbl_805E07A0, 0x202, 4, 0, m_owner, &lbl_805E07A0,
                                     s_colliders[i].m_radius);
        if (m_colliders[i].get() != 0) {
            m_colliders[i]->SetField2C(1);
            m_attacks[i] = CreateAttack(m_colliders[i], &s_colliders[i]);
        }
    }
}

void Body::CreateCapsules() {
    void *manager = fn_80021B08();
    for (int i = 0; i < 8; i++) {
        if (s_colliders[i].m_capsule) {
            m_colliders[i] = fn_800C799C(
                manager, &lbl_805E07A0,
                &Vec3f(ToRadians(90.0f), ToRadians(0.0f), ToRadians(0.0f)), 0, 0x202, 4, 0, m_owner,
                &Vec3f(0.0f, 0.0f, 80.0f), &lbl_805E07B8, 0, 80.0f, s_colliders[i].m_radius);
            if (m_colliders[i].get() != 0) {
                m_colliders[i]->SetField2C(1);
                m_attacks[i] = CreateAttack(m_colliders[i], &s_colliders[i]);
            }
        }
    }
}

boost::weak_ptr<CharaColliAttack> Body::CreateAttack(const boost::shared_ptr<CharaColliBase> &collider,
                                                     const ColliderInfo *info) {
    void *manager = fn_80021B08();
    boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, collider);
    boost::shared_ptr<CharaColliAttack> locked = attack.lock();
    if (locked.get() != 0) {
        if (info->m_damage) {
            fn_80096B64(locked.get(), 1, AttackCallback(boost::bind(&Body::OnDamage, this, _1)));
        }
        fn_80054DD0(locked.get(), 1, 1, 1.0f, AttackCallback(boost::bind(&Body::OnHit, this, _1)),
                    AttackCallback(boost::bind(&Body::OnHitEnd, this, _1)));
        locked->m_owner = m_owner;
    }
    return attack;
}

void Body::SetAttackKinds(int hitId) {
    for (int i = 0; i < 8; i++) {
        if (!fn_80053538(reinterpret_cast<AttackCallbackParam *>(&m_attacks[i]))) {
            boost::shared_ptr<CharaColliAttack> attack = m_attacks[i].lock();
            if (hitId == s_colliders[i].m_hitId) {
                attack->m_kind = 4;
            } else {
                attack->m_kind = 1;
            }
        }
    }
}

void Body::OnHit(AttackCallbackParam &param) {}

void Body::OnHitEnd(AttackCallbackParam &param) {}

void Body::OnDamage(AttackCallbackParam &param) {
    if (m_owner->m_executer != 0) {
        fn_802D9E4C(m_owner->m_executer, ActionDamage(param.m_damage));
    }
    param.m_result = 1;
    fn_801D2AFC(0, reinterpret_cast<Vec3f *>(param.field_0x8 + (0x18 - 0x8)),
                reinterpret_cast<Vec3f *>(param.field_0x8 + (0x24 - 0x8)), 0x225,
                &reinterpret_cast<UnkCharTime *&>(m_owner->field_0x60)->field_0x4);
    if (!fn_80053538(&param)) {
        boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock();
        boost::shared_ptr<CharInfo> chara = attacker->m_owner.lock();
        if (chara.get() != 0) {
            fn_8020642C(chara, 0x70);
        }
    }
    fn_801D54F4(fn_80053164(), 0x7E3C, m_owner.get(), 0, 1);
}

PartsBase *Body::Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                             const Mtx34f *parentMatrix) {
    return new Body(parent, owner, parentMatrix);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss

/* Torch (class name from RTTI: @unnamed@WeaponTorch_cpp@::Torch): a burning torch swung at close
 * range, weapon type 0x26 ("Torch0300"). It carries a flame effect at its tip, draws a trail while
 * it swings, and keeps a capsule attack collision stretched between where the tip was and where it
 * is now. Its pick-up item (TorchItem, RTTI name @unnamed@WeaponTorch_cpp@::TorchItem) burns with
 * the same flame. File name from the RTTI names. Method and member names are inferred. Definitions
 * are in reverse address order (deferred inlining emits functions last-defined first). */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/EffLocusMultiVtx.h"
#include "Game/GadgetUtil.h"
#include "Game/WeaponBase.h"
#include "Game/WeaponItem.h"
#include "Game/WeaponStickBase.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;
using Weapon::UnkWeaponEffect_80063428;
using Weapon::UnkWeaponEffectSet_80063428;
using Weapon::UnkWeaponModel_80063428;

class UnkColliderManager_80021B08;

extern boost::shared_ptr<CharInfo> lbl_805EE698;
extern const Vec3f lbl_805E07B8;

extern "C" {
/* Debug output (behaviour inferred). */
void fn_803A953C(const char *format, ...);
/* Sets up the pick-up item of weapon type type (behaviour inferred). */
void fn_8005B760(int type);
/* The translation of a matrix. */
Vec3f fn_8040FB08(const Mtx34f *matrix);
/* Whether task and all its parents are still running (behaviour inferred). */
bool fn_8004EAC0(Task *task);
/* The owner of a weapon (see WeaponVacuumBase.cpp). */
boost::weak_ptr<CharInfo> fn_80065034(Weapon::WeaponBase *weapon);
/* Starts effect id at position, pointing along direction, and moves an effect to a position
 * (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_80168C48(const Vec3f *position,
                                                           const Vec3f *direction, int id);
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
/* The collider manager, a free capsule collider, and the attack collision of a collider (see
 * DevilDoomCtrlUnit.cpp and ArkLaser.cpp). */
UnkColliderManager_80021B08 *fn_80021B08(void);
boost::shared_ptr<CharaColliBase> fn_800C799C(UnkColliderManager_80021B08 *manager,
                                              const Vec3f *position, const Vec3f *arg2, int arg3,
                                              int arg4, int arg5, int arg6,
                                              const boost::weak_ptr<CharInfo> &owner,
                                              const Vec3f *offset, const Vec3f *rotation,
                                              int arg10, float arg11, float arg12);
boost::weak_ptr<CharaColliAttack> fn_800C4374(UnkColliderManager_80021B08 *manager,
                                              boost::shared_ptr<CharaColliBase> collider);
/* Sets the two hit callbacks of an attack collision (see TurnOver.cpp). */
void fn_80054DD0(CharaColliAttack *attack, int arg1, int arg2, float arg3, const AttackCallback &onHit,
                 const AttackCallback &onEnd);
/* a + b, vector * scale, |v| and the Euler angles that turn the z axis onto a direction
 * (behaviour inferred). */
Vec3f fn_8040CDD0(const Vec3f *a, const Vec3f *b);
Vec3f fn_8040CDA8(const Vec3f *vector, float scale);
Vec3f fn_8040F7F8(const Vec3f *direction);
}

/* a - b by value (fn_8040CD74 stores the result through its first argument; see ArkCannon.cpp). */
typedef Vec3f (*VectorSubtractFn)(const Vec3f *a, const Vec3f *b);

namespace {

/* The torch lying in the stage: an item whose flame burns at the model's node 2. */
class TorchItem : public Weapon::Item {
  public:
    TorchItem(void *resource, const UnkWeaponEffectSet_80063428 *effects);
    virtual ~TorchItem();
    virtual void Tick(float dt);

    /* Ends, starts and moves the flame. */
    void StopFlame();
    void StartFlame();
    void UpdateFlame();

    static Weapon::Item *CreateItem();

    /* 0x10 */ Vec3f m_flamePosition;
    /* 0x1C */ boost::weak_ptr<Effect::System::EffectElement> m_flame;
};
SIZE_ASSERT(TorchItem, 0x24);

class Torch : public Weapon::WeaponStickBase {
  public:
    Torch(const boost::weak_ptr<CharInfo> &owner, int type, float reach, int model,
          const UnkWeaponEffectSet_80063428 *effects);
    virtual ~Torch();
    virtual void SetTransform(const Vec3f *position, const Vec3f *rotation);
    virtual void Tick(float dt);
    virtual void Show();
    virtual void Hide();
    virtual void SwingHeld();
    virtual void SwingReleased();

    /* Moves the blade, the trail, the flame and the attack to the model's tip and root. */
    void UpdateBlade();
    /* Ends, starts and moves the flame. */
    void StopFlame();
    void StartFlame();
    void UpdateFlame();
    /* Stretches the attack collision from the last position of the root to the current one. */
    void UpdateAttack();
    /* Creates the attack collision, and drops it. */
    void CreateCollider();
    void DropCollider();
    /* The callbacks of the collision's attack: hitting something and the end of a hit. */
    void OnHit(AttackCallbackParam &param);
    void OnHitEnd(AttackCallbackParam &param);
    /* Ends the trail. */
    void EndTrail() {
        if (m_trail != 0) {
            m_trail->fn_802DE9DC();
            m_trail = 0;
        }
    }

    static Weapon::WeaponBase *Instantiate(const boost::weak_ptr<CharInfo> &owner);

    /* 0x3C */ Vec3f m_points[2]; /* tip and root, for the trail */
    /* 0x54 */ Test::EffLocusMultiVtx *m_trail;
    /* 0x58 */ Vec3f m_flamePosition;
    /* 0x64 */ boost::weak_ptr<Effect::System::EffectElement> m_flame;
    /* 0x6C */ u8 m_hidden;
    /* 0x70 */ Vec3f m_attackPosition;
    /* 0x7C */ Vec3f m_lastAttackPosition;
    /* 0x88 */ boost::shared_ptr<CharaColliBase> m_collider;
};
SIZE_ASSERT(Torch, 0x90);

} // namespace

/* A distance (name inferred). As a call it is not a constant expression, so the values below are
 * stored by the static initializer, as in retail. */
inline float Span(float length) {
    return length;
}

/* The radius of the attack collision. */
static const float s_attackRadius = Span(10.0f);

Torch::Torch(const boost::weak_ptr<CharInfo> &owner, int type, float reach, int model,
             const UnkWeaponEffectSet_80063428 *effects)
    : Weapon::WeaponStickBase(owner, type, reach) {
    m_hidden = false;
    m_model = new UnkWeaponModel_80063428(m_owner, model, effects);
    StartFlame();
    CreateCollider();
    m_trail = 0;
}

Torch::~Torch() {
    EndTrail();
    DropCollider();
    StopFlame();
    delete m_model;
}

void Torch::SetTransform(const Vec3f *position, const Vec3f *rotation) {
    Weapon::WeaponBase::SetTransform(position, rotation);
    m_model->fn_800632E8(position, rotation);
}

void Torch::Tick(float dt) {
    Weapon::WeaponStickBase::Tick(dt);
    m_model->fn_800632A0(dt);
    UpdateBlade();
}

void Torch::Show() {
    if (m_model != 0) {
        m_model->fn_80063130();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9BC();
    }
    if (!m_flame.expired()) {
        m_flame.lock()->Show();
    }
    m_hidden = false;
}

void Torch::Hide() {
    if (m_model != 0) {
        m_model->fn_80063154();
    }
    if (m_trail != 0) {
        m_trail->fn_802DE9CC();
    }
    if (!m_flame.expired()) {
        m_flame.lock()->Hide();
    }
    m_hidden = true;
}

void Torch::SwingHeld() {
    if (m_trail == 0 && !m_hidden) {
        UnkColor_8040D2EC color(0x80, 0x20, 0);
        m_trail = new Test::EffLocusMultiVtx(10, 2, m_points, 0, &color);
        UpdateBlade();
    }
    Weapon::WeaponStickBase::SwingHeld();
}

void Torch::SwingReleased() {
    EndTrail();
    Weapon::WeaponStickBase::SwingReleased();
}

void Torch::UpdateBlade() {
    Vec3f tip;
    fn_800091EC(&tip, &(const Vec3f &)fn_8040FB08(m_model->fn_80063234(0)));
    Vec3f root;
    fn_800091EC(&root, &(const Vec3f &)fn_8040FB08(m_model->fn_80063234(1)));
    fn_80067870(&tip, &root);
    if (m_trail != 0) {
        if (!fn_8004EAC0(m_trail)) {
            m_trail = 0;
        } else {
            fn_800091D0(&m_points[0], &tip);
            fn_800091D0(&m_points[1], &root);
        }
    }
    fn_800091D0(&m_flamePosition, &root);
    fn_800091D0(&m_attackPosition, &root);
    UpdateFlame();
    UpdateAttack();
}

void Torch::UpdateFlame() {
    if (!m_flame.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_flame.lock();
        if (effect.get() != 0) {
            fn_80425E4C(effect.get(), &m_flamePosition);
        }
    }
}

void Torch::StartFlame() {
    if (m_flame.expired()) {
        Vec3f up(0.0f, 1.0f, 0.0f);
        m_flame = fn_80168C48(&m_flamePosition, &up, 0x19);
    }
}

void Torch::StopFlame() {
    if (!m_flame.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_flame.lock();
        if (effect.get() != 0) {
            effect->Stop();
        }
        m_flame.reset();
    }
}

void Torch::UpdateAttack() {
    if (m_collider) {
        if (!m_collider->GetField2A()) {
            fn_800091D0(&m_lastAttackPosition, &m_attackPosition);
            m_collider->TurnOn(-1);
        }
        Vec3f direction;
        ConstructVector(&direction, ((VectorSubtractFn)fn_8040CD74)(&m_attackPosition, &m_lastAttackPosition));
        float halfLength = 0.5f * fn_8040CF50(&direction);
        Vec3f rotation = fn_8040F7F8(&direction);
        /* Slot 0x3C (SetCharInfo in Game/CharaColli.h, a name inferred from other colliders) takes
         * the collision's centre here. */
        const Vec3f &center =
            fn_8040CDA8(&(const Vec3f &)fn_8040CDD0(&m_attackPosition, &m_lastAttackPosition), 0.5f);
        m_collider->SetCharInfo((CharInfo *)&center);
        m_collider->SetRotation(&rotation);
        m_collider->SetSize(s_attackRadius, halfLength, halfLength, -2);
        fn_800091D0(&m_lastAttackPosition, &m_attackPosition);
    }
}

void Torch::CreateCollider() {
    if (m_collider) {
        DropCollider();
    }
    UnkColliderManager_80021B08 *manager = fn_80021B08();
    m_collider = fn_800C799C(manager, &lbl_805E07A0, &lbl_805E07B8, 0, 0x40000, 3, 0, lbl_805EE698,
                             &lbl_805E07A0, &lbl_805E07B8, 0, 0.0f, s_attackRadius);
    boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(manager, m_collider);
    boost::shared_ptr<CharaColliAttack> collision = attack.lock();
    if (collision) {
        fn_80054DD0(collision.get(), 5, 0x400, 0.0f, boost::bind(&Torch::OnHit, this, _1),
                    boost::bind(&Torch::OnHitEnd, this, _1));
        collision->m_owner = fn_80065034(this);
    }
    m_collider->TurnOff(-1);
}

void Torch::DropCollider() {
    if (m_collider) {
        m_collider.reset();
    }
}

void Torch::OnHit(AttackCallbackParam &param) {
    boost::shared_ptr<CharaColliAttack> target =
        reinterpret_cast<boost::weak_ptr<CharaColliAttack> *>(param.field_0x8)->lock();
    if (target) {
        boost::shared_ptr<CharInfo> chara = target->m_owner.lock();
        if (chara && chara->field_0x5C != 4) {
            param.field_0x8[0x30 - 0x8] = 1;
        }
    }
}

void Torch::OnHitEnd(AttackCallbackParam &param) {}

TorchItem::TorchItem(void *resource, const UnkWeaponEffectSet_80063428 *effects)
    : Weapon::Item(resource, effects) {}

TorchItem::~TorchItem() {
    StopFlame();
}

void TorchItem::Tick(float dt) {
    Weapon::Item::Tick(dt);
    StartFlame();
    UpdateFlame();
}

void TorchItem::UpdateFlame() {
    UnkWeaponModel_80063428 *model = reinterpret_cast<UnkWeaponModel_80063428 *>(field_0x4);
    if (model != 0) {
        fn_800091D0(&m_flamePosition, &(const Vec3f &)fn_8040FB08(model->fn_80063234(2)));
    }
    if (!m_flame.expired()) {
        fn_80425E4C(m_flame.lock().get(), &m_flamePosition);
    }
}

void TorchItem::StartFlame() {
    if (m_flame.expired()) {
        Vec3f up(0.0f, 1.0f, 0.0f);
        m_flame = fn_80168C48(&m_flamePosition, &up, 0x19);
    }
}

void TorchItem::StopFlame() {
    if (!m_flame.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_flame.lock();
        if (effect.get() != 0) {
            effect->Stop();
        }
        m_flame.reset();
    }
}

/* Defined after the code that binds the callbacks: retail places the bound type's RTTI name and
 * record before the task name in .rodata and .sdata. */
static const char *s_taskName = "Torch0300";

/* The effects of the weapon model and of the item's model (meanings inferred). */
static const UnkWeaponEffect_80063428 s_effects[] = {
    {-1, 0x2328, 0x26, -1, 0, 0},
};

static const u32 s_sounds[] = {0x233C, 0x233D};

static const UnkWeaponEffectSet_80063428 s_effectSet = {s_effects, 1, s_sounds, 2};

static const UnkWeaponEffect_80063428 s_itemEffects[] = {
    {-1, 0x801, 0x26, -1, 0, 0},
};

static const u32 s_itemSounds[] = {0x2328, 0x801, 0x233D};

static const UnkWeaponEffectSet_80063428 s_itemEffectSet = {s_itemEffects, 1, s_itemSounds, 3};

extern "C" void fn_8023DDB4(void) {
    fn_803A953C("%s::Initialize\n", s_taskName);
    fn_8006B50C(0x26, 0);
    fn_8005B760(0x26);
}

extern "C" void fn_8023DD84(void) {
    fn_803A953C("%s::Finalize\n", s_taskName);
}

Weapon::WeaponBase *Torch::Instantiate(const boost::weak_ptr<CharInfo> &owner) {
    return new Torch(owner, 0x26, 1.0f, 1, &s_effectSet);
}

Weapon::Item *TorchItem::CreateItem() {
    return new TorchItem((void *)1, &s_itemEffectSet);
}

UnkWeaponTaskInfo lbl_80547D18 = {
    s_taskName, fn_8023DDB4, fn_8023DD84, 0, 0, 0x80, 0x01020400, 0, 0,
};

UnkWeaponTypeInfo lbl_80547D3C = {
    s_taskName, Torch::Instantiate, TorchItem::CreateItem, 0, Span(1.0f), Span(5.0f), 4, 3, 0x2001,
    0.0f, 0.0f, 0,
};

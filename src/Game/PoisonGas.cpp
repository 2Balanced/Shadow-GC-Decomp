/* PoisonGas::PoisonGas (class name from RTTI): the POISON_GAS gadget, a cloud of gas shown by an
 * effect, with a sphere collider and a target at the owner's position. Characters that touch the
 * collider are sent message 0x1C (fn_8020642C), and the gas then dissipates; so does it when a
 * character command of type 0x14 reaches it, or when an attack whose flag bit 4 is set hits it.
 * Dissipating launches effect 0x29F and finishes the gadget. The "scale X/Y/Z" parameters are read
 * but not used here. File name inferred from the class. Method names marked "(name inferred)" are
 * not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first), except the static HitCharacter, which is defined before the constructor:
 * that puts its code where the original has it. */
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "Effect/System/EffectElement.h"
#include "Game/TNumEdit.h"
/* After TNumEdit.h: the order of the casts sets the order of the RTTI base lists in .data. */
#include "Game/CharaColliBaseCharInfo.h"
#include "Game/CharCommand.h"
#include "Game/CharCommandUnit.h"
#include "Game/CharInfo.h"
#include "Game/CharaColli.h"
#include "Game/CharaColliAttack.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetColliDetectArgs.h"
#include "Game/GadgetCollision.h"
#include "Game/GadgetClassDesc.h"
#include "Game/GadgetUtil.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using SonicteamUSA::System::CharaColli::AttackCallback;
using SonicteamUSA::System::CharaColli::AttackCallbackParam;
using SonicteamUSA::System::CharaColli::CharaColliAttack;
using SonicteamUSA::System::CharaColli::CharaColliBase;

/* Target (class name from RTTI): a point other objects can aim at (meaning inferred). */
class Target {
  public:
    /* Sets the radius around the point (name inferred). */
    void SetRadius(const float &radius) {
        m_radius = radius;
    }

    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);

    /* 0x00 */ u8 field_0x0[0x20];
    /* 0x20 */ float m_radius;
};

extern "C" {
/* The collider manager (behaviour inferred). */
void *fn_80021B08(void);
/* Creates a sphere collider for charInfo (behaviour inferred; the meanings of the arguments are
 * unknown). */
boost::shared_ptr<CharaColliBase> fn_800C7DEC(void *manager, const boost::weak_ptr<CharInfo> &charInfo,
                                              int flags, int arg3, int arg4, const Vec3f *offset,
                                              float radius);
/* Returns the attack collision of a collider (behaviour inferred). */
boost::weak_ptr<CharaColliAttack> fn_800C4374(void *manager, boost::shared_ptr<CharaColliBase> collider);
/* Wraps collider in a new handle, and advances the handle by dt (behaviour inferred). */
boost::shared_ptr<GadgetCharCollision> fn_8015B8C4(boost::shared_ptr<CharaColliBase> collider);
void fn_8015BE9C(GadgetCharCollision *handle, float dt);
/* Creates the command receiver of a character info unless it has one (behaviour inferred). */
void fn_8015CF7C(GadgetCharInfo *charInfo);
/* Starts effect id at position, and moves an effect (behaviour inferred). */
boost::weak_ptr<Effect::System::EffectElement> fn_8015D794(const Vec3f *position, int id);
void fn_80425E4C(Effect::System::EffectElement *effect, const Vec3f *position);
/* Creates a target for the character with the given flags, and removes a gadget's target
 * (behaviour inferred). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &character, int flags);
void fn_801E7108(boost::shared_ptr<Target> *target);
/* Sends character a message (behaviour inferred). */
int fn_8020642C(const boost::shared_ptr<CharInfo> &character, int message);
/* Tells the owner that the gadget is done (behaviour inferred). */
void fn_8015A794(Gadget *gadget);
}

/* Returns x unchanged (name inferred). Being initialised through a function call, the radius
 * below is set up at startup. */
inline float Plain(float x) {
    return x;
}

/* The radius of the gadget's target (name inferred). */
static float s_targetRadius = Plain(40.0f);

namespace PoisonGas {

/* The three parameters the owner supplies (names from the parameter table). */
struct PoisonGasParams {
    PoisonGasParams() : scaleX(1.0f), scaleY(1.0f), scaleZ(1.0f) {}

    /* 0x0 */ float scaleX;
    /* 0x4 */ float scaleY;
    /* 0x8 */ float scaleZ;
};
SIZE_ASSERT(PoisonGasParams, 0xC);

class PoisonGas : public Gadget {
  public:
    PoisonGas(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~PoisonGas();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gas under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The attack collision's callback (name inferred). */
    void OnAttack(AttackCallbackParam &param);
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Lets the effect wind down (name inferred). */
    void StopEffect();
    /* The collision handle's callback (name inferred). */
    void OnColliDetect(ColliDetectArgs &args);
    /* Moves the effect and the target to the gas (name inferred). */
    void UpdateEffect();
    /* Copies the owner's position, rotation, switch id and parameters, and resets the state (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* Member names from 0x38 on are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x40 */ boost::shared_ptr<Target> m_target;
    /* 0x48 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x50 */ s32 m_state; /* 0 new, 1 active, 2 dissipating */
    /* 0x54 */ PoisonGasParams m_params;
    /* 0x60 */ u8 m_switchId;
    /* 0x64 */ u32 pad_0x64; /* the class size is a multiple of 8 */
};

/* What the collision callback passes to the hit function (name inferred). */
struct PoisonGasHitContext {
    PoisonGasHitContext() : m_hit(false) {}

    /* 0x0 */ boost::weak_ptr<GadgetCharInfo> m_chara;
    /* 0x8 */ bool m_hit;
};
SIZE_ASSERT(PoisonGasHitContext, 0xC);

} // namespace PoisonGas

SIZE_ASSERT(PoisonGas::PoisonGas, 0x68);

namespace PoisonGas {

const char *PoisonGas::s_taskName = "PoisonGas";

/* Sends the character message 0x1C and records the hit (name inferred). */
static void HitCharacter(boost::shared_ptr<CharInfo> hit, void *user) {
    PoisonGasHitContext *context = static_cast<PoisonGasHitContext *>(user);

    if (context != 0) {
        fn_8020642C(hit, 0x1C);
        context->m_hit = true;
    }
}

PoisonGas::PoisonGas(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x19, 4, "PoisonGas");
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&PoisonGas::OnCommand, this, _1)));
    LoadFromOwner(owner);
    m_effect = fn_8015D794(&m_chara->m_position, 0x8B);
    {
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C7DEC(fn_80021B08(), m_chara, 0, 10, 0, &lbl_805E07A0, 10.0f);
        m_handle = fn_8015B8C4(collider);
        fn_800F5980(m_handle.get(), boost::bind(&PoisonGas::OnColliDetect, this, _1));
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), collider);
        if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
            target->field_0x54 = 0;
            target->field_0x55 = 1;
            target->m_attackCallback = AttackCallback(boost::bind(&PoisonGas::OnAttack, this, _1));
        }
    }
    m_target = fn_801E7128(m_chara, 0x80);
    m_target->SetRadius(s_targetRadius);
    UpdateEffect();
}

PoisonGas::~PoisonGas() {
    fn_801E7108(&m_target);
    StopEffect();
}

void PoisonGas::Behave(float dt) {
    fn_8015BE9C(m_handle.get(), dt);

    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        break;
    case 2:
        fn_8015D794(&m_chara->m_position, 0x29F);
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void PoisonGas::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateEffect();
}

void PoisonGas::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9E60(owner, &m_switchId);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
}

void PoisonGas::UpdateEffect() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        fn_80425E4C(effect.get(), &m_chara->m_position);
    }
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
}

void PoisonGas::OnColliDetect(ColliDetectArgs &args) {
    PoisonGasHitContext context;

    context.m_chara = m_chara;
    fn_8015B004(args, HitCharacter, &context);
    if (context.m_hit) {
        m_state = 2;
    }
}

void PoisonGas::StopEffect() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        effect->Retire();
    }
}

void PoisonGas::OnCommand(CharCommand &command) {
    if (m_state == 1 && command.TypeId() == 0x14) {
        m_state = 2;
    }
}

void PoisonGas::OnAttack(AttackCallbackParam &param) {
    param.m_result = 5;
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if (!(attacker->m_flags & 0x10)) {
                param.m_result = 5;
                return;
            }
            param.m_result = 3;
            m_state = 2;
        }
    }
}

} // namespace PoisonGas

extern "C" void fn_80110818(void) {}

extern "C" void fn_80110814(void) {}

void PoisonGas::PoisonGas::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new PoisonGas(parent, owner);
}

static GadgetParamDesc lbl_80526924[] = {
    {4, "scale X", 0, 0, 0, 1.0f, 0.5f, 2.0f, fn_800CBC20, 0},
    {4, "scale Y", 0, 0, 0, 1.0f, 0.5f, 2.0f, fn_800CBC20, 0},
    {4, "scale Z", 0, 0, 0, 1.0f, 0.5f, 2.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805269C4 = {
    "POISON_GAS", fn_80110818, fn_80110814, PoisonGas::PoisonGas::Instantiate, 0, 0, 0x35, 1, 0, 10,
    lbl_80526924,
};

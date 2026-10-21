/* Fire::Fire (class name from RTTI): the FIRE gadget, a fire shown by an effect, with a sphere
 * collider and a target at the owner's position. It goes out when a character command of type 0x14
 * reaches it, or when an attack collision touches it; an attacker whose flags include one of the
 * bits 0x95 also gets message 0x1B (fn_8020642C). Going out launches effect 0x29C and finishes the
 * gadget. The "scale X/Y/Z" parameters are read but not used here. File name inferred from the
 * class. Method names marked "(name inferred)" are not original. Definitions are in reverse
 * address order (deferred inlining emits functions last-defined first). */
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

namespace Fire {

/* The three parameters the owner supplies (names from the parameter table). */
struct FireParams {
    FireParams() : scaleX(1.0f), scaleY(1.0f), scaleZ(1.0f) {}

    /* 0x0 */ float scaleX;
    /* 0x4 */ float scaleY;
    /* 0x8 */ float scaleZ;
};
SIZE_ASSERT(FireParams, 0xC);

class Fire : public Gadget {
  public:
    Fire(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~Fire();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the fire under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* The attack collision's two callbacks (names inferred). */
    void OnHit(AttackCallbackParam &param);
    void OnAttack(AttackCallbackParam &param);
    /* The command unit's callback (name inferred). */
    void OnCommand(CharCommand &command);
    /* Lets the effect wind down (name inferred). */
    void StopEffect();
    /* Moves the effect and the target to the fire (name inferred). */
    void UpdateEffect();
    /* Copies the owner's position, rotation and parameters, and resets the state (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* Member names are inferred. */
    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ boost::shared_ptr<GadgetCharCollision> m_handle;
    /* 0x40 */ boost::shared_ptr<Target> m_target;
    /* 0x48 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x50 */ s32 m_state; /* 0 new, 1 burning, 2 going out */
    /* 0x54 */ FireParams m_params;
};

} // namespace Fire

SIZE_ASSERT(Fire::Fire, 0x60);

namespace Fire {

const char *Fire::s_taskName = "Fire";

Fire::Fire(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CB88(0x1A, 4, "Fire");
    fn_8015CF7C(m_chara.get());
    fn_800729B0(&m_chara->m_commandUnit->m_callback,
                &(const CharCommandCallback &)CharCommandCallback(
                    boost::bind(&Fire::OnCommand, this, _1)));
    LoadFromOwner(owner);
    m_effect = fn_8015D794(&m_chara->m_position, 0xA6);
    {
        boost::shared_ptr<CharaColliBase> collider =
            fn_800C7DEC(fn_80021B08(), m_chara, 0, 10, 0, &lbl_805E07A0, 10.0f);
        m_handle = fn_8015B8C4(collider);
        boost::weak_ptr<CharaColliAttack> attack = fn_800C4374(fn_80021B08(), collider);
        if (boost::shared_ptr<CharaColliAttack> target = attack.lock()) {
            target->field_0x54 = 1;
            target->m_kind = 1;
            target->m_power = 1.0f;
            target->m_hitCallback = AttackCallback(boost::bind(&Fire::OnHit, this, _1));
            target->field_0x55 = 1;
            target->m_attackCallback = AttackCallback(boost::bind(&Fire::OnAttack, this, _1));
        }
    }
    m_target = fn_801E7128(m_chara, 0x80);
    m_target->SetRadius(s_targetRadius);
    UpdateEffect();
}

Fire::~Fire() {
    fn_801E7108(&m_target);
    StopEffect();
}

void Fire::Behave(float dt) {
    fn_8015BE9C(m_handle.get(), dt);

    switch (m_state) {
    case 0:
        m_state = 1;
        break;
    case 1:
        break;
    case 2:
        fn_8015D794(&m_chara->m_position, 0x29C);
        fn_8015A794(this);
        m_flags |= 1;
        break;
    }
}

void Fire::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    LoadFromOwner(owner);
    UpdateEffect();
}

void Fire::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    m_state = 0;
}

void Fire::UpdateEffect() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        fn_80425E4C(effect.get(), &m_chara->m_position);
    }
    if (m_target.get() != 0) {
        m_target->fn_800CCE38(&m_chara->m_position);
    }
}

void Fire::StopEffect() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        effect->Retire();
    }
}

void Fire::OnCommand(CharCommand &command) {
    if (m_state == 1 && command.TypeId() == 0x14) {
        m_state = 2;
    }
}

void Fire::OnAttack(AttackCallbackParam &param) {
    if (m_state == 1) {
        if (boost::shared_ptr<CharaColliAttack> attacker = param.m_attacker.lock()) {
            if (!(attacker->m_flags & 0x95)) {
                param.m_result = 5;
                return;
            }
            if (boost::shared_ptr<CharInfo> owner = attacker->m_owner.lock()) {
                fn_8020642C(owner, 0x1B);
            }
            param.m_result = 3;
            m_state = 2;
            return;
        }
    }
    param.m_result = 5;
}

void Fire::OnHit(AttackCallbackParam &param) {
    if (m_state == 1) {
        m_state = 2;
    }
}

} // namespace Fire

extern "C" void fn_8010403C(void) {}

extern "C" void fn_80104038(void) {}

void Fire::Fire::Instantiate(UnkPlacement_800C9CD0 *owner) {
    Task *parent = fn_801EB304(fn_8001C4F8(), 11);
    new Fire(parent, owner);
}

static GadgetParamDesc lbl_80525F04[] = {
    {4, "scale X", 0, 0, 0, 1.0f, 0.5f, 2.0f, fn_800CBC20, 0},
    {4, "scale Y", 0, 0, 0, 1.0f, 0.5f, 2.0f, fn_800CBC20, 0},
    {4, "scale Z", 0, 0, 0, 1.0f, 0.5f, 2.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_80525FA4 = {
    "FIRE", fn_8010403C, fn_80104038, Fire::Fire::Instantiate, 0, 0, 0x34, 1, 0, 10, lbl_80525F04,
};

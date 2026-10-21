/* SetParticle (class name from RTTI): the PARTICLE gadget. It launches effect number m_params.number
 * from the effect manager at the owner's position, scaled by the owner's X/Y/Z scale parameters,
 * and keeps it attached to its matrix until the owner changes the number or the gadget is
 * destroyed. File name inferred from the class. Method names marked "(name inferred)" are not
 * original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/Gadget.h"
#include "Game/GadgetCharInfo.h"
#include "Game/GadgetClassDesc.h"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

/* Builds the character info of a gadget type (behaviour inferred; the argument's meaning is
 * unknown). */
extern "C" boost::shared_ptr<GadgetCharInfo> fn_8015CE20(int arg0);
/* Launches effect number at charInfo's position, offset by up, through the effect manager and
 * returns the running element (behaviour inferred). */
extern "C" boost::weak_ptr<Effect::System::EffectElement>
fn_80168C48(GadgetCharInfo *charInfo, const Vec3f *up, int number);
/* Sets matrix to the identity (behaviour inferred). */
extern "C" void fn_8040B6F8(Mtx34f *matrix);

/* The four parameters the owner supplies (names from the parameter table). */
struct SetParticleParams {
    SetParticleParams() : number(0), scaleX(1.0f), scaleY(1.0f), scaleZ(1.0f) {}

    /* 0x0 */ int number;
    /* 0x4 */ float scaleX;
    /* 0x8 */ float scaleY;
    /* 0xC */ float scaleZ;
};
SIZE_ASSERT(SetParticleParams, 0x10);

class SetParticle : public Gadget {
  public:
    SetParticle(Task *parent, UnkPlacement_800C9CD0 *owner);
    virtual ~SetParticle();
    virtual void Behave(float dt);
    virtual void ReloadPlacement(UnkPlacement_800C9CD0 *owner);

    /* Creates the gadget under task layer 11 (name inferred). */
    static void Instantiate(UnkPlacement_800C9CD0 *owner);

    /* Copies the owner's position, rotation and parameters and rebuilds the matrix (name
     * inferred). */
    void LoadFromOwner(UnkPlacement_800C9CD0 *owner);

    static const char *s_taskName;

    /* 0x30 */ boost::shared_ptr<GadgetCharInfo> m_chara;
    /* 0x38 */ SetParticleParams m_params;
    /* 0x48 */ boost::weak_ptr<Effect::System::EffectElement> m_effect;
    /* 0x50 */ Mtx34f m_matrix;
};
SIZE_ASSERT(SetParticle, 0x80);

const char *SetParticle::s_taskName = "SetParticle";

SetParticle::SetParticle(Task *parent, UnkPlacement_800C9CD0 *owner) : Gadget(parent, owner) {
    m_name = s_taskName;
    m_chara = fn_8015CE20(0x39);
    LoadFromOwner(m_owner);

    Vec3f up(0.0f, 1.0f, 0.0f);
    m_effect = fn_80168C48(m_chara.px, &up, m_params.number);
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        if (effect.px != 0) {
            effect->SetMatrix(&m_matrix);
        }
    }
}

SetParticle::~SetParticle() {
    if (!m_effect.expired()) {
        boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
        if (effect.px != 0) {
            effect->Retire();
        }
        m_effect.reset();
    }
}

void SetParticle::Behave(float dt) {}

void SetParticle::ReloadPlacement(UnkPlacement_800C9CD0 *owner) {
    int number = m_params.number;

    LoadFromOwner(owner);
    if (number != m_params.number) {
        if (!m_effect.expired()) {
            boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
            if (effect.px != 0) {
                effect->Retire();
            }
            m_effect.reset();
        }

        Vec3f up(0.0f, 1.0f, 0.0f);
        m_effect = fn_80168C48(m_chara.px, &up, m_params.number);
        if (!m_effect.expired()) {
            boost::shared_ptr<Effect::System::EffectElement> effect = m_effect.lock();
            if (effect.px != 0) {
                effect->SetMatrix(&m_matrix);
            }
        }
    }
}

void SetParticle::LoadFromOwner(UnkPlacement_800C9CD0 *owner) {
    fn_800C9F14(owner, &m_chara->m_position);
    fn_800C9ED4(owner, &m_chara->m_rotation);
    fn_800C9DBC(owner, &m_params, sizeof(m_params));
    fn_8040B6F8(&m_matrix);
    fn_8040B59C(&m_matrix, &m_chara->m_position);
    fn_8040B848(&m_matrix, m_chara->m_rotation.y);
    fn_8040B884(&m_matrix, m_chara->m_rotation.x);
    fn_8040B80C(&m_matrix, m_chara->m_rotation.z);
    fn_8040B720(&m_matrix, m_params.scaleX, m_params.scaleY, m_params.scaleZ);
}

extern "C" void fn_8011DC94(void) {}

extern "C" void fn_8011DC90(void) {}

void SetParticle::Instantiate(UnkPlacement_800C9CD0 *owner) {
    new SetParticle(fn_801EB304(fn_8001C4F8(), 11), owner);
}

static GadgetParamDesc lbl_805274F0[] = {
    {1, "number", 0, 0xFFFF0000, 0x10000, 0.0f, 0.0f, 0.0f, fn_800CBC20, 0},
    {4, "scale X", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "scale Y", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {4, "scale Z", 0, 0, 0, 1.0f, 0.0f, 65536.0f, fn_800CBC20, 0},
    {0},
};

GadgetClassDesc lbl_805275B8 = {
    "PARTICLE", fn_8011DC94, fn_8011DC90, SetParticle::Instantiate, 0, 0, 0x258A, 1, 0, 10, lbl_805274F0,
};

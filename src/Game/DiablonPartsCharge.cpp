/* Boss::Diablon::Parts::Charge (class name from RTTI): the Diablon boss's charge, a part on a node
 * of the body. While the boss charges (flag 1) and its charge level (Diablon field 0xD0) is below
 * 5, it raises the level once per charge time with a flash effect and a sound, showing hints as
 * the charge starts and completes; each level has its own glow effect. Actions 12-14 stop the
 * charge and take levels away. File name inferred from the class. Names marked "(name inferred)"
 * are not original. Definitions are in reverse address order (deferred inlining emits functions
 * last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "Game/GadgetSound.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

class UnkHintManager_801FB180;
UnkHintManager_801FB180 *GetHintManager();

extern "C" {
/* A 32-bit flag word (see Game/EggLastMech.h); fn_8006127C clears bit. */
void fn_80014E04(u32 *flags);
void fn_80014E10(u32 *flags);
void fn_80014E1C(u32 *flags, int bit);
bool fn_80014E34(u32 *flags, int bit);
void fn_8006127C(u32 *flags, int bit);

/* The hint manager showing message id of branch (see Game/DiablonBehaviorBeam.cpp). */
s32 fn_801FAF00(UnkHintManager_801FB180 *manager, s32 id, s32 branch, int arg3, int arg4);
/* The sound manager, and plays sound id at the character info (see Game/BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, CharInfo *source, int arg3, int arg4);
/* Starts effect id of the boss's effect set following matrix (see Game/DiablonPartsBeam.cpp). */
boost::weak_ptr<EffectElement> fn_802DC998(u32 effects, const Mtx34f *matrix, int id);
/* Changes the boss's charge level by amount (behaviour inferred). */
void fn_802D9664(Boss::Diablon::Diablon *diablon, int amount);
}

namespace Boss {
namespace Diablon {
namespace Parts {

/* The glow effect of each charge level; none when negative (name inferred). */
static s32 s_levelEffects[6] = {-1, -1, -1, 0x21, 0x21, 0x54};

class Charge : public PartsBase {
  public:
    Charge(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *node);
    virtual ~Charge();
    virtual void Tick(float dt);
    virtual void OnAction(const Action &action);

    /* Stops the level's glow effect (name inferred). */
    void StopEffect();

    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node);

    /* 0x24 */ const Mtx34f *m_node;
    /* 0x28 */ float m_time; /* charged towards the next level */
    /* 0x2C */ s32 m_effectId;
    /* 0x30 */ boost::weak_ptr<EffectElement> m_flash;
    /* 0x38 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x40 */ UnkSoundHandle_801D3DC8 m_sound;
    /* 0x5C */ u32 m_flags; /* 0 the start hint is due */
};
SIZE_ASSERT(Charge, 0x60);

Charge::Charge(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *node)
    : PartsBase(parent, "Charge", owner) {
    fn_80014E04(&m_flags);
    m_node = node;
    m_time = 0.0f;
    m_effectId = -1;
    fn_80014E10(&m_flags);
}

Charge::~Charge() {
    if (!m_flash.expired()) {
        m_flash.lock()->Retire();
    }
    if (!m_effect.expired()) {
        m_effect.lock()->Retire();
    }
    m_sound.fn_801D455C();
}

void Charge::Tick(float dt) {
    if (m_owner->m_flags.Test(1) && m_owner->field_0xD0 < 5) {
        if (fn_80014E34(&m_flags, 0)) {
            fn_8006127C(&m_flags, 0);
            UnkHintManager_801FB180 *hint = GetHintManager();
            fn_801FAF00(hint, m_owner->m_params->field_0xB8, 0, 1, 0);
        }
        if (m_sound.m_id == 0) {
            m_sound = fn_801D3DC8(0x7E2E, reinterpret_cast<GadgetCharInfo *>(m_owner.get()), this);
        }
        m_time += dt;
        if (m_time >= m_owner->m_params->field_0x4) {
            m_time = m_time - m_owner->m_params->field_0x4;
            fn_802D9664(m_owner.get(), 1);
            m_flash = fn_802DC998(m_owner->field_0x6C, m_node, 0x1F);
            if (m_owner->field_0xD0 >= 5) {
                UnkHintManager_801FB180 *hint = GetHintManager();
                fn_801FAF00(hint, m_owner->m_params->field_0xBC, 0, 1, 0);
                fn_801D54F4(fn_80053164(), 0x7E2F, m_owner.get(), 0, 1);
            }
        }
    } else if (m_sound.m_id != 0) {
        m_sound.fn_801D455C();
    }
    s32 level = m_owner->field_0xD0;
    if (m_effectId != s_levelEffects[level]) {
        StopEffect();
        m_effectId = s_levelEffects[level];
        if (m_effectId >= 0) {
            m_effect = fn_802DC998(m_owner->field_0x6C, m_node, m_effectId);
        }
    }
}

void Charge::OnAction(const Action &action) {
    switch (action.m_type) {
    case 13:
        fn_802D9664(m_owner.get(), -1);
        break;
    case 14:
        fn_802D9664(m_owner.get(), -5);
        fn_80014E1C(&m_flags, 0);
        m_time = 0.0f;
        break;
    case 12:
        StopEffect();
        m_owner->m_flags.Reset(1);
        break;
    }
}

void Charge::StopEffect() {
    if (!m_effect.expired()) {
        m_effect.lock()->Kill();
        m_effect.reset();
    }
}

PartsBase *Charge::Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                               const Mtx34f *node) {
    return new Charge(parent, owner, node);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss

/* Boss::Diablon::Parts::Shoulder (class name from RTTI): one of the Diablon boss's shoulders (a
 * part with a model). It opens, with two effects and a sound, while boss flag 2 is set (the
 * barrier is up) and closes again when it clears. File name inferred from the class. Names marked
 * "(name inferred)" are not original. Definitions are in reverse address order (deferred inlining
 * emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

extern "C" {
/* Starts effect id of the boss's effect set following matrix (see Game/DiablonPartsBeam.cpp). */
boost::weak_ptr<EffectElement> fn_802DC998(u32 effects, const Mtx34f *matrix, int id);
/* The sound manager, and plays sound id at the character info (see Game/BAGunShip.cpp). */
void *fn_80053164(void);
void fn_801D54F4(void *manager, int id, CharInfo *source, int arg3, int arg4);
}

namespace Boss {
namespace Diablon {
namespace Parts {

static const ModelNode s_shoulderNodes[2] = {{16, 0}, {17, 0}};
static const s32 s_shoulderMotions[1] = {4110};
static const ModelDesc s_shoulderModel = {4, 11, s_shoulderNodes, 2, s_shoulderMotions, 1};

class Shoulder : public ModelBase {
  public:
    Shoulder(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *node);
    virtual ~Shoulder();
    virtual void Tick(float dt);

    /* Names inferred. */
    void Open();
    void Close();

    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node);

    /* 0x28 */ boost::weak_ptr<EffectElement> m_effect;
    /* 0x30 */ boost::weak_ptr<EffectElement> m_openEffect;
    /* 0x38 */ boost::weak_ptr<EffectElement> m_openEffect2;
    /* 0x40 */ s32 m_state;
};
SIZE_ASSERT(Shoulder, 0x44);

Shoulder::Shoulder(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *node)
    : ModelBase(parent, "Shoulder", owner, &s_shoulderModel, node) {
    const Mtx34f *matrix = GetNodeMatrix(0);
    m_effect = fn_802DC998(owner->field_0x6C, matrix, 5);
    m_state = 0;
}

Shoulder::~Shoulder() {
    if (!m_effect.expired()) {
        m_effect.lock()->Retire();
    }
    if (!m_openEffect.expired()) {
        m_openEffect.lock()->Retire();
    }
    if (!m_openEffect2.expired()) {
        m_openEffect2.lock()->Retire();
    }
    Close();
}

void Shoulder::Tick(float dt) {
    switch (m_state) {
    case 0:
        Close();
        m_state++;
        break;
    case 1:
        if (m_owner->m_flags.Test(2)) {
            m_state = 100;
        }
        break;
    case 100:
        Open();
        m_state++;
        break;
    case 101:
        if (!m_owner->m_flags.Test(2)) {
            m_state = 0;
        }
        break;
    }
    ModelBase::Tick(dt);
}

void Shoulder::Open() {
    if (GetMotionIndex() != 1) {
        PlayMotion(1, 0.0f);
    }
    const Mtx34f *matrix = GetNodeMatrix(0);
    m_openEffect = fn_802DC998(m_owner->field_0x6C, matrix, 7);
    m_openEffect2 = fn_802DC998(m_owner->field_0x6C, matrix, 14);
    fn_801D54F4(fn_80053164(), 0x7E3E, m_owner.get(), 0, 1);
}

void Shoulder::Close() {
    if (GetMotionIndex() != 0) {
        PlayMotion(0, 0.0f);
    }
    if (!m_openEffect.expired()) {
        m_openEffect.lock()->Stop();
        m_openEffect.reset();
    }
    if (!m_openEffect2.expired()) {
        m_openEffect2.lock()->Stop();
        m_openEffect2.reset();
    }
    fn_801D54F4(fn_80053164(), 0x7E3F, m_owner.get(), 0, 1);
}

PartsBase *Shoulder::Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                 const Mtx34f *node) {
    return new Shoulder(parent, owner, node);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss

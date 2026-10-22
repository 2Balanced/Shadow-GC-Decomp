/* Boss::Diablon::Parts::ChargeMeter (class name from RTTI): one of the two meters on the Diablon
 * boss's body (a part with a model), which shows the boss's charge level (Diablon field 0xD0)
 * with an effect for each level. File name inferred from the class. Names marked "(name
 * inferred)" are not original. Definitions are in reverse address order (deferred inlining emits
 * functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/Diablon.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

extern "C" {
/* Starts effect id of the boss's effect set following matrix (see Game/DiablonPartsBeam.cpp). */
boost::weak_ptr<EffectElement> fn_802DC998(u32 effects, const Mtx34f *matrix, int id);
}

namespace Boss {
namespace Diablon {
namespace Parts {

/* The effects of each charge level, the models, and both by side (names inferred). */
static const s32 s_leftEffects[6] = {0x1D, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F};
static const s32 s_rightEffects[6] = {0x1B, 0x26, 0x27, 0x28, 0x29, 0x2A};
static const ModelDesc s_leftModel = {5, -1, 0, 0, 0, 0};
static const ModelDesc s_rightModel = {6, -1, 0, 0, 0, 0};

struct ChargeMeterSide {
    const ModelDesc *m_model;
    const s32 *m_effects;
};

static ChargeMeterSide s_sides[2] = {
    {&s_leftModel, s_leftEffects},
    {&s_rightModel, s_rightEffects},
};

class ChargeMeter : public ModelBase {
  public:
    ChargeMeter(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, int side,
                const Mtx34f *node);
    virtual ~ChargeMeter();
    virtual void Tick(float dt);

    static PartsBase *InstantiateLeft(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                      const Mtx34f *node);
    static PartsBase *InstantiateRight(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                       const Mtx34f *node);

    /* 0x28 */ const s32 *m_effects;
    /* 0x2C */ s32 m_level;
    /* 0x30 */ boost::weak_ptr<EffectElement> m_effect;
};
SIZE_ASSERT(ChargeMeter, 0x38);

ChargeMeter::ChargeMeter(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, int side,
                         const Mtx34f *node)
    : ModelBase(parent, "ChargeMeter", owner, s_sides[side].m_model, node) {
    m_level = -1;
    m_effects = s_sides[side].m_effects;
}

ChargeMeter::~ChargeMeter() {
    if (!m_effect.expired()) {
        m_effect.lock()->Retire();
    }
}

void ChargeMeter::Tick(float dt) {
    ModelBase::Tick(dt);
    if (m_level != m_owner->field_0xD0) {
        m_level = m_owner->field_0xD0;
        if (!m_effect.expired()) {
            m_effect.lock()->Kill();
            m_effect.reset();
        }
        const Mtx34f *node = GetParentMatrix();
        m_effect = fn_802DC998(m_owner->field_0x6C, node, m_effects[m_level]);
    }
}

PartsBase *ChargeMeter::InstantiateLeft(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                        const Mtx34f *node) {
    return new ChargeMeter(parent, owner, 0, node);
}

PartsBase *ChargeMeter::InstantiateRight(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                         const Mtx34f *node) {
    return new ChargeMeter(parent, owner, 1, node);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss

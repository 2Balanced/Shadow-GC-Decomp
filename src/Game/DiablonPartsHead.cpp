/* Boss::Diablon::Parts::Head (class name from RTTI): the Diablon boss's head (a part with a
 * model). It shows an eye effect and a glow that changes once the boss is fully charged (Diablon
 * field 0xD0 at 5), and carries the target the player aims at, kept on a node of the body. File
 * name inferred from the class. Names marked "(name inferred)" are not original. Definitions are
 * in reverse address order (deferred inlining emits functions last-defined first). */
#include "Effect/System/EffectElement.h"
#include "Game/CharInfo.h"
#include "Game/Diablon.h"
#include "Game/MathTypes.h"
#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"
#include "size_assert.h"

using Effect::System::EffectElement;

/* Target (class name from RTTI): a point other objects can aim at (see Game/ArkCannon.cpp). */
class Target {
  public:
    /* Moves the target to position (name inferred). */
    void fn_800CCE38(const Vec3f *position);
};

extern "C" {
/* Starts effect id of the boss's effect set following matrix (see Game/DiablonPartsBeam.cpp). */
boost::weak_ptr<EffectElement> fn_802DC998(u32 effects, const Mtx34f *matrix, int id);
/* Creates a target for owner (see Game/ArkCannon.cpp). */
boost::shared_ptr<Target> fn_801E7128(const boost::weak_ptr<CharInfo> &owner, int flags);
Vec3f fn_8040FB08(const Mtx34f *matrix); /* its translation */
}

namespace Boss {
namespace Diablon {
namespace Parts {

static const ModelDesc s_headModel = {3, -1, 0, 0, 0, 0};

class Head : public ModelBase {
  public:
    Head(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *node,
         const Mtx34f *targetNode);
    virtual ~Head();
    virtual void Tick(float dt);

    static PartsBase *Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                                  const Mtx34f *node, const Mtx34f *targetNode);

    /* 0x28 */ boost::weak_ptr<EffectElement> m_eyeEffect;
    /* 0x30 */ boost::weak_ptr<EffectElement> m_glow;
    /* 0x38 */ s32 m_glowId;
    /* 0x3C */ const Mtx34f *m_targetNode;
    /* 0x40 */ boost::shared_ptr<Target> m_target;
};
SIZE_ASSERT(Head, 0x48);

Head::Head(PartsBase *parent, const boost::shared_ptr<Diablon> &owner, const Mtx34f *node,
           const Mtx34f *targetNode)
    : ModelBase(parent, "Head", owner, &s_headModel, node), m_targetNode(targetNode) {
    const Mtx34f *matrix = GetParentMatrix();
    m_eyeEffect = fn_802DC998(owner->field_0x6C, matrix, 12);
    m_glowId = 0x1E;
    m_glow = fn_802DC998(owner->field_0x6C, matrix, m_glowId);
    m_target = fn_801E7128(owner, 0x12C00);
}

Head::~Head() {
    if (!m_eyeEffect.expired()) {
        m_eyeEffect.lock()->Retire();
    }
    if (!m_glow.expired()) {
        m_glow.lock()->Retire();
    }
}

void Head::Tick(float dt) {
    ModelBase::Tick(dt);
    int glowId = 0x1E;
    if (m_owner->field_0xD0 >= 5) {
        glowId = 0x30;
    }
    if (m_glowId != glowId) {
        if (!m_glow.expired()) {
            m_glow.lock()->Stop();
        }
        m_glowId = glowId;
        m_glow = fn_802DC998(m_owner->field_0x6C, GetParentMatrix(), m_glowId);
    }
    Vec3f position;
    fn_800091EC(&position, &(const Vec3f &)fn_8040FB08(m_targetNode));
    m_target->fn_800CCE38(&position);
}

PartsBase *Head::Instantiate(PartsBase *parent, const boost::shared_ptr<Diablon> &owner,
                             const Mtx34f *node, const Mtx34f *targetNode) {
    return new Head(parent, owner, node, targetNode);
}

} // namespace Parts
} // namespace Diablon
} // namespace Boss
